/*
    vboy.c - Virtual Boy audio emulation

    By Richard Bannister and Gil Pedersen.
    MESS device adaptation by R. Belmont
*/

#include "emu.h"
#include "vboy.h"

// device type definition
const device_type VBOYSND = &device_creator<vboysnd_device>;

//#define LAME_COMPILER_BUG

#define SxINT       0x00
#define SxLRV       0x04
#define SxFQL       0x08
#define SxFQH       0x0c
#define SxEV0       0x10
#define SxEV1       0x14
#define SxRAM       0x18

#define SxINTb      0x400+SxINT
#define SxLRVb      0x400+SxLRV
#define SxFQLb      0x400+SxFQL
#define SxFQHb      0x400+SxFQH
#define SxEV0b      0x400+SxEV0
#define SxEV1b      0x400+SxEV1
#define SxRAMb      0x400+SxRAM

#define S5SWP       0x51c
#define SST0P       0x580

static const UINT16 outTbl[64][32] = {
	{ 0x0000, 0xffe0, 0xffc0, 0xffa0, 0xff80, 0xff60, 0xff40, 0xff20, 0xff00, 0xfee0, 0xfec0, 0xfea0, 0xfe80, 0xfe60, 0xfe40, 0xfe20,
		0xfe00, 0xfde0, 0xfdc0, 0xfda0, 0xfd80, 0xfd60, 0xfd40, 0xfd20, 0xfd00, 0xfce0, 0xfcc0, 0xfca0, 0xfc80, 0xfc60, 0xfc40, 0xfc20},
	{ 0x0000, 0xffe1, 0xffc2, 0xffa3, 0xff84, 0xff65, 0xff46, 0xff27, 0xff08, 0xfee9, 0xfeca, 0xfeab, 0xfe8c, 0xfe6d, 0xfe4e, 0xfe2f,
		0xfe10, 0xfdf1, 0xfdd2, 0xfdb3, 0xfd94, 0xfd75, 0xfd56, 0xfd37, 0xfd18, 0xfcf9, 0xfcda, 0xfcbb, 0xfc9c, 0xfc7d, 0xfc5e, 0xfc3f},
	{ 0x0000, 0xffe2, 0xffc4, 0xffa6, 0xff88, 0xff6a, 0xff4c, 0xff2e, 0xff10, 0xfef2, 0xfed4, 0xfeb6, 0xfe98, 0xfe7a, 0xfe5c, 0xfe3e,
		0xfe20, 0xfe02, 0xfde4, 0xfdc6, 0xfda8, 0xfd8a, 0xfd6c, 0xfd4e, 0xfd30, 0xfd12, 0xfcf4, 0xfcd6, 0xfcb8, 0xfc9a, 0xfc7c, 0xfc5e},
	{ 0x0000, 0xffe3, 0xffc6, 0xffa9, 0xff8c, 0xff6f, 0xff52, 0xff35, 0xff18, 0xfefb, 0xfede, 0xfec1, 0xfea4, 0xfe87, 0xfe6a, 0xfe4d,
		0xfe30, 0xfe13, 0xfdf6, 0xfdd9, 0xfdbc, 0xfd9f, 0xfd82, 0xfd65, 0xfd48, 0xfd2b, 0xfd0e, 0xfcf1, 0xfcd4, 0xfcb7, 0xfc9a, 0xfc7d},
	{ 0x0000, 0xffe4, 0xffc8, 0xffac, 0xff90, 0xff74, 0xff58, 0xff3c, 0xff20, 0xff04, 0xfee8, 0xfecc, 0xfeb0, 0xfe94, 0xfe78, 0xfe5c,
		0xfe40, 0xfe24, 0xfe08, 0xfdec, 0xfdd0, 0xfdb4, 0xfd98, 0xfd7c, 0xfd60, 0xfd44, 0xfd28, 0xfd0c, 0xfcf0, 0xfcd4, 0xfcb8, 0xfc9c},
	{ 0x0000, 0xffe5, 0xffca, 0xffaf, 0xff94, 0xff79, 0xff5e, 0xff43, 0xff28, 0xff0d, 0xfef2, 0xfed7, 0xfebc, 0xfea1, 0xfe86, 0xfe6b,
		0xfe50, 0xfe35, 0xfe1a, 0xfdff, 0xfde4, 0xfdc9, 0xfdae, 0xfd93, 0xfd78, 0xfd5d, 0xfd42, 0xfd27, 0xfd0c, 0xfcf1, 0xfcd6, 0xfcbb},
	{ 0x0000, 0xffe6, 0xffcc, 0xffb2, 0xff98, 0xff7e, 0xff64, 0xff4a, 0xff30, 0xff16, 0xfefc, 0xfee2, 0xfec8, 0xfeae, 0xfe94, 0xfe7a,
		0xfe60, 0xfe46, 0xfe2c, 0xfe12, 0xfdf8, 0xfdde, 0xfdc4, 0xfdaa, 0xfd90, 0xfd76, 0xfd5c, 0xfd42, 0xfd28, 0xfd0e, 0xfcf4, 0xfcda},
	{ 0x0000, 0xffe7, 0xffce, 0xffb5, 0xff9c, 0xff83, 0xff6a, 0xff51, 0xff38, 0xff1f, 0xff06, 0xfeed, 0xfed4, 0xfebb, 0xfea2, 0xfe89,
		0xfe70, 0xfe57, 0xfe3e, 0xfe25, 0xfe0c, 0xfdf3, 0xfdda, 0xfdc1, 0xfda8, 0xfd8f, 0xfd76, 0xfd5d, 0xfd44, 0xfd2b, 0xfd12, 0xfcf9},
	{ 0x0000, 0xffe8, 0xffd0, 0xffb8, 0xffa0, 0xff88, 0xff70, 0xff58, 0xff40, 0xff28, 0xff10, 0xfef8, 0xfee0, 0xfec8, 0xfeb0, 0xfe98,
		0xfe80, 0xfe68, 0xfe50, 0xfe38, 0xfe20, 0xfe08, 0xfdf0, 0xfdd8, 0xfdc0, 0xfda8, 0xfd90, 0xfd78, 0xfd60, 0xfd48, 0xfd30, 0xfd18},
	{ 0x0000, 0xffe9, 0xffd2, 0xffbb, 0xffa4, 0xff8d, 0xff76, 0xff5f, 0xff48, 0xff31, 0xff1a, 0xff03, 0xfeec, 0xfed5, 0xfebe, 0xfea7,
		0xfe90, 0xfe79, 0xfe62, 0xfe4b, 0xfe34, 0xfe1d, 0xfe06, 0xfdef, 0xfdd8, 0xfdc1, 0xfdaa, 0xfd93, 0xfd7c, 0xfd65, 0xfd4e, 0xfd37},
	{ 0x0000, 0xffea, 0xffd4, 0xffbe, 0xffa8, 0xff92, 0xff7c, 0xff66, 0xff50, 0xff3a, 0xff24, 0xff0e, 0xfef8, 0xfee2, 0xfecc, 0xfeb6,
		0xfea0, 0xfe8a, 0xfe74, 0xfe5e, 0xfe48, 0xfe32, 0xfe1c, 0xfe06, 0xfdf0, 0xfdda, 0xfdc4, 0xfdae, 0xfd98, 0xfd82, 0xfd6c, 0xfd56},
	{ 0x0000, 0xffeb, 0xffd6, 0xffc1, 0xffac, 0xff97, 0xff82, 0xff6d, 0xff58, 0xff43, 0xff2e, 0xff19, 0xff04, 0xfeef, 0xfeda, 0xfec5,
		0xfeb0, 0xfe9b, 0xfe86, 0xfe71, 0xfe5c, 0xfe47, 0xfe32, 0xfe1d, 0xfe08, 0xfdf3, 0xfdde, 0xfdc9, 0xfdb4, 0xfd9f, 0xfd8a, 0xfd75},
	{ 0x0000, 0xffec, 0xffd8, 0xffc4, 0xffb0, 0xff9c, 0xff88, 0xff74, 0xff60, 0xff4c, 0xff38, 0xff24, 0xff10, 0xfefc, 0xfee8, 0xfed4,
		0xfec0, 0xfeac, 0xfe98, 0xfe84, 0xfe70, 0xfe5c, 0xfe48, 0xfe34, 0xfe20, 0xfe0c, 0xfdf8, 0xfde4, 0xfdd0, 0xfdbc, 0xfda8, 0xfd94},
	{ 0x0000, 0xffed, 0xffda, 0xffc7, 0xffb4, 0xffa1, 0xff8e, 0xff7b, 0xff68, 0xff55, 0xff42, 0xff2f, 0xff1c, 0xff09, 0xfef6, 0xfee3,
		0xfed0, 0xfebd, 0xfeaa, 0xfe97, 0xfe84, 0xfe71, 0xfe5e, 0xfe4b, 0xfe38, 0xfe25, 0xfe12, 0xfdff, 0xfdec, 0xfdd9, 0xfdc6, 0xfdb3},
	{ 0x0000, 0xffee, 0xffdc, 0xffca, 0xffb8, 0xffa6, 0xff94, 0xff82, 0xff70, 0xff5e, 0xff4c, 0xff3a, 0xff28, 0xff16, 0xff04, 0xfef2,
		0xfee0, 0xfece, 0xfebc, 0xfeaa, 0xfe98, 0xfe86, 0xfe74, 0xfe62, 0xfe50, 0xfe3e, 0xfe2c, 0xfe1a, 0xfe08, 0xfdf6, 0xfde4, 0xfdd2},
	{ 0x0000, 0xffef, 0xffde, 0xffcd, 0xffbc, 0xffab, 0xff9a, 0xff89, 0xff78, 0xff67, 0xff56, 0xff45, 0xff34, 0xff23, 0xff12, 0xff01,
		0xfef0, 0xfedf, 0xfece, 0xfebd, 0xfeac, 0xfe9b, 0xfe8a, 0xfe79, 0xfe68, 0xfe57, 0xfe46, 0xfe35, 0xfe24, 0xfe13, 0xfe02, 0xfdf1},
	{ 0x0000, 0xfff0, 0xffe0, 0xffd0, 0xffc0, 0xffb0, 0xffa0, 0xff90, 0xff80, 0xff70, 0xff60, 0xff50, 0xff40, 0xff30, 0xff20, 0xff10,
		0xff00, 0xfef0, 0xfee0, 0xfed0, 0xfec0, 0xfeb0, 0xfea0, 0xfe90, 0xfe80, 0xfe70, 0xfe60, 0xfe50, 0xfe40, 0xfe30, 0xfe20, 0xfe10},
	{ 0x0000, 0xfff1, 0xffe2, 0xffd3, 0xffc4, 0xffb5, 0xffa6, 0xff97, 0xff88, 0xff79, 0xff6a, 0xff5b, 0xff4c, 0xff3d, 0xff2e, 0xff1f,
		0xff10, 0xff01, 0xfef2, 0xfee3, 0xfed4, 0xfec5, 0xfeb6, 0xfea7, 0xfe98, 0xfe89, 0xfe7a, 0xfe6b, 0xfe5c, 0xfe4d, 0xfe3e, 0xfe2f},
	{ 0x0000, 0xfff2, 0xffe4, 0xffd6, 0xffc8, 0xffba, 0xffac, 0xff9e, 0xff90, 0xff82, 0xff74, 0xff66, 0xff58, 0xff4a, 0xff3c, 0xff2e,
		0xff20, 0xff12, 0xff04, 0xfef6, 0xfee8, 0xfeda, 0xfecc, 0xfebe, 0xfeb0, 0xfea2, 0xfe94, 0xfe86, 0xfe78, 0xfe6a, 0xfe5c, 0xfe4e},
	{ 0x0000, 0xfff3, 0xffe6, 0xffd9, 0xffcc, 0xffbf, 0xffb2, 0xffa5, 0xff98, 0xff8b, 0xff7e, 0xff71, 0xff64, 0xff57, 0xff4a, 0xff3d,
		0xff30, 0xff23, 0xff16, 0xff09, 0xfefc, 0xfeef, 0xfee2, 0xfed5, 0xfec8, 0xfebb, 0xfeae, 0xfea1, 0xfe94, 0xfe87, 0xfe7a, 0xfe6d},
	{ 0x0000, 0xfff4, 0xffe8, 0xffdc, 0xffd0, 0xffc4, 0xffb8, 0xffac, 0xffa0, 0xff94, 0xff88, 0xff7c, 0xff70, 0xff64, 0xff58, 0xff4c,
		0xff40, 0xff34, 0xff28, 0xff1c, 0xff10, 0xff04, 0xfef8, 0xfeec, 0xfee0, 0xfed4, 0xfec8, 0xfebc, 0xfeb0, 0xfea4, 0xfe98, 0xfe8c},
	{ 0x0000, 0xfff5, 0xffea, 0xffdf, 0xffd4, 0xffc9, 0xffbe, 0xffb3, 0xffa8, 0xff9d, 0xff92, 0xff87, 0xff7c, 0xff71, 0xff66, 0xff5b,
		0xff50, 0xff45, 0xff3a, 0xff2f, 0xff24, 0xff19, 0xff0e, 0xff03, 0xfef8, 0xfeed, 0xfee2, 0xfed7, 0xfecc, 0xfec1, 0xfeb6, 0xfeab},
	{ 0x0000, 0xfff6, 0xffec, 0xffe2, 0xffd8, 0xffce, 0xffc4, 0xffba, 0xffb0, 0xffa6, 0xff9c, 0xff92, 0xff88, 0xff7e, 0xff74, 0xff6a,
		0xff60, 0xff56, 0xff4c, 0xff42, 0xff38, 0xff2e, 0xff24, 0xff1a, 0xff10, 0xff06, 0xfefc, 0xfef2, 0xfee8, 0xfede, 0xfed4, 0xfeca},
	{ 0x0000, 0xfff7, 0xffee, 0xffe5, 0xffdc, 0xffd3, 0xffca, 0xffc1, 0xffb8, 0xffaf, 0xffa6, 0xff9d, 0xff94, 0xff8b, 0xff82, 0xff79,
		0xff70, 0xff67, 0xff5e, 0xff55, 0xff4c, 0xff43, 0xff3a, 0xff31, 0xff28, 0xff1f, 0xff16, 0xff0d, 0xff04, 0xfefb, 0xfef2, 0xfee9},
	{ 0x0000, 0xfff8, 0xfff0, 0xffe8, 0xffe0, 0xffd8, 0xffd0, 0xffc8, 0xffc0, 0xffb8, 0xffb0, 0xffa8, 0xffa0, 0xff98, 0xff90, 0xff88,
		0xff80, 0xff78, 0xff70, 0xff68, 0xff60, 0xff58, 0xff50, 0xff48, 0xff40, 0xff38, 0xff30, 0xff28, 0xff20, 0xff18, 0xff10, 0xff08},
	{ 0x0000, 0xfff9, 0xfff2, 0xffeb, 0xffe4, 0xffdd, 0xffd6, 0xffcf, 0xffc8, 0xffc1, 0xffba, 0xffb3, 0xffac, 0xffa5, 0xff9e, 0xff97,
		0xff90, 0xff89, 0xff82, 0xff7b, 0xff74, 0xff6d, 0xff66, 0xff5f, 0xff58, 0xff51, 0xff4a, 0xff43, 0xff3c, 0xff35, 0xff2e, 0xff27},
	{ 0x0000, 0xfffa, 0xfff4, 0xffee, 0xffe8, 0xffe2, 0xffdc, 0xffd6, 0xffd0, 0xffca, 0xffc4, 0xffbe, 0xffb8, 0xffb2, 0xffac, 0xffa6,
		0xffa0, 0xff9a, 0xff94, 0xff8e, 0xff88, 0xff82, 0xff7c, 0xff76, 0xff70, 0xff6a, 0xff64, 0xff5e, 0xff58, 0xff52, 0xff4c, 0xff46},
	{ 0x0000, 0xfffb, 0xfff6, 0xfff1, 0xffec, 0xffe7, 0xffe2, 0xffdd, 0xffd8, 0xffd3, 0xffce, 0xffc9, 0xffc4, 0xffbf, 0xffba, 0xffb5,
		0xffb0, 0xffab, 0xffa6, 0xffa1, 0xff9c, 0xff97, 0xff92, 0xff8d, 0xff88, 0xff83, 0xff7e, 0xff79, 0xff74, 0xff6f, 0xff6a, 0xff65},
	{ 0x0000, 0xfffc, 0xfff8, 0xfff4, 0xfff0, 0xffec, 0xffe8, 0xffe4, 0xffe0, 0xffdc, 0xffd8, 0xffd4, 0xffd0, 0xffcc, 0xffc8, 0xffc4,
		0xffc0, 0xffbc, 0xffb8, 0xffb4, 0xffb0, 0xffac, 0xffa8, 0xffa4, 0xffa0, 0xff9c, 0xff98, 0xff94, 0xff90, 0xff8c, 0xff88, 0xff84},
	{ 0x0000, 0xfffd, 0xfffa, 0xfff7, 0xfff4, 0xfff1, 0xffee, 0xffeb, 0xffe8, 0xffe5, 0xffe2, 0xffdf, 0xffdc, 0xffd9, 0xffd6, 0xffd3,
		0xffd0, 0xffcd, 0xffca, 0xffc7, 0xffc4, 0xffc1, 0xffbe, 0xffbb, 0xffb8, 0xffb5, 0xffb2, 0xffaf, 0xffac, 0xffa9, 0xffa6, 0xffa3},
	{ 0x0000, 0xfffe, 0xfffc, 0xfffa, 0xfff8, 0xfff6, 0xfff4, 0xfff2, 0xfff0, 0xffee, 0xffec, 0xffea, 0xffe8, 0xffe6, 0xffe4, 0xffe2,
		0xffe0, 0xffde, 0xffdc, 0xffda, 0xffd8, 0xffd6, 0xffd4, 0xffd2, 0xffd0, 0xffce, 0xffcc, 0xffca, 0xffc8, 0xffc6, 0xffc4, 0xffc2},
	{ 0x0000, 0xffff, 0xfffe, 0xfffd, 0xfffc, 0xfffb, 0xfffa, 0xfff9, 0xfff8, 0xfff7, 0xfff6, 0xfff5, 0xfff4, 0xfff3, 0xfff2, 0xfff1,
		0xfff0, 0xffef, 0xffee, 0xffed, 0xffec, 0xffeb, 0xffea, 0xffe9, 0xffe8, 0xffe7, 0xffe6, 0xffe5, 0xffe4, 0xffe3, 0xffe2, 0xffe1},
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
	{ 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
		0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f},
	{ 0x0000, 0x0002, 0x0004, 0x0006, 0x0008, 0x000a, 0x000c, 0x000e, 0x0010, 0x0012, 0x0014, 0x0016, 0x0018, 0x001a, 0x001c, 0x001e,
		0x0020, 0x0022, 0x0024, 0x0026, 0x0028, 0x002a, 0x002c, 0x002e, 0x0030, 0x0032, 0x0034, 0x0036, 0x0038, 0x003a, 0x003c, 0x003e},
	{ 0x0000, 0x0003, 0x0006, 0x0009, 0x000c, 0x000f, 0x0012, 0x0015, 0x0018, 0x001b, 0x001e, 0x0021, 0x0024, 0x0027, 0x002a, 0x002d,
		0x0030, 0x0033, 0x0036, 0x0039, 0x003c, 0x003f, 0x0042, 0x0045, 0x0048, 0x004b, 0x004e, 0x0051, 0x0054, 0x0057, 0x005a, 0x005d},
	{ 0x0000, 0x0004, 0x0008, 0x000c, 0x0010, 0x0014, 0x0018, 0x001c, 0x0020, 0x0024, 0x0028, 0x002c, 0x0030, 0x0034, 0x0038, 0x003c,
		0x0040, 0x0044, 0x0048, 0x004c, 0x0050, 0x0054, 0x0058, 0x005c, 0x0060, 0x0064, 0x0068, 0x006c, 0x0070, 0x0074, 0x0078, 0x007c},
	{ 0x0000, 0x0005, 0x000a, 0x000f, 0x0014, 0x0019, 0x001e, 0x0023, 0x0028, 0x002d, 0x0032, 0x0037, 0x003c, 0x0041, 0x0046, 0x004b,
		0x0050, 0x0055, 0x005a, 0x005f, 0x0064, 0x0069, 0x006e, 0x0073, 0x0078, 0x007d, 0x0082, 0x0087, 0x008c, 0x0091, 0x0096, 0x009b},
	{ 0x0000, 0x0006, 0x000c, 0x0012, 0x0018, 0x001e, 0x0024, 0x002a, 0x0030, 0x0036, 0x003c, 0x0042, 0x0048, 0x004e, 0x0054, 0x005a,
		0x0060, 0x0066, 0x006c, 0x0072, 0x0078, 0x007e, 0x0084, 0x008a, 0x0090, 0x0096, 0x009c, 0x00a2, 0x00a8, 0x00ae, 0x00b4, 0x00ba},
	{ 0x0000, 0x0007, 0x000e, 0x0015, 0x001c, 0x0023, 0x002a, 0x0031, 0x0038, 0x003f, 0x0046, 0x004d, 0x0054, 0x005b, 0x0062, 0x0069,
		0x0070, 0x0077, 0x007e, 0x0085, 0x008c, 0x0093, 0x009a, 0x00a1, 0x00a8, 0x00af, 0x00b6, 0x00bd, 0x00c4, 0x00cb, 0x00d2, 0x00d9},
	{ 0x0000, 0x0008, 0x0010, 0x0018, 0x0020, 0x0028, 0x0030, 0x0038, 0x0040, 0x0048, 0x0050, 0x0058, 0x0060, 0x0068, 0x0070, 0x0078,
		0x0080, 0x0088, 0x0090, 0x0098, 0x00a0, 0x00a8, 0x00b0, 0x00b8, 0x00c0, 0x00c8, 0x00d0, 0x00d8, 0x00e0, 0x00e8, 0x00f0, 0x00f8},
	{ 0x0000, 0x0009, 0x0012, 0x001b, 0x0024, 0x002d, 0x0036, 0x003f, 0x0048, 0x0051, 0x005a, 0x0063, 0x006c, 0x0075, 0x007e, 0x0087,
		0x0090, 0x0099, 0x00a2, 0x00ab, 0x00b4, 0x00bd, 0x00c6, 0x00cf, 0x00d8, 0x00e1, 0x00ea, 0x00f3, 0x00fc, 0x0105, 0x010e, 0x0117},
	{ 0x0000, 0x000a, 0x0014, 0x001e, 0x0028, 0x0032, 0x003c, 0x0046, 0x0050, 0x005a, 0x0064, 0x006e, 0x0078, 0x0082, 0x008c, 0x0096,
		0x00a0, 0x00aa, 0x00b4, 0x00be, 0x00c8, 0x00d2, 0x00dc, 0x00e6, 0x00f0, 0x00fa, 0x0104, 0x010e, 0x0118, 0x0122, 0x012c, 0x0136},
	{ 0x0000, 0x000b, 0x0016, 0x0021, 0x002c, 0x0037, 0x0042, 0x004d, 0x0058, 0x0063, 0x006e, 0x0079, 0x0084, 0x008f, 0x009a, 0x00a5,
		0x00b0, 0x00bb, 0x00c6, 0x00d1, 0x00dc, 0x00e7, 0x00f2, 0x00fd, 0x0108, 0x0113, 0x011e, 0x0129, 0x0134, 0x013f, 0x014a, 0x0155},
	{ 0x0000, 0x000c, 0x0018, 0x0024, 0x0030, 0x003c, 0x0048, 0x0054, 0x0060, 0x006c, 0x0078, 0x0084, 0x0090, 0x009c, 0x00a8, 0x00b4,
		0x00c0, 0x00cc, 0x00d8, 0x00e4, 0x00f0, 0x00fc, 0x0108, 0x0114, 0x0120, 0x012c, 0x0138, 0x0144, 0x0150, 0x015c, 0x0168, 0x0174},
	{ 0x0000, 0x000d, 0x001a, 0x0027, 0x0034, 0x0041, 0x004e, 0x005b, 0x0068, 0x0075, 0x0082, 0x008f, 0x009c, 0x00a9, 0x00b6, 0x00c3,
		0x00d0, 0x00dd, 0x00ea, 0x00f7, 0x0104, 0x0111, 0x011e, 0x012b, 0x0138, 0x0145, 0x0152, 0x015f, 0x016c, 0x0179, 0x0186, 0x0193},
	{ 0x0000, 0x000e, 0x001c, 0x002a, 0x0038, 0x0046, 0x0054, 0x0062, 0x0070, 0x007e, 0x008c, 0x009a, 0x00a8, 0x00b6, 0x00c4, 0x00d2,
		0x00e0, 0x00ee, 0x00fc, 0x010a, 0x0118, 0x0126, 0x0134, 0x0142, 0x0150, 0x015e, 0x016c, 0x017a, 0x0188, 0x0196, 0x01a4, 0x01b2},
	{ 0x0000, 0x000f, 0x001e, 0x002d, 0x003c, 0x004b, 0x005a, 0x0069, 0x0078, 0x0087, 0x0096, 0x00a5, 0x00b4, 0x00c3, 0x00d2, 0x00e1,
		0x00f0, 0x00ff, 0x010e, 0x011d, 0x012c, 0x013b, 0x014a, 0x0159, 0x0168, 0x0177, 0x0186, 0x0195, 0x01a4, 0x01b3, 0x01c2, 0x01d1},
	{ 0x0000, 0x0010, 0x0020, 0x0030, 0x0040, 0x0050, 0x0060, 0x0070, 0x0080, 0x0090, 0x00a0, 0x00b0, 0x00c0, 0x00d0, 0x00e0, 0x00f0,
		0x0100, 0x0110, 0x0120, 0x0130, 0x0140, 0x0150, 0x0160, 0x0170, 0x0180, 0x0190, 0x01a0, 0x01b0, 0x01c0, 0x01d0, 0x01e0, 0x01f0},
	{ 0x0000, 0x0011, 0x0022, 0x0033, 0x0044, 0x0055, 0x0066, 0x0077, 0x0088, 0x0099, 0x00aa, 0x00bb, 0x00cc, 0x00dd, 0x00ee, 0x00ff,
		0x0110, 0x0121, 0x0132, 0x0143, 0x0154, 0x0165, 0x0176, 0x0187, 0x0198, 0x01a9, 0x01ba, 0x01cb, 0x01dc, 0x01ed, 0x01fe, 0x020f},
	{ 0x0000, 0x0012, 0x0024, 0x0036, 0x0048, 0x005a, 0x006c, 0x007e, 0x0090, 0x00a2, 0x00b4, 0x00c6, 0x00d8, 0x00ea, 0x00fc, 0x010e,
		0x0120, 0x0132, 0x0144, 0x0156, 0x0168, 0x017a, 0x018c, 0x019e, 0x01b0, 0x01c2, 0x01d4, 0x01e6, 0x01f8, 0x020a, 0x021c, 0x022e},
	{ 0x0000, 0x0013, 0x0026, 0x0039, 0x004c, 0x005f, 0x0072, 0x0085, 0x0098, 0x00ab, 0x00be, 0x00d1, 0x00e4, 0x00f7, 0x010a, 0x011d,
		0x0130, 0x0143, 0x0156, 0x0169, 0x017c, 0x018f, 0x01a2, 0x01b5, 0x01c8, 0x01db, 0x01ee, 0x0201, 0x0214, 0x0227, 0x023a, 0x024d},
	{ 0x0000, 0x0014, 0x0028, 0x003c, 0x0050, 0x0064, 0x0078, 0x008c, 0x00a0, 0x00b4, 0x00c8, 0x00dc, 0x00f0, 0x0104, 0x0118, 0x012c,
		0x0140, 0x0154, 0x0168, 0x017c, 0x0190, 0x01a4, 0x01b8, 0x01cc, 0x01e0, 0x01f4, 0x0208, 0x021c, 0x0230, 0x0244, 0x0258, 0x026c},
	{ 0x0000, 0x0015, 0x002a, 0x003f, 0x0054, 0x0069, 0x007e, 0x0093, 0x00a8, 0x00bd, 0x00d2, 0x00e7, 0x00fc, 0x0111, 0x0126, 0x013b,
		0x0150, 0x0165, 0x017a, 0x018f, 0x01a4, 0x01b9, 0x01ce, 0x01e3, 0x01f8, 0x020d, 0x0222, 0x0237, 0x024c, 0x0261, 0x0276, 0x028b},
	{ 0x0000, 0x0016, 0x002c, 0x0042, 0x0058, 0x006e, 0x0084, 0x009a, 0x00b0, 0x00c6, 0x00dc, 0x00f2, 0x0108, 0x011e, 0x0134, 0x014a,
		0x0160, 0x0176, 0x018c, 0x01a2, 0x01b8, 0x01ce, 0x01e4, 0x01fa, 0x0210, 0x0226, 0x023c, 0x0252, 0x0268, 0x027e, 0x0294, 0x02aa},
	{ 0x0000, 0x0017, 0x002e, 0x0045, 0x005c, 0x0073, 0x008a, 0x00a1, 0x00b8, 0x00cf, 0x00e6, 0x00fd, 0x0114, 0x012b, 0x0142, 0x0159,
		0x0170, 0x0187, 0x019e, 0x01b5, 0x01cc, 0x01e3, 0x01fa, 0x0211, 0x0228, 0x023f, 0x0256, 0x026d, 0x0284, 0x029b, 0x02b2, 0x02c9},
	{ 0x0000, 0x0018, 0x0030, 0x0048, 0x0060, 0x0078, 0x0090, 0x00a8, 0x00c0, 0x00d8, 0x00f0, 0x0108, 0x0120, 0x0138, 0x0150, 0x0168,
		0x0180, 0x0198, 0x01b0, 0x01c8, 0x01e0, 0x01f8, 0x0210, 0x0228, 0x0240, 0x0258, 0x0270, 0x0288, 0x02a0, 0x02b8, 0x02d0, 0x02e8},
	{ 0x0000, 0x0019, 0x0032, 0x004b, 0x0064, 0x007d, 0x0096, 0x00af, 0x00c8, 0x00e1, 0x00fa, 0x0113, 0x012c, 0x0145, 0x015e, 0x0177,
		0x0190, 0x01a9, 0x01c2, 0x01db, 0x01f4, 0x020d, 0x0226, 0x023f, 0x0258, 0x0271, 0x028a, 0x02a3, 0x02bc, 0x02d5, 0x02ee, 0x0307},
	{ 0x0000, 0x001a, 0x0034, 0x004e, 0x0068, 0x0082, 0x009c, 0x00b6, 0x00d0, 0x00ea, 0x0104, 0x011e, 0x0138, 0x0152, 0x016c, 0x0186,
		0x01a0, 0x01ba, 0x01d4, 0x01ee, 0x0208, 0x0222, 0x023c, 0x0256, 0x0270, 0x028a, 0x02a4, 0x02be, 0x02d8, 0x02f2, 0x030c, 0x0326},
	{ 0x0000, 0x001b, 0x0036, 0x0051, 0x006c, 0x0087, 0x00a2, 0x00bd, 0x00d8, 0x00f3, 0x010e, 0x0129, 0x0144, 0x015f, 0x017a, 0x0195,
		0x01b0, 0x01cb, 0x01e6, 0x0201, 0x021c, 0x0237, 0x0252, 0x026d, 0x0288, 0x02a3, 0x02be, 0x02d9, 0x02f4, 0x030f, 0x032a, 0x0345},
	{ 0x0000, 0x001c, 0x0038, 0x0054, 0x0070, 0x008c, 0x00a8, 0x00c4, 0x00e0, 0x00fc, 0x0118, 0x0134, 0x0150, 0x016c, 0x0188, 0x01a4,
		0x01c0, 0x01dc, 0x01f8, 0x0214, 0x0230, 0x024c, 0x0268, 0x0284, 0x02a0, 0x02bc, 0x02d8, 0x02f4, 0x0310, 0x032c, 0x0348, 0x0364},
	{ 0x0000, 0x001d, 0x003a, 0x0057, 0x0074, 0x0091, 0x00ae, 0x00cb, 0x00e8, 0x0105, 0x0122, 0x013f, 0x015c, 0x0179, 0x0196, 0x01b3,
		0x01d0, 0x01ed, 0x020a, 0x0227, 0x0244, 0x0261, 0x027e, 0x029b, 0x02b8, 0x02d5, 0x02f2, 0x030f, 0x032c, 0x0349, 0x0366, 0x0383},
	{ 0x0000, 0x001e, 0x003c, 0x005a, 0x0078, 0x0096, 0x00b4, 0x00d2, 0x00f0, 0x010e, 0x012c, 0x014a, 0x0168, 0x0186, 0x01a4, 0x01c2,
		0x01e0, 0x01fe, 0x021c, 0x023a, 0x0258, 0x0276, 0x0294, 0x02b2, 0x02d0, 0x02ee, 0x030c, 0x032a, 0x0348, 0x0366, 0x0384, 0x03a2},
	{ 0x0000, 0x001f, 0x003e, 0x005d, 0x007c, 0x009b, 0x00ba, 0x00d9, 0x00f8, 0x0117, 0x0136, 0x0155, 0x0174, 0x0193, 0x01b2, 0x01d1,
		0x01f0, 0x020f, 0x022e, 0x024d, 0x026c, 0x028b, 0x02aa, 0x02c9, 0x02e8, 0x0307, 0x0326, 0x0345, 0x0364, 0x0383, 0x03a2, 0x03c1}
};

