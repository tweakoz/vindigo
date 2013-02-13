/*************************************************************************

  Operation Wolf C-Chip Protection
  ================================

  The C-Chip (Taito TC0030CMD) is an unidentified mask programmed
  microcontroller of some sort with 64 pins.  It probably has
  about 2k of ROM and 8k of RAM.

  Interesting memory locations shared by cchip/68k:

    14 - dip switch A (written by 68k at start)
    15 - dip switch B (written by 68k at start)
    1b - Current level number (1-6)
    1c - Number of men remaining in level
    1e - Number of helicopters remaining in level
    1f - Number of tanks remaining in level
    20 - Number of boats remaining in level
    21 - Hostages in plane (last level)
    22 - Hostages remaining (last level)/Hostages saved (2nd last level)#
    27 - Set to 1 when final boss is destroyed
    32 - Set to 1 by cchip when level complete (no more enemies remaining)
    34 - Game state (0=attract mode, 1=intro, 2=in-game, 3=end-screen)
    51/52 - Used by cchip to signal change in credit level to 68k
    53 - Credit count
    75 - Set to 1 to trigger end of game boss
    7a - Used to trigger level data select command

  Notes on bootleg c-chip:
    Bootleg cchip forces english language mode
    Bootleg forces round 4 in attract mode
    Bootleg doesn't support service switch
    If you die after round 6 then the bootleg fails to reset the difficulty
    for the next game.

*************************************************************************/

#include "emu.h"
#include "includes/opwolf.h"

/* Select how coinage data is initialised in opwolf_cchip_data_w : 0 = user-defined in function - 1 = automatic */
#define OPWOLF_READ_COINAGE_FROM_ROM    1

/* List of possible regions */
enum {
	OPWOLF_REGION_BAD=0,
	OPWOLF_REGION_JAPAN,
	OPWOLF_REGION_US,
	OPWOLF_REGION_WORLD,
	OPWOLF_REGION_OTHER
};


static const UINT16 level_data_00[0xcc] =
{
	0x0480,0x1008,0x0300,0x5701,0x0001,0x0010,0x0480,0x1008,
	0x0300,0x5701,0x0001,0x002b,0x0780,0x0009,0x0300,0x4a01,
	0x0004,0x0020,0x0780,0x1208,0x0300,0x5d01,0x0004,0x0030,
	0x0780,0x0209,0x0300,0x4c01,0x0004,0x0038,0x0780,0x0309,
	0x0300,0x4d01,0x0004,0x0048,0x0980,0x1108,0x0300,0x5a01,
	0xc005,0x0018,0x0980,0x0109,0x0300,0x4b01,0xc005,0x0028,
	0x0b80,0x020a,0x0000,0x6401,0x8006,0x0004,0x0c80,0x010b,
	0x0000,0xf201,0x8006,0x8002,0x0b80,0x020a,0x0000,0x6401,
	0x8006,0x0017,0x0c80,0x010b,0x0000,0xf201,0x8006,0x8015,
	0x0b80,0x020a,0x0000,0x6401,0x0007,0x0034,0x0c80,0x010b,
	0x0000,0xf201,0x0007,0x8032,0x0b80,0x020a,0x0000,0x6401,
	0x8006,0x803e,0x0c80,0x010b,0x0000,0xf201,0x8006,0x803d,
	0x0b80,0x100a,0x0000,0x6001,0x0007,0x0008,0x0b80,0x100a,
	0x0000,0x6001,0x0007,0x000b,0x0b80,0x100a,0x0000,0x6001,
	0x0007,0x001b,0x0b80,0x100a,0x0000,0x6001,0x0007,0x001e,
	0x0b80,0x100a,0x0000,0x6001,0x8007,0x0038,0x0b80,0x100a,
	0x0000,0x6001,0x8007,0x003b,0x0b80,0x100a,0x0000,0x6001,
	0x0007,0x8042,0x0b80,0x100a,0x0000,0x6001,0x0007,0x8045,
	0x0c80,0x000b,0x0000,0xf101,0x800b,0x8007,0x0c80,0x000b,
	0x0000,0xf101,0x800b,0x801a,0x0c80,0x000b,0x0000,0xf101,
	0x000c,0x8037,0x0c80,0x000b,0x0000,0xf101,0x800b,0x0042,
	0x0c80,0xd04b,0x0000,0xf301,0x8006,0x8009,0x0c80,0xd04b,
	0x0000,0xf301,0x8006,0x801c,0x0c80,0xd04b,0x0000,0xf301,
	0x8006,0x0044,0x0c80,0x030b,0x0000,0xf401,0x0008,0x0028,
	0x0c80,0x030b,0x0000,0xf401,0x0008,0x804b,0x0c00,0x040b,
	0x0000,0xf501,0x0008,0x8026
};

