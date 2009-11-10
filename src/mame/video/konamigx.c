/*
 * video/konamigx.c - Konami GX video hardware (here there be dragons)
 *
 */

#include "driver.h"
#include "video/konamiic.h"
#include "machine/konamigx.h"

#define VERBOSE 0

static int layer_colorbase[4];
static INT32 gx_tilebanks[8], gx_oldbanks[8];
static int gx_tilemode, gx_rozenable, psac_colorbase, last_psac_colorbase;
static int gx_specialrozenable; // type 1 roz, with voxel height-map, rendered from 2 source tilemaps (which include height data) to temp bitmap for further processing
static tilemap *gx_psac_tilemap, *gx_psac_tilemap2;
extern UINT32 *gx_psacram, *gx_subpaletteram32;
static bitmap_t* type3_roz_temp_bitmap;

/* On Type-1 the K053936 output is rendered to these temporary bitmaps as raw data
   the 'voxel' effect to give the pixels height is a post-process operation on the
   output of the K053936 (this can clearly be seen in videos as large chunks of
   scenary flicker when in the distance due to single pixels in the K053936 output
   becoming visible / invisible due to drawing precision.

   -- however, progress on this has stalled as our K053936 doesn't seem to give
      the right output for post processing, I suspect the game is using some
      unsupported flipping modes (probably due to the way it's hooked up to the
      rest of the chips) which is causing entirely the wrong output.

   -- furthermore machine/konamigx.c (!) contains it's own implementation of
      the K053936_zoom_draw named K053936GP_zoom_draw ... It really shouldn't do,
      epsecially not in 'machine', which isn't meant to be video related.


   */
static bitmap_t *gxtype1_roz_dstbitmap;
static bitmap_t *gxtype1_roz_dstbitmap2;
static rectangle gxtype1_roz_dstbitmapclip;

static void (*game_tile_callback)(int layer, int *code, int *color, int *flags);

/* Run and Gun 2 / Rushing Heroes */
static TILE_GET_INFO( get_gx_psac_tile_info )
{
	int tileno, colour, flip = 0;
	if (tile_index&1)
	{
		tileno = gx_psacram[tile_index/2] & 0x00003fff;
		if      (gx_psacram[tile_index/2] & 0x00004000) flip |= TILE_FLIPX;
		if      (gx_psacram[tile_index/2] & 0x00008000) flip |= TILE_FLIPY;

	}
	else
	{
		tileno = (gx_psacram[tile_index/2] & 0x3fff0000)>>16;
		if       (gx_psacram[tile_index/2] & 0x40000000) flip |= TILE_FLIPX;
		if       (gx_psacram[tile_index/2] & 0x80000000) flip |= TILE_FLIPY;

	}

	colour = (psac_colorbase << 4);

	SET_TILE_INFO(0, tileno, colour, TILE_FLIPYX(flip));
}

UINT32* konamigx_type3_psac2_bank;
int konamigx_type3_psac2_actual_bank;
int konamigx_type3_psac2_actual_last_bank = 0;

WRITE32_HANDLER( konamigx_type3_psac2_bank_w )
{
	// other bits are used for something...

	COMBINE_DATA(&konamigx_type3_psac2_bank[offset]);
	konamigx_type3_psac2_actual_bank = (konamigx_type3_psac2_bank[0] & 0x10000000) >> 28;

	if (konamigx_type3_psac2_actual_bank!=konamigx_type3_psac2_actual_last_bank)
	{
		tilemap_mark_all_tiles_dirty (gx_psac_tilemap);
		konamigx_type3_psac2_actual_last_bank = konamigx_type3_psac2_actual_bank;
	}

}



/* Soccer Superstars (tile and flip bits now TRUSTED) */
 static TILE_GET_INFO( get_gx_psac3_tile_info )
 {
 	int tileno, colour, flip;
 	UINT8 *tmap = memory_region(machine, "gfx4");

	int base_index = tile_index;

	if (konamigx_type3_psac2_actual_bank)
		base_index+=0x20000/2;


	tileno =  tmap[base_index*2] | ((tmap[(base_index*2)+1] & 0x0f)<<8);
	colour = (tmap[(base_index*2)+1]&0xc0)>>6;

 	flip = 0;
	if (tmap[(base_index*2)+1] & 0x20) flip |= TILE_FLIPY;
	if (tmap[(base_index*2)+1] & 0x10) flip |= TILE_FLIPX;

 	SET_TILE_INFO(0, tileno, colour, flip);
 }

