/*******************************************************************************

PMD-85 driver by Krzysztof Strzecha

What's new:
-----------

27.06.2004  Mato crash fixed.
21.05.2004  V.24 / Tape switch added. V.24 is not emulated.
25.04.2004  PMD-85.1 tape emulation with support for .pmd format files added.
19.04.2004  Verified PMD-85.1 and PMD-85.2 monitor roms and replaced with
        unmodified ones.
        Memory system cleanups.
03.04.2004  PMD-85.3 and Mato (PMD-85.2 clone) drivers.
        Preliminary and not working tape support.
        Reset key fixed. PMD-85.1 fixed.
15.03.2004  Added drivers for: PMD-85.2, PMD-85.2A, PMD-85.2B and Didaktik
        Alfa (PMD-85.1 clone). Keyboard finished. Natural keyboard added.
        Memory system rewritten. I/O system rewritten. Support for Basic
        ROM module added. Video emulation rewritten.
30.11.2002  Memory mapping improved.
06.07.2002  Preliminary driver.

Notes on emulation status and to do list:
-----------------------------------------

1. V.24.
2. Tape emulation for other machines than PMD-85.1.
3. Flash video attribute.
4. External interfaces connectors (K2-K5).
5. Speaker.
6. Verify PMD-85.2A, PMD-85.3, Didaktik Alfa and Mato monitor roms.
7. Verify all Basic roms.
8. 8251 in Didaktik Alfa.
9. Colors (if any).
10. PMD-85, Didaktik Alfa 2 and Didaktik Beta (ROMs and documentation needed).
11. FDD interface (ROMs and disk images needed).
12. "Duch & Pampuch" Mato game displays scores with incorrect characters.

PMD-85 technical information
============================

Memory map:
-----------

    PMD-85.1, PMD-85.2
    ------------------

    start-up map (cleared by the first I/O write operation done by the CPU):
    0000-0fff ROM mirror #1
    1000-1fff not mapped
    2000-2fff ROM mirror #2
    3000-3fff not mapped
    4000-7fff Video RAM mirror #1
    8000-8fff ROM
    9000-9fff not mapped
    a000-afff ROM mirror #3
    b000-bfff not mapped
    c000-ffff Video RAM

    normal map:
    0000-7fff RAM
    8000-8fff ROM
    9000-9fff not mapped
    a000-afff ROM mirror #1
    b000-bfff not mapped
    c000-ffff Video RAM

    Didaktik Alfa (PMD-85.1 clone)
    ------------------------------

    start-up map (cleared by the first I/O write operation done by the CPU):
    0000-0fff ROM mirror
    1000-33ff BASIC mirror
    3400-3fff not mapped
    4000-7fff Video RAM mirror
    8000-8fff ROM
    9000-b3ff BASIC
    b400-bfff not mapped
    c000-ffff Video RAM

    normal map:
    0000-7fff RAM
    8000-8fff ROM
    9000-b3ff BASIC
    b400-bfff not mapped
    c000-ffff Video RAM

    PMD-85.2A, PMD-85.2B
    --------------------

    start-up map (cleared by the first I/O write operation done by the CPU):
    0000-0fff ROM mirror #1
    1000-1fff RAM #2 mirror
    2000-2fff ROM mirror #2
    3000-3fff RAM #3 mirror
    4000-7fff Video RAM mirror #1
    8000-8fff ROM
    9000-9fff RAM #2
    a000-afff ROM mirror #3
    b000-bfff RAM #3
    c000-ffff Video RAM

    normal map:
    0000-7fff RAM #1
    8000-8fff ROM
    9000-9fff RAM #2
    a000-afff ROM mirror #1
    b000-bfff RAM #3
    c000-ffff Video RAM

    PMD-85.3
    --------

    start-up map (cleared by the first I/O write operation done by the CPU):
    0000-1fff ROM mirror #1 read, RAM write
    2000-3fff ROM mirror #2 read, RAM write
    4000-5fff ROM mirror #3 read, RAM write
    6000-7fff ROM mirror #4 read, RAM write
    8000-9fff ROM mirror #5 read, RAM write
    a000-bfff ROM mirror #6 read, RAM write
    c000-dfff ROM mirror #7 read, Video RAM #1 write
    e000-ffff ROM, Video RAM #2 write

    normal map:
    0000-bfff RAM
    c000-dfff Video RAM #1
    e000-ffff Video RAM #2 / ROM read, Video RAM #2 write

    Mato
    ----

    start-up map (cleared by the first I/O write operation done by the CPU):
    0000-3fff ROM mirror #1
    4000-7fff Video RAM mirror #1
    8000-bfff ROM
    c000-ffff Video RAM

    normal map:
    0000-7fff RAM
    8000-bfff ROM
    c000-ffff Video RAM

I/O ports
---------

    I/O board
    ---------
    1xxx11aa    external interfaces connector (K2)

    0xxx11aa    I/O board interfaces
        000111aa    8251 (casette recorder, V24)
        010011aa    8255 (GPIO/0, GPIO/1)
        010111aa    8253
        011111aa    8255 (IMS-2)
    I/O board is not supported by Mato.

    Motherboard
    -----------
    1xxx01aa    8255 (keyboard, speaker, LEDs)
            PMD-85.3 memory banking
            Mato cassette recorder

    ROM Module
    ----------
    1xxx10aa    8255 (ROM reading)
    ROM module is not supported by Didaktik Alfa and Mato.


*******************************************************************************/

