/***************************************************************************

        BK driver by Miodrag Milanovic

        10/03/2008 Preliminary driver.

****************************************************************************/


#include "emu.h"
#include "cpu/t11/t11.h"
#include "sound/wave.h"
#include "includes/bk.h"
#include "imagedev/cassette.h"
#include "formats/rk_cas.h"

	/* Address maps */
static ADDRESS_MAP_START(bk0010_mem, AS_PROGRAM, 16, bk_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x0000, 0x3fff ) AM_RAM  // RAM
	AM_RANGE( 0x4000, 0x7fff ) AM_RAM  AM_SHARE("video_ram") // Video RAM
	AM_RANGE( 0x8000, 0x9fff ) AM_ROM  // ROM
	AM_RANGE( 0xa000, 0xbfff ) AM_ROM  // ROM
	AM_RANGE( 0xc000, 0xdfff ) AM_ROM  // ROM
	AM_RANGE( 0xe000, 0xfeff ) AM_ROM  // ROM
	AM_RANGE( 0xffb0, 0xffb1 ) AM_READWRITE(bk_key_state_r,bk_key_state_w)
	AM_RANGE( 0xffb2, 0xffb3 ) AM_READ(bk_key_code_r)
	AM_RANGE( 0xffb4, 0xffb5 ) AM_READWRITE(bk_vid_scrool_r,bk_vid_scrool_w)
	AM_RANGE( 0xffce, 0xffcf ) AM_READWRITE(bk_key_press_r,bk_key_press_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START(bk0010fd_mem, AS_PROGRAM, 16, bk_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x0000, 0x3fff ) AM_RAM  // RAM
	AM_RANGE( 0x4000, 0x7fff ) AM_RAM  AM_SHARE("video_ram") // Video RAM
	AM_RANGE( 0x8000, 0x9fff ) AM_ROM  // ROM
	AM_RANGE( 0xa000, 0xdfff ) AM_RAM  // RAM
	AM_RANGE( 0xe000, 0xfdff ) AM_ROM  // ROM
	AM_RANGE( 0xfe58, 0xfe59 ) AM_READWRITE(bk_floppy_cmd_r,bk_floppy_cmd_w)
	AM_RANGE( 0xfe5a, 0xfe5b ) AM_READWRITE(bk_floppy_data_r,bk_floppy_data_w)
	AM_RANGE( 0xffb0, 0xffb1 ) AM_READWRITE(bk_key_state_r,bk_key_state_w)
	AM_RANGE( 0xffb2, 0xffb3 ) AM_READ(bk_key_code_r)
	AM_RANGE( 0xffb4, 0xffb5 ) AM_READWRITE(bk_vid_scrool_r,bk_vid_scrool_w)
	AM_RANGE( 0xffce, 0xffcf ) AM_READWRITE(bk_key_press_r,bk_key_press_w)
ADDRESS_MAP_END

/* Input ports */
static INPUT_PORTS_START( bk0010 )
	PORT_START("LINE0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Ctrl") PORT_CODE(KEYCODE_LCONTROL) PORT_CODE(KEYCODE_RCONTROL)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Shift") PORT_CODE(KEYCODE_LSHIFT) PORT_CODE(KEYCODE_RSHIFT) PORT_CHAR(UCHAR_SHIFT_1)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("LINE1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_LEFT) PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_ENTER) PORT_CHAR(13)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Clear") PORT_CODE(KEYCODE_HOME)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Rus") PORT_CODE(KEYCODE_LALT)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Lat") PORT_CODE(KEYCODE_RALT)

	PORT_START("LINE2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Tab left") PORT_CODE(KEYCODE_PGUP)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Tab right") PORT_CODE(KEYCODE_PGDN)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("LINE3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Delete") PORT_CODE(KEYCODE_BACKSPACE) PORT_CHAR(8)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_RIGHT) PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_UP) PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_DOWN) PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("LINE4")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SPACE) PORT_CHAR(' ')
	PORT_BIT(0xFE, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("LINE5")
	PORT_BIT(0xFF, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("LINE6")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_0) PORT_CHAR('0')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHAR('!')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_2) PORT_CHAR('2') PORT_CHAR('\"')
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_3) PORT_CHAR('3') PORT_CHAR('#')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_4) PORT_CHAR('4') PORT_CHAR('\xA4')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_5) PORT_CHAR('5') PORT_CHAR('%')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_6) PORT_CHAR('6') PORT_CHAR('&')
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_7) PORT_CHAR('7') PORT_CHAR('\'')

	PORT_START("LINE7")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_8) PORT_CHAR('8') PORT_CHAR('(')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_9) PORT_CHAR('9') PORT_CHAR(')')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR(':') PORT_CHAR('*')
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COLON) PORT_CHAR(';') PORT_CHAR('+')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_COMMA) PORT_CHAR('<') PORT_CHAR(',')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_MINUS) PORT_CHAR('=') PORT_CHAR('-')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_STOP) PORT_CHAR('>') PORT_CHAR('.')
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_SLASH) PORT_CHAR('?') PORT_CHAR('/')

	PORT_START("LINE8")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("<>") PORT_CODE(KEYCODE_END)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_A) PORT_CHAR('A') PORT_CHAR('a')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_B) PORT_CHAR('B') PORT_CHAR('b')
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_C) PORT_CHAR('C') PORT_CHAR('c')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_D) PORT_CHAR('D') PORT_CHAR('d')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_E) PORT_CHAR('E') PORT_CHAR('e')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F) PORT_CHAR('F') PORT_CHAR('f')
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_G) PORT_CHAR('G') PORT_CHAR('g')

	PORT_START("LINE9")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_H) PORT_CHAR('H') PORT_CHAR('h')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_I) PORT_CHAR('I') PORT_CHAR('i')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_J) PORT_CHAR('J') PORT_CHAR('j')
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_K) PORT_CHAR('K') PORT_CHAR('k')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_L) PORT_CHAR('L') PORT_CHAR('l')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_M) PORT_CHAR('M') PORT_CHAR('m')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_N) PORT_CHAR('N') PORT_CHAR('n')
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_O) PORT_CHAR('O') PORT_CHAR('o')

	PORT_START("LINE10")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_P) PORT_CHAR('P') PORT_CHAR('p')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q') PORT_CHAR('q')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_R) PORT_CHAR('R') PORT_CHAR('r')
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_S) PORT_CHAR('S') PORT_CHAR('s')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_T) PORT_CHAR('T') PORT_CHAR('t')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_U) PORT_CHAR('U') PORT_CHAR('u')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_V) PORT_CHAR('V') PORT_CHAR('v')
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_W) PORT_CHAR('W') PORT_CHAR('w')

	PORT_START("LINE11")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_X) PORT_CHAR('X') PORT_CHAR('x')
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y') PORT_CHAR('y')
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z') PORT_CHAR('z')
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_OPENBRACE) PORT_CHAR('[') PORT_CHAR('{')
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\') PORT_CHAR('|')
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(']') PORT_CHAR('}')
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("~") PORT_CODE(KEYCODE_TILDE)    // this alone gives nothing and gives ` with Shift!
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("=") PORT_CODE(KEYCODE_EQUALS)   // this alone acts like Enter and gives no result with Shift
INPUT_PORTS_END

