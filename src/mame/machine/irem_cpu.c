/*****************************************************************************

    Irem Custom V30 CPU:

    It uses a simple opcode lookup encryption, the painful part is that it's
    preprogrammed into the cpu and isn't a algorithmic based one.

    Hasamu                             Nanao   08J27261A1 011 9102KK700
    Gunforce                           Nanao   08J27261A1 011 9106KK701
    Ken-Go                             Nanao   08J27261A1 011 9102KK701
    Bomberman                          Nanao   08J27261A1 012 9123KK200
    Atomic Punk                        Nanao   08J27291A1 012 9128KK440
    Blade Master / Cross Blades!       Nanao   08J27291A1 012 9123KK740
    Quiz F-1 1,2 Finish                Nanao   08J27291A4 014 9147KK700
    Gunforce 2                         Nanao   08J27291A4 014 9247KK700
    Lethal Thunder                     Nanao   08J27291A4 014 9147KK700
    Bomberman World / New Atomic Punk  Nanao   08J27291A5 015 9219KK700
    Undercover Cops                    Nanao   08J27291A5 015 9219KK700
    Gun Hohki                          Nanao   08J27291A6 016 9217NK700
    Skins Game                         Nanao   08J27291A7 017
    Hook                               Nanao   08J27291A8 018 9237NK700

    R-Type Leo                         Irem    D8000021A1 019 9242NK700
    Fire Barrel                        Irem    D8000010A1 019 9243NK700
    In The Hunt                        Irem    D8000011A1 020
    Risky Challenge/Gussun Oyoyo       Irem    D8000019A1 022 9331NK700
    Match It II/Shisensho II           Irem    D8000020A1 023 9320NK700
    World PK Soccer                    Irem    D8000021A1
    Ninja Baseball Batman              Irem    D8000021A1 024 9335NK700
    Perfect Soldiers                   Irem    D8000022A1
    Dream Soccer '94                   Irem    D8000023A1 026

    Please let me know if you can fill in any of the blanks.

    Emulation by Bryan McPhail, mish@tendril.co.uk, thanks to Chris Hardy too!

*****************************************************************************/

#include "driver.h"
#include "irem_cpu.h"

int m90_game_kludge;

// CAVEATS:
// 0x80 and 0x82 pre- opcodes can easily be confused. They perform exactly the same
// function when operating on memory, but when working with registers one affects
// byte registers and the other word registers. Gunforce, Blade Master and
// Lethal Thunder had this error.

//double check 0x00 0x22 0x28 0x4a 0x34 in these tables

#define xxxx 0x90/* Unknown */

const UINT8 gunforce_decryption_table[256] = {
	0xff,xxxx,xxxx,0x2c,xxxx,xxxx,0x43,0x88, xxxx,0x13,0x0a,0xbd,0xba,0x60,0xea,xxxx, /* 00 */
	xxxx,xxxx,0xf2,0x29,0xb3,0x22,xxxx,0x0c, 0xa9,0x5f,0x9d,0x07,xxxx,xxxx,0x0b,0xbb, /* 10 */
	0x8a,xxxx,xxxx,xxxx,0x3a,0x3c,0x5a,0x38, 0x99,xxxx,0xf8,0x89,xxxx,0x91,xxxx,0x55, /* 20 */
	0xac,0x40,0x73,xxxx,0x59,xxxx,0xfc,xxxx, 0x50,0xfa,xxxx,0x25,xxxx,0x34,0x47,0xb7, /* 30 */
	xxxx,xxxx,xxxx,0x49,xxxx,0x0f,0x8b,0x05, 0xc3,0xa5,0xbf,0x83,0x86,0xc5,xxxx,xxxx, /* 40 */
	0x28,0x77,0x24,0xb4,xxxx,0x92,xxxx,0x3b, 0x5e,0xb6,0x80,0x0d,0x2e,0xab,0xe7,xxxx, /* 50 */
	0x48,xxxx,0xad,0xc0,xxxx,0x1b,0xc6,0xa3, 0x04,xxxx,xxxx,xxxx,0x16,0xb0,0x7d,0x98, /* 60 */
	0x87,0x46,0x8c,xxxx,xxxx,0xfe,xxxx,0xcf, xxxx,0x68,0x84,xxxx,0xd2,xxxx,0x18,0x51, /* 70 */
	0x76,0xa4,0x36,0x52,0xfb,xxxx,0xb9,xxxx, xxxx,0xb1,0x1c,0x21,0xe6,0xb5,0x17,0x27, /* 80 */
	0x3d,0x45,0xbe,0xae,xxxx,0x4a,0x0e,0xe5, xxxx,0x58,0x1f,0x61,0xf3,0x02,xxxx,0xe8, /* 90 */
	xxxx,xxxx,xxxx,0xf7,0x56,0x96,xxxx,0xbc, 0x4f,xxxx,xxxx,0x79,0xd0,xxxx,0x2a,0x12, /* A0 */
	0x4e,0xb8,xxxx,0x41,xxxx,0x90,0xd3,xxxx, 0x2d,0x33,0xf6,xxxx,xxxx,0x14,xxxx,0x32, /* B0 */
	0x5d,0xa8,0x53,0x26,0x2b,0x20,0x81,0x75, 0x7f,0x3e,xxxx,xxxx,0x00,0x93,xxxx,0xb2, /* C0 */
	0x57,xxxx,0xa0,xxxx,0x39,xxxx,xxxx,0x72, xxxx,0x01,0x42,0x74,0x9c,0x1e,xxxx,0x5b, /* D0 */
	xxxx,0xf9,xxxx,0x2f,0x85,xxxx,0xeb,0xa2, xxxx,0xe2,0x11,xxxx,0x4b,0x7e,xxxx,0x78, /* E0 */
	xxxx,xxxx,0x09,0xa1,0x03,xxxx,0x23,0xc1, 0x8e,0xe9,0xd1,0x7c,xxxx,xxxx,0xc7,0x06, /* F0 */
};
// 0x13 (0x29) guess
// 0x18 (0xa9) guess
// 0x50 (0x28) guess
// 0x63 (0xc0) guess
// 0x7e (0x18) opcode is right but arguments could be swapped
// 0xcc (0x00) guess
// 0xea (0x11) guess
// 0x51 (0x77) guess (kengo)
// 0x96 (0x0e) complete guess (kengo), maybe wrong but I don't see what it could be


//double check 22 (boot bomb at 2a000)
//47a7 (46e0 in boot) - hmm

// 0x00 is NOT 0x20 (no context in bomberman)

const UINT8 bomberman_decryption_table[256] = {
	xxxx,xxxx,0x79,xxxx,0x9d,0x48,xxxx,xxxx, xxxx,xxxx,0x2e,xxxx,xxxx,0xa5,0x72,xxxx, /* 00 */
	0x46,0x5b,0xb1,0x3a,0xc3,xxxx,0x35,xxxx, xxxx,0x23,xxxx,0x99,xxxx,0x05,xxxx,0x3c, /* 10 */
	0x3b,0x76,0x11,xxxx,xxxx,0x4b,xxxx,0x92, xxxx,0x32,0x5d,xxxx,0xf7,0x5a,0x9c,xxxx, /* 20 */
	0x26,0x40,0x89,xxxx,xxxx,xxxx,xxxx,0x57, xxxx,xxxx,xxxx,xxxx,xxxx,0xba,0x53,0xbb, /* 30 */
	0x42,0x59,0x2f,xxxx,0x77,xxxx,xxxx,0x4f, 0xbf, 0x4a/*0x41*/, 0xcb,0x86,0x62,0x7d,xxxx,0xb8, /* 40 */
	xxxx,0x34,xxxx,0x5f,xxxx,0x7f,0xf8,0x80, 0xa0,0x84,0x12,0x52,xxxx,xxxx,xxxx,0x47, /* 50 */
	xxxx,0x2b,0x88,0xf9,xxxx,0xa3,0x83,xxxx, 0x75,0x87,xxxx,0xab,0xeb,xxxx,0xfe,xxxx, /* 60 */
	xxxx,0xaf,0xd0,0x2c,0xd1,0xe6,0x90,0x43, 0xa2,0xe7,0x85,0xe2,0x49,0x22,0x29,xxxx, /* 70 */
	0x7c,xxxx,xxxx,0x9a,xxxx,xxxx,0xb9,xxxx, 0x14,0xcf,0x33,0x02,xxxx,xxxx,xxxx,0x73, /* 80 */
	xxxx,0xc5,xxxx,xxxx,xxxx,0xf3,0xf6,0x24, xxxx,0x56,0xd3,xxxx,0x09,0x01,xxxx,xxxx, /* 90 */
	0x03,0x2d,0x1b,xxxx,0xf5,0xbe,xxxx,xxxx, 0xfb,0x8e,0x21,0x8d,0x0b,xxxx,xxxx,0xb2, /* A0 */
	0xfc,0xfa,0xc6,xxxx,0xe8,0xd2,xxxx,0x08, 0x0a,0xa8,0x78,0xff,xxxx,0xb5,xxxx,xxxx, /* B0 */
	0xc7,0x06,0x18,xxxx,xxxx,0x1e,0x7e,0xb0, 0x0e,0x0f,xxxx,xxxx,0x0c,0xaa,0x55,xxxx, /* C0 */
	xxxx,0x74,0x3d,xxxx,xxxx,0x38,0x27,0x50, xxxx,0xb6,0x5e,0x8b,0x07,0xe5,0x39,0xea, /* D0 */
	0xbd,xxxx,0x81,0xb7,xxxx,0x8a,0x0d,xxxx, 0x58,0xa1,0xa9,0x36,xxxx,0xc4,xxxx,0x8f, /* E0 */
	0x8c,0x1f,0x51,0x04,0xf2,xxxx,0xb3,0xb4, 0xe9,0x2a,xxxx,xxxx,xxxx,0x25,xxxx,0xbc, /* F0 */
};


