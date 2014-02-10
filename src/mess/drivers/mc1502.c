/***************************************************************************

    drivers/mc1502.c

    Driver file for Electronika MC 1502

***************************************************************************/

#include "emu.h"

#include "includes/mc1502.h"

#include "cpu/i86/i86.h"
#include "imagedev/serial.h"
#include "machine/kb_7007_3.h"
#include "sound/speaker.h"
#include "sound/wave.h"
#include "video/pc_cga.h"

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

static const cassette_interface mc1502_cassette_interface =
{
	cassette_default_formats,
	NULL,
	(cassette_state)(CASSETTE_STOPPED | CASSETTE_MOTOR_ENABLED | CASSETTE_SPEAKER_ENABLED),
	NULL,
	NULL
};

static const serial_image_interface mc1502_serial =
{
	9600, 8, device_serial_interface::STOP_BITS_1, device_serial_interface::PARITY_NONE, 1, "upd8251"
};

// Timer

static const isa8bus_interface mc1502_isabus_intf =
{
	// interrupts
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir2_w),
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir3_w),
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir4_w),
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir5_w),
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir6_w),
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir7_w),

	// dma request
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
};

/* check if any keys are pressed, raise IRQ1 if so */

TIMER_CALLBACK_MEMBER(mc1502_state::keyb_signal_callback)
{
	UINT8 key = 0;

	key |= ioport("Y1")->read();
	key |= ioport("Y2")->read();
	key |= ioport("Y3")->read();
	key |= ioport("Y4")->read();
	key |= ioport("Y5")->read();
	key |= ioport("Y6")->read();
	key |= ioport("Y7")->read();
	key |= ioport("Y8")->read();
	key |= ioport("Y9")->read();
	key |= ioport("Y10")->read();
	key |= ioport("Y11")->read();
	key |= ioport("Y12")->read();
//  DBG_LOG(1,"mc1502_k_s_c",("= %02X (%d) %s\n", key, m_kbd.pulsing,
//          (key || m_kbd.pulsing) ? " will IRQ" : ""));

	/*
	   If a key is pressed and we're not pulsing yet, start pulsing the IRQ1;
	   keep pulsing while any key is pressed, and pulse one time after all keys
	   are released.
	 */
	if (key) {
		if (m_kbd.pulsing < 2) {
			m_kbd.pulsing += 2;
		}
	}

	if (m_kbd.pulsing) {
		m_pic8259->ir1_w(m_kbd.pulsing & 1);
		m_kbd.pulsing--;
	}
}

WRITE8_MEMBER(mc1502_state::mc1502_ppi_portb_w)
{
//  DBG_LOG(2,"mc1502_ppi_portb_w",("( %02X )\n", data));
	m_ppi_portb = data;
	machine().device<pit8253_device>("pit8253")->gate2_w(BIT(data, 0));
//  mc1502_speaker_set_spkrdata(BIT(data, 1));
	m_centronics->write_strobe(BIT(data, 2));
	m_centronics->write_autofd(BIT(data, 3));
	m_centronics->write_init(BIT(data, 4));
}

// bit 0: parallel port data transfer direction (default = 0 = out)
// bits 1-2: CGA_FONT (default = 01)
// bit 3: i8251 SYNDET pin triggers NMI (default = 1 = no)
WRITE8_MEMBER(mc1502_state::mc1502_ppi_portc_w)
{
//  DBG_LOG(2,"mc1502_ppi_portc_w",("( %02X )\n", data));
	m_ppi_portc = data & 15;
}

//  0x80 -- serial RxD
//  0x40 -- CASS IN, also loops back T2OUT (gated by CASWR)
//  0x20 -- T2OUT
//  0x10 -- SNDOUT
READ8_MEMBER(mc1502_state::mc1502_ppi_portc_r)
{
	int timer2_output = machine().device<pit8253_device>("pit8253")->get_output(2);
	int data = 0xff;
	double tap_val = m_cassette->input();

	data = ( data & ~0x40 ) | ( tap_val < 0 ? 0x40 : 0x00 ) | ( (BIT(m_ppi_portb, 7) && timer2_output) ? 0x40 : 0x00 );
	data = ( data & ~0x20 ) | ( timer2_output ? 0x20 : 0x00 );
	data = ( data & ~0x10 ) | ( (BIT(m_ppi_portb, 1) && timer2_output) ? 0x10 : 0x00 );

//  DBG_LOG(2,"mc1502_ppi_portc_r",("= %02X (tap_val %f t2out %d) at %s\n",
//          data, tap_val, timer2_output, machine().describe_context()));
	return data;
}

