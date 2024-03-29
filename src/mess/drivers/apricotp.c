// license:?
// copyright-holders:Angelo Salese, Dirk Best
/***************************************************************************

    ACT Apricot F1 series

    preliminary driver by Angelo Salese


11/09/2011 - modernised. The portable doesn't seem to have
             scroll registers, and it sets the palette to black.
             I've added a temporary video output so that you can get
             an idea of what the screen should look like. [Robbbert]

****************************************************************************/

/*

    TODO:

    - devices
    - LCD
    - sound

*/

#include "includes/apricotp.h"
#include "apricotp.lh"



//**************************************************************************
//  MACROS/CONSTANTS
//**************************************************************************

enum
{
	LED_STOP = 0,
	LED_POWER,
	LED_SHIFT_LOCK,
	LED_DISK,
	LED_VOICE,
	LED_COLOUR_SELECT,
	LED_CAPS_LOCK
};



//**************************************************************************
//  VIDEO
//**************************************************************************

//-------------------------------------------------
//  mc6845_interface crtc_intf
//-------------------------------------------------

static MC6845_UPDATE_ROW( fp_update_row )
{
}


static MC6845_INTERFACE( crtc_intf )
{
	false,
	0,0,0,0,
	8,
	NULL,
	fp_update_row,
	NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	NULL
};


void fp_state::video_start()
{
	// allocate memory
	m_video_ram.allocate(0x20000);
}


UINT32 fp_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	offs_t addr = (!BIT(m_video, 4) << 15) | (BIT(m_video, 1) << 14);

	for (int y = 0; y < 200; y++)
	{
		for (int sx = 0; sx < 40; sx++)
		{
			UINT16 data = m_video_ram[addr++];

			for (int x = 0; x < 16; x++)
			{
				int color = BIT(data, 15);

				bitmap.pix16(y, (sx * 16) + x) = color;

				data <<= 1;
			}
		}
	}

	return 0;
}


static const gfx_layout charset_8x8 =
{
	8,8,
	256,
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};


static GFXDECODE_START( act_f1 )
	GFXDECODE_ENTRY( I8086_TAG, 0x0800, charset_8x8, 0, 1 )
GFXDECODE_END


READ8_MEMBER( fp_state::prtr_snd_r )
{
	/*

	    bit     description

	    0       BUSY
	    1       SE
	    2       _FAULT
	    3       PE
	    4       LP23
	    5       DCNG-L
	    6       J9 1-2
	    7       J9 3-4

	*/

	UINT8 data = 0;

	// centronics
	data |= m_centronics_busy;
	data |= m_centronics_select << 1;
	data |= m_centronics_fault << 2;
	data |= m_centronics_perror << 3;

	// floppy
	data |= (m_floppy ? m_floppy->dskchg_r() : 1) << 5;

	return data;
}

WRITE8_MEMBER( fp_state::pint_clr_w )
{
	m_pic->ir6_w(CLEAR_LINE);
}


WRITE8_MEMBER( fp_state::ls_w )
{
	m_centronics->write_strobe(!BIT(data, 0));
}


WRITE8_MEMBER( fp_state::contrast_w )
{
}


WRITE8_MEMBER( fp_state::palette_w )
{
	/*

	    bit     description

	    0       B
	    1       G
	    2       R
	    3       I
	    4       index
	    5       index
	    6       index
	    7       index

	*/
}


WRITE16_MEMBER( fp_state::video_w )
{
	/*

	    bit     description

	    0       CRTRES-H
	    1       SEL1
	    2       DON-H
	    3       LCDON-H
	    4       SEL2
	    5       L3 even access
	    6       L2 odd access
	    7       L1 video RAM enable
	    8
	    9       STOP LED
	    10      POWER LED
	    11      SHIFT LOCK LED
	    12      DISK LED
	    13      VOICE LED
	    14      COLOUR SELECT LED
	    15      CAPS LOCK LED

	*/

	m_video = data & 0xff;
}