static const INT8 outLvlTbl[16][16] = {
	{ 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
	{ 0,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2},
	{ 0,  1,  1,  1,  2,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  4},
	{ 0,  1,  1,  2,  2,  2,  3,  3,  4,  4,  4,  5,  5,  5,  6,  6},
	{ 0,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8},
	{ 0,  1,  2,  2,  3,  4,  4,  5,  6,  6,  7,  7,  8,  9,  9, 10},
	{ 0,  1,  2,  3,  4,  4,  5,  6,  7,  7,  8,  9, 10, 10, 11, 12},
	{ 0,  1,  2,  3,  4,  5,  6,  7,  7,  7,  8, 10, 11, 12, 13, 14},
	{ 0,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16},
	{ 0,  2,  3,  4,  5,  6,  7,  8, 10, 11, 12, 13, 14, 15, 16, 17},
	{ 0,  2,  3,  4,  6,  7,  8,  9, 11, 12, 13, 14, 16, 17, 18, 19},
	{ 0,  2,  3,  5,  6,  7,  9, 10, 12, 13, 14, 16, 17, 18, 20, 21},
	{ 0,  2,  4,  5,  7,  8, 10, 11, 13, 14, 16, 17, 19, 20, 22, 23},
	{ 0,  2,  4,  5,  7,  9, 10, 12, 14, 15, 17, 18, 20, 22, 23, 25},
	{ 0,  2,  4,  6,  8,  9, 11, 13, 15, 16, 18, 20, 22, 23, 25, 27},
	{ 0,  2,  4,  6,  8, 10, 12, 14, 16, 17, 19, 21, 23, 25, 27, 29}
};

INLINE UINT8 mgetb(register UINT8 *ptr) { return *ptr; }
INLINE void mputb(UINT8 *ptr, INT8 data) { *ptr = data; }

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  vboysnd_device - constructor
//-------------------------------------------------

vboysnd_device::vboysnd_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, VBOYSND, "Virtual Boy audio", tag, owner, clock),
		device_sound_interface(mconfig, *this)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void vboysnd_device::device_start()
{
	int i;

	// create the stream
	m_stream = machine().sound().stream_alloc(*this, 0, 2, AUDIO_FREQ, this);

	for (i=0; i<2048; i++)
		waveFreq2LenTbl[i] = AUDIO_FREQ / (5000000.0/(float)((2048-i) * 32));
	for (i=0; i<32; i++)
		waveTimer2LenTbl[i] = ((0.00384*(float)(i+1)) * (float)AUDIO_FREQ);
	for (i=0; i<8; i++)
		waveEnv2LenTbl[i] = ((0.01536*(float)(i+1)) * (float)AUDIO_FREQ);

	for (i = 0; i < 5; i++)
		memset(&snd_channel[i], 0, sizeof(s_snd_channel));

	memset(m_aram, 0, 0x600);

	m_timer = timer_alloc(0, NULL);
	m_timer->adjust(attotime::zero, 0, attotime::from_hz(AUDIO_FREQ/4));
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void vboysnd_device::device_reset()
{
	m_stream->update();

	int i;

	for (i=0; i<4; i++)
		snd_channel[i].playing = false;
}

//-------------------------------------------------
//  device_timer - called when our device timer expires
//-------------------------------------------------

void vboysnd_device::device_timer(emu_timer &timer, device_timer_id tid, int param, void *ptr)
{
	m_stream->update();
}

//-------------------------------------------------
//  sound_stream_update - handle update requests for
//  our sound stream
//-------------------------------------------------

void vboysnd_device::sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
	stream_sample_t *outL, *outR;
	int len, i, j, channel;

	outL = outputs[0];
	outR = outputs[1];

	len = samples;

//  if (mgetb(m_aram+SST0P) & 0x1)  // Sound Stop Reg
//      goto end;

	for (channel=0; channel<5; channel++)
	{
		float size;
		int k = 0, waveAddr;

		i = channel * 0x40;
		snd_channel[channel].sample_len = waveFreq2LenTbl[((mgetb(m_aram+SxFQHb+i) & 0x7) << 8) | mgetb(m_aram+SxFQLb+i)];
		waveAddr = (mgetb(m_aram + SxRAMb + i) & 0x3) << 7;

		size = (float)snd_channel[channel].sample_len / 32.0;
		for (j=0; j<32;)
		{
			INT8 byte = mgetb(m_aram + waveAddr + (j++ << 2)) & 0x3f;
//          INT8 nbyte = mgetb(m_aram + waveAddr + (j << 2)) & 0x3f;
			int end = (int)(((float)j)*size);
			float val, add;

			val = (float)byte;
			add = /*(nbyte-byte)/(end-k)*/ 0.0; // no interpolation

			for (; k<end; k++, val+=add)
			{
#ifdef LAME_COMPILER_BUG
				if ((INT32)&snd_channel[channel].sample[k] == (INT32)&snd_channel[channel].offset)
					Debugger();
#endif
				snd_channel[channel].sample[k] = ((UINT8)val);
			}
		}
	}

	for (j=0; j<len; j++)
	{
		INT32 note_left = 0;
		INT32 note_right = 0;

		// process first 4 sound channels
		for (i=0; i</*5*/5; i++)
		{
			UINT8 outLeft, outRight;
			const INT16 *outTblPtr;
//          UINT8 env0, env1;
			s_snd_channel *channel = &snd_channel[i];

			if (!channel->playing)
				continue;

			outLeft = outLvlTbl[channel->volLeft][channel->envelope];
			outRight = outLvlTbl[channel->volRight][channel->envelope];
//          env1 = ((UINT8)mgetb(m_aram+SxEV1b+i*0x40));
			if (channel->env1 & 0x01)
			{
				channel->env_time++;

//              env0 = ((UINT8)mgetb(m_aram+SxEV0b+i*0x40));
				if (channel->env_time >= channel->env_steptime /*waveEnv2LenTbl[env0 & 0x7]*/)
				{
					// it's time for a step
					channel->env_time = 0;
					if (channel->env0 & 0x08)
					{
						// step up
						channel->envelope++;
						if (channel->envelope >= 0xf)
						{
							if (channel->env1 & 0x02) channel->envelope = channel->env0 >> 4;
							else { channel->env1 &= ~0x01; channel->envelope = 0xf; }
						}
					}
					else
					{
						// step down
						channel->envelope--;
						if (channel->envelope <= 0)
						{
							if (channel->env1 & 0x02) channel->envelope = channel->env0 >> 4;
							else { channel->env1 &= ~0x01; channel->envelope = 0x0; }
						}
					}
				}
			}

			outTblPtr = (INT16 *) outTbl[channel->sample[channel->offset]];
			if (outLeft)  note_left  += outTblPtr[outLeft ];
			if (outRight) note_right += outTblPtr[outRight];
			channel->offset++;

			if (channel->offset >= channel->sample_len)
				channel->offset = 0;

			if (channel->time > 0)
			{
				channel->time--;
				if (channel->time <= 0)
					channel->playing = false;
			}
		}


		// scale to 16 bits
		note_left = (note_left << 5) | ((note_left >> 6) & 0x1f);
		note_right = (note_right << 5) | ((note_right >> 6) & 0x1f);
		if (note_left  < -32767) note_left  = -32767; if (note_left  > 32767) note_left  = 32767;
		if (note_right < -32767) note_right = -32767; if (note_right > 32767) note_right = 32767;

		*(outL++) = ((INT16)note_left);
		*(outR++) = ((INT16)note_right);
	}
}