const UINT8 lethalth_decryption_table[256] = {
	0x7f,0x26,0x5d,xxxx,0xba,xxxx,0x1e,0x5e, 0xb8,xxxx,0xbc,0xe8,0x01,xxxx,0x4a,0x25, /* 00 */
	xxxx,0xbd,xxxx,0x22,0x10,xxxx,0x02,0x57, 0x70,xxxx,0x7c,xxxx,0xe7,0x52,xxxx,0xa9, /* 10 */
//                      ^^^^                 ^^^^
	xxxx,xxxx,0xc6,0x06,0xa0,0xfe,0xcf,0x8e, 0x43,0x8f,0x2d,xxxx,0xd4,0x85,0x75,0xa2, /* 20 */
//                                                ^^^^
	0x3d,xxxx,xxxx,0x38,0x7c,0x89,0xd1,0x80, 0x3b,0x72,0x07,xxxx,0x42,0x37,0x0a,0x18, /* 30 */
//                                                                    ^^^^
	0x88,0xb4,0x98,0x8b,0xb9,0x9c,0xad,0x0e, 0x2b,xxxx,0xbf,xxxx,0x55,xxxx,0x56,0xb0, /* 40 */
//                                     ^^^^
	0x93,0x91,xxxx,0xeb,xxxx,0x50,0x41,0x29, 0x47,xxxx,xxxx,0x60,xxxx,0xab,xxxx,xxxx, /* 50 */
	0xc3,0xe2,0xd0,0xb2,0x11,0x79,xxxx,0x08, xxxx,0xfb,xxxx,0x2c,0x23,xxxx,0x28,0x0d, /* 60 */
	xxxx,xxxx,xxxx,0x83,0x3c,xxxx,0x1b,0x34, 0x5b,xxxx,0x40,xxxx,xxxx,0x04,0xfc,0x09, /* 70 */
//                                                                              ^^^^
	0xb1,0xf3,0x8a,xxxx,xxxx,0x87,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,0xbe,0x84,0x1f,0xe6, /* 80 */
	0xff,xxxx,0x12,xxxx,0xb5,0x36,xxxx,0xb3, xxxx,xxxx,xxxx,0xd2,0x4e,xxxx,xxxx,xxxx, /* 90 */
	0xa5,xxxx,xxxx,0xc7,xxxx,0x27,0x0b,xxxx, 0x20,xxxx,xxxx,xxxx,xxxx,xxxx,0x61,0x7f, /* A0 */
	xxxx,xxxx,0x86,0x0f,xxxx,0xb7,xxxx,0x4f, xxxx,xxxx,0xc0,0xfd,xxxx,0x39,xxxx,0x7d, /* B0 */
	0x05,0x3a,xxxx,0x48,0x92,0x7a,0x3e,0x03, xxxx,0xf8,xxxx,0x59,0xa8,0x5f,0xf9,0xbb, /* C0 */
	0x81,0xfa,0x9d,0xe9,0x2e,0xa1,0xc1,0x33, xxxx,0x78,xxxx,0x0c,xxxx,0x24,0xaa,0xac, /* D0 */
	xxxx,0xb6,xxxx,0xea,xxxx,0x73,0xe5,0x58, 0x00,0xf7,xxxx,0x74,xxxx,0x7e,xxxx,0xa3, /* E0 */
	xxxx,0x5a,0xf6,0x32,0x46,0x2a,xxxx,xxxx, 0x53,0x4b,0x90,xxxx,0x51,0x68,0x99,0x13, /* F0 */
//                                                                              ^^^^
};
// 0x2c (0xd4) complete guess
// 0x2d (0x85) complete guess
// 0xc4 (0x92) guess
// 0xbb (0xfd) guess
// 0x46 (0xad) guess
// 0x6e (0x28) guess
// 0x76 (0x1b) guess
// 0x8d (0x84) guess
// 0xa6 (0x0b) guess
// 0xa8 (0x20) guess
// 0xbd (0x39) guess
// 0xc3 (0x48) guess
// and our collection of conditional branches:
// 0xbf (0x7d) >= (monitor test)
// 0x34 (0x7c) < or <= (seems more like <) | these two are used toghether
// 0xaf (0x7f) > or >= (seems more like >) |
// 0xed (0x7e) <= or < (seems more like <=)
// 0x00 (0x7f) > ? | these two are used toghether
// 0x1a (0x7c) < ? | (rowscroll on pink screen on startup)
// 0xc5 (0x7a) completely in the dark (game start after car seelction)


const UINT8 dynablaster_decryption_table[256] = {
	0x1f,0x51,0x84,xxxx,0x3d,0x09,0x0d,xxxx, xxxx,0x57,xxxx,xxxx,xxxx,0x32,0x11,xxxx, /* 00 */
	xxxx,0x9c,xxxx,xxxx,0x4b,xxxx,xxxx,0x03, xxxx,xxxx,xxxx,0x89,0xb0,xxxx,xxxx,xxxx, /* 10 */
	xxxx,0xbb,0x18,0xbe,0x53,0x21,0x55,0x7c, xxxx,xxxx,0x47,0x58,0xf6,xxxx,xxxx,0xb2, /* 20 */
	0x06,xxxx,0x2b,xxxx,0x2f,0x0b,0xfc, 0x91 , xxxx,xxxx,0xfa,0x81,0x83,0x40,0x38,xxxx, /* 30 */
	xxxx,xxxx,0x49,0x85,0xd1,0xf5,0x07,0xe2, 0x5e,0x1e,xxxx,0x04,xxxx,xxxx,xxxx,0xb1, /* 40 */
	0xc7,xxxx,0x96, 0xf2 /*0xaf*/, 0xb6,0xd2,0xc3,xxxx, 0x87,0xba,0xcb,0x88,xxxx,0xb9,0xd0,0xb5, /* 50 */
	0x9a,0x80,0xa2,0x72,xxxx,0xb4,xxxx,0xaa, 0x26,0x7d,0x52,0x33,0x2e,0xbc,0x08,0x79, /* 60 */
	0x48,xxxx,0x76,0x36,0x02,xxxx,0x5b,0x12, 0x8b,0xe7,xxxx,xxxx,xxxx,0xab,xxxx,0x4f, /* 70 */
	xxxx,xxxx,0xa8,0xe5,0x39,0x0e,0xa9,xxxx, xxxx,0x14,xxxx,0xff, 0x7f/*0x75*/ ,xxxx,xxxx,0x27, /* 80 */
	xxxx,0x01,xxxx,xxxx,0xe6,0x8a,0xd3,xxxx, xxxx,0x8e,0x56,0xa5,0x92,xxxx,xxxx,0xf9, /* 90 */
	0x22,xxxx,0x5f,xxxx,xxxx,0xa1,xxxx,0x74, 0xb8,xxxx,0x46,0x05,0xeb,0xcf,0xbf,0x5d, /* a0 */
	0x24,xxxx,0x9d,xxxx,xxxx,xxxx,xxxx,xxxx, 0x59,0x8d,0x3c,0xf8,0xc5,xxxx,0xf3,0x4e, /* b0 */
	xxxx,xxxx,0x50,0xc6,0xe9,0xfe,0x0a,xxxx, 0x99,0x86,xxxx,xxxx,0xaf ,0x8c/*0x8e*/,0x42,0xf7, /* c0 */
	xxxx,0x41,xxxx,0xa3,xxxx,0x3a,0x2a,0x43, xxxx,0xb3,0xe8,xxxx,0xc4,0x35,0x78,0x25, /* d0 */
	0x75,xxxx,0xb7,xxxx,0x23,xxxx, xxxx/*0xe2*/,0x8f, xxxx,xxxx,0x2c,xxxx,0x77,0x7e,xxxx,0x0f, /* e0 */
	0x0c,0xa0,0xbd,xxxx,xxxx,0x2d,0x29,0xea, xxxx,0x3b,0x73,xxxx,0xfb,0x20,xxxx,0x5a /* f0 */
};
//double check 0x00/0xa0 AND.
//double check 0x8c (0x7d jg)
//double check 0xfd (0x20 AND) - 9d2 in code
//double check 0xd1 (0x41 INC cw) used in uccops and dynablaster (LOOKS GOOD)

