/* these are the MPU4 set listings / set specific code, for hardware emulation see mpu4hw.c */

/* todo: driver inits (basic hw reel, protection configs etc.) should probably be moved here
         once the actual code for them is cleaned up and can be put into neater structures
		 like bfm_sc4
		 
		 due to the vast number of sets here this might be further split up by manufacturer
		 
*/

#include "emu.h"
#include "includes/mpu4.h"

MACHINE_CONFIG_EXTERN( mod4oki );
MACHINE_CONFIG_EXTERN( mod4yam );
MACHINE_CONFIG_EXTERN( mpu4crys );
MACHINE_CONFIG_EXTERN( bwboki );
MACHINE_CONFIG_EXTERN( mod2 );

INPUT_PORTS_EXTERN( mpu4 );
INPUT_PORTS_EXTERN( mpu4jackpot8tkn );
INPUT_PORTS_EXTERN( mpu4jackpot8per );
INPUT_PORTS_EXTERN( grtecp );

ROM_START( m4tst )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASE00  )
	ROM_LOAD( "ut4.p1",  0xC000, 0x4000,  CRC(086dc325) SHA1(923caeb61347ac9d3e6bcec45998ddf04b2c8ffd))
ROM_END

ROM_START( m4tst2 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASE00  )
	ROM_LOAD( "ut2.p1",  0xE000, 0x2000,  CRC(f7fb6575) SHA1(f7961cbd0801b9561d8cd2d23081043d733e1902))
ROM_END

ROM_START( m4clr )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASE00  )
	ROM_LOAD( "meter-zero.p1",  0x8000, 0x8000,  CRC(e74297e5) SHA1(49a2cc85eda14199975ec37a794b685c839d3ab9))
ROM_END






ROM_START( m4addrd )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dal12.bin", 0x0000, 0x010000, CRC(4affa79a) SHA1(68bceab42b3616641a34a64a83306175ffc1ce32) )
ROM_END







ROM_START( m4amhiwy )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dah20", 0x0000, 0x010000, CRC(e3f92f00) SHA1(122c8a429a1f75dac80b90c4f218bd311813daf5) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "sdr6_1.snd", 0x000000, 0x080000, CRC(63ad952d) SHA1(acc0ac3898fcc281e2d7ba19ada52d727885fe06) )
	ROM_LOAD( "sdr6_2.snd", 0x080000, 0x080000, CRC(48d2ace5) SHA1(ada0180cc60266c0a6d981a019d66bbedbced21a) )
ROM_END









ROM_START( m4blkwhd )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dbw11.bin", 0x0000, 0x010000, CRC(337aaa2c) SHA1(26b12ea3ada9668293c6b44d62458590e5b4ac8f) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "bwsnd.bin", 0x0000, 0x080000, CRC(f247ba83) SHA1(9b173503e63a4a861d1380b2ab1fe14af1a189bd) )
ROM_END



ROM_START( m4blkbul )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cbb08.epr", 0x0000, 0x010000, CRC(09376df6) SHA1(ba3b101accb6bbfbf75b9d22621dbda4efcb7769) )
ROM_END

ROM_START( m4blkcat )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dbl14.bin", 0x0000, 0x010000, CRC(c5db9532) SHA1(309b5122b4a1cb33bbccfb97faf4fa996d29432e) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "dblcsnd.bin", 0x0000, 0x080000, CRC(c90fa8ad) SHA1(a98f03d4b6f5892333279bff7537d4d6d887da62) )

	ROM_REGION( 0x200000, "msm6376_alt", 0 ) // bad dump of some sound rom?
	ROM_LOAD( "sdbl_1.snd", 0x0000, 0x18008e, CRC(e36f71ae) SHA1(ebb643cfa02d28550f2bef135ceefc902baf0df6) )
ROM_END







ROM_START( m4bluedm )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dbd10.bin", 0x0000, 0x010000, CRC(b75e319d) SHA1(8b81e852e318cfde1f5ff2123e1ef7076b208253) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "bdsnd.bin", 0x0000, 0x080000, CRC(8ac4aae6) SHA1(70dba43b398010a8bd0d82cf91553d3f5e0921f0) )
ROM_END




ROM_START( m4brook )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "brkl10.epr", 0x0000, 0x010000, CRC(857255b3) SHA1(cfd77918a19b2532a02b8bb3fa8e2716db31fb0e) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "brkl_snd.epr", 0x0000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END



ROM_START( m4bucks )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "bufd.p1", 0x0000, 0x010000, CRC(02c575d3) SHA1(92dc7a0c298e4d2d19bf754a5c82cc15e4e6456c) )
	ROM_LOAD( "bufs.p1", 0x0000, 0x010000, CRC(e394ae40) SHA1(911077053c47cebba1bed9d359cd38bd676a46f1) )
ROM_END

ROM_START( m4calamab )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "bc302d.p1", 0x0000, 0x020000, CRC(36b87f8e) SHA1(6e3cbfa52d9ec52fe009d3331dda3781f7f7783a) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "bc302f.p1", 0x0000, 0x020000, CRC(4b356aca) SHA1(81ce1585f529f1717ec56ace0a4902ae901593ae) )
	ROM_LOAD( "bc302s.p1", 0x0000, 0x020000, CRC(b349bd2d) SHA1(9b026bece40584c4f53c30f3dacc91942c871a9f) )
	ROM_LOAD( "calamari.cl", 0x0000, 0x020000, CRC(bb5e81ac) SHA1(b27f71321978712d2950d58715d18fd5523d6b06) )
ROM_END

ROM_START( m4calama )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "cac03s.p1", 0x0000, 0x020000, CRC(edc97795) SHA1(58fb91809c7f475fbceacfc1c3bda41b86dff54b) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "ca301d.p1", 0x0000, 0x020000, CRC(9a220126) SHA1(d5b12955bb336f8233ed3f892e23a14ba755a511) )
	ROM_LOAD( "ca301f.p1", 0x0000, 0x020000, CRC(e7af1462) SHA1(72659ef85c3b7916e10b4dbc09ad62638e7ab7e1) )
	ROM_LOAD( "ca301s.p1", 0x0000, 0x020000, CRC(95beecf1) SHA1(70f72abc0d4280618033b61f9dbe5b90b455c2b1) )
	ROM_LOAD( "cac03d.p1", 0x0000, 0x020000, CRC(14436ec7) SHA1(eb654ef5cef94e24296512acb6134440a5f8d17e) )
	ROM_LOAD( "cac03f.p1", 0x0000, 0x020000, CRC(69ce7b83) SHA1(c1f2dea6fe7983f5cefbf58ad63bce5ae8d7f7a5) )

	ROM_REGION( 0x20000, "altbwb", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "bca04.p1", 0x0000, 0x020000, CRC(3f97fe65) SHA1(6bc2c7e60658f39701974426ab652e8dd96b1913) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "m407.chr", 0x0000, 0x000048, CRC(fa693a0d) SHA1(601afba4a6efe8334ecc2cadfee99273a9818c1c) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cala1.hex", 0x0000, 0x080000, CRC(c9768f65) SHA1(a8f2946fdba640033da0e21d4e18293b3fc004bf) )
	ROM_LOAD( "cala2.hex", 0x0000, 0x080000, CRC(56bd2950) SHA1(b109c726514c3ee04c1bbdf5f518f60dfd0375a8) )
ROM_END

ROM_START( m4calicl )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ca2s.p1", 0x0000, 0x010000, CRC(fad153fd) SHA1(bd1f1a5c73624df45d01cb4853d87e998e434d7a) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "ca2d.p1", 0x0000, 0x010000, CRC(75eb8c6f) SHA1(1bb923d06dcfa24eaf9533c083f68f4bd840834f) )
	ROM_LOAD( "ca2f.p1", 0x0000, 0x010000, CRC(6c53cf29) SHA1(2e58453891ab4faa17ef58a81c5f3c0618d046a5) )
	ROM_LOAD( "cald.p1", 0x0000, 0x010000, CRC(296fdeeb) SHA1(7782c0c7d8f44e2c0d48cc24c13015241e47b9ec) )
	ROM_LOAD( "cals.p1", 0x0000, 0x010000, CRC(28a1c5fe) SHA1(e8474df609ea7f3517780b54d6f493987aad3650) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "ca2s.chr", 0x0000, 0x000048, CRC(97618d38) SHA1(7958e99684d50b9bdb56c97f7fcfe161f0824578) )
ROM_END



ROM_START( m4casmul )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "casinomultiplay.bin", 0x0000, 0x010000, CRC(2ebd1800) SHA1(d15e2593d17d8db9c6946af3366cf429ad291f76) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "casinomultiplaysnd.bin", 0x0000, 0x080000, CRC(be293e95) SHA1(bf0d419c898920a7546b542d8b205e25004ef04f) )
ROM_END

ROM_START( m4oldtmr )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dot11.bin",  0x00000, 0x10000,  CRC(da095666) SHA1(bc7654dc9da1f830a43f925db8079f27e18bb61e))

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "m470.chr", 0x0000, 0x000048, CRC(10d302d4) SHA1(5858e550470a25dcd64efe004c79e6e9783bce07) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "sdot01.bin", 0x0000, 0x080000, CRC(f247ba83) SHA1(9b173503e63a4a861d1380b2ab1fe14af1a189bd) )
ROM_END

ROM_START( m4casot )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "casrom.bin",  0x00000, 0x10000,  CRC(da095666) SHA1(bc7654dc9da1f830a43f925db8079f27e18bb61e) ) // == old timer   (aka b&wrom.bin)

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "caschar.chr", 0x0000, 0x000048, CRC(10d302d4) SHA1(5858e550470a25dcd64efe004c79e6e9783bce07) ) // ( aka b&wchrt.chr )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cassound.bin", 0x0000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) ) // ( aka b&wsound.bin )
ROM_END



ROM_START( m4jpmcla )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "jcv2.epr",  0x00000, 0x10000,  CRC(da095666) SHA1(bc7654dc9da1f830a43f925db8079f27e18bb61e) ) // == old timer

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "jcchr.chr", 0x0000, 0x000048, CRC(e370e271) SHA1(2b712dd3590c31356e8b0b62ffc64ff8ce444f73) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "sjcv2.snd", 0x0000, 0x080000, CRC(f247ba83) SHA1(9b173503e63a4a861d1380b2ab1fe14af1a189bd) )
ROM_END


ROM_START( m4ceptr )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dce10.bin", 0x0000, 0x010000, CRC(c94d41ef) SHA1(58fdff2de8dd3ead3980f6f34362183d084ce917) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cepsnd.p1", 0x000000, 0x080000, CRC(3a91784a) SHA1(7297ccec3264aa9f1e7b3a2841f5f8a1e4ca6c54) )
	ROM_LOAD( "cepsnd.p2", 0x080000, 0x080000, CRC(a82f0096) SHA1(45b6b5a2ae06b45add9cdbb9f5e6f834687b4902) )
ROM_END


ROM_START( m4chasei )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ci2c.p1", 0x0000, 0x010000, CRC(fc49a2e1) SHA1(f4f02e168cd9bf0245c2b7340fe151da66f09c5c) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "ch20p8pn.rom", 0x0000, 0x010000, CRC(712bd2e7) SHA1(0e83fa077f42a051aaa07a7e13196955b0ac840d) )
	ROM_LOAD( "chin2010l", 0x0000, 0x010000, CRC(7fe97181) SHA1(1ccf65ff108bdaa46efcb3f831fccc953297b9ac) )
	ROM_LOAD( "ci2k.p1", 0x0000, 0x010000, CRC(8d715b8a) SHA1(5dd6f8d3d6710b0741df37af8792d942f41062d2) )
	ROM_LOAD( "ci2s.p1", 0x0000, 0x010000, CRC(8175e1e3) SHA1(9a4b0a0288508e7900ceac8bc3b245ac1f898b19) )
	ROM_LOAD( "ci2y.p1", 0x0000, 0x010000, CRC(80410946) SHA1(60a4f73eb9a35e5c246d8ef7b25bcf25b28bf8ed) )

	ROM_LOAD( "chase invaders 8.bin", 0x0000, 0x010000, CRC(0bf6a8a0) SHA1(cea5ea40d71484a455615e14f6708b1bc06bbbe8) ) // bad prg (no vectors?)


	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "chaseinvaders.chr", 0x0000, 0x000048, CRC(d7703dcd) SHA1(16fd998d1b44f35c10e5486882aa7f2d018dc82b) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cha.s1", 0x000000, 0x080000, CRC(8200b6bc) SHA1(bcc4ffbddcdcc1dd994fe29e9b24e83272f59442) )
	ROM_LOAD( "cha.s2", 0x080000, 0x080000, CRC(542863fa) SHA1(501d66b2badb5036bb5dd8bac3cdb681f630a982) )
ROM_END



ROM_START( m4c9c )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cncs.p1", 0x0000, 0x010000, CRC(10f15e2a) SHA1(c17ab13764d74302246984245485cb7692913b44) )
ROM_END







ROM_START( m4clbtro )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "tr2s.p1", 0x0000, 0x010000, CRC(6d43375c) SHA1(5be1dc85374c6a1235e0b137b46ebd7a2d7d922a) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "tr2f.p1", 0x0000, 0x010000, CRC(fbdcd06f) SHA1(27ccdc83e60a62227d33d8cf3d516fc43908ab99) )
	ROM_LOAD( "tr2d.p1", 0x0000, 0x010000, CRC(0cc23f89) SHA1(a66c8c28073f53381c43e3e597f15f81c5c61479) )
	ROM_LOAD( "tro20.bin", 0x0000, 0x010000, CRC(5e86c3fc) SHA1(ce2419991559839a8875060c1afe0f030190010a) )
	ROM_LOAD( "trod.p1", 0x0000, 0x010000, CRC(60c84612) SHA1(84dc8b34e41436331832c1a32ddac0fce269488a) )
	ROM_LOAD( "tros.p1", 0x0000, 0x010000, CRC(5e86c3fc) SHA1(ce2419991559839a8875060c1afe0f030190010a) )
ROM_END

ROM_START( m4clbveg )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "clas.p1", 0x0000, 0x010000, CRC(6aad03f0) SHA1(2f611cc6f020e334dc4b87d2d907727ba15ff7ff) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "claf.p1", 0x0000, 0x010000, CRC(79b83184) SHA1(7319a405b2b0b274e03f5cd1465436f8548065e4) )
	ROM_LOAD( "clal.p1", 0x0000, 0x010000, CRC(db0bb5a2) SHA1(2735e02642fb92bb824e3b1f415a1a3ef13a856d) )
	ROM_LOAD( "clad.p1", 0x0000, 0x010000, CRC(4fa45cce) SHA1(58a5d6cc8608eb1aa453429e26eacea589afa524) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "cvegas.chr", 0x0000, 0x000048, CRC(a6c341b0) SHA1(c8c838c9bb1ced52889504b9cea8d88f1e7fa79f) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cvegass1.hex", 0x0000, 0x080000, CRC(13a8c857) SHA1(c66e10bca1ad54f467b9c5eacd502c54397c09b2) )
	ROM_LOAD( "cvegass2.hex", 0x0000, 0x080000, CRC(88b37145) SHA1(1c6c9ad2010e1688d3370d1f2a5ae83dc683b500) )
ROM_END

ROM_START( m4clbx )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "clx12s.p1", 0x0000, 0x020000, CRC(6798c153) SHA1(e621e341a0fed1cb35637edb0769ae1cca72a663) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "clx12d.p1", 0x0000, 0x020000, CRC(43e797ba) SHA1(fb2fc843176fe50c1039214d48815d6e9871ae27) )
	ROM_LOAD( "clx12f.p1", 0x0000, 0x020000, CRC(3e6a82fe) SHA1(01ef9a15a3cf9b1191c573b36fb5758e79c3adc1) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cxs1.hex", 0x000000, 0x080000, CRC(4ce005f1) SHA1(ee0f59a9c7e0222dd63fa63ccff8f194abd01ddb) )
	ROM_LOAD( "cxs2.hex", 0x080000, 0x080000, CRC(495e0730) SHA1(7ba8150fbcf974ac494a82fd373ff02185543e35) )
ROM_END



ROM_START( m4coscas )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "cc_sj__c.3r1", 0x0000, 0x020000, CRC(44b940a6) SHA1(7e621873fcf6460f654e35cc74552e86b6253ddb) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "cosm15g", 0x0000, 0x020000, CRC(edd01d55) SHA1(49246fa1e12ceb3297f35616cdc1cf62472a379f) )
	ROM_LOAD( "cosmiccasinos15.bin", 0x0000, 0x020000, CRC(ddba1241) SHA1(7ca2928ae2ab4e323b60bb661b60681f89cc5663) )
	ROM_LOAD( "cc30s.p1", 0x0000, 0x020000, CRC(e308100a) SHA1(14cb07895d17237768877dd62ba7c3fc8e5b2630) )

	ROM_REGION( 0x20000, "altbwb", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "cc_sj___.3s1", 0x0000, 0x020000, CRC(52c312b0) SHA1(bd5381d58b1acb7adf6857c142eae4a253081fbd) )
	ROM_LOAD( "cc_sj__c.7_1", 0x0000, 0x020000, CRC(ee9e6126) SHA1(fab6fd04004acebf291544720ba06cea79d5a054) )
	ROM_LOAD( "cc_sj_b_.3s1", 0x0000, 0x020000, CRC(019f0a71) SHA1(7a97f4e89c16e25f8e7502bba37f49c8496fbb47) )
	ROM_LOAD( "cc_sj_bc.3r1", 0x0000, 0x020000, CRC(de9bb8e1) SHA1(7974b03974531eb4b5ed865b8eeb9649c1346df4) )
	ROM_LOAD( "cc_sj_bc.7_1", 0x0000, 0x020000, CRC(afe1aac6) SHA1(fc9c69e45db6a85c45ef8d32d048e5726d7da655) )
	ROM_LOAD( "cc_sj_d_.3s1", 0x0000, 0x020000, CRC(215e12f3) SHA1(68ed9923c6fd51e9305afac9d271c7b3ce38b12f) )
	ROM_LOAD( "cc_sj_dc.3r1", 0x0000, 0x020000, CRC(00e357c3) SHA1(02bf7427899d2e536442b87d41c140ebd787a580) )
	ROM_LOAD( "cc_sj_dc.7_1", 0x0000, 0x020000, CRC(330d68a2) SHA1(12410af5f37b26f29f5cd23606ab0e128675095a) )
	ROM_LOAD( "cc_sj_k_.3s1", 0x0000, 0x020000, CRC(9161912d) SHA1(d11109f4bdc1c60f4cf477e1f26556800a83abdb) )
	ROM_LOAD( "cc_sj_kc.3r1", 0x0000, 0x020000, CRC(b0dcd41d) SHA1(6b50a5e401bf854186331673dcc0c3fc5de2991b) )
	ROM_LOAD( "cc_sja__.3s1", 0x0000, 0x020000, CRC(1682b1d3) SHA1(24baaf789eca150f0f6fd9c510e245aa7b88cc4c) )
	ROM_LOAD( "cc_sja_c.3r1", 0x0000, 0x020000, CRC(373ff4e3) SHA1(55b7ab247863eb3c025e84782c8cab7734343077) )
	ROM_LOAD( "cc_sja_c.7_1", 0x0000, 0x020000, CRC(e956898e) SHA1(f51682651520551d481360bf86eba510cd758441) )
	ROM_LOAD( "cc_sjb__.3s1", 0x0000, 0x020000, CRC(5c451985) SHA1(517f634d31f7190ca6685c1037fb66a8b87effba) )
	ROM_LOAD( "cc_sjb_c.7_1", 0x0000, 0x020000, CRC(109e9ae9) SHA1(00f381beb33cae58fc3429d3501efa4a9d9f0035) )
	ROM_LOAD( "cc_sjbgc.3r1", 0x0000, 0x020000, CRC(2de82f88) SHA1(5c8029d43282a014e82b4f975616ed2bbc0e5641) )
	ROM_LOAD( "cc_sjbtc.3r1", 0x0000, 0x020000, CRC(976c2858) SHA1(a70a8fe51d1b9d903d099e89a40481ea6af13683) )
	ROM_LOAD( "cc_sjwb_.3s1", 0x0000, 0x020000, CRC(e2df8167) SHA1(c312b30402dd93c6d4a32932677430c9c996fd36) )
	ROM_LOAD( "cc_sjwbc.3r1", 0x0000, 0x020000, CRC(a33a59a6) SHA1(a74ffd647e8390d89df475cc3f5205462c9d93d7) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cc___snd.1_1", 0x000000, 0x080000, CRC(d858f238) SHA1(92a3dfacde8bfa8705e91fab5bb627f9b34ad2dc) )
	ROM_LOAD( "cc___snd.1_2", 0x080000, 0x080000, CRC(bab1bd8e) SHA1(c703d0e24c0a522ebf79895049e85f5471f7d7e9) )
ROM_END



ROM_START( m4crzjk )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "crjok2.04.bin", 0x0000, 0x010000, CRC(838336d6) SHA1(6f36de20c930cbbff479af2667c11152c6adb43e) )
ROM_END

