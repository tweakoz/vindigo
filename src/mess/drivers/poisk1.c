/***************************************************************************

    drivers/poisk1.c

    Driver file for Poisk-1

***************************************************************************/

#include "emu.h"

#include "includes/poisk1.h"
#include "machine/kb_poisk1.h"

#include "cpu/i86/i86.h"

#define CGA_PALETTE_SETS 83
/* one for colour, one for mono, 81 for colour composite */

#include "drivlgcy.h"
#include "scrlegcy.h"

#define VERBOSE_DBG 0

#define DBG_LOG(N,M,A) \
	do { \
		if(VERBOSE_DBG>=N) \
		{ \
			if( M ) \
				logerror("%11.6f: %-24s",machine().time().as_double(),(char*)M ); \
			logerror A; \
		} \
	} while (0)

/*
 * onboard devices:
 */

// Timer.  Poisk-1 uses single XTAL for everything? -- check

WRITE_LINE_MEMBER( p1_state::p1_speaker_set_spkrdata )
{
	m_p1_spkrdata = state ? 1 : 0;
	m_speaker->level_w(m_p1_spkrdata & m_p1_input);
}

WRITE_LINE_MEMBER( p1_state::p1_pit8253_out2_changed )
{
	m_p1_input = state ? 1 : 0;
	m_speaker->level_w(m_p1_spkrdata & m_p1_input);
}

const struct pit8253_interface p1_pit8253_config =
{
	{
		{
			XTAL_15MHz/12,              /* heartbeat IRQ */
			DEVCB_NULL,
			DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir0_w)
		}, {
			XTAL_15MHz/12,              /* keyboard poll -- XXX edge or level triggered? */
			DEVCB_NULL,
			DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir6_w)
		}, {
			XTAL_15MHz/12,              /* pio port c pin 4, and speaker polling enough */
			DEVCB_NULL,
			DEVCB_DEVICE_LINE_MEMBER(DEVICE_SELF_OWNER, p1_state, p1_pit8253_out2_changed)
		}
	}
};

// Keyboard (via PPI)

WRITE8_MEMBER(p1_state::p1_ppi_porta_w)
{
	m_kbpoll_mask = data;
	DBG_LOG(2,"p1_ppi_porta_w",("( %02X -> %02X )\n", data, m_kbpoll_mask));
}

READ8_MEMBER(p1_state::p1_ppi_porta_r)
{
	UINT8 ret;

	ret = m_kbpoll_mask;
	DBG_LOG(1,"p1_ppi_porta_r",("= %02X\n", ret));
	return ret;
}

READ8_MEMBER(p1_state::p1_ppi_portb_r)
{
	UINT16 key = 0xffff;
	UINT8 ret = 0;

	if (m_kbpoll_mask & 0x01) { key &= ioport("Y1")->read(); }
	if (m_kbpoll_mask & 0x02) { key &= ioport("Y2")->read(); }
	if (m_kbpoll_mask & 0x04) { key &= ioport("Y3")->read(); }
	if (m_kbpoll_mask & 0x08) { key &= ioport("Y4")->read(); }
	if (m_kbpoll_mask & 0x10) { key &= ioport("Y5")->read(); }
	if (m_kbpoll_mask & 0x20) { key &= ioport("Y6")->read(); }
	if (m_kbpoll_mask & 0x40) { key &= ioport("Y7")->read(); }
	if (m_kbpoll_mask & 0x80) { key &= ioport("Y8")->read(); }
	ret = key & 0xff;
//  DBG_LOG(1,"p1_ppi_portb_r",("= %02X\n", ret));
	return ret;
}

READ8_MEMBER(p1_state::p1_ppi_portc_r)
{
	UINT16 key = 0xffff;
	UINT8 ret = 0;

	if (m_kbpoll_mask & 0x01) { key &= ioport("Y1")->read(); }
	if (m_kbpoll_mask & 0x02) { key &= ioport("Y2")->read(); }
	if (m_kbpoll_mask & 0x04) { key &= ioport("Y3")->read(); }
	if (m_kbpoll_mask & 0x08) { key &= ioport("Y4")->read(); }
	if (m_kbpoll_mask & 0x10) { key &= ioport("Y5")->read(); }
	if (m_kbpoll_mask & 0x20) { key &= ioport("Y6")->read(); }
	if (m_kbpoll_mask & 0x40) { key &= ioport("Y7")->read(); }
	if (m_kbpoll_mask & 0x80) { key &= ioport("Y8")->read(); }
	ret = (key >> 8) & 0xff;
	DBG_LOG(2,"p1_ppi_portc_r",("= %02X\n", ret));
	return ret;
}

// XXX

READ8_MEMBER(p1_state::p1_ppi2_portc_r)
{
	int data = 0xff;
	double tap_val = m_cassette->input();

	data = ( data & ~0x10 ) | ( tap_val < 0 ? 0x10 : 0x00 );

	DBG_LOG(2,"p1_ppi_portc_r",("= %02X (tap_val %f) at %s\n",
		data, tap_val, machine().describe_context()));
	return data;
}

