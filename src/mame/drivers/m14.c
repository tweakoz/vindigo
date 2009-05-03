/***************************************************************************

M14 Hardware (c) 1979 Irem

driver by Angelo Salese

TODO:
- Inputs / Outputs;
- Sound (very likely to be discrete);
- What are the high 4 bits in the colorram for?

============================================================================
x (Mystery Rom)
(c)1978-1981? Irem?
PCB No.	:M14S-2
	:M14L-2
CPU	:NEC D8085AC
Sound	:?
OSC	:6MHz x2

mgpb1.bin
mgpa2.bin
mgpa3.bin
mgpa4.bin
mgpa5.bin
mgpb6.bin
mgpa7.bin
mgpb8.bin

mgpa9.bin
mgpa10.bin


--- Team Japump!!! ---
http://japump.i.am/
Dumped by Chackn
01/30/2000

***************************************************************************/

#include "driver.h"
#include "cpu/i8085/i8085.h"

VIDEO_START( m14 )
{

}

VIDEO_UPDATE( m14 )
{
	const gfx_element *gfx = screen->machine->gfx[0];
	int count = 0;

	int y,x;


	for (y=0;y<32;y++)
	{
		for (x=0;x<32;x++)
		{
			int tile = videoram[count];
			int colour = colorram[count] & 0x0f;
			/* bits 4-7? */

			drawgfx(bitmap,gfx,tile,colour,0,0,x*8,y*8,cliprect,TRANSPARENCY_NONE,0);

			count++;
		}
	}
	return 0;
}

static READ8_HANDLER( test_r )
{
	return mame_rand(space->machine);
}

static ADDRESS_MAP_START( m14_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0x2000, 0x23ff) AM_RAM
	AM_RANGE(0xe000, 0xe3ff) AM_RAM AM_BASE(&videoram)
	AM_RANGE(0xe400, 0xe7ff) AM_RAM AM_BASE(&colorram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( m14_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0xf8, 0xf8) AM_READ(test_r)
	AM_RANGE(0xf9, 0xf9) AM_READ(test_r)
	AM_RANGE(0xfa, 0xfa) AM_READ(test_r)
	AM_RANGE(0xfb, 0xfb) AM_READ(test_r)
ADDRESS_MAP_END

static INPUT_PORTS_START( m14 )
INPUT_PORTS_END

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	1,
	{ RGN_FRAC(0,1) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static GFXDECODE_START( m14 )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,     0, 0x10 )
GFXDECODE_END

static PALETTE_INIT( m14 )
{
	int i;

	for (i = 0; i < 0x20; i++)
	{
		rgb_t color;

		if (i & 0x01)
			color = MAKE_RGB(pal1bit(i >> 1), pal1bit(i >> 2), pal1bit(i >> 3));
		else if(i & 0x10)
			color = RGB_WHITE;
		else
			color = RGB_BLACK;


		palette_set_color(machine, i, color);
	}
}

static INTERRUPT_GEN( m14_irq )
{
	cpu_set_input_line(device, I8085_RST75_LINE, ASSERT_LINE);
	cpu_set_input_line(device, I8085_RST75_LINE, CLEAR_LINE);
}

static MACHINE_DRIVER_START( m14 )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu",8085A,8000000)
	MDRV_CPU_PROGRAM_MAP(m14_map,0)
	MDRV_CPU_IO_MAP(m14_io_map,0)
	MDRV_CPU_VBLANK_INT("screen",m14_irq)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 0*8, 28*8-1)
	MDRV_GFXDECODE(m14)
	MDRV_PALETTE_LENGTH(0x20)
	MDRV_PALETTE_INIT(m14)

	MDRV_VIDEO_START(m14)
	MDRV_VIDEO_UPDATE(m14)

	/* sound hardware */
//	MDRV_SPEAKER_STANDARD_MONO("mono")
//	MDRV_SOUND_ADD("ay", AY8910, 8000000/4 /* guess */)
//	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)
MACHINE_DRIVER_END

/***************************************************************************

  Game driver(s)

***************************************************************************/


ROM_START( ptmj )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "mgpb1.bin",   0x0000, 0x0400, CRC(47c041b8) SHA1(e834c375e689f99a13964863fc9847a8e148ec91) )
	ROM_LOAD( "mgpa2.bin",   0x0400, 0x0400, CRC(cf8bfa23) SHA1(091055e803255f1b5520f50b31af7135d71d0a40) )
	ROM_LOAD( "mgpa3.bin",   0x0800, 0x0400, CRC(a07a3093) SHA1(5b86bb11e83c06f828956e7db6dd2c105b023b03) )
	ROM_LOAD( "mgpa4.bin",   0x0c00, 0x0400, CRC(a420241c) SHA1(7497d90014dabb49f9db1d5d8e3014c634045725) )
	ROM_LOAD( "mgpa5.bin",   0x1000, 0x0400, CRC(a2df92a3) SHA1(97a3d4b188d26f172881f8cf86bdd83d549f5b74) )
	ROM_LOAD( "mgpb6.bin",   0x1400, 0x0400, CRC(f5c0fcd4) SHA1(14e2d04be105caeb221dfc226f84cb1722ae2627) )
	ROM_LOAD( "mgpa7.bin",   0x1800, 0x0400, CRC(56ed7eb2) SHA1(acf954bf4daadb225475937dd3c36baa996f7b65) )
	ROM_LOAD( "mgpb8.bin",   0x1c00, 0x0400, CRC(3ecb8214) SHA1(8575bcb49f693aa8798b8a7d9a76392bfcc90e0e) )

	ROM_REGION( 0x0800, "gfx1", ROMREGION_DISPOSE )
	ROM_LOAD( "mgpa9.bin",   0x0000, 0x0400, CRC(cb68b4ec) SHA1(2cf596affb155ae38729fcd95cae424073faf74d) )
	ROM_LOAD( "mgpa10.bin",  0x0400, 0x0400, CRC(e1a4ebdc) SHA1(d9df42424ede17f0634d8d0a56c0374a33c55333) )
ROM_END

GAME( 1979, ptmj,  0,       m14,  m14,  0, ROT0, "Irem", "PT Mahjong Game (Japan)", GAME_NO_SOUND|GAME_NOT_WORKING )
