#include "driver.h"

static tilemap *bg_tilemap, *fg_tilemap;

UINT8* mermaid_videoram2;
UINT8* mermaid_bg_scrollram;
UINT8* mermaid_fg_scrollram;

static bitmap_t* helper;
static bitmap_t* helper2;
static int collision_sprite_background,collision_sprite_foreground,collision_sprite_sprite_boat,collision_sprite_sprite_wind;

static int rougien_gfxbank1, rougien_gfxbank2;

static const rectangle spritevisiblearea =
{
	0*8, 26*8-1,
	2*8, 30*8-1
};

static const rectangle flip_spritevisiblearea =
{
	6*8, 31*8-1,
	2*8, 30*8-1
};

PALETTE_INIT( mermaid )
{
	int i;

	/* allocate the colortable */
	machine->colortable = colortable_alloc(machine, 0x41);

	for (i = 0; i < 0x40; i++)
	{
		int r = 0x21 * BIT(color_prom[i], 0) + 0x47 * BIT(color_prom[i], 1) + 0x97 * BIT(color_prom[i], 2);
		int g = 0x21 * BIT(color_prom[i], 3) + 0x47 * BIT(color_prom[i], 4) + 0x97 * BIT(color_prom[i], 5);
		int b =                                0x47 * BIT(color_prom[i], 6) + 0x97 * BIT(color_prom[i], 7);

		colortable_palette_set_color(machine->colortable, i, MAKE_RGB(r, g, b));
	}

	/* blue background */
	colortable_palette_set_color(machine->colortable, 0x40, MAKE_RGB(0, 0, 0xff));

	/* char/sprite palette */
	for (i = 0; i < 0x40; i++)
		colortable_entry_set_value(machine->colortable, i, i);

	/* background palette */
	colortable_entry_set_value(machine->colortable, 0x40, 0x20);
	colortable_entry_set_value(machine->colortable, 0x41, 0x21);
	colortable_entry_set_value(machine->colortable, 0x42, 0x40);
	colortable_entry_set_value(machine->colortable, 0x43, 0x21);
}

WRITE8_HANDLER( mermaid_videoram2_w )
{
	mermaid_videoram2[offset] = data;
	tilemap_mark_tile_dirty(bg_tilemap, offset);
}

WRITE8_HANDLER( mermaid_videoram_w )
{
	videoram[offset] = data;
	tilemap_mark_tile_dirty(fg_tilemap, offset);
}

WRITE8_HANDLER( mermaid_colorram_w )
{
	colorram[offset] = data;
	tilemap_mark_tile_dirty(fg_tilemap, offset);
}

WRITE8_HANDLER( mermaid_flip_screen_x_w )
{
	flip_screen_x_set(data & 0x01);
}

WRITE8_HANDLER( mermaid_flip_screen_y_w )
{
	flip_screen_y_set(data & 0x01);
}

WRITE8_HANDLER( mermaid_bg_scroll_w )
{
	mermaid_bg_scrollram[offset] = data;
	tilemap_set_scrolly(bg_tilemap, offset, data);
}

WRITE8_HANDLER( mermaid_fg_scroll_w )
{
	mermaid_fg_scrollram[offset] = data;
	tilemap_set_scrolly(fg_tilemap, offset, data);
}

WRITE8_HANDLER( rougien_gfxbankswitch1_w )
{
	rougien_gfxbank1 = data & 0x01;
}

WRITE8_HANDLER( rougien_gfxbankswitch2_w )
{
	rougien_gfxbank2 = data & 0x01;
}

READ8_HANDLER( mermaid_collision_r )
{
	/*
        collision register active LO
        Bit 0
        Bit 1 - Sprite - Foreground
        Bit 2 - Sprite - Stream
        Bit 3 - Sprite (Wind) - Sprite
        Bit 4
        Bit 5
        Bit 6 - Sprite (Boat) - Sprite
        Bit 7
    */

	int collision = 0xff;

	if(collision_sprite_foreground) collision  &= 0xfd;
	if(collision_sprite_background) collision  &= 0xfb;

	if(collision_sprite_sprite_wind) collision &= 0xf7;
	if(collision_sprite_sprite_boat) collision &= 0xbf;

	return collision;
}

static TILE_GET_INFO( get_bg_tile_info )
{
	int code = mermaid_videoram2[tile_index];
	int sx = tile_index % 32;
	int color = (sx >= 26) ? 0 : 1;

	SET_TILE_INFO(2, code, color, 0);
}

static TILE_GET_INFO( get_fg_tile_info )
{
	int attr = colorram[tile_index];
	int code = videoram[tile_index] + ((attr & 0x30) << 4);
	int color = attr & 0x0f;
	int flags = TILE_FLIPYX((attr & 0xc0) >> 6);

	code |= rougien_gfxbank1 * 0x2800;
	code |= rougien_gfxbank2 * 0x2400;

	SET_TILE_INFO(0, code, color, flags);
}

