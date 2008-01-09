/* Raiden 2 Sprite Decryption */

#include "driver.h"

/* INIT */

static const int swx[32] = {
  25, 28, 15, 19,  6,  0,  3, 24,
  11,  1,  2, 30, 16,  7, 22, 17,
  31, 14, 23,  9, 27, 18,  4, 10,
  13, 20,  5, 12,  8, 29, 26, 21,
};

static UINT32 sw(UINT32 v)
{
  UINT32 r = 0;
  int i;
  for(i=0; i<32; i++)
    if(v & (1 << swx[i]))
      r |= 1 << (31-i);
  return r;
}

static const UINT8 rotate[512] = {
  0x11, 0x17, 0x0d, 0x03, 0x17, 0x1f, 0x08, 0x1a, 0x0f, 0x04, 0x1e, 0x13, 0x19, 0x0e, 0x0e, 0x05,
  0x06, 0x07, 0x08, 0x08, 0x0d, 0x18, 0x11, 0x1a, 0x0b, 0x06, 0x12, 0x0c, 0x1f, 0x0b, 0x1c, 0x19,
  0x00, 0x1b, 0x0c, 0x09, 0x1d, 0x18, 0x1a, 0x16, 0x1a, 0x08, 0x03, 0x04, 0x0f, 0x1d, 0x16, 0x07,
  0x1a, 0x12, 0x01, 0x0b, 0x00, 0x0f, 0x1e, 0x10, 0x09, 0x0f, 0x10, 0x09, 0x0a, 0x1c, 0x0d, 0x08,
  0x06, 0x1a, 0x06, 0x02, 0x11, 0x1e, 0x0c, 0x1c, 0x11, 0x0f, 0x19, 0x0a, 0x16, 0x14, 0x18, 0x11,
  0x0b, 0x0d, 0x1c, 0x1f, 0x0d, 0x1f, 0x0d, 0x19, 0x0d, 0x04, 0x19, 0x0f, 0x06, 0x13, 0x0c, 0x1b,
  0x1f, 0x12, 0x15, 0x1a, 0x04, 0x02, 0x06, 0x03, 0x0a, 0x0d, 0x12, 0x09, 0x17, 0x1d, 0x12, 0x10,
  0x05, 0x07, 0x03, 0x00, 0x14, 0x07, 0x14, 0x1a, 0x1c, 0x0a, 0x10, 0x0f, 0x0b, 0x0c, 0x08, 0x0f,
  0x07, 0x00, 0x13, 0x1c, 0x04, 0x15, 0x0e, 0x02, 0x17, 0x17, 0x00, 0x03, 0x18, 0x00, 0x02, 0x13,
  0x14, 0x0c, 0x01, 0x0a, 0x15, 0x0b, 0x0a, 0x1c, 0x1b, 0x06, 0x17, 0x1d, 0x11, 0x1f, 0x10, 0x04,
  0x1a, 0x01, 0x1b, 0x13, 0x03, 0x09, 0x09, 0x0f, 0x0d, 0x03, 0x15, 0x1c, 0x04, 0x06, 0x06, 0x0b,
  0x04, 0x0a, 0x1f, 0x16, 0x11, 0x0a, 0x05, 0x05, 0x0c, 0x1c, 0x10, 0x0c, 0x11, 0x04, 0x10, 0x1a,
  0x06, 0x10, 0x19, 0x06, 0x15, 0x0f, 0x11, 0x01, 0x10, 0x0c, 0x1d, 0x05, 0x1f, 0x05, 0x12, 0x16,
  0x02, 0x12, 0x14, 0x0d, 0x14, 0x0f, 0x04, 0x07, 0x13, 0x01, 0x11, 0x1c, 0x1c, 0x1d, 0x0e, 0x06,
  0x1d, 0x13, 0x10, 0x06, 0x0f, 0x02, 0x12, 0x10, 0x1e, 0x0c, 0x17, 0x15, 0x0b, 0x1f, 0x01, 0x19,
  0x02, 0x01, 0x07, 0x1d, 0x13, 0x19, 0x0f, 0x0f, 0x10, 0x03, 0x1e, 0x03, 0x0d, 0x0a, 0x0c, 0x0d,

  0x16, 0x1f, 0x16, 0x1a, 0x1c, 0x16, 0x01, 0x03, 0x01, 0x08, 0x14, 0x19, 0x03, 0x1e, 0x08, 0x02,
  0x02, 0x1d, 0x15, 0x00, 0x09, 0x1d, 0x03, 0x11, 0x11, 0x0b, 0x1b, 0x14, 0x01, 0x1e, 0x11, 0x12,
  0x1d, 0x06, 0x0b, 0x13, 0x1e, 0x16, 0x0d, 0x10, 0x11, 0x1f, 0x1c, 0x15, 0x0d, 0x1a, 0x13, 0x1f,
  0x0e, 0x05, 0x10, 0x06, 0x0d, 0x1c, 0x07, 0x19, 0x06, 0x1d, 0x11, 0x00, 0x1c, 0x05, 0x0b, 0x1d,
  0x1c, 0x06, 0x05, 0x1d, 0x00, 0x13, 0x00, 0x12, 0x1b, 0x17, 0x1a, 0x1b, 0x17, 0x1c, 0x16, 0x0a,
  0x11, 0x15, 0x0f, 0x0b, 0x0f, 0x07, 0x0e, 0x04, 0x13, 0x00, 0x1c, 0x05, 0x16, 0x00, 0x1a, 0x04,
  0x17, 0x04, 0x08, 0x1b, 0x05, 0x12, 0x1d, 0x0d, 0x02, 0x16, 0x12, 0x0e, 0x06, 0x08, 0x14, 0x07,
  0x0e, 0x0f, 0x15, 0x13, 0x12, 0x00, 0x1d, 0x16, 0x1b, 0x18, 0x1f, 0x05, 0x12, 0x13, 0x01, 0x0c,
  0x12, 0x04, 0x19, 0x13, 0x12, 0x15, 0x07, 0x06, 0x0a, 0x00, 0x09, 0x14, 0x1e, 0x03, 0x10, 0x1b,
  0x08, 0x1a, 0x07, 0x02, 0x1b, 0x0d, 0x18, 0x13, 0x02, 0x07, 0x1e, 0x05, 0x15, 0x02, 0x06, 0x18,
  0x12, 0x09, 0x1c, 0x07, 0x0b, 0x02, 0x03, 0x00, 0x18, 0x18, 0x03, 0x0f, 0x02, 0x0f, 0x10, 0x09,
  0x05, 0x18, 0x08, 0x1b, 0x0d, 0x10, 0x03, 0x00, 0x0c, 0x14, 0x1d, 0x08, 0x02, 0x10, 0x0b, 0x0c,
  0x00, 0x0d, 0x0d, 0x0a, 0x06, 0x1c, 0x09, 0x19, 0x1b, 0x14, 0x18, 0x0f, 0x02, 0x07, 0x05, 0x04,
  0x1c, 0x15, 0x18, 0x00, 0x0b, 0x10, 0x19, 0x1c, 0x1b, 0x08, 0x1d, 0x12, 0x17, 0x1d, 0x0c, 0x01,
  0x03, 0x0d, 0x03, 0x0d, 0x15, 0x0e, 0x16, 0x08, 0x05, 0x11, 0x1f, 0x03, 0x16, 0x03, 0x0f, 0x10,
  0x08, 0x19, 0x18, 0x15, 0x1f, 0x05, 0x00, 0x09, 0x0e, 0x05, 0x16, 0x1b, 0x01, 0x08, 0x08, 0x1f,
};


