// license:BSD-3-Clause
// copyright-holders:Sandro Ronco
/***************************************************************************

        VTech PreComputer 1000 / 2000

        04/12/2009 Skeleton driver.

        TODO:
        - fix MisterX LCD
        - MisterX bankswitch
        - dump the chargen

****************************************************************************/


#include "emu.h"
#include "cpu/z80/z80.h"
#include "video/hd44780.h"
#include "sound/beep.h"
#include "imagedev/cartslot.h"
#include "rendlay.h"


class pc2000_state : public driver_device
{
public:
	pc2000_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, "maincpu"),
			m_lcdc(*this, "hd44780"),
			m_beep(*this, "beeper"),
			m_bank1(*this, "bank1"),
			m_bank2(*this, "bank2")
		{ }

	required_device<cpu_device> m_maincpu;
	required_device<hd44780_device> m_lcdc;
	required_device<beep_device> m_beep;
	required_memory_bank m_bank1;
	optional_memory_bank m_bank2;

	UINT8 m_mux_data;
	UINT8 m_beep_state;

	virtual void machine_start();
	virtual void machine_reset();

	DECLARE_DEVICE_IMAGE_LOAD_MEMBER( cart_load );
	DECLARE_READ8_MEMBER( key_matrix_r );
	DECLARE_WRITE8_MEMBER( key_matrix_w );
	DECLARE_WRITE8_MEMBER( rombank1_w );
	DECLARE_WRITE8_MEMBER( rombank2_w );
	DECLARE_READ8_MEMBER( beep_r );
	DECLARE_WRITE8_MEMBER( beep_w );
	virtual void palette_init();
};

class pc1000_state : public pc2000_state
{
public:
	pc1000_state(const machine_config &mconfig, device_type type, const char *tag)
		: pc2000_state(mconfig, type, tag)
		{ }


	virtual void machine_start();
	virtual void machine_reset();

	DECLARE_READ8_MEMBER( kb_r );
	DECLARE_READ8_MEMBER( lcdc_data_r );
	DECLARE_WRITE8_MEMBER( lcdc_data_w );
	DECLARE_READ8_MEMBER( lcdc_control_r );
	DECLARE_WRITE8_MEMBER( lcdc_control_w );
};


/* TODO: put a breakpoint at 1625 and test the inputs, writes at dce4 are the scancode values */
READ8_MEMBER( pc2000_state::key_matrix_r )
{
	static const char *const bitnames[2][8] =
	{
		{"IN0", "IN1", "IN2", "IN3", "IN4", "IN5", "IN6", "IN7"},
		{"IN8", "IN9", "INA", "INB", "INC", "IND", "INE", "INF"}
	};

	UINT8 data = 0xff;

	for (int line=0; line<8; line++)
		if (m_mux_data & (1<<line))
			data &= ioport(bitnames[offset][line])->read();

	return data;
}

WRITE8_MEMBER( pc2000_state::key_matrix_w )
{
	m_mux_data = data;
}

WRITE8_MEMBER( pc2000_state::rombank1_w )
{
	m_bank1->set_entry(data & 0x0f);
}

WRITE8_MEMBER( pc2000_state::rombank2_w )
{
	if (data & 0x80)
	{
		m_bank2->set_entry(data & 0x8f);   //cartridge
	}
	else
	{
		m_bank2->set_entry(data & 0x0f);
	}
}

READ8_MEMBER( pc2000_state::beep_r )
{
	return m_beep_state;
}

WRITE8_MEMBER( pc2000_state::beep_w )
{
	m_beep->set_state(BIT(data, 3));
	m_beep_state = data;
}