ROM_START( m4crzjwl )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "cj11bin", 0x0000, 0x020000, CRC(208fda73) SHA1(8b15c197693ea7749bc961fe4e5e36b317f9f6f8) ) // crown jewels (german)

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "cjexlow", 0x0000, 0x020000, CRC(07c227c1) SHA1(286341ed44ef7cd08ca411f2b3e6936b5e83a5f3) ) // crown jewels (german)
	ROM_LOAD( "cjgerman", 0x0000, 0x020000, CRC(b090e690) SHA1(bdbe4041085c995761306280c15f782ea3bdc110) )
	ROM_LOAD( "cjj54.bin", 0x0000, 0x020000, CRC(16dc92e7) SHA1(b791535054d5864c7053243408a54accfa014bd1) )
	ROM_LOAD( "gcn11", 0x0000, 0x020000, CRC(51493500) SHA1(901e60c1a7e9e628d723e199579fc82cf2e433e6) )
	ROM_LOAD( "gcn111", 0x0000, 0x020000, CRC(b1152ce6) SHA1(1d236bad57ad38b11215efe44008bb8e4014939e) )
	ROM_LOAD( "gjv4", 0x0000, 0x020000, CRC(df63105d) SHA1(56e28adef9ec8921da7ab8045859e834731196c5) )
	ROM_LOAD( "gjv5", 0x0000, 0x020000, CRC(e4f0bab2) SHA1(1a13d97ff2c4fbae39327f2a5a8b110f2617857e) )
	ROM_LOAD( "cjg.p1", 0x0000, 0x020000, CRC(1f4743bf) SHA1(f9a0da2ed9cad5e6685c8a6d1d09e5d4bbcfacec) ) 	// Crown Jewels Deluxe (german)

	ROM_REGION( 0x180000, "msm6376", 0 )
	ROM_LOAD( "cjsound1.bin", 0x000000, 0x080000, CRC(b023f6b9) SHA1(04c362c6511442d3ab775a5ff2051bfe26d5e624) )
	ROM_LOAD( "cjsound2.bin", 0x080000, 0x080000, CRC(02563a43) SHA1(dfcee4e0fdf81c726c8e13278e7950459bcaab18) )
	ROM_LOAD( "cjsound3.bin", 0x100000, 0x080000, CRC(e722e438) SHA1(070f3772920fa64d5214843c313b27a5b2a4c105) )
ROM_END

ROM_START( m4crjwl )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cjcf.p1", 0x0000, 0x010000, CRC(7feccc74) SHA1(4d1c7c6d2085492ee4205a7383ad7dc1de4e8d60) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "cjcd.p1", 0x0000, 0x010000, CRC(cb83f226) SHA1(f09996436b3db3c8f0fe237884d9125be2b7855e) )
	ROM_LOAD( "cjcs.p1", 0x0000, 0x010000, CRC(1054e02d) SHA1(067705f20862f6cfc4334c74e0fab1a1016d427c) )
	ROM_LOAD( "cjn02.p1", 0x0000, 0x010000, CRC(a3d50e20) SHA1(15698e74a37d5f95a5634d48ae2a9a5d19faa2b6) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* Missing? or in above set? */
ROM_END

ROM_START( m4crjwl2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cj214f.p1", 0x0000, 0x010000, CRC(7ee4d30c) SHA1(2bf702bc925c473f7e9eaeb5b3ae0b00e124161a) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "cj214d.p1", 0x0000, 0x010000, CRC(359e2a73) SHA1(c85eeebafca14e6f975953f5daf2772a62693051) )
	ROM_LOAD( "cj214s.hex", 0x0000, 0x010000, CRC(296aa885) SHA1(045b02848b37e8a04d950d54301dc6888d6178ad) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "chr.chr", 0x0000, 0x000048, CRC(c5812913) SHA1(d167b1f512c183cf01a1f4e1c1588ea0ae21331b) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cjcs1.hex", 0x000000, 0x080000, CRC(2ac3ba9f) SHA1(3332f29f81918c34aeec3da6f7d001dc9922840d) )
	ROM_LOAD( "cjcs2.hex", 0x080000, 0x080000, CRC(89838a9d) SHA1(502243cc0a14e63882b537f05c4cc0eb852e4a0c) )
ROM_END


ROM_START( m4dbldmn )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "cdd05s.p1", 0x0000, 0x020000, CRC(fc14771f) SHA1(f418af9fed331560195a694f20ef2fea27ed04b0) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "cdd05d.p1", 0x0000, 0x020000, CRC(fc1c5e90) SHA1(c756d2ac725168af5396c8ef7550db9087a50937) )
	ROM_LOAD( "cdd05f.p1", 0x0000, 0x020000, CRC(81914bd4) SHA1(cf286810ad6732ca1d706e70f4c2958d28cc979c) )
	ROM_LOAD( "cdd01.p1", 0x0000, 0x020000, CRC(e35dffde) SHA1(0bfc977f25f25785f20b510c44d2d3d79e23af8b) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cddsnd.p1", 0x000000, 0x080000, CRC(e1833e31) SHA1(1486e5afab347d6dee1543a55d1193b7db3c89d7) )
	ROM_LOAD( "cddsnd.p2", 0x080000, 0x080000, CRC(fd33ed2a) SHA1(f68ffadde40f88e7954d4a98bcd7ff023841b55b) )
ROM_END



ROM_START( m4drac )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "dra21.bin", 0x0000, 0x020000, CRC(23be387e) SHA1(08a78f4b8ddef46069d1c75113300b21e52338c1) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "dra24.bin", 0x0000, 0x020000, CRC(3db112ae) SHA1(b5303e2a65476931d4769327ca62afd0f6a9eda7) )
	ROM_LOAD( "dra27.bin", 0x0000, 0x020000, CRC(8a095175) SHA1(41006e298f1688499ce6820ec28196c7578684b9) )
	ROM_LOAD( "dra27.p1", 0x0000, 0x020000, CRC(8a095175) SHA1(41006e298f1688499ce6820ec28196c7578684b9) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "drasnd.p1", 0x000000, 0x080000, CRC(54c3821c) SHA1(1fcc62e2b127dd7f1d5d27a3afdf56dc27f122f8) )
	ROM_LOAD( "drasnd.p2", 0x080000, 0x080000, CRC(9096d2bc) SHA1(1b4c530b7b0fde869980d519255e2585c5461e13) )
	ROM_LOAD( "drasnd.p3", 0x100000, 0x080000, CRC(a07f412b) SHA1(cca8f5cfe620ece45ca40bf801f0643cd76547e9) )
	ROM_LOAD( "drasnd.p4", 0x180000, 0x080000, CRC(018ed789) SHA1(64202da2c542f5ef208faeb04945eb1a758d4746) )
ROM_END






ROM_START( m4exgam )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "czep30.bin", 0x0000, 0x010000, CRC(4614e6f6) SHA1(5602a68e9b47394cb31bbcd49a9920e19af6242f) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "ceg.chr", 0x0000, 0x000048, CRC(f694224e) SHA1(936ab5e349fa59accbb37959cce9519fd97f3978) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "sczep.bin", 0x0000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END

ROM_START( m4fastfw )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "fastf206", 0x0000, 0x010000, CRC(a830b121) SHA1(0bf813ee75bd8e109e6688b91bd0983d341a6695) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "ffo05__1.0", 0x0000, 0x010000, CRC(8b683969) SHA1(7469b551e4d6f65550d54ee39b2bac07cf3dbd4b) )
	ROM_LOAD( "ffo10__1.0", 0x0000, 0x010000, CRC(294288fd) SHA1(87d25f6333b6862fcc57a550b5cc7c0bc64e72cd) )
	ROM_LOAD( "ffo10d_1.0", 0x0000, 0x010000, CRC(8d96f3d4) SHA1(2070a335cfa3f9de1bd9e9094d91cce81b91347d) )
	ROM_LOAD( "ffo20__1.0", 0x0000, 0x010000, CRC(9528291e) SHA1(61c0eb8ce955f708e8a68a28f253706267e28254) )
	ROM_LOAD( "ffo20d_1.0", 0x0000, 0x010000, CRC(5bae35fe) SHA1(7e4d61ed97ddd170bd1424f34d0327093668da3f) )
	ROM_LOAD( "ffo20dy1.0", 0x0000, 0x010000, CRC(37167d46) SHA1(94b87697615f81b746ce3bcc64fc893f865e00dc) )
ROM_END




ROM_START( m4fortcb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cfod.p1", 0x0000, 0x010000, CRC(9d0e2b63) SHA1(cce871d2bbe486793de5de9fadfbddf67c382e5c) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "cfof.p1", 0x0000, 0x010000, CRC(010b3c1f) SHA1(b44c22c21d22603b277138eabf803e6d46ad4aae) )
	ROM_LOAD( "cfos.p1", 0x0000, 0x010000, CRC(f3b47df4) SHA1(3ad674864ba3a24283af14caaf2c999d4fde11fc) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cfosnd.p1", 0x000000, 0x080000, CRC(74bbf913) SHA1(52ddc89ab34b11ede2c0e9b9b27e119b0c1eb2d9) )
	ROM_LOAD( "cfosnd.p2", 0x080000, 0x080000, CRC(1b2bb79a) SHA1(5f19ea000f34bb404ed6c8ea5ec7b809ccb1ae36) )
ROM_END




ROM_START( m4frtgm )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "fruit.bin", 0x0000, 0x010000, CRC(dbe44316) SHA1(15cd49dd2e6166f7a7668663f7fea802d6cbb12f) )

	ROM_REGION( 0x800000, "msm6376", 0 ) /* this isn't OKI, or is corrupt (bad size) */
	ROM_LOAD( "fruitsnd.bin", 0x0000, 0x010000, CRC(86547dc7) SHA1(4bf64f22e84c0ee82d961b0ba64932b8bf6a521f) ) // matches 'Replay' on SC1 hardware, probably just belongs there.. or this is eurocoin with different sound hw here?
ROM_END






ROM_START( m4gb006 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "006s.p1", 0x0000, 0x010000, CRC(6e750ab9) SHA1(2e1f08df7991efe450633e0bcec201e6fa7fdbaa) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "006d.p1", 0x0000, 0x010000, CRC(7e0a4282) SHA1(8fd0cbdd9cf3ac74b7b202ce7615392c1a746906) )
	ROM_LOAD( "006y.p1", 0x0000, 0x010000, CRC(2947f4ed) SHA1(7d212bcef36e2bd792ded3e1e1638218e76da119) )
	ROM_LOAD( "bond20_11", 0x0000, 0x010000, CRC(8d810cb1) SHA1(065d8df33472a3476dd6cf21a684db9d7c8ba829) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "006s.chr", 0x0000, 0x000048, CRC(ee3d06eb) SHA1(570a715e71d4184e4df02b7e5b68fee70e03aeb0) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "006snd.p1", 0x000000, 0x080000, CRC(44afef7d) SHA1(d8a4b6dc04e0f337db6d3b5322d066ae5f5bda41) )
	ROM_LOAD( "006snd.p2", 0x080000, 0x080000, CRC(5f3c7cf8) SHA1(500f8fb07ef344d44c062f8d01878df1c917bcfc) )
ROM_END

ROM_START( m4gbust )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ghostbusters 2p.bin", 0x0000, 0x010000, CRC(abb288c4) SHA1(2012e027711996a552ab59674ae3bce1bf14f44b) )

	ROM_REGION( 0x10000, "altbwb", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "gb_02___.2n3", 0x0000, 0x010000, CRC(973b3538) SHA1(31df04d9f35cbde4d5e395256927f146d1613178) )
	ROM_LOAD( "gb_02___.3a3", 0x0000, 0x010000, CRC(2b9d94b6) SHA1(ca433240f9e926cdf5240209589951e6018a496a) )
	ROM_LOAD( "gb_02___.3n3", 0x0000, 0x010000, CRC(99514ddd) SHA1(432d484525867c6ad68cd93a4bfded4dba36cf56) )
	ROM_LOAD( "gb_02___.3s3", 0x0000, 0x010000, CRC(2634aa5f) SHA1(58ab973940138bdfd2690867e2ac3eb52bffb633) )
	ROM_LOAD( "gb_05___.4a3", 0x0000, 0x010000, CRC(8be6949e) SHA1(9731a1cb0d17c3cec2bec263cd6348f05662d917) )
	ROM_LOAD( "gb_05___.4n3", 0x0000, 0x010000, CRC(621b25f0) SHA1(bf699068284def8bad9143c5841f667f2cb6f20f) )
	ROM_LOAD( "gb_05___.4s3", 0x0000, 0x010000, CRC(e2227701) SHA1(271682c7bf6e0f6f49f6d6b138aa19b6ef6bc626) )
	ROM_LOAD( "gb_05_d_.4a3", 0x0000, 0x010000, CRC(a1b2b32f) SHA1(c1504b3768920f90dbd441b9d50db9676528ca97) )
	ROM_LOAD( "gb_10___.2a3", 0x0000, 0x010000, CRC(a5c692f3) SHA1(8305c88ab8b80b407f4723df25135c25a4c0794f) )
	ROM_LOAD( "gb_10___.2n3", 0x0000, 0x010000, CRC(de18c441) SHA1(5a7055fcd755c1ac58e1b94af243801f169f29f5) )
	ROM_LOAD( "gb_10___.3s3", 0x0000, 0x010000, CRC(427e043b) SHA1(2f64c11a04306692ac5eb9919892f7226156dce0) )
	ROM_LOAD( "gb_10_b_.3s3", 0x0000, 0x010000, CRC(091afb66) SHA1(ac32d7be1e1f4f1453e37017966990a481506024) )
	ROM_LOAD( "gb_10_d_.2a3", 0x0000, 0x010000, CRC(f1446bf5) SHA1(4011d60e13045476741c5a02c64dabbe6a1ae2d6) )
	ROM_LOAD( "gb_10_d_.2n3", 0x0000, 0x010000, CRC(cac5057d) SHA1(afcc21dbd07515ed134675b7dbfb53c048a465b0) )
	ROM_LOAD( "gb_10_d_.3s3", 0x0000, 0x010000, CRC(776736de) SHA1(4f80d9ffdf4468801cf830e9774b6028f7684864) )
	ROM_LOAD( "gb_20___.2n3", 0x0000, 0x010000, CRC(27fc2ee1) SHA1(2e6a042f7117b4594b2601ae166ee0db72c70ed5) )
	ROM_LOAD( "gb_20___.3s3", 0x0000, 0x010000, CRC(4a86d879) SHA1(72e92b6482fdeb4dca36d9426a712ac24d60f7f7) )
	ROM_LOAD( "gb_20_b_.2a3", 0x0000, 0x010000, CRC(4dd7d38f) SHA1(8a71c27189ec3089c016a8292db68f7cdc91b083) )
	ROM_LOAD( "gb_20_b_.2n3", 0x0000, 0x010000, CRC(28cbb217) SHA1(a74978ff5e1511a33f543006b3f8ad30a77ea462) )
	ROM_LOAD( "gb_20_b_.3s3", 0x0000, 0x010000, CRC(1a7cc3cf) SHA1(0d5764d35489bde284965c197b217a06f26a3e3b) )
	ROM_LOAD( "gb_20_d_.2a3", 0x0000, 0x010000, CRC(70f40688) SHA1(ed14f8f460825ffa087394ef5984ae064e02f7b6) )
	ROM_LOAD( "gb_20_d_.2n3", 0x0000, 0x010000, CRC(431c2965) SHA1(eb24e560d5c4bf419465fc760621a4fa853fff95) )
	ROM_LOAD( "gb_20_d_.3s3", 0x0000, 0x010000, CRC(4fc69155) SHA1(09a0f2122893d9fd90204a74c8862e01386503a4) )

ROM_END



ROM_START( m4gldgat )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dgg22.bin", 0x0000, 0x010000, CRC(ef8498df) SHA1(6bf164ef18445e83e4510a000bc924cbe916ad99) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "m450.chr", 0x0000, 0x000048, CRC(fb7b2a45) SHA1(b6d5537bde9c05a3e79221a5577b8ae77bace9e6) )
ROM_END

ROM_START( m4gldjok )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dgj12.bin", 0x0000, 0x010000, CRC(93ee0c35) SHA1(5ae67b14f7f3d8528fa106519a8a27437c997a70) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "sdgj.snd", 0x0000, 0x080000, CRC(b6cd118b) SHA1(51c5d694ed0dfde8d3fd682f2471d83eec236736) )
ROM_END






ROM_START( m4gnsmk )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dgu16", 0x0000, 0x010000, CRC(6aa23345) SHA1(45e129ec95b1a796f334bedd08469f2ab47a18f8) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "sdgu01.s1", 0x000000, 0x080000, CRC(bfb284a2) SHA1(860b98d54a3180fbb00b7b03feae049fb4cf9d7f) )
	ROM_LOAD( "sdgu01.s2", 0x080000, 0x080000, CRC(1a46ba28) SHA1(d7154e5f92be8631207620eb313b28990c6a1c7f) )
	ROM_LOAD( "sdgu01.s3", 0x100000, 0x080000, CRC(88bffcf4) SHA1(1da853193f6a22889edff5aafd9440c676a82ea6) )
	ROM_LOAD( "sdgu01.s4", 0x180000, 0x080000, CRC(a6160bef) SHA1(807f7d470728a479a55c782fca3df1eacd0b594c) )
	ROM_END

ROM_START( m4blkbuld )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dgu16", 0x0000, 0x010000, CRC(6aa23345) SHA1(45e129ec95b1a796f334bedd08469f2ab47a18f8) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "dbbsnd.p1", 0x000000, 0x080000, CRC(a913ad0d) SHA1(5f39b661912da903ce8d6658b7848081b191ea56) )
	ROM_LOAD( "dbbsnd.p2", 0x080000, 0x080000, CRC(6a22b39f) SHA1(0e0dbeac4310e03490b665fff514392481ad265f) )
ROM_END


ROM_START( m4hpyjok )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dhj12", 0x0000, 0x010000, CRC(982439d7) SHA1(8d27fcecf7a6a7fd774678580074f945675758f4) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "dhjsnd", 0x0000, 0x080000, CRC(8ac4aae6) SHA1(70dba43b398010a8bd0d82cf91553d3f5e0921f0) )
ROM_END

ROM_START( m4hirise )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "hiix.p1", 0x0000, 0x010000, CRC(c68c816c) SHA1(2ec89d83f3b658700433fc165358290ce58eba64) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "hirs.p1", 0x0000, 0x010000, CRC(a38f771e) SHA1(c1502200671389a1fe6dcb9c043d22583d5991dc) )
	ROM_LOAD( "hirs20dd", 0x0000, 0x010000, CRC(89941670) SHA1(28859adfa79dce53c348c63b46f6f5a068f2b2de) )
	ROM_LOAD( "hirx.p1", 0x0000, 0x010000, CRC(4280a16b) SHA1(c9179ec17404a6f084679ad5f04e53a50f00af98) )
	ROM_LOAD( "hirxc.p1", 0x0000, 0x010000, CRC(1ad1d942) SHA1(91d02212606e22b280be9640433e013bc50e5ea8) )
	ROM_LOAD( "hrise206", 0x0000, 0x010000, CRC(58b4bbdd) SHA1(0b76d27147fbadba97328eb9d2dc81cff9d576e0) )
ROM_END





ROM_START( m4holdtm )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dht10.hex", 0x0000, 0x010000, CRC(217d382b) SHA1(a27dd107c554d4787967633dff998d3962ee0ea5) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "ht01.chr", 0x0000, 0x000048, CRC(0fc2bb52) SHA1(0d0e47938f6e00166e7352732ddfb7c610f44db2) )
	ROM_LOAD( "m400.chr", 0x0000, 0x000048, CRC(8f00f720) SHA1(ea59fa2a3b016a7ae83be3caf863de87ce7aeffa) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "sun01.hex", 0x0000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END






ROM_START( m4jok300 )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "cjo", 0x0000, 0x020000, CRC(386e99db) SHA1(5bb0b513ef63ffaedd98b8e9e7206658fe784fda) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASEFF )
	/* missing? */
ROM_END

ROM_START( m4jokmil )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "cjm03.epr", 0x0000, 0x020000, CRC(e5e4986e) SHA1(149b950a739ad308f7759927c344de8193ce67c5) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASEFF )
	/* missing? */
