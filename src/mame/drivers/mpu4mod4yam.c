/* This is MPU4 MOD4 with a Y2413 instead of an OKI */

#include "emu.h"
#include "includes/mpu4.h"

MACHINE_CONFIG_EXTERN( mod4yam );
INPUT_PORTS_EXTERN( mpu4 );

#define GAME_FLAGS (GAME_NOT_WORKING|GAME_REQUIRES_ARTWORK)

#include "gamball.lh"


DRIVER_INIT_MEMBER( mpu4_state, m4_debug_mod4yam )
{
	// many original barcrest / bwb sets have identification info around here
	// this helps with sorting
	UINT8 *src = machine().root_device().memregion( "maincpu" )->base();
	int size = machine().root_device().memregion( "maincpu" )->bytes();
	
	for (int j=0;j<size;j+=0x10000)
	{
		if (size>0x10000) printf("\nblock 0x%06x:\n",j);
		printf("\ncopyright string:\n");
		for (int i = 0xffe0; i<0xfff0; i++)
		{
			printf("%c", src[j+i]);
		}
		printf("\n\nidentification string:\n");
		for (int i = 0xff28; i<0xff30; i++)
		{
			printf("%c", src[j+i]);
		}
		printf("\n");
	}
}

DRIVER_INIT_MEMBER( mpu4_state, m4_showstring_mod4yam )
{
	DRIVER_INIT_CALL( m4_debug_mod4yam );
	DRIVER_INIT_CALL( m4debug );
}



INPUT_PORTS_START( m4gambal )
	PORT_START("ORANGE1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("00")//  20p level
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("01")// 100p level
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("02")// Token 1 level
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("03")// Token 2 level
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("04")
	PORT_CONFNAME( 0xE0, 0x00, "Stake Key" )
	PORT_CONFSETTING(    0x00, "Not fitted / 5p"  )
	PORT_CONFSETTING(    0x20, "10p" )
	PORT_CONFSETTING(    0x40, "20p" )
	PORT_CONFSETTING(    0x60, "25p" )
	PORT_CONFSETTING(    0x80, "30p" )
	PORT_CONFSETTING(    0xA0, "40p" )
	PORT_CONFSETTING(    0xC0, "50p" )
	PORT_CONFSETTING(    0xE0, "1 GBP" )

	PORT_START("ORANGE2")
	PORT_CONFNAME( 0x0F, 0x00, "Jackpot / Prize Key" )
	PORT_CONFSETTING(    0x00, "Not fitted"  )
	PORT_CONFSETTING(    0x01, "3 GBP"  )
	PORT_CONFSETTING(    0x02, "4 GBP"  )
	PORT_CONFSETTING(    0x08, "5 GBP"  )
	PORT_CONFSETTING(    0x03, "6 GBP"  )
	PORT_CONFSETTING(    0x04, "6 GBP Token"  )
	PORT_CONFSETTING(    0x05, "8 GBP"  )
	PORT_CONFSETTING(    0x06, "8 GBP Token"  )
	PORT_CONFSETTING(    0x07, "10 GBP"  )
	PORT_CONFSETTING(    0x09, "15 GBP"  )
	PORT_CONFSETTING(    0x0A, "25 GBP"  )
	PORT_CONFSETTING(    0x0B, "25 GBP (Licensed Betting Office Profile)"  )
	PORT_CONFSETTING(    0x0C, "35 GBP"  )
	PORT_CONFSETTING(    0x0D, "70 GBP"  )
	PORT_CONFSETTING(    0x0E, "Reserved"  )
	PORT_CONFSETTING(    0x0F, "Reserved"  )

	PORT_CONFNAME( 0xF0, 0x00, "Percentage Key" )
	PORT_CONFSETTING(    0x00, "As Option Switches"  )
	PORT_CONFSETTING(    0x10, "70" )
	PORT_CONFSETTING(    0x20, "72" )
	PORT_CONFSETTING(    0x30, "74" )
	PORT_CONFSETTING(    0x40, "76" )
	PORT_CONFSETTING(    0x50, "78" )
	PORT_CONFSETTING(    0x60, "80" )
	PORT_CONFSETTING(    0x70, "82" )
	PORT_CONFSETTING(    0x80, "84" )
	PORT_CONFSETTING(    0x90, "86" )
	PORT_CONFSETTING(    0xA0, "88" )
	PORT_CONFSETTING(    0xB0, "90" )
	PORT_CONFSETTING(    0xC0, "92" )
	PORT_CONFSETTING(    0xD0, "94" )
	PORT_CONFSETTING(    0xE0, "96" )
	PORT_CONFSETTING(    0xF0, "98" )

	PORT_START("BLACK1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_NAME("Hi")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_BUTTON2) PORT_NAME("Lo")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("18")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("19")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("20")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_SERVICE) PORT_NAME("Test Button") PORT_CODE(KEYCODE_W)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_SERVICE) PORT_NAME("Refill Key") PORT_CODE(KEYCODE_R) PORT_TOGGLE
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_INTERLOCK) PORT_NAME("Cashbox Door")  PORT_CODE(KEYCODE_Q) PORT_TOGGLE

	PORT_START("BLACK2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("24")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_OTHER) PORT_NAME("25")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_BUTTON3) PORT_NAME("Cancel/Collect")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_BUTTON4) PORT_NAME("Hold/Nudge 1")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_BUTTON5) PORT_NAME("Hold/Nudge 2")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_BUTTON6) PORT_NAME("Hold/Nudge 3")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_BUTTON7) PORT_NAME("Hold/Nudge 4")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_START1)

	PORT_START("DIL1")
	PORT_DIPNAME( 0x80, 0x00, "DIL101" ) PORT_DIPLOCATION("DIL1:01")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On  ) )
	PORT_DIPNAME( 0x40, 0x00, "DIL102" ) PORT_DIPLOCATION("DIL1:02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On  ) )
	PORT_DIPNAME( 0x20, 0x00, "DIL103" ) PORT_DIPLOCATION("DIL1:03")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL104" ) PORT_DIPLOCATION("DIL1:04")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL105" ) PORT_DIPLOCATION("DIL1:05")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL106" ) PORT_DIPLOCATION("DIL1:06")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL107" ) PORT_DIPLOCATION("DIL1:07")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x01, 0x00, "DIL108" ) PORT_DIPLOCATION("DIL1:08")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )

	PORT_START("DIL2")
	PORT_DIPNAME( 0x80, 0x00, "DIL201" ) PORT_DIPLOCATION("DIL2:01")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On  ) )
	PORT_DIPNAME( 0x40, 0x00, "DIL202" ) PORT_DIPLOCATION("DIL2:02")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On  ) )
	PORT_DIPNAME( 0x20, 0x00, "DIL203" ) PORT_DIPLOCATION("DIL2:03")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On  ) )
	PORT_DIPNAME( 0x10, 0x00, "DIL204" ) PORT_DIPLOCATION("DIL2:04")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On  ) )
	PORT_DIPNAME( 0x08, 0x00, "DIL205" ) PORT_DIPLOCATION("DIL2:05")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On  ) )
	PORT_DIPNAME( 0x04, 0x00, "DIL206" ) PORT_DIPLOCATION("DIL2:06")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On  ) )
	PORT_DIPNAME( 0x02, 0x00, "DIL207" ) PORT_DIPLOCATION("DIL2:07")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On  ) )
	PORT_DIPNAME( 0x01, 0x00, "DIL208" ) PORT_DIPLOCATION("DIL2:08")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On  ) )

	PORT_START("AUX1")
	PORT_BIT(0xFF, IP_ACTIVE_HIGH, IPT_SPECIAL)//Handled by Gamball unit

	PORT_START("AUX2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_SPECIAL)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_SPECIAL)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_SPECIAL)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_SPECIAL)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_COIN1) PORT_NAME("10p")PORT_IMPULSE(5)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_COIN2) PORT_NAME("20p")PORT_IMPULSE(5)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_COIN3) PORT_NAME("50p")PORT_IMPULSE(5)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_COIN4) PORT_NAME("100p")PORT_IMPULSE(5)
INPUT_PORTS_END

ROM_START( m4cojok )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cojx.p1", 0x0000, 0x010000, CRC(a9c0aefb) SHA1(c5b367a01ddee2cb90e266f1e62459b9b96eb3e3) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "cojxb.p1", 0x0000, 0x010000, CRC(2680c84a) SHA1(6cf9bb72df41ea1389334597a772fd197aba4fc4) )
	ROM_LOAD( "cojxc.p1", 0x0000, 0x010000, CRC(a67db981) SHA1(08ac65baf774c63705c3a4db36248777375404f6) )
	ROM_LOAD( "cojxcd.p1", 0x0000, 0x010000, CRC(33d31701) SHA1(a7ccaa5a3b1c97cc84cdca2f77381ea4a8d743a3) )
	ROM_LOAD( "cojxd.p1", 0x0000, 0x010000, CRC(97c12c95) SHA1(282dfc5bc66fd4ad57f442c3ae75f6645919352d) )
	ROM_LOAD( "cojxdy.p1", 0x0000, 0x010000, CRC(4f0be63b) SHA1(d701b5c2d2c71942f8574598a4ba687f532c16a8) )
	ROM_LOAD( "cojxy.p1", 0x0000, 0x010000, CRC(88f1b57a) SHA1(cfc98d6ec90e7c186741d62d3ec68bd350196878) )
ROM_END


ROM_START( m4gambal )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "gbbx.p1",	0x0000, 0x10000,  CRC(0b5adcd0) SHA1(1a198bd4a1e7d6bf4cf025c43d35aaef351415fc))

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "gabcx.p1", 0x0000, 0x010000, CRC(52c35266) SHA1(bda49005de88094fbc84621f63b33f0e0a9c0bd3) )
	ROM_LOAD( "gabx.p1", 0x0000, 0x010000, CRC(74a8ed7e) SHA1(7363031c8a634ac13de957c62f32611963f797bd) )
	ROM_LOAD( "gbll20-6", 0x0000, 0x010000, CRC(f34d233a) SHA1(3f13563b2821b2f36267470c36ba346879521bc9) )
