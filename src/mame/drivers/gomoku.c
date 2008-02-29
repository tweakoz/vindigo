/******************************************************************************

    Gomoku Narabe Renju
    (c)1981 Nihon Bussan Co.,Ltd.

    Driver by Takahiro Nogi <nogi@kt.rim.or.jp> 1999/11/06 -
    Updated to compile again by David Haywood 19th Oct 2002

******************************************************************************/
/******************************************************************************

todo:

- BG(Go table) is generated by board circuitry, so not fully emulated.

- Couldn't figure out the method to specify palette, so I modified palette number manually.

- Couldn't figure out oneshot sound playback parameter. so I adjusted it manually.

******************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "sound/custom.h"


extern PALETTE_INIT( gomoku );
extern VIDEO_START( gomoku );
extern VIDEO_UPDATE( gomoku );

extern WRITE8_HANDLER( gomoku_videoram_w );
extern WRITE8_HANDLER( gomoku_colorram_w );
extern WRITE8_HANDLER( gomoku_bgram_w );
extern WRITE8_HANDLER( gomoku_flipscreen_w );
extern WRITE8_HANDLER( gomoku_bg_dispsw_w );

extern UINT8 *gomoku_videoram;
extern UINT8 *gomoku_colorram;
extern UINT8 *gomoku_bgram;

extern UINT8 *gomoku_soundregs1;
extern UINT8 *gomoku_soundregs2;
extern WRITE8_HANDLER( gomoku_sound1_w );
extern WRITE8_HANDLER( gomoku_sound2_w );

extern void *gomoku_sh_start(int clock, const struct CustomSound_interface *config);

/* input ports are rotated 90 degrees */
static READ8_HANDLER( input_port_r )
{
	int i, res;

	res = 0;
	for (i = 0; i < 8; i++)
		res |= ((readinputport(i) >> offset) & 1) << i;

	return res;
}


static ADDRESS_MAP_START( readmem_gomoku, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x47ff) AM_READ(MRA8_ROM)
	AM_RANGE(0x4800, 0x4fff) AM_READ(MRA8_RAM)
	AM_RANGE(0x5000, 0x53ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x5400, 0x57ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x5800, 0x58ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x7800, 0x7807) AM_READ(input_port_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_gomoku, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x47ff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x4800, 0x4fff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x5000, 0x53ff) AM_WRITE(gomoku_videoram_w) AM_BASE(&gomoku_videoram)
	AM_RANGE(0x5400, 0x57ff) AM_WRITE(gomoku_colorram_w) AM_BASE(&gomoku_colorram)
	AM_RANGE(0x5800, 0x58ff) AM_WRITE(gomoku_bgram_w) AM_BASE(&gomoku_bgram)
	AM_RANGE(0x6000, 0x601f) AM_WRITE(gomoku_sound1_w) AM_BASE(&gomoku_soundregs1)
	AM_RANGE(0x6800, 0x681f) AM_WRITE(gomoku_sound2_w) AM_BASE(&gomoku_soundregs2)
	AM_RANGE(0x7000, 0x7000) AM_WRITE(MWA8_NOP)
	AM_RANGE(0x7001, 0x7001) AM_WRITE(gomoku_flipscreen_w)
	AM_RANGE(0x7002, 0x7002) AM_WRITE(gomoku_bg_dispsw_w)
	AM_RANGE(0x7003, 0x7007) AM_WRITE(MWA8_NOP)
	AM_RANGE(0x7800, 0x7800) AM_WRITE(MWA8_NOP)
ADDRESS_MAP_END


static INPUT_PORTS_START( gomoku )
	PORT_START	/* 0 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_COCKTAIL

	PORT_START	/* 1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_DIPNAME (0x10, 0x10, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN3 )	/* service coin */
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN2 )

	PORT_START	/* 2 */
	PORT_SERVICE( 0x01, IP_ACTIVE_HIGH )
	PORT_DIPNAME( 0x06, 0x00, DEF_STR( Lives ))
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x06, "5" )
	PORT_DIPNAME( 0x08, 0x00, "Time" )
	PORT_DIPSETTING(    0x00, "60" )
	PORT_DIPSETTING(    0x08, "80" )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )

	PORT_START	/* 3 */
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* 4 */
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* 5 */
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* 6 */
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START	/* 7 */
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


static const gfx_layout charlayout =
{
	8, 8,		/* 8*8 characters */
	256,		/* 256 characters */
	2,			/* 2 bits per pixel */
	{ 0, 4 },	/* the two bitplanes are packed in one byte */
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8		/* every char takes 16 consecutive bytes */
};

