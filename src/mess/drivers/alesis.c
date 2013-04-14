/***************************************************************************

    Alesis HR-16 and SR-16 drum machines

    06/04/2010 Skeleton driver.

    http://www.vintagesynth.com/misc/hr16.php
    http://www.vintagesynth.com/misc/sr16.php

****************************************************************************/

#include "emu.h"
#include "includes/alesis.h"
#include "sr16.lh"


WRITE8_MEMBER( alesis_state::kb_matrix_w )
{
	m_kb_matrix = data;
}

READ8_MEMBER( alesis_state::kb_r )
{
	UINT8 data = 0xff;

	if (!(m_kb_matrix & 0x01))
		data &= ioport("COL1")->read();
	if (!(m_kb_matrix & 0x02))
		data &= ioport("COL2")->read();
	if (!(m_kb_matrix & 0x04))
		data &= ioport("COL3")->read();
	if (!(m_kb_matrix & 0x08))
		data &= ioport("COL4")->read();
	if (!(m_kb_matrix & 0x10))
		data &= ioport("COL5")->read();
	if (!(m_kb_matrix & 0x20))
		data &= ioport("COL6")->read();

	return data;
}

WRITE8_MEMBER( alesis_state::led_w )
{
	output_set_value("patt_led",  data & 0x01 ? 1 : 0);
	output_set_value("song_led",  data & 0x01 ? 0 : 1);
	output_set_value("play_led",  data & 0x02 ? 0 : 1);
	output_set_value("record_led",data & 0x04 ? 0 : 1);
	output_set_value("voice_led", data & 0x08 ? 0 : 1);
	output_set_value("tune_led",  data & 0x10 ? 0 : 1);
	output_set_value("mix_led",   data & 0x20 ? 0 : 1);
	output_set_value("tempo_led", data & 0x40 ? 0 : 1);
	output_set_value("midi_led",  data & 0x80 ? 0 : 1);
}

READ8_MEMBER( alesis_state::p3_r )
{
	UINT8 data = 0xff;

	data &= ~(m_cassette->input() > 0.01 ? 0x00 : 0x08);

	return data;
}

WRITE8_MEMBER( alesis_state::p3_w )
{
	m_cassette->output(data & 0x04 ? -1.0 : +1.0);
}

WRITE8_MEMBER( alesis_state::sr16_lcd_w )
{
	m_lcdc->write(space, BIT(m_kb_matrix,7), data);
}

static ADDRESS_MAP_START(hr16_mem, AS_PROGRAM, 8, alesis_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x7fff) AM_MIRROR(0x8000) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START(hr16_io, AS_IO, 8, alesis_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x0000) AM_READ(kb_r)
	AM_RANGE(0x0002, 0x0002) AM_DEVWRITE("dm3ag", alesis_dm3ag_device, write)
	AM_RANGE(0x0004, 0x0004) AM_WRITE(led_w)
	AM_RANGE(0x0006, 0x0007) AM_DEVREADWRITE("hd44780", hd44780_device, read, write)
	AM_RANGE(0x0008, 0x0008) AM_WRITE(kb_matrix_w)
	AM_RANGE(MCS51_PORT_P1, MCS51_PORT_P1) AM_READ_PORT("SELECT")   AM_WRITENOP
	AM_RANGE(MCS51_PORT_P2, MCS51_PORT_P2) AM_WRITENOP
	AM_RANGE(MCS51_PORT_P3, MCS51_PORT_P3) AM_READWRITE(p3_r, p3_w)
	AM_RANGE(0x8000, 0xffff) AM_RAM     AM_SHARE("nvram")   // 32Kx8 SRAM, (battery-backed)
ADDRESS_MAP_END