ROM_END

ROM_START( m4graff )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "graffo6", 0x0000, 0x010000, CRC(7349c9ca) SHA1(2744035d6c7897394c8fead27f48779047590fba) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "grax.p1", 0x0000, 0x010000, CRC(2e03a7d8) SHA1(333373fe15ae165dd24d5c11fef23f2e9b0388bf) )
	ROM_LOAD( "graxc.p1", 0x0000, 0x010000, CRC(7620657b) SHA1(2aec38ee0f826c7bb012522fd098a6fdb857c9da) )
	ROM_LOAD( "grfi20o6", 0x0000, 0x010000, CRC(7349c9ca) SHA1(2744035d6c7897394c8fead27f48779047590fba) )
ROM_END

ROM_START( m4graffd )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "grafittirom.bin", 0x0000, 0x010000, CRC(36135d6e) SHA1(e71eedabae36971739f8a6fd56a4a954de29944b) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	// these are just trash I think, only a few bits used, and if it's ym based it shouldn't have them anyway.
	ROM_LOAD( "grafittisound.bin", 0x0000, 0x010000, CRC(2d9bfff9) SHA1(ded774bcd2a7e10e4e2fc7b40976c9dcea0de6e3) )
	ROM_LOAD( "grafittisound2.bin", 0x0000, 0x010000, CRC(c06c5517) SHA1(9e11f93638d37ba6f7b34a78eea043821ca4e188) )
ROM_END


ROM_START( m4monte )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "nm8b.p1", 0x0000, 0x010000, CRC(1632080e) SHA1(9ca2cd8f00e49c29f4a216d3c9eacba221ada6ce) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "nm8ad.p1", 0x0000, 0x010000, CRC(92a07e05) SHA1(94015b219fffb8ad9a40a804a4e0b0fad61cdf21) )
	ROM_LOAD( "nm8bd.p1", 0x0000, 0x010000, CRC(a4bc134f) SHA1(72af6b66a5ea7566289bd9bdf8975c29dbb547cf) )
	ROM_LOAD( "nm8c.p1", 0x0000, 0x010000, CRC(7e558a64) SHA1(9f325aa9a5b036c317686b901b4c65c1e23fd845) )
	ROM_LOAD( "nm8d.p1", 0x0000, 0x010000, CRC(66716e7d) SHA1(719d32a3486accfa1c2e8e2ca53c05f916927e7a) )
	ROM_LOAD( "nm8dk.p1", 0x0000, 0x010000, CRC(ae4866e8) SHA1(5ec210b6b69f72b85abe5844b800b251fef20fc5) )
	ROM_LOAD( "nm8dy.p1", 0x0000, 0x010000, CRC(9d6f71a5) SHA1(577d39eef82761fff30f851282cd85b84ac22953) )
	ROM_LOAD( "nm8k.p1", 0x0000, 0x010000, CRC(47c00612) SHA1(647216e7489043f90e0cd807ddc3d631842b3f7f) )
	ROM_LOAD( "nm8s.p1", 0x0000, 0x010000, CRC(cf8fd333) SHA1(4b2b98d0c3d043a6425a6d82f7a98cf662582832) )
	ROM_LOAD( "nm8y.p1", 0x0000, 0x010000, CRC(cbb96053) SHA1(9fb6c449d8e26ecacfa9ba40979134c705ecb1be) )
	ROM_LOAD( "nmnc.p1", 0x0000, 0x010000, CRC(c2fdcc91) SHA1(aa3ec11425adee94c24b3a1472541e7e04e4000a) )
	ROM_LOAD( "nmnd.p1", 0x0000, 0x010000, CRC(94985809) SHA1(636b9106ea330a238f3d4168636fbf21021a7216) )
	ROM_LOAD( "nmnk.p1", 0x0000, 0x010000, CRC(8d022ae6) SHA1(01e12acbed34a2d4fb81dc9da12441ddc31f605b) )
	ROM_LOAD( "nmns.p1", 0x0000, 0x010000, CRC(48e2ab70) SHA1(bc452a36374a6e62516aad1a4887876ee9da37f7) )
ROM_END

ROM_START( m4nudbnk )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "nb6", 0x0000, 0x010000, CRC(010dd3fc) SHA1(645cbe54200a6c3327e10909b1ef3a80579e96e5) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "nbncx.p1", 0x0000, 0x010000, CRC(57bbbedf) SHA1(d42d3176f41aedf2ddc15cdf73ab97e963b92213) )
	ROM_LOAD( "nbnx.p1", 0x0000, 0x010000, CRC(075053d5) SHA1(43b9f6bb3a4ab531eb168007ceaf713261736144) )
	ROM_LOAD( "sbns.p1", 0x0000, 0x010000, CRC(92aa5b8d) SHA1(4f6e309e152266b8f40077a7d734b2b9042570d2) )
	ROM_LOAD( "sbnx.p1", 0x0000, 0x010000, CRC(861cbc50) SHA1(61166ea9092e2890ea9de421cc031d3a79335233) )
ROM_END

ROM_START( m4przmc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "mssb.p1", 0x0000, 0x010000, CRC(5210dae0) SHA1(cc9916718249505e031ccdbc126f3fa1e6675f27) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "mssad.p1", 0x0000, 0x010000, CRC(e3690c35) SHA1(fdaacda0d03ce8d54841525feff2529b1ee1f970) )
	ROM_LOAD( "mssd.p1", 0x0000, 0x010000, CRC(cf59305e) SHA1(7ba6f37aa1077561129f66ab663730fb6e5108ed) )
	ROM_LOAD( "mssdy.p1", 0x0000, 0x010000, CRC(12d7db63) SHA1(6e1e6b13783888f3d508d7cbecc52c65ffc99fb0) )
	ROM_LOAD( "mssk.p1", 0x0000, 0x010000, CRC(d56f62dc) SHA1(7df1fad20901607e710e8a7f64033f77d613a0fa) )
	ROM_LOAD( "msss.p1", 0x0000, 0x010000, CRC(c854c12f) SHA1(917d091383b07a995dc2c441717885b181a02d3c) )
	ROM_LOAD( "mssy.p1", 0x0000, 0x010000, CRC(159f4baa) SHA1(073c13e6bff4a641b29e5a45f88e3533aff460e4) )
ROM_END


