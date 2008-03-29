/***************************************************************************

    JPM Give us a Break hardware

    preliminary driver by Phil Bennett

    Games supported:
        * Give us a Break [2 sets]
        * Ten Up [2 sets]

    Looking for:
        * Numbers Game
        * Pac Quiz
        * Suit Pursuit
        * Treasure Trail?

    Known bugs:
        * Neither game registers coins. Protection maybe?

***************************************************************************/

#include "driver.h"
#include "machine/6840ptm.h"
#include "video/tms34061.h"
#include "sound/sn76496.h"
#include "deprecat.h"


/*************************************
 *
 *  Defines
 *
 *************************************/

#define LOG_FDC_COMMANDS	0

#if LOG_FDC_COMMANDS
	#define FDC_LOG(x)		mame_printf_debug x
#else
	#define FDC_LOG(x)
#endif


enum
{
	INT_UNKNOWN1     = 1,
	INT_UNKNOWN2     = 2,
	INT_6840PTM      = 3,
	INT_6850ACIA     = 4,
	INT_TMS34061     = 5,
	INT_FLOPPYCTRL   = 6,
	INT_WATCHDOG_INT = 7,
} int_levels;


/*************************************
 *
 *  6840 PTM
 *
 *************************************/

static void ptm_irq(int state)
{
	cpunum_set_input_line(Machine, 0, INT_6840PTM, state);
}

static const ptm6840_interface ptm_intf =
{
	1000000,
	{ 0, 0, 0 },
	{ 0, 0, 0 },
	ptm_irq
};


/*************************************
 *
 *  Video hardware
 *
 *************************************/

/*****************
 * TMS34061 CRTC
 *****************/

static void tms_interrupt(running_machine *machine, int state)
{
	cpunum_set_input_line(machine, 0, INT_TMS34061, state);
}

static const struct tms34061_interface tms34061intf =
{
	"main",			/* The screen we are acting on */
	8,				/* VRAM address is (row << rowshift) | col */
	0x40000,		/* Size of video RAM */
	tms_interrupt	/* Interrupt gen callback */
};


static WRITE16_HANDLER( guab_tms34061_w )
{
	int func = (offset >> 19) & 3;	
	int row = (offset >> 7) & 0xff;
	int col;

	if (func == 0 || func == 2)
		col = offset  & 0xff;
	else
		col = offset <<= 1;

	if (ACCESSING_MSB)
		tms34061_w(col, row, func, data >> 8);

	if (ACCESSING_LSB)
		tms34061_w(col | 1, row, func, data & 0xff);
}


static READ16_HANDLER( guab_tms34061_r )
{
	UINT16 data = 0;
	int func = (offset >> 19) & 3;	
	int row = (offset >> 7) & 0xff;
	int col;
	
	if (func == 0 || func == 2)
		col = offset  & 0xff;
	else
		col = offset <<= 1;

	if (ACCESSING_MSB)
		data |= tms34061_r(col, row, func) << 8;

	if (ACCESSING_LSB)
		data |= tms34061_r(col | 1, row, func);

	return data;
}


/****************************
 *  EF9369 color palette IC
 *  (16 colors from 4096)
 ****************************/

static struct ef9369
{
	UINT32 addr;
	UINT16 clut[16];	/* 13-bits - a marking bit and a 444 color */
} pal;


/* Non-multiplexed mode */
static WRITE16_HANDLER( ef9369_w )
{
	data &= 0x00ff;

	/* Address register */
	if (offset & 1)
	{
		pal.addr = data & 0x1f;
	}
	/* Data register */
	else
	{
		UINT32 entry = pal.addr >> 1;

		if ((pal.addr & 1) == 0)
		{
			pal.clut[entry] &= ~0x00ff;
			pal.clut[entry] |= data;
		}
		else
		{
			UINT16 col;

			pal.clut[entry] &= ~0x1f00;
			pal.clut[entry] |= (data & 0x1f) << 8;

			/* Remove the marking bit */
			col = pal.clut[entry] & 0xfff;

			/* Update the MAME palette */
			palette_set_color_rgb(Machine, entry, pal4bit(col >> 0), pal4bit(col >> 4), pal4bit(col >> 8));
		}

			/* Address register auto-increment */
		if (++pal.addr == 32)
			pal.addr = 0;
	}
}