//AND fd (0x20)
//0x37 (91) guess from dynablaster title screen

// BM - 0x61 NOT 82, but instead 0x80 verified in both Atomic Punk and UCCops
// 0x22 is 0x18 (SBB) verified from Gunforce
// 0x5b seems confirmed (previous commented out as 0x36)
// NS I expected 0x32 to be 0x1b (SBB) like in gunforce, but startup tests fail in bbmanw.
//    therefore it seems to be 0x2b (SUB)
// NS010718  0xa0 was 0x00 (ADDB), verified to be 0x22 (ANDB)


const UINT8 mysticri_decryption_table[256] = {
	xxxx,0x57,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, 0xbf,0x43,xxxx,xxxx,xxxx,xxxx,0xfc,xxxx, /* 00 */
	xxxx,xxxx,xxxx,xxxx,xxxx,0x52,0xa3,0x26, xxxx,0xc7,xxxx,0x0f,xxxx,0x0c,xxxx,xxxx, /* 10 */
	xxxx,xxxx,0xff,xxxx,xxxx,0x02,xxxx,xxxx, 0x2e,xxxx,0x5f,xxxx,xxxx,xxxx,0x73,0x50, /* 20 */
	0xb2,0x3a,xxxx,xxxx,0xbb,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 30 */
	xxxx,xxxx,0x8e,0x3c,0x42,xxxx,xxxx,0xb9, xxxx,xxxx,0x2a,xxxx,0x47,0xa0,0x2b,0x03, /* 40 */
	0xb5,0x1f,xxxx,0xaa,xxxx,0xfb,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,0x38,xxxx,xxxx,xxxx, /* 50 */
	0x2c,xxxx,xxxx,0xc6,xxxx,xxxx,0xb1,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0xa2,xxxx, /* 60 */
	0xe9,0xe8,xxxx,xxxx,0x86,xxxx,0x8b,xxxx, xxxx,xxxx,xxxx,xxxx,0x5b,0x72,xxxx,xxxx, /* 70 */
	xxxx,xxxx,0x5d,0x0a,xxxx,xxxx,0x89,xxxx, 0xb0,0x88,xxxx,xxxx,xxxx,0x87,0x75,0xbd, /* 80 */
	xxxx,0x51,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,0x5a,0x58,xxxx,xxxx,0x56, /* 90 */
	xxxx,0x8a,xxxx,0x55,xxxx,xxxx,xxxx,0xb4, 0x08,xxxx,0xf6,xxxx,xxxx,0x9d,xxxx,0xbc, /* A0 */
	0x0b,xxxx,xxxx,0x5e,xxxx,xxxx,xxxx,0x22, 0x36,xxxx,0x1e,xxxx,0xb6,0xba,0x23,xxxx, /* B0 */
	0x20,xxxx,xxxx,xxxx,0x59,0x53,xxxx,0x04, 0x81,xxxx,xxxx,0xf3,xxxx,xxxx,0x3b,0x06, /* C0 */
	xxxx,0x79,0x83,0x9c,xxxx,0x18,0x80,xxxx, 0xc3,xxxx,xxxx,xxxx,0x32,xxxx,0xcf,xxxx, /* D0 */
	0xeb,xxxx,xxxx,0x33,xxxx,0xfa,xxxx,xxxx, 0xd2,xxxx,0x24,xxxx,0x74,0x41,0xb8,xxxx, /* E0 */
	xxxx,xxxx,0xd0,0x07,xxxx,xxxx,xxxx,xxxx, xxxx,0x46,xxxx,0xea,0xfe,0x78,xxxx,xxxx, /* F0 */
};
// 0xd5 (0x18) opcode is right but arguments could be swapped
// 0x4e (0x2b) not sure, could be 0x1b


const UINT8 majtitl2_decryption_table[256] = {
	0x87,xxxx,0x78,0xaa,xxxx,xxxx,xxxx,0x2c, 0x32,0x0a,0x0f,xxxx,0x5e,xxxx,0xc6,0x8a, /* 00 */
	0x33,xxxx,xxxx,xxxx,xxxx,0xea,xxxx,0x72, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0x24,0x55, /* 10 */
	xxxx,xxxx,xxxx,0x89,0xfb,xxxx,0x59,0x02, xxxx,xxxx,0x5d,xxxx,xxxx,xxxx,0x36,xxxx, /* 20 */
	xxxx,0x06,0x79,xxxx,xxxx,0x1e,0x07,xxxx, xxxx,xxxx,0x83,xxxx,xxxx,xxxx,xxxx,xxxx, /* 30 */
	0x9d,xxxx,xxxx,0x74,xxxx,xxxx,xxxx,0x0c, 0x58,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 40 */
	0x3c,xxxx,0x03,xxxx,xxxx,0xfa,0x43,xxxx, 0xbf,xxxx,xxxx,0x75,xxxx,0x88,xxxx,0x80, /* 50 */
	xxxx,0xa3,xxxx,0xfe,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,0x3a,xxxx,xxxx,xxxx, /* 60 */
	0x2b,xxxx,xxxx,xxxx,xxxx,0xe9,0x5f,xxxx, 0x46,xxxx,0x41,xxxx,0x18,0xb8,xxxx,xxxx, /* 70 */
	0xb4,0x5a,0xb1,xxxx,xxxx,0x50,0xe8,0x20, xxxx,0xb2,xxxx,xxxx,xxxx,xxxx,xxxx,0x51, /* 80 */
	xxxx,xxxx,xxxx,0x56,xxxx,xxxx,xxxx,xxxx, xxxx,0xcf,xxxx,xxxx,xxxx,0xc3,xxxx,xxxx, /* 90 */
	xxxx,xxxx,xxxx,xxxx,0x0b,xxxx,xxxx,0xb5, 0x57,xxxx,xxxx,0xc7,0x3b,xxxx,xxxx,xxxx, /* A0 */
	xxxx,xxxx,xxxx,xxxx,0xb6,xxxx,0xeb,xxxx, 0x38,xxxx,0xa0,0x08,xxxx,0x86,0xb0,xxxx, /* B0 */
	0x42,0x1f,0x73,xxxx,0xf6,xxxx,xxxx,xxxx, 0x53,xxxx,0x52,xxxx,0x04,0xbd,xxxx,xxxx, /* C0 */
	0x26,0xff,0x2e,xxxx,0x81,xxxx,0x47,xxxx, xxxx,xxxx,xxxx,0xd0,0x22,xxxx,xxxx,0xb9, /* D0 */
	0x23,xxxx,0xf3,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,0xd2,0x8b,0xba,xxxx,xxxx,xxxx,0x5b, /* E0 */
	xxxx,xxxx,0x9c,xxxx,xxxx,xxxx,xxxx,0xfc, 0xbc,0xa2,0x2a,xxxx,xxxx,0x8e,0xbb,xxxx, /* F0 */
};
// 0x7c (0x18) opcode is right but arguments could be swapped
// 0x70 (0x2b) not sure, could be 0x1b


const UINT8 hook_decryption_table[256] = {
	0xb6,0x20,0x22,xxxx,0x0f,0x57,0x59,0xc6, 0xeb,xxxx,0xb0,0xbb,0x3b,xxxx,xxxx,xxxx, /* 00 */
	0x36,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,0xfe,xxxx,xxxx,xxxx,xxxx,xxxx,0xa0, /* 10 */
	0x2e,xxxx,0x0b,xxxx,xxxx,0x58,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,0x80,xxxx,xxxx, /* 20 */
	0x33,xxxx,xxxx,0xbf,0x55,xxxx,xxxx,xxxx, 0x53,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 30 */
	0x47,0x74,xxxx,0xb1,0xb4,xxxx,xxxx,0x88, xxxx,xxxx,0x38,0xcf,xxxx,0x8e,xxxx,xxxx, /* 40 */
	xxxx,0xc7,xxxx,0x32,xxxx,0x52,0x3c,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0x83,0x72, /* 50 */
	xxxx,0x73,xxxx,0x5a,xxxx,0x43,xxxx,xxxx, xxxx,xxxx,0x41,0xe9,0xbd,xxxx,0xb2,0xd2, /* 60 */
	xxxx,0xaa,0xa2,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,0x26,xxxx,xxxx,0x8a,xxxx, /* 70 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0x18, xxxx,0x9d,xxxx,xxxx,xxxx,0x5d,xxxx,0x46, /* 80 */
	xxxx,xxxx,xxxx,0xf6,0xc3,0xa3,0x1e,0x07, 0x5f,0x81,xxxx,0x0c,xxxx,0xb8,xxxx,0x75, /* 90 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0x79, /* A0 */
	xxxx,0x5e,xxxx,xxxx,0x06,xxxx,0xff,xxxx, 0x5b,0x24,xxxx,0x2b,xxxx,xxxx,xxxx,0x02, /* B0 */
	0x86,xxxx,xxxx,0xfb,xxxx,xxxx,0x50,0xfc, 0x08,xxxx,xxxx,xxxx,0x03,xxxx,0xb9,xxxx, /* C0 */
	xxxx,0xbc,0xe8,0x1f,0xfa,0x42,xxxx,xxxx, 0x89,xxxx,0x23,0x87,xxxx,0x2a,xxxx,xxxx, /* D0 */
	0x8b,xxxx,0xf3,0xea,0x04,0x2c,0xb5,xxxx, 0x0a,xxxx,0x51,xxxx,xxxx,0x3a,xxxx,0x9c, /* E0 */
	xxxx,xxxx,0x78,xxxx,0xba,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,0xd0,0x56,xxxx,xxxx, /* F0 */
};
// 0x87 (0x18) opcode is right but arguments could be swapped
// 0xbb (0x2b) not sure, could be 0x1b