/* PSAC4 */
/* these tilemaps are weird in both format and content, one of them
   doesn't really look like it should be displayed? - it's height data */
static TILE_GET_INFO( get_gx_psac1a_tile_info )
{
	int tileno, colour, flipx,flipy;
	int flip;
	flip=0;
	colour = 0;

	tileno = (gx_psacram[tile_index*2] & 0x00003fff)>>0;

	// scanrows
	//flipx  = (gx_psacram[tile_index*2+1] & 0x00800000)>>23;
	//flipy  = (gx_psacram[tile_index*2+1] & 0x00400000)>>22;
	// scancols
	flipy  = (gx_psacram[tile_index*2+1] & 0x00800000)>>23;
	flipx  = (gx_psacram[tile_index*2+1] & 0x00400000)>>22;

	if (flipx) flip |= TILE_FLIPX;
	if (flipy) flip |= TILE_FLIPY;

	SET_TILE_INFO(1, tileno, colour, flip);
}

static TILE_GET_INFO( get_gx_psac1b_tile_info )
{
	int tileno, colour, flipx,flipy;
	int flip;
	flip=0;

	colour = 0;
	tileno = (gx_psacram[tile_index*2+1] & 0x00003fff)>>0;

	// scanrows
	//flipx  = (gx_psacram[tile_index*2+1] & 0x00800000)>>23;
	//flipy  = (gx_psacram[tile_index*2+1] & 0x00400000)>>22;
	// scancols
	flipy  = (gx_psacram[tile_index*2+1] & 0x00200000)>>21;
	flipx  = (gx_psacram[tile_index*2+1] & 0x00100000)>>20;

	if (flipx) flip |= TILE_FLIPX;
	if (flipy) flip |= TILE_FLIPY;

	SET_TILE_INFO(0, tileno, colour, flip);
}

static void konamigx_type2_tile_callback(int layer, int *code, int *color, int *flags)
{
	int d = *code;

	*code = (gx_tilebanks[(d & 0xe000)>>13]<<13) + (d & 0x1fff);
	K055555GX_decode_vmixcolor(layer, color);
}

static void konamigx_alpha_tile_callback(int layer, int *code, int *color, int *flags)
{
	int mixcode;
	int d = *code;

	mixcode = K055555GX_decode_vmixcolor(layer, color);

	if (mixcode < 0)
		*code = (gx_tilebanks[(d & 0xe000)>>13]<<13) + (d & 0x1fff);
	else
	{
		/* save mixcode and mark tile alpha (unimplemented) */
		*code = 0;

		if (VERBOSE)
			popmessage("skipped alpha tile(layer=%d mix=%d)", layer, mixcode);
	}
}

/*
> bits 8-13 are the low priority bits
> i.e. pri 0-5
> pri 6-7 can be either 1, bits 14,15 or bits 16,17
> contro.bit 2 being 0 forces the 1
> when control.bit 2 is 1, control.bit 3 selects between the two
> 0 selects 16,17
> that gives you the entire 8 bits of the sprite priority
> ok, lemme see if I've got this.  bit2 = 0 means the top bits are 11, bit2=1 means the top bits are bits 14/15 (of the whatever word?) else
+16+17?
> bit3=1 for the second

 *   6  | ---------xxxxxxx | "color", but depends on external connections


> there are 8 color lines entering the 5x5
> that means the palette is 4 bits, not 5 as you currently have
> the bits 4-9 are the low priority bits
> bits 10/11 or 12/13 are the two high priority bits, depending on the control word
> and bits 14/15 are the shadow bits
> mix0/1 and brit0/1 come from elsewhere
> they come from the '673 all right, but not from word 6
> and in fact the top address bits are highly suspect
> only 18 of the address bits go to the roms
> the next 2 go to cai0/1 and the next 4 to bk0-3
> (the '246 indexes the roms, the '673 reads the result)
> the roms are 64 bits wide
> so, well, the top bits of the code are suspicious
*/