static ADDRESS_MAP_START(sr16_mem, AS_PROGRAM, 8, alesis_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0xffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START(sr16_io, AS_IO, 8, alesis_state)
	//ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x0000) AM_MIRROR(0xff) AM_DEVWRITE("dm3ag", alesis_dm3ag_device, write)
	AM_RANGE(0x0200, 0x0200) AM_MIRROR(0xff) AM_WRITE(sr16_lcd_w)
	AM_RANGE(0x0300, 0x0300) AM_MIRROR(0xff) AM_WRITE(kb_matrix_w)
	AM_RANGE(0x0400, 0x0400) AM_MIRROR(0xff) AM_READ(kb_r)
	AM_RANGE(MCS51_PORT_P1, MCS51_PORT_P1) AM_READNOP
	AM_RANGE(MCS51_PORT_P2, MCS51_PORT_P2) AM_WRITENOP
	AM_RANGE(MCS51_PORT_P3, MCS51_PORT_P3) AM_READWRITE(p3_r, p3_w)
	AM_RANGE(0x8000, 0xffff) AM_RAM     AM_SHARE("nvram")   // 32Kx8 SRAM, (battery-backed)
ADDRESS_MAP_END

/* Input ports */
static INPUT_PORTS_START( hr16 )
	PORT_START("COL1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("COPY")   PORT_CODE(KEYCODE_C)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("OFFSET") PORT_CODE(KEYCODE_O)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SWING")  PORT_CODE(KEYCODE_W)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("QUANT")  PORT_CODE(KEYCODE_Q)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("LENGTH") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PATT")   PORT_CODE(KEYCODE_P)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("MIDI/UTIL") PORT_CODE(KEYCODE_U)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TEMPO")  PORT_CODE(KEYCODE_T)
	PORT_START("COL2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("DELETE") PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("INSERT") PORT_CODE(KEYCODE_INSERT)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SONG")   PORT_CODE(KEYCODE_S)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1")      PORT_CODE(KEYCODE_1)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2")      PORT_CODE(KEYCODE_2)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3")      PORT_CODE(KEYCODE_3)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4")      PORT_CODE(KEYCODE_4)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5")      PORT_CODE(KEYCODE_5)
	PORT_START("COL3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("ERASE")  PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TAPE")   PORT_CODE(KEYCODE_Y)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("FILL")   PORT_CODE(KEYCODE_F)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6")      PORT_CODE(KEYCODE_6)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7")      PORT_CODE(KEYCODE_7)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8")      PORT_CODE(KEYCODE_8)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9")      PORT_CODE(KEYCODE_9)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0")      PORT_CODE(KEYCODE_0)
	PORT_START("COL4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PLAY")  PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("STOP/CONTINUE") PORT_CODE(KEYCODE_END)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("RECORD") PORT_CODE(KEYCODE_HOME)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("< -")    PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("> +")    PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("VOICE")  PORT_CODE(KEYCODE_V)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TUNE")   PORT_CODE(KEYCODE_H)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("MIX")    PORT_CODE(KEYCODE_M)
	PORT_START("COL5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TOM 1")  PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TOM 2")  PORT_CODE(KEYCODE_F2)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TOM 3")  PORT_CODE(KEYCODE_F3)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TOM 4")  PORT_CODE(KEYCODE_F4)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("RIDE")   PORT_CODE(KEYCODE_F5)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CRASH")  PORT_CODE(KEYCODE_F6)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PERC 1") PORT_CODE(KEYCODE_F7)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PERC 2") PORT_CODE(KEYCODE_F8)
	PORT_START("COL6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KICK")   PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SNARE")  PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CLOSED HAT") PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("MID HAT")    PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("OPEN HAT")   PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CLAPS")  PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PERC 3") PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PERC 4") PORT_CODE(KEYCODE_8_PAD)

	PORT_START("SELECT")
	PORT_BIT(0xff, 0x00, IPT_DIAL) PORT_NAME("SELECT Slider") PORT_SENSITIVITY(50) PORT_KEYDELTA(1) PORT_CODE_DEC(KEYCODE_DOWN) PORT_CODE_INC(KEYCODE_UP)
INPUT_PORTS_END