//-------------------------------------------------
//  read - read from the chip's registers and internal RAM
//-------------------------------------------------

READ8_MEMBER( vboysnd_device::read )
{
	m_stream->update();
	return m_aram[offset];
}

//-------------------------------------------------
//  write - write to the chip's registers and internal RAM
//-------------------------------------------------

WRITE8_MEMBER( vboysnd_device::write )
{
	int freq, i;
//    int waveAddr;
	int volReg, intervalReg;
	int channel, ouroffs;

	mputb((UINT8 *)m_aram+offset, data);
	if (offset < 0x400)
		return;

	channel = (offset-0x400) >> 6;
	ouroffs = offset & 0x3f;

	///process_sound(cpu.vcount);

	if (channel == 6 && ouroffs == 0)
	{
		if (mgetb(m_aram+SST0P) & 0x1)
			for (i=0; i<4; i++)
				snd_channel[i].playing = 0;
		return;
	}

	// Extract frequencies and build the wave for the first 4 sound sources
	i = channel * 0x40;
	switch (ouroffs)
	{
		case SxLRV:
			if (channel < 5)
			{
				volReg = mgetb(m_aram + SxLRVb + i);        // 6-4-4 - L/R output level setting
				if (snd_channel[channel].playing)
				{
					snd_channel[channel].volLeft = ((UINT8)volReg) >> 4;
					snd_channel[channel].volRight = ((UINT8)volReg) & 0xf;
					break;
				}
			}
		case SxINT:
			if (channel < 5)
			{
				UINT8 env0, env1;
				// find out if the sound is playing
				intervalReg = mgetb(m_aram + SxINTb + i);   // 6-4-6 - Sound Interval
				volReg = mgetb(m_aram + SxLRVb + i);        // 6-4-4 - L/R output level setting

				if (((intervalReg & 0x80) == 0) || (volReg == 0))
				{
					snd_channel[channel].playing = false;
					return;
				}
				snd_channel[channel].playing = true;

				// 6-4-1 - Waveform address comes from SxRAM.
//              waveAddr = (mgetb(m_aram + SxRAMb + i) & 0x3) << 7;

				// 6-4-15 - Frequency comes from here
				freq = ((mgetb(m_aram+SxFQHb+i) & 0x7) << 8) | mgetb(m_aram+SxFQLb+i);

				env0 = mgetb(m_aram+SxEV0b+i); env1 = mgetb(m_aram+SxEV1b+i);
				snd_channel[channel].env0 = env0;
				snd_channel[channel].env1 = env1;
				snd_channel[channel].volLeft = ((UINT8)volReg) >> 4;
				snd_channel[channel].volRight = ((UINT8)volReg) & 0xf;
				snd_channel[channel].env_steptime = (env1 & 0x1) ? waveEnv2LenTbl[env0 & 0x7] : 0;
				snd_channel[channel].sample_len = waveFreq2LenTbl[freq];

				snd_channel[channel].offset = 0;
				snd_channel[channel].time = (intervalReg & 0x20) ? waveTimer2LenTbl[intervalReg & 0x1f] : 0;
				snd_channel[channel].envelope = ((UINT8)mgetb(m_aram+SxEV0b+i)) >> 4;
				snd_channel[channel].env_time = 0;
			}
			else
			{
				intervalReg = mgetb(m_aram + SxINTb + i);   // 6-4-6 - Sound Interval
				volReg = mgetb(m_aram + SxLRVb + i);        // 6-4-4 - L/R output level setting

				if (((intervalReg & 0x80) == 0) || (volReg == 0))
				{
//                  snd_channel[channel].playing = false;
					return;
				}
//              Debugger(); // uses white noise
//              snd_channel[channel].playing = true;
			}
			break;

		case SxEV0:
			if (snd_channel[channel].playing)
				snd_channel[channel].envelope = ((UINT8)mgetb(m_aram+SxEV0b+i)) >> 4;
			break;
	}
}
