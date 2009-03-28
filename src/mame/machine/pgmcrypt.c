/* IGS PGM System Encryptions */

#include "driver.h"
#include "includes/pgm.h"


static const UINT8 kov_tab[256] = {
	0x17, 0x1c, 0xe3, 0x02, 0x62, 0x59, 0x97, 0x4a, 0x67, 0x4d, 0x1f, 0x11, 0x76, 0x64, 0xc1, 0xe1,
	0xd2, 0x41, 0x9f, 0xfd, 0xfa, 0x04, 0xfe, 0xab, 0x89, 0xeb, 0xc0, 0xf5, 0xac, 0x2b, 0x64, 0x22,
	0x90, 0x7d, 0x88, 0xc5, 0x8c, 0xe0, 0xd9, 0x70, 0x3c, 0xf4, 0x7d, 0x31, 0x1c, 0xca, 0xe2, 0xf1,
	0x31, 0x82, 0x86, 0xb1, 0x55, 0x95, 0x77, 0x01, 0x77, 0x3b, 0xab, 0xe6, 0x88, 0xef, 0x77, 0x11,
	0x56, 0x01, 0xac, 0x55, 0xf7, 0x6d, 0x9b, 0x6d, 0x92, 0x14, 0x23, 0xae, 0x4b, 0x80, 0xae, 0x6a,
	0x43, 0xcc, 0x35, 0xfe, 0xa1, 0x0d, 0xb3, 0x21, 0x4e, 0x4c, 0x99, 0x80, 0xc2, 0x3d, 0xce, 0x46,
	0x9b, 0x5d, 0x68, 0x75, 0xfe, 0x1e, 0x25, 0x41, 0x24, 0xa0, 0x79, 0xfd, 0xb5, 0x67, 0x93, 0x07,
	0x3a, 0x78, 0x24, 0x64, 0xe1, 0xa3, 0x62, 0x75, 0x38, 0x65, 0x8a, 0xbf, 0xf9, 0x7c, 0x00, 0xa0,
	0x6d, 0xdb, 0x1f, 0x80, 0x37, 0x37, 0x8e, 0x97, 0x1a, 0x45, 0x61, 0x0e, 0x10, 0x24, 0x8a, 0x27,
	0xf2, 0x44, 0x91, 0x3e, 0x62, 0x44, 0xc5, 0x55, 0xe6, 0x8e, 0x5a, 0x25, 0x8a, 0x90, 0x25, 0x74,
	0xa0, 0x95, 0x33, 0xf7, 0x51, 0xce, 0xe4, 0xa0, 0x13, 0xcf, 0x33, 0x1e, 0x59, 0x5b, 0xec, 0x42,
	0xc5, 0xb8, 0xe4, 0xc5, 0x71, 0x38, 0xc5, 0x6b, 0x8d, 0x1d, 0x84, 0xf8, 0x4e, 0x21, 0x6d, 0xdc,
	0x2c, 0xf1, 0xae, 0xad, 0x19, 0xc5, 0xed, 0x8e, 0x36, 0xb5, 0x81, 0x94, 0xfe, 0x62, 0x3a, 0xe8,
	0xc9, 0x95, 0x84, 0xbd, 0x65, 0x15, 0x16, 0x15, 0xd2, 0xe7, 0x16, 0xd7, 0x9c, 0xd3, 0xd2, 0x66,
	0xf6, 0x46, 0xe3, 0x32, 0x62, 0x51, 0x86, 0x4a, 0x67, 0xcc, 0x4d, 0xea, 0x37, 0x45, 0xd5, 0xa6,
	0x80, 0xe6, 0xba, 0xb3, 0x08, 0xd8, 0x30, 0x5b, 0x5f, 0xf2, 0x5a, 0xfb, 0x63, 0xb0, 0xa4, 0x41
};

void pgm_kov_decrypt(running_machine *machine)
{

	int i;
	UINT16 *src = (UINT16 *) (memory_region(machine, "maincpu")+0x100000);

	int rom_size = 0x400000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if((i & 0x040480) != 0x000080)
			x ^= 0x0001;

		if((i & 0x004008) == 0x004008)
			x ^= 0x0002;

		if((i & 0x000030) == 0x000010 && (i & 0x180000) != 0x080000)
			x ^= 0x0004;

		if((i & 0x000242) != 0x000042)
			x ^= 0x0008;

		if((i & 0x008100) == 0x008000)
			x ^= 0x0010;

		if((i & 0x022004) != 0x000004)
			x ^= 0x0020;

		if((i & 0x011800) != 0x010000)
			x ^= 0x0040;

		if((i & 0x004820) == 0x004820)
			x ^= 0x0080;

		x ^= kov_tab[i & 0xff] << 8;

		src[i] = x;
	}
}


static const UINT8 kovsh_tab[256] = {
	0xe7, 0x06, 0xa3, 0x70, 0xf2, 0x58, 0xe6, 0x59, 0xe4, 0xcf, 0xc2, 0x79, 0x1d, 0xe3, 0x71, 0x0e,
	0xb6, 0x90, 0x9a, 0x2a, 0x8c, 0x41, 0xf7, 0x82, 0x9b, 0xef, 0x99, 0x0c, 0xfa, 0x2f, 0xf1, 0xfe,
	0x8f, 0x70, 0xf4, 0xc1, 0xb5, 0x3d, 0x7c, 0x60, 0x4c, 0x09, 0xf4, 0x2e, 0x7c, 0x87, 0x63, 0x5f,
	0xce, 0x99, 0x84, 0x95, 0x06, 0x9a, 0x20, 0x23, 0x5a, 0xb9, 0x52, 0x95, 0x48, 0x2c, 0x84, 0x60,
	0x69, 0xe3, 0x93, 0x49, 0xb9, 0xd6, 0xbb, 0xd6, 0x9e, 0xdc, 0x96, 0x12, 0xfa, 0x60, 0xda, 0x5f,
	0x55, 0x5d, 0x5b, 0x20, 0x07, 0x1e, 0x97, 0x42, 0x77, 0xea, 0x1d, 0xe0, 0x70, 0xfb, 0x6a, 0x00,
	0x77, 0x9a, 0xef, 0x1b, 0xe0, 0xf9, 0x0d, 0xc1, 0x2e, 0x2f, 0xef, 0x25, 0x29, 0xe5, 0xd8, 0x2c,
	0xaf, 0x01, 0xd9, 0x6c, 0x31, 0xce, 0x5c, 0xea, 0xab, 0x1c, 0x92, 0x16, 0x61, 0xbc, 0xe4, 0x7c,
	0x5a, 0x76, 0xe9, 0x92, 0x39, 0x5b, 0x97, 0x60, 0xea, 0x57, 0x83, 0x9c, 0x92, 0x29, 0xa7, 0x12,
	0xa9, 0x71, 0x7a, 0xf9, 0x07, 0x68, 0xa7, 0x45, 0x88, 0x10, 0x81, 0x12, 0x2c, 0x67, 0x4d, 0x55,
	0x33, 0xf0, 0xfa, 0xd7, 0x1d, 0x4d, 0x0e, 0x63, 0x03, 0x34, 0x65, 0xe2, 0x76, 0x0f, 0x98, 0xa9,
	0x5f, 0x9a, 0xd3, 0xca, 0xdd, 0xc1, 0x5b, 0x3d, 0x4d, 0xf8, 0x40, 0x08, 0xdc, 0x05, 0x38, 0x00,
	0xcb, 0x24, 0x02, 0xff, 0x39, 0xe2, 0x9e, 0x04, 0x9a, 0x08, 0x63, 0xc8, 0x2b, 0x5a, 0x34, 0x06,
	0x62, 0xc1, 0xbb, 0x8a, 0xd0, 0x54, 0x4c, 0x43, 0x21, 0x4e, 0x4c, 0x99, 0x80, 0xc2, 0x3d, 0xce,
	0x2a, 0x7b, 0x09, 0x62, 0x1a, 0x91, 0x9b, 0xc3, 0x41, 0x24, 0xa0, 0xfd, 0xb5, 0x67, 0x93, 0x07,
	0xa7, 0xb8, 0x85, 0x8a, 0xa1, 0x1e, 0x4f, 0xb6, 0x75, 0x38, 0x65, 0x8a, 0xf9, 0x7c, 0x00, 0xa0,
};