static void _gxcommoninitnosprites(running_machine *machine)
{
	int i;

	K054338_vh_start(machine);
	K055555_vh_start(machine);

	konamigx_mixer_init(machine, 0);

	for (i = 0; i < 8; i++)
	{
		gx_tilebanks[i] = gx_oldbanks[i] = 0;
	}

	state_save_register_global_array(machine, gx_tilebanks);

	gx_tilemode = 0;

	gx_rozenable = 0;
	gx_specialrozenable = 0;

	// Documented relative offsets of non-flipped games are (-2, 0, 2, 3),(0, 0, 0, 0).
	// (+ve values move layers to the right and -ve values move layers to the left)
	// In most cases only a constant is needed to add to the X offsets to yield correct
	// displacement. This should be done by the CCU but the CRT timings have not been
	// figured out.
	K056832_set_LayerOffset(0, -2, 0);
	K056832_set_LayerOffset(1,  0, 0);
	K056832_set_LayerOffset(2,  2, 0);
	K056832_set_LayerOffset(3,  3, 0);
}

static void _gxcommoninit(running_machine *machine)
{
	// (+ve values move objects to the right and -ve values move objects to the left)
	K055673_vh_start(machine, "gfx2", K055673_LAYOUT_GX, -26, -23, konamigx_type2_sprite_callback);

	_gxcommoninitnosprites(machine);
}


VIDEO_START(konamigx_5bpp)
{
	if (!strcmp(machine->gamedrv->name,"sexyparo"))
		game_tile_callback = konamigx_alpha_tile_callback;
	else
		game_tile_callback = konamigx_type2_tile_callback;

	K056832_vh_start(machine, "gfx1", K056832_BPP_5, 0, NULL, game_tile_callback, 0);

	_gxcommoninit(machine);

	/* here are some hand tuned per game scroll offsets to go with the per game visible areas,
       i see no better way of doing this for now... */

	if (!strcmp(machine->gamedrv->name,"tbyahhoo"))
	{
		K056832_set_UpdateMode(1);
		gx_tilemode = 1;
	} else

	if (!strcmp(machine->gamedrv->name,"puzldama"))
	{
		K053247GP_set_SpriteOffset(-46, -23);
		konamigx_mixer_primode(5);
	} else

	if (!strcmp(machine->gamedrv->name,"daiskiss"))
	{
		konamigx_mixer_primode(4);
	} else

	if (!strcmp(machine->gamedrv->name,"gokuparo") || !strcmp(machine->gamedrv->name,"fantjour"))
 	{
		K053247GP_set_SpriteOffset(-46, -23);
	} else

	if (!strcmp(machine->gamedrv->name,"sexyparo"))
	{
		K053247GP_set_SpriteOffset(-42, -23);
	}
}

VIDEO_START(winspike)
{
	K056832_vh_start(machine, "gfx1", K056832_BPP_8, 0, NULL, konamigx_alpha_tile_callback, 2);
	K055673_vh_start(machine, "gfx2", K055673_LAYOUT_LE2, -53, -23, konamigx_type2_sprite_callback);

	_gxcommoninitnosprites(machine);
}

VIDEO_START(dragoonj)
{
	K056832_vh_start(machine, "gfx1", K056832_BPP_5, 1, NULL, konamigx_type2_tile_callback, 0);
	K055673_vh_start(machine, "gfx2", K055673_LAYOUT_RNG, -53, -23, konamigx_dragoonj_sprite_callback);

	_gxcommoninitnosprites(machine);

	K056832_set_LayerOffset(0, -2+1, 0);
	K056832_set_LayerOffset(1,  0+1, 0);
	K056832_set_LayerOffset(2,  2+1, 0);
	K056832_set_LayerOffset(3,  3+1, 0);
}

