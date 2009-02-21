/*******************************************************************************************

King Derby (c) 1981 Tatsumi

driver by Andrew Gardner, Angelo Salese & Roberto Fresca

TODO:
- remaining video issues, priorities, sprites etc.;
- inputs;
- colors (probably needs a color prom);
- add backup ram emulation;
- unknown memories;
- the name "King Derby" is a raw guess, there's a chance that it uses a different name
  (but there isn't any title screen on the game?)

============================================================================================

file   : readme.txt
author : Stefan Lindberg
created: 2009-01-03
updated: *
version: 1.0


Unknown Tazmi game, 1981?


Note:
Untested PCB.
A bet/gamble game i presume, possible "King Derby".
The PCB is marked 1981.

See included PCB pics.



Roms:

Name           Size     CRC32           Chip Type
----------------------------------------------------------------------------
im1_yk.g1      4096     0x1921605d      D2732D
im2_yk.f1      4096     0x8504314e      M5L2732K
im3_yk.e1      4096     0xb034314e      M5L2732K
im4_d.d6       4096     0x20f2d999      M5L2732K
im5_d.c6       4096     0xc192cecc      D2732D
im6_d.b6       4096     0x257f4e0d      D2732D
s1.d1          4096     0x26974007      D2732D
s10_a.l8       4096     0x37b2736f      D2732D
s2.e1          4096     0xbedebfa7      D2732D
s3.f1          4096     0x0aa59571      D2732D
s4.g1          4096     0xccd5fb0e      D2732D
s5.d2          4096     0x32613df3      D2732D
s6.e2          4096     0xa151c422      D2732D
s7.f2          4096     0x7cfcee55      D2732D
s8.g2          4096     0xad667c05      D2732D
s9_a.ka        4096     0xca82cd81      D2732D
sg1_b.e1       4096     0x92ef3c13      D2732D

*******************************************************************************************/

#include "driver.h"
#include "cpu/z80/z80.h"
#include "video/mc6845.h"
#include "machine/8255ppi.h"
#include "sound/ay8910.h"

#define CLK_1	XTAL_20MHz
#define CLK_2	XTAL_3_579545MHz

/*************************************
 *
 *  Video Hardware
 *
 *************************************/

static UINT8 *kingdrby_vram,*kingdrby_attr;
static tilemap *sc0_tilemap,*sc0w_tilemap;

static TILE_GET_INFO( get_sc0_tile_info )
{
	int tile = kingdrby_vram[tile_index] | kingdrby_attr[tile_index]<<8;
	int color = (kingdrby_attr[tile_index] & 0xf0) >> 4;

	tile&=0x1ff;

	SET_TILE_INFO(
			1,
			tile,
			color,
			0);
}


static VIDEO_START(kingdrby)
{
	sc0_tilemap = tilemap_create(machine, get_sc0_tile_info,tilemap_scan_rows,8,8,32,24);
	sc0w_tilemap = tilemap_create(machine, get_sc0_tile_info,tilemap_scan_rows,8,8,32,32);
}

static void draw_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect)
{
	int count = 0;

	/*sprites not fully understood.*/
	for(count=0;count<0x48;count+=4)
	{
		int x,y,spr_offs,colour,fx,mode,dx,dy,h,w;

		spr_offs = (spriteram[count]);
		spr_offs &=0x7f;
		spr_offs*=4;
		colour = 0;
		fx = spriteram[count] & 0x80;
		if(spriteram[count+1] == 0)
			y = 0;
		else
			y = 0x100-spriteram[count+1];
		x = spriteram[count+2];
		mode = (spriteram[count+3] & 0xff);

		if((mode & 1) == 1)
			continue;

		/*a simpler way?*/
		if((mode & 0xe0) == 0xc0 )      { h = 1; w = 1; }
		else if((mode & 0xe0) == 0xa0 ) { h = 2; w = 2; }
		else if((mode & 0xe0) == 0x80 ) { h = 2; w = 2; }
		else if((mode & 0xe0) == 0xe0 ) { h = 2; w = 2; }
		else              {  h = 3; w = 4; }

		if(fx)
		{
			for(dy=0;dy<h;dy++)
				for(dx=0;dx<w;dx++)
					drawgfx(bitmap,machine->gfx[0],spr_offs++,colour,1,0,(x+16*w)-dx*16,y+dy*16,cliprect,TRANSPARENCY_PEN,0);
		}
		else
		{
			for(dy=0;dy<h;dy++)
				for(dx=0;dx<w;dx++)
					drawgfx(bitmap,machine->gfx[0],spr_offs++,colour,0,0,x+dx*16,y+dy*16,cliprect,TRANSPARENCY_PEN,0);
		}
	}
}