void pgm_kovsh_decrypt(running_machine *machine)
{

	int i;
	UINT16 *src = (UINT16 *) (memory_region(machine, "maincpu")+0x100000);

	int rom_size = 0x400000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if((i & 0x040080) != 0x000080)
			x ^= 0x0001;

		if((i & 0x004008) == 0x004008 && (i & 0x180000) != 0x000000)
			x ^= 0x0002;

		if((i & 0x000030) == 0x000010)
			x ^= 0x0004;

		if((i & 0x000242) != 0x000042)
			x ^= 0x0008;

		if((i & 0x008100) == 0x008000)
			x ^= 0x0010;

		if((i & 0x002004) != 0x000004)
			x ^= 0x0020;

		if((i & 0x011800) != 0x010000)
			x ^= 0x0040;

		if((i & 0x000820) == 0x000820)
			x ^= 0x0080;

		x ^= kovsh_tab[i & 0xff] << 8;

		src[i] = x;
	}
}

void pgm_dw2_decrypt(running_machine *machine)
{

	int i;
	UINT16 *src = (UINT16 *) (memory_region(machine, "maincpu")+0x100000);

	int rom_size = 0x80000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if(((i & 0x020890) == 0x000000)
		   || ((i & 0x020000) == 0x020000 && (i & 0x001500) != 0x001400))
			x ^= 0x0002;

		if(((i & 0x020400) == 0x000000 && (i & 0x002010) != 0x002010)
		   || ((i & 0x020000) == 0x020000 && (i & 0x000148) != 0x000140))
			x ^= 0x0400;

		src[i] = x;
	}
}

static const UINT8 djlzz_tab[256] = {
  0xd9, 0x92, 0xb2, 0xbc, 0xa5, 0x88, 0xe3, 0x48, 0x7d, 0xeb, 0xc5, 0x4d, 0x31, 0xe4, 0x82, 0xbc,
  0x82, 0xcf, 0xe7, 0xf3, 0x15, 0xde, 0x8f, 0x91, 0xef, 0xc6, 0xb8, 0x81, 0x97, 0xe3, 0xdf, 0x4d,
  0x88, 0xbf, 0xe4, 0x05, 0x25, 0x73, 0x1e, 0xd0, 0xcf, 0x1e, 0xeb, 0x4d, 0x18, 0x4e, 0x6f, 0x9f,
  0x00, 0x72, 0xc3, 0x74, 0xbe, 0x02, 0x09, 0x0a, 0xb0, 0xb1, 0x8e, 0x9b, 0x08, 0xed, 0x68, 0x6d,
  0x25, 0xe8, 0x28, 0x94, 0xa6, 0x44, 0xa6, 0xfa, 0x95, 0x69, 0x72, 0xd3, 0x6d, 0xb6, 0xff, 0xf3,
  0x45, 0x4e, 0xa3, 0x60, 0xf2, 0x58, 0xe7, 0x59, 0xe4, 0x4f, 0x70, 0xd2, 0xdd, 0xc0, 0x6e, 0xf3,
  0xd7, 0xb2, 0xdc, 0x1e, 0xa8, 0x41, 0x07, 0x5d, 0x60, 0x15, 0xea, 0xcf, 0xdb, 0xc1, 0x1d, 0x4d,
  0xb7, 0x42, 0xec, 0xc4, 0xca, 0xa9, 0x40, 0x30, 0x0f, 0x3c, 0xe2, 0x81, 0xe0, 0x5c, 0x51, 0x07,
  0xb0, 0x1e, 0x4a, 0xb3, 0x64, 0x3e, 0x1c, 0x62, 0x17, 0xcd, 0xf2, 0xe4, 0x14, 0x9d, 0xa6, 0xd4,
  0x64, 0x36, 0xa5, 0xe8, 0x7e, 0x84, 0x0e, 0xb3, 0x5d, 0x79, 0x57, 0xea, 0xd7, 0xad, 0xbc, 0x9e,
  0x2d, 0x90, 0x03, 0x9e, 0x0e, 0xc6, 0x98, 0xdb, 0xe3, 0xb6, 0x9f, 0x9b, 0xf6, 0x21, 0xe6, 0x98,
  0x94, 0x77, 0xb7, 0x2b, 0xaa, 0xc9, 0xff, 0xef, 0x7a, 0xf2, 0x71, 0x4e, 0x52, 0x06, 0x85, 0x37,
  0x81, 0x8e, 0x86, 0x64, 0x39, 0x92, 0x2a, 0xca, 0xf3, 0x3e, 0x87, 0xb5, 0x0c, 0x7b, 0x42, 0x5e,
  0x04, 0xa7, 0xfb, 0xd7, 0x13, 0x7f, 0x83, 0x6a, 0x77, 0x0f, 0xa7, 0x34, 0x51, 0x88, 0x9c, 0xac,
  0x23, 0x90, 0x4d, 0x4d, 0x72, 0x4e, 0xa3, 0x26, 0x1a, 0x45, 0x61, 0x0e, 0x10, 0x24, 0x8a, 0x27,
  0x92, 0x14, 0x23, 0xae, 0x4b, 0x80, 0xae, 0x6a, 0x56, 0x01, 0xac, 0x55, 0xf7, 0x6d, 0x9b, 0x6d,
};

void pgm_djlzz_decrypt(running_machine *machine)
{

	int i;
	UINT16 *src = (UINT16 *) (memory_region(machine, "maincpu")+0x100000);

	int rom_size = 0x400000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

	    if((i & 0x40080) != 0x00080)
	      x ^= 0x0001;

	    if((i & 0x84008) == 0x84008)
	      x ^= 0x0002;

	    if((i & 0x00030) == 0x00010)
	      x ^= 0x0004;

	    if((i & 0x00242) != 0x00042)
	      x ^= 0x0008;

	    if((i & 0x48100) == 0x48000)
	      x ^= 0x0010;

	    if((i & 0x02004) != 0x00004)
	      x ^= 0x0020;

	    if((i & 0x01800) != 0x00000)
	      x ^= 0x0040;

	    if((i & 0x04820) == 0x04820)
	      x ^= 0x0080;

	    x ^= djlzz_tab[i & 0xff] << 8;

		src[i] = x;
	}
}