VIDEO_START(le2)
{
	K056832_vh_start(machine, "gfx1", K056832_BPP_8, 1, NULL, konamigx_type2_tile_callback, 0);
	K055673_vh_start(machine, "gfx2", K055673_LAYOUT_LE2, -46, -23, konamigx_le2_sprite_callback);

	_gxcommoninitnosprites(machine);

	konamigx_mixer_primode(-1); // swapped layer B and C priorities?
}

VIDEO_START(konamigx_6bpp)
{
	K056832_vh_start(machine, "gfx1", K056832_BPP_6, 0, NULL, konamigx_type2_tile_callback, 0);

	_gxcommoninit(machine);

	if (!strcmp(machine->gamedrv->name,"tokkae") || !strcmp(machine->gamedrv->name,"tkmmpzdm"))
	{
		K053247GP_set_SpriteOffset(-46, -23);
		konamigx_mixer_primode(5);
	}
}

VIDEO_START(konamigx_type3)
{
	int width = video_screen_get_width(machine->primary_screen);
	int height = video_screen_get_height(machine->primary_screen);

	K056832_vh_start(machine, "gfx1", K056832_BPP_6, 0, NULL, konamigx_type2_tile_callback, 1);
	K055673_vh_start(machine, "gfx2", K055673_LAYOUT_GX6, -132, -24, konamigx_type2_sprite_callback);

	_gxcommoninitnosprites(machine);

	gx_psac_tilemap = tilemap_create(machine, get_gx_psac3_tile_info, tilemap_scan_cols,  16, 16, 256, 256);
	gx_rozenable = 0;
	gx_specialrozenable = 2;


	/* set up tile layers */
	type3_roz_temp_bitmap = auto_bitmap_alloc(machine, width, height, BITMAP_FORMAT_INDEXED16);


	//tilemap_set_flip(gx_psac_tilemap, TILEMAP_FLIPX| TILEMAP_FLIPY);

	K053936_wraparound_enable(0, 1);
	K053936GP_set_offset(0, 0, 0);

	K056832_set_LayerOffset(0,  -48, 0);
	K056832_set_LayerOffset(1,  -48, 0);
	K056832_set_LayerOffset(2,  -48, 0);
	K056832_set_LayerOffset(3,  -48, 0);
}

VIDEO_START(konamigx_type4)
{
	K056832_vh_start(machine, "gfx1", K056832_BPP_8, 0, NULL, konamigx_type2_tile_callback, 0);
	K055673_vh_start(machine, "gfx2", K055673_LAYOUT_GX6, -78, -24, konamigx_type2_sprite_callback);

	_gxcommoninitnosprites(machine);

	gx_psac_tilemap = tilemap_create(machine, get_gx_psac_tile_info, tilemap_scan_cols,  16, 16, 128, 128);
	gx_rozenable = 0;
	gx_specialrozenable = 3;


	K056832_set_LayerOffset(0,  -22, 0);
	K056832_set_LayerOffset(1,  -22, 0);
	K056832_set_LayerOffset(2,  -22, 0);
	K056832_set_LayerOffset(3,  -22, 0);


	K053936_wraparound_enable(0, 0);
	K053936GP_set_offset(1, -36, 0);
}

VIDEO_START(konamigx_6bpp_2)
{
	K056832_vh_start(machine, "gfx1", K056832_BPP_6, 1, NULL, konamigx_type2_tile_callback, 0);

	if (!strcmp(machine->gamedrv->name,"salmndr2") || !strcmp(machine->gamedrv->name,"salmndr2a"))
	{
		K055673_vh_start(machine, "gfx2", K055673_LAYOUT_GX6, -48, -23, konamigx_salmndr2_sprite_callback);

		_gxcommoninitnosprites(machine);
	}
	else
	{
		_gxcommoninit(machine);
	}
}

