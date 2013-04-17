/*********************************************************************

    drivers/oric.c

    Systems supported by this driver:

    Oric 1,
    Oric Atmos,
    Oric Telestrat,
    Pravetz 8D

    Pravetz is a Bulgarian copy of the Oric Atmos and uses
    Apple 2 disc drives for storage.

    This driver originally by Paul Cook, rewritten by Kevin Thacker.

*********************************************************************/

#include "includes/oric.h"

/*
    Explanation of memory regions:

    I have split the memory region &c000-&ffff in this way because:

    All roms (os, microdisc and jasmin) use the 6502 IRQ vectors at the end
    of memory &fff8-&ffff, but they are different sizes. The os is 16k, microdisc
    is 8k and jasmin is 2k.

    There is also 16k of ram at &c000-&ffff which is normally masked
    by the os rom, but when the microdisc or jasmin interfaces are used,
    this ram can be accessed. For the microdisc and jasmin, the ram not
    covered by the roms for these interfaces, can be accessed
    if it is enabled.

    SMH_BANK(1),SMH_BANK(2) and SMH_BANK(3) are used for a 16k rom.
    SMH_BANK(2) and SMH_BANK(3) are used for a 8k rom.
    SMH_BANK(3) is used for a 2k rom.

    0x0300-0x03ff is I/O access. It is not defined below because the
    memory is setup dynamically depending on hardware that has been selected (microdisc, jasmin, apple2) etc.

*/


static ADDRESS_MAP_START(oric_mem, AS_PROGRAM, 8, oric_state )
	AM_RANGE( 0x0000, 0xbfff) AM_RAM AM_SHARE("ram")
	AM_RANGE( 0xc000, 0xdfff) AM_READ_BANK("bank1") AM_WRITE_BANK("bank5")
	AM_RANGE( 0xe000, 0xf7ff) AM_READ_BANK("bank2") AM_WRITE_BANK("bank6")
	AM_RANGE( 0xf800, 0xffff) AM_READ_BANK("bank3") AM_WRITE_BANK("bank7")
ADDRESS_MAP_END

/*
The telestrat has the memory regions split into 16k blocks.
Memory region &c000-&ffff can be ram or rom. */
static ADDRESS_MAP_START(telestrat_mem, AS_PROGRAM, 8, oric_state )
	AM_RANGE( 0x0000, 0x02ff) AM_RAM
	AM_RANGE( 0x0300, 0x030f) AM_DEVREADWRITE("via6522_0", via6522_device, read, write)
	AM_RANGE( 0x0310, 0x031b) AM_READWRITE(oric_microdisc_r, oric_microdisc_w )
	AM_RANGE( 0x031c, 0x031f) AM_DEVREADWRITE("acia", mos6551_device, read, write)
	AM_RANGE( 0x0320, 0x032f) AM_DEVREADWRITE("via6522_1", via6522_device, read, write)
	AM_RANGE( 0x0400, 0xbfff) AM_RAM
	AM_RANGE( 0xc000, 0xffff) AM_READ_BANK("bank1") AM_WRITE_BANK("bank2")
ADDRESS_MAP_END


