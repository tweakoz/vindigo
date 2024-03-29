/***************************************************************************

    Prehistoric Isle video routines

    Emulation by Bryan McPhail, mish@tendril.co.uk

***************************************************************************/

#include "emu.h"
#include "includes/prehisle.h"


WRITE16_MEMBER(prehisle_state::prehisle_bg_videoram16_w)
{
	COMBINE_DATA(&m_bg_videoram16[offset]);
	m_bg_tilemap->mark_tile_dirty(offset);
}

WRITE16_MEMBER(prehisle_state::prehisle_fg_videoram16_w)
{
	COMBINE_DATA(&m_videoram[offset]);
	m_fg_tilemap->mark_tile_dirty(offset);
}

READ16_MEMBER(prehisle_state::prehisle_control16_r)
{
	switch (offset)
	{
	case 0x08: return ioport("P2")->read();                     // Player 2
	case 0x10: return ioport("COIN")->read();                       // Coins, Tilt, Service
	case 0x20: return ioport("P1")->read() ^ m_invert_controls;     // Player 1
	case 0x21: return ioport("DSW0")->read();                       // DIPs
	case 0x22: return ioport("DSW1")->read();                       // DIPs + VBLANK
	default: return 0;
	}
}

WRITE16_MEMBER(prehisle_state::prehisle_control16_w)
{
	int scroll = 0;

	COMBINE_DATA(&scroll);

	switch (offset)
	{
	case 0x00: m_bg_tilemap->set_scrolly(0, scroll); break;
	case 0x08: m_bg_tilemap->set_scrollx(0, scroll); break;
	case 0x10: m_bg2_tilemap->set_scrolly(0, scroll); break;
	case 0x18: m_bg2_tilemap->set_scrollx(0, scroll); break;
	case 0x23: m_invert_controls = data ? 0x00ff : 0x0000; break;
	case 0x28: coin_counter_w(machine(), 0, data & 1); break;
	case 0x29: coin_counter_w(machine(), 1, data & 1); break;
	case 0x30: flip_screen_set(data & 0x01); break;
	}
}

TILE_GET_INFO_MEMBER(prehisle_state::get_bg2_tile_info)
{
	UINT8 *tilerom = memregion("gfx5")->base();

	int offs = tile_index * 2;
	int attr = tilerom[offs + 1] + (tilerom[offs] << 8);
	int code = (attr & 0x7ff) | 0x800;
	int color = attr >> 12;
	int flags = (attr & 0x800) ? TILE_FLIPX : 0;

	SET_TILE_INFO_MEMBER(1, code, color, flags);
}

TILE_GET_INFO_MEMBER(prehisle_state::get_bg_tile_info)
{
	int attr = m_bg_videoram16[tile_index];
	int code = attr & 0x7ff;
	int color = attr >> 12;
	int flags = (attr & 0x800) ? TILE_FLIPY : 0;

	SET_TILE_INFO_MEMBER(2, code, color, flags);
}

TILE_GET_INFO_MEMBER(prehisle_state::get_fg_tile_info)
{
	int attr = m_videoram[tile_index];
	int code = attr & 0xfff;
	int color = attr >> 12;

	SET_TILE_INFO_MEMBER(0, code, color, 0);
}

void prehisle_state::video_start()
{
	m_bg2_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(prehisle_state::get_bg2_tile_info),this), TILEMAP_SCAN_COLS,
			16, 16, 1024, 32);

	m_bg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(prehisle_state::get_bg_tile_info),this), TILEMAP_SCAN_COLS,
			16, 16, 256, 32);

	m_fg_tilemap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(prehisle_state::get_fg_tile_info),this), TILEMAP_SCAN_ROWS,
			8, 8, 32, 32);

	m_bg_tilemap->set_transparent_pen(15);
	m_fg_tilemap->set_transparent_pen(15);

	/* register for saving */
	save_item(NAME(m_invert_controls));
}

/* sprite layout
o fedcba9876543210

0 .......xxxxxxxxx y, other bits unused?
1 .......xxxxxxxxx x, other bits unused?

2 ...xxxxxxxxxxxxx code
2 ..x............. ?
2 .x.............. flipx
2 x............... flipy

3 xxxx............ color+priority, other bits unknown
*/
void prehisle_state::draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, int foreground )
{
	UINT16 *spriteram16 = m_spriteram;
	int offs;

	for (offs = 0; offs < 1024; offs += 4)
	{
		int attr = spriteram16[offs + 2];
		int code = attr & 0x1fff;
		int color = spriteram16[offs + 3] >> 12;
		int priority = (color < 0x4); // correct?
		int flipx = attr & 0x4000;
		int flipy = attr & 0x8000;
		int sx = spriteram16[offs + 1]&0x1ff;
		int sy = spriteram16[offs]&0x1ff;

		// coordinates are 9-bit signed
		if (sx&0x100) sx=-0x100+(sx&0xff);
		if (sy&0x100) sy=-0x100+(sy&0xff);

		if (flip_screen())
		{
			sx = 240 - sx;
			sy = 240 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		if ((foreground && priority) || (!foreground && !priority))
		{
			 machine().gfx[3]->transpen(bitmap,cliprect, code, color, flipx, flipy, sx, sy, 15);
		}
	}
}

UINT32 prehisle_state::screen_update_prehisle(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_bg2_tilemap->draw(screen, bitmap, cliprect, 0, 0);
	draw_sprites(bitmap, cliprect, 0);
	m_bg_tilemap->draw(screen, bitmap, cliprect, 0, 0);
	draw_sprites(bitmap, cliprect, 1);
	m_fg_tilemap->draw(screen, bitmap, cliprect, 0, 0);
	return 0;
}