VIDEO_START( mermaid )
{
	bg_tilemap = tilemap_create(get_bg_tile_info, tilemap_scan_rows, 8, 8, 32, 32);
	tilemap_set_scroll_cols(bg_tilemap, 32);

	fg_tilemap = tilemap_create(get_fg_tile_info, tilemap_scan_rows, 8, 8, 32, 32);
	tilemap_set_scroll_cols(fg_tilemap, 32);
	tilemap_set_transparent_pen(fg_tilemap, 0);

	helper = video_screen_auto_bitmap_alloc(machine->primary_screen);
	helper2 = video_screen_auto_bitmap_alloc(machine->primary_screen);
}

static void draw_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect)
{
	int offs;

	for (offs = spriteram_size - 4; offs >= 0; offs -= 4)
	{
		int attr = spriteram[offs + 2];
		int bank = (attr & 0x30) >> 4;
		int code = (spriteram[offs] & 0x3f) | (bank << 6);
		int color = attr & 0x0f;
		int flipx = spriteram[offs] & 0x40;
		int flipy = spriteram[offs] & 0x80;
		int sx = spriteram[offs + 3] + 1;
		int sy = 240 - spriteram[offs + 1];

		code |= rougien_gfxbank1 * 0x2800;
		code |= rougien_gfxbank2 * 0x2400;

		if (flip_screen_x_get())
		{
			flipx = !flipx;
			sx = 240 - sx;
		}

		if (flip_screen_y_get())
		{
			flipy = !flipy;
			sy = 240 - sy;
		}

		drawgfx(bitmap, machine->gfx[1], code, color, flipx, flipy, sx, sy,
			(flip_screen_x_get() ? &flip_spritevisiblearea : &spritevisiblearea),
			TRANSPARENCY_PEN, 0);
	}
}

VIDEO_UPDATE( mermaid )
{
	tilemap_draw(bitmap, cliprect, bg_tilemap, 0, 0);
	tilemap_draw(bitmap, cliprect, fg_tilemap, 0, 0);
	draw_sprites(screen->machine, bitmap, cliprect);
	return 0;
}

static UINT8 collision_check(colortable_t *colortable, rectangle* rect)
{
	UINT8 data = 0;

	int x;
	int y;

	for (y = rect->min_y; y <= rect->max_y; y++)
		for (x = rect->min_x; x <= rect->max_x; x++)
		{
			UINT16 a = colortable_entry_get_value(colortable, *BITMAP_ADDR16(helper, y, x));
			UINT16 b = colortable_entry_get_value(colortable, *BITMAP_ADDR16(helper2, y, x));
			if (b != 0)
				if (a != 0)
					data |= 0x01;
		}

	return data;
}