READ8_MEMBER(mc1502_state::mc1502_kppi_porta_r)
{
	UINT8 key = 0;

	if (m_kbd.mask & 0x0001) { key |= ioport("Y1")->read(); }
	if (m_kbd.mask & 0x0002) { key |= ioport("Y2")->read(); }
	if (m_kbd.mask & 0x0004) { key |= ioport("Y3")->read(); }
	if (m_kbd.mask & 0x0008) { key |= ioport("Y4")->read(); }
	if (m_kbd.mask & 0x0010) { key |= ioport("Y5")->read(); }
	if (m_kbd.mask & 0x0020) { key |= ioport("Y6")->read(); }
	if (m_kbd.mask & 0x0040) { key |= ioport("Y7")->read(); }
	if (m_kbd.mask & 0x0080) { key |= ioport("Y8")->read(); }
	if (m_kbd.mask & 0x0100) { key |= ioport("Y9")->read(); }
	if (m_kbd.mask & 0x0200) { key |= ioport("Y10")->read(); }
	if (m_kbd.mask & 0x0400) { key |= ioport("Y11")->read(); }
	if (m_kbd.mask & 0x0800) { key |= ioport("Y12")->read(); }
	key ^= 0xff;
//  DBG_LOG(2,"mc1502_kppi_porta_r",("= %02X\n", key));
	return key;
}

WRITE8_MEMBER(mc1502_state::mc1502_kppi_portb_w)
{
	m_kbd.mask &= ~255;
	m_kbd.mask |= data ^ 255;
	if (!BIT(data, 0))
		m_kbd.mask |= 1 << 11;
	else
		m_kbd.mask &= ~(1 << 11);
//  DBG_LOG(2,"mc1502_kppi_portb_w",("( %02X -> %04X )\n", data, m_kbd.mask));
}

WRITE8_MEMBER(mc1502_state::mc1502_kppi_portc_w)
{
	m_kbd.mask &= ~(7 << 8);
	m_kbd.mask |= ((data ^ 7) & 7) << 8;
//  DBG_LOG(2,"mc1502_kppi_portc_w",("( %02X -> %04X )\n", data, m_kbd.mask));
}

I8255_INTERFACE( mc1502_ppi8255_interface_1 )
{
	DEVCB_NULL,
	DEVCB_DEVICE_MEMBER("cent_data_out", output_latch_device, write),
	DEVCB_NULL,
	DEVCB_DRIVER_MEMBER(mc1502_state,mc1502_ppi_portb_w),
	DEVCB_DRIVER_MEMBER(mc1502_state,mc1502_ppi_portc_r),
	DEVCB_DRIVER_MEMBER(mc1502_state,mc1502_ppi_portc_w)
};

I8255_INTERFACE( mc1502_ppi8255_interface_2 )
{
	DEVCB_DRIVER_MEMBER(mc1502_state,mc1502_kppi_porta_r),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DRIVER_MEMBER(mc1502_state,mc1502_kppi_portb_w),
	DEVCB_DEVICE_MEMBER("cent_status_in", input_buffer_device, read),
	DEVCB_DRIVER_MEMBER(mc1502_state,mc1502_kppi_portc_w)
};

const i8251_interface mc1502_i8251_interface =
{
	/* XXX RxD data are accessible via PPI port C, bit 7 */
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir7_w), /* default handler does nothing */
	DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir7_w),
	DEVCB_NULL,
	DEVCB_DRIVER_LINE_MEMBER(mc1502_state, mc1502_i8251_syndet)
};

