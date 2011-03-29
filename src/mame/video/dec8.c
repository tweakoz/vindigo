/***************************************************************************

Cobra Command:
  2 BAC06 background generator chips, same as Dec0.
  1 MXC06 chip for sprites, same as Dec0.
  256 colours, palette generated by ram.

The Real Ghostbusters:
1 Deco VSC30 (M60348) (on DE-0259-1 sub board)
1 Deco HMC20 (M60232) (on DE-0259-1 sub board)
1 x BAC06 (on DE-0273-1 board)

  1 playfield, same as above, with rowscroll
  1024 colours from 2 proms.
  Sprite hardware close to above, there are some unused (unknown) bits per sprite.

Super Real Darwin:
  1 playfield, x-scroll only
  Closer to earlier Darwin 4078 board than above games.

Last Mission/Shackled:
    Has 1 Deco VSC30 (M60348) (From readme file)
    Has 1 Deco HMC20 (M60232) (From readme file)

    1 playfield
    Sprite hardware same as Karnov.
    (Shackled) Palettes 8-15 for tiles seem to have priority over sprites.

Gondomania:
    Has two large square surface mount chips: [ DRL 40, 8053, 8649a ]
    Has 1 Deco VSC30 (M60348)
    Has 1 Deco HMC20 (M60232)
    Priority - all tiles with *pens* 8-15 appear over sprites with palettes 8-15.

Oscar:
    Uses MXC-06 custom chip for sprites.
    Uses BAC-06 custom chip for background.
    I can't find what makes the fix chars...
    Priority - tiles with palettes 8-15 have their *pens* 8-15 appearing over
sprites.

***************************************************************************/

#include "emu.h"
#include "includes/dec8.h"
#include "video/decbac06.h"
#include "video/decmxc06.h"

/***************************************************************************

  Convert the color PROMs into a more useable format.

  Real Ghostbusters has two 1024x8 palette PROM.
  I don't know the exact values of the resistors between the RAM and the
  RGB output. I assumed these values (the same as Commando)

  bit 7 -- 220 ohm resistor  -- GREEN
        -- 470 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 2.2kohm resistor  -- GREEN
        -- 220 ohm resistor  -- RED
        -- 470 ohm resistor  -- RED
        -- 1  kohm resistor  -- RED
  bit 0 -- 2.2kohm resistor  -- RED

  bit 7 -- unused
        -- unused
        -- unused
        -- unused
        -- 220 ohm resistor  -- BLUE
        -- 470 ohm resistor  -- BLUE
        -- 1  kohm resistor  -- BLUE
  bit 0 -- 2.2kohm resistor  -- BLUE

***************************************************************************/

PALETTE_INIT( ghostb )
{
	int i;

	for (i = 0; i < machine.total_colors(); i++)
	{
		int bit0, bit1, bit2, bit3, r, g, b;

		bit0 = (color_prom[i] >> 0) & 0x01;
		bit1 = (color_prom[i] >> 1) & 0x01;
		bit2 = (color_prom[i] >> 2) & 0x01;
		bit3 = (color_prom[i] >> 3) & 0x01;
		r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		bit0 = (color_prom[i] >> 4) & 0x01;
		bit1 = (color_prom[i] >> 5) & 0x01;
		bit2 = (color_prom[i] >> 6) & 0x01;
		bit3 = (color_prom[i] >> 7) & 0x01;
		g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		bit0 = (color_prom[i + machine.total_colors()] >> 0) & 0x01;
		bit1 = (color_prom[i + machine.total_colors()] >> 1) & 0x01;
		bit2 = (color_prom[i + machine.total_colors()] >> 2) & 0x01;
		bit3 = (color_prom[i + machine.total_colors()] >> 3) & 0x01;
		b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		palette_set_color(machine, i, MAKE_RGB(r, g, b));
	}
}

WRITE8_HANDLER( dec8_bg_data_w )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();
	state->bg_data[offset] = data;
	tilemap_mark_tile_dirty(state->bg_tilemap, offset / 2);
}

READ8_HANDLER( dec8_bg_data_r )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();
	return state->bg_data[offset];
}


WRITE8_HANDLER( dec8_videoram_w )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();
	state->videoram[offset] = data;
	tilemap_mark_tile_dirty(state->fix_tilemap, offset / 2);
}