static READ16_HANDLER( ef9369_r )
{
	if ((offset & 1) == 0)
	{
		UINT16 col = pal.clut[pal.addr >> 1];

		if ((pal.addr & 1) == 0)
			return col & 0xff;
		else
			return col >> 8;
	}
	else
	{
		/* Address register is write only */
		return 0xffff;
	}
}


static VIDEO_START( guab )
{
	tms34061_start(machine, &tms34061intf);
}


static VIDEO_UPDATE( guab )
{
	int x, y;
	struct tms34061_display state;	

	tms34061_get_display_state(&state);

	/* If blanked, fill with black */
	if (state.blanked)
	{
		fillbitmap(bitmap, get_black_pen(screen->machine), cliprect);
		return 0;
	}

	for (y = cliprect->min_y; y <= cliprect->max_y; ++y)
	{
		UINT8 *src = &state.vram[256 * y];
		UINT16 *dest = BITMAP_ADDR16(bitmap, y, 0);

		for (x = cliprect->min_x; x <= cliprect->max_x; x += 2)
		{			
			UINT8 pen = src[x >> 1];

			/* Draw two 4-bit pixels */
			*dest++ = Machine->pens[pen >> 4];
			*dest++ = Machine->pens[pen & 0x0f];
		}
	}

	return 0;
}


/*************************************
 *
 *  WD1770 floppy disk controller
 *  (just enough to get by...)
 *
 *************************************/

struct wd1770
{
	UINT32	status;
	UINT8	cmd;
	UINT8	data;

	UINT32	side;
	INT32	track;
	INT32	sector;
	UINT32	sptr;
} fdc;

static emu_timer *fdc_timer;

#define USEC_DELAY			40
#define DISK_SIDES			2
#define DISK_TRACKS			80
#define DISK_SECTORS		18

#define DISK_SECTOR_SIZE	256
#define DISK_TRACK_SIZE		(DISK_SECTOR_SIZE * DISK_SECTORS)
#define DISK_SIDE_SIZE		(DISK_TRACK_SIZE * DISK_TRACKS)

enum
{
	BUSY			 = 1 << 0,
	DATA_REQUEST	 = 1 << 1,
	INDEX			 = 1 << 1,
	LOST_DATA		 = 1 << 2,
	TRACK_00		 = 1 << 2,
	CRC_ERROR		 = 1 << 3,
	RECORD_NOT_FOUND = 1 << 4,
	RECORD_TYPE		 = 1 << 5,
	SPIN_UP			 = 1 << 5,
	WRITE_PROTECT	 = 1 << 6,
	MOTOR_ON		 = 1 << 7
} wd1770_status;


static TIMER_CALLBACK( fdc_data_callback )
{
	UINT8* disk = (UINT8*)memory_region(REGION_USER1);
	int more_data = 0;

	/*
		Disk dumps are organised as:

		Side 0, Track 0: Sectors 0 - 17
		Side 1, Track 0: Sectors 0 - 17
		Side 0, Track 1: Sectors 0 - 17
		Side 1, Track 1: Sectors 0 - 17
		etc.
	*/

	int idx = 2 * fdc.track * (DISK_TRACK_SIZE) + (fdc.side ? DISK_TRACK_SIZE : 0)+
			  fdc.sector * (DISK_SECTOR_SIZE) +
			  fdc.sptr;

	/* Write or read? */
	if (fdc.cmd & 0x20)
		disk[idx] = fdc.data;
	else
		fdc.data = disk[idx];

	fdc.sptr++;

	if (fdc.sptr < DISK_SECTOR_SIZE)
	{
		more_data = 1;
	}
	else
	{
		fdc.sptr = 0;

		/* Multi-sector read */
		if (fdc.cmd & 0x10)
		{
			++fdc.sector;

			/* Keep going */
			if (fdc.sector < DISK_SECTORS)
				more_data = 1;
		}
	}

	if (more_data)
	{
		timer_adjust_oneshot(fdc_timer, ATTOTIME_IN_USEC(USEC_DELAY), 0);
	}
	else
	{
		/* Hack!*/
		fdc.cmd = 0;
		fdc.status &= ~BUSY;
	}

	fdc.status |= DATA_REQUEST;
	cpunum_set_input_line(machine, 0, INT_FLOPPYCTRL, ASSERT_LINE);
}


