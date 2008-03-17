#include "driver.h"
#include "video/resnet.h"

UINT8 *champbas_bg_videoram;

static UINT8 gfx_bank;
static UINT8 palette_bank;

static tilemap *bg_tilemap;


/***************************************************************************

  Convert the color PROMs into a more useable format.

  The palette PROM is connected to the RGB output this way:

  bit 7 -- 220 ohm resistor  -- BLUE
        -- 470 ohm resistor  -- BLUE
        -- 220 ohm resistor  -- GREEN
        -- 470 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 220 ohm resistor  -- RED
        -- 470 ohm resistor  -- RED
  bit 0 -- 1  kohm resistor  -- RED

***************************************************************************/

PALETTE_INIT( champbas )
{
	static const int resistances_rg[3] = { 1000, 470, 220 };
	static const int resistances_b [2] = { 470, 220 };
	double rweights[3], gweights[3], bweights[2];
	int i;

	/* compute the color output resistor weights */
	compute_resistor_weights(0,	255, -1.0,
			3, &resistances_rg[0], rweights, 0, 0,
			3, &resistances_rg[0], gweights, 0, 0,
			2, &resistances_b[0],  bweights, 0, 0);

	/* allocate the colortable */
	machine->colortable = colortable_alloc(machine, 0x20);

	/* create a lookup table for the palette */
	for (i = 0; i < 0x20; i++)
	{
		int bit0, bit1, bit2;
		int r, g, b;

		/* red component */
		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		r = combine_3_weights(rweights, bit0, bit1, bit2);

		/* green component */
		bit0 = (color_prom[i] >> 3) & 0x01;
		bit1 = (color_prom[i] >> 4) & 0x01;
		bit2 = (color_prom[i] >> 5) & 0x01;
		g = combine_3_weights(gweights, bit0, bit1, bit2);

		/* blue component */
		bit0 = (color_prom[i] >> 6) & 0x01;
		bit1 = (color_prom[i] >> 7) & 0x01;
		b = combine_2_weights(bweights, bit0, bit1);

		colortable_palette_set_color(machine->colortable, i, MAKE_RGB(r, g, b));
	}

	color_prom += 0x20;

	for (i = 0; i < 0x200; i++)
	{
		UINT8 ctabentry = ((i & 0x100) >> 4) | (color_prom[i & 0xff] & 0x0f);
		colortable_entry_set_value(machine->colortable, i, ctabentry);
	}
}




static TILE_GET_INFO( get_bg_tile_info )
{
	int code = champbas_bg_videoram[tile_index] | (gfx_bank << 8);
	int color = (champbas_bg_videoram[tile_index + 0x400] & 0x1f) | 0x20 | (palette_bank << 6);

	SET_TILE_INFO(0, code, color, 0);
}



VIDEO_START( champbas )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows, 8, 8, 32, 32);

	// talbot has only 1 bank
	gfx_bank = 0;
	palette_bank = 0;
}




WRITE8_HANDLER( champbas_bg_videoram_w )
{
	champbas_bg_videoram[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap, offset & 0x3ff);
}

WRITE8_HANDLER( champbas_gfxbank_w )
{
	data &= 1;
	if (gfx_bank != data)
	{
		gfx_bank = data;
		tilemap_mark_all_tiles_dirty(bg_tilemap);
	}
}

WRITE8_HANDLER( champbas_palette_bank_w )
{
	data &= 1;
	if (palette_bank != data)
	{
		palette_bank = data;
		tilemap_mark_all_tiles_dirty(bg_tilemap);
	}
}

WRITE8_HANDLER( champbas_flipscreen_w )
{
	flip_screen_set(~data & 1);
}



static void draw_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect)
{
	int offs;
	const gfx_element* const gfx = machine->gfx[1];

	for (offs = spriteram_size - 2; offs >= 0; offs -= 2)
	{
		int code = (spriteram[offs] >> 2) | (gfx_bank << 6);
		int color = (spriteram[offs + 1] & 0x1f) | (palette_bank << 6);
		int flipx = ~spriteram[offs] & 0x01;
		int flipy = ~spriteram[offs] & 0x02;
		int sx = spriteram_2[offs + 1] - 16;
		int sy = 255 - spriteram_2[offs];

		drawgfx(bitmap, gfx,
				code, color,
				flipx, flipy,
				sx, sy,
				cliprect,
				TRANSPARENCY_PENS,
				colortable_get_transpen_mask(machine->colortable, gfx, color, 0));

		// wraparound
		drawgfx(bitmap, gfx,
				code, color,
				flipx, flipy,
				sx + 256, sy,
				cliprect,
				TRANSPARENCY_PENS,
				colortable_get_transpen_mask(machine->colortable, gfx, color, 0));
	}
}



VIDEO_UPDATE( champbas )
{
	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
	draw_sprites(screen->machine, bitmap, cliprect);
	return 0;
}
