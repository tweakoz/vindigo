/***************************************************************************

        Radio-86RK driver by Miodrag Milanovic

        15/03/2008 Preliminary driver.

****************************************************************************/


#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "sound/wave.h"
#include "machine/i8255.h"
#include "machine/8257dma.h"
#include "video/i8275.h"
#include "imagedev/cassette.h"
#include "imagedev/cartslot.h"
#include "formats/rk_cas.h"
#include "includes/radio86.h"

/* Address maps */
static ADDRESS_MAP_START(radio86_mem, AS_PROGRAM, 8, radio86_state )
	AM_RANGE( 0x0000, 0x0fff ) AM_RAMBANK("bank1") // First bank
	AM_RANGE( 0x1000, 0x7fff ) AM_RAM  // RAM
	AM_RANGE( 0x8000, 0x8003 ) AM_DEVREADWRITE("ppi8255_1", i8255_device, read, write) AM_MIRROR(0x1ffc)
	//AM_RANGE( 0xa000, 0xa003 ) AM_DEVREADWRITE("ppi8255_2", i8255_device, read, write) AM_MIRROR(0x1ffc)
	AM_RANGE( 0xc000, 0xc001 ) AM_DEVREADWRITE("i8275", i8275_device, read, write) AM_MIRROR(0x1ffe) // video
	AM_RANGE( 0xe000, 0xffff ) AM_DEVWRITE("dma8257", i8257_device, i8257_w)    // DMA
	AM_RANGE( 0xf000, 0xffff ) AM_ROM  // System ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( radio86_io , AS_IO, 8, radio86_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x00, 0xff ) AM_READWRITE(radio_io_r,radio_io_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( rk7007_io , AS_IO, 8, radio86_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x80, 0x83 ) AM_DEVREADWRITE("ms7007", i8255_device, read, write)
ADDRESS_MAP_END

static ADDRESS_MAP_START(radio86rom_mem, AS_PROGRAM, 8, radio86_state )
	AM_RANGE( 0x0000, 0x0fff ) AM_RAMBANK("bank1") // First bank
	AM_RANGE( 0x1000, 0x7fff ) AM_RAM  // RAM
	AM_RANGE( 0x8000, 0x8003 ) AM_DEVREADWRITE("ppi8255_1", i8255_device, read, write) AM_MIRROR(0x1ffc)
	AM_RANGE( 0xa000, 0xa003 ) AM_DEVREADWRITE("ppi8255_2", i8255_device, read, write) AM_MIRROR(0x1ffc)
	AM_RANGE( 0xc000, 0xc001 ) AM_DEVREADWRITE("i8275", i8275_device, read, write) AM_MIRROR(0x1ffe) // video
	AM_RANGE( 0xe000, 0xffff ) AM_DEVWRITE("dma8257", i8257_device, i8257_w)    // DMA
	AM_RANGE( 0xf000, 0xffff ) AM_ROM  // System ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START(radio86ram_mem, AS_PROGRAM, 8, radio86_state )
	AM_RANGE( 0x0000, 0x0fff ) AM_RAMBANK("bank1") // First bank
	AM_RANGE( 0x1000, 0xdfff ) AM_RAM  // RAM
	AM_RANGE( 0xe000, 0xe7ff ) AM_ROM  // System ROM page 2
	AM_RANGE( 0xe800, 0xf5ff ) AM_RAM  // RAM
	AM_RANGE( 0xf700, 0xf703 ) AM_DEVREADWRITE("ppi8255_1", i8255_device, read, write)
	AM_RANGE( 0xf780, 0xf7bf ) AM_DEVREADWRITE("i8275", i8275_device, read, write) // video
	AM_RANGE( 0xf684, 0xf687 ) AM_DEVREADWRITE("ppi8255_2", i8255_device, read, write)
	AM_RANGE( 0xf688, 0xf688 ) AM_WRITE(radio86_pagesel )
	AM_RANGE( 0xf800, 0xffff ) AM_DEVWRITE("dma8257", i8257_device, i8257_w)    // DMA
	AM_RANGE( 0xf800, 0xffff ) AM_ROM  // System ROM page 1
ADDRESS_MAP_END

