/***************************************************************************

  Vapor Trail (World version)  (c) 1989 Data East Corporation
  Vapor Trail (USA version)    (c) 1989 Data East USA
  Kuhga (Japanese version)     (c) 1989 Data East Corporation

  Emulation by Bryan McPhail, mish@tendril.co.uk
 added pal & prom-maps - Highwayman.
***************************************************************************/

#include "driver.h"
#include "cpu/h6280/h6280.h"
#include "sound/2203intf.h"
#include "sound/2151intf.h"
#include "sound/okim6295.h"
#include "deco16ic.h"

VIDEO_START( vaportra );
VIDEO_UPDATE( vaportra );

WRITE16_HANDLER( vaportra_priority_w );
WRITE16_HANDLER( vaportra_palette_24bit_rg_w );
WRITE16_HANDLER( vaportra_palette_24bit_b_w );

/******************************************************************************/

static WRITE16_HANDLER( vaportra_sound_w )
{
	/* Force synchronisation between CPUs with fake timer */
	timer_call_after_resynch(NULL, 0, NULL);
	soundlatch_w(machine,0,data & 0xff);
	cpunum_set_input_line(machine, 1,0,ASSERT_LINE);
}

static READ16_HANDLER( vaportra_control_r )
{
	switch (offset<<1)
	{
		case 4: /* Dip Switches */
			return (input_port_read_indexed(machine, 4) + (input_port_read_indexed(machine, 3) << 8));
		case 2: /* Credits */
			return input_port_read_indexed(machine, 2);
		case 0: /* Player 1 & Player 2 joysticks & fire buttons */
			return (input_port_read_indexed(machine, 0) + (input_port_read_indexed(machine, 1) << 8));
	}

	logerror("Unknown control read at %d\n",offset);
	return ~0;
}

/******************************************************************************/

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x100000, 0x100003) AM_WRITE(vaportra_priority_w)
	AM_RANGE(0x100006, 0x100007) AM_WRITE(vaportra_sound_w)
	AM_RANGE(0x100000, 0x10000f) AM_READ(vaportra_control_r)
	AM_RANGE(0x200000, 0x201fff) AM_RAM_WRITE(deco16_pf3_data_w) AM_BASE(&deco16_pf3_data)
	AM_RANGE(0x202000, 0x203fff) AM_RAM_WRITE(deco16_pf4_data_w) AM_BASE(&deco16_pf4_data)
	AM_RANGE(0x240000, 0x24000f) AM_WRITE(SMH_RAM) AM_BASE(&deco16_pf34_control)
	AM_RANGE(0x280000, 0x281fff) AM_RAM_WRITE(deco16_pf1_data_w) AM_BASE(&deco16_pf1_data)
	AM_RANGE(0x282000, 0x283fff) AM_RAM_WRITE(deco16_pf2_data_w) AM_BASE(&deco16_pf2_data)
	AM_RANGE(0x2c0000, 0x2c000f) AM_WRITE(SMH_RAM) AM_BASE(&deco16_pf12_control)
	AM_RANGE(0x300000, 0x3009ff) AM_RAM_WRITE(vaportra_palette_24bit_rg_w) AM_BASE(&paletteram16)
	AM_RANGE(0x304000, 0x3049ff) AM_RAM_WRITE(vaportra_palette_24bit_b_w) AM_BASE(&paletteram16_2)
	AM_RANGE(0x308000, 0x308001) AM_NOP
	AM_RANGE(0x30c000, 0x30c001) AM_WRITE(buffer_spriteram16_w)
	AM_RANGE(0xff8000, 0xff87ff) AM_RAM AM_BASE(&spriteram16) AM_SIZE(&spriteram_size)
	AM_RANGE(0xffc000, 0xffffff) AM_RAM
ADDRESS_MAP_END


/******************************************************************************/

static READ8_HANDLER( vaportra_soundlatch_r )
{
	cpunum_set_input_line(machine, 1,0,CLEAR_LINE);
	return soundlatch_r(machine, offset);
}

static WRITE8_HANDLER( YM2151_w )
{
	switch (offset) {
	case 0:
		YM2151_register_port_0_w(machine,0,data);
		break;
	case 1:
		YM2151_data_port_0_w(machine,0,data);
		break;
	}
}