ROM_END







ROM_START( m4joljokh )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "jollyjokerhungarian.bin", 0x0000, 0x010000, CRC(85b6a406) SHA1(e277f9d3b62faead04d65efbc06de7f4a50ae38d) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "jollyjokerhungariansnd.bin", 0x0000, 0x080000, CRC(93460383) SHA1(2b179a1dde09ebdfe8c84641899df7be87d443e5) )
ROM_END









ROM_START( m4lineup )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "lineup5p1.bin", 0xc000, 0x004000, CRC(9ba9edbd) SHA1(385e01816b5631b6896e85343ae96b3c36f9647a) )
	ROM_LOAD( "lineup5p2.bin", 0x8000, 0x004000, CRC(e9e4dfb0) SHA1(46a0efa84770036366c7a6a33ef1d42c7b2b782b) )
	ROM_LOAD( "lineup5p3.bin", 0x6000, 0x002000, CRC(86623376) SHA1(e29442bfcd401361287852b87673368322e946b5) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "lu2_10p1.bin", 0x0000, 0x004000, CRC(2fb89062) SHA1(55e86de8fd0d36cca9aab8ad5aae7b4f5a62b940) )
	ROM_LOAD( "lu2_10p2.bin", 0x0000, 0x004000, CRC(9d820af2) SHA1(63d27df91f80e47eb8c9685fcd2c3eff902a2ef8) )
	ROM_LOAD( "lu2_10p3.bin", 0x0000, 0x002000, CRC(8c8a210c) SHA1(2599d979f1a62e9ef6acc70d0ad5c9b4a65d712a) )
ROM_END

ROM_START( m4luck7 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dl716.bin", 0x0000, 0x010000, CRC(141b23a9) SHA1(3bfb82ea0ee4104bd8739b545aba617f84bef770) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "dl7snd.bin", 0x0000, 0x080000, CRC(c90fa8ad) SHA1(a98f03d4b6f5892333279bff7537d4d6d887da62) )
ROM_END

ROM_START( m4luckdv )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cld_16.bin", 0x0000, 0x010000, CRC(89f63938) SHA1(8d3a5628e2c0bf39784afe2f00a007d40ea35423) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "cld_snd1.snd", 0x000000, 0x080000, CRC(f247ba83) SHA1(9b173503e63a4a861d1380b2ab1fe14af1a189bd) )
	ROM_LOAD( "cld_snd2.snd", 0x080000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END

ROM_START( m4luckdvd )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dld13", 0x0000, 0x010000, CRC(b8ceb29b) SHA1(84b6ebad300214610635fb8141d18de2b7065435) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "sdld01.snd", 0x000000, 0x080000, CRC(9b035fa6) SHA1(51b7e5bc3abdf4f1beba2347146a91a2b3f4de35) )
ROM_END

ROM_START( m4luckwb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "lwb10.bin", 0x0000, 0x010000, CRC(6d43a14e) SHA1(267aba1a01bfd5f0eaa7683d041d5fcb2d301934) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "lwb15.bin", 0x0000, 0x010000, CRC(b5af8cb2) SHA1(474975b83803627ad3ac4217d8cecb2d2db16fec) )
	ROM_LOAD( "lwb21.bin", 0x0000, 0x010000, CRC(6c570733) SHA1(7488318ca9689371e4f80be0a0fddd8ad141733e) )
	ROM_LOAD( "lwb22.bin", 0x0000, 0x010000, CRC(05b952a7) SHA1(952e328b280a18c1ffe253b6a56f2b5e893b1b72) )
	ROM_LOAD( "lwb27.bin", 0x0000, 0x010000, CRC(9d6b6637) SHA1(65bad12cd08de128ca31c9488e32e3cebfb8eedb) )
	ROM_LOAD( "lwb6.bin", 0x0000, 0x010000, CRC(8e7d4594) SHA1(4824a9a4628585a170c41e00f7b3fcb8a2330c02) )
	ROM_LOAD( "lwb7.bin", 0x0000, 0x010000, CRC(8e651705) SHA1(bd4d09d586d14759a17d4d7d4016c427f3eef015) )

	ROM_REGION( 0x100000, "msm6376", 0 ) // these are all different sound roms...
	ROM_LOAD( "lwbs3.bin", 0x0000, 0x07dc89, CRC(ee102376) SHA1(3fed581a4654acf285dd430fbfbac33cd67411b8) )
	ROM_LOAD( "lwbs7.bin", 0x0000, 0x080000, CRC(5d4177c7) SHA1(e13f145885bb719b0021ae4ce289261a3eaa2e18) )
	ROM_LOAD( "lwbs8.bin", 0x0000, 0x080000, CRC(187cdf5b) SHA1(87ec189af27c95f278a7531ec13df53a08889af8) )
	ROM_LOAD( "lwbs9.bin", 0x0000, 0x080000, CRC(2e02b617) SHA1(2502a1d2cff155a7fc5148e23a4723d4d60e9d42) )
ROM_END


ROM_START( m4magdrg )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dmd10.bin", 0x0000, 0x010000, CRC(9cc4f2f8) SHA1(46a90ffa18d35ad2b06542f91120c02bc34f0c40) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "mdsnd.bin", 0x000000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END

ROM_START( m4maglin )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dma21.bin", 0x0000, 0x010000, CRC(836a25e6) SHA1(5f83bb8a2c77dd3b02724c076d6b37d2c1c93b93) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "mlsound1.p1", 0x000000, 0x080000, CRC(ff8749ff) SHA1(509b53f09cdfe5ee865e60ab42fd578586ac53ea) )
	ROM_LOAD( "mlsound2.p2", 0x080000, 0x080000, CRC(c8165b6c) SHA1(7c5059ee8630da31fc3ad50d84a4730297757d46) )
ROM_END

ROM_START( m4magrep )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dmr13.bin", 0x0000, 0x010000, CRC(c3015da3) SHA1(23cd505eedf666c012e4064a5fcf5a983f098e83) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "mrdsound.bin", 0x000000, 0x080000, CRC(9b035fa6) SHA1(51b7e5bc3abdf4f1beba2347146a91a2b3f4de35) )
ROM_END











ROM_START( m4nspot )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ns2s.p1", 0x0000, 0x010000, CRC(ba0f5a81) SHA1(7015176d4528636cb8a753249c824c37941e8eae) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "ns2d.p1", 0x0000, 0x010000, CRC(5e66b7e0) SHA1(e82044e3c1e5cf3a2baf1fde7b7ab8b6e221d360) )
	ROM_LOAD( "nits.p1", 0x0000, 0x010000, CRC(47c965e6) SHA1(41a337a9a367c4e704a60e32d56b262d03f97b59) )
ROM_END

ROM_START( m4nile )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "gjn08.p1", 0x0000, 0x020000, CRC(2bafac0c) SHA1(363d08f798b5bea409510b1a9415098a69f19ee0) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "gjnsnd.p1", 0x000000, 0x080000, CRC(1d839591) SHA1(2e4ba74f96e7c0592b85409a3f50ec81e00e064c) )
	ROM_LOAD( "gjnsnd.p2", 0x080000, 0x080000, CRC(e2829c42) SHA1(2139c1625ad163cce99a522c2cf02ee47a8f9007) )
	ROM_LOAD( "gjnsnd.p3", 0x100000, 0x080000, CRC(db084eb4) SHA1(9b46a3cb16974942b0edd25b1b080d30fc60c3df) )
	ROM_LOAD( "gjnsnd.p4", 0x180000, 0x080000, CRC(da785b0a) SHA1(63358ab197eb1de8e489a9fd6ffbc2039efc9536) )
ROM_END




ROM_START( m4nudshf )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "nusx.p1", 0x0000, 0x010000, CRC(87caab84) SHA1(e2492ad0d25ded4d760c4cbe05e9b51ca1a10544) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "nus6", 0x0000, 0x010000, CRC(017c5354) SHA1(07491e4b03ab62ad923f8479300c1af4633e3e8c) )
	ROM_LOAD( "nuss.bin", 0x0000, 0x010000, CRC(d3b860ee) SHA1(d5d1262c715e4684748b0cae708eeed31b1dc50f) )
	ROM_LOAD( "nusxc.p1", 0x0000, 0x010000, CRC(e2557b45) SHA1(a9d1514d4fe3897f6fcef22a5039d6bdff8126ff) )
ROM_END



ROM_START( m4ordmnd )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "rab01.p1", 0x0000, 0x020000, CRC(99964fe7) SHA1(3745d09e7a4f417c8e85270d3ffec3e37ee1344d) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "odsnd1.bin", 0x000000, 0x080000, CRC(d746bae4) SHA1(293e1dc9edf88a183cc23dbb4576cefbc8f9d028) )
	ROM_LOAD( "odsnd2.bin", 0x080000, 0x080000, CRC(84ace1f4) SHA1(9cc70e59e9d26006870ea1cc522de33e71b71692) )
	ROM_LOAD( "odsnd3.bin", 0x100000, 0x080000, CRC(b1b12def) SHA1(d8debf8cfb3af2157d5d1571927588dc1c8d07b6) )
ROM_END


ROM_START( m4ptblkc )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "potblackcasinoprg.bin", 0x0000, 0x020000, CRC(29190084) SHA1(c7a778331369c0fac796ef3e306e12c98605f365) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "potblackcasinosnd.p1", 0x000000, 0x080000, CRC(72a3331d) SHA1(b7475ba0ad86a6277e3d4f7b4311a98f3fc29802) )
	ROM_LOAD( "potblackcasinosnd.p2", 0x080000, 0x080000, CRC(c2460eec) SHA1(7c62fbc69ffaa788bf3839e37a75a812a7b8caef) )
ROM_END





ROM_START( m4prem )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dpm14.bin", 0x0000, 0x010000, CRC(de344759) SHA1(d3e7514da83bbf1eba63661fb0675a6230af93cd) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "dpms.bin", 0x0000, 0x080000, CRC(93fd4253) SHA1(69feda7ffc56defd515c9cd1ce204af3d9731a3f) )
ROM_END



















ROM_START( m4rdht )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "drh12", 0x0000, 0x010000, CRC(b26cd308) SHA1(4e29f6cce773232a1c43cd2fb3ce9b844c446bb8) ) // aka gdjb

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "drh_1.snd", 0x0000, 0x080000, CRC(f652cd0c) SHA1(9ce986bc12bcf22a57e065329e82671d19cc96d7) ) // aka gn.snd
ROM_END







ROM_START( m4rwb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "drw14.bin", 0x0000, 0x010000, CRC(22c30ebe) SHA1(479f66732aac56dae60c80d11f05c084865f9389) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "rwb_1.snd", 0x000000, 0x080000, CRC(e0a6def5) SHA1(e3867b83e588fd6a9039b8d45186480a9d0433ea) )
	ROM_LOAD( "rwb_2.snd", 0x080000, 0x080000, CRC(54a2b2fd) SHA1(25875ff873bf22df510e7a4c56c336fbabcbdedb) )
ROM_END






ROM_START( m4magtbo )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "crmtb14.epr", 0x0000, 0x010000, CRC(79e1746c) SHA1(794317f3aba7b1a7994cde89d81abc2b687d0821) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "ctp.chr", 0x0000, 0x000048, CRC(ead61793) SHA1(f38a38601a67804111b8f8cf0a05d35ed79b7ed1) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "scrmtb.snd", 0x000000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END




ROM_START( m4rmtp )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "r4iha202.bin", 0x0000, 0x010000, CRC(b1588632) SHA1(ad21bbc5e99fd6b511e6881e8b20dcad177b937f) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "r4iha203.bin", 0x0000, 0x010000, CRC(7f31cb76) SHA1(9a2a595afb9ff1b3165638d247ab98475ae0bfcd) )
	ROM_LOAD( "r4iha204.bin", 0x0000, 0x010000, CRC(1cc3a32d) SHA1(b6ed012a6d743ba2416e25e7c49ce9985bbacbd7) )
	ROM_LOAD( "r4iha205.bin", 0x0000, 0x010000, CRC(1a238632) SHA1(a15ca5801d41985387bc65579b6d6ee2ef7d8eee) )
	ROM_LOAD( "r4iua202.bin", 0x0000, 0x010000, CRC(c96d630a) SHA1(90ed759602aa3a052434b3f604ec26ec9e204e68) )
	ROM_LOAD( "r4iua203.bin", 0x0000, 0x010000, CRC(550fdfec) SHA1(d57eaba6690cbff2302559e9cea9e5d0f79cf9f9) )
	ROM_LOAD( "r4iua204.bin", 0x0000, 0x010000, CRC(cd8d166f) SHA1(4d78726df35914444be26ac9e1e3e1949b6a3d99) )
	ROM_LOAD( "r4iua205.bin", 0x0000, 0x010000, CRC(46df24f3) SHA1(31000815a90e47e744091bbf0fe9e96baac8d7e3) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "rm.s3", 0x0000, 0x080000, CRC(250e64f2) SHA1(627c4dc5cdc7d0a7cb6f74991ae91b71a2f4dbc6) )
ROM_END