static INPUT_PORTS_START(oric)
	PORT_START("ROW0")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_3)           PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_X)           PORT_CHAR('x') PORT_CHAR('X')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_1)           PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT(0x10, 0x00, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_V)           PORT_CHAR('v') PORT_CHAR('V')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_5)           PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_N)           PORT_CHAR('n') PORT_CHAR('N')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_7)           PORT_CHAR('7') PORT_CHAR('&')

	PORT_START("ROW1")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_D)           PORT_CHAR('d') PORT_CHAR('D')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q)           PORT_CHAR('q') PORT_CHAR('Q')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_TAB)         PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT(0x10, 0x00, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F)           PORT_CHAR('f') PORT_CHAR('F')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_R)           PORT_CHAR('r') PORT_CHAR('R')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_T)           PORT_CHAR('t') PORT_CHAR('T')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_J)           PORT_CHAR('j') PORT_CHAR('J')

	PORT_START("ROW2")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_C)           PORT_CHAR('c') PORT_CHAR('C')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_2)           PORT_CHAR('2') PORT_CHAR('@')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z)           PORT_CHAR('z') PORT_CHAR('Z')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_4)           PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_B)           PORT_CHAR('b') PORT_CHAR('B')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_6)           PORT_CHAR('6') PORT_CHAR('^')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_M)           PORT_CHAR('m') PORT_CHAR('M')

	PORT_START("ROW3")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE)       PORT_CHAR('\'') PORT_CHAR('"')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH2)  PORT_CHAR('\\') PORT_CHAR('|')
	PORT_BIT(0x20, 0x00, IPT_UNUSED)
	PORT_BIT(0x10, 0x00, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS)       PORT_CHAR('-') PORT_CHAR('\xA3')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON)       PORT_CHAR(';') PORT_CHAR(':')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_9)           PORT_CHAR('9') PORT_CHAR('(')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_K)           PORT_CHAR('k') PORT_CHAR('K')

	PORT_START("ROW4")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_RIGHT)       PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_DOWN)        PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LEFT)        PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LSHIFT)      PORT_CHAR(UCHAR_MAMEKEY(LSHIFT))
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_UP)          PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP)        PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA)       PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE)       PORT_CHAR(' ')

	PORT_START("ROW5")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE)   PORT_CHAR('[') PORT_CHAR('{')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE)  PORT_CHAR(']') PORT_CHAR('}')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Del") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT(0x10, 0x00, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_P)           PORT_CHAR('p') PORT_CHAR('P')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_O)           PORT_CHAR('o') PORT_CHAR('O')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_I)           PORT_CHAR('i') PORT_CHAR('I')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_U)           PORT_CHAR('u') PORT_CHAR('U')

	PORT_START("ROW6")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_W)           PORT_CHAR('w') PORT_CHAR('W')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_S)           PORT_CHAR('s') PORT_CHAR('S')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_A)           PORT_CHAR('a') PORT_CHAR('A')
	PORT_BIT(0x10, 0x00, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_E)           PORT_CHAR('e') PORT_CHAR('E')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_G)           PORT_CHAR('g') PORT_CHAR('G')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_H)           PORT_CHAR('h') PORT_CHAR('H')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y)           PORT_CHAR('y') PORT_CHAR('Y')

	PORT_START("ROW7")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS)      PORT_CHAR('=') PORT_CHAR('+')
	PORT_BIT(0x40, 0x00, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Return") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_RSHIFT)      PORT_CHAR(UCHAR_MAMEKEY(RSHIFT))
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH)       PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_0)           PORT_CHAR('0') PORT_CHAR(')')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_L)           PORT_CHAR('l') PORT_CHAR('L')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_8)           PORT_CHAR('8') PORT_CHAR('*')

	PORT_START("FLOPPY")
	/* floppy interface  */
	PORT_CONFNAME( 0x03, 0x00, "Floppy disc interface" )
	PORT_CONFSETTING(    0x00, DEF_STR( None ) )
	PORT_CONFSETTING(    0x01, "Microdisc" )
	PORT_CONFSETTING(    0x02, "Jasmin" )
/*  PORT_CONFSETTING(    0x03, "Low 8D DOS" ) */
/*  PORT_CONFSETTING(    0x04, "High 8D DOS" ) */

	/* vsync cable hardware. This is a simple cable connected to the video output
	to the monitor/television. The sync signal is connected to the cassette input
	allowing interrupts to be generated from the vsync signal. */
	PORT_CONFNAME(0x08, 0x00, "Vsync cable hardware")
	PORT_CONFSETTING(   0x00, DEF_STR( Off ) )
	PORT_CONFSETTING(   0x08, DEF_STR( On ) )
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_CUSTOM) PORT_VBLANK("screen")
INPUT_PORTS_END

static INPUT_PORTS_START(orica)
	PORT_INCLUDE( oric )

	PORT_MODIFY("ROW5")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Funct") PORT_CODE(KEYCODE_END) PORT_CHAR(UCHAR_MAMEKEY(F1))
INPUT_PORTS_END