static ADDRESS_MAP_START(radio86_16_mem, AS_PROGRAM, 8, radio86_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x0000, 0x0fff ) AM_RAMBANK("bank1") // First bank
	AM_RANGE( 0x1000, 0x3fff ) AM_RAM  // RAM
	AM_RANGE( 0x4000, 0x7fff ) AM_READ(radio_cpu_state_r)
	AM_RANGE( 0x8000, 0x8003 ) AM_DEVREADWRITE("ppi8255_1", i8255_device, read, write) AM_MIRROR(0x1ffc)
	//AM_RANGE( 0xa000, 0xa003 ) AM_DEVREADWRITE_LEGACY("ppi8255_2", i8255a_r, i8255a_w) AM_MIRROR(0x1ffc)
	AM_RANGE( 0xc000, 0xc001 ) AM_DEVREADWRITE("i8275", i8275_device, read, write) AM_MIRROR(0x1ffe) // video
	AM_RANGE( 0xe000, 0xffff ) AM_DEVWRITE("dma8257", i8257_device, i8257_w)    // DMA
	AM_RANGE( 0xf000, 0xffff ) AM_ROM  // System ROM
ADDRESS_MAP_END


static ADDRESS_MAP_START(mikron2_mem, AS_PROGRAM, 8, radio86_state )
	AM_RANGE( 0x0000, 0x0fff ) AM_RAMBANK("bank1") // First bank
	AM_RANGE( 0x1000, 0x7fff ) AM_RAM  // RAM
	AM_RANGE( 0xc000, 0xc003 ) AM_DEVREADWRITE("ppi8255_1", i8255_device, read, write) AM_MIRROR(0x00fc)
	//AM_RANGE( 0xc100, 0xc103 ) AM_DEVREADWRITE_LEGACY("ppi8255_2", i8255a_r, i8255a_w) AM_MIRROR(0x00fc)
	AM_RANGE( 0xc200, 0xc201 ) AM_DEVREADWRITE("i8275", i8275_device, read, write) AM_MIRROR(0x00fe) // video
	AM_RANGE( 0xc300, 0xc3ff ) AM_DEVWRITE("dma8257", i8257_device, i8257_w)    // DMA
	AM_RANGE( 0xf000, 0xffff ) AM_ROM  // System ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START(impuls03_mem, AS_PROGRAM, 8, radio86_state )
	AM_RANGE( 0x0000, 0x0fff ) AM_RAMBANK("bank1") // First bank
	AM_RANGE( 0x1000, 0x7fff ) AM_RAM  // RAM
	AM_RANGE( 0x8000, 0x8003 ) AM_DEVREADWRITE("ppi8255_1", i8255_device, read, write) AM_MIRROR(0x1ffc)
	AM_RANGE( 0xa000, 0xbfff ) AM_ROM  // Basic ROM
	AM_RANGE( 0xc000, 0xc001 ) AM_DEVREADWRITE("i8275", i8275_device, read, write) AM_MIRROR(0x1ffe) // video
	AM_RANGE( 0xe000, 0xffff ) AM_DEVWRITE("dma8257", i8257_device, i8257_w)    // DMA
	AM_RANGE( 0xf000, 0xffff ) AM_ROM  // System ROM
ADDRESS_MAP_END