static ADDRESS_MAP_START(pc2000_mem, AS_PROGRAM, 8, pc2000_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x3fff) AM_ROM AM_REGION("bios", 0x00000)
	AM_RANGE(0x4000, 0x7fff) AM_ROMBANK("bank1")
	AM_RANGE(0x8000, 0xbfff) AM_ROMBANK("bank2")    //0x8000 - 0xbfff tests a cartridge, header is 0x55 0xaa 0x59 0x45, if it succeeds a jump at 0x8004 occurs
	AM_RANGE(0xc000, 0xdfff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( pc2000_io , AS_IO, 8, pc2000_state)
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x01, 0x01) AM_WRITE(rombank1_w)
	AM_RANGE(0x03, 0x03) AM_WRITE(rombank2_w)
	AM_RANGE(0x0a, 0x0b) AM_DEVREADWRITE("hd44780", hd44780_device, read, write)
	AM_RANGE(0x10, 0x11) AM_READWRITE(key_matrix_r, key_matrix_w)
	AM_RANGE(0x12, 0x12) AM_READWRITE(beep_r, beep_w)
ADDRESS_MAP_END

READ8_MEMBER( pc1000_state::kb_r )
{
	static const char *const bitnames[9] =
	{
		"IN0", "IN1", "IN2", "IN3", "IN4", "IN5", "IN6", "IN7", "IN8"
	};

	UINT8 data = 0xff;

	for (int line=0; line<9; line++)
		if (!(offset & (1<<line)))
			data &= ioport(bitnames[line])->read();

	return data;
}

READ8_MEMBER( pc1000_state::lcdc_data_r )
{
	//logerror("lcdc data r\n");
	return m_lcdc->data_read(space, 0)>>4;
}

WRITE8_MEMBER( pc1000_state::lcdc_data_w )
{
	//popmessage("%s", (char*)m_maincpu->space(AS_PROGRAM).get_read_ptr(0x4290));

	//logerror("lcdc data w %x\n", data);
	m_lcdc->data_write(space, 0, data<<4);
}

READ8_MEMBER( pc1000_state::lcdc_control_r )
{
	//logerror("lcdc control r\n");
	return m_lcdc->control_read(space, 0)>>4;
}

WRITE8_MEMBER( pc1000_state::lcdc_control_w )
{
	//logerror("lcdc control w %x\n", data);
	m_lcdc->control_write(space, 0, data<<4);
}

static HD44780_PIXEL_UPDATE(pc1000_pixel_update)
{
	UINT8 layout[] = { 0x00, 0x4f, 0x4e, 0x4d, 0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x47, 0x40, 0x3f, 0x3e, 0x3d, 0x3c, 0x3b, 0x3a, 0x39, 0x38, 0x37 };
	//UINT8 layout[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49 };

	for(int i=0; i<20; i++)
		if (pos == layout[i])
		{
			bitmap.pix16(line*9 + y, i*6 + x) = state;
			break;
		}
}

static ADDRESS_MAP_START(pc1000_mem, AS_PROGRAM, 8, pc1000_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x3fff) AM_ROM AM_REGION("bios", 0x00000)
	AM_RANGE(0x4000, 0x47ff) AM_RAM
	AM_RANGE(0x8000, 0xbfff) AM_ROM AM_REGION("cart", 0x00000)    //0x8000 - 0xbfff tests a cartridge, header is 0x55 0xaa 0x33, if it succeeds a jump at 0x8010 occurs
	AM_RANGE(0xc000, 0xffff) AM_ROMBANK("bank1")
ADDRESS_MAP_END

static ADDRESS_MAP_START( pc1000_io , AS_IO, 8, pc1000_state)
	AM_RANGE(0x0000, 0x01ff) AM_READ(kb_r)
	AM_RANGE(0x4000, 0x4000) AM_MIRROR(0xfe) AM_READWRITE(lcdc_control_r, lcdc_control_w)
	AM_RANGE(0x4100, 0x4100) AM_MIRROR(0xfe) AM_READWRITE(lcdc_data_r, lcdc_data_w)
ADDRESS_MAP_END

