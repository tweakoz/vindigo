/* Skill Fruit Bonus
 -- (unknown) encrypted CPU
 --  16-bit (program appears interleaved)
*/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/okim6295.h"

static UINT8* sfbonus_videoram;

VIDEO_START(sfbonus)
{

}

VIDEO_UPDATE(sfbonus)
{
	int y,x;
	int count = 0;
	const gfx_element *gfx = screen->machine->gfx[0];
	const gfx_element *gfx2 = screen->machine->gfx[1];

	for (y=0;y<64;y++)
	{
		for (x=0;x<128;x++)
		{
			UINT16 tiledat = sfbonus_videoram[count] | (sfbonus_videoram[count+1]<<8);

			drawgfx(bitmap,gfx,tiledat,0,0,0,x*8,y*8,cliprect,TRANSPARENCY_NONE,0);
			count+=2;
		}

	}
	
	for (y=0;y<32;y++)
	{
		for (x=0;x<64;x++)
		{
			UINT16 tiledat = sfbonus_videoram[count] | (sfbonus_videoram[count+1]<<8);

			drawgfx(bitmap,gfx2,tiledat,0,0,0,x*8,y*32,cliprect,TRANSPARENCY_PEN,255);
			count+=2;
		}

	}	
	
	return 0;
}


static WRITE8_HANDLER( paletteram_io_w )
{
	static int pal_offs,r,g,b,internal_pal_offs;

	switch(offset)
	{
		case 0:
			pal_offs = data;
			break;
		case 2:
			internal_pal_offs = 0;
			break;
		case 1:
			switch(internal_pal_offs)
			{
				case 0:
					r = ((data & 0x3f) << 2) | ((data & 0x30) >> 4);
					internal_pal_offs++;
					break;
				case 1:
					g = ((data & 0x3f) << 2) | ((data & 0x30) >> 4);
					internal_pal_offs++;
					break;
				case 2:
					b = ((data & 0x3f) << 2) | ((data & 0x30) >> 4);
					palette_set_color(space->machine, pal_offs, MAKE_RGB(r, g, b));
					internal_pal_offs = 0;
					pal_offs++;
					break;
			}

			break;
	}
}

static WRITE8_HANDLER( sfbonus_videoram_w )
{
	sfbonus_videoram[offset] = data;

}

static ADDRESS_MAP_START( sfbonus_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xefff) AM_ROMBANK(1) AM_WRITE(sfbonus_videoram_w)
	AM_RANGE(0xf000, 0xffff) AM_RAM
ADDRESS_MAP_END

static READ8_HANDLER( sfbonus_unk_r )
{
	return mame_rand(space->machine);
}

static WRITE8_HANDLER( sfbonus_bank_w )
{
	UINT8 *ROM = memory_region(space->machine, "main");
	UINT8 bank;

	bank = data & 3;

	memory_set_bankptr(space->machine, 1, &ROM[bank * 0x10000]);
}

static ADDRESS_MAP_START( sfbonus_io, ADDRESS_SPACE_IO, 8 )
	AM_RANGE(0x0400, 0x0400) AM_READ_PORT("IN0")
	AM_RANGE(0x0408, 0x0408) AM_READ_PORT("IN1")
	AM_RANGE(0x0410, 0x0410) AM_READ_PORT("IN2")

	AM_RANGE(0x0438, 0x0438) AM_READ_PORT("IN3")

	AM_RANGE(0x0c00, 0x0c03) AM_WRITE( paletteram_io_w )

	AM_RANGE(0x2400, 0x241f) AM_RAM

	AM_RANGE(0x2800, 0x2800) AM_READ(sfbonus_unk_r)
	AM_RANGE(0x2801, 0x2801) AM_READ(sfbonus_unk_r)	AM_WRITE(SMH_NOP)

	AM_RANGE(0x2c00, 0x2c00) AM_READ(sfbonus_unk_r)
	AM_RANGE(0x2c01, 0x2c01) AM_READ(sfbonus_unk_r) AM_WRITE(SMH_NOP)

	AM_RANGE(0x3400, 0x3400) AM_WRITE(sfbonus_bank_w)
	AM_RANGE(0x3800, 0x3800) AM_READ(sfbonus_unk_r) AM_WRITE(SMH_NOP)

	AM_RANGE(0x3801, 0x3801) AM_WRITE(SMH_NOP)
	AM_RANGE(0x3802, 0x3802) AM_WRITE(SMH_NOP)
	AM_RANGE(0x3803, 0x3803) AM_WRITE(SMH_NOP)
	AM_RANGE(0x3806, 0x3806) AM_WRITE(SMH_NOP)
	AM_RANGE(0x3807, 0x3807) AM_WRITE(SMH_NOP)
