/***************************************************************************

    Atari Toobin' hardware

****************************************************************************/

#include "driver.h"
#include "machine/atarigen.h"
#include "toobin.h"



/*************************************
 *
 *  Globals we own
 *
 *************************************/

static double brightness;
static bitmap_t *pfbitmap;



/*************************************
 *
 *  Tilemap callbacks
 *
 *************************************/

static TILE_GET_INFO( get_alpha_tile_info )
{
	UINT16 data = atarigen_alpha[tile_index];
	int code = data & 0x3ff;
	int color = (data >> 12) & 0x0f;
	SET_TILE_INFO(2, code, color, (data >> 10) & 1);
}


static TILE_GET_INFO( get_playfield_tile_info )
{
	UINT16 data1 = atarigen_playfield[tile_index * 2];
	UINT16 data2 = atarigen_playfield[tile_index * 2 + 1];
	int code = data2 & 0x3fff;
	int color = data1 & 0x0f;
	SET_TILE_INFO(0, code, color, TILE_FLIPYX(data2 >> 14));
	tileinfo->category = (data1 >> 4) & 3;
}



/*************************************
 *
 *  Video system start
 *
 *************************************/

VIDEO_START( toobin )
{
	static const atarimo_desc modesc =
	{
		1,					/* index to which gfx system */
		1,					/* number of motion object banks */
		1,					/* are the entries linked? */
		0,					/* are the entries split? */
		0,					/* render in reverse order? */
		1,					/* render in swapped X/Y order? */
		0,					/* does the neighbor bit affect the next object? */
		1024,				/* pixels per SLIP entry (0 for no-slip) */
		0,					/* pixel offset for SLIPs */
		0,					/* maximum number of links to visit/scanline (0=all) */

		0x100,				/* base palette entry */
		0x100,				/* maximum number of colors */
		0,					/* transparent pen index */

		{{ 0,0,0x00ff,0 }},	/* mask for the link */
		{{ 0 }},			/* mask for the graphics bank */
		{{ 0,0x3fff,0,0 }},	/* mask for the code index */
		{{ 0 }},			/* mask for the upper code index */
		{{ 0,0,0,0x000f }},	/* mask for the color */
		{{ 0,0,0,0xffc0 }},	/* mask for the X position */
		{{ 0x7fc0,0,0,0 }},	/* mask for the Y position */
		{{ 0x0007,0,0,0 }},	/* mask for the width, in tiles*/
		{{ 0x0038,0,0,0 }},	/* mask for the height, in tiles */
		{{ 0,0x4000,0,0 }},	/* mask for the horizontal flip */
		{{ 0,0x8000,0,0 }},	/* mask for the vertical flip */
		{{ 0 }},			/* mask for the priority */
		{{ 0 }},			/* mask for the neighbor */
		{{ 0x8000,0,0,0 }},	/* mask for absolute coordinates */

		{{ 0 }},			/* mask for the special value */
		0,					/* resulting value to indicate "special" */
		0					/* callback routine for special entries */
	};

	/* initialize the playfield */
	atarigen_playfield_tilemap = tilemap_create(get_playfield_tile_info, tilemap_scan_rows,  8,8, 128,64);

	/* initialize the motion objects */
	atarimo_init(machine, 0, &modesc);

	/* initialize the alphanumerics */
	atarigen_alpha_tilemap = tilemap_create(get_alpha_tile_info, tilemap_scan_rows,  8,8, 64,48);
	tilemap_set_transparent_pen(atarigen_alpha_tilemap, 0);

	/* allocate a playfield bitmap for rendering */
	pfbitmap = auto_bitmap_alloc(video_screen_get_width(machine->primary_screen), video_screen_get_height(machine->primary_screen), BITMAP_FORMAT_INDEXED16);
}



/*************************************
 *
 *  Palette RAM write handler
 *
 *************************************/

WRITE16_HANDLER( toobin_paletteram_w )
{
	int newword;

	COMBINE_DATA(&paletteram16[offset]);
	newword = paletteram16[offset];

	{
		int red =   (((newword >> 10) & 31) * 224) >> 5;
		int green = (((newword >>  5) & 31) * 224) >> 5;
		int blue =  (((newword      ) & 31) * 224) >> 5;

		if (red) red += 38;
		if (green) green += 38;
		if (blue) blue += 38;

		palette_set_color(space->machine, offset & 0x3ff, MAKE_RGB(red, green, blue));
		if (!(newword & 0x8000))
			palette_set_pen_contrast(space->machine, offset & 0x3ff, brightness);
		else
			palette_set_pen_contrast(space->machine, offset & 0x3ff, 1.0);
	}
}


