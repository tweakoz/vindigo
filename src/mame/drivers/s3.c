/***********************************************************************************

    Pinball
    Williams System 3

    Typical of Williams hardware: Motorola 8-bit CPUs, and lots of PIAs.

    Schematic and PinMAME used as references.

    Written during October 2012 [Robbbert]

    When first used, the nvram gets initialised but is otherwise unusable. A reboot
    will get it going.

ToDo:
- Diagnostic controls



************************************************************************************/


#include "machine/genpin.h"
#include "cpu/m6800/m6800.h"
#include "machine/6821pia.h"
#include "sound/dac.h"
#include "s3.lh"


class s3_state : public genpin_class
{
public:
	s3_state(const machine_config &mconfig, device_type type, const char *tag)
		: genpin_class(mconfig, type, tag),
	m_maincpu(*this, "maincpu"),
	m_dac(*this, "dac"),
	m_pia0(*this, "pia0"),
	m_pia1(*this, "pia1"),
	m_pia2(*this, "pia2"),
	m_pia3(*this, "pia3"),
	m_pia4(*this, "pia4")
	{ }

	DECLARE_READ8_MEMBER(dac_r);
	DECLARE_WRITE8_MEMBER(dac_w);
	DECLARE_WRITE8_MEMBER(dig0_w);
	DECLARE_WRITE8_MEMBER(dig1_w);
	DECLARE_WRITE8_MEMBER(lamp0_w);
	DECLARE_WRITE8_MEMBER(lamp1_w);
	DECLARE_WRITE8_MEMBER(sol0_w);
	DECLARE_WRITE8_MEMBER(sol1_w);
	DECLARE_READ8_MEMBER(switch_r);
	DECLARE_WRITE8_MEMBER(switch_w);
	DECLARE_READ_LINE_MEMBER(cb1_r);
	TIMER_DEVICE_CALLBACK_MEMBER(irq);
	DECLARE_INPUT_CHANGED_MEMBER(nmi);
	DECLARE_MACHINE_RESET(s3);
	DECLARE_MACHINE_RESET(s3a);
protected:

	// devices
	required_device<cpu_device> m_maincpu;
	optional_device<dac_device> m_dac;
	required_device<pia6821_device> m_pia0;
	required_device<pia6821_device> m_pia1;
	required_device<pia6821_device> m_pia2;
	required_device<pia6821_device> m_pia3;
	optional_device<pia6821_device> m_pia4;
private:
	UINT8 m_t_c;
	UINT8 m_sound_data;
	UINT8 m_strobe;
	UINT8 m_kbdrow;
	bool m_cb1;
	bool m_data_ok;
	bool m_chimes;
};