ADDRESS_MAP_END

static INPUT_PORTS_START( sfbonus )
	PORT_START("IN0")
	PORT_DIPNAME( 0x01, 0x01, "IN0" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x01, "IN1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Test button")  PORT_CODE(KEYCODE_Z)
	PORT_DIPNAME( 0x02, 0x02, "IN2" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("IN3")
	PORT_DIPNAME( 0x01, 0x01, "IN3" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static const gfx_layout sfbonus_layout =
{
	8,8,
	RGN_FRAC(1,1),
	8,
	{ 0,1,2,3,4,5,6,7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	{ 0*64,1*64,2*64,3*64,4*64,5*64,6*64,7*64 },
	8*64
};

static const gfx_layout sfbonus32_layout =
{
	8,32,
	RGN_FRAC(1,1),
	8,
	{ 0,1,2,3,4,5,6,7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	{ 0*64,1*64,2*64,3*64,4*64,5*64,6*64,7*64,
      8*64,9*64,10*64,11*64,12*64,13*64,14*64,15*64,
	  16*64,17*64,18*64,19*64,20*64,21*64,22*64,23*64,
	  24*64,25*64,26*64,27*64,28*64,29*64,30*64,31*64
	  },
	32*64
};



static GFXDECODE_START( sfbonus )
	GFXDECODE_ENTRY( "gfx1", 0, sfbonus_layout,   0x0, 2  )
	GFXDECODE_ENTRY( "gfx2", 0, sfbonus32_layout,   0x0, 2  )
GFXDECODE_END


static MACHINE_RESET( sfbonus )
{
	UINT8 *ROM = memory_region(machine, "main");

	memory_set_bankptr(machine, 1, &ROM[0]);
}

static MACHINE_DRIVER_START( sfbonus )
	MDRV_CPU_ADD("main", Z80, 16000000) // unknown CPU
	MDRV_CPU_PROGRAM_MAP(0,sfbonus_map)
	MDRV_CPU_IO_MAP(0,sfbonus_io)
	MDRV_CPU_VBLANK_INT("main",irq0_line_hold)
//	MDRV_CPU_PERIODIC_INT(nmi_line_pulse,100)

	MDRV_MACHINE_RESET( sfbonus )

	MDRV_GFXDECODE(sfbonus)

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(128*8, 128*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 128*8-1, 0*8, 128*8-1)

	MDRV_PALETTE_LENGTH(0x100)

	MDRV_VIDEO_START(sfbonus)
	MDRV_VIDEO_UPDATE(sfbonus)

//  MDRV_SPEAKER_STANDARD_STEREO("left", "right")
//  MDRV_SOUND_ADD("oki", OKIM6295, 1000000)
//  MDRV_SOUND_CONFIG(okim6295_interface_pin7high) // clock frequency & pin 7 not verified
//  MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 0.47)
//  MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 0.47)
MACHINE_DRIVER_END

// the gfx2 roms might be swapped on these sets
ROM_START( sfbonus )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80  Code */
	ROM_LOAD( "skfb16.bin", 0x00000, 0x40000, CRC(bfd53646) SHA1(bd58f8c6d5386649a6fc0f4bac46d1b6cd6248b1) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "skfbrom2.bin", 0x00000, 0x20000, CRC(3823a36e) SHA1(4136e380b63546b9490033ad26d776f326eb9290) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "skfbrom3.bin", 0x00000, 0x80000, CRC(36119517) SHA1(241bb256ab3ba595dcb0c81fd2e60ed35dd7c197) )
	ROM_LOAD16_BYTE( "skfbrom4.bin", 0x00001, 0x80000, CRC(a655bac1) SHA1(0faea01c09409f9182f08370dcc0b466a799f17f) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "skfbrom5.bin", 0x00000, 0x80000, CRC(752e6e3b) SHA1(46c3a1bbbf1a2afe36fa5333b6e74459e17e9bae) )
	ROM_LOAD16_BYTE( "skfbrom6.bin", 0x00001, 0x80000, CRC(30df6b6a) SHA1(7a180fa8ee64b9efb0321baffad72f0a9485d568) )
ROM_END

ROM_START( sfbonusa )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80  Code */
	ROM_LOAD( "skfb17.bin", 0x00000, 0x40000, CRC(e28ede82) SHA1(f320c4c9c30ec280ee2437d1ad4d2b6270580916) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "skfbrom2.bin", 0x00000, 0x20000, CRC(3823a36e) SHA1(4136e380b63546b9490033ad26d776f326eb9290) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "skfbrom3.bin", 0x00000, 0x80000, CRC(36119517) SHA1(241bb256ab3ba595dcb0c81fd2e60ed35dd7c197) )
	ROM_LOAD16_BYTE( "skfbrom4.bin", 0x00001, 0x80000, CRC(a655bac1) SHA1(0faea01c09409f9182f08370dcc0b466a799f17f) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "skfbrom5a.bin", 0x00000, 0x80000, CRC(ed07a635) SHA1(a31ee06d8cb78c43affa9d899e9cec87a0875934) )
	ROM_LOAD16_BYTE( "skfbrom6a.bin", 0x00001, 0x80000, CRC(b9a39e5a) SHA1(cb875db25d8bccf48542bd6dbecc474b1205563b) )
ROM_END

ROM_START( parrot3 )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "p4p24.bin", 0x00000, 0x40000, CRC(356a49c8) SHA1(7e0ed7d1063675b66bfe28c427712249654be6ab) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "p4rom2.bin", 0x00000, 0x40000, CRC(d0574efc) SHA1(dd6628450883f0f723744e7caf6525bca7b18a43) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "p4rom3.bin", 0x00000, 0x80000, CRC(c5fc21cb) SHA1(b4137a97611ff688fbfa688eb3108622bed8da5b) )
	ROM_LOAD16_BYTE( "p4rom4.bin", 0x00001, 0x80000, CRC(bbe174d3) SHA1(75d964d37470843962419ead170f1db9a1dcc4c4) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "p4rom5.bin", 0x00000, 0x80000, CRC(5e184b6e) SHA1(a00eb5a62246ec00e1af6e8c0629a118f71f0c58) )
	ROM_LOAD16_BYTE( "p4rom6.bin", 0x00001, 0x80000, CRC(598d2117) SHA1(8391054aa8deb8480a69de97b8f5316e7864ed2d) )
ROM_END

ROM_START( hldspin1 )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "hs1p25t.bin", 0x00000, 0x40000, CRC(0fce5691) SHA1(4920ee490fdd690987bee92525b48596a051f83d) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "hs_2.bin", 0x00000, 0x40000, CRC(3b0f5374) SHA1(7e7b185b62d1a321e2853b4b08e8ee2aa54933f5) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "hs1_3.bin", 0x00000, 0x40000, CRC(85a016cb) SHA1(abb32c0191a531706593088b2ecfb48ceb02a127)  )
	ROM_LOAD16_BYTE( "hs1_4.bin", 0x00001, 0x40000, CRC(4313c099) SHA1(620452ac607b044ce4c8a5a7b03bc831125c81eb) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "hs_5.bin", 0x00000, 0x40000, CRC(09931910) SHA1(cac792f7c67d0ea274ecb369cef0554a033e8d88) )
	ROM_LOAD16_BYTE( "hs_6.bin", 0x00001, 0x40000, CRC(801703e4) SHA1(7da822b03a6d4f53a49bb1fedc9e1262d8a84782) )
ROM_END

ROM_START( hldspin2 )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "hs2p26.bin", 0x00000, 0x40000, CRC(35844d85) SHA1(cd9bd3a95d1aaf4171bc9c57dec45b59fcc11902) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "hs_2.bin", 0x00000, 0x40000, CRC(3b0f5374) SHA1(7e7b185b62d1a321e2853b4b08e8ee2aa54933f5) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "hs2_3.bin", 0x00000, 0x40000, CRC(b6890061) SHA1(c196f8740d8487b108cff58d77a203b2d8431a67) )
	ROM_LOAD16_BYTE( "hs2_4.bin", 0x00001, 0x40000, CRC(132a2312) SHA1(9ad5c3a3ca895d290ff584f605f05d70386cfa10) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "hs_5.bin", 0x00000, 0x40000, CRC(09931910) SHA1(cac792f7c67d0ea274ecb369cef0554a033e8d88) )
	ROM_LOAD16_BYTE( "hs_6.bin", 0x00001, 0x40000, CRC(801703e4) SHA1(7da822b03a6d4f53a49bb1fedc9e1262d8a84782) )
ROM_END

ROM_START( pickwin )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "pw25t.bin", 0x00000, 0x40000, CRC(9b6bd032) SHA1(241c772d191841c72e973d5dc494be445d6fd668) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "pw-2.bin", 0x00000, 0x40000, CRC(3b0f5374) SHA1(7e7b185b62d1a321e2853b4b08e8ee2aa54933f5) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "pw-3.bin", 0x00000, 0x40000, CRC(9a27acbd) SHA1(3a3a6d0b5f6eeeebcb2ccaff773b8971b3bfd3c7) )
	ROM_LOAD16_BYTE( "pw-4.bin", 0x00001, 0x40000, CRC(6b629619) SHA1(3842a7579f67866aa9744b0d6fa6a47c923be6fe) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "pw-5.bin", 0x00000, 0x40000, CRC(ec2ac284) SHA1(35cfab27db3740823b3cba821bd178d28e0be5f8) )
	ROM_LOAD16_BYTE( "pw-6.bin", 0x00001, 0x40000, CRC(aba36d00) SHA1(5abb1fe7d4f212fa0f7d5314f76e7c6b07e6c4bb) )