static WRITE8_HANDLER( YM2203_w )
{
	switch (offset) {
	case 0:
		YM2203_control_port_0_w(machine,0,data);
		break;
	case 1:
		YM2203_write_port_0_w(machine,0,data);
		break;
	}
}

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x000000, 0x00ffff) AM_ROM
	AM_RANGE(0x100000, 0x100001) AM_READWRITE(YM2203_status_port_0_r, YM2203_w)
	AM_RANGE(0x110000, 0x110001) AM_READWRITE(YM2151_status_port_0_r, YM2151_w)
	AM_RANGE(0x120000, 0x120001) AM_READWRITE(OKIM6295_status_0_r, OKIM6295_data_0_w)
	AM_RANGE(0x130000, 0x130001) AM_READWRITE(OKIM6295_status_1_r, OKIM6295_data_1_w)
	AM_RANGE(0x140000, 0x140001) AM_READ(vaportra_soundlatch_r)
	AM_RANGE(0x1f0000, 0x1f1fff) AM_READWRITE(SMH_BANK8,SMH_BANK8)  /* ??? LOOKUP ??? */
	AM_RANGE(0x1fec00, 0x1fec01) AM_WRITE(H6280_timer_w)
	AM_RANGE(0x1ff400, 0x1ff403) AM_WRITE(H6280_irq_status_w)
ADDRESS_MAP_END

/******************************************************************************/

static INPUT_PORTS_START( vaportra )
	PORT_START	/* Player 1 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START	/* Player 2 controls */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START	/* Credits */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_VBLANK )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* Dip switch bank 1 */
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START	/* Dip switch bank 2 */
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x02, "4" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x20, "150k, 300k and 600k" )
	PORT_DIPSETTING(    0x30, "200k and 600k" )
	PORT_DIPSETTING(    0x10, "300k only" )
	PORT_DIPSETTING(    0x00, DEF_STR( None ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Allow_Continue ) )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Yes ) )
  	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

/******************************************************************************/

static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(0,2)+8,RGN_FRAC(0,2)+0,RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	8*16
};

static const gfx_layout tilelayout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(0,2)+8,RGN_FRAC(0,2)+0,RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0 },
	{ 256,257,258,259,260,261,262,263,0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,8*16,9*16,10*16,11*16,12*16,13*16,14*16,15*16 },
	32*16
};



static GFXDECODE_START( vaportra )
	GFXDECODE_ENTRY( REGION_GFX1, 0x000000, charlayout,    0x000, 0x500 )	/* Characters 8x8 */
	GFXDECODE_ENTRY( REGION_GFX1, 0x000000, tilelayout,    0x000, 0x500 )	/* Tiles 16x16 */
	GFXDECODE_ENTRY( REGION_GFX2, 0x000000, tilelayout,    0x000, 0x500 )	/* Tiles 16x16 */ // ok
	GFXDECODE_ENTRY( REGION_GFX3, 0x000000, tilelayout,    0x100, 16 )	/* Sprites 16x16 */
GFXDECODE_END

/******************************************************************************/

static void sound_irq(running_machine *machine, int state)
{
	cpunum_set_input_line(machine, 1,1,state); /* IRQ 2 */
}

static const struct YM2151interface ym2151_interface =
{
	sound_irq
};



static MACHINE_DRIVER_START( vaportra )

	/* basic machine hardware */
	MDRV_CPU_ADD("main", M68000,12000000) /* Custom chip 59 */
	MDRV_CPU_PROGRAM_MAP(main_map,0)
	MDRV_CPU_VBLANK_INT("main", irq6_line_hold)

	MDRV_CPU_ADD("audio", H6280, 32220000/4) /* Custom chip 45; Audio section crystal is 32.220 MHz */
	MDRV_CPU_PROGRAM_MAP(sound_map,0)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_BUFFERS_SPRITERAM)

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(58)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(529))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 1*8, 31*8-1)
	MDRV_GFXDECODE(vaportra)
	MDRV_PALETTE_LENGTH(1280)

	MDRV_VIDEO_START(vaportra)
	MDRV_VIDEO_UPDATE(vaportra)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ym1", YM2203, 32220000/8)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.60)

	MDRV_SOUND_ADD("ym2", YM2151, 32220000/9)
	MDRV_SOUND_CONFIG(ym2151_interface)
	MDRV_SOUND_ROUTE(0, "mono", 0.60)
	MDRV_SOUND_ROUTE(1, "mono", 0.60)

	MDRV_SOUND_ADD("oki1", OKIM6295, 32220000/32)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1_pin7high)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.75)

	MDRV_SOUND_ADD("oki2", OKIM6295, 32220000/16)
	MDRV_SOUND_CONFIG(okim6295_interface_region_2_pin7high)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.60)