static const UINT8 pstar[256] = {
  0x62, 0x59, 0x17, 0xe3, 0xe1, 0x11, 0x02, 0x97, 0x67, 0x4d, 0x4a, 0x1c, 0x1f, 0x76, 0x64, 0xc1,
  0xfa, 0x04, 0xd2, 0x9f, 0x22, 0xf5, 0xfd, 0xfe, 0x89, 0xeb, 0xab, 0x41, 0xc0, 0xac, 0x2b, 0x64,
  0xfe, 0x1e, 0x9b, 0x68, 0x07, 0xfd, 0x75, 0x25, 0x24, 0xa0, 0x41, 0x5d, 0x79, 0xb5, 0x67, 0x93,
  0xe1, 0xa3, 0x3a, 0x24, 0xa0, 0xbf, 0x64, 0x62, 0x38, 0x65, 0x75, 0x78, 0x8a, 0xf9, 0x7c, 0x00,
  0x71, 0x38, 0xc5, 0xe4, 0xdc, 0xf8, 0xc5, 0xc5, 0x8d, 0x1d, 0x6b, 0xb8, 0x84, 0x4e, 0x21, 0x6d,
  0x55, 0x95, 0x31, 0x86, 0x11, 0xe6, 0xb1, 0x77, 0x77, 0x3b, 0x01, 0x82, 0xab, 0x88, 0xef, 0x77,
  0x08, 0xd8, 0x80, 0xba, 0x41, 0xfb, 0xb3, 0x30, 0x5f, 0xf2, 0x5b, 0xe6, 0x5a, 0x63, 0xb0, 0xa4,
  0x37, 0x37, 0x6d, 0x1f, 0x27, 0x0e, 0x80, 0x8e, 0x1a, 0x45, 0x97, 0xdb, 0x61, 0x10, 0x24, 0x8a,
  0x62, 0x44, 0xf2, 0x91, 0x74, 0x25, 0x3e, 0xc5, 0xe6, 0x8e, 0x55, 0x44, 0x5a, 0x8a, 0x90, 0x25,
  0xa1, 0x0d, 0x43, 0x35, 0x46, 0x80, 0xfe, 0xb3, 0x4e, 0x4c, 0x21, 0xcc, 0x99, 0xc2, 0x3d, 0xce,
  0x19, 0xc5, 0x2c, 0xae, 0xe8, 0x94, 0xad, 0xed, 0x36, 0xb5, 0x8e, 0xf1, 0x81, 0xfe, 0x62, 0x3a,
  0x8c, 0xe0, 0x90, 0x88, 0xf1, 0x31, 0xc5, 0xd9, 0x3c, 0xf4, 0x70, 0x7d, 0x7d, 0x1c, 0xca, 0xe2,
  0x51, 0xce, 0xa0, 0x33, 0x42, 0x1e, 0xf7, 0xe4, 0x13, 0xcf, 0xa0, 0x95, 0x33, 0x59, 0x5b, 0xec,
  0xf7, 0x6d, 0x56, 0xac, 0x6a, 0xae, 0x55, 0x9b, 0x92, 0x14, 0x6d, 0x01, 0x23, 0x4b, 0x80, 0xae,
  0x65, 0x15, 0xc9, 0x84, 0x66, 0xd7, 0xbd, 0x16, 0xd2, 0xe7, 0x15, 0x95, 0x16, 0x9c, 0xd3, 0xd2,
  0x62, 0x51, 0xf6, 0xe3, 0xa6, 0xea, 0x32, 0x86, 0x67, 0xcc, 0x4a, 0x46, 0x4d, 0x37, 0x45, 0xd5,
};

void pgm_pstar_decrypt(running_machine *machine)
{

	int i;
	UINT16 *src = (UINT16 *) (memory_region(machine, "maincpu")+0x100000);

	int rom_size = 0x100000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if((i & 0x40480) != 0x00080)
			x ^= 0x0100;

		if((i & 0x00030) == 0x00010)
			x ^= 0x0400;

		if((i & 0x00242) != 0x00042)
			x ^= 0x0800;

		if((i & 0x08100) == 0x08000)
			x ^= 0x1000;

		if((i & 0x22004) != 0x00004)
			x ^= 0x2000;

		if((i & 0x11800) != 0x10000)
			x ^= 0x4000;

		if((i & 0x04820) == 0x04820)
			x ^= 0x8000;

		x = x ^ pstar[i & 255];
		src[i] = (x<<8)|(x>>8) ;
	}
}

void pgm_dw3_decrypt(running_machine *machine)
{

//  int i;
//  UINT16 *src=(UINT16 *) (OP_ROM+0x100000);

	int i;
	UINT16 *src = (UINT16 *) (memory_region(machine, "maincpu")+0x100000);

	int rom_size = 0x100000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if((i & 0x005460) == 0x001400)
			x ^= 0x0100;

		if((i & 0x005450) == 0x001040)
			x ^= 0x0100;

		if((i & 0x005e00) == 0x001c00)
			x ^= 0x40;

		if((i & 0x005580) == 0x001100)
			x ^= 0x40;



		src[i] = x;
	}
}

void pgm_killbld_decrypt(running_machine *machine)
{

//  int i;
//  UINT16 *src=(UINT16 *) (OP_ROM+0x100000);

	int i;
	UINT16 *src = (UINT16 *) (memory_region(machine, "maincpu")+0x100000);

	int rom_size = 0x200000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if((i & 0x6d00) == 0x0400 || (i & 0x6c80) == 0x0880)
			x ^= 0x0008;
		if((i & 0x7500) == 0x2400 || (i & 0x7600) == 0x3200)
			x ^= 0x1000;

		src[i] = x;
	}
}


static const UINT8 dfront_tab[256] = {
  0x51, 0xc4, 0xe3, 0x10, 0x1c, 0xad, 0x8a, 0x39, 0x8c, 0xe0, 0xa5, 0x04, 0x0f, 0xe4, 0x35, 0xc3,
  0x2d, 0x6b, 0x32, 0xe2, 0x60, 0x54, 0x63, 0x06, 0xa3, 0xf1, 0x0b, 0x5f, 0x6c, 0x5c, 0xb3, 0xec,
  0x77, 0x61, 0x69, 0xe7, 0x3c, 0xb7, 0x42, 0x72, 0x1a, 0x70, 0xb0, 0x96, 0xa4, 0x28, 0xc0, 0xfb,
  0x0a, 0x00, 0xcb, 0x15, 0x49, 0x48, 0xd3, 0x94, 0x58, 0xcf, 0x41, 0x86, 0x17, 0x71, 0xb1, 0xbd,
  0x21, 0x01, 0x37, 0x1e, 0xba, 0xeb, 0xf3, 0x59, 0xf6, 0xa7, 0x29, 0x4f, 0xb5, 0xca, 0x4c, 0x34,
  0x20, 0xa2, 0x62, 0x4b, 0x93, 0x9e, 0x47, 0x9f, 0x8d, 0x0e, 0x1b, 0xb6, 0x4d, 0x82, 0xd5, 0xf4,
  0x85, 0x79, 0x53, 0x92, 0x9b, 0xf7, 0xea, 0x44, 0x76, 0x1f, 0x22, 0x45, 0xed, 0xbe, 0x11, 0x55,
  0xaf, 0xf5, 0xf8, 0x50, 0x07, 0xe6, 0xc7, 0x5e, 0xd7, 0xde, 0xe5, 0x26, 0x2b, 0xf2, 0x6a, 0x8b,
  0xb8, 0x98, 0x89, 0xdb, 0x14, 0x5b, 0xc5, 0x78, 0xdc, 0xd0, 0x87, 0x5d, 0xc1, 0x0d, 0x95, 0x97,
  0x7e, 0xa8, 0x24, 0x3d, 0xe1, 0xd1, 0x19, 0xa6, 0x99, 0xd8, 0x83, 0x1d, 0xff, 0x30, 0x9d, 0x05,
  0xd4, 0x02, 0x27, 0x7b, 0x13, 0xb2, 0x7f, 0x40, 0x12, 0xa0, 0x68, 0x67, 0x4e, 0x3a, 0x46, 0xb9,
  0xee, 0xdf, 0x66, 0xd6, 0x8f, 0xa9, 0x0c, 0x91, 0x65, 0x18, 0x52, 0x56, 0xd9, 0x74, 0x09, 0x6e,
  0xc6, 0x73, 0xc9, 0xfc, 0x03, 0x43, 0xef, 0xaa, 0x7c, 0xbb, 0x2c, 0x90, 0xcc, 0xce, 0xe8, 0xae,
  0x2a, 0xf9, 0x57, 0x88, 0xc8, 0xe9, 0x5a, 0xdd, 0x2e, 0x7d, 0x64, 0xc2, 0x6d, 0x3e, 0xfa, 0x80,
  0x16, 0xcd, 0x6f, 0x84, 0x8e, 0x9c, 0xf0, 0xac, 0xb4, 0x9a, 0x2f, 0xbc, 0x31, 0x23, 0xfe, 0x38,
  0x08, 0x75, 0xa1, 0x33, 0xab, 0xd2, 0xda, 0x81, 0xbf, 0x7a, 0x3b, 0x3f, 0x4a, 0xfd, 0x25, 0x36,
};

void pgm_dfront_decrypt(running_machine *machine)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(machine, "user1"));

	int rom_size = 0x200000;

	for(i=0; i<rom_size/2; i++) {
    	UINT16 x = src[i];

		if((i & 0x040080) != 0x000080)
			x ^= 0x0001;

		if((i & 0x104008) == 0x104008)
			x ^= 0x0002;

		if((i & 0x080030) == 0x080010)
			x ^= 0x0004;

		if((i & 0x000042) != 0x000042)
			x ^= 0x0008;

		if((i & 0x008100) == 0x008000)
			x ^= 0x0010;

		if((i & 0x002004) != 0x000004)
			x ^= 0x0020;

		if((i & 0x011800) != 0x010000)
			x ^= 0x0040;

		if((i & 0x004820) == 0x004820)
			x ^= 0x0080;

		x ^= dfront_tab[(i>> 1) & 0xff] << 8;

		src[i] = x;
	}
}


