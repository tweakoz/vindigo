/***************************************************************************

                     -= 68020 + Imagetek 15000 Games =-

            driver by   David Haywood ( hazemamewip (at) hotmail.com )
            partly based on metro.c driver by Luca Elia


Main  CPU    :  MC68020

Video Chips   : Imagetek 15000 |
Sound Chips   : Imagetek 15000 |\- both sound and gfx
Other Features: Memory Blitter (part of Imagetek 15000)

---------------------------------------------------------------------------
Year + Game                     PCB         Video Chip  Issues / Notes
---------------------------------------------------------------------------
97  Rabbit                      VG5330-B    15000
97? Tokimeki Mahjong Paradise   VG5550-B    15000
---------------------------------------------------------------------------
Not dumped yet:
unknown

To Do:

- raster effects (rabbit only?, see left side of one of the levels in rabbit)
- clean up zoom code and make zoom effect more accurate
- sound (adpcm of some kind)
- status bar in rabbit is the wrong colour, timing of blitter / interrupts?
- eeprom hookup seems flakey

*/

/*

Rabbit PCB Layout
-----------------

VG5330-B
|---------------------------------|
|    62256  62256        61    60 |
|    62256  62256        51    50 |
|    62256  62256                 |
|    62256  62256        43    42 |
|                        41    40 |
|                                 |
|J     IMAGETEK          33    32 |
|A      15000            23    22 |
|M            40MHz               |
|M                       13    12 |
|A             68EC020   03    02 |
|     ALTERA                      |
|     EPM7032 24MHz      31    30 |
|     93C46              21    20 |
|                                 |
| JPR2  JPR0  62256      11    10 |
| JPR3  JPR1  62256      01    00 |
|---------------------------------|

Notes:
      68020 clock: 24.000MHz
      VSync: 60Hz

      Only ROMs positions 60, 50, 40, 02, 03, 10, 11, 01, 00 are populated.

Tokimeki Mahjong Paradise - Dear My Love Board Notes
----------------------------------------------------

Board:  VG5550-B

CPU:    MC68EC020FG25
OSC:    40.00000MHz
    24.00000MHz

Custom: Imagetek 15000 (2ch video & 2ch sound)

*/

#include "driver.h"
#include "cpu/m68000/m68000.h"
#include "deprecat.h"
#include "machine/eeprom.h"

#define VERBOSE_AUDIO_LOG (0)	// enable to show audio writes (very noisy when music is playing)

/* debug */
static UINT32 *rabbit_viewregs0;
static UINT32 *rabbit_viewregs6;
static UINT32 *rabbit_viewregs7;
static UINT32 *rabbit_viewregs9;
static UINT32 *rabbit_viewregs10;

static UINT32 *rabbit_tilemap_regs[4];
static UINT32 *rabbit_spriteregs;
static UINT32 *rabbit_blitterregs;
static bitmap_t *rabbit_sprite_bitmap;
static rectangle rabbit_sprite_clip;

static int rabbit_vblirqlevel, rabbit_bltirqlevel, rabbit_banking;

static UINT32 *rabbit_tilemap_ram[4];

static UINT32 *rabbit_spriteram;
static tilemap *rabbit_tilemap[4];

/* call with tilesize = 0 for 8x8 or 1 for 16x16 */
INLINE void get_rabbit_tilemap_info(running_machine *machine, tile_data *tileinfo, int tile_index, int whichtilemap, int tilesize)
{
	int tileno,colour,flipxy, depth;
	int bank;
	depth = (rabbit_tilemap_ram[whichtilemap][tile_index]&0x10000000)>>28;
	tileno = rabbit_tilemap_ram[whichtilemap][tile_index]&0xffff;
	bank = (rabbit_tilemap_ram[whichtilemap][tile_index]&0x000f0000)>>16;
	colour =  (rabbit_tilemap_ram[whichtilemap][tile_index]>>20)&0xff;
	flipxy =  (rabbit_tilemap_ram[whichtilemap][tile_index]>>29)&3;

	if(rabbit_banking)
	{
		switch (bank)
		{
			case 0x8:
				tileno += 0x10000;
				break;

			case 0xc:
				tileno += 0x20000;
				break;
		}
	}
	else
		tileno += (bank << 16);

	if (depth)
	{
		tileno >>=(1+tilesize*2);
		colour&=0x0f;
		colour+=0x20;
		tileinfo->group = 1;
		SET_TILE_INFO(6+tilesize,tileno,colour,TILE_FLIPXY(flipxy));
	}
	else
	{
		tileno >>=(0+tilesize*2);
		//colour&=0x3f; // fixes status bar.. but breaks other stuff
		colour+=0x200;
		tileinfo->group = 0;
		SET_TILE_INFO(4+tilesize,tileno,colour,TILE_FLIPXY(flipxy));
	}
}

static TILE_GET_INFO( get_rabbit_tilemap0_tile_info )
{
	get_rabbit_tilemap_info(machine,tileinfo,tile_index,0,1);
}

static TILE_GET_INFO( get_rabbit_tilemap1_tile_info )
{
	get_rabbit_tilemap_info(machine,tileinfo,tile_index,1,1);
}

static TILE_GET_INFO( get_rabbit_tilemap2_tile_info )
{
	get_rabbit_tilemap_info(machine,tileinfo,tile_index,2,1);
}

/* some bad colours on life bars for this layer .. fix them ..
    (needs mask of 0x3f but this breaks other gfx..) */
static TILE_GET_INFO( get_rabbit_tilemap3_tile_info )
{
	get_rabbit_tilemap_info(machine,tileinfo,tile_index,3,0);
}