static const UINT16 level_data_01[0xcc] =
{
	0x0780,0x0209,0x0300,0x4c01,0x0004,0x0010,0x0780,0x0209,
	0x0300,0x4c01,0x4004,0x0020,0x0780,0x0309,0x0300,0x4d01,
	0xe003,0x0030,0x0780,0x0309,0x0300,0x4d01,0x8003,0x0040,
	0x0780,0x0209,0x0300,0x4c01,0x8004,0x0018,0x0780,0x0309,
	0x0300,0x4d01,0xc003,0x0028,0x0b80,0x000b,0x0000,0x0b02,
	0x8009,0x0029,0x0b80,0x0409,0x0000,0x0f02,0x8008,0x8028,
	0x0b80,0x040a,0x0000,0x3502,0x000a,0x8028,0x0b80,0x050a,
	0x0000,0x1002,0x8006,0x8028,0x0b80,0x120a,0x0000,0x3602,
	0x0008,0x004d,0x0b80,0x120a,0x0000,0x3602,0x0008,0x004f,
	0x0b80,0x120a,0x0000,0x3602,0x0008,0x0001,0x0b80,0x120a,
	0x0000,0x3602,0x0008,0x0003,0x0b80,0x130a,0x0000,0x3a02,
	0x0007,0x0023,0x0b80,0x130a,0x0000,0x3a02,0x0007,0x8025,
	0x0b80,0x130a,0x0000,0x3a02,0x8009,0x0023,0x0b80,0x130a,
	0x0000,0x3a02,0x8009,0x8025,0x0b80,0x140a,0x0000,0x3e02,
	0x0007,0x000d,0x0b80,0x140a,0x0000,0x3e02,0x0007,0x800f,
	0x0b80,0x000b,0x0000,0x0102,0x0007,0x804e,0x0b80,0xd24b,
	0x0000,0x0302,0x0007,0x000e,0x0b80,0x000b,0x0000,0x0402,
	0x8006,0x0020,0x0b80,0xd34b,0x0000,0x0502,0x8006,0x0024,
	0x0b80,0x000b,0x0000,0x0602,0x8009,0x0001,0x0b80,0xd44b,
	0x0000,0x0702,0x800b,0x800b,0x0b80,0xd54b,0x0000,0x0802,
	0x800b,0x000e,0x0b80,0x000b,0x0000,0x0902,0x800b,0x0010,
	0x0b80,0x000b,0x0000,0x0a02,0x0009,0x0024,0x0b80,0xd64b,
	0x0000,0x0c02,0x000c,0x8021,0x0b80,0x000b,0x0000,0x0d02,
	0x000c,0x0025,0x0b80,0x000b,0x0000,0x0e02,0x8009,0x004e,
	0x0b80,0x0609,0x0300,0x4e01,0x8006,0x8012,0x0b80,0x0609,
	0x0300,0x4e01,0x0007,0x8007
};

static const UINT16 level_data_02[0xcc] =
{
	0x0480,0x000b,0x0300,0x4501,0x0001,0x0018,0x0480,0x000b,
	0x0300,0x4501,0x2001,0x0030,0x0780,0x1208,0x0300,0x5d01,
	0x0004,0x0010,0x0780,0x1208,0x0300,0x5d01,0x2004,0x001c,
	0x0780,0x1208,0x0300,0x5d01,0xe003,0x0026,0x0780,0x1208,
	0x0300,0x5d01,0x8003,0x0034,0x0780,0x1208,0x0300,0x5d01,
	0x3004,0x0040,0x0780,0x010c,0x0300,0x4601,0x4004,0x0022,
	0x0780,0x010c,0x0300,0x4601,0x6004,0x0042,0x0780,0x000c,
	0x0500,0x7b01,0x800b,0x0008,0x0780,0x010c,0x0300,0x4601,
	0x2004,0x0008,0x0000,0x0000,0x0000,0xf001,0x0000,0x0000,
	0x0000,0x0000,0x0000,0xf001,0x0000,0x0000,0x0000,0x0000,
	0x0000,0xf001,0x0000,0x0000,0x0b80,0x000b,0x0000,0x1902,
	0x000b,0x0004,0x0b80,0x000b,0x0000,0x1a02,0x0009,0x8003,
	0x0b80,0x000b,0x0000,0x1902,0x000b,0x000c,0x0b80,0x000b,
	0x0000,0x1a02,0x0009,0x800b,0x0b80,0x000b,0x0000,0x1902,
	0x000b,0x001c,0x0b80,0x000b,0x0000,0x1a02,0x0009,0x801b,
	0x0b80,0x000b,0x0000,0x1902,0x000b,0x002c,0x0b80,0x000b,
	0x0000,0x1a02,0x0009,0x802b,0x0b80,0x000b,0x0000,0x1902,
	0x000b,0x0044,0x0b80,0x000b,0x0000,0x1a02,0x0009,0x8043,
	0x0b80,0x000b,0x0000,0x1902,0x000b,0x004c,0x0b80,0x000b,
	0x0000,0x1a02,0x0009,0x804b,0x0b80,0x020c,0x0300,0x4801,
	0xa009,0x0010,0x0b80,0x020c,0x0300,0x4801,0xa009,0x0028,
	0x0b80,0x020c,0x0300,0x4801,0xa009,0x0036,0x0000,0x0000,
	0x0000,0xf001,0x0000,0x0000,0x0000,0x0000,0x0000,0xf001,
	0x0000,0x0000,0x0000,0x0000,0x0000,0xf001,0x0000,0x0000,
	0x0000,0x0000,0x0000,0xf001,0x0000,0x0000,0x0000,0x0000,
	0x0000,0xf001,0x0000,0x0000
};