ROM_END

ROM_START( pickwina )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "pw26.bin", 0x00000, 0x40000, CRC(9bedbe5a) SHA1(fb9ee63932b5f86fe42f84a5e1b8a3c29194761b) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "pw-2.bin", 0x00000, 0x40000, CRC(3b0f5374) SHA1(7e7b185b62d1a321e2853b4b08e8ee2aa54933f5) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "pw-3.bin", 0x00000, 0x40000, CRC(9a27acbd) SHA1(3a3a6d0b5f6eeeebcb2ccaff773b8971b3bfd3c7) )
	ROM_LOAD16_BYTE( "pw-4.bin", 0x00001, 0x40000, CRC(6b629619) SHA1(3842a7579f67866aa9744b0d6fa6a47c923be6fe) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "pw-5.bin", 0x00000, 0x40000, CRC(ec2ac284) SHA1(35cfab27db3740823b3cba821bd178d28e0be5f8) )
	ROM_LOAD16_BYTE( "pw-6.bin", 0x00001, 0x40000, CRC(aba36d00) SHA1(5abb1fe7d4f212fa0f7d5314f76e7c6b07e6c4bb) )
ROM_END

ROM_START( tighook )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "thk17.bin", 0x00000, 0x40000, CRC(0e27d3dd) SHA1(c85e2e03c36e0f6ec95e15597a6bd58e8eeb6353) )
	ROM_LOAD( "thk17xt.bin", 0x00000, 0x40000, CRC(02ca5fe2) SHA1(daa66d5ef7336e311cc8bb78ec6625620b9b2800) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "thkrom2.bin", 0x00000, 0x40000, CRC(61b61b75) SHA1(e71c5ab6aedb7ca4db32a2f4d7d2818dcdd92417) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "thkrom3.bin", 0x00000, 0x80000, CRC(ec4b4144) SHA1(36df0686b405a3c99707a6b63ad14bff1cd7b443) )
	ROM_LOAD16_BYTE( "thkrom4.bin", 0x00001, 0x80000, CRC(dbd1c526) SHA1(1f82f3e132bb5ac598e5d95254de48357130a0a0) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "thkrom5.bin", 0x00000, 0x80000, CRC(4085e345) SHA1(38c8e4727a782630527141e1586fcceff1d07d76) )
	ROM_LOAD16_BYTE( "thkrom6.bin", 0x00001, 0x80000, CRC(637695ff) SHA1(a5707b545968ac9d41c1a4ffd4de60a9df4bcbf1) )