const UINT8 rtypeleo_decryption_table[256] = {
	0x5d,xxxx,0xc6,xxxx,xxxx,xxxx,0x2a,0x3a, xxxx,xxxx,xxxx,0x86,xxxx,0x22,xxxx,0xf3, /* 00 */
	xxxx,xxxx,xxxx,xxxx,xxxx,0x38,0x01,0x42, 0x04,xxxx,xxxx,0x1f,xxxx,xxxx,xxxx,0x58, /* 10 */
//                                ^^^^
	0x57,0x2e,xxxx,xxxx,0x53,xxxx,0xb9,xxxx, xxxx,xxxx,xxxx,xxxx,0x20,0x55,xxxx,0x3d, /* 20 */
//                                                                              ^^^^
	0xa0,xxxx,xxxx,0x0c,0x03,xxxx,0x83,xxxx, xxxx,xxxx,0x8a,xxxx,xxxx,0xaa,xxxx,xxxx, /* 30 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,0x41,0x0a,0x26,0x8b,0x56,0x5e,xxxx, /* 40 */
	xxxx,0x74,xxxx,xxxx,xxxx,xxxx,0x06,xxxx, xxxx,0x89,0x5b,0xc7,0x43,xxxx,xxxx,xxxx, /* 50 */
	xxxx,0xb6,xxxx,0x3b,xxxx,xxxx,xxxx,xxxx, xxxx,0x36,0xea,0x80,xxxx,xxxx,xxxx,0x5f, /* 60 */
	xxxx,0x0f,xxxx,xxxx,xxxx,0x46,xxxx,xxxx, 0x3c,0x8e,xxxx,0xa3,0x87,xxxx,xxxx,xxxx, /* 70 */
	0x2b,0xfb,0x47,0x0b,xxxx,0xfc,0x02,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0x72,0x2c, /* 80 */
	0x33,xxxx,xxxx,xxxx,xxxx,xxxx,0x9d,0xbd, xxxx,0xb2,xxxx,0x78,0x75,0xb8,xxxx,xxxx, /* 90 */
	xxxx,xxxx,xxxx,xxxx,0xcf,0x5a,0x88,xxxx, xxxx,xxxx,0xc3,xxxx,0xeb,0xfa,xxxx,0x32, /* A0 */
	xxxx,xxxx,xxxx,0x52,0xb4,xxxx,xxxx,xxxx, xxxx,0xbc,xxxx,xxxx,xxxx,0xb1,0x59,0x50, /* B0 */
	xxxx,xxxx,0xb5,xxxx,0x08,0xa2,0xbf,0xbb, 0x1e,0x9c,xxxx,0x73,xxxx,0xd0,xxxx,xxxx, /* C0 */
	xxxx,xxxx,xxxx,xxxx,0x81,xxxx,0x79,xxxx, xxxx,0x24,0x23,xxxx,xxxx,0xb0,0x07,0xff, /* D0 */
	xxxx,0xba,0xf6,0x51,xxxx,xxxx,xxxx,0xfe, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0xe9,xxxx, /* E0 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0xe8,0xd2, xxxx,0x18,xxxx,xxxx,xxxx,0xd1,xxxx,xxxx, /* F0 */
//                                                                    ^^^^
};
// 0xf9 (0x18) opcode is right but arguments could be swapped
// 0x80 (0x2b) not sure, could be 0x1b
// 0x16 (0x01) guess (wrong?)


const UINT8 inthunt_decryption_table[256] = {
	0x1f,xxxx,0xbb,0x50,xxxx,0x58,0x42,0x57, xxxx,xxxx,0xe9,xxxx,xxxx,xxxx,xxxx,0x0b, /* 00 */
	xxxx,xxxx,0x9d,0x9c,xxxx,xxxx,0x1e,xxxx, xxxx,0xb4,0x5b,xxxx,xxxx,xxxx,xxxx,xxxx, /* 10 */
	xxxx,xxxx,0x78,0xc7,xxxx,xxxx,0x83,xxxx, xxxx,0x0c,0xb0,0x04,xxxx,xxxx,xxxx,xxxx, /* 20 */
	xxxx,xxxx,xxxx,xxxx,0x3b,0xc3,0xb5,0x47, xxxx,xxxx,xxxx,xxxx,0x59,xxxx,xxxx,xxxx, /* 30 */
	xxxx,xxxx,xxxx,0x38,xxxx,xxxx,xxxx,xxxx, 0x5f,0xa3,0xfa,xxxx,0xe8,0x36,0x75,xxxx, /* 40 */
	0x88,0x33,xxxx,xxxx,xxxx,xxxx,0x43,xxxx, xxxx,0x87,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 50 */
	xxxx,xxxx,xxxx,xxxx,0x8e,0xf3,0x56,xxxx, xxxx,xxxx,xxxx,0x26,0xff,xxxx,xxxx,xxxx, /* 60 */
	xxxx,xxxx,xxxx,0x2a,xxxx,0x8a,xxxx,0x18, xxxx,xxxx,0x03,0x89,0x24,xxxx,xxxx,xxxx, /* 70 */
	0x0a,xxxx,0xeb,xxxx,0x86,xxxx,xxxx,xxxx, 0x79,0x3a,xxxx,xxxx,xxxx,xxxx,0xa0,xxxx, /* 80 */
	0xea,xxxx,xxxx,xxxx,xxxx,xxxx,0x2c,xxxx, 0xc6,xxxx,xxxx,0x46,xxxx,0xaa,0xb6,0x5e, /* 90 */
	xxxx,xxxx,xxxx,xxxx,0x8b,xxxx,xxxx,xxxx, xxxx,xxxx,0xba,xxxx,0xb9,0x53,0xa2,xxxx, /* A0 */
	xxxx,0x07,xxxx,xxxx,xxxx,0x3c,0x32,xxxx, 0x2b,xxxx,0xb8,xxxx,xxxx,xxxx,xxxx,xxxx, /* B0 */
	0xbd,xxxx,xxxx,xxxx,xxxx,0x81,xxxx,0xd0, 0x08,xxxx,0x55,0x06,0xcf,xxxx,xxxx,0xfc, /* C0 */
	xxxx,xxxx,xxxx,0xb1,0xbf,xxxx,xxxx,0x51, 0x52,xxxx,0x5d,xxxx,0x5a,xxxx,0xb2,xxxx, /* D0 */
	0xfe,xxxx,xxxx,0x22,0x20,0x72,0xf6,0x80, 0x02,0x2e,xxxx,0x74,0x0f,xxxx,xxxx,xxxx, /* E0 */
	xxxx,xxxx,xxxx,xxxx,0xbc,0x41,xxxx,0xfb, 0x73,xxxx,xxxx,xxxx,0x23,0xd2,xxxx,xxxx, /* F0 */
};
// 0x77 (0x18) opcode is right but arguments could be swapped
// 0xb8 (0x2b) not sure, could be 0x1b


