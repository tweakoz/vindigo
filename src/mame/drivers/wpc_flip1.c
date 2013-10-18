
/* Williams WPC Fliptronics I */

#include "includes/wpc_pin.h"


static ADDRESS_MAP_START( wpc_flip1_map, AS_PROGRAM, 8, wpc_flip1_state )
	AM_RANGE(0x0000, 0x2fff) AM_READWRITE(ram_r,ram_w)
	AM_RANGE(0x3000, 0x31ff) AM_RAMBANK("dmdbank1")
	AM_RANGE(0x3200, 0x33ff) AM_RAMBANK("dmdbank2")
	AM_RANGE(0x3400, 0x35ff) AM_RAMBANK("dmdbank3")
	AM_RANGE(0x3600, 0x37ff) AM_RAMBANK("dmdbank4")
	AM_RANGE(0x3800, 0x39ff) AM_RAMBANK("dmdbank5")
	AM_RANGE(0x3a00, 0x3bff) AM_RAMBANK("dmdbank6")
	AM_RANGE(0x3c00, 0x3faf) AM_RAM
	AM_RANGE(0x3fb0, 0x3fff) AM_DEVREADWRITE("wpc",wpc_device,read,write) // WPC device
	AM_RANGE(0x4000, 0x7fff) AM_ROMBANK("cpubank")
	AM_RANGE(0x8000, 0xffff) AM_ROMBANK("fixedbank")
ADDRESS_MAP_END

static INPUT_PORTS_START( wpc_flip1 )
	PORT_START("INP0")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("INP1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER )  PORT_CODE(KEYCODE_LSHIFT)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER )  PORT_CODE(KEYCODE_RSHIFT)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER )  PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER )  PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER )  PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER )  PORT_CODE(KEYCODE_5_PAD)

	PORT_START("INP2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_A)  // slam tilt
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_CODE(KEYCODE_S) PORT_TOGGLE  // coin door
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_D)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  // always closed
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_G)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_H)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_J)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_K)

	PORT_START("INP4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_L)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Z)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_C)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_V)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_B)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_N)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_M)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_COMMA)

	PORT_START("INP8")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_STOP)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_SLASH)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_COLON)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_X)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_MINUS)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_EQUALS)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_BACKSPACE)

	PORT_START("INP10")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_BACKSLASH)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_ENTER)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_LEFT)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_UP)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_DOWN)

	PORT_START("INP20")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Q)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_W)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_E)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_R)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_Y)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_U)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_I)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_O)

	PORT_START("INP40")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_LALT)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_OTHER ) PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("INP80")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("COIN")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE ) PORT_NAME("Service / Escape") PORT_CODE(KEYCODE_DEL_PAD)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_VOLUME_DOWN ) PORT_CODE(KEYCODE_MINUS_PAD)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_VOLUME_UP ) PORT_CODE(KEYCODE_PLUS_PAD)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("Begin Test / Enter") PORT_CODE(KEYCODE_ENTER_PAD)

	PORT_START("DIPS")
	PORT_DIPNAME(0x01,0x01,"Switch 1") PORT_DIPLOCATION("SWA:1")
	PORT_DIPSETTING(0x00,DEF_STR( Off ))
	PORT_DIPSETTING(0x01,DEF_STR( On ))
	PORT_DIPNAME(0x02,0x02,"Switch 2") PORT_DIPLOCATION("SWA:2")
	PORT_DIPSETTING(0x00,DEF_STR( Off ))
	PORT_DIPSETTING(0x02,DEF_STR( On ))
	PORT_DIPNAME(0x04,0x00,"W20") PORT_DIPLOCATION("SWA:3")
	PORT_DIPSETTING(0x00,DEF_STR( Off ))
	PORT_DIPSETTING(0x04,DEF_STR( On ))
	PORT_DIPNAME(0x08,0x00,"W19") PORT_DIPLOCATION("SWA:4")
	PORT_DIPSETTING(0x00,DEF_STR( Off ))
	PORT_DIPSETTING(0x08,DEF_STR( On ))
	PORT_DIPNAME(0xf0,0x00,"Country") PORT_DIPLOCATION("SWA:5,6,7,8")
	PORT_DIPSETTING(0x00,"USA 1")
	PORT_DIPSETTING(0x10,"France 1")
	PORT_DIPSETTING(0x20,"Germany")
	PORT_DIPSETTING(0x30,"France 2")
	PORT_DIPSETTING(0x40,"Unknown 1")
	PORT_DIPSETTING(0x50,"Unknown 2")
	PORT_DIPSETTING(0x60,"Unknown 3")
	PORT_DIPSETTING(0x70,"Unknown 4")
	PORT_DIPSETTING(0x80,"Export 1")
	PORT_DIPSETTING(0x90,"France 3")
	PORT_DIPSETTING(0xa0,"Export 2")
	PORT_DIPSETTING(0xb0,"France 4")
	PORT_DIPSETTING(0xc0,"UK")
	PORT_DIPSETTING(0xd0,"Europe")
	PORT_DIPSETTING(0xe0,"Spain")
	PORT_DIPSETTING(0xf0,"USA 2")