static const UINT16 level_data_03[0xcc] =
{
	0x0480,0x000b,0x0300,0x4501,0x0001,0x0018,0x0480,0x000b,
	0x0300,0x4501,0x2001,0x002b,0x0780,0x010c,0x0300,0x4601,
	0x0004,0x000d,0x0780,0x000c,0x0500,0x7b01,0x800b,0x0020,
	0x0780,0x010c,0x0300,0x4601,0x2004,0x0020,0x0780,0x010c,
	0x0300,0x4601,0x8003,0x0033,0x0780,0x010c,0x0300,0x4601,
	0x0004,0x003c,0x0780,0x010c,0x0300,0x4601,0xd003,0x0045,
	0x0780,0x000c,0x0500,0x7b01,0x900b,0x0041,0x0780,0x010c,
	0x0300,0x4601,0x3004,0x0041,0x0b80,0x020c,0x0300,0x4801,
	0x0007,0x0000,0x0b80,0x410a,0x0000,0x2b02,0xe006,0x4049,
	0x0b80,0x020c,0x0300,0x4801,0x8007,0x000b,0x0b80,0x000b,
	0x0000,0x2702,0x800a,0x8005,0x0b80,0x000b,0x0000,0x1e02,
	0x0008,0x800e,0x0b80,0x000b,0x0000,0x1f02,0x8007,0x0011,
	0x0b80,0x000b,0x0000,0x2802,0x000b,0x0012,0x0b80,0x000b,
	0x0000,0x2002,0x0007,0x8015,0x0b80,0x000b,0x0000,0x2102,
	0x0007,0x801b,0x0b80,0x000b,0x0000,0x2902,0x800a,0x001a,
	0x0b80,0x000b,0x0000,0x2202,0x8007,0x001e,0x0b80,0x000b,
	0x0000,0x1e02,0x0008,0x0025,0x0b80,0x000b,0x0000,0x2302,
	0x8007,0x802c,0x0b80,0x000b,0x0000,0x2802,0x000b,0x8028,
	0x0b80,0x020c,0x0300,0x4801,0x0007,0x0030,0x0b80,0x400a,
	0x0000,0x2e02,0x4007,0x002d,0x0b80,0x000b,0x0000,0x2702,
	0x800a,0x8035,0x0b80,0x020c,0x0300,0x4801,0x8007,0x0022,
	0x0b80,0x000b,0x0000,0x2402,0x8007,0x0047,0x0b80,0x000b,
	0x0000,0x2a02,0x800a,0x004b,0x0b80,0x000b,0x0000,0x2502,
	0x0007,0x804b,0x0b80,0x000b,0x0000,0x2602,0x0007,0x004e,
	0x0b80,0x020c,0x0300,0x4801,0x0007,0x8043,0x0b80,0x020c,
	0x0300,0x4801,0x8007,0x803d
};

static const UINT16 level_data_04[0xcc] =
{
	0x0780,0x0209,0x0300,0x4c01,0x0004,0x0010,0x0780,0x0209,
	0x0300,0x4c01,0x4004,0x0020,0x0780,0x0309,0x0300,0x4d01,
	0xe003,0x0030,0x0780,0x0309,0x0300,0x4d01,0x8003,0x0040,
	0x0780,0x0209,0x0300,0x4c01,0x8004,0x0018,0x0780,0x0309,
	0x0300,0x4d01,0xc003,0x0028,0x0780,0x000b,0x0300,0x5601,
	0x8004,0x0008,0x0780,0x000b,0x0300,0x5601,0x8004,0x0038,
	0x0780,0x000b,0x0300,0x5501,0x8004,0x0048,0x0980,0x0509,
	0x0f00,0x0f01,0x4005,0x4007,0x0980,0x0509,0x0f00,0x0f01,
	0x4005,0x4037,0x0b80,0x030a,0x0000,0x1302,0x8006,0x0040,
	0x0b80,0x110a,0x0000,0x1502,0x8008,0x8048,0x0b80,0x110a,
	0x0000,0x1502,0x8008,0x8049,0x0b80,0x000b,0x0000,0xf601,
	0x0007,0x8003,0x0b80,0x000b,0x0000,0xf701,0x0007,0x0005,
	0x0b80,0x000b,0x0000,0xf901,0x0007,0x8008,0x0b80,0x000b,
	0x0000,0xf901,0x0007,0x0010,0x0b80,0x000b,0x0000,0xfa01,
	0x0007,0x8013,0x0b80,0x000b,0x0000,0xf801,0x800b,0x800b,
	0x0b80,0x000b,0x0000,0x0002,0x800b,0x801a,0x0b80,0x000b,
	0x0000,0xf901,0x0007,0x8017,0x0b80,0x000b,0x0000,0xfa01,
	0x0007,0x001b,0x0b80,0x000b,0x0000,0xf801,0x800b,0x0013,
	0x0b80,0x000b,0x0000,0x4202,0x800b,0x0016,0x0b80,0x000b,
	0x0000,0xfb01,0x8007,0x8020,0x0b80,0x000b,0x0000,0xf601,
	0x0007,0x8023,0x0b80,0x000b,0x0000,0x4202,0x800b,0x800e,
	0x0b80,0x000b,0x0000,0x4302,0x800b,0x801d,0x0b80,0x000b,
	0x0000,0xf701,0x0007,0x0025,0x0b80,0x000b,0x0000,0xfd01,
	0x8006,0x003f,0x0b80,0x000b,0x0000,0xfe01,0x0007,0x0046,
	0x0b80,0x000b,0x0000,0xff01,0x8007,0x8049,0x0b80,0x000b,
	0x0000,0xfc01,0x8009,0x0042
};