ROM_START( m4rmtpd )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "rdiua202.bin", 0x0000, 0x010000, CRC(faa875ea) SHA1(d8d206fed8965a26dd8ded38a3be018311ccf407) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "r2iha203.bin", 0x0000, 0x010000, CRC(1cea7710) SHA1(a250569800d3679f317a485ac7a31b4f4fa7db78) )
	ROM_LOAD( "r2iha204.bin", 0x0000, 0x010000, CRC(c82cd025) SHA1(f26f2bbd83d673c61bd2609914349b45c31f4a5d) )
	ROM_LOAD( "r2iha205.bin", 0x0000, 0x010000, CRC(e53da9a5) SHA1(5019f5bd89c230459629670b808c59888a0f1ee9) )
	ROM_LOAD( "r2iha206.bin", 0x0000, 0x010000, CRC(f89b73b3) SHA1(34a9a8053e881b8aad578ef58209c8ff888b30f7) )
	ROM_LOAD( "r2iua203.bin", 0x0000, 0x010000, CRC(9590a747) SHA1(9f1a1277bdcbe0f23abcf38850eae939997c2e00) )
	ROM_LOAD( "r2iua205.bin", 0x0000, 0x010000, CRC(2eefca1a) SHA1(cabc0c8a3dddc881aab899c5419663efff5412d3) )
	ROM_LOAD( "r2iua206.bin", 0x0000, 0x010000, CRC(c4a1a218) SHA1(8208468ae9ddde7d387f7194e1f7d44f6e7ca730) )
	ROM_LOAD( "r3iha224.bin", 0x0000, 0x010000, CRC(a2e161ac) SHA1(bd63c9726cdf037919c8655221bc6416cef322aa) )
	ROM_LOAD( "r3iha225.bin", 0x0000, 0x010000, CRC(f49a41e9) SHA1(6c29ba4bf76aaafa79ce68f58f6672baa47fe147) )
	ROM_LOAD( "r3iua224.bin", 0x0000, 0x010000, CRC(715b7de7) SHA1(013827680c389968f2f80f97c565716757d696b2) )
	ROM_LOAD( "r3iua225.bin", 0x0000, 0x010000, CRC(37086f91) SHA1(413b32a8e354467a30c71dce3d1cb76795ff813d) )
	ROM_LOAD( "r4iha201.bin", 0x0000, 0x010000, CRC(789cfca1) SHA1(31aa7bf9461cb6c4f692d605463fde1f604b1614) )
	ROM_LOAD( "r4iua201.bin", 0x0000, 0x010000, CRC(ce0e2553) SHA1(4c9df36a7b8950a273cefceb6ba6817d8b862c78) )
	ROM_LOAD( "rdiha202.bin", 0x0000, 0x010000, CRC(02e01481) SHA1(253c2c8e800a4e6d1008745101e2457d76ac57d4) )
	ROM_LOAD( "rdiha212.bin", 0x0000, 0x010000, CRC(90984ae9) SHA1(b25a12f0529af64315c461363c788c22e30d4016) )
	ROM_LOAD( "rdiha213.bin", 0x0000, 0x010000, CRC(e9fa4c97) SHA1(07c75418890231102cf336f2d3f0048fe4884862) )
	ROM_LOAD( "rdiha214.bin", 0x0000, 0x010000, CRC(42f3a5e0) SHA1(3cf26e55edf0dcde9510e50c4b781ba8b906f092) )
	ROM_LOAD( "rdiha215.bin", 0x0000, 0x010000, CRC(2b704591) SHA1(9b880f40d3b268c96af5dab179760994c5a074c9) )
	ROM_LOAD( "rdiha217.bin", 0x0000, 0x010000, CRC(6df58d97) SHA1(df8f419a1e3acc68a3755c49e258db5af9102598) )
	ROM_LOAD( "rdiha219.bin", 0x0000, 0x010000, CRC(66f3ffa6) SHA1(1b1daf4b02e400d943f2a917be0f4452be891aaf) )
	ROM_LOAD( "rdiha220.bin", 0x0000, 0x010000, CRC(2047c55b) SHA1(8f0e6608271634a6a0f06e76df93dddd404c93cd) )
	ROM_LOAD( "rdiha221.bin", 0x0000, 0x010000, CRC(6e87f591) SHA1(750e9f01c1a3143d7d97a5b9b11d09aed72ca928) )
	ROM_LOAD( "rdiha222.bin", 0x0000, 0x010000, CRC(d200f6ec) SHA1(07e0e270a2184f24373cbe0a8a5e44c3d215d9a2) )
	ROM_LOAD( "rdiha223.bin", 0x0000, 0x010000, CRC(042a5a96) SHA1(3bc2dfb89c6781eb9fb105e5f8ea1576d7b49ad3) )
	ROM_LOAD( "rdihb202.bin", 0x0000, 0x010000, CRC(136c31ec) SHA1(abb095bd4ec0a0879f49e668f1ea08df026262e7) )
	ROM_LOAD( "rdiua204.bin", 0x0000, 0x010000, CRC(a6110b45) SHA1(61d08250fa3b5d7eb7cdf63562d7a6cc9a27372c) )
	ROM_LOAD( "rdiua205.bin", 0x0000, 0x010000, CRC(9f20d810) SHA1(e2a576313fa49fc72001d5de67e93c08423e8dd8) )
	ROM_LOAD( "rdiua206.bin", 0x0000, 0x010000, CRC(2954e2c3) SHA1(e4c9f51748bc1296298f95ca817e852f9e0ca38b) )
	ROM_LOAD( "rdiua207.bin", 0x0000, 0x010000, CRC(58f334d1) SHA1(b91288731750445e4cfcf87fe6a9504723b59fa9) )
	ROM_LOAD( "rdiua208.bin", 0x0000, 0x010000, CRC(13e6d84d) SHA1(6f75a75dfd6922349f8d29c955c1849522f8656c) )
	ROM_LOAD( "rdiua209.bin", 0x0000, 0x010000, CRC(f41af938) SHA1(f2d4e23717f49961fe104971b3a0da9aabbf0e05) )
	ROM_LOAD( "rdiua212.bin", 0x0000, 0x010000, CRC(c56a6433) SHA1(7ff8943843c334a79fc3b40bb004abb3f2c2d079) )
	ROM_LOAD( "rdiua213.bin", 0x0000, 0x010000, CRC(cdd5f399) SHA1(a4359c5166fbcd4ea2bb6820bbfead6bc2b2a4ef) )
	ROM_LOAD( "rdiua214.bin", 0x0000, 0x010000, CRC(04fa9d21) SHA1(c337486ece94a7004420edca677e6688eef1ac9e) )
	ROM_LOAD( "rdiua215.bin", 0x0000, 0x010000, CRC(43d8ca5e) SHA1(e5e24ed24bd5c1135392c98910d2797e621ecbd5) )
	ROM_LOAD( "rdiua217.bin", 0x0000, 0x010000, CRC(3c58970e) SHA1(15b7368078750021202ee7b4886a6510fcc1ba0d) )
	ROM_LOAD( "rdiua219.bin", 0x0000, 0x010000, CRC(54f8fe63) SHA1(48d1b04dde6056b839ec84daa40a7d6871893b3e) )
	ROM_LOAD( "rdiua220.bin", 0x0000, 0x010000, CRC(768715f6) SHA1(5c4102b4d2400806dd0f5a6f3e48da4d290d5255) )
	ROM_LOAD( "rdiua222.bin", 0x0000, 0x010000, CRC(07413d93) SHA1(e752fe382d222eefd4fe975fa40559fedd579320) )
	ROM_LOAD( "rdiua223.bin", 0x0000, 0x010000, CRC(80185ebf) SHA1(ae885325f2c63f7dbb034f8e3f3882d0b36aff99) )
	ROM_LOAD( "rdiub202.bin", 0x0000, 0x010000, CRC(4a8d7cf7) SHA1(cb1525d89d3a411163bfe9e70c8b0d1aa6cefdf5) )
	ROM_LOAD( "rdmha210.bin", 0x0000, 0x010000, CRC(7061373e) SHA1(67da39d1de4f3877f12bd1fd5545046f9dabfde9) )
	ROM_LOAD( "rdmhb210.bin", 0x0000, 0x010000, CRC(12c71e8a) SHA1(9bb45e72f202d3af19988ebf30ea4c2248d387fc) )
	ROM_LOAD( "rdpka316.bin", 0x0000, 0x010000, CRC(5178175d) SHA1(a732a82226c34be0b7f84e9f9e4700bd72da1c19) )
	ROM_LOAD( "rdpka318.bin", 0x0000, 0x010000, CRC(2789179f) SHA1(8d4b1e75995ea5b64fac1a36a98506aacfd1800a) )
	ROM_LOAD( "rdpkb316.bin", 0x0000, 0x010000, CRC(09d4f4c5) SHA1(fbc2b0710ef048c221b007692e9a97b99f1edbc0) )
	ROM_LOAD( "rdpkb318.bin", 0x0000, 0x010000, CRC(428aa7f2) SHA1(f85d173c25d0ab9d8c3c4d87b4fc27c3342b3dec) )
	ROM_LOAD( "rduha511.bin", 0x0000, 0x010000, CRC(823e0323) SHA1(4137a05efe87851a9f9ffcd6519bb57398773095) )
	ROM_LOAD( "rduhb511.bin", 0x0000, 0x010000, CRC(2b65eb19) SHA1(b00543b74ad5262b85f66f5e8cfdaee351f62f23) )

	ROM_REGION( 0x10000, "gal", 0 )
	ROM_LOAD( "rmdxi", 0x0000, 0x000b57, CRC(c16021ec) SHA1(df77e410ea2edae1559e40a877e292f0d1969b0a) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )

ROM_END





ROM_START( m4reeltm )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "real.bin", 0x0000, 0x010000, CRC(5bd54924) SHA1(23fcf13c52ee7b9b39f30f999a9102171fffd642) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "charter.chr", 0x0000, 0x000048, CRC(4ff4eda2) SHA1(092435e34d79775910316a7bed0f90c4f086e5c4) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )

ROM_END

ROM_START( m4ringfr )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "rof03s.p1", 0x0000, 0x020000, CRC(4b4703fe) SHA1(853ce1f5932e09af2b5f3b5314709f13aa35cf19) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* missing? */
ROM_END






ROM_START( m4roadrn )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dro19", 0x0000, 0x010000, CRC(8b591766) SHA1(df156390b427e31cdda64826a6c1d2457c915f25) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "dro_1.snd", 0x000000, 0x080000, CRC(895cfe63) SHA1(02134e149cef3526bbdb6cb93ef3efa283b9d6a2) )
	ROM_LOAD( "dro_2.snd", 0x080000, 0x080000, CRC(1d5c8d4f) SHA1(15c18ae7286807cdc0feb825b958eae808445690) )
ROM_END




ROM_START( m4royjwl )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "rj.bin", 0x0000, 0x020000, CRC(3ffbe4a8) SHA1(47a0309cc9ff315ad9f64e6855863409443e94e2) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "rj_sound1.bin", 0x000000, 0x080000, CRC(443c4901) SHA1(7b3c6737b47dfe04c072f0e157d83c09340c3f9b) )
	ROM_LOAD( "rj_sound2.bin", 0x080000, 0x080000, CRC(9456523e) SHA1(ea1b6bf16b7d1015c188ad83760336d9851de391) )
ROM_END



ROM_START( m4salsa )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dsa15.epr", 0x0000, 0x010000, CRC(22b60b0b) SHA1(4ad184d1557bfd01650684ea9d8ad794fded65f7) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "dsa_1#97c2.snd", 0x0000, 0x080000, CRC(0281a6dd) SHA1(a35a8cd0da32c51f77856ea3eeff7c58fd032333) )
ROM_END






ROM_START( m4showtm )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dsh13.bin", 0x0000, 0x010000, CRC(4ce40ff1) SHA1(f145d6c8e926ca4368d43dacda0fa38615988d84) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "sdsh01s1.snd", 0x0000, 0x080000, CRC(f247ba83) SHA1(9b173503e63a4a861d1380b2ab1fe14af1a189bd) )
ROM_END



















ROM_START( m4squid )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "squidsin.bin", 0x0000, 0x020000, CRC(be369b43) SHA1(e5c7b7a858b264db2f8f726396ddeb42004d7cb9) )

	ROM_REGION( 0x20000, "altbwb", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "sq__x_dx.2_0", 0x0000, 0x020000, CRC(2eb6c814) SHA1(070ad5cb36220daf98043f175cf67d4d584c3d01) )
	ROM_LOAD( "sq__xa_x.2_0", 0x0000, 0x020000, CRC(196a6b34) SHA1(a044ba73b4cf04657ddfcf787dedcb151507ef15) )
	ROM_LOAD( "sq__xb_x.2_0", 0x0000, 0x020000, CRC(53adc362) SHA1(3920f08299bf284ee9f102ce1505d9e9cdc1d1f0) )


	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "squidsnd.p1", 0x000000, 0x080000, CRC(44cebe30) SHA1(a93f64897b4ba333d044649f28fa5dd68d3d2e94) )
	ROM_LOAD( "squidsnd.p2", 0x080000, 0x080000, CRC(d2a1b073) SHA1(d4931f18d369e89492fe72a7a1c511c8d3c23a71) )
ROM_END










ROM_START( m4steptm )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dst11.bin", 0x0000, 0x010000, CRC(3960f210) SHA1(c7c4fe74cb9a53eaa9114a84240de3bce4ffe75e) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "sdun01.bin", 0x0000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END










ROM_START( m4supbjc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sbjs.p1", 0x0000, 0x010000, CRC(f7fb2b99) SHA1(c860d3f95ee3fde02bf00b2e20eeee0ebaf01912) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "sbjd.p1", 0x0000, 0x010000, CRC(555361f4) SHA1(f5327b811ab3421307dc59d209a216798cd54393) )
	ROM_LOAD( "sbjf.p1", 0x0000, 0x010000, CRC(7966deff) SHA1(5cdb6c80ef56b27878eb1fffd6fdf31060e56291) )
	ROM_LOAD( "sbjl.p1", 0x0000, 0x010000, CRC(fc47ed74) SHA1(f29b2caac8168410e534e2f224c98dd4bbb9a7f7) )
	ROM_LOAD( "superbjclub.bin", 0x0000, 0x010000, CRC(68d11d27) SHA1(a0303f845fb5f5b396a7be3ca17a9eaf1a7baef4) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "sbj.chr", 0x0000, 0x000048, CRC(cc4b7911) SHA1(9f8a96a1f8b0f9b33b852e93483ce5c684703349) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "sbjsnd1.hex", 0x000000, 0x080000, CRC(70388bec) SHA1(256fa01b57049d73e88b0bb270fccb555b12dfb7) )
	ROM_LOAD( "sbjsnd2.hex", 0x080000, 0x080000, CRC(1d588554) SHA1(48c092ce83d2f881fc217a3d566e896718ad6f24) )

	ROM_LOAD( "sbj.s1", 0x000000, 0x080000, CRC(9bcba966) SHA1(5ced282aca9d39ebf0828aa19357026d5298e955) )
	ROM_LOAD( "sbj.s2", 0x080000, 0x080000, CRC(1d588554) SHA1(48c092ce83d2f881fc217a3d566e896718ad6f24) )
ROM_END


ROM_START( m4supbf )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sbff.p1", 0x0000, 0x010000, CRC(f27feba0) SHA1(157bf28e2d5fc2fa58bed11b3285cf56ae18abb8) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "sbfs.p1", 0x0000, 0x010000, CRC(c8c52d5e) SHA1(d53513b9faabc307623a7c2f5be0225fb812beeb) )
ROM_END







ROM_START( m4take5 )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "take5.bin", 0x0000, 0x020000, CRC(24beb7d6) SHA1(746beccaf57fd0c54c8cf8d742b8ef50563a40fd) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "tfive1.hex", 0x000000, 0x080000, CRC(70f16892) SHA1(e6448831d3ce7fa251b40023bc7d5d6dee9d6793) )
	ROM_LOAD( "tfive2.hex", 0x080000, 0x080000, CRC(5fc888b0) SHA1(8d50ee4f36bd36aed5d0e7a77f76bd6caffc6376) )
ROM_END







ROM_START( m4techno )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dte13.bin", 0x0000, 0x010000, CRC(cf661d06) SHA1(316b2c42e7253a03b2c12b713821045d9f95a8a7) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "dte13hack.bin", 0x0000, 0x010000, CRC(8b8eafe3) SHA1(93a7714eb4c749b7b19f4f844cf88da9443b0bb7) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "techno.bin", 0x0000, 0x080000, CRC(3e80f8bd) SHA1(2e3a195b49448da11cc0c089a8a9b462894c766b) )
ROM_END








ROM_START( m4toma )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dtk23.bin", 0x0000, 0x010000, CRC(ffba2b96) SHA1(c7635023ac5181e661e808c6b44ac1add58f4f56) )
ROM_END




ROM_START( m4topdk )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dtd26pj.bin", 0x0000, 0x010000, CRC(1f84d995) SHA1(7412632cf79008b980e48f14aea89c3f8d742ed2) )
ROM_END


ROM_START( m4toprn )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "toprun_v1_1.bin", 0xc000, 0x004000, CRC(9b924324) SHA1(7b155467f30cc22f7cda301ae770fb2a889c9c66) )
	ROM_LOAD( "toprun_v1_2.bin", 0x8000, 0x004000, CRC(940fafa9) SHA1(2a8b669c51c8df50710bd8b552ab30a5d1a136ab) )
ROM_END






ROM_START( m4toptim )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "toptimer.bin", 0x0000, 0x010000, CRC(74804012) SHA1(0d9460ba6b1d359d358483c4e8bfd5518f364518) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "dtt2-1.bin", 0x0000, 0x010000, CRC(f9c84a34) SHA1(ad654442f580d6a49658f0e4e39bacbd9d0d0018) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "ttimer.chr", 0x0000, 0x000048, CRC(f694224e) SHA1(936ab5e349fa59accbb37959cce9519fd97f3978) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "toptimer-snd.bin", 0x0000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END







ROM_START( m4tropcl )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "tro20.bin", 0x0000, 0x010000, CRC(5e86c3fc) SHA1(ce2419991559839a8875060c1afe0f030190010a) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "tr2d.p1", 0x0000, 0x010000, CRC(0cc23f89) SHA1(a66c8c28073f53381c43e3e597f15f81c5c61479) )
	ROM_LOAD( "tr2f.p1", 0x0000, 0x010000, CRC(fbdcd06f) SHA1(27ccdc83e60a62227d33d8cf3d516fc43908ab99) )
	ROM_LOAD( "tr2s.p1", 0x0000, 0x010000, CRC(6d43375c) SHA1(5be1dc85374c6a1235e0b137b46ebd7a2d7d922a) )
	ROM_LOAD( "trod.p1", 0x0000, 0x010000, CRC(60c84612) SHA1(84dc8b34e41436331832c1a32ddac0fce269488a) )
	ROM_LOAD( "tros.p1", 0x0000, 0x010000, CRC(5e86c3fc) SHA1(ce2419991559839a8875060c1afe0f030190010a) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "tro20.chr", 0x0000, 0x000048, CRC(97618d38) SHA1(7958e99684d50b9bdb56c97f7fcfe161f0824578) )
ROM_END




ROM_START( m4tbplay )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dtp13", 0x0000, 0x010000, CRC(de424bc3) SHA1(c82dd56a0b3ccea78325cd90ed8e72ed68a1af77) )

	ROM_REGION( 0x10000, "altbwb", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "rmtp4b", 0x0000, 0x010000, CRC(33a1764e) SHA1(7475f460dee015a2cd78fc3e0d1d14fd96fdbb9c) )
	ROM_LOAD( "trmyid", 0x0000, 0x010000, CRC(e7af5944) SHA1(64559c97375a3536f7929d7f4d8d19c30527a3ec) )


	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "dtpchr.chr", 0x0000, 0x000048, CRC(7743df66) SHA1(69b1943837ccf8671861ac8ef690138b41de0e5b) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "dtps10_1", 0x000000, 0x080000, CRC(d1d2c981) SHA1(6a4940248b0bc8df0a9de0d60e98cfebf1962504) )
	ROM_LOAD( "dtps20_1", 0x080000, 0x080000, CRC(f77c4f39) SHA1(dc0e056f4d8c00824b3e672a02da64613bbf204e) )
ROM_END











ROM_START( m4twintm )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "d2t11.bin", 0x0000, 0x010000, CRC(6a76ac6f) SHA1(824912ff1fc3155d11d32b597be53481532fdf5e) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "m533.chr", 0x0000, 0x000048, CRC(b1d7e29b) SHA1(e8ef07f85780e24b5f406478de50287b740379d9) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "sdun01.bin", 0x0000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END


ROM_START( m4twist )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "twist_again_mk29-6", 0x8000, 0x008000, CRC(cb331bee) SHA1(a88099a3f35caf02925f1a3f548fbf65c11e3ec9) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "twistagain-98-mkii.bin", 0x0000, 0x008000, CRC(1cbc7b58) SHA1(eda998a64272fe6796243c2db48ef988b9668c35) )
	ROM_LOAD( "twistagain-mki-27.bin", 0x0000, 0x008000, CRC(357f7072) SHA1(8a23509fff79a83a819b27eff8de8db08c679e3f) )
ROM_END




ROM_START( m4univ )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dun20", 0x0000, 0x010000, CRC(6a845d4d) SHA1(82bfc3f3a0ede76a4d482efc71b0390610db7acf) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "dunchr.chr", 0x0000, 0x000048, CRC(f694224e) SHA1(936ab5e349fa59accbb37959cce9519fd97f3978) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "sdun01.bin", 0x0000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END




ROM_START( m4vegastg )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "vs.p1", 0x0000, 0x020000, CRC(4099d572) SHA1(91a7c1575013e61c754b2c2cb841e7687b76d7f9) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "vssound.bin", 0x0000, 0x16ee37, CRC(456da6be) SHA1(f0e293f0a383878b581326f869c2e49bec61d0c5) )
ROM_END






ROM_START( m4vivalvd )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dlv11.bin", 0x0000, 0x010000, CRC(a890184c) SHA1(26d9952bf2eb4b55d21cdb934ffc73ff1a1cfbac) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "vegssnd.bin", 0x0000, 0x080000, CRC(93fd4253) SHA1(69feda7ffc56defd515c9cd1ce204af3d9731a3f) )
ROM_END







ROM_START( m4voodoo )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ddo32", 0x0000, 0x010000, CRC(260dfef1) SHA1(2b4918e40808963a86d289cd251740a9b0bed70a) )
ROM_END











ROM_START( m4wildtm )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "wildtimer.bin", 0x0000, 0x010000, CRC(5bd54924) SHA1(23fcf13c52ee7b9b39f30f999a9102171fffd642) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "charter.chr", 0x0000, 0x000048, CRC(4ff4eda2) SHA1(092435e34d79775910316a7bed0f90c4f086e5c4) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "wildtimer-snd.bin", 0x0000, 0x080000, CRC(50450909) SHA1(181659b0594ba8d196b7130c5999c91676a363c0) )
ROM_END