WRITE8_MEMBER(p1_state::p1_ppi2_portb_w)
{
	m_pit8253->gate2_w(BIT(data, 0));
	p1_speaker_set_spkrdata( data & 0x02 );
}

I8255_INTERFACE( p1_ppi8255_interface_1 )
{
/*60H*/ DEVCB_DEVICE_MEMBER(DEVICE_SELF_OWNER, p1_state, p1_ppi_porta_r),
	DEVCB_DEVICE_MEMBER(DEVICE_SELF_OWNER, p1_state, p1_ppi_porta_w),
/*69H*/ DEVCB_DEVICE_MEMBER(DEVICE_SELF_OWNER, p1_state, p1_ppi_portb_r),
	DEVCB_NULL,
/*6AH*/ DEVCB_DEVICE_MEMBER(DEVICE_SELF_OWNER, p1_state, p1_ppi_portc_r),
	DEVCB_DEVICE_MEMBER(DEVICE_SELF_OWNER, p1_state, p1_ppi_portc_w)
};

I8255_INTERFACE( p1_ppi8255_interface_2 )
{
/*68H*/ DEVCB_NULL,
	DEVCB_DEVICE_MEMBER(DEVICE_SELF_OWNER, p1_state, p1_ppi2_porta_w),
/*61H*/ DEVCB_NULL,
	DEVCB_DEVICE_MEMBER(DEVICE_SELF_OWNER, p1_state, p1_ppi2_portb_w),
/*62H*/ DEVCB_DEVICE_MEMBER(DEVICE_SELF_OWNER, p1_state, p1_ppi2_portc_r),
	DEVCB_NULL
};

READ8_MEMBER(p1_state::p1_ppi_r)
{
//  DBG_LOG(1,"p1ppi",("R %.2x\n", 0x60+offset));
	switch (offset) {
		case 0:
			return m_ppi8255n1->read(space, 0);
		case 9:
		case 10:
		case 11:
			return m_ppi8255n1->read(space, offset - 8);
		case 8:
			return m_ppi8255n2->read(space, 0);
		case 1:
		case 2:
		case 3:
			return m_ppi8255n2->read(space, offset);
		default:
			DBG_LOG(1,"p1ppi",("R %.2x (unimp)\n", 0x60+offset));
			return 0xff;
	}
}

WRITE8_MEMBER(p1_state::p1_ppi_w)
{
//  DBG_LOG(1,"p1ppi",("W %.2x $%02x\n", 0x60+offset, data));
	switch (offset) {
		case 0:
			return m_ppi8255n1->write(space, 0, data);
		case 9:
		case 10:
		case 11:
			return m_ppi8255n1->write(space, offset - 8, data);
		case 8:
			return m_ppi8255n2->write(space, 0, data);
		case 1:
		case 2:
		case 3:
			return m_ppi8255n2->write(space, offset, data);
		default:
			DBG_LOG(1,"p1ppi",("W %.2x $%02x (unimp)\n", 0x60+offset, data));
			return;
	}
}

/**********************************************************
 *
 * NMI handling
 *
 **********************************************************/

static const isa8bus_interface p1_isabus_intf =
{
	// interrupts
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir2_w),
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir3_w),
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir4_w),
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir5_w),
	DEVCB_NULL,
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir7_w),

	// dma request
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
};

static const cassette_interface p1_cassette_interface =
{
	cassette_default_formats,
	NULL,
	(cassette_state)(CASSETTE_PLAY | CASSETTE_MOTOR_DISABLED | CASSETTE_SPEAKER_ENABLED),
	NULL,
	NULL
};

IRQ_CALLBACK_MEMBER( p1_state::p1_irq_callback )
{
	return m_pic8259->acknowledge();
}

DRIVER_INIT_MEMBER( p1_state, poisk1 )
{
	address_space &program = m_maincpu->space(AS_PROGRAM);

	DBG_LOG(0,"init",("driver_init()\n"));

	program.unmap_readwrite(0, 0x7ffff);
	program.install_readwrite_bank(0, m_ram->size()-1, "bank10");
	membank( "bank10" )->set_base( m_ram->pointer() );
}

MACHINE_START_MEMBER( p1_state, poisk1 )
{
	DBG_LOG(0,"init",("machine_start()\n"));

	m_maincpu->set_irq_acknowledge_callback(device_irq_acknowledge_delegate(FUNC(p1_state::p1_irq_callback),this));
}

MACHINE_RESET_MEMBER( p1_state, poisk1 )
{
	DBG_LOG(0,"init",("machine_reset()\n"));

	m_kbpoll_mask = 0;
}

/*
 * macros
 */