static WRITE32_HANDLER( rabbit_tilemap0_w )
{
	COMBINE_DATA(&rabbit_tilemap_ram[0][offset]);
	tilemap_mark_tile_dirty(rabbit_tilemap[0],offset);
}

static WRITE32_HANDLER( rabbit_tilemap1_w )
{
	COMBINE_DATA(&rabbit_tilemap_ram[1][offset]);
	tilemap_mark_tile_dirty(rabbit_tilemap[1],offset);
}

static WRITE32_HANDLER( rabbit_tilemap2_w )
{
	COMBINE_DATA(&rabbit_tilemap_ram[2][offset]);
	tilemap_mark_tile_dirty(rabbit_tilemap[2],offset);
}


static WRITE32_HANDLER( rabbit_tilemap3_w )
{
	COMBINE_DATA(&rabbit_tilemap_ram[3][offset]);
	tilemap_mark_tile_dirty(rabbit_tilemap[3],offset);
}

/*

'spriteregs' format (7 dwords)

00000XXX 0YYYAAAA 0000BBBB 0000CCCC 03065000 00720008 00a803bc
             zoom     zoom     zoom          Strt/End

XXX = global Xpos
YYY = global Ypos

AAAA = 0100 when normal, 018c when zoomed out
BBBB = 9f80 when normal, f6ba when zoomed out  0x9f80 / 128 = 319
CCCC = 6f80 when normal, ac7a when zoomed out  0x6f80 / 128 = 223


sprites invisible at the end of a round in rabbit, why?

*/

static void draw_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect )
{
	int xpos,ypos,tileno,xflip,yflip, colr;
	const gfx_element *gfx = machine->gfx[1];
	int todraw = (rabbit_spriteregs[5]&0x0fff0000)>>16; // how many sprites to draw (start/end reg..) what is the other half?

	UINT32 *source = (rabbit_spriteram+ (todraw*2))-2;
	UINT32 *finish = rabbit_spriteram;

//  bitmap_fill(rabbit_sprite_bitmap, &rabbit_sprite_clip, 0x0); // sloooow

	while( source>=finish )
	{

		xpos = (source[0]&0x00000fff);
		ypos = (source[0]&0x0fff0000)>>16;

		xflip = (source[0]&0x00008000)>>15;
		yflip = (source[0]&0x00004000)>>14;
		colr = (source[1]&0x0ff00000)>>15;


		tileno = (source[1]&0x0001ffff);
		colr =   (source[1]&0x0ff00000)>>20;

		if(xpos&0x800)xpos-=0x1000;

		drawgfx_transpen(rabbit_sprite_bitmap,&rabbit_sprite_clip,gfx,tileno,colr,!xflip/*wrongdecode?*/,yflip,xpos+0x20-8/*-(rabbit_spriteregs[0]&0x00000fff)*/,ypos-24/*-((rabbit_spriteregs[1]&0x0fff0000)>>16)*/,15);
//      drawgfx_transpen(bitmap,cliprect,gfx,tileno,colr,!xflip/*wrongdecode?*/,yflip,xpos+0xa0-8/*-(rabbit_spriteregs[0]&0x00000fff)*/,ypos-24+0x80/*-((rabbit_spriteregs[1]&0x0fff0000)>>16)*/,0);


		source-=2;

	}

}

/* the sprite bitmap can probably be handled better than this ... */
static void rabbit_clearspritebitmap( bitmap_t *bitmap, const rectangle *cliprect )
{
	int startx, starty;
	int y;
	int amountx,amounty;
	UINT16 *dstline;

	/* clears a *sensible* amount of the sprite bitmap */
	startx = (rabbit_spriteregs[0]&0x00000fff);
	starty = (rabbit_spriteregs[1]&0x0fff0000)>>16;

	startx-=200;
	starty-=200;
	amountx =650;
	amounty =600;

	if (startx < 0) { amountx += startx; startx = 0; }
	if ((startx+amountx)>=0x1000) amountx-=(0x1000-(startx+amountx));

	for (y=0; y<amounty;y++)
	{
		dstline = BITMAP_ADDR16(rabbit_sprite_bitmap, (starty+y)&0xfff, 0);
		memset(dstline+startx,0x00,amountx*2);
	}
}

