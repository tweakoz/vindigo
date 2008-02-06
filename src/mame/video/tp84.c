/***************************************************************************

    video.c

    Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"
#include "video/resnet.h"
#include "deprecat.h"

UINT8 *tp84_videoram2, *tp84_colorram2;

static int col0;

/*
sprites are multiplexed, so we have to buffer the spriteram
scanline by scanline.
*/
static UINT8 *sprite_mux_buffer;
static int scanline;

static tilemap *bg_tilemap, *fg_tilemap;

/*
-The colortable is divided in 2 part:
 -The characters colors
 -The sprites colors

-The characters colors are indexed like this:
 -2 bits from the characters
 -4 bits from the attribute in colorram
 -2 bits from col0 (d3-d4)
 -3 bits from col0 (d0-d1-d2)
-So, there is 2048 bytes for the characters

-The sprites colors are indexed like this:
 -4 bits from the sprites (16 colors)
 -4 bits from the attribute of the sprites
 -3 bits from col0 (d0-d1-d2)
-So, there is 2048 bytes for the sprites

*/
/*
     The RGB signals are generated by 3 proms 256X4 (prom 2C, 2D and 1E)
        The resistors values are:
            1K  ohm
            470 ohm
            220 ohm
            100 ohm
*/
PALETTE_INIT( tp84 )
{
	static const int resistances[4] = { 1000, 470, 220, 100 };
	double rweights[4], gweights[4], bweights[4];
	int i;

	/* compute the color output resistor weights */
	compute_resistor_weights(0,	255, -1.0,
			4, resistances, rweights, 470, 0,
			4, resistances, gweights, 470, 0,
			4, resistances, bweights, 470, 0);

	/* allocate the colortable */
	machine->colortable = colortable_alloc(machine, 0x100);

	/* create a lookup table for the palette */
	for (i = 0; i < 0x100; i++)
	{
		int bit0, bit1, bit2, bit3;
		int r, g, b;

		/* red component */
		bit0 = (color_prom[i + 0x000] >> 0) & 0x01;
		bit1 = (color_prom[i + 0x000] >> 1) & 0x01;
		bit2 = (color_prom[i + 0x000] >> 2) & 0x01;
		bit3 = (color_prom[i + 0x000] >> 3) & 0x01;
		r = combine_4_weights(rweights, bit0, bit1, bit2, bit3);

		/* green component */
		bit0 = (color_prom[i + 0x100] >> 0) & 0x01;
		bit1 = (color_prom[i + 0x100] >> 1) & 0x01;
		bit2 = (color_prom[i + 0x100] >> 2) & 0x01;
		bit3 = (color_prom[i + 0x100] >> 3) & 0x01;
		g = combine_4_weights(gweights, bit0, bit1, bit2, bit3);

		/* blue component */
		bit0 = (color_prom[i + 0x200] >> 0) & 0x01;
		bit1 = (color_prom[i + 0x200] >> 1) & 0x01;
		bit2 = (color_prom[i + 0x200] >> 2) & 0x01;
		bit3 = (color_prom[i + 0x200] >> 3) & 0x01;
		b = combine_4_weights(bweights, bit0, bit1, bit2, bit3);

		colortable_palette_set_color(machine->colortable, i, MAKE_RGB(r, g, b));
	}

	/* color_prom now points to the beginning of the lookup table */
	color_prom += 0x300;

	/* characters use colors 0x80-0xff, sprites use colors 0-0x7f */
	for (i = 0; i < 0x200; i++)
	{
		int j;

		for (j = 0; j < 8; j++)
		{
			UINT8 ctabentry = ((~i & 0x100) >> 1) | (j << 4) | (color_prom[i] & 0x0f);
			colortable_entry_set_value(machine->colortable, ((i & 0x100) << 3) | (j << 8) | (i & 0xff), ctabentry);
		}
	}
}


WRITE8_HANDLER( tp84_videoram_w )
{
	videoram[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap, offset);
}

WRITE8_HANDLER( tp84_colorram_w )
{
	colorram[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap, offset);
}

WRITE8_HANDLER( tp84_videoram2_w )
{
	tp84_videoram2[offset] = data;
	tilemap_mark_tile_dirty(fg_tilemap, offset);
}

WRITE8_HANDLER( tp84_colorram2_w )
{
	tp84_colorram2[offset] = data;
	tilemap_mark_tile_dirty(fg_tilemap, offset);
}

WRITE8_HANDLER( tp84_scroll_x_w )
{
	tilemap_set_scrollx(bg_tilemap, 0, data);
}

WRITE8_HANDLER( tp84_scroll_y_w )
{
	tilemap_set_scrolly(bg_tilemap, 0, data);
}