WRITE16_HANDLER( toobin_intensity_w )
{
	int i;

	if (ACCESSING_BITS_0_7)
	{
		brightness = (double)(~data & 0x1f) / 31.0;

		for (i = 0; i < 0x400; i++)
			if (!(paletteram16[i] & 0x8000))
				palette_set_pen_contrast(space->machine, i, brightness);
	}
}



/*************************************
 *
 *  X/Y scroll handlers
 *
 *************************************/

WRITE16_HANDLER( toobin_xscroll_w )
{
	UINT16 oldscroll = *atarigen_xscroll;
	UINT16 newscroll = oldscroll;
	COMBINE_DATA(&newscroll);

	/* if anything has changed, force a partial update */
	if (newscroll != oldscroll)
		video_screen_update_partial(space->machine->primary_screen, video_screen_get_vpos(space->machine->primary_screen));

	/* update the playfield scrolling - hscroll is clocked on the following scanline */
	tilemap_set_scrollx(atarigen_playfield_tilemap, 0, newscroll >> 6);
	atarimo_set_xscroll(0, newscroll >> 6);

	/* update the data */
	*atarigen_xscroll = newscroll;
}


WRITE16_HANDLER( toobin_yscroll_w )
{
	UINT16 oldscroll = *atarigen_yscroll;
	UINT16 newscroll = oldscroll;
	COMBINE_DATA(&newscroll);

	/* if anything has changed, force a partial update */
	if (newscroll != oldscroll)
		video_screen_update_partial(space->machine->primary_screen, video_screen_get_vpos(space->machine->primary_screen));

	/* if bit 4 is zero, the scroll value is clocked in right away */
	tilemap_set_scrolly(atarigen_playfield_tilemap, 0, newscroll >> 6);
	atarimo_set_yscroll(0, (newscroll >> 6) & 0x1ff);

	/* update the data */
	*atarigen_yscroll = newscroll;
}



/*************************************
 *
 *  X/Y scroll handlers
 *
 *************************************/

WRITE16_HANDLER( toobin_slip_w )
{
	int oldslip = atarimo_0_slipram[offset];
	int newslip = oldslip;
	COMBINE_DATA(&newslip);

	/* if the SLIP is changing, force a partial update first */
	if (oldslip != newslip)
		video_screen_update_partial(space->machine->primary_screen, video_screen_get_vpos(space->machine->primary_screen));

	/* update the data */
	atarimo_0_slipram_w(space, offset, data, mem_mask);
}



/*************************************
 *
 *  Main refresh
 *
 *************************************/

VIDEO_UPDATE( toobin )
{
	const rgb_t *palette = palette_entry_list_adjusted(screen->machine->palette);
	atarimo_rect_list rectlist;
	bitmap_t *mobitmap;
	int x, y;

	/* draw the playfield */
	fillbitmap(priority_bitmap, 0, cliprect);
	tilemap_draw(pfbitmap, cliprect, atarigen_playfield_tilemap, 0, 0);
	tilemap_draw(pfbitmap, cliprect, atarigen_playfield_tilemap, 1, 1);
	tilemap_draw(pfbitmap, cliprect, atarigen_playfield_tilemap, 2, 2);
	tilemap_draw(pfbitmap, cliprect, atarigen_playfield_tilemap, 3, 3);

	/* draw and merge the MO */
	mobitmap = atarimo_render(0, cliprect, &rectlist);
	for (y = cliprect->min_y; y <= cliprect->max_y; y++)
	{
		UINT32 *dest = BITMAP_ADDR32(bitmap, y, 0);
		UINT16 *mo = BITMAP_ADDR16(mobitmap, y, 0);
		UINT16 *pf = BITMAP_ADDR16(pfbitmap, y, 0);
		UINT8 *pri = BITMAP_ADDR8(priority_bitmap, y, 0);
		for (x = cliprect->min_x; x <= cliprect->max_x; x++)
		{
			UINT16 pix = pf[x];
			if (mo[x])
			{
				/* not verified: logic is all controlled in a PAL

                   factors: LBPRI1-0, LBPIX3, ANPIX1-0, PFPIX3, PFPRI1-0,
                            (~LBPIX3 & ~LBPIX2 & ~LBPIX1 & ~LBPIX0)
               */

				/* only draw if not high priority PF */
				if (!pri[x] || !(pix & 8))
					pix = mo[x];

				/* erase behind ourselves */
				mo[x] = 0;
			}
			dest[x] = palette[pix];
		}
	}

	/* add the alpha on top */
	tilemap_draw(bitmap, cliprect, atarigen_alpha_tilemap, 0, 0);
	return 0;
}