WRITE8_HANDLER( srdarwin_videoram_w )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();
	state->videoram[offset] = data;
	tilemap_mark_tile_dirty(state->fix_tilemap, offset);
}


WRITE8_HANDLER( dec8_scroll2_w )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();
	state->scroll2[offset] = data;
}

WRITE8_HANDLER( srdarwin_control_w )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();

	switch (offset)
	{
	case 0: /* Top 3 bits - bank switch, bottom 4 - scroll MSB */
		memory_set_bank(space->machine(), "bank1", (data >> 5));
		state->scroll2[0] = data & 0xf;
		return;

	case 1:
		state->scroll2[1] = data;
		return;
    }
}

WRITE8_HANDLER( lastmisn_control_w )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();

	/*
        Bit 0x0f - ROM bank switch.
        Bit 0x10 - Unused
        Bit 0x20 - X scroll MSB
        Bit 0x40 - Y scroll MSB
        Bit 0x80 - Hold subcpu reset line high if clear, else low
    */
	memory_set_bank(space->machine(), "bank1", data & 0x0f);

	state->scroll2[0] = (data >> 5) & 1;
	state->scroll2[2] = (data >> 6) & 1;

	if (data & 0x80)
		device_set_input_line(state->subcpu, INPUT_LINE_RESET, CLEAR_LINE);
	else
		device_set_input_line(state->subcpu, INPUT_LINE_RESET, ASSERT_LINE);
}

WRITE8_HANDLER( shackled_control_w )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();

	/* Bottom 4 bits - bank switch, Bits 4 & 5 - Scroll MSBs */
	memory_set_bank(space->machine(), "bank1", data & 0x0f);

	state->scroll2[0] = (data >> 5) & 1;
	state->scroll2[2] = (data >> 6) & 1;
}

WRITE8_HANDLER( lastmisn_scrollx_w )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();
	state->scroll2[1] = data;
}

WRITE8_HANDLER( lastmisn_scrolly_w )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();
	state->scroll2[3] = data;
}

WRITE8_HANDLER( gondo_scroll_w )
{
	dec8_state *state = space->machine().driver_data<dec8_state>();
	switch (offset)
	{
	case 0x0:
		state->scroll2[1] = data; /* X LSB */
		break;
	case 0x8:
		state->scroll2[3] = data; /* Y LSB */
		break;
	case 0x10:
		state->scroll2[0] = (data >> 0) & 1; /* Bit 0: X MSB */
		state->scroll2[2] = (data >> 1) & 1; /* Bit 1: Y MSB */
		/* Bit 2 is also used in Gondo & Garyoret */
		break;
	}
}

/******************************************************************************/

/* 'Karnov' sprites, used by Gondomania, Last Mission, Shackled, Ghostbusters */
static void draw_sprites1( running_machine& machine, bitmap_t *bitmap, const rectangle *cliprect, int priority )
{
	UINT8 *buffered_spriteram = machine.generic.buffered_spriteram.u8;
	int offs, x, y, sprite, sprite2, colour, extra, fx, fy;

	for (offs = 0; offs < 0x800; offs += 8)
	{
		y = buffered_spriteram[offs + 1] + (buffered_spriteram[offs] << 8);
		if ((y & 0x8000) == 0) continue;

		fx = buffered_spriteram[offs + 3];

		if ((fx & 0x1) == 0) continue;

		extra = fx & 0x10;
		fy = fx & 0x2;
		fx = fx & 0x4;

		x = buffered_spriteram[offs + 5] + (buffered_spriteram[offs + 4] << 8);
		colour = buffered_spriteram[offs + 6] >> 4;
		if (priority == 1 && (colour & 8)) continue;
		if (priority == 2 && !(colour & 8)) continue;
		sprite = buffered_spriteram[offs + 7] + (buffered_spriteram[offs + 6] << 8);
		sprite &= 0x0fff;

		if (extra) {y = y + 16; sprite &= 0xffe;}

		x = x & 0x01ff;
		y = y & 0x01ff;
		x = (x + 16) % 0x200;
		y = (y + 16) % 0x200;
		x = 256 - x;
		y = 256 - y;
		if (flip_screen_get(machine))
		{
			y = 240 - y;
			x = 240 - x;
			if (fx) fx = 0; else fx = 1;
			if (fy) fy = 0; else fy = 1;
			if (extra) y = y - 16;
		}

		/* Y Flip determines order of multi-sprite */
		if (extra && fy)
		{
			sprite2 = sprite;
			sprite++;
		}
		else
			sprite2 = sprite + 1;

		drawgfx_transpen(bitmap,cliprect,machine.gfx[1],
				sprite,
				colour,fx,fy,x,y,0);

    	/* 1 more sprite drawn underneath */
    	if (extra)
    		drawgfx_transpen(bitmap,cliprect,machine.gfx[1],
				sprite2,
				colour,fx,fy,x,y+16,0);
	}
}


