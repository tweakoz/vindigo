/***************************************************************************

    Irem M58 hardware

***************************************************************************/

#include "driver.h"
#include "deprecat.h"
#include "m58.h"
#include "video/resnet.h"

UINT8 *yard_scroll_x_low;
UINT8 *yard_scroll_x_high;
UINT8 *yard_scroll_y_low;
UINT8 *yard_score_panel_disabled;
static mame_bitmap *scroll_panel_bitmap;

static tilemap *bg_tilemap;

#define SCROLL_PANEL_WIDTH  (14*4)
#define RADAR_PALETTE_BASE	(256)



/*************************************
 *
 *  Palette configuration
 *
 *************************************/

PALETTE_INIT( yard )
{
	const UINT8 *char_lopal = color_prom + 0x000;
	const UINT8 *char_hipal = color_prom + 0x100;
	const UINT8 *sprite_pal = color_prom + 0x200;
	const UINT8 *sprite_table = color_prom + 0x220;
	const UINT8 *radar_lopal = color_prom + 0x320;
	const UINT8 *radar_hipal = color_prom + 0x420;
	static const int resistances_3[3] = { 1000, 470, 220 };
	static const int resistances_2[2]  = { 470, 220 };
	double weights_r[3], weights_g[3], weights_b[3], scale;
	int i;

	machine->colortable = colortable_alloc(machine, 256+256+16);

	/* compute palette information for characters/radar */
	scale = compute_resistor_weights(0,	255, -1.0,
			2, resistances_2, weights_r, 0, 0,
			3, resistances_3, weights_g, 0, 0,
			3, resistances_3, weights_b, 0, 0);

	/* character palette */
	for (i = 0; i < 256; i++)
	{
		UINT8 promval = (char_lopal[i] & 0x0f) | (char_hipal[i] << 4);
		int r = combine_2_weights(weights_r, BIT(promval,6), BIT(promval,7));
		int g = combine_3_weights(weights_g, BIT(promval,3), BIT(promval,4), BIT(promval,5));
		int b = combine_3_weights(weights_b, BIT(promval,0), BIT(promval,1), BIT(promval,2));

		colortable_palette_set_color(machine->colortable, i, MAKE_RGB(r,g,b));
	}

	/* radar palette */
	for (i = 0; i < 256; i++)
	{
		UINT8 promval = (radar_lopal[i] & 0x0f) | (radar_hipal[i] << 4);
		int r = combine_2_weights(weights_r, BIT(promval,6), BIT(promval,7));
		int g = combine_3_weights(weights_g, BIT(promval,3), BIT(promval,4), BIT(promval,5));
		int b = combine_3_weights(weights_b, BIT(promval,0), BIT(promval,1), BIT(promval,2));

		colortable_palette_set_color(machine->colortable, 256+i, MAKE_RGB(r,g,b));
	}

	/* compute palette information for sprites */
	scale = compute_resistor_weights(0,	255, scale,
			2, resistances_2, weights_r, 470, 0,
			3, resistances_3, weights_g, 470, 0,
			3, resistances_3, weights_b, 470, 0);

	/* sprite palette */
	for (i = 0; i < 16; i++)
	{
		UINT8 promval = sprite_pal[i];
		int r = combine_2_weights(weights_r, BIT(promval,6), BIT(promval,7));
		int g = combine_3_weights(weights_g, BIT(promval,3), BIT(promval,4), BIT(promval,5));
		int b = combine_3_weights(weights_b, BIT(promval,0), BIT(promval,1), BIT(promval,2));

		colortable_palette_set_color(machine->colortable, 256+256+i, MAKE_RGB(r,g,b));
	}

	/* character lookup table */
	for (i = 0; i < 256; i++)
		colortable_entry_set_value(machine->colortable, i, i);

	/* radar lookup table */
	for (i = 0; i < 256; i++)
		colortable_entry_set_value(machine->colortable, 256+i, 256+i);

	/* sprite lookup table */
	for (i = 0; i < 256; i++)
	{
		UINT8 promval = sprite_table[i] & 0x0f;
		colortable_entry_set_value(machine->colortable, 256+256+i, 256+256+promval);
	}
}



/*************************************
 *
 *  Video RAM access
 *
 *************************************/

WRITE8_HANDLER( yard_videoram_w )
{
	videoram[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap, offset / 2);
}


WRITE8_HANDLER( yard_scroll_panel_w )
{
	int sx,sy,i;

	sx = ( offset % 16 );
	sy = ( offset / 16 );

	if (sx < 1 || sx > 14)  return;

	sx = 4 * (sx - 1);

	for (i = 0;i < 4;i++)
	{
		int col;

		col = (data >> i) & 0x11;
		col = ((col >> 3) | col) & 3;

		*BITMAP_ADDR16(scroll_panel_bitmap, sy, sx + i) = RADAR_PALETTE_BASE + (sy & 0xfc) + col;
	}
}



/*************************************
 *
 *  Tilemap info callback
 *
 *************************************/