ROM_END

ROM_START( robadv )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "ra15.bin", 0x00000, 0x40000, CRC(dd7e4ec9) SHA1(038b03855eaa8be1a97e34534822465a10886e10) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "rarom2.bin", 0x00000, 0x40000, CRC(092392cb) SHA1(fd52a0c4f46cb3242bf1b9e35ad5f41cda64010b) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "rarom3.bin", 0x00000, 0x80000, CRC(94e4cd71) SHA1(5c01e276dea3df7c367210af3d0d2399935c81c6) )
	ROM_LOAD16_BYTE( "rarom4.bin", 0x00001, 0x80000, CRC(72cfec99) SHA1(6612b8d04c0cc97dc5315fda861b606a6c158ea6) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "rarom5.bin", 0x00000, 0x80000, CRC(9bf41c2b) SHA1(8cc8ca5c2c63223e670e00ca5802b8677856bc16) )
	ROM_LOAD16_BYTE( "rarom6.bin", 0x00001, 0x80000, CRC(0fb69b4c) SHA1(8e1aaf5ade707b4045d55ff64e72cfe5db696332) )
ROM_END

ROM_START( anibonus )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "ab14.bin", 0x00000, 0x40000, CRC(d1dcb6e6) SHA1(4a95184e5d4f2e0527fdc8f29e56572cf3ba9987) )
	ROM_LOAD( "ab14a.bin", 0x00000, 0x40000, CRC(a8a0eea5) SHA1(c37a470b997ee5dbc976858c024bd67ed88061ce) )
	ROM_LOAD( "ab14xt.bin", 0x00000, 0x40000,  CRC(c6107445) SHA1(22fd3a7987219a940b965c953494939e0892661e) )
	ROM_LOAD( "ab14xta.bin", 0x00000, 0x40000,  CRC(eddf38af) SHA1(56a920ba1af213719210d25e6d8b5c7a0d513119) )
	ROM_LOAD( "ab15.bin", 0x00000, 0x40000, CRC(4640a2e7) SHA1(2659c037e88f43f89a5d8cd563eec5e4eb2025b9) )
	ROM_LOAD( "ab15xt.bin", 0x00000, 0x40000,  CRC(3aed6e7f) SHA1(51f9af92286e8b2fcfeae30913fbab4626decb99) )

	ROM_REGION( 0x040000, "oki", ROMREGION_ERASE00 ) /* Samples */
	/* None? */

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "abrom3.bin", 0x00000, 0x80000, CRC(85f19e19) SHA1(2dd259af132e0cfd34974526c96f0d96ff868516) )
	ROM_LOAD16_BYTE( "abrom4.bin", 0x00001, 0x80000, CRC(c12b954c) SHA1(83556abbad0f285360da5f5e0fb93514d46b436b) )

	ROM_LOAD16_BYTE( "abrom3a.bin", 0x00000, 0x80000, CRC(85f19e19) SHA1(2dd259af132e0cfd34974526c96f0d96ff868516) )
	ROM_LOAD16_BYTE( "abrom4a.bin", 0x00001, 0x80000, CRC(c12b954c) SHA1(83556abbad0f285360da5f5e0fb93514d46b436b) )

	ROM_LOAD16_BYTE( "abrom3n.bin", 0x00000, 0x80000, CRC(aab2161a) SHA1(d472746c68720935fedfc6b2d06a4fe1152cc804) )
	ROM_LOAD16_BYTE( "abrom4n.bin", 0x00001, 0x80000, CRC(d776862c) SHA1(03b3c0e9adb11b560b8773e88ea97e712323f25e) )

	
	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "abrom5.bin", 0x00000, 0x80000, CRC(74b4fa88) SHA1(922d9c4f864be2b125269a69639e6206aec26d72) )
	ROM_LOAD16_BYTE( "abrom6.bin", 0x00001, 0x80000, CRC(e8f4b079) SHA1(2597fa17b6a13e634ba9fe846661d09c65fa8cf2) )

	ROM_LOAD16_BYTE( "abrom5a.bin", 0x00000, 0x80000, CRC(9810f1e2) SHA1(a10954a46d52c5a53a3b11a04e66c4ed3ce2a0f7) )
	ROM_LOAD16_BYTE( "abrom6a.bin", 0x00001, 0x80000, CRC(22d2abbe) SHA1(65d82ed0fc799c4248696f1b2ef76e7e88bf7fb7) )

	ROM_LOAD16_BYTE( "abrom5n.bin", 0x00000, 0x80000, CRC(d3db86eb) SHA1(e7e2cdfa6b4795d4021f589d2a292c67cc32f03a) )
	ROM_LOAD16_BYTE( "abrom6n.bin", 0x00001, 0x80000, CRC(0d8dcaa1) SHA1(a74c64bb89b4273e9d1e092786a5cf8ebd60477c) )