VIDEO_START(opengolf)
{
	K056832_vh_start(machine, "gfx1", K056832_BPP_5, 0, NULL, konamigx_type2_tile_callback, 0);
	K055673_vh_start(machine, "gfx2", K055673_LAYOUT_GX6, -53, -23, konamigx_type2_sprite_callback);

	_gxcommoninitnosprites(machine);

	K056832_set_LayerOffset(0, -2+1, 0);
	K056832_set_LayerOffset(1,  0+1, 0);
	K056832_set_LayerOffset(2,  2+1, 0);
	K056832_set_LayerOffset(3,  3+1, 0);

	gx_psac_tilemap = tilemap_create(machine, get_gx_psac1a_tile_info, tilemap_scan_cols,  16, 16, 128, 128);
	gx_psac_tilemap2 = tilemap_create(machine, get_gx_psac1b_tile_info, tilemap_scan_cols,  16, 16, 128, 128);

	// transparency will be handled manually in post-processing
	//tilemap_set_transparent_pen(gx_psac_tilemap, 0);
	//tilemap_set_transparent_pen(gx_psac_tilemap2, 0);

	gx_rozenable = 0;
	gx_specialrozenable = 1;

	gxtype1_roz_dstbitmap =  auto_bitmap_alloc(machine,512,512,BITMAP_FORMAT_INDEXED16); // BITMAP_FORMAT_INDEXED16 because we NEED the raw pen data for post-processing
	gxtype1_roz_dstbitmap2 = auto_bitmap_alloc(machine,512,512,BITMAP_FORMAT_INDEXED16); // BITMAP_FORMAT_INDEXED16 because we NEED the raw pen data for post-processing


	gxtype1_roz_dstbitmapclip.min_x = 0;
	gxtype1_roz_dstbitmapclip.max_x = 512-1;
	gxtype1_roz_dstbitmapclip.min_y = 0;
	gxtype1_roz_dstbitmapclip.max_y = 512-1;


	K053936_wraparound_enable(0, 1);
	K053936GP_set_offset(0, 0, 0);

	// urgh.. the priority bitmap is global, and because our temp bitmaps are bigger than the screen, this causes issues.. so just allocate something huge
	// until there is a better solution, or priority bitmap can be specified manually.
	machine->priority_bitmap = auto_bitmap_alloc(machine,2048,2048,BITMAP_FORMAT_INDEXED16);

}

VIDEO_START(racinfrc)
{
	K056832_vh_start(machine, "gfx1", K056832_BPP_6, 0, NULL, konamigx_type2_tile_callback, 0);
	K055673_vh_start(machine, "gfx2", K055673_LAYOUT_GX, -53, -23, konamigx_type2_sprite_callback);

	_gxcommoninitnosprites(machine);

	K056832_set_LayerOffset(0, -2+1, 0);
	K056832_set_LayerOffset(1,  0+1, 0);
	K056832_set_LayerOffset(2,  2+1, 0);
	K056832_set_LayerOffset(3,  3+1, 0);

	gx_psac_tilemap = tilemap_create(machine, get_gx_psac1a_tile_info, tilemap_scan_cols,  16, 16, 128, 128);
	gx_psac_tilemap2 = tilemap_create(machine, get_gx_psac1b_tile_info, tilemap_scan_cols,  16, 16, 128, 128);

	// transparency will be handled manually in post-processing
	//tilemap_set_transparent_pen(gx_psac_tilemap, 0);
	//tilemap_set_transparent_pen(gx_psac_tilemap2, 0);

	gx_rozenable = 0;
	gx_specialrozenable = 1;

	gxtype1_roz_dstbitmap =  auto_bitmap_alloc(machine,512,512,BITMAP_FORMAT_INDEXED16); // BITMAP_FORMAT_INDEXED16 because we NEED the raw pen data for post-processing
	gxtype1_roz_dstbitmap2 = auto_bitmap_alloc(machine,512,512,BITMAP_FORMAT_INDEXED16); // BITMAP_FORMAT_INDEXED16 because we NEED the raw pen data for post-processing


	gxtype1_roz_dstbitmapclip.min_x = 0;
	gxtype1_roz_dstbitmapclip.max_x = 512-1;
	gxtype1_roz_dstbitmapclip.min_y = 0;
	gxtype1_roz_dstbitmapclip.max_y = 512-1;


	K053936_wraparound_enable(0, 1);
	K053936GP_set_offset(0, 0, 0);

	// urgh.. the priority bitmap is global, and because our temp bitmaps are bigger than the screen, this causes issues.. so just allocate something huge
	// until there is a better solution, or priority bitmap can be specified manually.
	machine->priority_bitmap = auto_bitmap_alloc(machine,2048,2048,BITMAP_FORMAT_INDEXED16);


}

