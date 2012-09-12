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
#include "video/deckarn.h"

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
	const UINT8 *color_prom = machine.root_device().memregion("proms")->base();
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

WRITE8_MEMBER(dec8_state::dec8_bg_data_w)
{
	m_bg_data[offset] = data;
	m_bg_tilemap->mark_tile_dirty(offset / 2);
}

READ8_MEMBER(dec8_state::dec8_bg_data_r)
{
	return m_bg_data[offset];
}


WRITE8_MEMBER(dec8_state::dec8_videoram_w)
{
	m_videoram[offset] = data;
	m_fix_tilemap->mark_tile_dirty(offset / 2);
}

WRITE8_MEMBER(dec8_state::srdarwin_videoram_w)
{
	m_videoram[offset] = data;
	m_fix_tilemap->mark_tile_dirty(offset);
}


WRITE8_MEMBER(dec8_state::dec8_scroll2_w)
{
	m_scroll2[offset] = data;
}

WRITE8_MEMBER(dec8_state::srdarwin_control_w)
{

	switch (offset)
	{
	case 0: /* Top 3 bits - bank switch, bottom 4 - scroll MSB */
		membank("bank1")->set_entry((data >> 5));
		m_scroll2[0] = data & 0xf;
		return;

	case 1:
		m_scroll2[1] = data;
		return;
    }
}

WRITE8_MEMBER(dec8_state::lastmisn_control_w)
{

	/*
        Bit 0x0f - ROM bank switch.
        Bit 0x10 - Unused
        Bit 0x20 - X scroll MSB
        Bit 0x40 - Y scroll MSB
        Bit 0x80 - Hold subcpu reset line high if clear, else low
    */
	membank("bank1")->set_entry(data & 0x0f);

	m_scroll2[0] = (data >> 5) & 1;
	m_scroll2[2] = (data >> 6) & 1;

	if (data & 0x80)
		m_subcpu->set_input_line(INPUT_LINE_RESET, CLEAR_LINE);
	else
		m_subcpu->set_input_line(INPUT_LINE_RESET, ASSERT_LINE);
}

WRITE8_MEMBER(dec8_state::shackled_control_w)
{

	/* Bottom 4 bits - bank switch, Bits 4 & 5 - Scroll MSBs */
	membank("bank1")->set_entry(data & 0x0f);

	m_scroll2[0] = (data >> 5) & 1;
	m_scroll2[2] = (data >> 6) & 1;
}

WRITE8_MEMBER(dec8_state::lastmisn_scrollx_w)
{
	m_scroll2[1] = data;
}

WRITE8_MEMBER(dec8_state::lastmisn_scrolly_w)
{
	m_scroll2[3] = data;
}

WRITE8_MEMBER(dec8_state::gondo_scroll_w)
{
	switch (offset)
	{
	case 0x0:
		m_scroll2[1] = data; /* X LSB */
		break;
	case 0x8:
		m_scroll2[3] = data; /* Y LSB */
		break;
	case 0x10:
		m_scroll2[0] = (data >> 0) & 1; /* Bit 0: X MSB */
		m_scroll2[2] = (data >> 1) & 1; /* Bit 1: Y MSB */
		/* Bit 2 is also used in Gondo & Garyoret */
		break;
	}
}

/******************************************************************************/


static void srdarwin_draw_sprites( running_machine& machine, bitmap_ind16 &bitmap, const rectangle &cliprect, int pri )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	UINT8 *buffered_spriteram = state->m_spriteram->buffer();
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

		if (state->flip_screen())
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
				fx,state->flip_screen(),
				sx,sy,0);
        if (multi)
    		drawgfx_transpen(bitmap,cliprect,machine.gfx[1],
				code+1,
				color,
				fx,state->flip_screen(),
				sx,sy2,0);
	}
}

/******************************************************************************/