const UINT8 gussun_decryption_table[256] = {
	0xcd,xxxx,xxxx,0x36,xxxx,0x52,0xb1,0x5b, 0x68,0xcd,xxxx,xxxx,xxxx,0xa8,xxxx,xxxx, /* 00 */
//      !!!!                                     ^^^^ !!!!                !!!!
	xxxx,xxxx,0x75,0x24,0x01,0x83,0x32,0xe9, xxxx,0x78,xxxx,0x19,0x22,xxxx,0xac,xxxx, /* 10 */
//                     !!!! ????      ^^^^            ????      ???? !!!!      ^^^^
	0x5d,0xa5,0x10,0x51,0x0a,xxxx,0x19,xxxx ,0xf8,0x90,0x91,0x40,0x28,0x22,0x03,0x5f, /* 20 */
//                ????      !!!!      ????       ^^^^ ???? ^^^^ gggg gggg ????
	0x26,xxxx,xxxx,0x8b,xxxx,0x02,xxxx,xxxx, 0x8e,0xab,xxxx,xxxx,0xbc,0xf1,0xb3,xxxx, /* 30 */
//
	xxxx,0x19,0xc6,xxxx,xxxx,0x3a,0x10,xxxx, xxxx,0x74,0x61,xxxx,0x33,xxxx,0x19,xxxx, /* 40 */
//           ????                     ????                 !!!!                ????
	xxxx,0x53,0xa0,0xc0,0xc3,0x41,0xfc,0xe7, xxxx,0x2c,0x7f,0x2b,xxxx,xxxx,0xba,0x2a, /* 50 */
//                !!!! ^^^^      ^^^^                 gggg ???? !!!!                !!!!
	0xb0,xxxx,0x28,0x7e,xxxx,xxxx,0xb5,0x07, 0xb9,xxxx,0x27,0x46,0xf9,xxxx,xxxx,xxxx, /* 60 */
//                ???? gggg                                !!!! ^^^^
	xxxx,0xea,0x72,0x73,0xad,0xd1,0x3b,0x5e, 0xe5,0x57,xxxx,0x0d,xxxx,xxxx,xxxx,0x3c, /* 70 */
//                          ^^^^
	xxxx,0x86,xxxx,0xb7,0x30,0x25,0x2d,xxxx, 0x9a,0xeb,0x04,0x0b,0xa2,0xb8,0xf6,xxxx, /* 80 */
//                     ???? ????                           ^^^^      !!!!      !!!!
	xxxx,xxxx,0x9d,xxxx,0xbb,xxxx,xxxx,0xcb, 0xa9,0xcf,xxxx,0x60,0x43,0x56,xxxx,0x19, /* 90 */
//                ^^^^                     ^^^^  !!!! ^^^^      !!!!                ????
	xxxx,0xa3,xxxx,xxxx,xxxx,xxxx,0xfa,0xb4, xxxx,0x81,0xe6,0x42,0x80,0x8c,0xd4,xxxx, /* a0 */
//                                         gggg                 ????      gggg !!!!
	xxxx,0x11,0x11,0x11,0x77,0x3d,0x3e,xxxx, xxxx,xxxx,0x4b,xxxx,xxxx,xxxx,xxxx,xxxx, /* b0 */
//           ???? ???? ???? gggg                           ????
	xxxx,0xff,0x47,xxxx,0x55,0x1e,xxxx,0x59, 0x93,xxxx,xxxx,xxxx,0x88,0xc1,0x11,0xb2, /* c0 */
//                ^^^^                           ^^^^                     ^^^^ ????
	xxxx,0x2e,0x06,0xc7,0x05,xxxx,0x8a,0x5a, 0x58,0xbe,xxxx,xxxx,xxxx,0x1f,0x23,xxxx, /* d0 */
//           ^^^^                                               ????
	0xe8,xxxx,0x89,0xa1,0xd0,xxxx,0x19,0xe2, 0x38,0xfe,0x50,0x9c,xxxx,xxxx,xxxx,0x49, /* e0 */
//                                    ????                      ^^^^                !!!!
	0xfb,xxxx,0xf3,xxxx,xxxx,0x0f,xxxx,xxxx, xxxx,0x70,0xf7,0xbd,0x39,0x7f,0xbf,xxxx, /* f0 */
//      !!!!                                          ????      !!!!      gggg
};


/*
missing opcode:

1**14 -> 09? (2097b - 20980 (01 09 11 31
+++19 -> 79? (2282f - routine from 2281f to 22871) (78, 79, 7a, 7b)
0++1b -> 19? (1d8f9 - routine from 1d8c7 to 1d8fc) - three bytes (09,11,21,29,31 - 01,19)
01+22 -> 00? (1deff - routine from 1dee8 to 1df26) no 08,18,20,30 - ok 00,10
0++26 -> 29? (14ed_ , 198e_
01*29 -> 48? (1df22 - routine from 1df1d to
01+2d -> 1a? (1df1d - routine from 1dee8 to 1df26) (00,01,10,11,12,13,20,21,28,29,30,31 - 08,09,18,19
---41 -> 19?
///46 -> 10? (1461b
---4e -> 19? (208ac -
1//5a -> 76? (222fc - routine from 222ed to ) (7x j...) 70 76 78 7a 7c
1//62 -> 28? (1cf86 1cfa3 - routine from 1cf61 to 1cff4)  (water in level 1)
1--83 -> b7? (194cd - routine from
0**84 -> 18? (1d8f1 - routine from 1d8c7 to 1d8fc) - three bytes (00,08,10,18,20,28,30)
---9f -> 19? (
///ab -> 42?
---b1 -> 19?
---b2 -> 19? (20a8c - 20acd
///b3 -> 19? (216b6 - 216cf
///ba -> 4b? (1094d, 10b28 - routine from 10948 to 10b73) one byte (4b
01+ce -> 11? (10236 - routine from 1017e to 10254) (01, 09, 11, 19, 21, 29, 31)
///db ->     (16992
///f9 -> 71? (16598 - 165a1 - 18de7 - routine from 18dc4 to ) 70,71


rz guess:
2b -> 40 (1d4d2 1db81 1dba9 - routine from 1d4b2 to 1d4de) (ok 40,48,96) -> 40
         (no 0e,16,17,27,2f,37,3f,41,42,44,45,4a,4b,4c,4d,4e,4f,54,5c,6c,6d,92,94,95,97)
2c -> 28 (20333 - routine from 2032a to 20366)
59 -> 2c (220cf - 2037f - routine from 2202f to ) 2bytes (2c,
63 -> 7d (1df7f, 1df8c, 1df95,21f08 - routine from 1df1d to ) - two bytes j..
a7 -> b4 (2029a - routine from 20290 to
ad -> 8c (1d559, 1d8d4 - routine from 1d547 - ; routine from 1d8b8 to 1d8fc)
b4 -> 77 (1d03a, 1d57a - routine from 1d4df ) 7f,7d,7b  -  79 - "77"
fd -> 7f (1d659 - routine from 1d63c to 1d65e) 77,7f

missing V35+ core:
00    -> 63    not supported (229e2 - routine from 229da to 229ec) (for now we use cd instruction + hack)
0f 92 -> of 92 not supported (1011d before of STI instruction) (for now no effects)

above - c8 (inc aw) guess from stos code
0xc5 -> 1e (push ds) guess (pop ds soon after) right?
0xa9 -> 81 (not 0x82 PRE) guess from 237df
0xcd -> c1 total guess (wrong but 3 bytes)

AS notes:
0x1e is lodsb not lds,noted from 2344f
0x16 is xor r8,r8 not xor r16,r16
0xc8 not inc aw but xch bw,aw
0xcd is 0xc1 not 0xbd(palette at startup)
0x97 guess,but seems right(228c1),known to *not* be ret %Iw.
0x00 wrong(for sure it needs a one byte operand due to push es called at one point...)
0x19 guess (0x82 PRE)
0xc2 guess,it could be dec iy...

RZ notes
0xef -> 49 (dec CW) (used in "Service Mode" / "CHARACTER menu")
0x24 -> (0a, 12) probably 0a (OR) (used in "Service Mode" / "CHARACTER menu")
0x5b -> 2b (used in "Service Mode" / "CHARACTER menu")
0x21 -> a5 (used in "Service Mode" / "CHARACTER menu")

checked against "hasamu" code: hasamu and gussun share code (i.e. you can compare gussun from 2002a and hasamu from 54a0)
0x09 -> cd (22a17 - routine from 229ed to 22a1a) - two bytes
0x13 -> 24 (interrupt routine)
0x1c -> 22 (interrupt routine)
0x34 -> 2f (20381 - routine from 2037b to 20391) - used to handle number of lives and game over
0x4a -> 61 (interrupt routine)
0x66 -> b5 (1daaf - routine from 1da61 to 1daca) - two bytes (colors effect)
0x6a -> 27 (20368 - routine from 20368 to 2037a) - used to limit the max lives number
0x73 -> 73 (1d4f7 - routine from 1d4df to 1d539) -> (no 70,71,72,74,75,76,77,78,79,7a,7b,7c,7d,7e,7f) - ok 73
0x8c -> a2 (interrupt routine)
0x8e -> f6 (interrupt routine)
0x9b -> 60 (interrupt routine)
0xf0 -> fb guess (interrupt)
0xae -> d4 (20215 - ) used when you insert a coin to handle the "coin number" in decimal
*/

