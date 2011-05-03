/***************************************************************************

  video.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "emu.h"
#include "includes/tnzs.h"
#include "video/seta001.h"

/***************************************************************************

  The New Zealand Story doesn't have a color PROM. It uses 1024 bytes of RAM
  to dynamically create the palette. Each couple of bytes defines one
  color (15 bits per pixel; the top bit of the second byte is unused).
  Since the graphics use 4 bitplanes, hence 16 colors, this makes for 32
  different color codes.

***************************************************************************/


/***************************************************************************

  Convert the color PROMs into a more useable format.

  Arkanoid has a two 512x8 palette PROMs. The two bytes joined together
  form 512 xRRRRRGGGGGBBBBB color values.

***************************************************************************/

PALETTE_INIT( arknoid2 )
{
	int i, col;

	for (i = 0; i < machine.total_colors(); i++)
	{
		col = (color_prom[i] << 8) + color_prom[i + 512];
		palette_set_color_rgb(machine, i, pal5bit(col >> 10), pal5bit(col >> 5), pal5bit(col >> 0));
	}
}


SCREEN_UPDATE( tnzs )
{
	tnzs_state *state = screen->machine().driver_data<tnzs_state>();
	screen->machine().device<seta001_device>("spritegen")->tnzs_draw_sprites(screen->machine(), bitmap, cliprect, state->m_objram, state->m_bg_flag );
	return 0;
}

SCREEN_EOF( tnzs )
{
	tnzs_state *state = machine.driver_data<tnzs_state>();
	machine.device<seta001_device>("spritegen")->tnzs_eof( state->m_objram);
}