/* Input ports */
INPUT_PORTS_START( radio86 )
	PORT_START("LINE0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_HOME) PORT_CHAR(UCHAR_MAMEKEY(HOME))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ins") PORT_CODE(KEYCODE_INSERT) PORT_CHAR(UCHAR_MAMEKEY(INSERT))
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ESC) PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F3) PORT_CHAR(UCHAR_MAMEKEY(F3))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F4) PORT_CHAR(UCHAR_MAMEKEY(F4))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F5) PORT_CHAR(UCHAR_MAMEKEY(F5))

	PORT_START("LINE1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_TAB) PORT_CHAR('\t')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_END) PORT_CHAR(UCHAR_MAMEKEY(END))
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Back") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Left") PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Up") PORT_CODE(KEYCODE_UP) PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Right") PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Down") PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(DOWN))

	PORT_START("LINE2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2) PORT_CHAR('2') PORT_CHAR('"')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3) PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4) PORT_CHAR('4') PORT_CHAR('\xA4')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5) PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6) PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7) PORT_CHAR('7') PORT_CHAR('\'')

	PORT_START("LINE3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8) PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9) PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS) PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON) PORT_CHAR(';') PORT_CHAR('+')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA) PORT_CHAR(',') PORT_CHAR('<')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS) PORT_CHAR('-') PORT_CHAR('=')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP) PORT_CHAR('.') PORT_CHAR('>')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH) PORT_CHAR('/') PORT_CHAR('?')

	PORT_START("LINE4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_TILDE) PORT_CHAR('@')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_E) PORT_CHAR('E')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_G) PORT_CHAR('G')

	PORT_START("LINE5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_I) PORT_CHAR('I')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_J) PORT_CHAR('J')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_M) PORT_CHAR('M')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_N) PORT_CHAR('N')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_O) PORT_CHAR('O')

	PORT_START("LINE6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_P) PORT_CHAR('P')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_T) PORT_CHAR('T')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_W) PORT_CHAR('W')

	PORT_START("LINE7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('[')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(']')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR('~')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')

	PORT_START("LINE8")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ctrl") PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_RCONTROL)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Rus/Lat") PORT_CODE(KEYCODE_LALT) PORT_CODE(KEYCODE_RALT)
INPUT_PORTS_END

INPUT_PORTS_START( ms7007 )
	PORT_START("LINE0")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0) PORT_CHAR(' ') PORT_CHAR('0')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9) PORT_CHAR(')') PORT_CHAR('9')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6_PAD) PORT_CHAR(UCHAR_MAMEKEY(6_PAD))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ins") PORT_CODE(KEYCODE_INSERT) PORT_CHAR(UCHAR_MAMEKEY(INSERT))
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_END) PORT_CHAR(UCHAR_MAMEKEY(END))
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH) PORT_CHAR('/') PORT_CHAR('?')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS) PORT_CHAR('=') PORT_CHAR('-')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_EQUALS) PORT_CHAR(':') PORT_CHAR('*')

	PORT_START("LINE1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7) PORT_CHAR('\'') PORT_CHAR('7')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8) PORT_CHAR('(') PORT_CHAR('8')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F1))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F2))
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F3) PORT_CHAR(UCHAR_MAMEKEY(F3))
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F4) PORT_CHAR(UCHAR_MAMEKEY(F4))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F5) PORT_CHAR(UCHAR_MAMEKEY(F5))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4) PORT_CHAR('\xA4') PORT_CHAR('4')

	PORT_START("LINE2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('[')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(']')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1) PORT_CHAR('!') PORT_CHAR('1')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2) PORT_CHAR('"') PORT_CHAR('2')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3) PORT_CHAR('#') PORT_CHAR('3')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5) PORT_CHAR('%') PORT_CHAR('5')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_6) PORT_CHAR('&') PORT_CHAR('6')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_E) PORT_CHAR('E')

	PORT_START("LINE3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_N) PORT_CHAR('N')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_G) PORT_CHAR('G')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_P) PORT_CHAR('P')

	PORT_START("LINE4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_TILDE) PORT_CHAR('@')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_W) PORT_CHAR('W')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_O) PORT_CHAR('O')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_I) PORT_CHAR('I')

	PORT_START("LINE5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA) PORT_CHAR('<') PORT_CHAR(',')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR('~')
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_M) PORT_CHAR('M')
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_T) PORT_CHAR('T')
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')

	PORT_START("LINE6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_4_PAD) PORT_CHAR(UCHAR_MAMEKEY(4_PAD))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_PLUS_PAD) PORT_CHAR(UCHAR_MAMEKEY(PLUS_PAD))
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Back") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP) PORT_CHAR('>') PORT_CHAR('.')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))

	PORT_START("LINE7")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_5_PAD) PORT_CHAR(UCHAR_MAMEKEY(5_PAD))
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F6) PORT_CHAR(UCHAR_MAMEKEY(F6))
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_HOME) PORT_CHAR(UCHAR_MAMEKEY(HOME))
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_UP) PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH_PAD) PORT_CHAR(UCHAR_MAMEKEY(SLASH_PAD))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)

	PORT_START("LINE8")
	PORT_BIT(0xFF, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("CLINE0")
	PORT_BIT(0x1F, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_9_PAD) PORT_CHAR(UCHAR_MAMEKEY(9_PAD))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER_PAD) PORT_CHAR(UCHAR_MAMEKEY(ENTER_PAD))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_3_PAD) PORT_CHAR(UCHAR_MAMEKEY(3_PAD))

	PORT_START("CLINE1")
	PORT_BIT(0x1F, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ASTERISK) PORT_CHAR(UCHAR_MAMEKEY(ASTERISK))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_ESC) PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH2) PORT_CHAR(';') PORT_CHAR('+')

	PORT_START("CLINE2")
	PORT_BIT(0x1F, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS_PAD) PORT_CHAR(UCHAR_MAMEKEY(MINUS_PAD))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_TAB) PORT_CHAR('\t')
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_J) PORT_CHAR('J')

	PORT_START("CLINE3")
	PORT_BIT(0x7F, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_F) PORT_CHAR('F')

	PORT_START("CLINE4")
	PORT_BIT(0x7F, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')

	PORT_START("CLINE5")
	PORT_BIT(0x1F, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Ctrl") PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_RCONTROL)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("Rus/Lat") PORT_CODE(KEYCODE_LALT) PORT_CODE(KEYCODE_RALT)

	PORT_START("CLINE6")
	PORT_BIT(0x1F, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_7_PAD) PORT_CHAR(UCHAR_MAMEKEY(7_PAD))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_0_PAD) PORT_CHAR(UCHAR_MAMEKEY(0_PAD))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_1_PAD) PORT_CHAR(UCHAR_MAMEKEY(1_PAD))

	PORT_START("CLINE7")
	PORT_BIT(0x1F, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_8_PAD) PORT_CHAR(UCHAR_MAMEKEY(8_PAD))
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_DEL_PAD) PORT_CHAR(UCHAR_MAMEKEY(DEL_PAD))
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_CODE(KEYCODE_2_PAD) PORT_CHAR(UCHAR_MAMEKEY(2_PAD))
INPUT_PORTS_END