static WRITE16_HANDLER( wd1770_w )
{
	data &= 0xff;

	switch (offset)
	{
		case 0:
		{
			UINT8 cmd = (data >> 4) & 0xf;

			FDC_LOG(("Command: %x:: ", data));

			switch (cmd)
			{
				/**** Type I ****/
				case 0:
				{
					FDC_LOG(("Restore\n"));
					fdc.track = 0;
					break;
				}

				case 1:
				{
					FDC_LOG(("Seek to track %d\n", fdc.data));
					fdc.track = fdc.data;
					break;
				}

				/* Step */
				case 2:
				case 3:
				{
					FDC_LOG(("Step (not implemented)\n"));
					break;
				}

				case 4: case 5:
				{
					if (++fdc.track >= DISK_TRACKS)
						fdc.track = DISK_TRACKS - 1;

					FDC_LOG(("Step-in to track %d\n", fdc.track));
					break;
				}

				case 6: case 7:
				{
					if (--fdc.track > 0)
						fdc.track = 0;

					FDC_LOG(("Step-out to track %d\n", fdc.track));
					break;
				}


				/**** Type II ****/
				case 8:
				case 9:
				{
					fdc.cmd = data;
					fdc.status |= BUSY;

					if (data & 0x10)
						FDC_LOG(("Multi "));

					FDC_LOG(("Sector read: H%d T%d S%d\n",	fdc.side,
															fdc.track,
															fdc.sector));

					/* Set the data read timer */
					timer_adjust_oneshot(fdc_timer, ATTOTIME_IN_USEC(USEC_DELAY), 0);

					break;
				}

				case 10:
				case 11:
				{
					fdc.cmd = data;
					fdc.status |= BUSY;

					if (data & 0x10)
						FDC_LOG(("Multi "));

					FDC_LOG(("Sector write: H%d T%d S%d\n",	fdc.side,
															fdc.track,
															fdc.sector));

					/* Trigger a DRQ interrupt on the CPU */
					cpunum_set_input_line(machine, 0, INT_FLOPPYCTRL, ASSERT_LINE);
					fdc.status |= DATA_REQUEST;
					break;
				}

				/**** Type III ****/
				case 12: /* Read address */
				case 14: /* Read track */
				case 15: /* Write track */
				{
					FDC_LOG(("Unsupported TYPE III operation\n"));
					break;
				}

				/**** Type IV ****/
				case 13:
				{
					/* Stop any operation in progress */
					timer_reset(fdc_timer, attotime_never);
					fdc.status &= ~BUSY;
					FDC_LOG(("Force Interrupt\n"));
					break;
				}
			}

			break;
		}
		case 1:
		{
			FDC_LOG(("Set track: %d\n", data));
			fdc.track = data;
			break;
		}
		case 2:
		{
			FDC_LOG(("Set sector: %d\n", data));
			fdc.sector = data;
			fdc.sptr = 0;
			break;
		}
		case 3:
		{
			fdc.data = data;

			/* Clear the DRQ */
			cpunum_set_input_line(machine, 0, INT_FLOPPYCTRL, CLEAR_LINE);

			/* Queue an event to write the data if write command was specified */
			if (fdc.cmd & 0x20)
				timer_adjust_oneshot(fdc_timer, ATTOTIME_IN_USEC(USEC_DELAY), 0);

			break;
		}
	}
}

