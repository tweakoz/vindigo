/* Scorpion 4 + 5 driver releated includes */
/* mainly used for stuff which is currently shared between sc4 / 5 sets to avoid duplication */

#define sc_adjb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008699.bin", 0x0000, 0x100000, CRC(6d49b54a) SHA1(d64caa1dae40c3bf536f8739d6995b589c7184b7) ) \
	ROM_LOAD( "95008700.bin", 0x0000, 0x100000, CRC(49c37b2a) SHA1(5a59a540becb8c83288ae3a15cfac069fd792e74) ) \

#define sc_adsnt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008478.bin", 0x0000, 0x100000, CRC(d8aa3daf) SHA1(dafeb014f04179882921622df9b5c9917279bd02) ) \
	ROM_LOAD( "95008479.bin", 0x0000, 0x100000, CRC(f1ec9928) SHA1(cd2eb8115217c205f248564c9340353c84db2ac5) ) \

#define sc_adwta_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_bpfpb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008771.bin", 0x0000, 0x100000, CRC(121b6db2) SHA1(634f435635fcb08013729629fc8f56a4180c2034) ) \
	ROM_LOAD( "95008772.bin", 0x0000, 0x100000, CRC(c8813870) SHA1(5c4571a3ca9a06c49e80c5e10e9fd288de560324) ) \

#define sc_btiab_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_btrip_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_bpb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008553.bin", 0x0000, 0x100000, CRC(f8019fb2) SHA1(aa5baed73538ebc6e39c6e35437b77a5cc22135e) ) \
	ROM_LOAD( "95008554.bin", 0x0000, 0x100000, CRC(8e00a011) SHA1(821686d96ee6fb45ab0b771934ce25ab7a230c0c) ) \

#define sc_bull_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008520.bin", 0x0000, 0x100000, CRC(ab418ce0) SHA1(39afbd449a21a31016acaffcfb4f6d684db300e7) ) /* PR1702 BULLSEYE */ \
	ROM_LOAD( "95008521.bin", 0x0000, 0x100000, CRC(7fd0f70c) SHA1(e6bf85b9e2528dac5811277ee89599e58e6358b0) ) \
	ROM_LOAD( "95008544.bin", 0x0000, 0x100000, CRC(19777f20) SHA1(da0cc08b00f9ab565a0e559761adbf7bcd7c327b) ) /* PR1743 CASINO BULLSEYE */ \
	ROM_LOAD( "95008575.bin", 0x0000, 0x100000, CRC(715901d2) SHA1(a7d3d0c7b9fc3eefb0df372cdf0cae7abe8bd959) ) /* PR1758 BULLSEYE TRIPLE */ \
	ROM_LOAD( "95009116.bin", 0x0000, 0x100000, CRC(5a01e206) SHA1(38ee3706d9edb68d17c90bb627aa17b4f908d721) ) /* PR3318 BULLSEYE */ \
	ROM_LOAD( "95009117.bin", 0x0000, 0x100000, CRC(c3623829) SHA1(92b0cf89678023bb02394699e2e214389317ec6c) ) \

#define sc_cabin_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008902.bin", 0x0000, 0x0e7fd3, CRC(c7b0f675) SHA1(e19d4e65d73329a58b9cbc6828c7e0c287f2906a) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890306.bin", 0x0000, 0x5000, CRC(eff2e8d8) SHA1(0749d543bf760b2037330ade671c7ce7ed64b275) ) \
	ROM_LOAD( "95890307.bin", 0x0000, 0x5000, CRC(9a64acec) SHA1(51c49e8d415d99e61fda99ade43bb925d99f3825) ) \
	ROM_LOAD( "95890308.bin", 0x0000, 0x5000, CRC(04de60b0) SHA1(cee280d9528f9e167be1c373e7a99f1f2f1bc1c0) ) \

#define sc_cclim_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008243.bin", 0x0000, 0x100000, CRC(b21e073b) SHA1(dfe309490c5c69052041c2121258c17b974f88ed) ) \

#define sc_cinv_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_crotr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009049.bin", 0x0000, 0x100000, CRC(10c94788) SHA1(8fb5113036a0122a4dbc9af15b600ffe99e51094) ) \
	ROM_LOAD( "95009050.bin", 0x0000, 0x100000, CRC(5e47e86c) SHA1(9aaca1390852f9079cd355c48b889fb5962c5c09) ) \

#define sc_crsgc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_crsgr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009139.bin", 0x0000, 0x100000, CRC(f2e4fa3c) SHA1(1909de32f232e5bfb3ee3c624f29c585cca254c2) ) \
	ROM_LOAD( "95009140.bin", 0x0000, 0x100000, CRC(81bdf47f) SHA1(49c02fb87a4ffc1be4c748c469f5518c314b1391) ) \

#define sc_copsr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "cops_rob_snd.bin", 0x0000, 0x080000, CRC(e25bddb7) SHA1(96d6be2407bf876ad685bdc13d96585782ad4a50) ) \
	ROM_LOAD( "95008393.bin", 0x0000, 0x100000, CRC(693cd9be) SHA1(e965b2dcd6d6619815c919b7d2c372c726abfea3) ) \
	ROM_LOAD( "95008787.bin", 0x0000, 0x100000, CRC(82713d8b) SHA1(317cb1c482bd13e1cad2e5739c99f6e3ca163684) ) \
	ROM_LOAD( "95008846.bin", 0x0000, 0x080000, CRC(44f9eaab) SHA1(394bd36121c33aba97bc957b3d46d2944c1daeed) ) \
	ROM_LOAD( "95009130.bin", 0x0000, 0x100000, CRC(f6813371) SHA1(bd9e5d009c7870d50149d9de396f363fb19b183c) ) \

#define sc_corst_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009037.bin", 0x0000, 0x100000, CRC(c2d3b6ee) SHA1(36e7e9c956ab91fd1b2e52800b4482ce3a3597f5) ) \
	ROM_LOAD( "95009038.bin", 0x0000, 0x100000, CRC(45dc4df3) SHA1(7710a0ba9bbe7ae6f590b61bbc1395cd408926d2) ) \

#define sc_derby_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008129.bin", 0x0000, 0x100000, CRC(4892cf07) SHA1(9ce745f4b3aee623a7855e2fee4cc35de20646a3) ) \
	ROM_LOAD( "95008130.bin", 0x0000, 0x100000, CRC(b9bfde2a) SHA1(25329d5b93318586095715111fbbb854bba394b9) ) \