WRITE_LINE_MEMBER(mc1502_state::mc1502_i8251_syndet)
{
	if (!BIT(m_ppi_portc,3))
		m_maincpu->set_input_line(INPUT_LINE_NMI, state ? ASSERT_LINE : CLEAR_LINE);
}

WRITE_LINE_MEMBER(mc1502_state::mc1502_pit8253_out1_changed)
{
	machine().device<i8251_device>("upd8251")->txc_w(state);
	machine().device<i8251_device>("upd8251")->rxc_w(state);
}

WRITE_LINE_MEMBER(mc1502_state::mc1502_pit8253_out2_changed)
{
//  mc1502_speaker_set_input( state );
	m_cassette->output(state ? 1 : -1);
}

const struct pit8253_interface mc1502_pit8253_config =
{
	{
		{
			XTAL_15MHz/12,              /* heartbeat IRQ */
			DEVCB_NULL,
			DEVCB_DEVICE_LINE_MEMBER("pic8259", pic8259_device, ir0_w)
		}, {
			XTAL_16MHz/12,              /* serial port */
			DEVCB_NULL,
			DEVCB_DRIVER_LINE_MEMBER(mc1502_state,mc1502_pit8253_out1_changed)
		}, {
			XTAL_16MHz/12,              /* pio port c pin 4, and speaker polling enough */
			DEVCB_NULL,
			DEVCB_DRIVER_LINE_MEMBER(mc1502_state,mc1502_pit8253_out2_changed)
		}
	}
};

IRQ_CALLBACK_MEMBER( mc1502_state::mc1502_irq_callback )
{
	return m_pic8259->acknowledge();
}

DRIVER_INIT_MEMBER( mc1502_state, mc1502 )
{
	address_space &program = m_maincpu->space(AS_PROGRAM);

	DBG_LOG(0,"init",("driver_init()\n"));

	program.unmap_readwrite(0, 0x7ffff);
	program.install_readwrite_bank(0, m_ram->size()-1, "bank10");
	membank( "bank10" )->set_base( m_ram->pointer() );
}

MACHINE_START_MEMBER( mc1502_state, mc1502 )
{
	DBG_LOG(0,"init",("machine_start()\n"));

	m_maincpu->set_irq_acknowledge_callback(device_irq_acknowledge_delegate(FUNC(mc1502_state::mc1502_irq_callback),this));
	/*
	       Keyboard polling circuit holds IRQ1 high until a key is
	       pressed, then it starts a timer that pulses IRQ1 low each
	       40ms (check) for 20ms (check) until all keys are released.
	       Last pulse causes BIOS to write a 'break' scancode into port 60h.
	 */
	m_pic8259->ir1_w(1);
	memset(&m_kbd, 0, sizeof(m_kbd));
	m_kbd.keyb_signal_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(mc1502_state::keyb_signal_callback),this));
	m_kbd.keyb_signal_timer->adjust( attotime::from_msec(20), 0, attotime::from_msec(20) );
}

MACHINE_RESET_MEMBER( mc1502_state, mc1502 )
{
	DBG_LOG(0,"init",("machine_reset()\n"));
}

/*
 * macros
 */

static ADDRESS_MAP_START( mc1502_map, AS_PROGRAM, 8, mc1502_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00000, 0x97fff) AM_RAM   /* 96K on mainboard + 512K on extension card */
	AM_RANGE(0xc0000, 0xfbfff) AM_NOP