static READ16_HANDLER( wd1770_r )
{
	UINT16 retval = 0;

	switch (offset)
	{
		case 0:
		{
			retval = fdc.status;
			break;
		}
		case 1:
		{
			retval = fdc.track;
			break;
		}
		case 2:
		{
			retval = fdc.sector;
			break;
		}
		case 3:
		{
			retval = fdc.data;

			/* Clear the DRQ */
			cpunum_set_input_line(machine, 0, INT_FLOPPYCTRL, CLEAR_LINE);
			fdc.status &= ~DATA_REQUEST;
			break;
		}
	}

	return retval;
}


/****************************************
 *
 *  Hardware inputs (coins, buttons etc)
 *
 ****************************************/

static READ16_HANDLER( io_r )
{
	switch (offset)
	{
		case 0x00:
		case 0x01:
		case 0x02:
		{
			return readinputport(offset);
		}
		case 0x30:
		{
			/* Whatever it is, bit 7 must be 0 */
			return 0x7f;
		}
		default:
		{
			return 0;
		}
	}
}


/****************************************
 *
 *  Hardware outputs (lamps, meters etc)
 *
 ****************************************/

static WRITE16_HANDLER( io_w )
{
	switch (offset)
	{
		case 0x10:
		{
			/* Outputs 0 - 7 */
			break;
		}
		case 0x11:
		{
			/* Outputs 8 - 15 */
			break;
		}
		case 0x12:
		{
			/* Outputs 16 - 23 */
			break;
		}
		case 0x20:
		{
			/* Outputs 24 - 31 */
			break;
		}
		case 0x21:
		{
			/* Outputs 32 - 39 */
			break;
		}
		case 0x22:
		{
			/* Outputs 40 - 47 */
			break;
		}
		case 0x30:
		{
			SN76496_0_w(0, 0, data & 0xff);
			break;
		}
		case 0x31:
		{
			/* Only JPM knows about the other bits... */
			fdc.side = (data >> 3) & 1;			
			break;
		}
		case 0x32:
		{
			/* Watchdog? */
			break;
		}
		case 0x33:
		{
			/* Dunno */
			break;
		}
		default:
		{
			mame_printf_debug("Unknown IO W:0x%x with %x\n", 0xc0000 + (offset * 2), data);
		}
	}
}


/*************************************
 *
 *  68000 CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( guab_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x00ffff) AM_ROM
	AM_RANGE(0x040000, 0x04ffff) AM_ROM AM_REGION(REGION_CPU1, 0x10000)
	AM_RANGE(0x0c0000, 0x0c007f) AM_READWRITE(io_r, io_w)
	AM_RANGE(0x0c0080, 0x0c0083) AM_NOP /* ACIA 1 */
	AM_RANGE(0x0c00a0, 0x0c00a3) AM_NOP /* ACIA 2 */
	AM_RANGE(0x0c00c0, 0x0c00cf) AM_READWRITE(ptm6840_0_lsb_r, ptm6840_0_lsb_w)
	AM_RANGE(0x0c00e0, 0x0c00e7) AM_READWRITE(wd1770_r, wd1770_w)
	AM_RANGE(0x080000, 0x080fff) AM_RAM
	AM_RANGE(0x100000, 0x100003) AM_READWRITE(ef9369_r, ef9369_w)
	AM_RANGE(0x800000, 0xb0ffff) AM_READWRITE(guab_tms34061_r, guab_tms34061_w)
	AM_RANGE(0xb10000, 0xb1ffff) AM_RAM
	AM_RANGE(0xb80000, 0xb8ffff) AM_RAM
	AM_RANGE(0xb90000, 0xb9ffff) AM_RAM
ADDRESS_MAP_END


/*************************************
 *
 *  Port definitions
 *
 *************************************/