#define sc_ddosh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008299.bin", 0x0000, 0x100000, CRC(fee81425) SHA1(188cb7275963193a39571d3e4da3b9f6693a79d0) ) \

#define sc_dhh_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008586.bin", 0x0000, 0x100000, CRC(43fde3ad) SHA1(662402449b025bbf75a10c3b1ac746d9359b0b0d) ) \
	ROM_LOAD( "95008587.bin", 0x0000, 0x100000, CRC(6f7a607f) SHA1(da3c8aaa69b30f10f122a9bde49d987dd0950474) ) \

#define sc_dmine_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008293.bin", 0x0000, 0x100000, CRC(acbed8e1) SHA1(607dafa2215632d02c34e65b79bf1341b981f63b) ) \

#define sc_dnd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* Regular */ \
	ROM_LOAD( "95008606.bin", 0x0000, 0x100000, CRC(c63c8663) SHA1(4718baf87340fe93ccfe642a1a0cdb9d9dcac57f) ) /* 0 (1907)  DOND SOUNDS 11 */ \
	ROM_LOAD( "95008607.bin", 0x0000, 0x100000, CRC(a615514f) SHA1(d4ff7d4fe6f1dd1d7b00cc504f90b2921aa5e8fb) ) /* 1 */ \
	\
	/*  Casino */ \
	ROM_LOAD( "95008631.bin", 0x0000, 0x100000, CRC(7208854a) SHA1(a02de60cfcbafe5da4f67596ab65237f5b5f41b7) ) /* 0 (1954) DOND SOUNDS 11 */ \
	\
	ROM_LOAD( "95008632.bin", 0x0000, 0x100000, CRC(fd81a317) SHA1(1e597fd58aab5a7a8321dc4daf138ee07b42c094) ) /* 0 (1945) DOND SOUNDS 11 */ \
	ROM_LOAD( "95008633.bin", 0x0000, 0x100000, CRC(a7a445d4) SHA1(dbb1938c33ce654c2d4aa3b6af8c210f5aad2ae3) ) /* 1 */ \
	\
	/*  Casino */ \
	ROM_LOAD( "95008661.bin", 0x0000, 0x100000, CRC(2d9ebcd5) SHA1(d824a227420cbe616aca6e2fd279af691ddfd87a) ) /* 0 (1945) DOND SOUNDS 12 */ \
	\
	ROM_LOAD( "95008680.bin", 0x0000, 0x100000, CRC(9bd439d1) SHA1(5e71d04e5697e92998bae28f7352ea7742cafe07) ) /* 0 (1964) DOND SOUNDS 11 */ \
	\
	ROM_LOAD( "95008698.bin", 0x0000, 0x100000, CRC(8eea7754) SHA1(7612c128d6c062bba3477d55aee3089e1255f61e) ) /* 0 (1964) DOND SOUNDS 12 */ \


#define sc_dndbb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008788.bin", 0x0000, 0x100000, CRC(51384d64) SHA1(3af87bcc7fb4881ae671d9cbb484fb6a2a534cce) ) \
	ROM_LOAD( "95008789.bin", 0x0000, 0x100000, CRC(4f2d72c3) SHA1(f167ad8d7178682833e7e9efcc393b5826108d70) ) \

#define sc_dndbo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009158.bin", 0x0000, 0x100000, CRC(27f11b81) SHA1(f2690c2828ad09d73cb14c5c1cb53c2ad1fc7b53) ) \
	ROM_LOAD( "95009159.bin", 0x0000, 0x100000, CRC(95e278c8) SHA1(7d85b6dbe3ccacd6c5a5b4eb475598a3d5548ce0) ) \