static void srdarwin_draw_sprites( running_machine& machine, bitmap_t *bitmap, const rectangle *cliprect, int pri )
{
	UINT8 *buffered_spriteram = machine.generic.buffered_spriteram.u8;
	int offs;

	/* Sprites */
	for (offs = 0; offs < 0x200; offs += 4)
	{
		int multi, fx, sx, sy, sy2, code, color;

		color = (buffered_spriteram[offs + 1] & 0x03) + ((buffered_spriteram[offs + 1] & 0x08) >> 1);
		if (pri == 0 && color != 0) continue;
		if (pri == 1 && color == 0) continue;

		code = buffered_spriteram[offs + 3] + ((buffered_spriteram[offs + 1] & 0xe0) << 3);
		if (!code) continue;

		sy = buffered_spriteram[offs];
		if (sy == 0xf8) continue;

		sx = (241 - buffered_spriteram[offs + 2]);

		fx = buffered_spriteram[offs + 1] & 0x04;
		multi = buffered_spriteram[offs + 1] & 0x10;

		if (flip_screen_get(machine))
		{
			sy = 240 - sy;
			sx = 240 - sx;
			if (fx) fx = 0; else fx = 1;
			sy2 = sy - 16;
		}
		else sy2 = sy + 16;

		drawgfx_transpen(bitmap,cliprect,machine.gfx[1],
        			code,
				color,
				fx,flip_screen_get(machine),
				sx,sy,0);
        if (multi)
    		drawgfx_transpen(bitmap,cliprect,machine.gfx[1],
				code+1,
				color,
				fx,flip_screen_get(machine),
				sx,sy2,0);
	}
}

/******************************************************************************/

SCREEN_UPDATE( cobracom )
{
	dec8_state *state = screen->machine().driver_data<dec8_state>();

	flip_screen_set(screen->machine(), state->bg_control[0] >> 7);

	screen->machine().device<deco_bac06_device>("tilegen1")->deco_bac06_pf_draw(screen->machine(),bitmap,cliprect,TILEMAP_DRAW_OPAQUE, 0x00, 0x00, 0x00, 0x00);
	screen->machine().device<deco_mxc06_device>("spritegen")->draw_sprites(screen->machine(), bitmap, cliprect, state->buffered_spriteram16, 0x04, 0x00, 0x03);
	screen->machine().device<deco_bac06_device>("tilegen2")->deco_bac06_pf_draw(screen->machine(),bitmap,cliprect,0, 0x00, 0x00, 0x00, 0x00);
	screen->machine().device<deco_mxc06_device>("spritegen")->draw_sprites(screen->machine(), bitmap, cliprect, state->buffered_spriteram16, 0x04, 0x04, 0x03);
	tilemap_draw(bitmap, cliprect, state->fix_tilemap, 0, 0);
	return 0;
}

/******************************************************************************/


static TILE_GET_INFO( get_cobracom_fix_tile_info )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	int offs = tile_index << 1;
	int tile = state->videoram[offs + 1] + (state->videoram[offs] << 8);
	int color = (tile & 0xe000) >> 13;

	SET_TILE_INFO(
			0,
			tile & 0xfff,
			color,
			0);
}

VIDEO_START( cobracom )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->fix_tilemap = tilemap_create(machine, get_cobracom_fix_tile_info, tilemap_scan_rows, 8, 8, 32, 32);

	tilemap_set_transparent_pen(state->fix_tilemap, 0);

	state->game_uses_priority = 0;
	machine.device<deco_bac06_device>("tilegen1")->set_colmask(0x3);
	machine.device<deco_bac06_device>("tilegen2")->set_colmask(0x3);
	
}

/******************************************************************************/