static INPUT_PORTS_START( guab )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN3 )   PORT_NAME("50p")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN4 )   PORT_NAME("100p")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Back door")  PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Cash door")  PORT_CODE(KEYCODE_T) PORT_TOGGLE
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Key switch") PORT_CODE(KEYCODE_Y) PORT_TOGGLE
	PORT_BIT( 0x20, IP_ACTIVE_HIGH,IPT_UNUSED )  PORT_NAME("50p level")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH,IPT_UNUSED )  PORT_NAME("100p level")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH,IPT_UNUSED )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("A")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("B")
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Select")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("C")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("D")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )   PORT_NAME("10p")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )   PORT_NAME("20p")
INPUT_PORTS_END

static INPUT_PORTS_START( tenup )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN3 )   PORT_NAME("50p")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN4 )   PORT_NAME("100p")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Back door")  PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Cash door")  PORT_CODE(KEYCODE_T) PORT_TOGGLE
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Key switch") PORT_CODE(KEYCODE_Y) PORT_TOGGLE
	PORT_BIT( 0x20, IP_ACTIVE_HIGH,IPT_UNUSED )  PORT_NAME("10p level")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH,IPT_UNUSED )  PORT_NAME("100p level")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH,IPT_UNUSED )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Pass")
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Collect")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("A")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("B")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("C")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )   PORT_NAME("10p")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )   PORT_NAME("20p")
INPUT_PORTS_END


/*************************************
 *
 *  Machine driver
 *
 *************************************/

 static MACHINE_START( guab )
{
	fdc_timer = timer_alloc(fdc_data_callback, NULL);
	ptm6840_config(0, &ptm_intf);
}

static MACHINE_RESET( guab )
{
	memset(&fdc, 0, sizeof(fdc));
}

static MACHINE_DRIVER_START( guab )
	/* TODO: Verify clock */
	MDRV_CPU_ADD_TAG("main", M68000, 8000000)
	MDRV_CPU_PROGRAM_MAP(guab_map, 0)

	MDRV_MACHINE_START(guab)
	MDRV_MACHINE_RESET(guab)

	/* TODO: Use real video timings */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0, 64*8-1, 0, 32*8-1)

	MDRV_PALETTE_LENGTH(16)

	MDRV_VIDEO_START(guab)
	MDRV_VIDEO_UPDATE(guab)

	MDRV_SPEAKER_STANDARD_MONO("mono")

	/* TODO: Verify clock */
	MDRV_SOUND_ADD(SN76489, 2000000)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END


/*************************************
 *
 *  ROM definition(s)
 *
 *************************************/

ROM_START( guab )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "guab1a1.rom", 0x00000, 0x8000, CRC(f23a9d7d) SHA1(f933e131bdcf21cfa6001c8e20fd11d94c7a9450) )
	ROM_LOAD16_BYTE( "guab1b1.rom", 0x00001, 0x8000, CRC(af3b5492) SHA1(6fd7f29e6ed2fadccc9246f1ebd049c3f9aeff13) )
	ROM_LOAD16_BYTE( "guab2a1.rom", 0x10000, 0x8000, CRC(ae7a162c) SHA1(d69721818b8e4daba776a678b62bc7f44f371a3f) )
	ROM_LOAD16_BYTE( "guab2b1.rom", 0x10001, 0x8000, CRC(29aa26a0) SHA1(8d425ad845ccfcd8995dbf6adc1ca17989a5d3ea) )

	ROM_REGION( 0xb4000, REGION_USER1, 0 )
	ROM_LOAD( "guabf6.dsk", 0x00000, 0xb4000, CRC(08804c28) SHA1(608d89d598b7acb133814540dba98cea29c6cad6) )
ROM_END