static const UINT16 level_data_05[0xcc] =
{
	0x0480,0x1008,0x0300,0x5701,0x0001,0x0010,0x0480,0x1008,
	0x0300,0x5701,0x0001,0x002b,0x0780,0x0009,0x0300,0x4a01,
	0x0004,0x0020,0x0780,0x1208,0x0300,0x5d01,0x0004,0x0030,
	0x0780,0x0209,0x0300,0x4c01,0x0004,0x0038,0x0780,0x0309,
	0x0300,0x4d01,0x0004,0x0048,0x0980,0x1108,0x0300,0x5a01,
	0xc005,0x0018,0x0980,0x0109,0x0300,0x4b01,0xc005,0x0028,
	0x0b80,0x020a,0x0000,0x6401,0x8006,0x0004,0x0c80,0x010b,
	0x0000,0xf201,0x8006,0x8002,0x0b80,0x020a,0x0000,0x6401,
	0x8006,0x0017,0x0c80,0x010b,0x0000,0xf201,0x8006,0x8015,
	0x0b80,0x020a,0x0000,0x6401,0x0007,0x0034,0x0c80,0x010b,
	0x0000,0xf201,0x0007,0x8032,0x0b80,0x020a,0x0000,0x6401,
	0x8006,0x803e,0x0c80,0x010b,0x0000,0xf201,0x8006,0x803d,
	0x0b80,0x100a,0x0000,0x6001,0x0007,0x0008,0x0b80,0x100a,
	0x0000,0x6001,0x0007,0x000b,0x0b80,0x100a,0x0000,0x6001,
	0x0007,0x001b,0x0b80,0x100a,0x0000,0x6001,0x0007,0x001e,
	0x0b80,0x100a,0x0000,0x6001,0x8007,0x0038,0x0b80,0x100a,
	0x0000,0x6001,0x8007,0x003b,0x0b80,0x100a,0x0000,0x6001,
	0x0007,0x8042,0x0b80,0x100a,0x0000,0x6001,0x0007,0x8045,
	0x0c80,0x000b,0x0000,0xf101,0x800b,0x8007,0x0c80,0x000b,
	0x0000,0xf101,0x800b,0x801a,0x0c80,0x000b,0x0000,0xf101,
	0x000c,0x8037,0x0c80,0x000b,0x0000,0xf101,0x800b,0x0042,
	0x0c80,0xd04b,0x0000,0xf301,0x8006,0x8009,0x0c80,0xd04b,
	0x0000,0xf301,0x8006,0x801c,0x0c80,0xd04b,0x0000,0xf301,
	0x8006,0x0044,0x0c80,0x030b,0x0000,0xf401,0x0008,0x0028,
	0x0c80,0x030b,0x0000,0xf401,0x0008,0x804b,0x0c00,0x040b,
	0x0000,0xf501,0x0008,0x8026
};