static VIDEO_UPDATE(kingdrby)
{
	const rectangle *visarea = video_screen_get_visible_area(screen);
	rectangle clip;
	tilemap_set_scrollx( sc0_tilemap,0, kingdrby_vram[0x342]);
	tilemap_set_scrolly( sc0_tilemap,0, kingdrby_vram[0x341]);
	tilemap_set_scrolly( sc0w_tilemap,0, 32);

	/* maybe it needs two window tilemaps? (one at the top, the other at the bottom)*/
	clip.min_x = visarea->min_x;
	clip.max_x = 256;
	clip.min_y = 192;
	clip.max_y = visarea->max_y;

	tilemap_draw(bitmap,cliprect,sc0_tilemap,0,0);
	tilemap_draw(bitmap,&clip,sc0w_tilemap,0,0);
	draw_sprites(screen->machine,bitmap,cliprect);

	return 0;
}

static WRITE8_HANDLER( sc0_vram_w )
{
	kingdrby_vram[offset] = data;
	tilemap_mark_tile_dirty(sc0_tilemap,offset);
	tilemap_mark_tile_dirty(sc0w_tilemap,offset);
}

static WRITE8_HANDLER( sc0_attr_w )
{
	kingdrby_attr[offset] = data;
	tilemap_mark_tile_dirty(sc0_tilemap,offset);
	tilemap_mark_tile_dirty(sc0w_tilemap,offset);
}

/*************************************
 *
 *  I/O (TODO)
 *
 *************************************/

static WRITE8_DEVICE_HANDLER( outport0_w )
{
//	printf("PPI0 port C out: %02X\n", data);
}

static WRITE8_DEVICE_HANDLER( outport1_w )
{
//	const address_space *space = cpu_get_address_space(device->machine->cpu[2], ADDRESS_SPACE_PROGRAM);

//	cpu_set_input_line(device->machine->cpu[2], INPUT_LINE_NMI, PULSE_LINE);
//	soundlatch_w(space,0, data);
}

static WRITE8_DEVICE_HANDLER( outport2_w )
{
//	printf("PPI1 port C(upper) out: %02X\n", data);
}

/*************************************
 *
 * Memory Map
 *
 *************************************/

static ADDRESS_MAP_START( master_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x2fff) AM_ROM
	AM_RANGE(0x3000, 0x33ff) AM_RAM AM_MIRROR(0xc00) AM_SHARE(1)
	AM_RANGE(0x4000, 0x43ff) AM_RAM_WRITE(sc0_vram_w) AM_BASE(&kingdrby_vram)
	AM_RANGE(0x5000, 0x53ff) AM_RAM_WRITE(sc0_attr_w) AM_BASE(&kingdrby_attr)
ADDRESS_MAP_END

