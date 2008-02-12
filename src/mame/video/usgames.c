#include "driver.h"
#include "video/crtc6845.h"
#include "deprecat.h"

UINT8 *usgames_videoram,*usgames_charram;


static crtc6845_t *crtc6845;
static tilemap *usgames_tilemap;



PALETTE_INIT(usgames)
{
	int j;

	for (j = 0;j < 16;j++)
	{
		int r = (j & 1) >> 0;
		int g = (j & 2) >> 1;
		int b = (j & 4) >> 2;
		int i = (j & 8) >> 3;

		r = 0xff * r;
		g = 0x7f * g * (i + 1);
		b = 0x7f * b * (i + 1);

		palette_set_color(machine,j,MAKE_RGB(r,g,b));
	}

	for (j = 0;j < 256;j++)
	{
		colortable[2*j] = j & 0x0f;
		colortable[2*j+1] = j >> 4;
	}
}



static TILE_GET_INFO( get_usgames_tile_info )
{
	int tileno, colour;

	tileno = usgames_videoram[tile_index*2];
	colour = usgames_videoram[tile_index*2+1];

	SET_TILE_INFO(0,tileno,colour,0);
}

VIDEO_START(usgames)
{
	crtc6845 = crtc6845_config(NULL);
	usgames_tilemap = tilemap_create(get_usgames_tile_info,tilemap_scan_rows, 8, 8,64,32);
}


WRITE8_HANDLER( usgames_videoram_w )
{
	usgames_videoram[offset] = data;
	tilemap_mark_tile_dirty(usgames_tilemap,offset/2);
}

WRITE8_HANDLER( usgames_charram_w )
{
	usgames_charram[offset] = data;

	decodechar(Machine->gfx[0], offset/8, usgames_charram);

	tilemap_mark_all_tiles_dirty(usgames_tilemap);
}


WRITE8_HANDLER( usgames_crtc6845_address_w )
{
	crtc6845_address_w(crtc6845, data);
}


WRITE8_HANDLER( usgames_crtc6845_register_w )
{
	crtc6845_register_w(crtc6845, data);
}


VIDEO_UPDATE(usgames)
{
	tilemap_draw(bitmap,cliprect,usgames_tilemap,0,0);
	return 0;
}