ROM_START( m4przsss )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ps302b.p1", 0x0000, 0x010000, CRC(1749ae18) SHA1(f04f91a1d534f2d2dc844862bb21160c5903d1df) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "ps302ad.p1", 0x0000, 0x010000, CRC(e57f52d7) SHA1(25384517b68c488acd38956aeb69dda26d63c3ca) )
	ROM_LOAD( "ps302bd.p1", 0x0000, 0x010000, CRC(d3633f9d) SHA1(2500425d736a5c45f5bf40a7660b549f822266dc) )
	ROM_LOAD( "ps302d.p1", 0x0000, 0x010000, CRC(df1bfe3b) SHA1(a82574ff9eb04deccfbb8907ca8936b53f691b2c) )
	ROM_LOAD( "ps302dk.p1", 0x0000, 0x010000, CRC(88b49246) SHA1(122384d6c350e28fdbb3e2a02e5db7076ec4bb43) )
	ROM_LOAD( "ps302dy.p1", 0x0000, 0x010000, CRC(ada3ab8c) SHA1(421aaf0951cb1d47b7138ca611d2ebd6caf24a61) )
	ROM_LOAD( "ps302k.p1", 0x0000, 0x010000, CRC(23719bee) SHA1(13b7fd4f9edc60727e37078f6f2e24a63abd09f1) )
	ROM_LOAD( "ps302s.p1", 0x0000, 0x010000, CRC(4521c521) SHA1(90b5e444829ecc9a9b3e46f942830d263fbf02d3) )
	ROM_LOAD( "ps302y.p1", 0x0000, 0x010000, CRC(2ffed329) SHA1(a917161a7ea8312ef6a4a9a85f36f3b0a42b3a0c) )
	ROM_LOAD( "ps8ad.p1", 0x0000, 0x010000, CRC(48917a87) SHA1(d32ac9e30ebddb6ca1d6a7d6c38026338c6df2cd) )
	ROM_LOAD( "ps8b.p1", 0x0000, 0x010000, CRC(7633226d) SHA1(581dfb56719682a744fe2b4f63bd1c20eb943903) )
	ROM_LOAD( "ps8bd.p1", 0x0000, 0x010000, CRC(92e384db) SHA1(ab1c2c7aebb9c8c0cff6dd43d74551c15de0c805) )
	ROM_LOAD( "ps8d.p1", 0x0000, 0x010000, CRC(4b8a1374) SHA1(112fc0f0d1311482d292704ab807e15024b37cb9) )
	ROM_LOAD( "ps8dj.p1", 0x0000, 0x010000, CRC(9949fe88) SHA1(8ba8fd30bb12e47b97ddb9f4aba1eac880e5a12e) )
	ROM_LOAD( "ps8dk.p1", 0x0000, 0x010000, CRC(61e56c80) SHA1(93ef6601397063f412b35cbe90a5f7ecb3af2491) )
	ROM_LOAD( "ps8dy.p1", 0x0000, 0x010000, CRC(d4080a4a) SHA1(9907fea71237742595e5acd583c190a6180b4af9) )
	ROM_LOAD( "ps8j.p1", 0x0000, 0x010000, CRC(a9dcd1a8) SHA1(ec840aace95cab8c626a54636b47058401ef1eed) )
	ROM_LOAD( "ps8k.p1", 0x0000, 0x010000, CRC(7ed46dac) SHA1(481556298696d7f73d834034d0ce8628eb95b76c) )
	ROM_LOAD( "ps8s.p1", 0x0000, 0x010000, CRC(820a600a) SHA1(48701e315a94f92048ceb2e98df2bac1f04415e1) )
	ROM_LOAD( "ps8y.p1", 0x0000, 0x010000, CRC(a4d6934b) SHA1(215ed246f37daf1f8cdd0113b7b87e89c1aa2514) )
	ROM_LOAD( "sspb.p1", 0x0000, 0x010000, CRC(a781cdb8) SHA1(cbb1b9a85a80db7c91752349546bf55df4aea3f2) )
	ROM_LOAD( "sspd.p1", 0x0000, 0x010000, CRC(bcce54d7) SHA1(00a967188ddf1588331cda60e2589f6635e0a7ea) )
	ROM_LOAD( "sspdb.p1", 0x0000, 0x010000, CRC(edb5961e) SHA1(e1127d34148f04f9e34074269ee3740269105c63) )
	ROM_LOAD( "sspdy.p1", 0x0000, 0x010000, CRC(a368812e) SHA1(f377f13b866196fdbba07529f25713f9b5b91df5) )
	ROM_LOAD( "sspr.p1", 0x0000, 0x010000, CRC(720bad67) SHA1(3ee25abfc15e1c36a3ac6ac94e5229f938a39991) )
	ROM_LOAD( "ssprd.p1", 0x0000, 0x010000, CRC(b2ec7b80) SHA1(b562fbf2501dbaf0ec7c66d993df867384e750ff) )
	ROM_LOAD( "ssps.p1", 0x0000, 0x010000, CRC(e36f4d48) SHA1(fb88e8bcddb7dd2722b203a0ebb3a64c6b75ff24) )
	ROM_LOAD( "sspy.p1", 0x0000, 0x010000, CRC(0ea8f052) SHA1(3134ff47e6c5c4d200ffcdf0a5a3cb7b05b0fc2c) )
ROM_END


ROM_START( m4sss )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "spend6 10m.bin", 0x0000, 0x010000, CRC(a7519725) SHA1(cdab0ae00b865291ff7389122d174ef2e2676c6e) )

	ROM_REGION( 0x10000, "altbwb", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "sp_05a__.1o3", 0x0000, 0x010000, CRC(044a0133) SHA1(7cf85cf19f5c3f588daf5c0d7efe4204d67161a2) )
	ROM_LOAD( "sp_05s__.1o3", 0x0000, 0x010000, CRC(2e000a62) SHA1(e60390a383388e385bbde79ca14c63e5d69a8869) )
	ROM_LOAD( "sp_05sb_.1o3", 0x0000, 0x010000, CRC(c6380ef5) SHA1(673044aae9998dfe52205a5e4a3d26361f01c518) )
	ROM_LOAD( "sp_05sd_.1o3", 0x0000, 0x010000, CRC(31f818e1) SHA1(bbfa45ef63a73aa726a8223be234fb8ffba45e24) )
	ROM_LOAD( "sp_10a__.1o3", 0x0000, 0x010000, CRC(918c038c) SHA1(608062dc4e39c15967e16d95945b65ef7feabea2) )
	ROM_LOAD( "sp_10s__.1o3", 0x0000, 0x010000, CRC(1bc5780a) SHA1(df1b5d0d6f4751a480aef77be40fb2cfd153bf18) )
	ROM_LOAD( "sp_10sb_.1o3", 0x0000, 0x010000, CRC(2dfc3926) SHA1(b6b201c65c182f9b18a590910183ce88b245af2b) )
	ROM_LOAD( "sp_10sd_.1o3", 0x0000, 0x010000, CRC(fe5c7e3e) SHA1(f5066f1f0c2220da874cbac0ce510cbac6fff8e7) )
	ROM_LOAD( "sx_05a__.2_1", 0x0000, 0x010000, CRC(ceb830a1) SHA1(c9bef44d64a64872460ae3c450533fd14c92ca43) )
	ROM_LOAD( "sx_10a__.2_1", 0x0000, 0x010000, CRC(73e3bc13) SHA1(004097cc9cd62b8fa4c584fcb9874cf998c7b89d) )
	ROM_LOAD( "sxi05___.2_1", 0x0000, 0x010000, CRC(a804a20b) SHA1(477d2a750c0c252ffa215c3cf89916cb3a296b92) )
	ROM_LOAD( "sxi10___.2_1", 0x0000, 0x010000, CRC(bbb23438) SHA1(2cc4376f6393c69c1e18ad06be18933592b6bdae) )
ROM_END