MACHINE_DRIVER_END

/******************************************************************************/

ROM_START( vaportra )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 code */
  	ROM_LOAD16_BYTE( "fl_02-1.bin", 0x00000, 0x20000, CRC(9ae36095) SHA1(c8d11a6033a44277a267915b4ca471c43acd1143) )
  	ROM_LOAD16_BYTE( "fl_00-1.bin", 0x00001, 0x20000, CRC(c08cc048) SHA1(b28f95856817b8a8cb6cc588d48e95196cbf52fd) )
	ROM_LOAD16_BYTE( "fl_03.bin",   0x40000, 0x20000, CRC(80bd2844) SHA1(3fcaa409c7134388fa9458df8e8aaecc93f085e6) )
 	ROM_LOAD16_BYTE( "fl_01.bin",   0x40001, 0x20000, CRC(9474b085) SHA1(5510309ddab5fbf1dbb0a7b1e424a5dff5ec263d) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "fj04",    0x00000, 0x10000, CRC(e9aedf9b) SHA1(f7bcf8f666015140aaad8ee5cf619636934b7066) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "vtmaa00.bin",   0x000000, 0x80000, CRC(0330e13b) SHA1(dce70667ea738295332556752d1305c5e941b383) ) /* chars & tiles */

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "vtmaa02.bin",   0x000000, 0x80000, CRC(091ff98e) SHA1(814dc08c055bad5368955a4b1fe6a706b58adc02) ) /* tiles 3 */
  	ROM_LOAD( "vtmaa01.bin",   0x080000, 0x80000, CRC(c217a31b) SHA1(e259d48190d6890781fb0338e17e14822876babb) ) /* tiles 2 */

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE )
  	ROM_LOAD( "vtmaa03.bin",   0x000000, 0x80000, CRC(1a30bf81) SHA1(00e6c713e12133a99d64ca80638c9cbc8e26b2c8) ) /* sprites */
  	ROM_LOAD( "vtmaa04.bin",   0x080000, 0x80000, CRC(b713e9cc) SHA1(af33943d75d2ee3a7385f624537008dca9e1d5d8) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "fj06",    0x00000, 0x20000, CRC(6e98a235) SHA1(374564b4e494d03cd1330c06e321b9452c22a075) )

	ROM_REGION( 0x20000, REGION_SOUND2, 0 )	/* ADPCM samples */
	ROM_LOAD( "fj05",    0x00000, 0x20000, CRC(39cda2b5) SHA1(f5c5a305025d451ab48f84cd63e36a3bbdefda96) )

	ROM_REGION( 0x200, REGION_PROMS, 0 )
	ROM_LOAD( "fj-27.bin",    0x00000, 0x00200, CRC(65045742) SHA1(5dfb6c85a70b208cd16d3bf8ec1897e77f4a9b7d) )

	ROM_REGION( 0x0a00, REGION_PLDS, ROMREGION_DISPOSE )
	ROM_LOAD( "pal16l8a.6l",  0x0000, 0x0104, CRC(ee748e8f) SHA1(6ffe8b11f076305e82f64e0a12b76ffe725ce345) )
	ROM_LOAD( "pal16l8b.13g", 0x0200, 0x0104, CRC(6da13bda) SHA1(d7bade089d87015e1e95fbf3f292db4688ee4624) )
	ROM_LOAD( "pal16l8b.13h", 0x0400, 0x0104, CRC(62a9e098) SHA1(7b7c371c040d250d41fde021d191d62ce95bfc20) )
	ROM_LOAD( "pal16l8b.14g", 0x0600, 0x0104, CRC(036768aa) SHA1(96185989031e0a9b38ff29bf4cf6162482d33964) )
	ROM_LOAD( "pal16l8b.14h", 0x0800, 0x0104, CRC(bf421fce) SHA1(e8b0895b1fe99a3d5b3dcca004a7bfd1a09766b2) )
ROM_END