static const UINT16 level_data_06[0xcc] =
{
	0x0000,0x1008,0x0300,0x5701,0x0001,0x0010,0x0000,0x1008,
	0x0300,0x5701,0x0001,0x002b,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0700,0x0009,0x0300,0x4a01,0x0004,0x0020,
	0x0700,0x1208,0x0300,0x5d01,0x0004,0x0030,0x0700,0x0209,
	0x0300,0x4c01,0x0004,0x0038,0x0700,0x0309,0x0300,0x4d01,
	0x0004,0x0048,0x0900,0x1108,0x0300,0x5a01,0xc005,0x0018,
	0x0900,0x0109,0x0300,0x4b01,0xc005,0x0028,0x0000,0x000b,
	0x0000,0x0000,0x0018,0x0000,0x0000,0x000b,0x0000,0x0000,
	0x0018,0x0000,0x0000,0x000b,0x0000,0x0000,0x0018,0x0000,
	0x0000,0x000b,0x0000,0x0000,0x0018,0x0000,0x0000,0x000b,
	0x0000,0x0000,0x0018,0x0000,0x0000,0x000b,0x0000,0x0000,
	0x0018,0x0000,0x0000,0x000b,0x0000,0x0000,0x0018,0x0000,
	0x0000,0x000b,0x0000,0x0000,0x0018,0x0000,0x0000,0x000b,
	0x0000,0x0000,0x0018,0x0000,0x0980,0xdb4c,0x0000,0x3202,
	0x0006,0x0004,0x0b80,0x0609,0x0300,0x4e01,0x5006,0x8002,
	0x0b80,0x0609,0x0300,0x4e01,0x5006,0x8003,0x0b80,0x0609,
	0x0300,0x4e01,0x5006,0x8004,0x0b80,0x0609,0x0300,0x4e01,
	0x5006,0x0008,0x0b80,0x0609,0x0300,0x4e01,0x5006,0x0010,
	0x0b80,0x0609,0x0300,0x4e01,0x5006,0x0012,0x0b80,0x0609,
	0x0300,0x4e01,0x5006,0x0014,0x0b80,0x0609,0x0300,0x4e01,
	0x5006,0x0016,0x0b80,0x0609,0x0300,0x4e01,0x5006,0x0018,
	0x0b80,0x0609,0x0300,0x4e01,0x5006,0x0020,0x0b80,0x0609,
	0x0300,0x4e01,0x5006,0x0023,0x0b80,0x0609,0x0300,0x4e01,
	0x5006,0x0030,0x0b80,0x0609,0x0300,0x4e01,0x5006,0x0038,
	0x0b80,0x0609,0x0300,0x4e01,0x5006,0x0040,0x0b80,0x0609,
	0x0300,0x4e01,0x5006,0x0042
};

static const UINT16 *const level_data_lookup[] =
{
	level_data_00,
	level_data_01,
	level_data_02,
	level_data_03,
	level_data_04,
	level_data_05,
	level_data_06,
};


TIMER_CALLBACK_MEMBER(opwolf_state::opwolf_timer_callback)
{
	// Level data command
	if (m_current_cmd == 0xf5)
	{
		int level = m_cchip_ram[0x1b];
		const UINT16* level_data = level_data_lookup[level];
		int i = 0;
		for (i = 0; i < 0xcc; i++)
		{
			m_cchip_ram[0x200 + i*2 + 0] = level_data[i]>>8;
			m_cchip_ram[0x200 + i*2 + 1] = level_data[i]&0xff;
		}

		// The bootleg cchip writes 0 to these locations - hard to tell what the real one writes
		m_cchip_ram[0x0] = 0;
		m_cchip_ram[0x76] = 0;
		m_cchip_ram[0x75] = 0;
		m_cchip_ram[0x74] = 0;
		m_cchip_ram[0x72] = 0;
		m_cchip_ram[0x71] = 0;
		m_cchip_ram[0x70] = 0;
		m_cchip_ram[0x66] = 0;
		m_cchip_ram[0x2b] = 0;
		m_cchip_ram[0x30] = 0;
		m_cchip_ram[0x31] = 0;
		m_cchip_ram[0x32] = 0;
		m_cchip_ram[0x27] = 0;
		m_c588 = 0;
		m_c589 = 0;
		m_c58a = 0;

		m_cchip_ram[0x1a] = 0;
		m_cchip_ram[0x7a] = 1; // Signal command complete
	}

	m_current_cmd = 0;
}

void opwolf_state::updateDifficulty( int mode )
{

	// The game is made up of 6 rounds, when you complete the
	// sixth you return to the start but with harder difficulty.
	if (mode == 0)
	{
		switch (m_cchip_ram[0x15]&3) // Dipswitch B
		{
		case 3:
			m_cchip_ram[0x2c] = 0x31;
			m_cchip_ram[0x77] = 0x05;
			m_cchip_ram[0x25] = 0x0f;
			m_cchip_ram[0x26] = 0x0b;
			break;
		case 0:
			m_cchip_ram[0x2c] = 0x20;
			m_cchip_ram[0x77] = 0x06;
			m_cchip_ram[0x25] = 0x07;
			m_cchip_ram[0x26] = 0x03;
			break;
		case 1:
			m_cchip_ram[0x2c] = 0x31;
			m_cchip_ram[0x77] = 0x05;
			m_cchip_ram[0x25] = 0x0f;
			m_cchip_ram[0x26] = 0x0b;
			break;
		case 2:
			m_cchip_ram[0x2c] = 0x3c;
			m_cchip_ram[0x77] = 0x04;
			m_cchip_ram[0x25] = 0x13;
			m_cchip_ram[0x26] = 0x0f;
			break;
		}
	}
	else
	{
		switch (m_cchip_ram[0x15]&3) // Dipswitch B
		{
		case 3:
			m_cchip_ram[0x2c] = 0x46;
			m_cchip_ram[0x77] = 0x05;
			m_cchip_ram[0x25] = 0x11;
			m_cchip_ram[0x26] = 0x0e;
			break;
		case 0:
			m_cchip_ram[0x2c] = 0x30;
			m_cchip_ram[0x77] = 0x06;
			m_cchip_ram[0x25] = 0x0b;
			m_cchip_ram[0x26] = 0x03;
			break;
		case 1:
			m_cchip_ram[0x2c] = 0x3a;
			m_cchip_ram[0x77] = 0x05;
			m_cchip_ram[0x25] = 0x0f;
			m_cchip_ram[0x26] = 0x09;
			break;
		case 2:
			m_cchip_ram[0x2c] = 0x4c;
			m_cchip_ram[0x77] = 0x04;
			m_cchip_ram[0x25] = 0x19;
			m_cchip_ram[0x26] = 0x11;
			break;
		};
	}
}