#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "sound/wave.h"
#include "imagedev/cassette.h"
#include "includes/pmd85.h"
#include "machine/i8255.h"
#include "machine/pit8253.h"
#include "machine/i8251.h"
#include "formats/pmd_cas.h"
#include "machine/ram.h"

/* I/O ports */

static ADDRESS_MAP_START( pmd85_io_map, AS_IO, 8, pmd85_state )
	AM_RANGE( 0x00, 0xff) AM_READWRITE(pmd85_io_r, pmd85_io_w )
ADDRESS_MAP_END

static ADDRESS_MAP_START( mato_io_map, AS_IO, 8, pmd85_state )
	AM_RANGE( 0x00, 0xff) AM_READWRITE(mato_io_r, mato_io_w )
ADDRESS_MAP_END

/* memory w/r functions */

static ADDRESS_MAP_START( pmd85_mem , AS_PROGRAM, 8, pmd85_state )
	AM_RANGE(0x0000, 0x0fff) AM_RAMBANK("bank1")
	AM_RANGE(0x1000, 0x1fff) AM_RAMBANK("bank2")
	AM_RANGE(0x2000, 0x2fff) AM_RAMBANK("bank3")
	AM_RANGE(0x3000, 0x3fff) AM_RAMBANK("bank4")
	AM_RANGE(0x4000, 0x7fff) AM_RAMBANK("bank5")
	AM_RANGE(0x8000, 0x8fff) AM_READ_BANK("bank6")
	AM_RANGE(0x9000, 0x9fff) AM_NOP
	AM_RANGE(0xa000, 0xafff) AM_READ_BANK("bank7")
	AM_RANGE(0xb000, 0xbfff) AM_NOP
	AM_RANGE(0xc000, 0xffff) AM_RAMBANK("bank8")
ADDRESS_MAP_END

static ADDRESS_MAP_START( pmd852a_mem , AS_PROGRAM, 8, pmd85_state )
	AM_RANGE(0x0000, 0x0fff) AM_RAMBANK("bank1")
	AM_RANGE(0x1000, 0x1fff) AM_RAMBANK("bank2")
	AM_RANGE(0x2000, 0x2fff) AM_RAMBANK("bank3")
	AM_RANGE(0x3000, 0x3fff) AM_RAMBANK("bank4")
	AM_RANGE(0x4000, 0x7fff) AM_RAMBANK("bank5")
	AM_RANGE(0x8000, 0x8fff) AM_READ_BANK("bank6")
	AM_RANGE(0x9000, 0x9fff) AM_RAMBANK("bank7")
	AM_RANGE(0xa000, 0xafff) AM_READ_BANK("bank8")
	AM_RANGE(0xb000, 0xbfff) AM_RAMBANK("bank9")
	AM_RANGE(0xc000, 0xffff) AM_RAMBANK("bank10")
ADDRESS_MAP_END