static INPUT_PORTS_START(prav8d)
	PORT_START("ROW0")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_3)                                   PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("X \xd0\xac") PORT_CODE(KEYCODE_X)           PORT_CHAR('X')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_1)                                   PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("V \xd0\x96") PORT_CODE(KEYCODE_V)           PORT_CHAR('V')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_5)                                   PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("N \xd0\x9d") PORT_CODE(KEYCODE_N)           PORT_CHAR('N')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_7)                                   PORT_CHAR('7') PORT_CHAR('\'')

	PORT_START("ROW1")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("D \xd0\x94") PORT_CODE(KEYCODE_D)           PORT_CHAR('D')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Q \xd0\xaf") PORT_CODE(KEYCODE_Q)           PORT_CHAR('Q')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Esc") PORT_CODE(KEYCODE_ESC)                PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F \xd0\xa4") PORT_CODE(KEYCODE_F)           PORT_CHAR('F')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("R \xd0\xa0") PORT_CODE(KEYCODE_R)           PORT_CHAR('R')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("T \xd0\xa2") PORT_CODE(KEYCODE_T)           PORT_CHAR('T')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("J \xd0\x99") PORT_CODE(KEYCODE_J)           PORT_CHAR('J')

	PORT_START("ROW2")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C \xd0\xa6") PORT_CODE(KEYCODE_C)           PORT_CHAR('C')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_2)                                   PORT_CHAR('2') PORT_CHAR('"')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Z \xd0\x97") PORT_CODE(KEYCODE_Z)           PORT_CHAR('Z')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MK") PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_SHIFT_2)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_4)                                   PORT_CHAR('4') PORT_CHAR('$')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("B \xd0\x91") PORT_CODE(KEYCODE_B)           PORT_CHAR('B')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_6)                                   PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M \xd0\x9c") PORT_CODE(KEYCODE_M)           PORT_CHAR('M')

	PORT_START("ROW3")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("] \xd0\xa9") PORT_CODE(KEYCODE_QUOTE)       PORT_CHAR(']')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH2)                          PORT_CHAR(';') PORT_CHAR('+')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C/L") PORT_CODE(KEYCODE_CAPSLOCK) PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK)) // this one is 5th line, 1st key from right
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS)                               PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("[ \xd0\xa8") PORT_CODE(KEYCODE_COLON)       PORT_CHAR('[')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_9)                                   PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("K \xd0\x9a") PORT_CODE(KEYCODE_K)           PORT_CHAR('K')

	PORT_START("ROW4")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(UTF8_RIGHT) PORT_CODE(KEYCODE_RIGHT)     PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(UTF8_DOWN) PORT_CODE(KEYCODE_DOWN)       PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(UTF8_LEFT) PORT_CODE(KEYCODE_LEFT)       PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LSHIFT)                              PORT_CHAR(UCHAR_MAMEKEY(LSHIFT))
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(UTF8_UP) PORT_CODE(KEYCODE_UP)       PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP)                                PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA)                               PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE)                               PORT_CHAR(' ')

	PORT_START("ROW5")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("@ \xd0\xae") PORT_CODE(KEYCODE_OPENBRACE)   PORT_CHAR('@')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("\\ \xd0\xad") PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR('\\')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Del") PORT_CODE(KEYCODE_BACKSPACE)          PORT_CHAR(8) // this one is 5th line, 1st key from left
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("P \xd0\x9f") PORT_CODE(KEYCODE_P)           PORT_CHAR('P')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("O \xd0\x9e") PORT_CODE(KEYCODE_O)           PORT_CHAR('O')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("I \xd0\x98") PORT_CODE(KEYCODE_I)           PORT_CHAR('I')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("U \xd0\xa3") PORT_CODE(KEYCODE_U)           PORT_CHAR('U')

	PORT_START("ROW6")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("W \xd0\x92") PORT_CODE(KEYCODE_W)           PORT_CHAR('W')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("S \xd0\xa1") PORT_CODE(KEYCODE_S)           PORT_CHAR('S')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("A \xd0\x90") PORT_CODE(KEYCODE_A)           PORT_CHAR('A')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("E \xd0\x95") PORT_CODE(KEYCODE_E)           PORT_CHAR('E')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("G \xd0\x93") PORT_CODE(KEYCODE_G)           PORT_CHAR('G')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("H \xd0\xa5") PORT_CODE(KEYCODE_H)           PORT_CHAR('H')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Y \xd0\xaa") PORT_CODE(KEYCODE_Y)           PORT_CHAR('Y')

	PORT_START("ROW7")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS)                              PORT_CHAR('-') PORT_CHAR('=')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("^ \xd0\xa7") PORT_CODE(KEYCODE_BACKSLASH)   PORT_CHAR('^') // this one would be on 2nd line, 3rd key from 'P'
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Return") PORT_CODE(KEYCODE_ENTER)           PORT_CHAR(13)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_RSHIFT)                              PORT_CHAR(UCHAR_MAMEKEY(RSHIFT))
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH)                               PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_0)                                   PORT_CHAR('0')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("L \xd0\x9b") PORT_CODE(KEYCODE_L)           PORT_CHAR('L')
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_8)                                   PORT_CHAR('8') PORT_CHAR('(')

	PORT_START("FLOPPY")
	/* force apple2 disc interface for pravetz */
	PORT_START("oric_floppy_interface")
	PORT_CONFNAME( 0x07, 0x00, "Floppy disc interface" )
	PORT_CONFSETTING(    0x00, DEF_STR( None ) )
	PORT_CONFSETTING(    0x03, "Low 8D DOS" )
	PORT_CONFSETTING(    0x04, "High 8D DOS" )

	/* vsync cable hardware. This is a simple cable connected to the video output
	to the monitor/television. The sync signal is connected to the cassette input
	allowing interrupts to be generated from the vsync signal. */
	PORT_CONFNAME(0x08, 0x00, "Vsync cable hardware")
	PORT_CONFSETTING(   0x00, DEF_STR( Off ) )
	PORT_CONFSETTING(   0x08, DEF_STR( On ) )
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_CUSTOM) PORT_VBLANK("screen")
INPUT_PORTS_END