SCREEN_UPDATE( ghostb )
{
	dec8_state *state = screen->machine().driver_data<dec8_state>();
	screen->machine().device<deco_bac06_device>("tilegen1")->deco_bac06_pf_draw(screen->machine(),bitmap,cliprect,TILEMAP_DRAW_OPAQUE, 0x00, 0x00, 0x00, 0x00);
	draw_sprites1(screen->machine(), bitmap, cliprect, 0);
	tilemap_draw(bitmap, cliprect, state->fix_tilemap, 0, 0);
	return 0;
}

static TILE_GET_INFO( get_ghostb_fix_tile_info )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	int offs = tile_index << 1;
	int tile = state->videoram[offs + 1] + (state->videoram[offs] << 8);
	int color = (tile & 0xc00) >> 10;

	SET_TILE_INFO(
			0,
			tile & 0x3ff,
			color,
			0);
}

VIDEO_START( ghostb )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->fix_tilemap = tilemap_create(machine, get_ghostb_fix_tile_info, tilemap_scan_rows, 8, 8, 32, 32);
	tilemap_set_transparent_pen(state->fix_tilemap, 0);

	state->game_uses_priority = 0;
	machine.device<deco_bac06_device>("tilegen1")->set_colmask(0xf);
}

/******************************************************************************/

SCREEN_UPDATE( oscar )
{
	dec8_state *state = screen->machine().driver_data<dec8_state>();
	flip_screen_set(screen->machine(), state->bg_control[1] >> 7);

	// we mimic the priority scheme in dec0.c, this was originally a bit different, so this could be wrong
	screen->machine().device<deco_bac06_device>("tilegen1")->deco_bac06_pf_draw(screen->machine(),bitmap,cliprect,TILEMAP_DRAW_OPAQUE, 0x00, 0x00, 0x00, 0x00);
	screen->machine().device<deco_mxc06_device>("spritegen")->draw_sprites(screen->machine(), bitmap, cliprect, state->buffered_spriteram16, 0x00, 0x00, 0x0f);
	screen->machine().device<deco_bac06_device>("tilegen1")->deco_bac06_pf_draw(screen->machine(),bitmap,cliprect,0, 0x08,0x08,0x08,0x08);
	tilemap_draw(bitmap,cliprect, state->fix_tilemap, 0, 0);
	return 0;
}

static TILE_GET_INFO( get_oscar_fix_tile_info )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	int offs = tile_index << 1;
	int tile = state->videoram[offs + 1] + (state->videoram[offs] << 8);
	int color = (tile & 0xf000) >> 14;

	SET_TILE_INFO(
			0,
			tile&0xfff,
			color,
			0);
}

VIDEO_START( oscar )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->fix_tilemap = tilemap_create(machine, get_oscar_fix_tile_info, tilemap_scan_rows, 8, 8, 32, 32);

	tilemap_set_transparent_pen(state->fix_tilemap, 0);

	state->game_uses_priority = 1;
	machine.device<deco_bac06_device>("tilegen1")->set_colmask(0x7);

}

/******************************************************************************/

SCREEN_UPDATE( lastmisn )
{
	dec8_state *state = screen->machine().driver_data<dec8_state>();
	tilemap_set_scrollx(state->bg_tilemap, 0, ((state->scroll2[0] << 8)+ state->scroll2[1]));
	tilemap_set_scrolly(state->bg_tilemap, 0, ((state->scroll2[2] << 8)+ state->scroll2[3]));

	tilemap_draw(bitmap, cliprect, state->bg_tilemap, 0, 0);
	draw_sprites1(screen->machine(), bitmap, cliprect, 0);
	tilemap_draw(bitmap, cliprect, state->fix_tilemap, 0, 0);
	return 0;
}

SCREEN_UPDATE( shackled )
{
	dec8_state *state = screen->machine().driver_data<dec8_state>();
	tilemap_set_scrollx(state->bg_tilemap, 0, ((state->scroll2[0] << 8) + state->scroll2[1]));
	tilemap_set_scrolly(state->bg_tilemap, 0, ((state->scroll2[2] << 8) + state->scroll2[3]));

	tilemap_draw(bitmap, cliprect, state->bg_tilemap, TILEMAP_DRAW_LAYER1 | 0, 0);
	tilemap_draw(bitmap, cliprect, state->bg_tilemap, TILEMAP_DRAW_LAYER1 | 1, 0);
	tilemap_draw(bitmap, cliprect, state->bg_tilemap, TILEMAP_DRAW_LAYER0 | 0, 0);
	draw_sprites1(screen->machine(), bitmap, cliprect, 0);
	tilemap_draw(bitmap, cliprect, state->bg_tilemap, TILEMAP_DRAW_LAYER0 | 1, 0);
	tilemap_draw(bitmap, cliprect, state->fix_tilemap, 0, 0);
	return 0;
}