#define	sc_dndbl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_dndbq_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95891057.bin", 0x0000, 0x5000, CRC(75c0cf91) SHA1(20e7fb3a279cc127cec08a78b6633852b0a950f7) ) \
	ROM_LOAD( "95891058.bin", 0x0000, 0x5000, CRC(69163f2d) SHA1(66b6b593126c452f01b36c03e82d003860340025) ) \
	ROM_LOAD( "95891059.bin", 0x0000, 0x5000, CRC(aa4c1dbd) SHA1(4ebd1b132a4125bdcf58d5867cffb821e3e7b1a3) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008887.bin", 0x0000, 0x100000, CRC(53cb9dc5) SHA1(66cdf8624736ea5a4e239bdb81fc96366ec4623e) ) \

#define sc_dndcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* club dond */ \
	ROM_LOAD( "95008629.bin", 0x0000, 0x100000, CRC(6f7cf500) SHA1(bf3db728872251116b32aa2c44f72a87d29b13d2) ) \
	ROM_LOAD( "95008630.bin", 0x0000, 0x100000, CRC(5dfda41e) SHA1(6d61d0eedc6a1599777f47d3880689f954f326c9) ) \
	/* beat the banker */ \
	ROM_LOAD( "95008713.bin", 0x0000, 0x100000, CRC(7311db5e) SHA1(24a4b3f44fa6dbbe756b0acac2b513c6e92cdf26) ) /* R3068? */ \
	ROM_LOAD( "95008714.bin", 0x0000, 0x100000, CRC(c6ebc845) SHA1(6a135f42940b8c44422736650e369fa12b51e0df) ) \

#define sc_dndcr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009033.bin", 0x0000, 0x100000, CRC(4e167844) SHA1(d0e7101fe033bc9fc0e279c3d457a7da8e8aab00) ) \
	ROM_LOAD( "95009034.bin", 0x0000, 0x100000, CRC(a3e7b40a) SHA1(09f1fde693750f15e78da91c796e8593d06d19e9) ) \

#define sc_dnddd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	/* dond double */ \
	ROM_LOAD( "95008804.bin", 0x0000, 0x100000, CRC(19fd19fa) SHA1(db68a40d8bb3de130c6d5777217fb3c3a4c13d17) ) \
	ROM_LOAD( "95008805.bin", 0x0000, 0x100000, CRC(f497337d) SHA1(9e827116f8b1d882cfcd0b56aaede9de14b2e943) ) \

#define sc_dndde_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009053.bin", 0x0000, 0x100000, CRC(f06acb47) SHA1(043756eda04d193f7365f2fd0fafc36c3b6b0bd9) ) \
	ROM_LOAD( "95009054.bin", 0x0000, 0x100000, CRC(595baae8) SHA1(9aa03b265294eaed0051b0a25cc9a0e1481b4390) ) \

#define sc_dnddi_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009191.bin", 0x0000, 0x100000, CRC(2f3cda1a) SHA1(017533caadf86d0d70e1b57b697621e0b4a8fef2) ) \
	ROM_LOAD( "95009192.bin", 0x0000, 0x100000, CRC(eb9b6b0b) SHA1(127bb283b32a997c04a1b29c18f4093f4d969151) ) \

#define sc_dnddt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009106.bin", 0x0000, 0x100000, CRC(afa266cf) SHA1(adc80bc677a7524bec4bbd102a4a5ff293571abc) ) \
	ROM_LOAD( "95009107.bin", 0x0000, 0x100000, CRC(493981a8) SHA1(77308e453848d092d63330e10e3b1e88caeb4fb5) ) \

#define sc_dnddo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009120.bin", 0x0000, 0x100000, CRC(327c01a0) SHA1(9b7fa28ff6ecc83c2e9ee73aac0af98be170b28f) ) \
	ROM_LOAD( "95009121.bin", 0x0000, 0x100000, CRC(d7991c7e) SHA1(4f4e4043f272ff2325b0fa64c66b34cb1aefb97c) ) \

#define sc_dndfl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008847.bin", 0x0000, 0x100000, CRC(7f3eae56) SHA1(f5d25c87caf76fc1961c8fb5c389bb73cefbcb28) ) \
	ROM_LOAD( "95008848.bin", 0x0000, 0x100000, CRC(ba00c16d) SHA1(f2e55441857e5f036e19d2ee29dfad97b99134b8) ) \

#define sc_dndgo_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009197.bin", 0x0000, 0x100000, CRC(bdf253cb) SHA1(6f046f1c27bae1141919874b27a8d87295b39261) ) \
	ROM_LOAD( "95009198.bin", 0x0000, 0x100000, CRC(420105d4) SHA1(23e4eac4146a3985c3a7dc5f80f1b5900a116303) ) \

#define sc_dndgl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009102.bin", 0x0000, 0x100000, CRC(9d03cd95) SHA1(e062c816a19af0dc86bfa5a66f69342e69fffcfa) ) \
	ROM_LOAD( "95009103.bin", 0x0000, 0x100000, CRC(51ae4095) SHA1(b397984d3b0beb4ca95050ff595648a80e5ea87a) ) \

#define sc_dndld_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009184.bin", 0x0000, 0x100000, CRC(25a36077) SHA1(92c05f410456414b825b09338a474c52f7c1bd9b) ) \
	ROM_LOAD( "95009185.bin", 0x0000, 0x100000, CRC(eacdfc26) SHA1(8ab462e7f49ca9374b9245d9d97cf909ea979ae3) ) \

#define sc_dndmb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009156.bin", 0x0000, 0x100000, CRC(7c68c70c) SHA1(eb4006e185f3cd4e7d66999b3ff064a7903f5bc8) ) \
	ROM_LOAD( "95009157.bin", 0x0000, 0x100000, CRC(a0541504) SHA1(689ba32fd1b1a9592a9cd7039424655a652b1bcd) ) \

#define sc_dndmd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009170.bin", 0x0000, 0x100000, CRC(6ab363a5) SHA1(1a98566161aec187074cbbdb7372ed8484202639) ) \
	ROM_LOAD( "95009171.bin", 0x0000, 0x100000, CRC(bc5d3113) SHA1(836eff78265f6d0371be18467c5edc6d32aba6d5) ) \

#define sc_dndpa_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009150.bin", 0x0000, 0x100000, CRC(54d9f01a) SHA1(6d62ca07443c42e16243a843dee6a11c7a1383fc) ) \
	ROM_LOAD( "95009151.bin", 0x0000, 0x100000, CRC(89bb7543) SHA1(e8bea8a00798329af590be5e2ccbfcc54f70f98d) ) \

#define sc_ddptg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009041.bin", 0x0000, 0x100000, CRC(143d05fb) SHA1(f3d2735dc5b1bdc4e3f4205521f1b614d4b4c4b0) ) \
	ROM_LOAD( "95009042.bin", 0x0000, 0x100000, CRC(65efd8fd) SHA1(c7f2d1f6b31767a56269f53e9327de3abe64218f) ) \

#define sc_ddply_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008830.bin", 0x0000, 0x100000, CRC(bb66ae0c) SHA1(4a7df180dd67aa96059a83986cb98721a81963d8) ) \
	ROM_LOAD( "95008831.bin", 0x0000, 0x100000, CRC(8a632940) SHA1(e955b05a82312ab356117f348903292d6f26a5a5) ) \

#define sc_dndrr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009188.bin", 0x0000, 0x100000, CRC(a2dceadd) SHA1(af1696bf32f37005c6a01bb209f38b90daab4e29) ) \
	ROM_LOAD( "95009189.bin", 0x0000, 0x100000, CRC(02b0d336) SHA1(7713212ebf59fa2464110b0238a870e81d43be05) ) \

#define sc_dndsi_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008838.bin", 0x0000, 0x100000, CRC(34e9d2eb) SHA1(140f9ccf287e24e3917b1a6bb8d8118bf20f232c) ) \
	ROM_LOAD( "95008839.bin", 0x0000, 0x100000, CRC(4475bb6b) SHA1(864575f9b35495bfe0d8f7563e2d4b5e35b6a396) ) \

#define sc_dndrt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009141.bin", 0x0000, 0x100000, CRC(e53f8ba3) SHA1(10eea3246352bf65e1ca7b4cd6e1a873d6a2f7fe) ) \
	ROM_LOAD( "95009142.bin", 0x0000, 0x100000, CRC(54b47cbd) SHA1(71e8f49b0d031fcc8c5450279dc26d39b90af678) ) \

#define sc_dndpd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009172.bin", 0x0000, 0x100000, CRC(f124785f) SHA1(2291dca731bb6037d655dc8bbdfa7f35f6a4d4d1) ) \
	ROM_LOAD( "95009173.bin", 0x0000, 0x100000, CRC(aae9318c) SHA1(4b53bec7284950fbca4650117e727ab9a01b5509) ) \

#define sc_dndpc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_dndtb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009186.bin", 0x0000, 0x100000, CRC(1da5489b) SHA1(b282e523b52e34a625502ec99ca42f059e701448) ) \
	ROM_LOAD( "95009187.bin", 0x0000, 0x100000, CRC(2c3ea82c) SHA1(50ade918503e224288407abf60b3d53e5334aadb) ) \

#define sc_dndhf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008705.bin", 0x0000, 0x100000, CRC(09e02843) SHA1(a6ac658c857eca2aca0bacda423dd012434e93bc) ) \
	ROM_LOAD( "95008706.bin", 0x0000, 0x100000, CRC(805e7eb8) SHA1(107c6ba26e37d8f2eec81b62c3edf3efb7e44ca2) ) \

#define sc_dndys_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008779.bin", 0x0000, 0x100000, CRC(cf04d115) SHA1(3255b58cd4fba7d231ca35c00d1cb70da1f6cfbd) ) \
	ROM_LOAD( "95008780.bin", 0x0000, 0x100000, CRC(ec270dbe) SHA1(f649ffd4530feed491dc050f40aa0205f4bfdd89) ) \

#define sc_dndww_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )  \
	ROM_LOAD( "95008730.bin", 0x0000, 0x100000, CRC(e2aebdb0) SHA1(d22fee7ff3d5912ea9a7440ec82de52a7d016090) ) /* casino wow */ \

#define sc_dndtr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008832.bin", 0x0000, 0x100000, CRC(1d2aa204) SHA1(52ad7a6ea369198b48d593de508f317ed11e84de) ) \
	ROM_LOAD( "95008833.bin", 0x0000, 0x100000, CRC(92f462f6) SHA1(c0782639d4cd2653a3d14e1b422c2d6117dac029) ) \

#define sc_dndwb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008651.bin", 0x0000, 0x100000, CRC(c3738d54) SHA1(bf3c62e196a2cea144868729f136000a7b924c3d) ) /* wiyb */ \
	ROM_LOAD( "95008652.bin", 0x0000, 0x100000, CRC(ba8f5b62) SHA1(2683623e8b90d5e9586295d720aa1d985b416117) )  \
	ROM_LOAD( "95008715.bin", 0x0000, 0x100000, CRC(1796b604) SHA1(9045f9424e6447d696a6fdd7f5bdcbfda4b57c90) ) /* wiyb */ \
	ROM_LOAD( "95008766.bin", 0x0000, 0x100000, CRC(62accb81) SHA1(ed0456cefed2fbf9a1cfd911f871262e892a4ce8) ) /* wiyb */ \
	ROM_LOAD( "95008767.bin", 0x0000, 0x100000, CRC(0aa49d8a) SHA1(a2baffc495656f0b5426b720cac298a78774b7fa) ) /* super slam?? */  \

#define sc_dndpg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008822.bin", 0x0000, 0x100000, CRC(16411f05) SHA1(11f2d17b6647b9d9e713e38bab6604e4feb51432) ) \
	ROM_LOAD( "95008823.bin", 0x0000, 0x100000, CRC(4f7bab89) SHA1(bbb203cff3212571c313e8f22f9083980baf2566) ) \

#define sc_dndtp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008673.bin", 0x0000, 0x100000, CRC(4b8d1e0b) SHA1(8cba9632abb2800f8e9a45d1bf0abbc9abe8cb8c) ) \
	ROM_LOAD( "95008674.bin", 0x0000, 0x100000, CRC(66236e20) SHA1(222fefdeddb22eb290302528a8f937468ccd5698) ) \
	

#define sc_dndbe_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008624.bin", 0x0000, 0x100000, CRC(bf9620ea) SHA1(63f5a209da3d0117fcb579364a53b23d2b02cfe5) ) \
	ROM_LOAD( "95008625.bin", 0x0000, 0x100000, CRC(2e1a1db0) SHA1(41ebad0615d0ad3fea6f2c00e2bb170d5e417e4a) ) \

#define sc_dndbc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009100.bin", 0x0000, 0x100000, CRC(b06110c0) SHA1(84289721a8c71124cc4df79fc90d0ded8d43fd07) ) \
	ROM_LOAD( "95009101.bin", 0x0000, 0x100000, CRC(53b38d0a) SHA1(1da40cbee8a18713864e3a578ac49c2108585e44) ) \

#define sc_dndlp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008792.bin", 0x0000, 0x100000, CRC(2e7e1a5a) SHA1(000cb48c67b7f23d00318d37206c2df426c79281) ) \
	ROM_LOAD( "95008793.bin", 0x0000, 0x100000, CRC(4b91b638) SHA1(b97cb1d22f91a791fc4d47bbae8965882898a317) ) \

#define sc_dndra_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008752.bin", 0x0000, 0x100000, CRC(82bfd510) SHA1(857f294d46d64275b15c56187bbbc19e2aa0f5bc) ) \
	ROM_LOAD( "95008753.bin", 0x0000, 0x100000, CRC(ee0b5da9) SHA1(f6cb6037f525df504c1ba8106f19990ecf8d1bd2) ) \

#define sc_dndbd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008790.bin", 0x0000, 0x100000, CRC(3c56a8b6) SHA1(8c06aa725233f4feb7c2e703f203cf9b0c6669a1) ) \
	ROM_LOAD( "95008791.bin", 0x0000, 0x100000, CRC(7378a4a8) SHA1(af9dd1fcbd8b77fab4afc85a325c0eb011f35e3a) ) \

#define sc_dndbr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008723.bin", 0x0000, 0x100000, CRC(6e1d9bd5) SHA1(92b2fd0b75a195af6ddfbdee3316f8af4bc4eb1a) ) \
	ROM_LOAD( "95008724.bin", 0x0000, 0x100000, CRC(309c1bf1) SHA1(82811b503a06e30a915eebdbdbcd63b567f241c1) ) \

#define sc_dndcc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008746.bin", 0x0000, 0x100000, CRC(ef3c39b9) SHA1(21b9788080de68acd436608d7d46fd9663d1589b) ) /* crazy chair */ \
	ROM_LOAD( "95008747.bin", 0x0000, 0x100000, CRC(9f9b638f) SHA1(5eae49f8a9571f8fade9acf0fc80ea3d70cc9e18) ) \
	\
	ROM_LOAD( "95008840.bin", 0x0000, 0x100000, CRC(2aa17d67) SHA1(43bdee1b3d0a0bf28f672620867fa3dc95727fbc) ) /* crazy chair */ \
	ROM_LOAD( "95008841.bin", 0x0000, 0x100000, CRC(594a9cd7) SHA1(667fa3106327ce4ff23877f2d48d3e3e360848d0) ) \
	\
	ROM_LOAD( "95009131.bin", 0x0000, 0x100000, CRC(59a9e50a) SHA1(15d79b177a1e926cd5aee0f969f5ef8b30fac203) ) /* crazy chair */ \
	ROM_LOAD( "95009132.bin", 0x0000, 0x100000, CRC(5e8a7ca6) SHA1(419ecc6ac15004bdd83bcd5b3e00d9dcd0d24936) ) \

#define sc_dnddw_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008754.bin", 0x0000, 0x100000, CRC(d2c3e3e5) SHA1(3d21c812456618471a331d596760ea8746afc036) ) \
	ROM_LOAD( "95008755.bin", 0x0000, 0x100000, CRC(e6906180) SHA1(81215fd3dfe315123d5b028047a93e30baa52b5d) ) \

#define sc_dnddf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008738.bin", 0x0000, 0x100000, CRC(2a06f488) SHA1(666bd9c8091edda4b003fa586e5ae270831c119f) ) \
	ROM_LOAD( "95008739.bin", 0x0000, 0x100000, CRC(33d677b0) SHA1(5283d23671e340e3fda4eb4c6795d1d56b073206) ) \

#define sc_dough_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008505.bin", 0x0000, 0x100000, CRC(e18549be) SHA1(7062f05b6245c4188ed68a51dc7fbc8b138698f4) ) \

#define sc_ducks_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890347.bin", 0x0000, 0x5000, CRC(9faa9c11) SHA1(5e2add80ad17a27dd7b5b0a298ea6753b6adca11) ) \
	ROM_LOAD( "95890348.bin", 0x0000, 0x5000, CRC(ea3cd825) SHA1(0ecef82b32b0166ef5f8deecd7a85fd52fc66bf8) ) \
	ROM_LOAD( "95890349.bin", 0x0000, 0x5000, CRC(74861479) SHA1(7ba29931336985b96995688afdfe2b83a7602a84) ) \

#define sc_emmer_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008973.bin", 0x0000, 0x0fc2ce, CRC(a6ef4f4c) SHA1(4749e008e8cd0a7f92da85fcbf1a9dfd9fd5e721) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890452.bin", 0x0000, 0x5000, CRC(193927ce) SHA1(fe02d1d1519139199fbb743ce0e67b810261e9a2) ) \
	ROM_LOAD( "95890453.bin", 0x0000, 0x5000, CRC(6caf63fa) SHA1(86657e0e14eac91f91bd4da4c04b88c8ee28de45) ) \
	ROM_LOAD( "95890454.bin", 0x0000, 0x5000, CRC(f215afa6) SHA1(0e28affa6557474856087dbeeb0a3127616b7fe6) ) \

#define sc_evol_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "evolsnd.p1", 0x0000, 0x100000, CRC(13e18cc3) SHA1(114301af1abf86cdd9c2f01b316d257bb1ca086d) ) \
	ROM_LOAD( "evolsnd.p2", 0x0000, 0x08399a, CRC(345f3f11) SHA1(dd4d4f0f32650304cc6010b8f059c23dd5535919) ) \

#define sc_fbspn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008526.bin", 0x0000, 0x100000, CRC(81890751) SHA1(1e9cedc07c9028eb65620371d9fcf73ae84ad274) ) \

#define sc_fguy_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008813.bin", 0x0000, 0x100000, CRC(e5548d68) SHA1(ac20af71cedd0c6d61d6a28afa8d700c1446e442) ) \
	ROM_LOAD( "95008814.bin", 0x0000, 0x100000, CRC(06f3165c) SHA1(936a8c1d31bca93a5b985e89c48a1faaef602ae0) ) \

#define sc_fmj_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008982.bin", 0x0000, 0x0fe74c, CRC(c5ce4a41) SHA1(abbecddba4dd4691240df112138fa95f1e125a85) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890477.bin", 0x0000, 0x5000, CRC(a7dd60c5) SHA1(dfa3a89939cec2225d975984caf4703d83708b85) ) \
	ROM_LOAD( "95890478.bin", 0x0000, 0x5000, CRC(d24b24f1) SHA1(0c3a9c9579bb1066ea3e82e58b65d3bcac7fe900) ) \
	ROM_LOAD( "95890479.bin", 0x0000, 0x5000, CRC(4cf1e8ad) SHA1(924b454aa9c7340374b98201fbbaf922b836d779) ) \

#define sc_gd_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008187.bin", 0x0000, 0x100000, CRC(1fbbc7cc) SHA1(6e19b582a3654bbbcf65b8f42bd556029f43e8cb) ) \
	ROM_LOAD( "95008188.bin", 0x0000, 0x100000, CRC(a74b23a7) SHA1(f7948edfb5524eaf80039c07ca7a9f76883f5a6f) ) \
	ROM_LOAD( "95009061.bin", 0x0000, 0x100000, CRC(c4cad720) SHA1(ce67af8d9e0b2f4f79d38b7a01dfc5ff7323e162) ) \
	ROM_LOAD( "95009062.bin", 0x0000, 0x100000, CRC(509761d3) SHA1(6a133cc33bac6a1696de98a4961572a86cefc1c8) ) \

#define sc_gdmz_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008980.bin", 0x0000, 0x0fe431, CRC(c09277a0) SHA1(227609194f7d7e8b00cbd5cb94f8679f8fff71c6) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890471.bin", 0x0000, 0x5000, CRC(98fce599) SHA1(559534c3b33c6390a06b2e97e4c595055c8c4fea) ) \
	ROM_LOAD( "95890472.bin", 0x0000, 0x5000, CRC(ed6aa1ad) SHA1(83b69606398a30cb9c2ebc2f7406ef3215afab9e) ) \
	ROM_LOAD( "95890473.bin", 0x0000, 0x5000, CRC(73d06df1) SHA1(6959ee1b1c05398bc95430fea3590e5a86dd4312) ) \

#define sc_gdclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008248.bin", 0x0000, 0x100000, CRC(abfba7ee) SHA1(95b61b34080196d745cc00ae32a333edb111da89) ) \

#define sc_glad_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_hapnt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008290.bin", 0x0000, 0x100000, CRC(427fc43d) SHA1(c63154c7f2bb3e7ec78e31268a8040d0eb131f2f) ) \
	ROM_LOAD( "95008312.bin", 0x0000, 0x100000, CRC(cf33e14d) SHA1(33382be257a145784d9de7050b5c90f725241de4) ) \

#define sc_hotdg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_lotrf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008251.bin", 0x0000, 0x100000, CRC(2af25e80) SHA1(fb606e434ac99eee6e114d6639f28f0fa1e7ffb3) ) \
	ROM_LOAD( "95008252.bin", 0x0000, 0x100000, CRC(fbca1cfb) SHA1(27c610af14b10d1d72774186fd3afc12d5db925e) ) \

#define sc_lotr2_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008320.bin", 0x0000, 0x100000, CRC(8ba801e3) SHA1(4d6a009c2f4f4478276f1227af6315b4be90fc87) ) \
	ROM_LOAD( "95008321.bin", 0x0000, 0x100000, CRC(42482ddc) SHA1(304961f267b85bc599dd64c97c14f34b2913dd59) ) \

#define sc_ltr2c_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008342.bin", 0x0000, 0x100000, CRC(44f48bf6) SHA1(2b36fb9ca6eb9cb35cf67e580d736c711d96ea25) ) \
	ROM_LOAD( "95008343.bin", 0x0000, 0x100000, CRC(f35a7927) SHA1(f2a93c8ff4699c7e6572a7a43b6b2bc50683594c) ) \

#define sc_lotrt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008517.bin", 0x0000, 0x100000, CRC(8092eaeb) SHA1(fea226ea7fb97226c79132aa67b89078f67cd920) ) \
	ROM_LOAD( "95008518.bin", 0x0000, 0x100000, CRC(6ac28d4e) SHA1(cdf50c856324130156115259efbfb7553480c749) ) \

#define sc_lotrr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008413.bin", 0x0000, 0x100000, CRC(66d6fd58) SHA1(dff5cc3f09fb4082fde4d5d4a9ecfe00cbce7242) ) \
	ROM_LOAD( "95008414.bin", 0x0000, 0x100000, CRC(8ec846ac) SHA1(3f66cbe5bc7190406425488beccb3e1d15356b1a) ) \

#define sc_luck7_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "lucky7s.bin", 0x0000, 0x200000, CRC(93002e7d) SHA1(50edf9064fb3bc9fd13efc65be6b12a4329fdb41) ) \

#define sc_mogta_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95009039.bin", 0x0000, 0x100000, CRC(0756b1b7) SHA1(82a8f9bf4c70a492ae2d4f478ecc73cccf9aecdd) ) \
	ROM_LOAD( "95009040.bin", 0x0000, 0x100000, CRC(9a928ef4) SHA1(5b19f70d067540aef3dc140115c12601d2b8176e) ) \

#define sc_monsp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004291.snd", 0x0000, 0x080000, CRC(0ea304f7) SHA1(3d524e108cd12e83956fec3997316a92a9e0898d) ) \
	ROM_LOAD( "95008183.bin", 0x0000, 0x080000, CRC(ab29f841) SHA1(908603184c3f7ead278dabd224d60e5362cdafbe) ) \
	ROM_LOAD( "95008184.bin", 0x0000, 0x080000, CRC(45ef95ea) SHA1(1e1126b542621e723eaf6e38918162fe701247e0) ) \

#define sc_monop_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008391.bin", 0x0000, 0x0ec7b3, CRC(fad36ca8) SHA1(67918353945429183befba56ab891a1d138f2e8b) ) \
	ROM_LOAD( "95008514.bin", 0x0000, 0x0ed840, CRC(2608cbde) SHA1(4831570fbc825592781e6473173d6ec911a0054d) ) \

#define sc_mcas_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008436.bin", 0x0000, 0x0e6369, CRC(32e8df77) SHA1(fa044287970d4e54af53ff492eabc50afa9a7a92) ) \
	ROM_LOAD( "95008991.bin", 0x0000, 0x0ec7b3, CRC(fad36ca8) SHA1(67918353945429183befba56ab891a1d138f2e8b) ) \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890727.bin", 0x0000, 0x5000, CRC(10ec3f00) SHA1(986bfce992e113bab26eea8d1981905a54367b94) ) \
	ROM_LOAD( "95890728.bin", 0x0000, 0x5000, CRC(0c3acfbc) SHA1(ff39c8236d5bd44c7d238edd1827af5aebc46e5c) ) \
	ROM_LOAD( "95890729.bin", 0x0000, 0x5000, CRC(cf60ed2c) SHA1(df015b71567d6762b4c8ea6ad6d719cefdfdc60f) ) \

#define sc_mowow_others \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890712.bin", 0x0000, 0x5000, CRC(ec6db00b) SHA1(d16a1527caa3c115e3326c897ce0fa66e3a0420d) ) \
	ROM_LOAD( "95890713.bin", 0x0000, 0x5000, CRC(f0bb40b7) SHA1(33c19dab3086cdeae4f503fbf3f3cc5f0dad98c4) ) \
	ROM_LOAD( "95890714.bin", 0x0000, 0x5000, CRC(33e16227) SHA1(87efc1a046ef6af0b72cc76a6ee393a4d1ddbce3) ) \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008550.bin", 0x0000, 0x100000, CRC(db6343bf) SHA1(e4d702020af67aa5be0560027706c1cbf34296fa) ) \
	ROM_LOAD( "95008551.bin", 0x0000, 0x100000, CRC(2d89a52a) SHA1(244101df7f6beae545f9b823750f908f532ac1e4) ) \
	ROM_LOAD( "95008850.bin", 0x0000, 0x0af41f, CRC(8ca16e09) SHA1(9b494ad6946c2c7bbfad6591e62fa699fd53b6dc) ) \
	ROM_LOAD( "95008869.bin", 0x0000, 0x0b9d9d, CRC(f3ef3bbb) SHA1(92f9835e96c4fc444a451e97b2b8a7b66e5794b7) ) \

#define sc_nunsm_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008522.bin", 0x0000, 0x0f9907, CRC(df612d06) SHA1(cbca56230c4ad4c6411aa5c2e2ca2ae8152b5297) ) \

#define sc_rich_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008084.lo", 0x000000, 0x100000, CRC(2231c842) SHA1(2180109949281b053313c8a8de33496d31d4279e) ) \
	ROM_LOAD( "95008085.hi", 0x100000, 0x100000, CRC(eb748aa3) SHA1(b81acfbceb5ff9df8bb30d3da82deea70bfce2fe) ) \

#define sc_srr_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008213.bin", 0x0000, 0x100000, CRC(e8f82b1d) SHA1(9357f587d638289b2cd5029e5895f69097d69089) ) \

#define sc_tpsht_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_tbox_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_typ_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008662.bin", 0x0000, 0x100000, CRC(51959c94) SHA1(8a4b68460da4223071e33158747ae0ea18b83a52) ) \
	ROM_LOAD( "95008663.bin", 0x0000, 0x100000, CRC(839687b2) SHA1(a9fd3645eb903cb52b348dd8313f0d953d5a5ec0) ) \

#define sc_gball_pthers \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008161.bin", 0x0000, 0x080000, CRC(ecd13fd9) SHA1(51d11b9133d4e840ce9afd7cf716520ea0fc0343) ) \
	ROM_LOAD( "95008162.bin", 0x0000, 0x080000, CRC(b4b4a5c5) SHA1(d0748decfaee7da52d2f6a4bc0877be4243ed6fb) ) \
	ROM_LOAD( "95008465.bin", 0x0000, 0x100000, CRC(5d1fa2c9) SHA1(c12de2b89f0bcb8f1b35630fffd205fd9d5b9777) ) \
	ROM_LOAD( "95008466.bin", 0x0000, 0x100000, CRC(418068ab) SHA1(342939e9bcc1d213bc2f52666cc3765442e18635) ) \

#define sc_gfev_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95416146.bin", 0x0000, 0x100000, CRC(17e92fa0) SHA1(7dea5166f3f70e5d249da56f01bbe2267ce43d6a) ) \

#define sc_ggame_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008515.bin", 0x0000, 0x0ab5c3, CRC(70cd8480) SHA1(2da34a0c1d9d84471005f5d6491499e707c0b0d4) ) /* KGXDX */  \
	ROM_LOAD( "95008958.bin", 0x0000, 0x0435d3, CRC(31ffdb64) SHA1(e48cfa2e5b158555b4ba204fc1175810b81cbbed) ) /* MULTIGG */ \
	ROM_LOAD( "95008992.bin", 0x0000, 0x0aa536, CRC(aad10089) SHA1(d8a32f66432ee901be05435e8930d3897f4b4e33) ) /* BARX */ \
	ROM_REGION( 0x5000, "pics", 0 ) \
	ROM_LOAD( "95890410.bin", 0x0000, 0x5000, CRC(9588ae1d) SHA1(ae45d9e0272b2b048b99e337def1acfb2524597e) ) \
	ROM_LOAD( "95890411.bin", 0x0000, 0x5000, CRC(895e5ea1) SHA1(070df49baca709f69fa1e522b21e42b716af0ba5) ) \
	ROM_LOAD( "95890412.bin", 0x0000, 0x5000, CRC(4a047c31) SHA1(aeb969801c89b60a644b4ffc2e1bbb73f6d61643) ) \

#define sc_ggg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008992.bin", 0x0000, 0x0aa536, CRC(aad10089) SHA1(d8a32f66432ee901be05435e8930d3897f4b4e33) ) \
	ROM_LOAD( "95009015.bin", 0x0000, 0x0b7821, CRC(8b6ca362) SHA1(4c50935b6d1038738ce631fbdc359416197b8c03) ) \
	ROM_REGION( 0x400000, "pivs", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95890730.bin", 0x0000, 0x5000, CRC(9673d1ce) SHA1(fee90139fc8de5e7b6dfe741b4852a363c17eb93) ) \
	ROM_LOAD( "95890731.bin", 0x0000, 0x5000, CRC(8aa52172) SHA1(441a649e3da00556a8ea966a88ee9b58b4943d3b) ) \
	ROM_LOAD( "95890732.bin", 0x0000, 0x5000, CRC(49ff03e2) SHA1(f8b77c97f2b1bb5de06f4c9835275ae9b83d0988) ) \

#define sc_ggcas_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_gnc_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_hellb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004211.bin", 0x0000, 0x080000, CRC(2e729642) SHA1(c8dcdce52f930b3fa894c46907691a28a5499a16) ) \
	ROM_LOAD( "95008341.bin", 0x0000, 0x100000, CRC(486e5395) SHA1(0ad68f271f4839d50a790b7f4427e1f1f1933bd4) ) \

#define sc_leg_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008093.bin", 0x0000, 0x100000, CRC(86e27465) SHA1(1d27358fe795286676a8031382387ff4225f7118) ) \
	ROM_LOAD( "95008094.bin", 0x0000, 0x100000, CRC(08909b00) SHA1(3e5b3da186036d7fe67ed2739de6fba79d4a978a) ) \

#define sc_legcb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008197.bin", 0x0000, 0x100000, CRC(234ff677) SHA1(571c2fa2b5a24d07e90e43061d9947f64874e482) ) \
	ROM_LOAD( "95008198.bin", 0x0000, 0x100000, CRC(4b03df47) SHA1(13c24cc90a618ebc7c150ab3694a1b787fb049d6) ) \

#define sc_luckb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 )  \
	ROM_LOAD( "95008161.bin", 0x0000, 0x080000, CRC(ecd13fd9) SHA1(51d11b9133d4e840ce9afd7cf716520ea0fc0343) ) /* golden balls */ \
	ROM_LOAD( "95008162.bin", 0x0000, 0x080000, CRC(b4b4a5c5) SHA1(d0748decfaee7da52d2f6a4bc0877be4243ed6fb) ) \
	ROM_LOAD( "95008167.bin", 0x0000, 0x080000, CRC(13aaf063) SHA1(70ab005c867c0c8f63b16722f4fc3ba0c935a96f) ) /* lucky balls */ \
	ROM_LOAD( "95008168.bin", 0x0000, 0x080000, CRC(71aebb68) SHA1(479b3915552be029d459d3f05ccf668c21f05554) ) \


#define	sc_manic_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008684.bin", 0x0000, 0x100000, CRC(52b7d26e) SHA1(ba6629dbad8d00c132c0ddf6a8a41ddc99231c75) ) \
	ROM_LOAD( "95008685.bin", 0x0000, 0x100000, CRC(dc9717c0) SHA1(27234bb7c7e7bd1f395972ce2958d55e84005313) ) \

#define sc_pacmn_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004183", 0x000000, 0x080000, CRC(4b28f610) SHA1(fff01c890a8c109bb4b522ee2391c15abdc2758c) ) \
	ROM_LOAD( "95004184", 0x080000, 0x080000, CRC(bee11fdd) SHA1(b5ce97108812e296c92a000444c1fb7a11286de4) ) \
	ROM_LOAD( "95004199.lo", 0x000000, 0x080000, CRC(3a9605c8) SHA1(ce1c94fe26eac9e145e94539f62f2bde740e5b9a) ) \
	ROM_LOAD( "95004199.hi", 0x080000, 0x080000, CRC(0ecfc531) SHA1(15e20eedf4b7d9102c40834612d111559b4dcbca) ) \
	ROM_LOAD( "97000000.evn", 0x000000, 0x080000, CRC(5b13fe7b) SHA1(1bd32e577914ab4e3bc3282261f8c3cdf015b85d) ) /* pacp */ \
	ROM_LOAD( "97000000.odd", 0x080000, 0x080000, CRC(8bab1c78) SHA1(ddc915a8c56473ba4d67d8c62c66105dd622b593) ) \

#define sc_paccs_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_paccl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008145.bin", 0x0000, 0x100000, CRC(e2ec54f8) SHA1(bb1c40b13151ed1e3c7ba603506701457392bb8b) ) \
	ROM_LOAD( "pacmancsnd.bin", 0x0000, 0x0bbb33, CRC(c505aa18) SHA1(a99bd1c4101269e2eb2b6becf210d9991fee1da1) ) \

#define sc_pacpl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008049", 0x0000, 0x0ff7f5, CRC(efb3c1aa) SHA1(345ad862a329eedf7c016ddd809c8b60185d9962) ) \

#define sc_pmani_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008201.bin", 0x0000, 0x100000, CRC(4e0358c5) SHA1(b9d8b78c77f87eebb9408a4ea1b9fd3a64ae724d) ) \

#define sc_polem_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "pole_p.sn1", 0x000000, 0x080000, CRC(cbb7b019) SHA1(1404ff5fe2c3e54370a79ea141617e58e56217e9) ) \
	ROM_LOAD( "pole_p.sn2", 0x080000, 0x080000, CRC(95d9939b) SHA1(29d03f2f2d33a807df002271e14b614bae3d10e4) ) \

#define sc_polen_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "polesnd1.bin", 0x0000, 0x080000, CRC(09f720e3) SHA1(1a5c581b16b974e949679c3d71696984755f3c7c) ) \
	ROM_LOAD( "polesnd2.bin", 0x0000, 0x080000, CRC(80c2702b) SHA1(6c7b1d535d8f1eeb25a1a84ccf9e97b3e453af6d) ) \

#define sc_pog_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008750.bin", 0x0000, 0x100000, CRC(25a94ab7) SHA1(9ee3a1acb2734a5663d925fa050c15d154f79678) ) \
	ROM_LOAD( "95008751.bin", 0x0000, 0x100000, CRC(53c97577) SHA1(5ec3868967073b4ed429fed943fbcd568a85b4f3) ) \

#define sc_rt_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008203_1.bin", 0x000000, 0x100000, CRC(778a18e5) SHA1(2ad4d0259f08786f50928064a4d345ffa6bb52e2) ) \
	ROM_LOAD( "95008204_2.bin", 0x100000, 0x100000, CRC(ef036383) SHA1(cb26a334fe043c6aba312dc6d3fe91bd93e0cb2f) ) \


#define sc_rtclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_sace_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008270.bin", 0x0000, 0x0fde91, CRC(5c87d4ce) SHA1(6fce212a89334a9595411aa2d20bde5e3fae4710) ) \

#define sc_sf_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_starp_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "star prize snd1.bin", 0x0000, 0x080000, CRC(9f7f7442) SHA1(576e5e92455455ad18ae596a9dae500a17912faa) ) \
	ROM_LOAD( "star prize snd2.bin", 0x0000, 0x080000, CRC(0ae992b5) SHA1(b3a8ba472aa3a6785678a554a7fe789f9f3dc6c0) ) \
	ROM_REGION( 0x400000, "altymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "sprizesn.1", 0x0000, 0x080000, CRC(5a9a9903) SHA1(ac79fedeaa1b37661d0ec28bf6c5b141c7fcc328) ) \

#define sc_tetri_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008097_1.bin", 0x0000, 0x100000, CRC(01a251eb) SHA1(0cb63736065b97956f02710867ac11609b1f282c) ) \
	ROM_LOAD( "95008098_2.bin", 0x0000, 0x100000, CRC(5ebcfd20) SHA1(be415d965732b3fde47684dacdfe93711182faf4) ) \

#define sc_trail_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "trailblazer.bin", 0x0000, 0x0f6a6c, CRC(c27b5f9d) SHA1(24ac4ae0bec9a898690dfa2a78f765026d58255b) ) \

#define sc_ttpie_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008579.bin", 0x0000, 0x100000, CRC(b3741823) SHA1(77890bf89b848fa2222e885aeb51f05f033143ba) ) \

#define sc_vivam_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95004246.bin", 0x0000, 0x080000, CRC(4e5a0143) SHA1(a9e668aceb21671a78b584fd55c21d5501ea1f8a) ) \
	ROM_LOAD( "95004247.bin", 0x0000, 0x080000, CRC(4a35b6a1) SHA1(24e4ed93149b7f6caf785ecaa5a4685585a36f5b) ) \
	ROM_LOAD( "95008719.bin", 0x0000, 0x100000, CRC(f99eafef) SHA1(7a7dedf5bf8292b94ac6da78478441335bfcb66f) ) \
	ROM_LOAD( "95008720.bin", 0x0000, 0x100000, CRC(6419033f) SHA1(3d222696dd9f4f201596fd444a031690cc8c1b0d) ) \
	ROM_LOAD( "viva_mex.sn1", 0x0000, 0x080000, CRC(ed357575) SHA1(610047e83062fd4a6d23d9b9281a3a6c04d63f7d) ) \
	ROM_LOAD( "viva_mex.sn2", 0x0000, 0x080000, CRC(aab9421d) SHA1(db2941e013597be3fe05a2b002aebdfb0f7a2b41) ) \

#define sc_vmclb_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008101.bin", 0x0000, 0x100000, CRC(751a433f) SHA1(36aaf1b2425c58bd49671fc6bd61addc33a082f1) ) \
	ROM_LOAD( "95008102.bin", 0x0000, 0x100000, CRC(155ec8ab) SHA1(5f38fb49facab94d041f315178a3d2adf9d95853) ) \

#define sc_vmnv_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \

#define sc_vrgcl_others \
	ROM_REGION( 0x400000, "ymz", ROMREGION_ERASE00 ) \
	ROM_LOAD( "95008143.bin", 0x0000, 0x100000, CRC(8bf35d69) SHA1(0a5017d02ba839f2f21402df99e7684ce559d931) ) \
	ROM_LOAD( "95008144.bin", 0x0000, 0x100000, CRC(1ce7f671) SHA1(ccca34515217e72ef1879a3cbad77c7adea3a665) ) \