static const UINT8 ddp2_tab[256] = {
  0x2a, 0x4a, 0x39, 0x98, 0xac, 0x39, 0xb2, 0x55, 0x72, 0xf3, 0x7b, 0x3c, 0xee, 0x94, 0x6e, 0xd5,
  0xcd, 0xbc, 0x9a, 0xd0, 0x45, 0x7d, 0x49, 0x68, 0xb1, 0x61, 0x54, 0xef, 0xa2, 0x84, 0x29, 0x20,
  0x32, 0x52, 0x82, 0x04, 0x38, 0x69, 0x9f, 0x24, 0x46, 0xf4, 0x3f, 0xc2, 0xf1, 0x25, 0xac, 0x2d,
  0xdf, 0x2d, 0xb4, 0x51, 0xc7, 0xb5, 0xe5, 0x88, 0xbd, 0x3b, 0x5a, 0x25, 0x5b, 0xc7, 0xae, 0x5f,
  0x43, 0xcf, 0x89, 0xd9, 0xe2, 0x63, 0xc6, 0x76, 0x21, 0x2b, 0x77, 0xc0, 0x27, 0x98, 0xfd, 0x09,
  0xe1, 0x8c, 0x26, 0x2e, 0x92, 0x99, 0xbc, 0xbe, 0x0e, 0xba, 0xbf, 0x70, 0xe7, 0xb7, 0xe9, 0x37,
  0x5c, 0xd1, 0x5e, 0xad, 0x22, 0x17, 0xc5, 0x67, 0x9d, 0xc6, 0xfb, 0x53, 0xc7, 0x4d, 0x32, 0xb4,
  0xf2, 0x43, 0x53, 0x7c, 0x01, 0xfe, 0xd2, 0x91, 0x40, 0x85, 0xa3, 0xe8, 0xdf, 0xdb, 0xff, 0x6c,
  0x64, 0x15, 0xcd, 0x8e, 0x07, 0x82, 0x78, 0x8d, 0x4e, 0x2d, 0x66, 0x8a, 0x62, 0x6f, 0xd3, 0x6a,
  0xae, 0x16, 0x44, 0x1e, 0xed, 0xc4, 0x12, 0x7a, 0xbe, 0x05, 0x06, 0xce, 0x9b, 0x8a, 0xf7, 0xf8,
  0x74, 0x23, 0x73, 0x74, 0xb8, 0x13, 0xc2, 0x42, 0xea, 0xf9, 0x7f, 0xa9, 0xaf, 0x56, 0xd6, 0xb3,
  0xb7, 0xc4, 0x47, 0x31, 0x67, 0xaa, 0x58, 0x8b, 0x47, 0x1b, 0xf5, 0x75, 0x95, 0x8f, 0xf0, 0x3a,
  0x85, 0x76, 0x59, 0x24, 0x0c, 0xd7, 0x00, 0xb3, 0xdc, 0xfc, 0x65, 0x34, 0xde, 0xfa, 0xd8, 0xc3,
  0xc3, 0x5e, 0xe3, 0x9e, 0x02, 0x28, 0x50, 0x81, 0x95, 0x2f, 0xe4, 0xb5, 0xa0, 0x4d, 0xa1, 0x36,
  0x9d, 0x18, 0x6d, 0x79, 0x19, 0x3b, 0x1d, 0xb8, 0xe1, 0xcc, 0x61, 0x1a, 0xe2, 0x31, 0x4c, 0x3f,
  0xdc, 0xca, 0xd4, 0xda, 0xcd, 0xd2, 0x83, 0xca, 0xeb, 0x4f, 0xf2, 0x2f, 0x2d, 0x2a, 0xec, 0x1f
};

void pgm_ddp2_decrypt(running_machine *machine)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(machine, "user1"));

	int rom_size = 0x200000;

	for(i=0; i<rom_size/2; i++) {
    	UINT16 x = src[i];

		if((i & 0x0480) != 0x0080)
			x ^= 0x0001;

		if((i & 0x0042) != 0x0042)
			x ^= 0x0008;

		if((i & 0x8100) == 0x8000)
			x ^= 0x0010;

		if((i & 0x2004) != 0x0004)
			x ^= 0x0020;

		if((i & 0x1800) != 0x0000)
			x ^= 0x0040;

		if((i & 0x0820) == 0x0820)
			x ^= 0x0080;

		x ^= ddp2_tab[(i>> 1) & 0xff] << 8;

		src[i] = x;
	}
}


static const UINT8 mm_tab[256] = {
  0xd0, 0x45, 0xbc, 0x84, 0x93, 0x60, 0x7d, 0x49, 0x68, 0xb1, 0x54, 0xa2, 0x05, 0x29, 0x41, 0x20,
  0x04, 0x08, 0x52, 0x25, 0x89, 0xf4, 0x69, 0x9f, 0x24, 0x46, 0x3d, 0xf1, 0xf9, 0xab, 0xa6, 0x2d,
  0x18, 0x19, 0x6d, 0x33, 0x79, 0x23, 0x3b, 0x1d, 0xe0, 0xb8, 0x61, 0x1a, 0xe1, 0x4c, 0x5d, 0x3f,
  0x5e, 0x02, 0xe3, 0x4d, 0x9e, 0x80, 0x28, 0x50, 0xa0, 0x81, 0xe4, 0xa5, 0x97, 0xa1, 0x86, 0x36,
  0x1e, 0xed, 0x16, 0x8a, 0x44, 0x06, 0x64, 0x12, 0x9a, 0x7e, 0xce, 0x9b, 0xef, 0xf7, 0x3e, 0xf8,
  0x15, 0x07, 0xcb, 0x6f, 0x8e, 0x3c, 0x82, 0x70, 0x62, 0x8d, 0x66, 0x7a, 0x4e, 0xd3, 0xb6, 0x6a,
  0x51, 0xa7, 0x2c, 0xc7, 0xa4, 0x0b, 0xb5, 0xe5, 0x88, 0xbd, 0x5a, 0x5b, 0x1b, 0xae, 0xe6, 0x5f,
  0x2e, 0x92, 0x8c, 0xb7, 0x96, 0xba, 0x99, 0xbb, 0xbe, 0x0e, 0xbf, 0xe7, 0x2f, 0xe9, 0x30, 0x37,
  0x98, 0xac, 0x4a, 0x94, 0x38, 0xf3, 0x39, 0xb2, 0x55, 0x72, 0x7b, 0xee, 0xdd, 0x6e, 0x11, 0xd5,
  0x26, 0xa8, 0x71, 0xd6, 0x74, 0x7f, 0x13, 0xc2, 0x56, 0xea, 0xa9, 0xaf, 0xc3, 0x42, 0x03, 0xb3,
  0xc4, 0x6b, 0x47, 0xf0, 0x31, 0xf5, 0xaa, 0x58, 0x8f, 0x48, 0x75, 0x95, 0x35, 0x8b, 0x57, 0x3a,
  0x73, 0x0c, 0x59, 0xd8, 0x14, 0x65, 0xd7, 0x00, 0xfa, 0xdc, 0x34, 0xde, 0xc0, 0xb0, 0x87, 0xc1,
  0xc8, 0xcd, 0xd4, 0x2a, 0xda, 0xe8, 0xd2, 0x83, 0x0d, 0xca, 0xf2, 0x0f, 0xeb, 0xec, 0x9c, 0x1f,
  0xad, 0x22, 0xd1, 0x4b, 0x5c, 0xf6, 0x17, 0xc5, 0x67, 0x9d, 0xfb, 0xc9, 0xcc, 0x32, 0x1c, 0xb4,
  0xd9, 0xe2, 0xcf, 0x90, 0xb9, 0x2b, 0x63, 0xc6, 0x76, 0x21, 0x77, 0x27, 0xfc, 0xfd, 0x0a, 0x09,
  0x7c, 0x01, 0x43, 0xdb, 0x53, 0x85, 0xfe, 0x78, 0x91, 0x40, 0xa3, 0xdf, 0x4f, 0xff, 0x10, 0x6c,
};