static const UINT32 xmap_low_01[8] = { 0x915b174c, 0xd1e3d41d, 0x7afd901e, 0x890aeda6, 0xdaa66bf6, 0xcf3a5859, 0x1fc8ae80, 0xd7c864c2 };
static const UINT32 xmap_low_03[8] = { 0xc9b43501, 0x2d4136ef, 0x5a3e2047, 0xccab4852, 0x67770213, 0xcc1c22ee, 0x7f767fe5, 0xae783fa3 };
static const UINT32 xmap_low_07[8] = { 0x533ce0ff, 0x21561e2b, 0x5e52735b, 0x2f89d3c0, 0x383ee980, 0x807ae78a, 0x6dfab360, 0xccd84e92 };
static const UINT32 xmap_low_23[8] = { 0xa3b39673, 0xb3a21d4a, 0x07440937, 0xa9005a05, 0x12bbf9d7, 0x257164a7, 0x6162a1e4, 0x862c5d73 };

static const UINT32 xmap_low_31[8] = { 0x76fa8a84, 0x2f3f4960, 0x82087362, 0x40aebf9e, 0x02854535, 0xfcbd325a, 0x7b8823f3, 0xcbd62b3a };

static const UINT32 xmap_high_00[8] = { 0x1bf05217, 0xe2b31951, 0x0458ee47, 0x6c06f22c, 0x3f1a7bad, 0xb658f2e4, 0xa2b24b18, 0x3cddd22f };
static const UINT32 xmap_high_02[8] = { 0x3caa374d, 0xfabf45a5, 0x2633d9ba, 0x05573b6a, 0x03234029, 0x185b17b0, 0x53afc974, 0x2067077d };
static const UINT32 xmap_high_03[8] = { 0xdb36b4d7, 0x1e79e916, 0xfcc75654, 0x8b552464, 0x856a3eb4, 0xb60c7c2e, 0xf325d2ee, 0x5cbd9b38 };
static const UINT32 xmap_high_04[8] = { 0x91a1acfe, 0x5adaac01, 0x9dc40024, 0x1c87c08b, 0x34ab1b76, 0x631175d5, 0x017b85e6, 0x13359cd1 };
static const UINT32 xmap_high_06[8] = { 0xd46b6286, 0x2da93768, 0xf95f5b47, 0x657b472e, 0x05ed940f, 0x86364f88, 0x863d5fed, 0xe3f1ef82 };
static const UINT32 xmap_high_21[8] = { 0x1d51f8b6, 0xcc1b30b3, 0x9bf75b9d, 0x2c57e2cd, 0x3b5138de, 0xba5c69c4, 0x422c4b8e, 0xd5465cf6 };
static const UINT32 xmap_high_20[8] = { 0x41d4146c, 0x536d7b04, 0x59d60240, 0x7d01cc23, 0x8a0e5ce4, 0x11e0b0db, 0x513381e1, 0x3264be61 };
static const UINT32 xmap_high_10[8] = { 0xc04f0362, 0x44fa6936, 0xc048b0db, 0x704897b2, 0x7e28568f, 0xfb9e070f, 0xc34a5704, 0xd5888a6f };
static const UINT32 xmap_high_11[8] = { 0xd88e9b92, 0xda49726b, 0xc13f86b7, 0x6ce2a1b0, 0xb3adc6e9, 0xd83c2f64, 0xa14c1efc, 0xe98a3c19 };
static const UINT32 xmap_high_13[8] = { 0x03f8a061, 0x19f39b5a, 0x13a17ae2, 0x85c06682, 0x42118566, 0x78e4ff8a, 0xbee64f97, 0x5eecb443 };
static const UINT32 xmap_high_15[8] = { 0x1c6f2b4f, 0x9eebe281, 0x784b85d8, 0x401d6412, 0x0370ae0a, 0xa791d0b3, 0x89d290ea, 0x4666f009 };
static const UINT32 xmap_high_16[8] = { 0xbe2beb93, 0xac9284fb, 0xa629fdbf, 0x82fe33dc, 0x75f1a31b, 0xee1f4f24, 0xaecc7e1e, 0xcd9b419e };