static TILE_GET_INFO( yard_get_bg_tile_info )
{
	int offs = tile_index * 2;
	int attr = videoram[offs + 1];
	int code = videoram[offs] + ((attr & 0xc0) << 2);
	int color = attr & 0x1f;
	int flags = (attr & 0x20) ? TILE_FLIPX : 0;

	SET_TILE_INFO(0, code, color, flags);
}


static UINT32 yard_tilemap_scan_rows( UINT32 col, UINT32 row, UINT32 num_cols, UINT32 num_rows )
{
	/* logical (col,row) -> memory offset */
	if (col >= 32)
		return (row+32)*32 + col-32;
	else
		return row*32 + col;
}



/*************************************
 *
 *  Video startup
 *
 *************************************/

VIDEO_START( yard )
{
	bg_tilemap = tilemap_create(yard_get_bg_tile_info, yard_tilemap_scan_rows,  8, 8, 64, 32);
	tilemap_set_scrolldx(bg_tilemap, machine->screen[0].visarea.min_x, machine->screen[0].width - (machine->screen[0].visarea.max_x + 1));
	tilemap_set_scrolldy(bg_tilemap, machine->screen[0].visarea.min_y - 8, machine->screen[0].height + 16 - (machine->screen[0].visarea.max_y + 1));

	scroll_panel_bitmap = auto_bitmap_alloc(SCROLL_PANEL_WIDTH, machine->screen[0].height, machine->screen[0].format);
}



/*************************************
 *
 *  Outputs
 *
 *************************************/

WRITE8_HANDLER( yard_flipscreen_w )
{
	/* screen flip is handled both by software and hardware */
	flip_screen_set((data & 0x01) ^ (~readinputportbytag("DSW2") & 0x01));

	coin_counter_w(0, data & 0x02);
	coin_counter_w(1, data & 0x20);
}



/*************************************
 *
 *  Sprite rendering
 *
 *************************************/

#define DRAW_SPRITE(code, sy) drawgfx(bitmap, machine->gfx[1], code, color, flipx, flipy, sx, sy, cliprect, TRANSPARENCY_PENS, colortable_get_transpen_mask(machine->colortable, machine->gfx[1], color, 512));

static void draw_sprites(running_machine *machine, mame_bitmap *bitmap, const rectangle *cliprect )
{
	int offs;

	for (offs = spriteram_size - 4; offs >= 0; offs -= 4)
	{
		int attr = spriteram[offs + 1];
		int bank = (attr & 0x20) >> 5;
		int code1 = spriteram[offs + 2] & 0xbf;
		int code2 = 0;
		int color = attr & 0x1f;
		int flipx = attr & 0x40;
		int flipy = attr & 0x80;
		int sx = spriteram[offs + 3];
		int sy1 = 233 - spriteram[offs];
		int sy2 = 0;

		if (flipy)
		{
			code2 = code1;
			code1 += 0x40;
		}
		else
		{
			code2 = code1 + 0x40;
		}

		if (flip_screen)
		{
			sx = 240 - sx;
			sy2 = 192 - sy1;
			sy1 = sy2 + 0x10;
			flipx = !flipx;
			flipy = !flipy;
		}
		else
		{
			sy2 = sy1 + 0x10;
		}

		DRAW_SPRITE(code1 + 256 * bank, machine->screen[0].visarea.min_y + sy1)
		DRAW_SPRITE(code2 + 256 * bank, machine->screen[0].visarea.min_y + sy2)
	}
}



/*************************************
 *
 *  Radar panel rendering
 *
 *************************************/

static void draw_panel( running_machine *machine, mame_bitmap *bitmap, const rectangle *cliprect )
{
	if (! *yard_score_panel_disabled)
	{
		static const rectangle clippanel =
		{
			26*8, 32*8-1,
			1*8, 31*8-1
		};
		static const rectangle clippanelflip =
		{
			0*8, 6*8-1,
			1*8, 31*8-1
		};
		rectangle clip = flip_screen ? clippanelflip : clippanel;
		int sx = flip_screen ? cliprect->min_x - 8 : cliprect->max_x + 1 - SCROLL_PANEL_WIDTH;
		int yoffs = flip_screen ? -40 : -16;

		clip.min_y += machine->screen[0].visarea.min_y + yoffs;
		clip.max_y += machine->screen[0].visarea.max_y + yoffs;

		copybitmap(bitmap, scroll_panel_bitmap, flip_screen, flip_screen,
				   sx, machine->screen[0].visarea.min_y + yoffs, &clip);
	}
}



/*************************************
 *
 *  Video update
 *
 *************************************/

VIDEO_UPDATE( yard )
{
	tilemap_set_scrollx(bg_tilemap, 0, (*yard_scroll_x_high * 0x100) + *yard_scroll_x_low);
	tilemap_set_scrolly(bg_tilemap, 0, *yard_scroll_y_low);

	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
	draw_sprites(machine, bitmap, cliprect);
	draw_panel(machine, bitmap, cliprect);
	return 0;
}