INPUT_PORTS_END

DRIVER_INIT_MEMBER(wpc_flip1_state,wpc_flip1)
{
	wpc_dot_state::init_wpc_dot();
}

static MACHINE_CONFIG_START( wpc_flip1, wpc_flip1_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6809, 2000000)
	MCFG_CPU_PROGRAM_MAP(wpc_flip1_map)

	MCFG_WMS_WPC_ADD("wpc")
	MCFG_WPC_IRQ_ACKNOWLEDGE(WRITELINE(wpc_dot_state,wpc_irq_w))
	MCFG_WPC_FIRQ_ACKNOWLEDGE(WRITELINE(wpc_dot_state,wpc_firq_w))
	MCFG_WPC_ROMBANK(WRITE8(wpc_dot_state,wpc_rombank_w))
	MCFG_WPC_SOUND_CTRL(READ8(wpc_dot_state,wpc_sound_ctrl_r),WRITE8(wpc_dot_state,wpc_sound_ctrl_w))
	MCFG_WPC_SOUND_DATA(READ8(wpc_dot_state,wpc_sound_data_r),WRITE8(wpc_dot_state,wpc_sound_data_w))
	MCFG_WPC_DMDBANK(WRITE8(wpc_dot_state,wpc_dmdbank_w))

	MCFG_WMS_WPC_SOUND_ADD("wpcsnd",":sound1")
	MCFG_WPC_SOUND_REPLY_CALLBACK(WRITELINE(wpc_dot_state,wpcsnd_reply_w))

	MCFG_DEFAULT_LAYOUT(layout_lcd)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_SIZE(128, 32)
	MCFG_SCREEN_VISIBLE_AREA(0, 128-1, 0, 32-1)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_UPDATE_DRIVER(wpc_dot_state, screen_update)
MACHINE_CONFIG_END

/*-----------------
/  The Addams Family
/------------------*/
ROM_START(taf_p2)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "code", 0)
	ROM_LOAD("addam_p2.rom", 0x00000, 0x40000, CRC(eabf0e72) SHA1(5b84d0315702b39b90beb6a92fb7ad9aba7e620c))
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("afsnd_p2.rom", 0x100000, 0x80000, CRC(73d19698) SHA1(d14a6ea36a93db185a599a7810dfbef2deb0adc0))
ROM_END

ROM_START(taf_l1)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "code", 0)
	ROM_LOAD("addam_l1.rom", 0x00000, 0x40000, CRC(db287bf7) SHA1(51574c7c04d85aa816a0bc6e9db74f2d2b407525))
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("tafu18l1.rom", 0x100000, 0x80000, CRC(131ae471) SHA1(5ed03b521dfef56cbb99814539d4c74da4216f67))
ROM_END

ROM_START(taf_l2)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "code", 0)
	ROM_LOAD("addam_l2.rom", 0x00000, 0x40000, CRC(952bfc92) SHA1(d95b4b9e6c496a9ce4ceb1aa368c862b2beeffd9))
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("tafu18l1.rom", 0x100000, 0x80000, CRC(131ae471) SHA1(5ed03b521dfef56cbb99814539d4c74da4216f67))
ROM_END

ROM_START(taf_l3)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "code", 0)
	ROM_LOAD("addam_l3.rom", 0x00000, 0x40000, CRC(d428a760) SHA1(29afee7b1ae64d7a41faf813cdfa1ab7cef1f247))
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("tafu18l1.rom", 0x100000, 0x80000, CRC(131ae471) SHA1(5ed03b521dfef56cbb99814539d4c74da4216f67))
ROM_END

ROM_START(taf_l4)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x40000, "code", 0)
	ROM_LOAD("addam_l4.rom", 0x00000, 0x40000, CRC(ea29935f) SHA1(9f711396728026546c8bd1f69a0833d15e02c192))
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("tafu18l1.rom", 0x100000, 0x80000, CRC(131ae471) SHA1(5ed03b521dfef56cbb99814539d4c74da4216f67))
ROM_END