/*************************************
 *
 * Writes to C-Chip - Important Bits
 *
 *************************************/

WRITE16_MEMBER(opwolf_state::opwolf_cchip_status_w)
{
	// This is written once after the C-Chip init is complete (and passes)
	// We use it to setup some initial state (it's not clear if the real
	// c-chip sets this here, or if it's as a side-effect of the other
	// init sequence data).

	m_cchip_ram[0x3d] = 1;
	m_cchip_ram[0x7a] = 1;
	updateDifficulty(0);
}

WRITE16_MEMBER(opwolf_state::opwolf_cchip_bank_w)
{
	m_current_bank = data & 7;
}

WRITE16_MEMBER(opwolf_state::opwolf_cchip_data_w)
{
	m_cchip_ram[(m_current_bank * 0x400) + offset] = data & 0xff;

//  if (offset != 0x64 && offset != 0x65 && offset != 0x66 && offset != 0x67 && offset != 0x68 && offset != 0x69)
//      logerror("%08x:  opwolf c write %04x %04x\n", space.device().safe_pc(), offset, data);

	if (m_current_bank == 0)
	{
		// Dip switch A is written here by the 68k - precalculate the coinage values
		// Shouldn't we directly read the values from the ROM area ?
		if (offset == 0x14)
		{
#if OPWOLF_READ_COINAGE_FROM_ROM
			UINT16* rom = (UINT16*)memregion("maincpu")->base();
			UINT32 coin_table[2] = {0, 0};
			UINT8 coin_offset[2];
			int slot;

			if ((m_opwolf_region == OPWOLF_REGION_JAPAN) || (m_opwolf_region == OPWOLF_REGION_US))
			{
				coin_table[0] = 0x03ffce;
				coin_table[1] = 0x03ffce;
			}
			if ((m_opwolf_region == OPWOLF_REGION_WORLD) || (m_opwolf_region == OPWOLF_REGION_OTHER))
			{
				coin_table[0] = 0x03ffde;
				coin_table[1] = 0x03ffee;
			}
			coin_offset[0] = 12 - (4 * ((data & 0x30) >> 4));
			coin_offset[1] = 12 - (4 * ((data & 0xc0) >> 6));

			for (slot = 0; slot < 2; slot++)
			{
				if (coin_table[slot])
				{
					m_cchip_coins_for_credit[slot] = rom[(coin_table[slot] + coin_offset[slot] + 0) / 2] & 0xff;
					m_cchip_credits_for_coin[slot] = rom[(coin_table[slot] + coin_offset[slot] + 2) / 2] & 0xff;
				}
			}
#else
			if ((m_opwolf_region == OPWOLF_REGION_JAPAN) || (m_opwolf_region == OPWOLF_REGION_US))
			{
				switch (data&0x30)  /* table at 0x03ffce.w - 4 * 2 words (coins for credits first) - inverted order */
				{
					case 0x00: m_cchip_coins_for_credit[0] = 2; cchip_credits_for_coin[0] = 3; break;
					case 0x10: m_cchip_coins_for_credit[0] = 2; cchip_credits_for_coin[0] = 1; break;
					case 0x20: m_cchip_coins_for_credit[0] = 1; cchip_credits_for_coin[0] = 2; break;
					case 0x30: m_cchip_coins_for_credit[0] = 1; cchip_credits_for_coin[0] = 1; break;
				}
				switch (data&0xc0)  /* table at 0x03ffce.w - 4 * 2 words (coins for credits first) - inverted order */
				{
					case 0x00: m_cchip_coins_for_credit[1] = 2; cchip_credits_for_coin[1] = 3; break;
					case 0x40: m_cchip_coins_for_credit[1] = 2; cchip_credits_for_coin[1] = 1; break;
					case 0x80: m_cchip_coins_for_credit[1] = 1; cchip_credits_for_coin[1] = 2; break;
					case 0xc0: m_cchip_coins_for_credit[1] = 1; cchip_credits_for_coin[1] = 1; break;
				}
			}

			if ((opwolf_region == OPWOLF_REGION_WORLD) || (opwolf_region == OPWOLF_REGION_OTHER))
			{
				switch (data&0x30)  /* table at 0x03ffde.w - 4 * 2 words (coins for credits first) - inverted order */
				{
					case 0x00: m_cchip_coins_for_credit[0] = 4; cchip_credits_for_coin[0] = 1; break;
					case 0x10: m_cchip_coins_for_credit[0] = 3; cchip_credits_for_coin[0] = 1; break;
					case 0x20: m_cchip_coins_for_credit[0] = 2; cchip_credits_for_coin[0] = 1; break;
					case 0x30: m_cchip_coins_for_credit[0] = 1; cchip_credits_for_coin[0] = 1; break;
				}
				switch (data & 0xc0)    /* table at 0x03ffee.w - 4 * 2 words (coins for credits first) - inverted order */
				{
					case 0x00: m_cchip_coins_for_credit[1] = 1; cchip_credits_for_coin[1] = 6; break;
					case 0x40: m_cchip_coins_for_credit[1] = 1; cchip_credits_for_coin[1] = 4; break;
					case 0x80: m_cchip_coins_for_credit[1] = 1; cchip_credits_for_coin[1] = 3; break;
					case 0xc0: m_cchip_coins_for_credit[1] = 1; cchip_credits_for_coin[1] = 2; break;
				}
			}
#endif
		}

		// Dip switch B
		if (offset == 0x15)
		{
			updateDifficulty(0);
		}
	}
}