static ADDRESS_MAP_START( poisk1_map, AS_PROGRAM, 8, p1_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00000, 0x7ffff) AM_RAM
	AM_RANGE(0xc0000, 0xc1fff) AM_ROM
	AM_RANGE(0xc0000, 0xfbfff) AM_NOP
	AM_RANGE(0xfc000, 0xfffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( poisk1_io, AS_IO, 8, p1_state )
	AM_RANGE(0x0020, 0x0021) AM_DEVREADWRITE("pic8259", pic8259_device, read, write)
	AM_RANGE(0x0028, 0x002B) AM_READWRITE(p1_trap_r, p1_trap_w)
	AM_RANGE(0x0040, 0x0043) AM_DEVREADWRITE("pit8253", pit8253_device, read, write)
	// can't use regular AM_DEVREADWRITE, because THIS IS SPARTA!
	// 1st PPI occupies ports 60, 69, 6A and 6B; 2nd PPI -- 68, 61, 62 and 63.
	AM_RANGE(0x0060, 0x006F) AM_READWRITE(p1_ppi_r, p1_ppi_w)
	AM_RANGE(0x03D0, 0x03DF) AM_READWRITE(p1_cga_r, p1_cga_w)
ADDRESS_MAP_END

static INPUT_PORTS_START( poisk1 )
	PORT_INCLUDE( poisk1_keyboard_v91 )
INPUT_PORTS_END

static MACHINE_CONFIG_START( poisk1, p1_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", I8088, 5000000)
	MCFG_CPU_PROGRAM_MAP(poisk1_map)
	MCFG_CPU_IO_MAP(poisk1_io)

	MCFG_MACHINE_START_OVERRIDE( p1_state, poisk1 )
	MCFG_MACHINE_RESET_OVERRIDE( p1_state, poisk1 )

	MCFG_PIT8253_ADD( "pit8253", p1_pit8253_config )

	MCFG_PIC8259_ADD( "pic8259", INPUTLINE(":maincpu", 0), VCC, NULL )

	MCFG_I8255A_ADD( "ppi8255n1", p1_ppi8255_interface_1 )
	MCFG_I8255A_ADD( "ppi8255n2", p1_ppi8255_interface_2 )

	MCFG_ISA8_BUS_ADD("isa", ":maincpu", p1_isabus_intf)
	MCFG_ISA8_SLOT_ADD("isa", "isa1", p1_isa8_cards, "fdc", false)
	MCFG_ISA8_SLOT_ADD("isa", "isa2", p1_isa8_cards, NULL, false)
	MCFG_ISA8_SLOT_ADD("isa", "isa3", p1_isa8_cards, NULL, false)
	MCFG_ISA8_SLOT_ADD("isa", "isa4", p1_isa8_cards, NULL, false)

	MCFG_CASSETTE_ADD( "cassette", p1_cassette_interface )

	MCFG_SPEAKER_STANDARD_MONO( "mono" )
	MCFG_SOUND_ADD( "speaker", SPEAKER_SOUND, 0 )
	MCFG_SOUND_ROUTE( ALL_OUTPUTS, "mono", 1.00 )

	MCFG_SCREEN_ADD( "screen", RASTER )
	MCFG_SCREEN_RAW_PARAMS( XTAL_15MHz, 912,0,640, 262,0,200 )
	MCFG_SCREEN_UPDATE_DRIVER( p1_state, screen_update )

	/* XXX verify palette */
	MCFG_PALETTE_LENGTH( CGA_PALETTE_SETS * 16 )

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("512K")
MACHINE_CONFIG_END

ROM_START( poisk1 )
	ROM_REGION16_LE(0x100000,"maincpu", 0)

	ROM_DEFAULT_BIOS("v91")
	ROM_SYSTEM_BIOS(0, "v89", "1989")
	ROMX_LOAD( "biosp1s.rf4", 0xfe000, 0x2000, CRC(1a85f671) SHA1(f0e59b2c4d92164abca55a96a58071ce869ff988), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "v91", "1991")
	ROMX_LOAD( "poisk_1991.bin", 0xfe000, 0x2000, CRC(d61c56fd) SHA1(de202e1f7422d585a1385a002a4fcf9d756236e5), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(2, "v91r2", "1991r2")
	ROMX_LOAD( "p_bios_nm.bin", 0xfe000, 0x2000, CRC(84430b4f) SHA1(3e477962be3cea09662cb2e3ad9966ad01c7455d), ROM_BIOS(3))

	// 0xc0000, sets 80x25 text and loops asking for 'Boot from hard disk (Y or N)?'
	ROM_LOAD( "boot_net.rf4", 0x00000, 0x2000, CRC(316c2030) SHA1(d043325596455772252e465b85321f1b5c529d0b)) // NET BIOS
	// 0xc0000, accesses ports 0x90..0x97
	ROM_LOAD( "pois_net.bin", 0x00000, 0x2000, CRC(cf9dd80a) SHA1(566bcb40c0cb2c8bfd5b485f0db689fdeaca3e86)) // ??? BIOS

	ROM_REGION(0x2000,"gfx1", ROMREGION_ERASE00)
	ROM_LOAD( "poisk.cga", 0x0000, 0x0800, CRC(f6eb39f0) SHA1(0b788d8d7a8e92cc612d044abcb2523ad964c200))
ROM_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

/*     YEAR     NAME        PARENT      COMPAT  MACHINE     INPUT       INIT                COMPANY       FULLNAME */
COMP ( 1989,    poisk1,    ibm5150,    0,      poisk1,    poisk1,    p1_state, poisk1,   "Electronmash",  "Poisk-1", 0)