static const UINT32 zmap_0[8] = { 0x08b01003, 0xed4037ec, 0x9a3a3044, 0x0daf5851, 0xa7725210, 0x0c1822ed, 0xbf726fe6, 0x6e783ea0 };
static const UINT32 zmap_1[8] = { 0xc6783a02, 0x1e8239df, 0xa53d108b, 0xcc5784a1, 0x9bbb0123, 0xcc2c11dd, 0xbfb9bfda, 0x5db43f53 };

static const UINT32 zmap_2[32] = {
  0x1b301017, 0x02310910, 0x04404644, 0x08042024, 0x050a3aa4, 0xb6087024, 0xa2204208, 0x1c9d9228,
  0x00c04200, 0xe0821041, 0x0018a803, 0x6402d208, 0x3a104109, 0x005082c0, 0x00920910, 0x20404007,
  0xc006a0c0, 0x1c48e006, 0xf8871010, 0x83510440, 0x80600410, 0x00040c0a, 0x510590e6, 0x40200910,
  0x24090928, 0x010406a8, 0x032001a8, 0x10a80993, 0x40858042, 0x49a30111, 0x0c482401, 0x830224c0
};

static const UINT32 zmap_3[32] = {
  0xd46b6286, 0x2da93768, 0xf95f5b47, 0x657b472e, 0x05ed940f, 0x86364f88, 0x863d5fed, 0xe3f1ef82,
  0x2b949d79, 0xd256c897, 0x06a0a4b8, 0x9a84b8d1, 0xfa126bf0, 0x79c9b077, 0x79c2a012, 0x1c0e107d,
  0x146dc646, 0x31e1d76e, 0x01d84b57, 0xe62a436e, 0x858d901f, 0x86324382, 0xd738cf0b, 0xa3d1e692,
  0x0f9d9451, 0xd352ce3f, 0x0580a510, 0x8a2cb142, 0xba97ebb2, 0x306ab166, 0x758a8413, 0x9f0c34bd,
};