/*************************************
 *
 * Reads from C-Chip
 *
 *************************************/

READ16_MEMBER(opwolf_state::opwolf_cchip_status_r)
{
	/*
	    Bit 0x4 = Error signal
	    Bit 0x1 = Ready signal
	*/
	return 0x1; /* Return 0x5 for C-Chip error */
}

READ16_MEMBER(opwolf_state::opwolf_cchip_data_r)
{
//  if (offset!=0x7f && offset!=0x1c && offset!=0x1d && offset!=0x1e && offset!=0x1f && offset!=0x20 && space.device().safe_pc()!=0xc18 && space.device().safe_pc()!=0xc2e && space.device().safe_pc()!=0xc9e && offset!=0x50 && offset!=0x51 && offset!=0x52 && offset!=0x53 && offset!=0x5 && offset!=0x13 && offset!=0x79 && offset!=0x12 && offset!=0x34)
//      logerror("%08x:  opwolf c read %04x (bank %04x)\n", space.device().safe_pc(), offset, m_current_bank);

	return m_cchip_ram[(m_current_bank * 0x400) + offset];
}

/*************************************
 *
 * C-Chip Tick
 *
 *************************************/

TIMER_CALLBACK_MEMBER(opwolf_state::cchip_timer)
{
	// Update input ports, these are used by both the 68k directly and by the c-chip
	m_cchip_ram[0x4] = ioport("IN0")->read();
	m_cchip_ram[0x5] = ioport("IN1")->read();

	// Coin slots
	if (m_cchip_ram[0x4] != m_cchip_last_04)
	{
		int slot = -1;

		if (m_cchip_ram[0x4] & 1) slot = 0;
		if (m_cchip_ram[0x4] & 2) slot = 1;

		if (slot != -1)
		{
			m_cchip_coins[slot]++;
			if (m_cchip_coins[slot] >= m_cchip_coins_for_credit[slot])
			{
				m_cchip_ram[0x53] += m_cchip_credits_for_coin[slot];
				m_cchip_ram[0x51] = 0x55;
				m_cchip_ram[0x52] = 0x55;
				m_cchip_coins[slot] -= m_cchip_coins_for_credit[slot];
			}
			coin_counter_w(machine(), slot, 1);
		}

		if (m_cchip_ram[0x53] > 9)
			m_cchip_ram[0x53] = 9;
	}
	m_cchip_last_04 = m_cchip_ram[0x4];

	// Service switch
	if (m_cchip_ram[0x5] != m_cchip_last_05)
	{
		if ((m_cchip_ram[0x5] & 4)==0)
		{
			m_cchip_ram[0x53]++;
			m_cchip_ram[0x51] = 0x55;
			m_cchip_ram[0x52] = 0x55;
		}
	}
	m_cchip_last_05=m_cchip_ram[0x5];

	// Cchip handles coin lockout (68k flags error if more than 9 coins)
	coin_lockout_w(machine(), 1, m_cchip_ram[0x53] == 9);
	coin_lockout_w(machine(), 0, m_cchip_ram[0x53] == 9);
	coin_counter_w(machine(), 0, 0);
	coin_counter_w(machine(), 1, 0);

	// Special handling for last level
	if (m_cchip_ram[0x1b] == 0x6)
	{
		// Check for triggering final helicopter (end boss)
		if (m_c58a == 0)
		{
			if ((m_cchip_ram[0x72] & 0x7f) >= 8 && m_cchip_ram[0x74] == 0 && m_cchip_ram[0x1c] == 0 && m_cchip_ram[0x1d] == 0 && m_cchip_ram[0x1f] == 0)
			{
				m_cchip_ram[0x30] = 1;
				m_cchip_ram[0x74] = 1;
				m_c58a = 1;
			}
		}

		if (m_cchip_ram[0x1a] == 0x90)
			m_cchip_ram[0x74] = 0;

		if (m_c58a != 0)
		{
			if (m_c589 == 0 && m_cchip_ram[0x27] == 0 && m_cchip_ram[0x75] == 0 && m_cchip_ram[0x1c] == 0 && m_cchip_ram[0x1d] == 0 && m_cchip_ram[0x1e] == 0 && m_cchip_ram[0x1f] == 0)
			{
				m_cchip_ram[0x31] = 1;
				m_cchip_ram[0x75] = 1;
				m_c589 = 1;
			}
		}

		if (m_cchip_ram[0x2b] == 0x1)
		{
			m_cchip_ram[0x2b] = 0;

			if (m_cchip_ram[0x30] == 0x1)
			{
				if (m_cchip_ram[0x1a] != 0x90)
					m_cchip_ram[0x1a]--;
			}

			if (m_cchip_ram[0x72] == 0x9)
			{
				if (m_cchip_ram[0x76] != 0x4)
				{
					m_cchip_ram[0x76] = 3;
				}
			}
			else
			{
				// This timer is derived from the bootleg rather than the real board, I'm not 100% sure about it
				m_c588 |= 0x80;

				m_cchip_ram[0x72] = m_c588;
				m_c588++;

				m_cchip_ram[0x1a]--;
				m_cchip_ram[0x1a]--;
				m_cchip_ram[0x1a]--;
			}
		}

		// Update difficulty settings
		if (m_cchip_ram[0x76] == 0)
		{
			m_cchip_ram[0x76] = 1;
			updateDifficulty(1);
		}
	}

	// These variables are cleared every frame during attract mode and the intro.
	if (m_cchip_ram[0x34] < 2)
	{
		updateDifficulty(0);
		m_cchip_ram[0x76] = 0;
		m_cchip_ram[0x75] = 0;
		m_cchip_ram[0x74] = 0;
		m_cchip_ram[0x72] = 0;
		m_cchip_ram[0x71] = 0;
		m_cchip_ram[0x70] = 0;
		m_cchip_ram[0x66] = 0;
		m_cchip_ram[0x2b] = 0;
		m_cchip_ram[0x30] = 0;
		m_cchip_ram[0x31] = 0;
		m_cchip_ram[0x32] = 0;
		m_cchip_ram[0x27] = 0;
		m_c588 = 0;
		m_c589 = 0;
		m_c58a = 0;
	}

	// Check for level completion (all enemies destroyed)
	if (m_cchip_ram[0x1c] == 0 && m_cchip_ram[0x1d] == 0 && m_cchip_ram[0x1e] == 0 && m_cchip_ram[0x1f] == 0 && m_cchip_ram[0x20] == 0)
	{
		// Special handling for end of level 6
		if (m_cchip_ram[0x1b] == 0x6)
		{
			// Don't signal end of level until final boss is destroyed
			if (m_cchip_ram[0x27] == 0x1)
				m_cchip_ram[0x32] = 1;
		}
		else
		{
			// Signal end of level
			m_cchip_ram[0x32] = 1;
		}
	}

	if (m_cchip_ram[0xe] == 1)
	{
		m_cchip_ram[0xe] = 0xfd;
		m_cchip_ram[0x61] = 0x04;
	}

	// Access level data command (address 0xf5 goes from 1 -> 0)
	if (m_cchip_ram[0x7a] == 0 && m_cchip_last_7a != 0 && m_current_cmd != 0xf5)
	{
		// Simulate time for command to execute (exact timing unknown, this is close)
		m_current_cmd = 0xf5;
		machine().scheduler().timer_set(m_maincpu->cycles_to_attotime(80000), timer_expired_delegate(FUNC(opwolf_state::opwolf_timer_callback),this));
	}
	m_cchip_last_7a = m_cchip_ram[0x7a];

	// This seems to some kind of periodic counter - results are expected
	// by the 68k when the counter reaches 0xa
	if (m_cchip_ram[0x7f] == 0xa)
	{
		m_cchip_ram[0xfe] = 0xf7;
		m_cchip_ram[0xff] = 0x6e;
	}

	// These are set every frame
	m_cchip_ram[0x64] = 0;
	m_cchip_ram[0x66] = 0;
}