/*
e0100 palette sub-routine:
12485: 23             push es
12486: 27 C2          mov  es,dw
12488: D7 C0          xor  aw,aw
1248A: D7 FF          xor  iy,iy
1248C: 44 00 01       mov  cw,$0100
1248F: 81 5D          repe stosw
12491: 95 22 06 97 39 mov  byte ss:[$3997],$FF
12497: 3A             pop  es
12498: 60             ret

d1a86

unk
add bw,bw [d8]
add ix,sp [e6]
add iy,sp [e7]
clc
ret

Unused: [2] 00  add %Eb,%Gb
Unused: [2] 01  add %Ev,%Gv
Unused: [2] 08  or %Eb,%Gb
Unused: [2] 09  or %Ev,%Gv
Unused: [2] 0a  or %Gb,%Eb
Unused: [2] 0c  or al,%Ib
Unused: [1] 0e  push cs
Unused: [2] 10  adc %Eb,%Gb
Unused: [2] 11  adc %Ev,%Gv
Unused: [2] 12  adc %Gb,%Eb
Unused: [2] 13  adc %Gv,%Ev
Unused: [2] 14  adc al,%Ib
Unused: [3] 15  adc aw,%Iv
Unused: [1] 16  push ss
Unused: [1] 17  pop ss
Unused: [2] 18  sbb %Eb,%Gb
Unused: [2] 19  sbb %Ev,%Gv
Unused: [2] 1a  sbb %Gb,%Eb
Unused: [2] 1b  sbb %Gv,%Ev
Unused: [2] 1c  sbb al,%Ib
Unused: [3] 1d  sbb aw,%Iv
Unused: [2] 20  and %Eb,%Gb
Unused: [2] 22  and %Gb,%Eb
Unused: [2] 24  and al,%Ib
Unused: [1] 27  adj4a
Unused: [2] 28  sub %Eb,%Gb
Unused: [2] 29  sub %Ev,%Gv
Unused: [2] 2a  sub %Gb,%Eb
Unused: [2] 2b  sub %Gv,%Ev
Unused: [2] 2c  sub al,%Ib
Unused: [1] 2f  adj4s
Unused: [2] 30  xor %Eb,%Gb
Unused: [2] 31  xor %Ev,%Gv
Unused: [2] 34  xor al,%Ib
Unused: [3] 35  xor aw,%Iv
Unused: [1] 37  adjba
Unused: [1] 3f  adjbs
Unused: [1] 40  inc aw
Unused: [1] 42  inc dw
Unused: [1] 44  inc sp
Unused: [1] 45  inc bp
Unused: [1] 47  inc iy
Unused: [1] 48  dec aw
Unused: [1] 49  dec cw
Unused: [1] 4a  dec dw
Unused: [1] 4b  dec bw
Unused: [1] 4c  dec sp
Unused: [1] 4d  dec bp
Unused: [1] 4e  dec ix
Unused: [1] 54  push sp
Unused: [1] 5c  pop sp
Unused: [1] 60  pusha
Unused: [1] 61  popa
Unused: [0] 62  chkind %Gv,%Ma
Unused: [0] 63  (null)
Unused: [0] 64  repnc %p
Unused: [0] 65  repc %p
Unused: [0] 66  (null)
Unused: [0] 67  (null)
Unused: [0] 69  imul %Gw,%Ew,%Iw
Unused: [0] 6a  push %Ix
Unused: [0] 6b  imul %Gw,%Ew,%Ib
Unused: [0] 6c  insb
Unused: [0] 6d  insw
Unused: [0] 6e  outsb
Unused: [0] 6f  outsw
Unused: [1] 71  jno %Jb
Unused: [1] 76  jbe %Jb
Unused: [1] 77  ja %Jb
Unused: [1] 78  js %Jb
Unused: [1] 79  jns %Jb
Unused: [1] 7a  jpe %Jb
Unused: [1] 7b  jpo %Jb
Unused: [1] 7c  jl %Jb
Unused: [1] 7e  jle %Jb
Unused: [1] 7f  jg %Jb
DUPLICATE: 81   %g0 %Ew,%Iw
Unused: [2] 84  test %Eb,%Gb
Unused: [2] 85  test %Ew,%Gw
Unused: [2] 87  xch %Ew,%Gw
Unused: [0] 8c  mov %Ew,%Sw
Unused: [0] 8d  ldea %Gw,%M
Unused: [0] 8f  pop %Ev
Unused: [1] 92  xch dw,aw
Unused: [1] 94  xch sp,aw
Unused: [1] 95  xch bp,aw
Unused: [1] 96  xch ix,aw
Unused: [1] 97  xch iy,aw
Unused: [0] 98  cvtbw
Unused: [0] 99  cvtwl
Unused: [0] 9b  fwait
Unused: [0] 9e  sahf
Unused: [0] 9f  lahf
Unused: [0] a0  mov al,%Oc
Unused: [0] a2  mov %Oc,al
Unused: [0] a5  %P movsw
Unused: [0] a6  %P cmpsb
Unused: [0] a7  %P cmpsw
Unused: [0] a9  test aw,%Iv
Unused: [0] aa  %P stosb
Unused: [0] ae  %P scasb
Unused: [0] af  %P scasw
Unused: [2] b4  mov ah,%Ib
Unused: [2] b5  mov ch,%Ib
Unused: [2] b6  mov dh,%Ib
Unused: [2] b7  mov bh,%Ib
Unused: [3] bd  mov bp,%Iv
Unused: [0] c2  ret %Iw
Unused: [0] c4  les %Gv,%Mp
Unused: [0] c5  lds %Gv,%Mp
Unused: [0] c8  enter %Iw,%Ib
Unused: [0] c9  leave
Unused: [0] ca  retf %Iw
Unused: [0] cc  int 03
Unused: [0] cd  int %Ib
Unused: [0] ce  into
Unused: [0] d2  %g1 %Eb,cl
Unused: [0] d3  %g1 %Ev,cl
Unused: [0] d4  aam ; %Ib
Unused: [0] d5  aad ; %Ib
Unused: [0] d6  (null)
Unused: [0] d7  trans
Unused: [0] d8  %f0
Unused: [0] d9  %f1
Unused: [0] da  %f2
Unused: [0] db  %f3
Unused: [0] dc  %f4
Unused: [0] dd  %f5
Unused: [0] de  %f6
Unused: [0] df  %f7
Unused: [0] e0  loopne %Jb
Unused: [0] e1  loope %Jb
Unused: [0] e3  j%j cxz %Jb
Unused: [0] e4  in al,%Ib
Unused: [0] ec  in al,dw
Unused: [0] ed  in aw,dw
Unused: [0] ee  out dw,al
Unused: [0] ef  out dx,aw
Unused: [0] f0  lock %p
DUPLICATE: f1   (null)
Unused: [0] f2  repne %p
Unused: [0] f4  hlt
Unused: [0] f5  not1 CY(cmc)
Unused: [0] f6  %g2
Unused: [1] fb  ei
Unused: [1] fd  std
*/

const UINT8 leagueman_decryption_table[256] = {
	xxxx,xxxx,xxxx,0x55,0xbb,xxxx,0x23,0x79, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0x38,xxxx, /* 00 */
	0x01,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, 0x3d,xxxx,xxxx,xxxx,0xba,xxxx,0x1e,xxxx, /* 10 */
	0x2c,0x46,xxxx,0xb5,xxxx,0x4b,xxxx,0xfe, xxxx,xxxx,0xfb,0x2e,xxxx,xxxx,0x36,0x04, /* 20 */
	0xcf,xxxx,0xf3,0x5a,0x8a,0x0c,0x9c,xxxx, xxxx,xxxx,0xb2,0x50,xxxx,xxxx,xxxx,0x5f, /* 30 */
	xxxx,xxxx,0x24,xxxx,xxxx,0x41,0x2b,xxxx, 0xe9,xxxx,0x08,0x3b,xxxx,xxxx,xxxx,xxxx, /* 40 */
	xxxx,0xd2,0x51,xxxx,xxxx,xxxx,0x22,xxxx, 0xeb,0x3a,0x5b,0xa2,0xb1,0x80,xxxx,xxxx, /* 50 */
	xxxx,xxxx,xxxx,xxxx,0x59,0xb4,0x88,xxxx, xxxx,0xbf,0xd1,xxxx,0xb9,0x57,xxxx,xxxx, /* 60 */
	0x72,xxxx,0x73,xxxx,xxxx,xxxx,xxxx,0x0f, xxxx,xxxx,xxxx,xxxx,0x56,xxxx,xxxx,0xc6, /* 70 */
	xxxx,xxxx,xxxx,xxxx,xxxx,0x2a,0x8e,xxxx, 0x81,0xa3,0x58,xxxx,0xaa,0x78,0x89,xxxx, /* 80 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0xbd,xxxx, xxxx,xxxx,0xff,xxxx,xxxx,xxxx,0x07,0x53, /* 90 */
	0xa0,xxxx,xxxx,0x5e,0xb0,xxxx,0x83,0xf6, xxxx,0x26,0x32,xxxx,xxxx,xxxx,0x74,0x0a, /* A0 */
	0x18,xxxx,xxxx,xxxx,0x75,0x03,xxxx,xxxx, 0xb6,0x02,xxxx,xxxx,0x43,xxxx,0xb8,xxxx, /* B0 */
	0xe8,xxxx,0xfc,xxxx,0x20,0xc3,xxxx,0x06, xxxx,0x1f,0x86,0x00,xxxx,xxxx,xxxx,0xd0, /* C0 */
	0x47,xxxx,0x87,xxxx,xxxx,0x9d,0x3c,0xc7, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* D0 */
	xxxx,xxxx,xxxx,0x8b,xxxx,xxxx,0x33,xxxx, xxxx,xxxx,xxxx,xxxx,0xfa,0x42,xxxx,xxxx, /* E0 */
	xxxx,xxxx,xxxx,0xea,xxxx,0x52,xxxx,0x5d, xxxx,xxxx,xxxx,xxxx,0xbc,xxxx,xxxx,xxxx, /* F0 */
};
// 0x25 (0x4b) guess
// 0x10 (0x01) guess (wrong?)


