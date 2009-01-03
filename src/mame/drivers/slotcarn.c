/***************************************************************************

  Slot Carnival, (1985, Wing Co,Ltd)

  Driver by David Haywood & Roberto Fresca.


  Notes:

  - Very similar to merit.c
  - We're using the MC6845 drawing code from merit.c, but it will need
    modifications to support the reels and proper colors.


****************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/ay8910.h"
#include "machine/8255ppi.h"
#include "video/mc6845.h"

#define MASTER_CLOCK			(XTAL_10MHz)
#define CPU_CLOCK				(MASTER_CLOCK / 4)
#define PIXEL_CLOCK				(MASTER_CLOCK / 1)
#define CRTC_CLOCK				(MASTER_CLOCK / 8)

#define NUM_PENS				(16)
#define RAM_PALETTE_SIZE		(1024)

static pen_t pens[NUM_PENS];

static UINT8 *ram_attr;
static UINT8 *ram_video;
static UINT8 *ram_palette;
static UINT8 *backup_ram;

/*

  b800-b803 = PPI (9b) III mode0 all.
  ba00-ba03 = PPI (90) IOO mode0 all.
  bc00-bc03 = PPI (92) IIO mode0 all.

  6000 = RW

*/

static READ8_HANDLER( palette_r )
{
	int co;

	co = ((ram_attr[offset] & 0x7F) << 3) | (offset & 0x07);
	return ram_palette[co];
}

static WRITE8_HANDLER( palette_w )
{
	int co;

	video_screen_update_now(space->machine->primary_screen);
	data &= 0x0f;

	co = ((ram_attr[offset] & 0x7F) << 3) | (offset & 0x07);
	ram_palette[co] = data;

}


static MC6845_BEGIN_UPDATE( begin_update )
{
	int i;
	int dim, bit0, bit1, bit2;

	for (i=0; i < NUM_PENS; i++)
	{
		dim = BIT(i,3) ? 255 : 127;
		bit0 = BIT(i,0);
		bit1 = BIT(i,1);
		bit2 = BIT(i,2);
		pens[i] = MAKE_RGB(dim*bit0, dim*bit1, dim*bit2);
	}

	return pens;
}


static MC6845_UPDATE_ROW( update_row )
{
	int extra_video_bank_bit = 0; // not used?
	int lscnblk = 0; // not used?


	UINT8 cx;
	pen_t *pens = (pen_t *)param;
	UINT8 *gfx[2];
	UINT16 x = 0;
	int rlen;

	gfx[0] = memory_region(device->machine, "gfx1");
	gfx[1] = memory_region(device->machine, "gfx2");
	rlen = memory_region_length(device->machine, "gfx2");

	//ma = ma ^ 0x7ff;
	for (cx = 0; cx < x_count; cx++)
	{
		int i;
		int attr = ram_attr[ma & 0x7ff];
		int region = (attr & 0x40) >> 6;
		int addr = ((ram_video[ma & 0x7ff] | ((attr & 0x80) << 1) | (extra_video_bank_bit)) << 4) | (ra & 0x0f);
		int colour = (attr & 0x7f) << 3;
		UINT8	*data;

		addr &= (rlen-1);
		data = gfx[region];

		for (i = 7; i>=0; i--)
		{
			int col = colour;

			col |= (BIT(data[0x0000 | addr],i)<<2);
			if (region==0)
			{
				col |= (BIT(data[rlen | addr],i)<<1);
				col |= (BIT(data[rlen<<1 | addr],i)<<0);
			}
			else
				col |= 0x03;

			col = ram_palette[col & 0x3ff];
			*BITMAP_ADDR32(bitmap, y, x) = pens[col ? col : (lscnblk ? 8 : 0)];

			x++;
		}
		ma++;
	}
}


static MC6845_ON_HSYNC_CHANGED(hsync_changed)
{
	/* update any video up to the current scanline */
	video_screen_update_now(device->machine->primary_screen);
}

static MC6845_ON_VSYNC_CHANGED(vsync_changed)
{
	cpu_set_input_line(device->machine->cpu[0], 0, vsync ? ASSERT_LINE : CLEAR_LINE);
}