static GFXDECODE_START( gomoku )
	GFXDECODE_ENTRY( REGION_GFX1, 0, charlayout, 0, 32 )
GFXDECODE_END


static const struct CustomSound_interface custom_interface =
{
	gomoku_sh_start
};

static MACHINE_DRIVER_START( gomoku )
	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 18432000/12)		 /* 1.536 MHz ? */
	MDRV_CPU_PROGRAM_MAP(readmem_gomoku, writemem_gomoku)
	MDRV_CPU_VBLANK_INT("main", irq0_line_hold)

	/* video hardware */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_SCREEN_VISIBLE_AREA(0, 256-1, 16, 256-16-1)

	MDRV_GFXDECODE(gomoku)
	MDRV_PALETTE_LENGTH(64)

	MDRV_PALETTE_INIT(gomoku)
	MDRV_VIDEO_START(gomoku)
	MDRV_VIDEO_UPDATE(gomoku)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(CUSTOM, 0)
	MDRV_SOUND_CONFIG(custom_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


ROM_START( gomoku )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	// program
	ROM_LOAD( "rj_1.7a",    0x0000, 0x1000, CRC(ed20d539) SHA1(7cbbc678cbe5c85b914ca44f82bdbd452cf694a0) )
	ROM_LOAD( "rj_2.7c",    0x1000, 0x1000, CRC(26a28516) SHA1(53d5d134cd91020fa06e380d355deb1df6b9cb6e) )
	ROM_LOAD( "rj_3.7d",    0x2000, 0x1000, CRC(d05db072) SHA1(9697c932c6dcee6f8536c9f0b3c84a719a7d3dee) )
	ROM_LOAD( "rj_4.7f",    0x3000, 0x1000, CRC(6e3d1c18) SHA1(e2f7e4c0de3c78d1b8e686152458972f996b023a) )
	ROM_LOAD( "rj_5.4e",    0x4000, 0x0800, CRC(eaf541b4) SHA1(bc7e7ec1ba68f71ab9ac86f9ae77971ddb9ce3a4) )

	ROM_REGION( 0x1000, REGION_GFX1, ROMREGION_DISPOSE )	// text char
	ROM_LOAD( "rj_6.4r",    0x0000, 0x1000, CRC(ed26ae36) SHA1(61cb73d7f2568e88e1c2981e7af3e9a3b26797d3) )

	ROM_REGION( 0x1000, REGION_SOUND1, 0 )	// sound
	ROM_LOAD( "rj_7.3c",    0x0000, 0x1000, CRC(d1ed1365) SHA1(4ef08f26fe7df4c400f72e09e56d8825d584f55f) )

	ROM_REGION( 0x0040, REGION_PROMS, 0 )
	ROM_LOAD( "rj_prom.1m", 0x0000, 0x0020, CRC(5da2f2bd) SHA1(4355ccf06cb09ec3240dc92bda19b1f707a010ef) )	// TEXT color
	ROM_LOAD( "rj_prom.1l", 0x0020, 0x0020, CRC(fe4ef393) SHA1(d4c63f8645afeadd13ff82087bcc497d8936d90b) )	// BG color

	ROM_REGION( 0x0100, REGION_USER1, 0 )	// BG draw data X
	ROM_LOAD( "rj_prom.8n", 0x0000, 0x0100, CRC(9ba43222) SHA1(a443df49d7ee9dbfd258b09731d392bf1249cbfa) )

	ROM_REGION( 0x0100, REGION_USER2, 0 )	// BG draw data Y
	ROM_LOAD( "rj_prom.7p", 0x0000, 0x0100, CRC(5b5464f8) SHA1(b945efb8a7233f501d67f6b1be4e9d4967dc6719) )

	ROM_REGION( 0x0100, REGION_USER3, 0 )	// BG character data
	ROM_LOAD( "rj_prom.7r", 0x0000, 0x0100, CRC(3004585a) SHA1(711b68140827f0f3dc71f2576fcf9b905c999e8d) )

	ROM_REGION( 0x0020, REGION_USER4, 0 )	// unknown
	ROM_LOAD( "rj_prom.9k", 0x0000, 0x0020, CRC(cff72923) SHA1(4f61375028ab62da46ed119bc81052f5f98c28d4) )
ROM_END


//    YEAR,     NAME,   PARENT,  MACHINE,    INPUT,     INIT,    MONITOR,      COMPANY, FULLNAME
GAME( 1981,   gomoku,        0,   gomoku,   gomoku,        0,      ROT90, "Nichibutsu", "Gomoku Narabe Renju", 0 )