/* Input ports */
static INPUT_PORTS_START( pc2000 )
	PORT_START("IN0")
	PORT_DIPNAME( 0x01, 0x01, "IN0" ) //0x83
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

	PORT_START("IN1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Caps Lock")  PORT_CODE(KEYCODE_CAPSLOCK)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1")          PORT_CODE(KEYCODE_1) PORT_CHAR('1')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Q")          PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A")          PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Z")          PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("\\")         PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Space")      PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("IN2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2")  PORT_CODE(KEYCODE_2) PORT_CHAR('2')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3")  PORT_CODE(KEYCODE_3) PORT_CHAR('3')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E")  PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("S")  PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D")  PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("X")  PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C")  PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("W")  PORT_CODE(KEYCODE_W) PORT_CHAR('W')

	PORT_START("IN3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4")  PORT_CODE(KEYCODE_4) PORT_CHAR('4')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5")  PORT_CODE(KEYCODE_5) PORT_CHAR('5')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("T")  PORT_CODE(KEYCODE_T) PORT_CHAR('T')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F")  PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G")  PORT_CODE(KEYCODE_G) PORT_CHAR('G')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("V")  PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B")  PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("R")  PORT_CODE(KEYCODE_R) PORT_CHAR('R')

	PORT_START("IN4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6")  PORT_CODE(KEYCODE_6) PORT_CHAR('6')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7")  PORT_CODE(KEYCODE_7) PORT_CHAR('7')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("U")  PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("H")  PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("J")  PORT_CODE(KEYCODE_J) PORT_CHAR('J')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("N")  PORT_CODE(KEYCODE_N) PORT_CHAR('N')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("M")  PORT_CODE(KEYCODE_M) PORT_CHAR('M')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Y")  PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')

	PORT_START("IN5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8")  PORT_CODE(KEYCODE_8) PORT_CHAR('8')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9")  PORT_CODE(KEYCODE_9) PORT_CHAR('9')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("O")  PORT_CODE(KEYCODE_O) PORT_CHAR('O')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("K")  PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("L")  PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(",")  PORT_CODE(KEYCODE_COMMA) PORT_CHAR(',')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(".")  PORT_CODE(KEYCODE_STOP) PORT_CHAR('.')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("I")  PORT_CODE(KEYCODE_I) PORT_CHAR('I')

	PORT_START("IN6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0")  PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("HOME")   PORT_CODE(KEYCODE_HOME)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("=")  PORT_CODE(KEYCODE_EQUALS)   PORT_CHAR('=') PORT_CHAR('+')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(";")  PORT_CODE(KEYCODE_COLON) PORT_CHAR(';')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("\'") PORT_CODE(KEYCODE_QUOTE) PORT_CHAR('\'')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("/")  PORT_CODE(KEYCODE_SLASH) PORT_CHAR('/')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("P")  PORT_CODE(KEYCODE_P) PORT_CHAR('P')

	PORT_START("IN7")
	PORT_BIT(0x03, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("LEFT")   PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("RIGHT")  PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("ENTER")  PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("DEL")    PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("ESC")    PORT_CODE(KEYCODE_ESC)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("IN8")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Scramble")           PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Guess Rork")         PORT_CODE(KEYCODE_F2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Missing Letter")     PORT_CODE(KEYCODE_F3)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Letter Hunt")        PORT_CODE(KEYCODE_F4)
	PORT_BIT(0xf0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("IN9")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Letter Zapper")      PORT_CODE(KEYCODE_F5)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Letter Switch")      PORT_CODE(KEYCODE_F6)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Sentence Jumble")    PORT_CODE(KEYCODE_F7)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Grammar Challenge")  PORT_CODE(KEYCODE_F8)
	PORT_BIT(0xf0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("INA")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Plurals")            PORT_CODE(KEYCODE_F9)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Past Tense")         PORT_CODE(KEYCODE_F10)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Synonyms")           PORT_CODE(KEYCODE_F11)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Antonyms")           PORT_CODE(KEYCODE_F12)
	PORT_BIT(0xf0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("INB")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Basic Math")         PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Advanced Math")      PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Math Riddles")       PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Number Challenge")   PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0xf0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("INC")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ecology")            PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("History")            PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Geography")          PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Super Power")        PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0xf0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("IND")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Typing Game")        PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Alpha Jumble")       PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Word Flash")         PORT_CODE(KEYCODE_PGUP)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Sign Search")        PORT_CODE(KEYCODE_PGDN)
	PORT_BIT(0xf0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("INE")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PC2000 Basic")       PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Spell Checker")      PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Calculator")         PORT_CODE(KEYCODE_RSHIFT)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Load Cartridge")     PORT_CODE(KEYCODE_RCONTROL)
	PORT_BIT(0xf0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("INF")
	PORT_DIPNAME( 0x01, 0x01, "INF" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(DEF_STR( Level_Select )) PORT_CODE(KEYCODE_LSHIFT)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Num Players")        PORT_CODE(KEYCODE_LCONTROL)
	PORT_BIT(0xf0, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END

static INPUT_PORTS_START( pc1000 )
	PORT_START("IN0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SCHREIBMASCHINENKURS") PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("COMPUTER-UBUNGEN") PORT_CODE(KEYCODE_F2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("ALLGEMEINWISSEN") PORT_CODE(KEYCODE_F3)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("MATHE")      PORT_CODE(KEYCODE_F7)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("GESCHICHTE") PORT_CODE(KEYCODE_F4)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("GEOGRAPHIE") PORT_CODE(KEYCODE_F5)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("NATURWISSENSCHAFTEN") PORT_CODE(KEYCODE_F6)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SPIELE")     PORT_CODE(KEYCODE_F8)

	PORT_START("IN1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KASSETTE")   PORT_CODE(KEYCODE_F9)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("AUS")        PORT_CODE(KEYCODE_F12)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Left")       PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Right")      PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("?")          PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("?")          PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("?")          PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("ELEKRONIK-RECHNER")   PORT_CODE(KEYCODE_F10)

	PORT_START("IN2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0")  PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("?")  PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("\xc3\xb6")  PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("\xc3\xa4")  PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("\xc3\xbc")  PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("-")  PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift")  PORT_CODE(KEYCODE_LSHIFT)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("P")  PORT_CODE(KEYCODE_P) PORT_CHAR('P')

	PORT_START("IN3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8")  PORT_CODE(KEYCODE_8) PORT_CHAR('8')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9")  PORT_CODE(KEYCODE_9) PORT_CHAR('9')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("O")  PORT_CODE(KEYCODE_O) PORT_CHAR('O')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("K")  PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("L")  PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(",")  PORT_CODE(KEYCODE_COMMA) PORT_CHAR(',')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME(".")  PORT_CODE(KEYCODE_STOP) PORT_CHAR('.')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("I")  PORT_CODE(KEYCODE_I) PORT_CHAR('I')

	PORT_START("IN4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6")  PORT_CODE(KEYCODE_6) PORT_CHAR('6')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7")  PORT_CODE(KEYCODE_7) PORT_CHAR('7')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("U")  PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("H")  PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("J")  PORT_CODE(KEYCODE_J) PORT_CHAR('J')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("N")  PORT_CODE(KEYCODE_N) PORT_CHAR('N')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("M")  PORT_CODE(KEYCODE_M) PORT_CHAR('M')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Z")  PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')

	PORT_START("IN5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4")  PORT_CODE(KEYCODE_4) PORT_CHAR('4')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5")  PORT_CODE(KEYCODE_5) PORT_CHAR('5')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("T")  PORT_CODE(KEYCODE_T) PORT_CHAR('T')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("F")  PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("G")  PORT_CODE(KEYCODE_G) PORT_CHAR('G')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("V")  PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B")  PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("R")  PORT_CODE(KEYCODE_R) PORT_CHAR('R')

	PORT_START("IN6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2")  PORT_CODE(KEYCODE_2) PORT_CHAR('2')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3")  PORT_CODE(KEYCODE_3) PORT_CHAR('3')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("E")  PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("S")  PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("D")  PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("X")  PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("C")  PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("W")  PORT_CODE(KEYCODE_W) PORT_CHAR('W')

	PORT_START("IN7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Caps Lock")  PORT_CODE(KEYCODE_CAPSLOCK)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1")  PORT_CODE(KEYCODE_1) PORT_CHAR('1')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Q")  PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A")  PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Y")  PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("DEL") PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Space") PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5_PAD)

	PORT_START("IN8")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0xfc, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END