SCREEN_UPDATE_IND16( cobracom )
{
	dec8_state *state = screen.machine().driver_data<dec8_state>();

	state->flip_screen_set(state->m_bg_control[0] >> 7);

	screen.machine().device<deco_bac06_device>("tilegen1")->deco_bac06_pf_draw(screen.machine(),bitmap,cliprect,TILEMAP_DRAW_OPAQUE, 0x00, 0x00, 0x00, 0x00);
	screen.machine().device<deco_mxc06_device>("spritegen")->draw_sprites(screen.machine(), bitmap, cliprect, state->m_buffered_spriteram16, 0x04, 0x00, 0x03);
	screen.machine().device<deco_bac06_device>("tilegen2")->deco_bac06_pf_draw(screen.machine(),bitmap,cliprect,0, 0x00, 0x00, 0x00, 0x00);
	screen.machine().device<deco_mxc06_device>("spritegen")->draw_sprites(screen.machine(), bitmap, cliprect, state->m_buffered_spriteram16, 0x04, 0x04, 0x03);
	state->m_fix_tilemap->draw(bitmap, cliprect, 0, 0);
	return 0;
}

/******************************************************************************/


TILE_GET_INFO_MEMBER(dec8_state::get_cobracom_fix_tile_info)
{
	int offs = tile_index << 1;
	int tile = m_videoram[offs + 1] + (m_videoram[offs] << 8);
	int color = (tile & 0xe000) >> 13;

	SET_TILE_INFO_MEMBER(
			0,
			tile & 0xfff,
			color,
			0);
}

VIDEO_START( cobracom )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->m_fix_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_cobracom_fix_tile_info),state), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);

	state->m_fix_tilemap->set_transparent_pen(0);

	state->m_game_uses_priority = 0;
	machine.device<deco_bac06_device>("tilegen1")->set_colmask(0x3);
	machine.device<deco_bac06_device>("tilegen2")->set_colmask(0x3);

}

/******************************************************************************/

SCREEN_UPDATE_IND16( ghostb )
{
	dec8_state *state = screen.machine().driver_data<dec8_state>();
	screen.machine().device<deco_bac06_device>("tilegen1")->deco_bac06_pf_draw(screen.machine(),bitmap,cliprect,TILEMAP_DRAW_OPAQUE, 0x00, 0x00, 0x00, 0x00);
	screen.machine().device<deco_karnovsprites_device>("spritegen")->draw_sprites(screen.machine(), bitmap, cliprect, state->m_buffered_spriteram16, 0x400, 0);
	state->m_fix_tilemap->draw(bitmap, cliprect, 0, 0);
	return 0;
}

TILE_GET_INFO_MEMBER(dec8_state::get_ghostb_fix_tile_info)
{
	int offs = tile_index << 1;
	int tile = m_videoram[offs + 1] + (m_videoram[offs] << 8);
	int color = (tile & 0xc00) >> 10;

	SET_TILE_INFO_MEMBER(
			0,
			tile & 0x3ff,
			color,
			0);
}

VIDEO_START( ghostb )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->m_fix_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_ghostb_fix_tile_info),state), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);
	state->m_fix_tilemap->set_transparent_pen(0);

	state->m_game_uses_priority = 0;
	machine.device<deco_bac06_device>("tilegen1")->set_colmask(0xf);
}

/******************************************************************************/

SCREEN_UPDATE_IND16( oscar )
{
	dec8_state *state = screen.machine().driver_data<dec8_state>();
	state->flip_screen_set(state->m_bg_control[1] >> 7);

	// we mimic the priority scheme in dec0.c, this was originally a bit different, so this could be wrong
	screen.machine().device<deco_bac06_device>("tilegen1")->deco_bac06_pf_draw(screen.machine(),bitmap,cliprect,TILEMAP_DRAW_OPAQUE, 0x00, 0x00, 0x00, 0x00);
	screen.machine().device<deco_mxc06_device>("spritegen")->draw_sprites(screen.machine(), bitmap, cliprect, state->m_buffered_spriteram16, 0x00, 0x00, 0x0f);
	screen.machine().device<deco_bac06_device>("tilegen1")->deco_bac06_pf_draw(screen.machine(),bitmap,cliprect,0, 0x08,0x08,0x08,0x08);
	state->m_fix_tilemap->draw(bitmap, cliprect, 0, 0);
	return 0;
}