ROM_START( m4ra )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "r2tx.p1", 0x0000, 0x010000, CRC(7efffe3d) SHA1(5472bc76f4450726fc49fce281a6ec69693d0923) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "r2txr.p1", 0x0000, 0x010000, CRC(9ff95e34) SHA1(79d19602b88e1c9d23e910332a968e6b820a39f5) )
	ROM_LOAD( "ra2s.p1", 0x0000, 0x010000, CRC(cd0fd068) SHA1(a347372f7f737ca87f44e692015338831465f123) )
	ROM_LOAD( "ra2x.p1", 0x0000, 0x010000, CRC(8217e235) SHA1(e17483afea2a9d9e70e88687f899e1b98b982b63) )
	ROM_LOAD( "ra2xa.p1", 0x0000, 0x010000, CRC(0e6b2123) SHA1(af7c5ddddbfffef6fa5746a7b7927845457d02f8) )
	ROM_LOAD( "ra2xb.p1", 0x0000, 0x010000, CRC(97fe4933) SHA1(201860b64577828547adb8a216a6a205c4a4f34b) )
	ROM_LOAD( "ra2xr.p1", 0x0000, 0x010000, CRC(12e8eb9b) SHA1(2bcd2c911626a2cb2419f9540649e99d7f335b3b) )
	ROM_LOAD( "ra3xad.p1", 0x0000, 0x010000, CRC(75957d43) SHA1(f7d00842b8390f5464733a6fe1d61d7431a16817) )
	ROM_LOAD( "ra3xb.p1", 0x0000, 0x010000, CRC(f37e9bd5) SHA1(584a1f6f1bfb35de813466448e35fc1251fa90bc) )
	ROM_LOAD( "ra3xbd.p1", 0x0000, 0x010000, CRC(43891009) SHA1(5d9ebe9d48a39f0a121ae7b832b277910bfd0ad6) )
	ROM_LOAD( "ra3xd.p1", 0x0000, 0x010000, CRC(bc59a07a) SHA1(3a8fc99690759ea376660feaf65bfda5386dcf0d) )
	ROM_LOAD( "ra3xdr.p1", 0x0000, 0x010000, CRC(036950ba) SHA1(f0a534352b41c2762330762c3c7024d9a6d49cd4) )
	ROM_LOAD( "ra3xdy.p1", 0x0000, 0x010000, CRC(468508d4) SHA1(ba6db1e1f7bca13b9c40173fb68418f319e2a9d8) )
	ROM_LOAD( "ra3xr.p1", 0x0000, 0x010000, CRC(1a2b813d) SHA1(5d3b5d4ab31dd1848b3d0b2a5ff5798cc01e0c6f) )
	ROM_LOAD( "ra3xs.p1", 0x0000, 0x010000, CRC(a1ba9673) SHA1(7d5441522e8676805f7e75a3d445acae83d8a03b) )
	ROM_LOAD( "ra3xy.p1", 0x0000, 0x010000, CRC(3e2287de) SHA1(ba0861a0bfb6eb76f9786c0a4c098db362117618) )
	ROM_LOAD( "rahx.p1", 0x0000, 0x010000, CRC(6887014e) SHA1(25e4c008588a219895c1b326314fd11e1f0ad35f) )
	ROM_LOAD( "reda_20_.8", 0x0000, 0x010000, CRC(915aff5b) SHA1(e8e58c263e2bdb64a80e9355ac5e114fff1d59f8) )
	ROM_LOAD( "redx_20_.8", 0x0000, 0x010000, CRC(b5e8dec5) SHA1(74777ed7f78ef7cc615beadf097380569832a75a) )
ROM_END

ROM_START( m4sayno )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "snms.p1", 0x0000, 0x010000, CRC(be1f2222) SHA1(7d8319796e1d45a3d0246bf13b6d818f20796db3) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "snm 5p.bin", 0x0000, 0x010000, CRC(4fba5c0a) SHA1(85438c531d4122bc31f59127a577dc6d71a4ba9d) )
	ROM_LOAD( "say no more 425b.bin", 0x0000, 0x010000, CRC(2cf27394) SHA1(fb7688b7d9d2e68f0c84a57b66dd02dbbc6accc7) )
	ROM_LOAD( "snm 6.bin", 0x0000, 0x010000, CRC(0d14730b) SHA1(2a35d72bdcc9402b00153621ec852f902720c104) )
	ROM_LOAD( "snmx.p1", 0x0000, 0x010000, CRC(61a78035) SHA1(1d6c553c60fee0b80e06f8421b8a3806d1f3a587) )
ROM_END

ROM_START( m4vivasx )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "vlvs.p1", 0x0000, 0x010000, CRC(b7fb3e19) SHA1(c6cc4175f8c100fc37e6e7014b0744054b4e547a) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "vlvs.chr", 0x0000, 0x000048, CRC(31acf949) SHA1(d622ea1caee968b786f3183ca44355f9db190081) )
ROM_END


ROM_START( m4acechs )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ae_05a__.2_3", 0x0000, 0x010000, CRC(c9a03623) SHA1(8daf7e71057528c481915eb8506e03ce9cf372c8) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "ae_05a__.3_1", 0x0000, 0x010000, CRC(900e1789) SHA1(dbb13f1728d8527a7de5d257c866732adb0a95b5) )
	ROM_LOAD( "ae_05s__.2_3", 0x0000, 0x010000, CRC(eb64ab0a) SHA1(4d4c6908c8ca8b1d3c39c8973c8386da079cbd39) )
	ROM_LOAD( "ae_05sb_.2_3", 0x0000, 0x010000, CRC(5d67c6f6) SHA1(213225405defb3be7f564459d71aeca6f5856f8f) )
	ROM_LOAD( "ae_05sd_.2_3", 0x0000, 0x010000, CRC(2bdbe356) SHA1(a328a8f50847cbb199b31672ca50e1e95a474e4b) )
	ROM_LOAD( "ae_10a__.2_3", 0x0000, 0x010000, CRC(d718d498) SHA1(d13970b0ca86b988bcc91cd3c2dbee4c637944ca) )
	ROM_LOAD( "ae_10a__.3_1", 0x0000, 0x010000, CRC(e20c2513) SHA1(857ed8a6b155863c769ee9c3aca5e4702c1372b6) )
	ROM_LOAD( "ae_10bg_.2_3", 0x0000, 0x010000, CRC(7ed7fcee) SHA1(7b2b0c47dc8a75d11f49f09441a4320815d838ac) )
	ROM_LOAD( "ae_10s__.2_3", 0x0000, 0x010000, CRC(31932d3f) SHA1(a1809c7baaea22d24491829a8638f232e2d75849) )
	ROM_LOAD( "ae_10sb_.2_3", 0x0000, 0x010000, CRC(d6bcd1fd) SHA1(664ec7e7821c09bddfd1996892ae3f9fbdbc6809) )
	ROM_LOAD( "ae_10sd_.2_3", 0x0000, 0x010000, CRC(5920b9ad) SHA1(fb8de53e7877505fe53ff874b396707ee8e01e5e) )
	ROM_LOAD( "ae_20a__.3_1", 0x0000, 0x010000, CRC(43f6cc19) SHA1(3eda49477b141c649a4ba7a4ecc021694d9830db) )
	ROM_LOAD( "ae_20b__.3_1", 0x0000, 0x010000, CRC(30060ac4) SHA1(488263a1d3cfe067d43de29c57e58fe55024437c) )
	ROM_LOAD( "ae_20bd_.3_1", 0x0000, 0x010000, CRC(f9b922c2) SHA1(fc0deb79fc6c33732872da8925a6729f3d11bcaf) )
	ROM_LOAD( "ae_20bg_.3_1", 0x0000, 0x010000, CRC(02706741) SHA1(8388d91091945d1f73aa5e68a86f930f5d9dafa2) )
	ROM_LOAD( "ae_20bt_.3_1", 0x0000, 0x010000, CRC(3b313958) SHA1(9fe4cb99dc30d1305816f9a27079d97c4d07cb15) )
	ROM_LOAD( "ae_20sb_.3_1", 0x0000, 0x010000, CRC(471f2ba4) SHA1(baaf8339d8ee15365886cea2ecb36ad298975633) )
	ROM_LOAD( "aei05___.2_3", 0x0000, 0x010000, CRC(f035ba55) SHA1(d13bebec00650018a9236cc18df73b06c970cfd0) )
	ROM_LOAD( "aei05___.3_1", 0x0000, 0x010000, CRC(bb84d01f) SHA1(f1653590e8cd642faf09a16c5c1b0a4b267d42e7) )
	ROM_LOAD( "aei10___.2_3", 0x0000, 0x010000, CRC(96edf44f) SHA1(8abcb5d4018e0a4c879eb1a1550af09f55f75135) )
	ROM_LOAD( "aei10___.3_1", 0x0000, 0x010000, CRC(db99a965) SHA1(1fb200b30e10d502af39bcd2e58d3e36e13f3695) )
	ROM_LOAD( "aei20___.3_1", 0x0000, 0x010000, CRC(1744e7f4) SHA1(bf2f1b720a1a2610aff46a1de5c789a17828eae0) )
ROM_END

ROM_START( m4stc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "stc01s", 0x0000, 0x010000, CRC(8371bb8f) SHA1(bd60825b3f5011c218b34f00886b6b54afe61b9f) )
ROM_END

ROM_START( m4sstrek )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "rhr2pprgpatched.bin", 0x0000, 0x010000, CRC(a0b3439d) SHA1(0976537a5170bf4c4f595f7fa04243a68f14b2ae) )
ROM_END