static const cassette_interface radio86_cassette_interface =
{
	rkr_cassette_formats,
	NULL,
	(cassette_state)(CASSETTE_STOPPED | CASSETTE_SPEAKER_ENABLED | CASSETTE_MOTOR_ENABLED),
	"radio86_cass",
	NULL
};


/* F4 Character Displayer */
static const gfx_layout radio86_charlayout =
{
	8, 8,                   /* 8 x 8 characters */
	128,                    /* 128 characters */
	1,                  /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	/* x offsets */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	/* y offsets */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8                 /* every char takes 8 bytes */
};

static GFXDECODE_START( radio86 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, radio86_charlayout, 0, 1 )
GFXDECODE_END


/* Machine driver */
static MACHINE_CONFIG_START( radio86, radio86_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",I8080, XTAL_16MHz / 9)
	MCFG_CPU_PROGRAM_MAP(radio86_mem)
	MCFG_CPU_IO_MAP(radio86_io)
	MCFG_MACHINE_RESET_OVERRIDE(radio86_state, radio86 )

	MCFG_I8255_ADD( "ppi8255_1", radio86_ppi8255_interface_1 )

	MCFG_I8275_ADD  ( "i8275", radio86_i8275_interface)
	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_UPDATE_DEVICE("i8275", i8275_device, screen_update)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(78*6, 30*10)
	MCFG_SCREEN_VISIBLE_AREA(0, 78*6-1, 0, 30*10-1)
	MCFG_GFXDECODE(radio86)
	MCFG_PALETTE_LENGTH(3)
	MCFG_PALETTE_INIT_OVERRIDE(radio86_state,radio86)

	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MCFG_I8257_ADD("dma8257", XTAL_16MHz / 9, radio86_dma)

	MCFG_CASSETTE_ADD( "cassette", radio86_cassette_interface )
	MCFG_SOFTWARE_LIST_ADD("cass_list","radio86")
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( radio16, radio86 )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(radio86_16_mem)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( radiorom, radio86 )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(radio86rom_mem)

	MCFG_I8255_ADD( "ppi8255_2", radio86_ppi8255_interface_2 )

	MCFG_CARTSLOT_ADD("cart")
	MCFG_CARTSLOT_EXTENSION_LIST("bin,rom")
	MCFG_CARTSLOT_NOT_MANDATORY
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( radioram, radio86 )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(radio86ram_mem)

	MCFG_I8255_ADD( "ppi8255_2", radio86_ppi8255_interface_2 )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( rk7007, radio86 )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_IO_MAP(rk7007_io)

	MCFG_I8255_ADD( "ms7007", rk7007_ppi8255_interface )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( rk700716, radio16 )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_IO_MAP(rk7007_io)

	MCFG_I8255_ADD( "ms7007", rk7007_ppi8255_interface )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( mikron2, radio86 )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(mikron2_mem)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( impuls03, radio86 )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(impuls03_mem)
MACHINE_CONFIG_END

/* ROM definition */
ROM_START( radio86 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "bios.rom", 0xf800, 0x0800, CRC(bf1ceea5) SHA1(8f3d472203e550e9854dd79e1f44628635581ed0))
	ROM_COPY( "maincpu", 0xf800, 0xf000, 0x0800 )
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("radio86.fnt", 0x0000, 0x0400, CRC(7666bd5e) SHA1(8652787603bee9b4da204745e3b2aa07a4783dfc))
ROM_END

ROM_START( radio4k )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "bios4k.rom", 0xf000, 0x1000, CRC(2ac9d864) SHA1(296716c6cddc9dd31d500ba421aa807c45757cfd))
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("radio86.fnt", 0x0000, 0x0400, CRC(7666bd5e) SHA1(8652787603bee9b4da204745e3b2aa07a4783dfc))
ROM_END