const UINT8 psoldier_decryption_table[256] = {
	xxxx,xxxx,xxxx,0x8a,xxxx,0xaa,xxxx,xxxx, xxxx,0x20,0x23,0x55,xxxx,0xb5,0x0a,xxxx, /* 00 */
	xxxx,0x46,xxxx,0xb6,xxxx,0x74,0x8b,xxxx, xxxx,0xba,0x01,xxxx,xxxx,0x5a,0x86,0xfb, /* 10 */
	0xb2,xxxx,0xb0,xxxx,0x42,0x06,0x1e,0x08, 0x22,0x9d,xxxx,xxxx,xxxx,xxxx,xxxx,0x73, /* 20 */
	xxxx,xxxx,0x5f,xxxx,xxxx,0xd0,xxxx,0xff, xxxx,xxxx,0xbd,xxxx,0x03,xxxx,0xb9,xxxx, /* 30 */
	xxxx,xxxx,xxxx,0x51,0x5e,0x24,xxxx,xxxx, xxxx,xxxx,xxxx,0x58,0x59,xxxx,xxxx,xxxx, /* 40 */
	0x52,xxxx,xxxx,xxxx,0xa0,xxxx,xxxx,0x02, 0xd2,xxxx,0x79,0x26,0x3a,0x0f,0xcf,0xb4, /* 50 */
	0xf3,xxxx,xxxx,0x50,xxxx,0x75,0xb1,xxxx, 0xd1,0x47,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 60 */
	0xc6,xxxx,xxxx,xxxx,xxxx,xxxx,0xbc,xxxx, xxxx,xxxx,xxxx,xxxx,0x53,0x41,xxxx,xxxx, /* 70 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,0x04,xxxx, xxxx,xxxx,xxxx,0x2c,xxxx,0xbf,xxxx,xxxx, /* 80 */
	xxxx,xxxx,0xe8,xxxx,xxxx,0x78,xxxx,0xbb, xxxx,xxxx,0x1f,0x2b,0x87,xxxx,0x4b,0x56, /* 90 */
	0x36,0x33,xxxx,xxxx,xxxx,0x9c,0xc3,xxxx, xxxx,0x81,xxxx,0xe9,xxxx,0xfa,xxxx,xxxx, /* A0 */
	xxxx,0x72,xxxx,0xa2,xxxx,xxxx,0xc7,xxxx, xxxx,0x92,xxxx,xxxx,0x88,xxxx,xxxx,xxxx, /* B0 */
	0x3b,xxxx,0x0c,xxxx,0x80,xxxx,xxxx,xxxx, xxxx,0x2e,xxxx,xxxx,xxxx,0x57,xxxx,0x8e, /* C0 */
	0x07,xxxx,0xa3,xxxx,xxxx,xxxx,0x3d,xxxx, 0xfe,xxxx,xxxx,0xfc,0xea,xxxx,0x38,xxxx, /* D0 */
	0x3c,0xf6,xxxx,xxxx,xxxx,0x18,xxxx,xxxx, 0xb8,xxxx,xxxx,xxxx,0x2a,0x5d,0x5b,xxxx, /* E0 */
	xxxx,0x43,0x32,xxxx,xxxx,xxxx,0xeb,xxxx, xxxx,xxxx,xxxx,xxxx,0x83,0x89,xxxx,xxxx, /* F0 */
};
// 0x9e (0x4b) guess
// 0x1a (0x01) guess (wrong?)


const UINT8 dsoccr94_decryption_table[256] = {
	xxxx,0xd1,xxxx,xxxx,xxxx,0x79,0x2e,xxxx, xxxx,xxxx,0x5a,0x0f,xxxx,xxxx,0x43,xxxx, /* 00 */
	xxxx,xxxx,0xe8,0x50,xxxx,xxxx,xxxx,0xa0, 0x5d,0x22,xxxx,xxxx,0xb2,0x3a,xxxx,xxxx, /* 10 */
	0xf6,0x8a,0x41,xxxx,xxxx,0x81,xxxx,xxxx, xxxx,xxxx,0x2b,0x58,xxxx,xxxx,xxxx,0xc6, /* 20 */
	xxxx,xxxx,0xb9,xxxx,xxxx,0x2a,xxxx,0x3c, xxxx,0x80,0x26,xxxx,0xb0,xxxx,0x47,xxxx, /* 30 */
	xxxx,xxxx,0x0a,0x55,xxxx,xxxx,xxxx,0x88, xxxx,xxxx,0x87,xxxx,xxxx,0xb4,0x0c,xxxx, /* 40 */
	0x73,0x53,xxxx,xxxx,0x3b,0x1f,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 50 */
	0x01,xxxx,xxxx,xxxx,xxxx,0x1e,xxxx,xxxx, 0xc3,xxxx,0xa3,0x74,xxxx,0x32,0x42,0x75, /* 60 */
	0xfc,xxxx,0xb8,xxxx,0x33,xxxx,0x5e,xxxx, xxxx,0xaa,xxxx,xxxx,0x04,xxxx,0x9c,0xba, /* 70 */
	xxxx,xxxx,0x24,0x89,xxxx,xxxx,0xea,xxxx, 0x23,xxxx,xxxx,xxxx,0xbb,xxxx,xxxx,0xc7, /* 80 */
	xxxx,0x8e,xxxx,0x52,xxxx,0x18,xxxx,0x72, xxxx,xxxx,xxxx,xxxx,0xb6,xxxx,xxxx,xxxx, /* 90 */
	0xfa,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, 0xb1,xxxx,xxxx,xxxx,xxxx,0x57,0x78,0xa2, /* A0 */
	xxxx,0x3d,0x51,xxxx,xxxx,0xbf,0x46,0x2c, xxxx,xxxx,0xfb,xxxx,xxxx,xxxx,xxxx,0x38, /* B0 */
	0x56,xxxx,0xcf,xxxx,0x08,xxxx,xxxx,xxxx, 0x5b,0x07,xxxx,xxxx,xxxx,0x20,0x9d,xxxx, /* C0 */
	0x03,xxxx,xxxx,xxxx,xxxx,xxxx,0xbc,0x86, 0x59,xxxx,0x02,xxxx,0xff,0xd2,0x8b,xxxx, /* D0 */
	xxxx,0xd0,xxxx,xxxx,xxxx,xxxx,xxxx,0xe9, 0x06,xxxx,0x5f,0xf3,xxxx,0xb5,xxxx,xxxx, /* E0 */
	0xeb,xxxx,xxxx,xxxx,xxxx,xxxx,0x83,xxxx, 0x36,xxxx,xxxx,xxxx,xxxx,0xbd,0xfe,xxxx, /* F0 */
};

// 0x95 (0x18) opcode is right but arguments could be swapped
// 0x2a (0x2b) not sure, could be 0x1b
// 0x60 (0x01) guess (wrong?)