ROM_START( vaportru )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 code */
  	ROM_LOAD16_BYTE( "fj02",   0x00000, 0x20000, CRC(a2affb73) SHA1(0d49397cc9891047a0b92e92e2e3d0e7fcaf8db9) )
  	ROM_LOAD16_BYTE( "fj00",   0x00001, 0x20000, CRC(ef05e07b) SHA1(0e505709fa251e6b30f019c0c28ee9ba2b29a50a) )
	ROM_LOAD16_BYTE( "fj03",   0x40000, 0x20000, CRC(44893379) SHA1(da1340bc1821a552c317cb9a7c1ba69eb080b055) )
 	ROM_LOAD16_BYTE( "fj01",   0x40001, 0x20000, CRC(97fbc107) SHA1(b2899eb4347c0471397b83051e46c94dff3526f5) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "fj04",    0x00000, 0x10000, CRC(e9aedf9b) SHA1(f7bcf8f666015140aaad8ee5cf619636934b7066) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "vtmaa00.bin",   0x000000, 0x80000, CRC(0330e13b) SHA1(dce70667ea738295332556752d1305c5e941b383) ) /* chars & tiles */

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "vtmaa02.bin",   0x000000, 0x80000, CRC(091ff98e) SHA1(814dc08c055bad5368955a4b1fe6a706b58adc02) ) /* tiles 3 */
  	ROM_LOAD( "vtmaa01.bin",   0x080000, 0x80000, CRC(c217a31b) SHA1(e259d48190d6890781fb0338e17e14822876babb) ) /* tiles 2 */

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE )
  	ROM_LOAD( "vtmaa03.bin",   0x000000, 0x80000, CRC(1a30bf81) SHA1(00e6c713e12133a99d64ca80638c9cbc8e26b2c8) ) /* sprites */
  	ROM_LOAD( "vtmaa04.bin",   0x080000, 0x80000, CRC(b713e9cc) SHA1(af33943d75d2ee3a7385f624537008dca9e1d5d8) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "fj06",    0x00000, 0x20000, CRC(6e98a235) SHA1(374564b4e494d03cd1330c06e321b9452c22a075) )

	ROM_REGION( 0x20000, REGION_SOUND2, 0 )	/* ADPCM samples */
	ROM_LOAD( "fj05",    0x00000, 0x20000, CRC(39cda2b5) SHA1(f5c5a305025d451ab48f84cd63e36a3bbdefda96) )

	ROM_REGION( 0x200, REGION_PROMS, 0 )
	ROM_LOAD( "fj-27.bin",    0x00000, 0x00200, CRC(65045742) SHA1(5dfb6c85a70b208cd16d3bf8ec1897e77f4a9b7d) )

	ROM_REGION( 0x0a00, REGION_PLDS, ROMREGION_DISPOSE )
	ROM_LOAD( "pal16l8a.6l",  0x0000, 0x0104, CRC(ee748e8f) SHA1(6ffe8b11f076305e82f64e0a12b76ffe725ce345) )
	ROM_LOAD( "pal16l8b.13g", 0x0200, 0x0104, CRC(6da13bda) SHA1(d7bade089d87015e1e95fbf3f292db4688ee4624) )
	ROM_LOAD( "pal16l8b.13h", 0x0400, 0x0104, CRC(62a9e098) SHA1(7b7c371c040d250d41fde021d191d62ce95bfc20) )
	ROM_LOAD( "pal16l8b.14g", 0x0600, 0x0104, CRC(036768aa) SHA1(96185989031e0a9b38ff29bf4cf6162482d33964) )
	ROM_LOAD( "pal16l8b.14h", 0x0800, 0x0104, CRC(bf421fce) SHA1(e8b0895b1fe99a3d5b3dcca004a7bfd1a09766b2) )
ROM_END