/* todo: fix zoom, its inaccurate and this code is ugly */
static void draw_sprite_bitmap( bitmap_t *bitmap, const rectangle *cliprect )
{

	UINT32 x,y;
	UINT16 *srcline;
	UINT16 *dstline;
	UINT16 pixdata;
	UINT32 xsize, ysize;
	UINT32 xdrawpos, ydrawpos;
	UINT32 xstep,ystep;

	int startx, starty;
	startx = ((rabbit_spriteregs[0]&0x00000fff));
	starty = ((rabbit_spriteregs[1]&0x0fff0000)>>16);

	/* zoom compensation? */
	startx-=((rabbit_spriteregs[1]&0x000001ff)>>1);
	starty-=((rabbit_spriteregs[1]&0x000001ff)>>1);


	xsize = ((rabbit_spriteregs[2]&0x0000ffff));
	ysize = ((rabbit_spriteregs[3]&0x0000ffff));
	xsize+=0x80;
	ysize+=0x80;
	xstep = ((320*128)<<16) / xsize;
	ystep = ((224*128)<<16) / ysize;
	ydrawpos = 0;
	for (y=0;y<ysize;y+=0x80)
	{
		ydrawpos = ((y>>7)*ystep);
		ydrawpos >>=16;

		if ((ydrawpos >= cliprect->min_y) && (ydrawpos <= cliprect->max_y))
		{
			srcline = BITMAP_ADDR16(rabbit_sprite_bitmap, (starty+(y>>7))&0xfff, 0);
			dstline = BITMAP_ADDR16(bitmap, ydrawpos, 0);

			for (x=0;x<xsize;x+=0x80)
			{
				xdrawpos = ((x>>7)*xstep);
				xdrawpos >>=16;
				pixdata = srcline[(startx+(x>>7))&0xfff];

				if (pixdata)
					if ((xdrawpos >= cliprect->min_x) && (xdrawpos <= cliprect->max_x))
						dstline[xdrawpos] = pixdata;
			}
		}
	}



}
static VIDEO_START(rabbit)
{
	/* the tilemaps are bigger than the regions the cpu can see, need to allocate the ram here */
	/* or maybe not for this game/hw .... */
	rabbit_tilemap_ram[0] = auto_alloc_array_clear(machine, UINT32, 0x20000/4);
	rabbit_tilemap_ram[1] = auto_alloc_array_clear(machine, UINT32, 0x20000/4);
	rabbit_tilemap_ram[2] = auto_alloc_array_clear(machine, UINT32, 0x20000/4);
	rabbit_tilemap_ram[3] = auto_alloc_array_clear(machine, UINT32, 0x20000/4);

	rabbit_tilemap[0] = tilemap_create(machine, get_rabbit_tilemap0_tile_info,tilemap_scan_rows,16, 16, 128,32);
	rabbit_tilemap[1] = tilemap_create(machine, get_rabbit_tilemap1_tile_info,tilemap_scan_rows,16, 16, 128,32);
	rabbit_tilemap[2] = tilemap_create(machine, get_rabbit_tilemap2_tile_info,tilemap_scan_rows,16, 16, 128,32);
	rabbit_tilemap[3] = tilemap_create(machine, get_rabbit_tilemap3_tile_info,tilemap_scan_rows, 8,  8, 128,32);

	/* the tilemaps mix 4bpp and 8bbp tiles, we split these into 2 groups, and set a different transpen for each group */
    tilemap_map_pen_to_layer(rabbit_tilemap[0], 0, 15,  TILEMAP_PIXEL_TRANSPARENT);
    tilemap_map_pen_to_layer(rabbit_tilemap[0], 1, 255, TILEMAP_PIXEL_TRANSPARENT);
    tilemap_map_pen_to_layer(rabbit_tilemap[1], 0, 15,  TILEMAP_PIXEL_TRANSPARENT);
    tilemap_map_pen_to_layer(rabbit_tilemap[1], 1, 255, TILEMAP_PIXEL_TRANSPARENT);
    tilemap_map_pen_to_layer(rabbit_tilemap[2], 0, 15,  TILEMAP_PIXEL_TRANSPARENT);
    tilemap_map_pen_to_layer(rabbit_tilemap[2], 1, 255, TILEMAP_PIXEL_TRANSPARENT);
    tilemap_map_pen_to_layer(rabbit_tilemap[3], 0, 15,  TILEMAP_PIXEL_TRANSPARENT);
    tilemap_map_pen_to_layer(rabbit_tilemap[3], 1, 255, TILEMAP_PIXEL_TRANSPARENT);

	rabbit_sprite_bitmap = auto_bitmap_alloc(machine,0x1000,0x1000,video_screen_get_format(machine->primary_screen));
	rabbit_sprite_clip.min_x = 0;
	rabbit_sprite_clip.max_x = 0x1000-1;
	rabbit_sprite_clip.min_y = 0;
	rabbit_sprite_clip.max_y = 0x1000-1;
}

/*

info based on rabbit

tilemap regs format (6 dwords, 1 set for each tilemap)

each line represents the differences on each tilemap for unknown variables
(tilemap)
(0) 0P660000 YYYYXXXX 0000**00 0000AAAA BBBB0000 00fx0000   ** = 00 during title, 8f during game, x = 0 during title, 1 during game
(1)     1                 00                        0
(2)     2                 00                        0
(3)   263                 00                        0
 0123456789abcdef = never change?

 P = priority
 Y = Yscroll
 X = Xscroll
 A = Yzoom (maybe X) (0800 = normal, 0C60 = fully zoomed out)
 B = Xzoom (maybe Y) (0800 = normal, 0C60 = fully zoomed out)

*/

static void rabbit_drawtilemap( bitmap_t *bitmap, const rectangle *cliprect, int whichtilemap )
{
	INT32 startx, starty, incxx, incxy, incyx, incyy, tran;

	startx=((rabbit_tilemap_regs[whichtilemap][1]&0x0000ffff));  // >>4 for nonzoomed pixel scroll value
	starty=((rabbit_tilemap_regs[whichtilemap][1]&0xffff0000)>>16); // >> 20 for nonzoomed pixel scroll value
	incxx= ((rabbit_tilemap_regs[whichtilemap][3]&0x00000fff)); // 0x800 when non-zoomed
	incyy= ((rabbit_tilemap_regs[whichtilemap][4]&0x0fff0000)>>16);

	incxy = 0; incyx = 0;
	tran = 1;


	/* incxx and incyy and standard zoom, 16.16, a value of 0x10000 means no zoom
       startx/starty are also 16.16 scrolling
      */

	tilemap_draw_roz(bitmap,cliprect,rabbit_tilemap[whichtilemap],startx << 12,starty << 12,
			incxx << 5,incxy << 8,incyx << 8,incyy << 5,
			1,	/* wraparound */
			tran ? 0 : TILEMAP_DRAW_OPAQUE,0);
}