static const struct t11_setup t11_data =
{
	0x36ff          /* initial mode word has DAL15,14,11,8 pulled low */
};

/* Machine driver */
static const cassette_interface bk0010_cassette_interface =
{
	/*rk8_cassette_formats*/cassette_default_formats,
	NULL,
	(cassette_state)(CASSETTE_STOPPED | CASSETTE_SPEAKER_ENABLED | CASSETTE_MOTOR_ENABLED),
	"bk0010_cass",
	NULL
};


static MACHINE_CONFIG_START( bk0010, bk_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", T11, 3000000)
	MCFG_CPU_CONFIG(t11_data)
	MCFG_CPU_PROGRAM_MAP(bk0010_mem)


	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(512, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 512-1, 0, 256-1)
	MCFG_SCREEN_UPDATE_DRIVER(bk_state, screen_update_bk0010)

	MCFG_PALETTE_LENGTH(2)
	MCFG_PALETTE_INIT(black_and_white)


	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_WAVE_ADD(WAVE_TAG, CASSETTE_TAG)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)

	MCFG_CASSETTE_ADD( CASSETTE_TAG, bk0010_cassette_interface )
	MCFG_SOFTWARE_LIST_ADD("cass_list","bk0010")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( bk0010fd, bk0010 )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(bk0010fd_mem)
