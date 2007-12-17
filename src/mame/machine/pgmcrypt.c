/* IGS PGM System Encryptions */

#include "driver.h"

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

void pgm_kov_decrypt(void)
{

	int i;
	UINT16 *src = (UINT16 *) (memory_region(REGION_CPU1)+0x100000);

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


void pgm_kovsh_decrypt(void)
{

	int i;
	UINT16 *src = (UINT16 *) (memory_region(REGION_CPU1)+0x100000);

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

void pgm_dw2_decrypt(void)
{

	int i;
	UINT16 *src = (UINT16 *) (memory_region(REGION_CPU1)+0x100000);

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

void pgm_djlzz_decrypt(void)
{

	int i;
	UINT16 *src = (UINT16 *) (memory_region(REGION_CPU1)+0x100000);

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

void pgm_pstar_decrypt(void)
{

	int i;
	UINT16 *src = (UINT16 *) (memory_region(REGION_CPU1)+0x100000);

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

void pgm_dw3_decrypt(void)
{

//  int i;
//  UINT16 *src=(UINT16 *) (OP_ROM+0x100000);

	int i;
	UINT16 *src = (UINT16 *) (memory_region(REGION_CPU1)+0x100000);

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

void pgm_killbld_decrypt(void)
{

//  int i;
//  UINT16 *src=(UINT16 *) (OP_ROM+0x100000);

	int i;
	UINT16 *src = (UINT16 *) (memory_region(REGION_CPU1)+0x100000);

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


#ifdef UNUSED_FUNCTION
static UINT8 dfront_tab[256] = {
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

void pgm_dfront_decrypt(void)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(REGION_USER1));

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


static UINT8 ddp2_tab[256] = {
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

void pgm_ddp2_decrypt(void)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(REGION_USER1));

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
#endif


static UINT8 mm_tab[256] = {
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

void pgm_mm_decrypt(void)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(REGION_USER1));

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

void pgm_kov2_decrypt(void)
{
	int i;
	UINT16 *src = (UINT16 *)(memory_region(REGION_USER1));

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

void pgm_puzzli2_decrypt(void)
{
	int i;
	UINT16 *src = (UINT16 *) (memory_region(REGION_CPU1)+0x100000);

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