GAME(199?, m4cojok	,0			,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Carry On Joker (Barcrest) (MPU4)",						GAME_FLAGS )
GAMEL(198?,m4gambal	,0			,mod4yam	,m4gambal			, mpu4_state,m4gambal			,ROT0,   "Barcrest","Gamball (Barcrest) (MPU4)",							GAME_REQUIRES_ARTWORK|GAME_MECHANICAL,layout_gamball )//Mechanical ball launcher
GAME(199?, m4graff	,0			,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Graffiti (Barcrest) (MPU4)",						GAME_FLAGS )
GAME(199?, m4graffd	,m4graff	,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Grafitti (Barcrest) [Dutch] (MPU4)",						GAME_FLAGS )
GAME(199?, m4monte	,0			,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Monte Carlo (Barcrest) (MPU4)",						GAME_FLAGS )
GAME(199?, m4nudbnk	,0			,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Nudge Banker (Barcrest) (MPU4)",						GAME_FLAGS )
GAME(199?, m4przmc	,0			,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Prize Monte Carlo (Barcrest) (MPU4)",						GAME_FLAGS )
GAME(199?, m4przsss	,0			,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Prize Spend Spend Spend (Barcrest) (MPU4)",						GAME_FLAGS )
GAME(199?, m4sss	,0			,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Spend Spend Spend (Barcrest) (MPU4)",						GAME_FLAGS )
GAME(199?, m4ra		,0			,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Red Alert (Barcrest) (MPU4)",						GAME_FLAGS )
GAME(199?, m4sayno	,0			,mod4yam   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Say No More (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4vivasx	,0			,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Viva Las Vegas Six (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4acechs	,0			,mod4yam	,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Ace Chase (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4stc	,0			,mod4yam   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","unknown MPU4 'STC 0.1' (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )


#define M4SUPST_EXTRA_ROMS \
	ROM_REGION( 0x48, "fakechr", 0 ) \
	ROM_LOAD( "ss.chr", 0x0000, 0x000048, CRC(bd206d57) SHA1(ecfe38d9b4823ae6bc2fc440c243e6ae5e2edaa4) ) \

#define M4SUPST_SET(year, setname,parent,name,offset,length,hash,company,title) \
	ROM_START( setname ) \
		ROM_REGION( length, "maincpu", 0 ) \
		ROM_LOAD( name, offset, length, hash ) \
		M4SUPST_EXTRA_ROMS \
	ROM_END \
	GAME(year, setname, parent ,mod4yam	,mpu4 , mpu4_state,m4_showstring ,ROT0,company,title,GAME_FLAGS ) \


M4SUPST_SET( 199?, m4supst,		0,			"cs4b.p1",				0x0000, 0x010000, CRC(fb0aac20) SHA1(3a40be78f7add7905afa8d1226ad41bf0041a2ec), "Barcrest","Super Streak (Barcrest) (MPU4) (set 1)" )
M4SUPST_SET( 199?, m4supst__a,	m4supst,	"cs4ad.p1",				0x0000, 0x010000, CRC(c0e81dfd) SHA1(2da922df6c102f8d0f1678e974df9e4d356e5133), "Barcrest","Super Streak (Barcrest) (MPU4) (set 2)" )
M4SUPST_SET( 199?, m4supst__b,	m4supst,	"cs4bd.p1",				0x0000, 0x010000, CRC(dafc7ed6) SHA1(3e92d5557d2f587132f4b3b633978ab7d4333fcc), "Barcrest","Super Streak (Barcrest) (MPU4) (set 3)" )
M4SUPST_SET( 199?, m4supst__c,	m4supst,	"cs4d.p1",				0x0000, 0x010000, CRC(c1fcda65) SHA1(11f2a45f3f821eac6b98b1988824d77aada3d759), "Barcrest","Super Streak (Barcrest) (MPU4) (set 4)" )
M4SUPST_SET( 199?, m4supst__d,	m4supst,	"cs4dk.p1",				0x0000, 0x010000, CRC(30a46171) SHA1(ef1f2951b478ba2b2d42dfb0ec4ed59f28d79972), "Barcrest","Super Streak (Barcrest) (MPU4) (set 5)" )
M4SUPST_SET( 199?, m4supst__e,	m4supst,	"cs4dy.p1",				0x0000, 0x010000, CRC(72b15ce7) SHA1(c451ac552ffe9bcde1990b97a60b0ed8918bf8c8), "Barcrest","Super Streak (Barcrest) (MPU4) (set 6)" )
M4SUPST_SET( 199?, m4supst__f,	m4supst,	"cs4k.p1",				0x0000, 0x010000, CRC(f252f9ea) SHA1(251998ea752deb4f4a05c833b19e89d334334fac), "Barcrest","Super Streak (Barcrest) (MPU4) (set 7)" )
M4SUPST_SET( 199?, m4supst__g,	m4supst,	"cs4s.p1",				0x0000, 0x010000, CRC(10f7b88d) SHA1(0aac0ebbe0ce04db49fc7de4325eea9abdfd74b5), "Barcrest","Super Streak (Barcrest) (MPU4) (set 8)" )
M4SUPST_SET( 199?, m4supst__h,	m4supst,	"cs4y.p1",				0x0000, 0x010000, CRC(a464d09d) SHA1(d38c0f8c7c9b7f560b685781a7dcf82bc031a191), "Barcrest","Super Streak (Barcrest) (MPU4) (set 9)" )
M4SUPST_SET( 199?, m4supst__i,	m4supst,	"csp02ad.p1",			0x0000, 0x010000, CRC(96bbbc26) SHA1(ca127151c771963c07f0f368102ede8095d11863), "Barcrest","Super Streak (Barcrest) (MPU4) (set 10)" )
M4SUPST_SET( 199?, m4supst__j,	m4supst,	"csp02b.p1",			0x0000, 0x010000, CRC(913ea9ff) SHA1(182bcc007d007a1c7f57767358600d2de7d1e3cf), "Barcrest","Super Streak (Barcrest) (MPU4) (set 11)" )
M4SUPST_SET( 199?, m4supst__k,	m4supst,	"csp02bd.p1",			0x0000, 0x010000, CRC(ad0137a1) SHA1(d043372ba09081dd4e807f009a6460b4b30e6453), "Barcrest","Super Streak (Barcrest) (MPU4) (set 12)" )
M4SUPST_SET( 199?, m4supst__l,	m4supst,	"csp02c.p1",			0x0000, 0x010000, CRC(fdad4b22) SHA1(4f19922821a9d1663bd9355447209384272e7542), "Barcrest","Super Streak (Barcrest) (MPU4) (set 13)" )
M4SUPST_SET( 199?, m4supst__m,	m4supst,	"csp02d.p1",			0x0000, 0x010000, CRC(9717a58d) SHA1(8bc495dc4db0041718ae2db14a01a789616c8764), "Barcrest","Super Streak (Barcrest) (MPU4) (set 14)" )
M4SUPST_SET( 199?, m4supst__n,	m4supst,	"csp02dk.p1",			0x0000, 0x010000, CRC(cd8aa547) SHA1(a13dcb75507878cb133b9ef739fb41d932d4eed5), "Barcrest","Super Streak (Barcrest) (MPU4) (set 15)" )
M4SUPST_SET( 199?, m4supst__o,	m4supst,	"csp02dr.p1",			0x0000, 0x010000, CRC(6656e588) SHA1(4001ec0d1145ef0107e62ccda61e22ba8b0cdc92), "Barcrest","Super Streak (Barcrest) (MPU4) (set 16)" )
M4SUPST_SET( 199?, m4supst__p,	m4supst,	"csp02dy.p1",			0x0000, 0x010000, CRC(14ff7e1d) SHA1(455b6ff93a5f25dc5f43c62a6c1d9a18de1ce94b), "Barcrest","Super Streak (Barcrest) (MPU4) (set 17)" )
M4SUPST_SET( 199?, m4supst__q,	m4supst,	"csp02k.p1",			0x0000, 0x010000, CRC(c438c754) SHA1(c1d2e664091c1eaf1e4d964a3bfd446b11d7ba41), "Barcrest","Super Streak (Barcrest) (MPU4) (set 18)" )
M4SUPST_SET( 199?, m4supst__r,	m4supst,	"csp02r.p1",			0x0000, 0x010000, CRC(4abe0f80) SHA1(67f7f9946a26b5097b6ce719dbd599790078f365), "Barcrest","Super Streak (Barcrest) (MPU4) (set 19)" )
M4SUPST_SET( 199?, m4supst__s,	m4supst,	"csp02s.p1",			0x0000, 0x010000, CRC(47c0068d) SHA1(5480a519a6e6df2757e66cfcf904dd6c2873cc43), "Barcrest","Super Streak (Barcrest) (MPU4) (set 20)" )
M4SUPST_SET( 199?, m4supst__t,	m4supst,	"csp02y.p1",			0x0000, 0x010000, CRC(d51d18d8) SHA1(a65fd4326872775364d2d7a886e98a1ee07596b7), "Barcrest","Super Streak (Barcrest) (MPU4) (set 21)" )
M4SUPST_SET( 199?, m4supst__u,	m4supst,	"cst04ad.p1",			0x0000, 0x010000, CRC(b946d40d) SHA1(c03fa48f8b64c3cf4504f472f21a38f8a55f12e6), "Barcrest","Super Streak (Barcrest) (MPU4) (set 22)" )
M4SUPST_SET( 199?, m4supst__v,	m4supst,	"cst04b.p1",			0x0000, 0x010000, CRC(45333d45) SHA1(d6ccb39ee9b316772052f856f79424c34ff273c5), "Barcrest","Super Streak (Barcrest) (MPU4) (set 23)" )
M4SUPST_SET( 199?, m4supst__w,	m4supst,	"cst04bd.p1",			0x0000, 0x010000, CRC(03b56b07) SHA1(903b24ab93f9584f228278729b5a99451b8e81f7), "Barcrest","Super Streak (Barcrest) (MPU4) (set 24)" )
M4SUPST_SET( 199?, m4supst__x,	m4supst,	"cst04c.p1",			0x0000, 0x010000, CRC(9c000883) SHA1(da0a9f1afc218c14a57a46fe2ea63e166f4e3739), "Barcrest","Super Streak (Barcrest) (MPU4) (set 25)" )
M4SUPST_SET( 199?, m4supst__y,	m4supst,	"cst04d.p1",			0x0000, 0x010000, CRC(32281bec) SHA1(a043fb615c2a66d23d85ae80cb0b1705523f411c), "Barcrest","Super Streak (Barcrest) (MPU4) (set 26)" )
M4SUPST_SET( 199?, m4supst__z,	m4supst,	"cst04dk.p1",			0x0000, 0x010000, CRC(9345e7b7) SHA1(8bff80d2b847fbae050f77215efe3e55b98a4657), "Barcrest","Super Streak (Barcrest) (MPU4) (set 27)" )
M4SUPST_SET( 199?, m4supst__0,	m4supst,	"cst04dr.p1",			0x0000, 0x010000, CRC(8d397063) SHA1(45642de2629e89e2495d1cbd5aed90cf2a4cf1c1), "Barcrest","Super Streak (Barcrest) (MPU4) (set 28)" )
M4SUPST_SET( 199?, m4supst__1,	m4supst,	"cst04dy.p1",			0x0000, 0x010000, CRC(4a303ced) SHA1(6c12b956358753c8bf99bd3316646721c9ec2585), "Barcrest","Super Streak (Barcrest) (MPU4) (set 29)" )
M4SUPST_SET( 199?, m4supst__2,	m4supst,	"cst04k.p1",			0x0000, 0x010000, CRC(a59584f5) SHA1(8cfcf069ad905277f1925e682602e129e97e619b), "Barcrest","Super Streak (Barcrest) (MPU4) (set 30)" )
M4SUPST_SET( 199?, m4supst__3,	m4supst,	"cst04r.p1",			0x0000, 0x010000, CRC(c9771997) SHA1(ed98650c0d73f2db0fe380777d10404ccabced31), "Barcrest","Super Streak (Barcrest) (MPU4) (set 31)" )
M4SUPST_SET( 199?, m4supst__4,	m4supst,	"cst04s.p1",			0x0000, 0x010000, CRC(cd5b848d) SHA1(4dd3dd1c883552c7b5c475156308604b12eff75a), "Barcrest","Super Streak (Barcrest) (MPU4) (set 32)" )
M4SUPST_SET( 199?, m4supst__5,	m4supst,	"cst04y.p1",			0x0000, 0x010000, CRC(7adc00ae) SHA1(5688f0876c18faf474a6d8487fdd85f20f9fc144), "Barcrest","Super Streak (Barcrest) (MPU4) (set 33)" )
M4SUPST_SET( 199?, m4supst__6,	m4supst,	"csu03ad.p1",			0x0000, 0x010000, CRC(5d7b6393) SHA1(19c24f4113efb6a1499936e5f89a8ad859ff8df0), "Barcrest","Super Streak (Barcrest) (MPU4) (set 34)" )
M4SUPST_SET( 199?, m4supst__7,	m4supst,	"csu03b.p1",			0x0000, 0x010000, CRC(57826c2a) SHA1(b835eb3066fec468ab55851d1dd023484e2d57e3), "Barcrest","Super Streak (Barcrest) (MPU4) (set 35)" )
M4SUPST_SET( 199?, m4supst__8,	m4supst,	"csu03bd.p1",			0x0000, 0x010000, CRC(092e7039) SHA1(36a7c18872e4012e3acce0d01d2cc2c201a3c867), "Barcrest","Super Streak (Barcrest) (MPU4) (set 36)" )
M4SUPST_SET( 199?, m4supst__9,	m4supst,	"csu03c.p1",			0x0000, 0x010000, CRC(b30a3c00) SHA1(066b0007092720a6f89edf8eafffe2f8fd83edbc), "Barcrest","Super Streak (Barcrest) (MPU4) (set 37)" )
M4SUPST_SET( 199?, m4supst__aa,	m4supst,	"csu03d.p1",			0x0000, 0x010000, CRC(03ff9d99) SHA1(390087c136e4c314de9086adb7b020e8adabe34a), "Barcrest","Super Streak (Barcrest) (MPU4) (set 38)" )
M4SUPST_SET( 199?, m4supst__ab,	m4supst,	"csu03dk.p1",			0x0000, 0x010000, CRC(cf7e61ff) SHA1(0e328ce5ff86770fabaf91d48a8de039323d112a), "Barcrest","Super Streak (Barcrest) (MPU4) (set 39)" )
M4SUPST_SET( 199?, m4supst__ac,	m4supst,	"csu03dr.p1",			0x0000, 0x010000, CRC(00d700d1) SHA1(8bcc3c470c42780b1f1404fc6ff53e6ec7d89ad0), "Barcrest","Super Streak (Barcrest) (MPU4) (set 40)" )
M4SUPST_SET( 199?, m4supst__ad,	m4supst,	"csu03dy.p1",			0x0000, 0x010000, CRC(8ec77c04) SHA1(64708460439a7e124f90eef6b9628e57f7d78ebc), "Barcrest","Super Streak (Barcrest) (MPU4) (set 41)" )
M4SUPST_SET( 199?, m4supst__ae,	m4supst,	"csu03k.p1",			0x0000, 0x010000, CRC(701a0837) SHA1(31237fd108b354fb2afc449efa3a53dee2cf7be8), "Barcrest","Super Streak (Barcrest) (MPU4) (set 42)" )
M4SUPST_SET( 199?, m4supst__af,	m4supst,	"csu03r.p1",			0x0000, 0x010000, CRC(d86a6895) SHA1(2c42bcf5de739f01e18bd1b766eec26a6da5aa52), "Barcrest","Super Streak (Barcrest) (MPU4) (set 43)" )
M4SUPST_SET( 199?, m4supst__ag,	m4supst,	"csu03s.p1",			0x0000, 0x010000, CRC(197bb032) SHA1(06e98713ff5fc72bffccde1cc92fc8cb63665fad), "Barcrest","Super Streak (Barcrest) (MPU4) (set 44)" )
M4SUPST_SET( 199?, m4supst__ah,	m4supst,	"csu03y.p1",			0x0000, 0x010000, CRC(bee0e7e1) SHA1(6a1ab766af9147f0d4a7c1d2a95c9a6e3e3f4986), "Barcrest","Super Streak (Barcrest) (MPU4) (set 45)" )
M4SUPST_SET( 199?, m4supst__ai,	m4supst,	"eeh02ad.p1",			0x0000, 0x010000, CRC(25874a6d) SHA1(12e4fb36d231c3104df3613dd3851f411a876eb0), "Barcrest","Super Streak (Barcrest) (MPU4) (set 46)" )
M4SUPST_SET( 199?, m4supst__aj,	m4supst,	"eeh02b.p1",			0x0000, 0x010000, CRC(ef280a8a) SHA1(912a825e69482a540cf0cadfc49a37a2822f3ecb), "Barcrest","Super Streak (Barcrest) (MPU4) (set 47)" )
M4SUPST_SET( 199?, m4supst__ak,	m4supst,	"eeh02bd.p1",			0x0000, 0x010000, CRC(5f126810) SHA1(8fe1cbc7d93e2db35225388ee0773f6a98762ca1), "Barcrest","Super Streak (Barcrest) (MPU4) (set 48)" )
M4SUPST_SET( 199?, m4supst__al,	m4supst,	"eeh02c.p1",			0x0000, 0x010000, CRC(3f49b936) SHA1(a0d07e0101f8cc38ebc28cfc1b239793b961f5ab), "Barcrest","Super Streak (Barcrest) (MPU4) (set 49)" )
M4SUPST_SET( 199?, m4supst__am,	m4supst,	"eeh02d.p1",			0x0000, 0x010000, CRC(14dcfe63) SHA1(3ac77c9aa9b3b77fb1df98d2b427564be41dca78), "Barcrest","Super Streak (Barcrest) (MPU4) (set 50)" )
M4SUPST_SET( 199?, m4supst__an,	m4supst,	"eeh02dk.p1",			0x0000, 0x010000, CRC(81a39421) SHA1(6fa43e8cb83e7fb940cc224eed5ee3f254c18c4d), "Barcrest","Super Streak (Barcrest) (MPU4) (set 51)" )
M4SUPST_SET( 199?, m4supst__ao,	m4supst,	"eeh02dr.p1",			0x0000, 0x010000, CRC(c7755823) SHA1(05626ed49a2f800555f3f404273fa910b68de75c), "Barcrest","Super Streak (Barcrest) (MPU4) (set 52)" )
M4SUPST_SET( 199?, m4supst__ap,	m4supst,	"eeh02dy.p1",			0x0000, 0x010000, CRC(5a1e70cd) SHA1(88bb29fd52d2331b72bb04652f9578f2c2f5a9ac), "Barcrest","Super Streak (Barcrest) (MPU4) (set 53)" )
M4SUPST_SET( 199?, m4supst__aq,	m4supst,	"eeh02k.p1",			0x0000, 0x010000, CRC(b78882ec) SHA1(79c6a6d2cfe113743d3a93eb825fccab2b025933), "Barcrest","Super Streak (Barcrest) (MPU4) (set 54)" )
M4SUPST_SET( 199?, m4supst__ar,	m4supst,	"eeh02r.p1",			0x0000, 0x010000, CRC(ff54884e) SHA1(2783f0e562e946597288ddbec4dcd1101e188d1d), "Barcrest","Super Streak (Barcrest) (MPU4) (set 55)" )
M4SUPST_SET( 199?, m4supst__as,	m4supst,	"eeh02s.p1",			0x0000, 0x010000, CRC(c5856c3c) SHA1(5a0e5a7188913e1c36eac894bbeeae47a4f3589c), "Barcrest","Super Streak (Barcrest) (MPU4) (set 56)" )
M4SUPST_SET( 199?, m4supst__at,	m4supst,	"eeh02y.p1",			0x0000, 0x010000, CRC(623fa0a0) SHA1(5a49cea5e94afccbf965cbda7a8d9a74f9734a6e), "Barcrest","Super Streak (Barcrest) (MPU4) (set 57)" )
M4SUPST_SET( 199?, m4supst__au,	m4supst,	"sp8b.p1",				0x0000, 0x010000, CRC(3b12d7e8) SHA1(92a15e5f8391d74c192e8386abdb8853a76bff05), "Barcrest","Super Streak (Barcrest) (MPU4) (set 58)" )
M4SUPST_SET( 199?, m4supst__av,	m4supst,	"sp8bd.p1",				0x0000, 0x010000, CRC(e0d7f789) SHA1(f6157469e43059adb44e7f2eff5bf73861d5636c), "Barcrest","Super Streak (Barcrest) (MPU4) (set 59)" )
M4SUPST_SET( 199?, m4supst__aw,	m4supst,	"sp8c.p1",				0x0000, 0x010000, CRC(da0af8ae) SHA1(91042506050967c508b30c3dc2bfa6f6a6e8b532), "Barcrest","Super Streak (Barcrest) (MPU4) (set 60)" )
M4SUPST_SET( 199?, m4supst__ax,	m4supst,	"sp8dk.p1",				0x0000, 0x010000, CRC(92432e8f) SHA1(5e6df963ccf92a89c71ae1edd7b71ec1e3f97522), "Barcrest","Super Streak (Barcrest) (MPU4) (set 61)" )
M4SUPST_SET( 199?, m4supst__ay,	m4supst,	"sp8k.p1",				0x0000, 0x010000, CRC(e39f74d8) SHA1(9d776e7d67859f4514c69fc4f9f43160da9a2ca1), "Barcrest","Super Streak (Barcrest) (MPU4) (set 62)" )
M4SUPST_SET( 199?, m4supst__az,	m4supst,	"sp8s.p1",				0x0000, 0x010000, CRC(fab99461) SHA1(82f8ca06bb04396f86124dfe4de46265b2edc393), "Barcrest","Super Streak (Barcrest) (MPU4) (set 63)" )
M4SUPST_SET( 199?, m4supst__a0,	m4supst,	"spsbd.p1",				0x0000, 0x010000, CRC(b621b32d) SHA1(9aab0e074c120cb12beac585f9c513053502955c), "Barcrest","Super Streak (Barcrest) (MPU4) (set 64)" )
M4SUPST_SET( 199?, m4supst__a1,	m4supst,	"spsc.p1",				0x0000, 0x010000, CRC(8c7a24f5) SHA1(f86be164e05235281fb275e950cedaf6f630d29a), "Barcrest","Super Streak (Barcrest) (MPU4) (set 65)" )
M4SUPST_SET( 199?, m4supst__a2,	m4supst,	"spsd.p1",				0x0000, 0x010000, CRC(d34d3617) SHA1(5373335557e4bbb21264bbd9d0fbaf3640f9ab35), "Barcrest","Super Streak (Barcrest) (MPU4) (set 66)" )
M4SUPST_SET( 199?, m4supst__a3,	m4supst,	"spsdk.p1",				0x0000, 0x010000, CRC(cf2fd3e7) SHA1(50d3c0851bec90037cd65a5c55654b0e688b96ca), "Barcrest","Super Streak (Barcrest) (MPU4) (set 67)" )
M4SUPST_SET( 199?, m4supst__a4,	m4supst,	"spsk.p1",				0x0000, 0x010000, CRC(873a1414) SHA1(47b2bbef168382112cd12ace2d6a58695f4b0254), "Barcrest","Super Streak (Barcrest) (MPU4) (set 68)" )
M4SUPST_SET( 199?, m4supst__a5,	m4supst,	"spss.p1",				0x0000, 0x010000, CRC(5e28bdb7) SHA1(3865c891178feb744ad11b2dea491350efc48bea), "Barcrest","Super Streak (Barcrest) (MPU4) (set 69)" )
M4SUPST_SET( 199?, m4supst__a6,	m4supst,	"stc02ad.p1",			0x0000, 0x010000, CRC(d9a2b4d1) SHA1(9a6862a44817b3ec465f126fd2a5d2c9825d846e), "Barcrest","Super Streak (Barcrest) (MPU4) (set 70)" )
M4SUPST_SET( 199?, m4supst__a7,	m4supst,	"stc02b.p1",			0x0000, 0x010000, CRC(bd2e8e6c) SHA1(71670dccedc2f47888c1205de59a81677ffeabaa), "Barcrest","Super Streak (Barcrest) (MPU4) (set 71)" )
M4SUPST_SET( 199?, m4supst__a8,	m4supst,	"stc02bd.p1",			0x0000, 0x010000, CRC(efbed99b) SHA1(62d80248bb666bfb49ed7546936da744e43fa870), "Barcrest","Super Streak (Barcrest) (MPU4) (set 72)" )
M4SUPST_SET( 199?, m4supst__a9,	m4supst,	"stc02c.p1",			0x0000, 0x010000, CRC(9d342386) SHA1(b50f64d66d89dbd3dee1ff2cb430a2caa050e7c8), "Barcrest","Super Streak (Barcrest) (MPU4) (set 73)" )
M4SUPST_SET( 199?, m4supst__ba,	m4supst,	"stc02d.p1",			0x0000, 0x010000, CRC(c43f6e65) SHA1(0278cf389f8289d7b819125ae0a612c81ea75fab), "Barcrest","Super Streak (Barcrest) (MPU4) (set 74)" )
M4SUPST_SET( 199?, m4supst__bb,	m4supst,	"stc02dk.p1",			0x0000, 0x010000, CRC(36576570) SHA1(214a57344d8e161b3dbd07457291ed9bce011842), "Barcrest","Super Streak (Barcrest) (MPU4) (set 75)" )
M4SUPST_SET( 199?, m4supst__bc,	m4supst,	"stc02dr.p1",			0x0000, 0x010000, CRC(450c553f) SHA1(46050285eeb10dc368ad501c61d41351c4e2fcde), "Barcrest","Super Streak (Barcrest) (MPU4) (set 76)" )
M4SUPST_SET( 199?, m4supst__bd,	m4supst,	"stc02dy.p1",			0x0000, 0x010000, CRC(d8677dd1) SHA1(18abc0a1d28458c3b26a0d1dbf6ca8aba3f3e240), "Barcrest","Super Streak (Barcrest) (MPU4) (set 77)" )
M4SUPST_SET( 199?, m4supst__be,	m4supst,	"stc02k.p1",			0x0000, 0x010000, CRC(c6e8d110) SHA1(9e05961b9bba502f52a03de27e608afc52f6c025), "Barcrest","Super Streak (Barcrest) (MPU4) (set 78)" )
M4SUPST_SET( 199?, m4supst__bf,	m4supst,	"stc02r.p1",			0x0000, 0x010000, CRC(918d769f) SHA1(2a4438828d9e7efd3a94eaebe56585e7ae23d9d1), "Barcrest","Super Streak (Barcrest) (MPU4) (set 79)" )
M4SUPST_SET( 199?, m4supst__bg,	m4supst,	"stc02s.p1",			0x0000, 0x010000, CRC(9c50fff7) SHA1(3468340d2d04cbdecd669817f8a9c4028e301eeb), "Barcrest","Super Streak (Barcrest) (MPU4) (set 80)" )
M4SUPST_SET( 199?, m4supst__bh,	m4supst,	"stc02y.p1",			0x0000, 0x010000, CRC(0ce65e71) SHA1(02ae1fd5a41ab5a96ddcfe1cf3e8567561291961), "Barcrest","Super Streak (Barcrest) (MPU4) (set 81)" )
M4SUPST_SET( 199?, m4supst__bi,	m4supst,	"sttad.p1",				0x0000, 0x010000, CRC(af615f05) SHA1(b2c1b8ba086a4d33f1269c28d4caa7286a27f085), "Barcrest","Super Streak (Barcrest) (MPU4) (set 82)" )
M4SUPST_SET( 199?, m4supst__bj,	m4supst,	"sttb.p1",				0x0000, 0x010000, CRC(3119149f) SHA1(e749fcc5f95ccd29f42bfd0b140cf3cbb84d9599), "Barcrest","Super Streak (Barcrest) (MPU4) (set 83)" )
M4SUPST_SET( 199?, m4supst__bk,	m4supst,	"sttbd.p1",				0x0000, 0x010000, CRC(cfddaf39) SHA1(0f24b5e691e1d43f6604087f0b3bc2571d2c4002), "Barcrest","Super Streak (Barcrest) (MPU4) (set 84)" )
M4SUPST_SET( 199?, m4supst__bl,	m4supst,	"sttd.p1",				0x0000, 0x010000, CRC(8bc2498c) SHA1(a9cd3a6968186818a8c4033b1f304eac152244cf), "Barcrest","Super Streak (Barcrest) (MPU4) (set 85)" )
M4SUPST_SET( 199?, m4supst__bm,	m4supst,	"sttdk.p1",				0x0000, 0x010000, CRC(39903dde) SHA1(f92c4380051ada7bbc5739550c8dfdd6ddaaa3fe), "Barcrest","Super Streak (Barcrest) (MPU4) (set 86)" )
M4SUPST_SET( 199?, m4supst__bn,	m4supst,	"sttdr.p1",				0x0000, 0x010000, CRC(866f69f0) SHA1(ef9717f89b9718f1bcf8d3592f240ec9cf48bca3), "Barcrest","Super Streak (Barcrest) (MPU4) (set 87)" )
M4SUPST_SET( 199?, m4supst__bo,	m4supst,	"sttdy.p1",				0x0000, 0x010000, CRC(74ebd933) SHA1(b308c8cae2c74e4e07c6e4afb505068220714824), "Barcrest","Super Streak (Barcrest) (MPU4) (set 88)" )
M4SUPST_SET( 199?, m4supst__bp,	m4supst,	"sttk.p1",				0x0000, 0x010000, CRC(461db2f5) SHA1(8b97342d7ebfb33aa6aff246e8d799f4435788b7), "Barcrest","Super Streak (Barcrest) (MPU4) (set 89)" )
M4SUPST_SET( 199?, m4supst__bq,	m4supst,	"sttr.p1",				0x0000, 0x010000, CRC(2591f6ec) SHA1(3d83d930e41e164e71d67b529967320e1eee8354), "Barcrest","Super Streak (Barcrest) (MPU4) (set 90)" )
M4SUPST_SET( 199?, m4supst__br,	m4supst,	"stts.p1",				0x0000, 0x010000, CRC(a5e29c32) SHA1(8ba2f76505c2f40493c918b9d9524fa67999f7c1), "Barcrest","Super Streak (Barcrest) (MPU4) (set 91)" )
M4SUPST_SET( 199?, m4supst__bs,	m4supst,	"stty.p1",				0x0000, 0x010000, CRC(7306fab9) SHA1(0da1612490fcff9b7a17f97190b6b561016c3b18), "Barcrest","Super Streak (Barcrest) (MPU4) (set 92)" )
M4SUPST_SET( 199?, m4supst__bt,	m4supst,	"stuad.p1",				0x0000, 0x010000, CRC(e7a01b7b) SHA1(3db08800a35d440f012ca69d84c30465818b4993), "Barcrest","Super Streak (Barcrest) (MPU4) (set 93)" )
M4SUPST_SET( 199?, m4supst__bu,	m4supst,	"stub.p1",				0x0000, 0x010000, CRC(9044badf) SHA1(af8e218e3dc457bb5f24e3f2d74a8639466c3f11), "Barcrest","Super Streak (Barcrest) (MPU4) (set 94)" )
M4SUPST_SET( 199?, m4supst__bv,	m4supst,	"stubd.p1",				0x0000, 0x010000, CRC(438e1687) SHA1(5e0f27e95bf861d4edc55709efc79496c7353e8b), "Barcrest","Super Streak (Barcrest) (MPU4) (set 95)" )
M4SUPST_SET( 199?, m4supst__bw,	m4supst,	"stud.p1",				0x0000, 0x010000, CRC(1cbe3bec) SHA1(005dde84e57c5517fc6d6b975cc882dae11cbf63), "Barcrest","Super Streak (Barcrest) (MPU4) (set 96)" )
M4SUPST_SET( 199?, m4supst__bx,	m4supst,	"studk.p1",				0x0000, 0x010000, CRC(0931d501) SHA1(afa078248230cbc0acc9d3af641ec63ed0424a75), "Barcrest","Super Streak (Barcrest) (MPU4) (set 97)" )
M4SUPST_SET( 199?, m4supst__by,	m4supst,	"studr.p1",				0x0000, 0x010000, CRC(e06e1c59) SHA1(f4454f640335dbf6f9b8154d7805102253f605b4), "Barcrest","Super Streak (Barcrest) (MPU4) (set 98)" )
M4SUPST_SET( 199?, m4supst__bz,	m4supst,	"study.p1",				0x0000, 0x010000, CRC(8b4275e0) SHA1(267a9d2eddf41b8838eeaee06bba45f0a8b8451f), "Barcrest","Super Streak (Barcrest) (MPU4) (set 99)" )
M4SUPST_SET( 199?, m4supst__b0,	m4supst,	"stuk.p1",				0x0000, 0x010000, CRC(a66fb54f) SHA1(4351edbf6c5de817cf6972885ff1f6c7df837c37), "Barcrest","Super Streak (Barcrest) (MPU4) (set 100)" )
M4SUPST_SET( 199?, m4supst__b1,	m4supst,	"stur.p1",				0x0000, 0x010000, CRC(eeb3bfed) SHA1(87a753511fb384a505d3cc69ca67fe4e288cf3bb), "Barcrest","Super Streak (Barcrest) (MPU4) (set 101)" )
M4SUPST_SET( 199?, m4supst__b2,	m4supst,	"stus.p1",				0x0000, 0x010000, CRC(19aca6ad) SHA1(1583e76a4e1058fa97efdd9a7e6f7c4fe806b2f4), "Barcrest","Super Streak (Barcrest) (MPU4) (set 102)" )
M4SUPST_SET( 199?, m4supst__b3,	m4supst,	"stuy.p1",				0x0000, 0x010000, CRC(e6b2b76f) SHA1(bf251b751e6a8d2764c63e92d48e1a64666b9a47), "Barcrest","Super Streak (Barcrest) (MPU4) (set 103)" )
M4SUPST_SET( 199?, m4supst__b4,	m4supst,	"superstreak1deb.bin",	0x0000, 0x010000, CRC(892ccad9) SHA1(c88daadd9778e363e154b674b57ccd07cea59836), "Barcrest","Super Streak (Barcrest) (MPU4) (set 104)" )
M4SUPST_SET( 199?, m4supst__b5,	m4supst,	"supst2515",			0x0000, 0x010000, CRC(c073a249) SHA1(4ae37eb61dd5e50687f433fb89f65b97926b7358), "Barcrest","Super Streak (Barcrest) (MPU4) (set 105)" )

GAME(199?, m4sstrek,  0,		mod4yam    ,mpu4, mpu4_state, m4default, ROT0,   "bootleg","Super Streak (bootleg) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND ) // works?, no sound