MACHINE_CONFIG_END


/* ROM definition */

ROM_START( bk0010 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "monit10.rom", 0x8000, 0x2000, CRC(26c6e8a0) SHA1(4e83a94ae5155bbea14d7331a5a8db82457bd5ae) )
	ROM_LOAD( "focal.rom",   0xa000, 0x2000, CRC(717149b7) SHA1(75df26f81ebd281bcb5c55ba81a7d97f31e388b2) )
	ROM_LOAD( "tests.rom",   0xe000, 0x1f80, CRC(91aecb4d) SHA1(6b14d552045194a3004bb6b795a2538110921519) )
ROM_END

ROM_START( bk001001 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "monit10.rom",   0x8000, 0x2000, CRC(26c6e8a0) SHA1(4e83a94ae5155bbea14d7331a5a8db82457bd5ae) )
	ROM_LOAD( "basic10-1.rom", 0xa000, 0x2000, CRC(5e3ff5da) SHA1(5ea4db1eaac87bd0ac96e52a608bc783709f5042) )
	ROM_LOAD( "basic10-2.rom", 0xc000, 0x2000, CRC(ea63863c) SHA1(acf068925e4052989b05dd5cf736a1dab5438011) )
	ROM_LOAD( "basic10-3.rom", 0xe000, 0x1f80, CRC(63f3df2e) SHA1(b5463f08e7c5f9f5aa31a2e7b6c1ed94fe029d65) )
ROM_END

ROM_START( bk0010fd )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "monit10.rom",  0x8000, 0x2000, CRC(26c6e8a0) SHA1(4e83a94ae5155bbea14d7331a5a8db82457bd5ae) )
	ROM_LOAD( "disk_327.rom", 0xe000, 0x1000, CRC(ed8a43ae) SHA1(28eefbb63047b26e4aec104aeeca74e2f9d0276c) )
ROM_END

ROM_START( bk0011m )
	ROM_REGION( 0x1A000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "bas11m_0.rom", 0x10000, 0x4000, CRC(E71F9339) SHA1(9d76f3eefd64e032c763fa1ebf9cd3d9bd22317a) )
	ROM_LOAD( "bas11m_1.rom", 0x14000, 0x2000, CRC(4C68FF59) SHA1(34fa37599f2f9eb607390ef2458a3c22d87f09a9) )
	ROM_LOAD( "b11m_ext.rom", 0x16000, 0x2000, CRC(B2E4C33C) SHA1(f087af69044432a1ef2431a72ac06946e32f2dd3) )
	ROM_LOAD( "b11m_bos.rom", 0x18000, 0x2000, CRC(8FFD5EFA) SHA1(7e9a30e38d7b78981999821640a68a201bb6df01) )
	ROM_LOAD( "disk_327.rom", 0x19000, 0x1000, CRC(ed8a43ae) SHA1(28eefbb63047b26e4aec104aeeca74e2f9d0276c) )
ROM_END

/* Driver */

/*    YEAR  NAME    PARENT  COMPAT  MACHINE     INPUT       INIT    COMPANY                  FULLNAME   FLAGS */
COMP( 1985, bk0010,     0,       0, bk0010,     bk0010, driver_device,  0,      "Elektronika",           "BK-0010",  0)
COMP( 1986, bk001001,   bk0010,  0, bk0010,     bk0010, driver_device,  0,      "Elektronika",           "BK-0010.01",   0)
COMP( 1986, bk0010fd,   bk0010,  0, bk0010fd,   bk0010, driver_device,  0,      "Elektronika",           "BK-0010 FDD",  GAME_NOT_WORKING)
COMP( 1986, bk0011m,    bk0010,  0, bk0010fd,   bk0010, driver_device,  0,      "Elektronika",           "BK-0011M",     GAME_NOT_WORKING)
