/***************************************************************************

    Atari Cyberball hardware

****************************************************************************/

#include "emu.h"
#include "machine/atarigen.h"
#include "video/atarimo.h"
#include "includes/cyberbal.h"


#define SCREEN_WIDTH        (42*16)



/*************************************
 *
 *  Tilemap callbacks
 *
 *************************************/

TILE_GET_INFO_MEMBER(cyberbal_state::get_alpha_tile_info)
{
	UINT16 data = tilemap.basemem_read(tile_index);
	int code = data & 0xfff;
	int color = (data >> 12) & 0x07;
	SET_TILE_INFO_MEMBER(2, code, color, (data >> 15) & 1);
}


TILE_GET_INFO_MEMBER(cyberbal_state::get_playfield_tile_info)
{
	UINT16 data = tilemap.basemem_read(tile_index);
	int code = data & 0x1fff;
	int color = (data >> 11) & 0x0f;
	SET_TILE_INFO_MEMBER(0, code, color, (data >> 15) & 1);
}



/*************************************
 *
 *  Video system start
 *
 *************************************/

void cyberbal_state::video_start_common(int screens)
{
	static const atarimo_desc modesc =
	{
		1,                  /* index to which gfx system */
		1,                  /* number of motion object banks */
		1,                  /* are the entries linked? */
		0,                  /* are the entries split? */
		0,                  /* render in reverse order? */
		0,                  /* render in swapped X/Y order? */
		1,                  /* does the neighbor bit affect the next object? */
		1024,               /* pixels per SLIP entry (0 for no-slip) */
		0,                  /* pixel offset for SLIPs */
		0,                  /* maximum number of links to visit/scanline (0=all) */

		0x600,              /* base palette entry */
		0x100,              /* maximum number of colors */
		0,                  /* transparent pen index */

		{{ 0,0,0x07f8,0 }}, /* mask for the link */
		{{ 0 }},            /* mask for the graphics bank */
		{{ 0x7fff,0,0,0 }}, /* mask for the code index */
		{{ 0 }},            /* mask for the upper code index */
		{{ 0,0,0,0x000f }}, /* mask for the color */
		{{ 0,0,0,0xffc0 }}, /* mask for the X position */
		{{ 0,0xff80,0,0 }}, /* mask for the Y position */
		{{ 0 }},            /* mask for the width, in tiles*/
		{{ 0,0x000f,0,0 }}, /* mask for the height, in tiles */
		{{ 0x8000,0,0,0 }}, /* mask for the horizontal flip */
		{{ 0 }},            /* mask for the vertical flip */
		{{ 0 }},            /* mask for the priority */
		{{ 0,0,0,0x0010 }}, /* mask for the neighbor */
		{{ 0 }},            /* mask for absolute coordinates */

		{{ 0 }},            /* mask for the special value */
		0,                  /* resulting value to indicate "special" */
		0                   /* callback routine for special entries */
	};

	/* initialize the motion objects */
	atarimo_init(machine(), 0, &modesc);
	atarimo_set_slipram(0, &m_current_slip[0]);

	/* allocate the second screen if necessary */
	if (screens == 2)
	{
		/* initialize the tilemaps */
		m_playfield2_tilemap->set_scrollx(0, 0);
		m_playfield2_tilemap->set_palette_offset(0x800);
		m_alpha2_tilemap->set_scrollx(0, 0);
		m_alpha2_tilemap->set_palette_offset(0x800);

		/* initialize the motion objects */
		atarimo_init(machine(), 1, &modesc);
		atarimo_set_slipram(1, &m_current_slip[1]);
	}

	/* save states */
	save_item(NAME(m_current_slip));
	save_item(NAME(m_playfield_palette_bank));
	save_item(NAME(m_playfield_xscroll));
	save_item(NAME(m_playfield_yscroll));
}


VIDEO_START_MEMBER(cyberbal_state,cyberbal)
{
	video_start_common(2);

	/* adjust the sprite positions */
	atarimo_set_xscroll(0, 4);
	atarimo_set_xscroll(1, 4);
}


VIDEO_START_MEMBER(cyberbal_state,cyberbal2p)
{
	video_start_common(1);

	/* adjust the sprite positions */
	atarimo_set_xscroll(0, 5);
}



/*************************************
 *
 *  Palette tweaker
 *
 *************************************/

inline void cyberbal_state::set_palette_entry(int entry, UINT16 value)
{
	int r, g, b;

	r = ((value >> 9) & 0x3e) | ((value >> 15) & 1);
	g = ((value >> 4) & 0x3e) | ((value >> 15) & 1);
	b = ((value << 1) & 0x3e) | ((value >> 15) & 1);

	palette_set_color_rgb(machine(), entry, pal6bit(r), pal6bit(g), pal6bit(b));
}



/*************************************
 *
 *  Palette RAM write handlers
 *
 *************************************/

WRITE16_MEMBER(cyberbal_state::paletteram_0_w)
{
	COMBINE_DATA(&m_paletteram_0[offset]);
	set_palette_entry(offset, m_paletteram_0[offset]);
}

READ16_MEMBER(cyberbal_state::paletteram_0_r)
{
	return m_paletteram_0[offset];
}


WRITE16_MEMBER(cyberbal_state::paletteram_1_w)
{
	COMBINE_DATA(&m_paletteram_1[offset]);
	set_palette_entry(offset + 0x800, m_paletteram_1[offset]);
}

