/***********************************************************************************

  Flicker Pinball

  Prototype create by Nutting Associates for Bally.

  Seems to be the first ever microprocessor-controlled pinball machine.

  2012-08-23 Made working [Robbbert]

  Inputs from US Patent 4093232
  Some clues from PinMAME

ToDo:
- Better artwork
- It freezes when F3 pressed or game tilted


************************************************************************************/

#include "emu.h"
#include "cpu/i4004/i4004.h"
#include "sound/beep.h"
#include "flicker.lh"

class flicker_state : public driver_device
{
public:
	flicker_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
	m_maincpu(*this, "maincpu"),
	m_beeper(*this, BEEPER_TAG)
	{ }

	DECLARE_WRITE8_MEMBER(port00_w);
	DECLARE_WRITE8_MEMBER(port01_w);
	DECLARE_WRITE8_MEMBER(port10_w);
	DECLARE_READ8_MEMBER(port02_r);

protected:

	// devices
	required_device<cpu_device> m_maincpu;
	required_device<device_t> m_beeper;

	// driver_device overrides
	virtual void machine_reset();
};


static ADDRESS_MAP_START( flicker_rom, AS_PROGRAM, 8, flicker_state )
	AM_RANGE(0x0000, 0x03FF) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START(flicker_map, AS_DATA, 8, flicker_state )
	AM_RANGE(0x0000, 0x00FF) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( flicker_io, AS_IO, 8, flicker_state )
	AM_RANGE(0x0000, 0x0000) AM_WRITE(port00_w)
	AM_RANGE(0x0001, 0x0001) AM_WRITE(port01_w)
	AM_RANGE(0x0002, 0x0002) AM_READ(port02_r)
	AM_RANGE(0x0010, 0x0010) AM_WRITE(port10_w)
ADDRESS_MAP_END

static INPUT_PORTS_START( flicker )
	PORT_START("TEST")
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Door Slam") PORT_CODE(KEYCODE_HOME)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("1 coin credit") PORT_CODE(KEYCODE_5)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("2 credit") PORT_CODE(KEYCODE_6)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("3 credit") PORT_CODE(KEYCODE_7)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("4 credit") PORT_CODE(KEYCODE_8)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("5 credit") PORT_CODE(KEYCODE_9)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("6 credit") PORT_CODE(KEYCODE_0)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Tilt") PORT_CODE(KEYCODE_T)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Start") PORT_CODE(KEYCODE_1)
	PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Test")

	PORT_START("B0")
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Left Lane Target") PORT_CODE(KEYCODE_W)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("/B Target") PORT_CODE(KEYCODE_E)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Left Lane 1000") PORT_CODE(KEYCODE_R)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("/A Target") PORT_CODE(KEYCODE_Y)
	PORT_START("B1")
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Right Lane Target") PORT_CODE(KEYCODE_U)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("/C Target") PORT_CODE(KEYCODE_I)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Right Lane 1000") PORT_CODE(KEYCODE_O)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("/D Target") PORT_CODE(KEYCODE_A)
	PORT_START("B2")
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Spinner") PORT_CODE(KEYCODE_S)
	PORT_START("B3")
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("10's Target") PORT_CODE(KEYCODE_D)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("100's Target") PORT_CODE(KEYCODE_F)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Pot Bumper") PORT_CODE(KEYCODE_G)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("3000 Hole") PORT_CODE(KEYCODE_H)
	PORT_START("B4")
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("1000 Bonus") PORT_CODE(KEYCODE_J)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("500 Target") PORT_CODE(KEYCODE_K)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Out Hole") PORT_CODE(KEYCODE_X)
	PORT_START("B5")
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Left 500 Out") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Left Bumper") PORT_CODE(KEYCODE_Z)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Right 500 Out") PORT_CODE(KEYCODE_C)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("Right Bumper") PORT_CODE(KEYCODE_V)
	PORT_START("B6")
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("A Target") PORT_CODE(KEYCODE_B)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("B target") PORT_CODE(KEYCODE_N)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("C target") PORT_CODE(KEYCODE_M)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("D Target") PORT_CODE(KEYCODE_COMMA)
INPUT_PORTS_END

READ8_MEMBER( flicker_state::port02_r )
{
	offset = cpu_get_reg(m_maincpu, I4004_RAM) - 0x20; // we need the full address

	if (offset < 7)
	{
		char kbdrow[6];
		sprintf(kbdrow,"B%X",offset);
		return ioport(kbdrow)->read();
	}
	return 0;
}

WRITE8_MEMBER( flicker_state::port00_w )
{
	static const UINT8 patterns[16] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0, 0, 0, 0, 0, 0 };
	offset = cpu_get_reg(m_maincpu, I4004_RAM); // we need the full address
	output_set_digit_value(offset, patterns[data]);
}

WRITE8_MEMBER( flicker_state::port01_w )
{
// The output lines operate the various lamps (44 of them)
	offset = cpu_get_reg(m_maincpu, I4004_RAM) - 0x10; // we need the full address

	if (offset < 0x10)
		i4004_set_test(m_maincpu, BIT(ioport("TEST")->read(), offset));
}

WRITE8_MEMBER( flicker_state::port10_w )
{
/* Outputs depend on data:
    1 = tens chime
    2 = hundreds chime
    3 = thousands chime
    4 = left bumper
    5 = right bumper
    6 = pot bumper
    7 = out hole
    8 = 3000 hole
    9 = knocker
    A = coin counter
    B = coin acceptor */
	offset = cpu_get_reg(m_maincpu, I4004_RAM) - 0x10; // we need the full address
	if (data && data != offset)
	{
		switch (offset)
		{
			case 0x01:
				beep_set_state(m_beeper, 1);
				beep_set_frequency(m_beeper, 2000);
				break;
			case 0x02:
				beep_set_state(m_beeper, 1);
				beep_set_frequency(m_beeper, 1500);
				break;
			case 0x03:
				beep_set_state(m_beeper, 1);
				beep_set_frequency(m_beeper, 800);
				break;
			case 0x09:
				beep_set_state(m_beeper, 1);
				beep_set_frequency(m_beeper, 200);
				break;
			case 0x0a:
				coin_counter_w(machine(), 0, 1);
				coin_counter_w(machine(), 0, 0);
				break;
			default:
				break;
		}
	}
	else
		beep_set_state(m_beeper, 0);
}


void flicker_state::machine_reset()
{
}

static MACHINE_CONFIG_START( flicker, flicker_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", I4004, XTAL_5MHz / 8)
	MCFG_CPU_PROGRAM_MAP(flicker_rom)
	MCFG_CPU_DATA_MAP(flicker_map)
	MCFG_CPU_IO_MAP(flicker_io)

	/* Video */
	MCFG_DEFAULT_LAYOUT(layout_flicker)

	/* Sound */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD(BEEPER_TAG, BEEP, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_CONFIG_END


ROM_START(flicker)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD("flicker.rom", 0x0000, 0x0400, CRC(c692e586) SHA1(5cabb28a074d18b589b5b8f700c57e1610071c68))
ROM_END

//   YEAR    GAME     PARENT  MACHINE   INPUT    CLASS           INIT      ORIENTATION    COMPANY             DESCRIPTION             FLAGS
GAME(1974,  flicker,  0,      flicker,  flicker, driver_device,  0,        ROT0,        "Nutting Associates", "Flicker (Prototype)", GAME_MECHANICAL )