ROM_START( m4ch30 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ch301s", 0x0000, 0x010000, CRC(d31c9081) SHA1(21d1f4cc3de2343d830e3ee02e3a53abd12b6b9d) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* missing */
ROM_END
















ROM_START( m4cshenc )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "ca_sj__c.5_1", 0x0000, 0x020000, CRC(d9131b39) SHA1(4af89a7bc10de1406f401bede41e1bc452dbb159) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "ca_sj_bc.5_1", 0x0000, 0x020000, CRC(30d1fb6d) SHA1(f845bef4ad7f2f48077eed74840916e87abb24b2) )
	ROM_LOAD( "ca_sj_dc.5_1", 0x0000, 0x020000, CRC(ac3ec716) SHA1(4ff8c26c46ec6e1321249b4d6d0c5194ed917f33) )
	ROM_LOAD( "ca_sja_c.5_1", 0x0000, 0x020000, CRC(c56a9d0b) SHA1(b0298c2e03097ab8ba5f99892e732ff1ab784c9b) )
	ROM_LOAD( "ca_sjb_c.5_1", 0x0000, 0x020000, CRC(8fad355d) SHA1(2ac16ad85ab8239a3e961abb06f9f71d17e5832a) )

	ROM_REGION( 0x180000, "msm6376", 0 )
	ROM_LOAD( "cesnd.p1", 0x000000, 0x080000, CRC(2a10dc1a) SHA1(f6803f6e1fee2b58fe4831f59ddc08ec02792823) )
	ROM_LOAD( "cesnd.p2", 0x080000, 0x080000, CRC(6f0b75c0) SHA1(33898d75a1e51b49950d7843069066d17c4736c5) )
ROM_END


ROM_START( m4czne )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "czone 6.bin", 0x0000, 0x010000, CRC(e5b2b64e) SHA1(b73a2aed7b04184bc7c5c3d0a11d44e624a47428) )
ROM_END








ROM_START( m4fourmr )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "frmr5p26.bin", 0x8000, 0x008000, CRC(f0c5bd8a) SHA1(39026459008ed5b5bd3a10841799227fef70e5b5) )
ROM_END




ROM_START( m4holywd )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "hollywood 5p.bin", 0x0000, 0x010000, CRC(fb4ebb6e) SHA1(1ccfa81c173011ce70640097c85b532fd44f5a6e) )
ROM_END








ROM_START( m4lazy )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "lb_sj___.1_0", 0x0000, 0x020000, CRC(8628dcf1) SHA1(80cb9348e2704d0f72a44b4aa74b24fe03e279bc) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "lb_sj___.1_2", 0x0000, 0x020000, CRC(2b906f52) SHA1(802bcf6b3679e135308026752a55e55f00f21e85) )
	ROM_LOAD( "lb_sj_d_.1_2", 0x0000, 0x020000, CRC(a7691bad) SHA1(6cda3f3c18c13c04dbe0e4c1e4c817eedc34aa92) )

	ROM_REGION( 0x180000, "msm6376", ROMREGION_ERASE00 )
	/* missing? */
ROM_END


ROM_START( m4lvlcl )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ll__x__x.1_1", 0x0000, 0x010000, CRC(1ef1c5b4) SHA1(455c147f158f8a36a9add9b984abc22af78258cf) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "ll__x__x.3_1", 0x0000, 0x010000, CRC(42b85ebc) SHA1(a352d8389674fcfd90dc4e8155e6f4a78c9ec70d) )
	ROM_LOAD( "ll__x_dx.3_1", 0x0000, 0x010000, CRC(7753c8f0) SHA1(9600fee08529f29716697c4630730f15ef8a457b) )
	ROM_LOAD( "ll__xa_x.3_1", 0x0000, 0x010000, CRC(79468e93) SHA1(4beaa6fe2ad095b4674473ab99a7216513923077) )
	ROM_LOAD( "ll__xb_x.3_1", 0x0000, 0x010000, CRC(73b2fb34) SHA1(c127bc0954f8d01e9d8365a4506dde6f17da33fd) )
	ROM_LOAD( "ll__xgdx.1_1", 0x0000, 0x010000, CRC(65824c4f) SHA1(a514e48ac0f9d4a8d7506bf6932aeee88ca17104) )
	ROM_LOAD( "ll__xgdx.3_1", 0x0000, 0x010000, CRC(ba5b951a) SHA1(9ee36d3d42ce68f5797208633be87ddbbe605cf1) )

	ROM_REGION( 0x180000, "msm6376", ROMREGION_ERASE00 )
	/* missing? */
ROM_END









ROM_START( m4oadrac )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "dr__x__x.2_0", 0x0000, 0x020000, CRC(4ca65bd9) SHA1(deb0a7d3596647210061b69a10fc6cdfc066538e) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "dr__x__x.2_1", 0x0000, 0x020000, CRC(d91773af) SHA1(3d8dda0f409f55bce9c4d4e2a8377e43fe2f1f7d) )
	ROM_LOAD( "dr__x_dx.2_0", 0x0000, 0x020000, CRC(47f3ac5a) SHA1(e0413c55b897e96e32c3332dac041bc94da6dea3) )
	ROM_LOAD( "dr__x_dx.2_1", 0x0000, 0x020000, CRC(f8c36b67) SHA1(c765d7a5eb4d7cd74295da26a7c6f5341a1ca257) )
	ROM_LOAD( "dr__xa_x.2_0", 0x0000, 0x020000, CRC(702f0f7a) SHA1(8529c3eaa33cb972cc38067d176c7c8af0674147) )
	ROM_LOAD( "dr__xa_x.2_1", 0x0000, 0x020000, CRC(cf1fc847) SHA1(6b09c0de15a380da1783a387569d83328f5b29a0) )
	ROM_LOAD( "dr__xb_x.2_0", 0x0000, 0x020000, CRC(3ae8a72c) SHA1(a27faba69430b1d16abf62e0ef37182ab302bbbd) )
	ROM_LOAD( "dr__xb_x.2_1", 0x0000, 0x020000, CRC(85d86011) SHA1(81f8624908299aa37e75fc5d12059b3600212d35) )
	ROM_LOAD( "dri_xa_x.2_0", 0x0000, 0x020000, CRC(849d2a80) SHA1(c9ff0a5a543b62ca5b885f93a35b5f40e88db8c3) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "oad.chr", 0x0000, 0x000048, CRC(910b09db) SHA1(d54399660b1bf1a89712b25292ac99b740442e5c) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "oadsnd1.bin", 0x000000, 0x080000, CRC(b9a9b49b) SHA1(261e939da031768e2a2b5b171cbba55c87d1a758) )
	ROM_LOAD( "oadsnd2.bin", 0x080000, 0x080000, CRC(94e34646) SHA1(8787d6757e4ed86417aafac0e042091189974d3b) )
ROM_END




ROM_START( m4rhs )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "rh_sj___.4s1", 0x0000, 0x020000, CRC(be6179cd) SHA1(8aefffdffb25bc4dd7d083c7027be746181c2ff9) )

	ROM_REGION( 0x20000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "rh_sj__c.6_1", 0x0000, 0x020000, CRC(476f3cf2) SHA1(18ce990e28ca8565ade5eec9a62f0b243121af73) )
	ROM_LOAD( "rh_sj_b_.4s1", 0x0000, 0x020000, CRC(58a4480e) SHA1(f4ecfa1debbfa9dba75263bce2c9f66741c3466f) )
	ROM_LOAD( "rh_sj_bc.6_1", 0x0000, 0x020000, CRC(2e37a58c) SHA1(a48c96384aa81f98bfa980c93e93523ecef3d43c) )
	ROM_LOAD( "rh_sj_d_.4s1", 0x0000, 0x020000, CRC(8f1176db) SHA1(283ef0b9515eac342a02489118bd30016ba85399) )
	ROM_LOAD( "rh_sj_k_.4s1", 0x0000, 0x020000, CRC(3f2ef505) SHA1(28c3806bc48af21a2b7ea27d42ea9f6b4346f3b8) )
	ROM_LOAD( "rh_sja__.4s1", 0x0000, 0x020000, CRC(b8cdd5fb) SHA1(4e336dd3d61f4fdba731951c56e440766ea8efeb) )
	ROM_LOAD( "rh_sja_c.6_1", 0x0000, 0x020000, CRC(b7b790e5) SHA1(e2b34dc2f6ede4f4c22b11123dfaed46f2c5c45e) )
	ROM_LOAD( "rh_sjab_.4s1", 0x0000, 0x020000, CRC(c8468d4c) SHA1(6a9f8fe10949712ecacca3bfcd7d5ab4860682e2) )
	ROM_LOAD( "rh_sjad_.4s1", 0x0000, 0x020000, CRC(df4768f0) SHA1(74894b232b27e65058d59acf174172da86def95a) )
	ROM_LOAD( "rh_sjak_.4s1", 0x0000, 0x020000, CRC(6f78eb2e) SHA1(a9fec7a7ad9334c3d8760e1982ac00651858cee8) )
	ROM_LOAD( "rocky15g", 0x0000, 0x020000, CRC(05f4f333) SHA1(a1b917f6c91d751fb2433e46c4c60840b47eed9e) )
	ROM_LOAD( "rocky15t", 0x0000, 0x020000, CRC(3fbad6de) SHA1(e8d76b3878794c769187d92d2834018a84e764ac) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "rh.chr", 0x0000, 0x000048, CRC(6e246d8f) SHA1(c6b0dff1b918c578b76f020ff70a24ea48dbae2e) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "rhp2snd", 0x0000, 0x080000, CRC(18112293) SHA1(b2bf838849ad1a9931c294ccc291ba2f5c5f45e9) )

	ROM_LOAD( "rh___snd.1_1", 0x000000, 0x080000, CRC(ceebd8f4) SHA1(fe9f62034aae7d2ec097d80dc471a7fd27ddec8a) )
	ROM_LOAD( "rh___snd.1_2", 0x080000, 0x080000, CRC(1f24cfb6) SHA1(cf1dc9d2a1c1cfb8718c89e245e9bf375fef8bfd) )
	ROM_LOAD( "rh___snd.1_3", 0x100000, 0x080000, CRC(726958d8) SHA1(6373765b80971dd7ff5c8eaeee83966335db4d27) )
ROM_END




ROM_START( m4specu )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "speculator.bin", 0x8000, 0x008000, CRC(4035d20c) SHA1(4a534294c5c7332eacd09ca44f351d6a6850cc29) )
ROM_END









ROM_START( m4sunclb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sucxe__0.2", 0x0000, 0x010000, CRC(fd702a6f) SHA1(0f6d553fcb096ca4874bb971425dabfbe18db31d) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "sucxed_0.2", 0x0000, 0x010000, CRC(70802bc3) SHA1(69b36f716cb608931f933cb58e47232b18064f9d) )
ROM_END



ROM_START( m4sunscl )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sc_xe___.3_3", 0x0000, 0x010000, CRC(e3732cc6) SHA1(77f0368bb29ad00030f83af794a52df92fe97e5d) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "sc_xe_d_.3_3", 0x0000, 0x010000, CRC(b8627c4a) SHA1(ad616d38773cbd82376b518aa15dc3d7027237c5) )
	ROM_LOAD( "sc_xef__.3_3", 0x0000, 0x010000, CRC(8e7e1100) SHA1(7648ea860a546081388a213845e27312730f46d9) )
ROM_END





ROM_START( m4ssclas )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cs__x__x.6_0", 0x0000, 0x010000, CRC(3230284d) SHA1(bca3b4c43859ed424956c4119fa6a91a2e7d6eec) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "cs__x_dx.2_0", 0x0000, 0x010000, CRC(ea004a13) SHA1(db9a187b0672c69a6a149ec6d1025bd6da9beccd) )
	ROM_LOAD( "cs__x_dx.6_0", 0x0000, 0x010000, CRC(6dd2d11f) SHA1(8c7e60d3e5a0d4fccb024b5c0aa21fd2b9a5ada9) )
	ROM_LOAD( "cs__xa_x.6_0", 0x0000, 0x010000, CRC(6657e810) SHA1(0860076cf01c732f419483876991fb42a838622a) )
	ROM_LOAD( "cs__xb_x.5_0", 0x0000, 0x010000, CRC(a5f46ff5) SHA1(a068029f774bc6ed2e76acc2eb509bc6e2490945) )
	ROM_LOAD( "cs__xb_x.6_0", 0x0000, 0x010000, CRC(801d543c) SHA1(f0905947312fb2a526765d17cde01af5095ef923) )

	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "ssbwb.chr", 0x0000, 0x000048, CRC(910b09db) SHA1(d54399660b1bf1a89712b25292ac99b740442e5c) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "css_____.1_1", 0x0000, 0x080000, CRC(e738fa1e) SHA1(7a1125320e0d488729aec66e658d418b96228fd0) )
ROM_END




ROM_START( m4ticcla )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ct_20_b4.7_1", 0x0000, 0x010000, CRC(48b9a162) SHA1(2d19a5d6379dc93a56c920b3cd61a0d1a8c6b303) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "ct_20_bc.4_1", 0x0000, 0x010000, CRC(fb40b5ff) SHA1(723a07a2b6b08483aa75ecdd4fd9720a66201fc3) )
	ROM_LOAD( "ct_20_d4.7_1", 0x0000, 0x010000, CRC(3c7c862c) SHA1(a3577f29950e845a14ca68750d2ab6c56a395dba) )
	ROM_LOAD( "ct_20_dc.4_1", 0x0000, 0x010000, CRC(0f20a790) SHA1(02876178f0af64154d490cc048a7bc1c9a6f521b) )
	ROM_LOAD( "ct_20a_4.7_1", 0x0000, 0x010000, CRC(35318095) SHA1(888105a674c9ea8ccad33e24c05ef42936f5f4cf) )
	ROM_LOAD( "ct_20a_c.4_1", 0x0000, 0x010000, CRC(e409f49f) SHA1(8774015ec20ed9fe54e812013dfc12d408276c31) )
	ROM_LOAD( "ct_20b_c.4_1", 0x0000, 0x010000, CRC(864a59cf) SHA1(abd9b7a47c791ce4f91abbd3bf97bdcd9d8296ee) )
	ROM_LOAD( "ct_20bg4.7_1", 0x0000, 0x010000, CRC(7f200f42) SHA1(0ea6aa0de88982737d818c9dac9f2605cea7bc11) )
	ROM_LOAD( "ct_20bgc.4_1", 0x0000, 0x010000, CRC(215b8965) SHA1(883735066a1425b502e89d1234575294ac83746c) )
	ROM_LOAD( "ct_20bt4.7_1", 0x0000, 0x010000, CRC(7c7280a4) SHA1(3dbdc53a3474f4147427ed4fa8a161a3b364d43b) )
	ROM_LOAD( "ct_25_bc.3_1", 0x0000, 0x010000, CRC(9d6fb3b0) SHA1(a6278579d217b5544d9f0b942a7a344596153950) )
	ROM_LOAD( "ct_25_dc.2_1", 0x0000, 0x010000, CRC(b49af435) SHA1(e5f92f114931e554eb8eb5fe89f50298783d541c) )
	ROM_LOAD( "ct_25_dc.3_1", 0x0000, 0x010000, CRC(eb359c82) SHA1(c137768461b859d5277b08c8783b0c8625f9b1be) )
	ROM_LOAD( "ct_25_kc.2_1", 0x0000, 0x010000, CRC(43309e7b) SHA1(d8f6ecbea618da7f54309f2a6e93210c51b68b81) )
	ROM_LOAD( "ct_25a_c.2_1", 0x0000, 0x010000, CRC(717396ed) SHA1(6cdb0f99b40096178f6e85a0966182e704d1b99a) )
	ROM_LOAD( "ct_25a_c.3_1", 0x0000, 0x010000, CRC(28e0a15b) SHA1(b3678ba3d1f392665cc6ec9c24c2c506a41cd4fa) )
	ROM_LOAD( "ct_25b_c.2_1", 0x0000, 0x010000, CRC(b3d7e79c) SHA1(86c0b419c3ca054f8a2ed785cffeb03e6c5b69f2) )
	ROM_LOAD( "ct_25b_c.3_1", 0x0000, 0x010000, CRC(e0ba763d) SHA1(453d8a0dbe616c5a8c4313b918fcfe21fed473e0) )
	ROM_LOAD( "ct_25bgc.2_1", 0x0000, 0x010000, CRC(0869d04c) SHA1(0f0fd3982ac376c66d139655a50639f48bf740b4) )
	ROM_LOAD( "ct_25bgc.3_1", 0x0000, 0x010000, CRC(1e0ca1d1) SHA1(0b1023cdd5cd3db657cea53c85e31ed83c2e5524) )
	ROM_LOAD( "ct_25btc.2_1", 0x0000, 0x010000, CRC(032ec96d) SHA1(c5cef956bc0e3eb45cf128c8d0b4e1d6e5b01afe) )
	ROM_LOAD( "ct_25btc.3_1", 0x0000, 0x010000, CRC(f656897a) SHA1(92ad5c6ce2a696298bbfc8c1750825db4e3bc80b) )
	ROM_LOAD( "ct_30_dc.2_1", 0x0000, 0x010000, CRC(57fabdfb) SHA1(ad86621e4bc8141508c691e148a66e74fc070a88) )
	ROM_LOAD( "ct_30a_c.2_1", 0x0000, 0x010000, CRC(800c94c3) SHA1(c78497899ea9cf27e66f6e8526b95d51215053b2) )
	ROM_LOAD( "ct_30b_c.2_1", 0x0000, 0x010000, CRC(3036ef04) SHA1(de514a85d45d11a880ed147aebe211ffb5bee146) )
	ROM_LOAD( "ct_30bdc.2_1", 0x0000, 0x010000, CRC(9852c9d4) SHA1(37bb20d63fa70ea99e18a16a8f11c461a377a07a) )
	ROM_LOAD( "ct_30bgc.2_1", 0x0000, 0x010000, CRC(a1bc89b4) SHA1(4c82ce8fe78768443823e868f7cc49a06e7cc441) )
	ROM_LOAD( "ct_30btc.2_1", 0x0000, 0x010000, CRC(cde0d12e) SHA1(5427ad700311c30cc86eccc7f1ff36cf0da3b980) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "ct______.5_a", 0x0000, 0x080000, CRC(9a936f50) SHA1(f3f66d6093a939220d24aee985e210cdfd214db4) )
ROM_END




ROM_START( m4ticglc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "tg_25a_c.3_1", 0x0000, 0x010000, CRC(44b2b6b0) SHA1(c2caadd68659bd474df534101e3bc13b15a43694) )

	ROM_REGION( 0x10000, "altrevs", 0 ) /* alternate revisions - to be sorted / split into clones in the future */
	ROM_LOAD( "tg_30_dc.4_1", 0x0000, 0x010000, CRC(19c0fb1e) SHA1(955da095df56f28ace6839c9b6df5669f576730c) )
	ROM_LOAD( "tg_30a_c.4_1", 0x0000, 0x010000, CRC(3e4dcc70) SHA1(c4ad3a8633e19015d4d2b08a653119e9e4c5dcbb) )
	ROM_LOAD( "tg_30b_c.4_1", 0x0000, 0x010000, CRC(83d1517a) SHA1(38a9269dac53ca701e4b621d5e77696142f429cd) )
	ROM_LOAD( "tg_30bgc.4_1", 0x0000, 0x010000, CRC(a366c32d) SHA1(8d86778411ef07e06d99c12147a211d7620af9bf) )

	ROM_REGION( 0x180000, "msm6376", ROMREGION_ERASE00 )
	ROM_LOAD( "ct______.5_a", 0x0000, 0x080000, CRC(9a936f50) SHA1(f3f66d6093a939220d24aee985e210cdfd214db4) )
ROM_END








ROM_START( m4aao )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "aao2_8.bin", 0x0000, 0x010000, CRC(94ce4016) SHA1(2aecb6dbe798b7bbfb3d27f4d115b6611c7d990f) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "aaosnd.bin", 0x0000, 0x080000, CRC(7bf30b96) SHA1(f0086ae239b1d973018a3ea04e816a87f8f20bad) )
ROM_END