void fp_state::lat_ls259_w(offs_t offset, int state)
{
	switch (offset)
	{
	case 0:
		{
			m_floppy = NULL;

			if (state) m_floppy = m_floppy0->get_device();
			else m_floppy = m_floppy1->get_device();

			m_fdc->set_floppy(m_floppy);

			if (m_floppy)
			{
				m_floppy->set_rpm(600);
				m_floppy->mon_w(0);
			}
		}
		break;
	}
}

WRITE8_MEMBER( fp_state::lat_w )
{
	lat_ls259_w((offset >> 1) & 0x07, BIT(data, 0));
}


READ16_MEMBER( fp_state::mem_r )
{
	UINT16 data = 0xffff;

	if (offset >= 0xd0000/2 && offset < 0xf0000/2)
	{
		if (BIT(m_video, 7))
		{
			data = m_video_ram[offset - 0xd0000/2];
		}
		else
		{
			if (offset < m_ram->size()/2)
			{
				data = m_work_ram[offset];
			}
		}
	}
	else
	{
		if (offset < m_ram->size()/2)
		{
			data = m_work_ram[offset];
		}
	}

	return data;
}


WRITE16_MEMBER( fp_state::mem_w )
{
	if (offset >= 0xd0000/2 && offset < 0xe0000/2)
	{
		if (BIT(m_video, 7))
		{
			m_video_ram[offset - 0xd0000/2] = data;
		}
		else
		{
			if (offset < m_ram->size()/2)
			{
				m_work_ram[offset] = data;
			}
		}
	}
	else if (offset >= 0xe0000/2 && offset < 0xf0000/2)
	{
		if (BIT(m_video, 7))
		{
			if (BIT(m_video, 5))
			{
				m_video_ram[offset - 0xd0000/2] = (data & 0xff00) | (m_video_ram[offset - 0xd0000/2] & 0x00ff);
			}

			if (BIT(m_video, 6))
			{
				m_video_ram[offset - 0xd0000/2] = (data & 0x00ff) | (m_video_ram[offset - 0xd0000/2] & 0xff00);
			}
		}
		else
		{
			if (offset < m_ram->size()/2)
			{
				m_work_ram[offset] = data;
			}
		}
	}
	else
	{
		if (offset < m_ram->size()/2)
		{
			m_work_ram[offset] = data;
		}
	}
}


//**************************************************************************
//  ADDRESS MAPS
//**************************************************************************

//-------------------------------------------------
//  ADDRESS_MAP( fp_mem )
//-------------------------------------------------

static ADDRESS_MAP_START( fp_mem, AS_PROGRAM, 16, fp_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00000, 0xf7fff) AM_READWRITE(mem_r, mem_w)
	AM_RANGE(0xf8000, 0xfffff) AM_ROM AM_REGION(I8086_TAG, 0)
ADDRESS_MAP_END


//-------------------------------------------------
//  ADDRESS_MAP( fp_io )
//-------------------------------------------------

static ADDRESS_MAP_START( fp_io, AS_IO, 16, fp_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x000, 0x007) AM_DEVREADWRITE8(WD2797_TAG, wd2797_t, read, write, 0x00ff)
	AM_RANGE(0x008, 0x00f) AM_DEVREADWRITE8(I8253A5_TAG, pit8253_device, read, write, 0x00ff)
	AM_RANGE(0x018, 0x01f) AM_DEVREADWRITE8(Z80SIO0_TAG, z80sio0_device, ba_cd_r, ba_cd_w, 0x00ff)
	AM_RANGE(0x020, 0x021) AM_DEVWRITE8("cent_data_out", output_latch_device, write, 0x00ff)
	AM_RANGE(0x022, 0x023) AM_WRITE8(pint_clr_w, 0x00ff)
	AM_RANGE(0x024, 0x025) AM_READ8(prtr_snd_r, 0x00ff)
	AM_RANGE(0x026, 0x027) AM_DEVWRITE8(SN76489AN_TAG, sn76489a_device, write, 0x00ff)
	AM_RANGE(0x028, 0x029) AM_WRITE8(contrast_w, 0x00ff)
	AM_RANGE(0x02a, 0x02b) AM_WRITE8(palette_w, 0x00ff)
	AM_RANGE(0x02e, 0x02f) AM_WRITE(video_w)
	AM_RANGE(0x040, 0x05f) AM_DEVREADWRITE8(I8237_TAG, am9517a_device, read, write, 0x00ff)
	AM_RANGE(0x068, 0x06b) AM_DEVREADWRITE8(I8259A_TAG, pic8259_device, read, write, 0x00ff)
	AM_RANGE(0x06c, 0x06d) AM_DEVWRITE8(MC6845_TAG, mc6845_device, address_w, 0x00ff)
	AM_RANGE(0x06e, 0x06f) AM_DEVREADWRITE8(MC6845_TAG, mc6845_device, register_r, register_w, 0x00ff)