//  AM_RANGE(0xe8000, 0xeffff) AM_ROM       /* BASIC */
	AM_RANGE(0xfc000, 0xfffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( pk88_map, AS_PROGRAM, 8, mc1502_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00000, 0x1ffff) AM_RAMBANK("bank10") /* 96K on mainboard */
	AM_RANGE(0xf0000, 0xf7fff) AM_ROM       /* BASIC */
	AM_RANGE(0xfc000, 0xfffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START(mc1502_io, AS_IO, 8, mc1502_state )
	AM_RANGE(0x0020, 0x0021) AM_DEVREADWRITE("pic8259", pic8259_device, read, write)
	AM_RANGE(0x0028, 0x0028) AM_DEVREADWRITE("upd8251", i8251_device, data_r, data_w)   // not working yet
	AM_RANGE(0x0029, 0x0029) AM_DEVREADWRITE("upd8251", i8251_device, status_r, control_w)
	AM_RANGE(0x0040, 0x0043) AM_DEVREADWRITE("pit8253", pit8253_device, read, write)
	AM_RANGE(0x0060, 0x0063) AM_DEVREADWRITE("ppi8255n1", i8255_device, read, write)
	AM_RANGE(0x0068, 0x006B) AM_DEVREADWRITE("ppi8255n2", i8255_device, read, write)    // keyboard poll
ADDRESS_MAP_END

static INPUT_PORTS_START( mc1502 )
	PORT_START("IN0") /* IN0 */
	PORT_BIT ( 0xf0, 0xf0,   IPT_UNUSED )
	PORT_BIT ( 0x08, 0x08,   IPT_CUSTOM ) PORT_VBLANK("screen")
	PORT_BIT ( 0x07, 0x07,   IPT_UNUSED )

	PORT_INCLUDE( mc7007_3_keyboard )
	PORT_INCLUDE( pcvideo_mc1502 )
INPUT_PORTS_END

static MACHINE_CONFIG_START( mc1502, mc1502_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", I8088, XTAL_16MHz/3)
	MCFG_CPU_PROGRAM_MAP(mc1502_map)
	MCFG_CPU_IO_MAP(mc1502_io)

	MCFG_MACHINE_START_OVERRIDE( mc1502_state, mc1502 )
	MCFG_MACHINE_RESET_OVERRIDE( mc1502_state, mc1502 )

	MCFG_PIT8253_ADD( "pit8253", mc1502_pit8253_config )

	MCFG_PIC8259_ADD( "pic8259", INPUTLINE("maincpu", 0), VCC, NULL )

	MCFG_I8255_ADD( "ppi8255n1", mc1502_ppi8255_interface_1 )
	MCFG_I8255_ADD( "ppi8255n2", mc1502_ppi8255_interface_2 )

	MCFG_I8251_ADD( "upd8251", mc1502_i8251_interface )
	MCFG_SERIAL_ADD( "irps", mc1502_serial )

	MCFG_ISA8_BUS_ADD("isa", ":maincpu", mc1502_isabus_intf)
	MCFG_ISA8_SLOT_ADD("isa", "isa1", mc1502_isa8_cards, "fdc", false)
	MCFG_ISA8_SLOT_ADD("isa", "isa2", mc1502_isa8_cards, "rom", false)

	/* video hardware (only 1 chargen in ROM; CGA_FONT dip always 1 */
	MCFG_FRAGMENT_ADD( pcvideo_mc1502 )

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.80)

	MCFG_CENTRONICS_ADD("centronics", centronics_printers, "image")
	MCFG_CENTRONICS_ACK_HANDLER(DEVWRITELINE("cent_status_in", input_buffer_device, write_bit6))
	MCFG_CENTRONICS_BUSY_HANDLER(DEVWRITELINE("cent_status_in", input_buffer_device, write_bit7))
	MCFG_CENTRONICS_FAULT_HANDLER(DEVWRITELINE("cent_status_in", input_buffer_device, write_bit4))
	MCFG_CENTRONICS_PERROR_HANDLER(DEVWRITELINE("cent_status_in", input_buffer_device, write_bit5))

	MCFG_DEVICE_ADD("cent_status_in", INPUT_BUFFER, 0)

	MCFG_CENTRONICS_OUTPUT_LATCH_ADD("cent_data_out", "centronics")

	MCFG_CASSETTE_ADD( "cassette", mc1502_cassette_interface )

	MCFG_SOFTWARE_LIST_ADD("flop_list","mc1502_flop")
//  MCFG_SOFTWARE_LIST_ADD("cass_list","mc1502_cass")

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("608K")                   /* 96 base + 512 on expansion card */
	MCFG_RAM_EXTRA_OPTIONS("96K")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( pk88, mc1502 )
	MCFG_CPU_REPLACE("maincpu", I8088, XTAL_16MHz/3)
	MCFG_CPU_PROGRAM_MAP(pk88_map)
	MCFG_CPU_IO_MAP(mc1502_io)
MACHINE_CONFIG_END


ROM_START( mc1502 )
	ROM_REGION16_LE(0x100000,"maincpu", 0)

	ROM_DEFAULT_BIOS("v52")
	ROM_SYSTEM_BIOS(0, "v50", "v5.0")
	ROMX_LOAD( "monitor_5_0.rom",  0xfc000, 0x4000, CRC(9e97c6a0) SHA1(16a304e8de69ec4d8b92acda6bf28454c361a24f),ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "v52", "v5.2")
	ROMX_LOAD( "monitor_5_2.rom",  0xfc000, 0x4000, CRC(0e65491e) SHA1(8a4d556473b5e0e59b05fab77c79c29f4d562412),ROM_BIOS(2))
	ROM_SYSTEM_BIOS(2, "v531", "v5.31")
	ROMX_LOAD( "monitor_5_31.rom", 0xfc000, 0x4000, CRC(a48295d5) SHA1(6f38977c22f9cc6c2bc6f6e53edc4048ca6b6721),ROM_BIOS(3))
	ROM_SYSTEM_BIOS(3, "v533", "v5.33")
	ROMX_LOAD( "0_(cbc0).bin", 0xfc000, 0x2000, CRC(9a55bc4f) SHA1(81da44eec2e52cf04b1fc7053502270f51270590),ROM_BIOS(4))
	ROMX_LOAD( "1_(dfe2).bin", 0xfe000, 0x2000, CRC(8dec077a) SHA1(d6f6d7cc2183abc77fbd9cd59132de5766f7c458),ROM_BIOS(4))

	ROM_REGION(0x2000,"gfx1", ROMREGION_ERASE00)
	ROM_LOAD( "symgen.rom", 0x0000, 0x2000, CRC(b2747a52) SHA1(6766d275467672436e91ac2997ac6b77700eba1e))
ROM_END

ROM_START( pk88 )
	ROM_REGION16_LE(0x100000,"maincpu", 0)

	ROM_LOAD( "b0.064", 0xf0000, 0x2000, CRC(80d3cf5d) SHA1(64769b7a8b60ffeefa04e4afbec778069a2840c9))
	ROM_LOAD( "b1.064", 0xf2000, 0x2000, CRC(673a4acc) SHA1(082ae803994048e225150f771794ca305f73d731))
	ROM_LOAD( "b2.064", 0xf4000, 0x2000, CRC(1ee66152) SHA1(7ed8c4c6c582487e802beabeca5b86702e5083e8))
	ROM_LOAD( "b3.064", 0xf6000, 0x2000, CRC(3062b3fc) SHA1(5134dd64721cbf093d059ee5d3fd09c7f86604c7))
	ROM_LOAD( "pk88-0.064", 0xfc000, 0x2000, CRC(1e4666cf) SHA1(6364c5241f2792909ff318194161eb2c29737546))
	ROM_LOAD( "pk88-1.064", 0xfe000, 0x2000, CRC(6fa7e7ef) SHA1(d68bc273baa46ba733ac6ad4df7569dd70cf60dd))

	ROM_REGION(0x2000,"gfx1", ROMREGION_ERASE00)
	// taken from mc1502
	ROM_LOAD( "symgen.rom", 0x0000, 0x2000, CRC(b2747a52) SHA1(6766d275467672436e91ac2997ac6b77700eba1e))
ROM_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

/*     YEAR     NAME        PARENT      COMPAT  MACHINE     INPUT       INIT                COMPANY       FULLNAME */
COMP ( 1989,    mc1502,     ibm5150,    0,      mc1502,     mc1502,     mc1502_state, mc1502,   "NPO Microprocessor", "Elektronika MC-1502", 0)
COMP ( 1990,    pk88,       ibm5150,    0,      pk88,       mc1502,     mc1502_state, mc1502,   "NPO Microprocessor", "Elektronika PK-88", GAME_NOT_WORKING | GAME_NO_SOUND)