ROM_START(taf_l7)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x80000, "code", 0)
	ROM_LOAD("addam_l7.rom", 0x00000, 0x80000, CRC(4401b43a) SHA1(64e9678334cc900d1f44b95d25bb90c1fff566f8))
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("tafu18l1.rom", 0x100000, 0x80000, CRC(131ae471) SHA1(5ed03b521dfef56cbb99814539d4c74da4216f67))
ROM_END

ROM_START(taf_l5)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x80000, "code", 0)
	ROM_LOAD("addam_l5.rom", 0x00000, 0x80000, CRC(4c071564) SHA1(d643506db1b3ba1ea20f34ddb38837df379fb5ab))
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("tafu18l1.rom", 0x100000, 0x80000, CRC(131ae471) SHA1(5ed03b521dfef56cbb99814539d4c74da4216f67))
ROM_END

ROM_START(taf_l6)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x80000, "code", 0)
	ROM_LOAD("taf_l6.u6", 0x00000, 0x80000, CRC(06b37e65) SHA1(ce6f9cc45df08f50f5ece2a4c9376ecf67b0466a))
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("tafu18l1.rom", 0x100000, 0x80000, CRC(131ae471) SHA1(5ed03b521dfef56cbb99814539d4c74da4216f67))
ROM_END

ROM_START(taf_h4)
	ROM_REGION(0x10000, "maincpu", ROMREGION_ERASEFF)
	ROM_REGION(0x80000, "code", 0)
	ROM_LOAD("addam_h4.rom", 0x00000, 0x80000, CRC(d0bbd679) SHA1(ebd8c4981dd68a4f8e2dea90144486cb3cbd6b84))
	ROM_REGION(0x180000, "sound1", 0)
	ROM_LOAD("tafu18l1.rom", 0x100000, 0x80000, CRC(131ae471) SHA1(5ed03b521dfef56cbb99814539d4c74da4216f67))
ROM_END

/*--------------
/  Game drivers
/---------------*/
GAME(1992,  taf_l5,  0,       wpc_flip1,  wpc_flip1, wpc_flip1_state,  wpc_flip1,  ROT0,  "Bally",    "The Addams Family (L-5)",                    GAME_IS_SKELETON_MECHANICAL)
GAME(1992,  taf_p2,  taf_l5,  wpc_flip1,  wpc_flip1, wpc_flip1_state,  wpc_flip1,  ROT0,  "Bally",    "The Addams Family (Prototype) (P-2)",        GAME_IS_SKELETON_MECHANICAL)
GAME(1992,  taf_l1,  taf_l5,  wpc_flip1,  wpc_flip1, wpc_flip1_state,  wpc_flip1,  ROT0,  "Bally",    "The Addams Family (L-1)",                    GAME_IS_SKELETON_MECHANICAL)
GAME(1992,  taf_l2,  taf_l5,  wpc_flip1,  wpc_flip1, wpc_flip1_state,  wpc_flip1,  ROT0,  "Bally",    "The Addams Family (L-2)",                    GAME_IS_SKELETON_MECHANICAL)
GAME(1992,  taf_l3,  taf_l5,  wpc_flip1,  wpc_flip1, wpc_flip1_state,  wpc_flip1,  ROT0,  "Bally",    "The Addams Family (L-3)",                    GAME_IS_SKELETON_MECHANICAL)
GAME(1992,  taf_l4,  taf_l5,  wpc_flip1,  wpc_flip1, wpc_flip1_state,  wpc_flip1,  ROT0,  "Bally",    "The Addams Family (L-4)",                    GAME_IS_SKELETON_MECHANICAL)
GAME(1992,  taf_l7,  taf_l5,  wpc_flip1,  wpc_flip1, wpc_flip1_state,  wpc_flip1,  ROT0,  "Bally",    "The Addams Family (Prototype L-5) (L-7)",    GAME_IS_SKELETON_MECHANICAL)
GAME(1992,  taf_l6,  taf_l5,  wpc_flip1,  wpc_flip1, wpc_flip1_state,  wpc_flip1,  ROT0,  "Bally",    "The Addams Family (L-6)",                    GAME_IS_SKELETON_MECHANICAL)
GAME(1992,  taf_h4,  taf_l5,  wpc_flip1,  wpc_flip1, wpc_flip1_state,  wpc_flip1,  ROT0,  "Bally",    "The Addams Family (H-4)",                    GAME_IS_SKELETON_MECHANICAL)