ROM_END

ROM_START( abnudge )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "ab17n.bin", 0x00000, 0x40000, CRC(aca7c2af) SHA1(8f23b4aff006fcd983769f833c2fabdbb087d36b) )

	ROM_REGION( 0x040000, "oki", ROMREGION_ERASE00 ) /* Samples */
	/* None? */

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "abrom3n.bin", 0x00000, 0x80000, CRC(aab2161a) SHA1(d472746c68720935fedfc6b2d06a4fe1152cc804) )
	ROM_LOAD16_BYTE( "abrom4n.bin", 0x00001, 0x80000, CRC(d776862c) SHA1(03b3c0e9adb11b560b8773e88ea97e712323f25e) )

	
	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "abrom5n.bin", 0x00000, 0x80000, CRC(d3db86eb) SHA1(e7e2cdfa6b4795d4021f589d2a292c67cc32f03a) )
	ROM_LOAD16_BYTE( "abrom6n.bin", 0x00001, 0x80000, CRC(0d8dcaa1) SHA1(a74c64bb89b4273e9d1e092786a5cf8ebd60477c) )
ROM_END

ROM_START( anithunt )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "ath15.bin", 0x00000, 0x40000, CRC(917ae674) SHA1(67808a9d3bd48a8f7f839eb85356269a357581ad) )
	ROM_LOAD( "ath17.bin", 0x00000, 0x40000, CRC(07facf55) SHA1(2de5ca12e06a6896099672ec7383e6324d23fa12) )

	ROM_REGION( 0x040000, "oki", ROMREGION_ERASE00 ) /* Samples */
	/* None? */

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "ath-rom3.bin", 0x00000, 0x80000, CRC(2ce266b2) SHA1(34dcc504d48a26976e17ad0b8399904e5ecc3379) )
	ROM_LOAD16_BYTE( "ath-rom4.bin", 0x00001, 0x80000, CRC(59d25672) SHA1(212ba0aa7794b7a37121896190e64069f005b1ea) )

	ROM_LOAD16_BYTE( "athrom3.bin", 0x00000, 0x80000, CRC(f784ec01) SHA1(69474fc9d10882fd9ec0c02675193df7aa31f6a7) )
	ROM_LOAD16_BYTE( "athrom4.bin", 0x00001, 0x80000, CRC(49749939) SHA1(6deb10c2b51b5718f0cba31f6bda54bcc001bc71) )

	
	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "ath-rom5.bin", 0x00000, 0x80000, CRC(536a7e23) SHA1(51dc6b2b022a672810b00e1006b0c7ee610a4e4f) )
	ROM_LOAD16_BYTE( "ath-rom6.bin", 0x00001, 0x80000, CRC(23bc5067) SHA1(1e279e58437b897c7a68c9cdd15277c6a906a142) )