static ADDRESS_MAP_START( master_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
//	AM_RANGE(0x00, 0x00) AM_READ_PORT("UNK")
ADDRESS_MAP_END

static ADDRESS_MAP_START( slave_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x2fff) AM_ROM
	AM_RANGE(0x3000, 0x3fff) AM_ROM //sound rom tested for the post check
	AM_RANGE(0x4000, 0x43ff) AM_RAM //backup ram
	AM_RANGE(0x5000, 0x5003) AM_DEVREADWRITE(PPI8255, "ppi8255_0", ppi8255_r, ppi8255_w)	/* I/O Ports */
	AM_RANGE(0x6000, 0x6003) AM_DEVREADWRITE(PPI8255, "ppi8255_1", ppi8255_r, ppi8255_w)	/* I/O Ports */
	AM_RANGE(0x7000, 0x73ff) AM_RAM AM_SHARE(1)
	AM_RANGE(0x7400, 0x75ff) AM_RAM AM_BASE(&spriteram)
	AM_RANGE(0x7600, 0x7600) AM_DEVWRITE(MC6845, "crtc", mc6845_address_w)
	AM_RANGE(0x7601, 0x7601) AM_DEVREADWRITE(MC6845, "crtc", mc6845_register_r, mc6845_register_w)
//	AM_RANGE(0x7a00, 0x7a00) AM_READ_PORT("UNK")
	AM_RANGE(0x7c00, 0x7c00) AM_READ_PORT("DSW")
ADDRESS_MAP_END

static ADDRESS_MAP_START( slave_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
//	AM_RANGE(0x00, 0x00) AM_READ_PORT("UNK")
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x0fff) AM_ROM
	AM_RANGE(0x2000, 0x23ff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_io_map, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x40, 0x40) AM_READ(soundlatch_r)
	AM_RANGE(0x40, 0x41) AM_DEVWRITE(SOUND, "ay", ay8910_address_data_w)
ADDRESS_MAP_END

/*************************************
*
* PPI configuration
*
* 5000-5003 PPI group modes 0/0 - A & B as input, C (all) as output.
* 6000-6003 PPI group modes 0/0 - B & C (lower) as input, A & C (upper) as output.
*
*************************************/

static const ppi8255_interface ppi8255_intf[2] =
{
	/* A & B as input, C (all) as output */
	{
		DEVCB_INPUT_PORT("IN0"),	/* Port A read */
		DEVCB_INPUT_PORT("IN1"),	/* Port B read */
		DEVCB_NULL,					/* Port C read */
		DEVCB_NULL,					/* Port A write */
		DEVCB_NULL,					/* Port B write */
		DEVCB_HANDLER(outport0_w)   /* Port C write */
	},

	/* B & C (lower) as input, A & C (upper) as output */
	{
		DEVCB_NULL,					/* Port A read */
		DEVCB_INPUT_PORT("IN2"),	/* Port B read */
		DEVCB_INPUT_PORT("IN3"),	/* Port C read */
		DEVCB_HANDLER(outport1_w),  /* Port A write */
		DEVCB_NULL,					/* Port B write */
		DEVCB_HANDLER(outport2_w)	/* Port C write */
	}
};

/*************************************
 *
 *  Input Port Definitions
 *
 *************************************/

static INPUT_PORTS_START( kingdrby )
	PORT_START("IN0")	// ppi0 (5000)
	PORT_DIPNAME( 0x01, 0x01, "SYSTEM" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_NAME("1P 1C/1C") //service?
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("2P 1C/1C") //service?
	PORT_DIPNAME( 0x10, 0x10, "Hopper I/O" )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("1P Credit Clear") PORT_CODE(KEYCODE_A)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("2P Credit Clear")


	PORT_START("IN1")	// ppi0 (5001)
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_VBLANK ) //?
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )  PORT_NAME( "Analyzer" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_NAME("1P 1C/10C")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("2P 1C/10C")
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")	// ppi1 (6001)
	PORT_DIPNAME( 0x01, 0x01, "SYSTEM" )
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

	PORT_START("IN3")	// ppi1 (6002)
	PORT_DIPNAME( 0x01, 0x01, "SYSTEM" )
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

	PORT_START("DSW")
	PORT_DIPNAME( 0x01, 0x01, "SYSTEM" )
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
	PORT_DIPNAME( 0x20, 0x20, "Game Type?" ) //enables two new msgs "advance" and "exchange" in analyzer mode
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "POST Check" )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("UNK")
	PORT_DIPNAME( 0x01, 0x01, "SYSTEM" )
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