static const mc6845_interface mc6845_intf =
{
	"main",					/* screen we are acting on */
	8,						/* number of pixels per video memory address */
	begin_update,			/* before pixel update callback */
	update_row,				/* row update callback */
	0,						/* after pixel update callback */
	0,						/* callback for display state changes */
	hsync_changed,			/* HSYNC callback */
	vsync_changed			/* VSYNC callback */
};


/*******************************
*          Memory Map          *
*******************************/

static ADDRESS_MAP_START( slotcarn_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_ROM
	AM_RANGE(0x6000, 0x67ff) AM_RAM AM_BASE(&backup_ram)

	AM_RANGE(0xb000, 0xb000) AM_WRITE(ay8910_control_port_0_w)
	AM_RANGE(0xb100, 0xb100) AM_READWRITE(ay8910_read_port_0_r,ay8910_write_port_0_w)

	AM_RANGE(0xb800, 0xb803) AM_DEVREADWRITE(PPI8255, "ppi8255_0", ppi8255_r, ppi8255_w)	/* Input Ports */
	AM_RANGE(0xba00, 0xba03) AM_DEVREADWRITE(PPI8255, "ppi8255_1", ppi8255_r, ppi8255_w)	/* Input Ports */
	AM_RANGE(0xbc00, 0xbc03) AM_DEVREADWRITE(PPI8255, "ppi8255_2", ppi8255_r, ppi8255_w)	/* Input/Output Ports */

	AM_RANGE(0xc000, 0xc000) AM_READ_PORT("DSW3")
	AM_RANGE(0xc400, 0xc400) AM_READ_PORT("DSW4")

	AM_RANGE(0xd800, 0xd81f) AM_RAM // column scroll for reels?

	AM_RANGE(0xe000, 0xe000) AM_DEVWRITE(MC6845, "crtc", mc6845_address_w)
	AM_RANGE(0xe001, 0xe001) AM_DEVWRITE(MC6845, "crtc", mc6845_register_w)

	AM_RANGE(0xe800, 0xefff) AM_RAM AM_BASE(&ram_attr)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM AM_BASE(&ram_video)
	AM_RANGE(0xf800, 0xfbff) AM_READWRITE(palette_r, palette_w)
ADDRESS_MAP_END


/********************************
*          Input Ports          *
********************************/

static INPUT_PORTS_START( slotcarn )
	PORT_START("IN0")	/* b800 (ppi8255) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Key In") PORT_CODE(KEYCODE_Q)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Start")  PORT_CODE(KEYCODE_N)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("Cancel") PORT_CODE(KEYCODE_M)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Select") PORT_CODE(KEYCODE_B)

	PORT_START("IN1")	/* b801 (ppi8255) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1)    PORT_IMPULSE(2)       /* Coin A */
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2)    PORT_IMPULSE(2)       /* Coin B */
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Stats")    PORT_CODE(KEYCODE_0)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Settings") PORT_CODE(KEYCODE_9)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN3)    PORT_IMPULSE(2)       /* Coin C */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN4)    PORT_IMPULSE(2)       /* Coin D */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")	/* b802 (ppi8255) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN3")	/* bc00 (ppi8255) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Double-Up")    PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Take Score")   PORT_CODE(KEYCODE_V)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Bet")          PORT_CODE(KEYCODE_Z)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Payout")       PORT_CODE(KEYCODE_W)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Empty Hopper") PORT_CODE(KEYCODE_H)

	PORT_START("IN4")	/* bc01 (ppi8255) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW1")	/* ba00 (ppi8255) */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW1:8")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW1:7")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW1:6")
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW1:5")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW1:4")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW1:3")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0xc0, "D-UP Pay Rate" )		PORT_DIPLOCATION("DSW1:1,2")
	PORT_DIPSETTING(    0xc0, "80%" )
	PORT_DIPSETTING(    0x80, "85%" )
	PORT_DIPSETTING(    0x40, "90%" )
	PORT_DIPSETTING(    0x00, "95%" )

	PORT_START("DSW2")	/* ay8910, port B */
	PORT_DIPNAME( 0x01, 0x01, "FIVE LINE Pay Rate" )	PORT_DIPLOCATION("DSW2:8")
	PORT_DIPSETTING(    0x01, "75%" )
	PORT_DIPSETTING(    0x00, "85%" )
	PORT_DIPNAME( 0x02, 0x02, "SUPER CONTI Pay Rate" )	PORT_DIPLOCATION("DSW2:7")
	PORT_DIPSETTING(    0x02, "75%" )
	PORT_DIPSETTING(    0x00, "85%" )
	PORT_DIPNAME( 0x04, 0x04, "LUCKY BAR Pay Rate" )	PORT_DIPLOCATION("DSW2:6")
	PORT_DIPSETTING(    0x04, "75%" )
	PORT_DIPSETTING(    0x00, "85%" )
	PORT_DIPNAME( 0x08, 0x08, "BONUS LINE Pay Rate" )	PORT_DIPLOCATION("DSW2:5")
	PORT_DIPSETTING(    0x08, "75%" )
	PORT_DIPSETTING(    0x00, "85%" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW2:4")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW2:3")
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW2:2")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW2:1")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW3")	/* c000 direct */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW4")	/* c400 direct */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