ROM_START( kuhga )
	ROM_REGION( 0x80000, REGION_CPU1, 0 ) /* 68000 code */
  	ROM_LOAD16_BYTE( "fp02-3.bin", 0x00000, 0x20000, CRC(d0705ef4) SHA1(781efbf36d9dda543895e0a59cd4d72667439a93) )
  	ROM_LOAD16_BYTE( "fp00-3.bin", 0x00001, 0x20000, CRC(1da92e48) SHA1(6507bd9bbc31ee03e38b82cc135aebf090902761) )
	ROM_LOAD16_BYTE( "fp03.bin",   0x40000, 0x20000, CRC(ea0da0f1) SHA1(ca40e694cb0aa0c13672c14fd4a389bc6d26cbc6) )
 	ROM_LOAD16_BYTE( "fp01.bin",   0x40001, 0x20000, CRC(e3ecbe86) SHA1(382e959111ec37ad94da8fd6dcefe2d2aab346b6) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* Sound CPU */
	ROM_LOAD( "fj04",    0x00000, 0x10000, CRC(e9aedf9b) SHA1(f7bcf8f666015140aaad8ee5cf619636934b7066) )

	ROM_REGION( 0x080000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "vtmaa00.bin",   0x000000, 0x80000, CRC(0330e13b) SHA1(dce70667ea738295332556752d1305c5e941b383) ) /* chars & tiles */

	ROM_REGION( 0x100000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "vtmaa02.bin",   0x000000, 0x80000, CRC(091ff98e) SHA1(814dc08c055bad5368955a4b1fe6a706b58adc02) ) /* tiles 3 */
  	ROM_LOAD( "vtmaa01.bin",   0x080000, 0x80000, CRC(c217a31b) SHA1(e259d48190d6890781fb0338e17e14822876babb) ) /* tiles 2 */

	ROM_REGION( 0x100000, REGION_GFX3, ROMREGION_DISPOSE )
  	ROM_LOAD( "vtmaa03.bin",   0x000000, 0x80000, CRC(1a30bf81) SHA1(00e6c713e12133a99d64ca80638c9cbc8e26b2c8) ) /* sprites */
  	ROM_LOAD( "vtmaa04.bin",   0x080000, 0x80000, CRC(b713e9cc) SHA1(af33943d75d2ee3a7385f624537008dca9e1d5d8) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* ADPCM samples */
	ROM_LOAD( "fj06",    0x00000, 0x20000, CRC(6e98a235) SHA1(374564b4e494d03cd1330c06e321b9452c22a075) )

	ROM_REGION( 0x20000, REGION_SOUND2, 0 )	/* ADPCM samples */
	ROM_LOAD( "fj05",    0x00000, 0x20000, CRC(39cda2b5) SHA1(f5c5a305025d451ab48f84cd63e36a3bbdefda96) )

	ROM_REGION( 0x200, REGION_PROMS, 0 )
	ROM_LOAD( "fj-27.bin",    0x00000, 0x00200, CRC(65045742) SHA1(5dfb6c85a70b208cd16d3bf8ec1897e77f4a9b7d) )
ROM_END

/*
Pals
----
Fuse Plot - 1
?

PAL16L8/A/A-2/A-4
*
DD PAL16L8/A/A-2/A-4*
QP20*
QF2048*
G0*
F0*
L0     11111111111111111111111111111111*
L32    10111011101110111111111111111111*
L64    00000000000000000000000000000000*
L96    00000000000000000000000000000000*
L128   00000000000000000000000000000000*
L160   00000000000000000000000000000000*
L192   00000000000000000000000000000000*
L224   00000000000000000000000000000000*
L256   11111111111111111111111111111111*
L288   10101011101101111111111111111111*
L320   00000000000000000000000000000000*
L352   00000000000000000000000000000000*
L384   00000000000000000000000000000000*
L416   00000000000000000000000000000000*
L448   00000000000000000000000000000000*
L480   00000000000000000000000000000000*
L512   11111111111111111111111111111111*
L544   10011011101101111111111111111111*
L576   00000000000000000000000000000000*
L608   00000000000000000000000000000000*
L640   00000000000000000000000000000000*
L672   00000000000000000000000000000000*
L704   00000000000000000000000000000000*
L736   00000000000000000000000000000000*
L768   11111111111111111111111111111111*
L800   01011011101101111111111111111111*
L832   00000000000000000000000000000000*
L864   00000000000000000000000000000000*
L896   00000000000000000000000000000000*
L928   00000000000000000000000000000000*
L960   00000000000000000000000000000000*
L992   00000000000000000000000000000000*
L1024  11111111111111111111111111111111*
L1056  01101011101101111111111111111111*
L1088  00000000000000000000000000000000*
L1120  00000000000000000000000000000000*
L1152  00000000000000000000000000000000*
L1184  00000000000000000000000000000000*
L1216  00000000000000000000000000000000*
L1248  00000000000000000000000000000000*
L1280  11111111111111111111111111111111*
L1312  10100111101101111111111111111111*
L1344  00000000000000000000000000000000*
L1376  00000000000000000000000000000000*
L1408  00000000000000000000000000000000*
L1440  00000000000000000000000000000000*
L1472  00000000000000000000000000000000*
L1504  00000000000000000000000000000000*
L1536  11111111111111111111111111111111*
L1568  11111111111111111011111110111111*
L1600  00000000000000000000000000000000*
L1632  00000000000000000000000000000000*
L1664  00000000000000000000000000000000*
L1696  00000000000000000000000000000000*
L1728  00000000000000000000000000000000*
L1760  00000000000000000000000000000000*
L1792  11111111111111111111111111111111*
L1824  11111111111111111111101110111111*
L1856  00000000000000000000000000000000*
L1888  00000000000000000000000000000000*
L1920  00000000000000000000000000000000*
L1952  00000000000000000000000000000000*
L1984  00000000000000000000000000000000*
L2016  00000000000000000000000000000000*
C3E44*
?

Fuse Plot - TD0
?

PAL16L8B/D/H-15
*
DD PAL16L8B/D/H-15*
QP20*
QF2048*
G0*
F0*
L0     11111111111111111111111111111111*
L32    10101111111111111111111111111111*
L64    00000000000000000000000000000000*
L96    00000000000000000000000000000000*
L128   00000000000000000000000000000000*
L160   00000000000000000000000000000000*
L192   00000000000000000000000000000000*
L224   00000000000000000000000000000000*
L256   11111111111111111111111111111111*
L288   11111111111110111111111111111111*
L320   11111111111111111011111111111111*
L352   11111111111111111111101111111111*
L384   00000000000000000000000000000000*
L416   00000000000000000000000000000000*
L448   00000000000000000000000000000000*
L480   00000000000000000000000000000000*
L512   11111111111111111111111111111111*
L544   11111111111111111111011111101111*
L576   00000000000000000000000000000000*
L608   00000000000000000000000000000000*
L640   00000000000000000000000000000000*
L672   00000000000000000000000000000000*
L704   00000000000000000000000000000000*
L736   00000000000000000000000000000000*
L768   11111111111111111111111111111111*
L800   11111111111111111111101111101111*
L832   00000000000000000000000000000000*
L864   00000000000000000000000000000000*
L896   00000000000000000000000000000000*
L928   00000000000000000000000000000000*
L960   00000000000000000000000000000000*
L992   00000000000000000000000000000000*
L1024  11111111111111111111111111111111*
L1056  10011111101111111111111111111111*
L1088  00000000000000000000000000000000*
L1120  00000000000000000000000000000000*
L1152  00000000000000000000000000000000*
L1184  00000000000000000000000000000000*
L1216  00000000000000000000000000000000*
L1248  00000000000000000000000000000000*
L1280  11111111111111111111111111111111*
L1312  01011111101111111111111111111111*
L1344  00000000000000000000000000000000*
L1376  00000000000000000000000000000000*
L1408  00000000000000000000000000000000*
L1440  00000000000000000000000000000000*
L1472  00000000000000000000000000000000*
L1504  00000000000000000000000000000000*
L1536  11111111111111111111111111111111*
L1568  01101111101111111111111111111111*
L1600  00000000000000000000000000000000*
L1632  00000000000000000000000000000000*
L1664  00000000000000000000000000000000*
L1696  00000000000000000000000000000000*
L1728  00000000000000000000000000000000*
L1760  00000000000000000000000000000000*
L1792  11111111111111111111111111111111*
L1824  10100111111111111111111111111111*
L1856  11111111111111111111111110111011*
L1888  10011111101111111111111111111111*
L1920  01011111101111111111111111111111*
L1952  00000000000000000000000000000000*
L1984  00000000000000000000000000000000*
L2016  00000000000000000000000000000000*
C52BB*
?

Fuse Plot - TD1
?

PAL16L8B/D/H-15
*
DD PAL16L8B/D/H-15*
QP20*
QF2048*
G0*
F0*
L0     11111111111111111111111111111111*
L32    10111011111111111111111111111111*
L64    00000000000000000000000000000000*
L96    00000000000000000000000000000000*
L128   00000000000000000000000000000000*
L160   00000000000000000000000000000000*
L192   00000000000000000000000000000000*
L224   00100000000000000000000000000000*
L256   11111111111111111111111111111111*
L288   11101111101110111111111111111111*
L320   00000000000000000000000000000000*
L352   00000000000000000000000000000000*
L384   00000000000000000000000000000000*
L416   00000000000000000000000000000000*
L448   00000000000000000000000000000000*
L480   00000000000000000000000000000000*
L512   11111111111111111111111111111111*
L544   11101111011110111111111111111111*
L576   00000000000000000000000000000000*
L608   00000000000000000000000000000000*
L640   00000000000000000000000000000000*
L672   00000000000000000000000000000000*
L704   00000000000000000000000000000000*
L736   00100000000000000000000000000000*
L768   11111111111111111111111111111111*
L800   10111111101110111111111111111111*
L832   00000000000000000000000000000000*
L864   00000000000000000000000000000000*
L896   00000000000000000000000000000000*
L928   00000000000000000000000000000000*
L960   00000000000000000000000000000000*
L992   00000000000000000000000000000000*
L1024  11111111111111111111111111111111*
L1056  10111111011110111111111111111111*
L1088  00000000000000000000000000000000*
L1120  00000000000000000000000000000000*
L1152  00000000000000000000000000000000*
L1184  00000000000000000000000000000000*
L1216  00000000000000000000000000000000*
L1248  00000000000000000000000000000000*
L1280  11111111111111111111111111111111*
L1312  11100111111111111111111111111111*
L1344  00000000000000000000000000000000*
L1376  00000000000000000000000000000000*
L1408  00000000000000000000000000000000*
L1440  00000000000000000000000000000000*
L1472  00000000000000000000000000000000*
L1504  00000000000000000000000000000000*
L1536  11111111111111111111111111111111*
L1568  10110111111111111111111111111111*
L1600  00000000000000000000000000000000*
L1632  00000000000000000000000000000000*
L1664  00000000000000000000000000000000*
L1696  00000000000000000000000000000000*
L1728  00000000000000000000000000000000*
L1760  00000000000000000000000000000000*
L1792  11111111111111111111111111111111*
L1824  11101011111111111111111111111111*
L1856  00000000000000000000000000000000*
L1888  00000000000000000000000000000000*
L1920  00000000000000000000000000000000*
L1952  00000000000000000000000000000000*
L1984  00000000000000000000000000000000*
L2016  00000000000000000000000000000000*
C3EBA*
?

Fuse Plot - TD2
?

PAL16L8B/D/H-15
*
DD PAL16L8B/D/H-15*
QP20*
QF2048*
G0*
F0*
L0     11111111111111111111111111111111*
L32    11110111011110110111111111111111*
L64    00000000000000000000000000000000*
L96    00000000000000000000000000000000*
L128   00000000000000000000000000000000*
L160   00000000000000000000000000000000*
L192   00000000000000000000000000000000*
L224   00000000000000000000000000000000*
L256   11111111111111111111111111111111*
L288   11111011011110110111111111111111*
L320   00000000000000000000000000000000*
L352   00000000000000000000000000000000*
L384   00000000000000000000000000000000*
L416   00000000000000000000000000000000*
L448   00000000000000000000000000000000*
L480   00000000000000000000000000000000*
L512   11111111111111111111111111111111*
L544   11111011101110111011111111111111*
L576   00000000000000000000000000000000*
L608   00000000000000000000000000000000*
L640   00000000000000000000000000000000*
L672   00000000000000000000000000000000*
L704   00000000000000000000000000000000*
L736   00000000000000000000000000000000*
L768   11111111111111111111111111111111*
L800   11110111101110111011111111111111*
L832   00000000000000000000000000000000*
L864   00000000000000000000000000000000*
L896   00000000000000000000000000000000*
L928   00000000000000000000000000000000*
L960   00000000000000000000000000000000*
L992   00000000000000000000000000000000*
L1024  11111111111111111111111111111111*
L1056  11111011011110111011111111111111*
L1088  00000000000000000000000000000000*
L1120  00000000000000000000000000000000*
L1152  00000000000000000000000000000000*
L1184  00000000000000000000000000000000*
L1216  00000000000000000000000000000000*
L1248  00000000000000000000000000000000*
L1280  11111111111111111111111111111111*
L1312  11110111011110111011111111111111*
L1344  00000000000000000000000000000000*
L1376  00000000000000000000000000000000*
L1408  00000000000000000000000000000000*
L1440  00000000000000000000000000000000*
L1472  00000000000000000000000000000000*
L1504  00000000000000000000000000000000*
L1536  11111011101101111011111111111111*
L1568  11101111111111111111111111111111*
L1600  00000000000000000000000000000000*
L1632  00000000000000000000000000000000*
L1664  00000000000000000000000000000000*
L1696  00000000000000000000000000000000*
L1728  00000000000000000000000000000000*
L1760  00000000000000000000000000000000*
L1792  11111011101101111011111111111111*
L1824  10111111111111111111111111111111*
L1856  00000000000000000000000000000000*
L1888  00000000000000000000000000000000*
L1920  00000000000000000000000000000000*
L1952  00000000000000000000000000000000*
L1984  00000000000000000000000000000000*
L2016  00000000000000000000000000000000*
C3DEC*
?

Fuse Plot - TD3
?

PAL16L8B/D/H-15
*
DD PAL16L8B/D/H-15*
QP20*
QF2048*
G0*
F0*
L0     11111111111111111111111111111111*
L32    11111010111111111111111111111111*
L64    11111101111111111111111111111111*
L96    00000000000000000000000000000000*
L128   00000000000000000000000000000000*
L160   00000000000000000000000000000000*
L192   00000000000000000000000000000000*
L224   00000000000000000000000000000000*
L256   11111111111111111111111111111111*
L288   11111011111111111111111111111111*
L320   10111111111111111111111111111111*
L352   00000000000000000000000000000000*
L384   00000000000000000000000000000000*
L416   00000000000000000000000000000000*
L448   00000000000000000000000000000000*
L480   00000000000000000000000000000000*
L512   11111111111111111111111111111111*
L544   11111110111111111111111110111111*
L576   00000000000000000000000000000000*
L608   00000000000000000000000000000000*
L640   00000000000000000000000000000000*
L672   00000000000000000000000000000000*
L704   00000000000000000000000000000000*
L736   00000000000000000000000000000000*
L768   11111111111111111111111111111111*
L800   11111110111111111111101111111111*
L832   00000000000000000000000000000000*
L864   00000000000000000000000000000000*
L896   00000000000000000000000000000000*
L928   00000000000000000000000000000000*
L960   00000000000000000000000000000000*
L992   00000000000000000000000000000000*
L1024  11111111111111111111111111111111*
L1056  11111011111111111011111111111111*
L1088  00000000000000000000000000000000*
L1120  00000000000000000000000000000000*
L1152  00000000000000000000000000000000*
L1184  00000000000000000000000000000000*
L1216  00000000000000000000000000000000*
L1248  00000000000000000000000000000000*
L1280  11111111111111111111111111111111*
L1312  10111111111111111011111111111111*
L1344  00000000000000000000000000000000*
L1376  00000000000000000000000000000000*
L1408  00000000000000000000000000000000*
L1440  00000000000000000000000000000000*
L1472  00000000000000000000000000000000*
L1504  00000000000000000000000000000000*
L1536  11111111111111111111111111111111*
L1568  10111111111110111111111111111111*
L1600  00000000000000000000000000000000*
L1632  00000000000000000000000000000000*
L1664  00000000000000000000000000000000*
L1696  00000000000000000000000000000000*
L1728  00000000000000000000000000000000*
L1760  00000000000000000000000000000000*
L1792  00000000000000000000000000000000*
L1824  00000000000000000000000000000000*
L1856  00000000000000000000000000000000*
L1888  00000000000000000000000000000000*
L1920  00000000000000000000000000000000*
L1952  00000000000000000000000000000000*
L1984  00000000000000000000000000000000*
L2016  00000000000000000000000000000000*
C3D54*
?

*/
/******************************************************************************/

static DRIVER_INIT( vaportra )
{
	UINT8 *RAM = memory_region(machine, REGION_CPU1);
	int i;

	for (i=0x00000; i<0x80000; i++)
		RAM[i]=(RAM[i] & 0x7e) | ((RAM[i] & 0x01) << 7) | ((RAM[i] & 0x80) >> 7);
}

/******************************************************************************/

GAME( 1989, vaportra, 0,        vaportra, vaportra, vaportra, ROT270, "Data East Corporation", "Vapor Trail - Hyper Offence Formation (World revision 1)", 0 )
GAME( 1989, vaportru, vaportra, vaportra, vaportra, vaportra, ROT270, "Data East USA", "Vapor Trail - Hyper Offence Formation (US)", 0 )
GAME( 1989, kuhga,    vaportra, vaportra, vaportra, vaportra, ROT270, "Data East Corporation", "Kuhga - Operation Code 'Vapor Trail' (Japan revision 3)", 0 )