void pgm_mm_decrypt(running_machine *machine)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(machine, "user1"));

	int rom_size = 0x200000;

	for(i=0; i<rom_size/2; i++) {
    	UINT16 x = src[i];

		if((i & 0x040480) != 0x000080)
			x ^= 0x0001;

		if((i & 0x004008) == 0x004008)
			x ^= 0x0002;

		if((i & 0x000030) == 0x000010)
			x ^= 0x0004;

		if((i & 0x000242) != 0x000042)
			x ^= 0x0008;

		if((i & 0x008100) == 0x008000)
			x ^= 0x0010;

		if((i & 0x022004) != 0x000004)
			x ^= 0x0020;

		if((i & 0x011800) != 0x010000)
			x ^= 0x0040;

		if((i & 0x000820) == 0x000820)
			x ^= 0x0080;

		x ^= mm_tab[(i>> 1) & 0xff] << 8;

		src[i] = x;
	}
}

static const UINT8 kov2_tab[256] = {
 0x11, 0x4a, 0x38, 0x98, 0xac, 0x39, 0xb2, 0x55, 0x72, 0xf3, 0x7b, 0x3c, 0xee, 0x94, 0x6e, 0xd5,
 0x41, 0xbc, 0x93, 0xd0, 0x45, 0x7d, 0x49, 0x68, 0xb1, 0x60, 0x54, 0xef, 0xa2, 0x84, 0x29, 0x20,
 0xa6, 0x52, 0x89, 0x04, 0x08, 0x69, 0x9f, 0x24, 0x46, 0xf4, 0x3d, 0xc3, 0xf1, 0x25, 0xab, 0x2d,
 0xe6, 0x2c, 0xa4, 0x51, 0xa7, 0xb5, 0xe5, 0x88, 0xbd, 0x0b, 0x5a, 0x35, 0x5b, 0xc7, 0xae, 0x5f,
 0x0a, 0xcf, 0xb9, 0xd9, 0xe2, 0x63, 0xc6, 0x76, 0x21, 0x2b, 0x77, 0xc0, 0x27, 0x90, 0xfd, 0x09,
 0x30, 0x8c, 0x96, 0x2e, 0x92, 0x99, 0xbb, 0xbe, 0x0e, 0xba, 0xbf, 0x80, 0xe7, 0xb7, 0xe9, 0x37,
 0x1c, 0xd1, 0x5c, 0xad, 0x22, 0x17, 0xc5, 0x67, 0x9d, 0xf6, 0xfb, 0x23, 0xc9, 0x4b, 0x32, 0xb4,
 0x10, 0x43, 0x53, 0x7c, 0x01, 0xfe, 0x78, 0x91, 0x40, 0x85, 0xa3, 0xe8, 0xdf, 0xdb, 0xff, 0x6c,
 0xb6, 0x15, 0xcb, 0x8e, 0x07, 0x82, 0x70, 0x8d, 0x4e, 0xdd, 0x66, 0x7a, 0x62, 0x6f, 0xd3, 0x6a,
 0x3e, 0x16, 0x44, 0x1e, 0xed, 0x64, 0x12, 0x9a, 0x7e, 0x05, 0x06, 0xce, 0x9b, 0x8a, 0xf7, 0xf8,
 0x03, 0x26, 0x71, 0x74, 0xa8, 0x13, 0xc2, 0x42, 0xea, 0xf9, 0x7f, 0xa9, 0xaf, 0x56, 0xd6, 0xb3,
 0x57, 0xc4, 0x47, 0x31, 0x6b, 0xaa, 0x58, 0x8b, 0x48, 0x1b, 0xf5, 0x75, 0x95, 0x8f, 0xf0, 0x3a,
 0x87, 0x73, 0x59, 0x14, 0x0c, 0xd7, 0x00, 0xb0, 0xdc, 0xfc, 0x65, 0x34, 0xde, 0xfa, 0xd8, 0xc1,
 0x86, 0x5e, 0xe3, 0x9e, 0x02, 0x28, 0x50, 0x81, 0x97, 0x2f, 0xe4, 0xa5, 0xa0, 0x4d, 0xa1, 0x36,
 0x5d, 0x18, 0x6d, 0x79, 0x19, 0x3b, 0x1d, 0xb8, 0xe1, 0xcc, 0x61, 0x1a, 0xe0, 0x33, 0x4c, 0x3f,
 0x9c, 0xc8, 0xd4, 0xda, 0xcd, 0xd2, 0x83, 0xca, 0xeb, 0x4f, 0xf2, 0x0f, 0x0d, 0x2a, 0xec, 0x1f,
};

void pgm_kov2_decrypt(running_machine *machine)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(machine, "user1"));

	int rom_size = 0x200000;

	for(i=0; i<rom_size/2; i++) {
    	UINT16 x = src[i];

    	if((i & 0x40080) != 0x00080)
    	  x ^= 0x0001;

    	if((i & 0x80030) == 0x80010)
    	  x ^= 0x0004;

    	if((i & 0x00042) != 0x00042)
    	  x ^= 0x0008;

    	if((i & 0x48100) == 0x48000)
    	  x ^= 0x0010;

    	if((i & 0x22004) != 0x00004)
    	  x ^= 0x0020;

    	if((i & 0x01800) != 0x00000)
    	  x ^= 0x0040;

    	if((i & 0x00820) == 0x00820)
    	  x ^= 0x0080;

    	x ^= kov2_tab[(i >> 1) & 0xff] << 8;

    	src[i] = x;
	}
}

static const UINT8 kov2p_tab[256] = {
 0x44, 0x47, 0xb8, 0x28, 0x03, 0xa2, 0x21, 0xbc, 0x17, 0x32, 0x4e, 0xe2, 0xdf, 0x69, 0x35, 0xc7,
 0xa2, 0x06, 0xec, 0x36, 0xd2, 0x44, 0x12, 0x6a, 0x8d, 0x51, 0x6b, 0x20, 0x69, 0x01, 0xca, 0xf0,
 0x71, 0xc4, 0x34, 0xdc, 0x6b, 0xd6, 0x42, 0x2a, 0x5d, 0xb5, 0xc7, 0x6f, 0x4f, 0xd8, 0xb3, 0xed,
 0x51, 0x9e, 0x37, 0x1e, 0xc0, 0x85, 0x2a, 0x91, 0xc6, 0x9c, 0xac, 0xf5, 0x20, 0x3b, 0x09, 0x74,
 0x24, 0xf1, 0xe0, 0x42, 0x02, 0xbe, 0x84, 0x75, 0x4a, 0x82, 0xa2, 0x17, 0xae, 0xb6, 0x24, 0x79,
 0x0a, 0x5a, 0x56, 0xcb, 0xa1, 0x2e, 0x47, 0xea, 0xa9, 0x25, 0x73, 0x79, 0x0b, 0x17, 0x9e, 0x33,
 0x64, 0xb6, 0x03, 0x7f, 0x4f, 0xc3, 0xae, 0x45, 0xe6, 0x82, 0x27, 0x01, 0x86, 0x6b, 0x50, 0x16,
 0xd3, 0x22, 0x90, 0x64, 0xfc, 0xa9, 0x31, 0x1c, 0x41, 0xd5, 0x07, 0xd3, 0xb2, 0xfe, 0x53, 0xd6,
 0x39, 0xfb, 0xe6, 0xbe, 0xda, 0x4d, 0x8a, 0x44, 0x3a, 0x9b, 0x9d, 0x56, 0x5e, 0x5f, 0xff, 0x6a,
 0xb6, 0xde, 0x2f, 0x12, 0x5a, 0x5d, 0xb0, 0xd0, 0x93, 0x92, 0xb2, 0x2c, 0x9d, 0x59, 0xee, 0x05,
 0xab, 0xa8, 0xd2, 0x25, 0x2c, 0xc5, 0xde, 0x18, 0x4d, 0xb6, 0x4e, 0x3d, 0xbf, 0xfa, 0xf9, 0x1d,
 0xba, 0x76, 0x79, 0xfc, 0x42, 0xb2, 0x8c, 0xae, 0xa9, 0x45, 0xba, 0xac, 0x55, 0x8e, 0x38, 0x67,
 0xc3, 0xa5, 0x0d, 0xdc, 0xcc, 0x91, 0x73, 0x69, 0x27, 0xbc, 0x80, 0xdf, 0x30, 0xa4, 0x05, 0xd8,
 0xe7, 0xd2, 0xb7, 0x4b, 0x3c, 0x10, 0x8c, 0x5d, 0x8a, 0xd7, 0x68, 0x7a, 0x61, 0x07, 0xf9, 0xa5,
 0x88, 0xda, 0xdf, 0x0c, 0x42, 0x1b, 0x11, 0xe0, 0xd1, 0x93, 0x7c, 0x63, 0x39, 0xc5, 0xed, 0x43,
 0x46, 0xdb, 0x30, 0x26, 0xd0, 0xdf, 0x7a, 0x86, 0x3e, 0x2e, 0x04, 0xbf, 0x49, 0x2a, 0xf9, 0x66,
};