static const UINT32 zmap_4[16] = {
  0xdb36b4d7, 0x1e79e916, 0xfcc75654, 0x8b552464, 0x856a3eb4, 0xb60c7c2e, 0xf325d2ee, 0x5cbd9b38,
  0x24c94b28, 0xe18616e9, 0x0338a9ab, 0x74aadb9b, 0x7a95c14b, 0x49f383d1, 0x0cda2d11, 0xa34264c7,
};

static const UINT32 zmap_5[32] = {
  0x1bf05217, 0xe2b31951, 0x0458ee47, 0x6c06f22c, 0x3f1a7bad, 0xb658f2e4, 0xa2b24b18, 0x3cddd22f,
  0x1bf05217, 0xe2b31951, 0x0458ee47, 0x6c06f22c, 0x3f1a7bad, 0xb658f2e4, 0xa2b24b18, 0x3cddd22f,
  0x3f39193f, 0x03350fb8, 0x076047ec, 0x18ac29b7, 0x458fbae6, 0xffab7135, 0xae686609, 0x9f9fb6e8,
  0xc0c6e6c0, 0xfccaf047, 0xf89fb813, 0xe753d648, 0xba704519, 0x00548eca, 0x519799f6, 0x60604917,
};

#if 0
static UINT32 xrot(UINT32 v, int r)
{
  return (v >> r) | (v << (32-r));
}
#endif

static UINT32 yrot(UINT32 v, int r)
{
  return (v << r) | (v >> (32-r));
}

static int bt(const UINT32 *tb, int v)
{
  return (tb[v/32] & (1<<(v % 32))) != 0;
}

static UINT32 gr(int i)
{
  int idx = i & 0xff;
  if(i & 0x008000)
    idx ^= 1;
  if(i & 0x100000)
    idx ^= 256;
  return rotate[idx];
}