static VIDEO_UPDATE(rabbit)
{
	int prilevel;

	bitmap_fill(bitmap,cliprect,get_black_pen(screen->machine));

//  popmessage("%08x %08x", rabbit_viewregs0[0], rabbit_viewregs0[1]);
//  popmessage("%08x %08x %08x %08x %08x %08x", rabbit_tilemap_regs[0][0],rabbit_tilemap_regs[0][1],rabbit_tilemap_regs[0][2],rabbit_tilemap_regs[0][3],rabbit_tilemap_regs[0][4],rabbit_tilemap_regs[0][5]);
//  popmessage("%08x %08x %08x %08x %08x %08x", rabbit_tilemap_regs[1][0],rabbit_tilemap_regs[1][1],rabbit_tilemap_regs[1][2],rabbit_tilemap_regs[1][3],rabbit_tilemap_regs[1][4],rabbit_tilemap_regs[1][5]);
//  popmessage("%08x %08x %08x %08x %08x %08x", rabbit_tilemap_regs[2][0],rabbit_tilemap_regs[2][1],rabbit_tilemap_regs[2][2],rabbit_tilemap_regs[2][3],rabbit_tilemap_regs[2][4],rabbit_tilemap_regs[2][5]);
//  popmessage("%08x %08x %08x %08x %08x %08x", rabbit_tilemap_regs[3][0],rabbit_tilemap_regs[3][1],rabbit_tilemap_regs[3][2],rabbit_tilemap_regs[3][3],rabbit_tilemap_regs[3][4],rabbit_tilemap_regs[3][5]);
//  popmessage("%08x %08x %08x %08x %08x %08x %08x", rabbit_spriteregs[0],rabbit_spriteregs[1],rabbit_spriteregs[2],rabbit_spriteregs[3],rabbit_spriteregs[4],rabbit_spriteregs[5], rabbit_spriteregs[6]);
//  popmessage("%08x %08x %08x %08x %08x", rabbit_viewregs6[0],rabbit_viewregs6[1],rabbit_viewregs6[2],rabbit_viewregs6[3],rabbit_viewregs6[4]);
//  popmessage("%08x", rabbit_viewregs7[0]);
//  popmessage("%08x %08x %08x %08x", rabbit_blitterregs[0],rabbit_blitterregs[1],rabbit_blitterregs[2],rabbit_blitterregs[3]);
//  popmessage("%08x %08x %08x %08x", rabbit_viewregs9[0],rabbit_viewregs9[1],rabbit_viewregs9[2],rabbit_viewregs9[3]);

//  popmessage("%08x %08x %08x %08x %08x", rabbit_viewregs10[0],rabbit_viewregs10[1],rabbit_viewregs10[2],rabbit_viewregs10[3],rabbit_viewregs10[4]);

	/* prio isnt certain but seems to work.. */
	for (prilevel = 0xf; prilevel >0; prilevel--)
	{
		if (prilevel == ((rabbit_tilemap_regs[3][0]&0x0f000000)>>24)) rabbit_drawtilemap(bitmap,cliprect, 3);
		if (prilevel == ((rabbit_tilemap_regs[2][0]&0x0f000000)>>24)) rabbit_drawtilemap(bitmap,cliprect, 2);
		if (prilevel == ((rabbit_tilemap_regs[1][0]&0x0f000000)>>24)) rabbit_drawtilemap(bitmap,cliprect, 1);
		if (prilevel == ((rabbit_tilemap_regs[0][0]&0x0f000000)>>24)) rabbit_drawtilemap(bitmap,cliprect, 0);

		if (prilevel == 0x09) // should it be selectable?
		{
			rabbit_clearspritebitmap(bitmap,cliprect);
			draw_sprites(screen->machine,bitmap,cliprect);  // render to bitmap
			draw_sprite_bitmap(bitmap,cliprect); // copy bitmap to screen
		}
	}
	return 0;
}


static WRITE32_HANDLER( rabbit_paletteram_dword_w )
{
	int r,g,b;
	COMBINE_DATA(&paletteram32[offset]);

	b = ((paletteram32[offset] & 0x000000ff) >>0);
	r = ((paletteram32[offset] & 0x0000ff00) >>8);
	g = ((paletteram32[offset] & 0x00ff0000) >>16);

	palette_set_color(space->machine,offset,MAKE_RGB(r,g,b));
}

static READ32_HANDLER( rabbit_tilemap0_r )
{
	return rabbit_tilemap_ram[0][offset];
}

static READ32_HANDLER( rabbit_tilemap1_r )
{
	return rabbit_tilemap_ram[1][offset];
}

static READ32_HANDLER( rabbit_tilemap2_r )
{
	return rabbit_tilemap_ram[2][offset];
}

static READ32_HANDLER( rabbit_tilemap3_r )
{
	return rabbit_tilemap_ram[3][offset];
}

static READ32_HANDLER( randomrabbits )
{
	return mame_rand(space->machine);
}

/* rom bank is used when testing roms, not currently hooked up */
static WRITE32_HANDLER ( rabbit_rombank_w )
{
	UINT8 *dataroms = memory_region(space->machine, "gfx1");
#if 0
	int bank;
	printf("rabbit rombank %08x\n",data);
	bank = data & 0x3ff;

	memory_set_bankptr(space->machine, 1,&dataroms[0x40000*(bank&0x3ff)]);
#else
	memory_set_bankptr(space->machine, 1,&dataroms[0]);
#endif

}

/*
    Audio notes:

    There are 16 PCM voices.  Each voice has 4 16-bit wide registers.
    Voice 0 uses registers 0-3, 1 uses registers 4-7, etc.

    The first 2 registers for each voice are the LSW and MSW of the sample
    starting address.  The remaining 2 haven't been figured out yet.

    Registers 64 and up are "global", they don't belong to any specific voice.

    Register 66 is key-on (bitmapped so bit 0 = voice 0, bit 15 = voice 15).
    Register 67 is key-off (bitmapped identically to the key-on register).

    There are a few other "global" registers, their purpose is unknown at this
    time (timer?  the game seems to "play music" fine with just the VBL).
*/