DEVICE_IMAGE_LOAD_MEMBER(pc2000_state, cart_load)
{
	UINT8 *cart = memregion("cart")->base();

	if (image.software_entry() == NULL)
	{
		UINT32 size = MIN(image.length(), memregion("cart")->bytes());

		if (image.fread(cart, size) != size)
			return IMAGE_INIT_FAIL;
	}
	else
	{
		UINT32 size = MIN(image.get_software_region_length("rom"), memregion("cart")->bytes());
		const char *pcb_type = image.get_feature("pcb_type");

		if (pcb_type && !strcmp(pcb_type, "allgeme2"))
			memcpy(cart, image.get_software_region("rom"), size >> 1);
		else if (pcb_type && !strcmp(pcb_type, "ftrivia"))
			memcpy(cart, image.get_software_region("rom") + (1<<16), size >> 1);
		else
			memcpy(cart, image.get_software_region("rom"), size);
	}

	return IMAGE_INIT_PASS;
}


void pc2000_state::machine_start()
{
	UINT8 *bios = memregion("bios")->base();
	UINT8 *cart = memregion("cart")->base();

	m_bank1->configure_entries(0, 0x10, bios, 0x4000);
	m_bank2->configure_entries(0, 0x10, bios, 0x4000);
	m_bank2->configure_entries(0x80, 0x10, cart, 0x4000);
}