/* preliminary table by Pierpaolo Prazzoli */
const UINT8 matchit2_decryption_table[256] = {
	xxxx,0x86,0x0a,xxxx,0x32,0x01,0x81,0xbe, 0xea,xxxx,0xbb,xxxx,xxxx,xxxx,0xa5,0xf6, /* 00 */
//  new  new       new  new  new  new                  new                 new  new
	0x5d,0x8c,0xf3,0xc4,0x42,0x5a,0x22,0x26, xxxx,0x58,xxxx,0xfd,0x59,0x53,0x80,0x09, /* 10 */
//  new  new  new  new  !!!! new  !!!!            new       !!!! new  new  new  !!!!
	xxxx,0x1e,0x48,0xe2,0x50,xxxx,0xc3,0x23, xxxx,xxxx,0xe9,xxxx,0x40,0x83,0xa3,0x46, /* 20 */
//       new  new  new  new       new  new             new       new  new  new
	0x49,0xb4,0xa9,xxxx,0xd3,0x8b,0xe8,0xb8, 0xa0,xxxx,xxxx,xxxx,0x84,xxxx,xxxx,xxxx, /* 30 */
//  !!!! ???? new       !!!! new  new        new                 new
	xxxx,xxxx,xxxx,xxxx,0x14,xxxx,0x25,xxxx, xxxx,0x5e,xxxx,0x87,0x56,0xb9,xxxx,0x39, /* 40 */
//                      new       new             new       new  new  new  ???? new
	0x89,xxxx,xxxx,xxxx,xxxx,0x1f,0xa4,xxxx, 0xf8,0x5f,0x21,0xb3,0x5b,xxxx,0x8d,xxxx, /* 50 */
//  new                      new  !!!!       new  new  !!!! new            new
	xxxx,0xc5,0x7c,0x07,xxxx,0x88,0xba,0x47, 0x35,0xfb,xxxx,0x7f,xxxx,xxxx,0xc6,0xeb, /* 60 */
//       new  !!!! new       new  new  new   new  new       !!!!                new
	xxxx,0xc7,xxxx,xxxx,0xd2,0xa1,0x72,0x79, 0xfe,0x24,0xab,0x2a,0xbc,0x0d,0x8f,0x7e, /* 70 */
//                      !!!! new  new  new   new  new  new  new       new  new  !!!!
	xxxx,0x7d,xxxx,0xe7,0x2d,xxxx,xxxx,0x57, 0x0b,0xa2,xxxx,0x9d,xxxx,xxxx,0x74,0x85, /* 80 */
//       !!!!      new  new            new   new  new       new            new  new
	0xaf,0x2f,0x8a,0xe6,0x08,xxxx,0xff,xxxx, xxxx,0x1c,xxxx,xxxx,0x02,xxxx,xxxx,xxxx, /* 90 */
//  new  !!!! new  new  new       new             !!!!           new
	0x43,0x04,xxxx,xxxx,0xbf,0x3b,0x93,0x38, xxxx,xxxx,xxxx,0x77,xxxx,0xb0,xxxx,0x3a, /* A0 */
//  new  new            new  new  new  new                  new       new       new
	0xfc,xxxx,0xb5,xxxx,xxxx,xxxx,xxxx,0x05, 0x52,0x76,0x2b,0xe5,0xbd,xxxx,0x0e,0xb1, /* B0 */
//  new       new                      new   new  new  new  new  new       new  !!!!
	0x73,xxxx,xxxx,0x45,0x92,0x99,xxxx,0xf7, 0x3d,0xd0,0xb6,0x36,0xf9,0xfa,0x0f,xxxx, /* C0 */
//  new            new  new  new       new   new  new  !!!! new  new
	0x75,xxxx,0xaa,0x9c,xxxx,0x11,xxxx,xxxx, 0x27,0x4b,xxxx,0x2c,0x51,0x2e,0x4d,xxxx, /* D0 */
//  new       !!!! new       new             new  new       new  new  new  !!!!
	0x55,0x3c,xxxx,0xb7,xxxx,0xd1,0x8e,xxxx, 0xb2,xxxx,0x78,xxxx,0x12,xxxx,0x29,0x0c, /* E0 */
//  new  new       new  ???? new  new        new       new       new       new  new
	0x33,xxxx,0xf2,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,0x03,0x06,0xa8,xxxx,xxxx,0xcf,xxxx, /* F0 */
//  new       new                                 new  new  new            new
};
/*
Unknown (marked "????")

from shisen 2:
E4 -> pc: b08; f458, 1920 (99% 1 byte at boot and sometimes when a piece is selected)
          b65c (after one match is finished with the girl on the background or when you finish some levels in the other modes)
         (16 37 3e 3f 41 4a 4d 4e 4f 90 91 95 96)
4E -> pc: 7ac3, 7ae8 (after a 2 players match and in a 2 players match in "stalemate")

Found (marked "!!!!")
14 -> pc: 8b2a -> 42
16 -> pc: 1714, 1804, 1a70, 1cc3 (00 10 13 1C 20 21 22 34 D4 D5 after a match) -> 22
1B -> pc: 630e (it's used to update the high score) -> FD
1F -> pc: f30, f35, af74, 153a, 6dd8, 674f -> 09
30 -> pc: 19af, 1986 -> 49
31 -> pc: c804, c813, c822, 1358, c303, c312, c321, 1315, 1338 (2 bytes opcode) -> B4
34 -> pc: 42cd -> D3
56 -> pc: 6587 (can be 6C, 6D, 6E, 6F, A4, A6, A7, AA, AC, AD, AE at boot) -> A4
5A -> pc: ae83 (after an item is selected) (00 10 13 15 1B -1C already used- 1D 21 28) -> 21 (it's used to evidence tiles similar to the one selected)
62 -> pc: 5b3f (jump 71, 7C) -> 7C
6B -> pc: 1810, 1936 (jump) -> 7F
74 -> pc: ca75 -> D2
7F -> pc: 6b7e, 51d7, 5a8c, 5a9a, 5996 (jump: 7C, 7E) -> 7E
81 -> pc: 599d (jump: 7D, 7F) -> 7D
91 -> pc: 6e0c, 96ef, 96d0 (1 byte opcode) -> 2F
99 -> pc: 96f7, 9702 (after undo button is pressed) -> 1C (it's used to update the score when the you undo the moves)
BF -> pc: 6af3, 6b01, ca73, ab39 (2 bytes opcode) -> B1
C4 -> pc: deb4 -> 92 from bbmanw
EC -> pc: 966e, 9679 -> 12 from bbmanw
D2 -> pc: 631c (it's used to update the high score) (6C, 6E, A6, AA, AE) -> AA

the ones marked with "new" are checked against dynablst and bomberman tables
*/


const UINT8 test_decryption_table[256] = {
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 00 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 10 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 20 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 30 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 40 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 50 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 60 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 70 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 80 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* 90 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* A0 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* B0 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* C0 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* D0 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* E0 */
	xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx,xxxx, /* F0 */
};

static const UINT8 byte_count_table[256] = {
	2,2,2,2,2,3,1,1, 2,2,2,2,2,3,1,0, /* 00 */
	2,2,2,2,2,3,1,1, 2,2,2,2,2,3,1,1, /* 10 */
	2,2,2,2,2,3,1,1, 2,2,2,2,2,3,1,1, /* 20 */
	2,2,2,2,2,3,1,1, 2,2,2,2,2,3,1,1, /* 30 */
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, /* 40 */
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, /* 50 */
	1,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, /* 60 */
	1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, /* 70 */
	3,3,3,3,2,2,2,2, 2,2,2,2,0,0,0,0, /* 80 */
	1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, /* 90 */
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, /* A0 */
	2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3, /* B0 */
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, /* C0 */
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, /* D0 */
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, /* E0 */
	0,0,0,0,0,0,0,0, 1,1,1,1,1,1,0,0, /* F0 */
};

UINT8 *irem_cpu_decrypted;

void irem_cpu_decrypt(int cpu,const UINT8 *decryption_table)
{
	int A,size;
	UINT8 *rom;
//  int t[256];
#ifdef MAME_DEBUG
//    extern char *opmap1[];
#endif

	rom = memory_region(cpu+REGION_CPU1);
	size = memory_region_length(cpu+REGION_CPU1);
	irem_cpu_decrypted = auto_malloc(size);

	memory_set_decrypted_region(cpu,0,size-1,irem_cpu_decrypted);
	for (A = 0;A < size; A++)
		irem_cpu_decrypted[A] = decryption_table[rom[A]];

	// RZ note:
	// for "gussun" and "riskchal" an hack to not decrypt a not encrypted routine
	// we need a real nec v25+/35+ core to support 0x63 (brkn for "break native") instruction
	// for now we use "cd" (int) instruction + hack (to force to not decrypt the code from 0xa8fd to 0xa90b)

	if (m90_game_kludge==1) // for gussun and riskchal
		for (A = 0xa8fd;A < 0xa90c; A++)
			irem_cpu_decrypted[A] = rom[A];

/*
    for (A=0; A<256; A++) {
        t[A]=0;
        for (diff=0; diff<256; diff++)
            if (decryption_table[diff]==A) {
                t[A]++;
            }
#ifdef MAME_DEBUG
//        if (t[A]==0) logerror("Unused: [%d] %02x\t%s\n",byte_count_table[A],A,opmap1[A]);
//        if (t[A]>1) logerror("DUPLICATE: %02x\t%s\n",A,opmap1[A]);
#else
        if (t[A]==0) logerror("Unused: [%d] %02x\n",byte_count_table[A],A);
        if (t[A]>1) logerror("DUPLICATE: %02x\n",A);
#endif
    }
*/
}