void pgm_kov2p_decrypt(running_machine *machine)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(machine, "user1"));

	int rom_size = 0x200000;

	for(i=0; i<rom_size/2; i++) {
    	UINT16 x = src[i];

    	if((i & 0x40080) != 0x00080)
    	  x ^= 0x0001;

    	if((i & 0x04008) == 0x04008)
    	  x ^= 0x0002;

    	if((i & 0x80030) == 0x80010)
    	  x ^= 0x0004;

    	if((i & 0x00242) != 0x00042)
    	  x ^= 0x0008;

    	if((i & 0x08100) == 0x08000)
    	  x ^= 0x0010;

    	if((i & 0x02004) != 0x00004)
    	  x ^= 0x0020;

    	if((i & 0x11800) != 0x10000)
    	  x ^= 0x0040;

    	if((i & 0x00820) == 0x00820)
    	  x ^= 0x0080;

    	x ^= kov2p_tab[(i >> 1) & 0xff] << 8;

    	src[i] = x;
	}
}

static const UINT8 puzzli2_tab[256] = {
  0xb7, 0x66, 0xa3, 0xc0, 0x51, 0x55, 0x6d, 0x63, 0x86, 0x60, 0x64, 0x6c, 0x67, 0x18, 0x0b, 0x05,
  0x62, 0xff, 0xe0, 0x1e, 0x30, 0x21, 0x2e, 0x40, 0x41, 0xb9, 0x60, 0x38, 0xd1, 0x24, 0x7e, 0x36,
  0x7a, 0x0b, 0x1c, 0x69, 0x4f, 0x09, 0xe1, 0x9e, 0xcf, 0xcd, 0x7c, 0x00, 0x73, 0x08, 0x77, 0x37,
  0x5f, 0x50, 0x32, 0x3e, 0xd3, 0x54, 0x77, 0x6b, 0x60, 0x60, 0x74, 0x7c, 0x55, 0x4f, 0x44, 0x5e,
  0x66, 0x5c, 0x58, 0x26, 0x35, 0x29, 0x3f, 0x35, 0x3f, 0x1c, 0x0b, 0x0d, 0x08, 0x5b, 0x59, 0x5c,
  0xa0, 0xa5, 0x87, 0x85, 0x24, 0x75, 0x5f, 0x42, 0x1b, 0xf3, 0x1a, 0x58, 0x17, 0x58, 0x71, 0x6b,
  0x69, 0x89, 0x7d, 0x3a, 0xf3, 0xc4, 0x5d, 0xa0, 0x4f, 0x27, 0x58, 0xc4, 0xa8, 0xdd, 0xa8, 0xfb,
  0xbe, 0xa4, 0xe2, 0xee, 0x07, 0x10, 0x90, 0x72, 0x99, 0x08, 0x68, 0x6d, 0x5c, 0x5c, 0x6d, 0x58,
  0x2f, 0xdc, 0x15, 0xd5, 0xd6, 0xd6, 0x3b, 0x3b, 0xf9, 0x32, 0xcc, 0xdd, 0xd4, 0xf1, 0xea, 0xed,
  0xe4, 0xf6, 0xf2, 0x91, 0xca, 0xc1, 0xed, 0xf2, 0xf6, 0xfb, 0xc0, 0xe8, 0xe3, 0xe7, 0xfa, 0xf1,
  0xf5, 0x08, 0x26, 0x2b, 0x2f, 0x34, 0x39, 0x13, 0x28, 0x07, 0x88, 0x5b, 0x8f, 0x94, 0x9b, 0x2e,
  0xf5, 0xab, 0x72, 0x76, 0x7a, 0x40, 0xb9, 0x09, 0xd8, 0x3b, 0xcd, 0x31, 0x3d, 0x42, 0xab, 0xb1,
  0xb5, 0xb9, 0x3b, 0xe3, 0x0b, 0x65, 0x18, 0xfb, 0x1f, 0x12, 0xe4, 0xe8, 0xec, 0xf2, 0xf7, 0xfc,
  0xc0, 0xe8, 0xe0, 0xe6, 0xfa, 0xf1, 0xf4, 0x0b, 0x26, 0x2b, 0x30, 0x35, 0x39, 0x13, 0x29, 0x21,
  0x0c, 0x11, 0x16, 0x1b, 0x1f, 0x64, 0x0e, 0x60, 0x05, 0x79, 0x7c, 0x37, 0x00, 0x0f, 0x4f, 0x38,
  0x1d, 0x18, 0xa2, 0xb6, 0xb2, 0xa9, 0xac, 0xab, 0xae, 0x91, 0x98, 0x8d, 0x91, 0xbb, 0xb1, 0xc0,
};

void pgm_puzzli2_decrypt(running_machine *machine)
{
	int i;
	UINT16 *src = (UINT16 *) (memory_region(machine, "maincpu")+0x100000);

	int rom_size = 0x100000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

    	if((i & 0x040080) != 0x000080)
    	  x ^= 0x0100;

    	if((i & 0x004008) == 0x004008)
    	  x ^= 0x0200;

    	if((i & 0x000030) == 0x000010)
    	  x ^= 0x0400;

    	if((i & 0x000242) != 0x000042)
    	  x ^= 0x0800;

    	if((i & 0x008100) == 0x008000)
    	  x ^= 0x1000;

    	if((i & 0x022004) != 0x000004)
    	  x ^= 0x2000;

    	if((i & 0x011800) != 0x010000)
    	  x ^= 0x4000;

    	if((i & 0x004820) == 0x004820)
    	  x ^= 0x8000;

    	x ^= puzzli2_tab[i & 0xff];
		src[i] = (x<<8)|(x>>8) ;
	}
}