VIDEO_UPDATE(konamigx)
{
	int i, newbank, newbase, dirty, unchained;

	/* if any banks are different from last render, we need to flush the planes */
	for (dirty = 0, i = 0; i < 8; i++)
	{
		newbank = gx_tilebanks[i];
		if (gx_oldbanks[i] != newbank) { gx_oldbanks[i] = newbank; dirty = 1; }
	}

	if (gx_tilemode == 0)
	{
		// driver approximates tile update in mode 0 for speed
		unchained = K056832_get_LayerAssociation();
		for (i=0; i<4; i++)
		{
			newbase = K055555_get_palette_index(i)<<6;
			if (layer_colorbase[i] != newbase)
			{
				layer_colorbase[i] = newbase;

				if (unchained)
					K056832_mark_plane_dirty(i);
				else
					dirty = 1;
			}
		}
	}
	else
	{
		// K056832 does all the tracking in mode 1 for accuracy (Twinbee needs this)
	}

	// sub2 is PSAC colorbase on GX
	if (gx_rozenable)
	{
		last_psac_colorbase = psac_colorbase;
		psac_colorbase = K055555_get_palette_index(6);

		if (psac_colorbase != last_psac_colorbase)
		{
			tilemap_mark_all_tiles_dirty(gx_psac_tilemap);
			if (gx_rozenable == 3)
			{
				tilemap_mark_all_tiles_dirty(gx_psac_tilemap2);
			}
		}
	}

	if (dirty) K056832_MarkAllTilemapsDirty();

	// Type-1
	if (gx_specialrozenable == 1)
	{
		K053936_0_zoom_draw(gxtype1_roz_dstbitmap, &gxtype1_roz_dstbitmapclip,gx_psac_tilemap, 0,0,0); // height data
		K053936_0_zoom_draw(gxtype1_roz_dstbitmap2,&gxtype1_roz_dstbitmapclip,gx_psac_tilemap2,0,0,0); // colour data (+ some voxel height data?)
	}



	if (gx_specialrozenable==3)
	{
		// hold W to see the roz layer
		if ( input_code_pressed(screen->machine, KEYCODE_W) )
		{
			konamigx_mixer(screen->machine, bitmap, cliprect,0, 0, gx_psac_tilemap, GXSUB_8BPP,  0, 0);
		}
		else
		{
			konamigx_mixer(screen->machine, bitmap, cliprect,0, 0, 0, 0,  0, 0);
		}
	}
 	// hack, draw the roz tilemap if W is held
 	// todo: fix so that it works with the mixer without crashing(!)
	else if (gx_specialrozenable == 2)
	{
		// we're going to throw half of this away anyway in post-process, so only render what's needed
		rectangle temprect;
		temprect.min_x = cliprect->min_x;
		temprect.max_x = cliprect->min_x+320;
		temprect.min_y = cliprect->min_y;
		temprect.max_y = cliprect->max_y;

		K053936_0_zoom_draw(type3_roz_temp_bitmap, &temprect,gx_psac_tilemap, 0,0,0); // soccerss playfield
 		konamigx_mixer(screen->machine, bitmap, cliprect, 0, 0, 0, 0, 0, type3_roz_temp_bitmap);
	}
	else
	{
 		konamigx_mixer(screen->machine, bitmap, cliprect, 0, 0, 0, 0, 0, 0);
	}



	/* Hack! draw type-1 roz layer here for testing purposes only */
	if (gx_specialrozenable == 1)
	{
		const pen_t *paldata = screen->machine->pens;

		if ( input_code_pressed(screen->machine, KEYCODE_W) )
		{
			int y,x;

			// make it flicker, to compare positioning
			//if (video_screen_get_frame_number(screen) & 1)
			{

				for (y=0;y<256;y++)
				{
					//UINT16* src = BITMAP_ADDR16( gxtype1_roz_dstbitmap, y, 0);

					//UINT32* dst = BITMAP_ADDR32( bitmap, y, 0);
					// ths K053936 rendering should probably just be flipped
					// this is just kludged to align the racing force 2d logo
					UINT16* src = BITMAP_ADDR16( gxtype1_roz_dstbitmap2, y+30, 0);
					UINT32* dst = BITMAP_ADDR32( bitmap, 256-y, 0);

					for (x=0;x<512;x++)
					{
						UINT16 dat = src[x];
						dst[x] = paldata[dat];
					}
				}
			}

		}

	}

	return 0;
}