static TILEMAP_MAPPER( lastmisn_scan_rows )
{
	/* logical (col,row) -> memory offset */
	return ((col & 0x0f) + ((row & 0x0f) << 4)) + ((col & 0x10) << 4) + ((row & 0x10) << 5);
}

static TILE_GET_INFO( get_lastmisn_tile_info )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	int offs = tile_index * 2;
	int tile = state->bg_data[offs + 1] + (state->bg_data[offs] << 8);
	int color = tile >> 12;

	if (color > 7 && state->game_uses_priority)
		tileinfo->category = 1;
	else
		tileinfo->category = 0;

	SET_TILE_INFO(
			2,
			tile & 0xfff,
			color,
			0);
}

static TILE_GET_INFO( get_lastmisn_fix_tile_info )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	int offs = tile_index << 1;
	int tile = state->videoram[offs + 1] + (state->videoram[offs] << 8);
	int color = (tile & 0xc000) >> 14;

	SET_TILE_INFO(
			0,
			tile&0xfff,
			color,
			0);
}

VIDEO_START( lastmisn )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->bg_tilemap = tilemap_create(machine, get_lastmisn_tile_info, lastmisn_scan_rows, 16, 16, 32, 32);
	state->fix_tilemap = tilemap_create(machine, get_lastmisn_fix_tile_info, tilemap_scan_rows, 8, 8, 32, 32);

	tilemap_set_transparent_pen(state->fix_tilemap, 0);
	state->game_uses_priority = 0;
}

VIDEO_START( shackled )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->bg_tilemap = tilemap_create(machine, get_lastmisn_tile_info, lastmisn_scan_rows, 16, 16, 32, 32);
	state->fix_tilemap = tilemap_create(machine, get_lastmisn_fix_tile_info, tilemap_scan_rows, 8, 8, 32, 32);

	tilemap_set_transparent_pen(state->fix_tilemap, 0);
	tilemap_set_transmask(state->bg_tilemap, 0, 0x000f, 0xfff0); /* Bottom 12 pens */
	state->game_uses_priority = 1;
}

/******************************************************************************/

SCREEN_UPDATE( srdarwin )
{
	dec8_state *state = screen->machine().driver_data<dec8_state>();
	tilemap_set_scrollx(state->bg_tilemap, 0, (state->scroll2[0] << 8) + state->scroll2[1]);

	tilemap_draw(bitmap, cliprect, state->bg_tilemap, TILEMAP_DRAW_LAYER1, 0);
	srdarwin_draw_sprites(screen->machine(), bitmap, cliprect, 0); //* (srdarwin37b5gre)
	tilemap_draw(bitmap, cliprect, state->bg_tilemap, TILEMAP_DRAW_LAYER0, 0);
	srdarwin_draw_sprites(screen->machine(), bitmap, cliprect, 1);
	tilemap_draw(bitmap, cliprect, state->fix_tilemap, 0, 0);
	return 0;
}

static TILE_GET_INFO( get_srdarwin_fix_tile_info )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	int tile = state->videoram[tile_index];
	int color = 0; /* ? */

	if (color > 1)
		tileinfo->category = 1;
	else
		tileinfo->category = 0;

	SET_TILE_INFO(
			0,
			tile,
			color,
			0);
}

//AT: improved priority and fixed stage 4+ crashes caused by bank overflow
static TILE_GET_INFO( get_srdarwin_tile_info )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	int tile = state->bg_data[2 * tile_index + 1] + (state->bg_data[2 * tile_index] << 8);
	int color = tile >> 12 & 3;
	int bank;

	tile = tile & 0x3ff;
	bank = (tile / 0x100) + 2;

	SET_TILE_INFO(
			bank,
			tile,
			color,
			0);
	tileinfo->group = color;
}