ROM_START( spektr01 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "spektr001.rom", 0xf800, 0x0800, CRC(5a38e6d5) SHA1(799c3bbe2a9f08f3aba55379cc093329048350ff))
	ROM_COPY( "maincpu", 0xf800, 0xf000, 0x0800 )
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("radio86.fnt", 0x0000, 0x0400, CRC(7666bd5e) SHA1(8652787603bee9b4da204745e3b2aa07a4783dfc))
ROM_END

ROM_START( radio16 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "rk86.16k", 0xf800, 0x0800, CRC(fd8a4caf) SHA1(90d6af571049a7c8748eac03541e921eac3f70c5))
	ROM_COPY( "maincpu", 0xf800, 0xf000, 0x0800 )
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("radio86.fnt", 0x0000, 0x0400, CRC(7666bd5e) SHA1(8652787603bee9b4da204745e3b2aa07a4783dfc))
ROM_END

ROM_START( radiorom )
	ROM_REGION( 0x20000, "maincpu", ROMREGION_ERASEFF )
	ROM_SYSTEM_BIOS(0, "32k", "32 KB rom disk")
	ROMX_LOAD( "radiorom.rom", 0xf800, 0x0800, CRC(B5CDEAB7) SHA1(1c80d72082f2fb2190b575726cb82d86ae0ee7d8), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "64k", "64 KB rom disk")
	ROMX_LOAD( "radiorom.64",  0xf800, 0x0800, CRC(5250b927) SHA1(e885e0f5b2325190b38a4c92b20a8b4fa78fbd8f), ROM_BIOS(2))
	ROM_COPY( "maincpu", 0xf800, 0xf000, 0x0800 )
	ROM_CART_LOAD("cart", 0x10000,  0x10000, ROM_NOMIRROR | ROM_OPTIONAL)
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("radio86.fnt", 0x0000, 0x0400, CRC(7666bd5e) SHA1(8652787603bee9b4da204745e3b2aa07a4783dfc))
ROM_END

ROM_START( radioram )
	ROM_REGION( 0x20000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "r86-1.bin", 0xf800, 0x0800, CRC(7E7AB7CB) SHA1(fedb00b6b8fbe1167faba3e4611b483f800e6934))
	ROM_LOAD( "r86-2.bin", 0xe000, 0x0800, CRC(955F0616) SHA1(d2b9f960558bdcb60074091fc79d1ad56c313586))
	ROM_LOAD( "romdisk.bin", 0x10000, 0x10000, CRC(43C0279B) SHA1(bc1dfd9bdbce39460616e2158f5d96279d0af3cf))
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("radio86.fnt", 0x0000, 0x0400, CRC(7666bd5e) SHA1(8652787603bee9b4da204745e3b2aa07a4783dfc))
ROM_END

ROM_START( rk7007 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "ms7007.rom", 0xf800, 0x0800, CRC(002811DC) SHA1(4529eb72198c49af77fbcd7833bcd06a1cf9b1ac))
	ROM_COPY( "maincpu", 0xf800, 0xf000, 0x0800 )
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("radio86.fnt", 0x0000, 0x0400, CRC(7666bd5e) SHA1(8652787603bee9b4da204745e3b2aa07a4783dfc))
ROM_END