/*************************************
*
* GFX decoding
*
*************************************/

static const gfx_layout layout8x8x2 =
{
	8,8,
	RGN_FRAC(1,2),
	2,
	{
		RGN_FRAC(0,2),
		RGN_FRAC(1,2)
	},
	{ STEP8(0,1) },
	{ STEP8(0,8) },
	8*8
};

static const gfx_layout layout16x16x2 =
{
	16,16,
	RGN_FRAC(1,2),
	2,
	{
		RGN_FRAC(0,2),
		RGN_FRAC(1,2),
	},
	{ 0,1,2,3,4,5,6,7,16*8+0,16*8+1,16*8+2,16*8+3,16*8+4,16*8+5,16*8+6,16*8+7, },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8,8*8,9*8,10*8,11*8,12*8,13*8,14*8,15*8  },
	16*16
};

static GFXDECODE_START( kingdrby )
	GFXDECODE_ENTRY( "gfx1", 0x0000, layout16x16x2, 0, 0x40 )
	GFXDECODE_ENTRY( "gfx2", 0x0000, layout8x8x2,   0, 0x40 )
GFXDECODE_END

/**********************************************************************************************************
*
* MC6845 interface
*
* screen size:  384x272    registers 00 & 04. (value-1)
* visible area: 256x224    registers 01 & 06.
*
* the clocks are a guess, but is the only logical combination I found to get a reasonable vertical of ~53Hz.
*
***********************************************************************************************************/

static const mc6845_interface mc6845_intf =
{
	"main",	/* screen we are acting on */
	8,		/* number of pixels per video memory address */
	NULL,	/* before pixel update callback */
	NULL,	/* row update callback */
	NULL,	/* after pixel update callback */
	NULL,	/* callback for display state changes */
	NULL,	/* HSYNC callback */
	NULL	/* VSYNC callback */
};

/*************************************
 *
 *  Sound HW Config (TODO)
 *
 *************************************/

static const ay8910_interface ay8910_config =
{
	AY8910_LEGACY_OUTPUT,
	AY8910_DEFAULT_LOADS,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};

static MACHINE_DRIVER_START( kingdrby )
	MDRV_CPU_ADD("master", Z80, CLK_2)
	MDRV_CPU_PROGRAM_MAP(master_map,0)
	MDRV_CPU_IO_MAP(master_io_map,0)
	MDRV_CPU_VBLANK_INT("main", irq0_line_hold)

	MDRV_CPU_ADD("slave", Z80, CLK_2)
	MDRV_CPU_PROGRAM_MAP(slave_map,0)
	MDRV_CPU_IO_MAP(slave_io_map,0)
	MDRV_CPU_VBLANK_INT("main", irq0_line_hold)

	MDRV_CPU_ADD("sound", Z80, CLK_2)
	MDRV_CPU_PROGRAM_MAP(sound_map,0)
	MDRV_CPU_IO_MAP(sound_io_map,0)
	MDRV_CPU_VBLANK_INT("main", irq0_line_hold)

	MDRV_QUANTUM_PERFECT_CPU("master")

	MDRV_PPI8255_ADD( "ppi8255_0", ppi8255_intf[0] )
	MDRV_PPI8255_ADD( "ppi8255_1", ppi8255_intf[1] )

	MDRV_GFXDECODE(kingdrby)
	MDRV_PALETTE_LENGTH(0x100)

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_SCREEN_VISIBLE_AREA(0, 256-1, 0, 224-1)	/* controlled by CRTC */

	MDRV_VIDEO_START(kingdrby)
	MDRV_VIDEO_UPDATE(kingdrby)

	MDRV_MC6845_ADD("crtc", MC6845, CLK_1/32, mc6845_intf)	/* 53.333 Hz. guess */

	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("ay", AY8910, CLK_1/16)	/* guess */
	MDRV_SOUND_CONFIG(ay8910_config)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.25)
MACHINE_DRIVER_END