ADDRESS_MAP_END


//-------------------------------------------------
//  ADDRESS_MAP( sound_mem )
//-------------------------------------------------

static ADDRESS_MAP_START( sound_mem, AS_PROGRAM, 8, fp_state )
	AM_RANGE(0xf000, 0xffff) AM_ROM AM_REGION(HD63B01V1_TAG, 0)
ADDRESS_MAP_END


//-------------------------------------------------
//  ADDRESS_MAP( sound_io )
//-------------------------------------------------

static ADDRESS_MAP_START( sound_io, AS_IO, 8, fp_state )
	AM_RANGE(M6801_PORT1, M6801_PORT1)
	AM_RANGE(M6801_PORT2, M6801_PORT2)
	AM_RANGE(M6801_PORT3, M6801_PORT3)
	AM_RANGE(M6801_PORT4, M6801_PORT4)
ADDRESS_MAP_END



//**************************************************************************
//  INPUT PORTS
//**************************************************************************

//-------------------------------------------------
//  INPUT_PORTS( act )
//-------------------------------------------------

static INPUT_PORTS_START( fp )
	// defined in machine/apricotkb.c
INPUT_PORTS_END



//**************************************************************************
//  DEVICE CONFIGURATION
//**************************************************************************

//-------------------------------------------------
//  APRICOT_KEYBOARD_INTERFACE( kb_intf )
//-------------------------------------------------

static APRICOT_KEYBOARD_INTERFACE( kb_intf )
{
	DEVCB_NULL
};


//-------------------------------------------------
//  pic8259_interface pic_intf
//-------------------------------------------------

	IRQ_CALLBACK_MEMBER(fp_state::fp_irq_callback)
{
	return m_pic->inta_r();
}

/*

    INT0    TIMER
    INT1    FDC
    INT2    6301
    INT3    COMS
    INT4    USART
    INT5    COMS
    INT6    PRINT
    INT7    EOP

*/

//-------------------------------------------------
//  pit8253_config pit_intf
//-------------------------------------------------

static const struct pit8253_interface pit_intf =
{
	{
		{
			2000000,
			DEVCB_LINE_VCC,
			DEVCB_DEVICE_LINE_MEMBER(I8259A_TAG, pic8259_device, ir0_w)
		}, {
			2000000,
			DEVCB_LINE_VCC,
			DEVCB_NULL
		}, {
			2000000,
			DEVCB_LINE_VCC,
			DEVCB_NULL
		}
	}
};


//-------------------------------------------------
//  I8237_INTERFACE( dmac_intf )
//-------------------------------------------------

static I8237_INTERFACE( dmac_intf )
{
	DEVCB_NULL,
	DEVCB_DEVICE_LINE_MEMBER(I8259A_TAG, pic8259_device, ir7_w),
	DEVCB_NULL,
	DEVCB_NULL,
	{ DEVCB_NULL, DEVCB_DEVICE_MEMBER(WD2797_TAG, wd_fdc_t, data_r), DEVCB_NULL, DEVCB_NULL },
	{ DEVCB_NULL, DEVCB_DEVICE_MEMBER(WD2797_TAG, wd_fdc_t, data_w), DEVCB_NULL, DEVCB_NULL },
	{ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL }
};


//-------------------------------------------------
//  Z80SIO_INTERFACE( sio_intf )
//-------------------------------------------------