static ADDRESS_MAP_START( pmd853_mem , AS_PROGRAM, 8, pmd85_state )
	AM_RANGE(0x0000, 0x1fff) AM_READ_BANK("bank1") AM_WRITE_BANK("bank9")
	AM_RANGE(0x2000, 0x3fff) AM_READ_BANK("bank2") AM_WRITE_BANK("bank10")
	AM_RANGE(0x4000, 0x5fff) AM_READ_BANK("bank3") AM_WRITE_BANK("bank11")
	AM_RANGE(0x6000, 0x7fff) AM_READ_BANK("bank4") AM_WRITE_BANK("bank12")
	AM_RANGE(0x8000, 0x9fff) AM_READ_BANK("bank5") AM_WRITE_BANK("bank13")
	AM_RANGE(0xa000, 0xbfff) AM_READ_BANK("bank6") AM_WRITE_BANK("bank14")
	AM_RANGE(0xc000, 0xdfff) AM_READ_BANK("bank7") AM_WRITE_BANK("bank15")
	AM_RANGE(0xe000, 0xffff) AM_READ_BANK("bank8") AM_WRITE_BANK("bank16")
ADDRESS_MAP_END

static ADDRESS_MAP_START( alfa_mem , AS_PROGRAM, 8, pmd85_state )
	AM_RANGE(0x0000, 0x0fff) AM_RAMBANK("bank1")
	AM_RANGE(0x1000, 0x33ff) AM_RAMBANK("bank2")
	AM_RANGE(0x3400, 0x3fff) AM_RAMBANK("bank3")
	AM_RANGE(0x4000, 0x7fff) AM_RAMBANK("bank4")
	AM_RANGE(0x8000, 0x8fff) AM_READ_BANK("bank5")
	AM_RANGE(0x9000, 0xb3ff) AM_READ_BANK("bank6")
	AM_RANGE(0xb400, 0xbfff) AM_NOP
	AM_RANGE(0xc000, 0xffff) AM_RAMBANK("bank7")
ADDRESS_MAP_END

static ADDRESS_MAP_START( mato_mem , AS_PROGRAM, 8, pmd85_state )
	AM_RANGE(0x0000, 0x3fff) AM_RAMBANK("bank1")
	AM_RANGE(0x4000, 0x7fff) AM_RAMBANK("bank2")
	AM_RANGE(0x8000, 0xbfff) AM_READ_BANK("bank3")
	AM_RANGE(0xc000, 0xffff) AM_RAMBANK("bank4")
ADDRESS_MAP_END

static ADDRESS_MAP_START( c2717_mem , AS_PROGRAM, 8, pmd85_state )
	AM_RANGE(0x0000, 0x3fff) AM_RAMBANK("bank1")
	AM_RANGE(0x4000, 0x7fff) AM_RAMBANK("bank2")
	AM_RANGE(0x8000, 0xbfff) AM_READ_BANK("bank3")
	AM_RANGE(0xc000, 0xffff) AM_RAMBANK("bank4")
ADDRESS_MAP_END

/* keyboard input */