static ADDRESS_MAP_START( s3_main_map, AS_PROGRAM, 8, s3_state )
	ADDRESS_MAP_GLOBAL_MASK(0x7fff)
	AM_RANGE(0x0000, 0x00ff) AM_RAM
	AM_RANGE(0x0100, 0x017f) AM_RAM AM_SHARE("nvram")
	AM_RANGE(0x2200, 0x2203) AM_DEVREADWRITE("pia0", pia6821_device, read, write) // solenoids
	AM_RANGE(0x2400, 0x2403) AM_DEVREADWRITE("pia1", pia6821_device, read, write) // lamps
	AM_RANGE(0x2800, 0x2803) AM_DEVREADWRITE("pia2", pia6821_device, read, write) // display
	AM_RANGE(0x3000, 0x3003) AM_DEVREADWRITE("pia3", pia6821_device, read, write) // inputs
	AM_RANGE(0x6000, 0x67ff) AM_ROM
	AM_RANGE(0x7000, 0x7fff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( s3_audio_map, AS_PROGRAM, 8, s3_state )
	ADDRESS_MAP_GLOBAL_MASK(0xfff)
	AM_RANGE(0x0000, 0x007f) AM_RAM
	AM_RANGE(0x0400, 0x0403) AM_DEVREADWRITE("pia4", pia6821_device, read, write) // sounds
	AM_RANGE(0x0800, 0x0fff) AM_ROM
ADDRESS_MAP_END

static INPUT_PORTS_START( s3 )
	PORT_START("X0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Q)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_W)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_E)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_R)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Y)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_U)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_I)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_O)

	PORT_START("X1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER )

	PORT_START("X2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_S)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_D)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_F)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_G)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_H)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_J)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_K)

	PORT_START("X4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_L)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Z)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_V)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_B)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_N)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_M)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_COMMA)

	PORT_START("X8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_STOP)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_SLASH)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_COLON)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Outhole") PORT_CODE(KEYCODE_X)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_MINUS)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_EQUALS)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_BACKSPACE)

	PORT_START("X10")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_BACKSLASH)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_ENTER)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_LEFT)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_UP)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_DOWN)

	PORT_START("X20")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("X40")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("X80")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SND")
	PORT_BIT( 0xbf, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Music") PORT_CODE(KEYCODE_9) PORT_TOGGLE

	PORT_START("DIAGS")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("Diagnostic") PORT_CODE(KEYCODE_0) PORT_CHANGED_MEMBER(DEVICE_SELF, s3_state, nmi, 1)
INPUT_PORTS_END

MACHINE_RESET_MEMBER( s3_state, s3 )
{
	m_t_c = 0;
	m_chimes = 1;
}

MACHINE_RESET_MEMBER( s3_state, s3a )
{
	m_t_c = 0;
	m_chimes = 0;
}

INPUT_CHANGED_MEMBER( s3_state::nmi )
{
	// Diagnostic button sends a pulse to NMI pin
	if (newval==CLEAR_LINE)
		m_maincpu->set_input_line(INPUT_LINE_NMI, PULSE_LINE);
}

WRITE8_MEMBER( s3_state::sol0_w )
{
	if (BIT(data, 0))
		m_samples->start(2, 5); // outhole
}

WRITE8_MEMBER( s3_state::sol1_w )
{
	if (m_chimes)
	{
		if (BIT(data, 0))
			m_samples->start(1, 1); // 10 chime
		else
		if (BIT(data, 1))
			m_samples->start(2, 2); // 100 chime
		else
		if (BIT(data, 2))
			m_samples->start(3, 3); // 1000 chime
		// we don't have a 10k chime in samples yet
		//else
		//if (BIT(data, 3))
			//m_samples->start(1, x); // 10k chime
	}
	else
	{
		m_sound_data = ioport("SND")->read(); // 0xff or 0xbf
		if (BIT(data, 0))
			m_sound_data &= 0xfe;
		else
		if (BIT(data, 1))
			m_sound_data &= 0xfd;
		else
		if (BIT(data, 2))
			m_sound_data &= 0xfb;
		else
		if (BIT(data, 3))
			m_sound_data &= 0xf7;
		else
		if (BIT(data, 4))
			m_sound_data &= 0x7f;

		if ((m_sound_data & 0xbf) == 0xbf)
		{
			m_cb1 = 0;
			m_pia4->cb1_w(0);
		}
		else
		{
			m_cb1 = 1;
			m_pia4->cb1_w(1);
		}
	}

	if (BIT(data, 5))
		m_samples->start(0, 6); // knocker
}

static const pia6821_interface pia0_intf =
{
	DEVCB_NULL,		/* port A in */
	DEVCB_NULL,		/* port B in */
	DEVCB_LINE_GND,		/* line CA1 in */
	DEVCB_LINE_GND,		/* line CB1 in */
	DEVCB_NULL,		/* line CA2 in */
	DEVCB_NULL,		/* line CB2 in */
	DEVCB_DRIVER_MEMBER(s3_state, sol0_w),		/* port A out */
	DEVCB_DRIVER_MEMBER(s3_state, sol1_w),		/* port B out */
	DEVCB_NULL,		/* line CA2 out */
	DEVCB_NULL,		/* port CB2 out */
	DEVCB_CPU_INPUT_LINE("maincpu", M6800_IRQ_LINE),		/* IRQA */
	DEVCB_CPU_INPUT_LINE("maincpu", M6800_IRQ_LINE)		/* IRQB */
};

WRITE8_MEMBER( s3_state::lamp0_w )
{
	m_maincpu->set_input_line(M6800_IRQ_LINE, CLEAR_LINE);
}

WRITE8_MEMBER( s3_state::lamp1_w )
{
}

static const pia6821_interface pia1_intf =
{
	DEVCB_NULL,		/* port A in */
	DEVCB_NULL,		/* port B in */
	DEVCB_LINE_GND,		/* line CA1 in */
	DEVCB_LINE_GND,		/* line CB1 in */
	DEVCB_NULL,		/* line CA2 in */
	DEVCB_NULL,		/* line CB2 in */
	DEVCB_DRIVER_MEMBER(s3_state, lamp0_w),		/* port A out */
	DEVCB_DRIVER_MEMBER(s3_state, lamp1_w),		/* port B out */
	DEVCB_NULL,		/* line CA2 out */
	DEVCB_NULL,		/* port CB2 out */
	DEVCB_CPU_INPUT_LINE("maincpu", M6800_IRQ_LINE),		/* IRQA */
	DEVCB_CPU_INPUT_LINE("maincpu", M6800_IRQ_LINE)		/* IRQB */
};

WRITE8_MEMBER( s3_state::dig0_w )
{
	m_strobe = data;
	m_data_ok = true;
}

WRITE8_MEMBER( s3_state::dig1_w )
{
	static const UINT8 patterns[16] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67, 0, 0, 0, 0, 0, 0 }; // MC14558
	if (m_data_ok)
	{
		output_set_digit_value(m_strobe+16, patterns[data&15]);
		output_set_digit_value(m_strobe, patterns[data>>4]);
	}
	m_data_ok = false;
}