static UINT32 gm(int i)
{
  UINT32 x;
  int idx = i & 0xff;
  int idx2 = ((i>>8) & 0x1ff) | ((i>>9) & 0x200);
  int i1, i2;

  if(i & 0x008000)
    idx ^= 1;
  if(i & 0x100000)
    idx ^= 256;

  i1 = idx & 0xff;
  i2 = (i >> 8) & 0xff;

  x    = 0x41135012;

  if(bt(xmap_low_01, i1))
    x ^= 0x00c01000;
  if(bt(xmap_low_03, i1))
    x ^= 0x03000800;
  if(bt(xmap_low_07, i1))
    x ^= 0x00044000;
  if(bt(xmap_low_23, i1))
    x ^= 0x00102000;
  if(bt(xmap_low_31, i1))
    x ^= 0x00008000;

  if(bt(xmap_high_00, i2))
    x ^= 0x00000400;
  if(bt(xmap_high_02, i2))
    x ^= 0x00200020;
  if(bt(xmap_high_03, i2))
    x ^= 0x02000008;
  if(bt(xmap_high_04, i2))
    x ^= 0x10000200;
  if(bt(xmap_high_06, i2))
    x ^= 0x00000004;
  if(bt(xmap_high_21, i2))
    x ^= 0x80000001;
  if(bt(xmap_high_20, i2))
    x ^= 0x00100040;
  if(bt(xmap_high_10, i2))
    x ^= 0x40000100;
  if(bt(xmap_high_11, i2))
    x ^= 0x00800010;
  if(bt(xmap_high_13, i2))
    x ^= 0x00020080;
  if(bt(xmap_high_15, i2))
    x ^= 0x20000002;
  if(bt(xmap_high_16, i2))
    x ^= 0x00080000;

  if(i & 0x010000)
    x ^= 0xa200000f;
  if(i & 0x020000)
    x ^= 0x00ba00f0;
  if(i & 0x040000)
    x ^= 0x53000f00;
  if(i & 0x080000)
    x ^= 0x00d4f000;

  if(bt(zmap_2, idx2) && bt(xmap_low_03, i1))
    x ^= 0x08000000;

  if(bt(zmap_3, idx2))
    x ^= 0x08000000;

  if(bt(zmap_4, idx2&0x1ff) && bt(xmap_low_03, i1))
    x ^= 0x04000000;

  if(bt(zmap_5, idx2))
    x ^= 0x04000000;

  return x;
}

static UINT32 trans(UINT32 v, UINT32 x)
{
  UINT32 R = v^x, r = R;

  if((R & (1<<8)) && (v & (1<<30)))
    r ^= 1<<9;

  if((R & (1<<12)) && (v & (1<<22)))
    r ^= 1<<13;

  if((v & (1<<18)) && (x & (1<<14)))
    r ^= 1<<19;

  if((v & (1<<19)) && (x & (1<<6)))
    r ^= 1<<20;

  if((R & (1<<22)) && (x & (1<<22)))
    r ^= 1<<23;

  if((R & (1<<24)) && (x & (1<<24)))
    r ^= 1<<25;

  if((R & (1<<25)) && (v & (1<<3)))
    r ^= 1<<26;

  if((R & (1<<26)) && (x & (1<<26)))
    r ^= 1<<27;

  if((R & (1<<28)) && (v & (1<<28)))
    r ^= 1<<29;

  return r;
}

void raiden2_decrypt_sprites(void)
{
  int i;
  UINT32 *data = (UINT32 *)memory_region(REGION_GFX3);
  for(i=0; i<0x800000/4; i++) {
    UINT32 x1, v1, y1;

    int idx = i & 0xff;
    int i2;
    int idx2;

    idx2 = ((i>>7) & 0x3ff) | ((i>>8) & 0x400);
    if(i & 0x008000)
      idx ^= 1;
    if(i & 0x100000)
      idx ^= 256;

    i2 = i >> 8;

    v1 = sw(yrot(data[i], gr(i)));

    x1 = gm(i);

    y1 = ~trans(v1, x1);

    data[i] = y1;
  }
}