ROM_START( rk700716 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "ms7007.16k", 0xf800, 0x0800, CRC(5268D7B6) SHA1(efd69d8456b8cf8b37f33237153c659725608528))
	ROM_COPY( "maincpu", 0xf800, 0xf000, 0x0800 )
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("radio86.fnt", 0x0000, 0x0400, CRC(7666bd5e) SHA1(8652787603bee9b4da204745e3b2aa07a4783dfc))
ROM_END

ROM_START( mikron2 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "mikron2.bin", 0xf800, 0x0800, CRC(2cd79bb4) SHA1(501df47e65aaa8f4ce27751bc2a7e7089e2e888c))
	ROM_COPY( "maincpu", 0xf800, 0xf000, 0x0800 )
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("radio86.fnt", 0x0000, 0x0400, CRC(7666bd5e) SHA1(8652787603bee9b4da204745e3b2aa07a4783dfc))
ROM_END

ROM_START( kr03 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "kr03-dd17.rf2", 0xf800, 0x0800, CRC(AC2E24D5) SHA1(a1317a261bfd55b3b37109b14d1391308dee04de))
	ROM_COPY( "maincpu", 0xf800, 0xf000, 0x0800 )
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("kr03-dd12.rf2", 0x0000, 0x0800, CRC(085F4259) SHA1(11c5829b072a00961ad936c26559fb63bf2dc896))
ROM_END

ROM_START( impuls03 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "imp03bas.rom", 0xa000, 0x2000, CRC(b13b2de4) SHA1(9af8c49d72ca257bc34cad3c62e530730929702e))
	ROM_LOAD( "imp03mon.rom", 0xf800, 0x0800, CRC(8c591ce4) SHA1(8e8e9cba6b3123d74218b92f4b4210606ba53376))
	ROM_COPY( "maincpu", 0xf800, 0xf000, 0x0800 )
	ROM_REGION(0x0800, "gfx1",0)
	ROM_LOAD ("radio86.fnt", 0x0000, 0x0400, CRC(7666bd5e) SHA1(8652787603bee9b4da204745e3b2aa07a4783dfc))
ROM_END
/* Driver */

/*    YEAR  NAME    PARENT  COMPAT   MACHINE    INPUT    INIT     COMPANY   FULLNAME       FLAGS */
COMP( 1986, radio86,  0,       0,   radio86,    radio86, radio86_state, radio86,  "<unknown>", "Radio-86RK",    0)
COMP( 1986, radio16,  radio86, 0,   radio16,    radio86, radio86_state, radio86,  "<unknown>", "Radio-86RK (16K RAM)",  0)
COMP( 1986, kr03,     radio86, 0,   radio86,    radio86, radio86_state, radio86,  "Elektronika", "KR-03",   0)
COMP( 1986, radio4k,  radio86, 0,   radio86,    radio86, radio86_state, radio86,  "<unknown>", "Radio-86RK (4K ROM)",   0)
COMP( 1986, radiorom, radio86, 0,   radiorom,   radio86, radio86_state, radio86,  "<unknown>", "Radio-86RK (ROM-Disk)", 0)
COMP( 1986, radioram, radio86, 0,   radioram,   radio86, radio86_state, radioram, "<unknown>", "Radio-86RK (ROM/RAM Disk)", 0)
COMP( 1986, spektr01, radio86, 0,   radio86,    radio86, radio86_state, radio86,  "<unknown>", "Spektr-001",    0)
COMP( 1986, rk7007,   radio86, 0,   rk7007,     ms7007, radio86_state,  radio86,  "<unknown>", "Radio-86RK (MS7007)",   0)
COMP( 1986, rk700716, radio86, 0,   rk700716,   ms7007, radio86_state,  radio86,  "<unknown>", "Radio-86RK (MS7007 16K RAM)",   0)
COMP( 1986, mikron2,  radio86, 0,   mikron2,    radio86, radio86_state, radio86,  "<unknown>", "Mikron-2",  0)
COMP( 1986, impuls03,  radio86, 0,  impuls03,   radio86, radio86_state, radio86,  "<unknown>", "Impuls-03", 0)