ROM_START( kingdrby )
	ROM_REGION( 0x3000, "master", 0 )
	ROM_LOAD( "im4_d.d6",  0x0000, 0x1000, CRC(20f2d999) SHA1(91db46059f32b4791460df3330260f4e60f016a5) )
	ROM_LOAD( "im5_d.c6",  0x1000, 0x1000, CRC(c192cecc) SHA1(63436bf3d9c1e34f6549830c8164295b7758d666) )
	ROM_LOAD( "im6_d.b6",  0x2000, 0x1000, CRC(257f4e0d) SHA1(cd61f3cf70c536aa207ebfdd28be54ac586b5249) )

	ROM_REGION( 0x1000, "sound", 0 )
	ROM_LOAD( "sg1_b.e1", 0x0000, 0x1000, CRC(92ef3c13) SHA1(1bf1e4106b37aadfc02822184510740e18a54d5c) )

	ROM_REGION( 0x4000, "slave", 0 )
	ROM_LOAD( "im1_yk.g1", 0x0000, 0x1000, CRC(1921605d) SHA1(0aa6f7195ea59d0080620ab02a737e5c319dd3e7) )
	ROM_LOAD( "im2_yk.f1", 0x1000, 0x1000, CRC(8504314e) SHA1(309645e17fb3149dce57ae6844cc58652a1eeb35) )
	ROM_LOAD( "im3_yk.e1", 0x2000, 0x1000, CRC(b0e473ec) SHA1(234598548b2a2a8f53d40bc07c3b1759074b7d93) )
	ROM_COPY( "sound", 0x0000, 0x3000, 0x1000 )

	/* sprites gfxs */
	ROM_REGION( 0x8000, "gfx1", 0 )
	ROM_LOAD( "s1.d1",    0x0000, 0x1000, CRC(26974007) SHA1(5079daf9ad7d84f935c256458060db9497daef91) )
	ROM_LOAD( "s2.e1",    0x1000, 0x1000, CRC(bedebfa7) SHA1(5a2116ed4af6bc4b72199017515980e4a937236c) )
	ROM_LOAD( "s3.f1",    0x2000, 0x1000, CRC(0aa59571) SHA1(5005ffdd0030e4d4c1d8033fd3c78177c0fbd1b0) )
	ROM_LOAD( "s4.g1",    0x3000, 0x1000, CRC(ccd5fb0e) SHA1(3ee4377d15e7731586b7a3457dbae52edaed72d3) )
	ROM_LOAD( "s5.d2",    0x4000, 0x1000, CRC(32613df3) SHA1(21ce057c416e6f1d0a3e112d640b1cf52ba69206) )
	ROM_LOAD( "s6.e2",    0x5000, 0x1000, CRC(a151c422) SHA1(354efaee64c8cc457f96cba4722f6a0df66e14d3) )
	ROM_LOAD( "s7.f2",    0x6000, 0x1000, CRC(7cfcee55) SHA1(590ac02941e82371d56113d052eb4d4bcdbf83b0) )
	ROM_LOAD( "s8.g2",    0x7000, 0x1000, CRC(ad667c05) SHA1(d9bdf3a125eba2d40191b0659c2007ccbc6fd12b) )

	/* tile gfxs */
	ROM_REGION( 0x2000, "gfx2", 0 )
	ROM_LOAD( "s9_a.k8",  0x0000, 0x1000, CRC(ca82cd81) SHA1(fdf47df7705c8d0ae70b5a0e29b35819f3d0749a) )
	ROM_LOAD( "s10_a.l8", 0x1000, 0x1000, CRC(37b2736f) SHA1(15ef3f563aebd1f5506135c7c01e9a1db30a9ccc) )

	/* color proms */
	ROM_REGION( 0x200, "proms", 0 )
	ROM_LOAD( "prom.x", 0x0000, 0x0200, NO_DUMP )
ROM_END

GAME( 1981, kingdrby,  0,      kingdrby,   kingdrby,   0,       ROT0,   "Tazmi",    "King Derby",   GAME_NOT_WORKING | GAME_NO_SOUND )