static const pia6821_interface pia2_intf =
{
	DEVCB_NULL,		/* port A in */
	DEVCB_NULL,		/* port B in */
	DEVCB_NULL,		/* line CA1 in */
	DEVCB_NULL,		/* line CB1 in */
	DEVCB_NULL,		/* line CA2 in */
	DEVCB_NULL,		/* line CB2 in */
	DEVCB_DRIVER_MEMBER(s3_state, dig0_w),		/* port A out */
	DEVCB_DRIVER_MEMBER(s3_state, dig1_w),		/* port B out */
	DEVCB_NULL,		/* line CA2 out */
	DEVCB_NULL,		/* port CB2 out */
	DEVCB_CPU_INPUT_LINE("maincpu", M6800_IRQ_LINE),		/* IRQA */
	DEVCB_CPU_INPUT_LINE("maincpu", M6800_IRQ_LINE)		/* IRQB */
};

READ8_MEMBER( s3_state::switch_r )
{
	char kbdrow[8];
	sprintf(kbdrow,"X%X",m_kbdrow);
	return ioport(kbdrow)->read();
}

WRITE8_MEMBER( s3_state::switch_w )
{
	m_kbdrow = data;
}

static const pia6821_interface pia3_intf =
{
	DEVCB_DRIVER_MEMBER(s3_state, switch_r),		/* port A in */
	DEVCB_NULL,		/* port B in */
	DEVCB_LINE_GND,		/* line CA1 in */
	DEVCB_LINE_GND,		/* line CB1 in */
	DEVCB_NULL,		/* line CA2 in */
	DEVCB_NULL,		/* line CB2 in */
	DEVCB_NULL,		/* port A out */
	DEVCB_DRIVER_MEMBER(s3_state, switch_w),		/* port B out */
	DEVCB_NULL,		/* line CA2 out */
	DEVCB_NULL,		/* port CB2 out */
	DEVCB_CPU_INPUT_LINE("maincpu", M6800_IRQ_LINE),		/* IRQA */
	DEVCB_CPU_INPUT_LINE("maincpu", M6800_IRQ_LINE)		/* IRQB */
};