static INPUT_PORTS_START(telstrat)
	PORT_INCLUDE( oric )

	PORT_MODIFY("ROW5")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Funct") PORT_CODE(KEYCODE_END) PORT_CHAR(UCHAR_MAMEKEY(F1))

	PORT_MODIFY("FLOPPY")
	/* vsync cable hardware. This is a simple cable connected to the video output
	to the monitor/television. The sync signal is connected to the cassette input
	allowing interrupts to be generated from the vsync signal. */
	PORT_BIT(0x07, 0x00, IPT_UNUSED)
	PORT_CONFNAME(0x08, 0x00, "Vsync cable hardware")
	PORT_CONFSETTING(   0x00, DEF_STR( Off ) )
	PORT_CONFSETTING(   0x08, DEF_STR( On ) )
	PORT_BIT( 0x010, IP_ACTIVE_HIGH, IPT_CUSTOM) PORT_VBLANK("screen")

	PORT_START("JOY0")      /* left joystick port */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joystick 0 Up") PORT_CODE(JOYCODE_X_RIGHT_SWITCH)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joystick 0 Down") PORT_CODE(JOYCODE_X_LEFT_SWITCH)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joystick 0 Left") PORT_CODE(JOYCODE_BUTTON1)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joystick 0 Right") PORT_CODE(JOYCODE_Y_DOWN_SWITCH)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joystick 0 Fire 1") PORT_CODE(JOYCODE_Y_UP_SWITCH)

	PORT_START("JOY1")      /* right joystick port */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joystick 1 Up") PORT_CODE(JOYCODE_X_RIGHT_SWITCH)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joystick 1 Down") PORT_CODE(JOYCODE_X_LEFT_SWITCH)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joystick 1 Left") PORT_CODE(JOYCODE_BUTTON1)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joystick 1 Right") PORT_CODE(JOYCODE_Y_DOWN_SWITCH)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Joystick 1 Fire 1") PORT_CODE(JOYCODE_Y_UP_SWITCH)
INPUT_PORTS_END