READ16_MEMBER(cyberbal_state::paletteram_1_r)
{
	return m_paletteram_1[offset];
}



/*************************************
 *
 *  Periodic scanline updater
 *
 *************************************/

void cyberbal_state::scanline_update(screen_device &screen, int scanline)
{
	int i;
	screen_device *update_screen;

	/* loop over screens */
	screen_device_iterator iter(*this);
	for (i = 0, update_screen = iter.first(); update_screen != NULL; i++, update_screen = iter.next())
	{
		tilemap_t &curplayfield = i ? static_cast<tilemap_t &>(m_playfield2_tilemap) : static_cast<tilemap_t &>(m_playfield_tilemap);
		tilemap_t &curalpha = i ? static_cast<tilemap_t &>(m_alpha2_tilemap) : static_cast<tilemap_t &>(m_alpha_tilemap);

		/* keep in range */
		int offset = ((scanline - 8) / 8) * 64 + 47;
		if (offset < 0)
			offset += 0x800;
		else if (offset >= 0x800)
			return;

		/* update the current parameters */
		UINT16 word = curalpha.device()->basemem_read(offset + 3);
		if (!(word & 1))
		{
			if (((word >> 1) & 7) != m_playfield_palette_bank[i])
			{
				if (scanline > 0)
					update_screen->update_partial(scanline - 1);
				m_playfield_palette_bank[i] = (word >> 1) & 7;
				curplayfield.set_palette_offset(i * 0x800 + (m_playfield_palette_bank[i] << 8));
			}
		}
		word = curalpha.device()->basemem_read(offset + 4);
		if (!(word & 1))
		{
			int newscroll = 2 * (((word >> 7) + 4) & 0x1ff);
			if (newscroll != m_playfield_xscroll[i])
			{
				if (scanline > 0)
					update_screen->update_partial(scanline - 1);
				curplayfield.set_scrollx(0, newscroll);
				m_playfield_xscroll[i] = newscroll;
			}
		}
		word = curalpha.device()->basemem_read(offset + 5);
		if (!(word & 1))
		{
			/* a new vscroll latches the offset into a counter; we must adjust for this */
			int newscroll = ((word >> 7) - (scanline)) & 0x1ff;
			if (newscroll != m_playfield_yscroll[i])
			{
				if (scanline > 0)
					update_screen->update_partial(scanline - 1);
				curplayfield.set_scrolly(0, newscroll);
				m_playfield_yscroll[i] = newscroll;
			}
		}
		word = curalpha.device()->basemem_read(offset + 7);
		if (!(word & 1))
		{
			if (m_current_slip[i] != word)
			{
				if (scanline > 0)
					update_screen->update_partial(scanline - 1);
				m_current_slip[i] = word;
			}
		}
	}
}



/*************************************
 *
 *  Main refresh
 *
 *************************************/

UINT32 cyberbal_state::update_one_screen(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int index)
{
	atarimo_rect_list rectlist;
	rectangle tempclip = cliprect;
	bitmap_ind16 *mobitmap;
	int x, y, r, mooffset, temp;
	rectangle visarea = screen.visible_area();

	/* draw the playfield */
	tilemap_t &curplayfield = index ? static_cast<tilemap_t &>(m_playfield2_tilemap) : static_cast<tilemap_t &>(m_playfield_tilemap);
	curplayfield.draw(bitmap, cliprect, 0, 0);

	/* draw the MOs -- note some kludging to get this to work correctly for 2 screens */
	mooffset = 0;
	tempclip.min_x -= mooffset;
	tempclip.max_x -= mooffset;
	temp = visarea.max_x;
	if (temp > SCREEN_WIDTH)
		visarea.max_x /= 2;
	mobitmap = atarimo_render((index == 0) ? 0 : 1, cliprect, &rectlist);
	tempclip.min_x += mooffset;
	tempclip.max_x += mooffset;
	visarea.max_x = temp;

	/* draw and merge the MO */
	int palbase = index * 0x800;
	for (r = 0; r < rectlist.numrects; r++, rectlist.rect++)
		for (y = rectlist.rect->min_y; y <= rectlist.rect->max_y; y++)
		{
			UINT16 *mo = &mobitmap->pix16(y);
			UINT16 *pf = &bitmap.pix16(y) + mooffset;
			for (x = rectlist.rect->min_x; x <= rectlist.rect->max_x; x++)
				if (mo[x])
				{
					/* not verified: logic is all controlled in a PAL
					*/
					pf[x] = palbase + mo[x];

					/* erase behind ourselves */
					mo[x] = 0;
				}
		}

	/* add the alpha on top */
	tilemap_t &curalpha = index ? static_cast<tilemap_t &>(m_alpha2_tilemap) : static_cast<tilemap_t &>(m_alpha_tilemap);
	curalpha.draw(bitmap, cliprect, 0, 0);
	return 0;
}


UINT32 cyberbal_state::screen_update_cyberbal_left(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	return update_one_screen(screen, bitmap, cliprect, 0);
}

UINT32 cyberbal_state::screen_update_cyberbal_right(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	return update_one_screen(screen, bitmap, cliprect, 1);
}

UINT32 cyberbal_state::screen_update_cyberbal2p(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	return update_one_screen(screen, bitmap, cliprect, 0);
}