ROM_END


ROM_START( dblchal )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "dc11.bin", 0x00000, 0x40000, CRC(05a27f07) SHA1(02b7b2731f8821bd7e0e3be005bd3024db0a7e42) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "dcrom2.bin", 0x00000, 0x20000, CRC(ce099327) SHA1(b1dc43839e9e3bf788141d58c81e1380a12d582a) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "dcrom3.bin", 0x00000, 0x40000, CRC(50b1c522) SHA1(620f8a1df6954c5db4a85448c810901d69859fec) )
	ROM_LOAD16_BYTE( "dcrom4.bin", 0x00001, 0x40000, CRC(a6f46957) SHA1(b4c0a28e428b9fc091ac5fc041a1ce01f65ff402) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "dcrom5.bin", 0x00000, 0x80000, CRC(a55f4ed3) SHA1(9f7427357af84026c056624523fd20bc556f3c22) )
	ROM_LOAD16_BYTE( "dcrom6.bin", 0x00001, 0x80000, CRC(cf783d82) SHA1(d3f8ae5cb3a5f848e2d84721a5a4ee486a52de85) )
ROM_END

ROM_START( robadv2 )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "r2a15.bin", 0x00000, 0x40000, CRC(e1932e13) SHA1(918d51e64aefaa308f92748bb5bfa92b88e00feb) )
	ROM_LOAD( "r2a15sh.bin", 0x00000, 0x40000, CRC(c53af9be) SHA1(86cb2dae1315227f01f430d23fb4e09d015f1206) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "rarom2.bin", 0x00000, 0x40000, CRC(092392cb) SHA1(fd52a0c4f46cb3242bf1b9e35ad5f41cda64010b) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "ra2rom3.bin", 0x00000, 0x80000, CRC(eacd0cf7) SHA1(a04ddc339d330be4b278f12a54fe65d7eb08ffd0) )
	ROM_LOAD16_BYTE( "ra2rom4.bin", 0x00001, 0x80000, CRC(adac68d2) SHA1(6287a979a57004b1c4eea7f539550e23bac22904) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "ra2rom5.bin", 0x00000, 0x80000, CRC(ad9379a2) SHA1(93126969677cfce20e5b2e287662ac6b2ceee425) )
	ROM_LOAD16_BYTE( "ra2rom6.bin", 0x00001, 0x80000, CRC(12312874) SHA1(9d4d9d9fbec8536e8a003892643654d15f4535fa) )