static WRITE32_HANDLER( rabbit_audio_w )
{
	int reg, voice, base, i;

if (VERBOSE_AUDIO_LOG)
{
	if (mem_mask == 0xffff0000)
	{
		reg = offset*2;
		data >>= 16;
	}
	else if (mem_mask == 0x0000ffff)
	{
		reg = (offset*2)+1;
		data &= 0xffff;
	}
	else	logerror("audio error: unknown mask %08x\n", mem_mask);

	if (reg < 64)
	{
		voice = reg / 4;
		base = voice*4;
		logerror("V%02d: parm %d = %04x\n", voice, reg-base, data);
	}
	else
	{
		if (reg == 66)
		{
			logerror("Key on [%04x]: ", data);
			for (i = 0; i < 16; i++)
			{
				if (data & (1<<i))
				{
					logerror("%02d ", i);
				}
			}
			logerror("\n");
		}
		else if (reg == 67)
		{
			logerror("Key off [%04x]: ", data);
			for (i = 0; i < 16; i++)
			{
				if (data & (1<<i))
				{
					logerror("%02d ", i);
				}
			}
			logerror("\n");
		}
		else
		{
			logerror("Unknown write %04x to global reg %d\n", data, reg);
		}
	}
}
}

#define BLITCMDLOG 0
#define BLITLOG 0

static TIMER_CALLBACK( rabbit_blit_done )
{
	cputag_set_input_line(machine, "maincpu", rabbit_bltirqlevel, HOLD_LINE);
}

static void rabbit_do_blit(running_machine *machine)
{
	UINT8 *blt_data = memory_region(machine, "gfx1");
	int blt_source = (rabbit_blitterregs[0]&0x000fffff)>>0;
	int blt_column = (rabbit_blitterregs[1]&0x00ff0000)>>16;
	int blt_line   = (rabbit_blitterregs[1]&0x000000ff);
	int blt_tilemp = (rabbit_blitterregs[2]&0x0000e000)>>13;
	int blt_oddflg = (rabbit_blitterregs[2]&0x00000001)>>0;
	int mask,shift;


	if(BLITCMDLOG) mame_printf_debug("BLIT command %08x %08x %08x\n", rabbit_blitterregs[0], rabbit_blitterregs[1], rabbit_blitterregs[2]);

	if (blt_oddflg&1)
	{
		mask = 0xffff0000;
		shift= 0;
	}
	else
	{
		mask = 0x0000ffff;
		shift= 16;
	}

	blt_oddflg>>=1; /* blt_oddflg is now in dword offsets*/
	blt_oddflg+=0x80*blt_line;

	blt_source<<=1; /* blitsource is in word offsets */

	while(1)
	{
		int blt_commnd = blt_data[blt_source+1];
		int blt_amount = blt_data[blt_source+0];
		int blt_value;
		int loopcount;
		int writeoffs;
		blt_source+=2;

		switch (blt_commnd)
		{
			case 0x00: /* copy nn bytes */
				if (!blt_amount)
				{
					if(BLITLOG) mame_printf_debug("end of blit list\n");
					timer_set(machine, ATTOTIME_IN_USEC(500), NULL,0,rabbit_blit_done);
					return;
				}

				if(BLITLOG) mame_printf_debug("blit copy %02x bytes\n", blt_amount);
				for (loopcount=0;loopcount<blt_amount;loopcount++)
				{
					blt_value = ((blt_data[blt_source+1]<<8)|(blt_data[blt_source+0]));
					blt_source+=2;
					writeoffs=blt_oddflg+blt_column;
					rabbit_tilemap_ram[blt_tilemp][writeoffs]=(rabbit_tilemap_ram[blt_tilemp][writeoffs]&mask)|(blt_value<<shift);
					tilemap_mark_tile_dirty(rabbit_tilemap[blt_tilemp],writeoffs);

					blt_column++;
					blt_column&=0x7f;
				}

				break;

			case 0x02: /* fill nn bytes */
				if(BLITLOG) mame_printf_debug("blit fill %02x bytes\n", blt_amount);
				blt_value = ((blt_data[blt_source+1]<<8)|(blt_data[blt_source+0]));
				blt_source+=2;

				for (loopcount=0;loopcount<blt_amount;loopcount++)
				{
					writeoffs=blt_oddflg+blt_column;
					rabbit_tilemap_ram[blt_tilemp][writeoffs]=(rabbit_tilemap_ram[blt_tilemp][writeoffs]&mask)|(blt_value<<shift);
					tilemap_mark_tile_dirty(rabbit_tilemap[blt_tilemp],writeoffs);
					blt_column++;
					blt_column&=0x7f;
				}

				break;

			case 0x03: /* next line */
				if(BLITLOG) mame_printf_debug("blit: move to next line\n");
				blt_column = (rabbit_blitterregs[1]&0x00ff0000)>>16; /* --CC---- */
				blt_oddflg+=128;
				break;

			default: /* unknown / illegal */
				if(BLITLOG) mame_printf_debug("uknown blit command %02x\n",blt_commnd);
				break;
		}
	}

}



static WRITE32_HANDLER( rabbit_blitter_w )
{
	COMBINE_DATA(&rabbit_blitterregs[offset]);

	if (offset == 0x0c/4)
	{
		rabbit_do_blit(space->machine);
	}
}