VIDEO_START( srdarwin )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->bg_tilemap = tilemap_create(machine, get_srdarwin_tile_info, tilemap_scan_rows, 16, 16, 32, 16);
	state->fix_tilemap = tilemap_create(machine, get_srdarwin_fix_tile_info, tilemap_scan_rows, 8, 8, 32, 32);

	tilemap_set_transparent_pen(state->fix_tilemap, 0);
	tilemap_set_transmask(state->bg_tilemap, 0, 0xffff, 0x0000); //* draw as background only
	tilemap_set_transmask(state->bg_tilemap, 1, 0x00ff, 0xff00); /* Bottom 8 pens */
	tilemap_set_transmask(state->bg_tilemap, 2, 0x00ff, 0xff00); /* Bottom 8 pens */
	tilemap_set_transmask(state->bg_tilemap, 3, 0x0000, 0xffff); //* draw as foreground only
}

/******************************************************************************/

SCREEN_UPDATE( gondo )
{
	dec8_state *state = screen->machine().driver_data<dec8_state>();
	tilemap_set_scrollx(state->bg_tilemap, 0, ((state->scroll2[0] << 8) + state->scroll2[1]));
	tilemap_set_scrolly(state->bg_tilemap, 0, ((state->scroll2[2] << 8) + state->scroll2[3]));

	tilemap_draw(bitmap, cliprect, state->bg_tilemap, TILEMAP_DRAW_LAYER1, 0);
	draw_sprites1(screen->machine(), bitmap, cliprect, 2);
	tilemap_draw(bitmap, cliprect, state->bg_tilemap, TILEMAP_DRAW_LAYER0, 0);
	draw_sprites1(screen->machine(), bitmap, cliprect, 1);
	tilemap_draw(bitmap, cliprect, state->fix_tilemap, 0, 0);
	return 0;
}

SCREEN_UPDATE( garyoret )
{
	dec8_state *state = screen->machine().driver_data<dec8_state>();
	tilemap_set_scrollx(state->bg_tilemap, 0, ((state->scroll2[0] << 8) + state->scroll2[1]));
	tilemap_set_scrolly(state->bg_tilemap, 0, ((state->scroll2[2] << 8) + state->scroll2[3]));

	tilemap_draw(bitmap, cliprect, state->bg_tilemap, 0, 0);
	draw_sprites1(screen->machine(), bitmap, cliprect, 0);
	tilemap_draw(bitmap, cliprect, state->bg_tilemap, 1, 0);
	tilemap_draw(bitmap, cliprect, state->fix_tilemap, 0, 0);
	return 0;
}

static TILE_GET_INFO( get_gondo_fix_tile_info )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	int offs = tile_index * 2;
	int tile = state->videoram[offs + 1] + (state->videoram[offs] << 8);
	int color = (tile & 0x7000) >> 12;

	SET_TILE_INFO(
			0,
			tile&0xfff,
			color,
			0);
}

static TILE_GET_INFO( get_gondo_tile_info )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	int offs = tile_index * 2;
	int tile = state->bg_data[offs + 1] + (state->bg_data[offs] << 8);
	int color = tile>> 12;

	if (color > 7 && state->game_uses_priority)
		tileinfo->category = 1;
	else
		tileinfo->category = 0;

	SET_TILE_INFO(
			2,
			tile&0xfff,
			color,
			0);
}

VIDEO_START( gondo )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->fix_tilemap = tilemap_create(machine, get_gondo_fix_tile_info, tilemap_scan_rows, 8, 8, 32, 32);
	state->bg_tilemap = tilemap_create(machine, get_gondo_tile_info, tilemap_scan_rows, 16, 16, 32, 32);

	tilemap_set_transparent_pen(state->fix_tilemap, 0);
	tilemap_set_transmask(state->bg_tilemap, 0, 0x00ff, 0xff00); /* Bottom 8 pens */
	state->game_uses_priority = 0;
}

VIDEO_START( garyoret )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->fix_tilemap = tilemap_create(machine, get_gondo_fix_tile_info, tilemap_scan_rows, 8, 8, 32, 32);
	state->bg_tilemap = tilemap_create(machine, get_gondo_tile_info, tilemap_scan_rows, 16, 16, 32, 32);

	tilemap_set_transparent_pen(state->fix_tilemap, 0);
	state->game_uses_priority = 1;
}

/******************************************************************************/