static Z80SIO_INTERFACE( sio_intf )
{
	0, 0, 0, 0,

	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,

	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,

	DEVCB_DEVICE_LINE_MEMBER(I8259A_TAG, pic8259_device, ir4_w),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};


//-------------------------------------------------
//  wd17xx_interface fdc_intf
//-------------------------------------------------

static SLOT_INTERFACE_START( fp_floppies )
	SLOT_INTERFACE( "35dd", FLOPPY_35_DD ) // Sony OA-D32W (600 rpm)
SLOT_INTERFACE_END
/*
static LEGACY_FLOPPY_OPTIONS_START( act )
    LEGACY_FLOPPY_OPTION( img2hd, "dsk", "2HD disk image", basicdsk_identify_default, basicdsk_construct_default, NULL,
        HEADS([2])
        TRACKS([80])
        SECTORS([16])
        SECTOR_LENGTH([256])
        FIRST_SECTOR_ID([1]))
LEGACY_FLOPPY_OPTIONS_END
*/

void fp_state::fdc_intrq_w(bool state)
{
	m_pic->ir1_w(state);
}

void fp_state::fdc_drq_w(bool state)
{
	m_dmac->dreq1_w(state);
}


WRITE_LINE_MEMBER( fp_state::write_centronics_busy )
{
	m_centronics_busy = state;
	if (!state) m_pic->ir6_w(ASSERT_LINE);
}

WRITE_LINE_MEMBER( fp_state::write_centronics_select )
{
	m_centronics_select = state;
}

WRITE_LINE_MEMBER( fp_state::write_centronics_fault )
{
	m_centronics_fault = state;
}

WRITE_LINE_MEMBER( fp_state::write_centronics_perror )
{
	m_centronics_perror = state;
}

//**************************************************************************
//  MACHINE INITIALIZATION
//**************************************************************************

//-------------------------------------------------
//  MACHINE_START( fp )
//-------------------------------------------------

void fp_state::machine_start()
{
	// floppy callbacks
	m_fdc->setup_intrq_cb(wd_fdc_t::line_cb(FUNC(fp_state::fdc_intrq_w), this));
	m_fdc->setup_drq_cb(wd_fdc_t::line_cb(FUNC(fp_state::fdc_drq_w), this));

	// register CPU IRQ callback
	m_maincpu->set_irq_acknowledge_callback(device_irq_acknowledge_delegate(FUNC(fp_state::fp_irq_callback),this));

	// allocate memory
	m_work_ram.allocate(m_ram->size() / 2);
}


void fp_state::machine_reset()
{
	m_video = 0;

	m_fdc->dden_w(0);

	for (offs_t offset = 0; offset < 7; offset++)
	{
		lat_ls259_w(offset, 0);
	}
}



//**************************************************************************
//  MACHINE DRIVERS
//**************************************************************************

//-------------------------------------------------
//  MACHINE_CONFIG( fp )
//-------------------------------------------------