/*************************************
*          Graphics Layouts          *
*************************************/

static const gfx_layout slotcarntiles8x8x3_layout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3),RGN_FRAC(1,3),RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	16*8 // the flipped tiles are mixed with normal tiles, so skip a tile each time
};

static const gfx_layout slotcarntiles8x8x1_layout =
{
	8,8,
	RGN_FRAC(1,1),
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	16*8 // the flipped tiles are mixed with normal tiles, so skip a tile each time
};


/************************************
*          Graphics Decode          *
************************************/

static GFXDECODE_START( slotcarn )
	GFXDECODE_ENTRY( "gfx1", 0, slotcarntiles8x8x3_layout, 0, 16 )
	GFXDECODE_ENTRY( "gfx1", 8, slotcarntiles8x8x3_layout, 0, 16 ) // flipped
	GFXDECODE_ENTRY( "gfx2", 0, slotcarntiles8x8x1_layout, 0, 4 )
	GFXDECODE_ENTRY( "gfx2", 8, slotcarntiles8x8x1_layout, 0, 4 ) // flipped
GFXDECODE_END



static VIDEO_UPDATE( slotcarn )
{
	const device_config *mc6845 = device_list_find_by_tag(screen->machine->config->devicelist, MC6845, "crtc");
	mc6845_update(mc6845, bitmap, cliprect);

	return 0;
}


static MACHINE_START(merit)
{
	ram_palette = auto_malloc(RAM_PALETTE_SIZE);
	state_save_register_global_pointer(machine, ram_palette, RAM_PALETTE_SIZE);
}


/***************************************
*       PPI 8255 (x3) Interfaces       *
***************************************/

static const ppi8255_interface scarn_ppi8255_intf[3] =
{
	{	/* A, B & C set as input */
		DEVICE8_PORT("IN0"),	/* Port A read */
		DEVICE8_PORT("IN1"),	/* Port B read */
		DEVICE8_PORT("IN2"),	/* Port C read */
		NULL,					/* Port A write */
		NULL,					/* Port B write */
		NULL					/* Port C write */
	},
	{	/* A set as input */
		DEVICE8_PORT("DSW1"),	/* Port A read */
		NULL,					/* Port B read */
		NULL,					/* Port C read */
		NULL,					/* Port A write */
		NULL,					/* Port B write */
		NULL					/* Port C write */
	},
	{	/* A & B set as input */
		DEVICE8_PORT("IN3"),	/* Port A read */
		DEVICE8_PORT("IN4"),	/* Port B read */
		NULL,					/* Port C read */
		NULL,					/* Port A write */
		NULL,					/* Port B write */
		NULL					/* Port C write */
	}
};


/*************************************
*          AY8910 Interface          *
*************************************/

static const ay8910_interface scarn_ay8910_config =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	NULL,
	input_port_6_r,	/* DSW 2 */
	NULL,
	NULL
};


/***********************************
*          Machine Driver          *
***********************************/