ROM_END

ROM_START( pirpok2 )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "p3p20.bin", 0x00000, 0x40000, CRC(0e477094) SHA1(cd35c9ac1ed4b843886b1fc554e749f38573ca21) )

	ROM_REGION( 0x040000, "oki", 0 ) /* Samples */
	ROM_LOAD( "p3rom2.bin", 0x00000, 0x20000, CRC(db6182e4) SHA1(65f05247629d5a1f37bf179f468acf8420342d2c) )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "p3rom3.bin", 0x00000, 0x80000, CRC(34d3e6c5) SHA1(1d89677605188f135c8dbbc2ab20510cae7548fe) )
	ROM_LOAD16_BYTE( "p3rom4.bin", 0x00001, 0x80000, CRC(3861b5fb) SHA1(72f085a1fd951919f479e5c6984304f7bbddc054) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "p3rom5.bin", 0x00000, 0x80000, CRC(c5eca135) SHA1(bdaccd32e1434016c77579bc8c4214ab2a3ae474) )
	ROM_LOAD16_BYTE( "p3rom6.bin", 0x00001, 0x80000, CRC(d990cbb8) SHA1(6f822e38bf401b2eb0b2e36f3b4fc6822fafd3fa) )
ROM_END





ROM_START( fcnudge )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "fc17n.bin", 0x00000, 0x40000, CRC(b9193d4f) SHA1(5ed77802e5a8f246eb1a559c13ad544adae35201) )

	ROM_REGION( 0x040000, "oki", ROMREGION_ERASE00 ) /* Samples */
	/* none? */

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "fcrom3n.bin", 0x00000, 0x80000, CRC(3d1c3d7f) SHA1(bcb20c08a0a2a36775052ae45258862afc00d61d) )
	ROM_LOAD16_BYTE( "fcrom4n.bin", 0x00001, 0x80000, CRC(a047861e) SHA1(b5d160c25945c7c103160e80d545cb3e1091e631) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "fcrom5n.bin", 0x00000, 0x80000, CRC(42955842) SHA1(383be3049da5b10ea57a278bc6578ece046058fd) )
	ROM_LOAD16_BYTE( "fcrom6n.bin", 0x00001, 0x80000, CRC(eee0f84d) SHA1(4ac096ccea258710f58c8121e7f0af28593d6368) )
ROM_END

ROM_START( sfruitb )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "sfb18xt.bin", 0x00000, 0x40000, CRC(15a7fc47) SHA1(4f1af0bab7807a69f8c67c8e83b35c8c5c2a13f1) )
	ROM_LOAD( "sfb20.bin", 0x00000, 0x40000, CRC(73a2be7f) SHA1(95b51a63ede10247fde944d980d85781947a8435) )
	ROM_LOAD( "sfb20b.bin", 0x00000, 0x40000, CRC(6fe1b8ba) SHA1(46fe3940d80578f3818702fd449fc4119ea5fc30) )

	ROM_REGION( 0x040000, "oki", ROMREGION_ERASE00 ) /* Samples */
	/* none? */

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "sfbrom3.bin", 0x00000, 0x80000, CRC(b48eb491) SHA1(0369873231ffa3fb78863623209ad1e05222fc8a) )
	ROM_LOAD16_BYTE( "sfbrom4.bin", 0x00001, 0x80000, CRC(a307119c) SHA1(b45a0e73d4e2d665de634dbf0034b3dcc9152b3d) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "sfbrom5.bin", 0x00000, 0x80000, CRC(31588ff3) SHA1(6e2a65d50457ec0e93a647fd8ca5ebeeb16bdb1c) )
	ROM_LOAD16_BYTE( "sfbrom6.bin", 0x00001, 0x80000, CRC(232d6216) SHA1(ca7780adc85fa570698736785ad700797e6a98fb) )