VIDEO_EOF( mermaid )
{
	const rectangle *visarea = video_screen_get_visible_area(machine->primary_screen);

	int offs,offs2;

	collision_sprite_foreground = 0;
	collision_sprite_sprite_boat = 0;
	collision_sprite_sprite_wind = 0;

	// check for "boat" sprite

	for (offs = spriteram_size - 4; offs >= spriteram_size - (4*2); offs -= 4)
	{
		int attr = spriteram[offs + 2];
		int bank = (attr & 0x30) >> 4;
		int code = (spriteram[offs] & 0x3f) | (bank << 6);
//      int color = attr & 0x0f;
		int flipx = spriteram[offs] & 0x40;
		int flipy = spriteram[offs] & 0x80;
		int sx = spriteram[offs + 3] + 1;
		int sy = 240 - spriteram[offs + 1];

		rectangle rect;

		code |= rougien_gfxbank1 * 0x2800;
		code |= rougien_gfxbank2 * 0x2400;

		if (flip_screen_x_get())
		{
			flipx = !flipx;
			sx = 240 - sx;
		}

		if (flip_screen_y_get())
		{
			flipy = !flipy;
			sy = 240 - sy;
		}

		rect.min_x = sx;
		rect.min_y = sy;
		rect.max_x = sx + machine->gfx[1]->width - 1;
		rect.max_y = sy + machine->gfx[1]->height - 1;

		if (rect.min_x < visarea->min_x)
			rect.min_x = visarea->min_x;
		if (rect.min_y < visarea->min_y)
			rect.min_y = visarea->min_y;
		if (rect.max_x > visarea->max_x)
			rect.max_x = visarea->max_x;
		if (rect.max_y > visarea->max_y)
			rect.max_y = visarea->max_y;

		// check collision sprite-background (stream)

		fillbitmap(helper,0,&rect);
		fillbitmap(helper2,0,&rect);

		tilemap_draw(helper, &rect, bg_tilemap, 0, 0);

		drawgfx(helper2, machine->gfx[1], code, 0, flipx, flipy, sx, sy,
			&rect,TRANSPARENCY_PEN, 0);

		collision_sprite_background |= collision_check(machine->colortable, &rect);

		// check collision sprite-foreground

		fillbitmap(helper,0,&rect);
		fillbitmap(helper2,0,&rect);

		tilemap_draw(helper, &rect, fg_tilemap, 0, 0);

		drawgfx(helper2, machine->gfx[1], code, 0, flipx, flipy, sx, sy,
			&rect,TRANSPARENCY_PEN, 0);

		collision_sprite_foreground |= collision_check(machine->colortable, &rect);

		// check collision sprite "boat" - sprite

		fillbitmap(helper,0,&rect);
		fillbitmap(helper2,0,&rect);

		for (offs2 = spriteram_size - (4*6) - 4; offs2 >= 0; offs2 -= 4)
			if (offs != offs2)
			{
				int attr2 = spriteram[offs2 + 2];
				int bank2 = (attr2 & 0x30) >> 4;
				int code2 = (spriteram[offs2] & 0x3f) | (bank2 << 6);
//              int color2 = attr2 & 0x0f;
				int flipx2 = spriteram[offs2] & 0x40;
				int flipy2 = spriteram[offs2] & 0x80;
				int sx2 = spriteram[offs2 + 3] + 1;
				int sy2 = 240 - spriteram[offs2 + 1];

				code2 |= rougien_gfxbank1 * 0x2800;
				code2 |= rougien_gfxbank2 * 0x2400;

				if (flip_screen_x_get())
				{
					flipx2 = !flipx2;
					sx2 = 240 - sx2;
				}

				if (flip_screen_y_get())
				{
					flipy2 = !flipy2;
					sy2 = 240 - sy2;
				}

				drawgfx(helper, machine->gfx[1], code2, 0, flipx2, flipy2, sx2, sy2,
					&rect,TRANSPARENCY_PEN, 0);
			}

			drawgfx(helper2, machine->gfx[1], code, 0, flipx, flipy, sx, sy,
				&rect,TRANSPARENCY_PEN, 0);

			collision_sprite_sprite_boat |= collision_check(machine->colortable, &rect);
		}

	// check for "sail" sprite

	for (offs = spriteram_size - 4*3; offs >= spriteram_size - (4*4); offs -= 4)
	{
		int attr = spriteram[offs + 2];
		int bank = (attr & 0x30) >> 4;
		int code = (spriteram[offs] & 0x3f) | (bank << 6);
//      int color = attr & 0x0f;
		int flipx = spriteram[offs] & 0x40;
		int flipy = spriteram[offs] & 0x80;
		int sx = spriteram[offs + 3] + 1;
		int sy = 240 - spriteram[offs + 1];

		rectangle rect;

		code |= rougien_gfxbank1 * 0x2800;
		code |= rougien_gfxbank2 * 0x2400;

		if (flip_screen_x_get())
		{
			flipx = !flipx;
			sx = 240 - sx;
		}

		if (flip_screen_y_get())
		{
			flipy = !flipy;
			sy = 240 - sy;
		}

		rect.min_x = sx;
		rect.min_y = sy;
		rect.max_x = sx + machine->gfx[1]->width - 1;
		rect.max_y = sy + machine->gfx[1]->height - 1;

		if (rect.min_x < visarea->min_x)
			rect.min_x = visarea->min_x;
		if (rect.min_y < visarea->min_y)
			rect.min_y = visarea->min_y;
		if (rect.max_x > visarea->max_x)
			rect.max_x = visarea->max_x;
		if (rect.max_y > visarea->max_y)
			rect.max_y = visarea->max_y;

		// check collision sprite "sail" - sprite

		fillbitmap(helper,0,&rect);
		fillbitmap(helper2,0,&rect);

		for (offs2 = spriteram_size - (4*6) - 4; offs2 >= 0; offs2 -= 4)
			if (offs != offs2)
			{
				int attr2 = spriteram[offs2 + 2];
				int bank2 = (attr2 & 0x30) >> 4;
				int code2 = (spriteram[offs2] & 0x3f) | (bank2 << 6);
//              int color2 = attr2 & 0x0f;
				int flipx2 = spriteram[offs2] & 0x40;
				int flipy2 = spriteram[offs2] & 0x80;
				int sx2 = spriteram[offs2 + 3] + 1;
				int sy2 = 240 - spriteram[offs2 + 1];

				code2 |= rougien_gfxbank1 * 0x2800;
				code2 |= rougien_gfxbank2 * 0x2400;

				if (flip_screen_x_get())
				{
					flipx2 = !flipx2;
					sx2 = 240 - sx2;
				}

				if (flip_screen_y_get())
				{
					flipy2 = !flipy2;
					sy2 = 240 - sy2;
				}

				drawgfx(helper, machine->gfx[1], code2, 1, flipx2, flipy2, sx2, sy2,
					&rect,TRANSPARENCY_PEN, 0);
			}

			drawgfx(helper2, machine->gfx[1], code, 0, flipx, flipy, sx, sy,
				&rect,TRANSPARENCY_PEN, 0);

			collision_sprite_sprite_wind |= collision_check(machine->colortable, &rect);
		}
}