static WRITE32_HANDLER( rabbit_eeprom_write )
{
	// don't disturb the EEPROM if we're not actually writing to it
	// (in particular, data & 0x100 here with mask = ffff00ff looks to be the watchdog)
	if (mem_mask == 0xff000000)
	{
		// latch the bit
		eeprom_write_bit(data & 0x01000000);

		// reset line asserted: reset.
		eeprom_set_cs_line((data & 0x04000000) ? CLEAR_LINE : ASSERT_LINE );

		// clock line asserted: write latch or select next bit to read
		eeprom_set_clock_line((data & 0x02000000) ? ASSERT_LINE : CLEAR_LINE );
	}
}

static ADDRESS_MAP_START( rabbit_map, ADDRESS_SPACE_PROGRAM, 32 )
	AM_RANGE(0x000000, 0x1fffff) AM_ROM
	AM_RANGE(0x000000, 0x000003) AM_WRITENOP // bug in code / emulation?
	AM_RANGE(0x000010, 0x000013) AM_WRITENOP // bug in code / emulation?
	AM_RANGE(0x000024, 0x000027) AM_WRITENOP // bug in code / emulation?
	AM_RANGE(0x00719c, 0x00719f) AM_WRITENOP // bug in code / emulation?
	AM_RANGE(0x200000, 0x200003) AM_READ_PORT("INPUTS") AM_WRITE(rabbit_eeprom_write)
	AM_RANGE(0x400010, 0x400013) AM_READ(randomrabbits) // gfx chip status?
	AM_RANGE(0x400980, 0x400983) AM_READ(randomrabbits) // sound chip status?
	AM_RANGE(0x400984, 0x400987) AM_READ(randomrabbits) // sound chip status?
	/* this lot are probably gfxchip/blitter etc. related */
	AM_RANGE(0x400010, 0x400013) AM_WRITEONLY AM_BASE( &rabbit_viewregs0 )
	AM_RANGE(0x400100, 0x400117) AM_WRITEONLY AM_BASE( &rabbit_tilemap_regs[0] ) // tilemap regs1
	AM_RANGE(0x400120, 0x400137) AM_WRITEONLY AM_BASE( &rabbit_tilemap_regs[1] ) // tilemap regs2
	AM_RANGE(0x400140, 0x400157) AM_WRITEONLY AM_BASE( &rabbit_tilemap_regs[2] ) // tilemap regs3
	AM_RANGE(0x400160, 0x400177) AM_WRITEONLY AM_BASE( &rabbit_tilemap_regs[3] ) // tilemap regs4
	AM_RANGE(0x400200, 0x40021b) AM_WRITEONLY AM_BASE( &rabbit_spriteregs ) // sprregs?
	AM_RANGE(0x400300, 0x400303) AM_WRITE(rabbit_rombank_w) // used during rom testing, rombank/area select + something else?
	AM_RANGE(0x400400, 0x400413) AM_WRITEONLY AM_BASE( &rabbit_viewregs6 ) // some global controls? (brightness etc.?)
	AM_RANGE(0x400500, 0x400503) AM_WRITEONLY AM_BASE( &rabbit_viewregs7 )
	AM_RANGE(0x400700, 0x40070f) AM_WRITE(rabbit_blitter_w) AM_BASE( &rabbit_blitterregs )
	AM_RANGE(0x400800, 0x40080f) AM_WRITEONLY AM_BASE( &rabbit_viewregs9 ) // never changes?
	AM_RANGE(0x400900, 0x40098f) AM_WRITE(rabbit_audio_w)
	/* hmm */
	AM_RANGE(0x479700, 0x479713) AM_WRITEONLY AM_BASE( &rabbit_viewregs10 )

	AM_RANGE(0x440000, 0x47ffff) AM_ROMBANK(1) // data (gfx / sound) rom readback for ROM testing
	/* tilemaps */
	AM_RANGE(0x480000, 0x483fff) AM_READWRITE(rabbit_tilemap0_r,rabbit_tilemap0_w)
	AM_RANGE(0x484000, 0x487fff) AM_READWRITE(rabbit_tilemap1_r,rabbit_tilemap1_w)
	AM_RANGE(0x488000, 0x48bfff) AM_READWRITE(rabbit_tilemap2_r,rabbit_tilemap2_w)
	AM_RANGE(0x48c000, 0x48ffff) AM_READWRITE(rabbit_tilemap3_r,rabbit_tilemap3_w)
	AM_RANGE(0x494000, 0x497fff) AM_RAM AM_BASE(&rabbit_spriteram) // sprites?
	AM_RANGE(0x4a0000, 0x4affff) AM_RAM_WRITE(rabbit_paletteram_dword_w) AM_BASE(&paletteram32)
	AM_RANGE(0xff0000, 0xffffff) AM_RAM
ADDRESS_MAP_END