READ_LINE_MEMBER( s3_state::cb1_r )
{
	return m_cb1;
}

READ8_MEMBER( s3_state::dac_r )
{
	return m_sound_data;	
}

WRITE8_MEMBER( s3_state::dac_w )
{
	m_dac->write_unsigned8(data);
}

static const pia6821_interface pia4_intf =
{
	DEVCB_NULL,		/* port A in */
	DEVCB_DRIVER_MEMBER(s3_state, dac_r),		/* port B in */
	DEVCB_NULL,		/* line CA1 in */
	DEVCB_DRIVER_LINE_MEMBER(s3_state, cb1_r),		/* line CB1 in */
	DEVCB_NULL,		/* line CA2 in */
	DEVCB_NULL,		/* line CB2 in */
	DEVCB_DRIVER_MEMBER(s3_state, dac_w),		/* port A out */
	DEVCB_NULL,		/* port B out */
	DEVCB_NULL,		/* line CA2 out */
	DEVCB_NULL,		/* port CB2 out */
	DEVCB_CPU_INPUT_LINE("audiocpu", M6800_IRQ_LINE),		/* IRQA */
	DEVCB_CPU_INPUT_LINE("audiocpu", M6800_IRQ_LINE)		/* IRQB */
};

TIMER_DEVICE_CALLBACK_MEMBER( s3_state::irq)
{
	if (m_t_c > 0x70)
		m_maincpu->set_input_line(M6800_IRQ_LINE, ASSERT_LINE);
	else
		m_t_c++;
}

static MACHINE_CONFIG_START( s3, s3_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6800, 3580000)
	MCFG_CPU_PROGRAM_MAP(s3_main_map)
	MCFG_TIMER_DRIVER_ADD_PERIODIC("irq", s3_state, irq, attotime::from_hz(1000))
	MCFG_MACHINE_RESET_OVERRIDE(s3_state, s3)

	/* Video */
	MCFG_DEFAULT_LAYOUT(layout_s3)

	/* Sound */
	MCFG_FRAGMENT_ADD( genpin_audio )

	/* Devices */
	MCFG_PIA6821_ADD("pia0", pia0_intf)
	MCFG_PIA6821_ADD("pia1", pia1_intf)
	MCFG_PIA6821_ADD("pia2", pia2_intf)
	MCFG_PIA6821_ADD("pia3", pia3_intf)
	MCFG_NVRAM_ADD_1FILL("nvram")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( s3a, s3 )
	/* Add the soundcard */
	MCFG_CPU_ADD("audiocpu", M6802, 3580000)
	MCFG_CPU_PROGRAM_MAP(s3_audio_map)
	MCFG_MACHINE_RESET_OVERRIDE(s3_state, s3a)
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("dac", DAC, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)
	MCFG_PIA6821_ADD("pia4", pia4_intf)
MACHINE_CONFIG_END


/*-------------------------------------
/ Contact - Sys.3 (Game #482)
/-------------------------------------*/
ROM_START(cntct_l1)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("gamerom.716", 0x6000, 0x0800, CRC(35359b60) SHA1(ab4c3328d93bdb4c952090b327c91b0ded36152c))
	ROM_LOAD("white1.716", 0x7000, 0x0800, CRC(9bbbf14f) SHA1(b0542ffdd683fa0ea4a9819576f3789cd5a4b2eb))
	ROM_LOAD("white2.716", 0x7800, 0x0800, CRC(4d4010dd) SHA1(11221124fef3b7bf82d353d65ce851495f6946a7))

	ROM_REGION(0x10000, "audiocpu", 0)
	ROM_LOAD("sound1.716", 0x0800, 0x0800, CRC(f4190ca3) SHA1(ee234fb5c894fca5876ee6dc7ea8e89e7e0aec9c))
ROM_END