ROM_START( m4bandgd )
	ROM_REGION( 0x020000, "maincpu", 0 )
	ROM_LOAD( "bog.bin", 0x0000, 0x020000, CRC(21186fb9) SHA1(3d536098c7541cbdf02d68a18a38cae71155d7ff) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "bandsofgoldsnd.bin", 0x0000, 0x080000, CRC(95c6235f) SHA1(a13afa048b73fabfad229b5c2f8ef5ee9948d9fb) )
ROM_END



ROM_START( m4bigben )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "b_bv2_7.bin", 0x0000, 0x010000, CRC(9f3a7638) SHA1(b7169dc26a6e136d6daaf8d012f4c3d017e99e4a) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "big-bensnd1.bin", 0x000000, 0x080000, CRC(e41c3ec1) SHA1(a0c09f51229afcd14f09bb9080d4f3bb198b2050) )
	ROM_LOAD( "big-bensnd2.bin", 0x080000, 0x080000, CRC(ed71dbe1) SHA1(e67ca3c178caacb99118bacfcd7612e699f40455) )
ROM_END

ROM_START( m4bigbena )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "b_bv2_9.bin", 0x0000, 0x010000, CRC(86a745ee) SHA1(2347e8e38c743ea4d00faee6a56bb77e05c9c94d) ) // aka bb2_9.bin

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "big-bensnd1.bin", 0x000000, 0x080000, CRC(e41c3ec1) SHA1(a0c09f51229afcd14f09bb9080d4f3bb198b2050) )
	ROM_LOAD( "big-bensnd2.bin", 0x080000, 0x080000, CRC(ed71dbe1) SHA1(e67ca3c178caacb99118bacfcd7612e699f40455) )
ROM_END

ROM_START( m4bigbenb )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "bb1_9p.bin", 0x0000, 0x010000, CRC(c76c5a09) SHA1(b0e3b38998428f535841ab5373d57cb0d5b21ed3) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "big-bensnd1.bin", 0x000000, 0x080000, CRC(e41c3ec1) SHA1(a0c09f51229afcd14f09bb9080d4f3bb198b2050) )
	ROM_LOAD( "big-bensnd2.bin", 0x080000, 0x080000, CRC(ed71dbe1) SHA1(e67ca3c178caacb99118bacfcd7612e699f40455) )
ROM_END



ROM_START( m4bigbend )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "bb_2_1.bin", 0x0000, 0x010000, CRC(d3511805) SHA1(c86756998d36e729874c71a5d6442785069c57e9) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "big-bensnd1.bin", 0x000000, 0x080000, CRC(e41c3ec1) SHA1(a0c09f51229afcd14f09bb9080d4f3bb198b2050) )
	ROM_LOAD( "big-bensnd2.bin", 0x080000, 0x080000, CRC(ed71dbe1) SHA1(e67ca3c178caacb99118bacfcd7612e699f40455) )
ROM_END

ROM_START( m4bigbene )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "bbs_2_9p.bin", 0x0000, 0x010000, CRC(0107608d) SHA1(9e5def90e77f65c366aea2a9ac24d5f17c4d0ae8) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "big-bensnd1.bin", 0x000000, 0x080000, CRC(e41c3ec1) SHA1(a0c09f51229afcd14f09bb9080d4f3bb198b2050) )
	ROM_LOAD( "big-bensnd2.bin", 0x080000, 0x080000, CRC(ed71dbe1) SHA1(e67ca3c178caacb99118bacfcd7612e699f40455) )
ROM_END





ROM_START( m4boltbl )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "bfb.bin", 0x8000, 0x008000, CRC(63058a6b) SHA1(ebccc647a937c36ffc6c7cfc01389f04f829999c) )
ROM_END

ROM_START( m4boltbla )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "bfb1.1.bin", 0x8000, 0x008000, CRC(7a91122d) SHA1(28229e86feb4411978e556f7f7bd85bfd996b8aa) )
ROM_END

ROM_START( m4boltblb )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "bfb9 5p cash.bin", 0x8000, 0x008000, CRC(792bff34) SHA1(6996e87f22df6bac7bbe9908534b7e0480f03ede) )
ROM_END

ROM_START( m4boltblc )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "bolt-gilwern.bin", 0x8000, 0x008000, CRC(74e2c821) SHA1(1dcdc58585d1dcfc93e2aeb3df0cd41705cde196) )
ROM_END

ROM_START( m4dblchn )
	ROM_REGION( 0x010000, "maincpu", 0 )
	ROM_LOAD( "doublechance.bin", 0x0000, 0x010000, CRC(6feeeb7d) SHA1(40fe67d854fbf48959e08fdb5743e14d340c16e7) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "doublechancesnd.bin", 0x0000, 0x080000, CRC(3e80f8bd) SHA1(2e3a195b49448da11cc0c089a8a9b462894c766b) )
ROM_END


ROM_START( m4kqclub )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "kingsque.p1", 0x8000, 0x008000, CRC(6501e501) SHA1(e289a9418c640415967fafda43f20877b38e3671) )
ROM_END

ROM_START( m4snookr )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "snooker.ts2", 0x8000, 0x004000, CRC(a6906eb3) SHA1(43b91e88f909b758f880d83df4f889f15aa17eb3) )
	ROM_LOAD( "snooker.ts1", 0xc000, 0x004000, CRC(3e3072dd) SHA1(9ea8b270044b48767a2e6c19e8ed257d5491c1d0) )
ROM_END



ROM_START( m4stakex )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "stakex.bin", 0x0000, 0x010000, CRC(098c7117) SHA1(27f04cfb88ef870fc30afd055cf32ffe448275ea) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "stakexsnd.bin", 0x0000, 0x080000, CRC(baf17991) SHA1(282e0ac9d18299e9f7a0fecaf9edf0cb4205ef0e) )
ROM_END

ROM_START( m4stakexa )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "stakex2.bin", 0x0000, 0x010000, CRC(77ae3f63) SHA1(c5f1cfd5bffcf3156f584757de57ef6530214511) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "stakexsnd.bin", 0x0000, 0x080000, CRC(baf17991) SHA1(282e0ac9d18299e9f7a0fecaf9edf0cb4205ef0e) )
ROM_END


ROM_START( m4stand2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "stand 2 del 8.bin", 0x08000, 0x08000, CRC(a9a5edc7) SHA1(035d3f3b3373cec475753f1b0de2f4db48d6d288) )
ROM_END










ROM_START( m4bigban )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "big04.p1", 0x0000, 0x020000, CRC(f7ead9c6) SHA1(46c10abb892cb6d427ad508aae96752c14b4cb83) )
	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* Missing? */
ROM_END

ROM_START( m4crzcsn )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "crz03.bin", 0x0000, 0x020000, CRC(48610c4f) SHA1(a62ac8b3ee704ee4e98f9d56bfc723d4cbb25b54) )
	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* Missing? */
ROM_END

ROM_START( m4crzcav )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "gcv05.p1", 0x0000, 0x020000, CRC(b9ba46f6) SHA1(78b745d85b36444c39747982987088a772b20a7e) )
	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* Missing? */
ROM_END

ROM_START( m4dragon )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "dgl01.p1", 0x0000, 0x020000, CRC(d7d39c9b) SHA1(5350c9db549edee30815516b1ce74a018390ff3d) )
	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* Missing? */
ROM_END

ROM_START( m4hilonv )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "hnc02.p1", 0x0000, 0x020000, CRC(33a8022b) SHA1(5168b8f32630aa2cb56f30c941695f1728e4fb7a) )
	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* Missing? */
ROM_END

ROM_START( m4octo )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "oct03.p1", 0x0000, 0x020000, CRC(8df66e94) SHA1(e1ab93982846d83becae36b5814ebbd515b9078e) )
	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* Missing? */
ROM_END

ROM_START( m4sctagt )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "gse3_0.p1", 0x0000, 0x010000, CRC(eff705ff) SHA1(6bf96872ef4bcc8f8041c5384d892f072c72be2b) )
	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* Missing? */
ROM_END










ROM_START( m4cld02 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cru0_2.bin", 0x0000, 0x010000, CRC(e3c01944) SHA1(33a2b2c05686f53811349b2980e590fdc4b72756) )
ROM_END

ROM_START( m4barcrz )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "barcrazy.bin", 0x0000, 0x010000, CRC(917ad749) SHA1(cb0a3f6737b8f183d2efb0a3f8adbf86d40a38ff) )

	ROM_REGION( 0x080000, "msm6376", 0 )
	ROM_LOAD( "barcrazysnd.bin", 0x0000, 0x080000, CRC(0e155193) SHA1(7583e9f3e3624f82f2329565bdcbdaa5a5b03ee0) )
ROM_END

ROM_START( m4bonzbn )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "bingo-bonanza_v1.bin", 0x0000, 0x010000, CRC(3d137ddf) SHA1(1ce23db111448e44a166554dd8853dc379e787da) )

	ROM_REGION( 0x100000, "msm6376", 0 )
	ROM_LOAD( "bingo-bonanzasnd1.bin", 0x000000, 0x080000, CRC(e0eb2a92) SHA1(cbc0b3bba7857d87535d1c2a7459aed60709734a) )
	ROM_LOAD( "bingo-bonanzasnd2.bin", 0x080000, 0x080000, CRC(7db27b28) SHA1(98c5fa4bf8c7f67fae90a1ca98b74057f5ed9b6b) )
ROM_END

ROM_START( m4dnj )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "d.n.j 1-02", 0x0000, 0x010000, CRC(5750843d) SHA1(b87923e84071ea4a1af7566a7f413f8e30e208e9) )
	ROM_LOAD( "d.n.j 1-03", 0x0000, 0x010000, CRC(7b805255) SHA1(f62765bfa66e2422ac0a71ebaff27f1ccd470fe2) )
	ROM_LOAD( "d.n.j 1-06", 0x0000, 0x010000, CRC(aab770c7) SHA1(f24fff8346915017bc43fef9fac356a067676d86) )
ROM_END


ROM_START( m4matdr )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "matador.bin", 0x0000, 0x020000, CRC(367788a4) SHA1(3c9b077a64f993cb60107558efdfcbee0fe5c958) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* missing */
ROM_END



ROM_START( m4hslo )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "hot30", 0x0000, 0x010000, CRC(62f2c420) SHA1(5ae89a1b585738255e8d9ae153c3c63b4a2893e4) )
ROM_END

ROM_START( m4sbx )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sbx-2.1-cash.bin", 0x8000, 0x008000, CRC(2dca703e) SHA1(aef398f4ed38ba34f28009058c9486a570f64e0f) )

	ROM_REGION( 0x10000, "altrevs", 0 )
	ROM_LOAD( "b_sbx23.bin", 0x8000, 0x008000, CRC(8188e94f) SHA1(dfbfc549d12c8f7c7db6c12ba766c28f1cf0873f) )
	ROM_LOAD( "s bears v1-4 20p po.bin", 0x8000, 0x008000, CRC(03486714) SHA1(91c237956bbec58cc08a3e92543488d8e2daa673) )
	ROM_LOAD( "s bears v2-4 10p 8.bin", 0x8000, 0x008000, CRC(9b94f8d0) SHA1(9808386def14c8a058730e90135a4d6506e6ed3d) )
	ROM_LOAD( "s bears v2-4 20p po.bin", 0x8000, 0x008000, CRC(ad8f8d9d) SHA1(abd808f95b587a84e8b3aad1af9fe1cb613c9821) )
	ROM_LOAD( "superbea.10p", 0x8000, 0x008000, CRC(70020466) SHA1(473c9feb9ce0024b870612af19ec8a47a7798506) )

	ROM_REGION( 0x40000, "upd", 0 ) // not oki at least...
	ROM_LOAD( "sbsnd", 0x0000, 0x040000, CRC(27fd9fe6) SHA1(856fdc95a833affde0ada7041c68a4b6b729b715) )
ROM_END

ROM_START( m4bclimb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "bc8pv4.bin", 0x8000, 0x008000, CRC(229a7607) SHA1(b20b2c9f9d19ccd6146affdf519fa4bc0322c971) )

	ROM_REGION( 0x40000, "upd", 0 ) // not oki at least...
	ROM_LOAD( "sbsnd", 0x0000, 0x040000, CRC(27fd9fe6) SHA1(856fdc95a833affde0ada7041c68a4b6b729b715) )
ROM_END

ROM_START( m4captb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "c_bear21.rom", 0x8000, 0x008000, CRC(2e9a42e9) SHA1(0c3f33311f1543daf2ff5c0443dc8c000d49c26d) )

	ROM_REGION( 0x40000, "upd", ROMREGION_ERASE00 ) // not oki at least...
//  ROM_LOAD( "sbsnd", 0x0000, 0x040000, CRC(27fd9fe6) SHA1(856fdc95a833affde0ada7041c68a4b6b729b715) )
ROM_END

ROM_START( m4jungj )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "jj2410p.bin", 0x8000, 0x008000, CRC(490838c6) SHA1(a1e9963df9a429ae594592312e977f22f96c6073) )

	ROM_REGION( 0x10000, "altrevs", 0 )
	ROM_LOAD( "jj2420p.bin", 0x8000, 0x008000, CRC(39329ccf) SHA1(6b79e4fc553bad935ec9989ad5ef3e186e720633) )
	ROM_LOAD( "jjv2_4p.bin", 0x8000, 0x008000, CRC(125a8138) SHA1(18c62df5b331bd09d6dcda6280351e94b7b816fd) )
	ROM_LOAD( "jjv4.bin", 0x8000, 0x008000, CRC(bf583156) SHA1(084c5ed3d96c92f265ad08cc7aed7fe6092217a5) )

	ROM_REGION( 0x40000, "upd", ROMREGION_ERASE00 ) // not oki at least...
ROM_END


ROM_START( m4fsx )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "funspotx.10p", 0x8000, 0x008000, CRC(55199f36) SHA1(7af376781e381582b06972725a2022cc28ba60b3) )

	ROM_REGION( 0x10000, "altrevs", 0 )
	ROM_LOAD( "funspotx.20p", 0x8000, 0x008000, CRC(08d1eb6e) SHA1(7c7c02d9c34696d75490df8596ffe64fba93dcc4) )
	ROM_LOAD( "b_fsv1.bin", 0x8000, 0x008000, CRC(b077f944) SHA1(97d96594b8d2d7232bad087cc55912dec02d7484) )

	ROM_REGION( 0x40000, "upd", ROMREGION_ERASE00 ) // not oki at least...
ROM_END


ROM_START( m4ccop )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "cashcop9.bin", 0x0000, 0x010000, CRC(5f993207) SHA1(ab0614e6a1355d275158b1a32f65086e40c2f890) )

	ROM_REGION( 0x80000, "altrevs", 0 )
	ROM_LOAD( "cash-cops_v4-de.bin", 0x0000, 0x010000, CRC(df3da824) SHA1(c275a33e4a89f1b9ecbae80cb7b62007b29b9fd2) )
	ROM_LOAD( "cashcop8.bin", 0x0000, 0x010000, CRC(165603df) SHA1(d301696a340ed136a43c5753c8bf73283a925fd7) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	// 3 different sets of samples(!)
	ROM_LOAD( "cash-copssnd1-de.bin", 0x000000, 0x080000, CRC(cd03f7f7) SHA1(4c09a86bcdf9a9eb224b19b932b75c9db3784fad) )
	ROM_LOAD( "cash-copssnd2-de.bin", 0x080000, 0x080000, CRC(107816a2) SHA1(f5d4a0390b85a665a3536da4689ec91b1a2da3ae) )

	ROM_LOAD( "cash-copssnd1.bin", 0x000000, 0x080000, CRC(776a303d) SHA1(a5a282674674f25bc6ca169eeebee7309239871f) )
	ROM_LOAD( "cash-copssnd2.bin", 0x080000, 0x080000, CRC(107816a2) SHA1(f5d4a0390b85a665a3536da4689ec91b1a2da3ae) )

	ROM_LOAD( "cashcops.p1", 0x000000, 0x080000, CRC(9a59a3a1) SHA1(72cfc99b22ec5fb89714c6d2d66760d86dc19f2f) )
	ROM_LOAD( "cashcops.p2", 0x080000, 0x080000, CRC(deb3e755) SHA1(01f92881c451919be549a1c58afa1fa4630bf171) )
ROM_END

ROM_START( m4ccc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "ccc12.bin", 0x8000, 0x008000, CRC(570cc766) SHA1(036c95ff6428ab38cceb0537dcc990be78fb331a) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	ROM_LOAD( "criss cross crazy sound (27c2001)", 0x0000, 0x040000, CRC(1994c509) SHA1(2bbe91a43aa9953b7776faf67e81e30a4f7b7cb2) )
ROM_END


ROM_START( m4treel )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "trgv1.1s", 0x0000, 0x010000, CRC(a9c76b08) SHA1(a5b3bc980eb58e346cb02d8ca43401f304e5b6de) )

	ROM_REGION( 0x20000, "altrevs", 0 )
	ROM_LOAD( "trgv1.1b", 0x0000, 0x020000, CRC(7eaebef6) SHA1(5ab86329041e7df09cc2e3ce8d5afd44d88c246c) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
ROM_END



ROM_START( m4unkjok )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "joker 10p 3.bin", 0x0000, 0x010000, CRC(009823ac) SHA1(5ab25da5876c87a8d8701f84446bb3d377e4c1ca) )
	ROM_LOAD( "joker 10p 6.bin", 0x0000, 0x010000, CRC(f25f0704) SHA1(35298b49f79c5029277f4777fe88d5e4344c115f) )
	ROM_LOAD( "joker 20p 3 or 6.bin", 0x0000, 0x010000, CRC(cae4397e) SHA1(53b61fd41c97a6ed29ce6a7b555e061ecf2b0ae2) )
	ROM_LOAD( "joker new 20p 6 or 3.bin", 0x0000, 0x010000, CRC(b8d77b97) SHA1(54f69823bb3fd9c2cca014dc7c51913b2d6c8058) )
ROM_END

ROM_START( m4remag )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "remagv2", 0x0000, 0x010000, CRC(80d9c1c2) SHA1(c77d443d92084c324ef75575acca66ffbd9beef3) )
ROM_END

ROM_START( m4rmg )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "rmgicdd", 0x0000, 0x010000, CRC(bd64be0d) SHA1(772b80619c7d514a7a253f35137896d6a73bf4c6) )
ROM_END

ROM_START( m4wnud )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "wnudge.bin", 0x8000, 0x008000, CRC(1d935575) SHA1(c4177c41473c0fb511e0ee035961f55ad43be14d) )
ROM_END

ROM_START( m4t266 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "t2 66.bin", 0x0000, 0x010000, CRC(5c99c6bb) SHA1(7b74e0e5207c00b31cb1859e0cc458c0412a1a07) )
ROM_END

ROM_START( m4brnze )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "bv25", 0x0000, 0x010000, CRC(5c66f460) SHA1(c7587a6e992549ad8814f77c65b33a17a3641431) )
	ROM_LOAD( "bv25v2", 0x0000, 0x010000, CRC(a675edb3) SHA1(a3c6ee6a0bfb301fed72b45ee8e363d77b8b8dbb) )
	ROM_LOAD( "bv55", 0x0000, 0x010000, CRC(93905bc9) SHA1(e8d3cd125dced43fc2cf23cbccc59110561d2a40) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	/* missing */
ROM_END

ROM_START( m4riotrp )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "drt10.bin", 0x0000, 0x010000, CRC(a1badb8a) SHA1(871786ea4e65ecbf61c9a776100321253922d11e) )

	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
	ROM_LOAD( "dblcsnd.bin", 0x0000, 0x080000, CRC(c90fa8ad) SHA1(a98f03d4b6f5892333279bff7537d4d6d887da62) )