TILE_GET_INFO_MEMBER(dec8_state::get_oscar_fix_tile_info)
{
	int offs = tile_index << 1;
	int tile = m_videoram[offs + 1] + (m_videoram[offs] << 8);
	int color = (tile & 0xf000) >> 14;

	SET_TILE_INFO_MEMBER(
			0,
			tile&0xfff,
			color,
			0);
}

VIDEO_START( oscar )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->m_fix_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_oscar_fix_tile_info),state), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);

	state->m_fix_tilemap->set_transparent_pen(0);

	state->m_game_uses_priority = 1;
	machine.device<deco_bac06_device>("tilegen1")->set_colmask(0x7);

}

/******************************************************************************/

SCREEN_UPDATE_IND16( lastmisn )
{
	dec8_state *state = screen.machine().driver_data<dec8_state>();
	state->m_bg_tilemap->set_scrollx(0, ((state->m_scroll2[0] << 8)+ state->m_scroll2[1]));
	state->m_bg_tilemap->set_scrolly(0, ((state->m_scroll2[2] << 8)+ state->m_scroll2[3]));

	state->m_bg_tilemap->draw(bitmap, cliprect, 0, 0);
	screen.machine().device<deco_karnovsprites_device>("spritegen")->draw_sprites(screen.machine(), bitmap, cliprect, state->m_buffered_spriteram16, 0x400, 0);
	state->m_fix_tilemap->draw(bitmap, cliprect, 0, 0);
	return 0;
}

SCREEN_UPDATE_IND16( shackled )
{
	dec8_state *state = screen.machine().driver_data<dec8_state>();
	state->m_bg_tilemap->set_scrollx(0, ((state->m_scroll2[0] << 8) + state->m_scroll2[1]));
	state->m_bg_tilemap->set_scrolly(0, ((state->m_scroll2[2] << 8) + state->m_scroll2[3]));

	state->m_bg_tilemap->draw(bitmap, cliprect, TILEMAP_DRAW_LAYER1 | 0, 0);
	state->m_bg_tilemap->draw(bitmap, cliprect, TILEMAP_DRAW_LAYER1 | 1, 0);
	state->m_bg_tilemap->draw(bitmap, cliprect, TILEMAP_DRAW_LAYER0 | 0, 0);
	screen.machine().device<deco_karnovsprites_device>("spritegen")->draw_sprites(screen.machine(), bitmap, cliprect, state->m_buffered_spriteram16, 0x400, 0);
	state->m_bg_tilemap->draw(bitmap, cliprect, TILEMAP_DRAW_LAYER0 | 1, 0);
	state->m_fix_tilemap->draw(bitmap, cliprect, 0, 0);
	return 0;
}

TILEMAP_MAPPER_MEMBER(dec8_state::lastmisn_scan_rows)
{
	/* logical (col,row) -> memory offset */
	return ((col & 0x0f) + ((row & 0x0f) << 4)) + ((col & 0x10) << 4) + ((row & 0x10) << 5);
}

TILE_GET_INFO_MEMBER(dec8_state::get_lastmisn_tile_info)
{
	int offs = tile_index * 2;
	int tile = m_bg_data[offs + 1] + (m_bg_data[offs] << 8);
	int color = tile >> 12;

	if (color > 7 && m_game_uses_priority)
		tileinfo.category = 1;
	else
		tileinfo.category = 0;

	SET_TILE_INFO_MEMBER(
			2,
			tile & 0xfff,
			color,
			0);
}