static const UINT8 theglad_tab[256] = {
	0x49, 0x47, 0x53, 0x30, 0x30, 0x30, 0x35, 0x52, 0x44, 0x31, 0x30, 0x32, 0x31, 0x32, 0x30, 0x33, 
	0xC4, 0xA3, 0x46, 0x78, 0x30, 0xB3, 0x8B, 0xD5, 0x2F, 0xC4, 0x44, 0xBF, 0xDB, 0x76, 0xDB, 0xEA,
	0xB4, 0xEB, 0x95, 0x4D, 0x15, 0x21, 0x99, 0xA1, 0xD7, 0x8C, 0x40, 0x1D, 0x43, 0xF3, 0x9F, 0x71,
	0x3D, 0x8C, 0x52, 0x01, 0xAF, 0x5B, 0x8B, 0x63, 0x34, 0xC8, 0x5C, 0x1B, 0x06, 0x7F, 0x41, 0x96,
	0x2A, 0x8D, 0xF1, 0x64, 0xDA, 0xB8, 0x67, 0xBA, 0x33, 0x1F, 0x2B, 0x28, 0x20, 0x13, 0xE6, 0x96,
	0x86, 0x34, 0x25, 0x85, 0xB0, 0xD0, 0x6D, 0x85, 0xFE, 0x78, 0x81, 0xF1, 0xCA, 0xE4, 0xEF, 0xF2,
	0x9B, 0x09, 0xE1, 0xB4, 0x8D, 0x79, 0x22, 0xE2, 0x00, 0xFB, 0x6F, 0x68, 0x80, 0x6A, 0x00, 0x69,
	0xF5, 0xD3, 0x57, 0x7E, 0x0C, 0xCA, 0x48, 0x31, 0xE5, 0x0D, 0x4A, 0xB9, 0xFD, 0x5C, 0xFD, 0xF8,
	0x5F, 0x98, 0xFB, 0xB3, 0x07, 0x1A, 0xE3, 0x10, 0x96, 0x56, 0xA3, 0x56, 0x3D, 0xB1, 0x07, 0xE0,
	0xE3, 0x9F, 0x7F, 0x62, 0x99, 0x01, 0x35, 0x60, 0x40, 0xBE, 0x4F, 0xEB, 0x79, 0xA0, 0x82, 0x9F,
	0xCD, 0x71, 0xD8, 0xDA, 0x1E, 0x56, 0xC2, 0x3E, 0x4E, 0x6B, 0x60, 0x69, 0x2D, 0x9F, 0x10, 0xF4,
	0xA9, 0xD3, 0x36, 0xAA, 0x31, 0x2E, 0x4C, 0x0A, 0x69, 0xC3, 0x2A, 0xFF, 0x15, 0x67, 0x96, 0xDE,
	0x3F, 0xCC, 0x0F, 0xA1, 0xAC, 0xE2, 0xD6, 0x62, 0x7E, 0x6F, 0x3E, 0x1B, 0x2A, 0xED, 0x36, 0x9C,
	0x9D, 0xA4, 0x14, 0xCD, 0xAA, 0x08, 0xA4, 0x26, 0xB7, 0x55, 0x70, 0x6C, 0xA9, 0x69, 0x52, 0xAE,
	0x0C, 0xE1, 0x38, 0x7F, 0x87, 0x78, 0x38, 0x75, 0x80, 0x9C, 0xD4, 0xE2, 0x0B, 0x52, 0x8F, 0xD2,
	0x19, 0x4C, 0xB0, 0x45, 0xDE, 0x48, 0x55, 0xAE, 0x82, 0xAB, 0xBC, 0xAB, 0x0C, 0x5E, 0xCE, 0x07,
};

void pgm_theglad_decrypt(running_machine *machine)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(machine, "user1"));

	int rom_size = 0x800000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if((i & 0x40080) != 0x00080)
			x ^= 0x0001;

		if((i & 0x104008) == 0x104008)
			x ^= 0x0002;

		if((i & 0x80030) == 0x80010)
			x ^= 0x0004;

		if((i & 0x000042) != 0x000042)
			x ^= 0x0008;

		if((i & 0x08100) == 0x08000)
			x ^= 0x0010;

		if((i & 0x022004) != 0x000004)
			x ^= 0x0020;

		if((i & 0x11800) != 0x10000)
			x ^= 0x0040;

		if((i & 0x00820) == 0x00820)
			x ^= 0x0080;

		x ^= theglad_tab[(i >> 1) & 0xff] << 8;

		src[i] = x;
	}
}


void pgm_svg_decrypt(running_machine *machine)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(machine, "user1"));

	int rom_size = 0x800000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if((i & 0x040080) != 0x000080)
			x ^= 0x0001;

		if((i & 0x004008) == 0x004008)
			x ^= 0x0002;

		if((i & 0x080030) == 0x080010)
			x ^= 0x0004;

		if((i & 0x000042) != 0x000042)
			x ^= 0x0008;

		if((i & 0x048100) == 0x048000)
			x ^= 0x0010;

		if((i & 0x002004) != 0x000004)
			x ^= 0x0020;

		if((i & 0x011800) != 0x010000)
			x ^= 0x0040;

		if((i & 0x000820) == 0x000820)
			x ^= 0x0080;

		src[i] = x;
	}
}


static const UINT8 killbldp_tab[256] = {
	0x49,0x47,0x53,0x30,0x30,0x32,0x34,0x52,0x44,0x31,0x30,0x35,0x30,0x39,0x30,0x38,
	0x12,0xA0,0xD1,0x9E,0xB1,0x8A,0xFB,0x1F,0x50,0x51,0x4B,0x81,0x28,0xDA,0x5F,0x41,
	0x78,0x6C,0x7A,0xF0,0xCD,0x6B,0x69,0x14,0x94,0x55,0xB6,0x42,0xDF,0xFE,0x10,0x79,
	0x74,0x08,0xFA,0xC0,0x1C,0xA5,0xB4,0x03,0x2A,0x91,0x67,0x2B,0x49,0x4A,0x94,0x7D,
	0x8B,0x92,0xBE,0x35,0xAF,0x28,0x56,0x63,0xB3,0xC2,0xE8,0x06,0x9B,0x4E,0x85,0x66,
	0x7F,0x6B,0x70,0xB7,0xDB,0x22,0x0C,0xEB,0x13,0xE9,0x06,0xD7,0x45,0xDA,0xBE,0x8B,
	0x54,0x30,0xFC,0xEB,0x32,0x02,0xD0,0x92,0x6D,0x44,0xCA,0xE8,0xFD,0xFB,0x5B,0x81,
	0x4C,0xC0,0x8B,0xB9,0x87,0x78,0xDD,0x8E,0x24,0x52,0x80,0xBE,0xB4,0x01,0xB7,0x21,
	0xEB,0x3C,0x8A,0x49,0xED,0x73,0xAE,0x58,0xDB,0xD2,0xB2,0x21,0x9E,0x7C,0x6C,0x82,
	0xF3,0x01,0xA3,0x00,0xB7,0x21,0xFE,0xA5,0x75,0xC4,0x2D,0x17,0x2D,0x39,0x56,0xF9,
	0x67,0xAE,0xC2,0x87,0x79,0xF1,0xC8,0x6D,0x15,0x66,0xFA,0xE8,0x16,0x48,0x8F,0x1F,
	0x8B,0x24,0x10,0xC4,0x04,0x93,0x47,0xE6,0x1D,0x37,0x65,0x1A,0x49,0xF8,0x72,0xCB,
	0xE1,0x80,0xFA,0xDD,0x6D,0xF5,0xF6,0x89,0x32,0xF6,0xF8,0x75,0xFC,0xD8,0x9B,0x12,
	0x2D,0x22,0x2A,0x3B,0x06,0x46,0x90,0x0C,0x35,0xA2,0x80,0xFF,0xA0,0xB7,0xE5,0x4D,
	0x71,0xA9,0x8C,0x84,0x62,0xF7,0x10,0x65,0x4A,0x7B,0x06,0x00,0xE8,0xA4,0x6A,0x13,
	0xF0,0xF3,0x4A,0x9F,0x54,0xB4,0xB1,0xCC,0xD4,0xFF,0xD6,0xFF,0xC9,0xEE,0x86,0x39,
};

void pgm_killbldp_decrypt(running_machine *machine)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(machine, "user1"));

	int rom_size = 0x800000;

	for(i=0; i<rom_size/2; i++) {
		UINT16 x = src[i];

		if((i & 0x040480) != 0x000080)
			x ^= 0x0001;

		if((i & 0x104008) == 0x104008)
			x ^= 0x0002;

		if((i & 0x080030) == 0x080010)
			x ^= 0x0004;

		if((i & 0x000242) != 0x000042)
			x ^= 0x0008;

		if((i & 0x008100) == 0x008000)
			x ^= 0x0010;

		if((i & 0x002004) != 0x000004)
			x ^= 0x0020;

		if((i & 0x011800) != 0x010000)
			x ^= 0x0040;

		if((i & 0x000820) == 0x000820)
			x ^= 0x0080;

		x ^= killbldp_tab[(i >> 1) & 0xff] << 8;

		src[i] = x;
	}
}