static INPUT_PORTS_START( pmd85 )
	PORT_START("KEY0") /* port 0x00 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K0") PORT_CODE(KEYCODE_ESC) PORT_CHAR(UCHAR_MAMEKEY(F1))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1)          PORT_CHAR('1') PORT_CHAR('!')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q)          PORT_CHAR('q') PORT_CHAR('Q')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A)          PORT_CHAR('a') PORT_CHAR('A')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SPACE)      PORT_CHAR(' ')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY1") /* port 0x01 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K1") PORT_CODE(KEYCODE_F1) PORT_CHAR(UCHAR_MAMEKEY(F2))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2)          PORT_CHAR('2') PORT_CHAR('"')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W)          PORT_CHAR('w') PORT_CHAR('W')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S)          PORT_CHAR('s') PORT_CHAR('S')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z)          PORT_CHAR('z') PORT_CHAR('Z')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY2") /* port 0x02 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K2") PORT_CODE(KEYCODE_F2) PORT_CHAR(UCHAR_MAMEKEY(F3))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3)          PORT_CHAR('3') PORT_CHAR('#')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E)          PORT_CHAR('e') PORT_CHAR('E')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D)          PORT_CHAR('d') PORT_CHAR('D')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X)          PORT_CHAR('x') PORT_CHAR('X')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY3") /* port 0x03 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K3") PORT_CODE(KEYCODE_F3) PORT_CHAR(UCHAR_MAMEKEY(F4))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4)          PORT_CHAR('4') PORT_CHAR('$')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R)          PORT_CHAR('r') PORT_CHAR('R')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F)          PORT_CHAR('f') PORT_CHAR('F')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C)          PORT_CHAR('c') PORT_CHAR('C')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY4") /* port 0x04 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K4") PORT_CODE(KEYCODE_F4) PORT_CHAR(UCHAR_MAMEKEY(F5))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5)          PORT_CHAR('5') PORT_CHAR('%')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T)          PORT_CHAR('t') PORT_CHAR('T')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G)          PORT_CHAR('g') PORT_CHAR('G')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V)          PORT_CHAR('v') PORT_CHAR('V')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY5") /* port 0x05 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K5") PORT_CODE(KEYCODE_F5) PORT_CHAR(UCHAR_MAMEKEY(F6))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6)          PORT_CHAR('6') PORT_CHAR('&')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y)          PORT_CHAR('y') PORT_CHAR('Y')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_H)          PORT_CHAR('h') PORT_CHAR('H')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B)          PORT_CHAR('b') PORT_CHAR('B')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY6") /* port 0x06 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K6") PORT_CODE(KEYCODE_F6) PORT_CHAR(UCHAR_MAMEKEY(F7))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7)          PORT_CHAR('7') PORT_CHAR('\'')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U)          PORT_CHAR('u') PORT_CHAR('U')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J)          PORT_CHAR('j') PORT_CHAR('J')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_N)          PORT_CHAR('n') PORT_CHAR('N')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY7") /* port 0x07 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K7") PORT_CODE(KEYCODE_F7) PORT_CHAR(UCHAR_MAMEKEY(F8))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8)          PORT_CHAR('8') PORT_CHAR('(')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I)          PORT_CHAR('i') PORT_CHAR('I')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K)          PORT_CHAR('k') PORT_CHAR('K')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M)          PORT_CHAR('m') PORT_CHAR('M')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY8") /* port 0x08 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K8") PORT_CODE(KEYCODE_F8) PORT_CHAR(UCHAR_MAMEKEY(F9))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9)          PORT_CHAR('9') PORT_CHAR(')')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O)          PORT_CHAR('o') PORT_CHAR('O')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L)          PORT_CHAR('l') PORT_CHAR('L')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COMMA)      PORT_CHAR(',') PORT_CHAR('<')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY9") /* port 0x09 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K9") PORT_CODE(KEYCODE_F9) PORT_CHAR(UCHAR_MAMEKEY(F10))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0)          PORT_CHAR('0') PORT_CHAR('-')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P)          PORT_CHAR('p') PORT_CHAR('P')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COLON)      PORT_CHAR(';') PORT_CHAR('+')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_STOP)       PORT_CHAR('.') PORT_CHAR('>')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY10") /* port 0x0a */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K10") PORT_CODE(KEYCODE_F10) PORT_CHAR(UCHAR_MAMEKEY(F11))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_MINUS)      PORT_CHAR('_') PORT_CHAR('=')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_OPENBRACE)  PORT_CHAR('@') PORT_CHAR('`')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_QUOTE)      PORT_CHAR(':') PORT_CHAR('*')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SLASH)      PORT_CHAR('/') PORT_CHAR('?')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY11") /* port 0x0b */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("K11") PORT_CODE(KEYCODE_F11) PORT_CHAR(UCHAR_MAMEKEY(F12))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_EQUALS)     PORT_CHAR('{') PORT_CHAR('}')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR('\\') PORT_CHAR('^')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_BACKSLASH)  PORT_CHAR('[') PORT_CHAR(']')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY12") /* port 0x0c */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("WRK") PORT_CODE(KEYCODE_INSERT) PORT_CHAR(UCHAR_MAMEKEY(RCONTROL))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("INS PTL") PORT_CODE(KEYCODE_DEL) PORT_CHAR(UCHAR_MAMEKEY(INSERT)) PORT_CHAR(UCHAR_MAMEKEY(TAB))
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("<-") PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("|<-") PORT_CODE(KEYCODE_LALT) PORT_CHAR(UCHAR_MAMEKEY(LALT))
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY13") /* port 0x0d */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("C-D") PORT_CODE(KEYCODE_HOME) PORT_CHAR(UCHAR_MAMEKEY(CAPSLOCK))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("DEL") PORT_CODE(KEYCODE_END) PORT_CHAR(UCHAR_MAMEKEY(DEL))
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("^\\") PORT_CODE(KEYCODE_UP) PORT_CHAR(UCHAR_MAMEKEY(HOME))
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("END") PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(END))
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("EOL1") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(UCHAR_MAMEKEY(ENTER))
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY14") /* port 0x0e */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("CLR") PORT_CODE(KEYCODE_PGUP) PORT_CHAR(UCHAR_MAMEKEY(PGUP))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("RCL") PORT_CODE(KEYCODE_PGDN) PORT_CHAR(UCHAR_MAMEKEY(PGDN))
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("->") PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("->|") PORT_CODE(KEYCODE_RALT) PORT_CHAR(UCHAR_MAMEKEY(RALT))
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("EOL2") PORT_CODE(KEYCODE_TAB) PORT_CHAR(UCHAR_MAMEKEY(ENTER_PAD))
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("KEY15") /* port 0x0f */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_MAMEKEY(RSHIFT))
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Stop") PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_MAMEKEY(LCONTROL))
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("RESET") /* port 0x10 */
		PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("RST") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(UCHAR_MAMEKEY(BACKSPACE)) PORT_CHANGED_MEMBER(DEVICE_SELF, pmd85_state, pmd85_reset, 0)

	PORT_START("DSW0") /* port 0x11 */
		PORT_CONFNAME( 0x01, 0x00, "Basic ROM Module" )
			PORT_CONFSETTING( 0x00, DEF_STR( Off ) )
			PORT_CONFSETTING( 0x01, DEF_STR( On ) )
		PORT_CONFNAME( 0x02, 0x00, "Tape/V.24" )
			PORT_CONFSETTING( 0x00, "Tape" )
			PORT_CONFSETTING( 0x02, "V.24" )