ROM_END

ROM_START( fb2gen )
	ROM_REGION( 0x80000, "main", 0 ) /* Z80 Code */
	ROM_LOAD( "fb2g15r.bin", 0x00000, 0x40000, CRC(a8daf67d) SHA1(6e980748ec77c4842676f14ffffe3f630879e9d9) )
	ROM_LOAD( "fb2g16xt.bin", 0x00000, 0x40000, CRC(ea525ebb) SHA1(965bba045ba69ac4316b27d0d69b130119f9ce04) )

	ROM_REGION( 0x040000, "oki", ROMREGION_ERASE00 ) /* Samples */
	ROM_LOAD( "fb2grom2.bin", 0x00000, 0x40000, CRC(1cbbd43a) SHA1(6e31c3bdd677d9d3cb445294cf17a0efcb16d4ed) )
	//ROM_LOAD( "fb2grom2a.bin", 0x00000, 0x40000, CRC(1cbbd43a) SHA1(6e31c3bdd677d9d3cb445294cf17a0efcb16d4ed))

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD16_BYTE( "fb2grom3.bin", 0x00000, 0x80000, CRC(a4f33c67) SHA1(ec7f539725b2684add019c1dad3f230b5c798daa) )
	ROM_LOAD16_BYTE( "fb2grom4.bin", 0x00001, 0x80000, CRC(c142f2af) SHA1(3323de8cd09b64c1c8ccf51acf74444e577fdfb3) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "fb2grom5.bin", 0x00000, 0x80000, CRC(1c4172a8) SHA1(c45a57cd799681d442de02f8f07dbd9751929ca4) )
	ROM_LOAD16_BYTE( "fb2grom6.bin", 0x00001, 0x80000, CRC(953fdcc4) SHA1(c57e2b4a8273e789b96d39fe28d02bec5359b5f4) )
ROM_END



static DRIVER_INIT( sfbonus )
{
	int i;
	UINT8 *ROM = memory_region(machine, "main");

	for(i=0;i<0x40000;i++)
	{
		UINT8 x = ROM[i];

		switch(i & 7)
		{
			case 0: x = BITSWAP8(x^0x2a, 1,3,7,6,5,2,0,4); break;
			case 1: x = BITSWAP8(x^0xe4, 3,7,6,5,2,0,4,1); break;
			case 2: x = BITSWAP8(x^0x2d, 4,1,3,7,6,5,2,0); break;
			case 3: x = BITSWAP8(x^0xba, 4,3,0,2,1,7,6,5); break;
			case 4: x = BITSWAP8(x^0x30, 2,1,7,6,5,0,3,4); break;
			case 5: x = BITSWAP8(x^0xf1, 2,7,6,5,1,3,4,0); break;
			case 6: x = BITSWAP8(x^0x3d, 2,1,4,7,6,5,3,0); break;
			case 7: x = BITSWAP8(x^0xba, 4,3,0,1,2,7,6,5); break;
		}

		ROM[i] = x;
	}

	sfbonus_videoram = auto_malloc(0x10000);
	state_save_register_global_pointer(machine, sfbonus_videoram, 0x10000);

}

GAME( 199?, sfbonus,     0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Skill Fruit Bonus (set 1)", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 199?, sfbonusa,    sfbonus,  sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Skill Fruit Bonus (set 2)", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 199?, parrot3,     0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Parrot Poker III", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, hldspin1,    0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Hold & Spin I", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, hldspin2,    0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Hold & Spin II", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, fcnudge,     0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Fruit Carnival Nudge", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, pickwin,     0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Pick & Win (set 1)", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, pickwina,    pickwin,  sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Pick & Win (set 2)", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, tighook,     0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Tiger Hook", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, robadv,      0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Robin Adventure", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, robadv2,     0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Robin Adventure 2", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, pirpok2,     0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Pirate Poker II", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, anibonus,    0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Animal Bonus", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, abnudge,     0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Animal Bonus Nudge", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, dblchal,     0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Double Challenge", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, anithunt,    0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Animal Treasure Hunt", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, sfruitb,     0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Super Fruit Bonus", GAME_NOT_WORKING|GAME_NO_SOUND )
GAME( 2000, fb2gen,      0,        sfbonus,    sfbonus,    sfbonus, ROT0,  "Amcoe", "Fruit Bonus 2nd Generation", GAME_NOT_WORKING|GAME_NO_SOUND )