ROM_START( guab3 )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "guab1a1.rom", 0x00000, 0x8000, CRC(f23a9d7d) SHA1(f933e131bdcf21cfa6001c8e20fd11d94c7a9450) )
	ROM_LOAD16_BYTE( "guab1b1.rom", 0x00001, 0x8000, CRC(af3b5492) SHA1(6fd7f29e6ed2fadccc9246f1ebd049c3f9aeff13) )
	ROM_LOAD16_BYTE( "guab2a1.rom", 0x10000, 0x8000, CRC(ae7a162c) SHA1(d69721818b8e4daba776a678b62bc7f44f371a3f) )
	ROM_LOAD16_BYTE( "guab2b1.rom", 0x10001, 0x8000, CRC(29aa26a0) SHA1(8d425ad845ccfcd8995dbf6adc1ca17989a5d3ea) )

	ROM_REGION( 0xb4000, REGION_USER1, 0 )
	ROM_LOAD( "guab3.dsk", 0x00000, 0xb4000, CRC(ecb41e06) SHA1(2900a95046b38312c6035ea394b04b62c1d29f42) )
ROM_END

ROM_START( tenup )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "tu-11.bin", 0x00000, 0x8000, CRC(01843086) SHA1(106a226900e8cf929f89edf801c627f02e4afce3) )
	ROM_LOAD16_BYTE( "tu-12.bin", 0x00001, 0x8000, CRC(1c7f32b1) SHA1(2b14e2206695ae53909ae838a5c036248d9ab940) )
	ROM_LOAD16_BYTE( "tu-13.bin", 0x10000, 0x8000, CRC(d19e2bf7) SHA1(76a9cbd4f604ad39eb0e319a9a6d5a6739b0ed8c) )
	ROM_LOAD16_BYTE( "tu-14.bin", 0x10001, 0x8000, CRC(fd8a0c3c) SHA1(f87289ce6f0d2bc9b7d3a0b6deff38ba3aadf391) )

	ROM_REGION( 0xb4000, REGION_USER1, 0 )
	ROM_LOAD( "10up17.dsk", 0x00000, 0xb4000, CRC(7bc328df) SHA1(5f8e40d8ffc370fb19be9e386befa5fcd1f35a75) )
ROM_END

ROM_START( tenup3 )
	ROM_REGION( 0x20000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "tu-11.bin", 0x00000, 0x8000, CRC(01843086) SHA1(106a226900e8cf929f89edf801c627f02e4afce3) )
	ROM_LOAD16_BYTE( "tu-12.bin", 0x00001, 0x8000, CRC(1c7f32b1) SHA1(2b14e2206695ae53909ae838a5c036248d9ab940) )
	ROM_LOAD16_BYTE( "tu-13.bin", 0x10000, 0x8000, CRC(d19e2bf7) SHA1(76a9cbd4f604ad39eb0e319a9a6d5a6739b0ed8c) )
	ROM_LOAD16_BYTE( "tu-14.bin", 0x10001, 0x8000, CRC(fd8a0c3c) SHA1(f87289ce6f0d2bc9b7d3a0b6deff38ba3aadf391) )

	ROM_REGION( 0xb4000, REGION_USER1, 0 )
	ROM_LOAD( "10up3.dsk", 0x00000, 0xb4000, CRC(2767f017) SHA1(1c6551b089c3e3df48e0c03bd502b91fd88f0e94) )
ROM_END


/*************************************
 *
 *  Game driver(s)
 *
 *************************************/

GAME( 1986, guab,   0,     guab, guab,  0, ROT0, "JPM", "Give us a Break (6th edition)", GAME_NOT_WORKING )
GAME( 1986, guab3,  guab,  guab, guab,  0, ROT0, "JPM", "Give us a Break (3rd edition)", GAME_NOT_WORKING )

GAME( 1988, tenup,  0,     guab, tenup, 0, ROT0, "JPM", "Ten Up (compendium 17)",        GAME_NOT_WORKING )
GAME( 1988, tenup3, tenup, guab, tenup, 0, ROT0, "JPM", "Ten Up (compendium 3)",         GAME_NOT_WORKING )