ROM_END


















ROM_START( m4surf )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "s_surfin._pound5", 0x0000, 0x020000, CRC(5f800636) SHA1(5b1789890eea44e5275e13f360876374d862935f) )

	ROM_REGION( 0x40000, "altrevs", 0 )
	ROM_LOAD( "s_surfin.upd", 0x0000, 0x020000, CRC(d0bef9cd) SHA1(9d53bfe8d928b190202bf747c0d7bb4cc0ae0efd) )
	ROM_LOAD( "s_surfin._pound15", 0x0000, 0x020000, CRC(eabce7fd) SHA1(4bb2bbcc7d2917eca72385a21ab85d2d94a882ec) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "s_surf.sn1", 0x000000, 0x080000, CRC(f20a7d69) SHA1(7887230613b497dc71a60125dd1e265ebbc8eb23) )
	ROM_LOAD( "s_surf.sn2", 0x080000, 0x080000, CRC(6c4a9074) SHA1(3b993120156677de893e5dc1e0c5d6e0285c5570) )
ROM_END

ROM_START( m4wife )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "moy_wife.p1", 0x0000, 0x020000, CRC(293d35a6) SHA1(980a28ca5e9ec3ca2e1a5b34f658b622dca4cf50) )

	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
	// missing?
ROM_END



ROM_START( m4blkgd )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "blackgoldprg.bin", 0x0000, 0x080000, CRC(a04736b2) SHA1(9e060cc79e7922b38115f1412ed76f8c76deb917) )

	ROM_REGION( 0x40000, "altrevs", 0 )
	ROM_LOAD( "blackgoldversion2.4.bin", 0x0000, 0x040000, CRC(fad4e360) SHA1(23c6a13e8d1ca307b0ef22edffed536675985aca) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "blackgoldsnd1.bin", 0x000000, 0x080000, CRC(d251b59e) SHA1(960b81b87f0fb5000028c863892a273362cb897f) )
	ROM_LOAD( "blackgoldsnd2.bin", 0x080000, 0x080000, CRC(87cbcd1e) SHA1(a6cd186af7c5682e216f549b77735b9bf1b985ae) )
	ROM_LOAD( "blackgoldsnd3.bin", 0x100000, 0x080000, CRC(258f7b83) SHA1(a6df577d98ade8c5c5ff68ef891667e65e83ac17) )
ROM_END


ROM_START( m4zill )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "zillprgh.bin", 0x0000, 0x080000, CRC(6f831f6d) SHA1(6ab6d7f1752d27bc216bc11533b90178ce188715) )

	ROM_REGION( 0x80000, "altrevs", 0 )
	ROM_LOAD( "zillprog.bin", 0x0000, 0x080000, CRC(0f730bab) SHA1(3ea82c8f7d62c70897a5c132273820c9f192cd72) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "zillsnd.bin", 0x0000, 0x080000, CRC(171ed677) SHA1(25d63f4d9c64f13bec4feffa265c5b0c5f6be4ec) )
ROM_END

ROM_START( m4hstr )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "h_s_v1_2.bin", 0x0000, 0x010000, CRC(ef3d3461) SHA1(aa5b1934ab1c6739f36ac7b55d3fda2c640fe4f4) )

	ROM_REGION( 0x80000, "altrevs", 0 )
	ROM_LOAD( "happystreak.p1", 0x0000, 0x080000, CRC(b1f328ff) SHA1(2bc6605965cb5743a2f8b813d68cf1646a4bcac1) )
	ROM_LOAD( "hs2_5.bin", 0x0000, 0x010000, CRC(f669a4c9) SHA1(46813ba7104c97eaa851b50019af9b80046d03b3) )
	ROM_LOAD( "hs2_5p.bin", 0x0000, 0x010000, CRC(71c981aa) SHA1(5effe7487e7216078127d3dc4a0a7ad02ad84390) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "happystreak.p1", 0x0000, 0x080000, CRC(b1f328ff) SHA1(2bc6605965cb5743a2f8b813d68cf1646a4bcac1) ) // alt sound rom

	ROM_LOAD( "happystreaksnd.p1", 0x0000, 0x080000, CRC(76cda195) SHA1(21a985cd6cf1f63f4aa799563099a0527a7c0ea2) )
	ROM_LOAD( "happystreaksnd.p2", 0x080000, 0x080000, CRC(f3b4c763) SHA1(7fd6230c13b66a16daad9d45935c7803a5a4c35c) )
ROM_END

ROM_START( m4hstrcs )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "chs3_6.bin", 0x0000, 0x010000, CRC(d097ae0c) SHA1(bd78c14e7f057f173859bcb1db5e6a142d0c4062) )

	ROM_REGION( 0x80000, "altrevs", 0 )
	ROM_LOAD( "chs3_6p.bin", 0x0000, 0x010000, CRC(57378b6f) SHA1(cf1cf528b9790c1013d87ccf63dcbf59f365067f) )
	ROM_LOAD( "chs3_6pk.bin", 0x0000, 0x010000, CRC(f95f1afe) SHA1(fffa409e8c7148a840d5dedf490fd9f6975e9476) )
	ROM_LOAD( "chs3_6k.bin", 0x0000, 0x010000, CRC(7eff3f9d) SHA1(31dedb0d9476633e8eb947a687c7b8a94b0e182c) )
	ROM_LOAD( "chs_4_2.bin", 0x0000, 0x010000, CRC(ec148b65) SHA1(2d6252ce68719281f5597955227a1f662743f006) )

	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
	/* might use the same roms as the normal version here */
ROM_END


ROM_START( m4ddb )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "ddb3_1.bin", 0x0000, 0x010000, CRC(3b2da727) SHA1(8a677be3b82464d1bf1e97d22adad3b27374079f) )

	ROM_REGION( 0x80000, "altrevs", 0 )
	ROM_LOAD( "ddb3_1p.bin", 0x0000, 0x010000, CRC(bc8d8244) SHA1(9b8e0706b3add42e5e4a8b6c6a2f80a333a2f49e) )

	ROM_REGION( 0x200000, "msm6376", 0 )
	ROM_LOAD( "ddbsound1", 0x000000, 0x080000, CRC(47c87bd5) SHA1(c1578ae553c38e93235cea2142cb139170de2a7e) )
	ROM_LOAD( "ddbsound2", 0x080000, 0x080000, CRC(9c733ab1) SHA1(a83c3ebe99703bb016370a8caf76bdeaff5f2f40) )
ROM_END

ROM_START( m4hapfrt )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "hf1_1.bin", 0x0000, 0x010000, CRC(6c16cb05) SHA1(421b164c8410629956177355e505859757c97a6b) )

	ROM_REGION( 0x80000, "altrevs", 0 )
	ROM_LOAD( "hf1_1p.bin", 0x0000, 0x010000, CRC(ebb6ee66) SHA1(1f9b67260e5becd013d95358cc89acb1099d655d) )
	ROM_LOAD( "hf1_4pk.bin", 0x0000, 0x010000, CRC(0944b3c6) SHA1(00cdb75dda4f8984f77806047ad79fe9a1a8760a) )

	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
ROM_END






ROM_START( m4sunday )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "sunday_sport_v11", 0x0000, 0x010000, CRC(14147d59) SHA1(03b14f4f83a545b3252702267ac012b3be76013d) )
	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
ROM_END

ROM_START( m4jp777 )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "jpot71", 0x0000, 0x010000, CRC(f4564a05) SHA1(97d21e2268e5d99e6e51cb12c45e09445cff1f50) )
	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
ROM_END

ROM_START( m4booze )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "boozecruise10_v10.bin", 0x0000, 0x010000, CRC(b37f752b) SHA1(166f7d17694689bd9d51d859c13ddafa1c6e5e7f) )
	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
ROM_END

ROM_START( m4cbing )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "cherrybingoprg.bin", 0x0000, 0x010000, CRC(00c1d4f3) SHA1(626df7f2f597ed13c32ce0fa8846f2e27ca68eae) )
	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 ) // not oki!
	ROM_LOAD( "cherrybingosnd.p1", 0x000000, 0x100000, CRC(11bed9f9) SHA1(63ed45122dda8e412bb1eaeb967d8a0f925d4bde) )
	ROM_LOAD( "cherrybingosnd.p2", 0x100000, 0x100000, CRC(b2a7ec28) SHA1(307f19ffb46f4a2e8e93923ddb666e50de43a00e) )
ROM_END



ROM_START( m4nod )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "nod.bin", 0x0000, 0x010000, CRC(bc738af5) SHA1(8df436139554ccfb48c4db0a32e3333dbf3c4f46) )
	ROM_REGION( 0x200000, "upd", ROMREGION_ERASE00 )
	ROM_LOAD( "nodsnd.bin", 0x0000, 0x080000, CRC(2134494a) SHA1(3b665bf79567a71195b20e76c50b02707d15b78d) )
ROM_END





ROM_START( m4aliz )
	ROM_REGION( 0x40000, "maincpu", 0 )
	ROM_LOAD( "70000000.bin", 0x0000, 0x040000, CRC(56f64dd9) SHA1(11f990c9a6864a969dc9a4146e1ac2c963e3eb9b) )

	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
	ROM_LOAD( "alizsnd.hi", 0x0000, 0x080000, CRC(c7bd937a) SHA1(cc4d85a3d4cdf57fa96c812a4cd78b599c7052ff) )
	ROM_LOAD( "alizsnd.lo", 0x080000, 0x04e15e, CRC(111cc111) SHA1(413efedbc9e85240df833c10d680b0e907da10b3) )

	ROM_REGION( 0x200000, "misc", ROMREGION_ERASE00 ) // i think this is just the sound roms as intelhex
	ROM_LOAD( "71000000.hi", 0x0000, 0x0bbe9c, CRC(867058c1) SHA1(bd980cb0bb3075854cc2e9b829c31f3742f4f1c2) )
	ROM_LOAD( "71000000.lo", 0x0000, 0x134084, CRC(53046751) SHA1(b8f9eca933315b497732c895f4311f62103344fc) )
ROM_END




ROM_START( m4c2 )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "ci2-0601.bin", 0x0000, 0x010000, CRC(84cc8aca) SHA1(1471e3ad9c9ba957b6cc99c204fe588cc55fbc50) )
	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
ROM_END


ROM_START( m4coney )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "70000060.bin", 0x0000, 0x010000, CRC(fda208e4) SHA1(b1a243b2681faa03add4ab6e4df98814f9c52fc5) )
	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
ROM_END





ROM_START( m4goldnn )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "goldenyears10.bin", 0x0000, 0x020000, CRC(1074bac6) SHA1(967ee64f267a80017fc95bbc6c5a38354e9cab65) )

	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
	ROM_LOAD( "tgyosnd.p1", 0x000000, 0x080000, CRC(bda49b46) SHA1(fac143003641824bf0db4ac6841292e509fa00da) )
	ROM_LOAD( "tgyosnd.p2", 0x080000, 0x080000, CRC(43d28a0a) SHA1(5863e493e84641e4fabcd69e6402e3bcca87dde2) )
	ROM_LOAD( "tgyosnd.p3", 0x100000, 0x080000, CRC(b5b9eb68) SHA1(8d5a0a687dd7096da8dfd2a59c6fe96f4b1949f9) )
ROM_END







ROM_START( m4looplt )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "70000500.bin", 0x0000, 0x080000, CRC(040699a5) SHA1(e1ebc23684c5bc1faaac7409d2179488c3022872) )

	ROM_REGION( 0x80000, "altrevs", 0 )
	ROM_LOAD( "70000500a.bin", 0x0000, 0x080000, CRC(0668f52d) SHA1(6560309facf0022e3c14421b848f212b18be7550) )
	ROM_LOAD( "70000501.bin", 0x0000, 0x080000, CRC(e2fbbfcf) SHA1(fc060468bf5e732626af8c3d0d6fc119a529c330) )
	ROM_LOAD( "70000501a.bin", 0x0000, 0x080000, CRC(42bef934) SHA1(c332eb6566ef5f9ac56d1c3944635296c21b3193) )
	ROM_LOAD( "70000504.bin", 0x0000, 0x080000, CRC(15e2c1c3) SHA1(69257749f1909b7ecc9c94cc2a27a5d4e6608251) )
	ROM_LOAD( "70000505.bin", 0x0000, 0x080000, CRC(f28f59bd) SHA1(d5cdb0c020693c7922c5243f9d18054d47ed039d) )
	ROM_LOAD( "70000506.bin", 0x0000, 0x080000, CRC(a3d40e9a) SHA1(97ac40e814824450e6705bc3240fffd4d0015b46) )
	ROM_LOAD( "70000507.bin", 0x0000, 0x080000, CRC(756eefe4) SHA1(b253fbd94fdab5df32375a02d16d9ba333e8d71c) )
	ROM_LOAD( "70001500.bin", 0x0000, 0x080000, CRC(0b9761a4) SHA1(e7a5e4b90d2e60808a7797d124308973130c440d) )
	ROM_LOAD( "70001500a.bin", 0x0000, 0x080000, CRC(09f90d2c) SHA1(addfd0d20ef9cafba042aa05ee84db85f060b67a) )
	ROM_LOAD( "70001501.bin", 0x0000, 0x080000, CRC(6ce9f76c) SHA1(467701786f8de136c9780a4ef93be6bb932d235d) )
	ROM_LOAD( "70001501a.bin", 0x0000, 0x080000, CRC(ccacb197) SHA1(c7573f309e9c79b2999229c46f78fd0283c4a064) )
	ROM_LOAD( "70001504.bin", 0x0000, 0x080000, CRC(1a7339c2) SHA1(575477d8abe3765d9cd4345336d0f7fa3a69202a) )
	ROM_LOAD( "70001505.bin", 0x0000, 0x080000, CRC(7c9d111e) SHA1(8f98feb70cdcd77b5e7bb6a015c935403a53f428) )

	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
	ROM_LOAD( "71000500.bin", 0x0000, 0x080000, CRC(94fe58f4) SHA1(e07d8e6d4b1e660abc4fa08d703fc0e586f3570d) )
ROM_END


ROM_START( m4mgpn )
	ROM_REGION( 0x40000, "maincpu", 0 )
	ROM_LOAD( "mgp15.p1", 0x0000, 0x010000, CRC(ec76233f) SHA1(aa8595c639c83026d7fe5c3a161f8b08ff9a8b46) )

	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
	ROM_LOAD( "mgpsnd.p1", 0x000000, 0x080000, CRC(d5f0b845) SHA1(6d97d0d4d07407bb0a51e1d62da95c664418a9e9) )
	ROM_LOAD( "mgpsnd.p2", 0x080000, 0x080000, CRC(cefeea06) SHA1(45142ca1bab898dc6f3c32e382ee9157132810a6) )
	ROM_LOAD( "mgpsnd.p3", 0x100000, 0x080000, CRC(be4b3bd0) SHA1(f14c08dc770a24db8bbd00a65d3edf6ee9895ca3) )
	ROM_LOAD( "mgpsnd.p4", 0x180000, 0x080000, CRC(d74b4b03) SHA1(a35c99040a72485a6c2d4a4fdfc203634f6a9ad0) )
ROM_END








ROM_START( m4spotln )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "gsp01.p1", 0x0000, 0x020000, CRC(54c56a07) SHA1(27f21872a7ffe0c497983fa5bbb59e967bf48974) )
	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
ROM_END






ROM_START( m4vivan )
	ROM_REGION( 0x40000, "maincpu", 0 )
	ROM_LOAD( "vlv.bin", 0x0000, 0x010000, CRC(f20c4858) SHA1(94bf19cfa79a1f5347ab61a80cbbce06942187a2) )

	ROM_REGION( 0x200000, "msm6376", ROMREGION_ERASE00 )
	ROM_LOAD( "vlvsound1.bin", 0x0000, 0x080000, CRC(ce4da47a) SHA1(7407f8053ee482db4d8d0732fdd7229aa531b405) )
	ROM_LOAD( "vlvsound2.bin", 0x0000, 0x080000, CRC(571c00d1) SHA1(5e7be40d3caae88dc3a580415f8ab796f6efd67f) )
ROM_END







ROM_START( m4sunseta )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "b2512s.p1", 0x0000, 0x010000, CRC(8c509538) SHA1(eab6a1e44e77cb48cf490616facc74932acc93c5) )
	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "sunsetb.chr", 0x0000, 0x000048, CRC(f166963b) SHA1(5cc6ada61036d8dbeca470e9548f9f5d2bd545a8) )
	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
ROM_END

ROM_START( m4sunsetb )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "b2512y.p1", 0x0000, 0x010000, CRC(65fa2cd9) SHA1(d2ab1ae25d5425a0788f86535a20d3ebe4a9db2b) )
	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "sunsetb.chr", 0x0000, 0x000048, CRC(f166963b) SHA1(5cc6ada61036d8dbeca470e9548f9f5d2bd545a8) )
	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
ROM_END

ROM_START( m4sunsetc )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sunboul-5p3.bin", 0x0000, 0x010000, CRC(5ccbf062) SHA1(cf587018511d1a06624d271f2fde4e40f16ec87c) )
	ROM_REGION( 0x48, "fakechr", 0 )
	ROM_LOAD( "sunsetb.chr", 0x0000, 0x000048, CRC(f166963b) SHA1(5cc6ada61036d8dbeca470e9548f9f5d2bd545a8) )
	ROM_REGION( 0x100000, "msm6376", ROMREGION_ERASE00 )
ROM_END


/* Barcrest */
GAME( 198?, m4tst,        0, mod2    ,   mpu4, mpu4_state,       m4tst,   ROT0, "Barcrest","MPU4 Unit Test (Program 4)",GAME_MECHANICAL )
GAME( 198?, m4tst2,       0, mod2    ,   mpu4, mpu4_state,       m4tst2,  ROT0, "Barcrest","MPU4 Unit Test (Program 2)",GAME_MECHANICAL )
GAME( 198?, m4clr,        0, mod2    ,   mpu4, driver_device,       0,       ROT0, "Barcrest","MPU4 Meter Clear ROM",GAME_MECHANICAL )

#define GAME_FLAGS (GAME_NOT_WORKING|GAME_REQUIRES_ARTWORK)