static INPUT_PORTS_START( rabbit )
	PORT_START("INPUTS")
	PORT_BIT( 0x00000001, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM(eeprom_bit_r, NULL)	// as per code at 4d932
	PORT_BIT( 0x00000002, IP_ACTIVE_LOW, IPT_UNKNOWN ) // unlabeled in input test
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_SERVICE( 0x00000020, IP_ACTIVE_LOW )
	PORT_BIT( 0x00000040, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x00000080, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x00000100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x00000800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x00001000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x00002000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x00004000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x00008000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1)

	PORT_BIT( 0x00010000, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x00020000, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x00040000, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x00080000, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x00100000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x00200000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x00400000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x00800000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2)
	PORT_DIPNAME( 0x01000000, 0x01000000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(          0x01000000, DEF_STR( Off ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x02000000, 0x02000000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(          0x02000000, DEF_STR( Off ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x04000000, 0x04000000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(          0x04000000, DEF_STR( Off ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x08000000, 0x08000000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(          0x08000000, DEF_STR( Off ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x10000000, 0x10000000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(          0x10000000, DEF_STR( Off ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x20000000, 0x20000000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(          0x20000000, DEF_STR( Off ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x40000000, 0x40000000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(          0x40000000, DEF_STR( Off ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x80000000, 0x80000000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(          0x80000000, DEF_STR( Off ) )
	PORT_DIPSETTING(          0x00000000, DEF_STR( On ) )
INPUT_PORTS_END


static const gfx_layout rabbit_sprite_8x8x4_layout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 8,12,0,4, 24,28,16,20 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	8*32
};

static const gfx_layout rabbit_sprite_8x8x8_layout =
{
	8,8,
	RGN_FRAC(1,1),
	8,
	{ 0,1,2,3,4,5,6,7 },
	{ 8,0, 40,32,24,16,56, 48 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64 },
	8*64
};



static const gfx_layout rabbit_sprite_16x16x4_layout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ 0,1,2,3 },
	{ RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+12,RGN_FRAC(1,2)+0,RGN_FRAC(1,2)+4,RGN_FRAC(1,2)+24,RGN_FRAC(1,2)+28,RGN_FRAC(1,2)+16,RGN_FRAC(1,2)+20, 8,12,0,4, 24,28,16,20  },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,8*32,9*32,10*32,11*32,12*32,13*32,14*32,15*32 },
	16*32
};

static const gfx_layout rabbit_sprite_16x16x8_layout =
{
	16,16,
	RGN_FRAC(1,2),
	8,
	{ 0,1,2,3,4,5,6,7 },
	{ RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0,RGN_FRAC(1,2)+24,RGN_FRAC(1,2)+16,RGN_FRAC(1,2)+40,RGN_FRAC(1,2)+32,RGN_FRAC(1,2)+56,RGN_FRAC(1,2)+48, 8,0,24,16, 40,32,56,48  },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,8*64,9*64,10*64,11*64,12*64,13*64,14*64,15*64 },
	16*64
};

static const gfx_layout rabbit_8x8x4_layout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 4,0,12,8,20,16,28,24 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	8*32
};

static const gfx_layout rabbit_16x16x4_layout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 4,0,12,8, 20,16,28,24,36,32,44,40,52,48,60,56 },
	{ 0*64, 1*64, 2*64,3*64,4*64,5*64,6*64,7*64,8*64,9*64,10*64,11*64,12*64,13*64,14*64,15*64 },
	16*64
};

static const gfx_layout rabbit_8x8x8_layout =
{
	8,8,
	RGN_FRAC(1,1),
	8,
	{ 0,1,2,3,4,5,6,7 },
	{ 0,8,16,24,32,40,48,56 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64 },
	8*64
};

static const gfx_layout rabbit_16x16x8_layout =
{
	16,16,
	RGN_FRAC(1,1),
	8,
	{ 0,1,2,3,4,5,6,7 },
	{ 0,8,16,24,32,40,48,56, 64, 72, 80, 88, 96, 104, 112, 120 },
	{ 0*128, 1*128, 2*128,3*128,4*128,5*128,6*128,7*128,8*128,9*128,10*128,11*128,12*128,13*128,14*128,15*128 },
	16*128
};



static GFXDECODE_START( rabbit )
	/* this seems to be sprites */
	GFXDECODE_ENTRY( "gfx1", 0, rabbit_sprite_8x8x4_layout,   0x0, 0x1000  )
	GFXDECODE_ENTRY( "gfx1", 0, rabbit_sprite_16x16x4_layout, 0x0, 0x1000  )
	GFXDECODE_ENTRY( "gfx1", 0, rabbit_sprite_8x8x8_layout,   0x0, 0x1000  ) // wrong
	GFXDECODE_ENTRY( "gfx1", 0, rabbit_sprite_16x16x8_layout, 0x0, 0x1000  ) // wrong

	/* this seems to be backgrounds and tilemap gfx */
	GFXDECODE_ENTRY( "gfx2", 0, rabbit_8x8x4_layout,   0x0, 0x1000  )
	GFXDECODE_ENTRY( "gfx2", 0, rabbit_16x16x4_layout, 0x0, 0x1000  )
	GFXDECODE_ENTRY( "gfx2", 0, rabbit_8x8x8_layout,   0x0, 0x1000  )
	GFXDECODE_ENTRY( "gfx2", 0, rabbit_16x16x8_layout, 0x0, 0x1000  )

GFXDECODE_END

/* irq 6 = vblank
   irq 4 = blitter done?
   irq 5 = ?
   irq 1 = ?

  */

static INTERRUPT_GEN( rabbit_interrupts )
{
	int intlevel = 0;

	int line = 262 - cpu_getiloops(device);

	if(line==262)
	{
		intlevel = rabbit_vblirqlevel;
	}
	else
	{
		return;
	}

	cpu_set_input_line(device, intlevel, HOLD_LINE);
}

static MACHINE_DRIVER_START( rabbit )
	MDRV_CPU_ADD("maincpu",M68EC020,24000000) /* 24 MHz */
	MDRV_CPU_PROGRAM_MAP(rabbit_map)
	MDRV_CPU_VBLANK_INT_HACK(rabbit_interrupts,262)
	MDRV_NVRAM_HANDLER(93C46)

	MDRV_GFXDECODE(rabbit)

	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*16, 64*16)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 0*8, 28*8-1)
//  MDRV_SCREEN_VISIBLE_AREA(0*8, 64*16-1, 0*16, 64*16-1)
//  MDRV_SCREEN_VISIBLE_AREA(0*8, 20*16-1, 32*16, 48*16-1)

	MDRV_PALETTE_LENGTH(0x4000)
	MDRV_PALETTE_INIT( all_black ) // the status bar palette doesn't get transfered (or our colour select is wrong).. more obvious when it's black than in 'MAME default' colours

	MDRV_VIDEO_START(rabbit)
	MDRV_VIDEO_UPDATE(rabbit)
MACHINE_DRIVER_END





static DRIVER_INIT(rabbit)
{
	rabbit_banking = 1;
	rabbit_vblirqlevel = 6;
	rabbit_bltirqlevel = 4;
	/* 5 and 1 are also valid and might be raster related */
}



ROM_START( rabbit )
	ROM_REGION( 0x200000, "maincpu", 0 ) /* 68020 Code */
	ROM_LOAD32_BYTE( "jpr0.0", 0x000000, 0x080000, CRC(52bb18c0) SHA1(625bc8a4daa6d08cacd92d9110cf67a95a91325a) )
	ROM_LOAD32_BYTE( "jpr1.1", 0x000001, 0x080000, CRC(38299d0d) SHA1(72ccd51781b47636bb16ac18037cb3121d17199f) )
	ROM_LOAD32_BYTE( "jpr2.2", 0x000002, 0x080000, CRC(fa3fd91a) SHA1(ac0e658af30b37b752ede833b44ff5423b93bdb1) )
	ROM_LOAD32_BYTE( "jpr3.3", 0x000003, 0x080000, CRC(d22727ca) SHA1(8415cb2d3864b11fe5623ac65f2e28fd62c61bd1) )


// the rom test tests as if things were mapped like this (video chip / blitter space?)
#if 0
	ROM_REGION( 0x9000000, "test", ROMREGION_ERASE )
	ROM_LOAD32_WORD( "jfv0.00", 0x0000002, 0x400000, CRC(b2a4d3d3) SHA1(0ab71d82a37ff94442b91712a28d3470619ba575) ) // sprite gfx
	ROM_LOAD32_WORD( "jfv1.01", 0x0000000, 0x400000, CRC(83f3926e) SHA1(b1c479e675d35fc08c9a7648ff40348a24654e7e) ) // sprite gfx
	ROM_LOAD32_WORD( "jsn0.11", 0x0800002, 0x400000, CRC(e1f726e8) SHA1(598d75f3ff9e43ec8ce6131ed37f4345bf2f2d8e) ) // sound
	ROM_LOAD32_WORD( "jfv2.02", 0x2000002, 0x400000, CRC(b264bfb5) SHA1(8fafedb6af74150465b1773e80aef0edc3da4678) ) // sprite gfx
	ROM_LOAD32_WORD( "jfv3.03", 0x2000000, 0x400000, CRC(3e1a9be2) SHA1(2082a4ae8cda84cec5ea0fc08753db387bb70d41) ) // sprite gfx
	ROM_LOAD16_BYTE( "jbg0.40", 0x4000001, 0x200000, CRC(89662944) SHA1(ca916ba38480fa588af19fc9682603f5195ad6c7) ) // bg gfx (fails check?)
	ROM_LOAD16_BYTE( "jbg1.50", 0x6000000, 0x200000, CRC(1fc7f6e0) SHA1(b36062d2a9683683ffffd3003d5244a185f53280) ) // bg gfx
	ROM_LOAD16_BYTE( "jbg2.60", 0x8000001, 0x200000, CRC(aee265fc) SHA1(ec420ab30b9b5141162223fc1fbf663ad9f211e6) ) // bg gfx
#endif

	ROM_REGION( 0x1000000, "gfx1", 0 ) /* Sprite Roms (and Blitter Data) */
	ROM_LOAD32_WORD( "jfv0.00", 0x0000002, 0x400000, CRC(b2a4d3d3) SHA1(0ab71d82a37ff94442b91712a28d3470619ba575) )
	ROM_LOAD32_WORD( "jfv1.01", 0x0000000, 0x400000, CRC(83f3926e) SHA1(b1c479e675d35fc08c9a7648ff40348a24654e7e) )
	ROM_LOAD32_WORD( "jfv2.02", 0x0800002, 0x400000, CRC(b264bfb5) SHA1(8fafedb6af74150465b1773e80aef0edc3da4678) )
	ROM_LOAD32_WORD( "jfv3.03", 0x0800000, 0x400000, CRC(3e1a9be2) SHA1(2082a4ae8cda84cec5ea0fc08753db387bb70d41) )


	ROM_REGION( 0x600000, "gfx2", 0 ) /* BG Roms */
	ROM_LOAD( "jbg0.40", 0x000000, 0x200000, CRC(89662944) SHA1(ca916ba38480fa588af19fc9682603f5195ad6c7) )
	ROM_LOAD( "jbg1.50", 0x200000, 0x200000, CRC(1fc7f6e0) SHA1(b36062d2a9683683ffffd3003d5244a185f53280) )
	ROM_LOAD( "jbg2.60", 0x400000, 0x200000, CRC(aee265fc) SHA1(ec420ab30b9b5141162223fc1fbf663ad9f211e6) )

	ROM_REGION( 0x400000, "unknown", 0 ) /* sound rom */
	ROM_LOAD( "jsn0.11", 0x0000000, 0x400000, CRC(e1f726e8) SHA1(598d75f3ff9e43ec8ce6131ed37f4345bf2f2d8e) )

ROM_END


GAME( 1997, rabbit,        0, rabbit,  rabbit,  rabbit,  ROT0, "Electronic Arts / Aorn", "Rabbit", GAME_IMPERFECT_GRAPHICS | GAME_NO_SOUND ) // somewhat playable