WRITE8_HANDLER( tp84_flipscreen_x_w )
{
	flip_screen_x_set(data & 0x01);
}

WRITE8_HANDLER( tp84_flipscreen_y_w )
{
	flip_screen_y_set(data & 0x01);
}

/*****
  col0 is a register to index the color Proms
*****/
WRITE8_HANDLER( tp84_col0_w )
{
	if (col0 != data)
	{
		col0 = data;
		tilemap_mark_all_tiles_dirty(ALL_TILEMAPS);
	}
}

/* Return the current video scan line */
READ8_HANDLER( tp84_scanline_r )
{
	return scanline;
}

static TILE_GET_INFO( get_bg_tile_info )
{
	int coloffs = ((col0 & 0x18) << 1) + ((col0 & 0x07) << 6);
	int attr = colorram[tile_index];
	int code = videoram[tile_index] + ((attr & 0x30) << 4);
	int color = (attr & 0x0f) + coloffs;
	int flags = ((attr & 0x40) ? TILE_FLIPX : 0) | ((attr & 0x80) ? TILE_FLIPY : 0);

	SET_TILE_INFO(0, code, color, flags);
}

static TILE_GET_INFO( get_fg_tile_info )
{
	int coloffs = ((col0 & 0x18) << 1) + ((col0 & 0x07) << 6);
	int attr = tp84_colorram2[tile_index];
	int code = tp84_videoram2[tile_index]+ ((attr & 0x30) << 4);
	int color = (attr & 0x0f) + coloffs;
	int flags = ((attr & 0x40) ? TILE_FLIPX : 0) | ((attr & 0x80) ? TILE_FLIPY : 0) | TILE_FORCE_LAYER0;

	SET_TILE_INFO(0, code, color, flags);
}

VIDEO_START( tp84 )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows,
		 8, 8, 32, 32);

	fg_tilemap = tilemap_create(get_fg_tile_info, tilemap_scan_rows,
		 8, 8, 32, 32);

	tilemap_set_transparent_pen(fg_tilemap, 0);

	sprite_mux_buffer = auto_malloc(256 * spriteram_size);
}

static void draw_sprites(running_machine *machine, mame_bitmap *bitmap, const rectangle *cliprect)
{
	const gfx_element *gfx = machine->gfx[1];
	int offs;
	int line;
	int coloffset = ((col0&0x07) << 4);

	for (line = cliprect->min_y; line <= cliprect->max_y; line++)
	{
		UINT8 *sr;
		rectangle clip = *cliprect;

		sr = sprite_mux_buffer + line * spriteram_size;
		clip.min_y = clip.max_y = line;

		for (offs = spriteram_size - 4;offs >= 0;offs -= 4)
		{
			int code,color,sx,sy,flipx,flipy;

			sx = sr[offs];
			sy = 240 - sr[offs + 3];

			if (sy > line-16 && sy <= line)
			{
				code = sr[offs + 1];
				color = (sr[offs + 2] & 0x0f) + coloffset;
				flipx = ~sr[offs + 2] & 0x40;
				flipy = sr[offs + 2] & 0x80;

				drawgfx(bitmap,gfx,
						code,
						color,
						flipx,flipy,
						sx,sy,
						&clip,TRANSPARENCY_PENS,
						colortable_get_transpen_mask(machine->colortable, gfx, color, coloffset));
			}
		}
	}
}

VIDEO_UPDATE( tp84 )
{
	rectangle clip;

	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
	draw_sprites(machine, bitmap, cliprect);

	/* draw top fg_tilemap status layer part */
	clip.min_x = machine->screen[0].visarea.min_x;
	clip.max_x = machine->screen[0].visarea.min_x+15;
	clip.min_y = machine->screen[0].visarea.min_y;
	clip.max_y = machine->screen[0].visarea.max_y;
	tilemap_draw(bitmap, &clip, fg_tilemap, 0, 0);

	/* the middle part of fg_tilemap seems to be used as normal ram and is skipped */

	/* draw bottom fg_tilemap status layer part */
	clip.min_x = machine->screen[0].visarea.max_x-15;
	clip.max_x = machine->screen[0].visarea.max_x;
	clip.min_y = machine->screen[0].visarea.min_y;
	clip.max_y = machine->screen[0].visarea.max_y;
	tilemap_draw(bitmap, &clip, fg_tilemap, 0, 0);
	return 0;
}

INTERRUPT_GEN( tp84_6809_interrupt )
{
	scanline = 255 - cpu_getiloops();

	memcpy(sprite_mux_buffer + scanline * spriteram_size,spriteram,spriteram_size);

	if (scanline == 255)
		irq0_line_hold(machine, cpunum);
}