// needs better reel (correct setup) handling to boot
GAME(199?, m4blkbul	,0			,mod2   	,mpu4				, mpu4_state,m4default_alt		,ROT0,   "Barcrest","Super Play (Black Bull?) (Barcrest) [XSP] (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // XSP??
GAME(199?, m4calicl	,0			,mod2   	,mpu4				, mpu4_state,m4default_alt		,ROT0,   "Barcrest","California Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4bucks	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Bucks Fizz Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4clbtro	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Club Tropicana (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4gldgat	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Golden Gate (Barcrest) [DGG, Dutch] (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4hirise	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","High Rise (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4nspot	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Night Spot Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4supbf	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Super Bucks Fizz Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4toma	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Tomahawk (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4tropcl	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Tropicana Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )

// other issues
GAME(199?, m4casmul	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Casino Multiplay (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // crashes mame
GAME(199?, m4fastfw	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Fast Forward (Barcrest - Bwb) [FFD 1.0] (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )



// barcrest, to split
GAME(199?, m4gb006	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Games Bond 006 (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4c9c	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Cloud Nine Club (Barcrest) (MPU4) (CNC 2.1)",						GAME_FLAGS|GAME_NO_SOUND ) // doesn't boot
GAME(199?, m4ch30	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","unknown MPU4 'CH3 0.1' (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )

// bwb? to split
GAME(199?, m4coscas	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Cosmic Casino (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // bwb?
// corrupt vfd (bwb?)
GAME(199?, m4clbx	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Club X (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4calama	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Calamari Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4calamab,m4calama	,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Calamari Club (Barcrest - Bwb) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4ringfr	,0			,mod4oki    ,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Ring Of Fire (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4royjwl	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Royal Jewels (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(1999, m4squid	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Bwb","Squids In (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )

// play but behavior isn't like barcrest
GAME(199?, m4crjwl	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Crown Jewels Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4crjwl2	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Crown Jewels Mk II Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4supbjc	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Super Blackjack Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // set stake


// check sound type etc.
GAME(199?, m4gbust	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Ghost Buster (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )

// corrupt VFD (many XX)
GAME(199?, m4luckwb	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Lucky Wild Boar (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )

// corrupt VFD on startup, then 'WRONG SOUND' but plays
GAME(199?, m4dbldmn	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Double Diamond Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )

// won't boot with current reel setup, not even in test mode
GAME(199?, m4maglin	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Magic Liner (Barcrest) (MPU4) (DMA2.1)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4magdrg	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Magic Dragon (Barcrest) (MPU4) (DMD1.0)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4holdtm	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Hold Timer (Barcrest) (MPU4) (DHT)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4frtgm	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Fruit Game (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4fortcb	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Fortune Club (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4exgam	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Extra Game (Fairplay - Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4clbveg	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Club Vegas (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4chasei	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Chase Invaders (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4brook	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Brooklyn (Barcrest) (MPU4) (PFT 1.8)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4bluedm	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Blue Diamond (Barcrest) (MPU4) (DBD1.0)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4amhiwy	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","American Highway (Barcrest) (MPU4) (DAH)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4voodoo	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Voodoo 1000 (Barcrest) (Dutch) (MPU4) (DDO 3.2)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4addrd	,m4addr		,mod2   	,mpu4				, mpu4_state,m4default_alt		,ROT0,   "Barcrest","Adders & Ladders (Barcrest) (DAL, Dutch) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4nudshf	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Nudge Shuffle (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4prem	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Premier (Barcrest) (MPU4) (DPM)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4reeltm	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Reel Timer (Barcrest) (MPU4) (DWT)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4rdht	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Red Heat (Golden Nugget?) (Barcrest) (MPU4) (DRH 1.2)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4rwb	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Red White & Blue (Barcrest) (MPU4) (DRW)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4roadrn	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Road Runner (Barcrest) (MPU4) (DRO1.9)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4salsa	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Salsa (Barcrest) (MPU4) (DSA)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4showtm	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Show Timer (Barcrest) (MPU4) (DSH1.3)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4steptm	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Step Timer (Barcrest) (MPU4) (DST 1.1)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4techno	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Techno Reel (Barcrest) (MPU4) (DTE)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4toptim	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Top Timer (Barcrest) (MPU4) (DTT)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4twintm	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Twin Timer (Barcrest) (MPU4) (D2T 1.1)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4univ	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Universe (Barcrest) (MPU4) (DUN)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4wildtm	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Wild Timer (Barcrest) (MPU4) (DWT 1.3)",						GAME_FLAGS|GAME_NO_SOUND )

// GEEN TUBES (even in test mode)
GAME(199?, m4topdk	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Top Deck (Barcrest) (Dutch) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )

// non-english sets
GAME(199?, m4magrep	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Magic Replay (Barcrest) (Dutch) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4nile	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Nile Jewels (Barcrest) (German) (MPU4) (GJN0.8)",						GAME_FLAGS|GAME_NO_SOUND ) // DM1 SW ALM
GAME(199?, m4jokmil	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Jokers Millennium (Barcrest) (German) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // DM1 SW ALM
GAME(199?, m4drac	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Dracula (Barcrest - Nova) (German) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // DM1 SW ALM
GAME(199?, m4crzjwl	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Crown Jewels (Barcrest) (German) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // DM1 SW ALM
GAME(199?, m4vegastg,m4vegast	,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Vegas Strip (Barcrest) [German] (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // 1 DM SW ALM
GAME(199?, m4jok300	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Jokers 300 (Barcrest) (German?) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // also contains crystal maze stuff??
GAME(199?, m4luckdv	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Lucky Devil (Barcrest) [Czech] (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // AUX2 locked
GAME(199?, m4luckdvd,m4luckdv	,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Lucky Devil (Barcrest) [Dutch] (MPU4) (DLD)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4luck7	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Lucky 7 (Barcrest) (Dutch) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // '1,2' error
GAME(199?, m4joljokh,m4joljok	,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Jolly Joker (Barcrest) [Hungarian] (MPU4) (HJJ)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4hpyjok	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Happy Joker (Barcrest) (Dutch) (MPU4) (DHJ1.2)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4gldjok	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Golden Joker (Barcrest) (Dutch) (MPU4) (DGJ 1.2)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4ceptr	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Ceptor (Barcrest) (Dutch) (MPU4) (DCE 1.0)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4blkcat	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Black Cat (Barcrest) (Dutch) (MPU4) (DBL 1.4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4gnsmk	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Gun Smoke (Barcrest) (Dutch) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4blkbuld,m4blkbul	,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Gun Smoke (Barcrest) (Dutch, alt sound roms) (MPU4)",			GAME_FLAGS|GAME_NO_SOUND ) // not sure either set of sound roms is right
GAME(199?, m4blkwhd	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Black & White (Barcrest) [Dutch] (MPU4) (DBW 1.1)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4oldtmr	,0			,mod4oki	,mpu4				, mpu4_state,m_oldtmr			,ROT0,   "Barcrest","Old Timer (Barcrest) (Dutch) (MPU4) (DOT1.1)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4casot	,m4oldtmr	,mod4oki	,mpu4				, mpu4_state,m_oldtmr			,ROT0,   "Barcrest","Old Timer (Barcrest) (Dutch, alt 'Black and White' sound roms) (DOT1.1)",						GAME_FLAGS|GAME_NO_SOUND ) // uses the same program???
GAME(199?, m4jpmcla	,m4oldtmr	,mod4oki	,mpu4				, mpu4_state,m_oldtmr			,ROT0,   "Barcrest","Old Timer (Barcrest) (Dutch, alt 'JPM Classic' sound roms) (DOT1.1)",							GAME_FLAGS|GAME_NO_SOUND ) // uses the same program???
GAME(199?, m4tbplay	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Turbo Play (Barcrest) (Dutch) (MPU4) (DTP)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4vivalvd,m4vivalv	,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Viva Las Vegas (Barcrest) [Dutch] (MPU4) (DLV)",						GAME_FLAGS|GAME_NO_SOUND )

GAME(199?, m4toprn	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Top Run (Barcrest) (Dutch) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // unique behavior  (START UP IN countdown)


// bwb/nova?
GAME(199?, m4ordmnd	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Oriental Diamonds (Barcrest) (German) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
// ?
GAME(199?, m4ptblkc	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Pot Black Casino (Bwb - Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // main cpu crashes?

// strange, bad vfd, all lamps on
GAME(199?, m4rmtp	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Reel Magic Turbo Play / Star Play 300? (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4rmtpd	,0			,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Reel Magic Turbo Play Deluxe (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )

// badchr
GAME(199?, m4take5	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Barcrest","Take 5 (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )

// REEL 1 FAULT
GAME(199?, m4twist	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Twist Again (Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // TA 9.6  REEL 1 FAULT



// might need samples, but run silent with none
GAME(199?, m4lineup	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Line Up (Bwb - Barcrest) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND ) // no sound with any system?

GAME(199?, m4cshenc	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Bwb","Cash Encounters (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4czne	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Cash Zone (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4fourmr	,0			,mod2		,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Four More (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND ) // no sound with either system?
GAME(199?, m4holywd	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Hollywood (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4lazy	,0			,mod4oki    ,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Bwb","Lazy Bones (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4lvlcl	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Lucky Las Vegas Classic (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4oadrac	,0			,mod4oki    ,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Bwb","Ooh Aah Dracula (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4rhs	,0			,mod4oki	,mpu4				, mpu4_state,m4default_bigbank	,ROT0,   "Bwb","Rocky Horror Show (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4specu	,0			,mod2		,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Speculator Club (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND ) // no sound with either system
GAME(199?, m4sunclb	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Sun Club (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4sunscl	,0			,mod2   	,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Sunset Club (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4ssclas	,0			,mod4oki    ,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Super Streak Classic (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4ticcla	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Tic Tac Toe Classic (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4ticglc	,0			,mod4oki	,mpu4				, mpu4_state,m4default			,ROT0,   "Bwb","Tic Tac Toe Gold (Bwb) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4bigban	,0			,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Nova","Big Bandit (Nova) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4crzcsn	,0			,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Nova","Crazy Casino (Nova) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4crzcav	,0			,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Nova","Crazy Cavern (Nova) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4dragon	,0			,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Nova","Dragon (Nova) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4hilonv	,0			,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Nova","Hi Lo Casino (Nova) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4octo	,0			,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Nova","Octopus (Nova) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4sctagt	,0			,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Nova","Secret Agent (Nova) (MPU4)",						GAME_FLAGS|GAME_NO_SOUND )



/* Others */


GAME(199?, m4aao,     0,		mod4oki, mpu4, mpu4_state, m4default, ROT0,   "Eurotek","Against All Odds (Eurotek) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4bandgd,  0,		mod4oki, mpu4, mpu4_state, m4default, ROT0,   "Eurogames","Bands Of Gold (Eurogames) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )

GAME(199?, m4bigben,  0,		mod4oki, mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Big Ben (Coinworld) (MPU4, set 1)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4bigbena, m4bigben,	mod4oki, mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Big Ben (Coinworld) (MPU4, set 2)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4bigbenb, m4bigben,	mod4oki, mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Big Ben (Coinworld) (MPU4, set 3)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4bigbend, m4bigben,	mod4oki, mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Big Ben (Coinworld) (MPU4, set 4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4bigbene, m4bigben,	mod4oki, mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Big Ben (Coinworld) (MPU4, set 5)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4kqclub,  0,		mod2    ,mpu4, mpu4_state, m4default, ROT0,   "Newby","Kings & Queens Club (Newby) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4snookr,  0,		mod2    ,mpu4, mpu4_state, m4default, ROT0,   "Eurocoin","Snooker (Eurocoin) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND ) // works?
GAME(199?, m4stakex,  0,		mod4oki, mpu4, mpu4_state, m4default, ROT0,   "Leisurama","Stake X (Leisurama) (MPU4, set 1)",   GAME_FLAGS|GAME_NO_SOUND ) // can't coin, no sound
GAME(199?, m4stakexa, m4stakex,	mod4oki, mpu4, mpu4_state, m4default, ROT0,   "Leisurama","Stake X (Leisurama) (MPU4, set 2)",   GAME_FLAGS|GAME_NO_SOUND ) // works?
GAME(199?, m4boltbl,  0,		mod2    ,mpu4, mpu4_state, m4default, ROT0,   "DJE","Bolt From The Blue (DJE) (MPU4, set 1)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4boltbla, m4boltbl,	mod2    ,mpu4, mpu4_state, m4default, ROT0,   "DJE","Bolt From The Blue (DJE) (MPU4, set 2)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4boltblb, m4boltbl,	mod2    ,mpu4, mpu4_state, m4default, ROT0,   "DJE","Bolt From The Blue (DJE) (MPU4, set 3)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4boltblc, m4boltbl,	mod2    ,mpu4, mpu4_state, m4default, ROT0,   "DJE","Bolt From The Blue (DJE) (MPU4, set 4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4dblchn,  0,		mod4oki, mpu4, mpu4_state, m4default, ROT0,   "DJE","Double Chance (DJE) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4stand2,  0,		mod2    ,mpu4, mpu4_state, m4default, ROT0,   "DJE","Stand To Deliver (DJE) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )

/* Unknown stuff that looks like it might be MPU4, but needs further verification, some could be bad */

GAME(199?, m4barcrz	,  0,		mod4oki	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","Bar Crazy (unknown) (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4bonzbn	,  0,		mod4oki	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","Bingo Bonanza (unknown) (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4cld02	,  0,		mod4oki	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","unknown MPU4 'CLD 0.2C' (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4dnj	,  0,		mod4oki	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","Double Nudge (unknown) (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4matdr	,  0,		mod4oki	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","Matador (unknown) (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4hslo	,  0,		mod2	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","unknown MPU4 'HOT 3.0' (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4unkjok	,  0,		mod2	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","unknown MPU4 'Joker' (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4remag	,  0,		mod2	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","unknown MPU4 'ZTP 0.7' (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4rmg	,  0,		mod2	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","unknown MPU4 'CTP 0.4' (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4wnud	,  0,		mod2	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","unknown MPU4 'W Nudge' (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4t266	,  0,		mod2	,mpu4, mpu4_state, m4default, ROT0,   "<unknown>","unknown MPU4 'TTO 1.1' (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4brnze	,  0,		mod4oki, mpu4, mpu4_state, m4default, ROT0,   "<unknown>","Bronze Voyage (unknown) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4riotrp	,  0,		mod4oki, mpu4, mpu4_state, m4default, ROT0,   "<unknown>","Rio Tropico (unknown) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )

/* *if* these are MPU4 they have a different sound system at least - The copyright strings in them are 'AET' tho (Ace?) - Could be related to the Crystal stuff? */
GAME(199?, m4sbx	,  0,		mpu4crys	,mpu4, mpu4_state, m_frkstn, ROT0,   "AET/Coinworld","Super Bear X (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4bclimb	,  0,		mpu4crys	,mpu4, mpu4_state, m_frkstn, ROT0,   "AET/Coinworld","Bear Climber (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4captb	,  0,		mpu4crys	,mpu4, mpu4_state, m_frkstn, ROT0,   "AET/Coinworld","Captain Bear (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4jungj	,  0,		mpu4crys	,mpu4, mpu4_state, m_frkstn, ROT0,   "AET/Coinworld","Jungle Japes (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4fsx	,  0,		mpu4crys	,mpu4, mpu4_state, m_frkstn, ROT0,   "AET/Coinworld","Fun Spot X (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4ccop	,  0,		mod4oki		,mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Cash Cops (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4ccc	,  0,		mod4oki		,mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Criss Cross Crazy (Coinworld) (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4treel	,  0,		mod2		,mpu4, mpu4_state, m4default, ROT0,   "Jpm","Turbo Reels (Jpm) (MPU4?)",   GAME_FLAGS|GAME_NO_SOUND )




GAME(199?, m4surf, 0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Gemini","Super Surfin' (Gemini) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4wife, 0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Gemini","Money Or Yer Wife (Gemini) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4blkgd, 0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Gemini","Black Gold (Gemini) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )

GAME(199?, m4zill, 0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Pure Leisure","Zillionare's Challenge (Pure Leisure) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )

GAME(199?, m4hstr, 0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Happy Streak (Coinworld) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4hstrcs, 0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Casino Happy Streak (Coinworld) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4ddb,  0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Ding Dong Bells (Coinworld) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4hapfrt,  0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Coinworld","Happy Fruits (Coinworld) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )


GAME(199?, m4sunday, 0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Pcp","Sunday Sport (Pcp) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4jp777, 0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Cotswold Microsystems","Jackpot 777 (Cotswold Microsystems) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4booze, 0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Extreme","Booze Cruise (Extreme) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4cbing, 0,			mod4oki    ,mpu4, mpu4_state, m4default, ROT0,   "Redpoint Systems","Cherry Bingo (Redpoint Systems) (MPU4)",   GAME_FLAGS|GAME_NO_SOUND ) // custom sound system


GAME( 199?, m4nod		, 0			,  mod4oki		, mpu4		, mpu4_state, m4default		, 0,		 "Eurotech",   "Nod And A Wink (Eurotech) (MPU4)", GAME_FLAGS|GAME_MECHANICAL|GAME_SUPPORTS_SAVE) // this has valid strings in it BEFORE the bfm decode, but decodes to valid code, does it use some funky mapping, or did they just fill unused space with valid looking data?


// not sure about several of the nova ones
GAME( 199?, m4aliz		, 0			,  mod4oki		, mpu4		, mpu4_state, m4default		, 0,		 "Qps",  "AlizBaz (Qps) (German) (MPU4)", GAME_FLAGS|GAME_MECHANICAL|GAME_SUPPORTS_SAVE)
GAME( 199?, m4coney		, 0			,  mod4oki		, mpu4		, mpu4_state, m4default		, 0,		 "Qps",   "Coney Island (Qps) (MPU4)", GAME_FLAGS|GAME_MECHANICAL|GAME_SUPPORTS_SAVE)
GAME( 199?, m4looplt	, 0			,  mod4oki		, mpu4		, mpu4_state, m4default		, 0,		 "Qps",   "Loop The Loot (Qps) (MPU4)", GAME_FLAGS|GAME_MECHANICAL|GAME_SUPPORTS_SAVE)
GAME( 199?, m4crzjk		, 0			,  mod2   		, mpu4		, mpu4_state, m4default		, 0,		 "Nova?", "Crazy Jokers (Nova?) (MPU4)",GAME_FLAGS|GAME_NO_SOUND )  // COIN   ALM
GAME( 199?, m4c2		, 0			,  mod4oki		, mpu4		, mpu4_state, m4default		, 0,		 "Nova?", "Circus Circus 2 (Nova?) (MPU4)", GAME_FLAGS|GAME_MECHANICAL|GAME_SUPPORTS_SAVE) // COIN   ALM
// regular barcrest structure
GAME( 199?, m4vivan		, 0			,  mod4oki		, mpu4		, mpu4_state, m4default		, 0,		 "Nova",  "Viva Las Vegas (Nova) (MPU4)", GAME_FLAGS|GAME_MECHANICAL|GAME_SUPPORTS_SAVE)
GAME( 199?, m4spotln	, 0			,  mod4oki		, mpu4		, mpu4_state, m4default		, 0,		 "Nova",  "Spotlight (Nova) (MPU4)", GAME_FLAGS|GAME_MECHANICAL|GAME_SUPPORTS_SAVE)
GAME( 199?, m4mgpn		, 0			,  mod4oki		, mpu4		, mpu4_state, m4default		, 0,		 "Nova",  "Monaco Grand Prix (Nova) (MPU4)", GAME_FLAGS|GAME_MECHANICAL|GAME_SUPPORTS_SAVE)
GAME( 199?, m4goldnn	, 0			,  mod4oki		, mpu4		, mpu4_state, m4default		, 0,		 "Nova",  "Golden Years (Nova) (MPU4)", GAME_FLAGS|GAME_MECHANICAL|GAME_SUPPORTS_SAVE)



// these don't contain a valid vector in the first bank?



GAME(199?, m4sunseta	,m4sunset	,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Sunset Boulevard (Barcrest) (MPU4) (B25 1.2, set 1)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4sunsetb	,m4sunset	,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Sunset Boulevard (Barcrest) (MPU4) (B25 1.2, set 2)",						GAME_FLAGS|GAME_NO_SOUND )
GAME(199?, m4sunsetc	,m4sunset	,mod4oki   	,mpu4				, mpu4_state,m4default			,ROT0,   "Barcrest","Sunset Boulevard (Barcrest) (MPU4) (OSB 0.2)",						GAME_FLAGS|GAME_NO_SOUND ) // might be a mod 2