static INPUT_PORTS_START( sr16 )
	PORT_START("COL1")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PRESET/USER")  PORT_CODE(KEYCODE_U)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PATTERN/SONG") PORT_CODE(KEYCODE_P)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("1")      PORT_CODE(KEYCODE_1)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("2")      PORT_CODE(KEYCODE_2)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("3")      PORT_CODE(KEYCODE_3)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("4")      PORT_CODE(KEYCODE_4)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("5")      PORT_CODE(KEYCODE_5)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("UP")     PORT_CODE(KEYCODE_UP)
	PORT_START("COL2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("RECORD SETUP") PORT_CODE(KEYCODE_R)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("DRUM SET") PORT_CODE(KEYCODE_D)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("6")      PORT_CODE(KEYCODE_6)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("7")      PORT_CODE(KEYCODE_7)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("8")      PORT_CODE(KEYCODE_8)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("9")      PORT_CODE(KEYCODE_9)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("0")      PORT_CODE(KEYCODE_0)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("DOWN")   PORT_CODE(KEYCODE_DOWN)
	PORT_START("COL3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("BACKUP") PORT_CODE(KEYCODE_K)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("ERASE")  PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PLAY")   PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("STOP")   PORT_CODE(KEYCODE_END)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("A")      PORT_CODE(KEYCODE_A)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("B")      PORT_CODE(KEYCODE_B)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("FILL")   PORT_CODE(KEYCODE_F)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("COPY")   PORT_CODE(KEYCODE_C)
	PORT_START("COL4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("MIDI SETUP") PORT_CODE(KEYCODE_M)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TEMPO/PAGE UP") PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TOM 1")  PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TOM 2")  PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TOM 3")  PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("RIDE")   PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CRASH")  PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PERC 1") PORT_CODE(KEYCODE_6_PAD)
	PORT_START("COL5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PERFORM/COMPOSE") PORT_CODE(KEYCODE_HOME)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("TEMPO/PAGE DOWN") PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("KICK")   PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("SNARE")  PORT_CODE(KEYCODE_F2)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CLOSED HAT") PORT_CODE(KEYCODE_F3)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("OPEN HAT")   PORT_CODE(KEYCODE_F4)
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("CLAPS")  PORT_CODE(KEYCODE_F5)
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_KEYBOARD) PORT_NAME("PERC 2") PORT_CODE(KEYCODE_F6)
	PORT_START("COL6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END


void alesis_state::palette_init()
{
	palette_set_color(machine(), 0, MAKE_RGB(138, 146, 148));
	palette_set_color(machine(), 1, MAKE_RGB(92, 83, 88));
}

void alesis_state::machine_reset()
{
	m_kb_matrix = 0xff;
	memset(m_lcd_digits, 0, sizeof(m_lcd_digits));
}


static HD44780_PIXEL_UPDATE(sr16_pixel_update)
{
	alesis_state *driv_state = device.machine().driver_data<alesis_state>();

	if (line == 1 && pos >= 6 && pos < 8)  // last 2 characters of the second line are used to control the LCD symbols
		driv_state->update_lcd_symbols(bitmap, pos, y, x, state);
	else if (pos < 8)
		bitmap.pix16(line*9 + y, pos*6 + x) = state;
}

static const cassette_interface hr16_cassette_interface =
{
	cassette_default_formats,
	NULL,
	(cassette_state)(CASSETTE_STOPPED),
	"hr16_cass",
	NULL
};

static MACHINE_CONFIG_START( hr16, alesis_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",I8031, XTAL_12MHz)
	MCFG_CPU_PROGRAM_MAP(hr16_mem)
	MCFG_CPU_IO_MAP(hr16_io)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", LCD)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(6*16, 9*2)
	MCFG_SCREEN_VISIBLE_AREA(0, 6*16-1, 0, 9*2-1)
	MCFG_DEFAULT_LAYOUT(layout_lcd)
	MCFG_SCREEN_UPDATE_DEVICE("hd44780", hd44780_device, screen_update)

	MCFG_PALETTE_LENGTH(2)

	MCFG_CASSETTE_ADD( CASSETTE_TAG, hr16_cassette_interface )

	MCFG_HD44780_ADD("hd44780")
	MCFG_HD44780_LCD_SIZE(2, 16)

	/* sound hardware */
	MCFG_ALESIS_DM3AG_ADD("dm3ag", XTAL_12MHz/2)

	MCFG_NVRAM_ADD_0FILL("nvram")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( sr16, hr16 )
	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(sr16_mem)
	MCFG_CPU_IO_MAP(sr16_io)

	/* video hardware */
	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_SIZE(6*8, 9*2)
	MCFG_SCREEN_VISIBLE_AREA(0, 6*8-1, 0, 9*2-1)
	MCFG_DEFAULT_LAYOUT(layout_sr16)

	MCFG_PALETTE_INIT(black_and_white)

	MCFG_DEVICE_MODIFY("hd44780")
	MCFG_HD44780_LCD_SIZE(2, 8)
	MCFG_HD44780_PIXEL_UPDATE_CB(sr16_pixel_update)
MACHINE_CONFIG_END

/* ROM definition */
ROM_START( hr16 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )

	ROM_REGION( 0x10000, "user1", ROMREGION_ERASEFF )
	ROM_DEFAULT_BIOS("v109")
	ROM_SYSTEM_BIOS(0, "v106", "ver 1.06")
	ROMX_LOAD( "hr16-v1.06.bin",  0x0000, 0x8000, CRC(f0cdb899) SHA1(f21cd87af15ad5a0bfec992e38131c4f4e4c5102), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(1, "v107", "ver 1.07")
	ROMX_LOAD( "2-19-0256-v107.u11",  0x0000, 0x8000, CRC(2582b6a2) SHA1(f1f135335578c938be63b37ed207e82b7a0e13be), ROM_BIOS(2))
	ROM_SYSTEM_BIOS(2, "v109", "ver 1.09")
	ROMX_LOAD( "2-19-0256-v109.u11",  0x0000, 0x8000, CRC(a9bdbf20) SHA1(229b4230c7b5380efbfd42fa95645723d3fd6d55), ROM_BIOS(3))
	ROM_SYSTEM_BIOS(3, "v200", "ver 2.00")
	ROMX_LOAD( "hr16-v2.0.bin",  0x0000, 0x8000, CRC(a3fcba12) SHA1(4c94be7e94e5a1d86443571cd4d375158a6e7b65), ROM_BIOS(4))

	ROM_REGION( 0x100000, "dm3ag", 0 )
	ROM_LOAD( "2-27-0004.u16", 0x00000, 0x80000, CRC(8e103536) SHA1(092e1cf649fbef171cfaf91e20707d89998b7a1e))
	ROM_LOAD( "2-27-0003.u15", 0x80000, 0x80000, CRC(82e9b78c) SHA1(89728cb38ae172b5e347a03018617c94a087dce0))
ROM_END

ROM_START( hr16b )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )

	ROM_REGION( 0x10000, "user1", ROMREGION_ERASEFF )
	ROM_SYSTEM_BIOS(0, "v200", "ver 2.00")
	ROMX_LOAD( "2-19-0256-v200.u11",0x0000,  0x8000, CRC(19cf0fce) SHA1(f8b3786b32d68e3627a654b8b3916befbe9bc540), ROM_BIOS(1))

	ROM_REGION( 0x100000, "dm3ag", 0 )
	ROM_LOAD( "2-27-0008.u16", 0x00000, 0x80000, CRC(11ca930e) SHA1(2f57fdd02f9b2146a551370a74cab1fa800145ab))
	ROM_LOAD( "2-27-0007.u15", 0x80000, 0x80000, CRC(319746db) SHA1(46b32a3ab2fbad67fb4566f607f578a2e9defd63))
ROM_END

ROM_START( sr16 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_SYSTEM_BIOS(0, "v104", "ver 1.04")
	ROMX_LOAD( "sr16_v1_04.bin", 0x0000, 0x10000, CRC(d049af6e) SHA1(0bbeb4bd25e33a9eca64d5a31480f96a0040617e), ROM_BIOS(1))

	ROM_REGION( 0x100000, "dm3ag", ROMREGION_ERASEFF )
	ROM_LOAD( "sr16.u6", 0x00000, 0x80000, CRC(6da96987) SHA1(3ec8627d440bc73841e1408a19def09a8b0b77f7))
	ROM_LOAD( "sr16.u5", 0x80000, 0x80000, CRC(8bb25cfa) SHA1(273ad59d017b54a7e8d5e1ec61c8cd807a0e4af3))
ROM_END


DRIVER_INIT_MEMBER(alesis_state,hr16)
{
	int i;
	UINT8 *ROM = memregion("maincpu")->base();
	UINT8 *orig = memregion("user1")->base();
	for (i = 0; i < 0x8000; i++)
	{
		ROM[BITSWAP16(i,15,14,13,12,11,10,9,8,0,1,2,3,4,5,6,7)] = orig[i];
	}
}

/* Driver */
/*    YEAR  NAME    PARENT  COMPAT   MACHINE    INPUT    INIT    COMPANY    FULLNAME       FLAGS */
SYST( 1987, hr16,  0,       0,       hr16,      hr16, alesis_state,  hr16,   "Alesis",  "HR-16",       GAME_NOT_WORKING | GAME_NO_SOUND)
SYST( 1989, hr16b, hr16,    0,       hr16,      hr16, alesis_state,  hr16,   "Alesis",  "HR-16B",      GAME_NOT_WORKING | GAME_NO_SOUND)
SYST( 1990, sr16,  0,       0,       sr16,      sr16, driver_device,    0,   "Alesis",  "SR-16",       GAME_NOT_WORKING | GAME_NO_SOUND)