static MACHINE_CONFIG_START( fp, fp_state )
	/* basic machine hardware */
	MCFG_CPU_ADD(I8086_TAG, I8086, XTAL_15MHz/3)
	MCFG_CPU_PROGRAM_MAP(fp_mem)
	MCFG_CPU_IO_MAP(fp_io)

	MCFG_CPU_ADD(HD63B01V1_TAG, HD6301, 2000000)
	MCFG_CPU_PROGRAM_MAP(sound_mem)
	MCFG_CPU_IO_MAP(sound_io)
	MCFG_DEVICE_DISABLE()

	/* video hardware */
	MCFG_DEFAULT_LAYOUT( layout_apricotp )

	MCFG_SCREEN_ADD(SCREEN_LCD_TAG, LCD)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_UPDATE_DRIVER(fp_state, screen_update)
	MCFG_SCREEN_SIZE(640, 200)
	MCFG_SCREEN_VISIBLE_AREA(0, 640-1, 0, 200-1)

	MCFG_SCREEN_ADD(SCREEN_CRT_TAG, RASTER)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_UPDATE_DEVICE(MC6845_TAG, mc6845_device, screen_update)
	MCFG_SCREEN_SIZE(640, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 640-1, 0, 256-1)

	MCFG_PALETTE_LENGTH(16)
	MCFG_GFXDECODE(act_f1)
	MCFG_MC6845_ADD(MC6845_TAG, MC6845, SCREEN_CRT_TAG, 4000000, crtc_intf)

	// sound hardware
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD(SN76489AN_TAG, SN76489A, 2000000)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)

	/* Devices */
	MCFG_APRICOT_KEYBOARD_ADD(kb_intf)
	MCFG_I8237_ADD(I8237_TAG, 250000, dmac_intf)
	MCFG_PIC8259_ADD(I8259A_TAG, INPUTLINE(I8086_TAG, INPUT_LINE_IRQ0), VCC, NULL)
	MCFG_PIT8253_ADD(I8253A5_TAG, pit_intf)
	MCFG_Z80SIO0_ADD(Z80SIO0_TAG, 2500000, sio_intf)
	MCFG_WD2797x_ADD(WD2797_TAG, 2000000)
	MCFG_FLOPPY_DRIVE_ADD(WD2797_TAG":0", fp_floppies, "35dd", floppy_image_device::default_floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD(WD2797_TAG":1", fp_floppies, NULL,   floppy_image_device::default_floppy_formats)

	MCFG_CENTRONICS_ADD("centronics", centronics_printers, "image")
	MCFG_CENTRONICS_BUSY_HANDLER(WRITELINE(fp_state, write_centronics_busy))
	MCFG_CENTRONICS_SELECT_HANDLER(WRITELINE(fp_state, write_centronics_select))
	MCFG_CENTRONICS_FAULT_HANDLER(WRITELINE(fp_state, write_centronics_fault))
	MCFG_CENTRONICS_PERROR_HANDLER(WRITELINE(fp_state, write_centronics_perror))

	MCFG_CENTRONICS_OUTPUT_LATCH_ADD("cent_data_out", CENTRONICS_TAG)

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("256K")
	MCFG_RAM_EXTRA_OPTIONS("512K,1M")
MACHINE_CONFIG_END



//**************************************************************************
//  ROMS
//**************************************************************************

//-------------------------------------------------
//  ROM( fp )
//-------------------------------------------------

ROM_START( fp )
	ROM_REGION( 0x8000, I8086_TAG, 0 )
	ROM_LOAD16_BYTE( "lo_fp_3.1.ic20", 0x0000, 0x4000, CRC(0572add2) SHA1(c7ab0e5ced477802e37f9232b5673f276b8f5623) )   // Labelled 11212721 F97E PORT LO VR 3.1
	ROM_LOAD16_BYTE( "hi_fp_3.1.ic9",  0x0001, 0x4000, CRC(3903674b) SHA1(8418682dcc0c52416d7d851760fea44a3cf2f914) )   // Labelled 11212721 BD2D PORT HI VR 3.1

	ROM_REGION( 0x1000, HD63B01V1_TAG, 0 )
	ROM_LOAD( "voice interface hd63b01v01.ic29", 0x0000, 0x1000, NO_DUMP )

	ROM_REGION( 0x100, "proms", 0 )
	ROM_LOAD( "tbp24s10.ic73", 0x000, 0x100, NO_DUMP ) // address decoder 256x4

	ROM_REGION( 0x100, "plds", 0 )
	ROM_LOAD( "pal1 pal12l6.ic2", 0x000, 0x100, NO_DUMP ) // ?
	ROM_LOAD( "pal2 pal10l8.ic35", 0x000, 0x100, NO_DUMP ) // address decoder
	ROM_LOAD( "pal3 pal12l6.ic77", 0x000, 0x100, NO_DUMP ) // ?
ROM_END



//**************************************************************************
//  SYSTEM DRIVERS
//**************************************************************************

//    YEAR  NAME        PARENT      COMPAT  MACHINE     INPUT   INIT     COMPANY             FULLNAME        FLAGS
COMP( 1984, fp,    0,      0,      fp,   fp, driver_device,    0,     "ACT",   "Apricot Portable / FP", GAME_NOT_WORKING )