INPUT_PORTS_END


static INPUT_PORTS_START( alfa )
	PORT_INCLUDE( pmd85 )

	PORT_MODIFY( "DSW0" )
		PORT_BIT( 0x01, 0x00, IPT_UNUSED )
INPUT_PORTS_END


static INPUT_PORTS_START (mato)
	PORT_START("KEY0") /* port 0x00 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1)          PORT_CHAR('1') PORT_CHAR('!')
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2)          PORT_CHAR('2') PORT_CHAR('\"')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3)          PORT_CHAR('3') PORT_CHAR('#')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4)          PORT_CHAR('4') PORT_CHAR('$')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5)          PORT_CHAR('5') PORT_CHAR('%')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6)          PORT_CHAR('6') PORT_CHAR('&')
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7)          PORT_CHAR('7') PORT_CHAR('\'')
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("EOL") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_START("KEY1") /* port 0x01 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q)          PORT_CHAR('q') PORT_CHAR('Q')
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W)          PORT_CHAR('w') PORT_CHAR('W')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E)          PORT_CHAR('e') PORT_CHAR('E')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R)          PORT_CHAR('r') PORT_CHAR('R')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T)          PORT_CHAR('t') PORT_CHAR('T')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y)          PORT_CHAR('y') PORT_CHAR('Y')
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U)          PORT_CHAR('u') PORT_CHAR('U')
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("EOL") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_START("KEY2") /* port 0x02 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A)          PORT_CHAR('a') PORT_CHAR('A')
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S)          PORT_CHAR('s') PORT_CHAR('S')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D)          PORT_CHAR('d') PORT_CHAR('D')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F)          PORT_CHAR('f') PORT_CHAR('F')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G)          PORT_CHAR('g') PORT_CHAR('G')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_H)          PORT_CHAR('h') PORT_CHAR('H')
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J)          PORT_CHAR('j') PORT_CHAR('J')
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("EOL") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_START("KEY3") /* port 0x03 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z)          PORT_CHAR('z') PORT_CHAR('Z')
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X)          PORT_CHAR('x') PORT_CHAR('X')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C)          PORT_CHAR('c') PORT_CHAR('C')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V)          PORT_CHAR('v') PORT_CHAR('V')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B)          PORT_CHAR('b') PORT_CHAR('B')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_N)          PORT_CHAR('n') PORT_CHAR('N')
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M)          PORT_CHAR('m') PORT_CHAR('M')
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("EOL") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_START("KEY4") /* port 0x04 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_MINUS)      PORT_CHAR('=') PORT_CHAR('_')
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0)          PORT_CHAR('0') PORT_CHAR('-')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR('\\') PORT_CHAR('^')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8)          PORT_CHAR('8') PORT_CHAR('(')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COMMA)      PORT_CHAR(',') PORT_CHAR('<')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K)          PORT_CHAR('k') PORT_CHAR('K')
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I)          PORT_CHAR('i') PORT_CHAR('I')
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("EOL") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_START("KEY5") /* port 0x05 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P)          PORT_CHAR('p') PORT_CHAR('P')
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SLASH)      PORT_CHAR('/') PORT_CHAR('?')
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COLON)      PORT_CHAR(';') PORT_CHAR('+')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9)          PORT_CHAR('9') PORT_CHAR(')')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_STOP)           PORT_CHAR('.') PORT_CHAR('<')
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L)          PORT_CHAR('l') PORT_CHAR('L')
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O)          PORT_CHAR('o') PORT_CHAR('O')
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("EOL") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_START("KEY6") /* port 0x06 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_DOWN)       PORT_CHAR(UCHAR_MAMEKEY(DOWN))
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_UP)         PORT_CHAR(UCHAR_MAMEKEY(UP))
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SPACE)      PORT_CHAR(' ')
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_OPENBRACE)  PORT_CHAR('@')
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_LEFT)           PORT_CHAR(UCHAR_MAMEKEY(LEFT))
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_RIGHT)      PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_CODE(KEYCODE_QUOTE)      PORT_CHAR(':') PORT_CHAR('*')
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("EOL") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_START("KEY7") /* port 0x07 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("EOL") PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_START("KEY8") /* port 0x08 */
		PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNUSED )
		PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Stop") PORT_CODE(KEYCODE_LCONTROL) PORT_CHAR(UCHAR_MAMEKEY(LCONTROL))
		PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_MAMEKEY(RSHIFT))
		PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_KEYBOARD ) PORT_NAME("Continue") PORT_CODE(KEYCODE_RCONTROL) PORT_CHAR(UCHAR_MAMEKEY(RCONTROL))
		PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("RESET") /* port 0x09 */
		PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_NAME("RST") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(UCHAR_MAMEKEY(BACKSPACE)) PORT_CHANGED_MEMBER(DEVICE_SELF, pmd85_state, pmd85_reset, 0)