TILE_GET_INFO_MEMBER(dec8_state::get_lastmisn_fix_tile_info)
{
	int offs = tile_index << 1;
	int tile = m_videoram[offs + 1] + (m_videoram[offs] << 8);
	int color = (tile & 0xc000) >> 14;

	SET_TILE_INFO_MEMBER(
			0,
			tile&0xfff,
			color,
			0);
}

VIDEO_START( lastmisn )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->m_bg_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_lastmisn_tile_info),state), tilemap_mapper_delegate(FUNC(dec8_state::lastmisn_scan_rows),state), 16, 16, 32, 32);
	state->m_fix_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_lastmisn_fix_tile_info),state), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);

	state->m_fix_tilemap->set_transparent_pen(0);
	state->m_game_uses_priority = 0;
}

VIDEO_START( shackled )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->m_bg_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_lastmisn_tile_info),state), tilemap_mapper_delegate(FUNC(dec8_state::lastmisn_scan_rows),state), 16, 16, 32, 32);
	state->m_fix_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_lastmisn_fix_tile_info),state), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);

	state->m_fix_tilemap->set_transparent_pen(0);
	state->m_bg_tilemap->set_transmask(0, 0x000f, 0xfff0); /* Bottom 12 pens */
	state->m_game_uses_priority = 1;
}

/******************************************************************************/

SCREEN_UPDATE_IND16( srdarwin )
{
	dec8_state *state = screen.machine().driver_data<dec8_state>();
	state->m_bg_tilemap->set_scrollx(0, (state->m_scroll2[0] << 8) + state->m_scroll2[1]);

	state->m_bg_tilemap->draw(bitmap, cliprect, TILEMAP_DRAW_LAYER1, 0);
	srdarwin_draw_sprites(screen.machine(), bitmap, cliprect, 0); //* (srdarwin37b5gre)
	state->m_bg_tilemap->draw(bitmap, cliprect, TILEMAP_DRAW_LAYER0, 0);
	srdarwin_draw_sprites(screen.machine(), bitmap, cliprect, 1);
	state->m_fix_tilemap->draw(bitmap, cliprect, 0, 0);
	return 0;
}

TILE_GET_INFO_MEMBER(dec8_state::get_srdarwin_fix_tile_info)
{
	int tile = m_videoram[tile_index];
	int color = 0; /* ? */

	if (color > 1)
		tileinfo.category = 1;
	else
		tileinfo.category = 0;

	SET_TILE_INFO_MEMBER(
			0,
			tile,
			color,
			0);
}

//AT: improved priority and fixed stage 4+ crashes caused by bank overflow
TILE_GET_INFO_MEMBER(dec8_state::get_srdarwin_tile_info)
{
	int tile = m_bg_data[2 * tile_index + 1] + (m_bg_data[2 * tile_index] << 8);
	int color = tile >> 12 & 3;
	int bank;

	tile = tile & 0x3ff;
	bank = (tile / 0x100) + 2;

	SET_TILE_INFO_MEMBER(
			bank,
			tile,
			color,
			0);
	tileinfo.group = color;
}

VIDEO_START( srdarwin )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->m_bg_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_srdarwin_tile_info),state), TILEMAP_SCAN_ROWS, 16, 16, 32, 16);
	state->m_fix_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_srdarwin_fix_tile_info),state), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);

	state->m_fix_tilemap->set_transparent_pen(0);
	state->m_bg_tilemap->set_transmask(0, 0xffff, 0x0000); //* draw as background only
	state->m_bg_tilemap->set_transmask(1, 0x00ff, 0xff00); /* Bottom 8 pens */
	state->m_bg_tilemap->set_transmask(2, 0x00ff, 0xff00); /* Bottom 8 pens */
	state->m_bg_tilemap->set_transmask(3, 0x0000, 0xffff); //* draw as foreground only
}

/******************************************************************************/