static const unsigned char oric_palette[8*3] =
{
	0x00, 0x00, 0x00, 0xff, 0x00, 0x00,
	0x00, 0xff, 0x00, 0xff, 0xff, 0x00,
	0x00, 0x00, 0xff, 0xff, 0x00, 0xff,
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
};

/* Initialise the palette */
void oric_state::palette_init()
{
	int i;

	for ( i = 0; i < sizeof(oric_palette) / 3; i++ ) {
		palette_set_color_rgb(machine(), i, oric_palette[i*3], oric_palette[i*3+1], oric_palette[i*3+2]);
	}
}



static const ay8910_interface oric_ay_interface =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DRIVER_MEMBER(oric_state, oric_psg_porta_write),
	DEVCB_NULL,
};


static const cassette_interface oric_cassette_interface =
{
	oric_cassette_formats,
	NULL,
	(cassette_state)(CASSETTE_PLAY | CASSETTE_MOTOR_DISABLED),
	NULL,
	NULL
};

static const centronics_interface oric_centronics_config =
{
	DEVCB_DEVICE_LINE_MEMBER("via6522_0", via6522_device, write_ca1),
	DEVCB_NULL,
	DEVCB_NULL
};

static const floppy_interface oric1_floppy_interface =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	FLOPPY_STANDARD_5_25_DSHD,
	LEGACY_FLOPPY_OPTIONS_NAME(oric),
	NULL,
	NULL
};

static const floppy_interface prav8d_floppy_interface =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	FLOPPY_STANDARD_5_25_DSHD,
	LEGACY_FLOPPY_OPTIONS_NAME(apple2),
	NULL,
	NULL
};