/*-------------------------------------
/ Disco Fever - Sys.3 (Game #483)
/-------------------------------------*/
ROM_START(disco_l1)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("gamerom.716", 0x6000, 0x0800, CRC(831d8adb) SHA1(99a9c3d5c8cbcdf3bb9c210ad9d05c34905b272e))
	ROM_LOAD("white1.716", 0x7000, 0x0800, CRC(9bbbf14f) SHA1(b0542ffdd683fa0ea4a9819576f3789cd5a4b2eb))
	ROM_LOAD("white2.716", 0x7800, 0x0800, CRC(4d4010dd) SHA1(11221124fef3b7bf82d353d65ce851495f6946a7))

	ROM_REGION(0x10000, "audiocpu", 0)
	ROM_LOAD("sound1.716", 0x0800, 0x0800, CRC(f4190ca3) SHA1(ee234fb5c894fca5876ee6dc7ea8e89e7e0aec9c))
ROM_END

/*----------------------------
/ Hot Tip - Sys.3 (Game #477) - No Sound board
/----------------------------*/
ROM_START(httip_l1)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("gamerom.716", 0x6000, 0x0800, CRC(b1d4fd9b) SHA1(e55ecf1328a55979c4cf8f3fb4e6761747e0abc4))
	ROM_LOAD("white1.716", 0x7000, 0x0800, CRC(9bbbf14f) SHA1(b0542ffdd683fa0ea4a9819576f3789cd5a4b2eb))
	ROM_LOAD("white2.716", 0x7800, 0x0800, CRC(4d4010dd) SHA1(11221124fef3b7bf82d353d65ce851495f6946a7))
ROM_END

/*---------------------------------
/ Lucky Seven - Sys.3 (Game #480) - No Sound board
/---------------------------------*/
ROM_START(lucky_l1)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("gamerom.716", 0x6000, 0x0800, CRC(7cfbd4c7) SHA1(825e2245fd1615e932973f5e2b5ed5f2da9309e7))
	ROM_LOAD("white1.716", 0x7000, 0x0800, CRC(9bbbf14f) SHA1(b0542ffdd683fa0ea4a9819576f3789cd5a4b2eb))
	ROM_LOAD("white2.716", 0x7800, 0x0800, CRC(4d4010dd) SHA1(11221124fef3b7bf82d353d65ce851495f6946a7))
ROM_END

/*-------------------------------------
/ World Cup Soccer - Sys.3 (Game #481)
/-------------------------------------*/
ROM_START(wldcp_l1)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("gamerom.716", 0x6000, 0x0800, CRC(c8071956) SHA1(0452aaf2ec1bcc5717fe52a6c541d79402bebb17))
	ROM_LOAD("white1.716", 0x7000, 0x0800, CRC(9bbbf14f) SHA1(b0542ffdd683fa0ea4a9819576f3789cd5a4b2eb))
	ROM_LOAD("white2wc.716", 0x7800, 0x0800, CRC(618d15b5) SHA1(527387893eeb2cd4aa563a4cfb1948a15d2ed741))

	ROM_REGION(0x10000, "audiocpu", 0)
	ROM_LOAD("sound1.716", 0x0800, 0x0800, CRC(f4190ca3) SHA1(ee234fb5c894fca5876ee6dc7ea8e89e7e0aec9c))
ROM_END


GAME( 1977, httip_l1, 0, s3,  s3, driver_device, 0, ROT0, "Williams", "Hot Tip (L-1)", GAME_MECHANICAL )
GAME( 1977, lucky_l1, 0, s3,  s3, driver_device, 0, ROT0, "Williams", "Lucky Seven (L-1)", GAME_MECHANICAL )
GAME( 1978, wldcp_l1, 0, s3a, s3, driver_device, 0, ROT0, "Williams", "World Cup Soccer (L-1)", GAME_MECHANICAL | GAME_NOT_WORKING)
GAME( 1978, cntct_l1, 0, s3a, s3, driver_device, 0, ROT0, "Williams", "Contact (L-1)", GAME_MECHANICAL)
GAME( 1978, disco_l1, 0, s3a, s3, driver_device, 0, ROT0, "Williams", "Disco Fever (L-1)", GAME_MECHANICAL)