static MACHINE_DRIVER_START( slotcarn )

	/* basic machine hardware */
	MDRV_CPU_ADD("main", Z80, CPU_CLOCK) // 2.5 Mhz?
	MDRV_CPU_PROGRAM_MAP(slotcarn_map,0)
	//MDRV_CPU_IO_MAP(goldstar_readport,0)
	//MDRV_CPU_VBLANK_INT("main", nmi_line_pulse)

	/* 3x 8255 */
	MDRV_PPI8255_ADD( "ppi8255_0", scarn_ppi8255_intf[0] )
	MDRV_PPI8255_ADD( "ppi8255_1", scarn_ppi8255_intf[1] )
	MDRV_PPI8255_ADD( "ppi8255_2", scarn_ppi8255_intf[2] )

	MDRV_MACHINE_START(merit)
	/* video hardware */

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_RGB32)
	MDRV_SCREEN_RAW_PARAMS(PIXEL_CLOCK, 512, 0, 512, 256, 0, 256)	/* temporary, CRTC will configure screen */

	MDRV_MC6845_ADD("crtc", MC6845, CRTC_CLOCK, mc6845_intf)

	MDRV_VIDEO_UPDATE(slotcarn)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay", AY8910,1500000)
	MDRV_SOUND_CONFIG(scarn_ay8910_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END


/******************************
*          ROMs Load          *
******************************/
/*

Slot Carnival (c) WING 1985

roms 1 - 3 M5L2764K
roms 4 - 7 HN4827128G-30
proms 1 X MB7051

xtal 1 x 10mhz

cpus

1 x NEC D780C-1  (Z80)
3 x NEC  D8255AC-5
1 x AY-3-8910 (SOUND)
1 X HD46505SP-1  (VIDEO?)

ram

2x 2114
3x 58725
1x 6116

sound amp

HA1366W

(has 23 pots for volume)

4 banks of dipswitches 8 switches each
set to

- off + on

bank1
++------
bank2
+---++++
bank3
-+------
bank4
------++

board has a ton is transistors on it presumably for driving the coin hopper?
and a battery 3.5v 50ma

rom3 has mention of coin hopper and coin jam.

*/

ROM_START( slotcarn )
	ROM_REGION( 0x20000, "main", 0 )
	ROM_LOAD( "rom1.e10", 0x0000, 0x2000, CRC(a7ea6420) SHA1(4dd88f1bcaf354da93c3e88979a5e1a026105598) )
	ROM_LOAD( "rom2.e9",  0x2000, 0x2000, CRC(8156a603) SHA1(92618ac2ac908d24adb75eb705dc2f84eef12211) )
	ROM_LOAD( "rom3.e8",  0x4000, 0x2000, CRC(bf74ccad) SHA1(7f5049693de236790671b16dd1e1d0d2ac120e1a) )

	ROM_REGION( 0xc000, "gfx1", 0 )
	ROM_LOAD( "rom5.a4", 0x0000, 0x4000, CRC(99235fc7) SHA1(e93be11ee139f0845272a63ef2d50962e85e155a) )
	ROM_LOAD( "rom6.a3", 0x4000, 0x4000, CRC(2a86ce1f) SHA1(6dadbed41ae4b6e6e9efdb3a9d9d1f52dc76fe13) )
	ROM_LOAD( "rom7.a2", 0x8000, 0x4000, CRC(c8196687) SHA1(68233c3f039a01e4e25113c929f6a1b8d60af177) )

	ROM_REGION( 0x04000, "gfx2", 0 ) // 1bpp (both halves identical)
	ROM_LOAD( "rom4.a5", 0x0000, 0x4000, CRC(1428c46c) SHA1(ea30eeebcc2cc825f33e1ffeb590b047e3072b9c) )

	ROM_REGION( 0xa0000, "user1", ROMREGION_ERASEFF ) /* questions */
ROM_END


/*********************************************
*                Game Drivers                *
**********************************************

      YEAR  NAME      PARENT   MACHINE   INPUT     INIT   ROT    COMPANY        FULLNAME        FLAGS  */
GAME( 1985, slotcarn, 0,       slotcarn, slotcarn, 0,     ROT0, "Wing Co.Ltd", "Slot Carnival", GAME_NOT_WORKING )