WRITE32_HANDLER( konamigx_palette_w )
{
	int r,g,b;

	COMBINE_DATA(&paletteram32[offset]);

 	r = (paletteram32[offset] >>16) & 0xff;
	g = (paletteram32[offset] >> 8) & 0xff;
	b = (paletteram32[offset] >> 0) & 0xff;

	palette_set_color(space->machine,offset,MAKE_RGB(r,g,b));
}

WRITE32_HANDLER( konamigx_palette2_w )
{
	int r,g,b;

	COMBINE_DATA(&gx_subpaletteram32[offset]);

 	r = (gx_subpaletteram32[offset] >>16) & 0xff;
	g = (gx_subpaletteram32[offset] >> 8) & 0xff;
	b = (gx_subpaletteram32[offset] >> 0) & 0xff;

	offset += (0x8000/4);

	palette_set_color(space->machine,offset,MAKE_RGB(r,g,b));
}

INLINE void set_color_555(running_machine *machine, pen_t color, int rshift, int gshift, int bshift, UINT16 data)
{
	palette_set_color_rgb(machine, color, pal5bit(data >> rshift), pal5bit(data >> gshift), pal5bit(data >> bshift));
}

// main monitor for type 3
WRITE32_HANDLER( konamigx_555_palette_w )
{
	UINT32 coldat;
	COMBINE_DATA(&paletteram32[offset]);

	coldat = paletteram32[offset];

	set_color_555(space->machine, offset*2, 0, 5, 10,coldat >> 16);
	set_color_555(space->machine, offset*2+1, 0, 5, 10,coldat & 0xffff);
}

// sub monitor for type 3
WRITE32_HANDLER( konamigx_555_palette2_w )
{
	UINT32 coldat;
	COMBINE_DATA(&gx_subpaletteram32[offset]);
	coldat = gx_subpaletteram32[offset];

	offset += (0x4000/4);

	set_color_555(space->machine, offset*2, 0, 5, 10,coldat >> 16);
	set_color_555(space->machine, offset*2+1, 0, 5, 10,coldat & 0xffff);
}

WRITE32_HANDLER( konamigx_tilebank_w )
{
	if (ACCESSING_BITS_24_31)
		gx_tilebanks[offset*4] = (data>>24)&0xff;
	if (ACCESSING_BITS_16_23)
		gx_tilebanks[offset*4+1] = (data>>16)&0xff;
	if (ACCESSING_BITS_8_15)
		gx_tilebanks[offset*4+2] = (data>>8)&0xff;
	if (ACCESSING_BITS_0_7)
		gx_tilebanks[offset*4+3] = data&0xff;
}

// type 1 RAM-based PSAC tilemap
WRITE32_HANDLER(konamigx_t1_psacmap_w)
{
	COMBINE_DATA(&gx_psacram[offset]);
	tilemap_mark_tile_dirty(gx_psac_tilemap, offset/2);
	tilemap_mark_tile_dirty(gx_psac_tilemap2, offset/2);
}

// type 4 RAM-based PSAC tilemap
WRITE32_HANDLER( konamigx_t4_psacmap_w )
{
	COMBINE_DATA(&gx_psacram[offset]);

	tilemap_mark_tile_dirty(gx_psac_tilemap, offset*2);
	tilemap_mark_tile_dirty(gx_psac_tilemap, (offset*2)+1);
}