INPUT_PORTS_END



static const struct CassetteOptions pmd85_cassette_options =
{
	1,      /* channels */
	16,     /* bits per sample */
	7200    /* sample frequency */
};

static const cassette_interface pmd85_cassette_interface =
{
	pmd85_cassette_formats,
	&pmd85_cassette_options,
	(cassette_state)(CASSETTE_STOPPED | CASSETTE_SPEAKER_ENABLED),
	NULL,
	NULL
};

const i8251_interface pmd85_i8251_interface =
{
	DEVCB_DRIVER_LINE_MEMBER(pmd85_state, write_cas_tx),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};


/* machine definition */
static MACHINE_CONFIG_START( pmd85, pmd85_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", I8080, 2000000)     /* 2.048MHz ??? */
	MCFG_CPU_PROGRAM_MAP(pmd85_mem)
	MCFG_CPU_IO_MAP(pmd85_io_map)
	MCFG_QUANTUM_TIME(attotime::from_hz(60))


	MCFG_PIT8253_ADD( "pit8253", pmd85_pit8253_interface )

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(0)
	MCFG_SCREEN_SIZE(288, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 288-1, 0, 256-1)
	MCFG_SCREEN_UPDATE_DRIVER(pmd85_state, screen_update_pmd85)

	MCFG_PALETTE_LENGTH(sizeof (pmd85_palette) / 3)


	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, "cassette")
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	/* cassette */
	MCFG_CASSETTE_ADD( "cassette", pmd85_cassette_interface )

	/* uart */
	MCFG_I8251_ADD("uart", pmd85_i8251_interface)

	/* internal ram */
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("64K")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( pmd851, pmd85 )

	MCFG_I8255_ADD( "ppi8255_0", pmd85_ppi8255_interface[0] )

	MCFG_I8255_ADD( "ppi8255_1", pmd85_ppi8255_interface[1] )

	MCFG_I8255_ADD( "ppi8255_2", pmd85_ppi8255_interface[2] )

	MCFG_I8255_ADD( "ppi8255_3", pmd85_ppi8255_interface[3] )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( pmd852a, pmd851 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(pmd852a_mem)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( pmd853, pmd851 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(pmd853_mem)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( alfa, pmd85 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(alfa_mem)

	MCFG_I8255_ADD( "ppi8255_0", alfa_ppi8255_interface[0] )

	MCFG_I8255_ADD( "ppi8255_1", alfa_ppi8255_interface[1] )

	MCFG_I8255_ADD( "ppi8255_2", alfa_ppi8255_interface[1] )

MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( mato, pmd85 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(mato_mem)
	MCFG_CPU_IO_MAP(mato_io_map)

	MCFG_I8255_ADD( "ppi8255_0", mato_ppi8255_interface )

	/* no uart */
	MCFG_DEVICE_REMOVE( "uart" )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( c2717, pmd851 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(c2717_mem)
MACHINE_CONFIG_END


ROM_START(pmd851)
	ROM_REGION(0x11000,"maincpu",0)
	ROM_LOAD("pmd85-1.bin", 0x10000, 0x1000, CRC(ef50b416) SHA1(afa3ec0d03228adc5287a4cba905ce7ad0497dff))
	ROM_REGION(0x2400,"user1",0)
	ROM_LOAD_OPTIONAL("pmd85-1.bas", 0x0000, 0x2400, CRC(4fc37d45) SHA1(3bd0f92f37a3f2ee539916dc75508bda37433a72))
ROM_END

ROM_START(pmd852)
	ROM_REGION(0x11000,"maincpu",0)
	ROM_LOAD("pmd85-2.bin", 0x10000, 0x1000, CRC(d4786f63) SHA1(6facdf37bb012714244b012a0c4bd715a956e42b))
	ROM_REGION(0x2400,"user1",0)
	ROM_LOAD_OPTIONAL("pmd85-2.bas", 0x0000, 0x2400, CRC(fc4a3ebf) SHA1(3bfc0e9a5cd5187da573b5d539d7246358125a88))
ROM_END

ROM_START(pmd852a)
	ROM_REGION(0x11000,"maincpu",0)
	ROM_LOAD("pmd85-2a.bin", 0x10000, 0x1000, CRC(5a9a961b) SHA1(7363341596367d08b9a98767c6585ce18dfd03af))
	ROM_REGION(0x2400,"user1",0)
	ROM_LOAD_OPTIONAL("pmd85-2a.bas", 0x0000, 0x2400, CRC(6ff379ad) SHA1(edcaf2420cac9771596ead5c86c41116b228eca3))
ROM_END

ROM_START(pmd852b)
	ROM_REGION(0x11000,"maincpu",0)
	ROM_LOAD("pmd85-2a.bin", 0x10000, 0x1000, CRC(5a9a961b) SHA1(7363341596367d08b9a98767c6585ce18dfd03af))
	ROM_REGION(0x2400,"user1",0)
	ROM_LOAD_OPTIONAL("pmd85-2a.bas", 0x0000, 0x2400, CRC(6ff379ad) SHA1(edcaf2420cac9771596ead5c86c41116b228eca3))
ROM_END

ROM_START(pmd853)
	ROM_REGION(0x12000,"maincpu",0)
	ROM_LOAD("pmd85-3.bin", 0x10000, 0x2000, CRC(83e22c47) SHA1(5f131e27ae3ec8907adbe5cd228c67d131066084))
	ROM_REGION(0x2800,"user1",0)
	ROM_LOAD_OPTIONAL("pmd85-3.bas", 0x0000, 0x2800, CRC(1e30e91d) SHA1(d086040abf4c0a7e5da8cf4db7d1668a1d9309a4))
ROM_END

ROM_START(alfa)
	ROM_REGION(0x13400,"maincpu",0)
	ROM_LOAD("alfa.bin", 0x10000, 0x1000, CRC(e425eedb) SHA1(db93b5de1e16b5ae71be08feb083a2ac15759495))
	ROM_LOAD("alfa.bas", 0x11000, 0x2400, CRC(9a73bfd2) SHA1(74314d989846f64e715f64deb84cb177fa62f4a9))
ROM_END

ROM_START(mato)
	ROM_REGION(0x14000,"maincpu",0)
	ROM_SYSTEM_BIOS(0, "default", "BASIC")
	ROMX_LOAD("mato.bin",  0x10000, 0x4000, CRC(574110a6) SHA1(4ff2cd4b07a1a700c55f92e5b381c04f758fb461), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "ru", "Russian")
	ROMX_LOAD("mato-ru.rom",  0x10000, 0x4000, CRC(44b68be4) SHA1(0d9ea9a9380e2af011a2f0b64c534dd0eb0a1fac), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(2, "lan", "BASIC LAN")
	ROMX_LOAD("mato-lan.rom",  0x10000, 0x4000, CRC(422cddde) SHA1(2a3dacf8e3e7637109c9d267f589a00881e9a5f4), ROM_BIOS(3))
	ROM_SYSTEM_BIOS(3, "games", "Games v1")
	ROMX_LOAD("matoh.bin", 0x10000, 0x4000, CRC(ca25880d) SHA1(38ce0b6a26d48a09fdf96863c3eaf3705aca2590), ROM_BIOS(4))
	ROM_SYSTEM_BIOS(4, "gamesen", "Games v2 EN")
	ROMX_LOAD("matogmen.rom", 0x10000, 0x4000, CRC(47e039c8) SHA1(6cc73a6b58921b33691d2751dee28428456eb222), ROM_BIOS(5))
	ROM_SYSTEM_BIOS(5, "gamessk", "Games v2 SK")
	ROMX_LOAD("matogmsk.rom", 0x10000, 0x4000, CRC(d0c9b1e7) SHA1(9e7289d971a957bf161c317e5fa76db3289ee23c), ROM_BIOS(6))
	ROM_SYSTEM_BIOS(6, "games3", "Games v3")
	ROMX_LOAD("matogm3.rom", 0x10000, 0x4000, CRC(9352f2c1) SHA1(b3e45c56d2800c69a0bb02febda6fa715f1afbc3), ROM_BIOS(7))
ROM_END

ROM_START(c2717)
	ROM_REGION(0x14000,"maincpu",0)
	ROM_LOAD("c2717.rom", 0x10000, 0x4000, CRC(da1703b1) SHA1(9fb93e6cae8b551064c7175bf3b4e3113429ce73))
ROM_END

ROM_START(c2717pmd)
	ROM_REGION(0x14000,"maincpu",0)
	ROM_LOAD( "c2717-pmd32.rom", 0x10000, 0x4000, CRC(cbdd323c) SHA1(ee9fea11be8bd4f945c583b0ae5684269906d0ce))
ROM_END


/*    YEAR  NAME     PARENT  COMPAT MACHINE  INPUT  INIT      COMPANY  FULLNAME */
COMP( 1985, pmd851,  0,      0,     pmd851,  pmd85, pmd85_state, pmd851,   "Tesla", "PMD-85.1" , 0)
COMP( 1985, pmd852,  pmd851, 0,     pmd851,  pmd85, pmd85_state, pmd851,   "Tesla", "PMD-85.2" , 0)
COMP( 1985, pmd852a, pmd851, 0,     pmd852a, pmd85, pmd85_state, pmd852a,  "Tesla", "PMD-85.2A" , 0)
COMP( 1985, pmd852b, pmd851, 0,     pmd852a, pmd85, pmd85_state, pmd852a,  "Tesla", "PMD-85.2B" , 0)
COMP( 1988, pmd853,  pmd851, 0,     pmd853,  pmd85, pmd85_state, pmd853,   "Tesla", "PMD-85.3" , 0)
COMP( 1986, alfa,    pmd851, 0,     alfa,    alfa, pmd85_state,  alfa,     "Didaktik Skalica", "Didaktik Alfa" , 0)
COMP( 1985, mato,    pmd851, 0,     mato,    mato, pmd85_state,  mato,     "Statny", "Mato" , 0)
COMP( 1989, c2717,   pmd851, 0,     c2717,   pmd85, pmd85_state, c2717,    "Zbrojovka Brno", "Consul 2717" , 0)
COMP( 1989, c2717pmd,pmd851, 0,     c2717,   pmd85, pmd85_state, c2717,    "Zbrojovka Brno", "Consul 2717 (with PMD-32)" , GAME_NOT_WORKING)