void pc2000_state::machine_reset()
{
	//set the initial bank
	m_bank1->set_entry(0);
	m_bank2->set_entry(0);
}

void pc1000_state::machine_start()
{
	UINT8 *bios = memregion("bios")->base();
	m_bank1->configure_entries(0, 0x08, bios, 0x4000);
}

void pc1000_state::machine_reset()
{
	m_bank1->set_entry(0);
}

void pc2000_state::palette_init()
{
	palette_set_color(machine(), 0, MAKE_RGB(138, 146, 148));
	palette_set_color(machine(), 1, MAKE_RGB(92, 83, 88));
}

static const gfx_layout hd44780_charlayout =
{
	5, 8,                   /* 5 x 8 characters */
	256,                    /* 256 characters */
	1,                      /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	{ 3, 4, 5, 6, 7},
	{ 0, 8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8},
	8*8                     /* 8 bytes */
};

static GFXDECODE_START( pc2000 )
	GFXDECODE_ENTRY( "hd44780:cgrom", 0x0000, hd44780_charlayout, 0, 1 )
GFXDECODE_END

static MACHINE_CONFIG_START( pc2000, pc2000_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",Z80, XTAL_4MHz) /* probably not accurate */
	MCFG_CPU_PROGRAM_MAP(pc2000_mem)
	MCFG_CPU_IO_MAP(pc2000_io)
	MCFG_CPU_PERIODIC_INT_DRIVER(pc2000_state, irq0_line_hold, 50)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", LCD)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_UPDATE_DEVICE("hd44780", hd44780_device, screen_update)
	MCFG_SCREEN_SIZE(120, 18) //2x20 chars
	MCFG_SCREEN_VISIBLE_AREA(0, 120-1, 0, 18-1)

	MCFG_PALETTE_LENGTH(2)
	MCFG_GFXDECODE(pc2000)
	MCFG_DEFAULT_LAYOUT(layout_lcd)

	MCFG_HD44780_ADD("hd44780")
	MCFG_HD44780_LCD_SIZE(2, 20)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO( "mono" )
	MCFG_SOUND_ADD( "beeper", BEEP, 0 )
	MCFG_SOUND_ROUTE( ALL_OUTPUTS, "mono", 1.00 )

	MCFG_CARTSLOT_ADD("cart")
	MCFG_CARTSLOT_EXTENSION_LIST("bin")
	MCFG_CARTSLOT_INTERFACE("pc1000_cart")
	MCFG_CARTSLOT_LOAD(pc2000_state, cart_load)
	MCFG_CARTSLOT_NOT_MANDATORY
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( gl2000, pc2000 )
	MCFG_SOFTWARE_LIST_COMPATIBLE_ADD("cart_list", "misterx")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED_CLASS( misterx, pc2000, pc1000_state )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(pc1000_mem)
	MCFG_CPU_IO_MAP(pc1000_io)
	MCFG_CPU_PERIODIC_INT_DRIVER(pc1000_state, irq0_line_hold,  10)

	/* video hardware */
	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_SIZE(120, 9) //1x20 chars
	MCFG_SCREEN_VISIBLE_AREA(0, 120-1, 0, 9-1)

	MCFG_DEVICE_MODIFY("hd44780")
	MCFG_HD44780_LCD_SIZE(1, 20)
	MCFG_HD44780_PIXEL_UPDATE_CB(pc1000_pixel_update)

	MCFG_CARTSLOT_MODIFY("cart")
	MCFG_CARTSLOT_INTERFACE("pc1000_cart")
	MCFG_CARTSLOT_LOAD(pc1000_state, cart_load)

	/* Software lists */
	MCFG_SOFTWARE_LIST_ADD("cart_list", "misterx")
MACHINE_CONFIG_END

/* ROM definition */
ROM_START( pc2000 )
	ROM_REGION( 0x40000, "bios", 0 )
	ROM_LOAD( "lh532hee_9344_d.u4", 0x000000, 0x040000, CRC(0b03bf33) SHA1(cb344b94b14975c685041d3e669f386e8a21909f))

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( gl2000 )
	ROM_REGION( 0x40000, "bios", 0 )
	ROM_LOAD( "lh532hez_9416_d.bin", 0x000000, 0x040000, CRC(532f219e) SHA1(4044f0cf098087af4cc9d1b2a80c3c9ec06f154e))

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( gl2000p )
	ROM_REGION( 0x40000, "bios", 0 )
	ROM_LOAD( "27-5615-00_9534_d.bin", 0x000000, 0x040000, CRC(481c1000) SHA1(da6f60e5bb25145ec5239310296bedaabeeaee28))

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( gl3000s )
	ROM_REGION(0x40000, "bios", 0)
	ROM_LOAD( "27-5713-00", 0x000000, 0x040000, CRC(18b113e0) SHA1(27a12893c38068efa35a99fa97a260dbfbd497e3) )

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( gl4000 )
	ROM_REGION(0x80000, "bios", 0)
	ROM_LOAD( "27-5480-00",   0x000000, 0x040000, CRC(8de047d3) SHA1(bb1d869954773bb7b8b51caa54531015d6b751ec) )

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( gl4004 )
	ROM_REGION(0x80000, "bios", 0)
	ROM_LOAD( "27-5762-00.u2", 0x000000, 0x080000, CRC(fb242f0f) SHA1(aae1beeb94873e29920726ad35475641d9f1e94e) )

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( gl5000 )
	ROM_REGION(0x80000, "bios", 0)
	ROM_LOAD( "27-5912-00.u1", 0x000000, 0x080000, CRC(9fe4c04a) SHA1(823d1d46e49e21f921260296874bc3ee5f718a5f) )

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( gl5005x )
	ROM_REGION(0x200000, "bios", 0)
	ROM_LOAD( "27-6426-00.u1", 0x000000, 0x200000, CRC(adde3581) SHA1(80f2bde7c5c339534614f24a9ca6ea362ee2f816) )

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( gl6000sl )
	ROM_REGION(0x80000, "bios", 0)
	ROM_LOAD( "27-5894-01",   0x000000, 0x080000, CRC(7336231c) SHA1(35a1f739994b5c8fb67a7f76d423e50d8154e9ea) )

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( gl7007sl )
	ROM_REGION(0x100000, "bios", 0)
	ROM_LOAD( "27-6060-00", 0x000000, 0x100000, CRC(06b2a595) SHA1(654d00e55ee43627ff947d72676c8e48e0518123) )

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( gln ) // not Z80 code
	ROM_REGION( 0x80000, "bios", 0 )
	ROM_LOAD( "27-5308-00_9524_d.bin", 0x000000, 0x080000, CRC(d1b994ee) SHA1(b5cf0810df0676712e4f30e279cc46c19b4277dd))

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
	ROM_CART_LOAD( "cart", 0, 0x40000, 0 )
ROM_END

ROM_START( misterx )
	ROM_REGION( 0x20000, "bios", 0 )
	ROM_LOAD( "27-00882-001.bin", 0x000000, 0x020000, CRC(30e0dc94) SHA1(2f4675746a41399b3d9e3e8001a9b4a0dcc5b620))

	ROM_REGION( 0x40000, "cart", ROMREGION_ERASEFF )
ROM_END


/* Driver */

/*    YEAR  NAME     PARENT  COMPAT   MACHINE    INPUT   CLASS          INIT    COMPANY   FULLNAME       FLAGS */
COMP( 1988, misterx,  0,       0,     misterx,   pc1000, driver_device,   0,  "Video Technology / Yeno", "MisterX", GAME_NOT_WORKING)
COMP( 1993, pc2000,   0,       0,     pc2000,    pc2000, driver_device,   0,  "Video Technology", "PreComputer 2000", GAME_NOT_WORKING)
COMP( 1993, gl2000,   0,       0,     gl2000,    pc2000, driver_device,   0,  "Video Technology", "Genius Leader 2000", GAME_NOT_WORKING)
COMP( 1995, gl2000p,  gl2000,  0,     gl2000,    pc2000, driver_device,   0,  "Video Technology", "Genius Leader 2000 Plus", GAME_NOT_WORKING)
COMP( 1996, gl3000s,  0,       0,     pc2000,    pc2000, driver_device,   0,  "Video Technology", "Genius Leader 3000S (Germany)", GAME_IS_SKELETON)
COMP( 1994, gl4000,   0,       0,     pc2000,    pc2000, driver_device,   0,  "Video Technology", "Genius Leader 4000 Quadro (Germany)", GAME_IS_SKELETON)
COMP( 1996, gl4004,   0,       0,     pc2000,    pc2000, driver_device,   0,  "Video Technology", "Genius Leader 4004 Quadro L (Germany)", GAME_IS_SKELETON)
COMP( 1997, gl5000,   0,       0,     pc2000,    pc2000, driver_device,   0,  "Video Technology", "Genius Leader 5000 (Germany)", GAME_IS_SKELETON)
COMP( 1997, gl5005x,  0,       0,     pc2000,    pc2000, driver_device,   0,  "Video Technology", "Genius Leader 5005X (Germany)", GAME_IS_SKELETON)
COMP( 1997, gl6000sl, 0,       0,     pc2000,    pc2000, driver_device,   0,  "Video Technology", "Genius Leader 6000SL (Germany)", GAME_IS_SKELETON)
COMP( 1998, gl7007sl, 0,       0,     pc2000,    pc2000, driver_device,   0,  "Video Technology", "Genius Leader 7007SL (Germany)", GAME_IS_SKELETON)
COMP( 1993, gln,      0,       0,     pc2000,    pc2000, driver_device,   0,  "Video Technology", "Genius Leader Notebook", GAME_NOT_WORKING)