/*************************************
 *
 * C-Chip State Saving
 *
 *************************************/

void opwolf_state::opwolf_cchip_init(  )
{

	m_cchip_ram.allocate(0x400 * 8);

	save_item(NAME(m_current_bank));
	save_item(NAME(m_current_cmd));
	save_item(NAME(m_cchip_last_7a));
	save_item(NAME(m_cchip_last_04));
	save_item(NAME(m_cchip_last_05));
	save_item(NAME(m_c588));
	save_item(NAME(m_c589));
	save_item(NAME(m_c58a));
	save_item(NAME(m_cchip_coins));
	save_item(NAME(m_cchip_coins_for_credit));
	save_item(NAME(m_cchip_credits_for_coin));

	m_current_bank = 0;
	m_current_cmd = 0;
	m_cchip_last_7a = 0;
	m_cchip_last_04 = 0xfc;
	m_cchip_last_05 = 0xff;
	m_c588 = 0;
	m_c589 = 0;
	m_c58a = 0;
	m_cchip_coins[0] = 0;
	m_cchip_coins[1] = 0;
	m_cchip_coins_for_credit[0] = 1;
	m_cchip_credits_for_coin[0] = 1;
	m_cchip_coins_for_credit[1] = 1;
	m_cchip_credits_for_coin[1] = 1;

	machine().scheduler().timer_pulse(attotime::from_hz(60), timer_expired_delegate(FUNC(opwolf_state::cchip_timer),this));
}