static MACHINE_CONFIG_START( oric, oric_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6502, 1000000)
	MCFG_CPU_PROGRAM_MAP(oric_mem)
	MCFG_QUANTUM_TIME(attotime::from_hz(60))


	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(40*6, 28*8)
	MCFG_SCREEN_VISIBLE_AREA(0, 40*6-1, 0, 28*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(oric_state, screen_update_oric)

	MCFG_PALETTE_LENGTH(8)


	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
	MCFG_SOUND_ADD("ay8912", AY8912, 1000000)
	MCFG_SOUND_CONFIG(oric_ay_interface)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	/* printer */
	MCFG_CENTRONICS_PRINTER_ADD("centronics", oric_centronics_config)

	/* cassette */
	MCFG_CASSETTE_ADD( "cassette", oric_cassette_interface )

	/* via */
	MCFG_VIA6522_ADD( "via6522_0", 1000000, oric_6522_interface )

	MCFG_WD1770_ADD("wd179x", oric_wd17xx_interface )

	MCFG_LEGACY_FLOPPY_4_DRIVES_ADD(oric1_floppy_interface)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( prav8d, oric )
	MCFG_LEGACY_FLOPPY_4_DRIVES_REMOVE()
	MCFG_LEGACY_FLOPPY_DRIVE_ADD(FLOPPY_0, prav8d_floppy_interface)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( telstrat, oric )
	MCFG_CPU_MODIFY( "maincpu" )
	MCFG_CPU_PROGRAM_MAP( telestrat_mem)

	MCFG_MACHINE_START_OVERRIDE(oric_state, telestrat )

	/* acia */
	MCFG_MOS6551_ADD("acia", XTAL_1_8432MHz, NULL)

	/* via */
	MCFG_VIA6522_ADD( "via6522_1", 1000000, telestrat_via2_interface )
MACHINE_CONFIG_END


ROM_START(oric1)
	ROM_REGION(0x16800, "maincpu", 0)   /* 0x10000 + 0x04000 + 0x02000 + 0x00800 */
	ROM_LOAD ("basic10.rom", 0x10000, 0x04000, CRC(f18710b4) SHA1(333116e6884d85aaa4dfc7578a91cceeea66d016))
	ROM_LOAD_OPTIONAL ("microdis.rom", 0x14000, 0x02000, CRC(a9664a9c) SHA1(0d2ef6e67322f48f4b7e08d8bbe68827e2074561) )
	ROM_LOAD_OPTIONAL ("jasmin.rom",   0x16000, 0x00800, CRC(37220e89) SHA1(70e59b8abd67092f050462abc6cb5271e4c15f01) )
ROM_END

ROM_START(orica)
	ROM_REGION(0x16800, "maincpu", 0)   /* 0x10000 + 0x04000 + 0x02000 + 0x00800 */
	ROM_SYSTEM_BIOS( 0, "ver11", "Basic 1.1")
	ROMX_LOAD ("basic11b.rom", 0x10000, 0x04000, CRC(c3a92bef) SHA1(9451a1a09d8f75944dbd6f91193fc360f1de80ac), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "ver12", "Basic 1.2 (Pascal Leclerc)")      // 1987/1999 - various enhancements and bugfixes
	ROMX_LOAD ("basic12.rom",  0x10000, 0x04000, CRC(dc4f22dc) SHA1(845e1a893de3dc0f856fdf2f69c3b73770b4094f), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 2, "ver121", "Basic 1.21 (Pascal Leclerc)")        // 07.1999 - DRAW enhancement
	ROMX_LOAD ("basic121.rom", 0x10000, 0x04000, CRC(0a2860b1) SHA1(b727d5c3bbc8cb1d510f224eb1e0d90d609e8506), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS( 3, "ver122", "Basic 1.22 (Pascal Leclerc)")        // 08.2001 - added EUR symbol
	ROMX_LOAD ("basic122.rom", 0x10000, 0x04000, CRC(5ef2a861) SHA1(9ab6dc47b6e9dc65a4137ce0f0f12fc2b6ca8442), ROM_BIOS(4) )
	ROM_SYSTEM_BIOS( 4, "ver11de", "Basic 1.1 DE")
	ROMX_LOAD( "bas11_de.rom", 0x10000, 0x04000, CRC(65233b2d) SHA1(b01cabb1a21980a6785a2fe37a8f8572c892123f), ROM_BIOS(5))
	ROM_SYSTEM_BIOS( 5, "ver11es", "Basic 1.1 ES")
	ROMX_LOAD( "bas11_es.rom", 0x10000, 0x04000, CRC(47bf26c7) SHA1(4fdbadd68db9ab8ad1cd56b4e5cbe51a9c3f11ae), ROM_BIOS(6))
	ROM_SYSTEM_BIOS( 6, "ver11fr", "Basic 1.1 FR")
	ROMX_LOAD( "bas11_fr.rom", 0x10000, 0x04000, CRC(603b1fbf) SHA1(2a4583df3b59ca454d67d5631f242c96ec4cf99a), ROM_BIOS(7))
	ROM_SYSTEM_BIOS( 7, "ver11se", "Basic 1.1 SE")
	ROMX_LOAD( "bas11_se.rom", 0x10000, 0x04000, CRC(a71523ac) SHA1(ce53acf84baec6ab5cbac9f9cefa71b3efeb2ead), ROM_BIOS(8))
	ROM_SYSTEM_BIOS( 8, "ver11uk", "Basic 1.1 UK")
	ROMX_LOAD( "bas11_uk.rom", 0x10000, 0x04000, CRC(303370d1) SHA1(589ff66fac8e06d65af3369491faa67a71f1322a), ROM_BIOS(9))
	ROM_SYSTEM_BIOS( 9, "ver12es", "Basic 1.2 ES")
	ROMX_LOAD( "bas12es_le.rom", 0x10000, 0x04000, CRC(70de4aeb) SHA1(b327418aa7d8a5a03c135e3d8acdd511df625893), ROM_BIOS(10))
	ROM_SYSTEM_BIOS( 10, "ver12fr", "Basic 1.2 FR")
	ROMX_LOAD( "bas12fr_le.rom", 0x10000, 0x04000, CRC(47a437fc) SHA1(70271bc3ed5c3bf4d339d6f5de3de8c3c50ff573), ROM_BIOS(11))
	ROM_SYSTEM_BIOS( 11, "ver12ge", "Basic 1.2 GE")
	ROMX_LOAD( "bas12ge_le.rom", 0x10000, 0x04000, CRC(f5f0dd52) SHA1(75359302452ee7b19537698f124aaefd333688d0), ROM_BIOS(12))
	ROM_SYSTEM_BIOS( 12, "ver12sw", "Basic 1.2 SW")
	ROMX_LOAD( "bas12sw_le.rom", 0x10000, 0x04000, CRC(100abe68) SHA1(6211d5969c4d7a6acb86ed19c5e51a33a3bef431), ROM_BIOS(13))
	ROM_SYSTEM_BIOS( 13, "ver12uk", "Basic 1.2 UK")
	ROMX_LOAD( "bas12uk_le.rom", 0x10000, 0x04000, CRC(00fce8a6) SHA1(d40558bdf61b8aba6260293c9424fd463be7fad8), ROM_BIOS(14))
	ROM_SYSTEM_BIOS( 14, "ver121es", "Basic 1.211 ES")
	ROMX_LOAD( "bas121es_le.rom", 0x10000, 0x04000, CRC(87ec679b) SHA1(5de6a5f5121f69069c9b93d678046e814b5b64e9), ROM_BIOS(15))
	ROM_SYSTEM_BIOS( 15, "ver121fr", "Basic 1.211 FR")
	ROMX_LOAD( "bas121fr_le.rom", 0x10000, 0x04000, CRC(e683dec2) SHA1(20df7ebc0f13aa835f286d50137f1a7ff7430c29), ROM_BIOS(16))
	ROM_SYSTEM_BIOS( 16, "ver121ge", "Basic 1.211 GE")
	ROMX_LOAD( "bas121ge_le.rom", 0x10000, 0x04000, CRC(94fe32bf) SHA1(1024776d20030d602e432e50014502524658643a), ROM_BIOS(17))
	ROM_SYSTEM_BIOS( 17, "ver121sw", "Basic 1.211 SW")
	ROMX_LOAD( "bas121sw_le.rom", 0x10000, 0x04000, CRC(e6ad11c7) SHA1(309c94a9861fcb770636dcde1801a5c68ca819b4), ROM_BIOS(18))
	ROM_SYSTEM_BIOS( 18, "ver121uk", "Basic 1.211 UK")
	ROMX_LOAD( "bas121uk_le.rom", 0x10000, 0x04000, CRC(75aa1aa9) SHA1(ca99e244d9cbef625344c2054023504a4f9dcfe4), ROM_BIOS(19))
	ROM_SYSTEM_BIOS( 19, "ver122es", "Basic 1.22 ES")
	ROMX_LOAD( "bas122es_le.rom", 0x10000, 0x04000, CRC(9144f9e0) SHA1(acf2094078af057e74a31d90d7010be51b9033fa), ROM_BIOS(20))
	ROM_SYSTEM_BIOS( 20, "ver122fr", "Basic 1.22 FR")
	ROMX_LOAD( "bas122fr_le.rom", 0x10000, 0x04000, CRC(370cfda4) SHA1(fad9a0661256e59bcc2915578647573e4128e1bb), ROM_BIOS(21))
	ROM_SYSTEM_BIOS( 21, "ver122ge", "Basic 1.22 GE")
	ROMX_LOAD( "bas122ge_le.rom", 0x10000, 0x04000, CRC(9a42bd62) SHA1(8a9c80f314daf4e5e64fa202e583b8a65796db8b), ROM_BIOS(22))
	ROM_SYSTEM_BIOS( 22, "ver122sw", "Basic 1.22 SW")
	ROMX_LOAD( "bas122sw_le.rom", 0x10000, 0x04000, CRC(e7fd57a4) SHA1(c75cbf7cfafaa02712dc7ca2f972220aef86fb8d), ROM_BIOS(23))
	ROM_SYSTEM_BIOS( 23, "ver122uk", "Basic 1.22 UK")
	ROMX_LOAD( "bas122uk_le.rom", 0x10000, 0x04000, CRC(9865bcd7) SHA1(2a92e2d119463e682bf10647e3880e26656d65b5), ROM_BIOS(24))

	ROM_LOAD_OPTIONAL ("microdis.rom", 0x14000, 0x02000, CRC(a9664a9c) SHA1(0d2ef6e67322f48f4b7e08d8bbe68827e2074561) )
	ROM_LOAD_OPTIONAL ("jasmin.rom",   0x16000, 0x00800, CRC(37220e89) SHA1(70e59b8abd67092f050462abc6cb5271e4c15f01) )
ROM_END

ROM_START(telstrat)
	ROM_REGION(0x30000, "maincpu", 0)   /* 0x10000 + (0x04000 * 4) */
	ROM_LOAD ("telmatic.rom", 0x010000, 0x02000, CRC(94358dc6) SHA1(35f92a0477a88f5cf564971125047ffcfa02ec10) )
	ROM_LOAD ("teleass.rom",  0x014000, 0x04000, CRC(68b0fde6) SHA1(9e9af51dae3199cccf49ab3f0d47e2b9be4ba97d) )
	ROM_LOAD ("hyperbas.rom", 0x018000, 0x04000, CRC(1d96ab50) SHA1(f5f70a0eb59f8cd6c261e179ae78ef906f68ed63) )
	ROM_LOAD ("telmon24.rom", 0x01c000, 0x04000, CRC(aa727c5d) SHA1(86fc8dc0932f983efa199e31ae05a4424772f959) )
ROM_END

ROM_START(prav8d)
	ROM_REGION(0x14300, "maincpu", 0)   /* 0x10000 + 0x04000 + 0x00100 + 0x00200 */
	ROM_LOAD( "pravetzt.rom", 0x10000, 0x4000, CRC(58079502) SHA1(7afc276cb118adff72e4f16698f94bf3b2c64146) )
	ROM_LOAD_OPTIONAL( "8ddoslo.rom", 0x014000, 0x0100, CRC(0c82f636) SHA1(b29d151a0dfa3c7cd50439b51d0a8f95559bc2b6) )
	ROM_LOAD_OPTIONAL( "8ddoshi.rom", 0x014100, 0x0200, CRC(66309641) SHA1(9c2e82b3c4d385ade6215fcb89f8b92e6fd2bf4b) )
ROM_END

ROM_START(prav8dd)
	ROM_REGION(0x14300, "maincpu", 0)   /* 0x10000 + 0x04000 + 0x00100 + 0x00200 */
	ROM_SYSTEM_BIOS( 0, "default", "Disk ROM, 1989")
	ROMX_LOAD( "8d.rom",       0x10000, 0x4000, CRC(b48973ef) SHA1(fd47c977fc215a3b577596a7483df53e8a1e9c83), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "radosoft", "RadoSoft Disk ROM, 1992")
	ROMX_LOAD( "pravetzd.rom", 0x10000, 0x4000, CRC(f8d23821) SHA1(f87ad3c5832773b6e0614905552a80c98dc8e2a5), ROM_BIOS(2) )
	ROM_LOAD_OPTIONAL( "8ddoslo.rom", 0x014000, 0x0100, CRC(0c82f636) SHA1(b29d151a0dfa3c7cd50439b51d0a8f95559bc2b6) )
	ROM_LOAD_OPTIONAL( "8ddoshi.rom", 0x014100, 0x0200, CRC(66309641) SHA1(9c2e82b3c4d385ade6215fcb89f8b92e6fd2bf4b) )
ROM_END


/*    YEAR   NAME       PARENT  COMPAT  MACHINE     INPUT       INIT    COMPANY         FULLNAME */
COMP( 1983, oric1,      0,      0,      oric,       oric, driver_device,       0,    "Tangerine",    "Oric 1" , 0)
COMP( 1984, orica,      oric1,  0,      oric,       orica, driver_device,      0,    "Tangerine",    "Oric Atmos" , 0)
COMP( 1985, prav8d,     oric1,  0,      prav8d,     prav8d, driver_device,     0,    "Pravetz",      "Pravetz 8D", 0)
COMP( 1989, prav8dd,    oric1,  0,      prav8d,     prav8d, driver_device,     0,    "Pravetz",      "Pravetz 8D (Disk ROM)", GAME_UNOFFICIAL)
COMP( 1986, telstrat,   oric1,  0,      telstrat,   telstrat, driver_device,   0,    "Tangerine",    "Oric Telestrat", GAME_NOT_WORKING )