static const UINT8 kovh_tab[256] = {
	0x49,0x47,0x53,0x30,0x30,0x30,0x39,0x72,0x64,0x31,0x30,0x34,0x30,0x32,0x31,0x39,
	0xF9,0x8C,0xBD,0x87,0x16,0x07,0x39,0xEB,0x29,0x9E,0x17,0xEF,0x4F,0x64,0x7C,0xE0,
	0x5F,0x73,0x5B,0xA1,0x5E,0x95,0x0D,0xF1,0x40,0x36,0x2F,0x00,0xE2,0x8A,0xBC,0x32,
	0x44,0xFA,0x6C,0x33,0x0B,0xD5,0x4C,0x3B,0x36,0x34,0x9E,0xA3,0x20,0x2E,0xF3,0xA9,
	0xB7,0x3E,0x87,0x80,0xFB,0xF1,0xDD,0x9C,0xBA,0xD3,0x9B,0x3B,0x8A,0x9C,0xA8,0x37,
	0x07,0x97,0x84,0x0C,0x4E,0x54,0xE7,0x25,0xBA,0x8E,0x9D,0x6B,0xDE,0x5F,0xA1,0x10,
	0xC3,0xA2,0x79,0x99,0x63,0xA9,0xD1,0x2A,0x65,0x20,0x5B,0x16,0x1B,0x41,0xE6,0xA7,
	0xBA,0x3A,0xBD,0x2A,0xD8,0xDB,0x43,0x3F,0x2B,0x85,0xCC,0x5F,0x80,0x4F,0xBE,0xAE,
	0xFA,0x79,0xE8,0x03,0x8D,0x16,0x22,0x35,0xBB,0xF6,0x26,0xA9,0x8D,0xD2,0xAF,0x19,
	0xD4,0xBB,0xD0,0xA6,0xA1,0xC4,0x96,0x21,0x02,0xEF,0xE1,0x96,0x00,0x56,0x80,0x1B,
	0xD6,0x9A,0x8C,0xD7,0x73,0x91,0x07,0x55,0x32,0x2B,0xB5,0x0B,0xD8,0xA5,0x39,0x26,
	0xCE,0xF2,0x74,0x98,0xA1,0x66,0x1A,0x64,0xB8,0xA5,0x96,0x29,0x54,0xCB,0x21,0xED,
	0xCD,0xDD,0x1E,0x2C,0x0B,0x70,0xB8,0x22,0x43,0x98,0xBE,0x54,0xF3,0x14,0xBE,0x65,
	0x21,0xB7,0x61,0x17,0xCF,0x19,0x07,0xA0,0xC2,0x7F,0xA3,0x30,0x75,0x08,0xD8,0xBF,
	0x58,0x1A,0x55,0x1B,0x4E,0x0D,0x6D,0x32,0x65,0x15,0xFB,0x9E,0xD8,0x75,0x76,0x6F,
	0x42,0xE2,0x4F,0x3C,0x25,0x35,0x93,0x6C,0x9B,0x56,0xBE,0xC1,0x5B,0x65,0xDE,0x27,
};

void pgm_kovh_decrypt(running_machine *machine)
{
	int i;
	unsigned short *src = (unsigned short *)(memory_region(machine, "maincpu")+0x100000);

	int rom_size = 0x400000;

	for(i=0; i<rom_size/2; i++) {
		unsigned short x = src[i];

		if((i & 0x040080) != 0x000080)
			x ^= 0x0001;

		if((i & 0x004008) == 0x004008 && (i & 0x180000) != 0x000000)
			x ^= 0x0002;

		if((i & 0x000030) == 0x000010)
			x ^= 0x0004;

		if((i & 0x000042) != 0x000042)
			x ^= 0x0008;

		if((i & 0x008100) == 0x008000)
			x ^= 0x0010;

		if((i & 0x022004) != 0x000004)
			x ^= 0x0020;

		if((i & 0x011800) != 0x010000)
			x ^= 0x0040;

		if((i & 0x000820) == 0x000820)
			x ^= 0x0080;

		x ^= kovh_tab[i & 0xff] << 8;

		src[i] = x;
	}
}


static const UINT8 oldss_tab[256] = {
	0x49,0x47,0x53,0x30,0x30,0x31,0x33,0x52,0x44,0x31,0x30,0x34,0x30,0x37,0x32,0x37,
	0xF5,0x79,0x6D,0xAB,0x04,0x22,0x51,0x96,0xF2,0x72,0xE8,0x3A,0x96,0xD2,0x9A,0xCC,
	0x3F,0x47,0x3C,0x09,0xF2,0xD9,0x72,0x41,0xE6,0x44,0x43,0xA7,0x3E,0xE2,0xFD,0xD8,
	0x06,0xD8,0x4C,0xA9,0x70,0x80,0x95,0x35,0x50,0x17,0x99,0x27,0xD5,0xA8,0x47,0x45,
	0x89,0x38,0xE1,0x3D,0x8C,0x33,0x53,0xB4,0x0D,0x17,0xD1,0x8D,0x09,0x5F,0xAF,0x76,
	0x48,0xB2,0x85,0xB9,0x95,0x4C,0x83,0x42,0x3D,0xAD,0x11,0xEC,0xCA,0x82,0xAC,0x10,
	0x01,0xD0,0xFD,0x50,0x19,0x67,0x3B,0xA0,0x3E,0x86,0xC2,0x97,0x46,0xCB,0xF4,0xF5,
	0xB3,0x5F,0x50,0x74,0xE9,0x5F,0xD2,0xD4,0xB0,0x8D,0x8A,0x21,0xED,0x37,0x80,0x47,
	0x9D,0x68,0xC7,0xD9,0x12,0x4E,0xDF,0x1E,0x72,0xEB,0x50,0x5E,0x6D,0x00,0x85,0x6B,
	0x3E,0x37,0xE6,0x72,0xE5,0x8F,0x3A,0x03,0xA3,0x0D,0x3B,0x5F,0xB6,0xA1,0x7B,0x02,
	0x56,0x56,0x77,0x71,0xEF,0xBE,0xF9,0x46,0xA1,0x9D,0xB3,0x79,0xF6,0xD5,0x19,0xF0,
	0xE2,0x91,0x7E,0x4A,0x01,0xB6,0x73,0xE8,0x0C,0x86,0x5D,0x3E,0x9C,0x97,0x55,0x58,
	0x23,0xF4,0x45,0xB0,0x28,0x91,0x40,0x2F,0xC2,0xF4,0x21,0x81,0x58,0x22,0x68,0x9D,
	0x97,0xC7,0x51,0x95,0xB4,0xAA,0x36,0x9B,0xE4,0x51,0x27,0x55,0x18,0xF0,0xC7,0x62,
	0xFE,0x98,0x6A,0x2D,0x35,0x9D,0x6C,0xF1,0xCF,0x48,0xD4,0x0D,0x0C,0xBE,0x2A,0x8A,
	0x55,0x31,0x96,0xEA,0x78,0x45,0x3A,0x33,0x23,0xC5,0xD1,0x3C,0xA3,0x86,0x88,0x38,
};

void pgm_oldss_decrypt(running_machine *machine)
{
	int i;
	unsigned short *src = (unsigned short *)(memory_region(machine, "maincpu")+0x100000);

	int rom_size = 0x400000;

	for(i=0; i<rom_size/2; i++) {
		unsigned short x = src[i];

		if((i & 0x040480) != 0x000080 ) 
			x ^= 0x0001;

		if((i & 0x004008) == 0x004008 )
			x ^= 0x0002;

		if((i & 0x000030) == 0x000010 )
			x ^= 0x0004;

		if((i & 0x000242) != 0x000042 )
			x ^= 0x0008;

		if((i & 0x048100) == 0x048000 )
			x ^= 0x0010;

		if((i & 0x002004) != 0x000004 )
			x ^= 0x0020;

		if((i & 0x011800) != 0x010000 )
			x ^= 0x0040;

		if((i & 0x000820) == 0x000820 )
			x ^= 0x0080;

		x ^= oldss_tab[i & 0xff] << 8;

		src[i] = x;
	}
}