SCREEN_UPDATE_IND16( gondo )
{
	dec8_state *state = screen.machine().driver_data<dec8_state>();
	state->m_bg_tilemap->set_scrollx(0, ((state->m_scroll2[0] << 8) + state->m_scroll2[1]));
	state->m_bg_tilemap->set_scrolly(0, ((state->m_scroll2[2] << 8) + state->m_scroll2[3]));

	state->m_bg_tilemap->draw(bitmap, cliprect, TILEMAP_DRAW_LAYER1, 0);
	screen.machine().device<deco_karnovsprites_device>("spritegen")->draw_sprites(screen.machine(), bitmap, cliprect, state->m_buffered_spriteram16, 0x400, 2);
	state->m_bg_tilemap->draw(bitmap, cliprect, TILEMAP_DRAW_LAYER0, 0);
	screen.machine().device<deco_karnovsprites_device>("spritegen")->draw_sprites(screen.machine(), bitmap, cliprect, state->m_buffered_spriteram16, 0x400, 1);
	state->m_fix_tilemap->draw(bitmap, cliprect, 0, 0);
	return 0;
}

SCREEN_UPDATE_IND16( garyoret )
{
	dec8_state *state = screen.machine().driver_data<dec8_state>();
	state->m_bg_tilemap->set_scrollx(0, ((state->m_scroll2[0] << 8) + state->m_scroll2[1]));
	state->m_bg_tilemap->set_scrolly(0, ((state->m_scroll2[2] << 8) + state->m_scroll2[3]));

	state->m_bg_tilemap->draw(bitmap, cliprect, 0, 0);
	screen.machine().device<deco_karnovsprites_device>("spritegen")->draw_sprites(screen.machine(), bitmap, cliprect, state->m_buffered_spriteram16, 0x400, 0);
	state->m_bg_tilemap->draw(bitmap, cliprect, 1, 0);
	state->m_fix_tilemap->draw(bitmap, cliprect, 0, 0);
	return 0;
}

TILE_GET_INFO_MEMBER(dec8_state::get_gondo_fix_tile_info)
{
	int offs = tile_index * 2;
	int tile = m_videoram[offs + 1] + (m_videoram[offs] << 8);
	int color = (tile & 0x7000) >> 12;

	SET_TILE_INFO_MEMBER(
			0,
			tile&0xfff,
			color,
			0);
}

TILE_GET_INFO_MEMBER(dec8_state::get_gondo_tile_info)
{
	int offs = tile_index * 2;
	int tile = m_bg_data[offs + 1] + (m_bg_data[offs] << 8);
	int color = tile>> 12;

	if (color > 7 && m_game_uses_priority)
		tileinfo.category = 1;
	else
		tileinfo.category = 0;

	SET_TILE_INFO_MEMBER(
			2,
			tile&0xfff,
			color,
			0);
}

VIDEO_START( gondo )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->m_fix_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_gondo_fix_tile_info),state), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);
	state->m_bg_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_gondo_tile_info),state), TILEMAP_SCAN_ROWS, 16, 16, 32, 32);

	state->m_fix_tilemap->set_transparent_pen(0);
	state->m_bg_tilemap->set_transmask(0, 0x00ff, 0xff00); /* Bottom 8 pens */
	state->m_game_uses_priority = 0;
}

VIDEO_START( garyoret )
{
	dec8_state *state = machine.driver_data<dec8_state>();
	state->m_fix_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_gondo_fix_tile_info),state), TILEMAP_SCAN_ROWS, 8, 8, 32, 32);
	state->m_bg_tilemap = &machine.tilemap().create(tilemap_get_info_delegate(FUNC(dec8_state::get_gondo_tile_info),state), TILEMAP_SCAN_ROWS, 16, 16, 32, 32);

	state->m_fix_tilemap->set_transparent_pen(0);
	state->m_game_uses_priority = 1;
}

/******************************************************************************/
