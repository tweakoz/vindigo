/***************************************************************************

  video.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "emu.h"
#include "includes/funkybee.h"

void funkybee_state::palette_init()
{
	const UINT8 *color_prom = machine().root_device().memregion("proms")->base();
	int i;

	/* first, the character/sprite palette */
	for (i = 0; i < 32; i++)
	{
		int bit0, bit1, bit2, r, g, b;

		/* red component */
		bit0 = (*color_prom >> 0) & 0x01;
		bit1 = (*color_prom >> 1) & 0x01;
		bit2 = (*color_prom >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* green component */
		bit0 = (*color_prom >> 3) & 0x01;
		bit1 = (*color_prom >> 4) & 0x01;
		bit2 = (*color_prom >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		/* blue component */
		bit0 = 0;
		bit1 = (*color_prom >> 6) & 0x01;
		bit2 = (*color_prom >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		palette_set_color(machine(), i, MAKE_RGB(r,g,b));
		color_prom++;
	}
}

WRITE8_MEMBER(funkybee_state::funkybee_videoram_w)
{
	m_videoram[offset] = data;
	m_bg_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER(funkybee_state::funkybee_colorram_w)
{
	m_colorram[offset] = data;
	m_bg_tilemap->mark_tile_dirty(offset);
}

WRITE8_MEMBER(funkybee_state::funkybee_gfx_bank_w)
{
	if (m_gfx_bank != (data & 0x01))
	{
		m_gfx_bank = data & 0x01;
		machine().tilemap().mark_all_dirty();
	}
}

WRITE8_MEMBER(funkybee_state::funkybee_scroll_w)
{
	m_bg_tilemap->set_scrollx(0, flip_screen() ? -data : data);
}

WRITE8_MEMBER(funkybee_state::funkybee_flipscreen_w)
{
	flip_screen_set(data & 0x01);
}

TILE_GET_INFO_MEMBER(funkybee_state::get_bg_tile_info)
{
	int code = m_videoram[tile_index] + ((m_colorram[tile_index] & 0x80) << 1);
	int color = m_colorram[tile_index] & 0x03;

	SET_TILE_INFO_MEMBER(m_gfx_bank, code, color, 0);
}

TILEMAP_MAPPER_MEMBER(funkybee_state::funkybee_tilemap_scan)
{
	/* logical (col,row) -> memory offset */
	return 256 * row + col;
}

void funkybee_state::video_start()
{
	m_bg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(funkybee_state::get_bg_tile_info),this), tilemap_mapper_delegate(FUNC(funkybee_state::funkybee_tilemap_scan),this), 8, 8, 32, 32);
}

static void draw_sprites( running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	funkybee_state *state = machine.driver_data<funkybee_state>();
	int offs;

	for (offs = 0x0f; offs >= 0; offs--)
	{
		int offs2 = offs + 0x1e00;
		int attr = state->m_videoram[offs2];
		int code = (attr >> 2) | ((attr & 2) << 5);
		int color = state->m_colorram[offs2 + 0x10];
		int flipx = 0;
		int flipy = attr & 0x01;
		int sx = state->m_videoram[offs2 + 0x10];
		int sy = 224 - state->m_colorram[offs2];

		if (state->flip_screen())
		{
			sy += 32;
			flipx = !flipx;
		}

		drawgfx_transpen(bitmap,cliprect, machine.gfx[2 + state->m_gfx_bank],
			code, color,
			flipx, flipy,
			sx, sy, 0);
	}
}

static void draw_columns( running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	funkybee_state *state = machine.driver_data<funkybee_state>();
	int offs;

	for (offs = 0x1f; offs >= 0; offs--)
	{
		int const flip = state->flip_screen();
		int code = state->m_videoram[0x1c00 + offs];
		int color = state->m_colorram[0x1f10] & 0x03;
		int sx = flip ? state->m_videoram[0x1f1f] : state->m_videoram[0x1f10];
		int sy = offs * 8;

		if (flip)
			sy = 248 - sy;

		drawgfx_transpen(bitmap,cliprect,machine.gfx[state->m_gfx_bank],
				code, color,
				flip, flip,
				sx, sy,0);

		code = state->m_videoram[0x1d00 + offs];
		color = state->m_colorram[0x1f11] & 0x03;
		sx = flip ? state->m_videoram[0x1f1e] : state->m_videoram[0x1f11];
		sy = offs * 8;

		if (flip)
			sy = 248 - sy;

		drawgfx_transpen(bitmap,cliprect,machine.gfx[state->m_gfx_bank],
				code, color,
				flip, flip,
				sx, sy,0);
	}
}

SCREEN_UPDATE_IND16( funkybee )
{
	funkybee_state *state = screen.machine().driver_data<funkybee_state>();
	state->m_bg_tilemap->draw(bitmap, cliprect, 0, 0);
	draw_sprites(screen.machine(), bitmap, cliprect);
	draw_columns(screen.machine(), bitmap, cliprect);
	return 0;
}
