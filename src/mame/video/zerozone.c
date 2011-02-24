/***************************************************************************

  video/zerozone.c

***************************************************************************/

#include "emu.h"
#include "includes/zerozone.h"

WRITE16_HANDLER( zerozone_tilemap_w )
{
	zerozone_state *state = space->machine->driver_data<zerozone_state>();

	COMBINE_DATA(&state->videoram[offset]);
	tilemap_mark_tile_dirty(state->zz_tilemap,offset);
}


WRITE16_HANDLER(zerozone_tilebank_w)
{
	zerozone_state *state = space->machine->driver_data<zerozone_state>();

//  popmessage ("Data %04x",data);
	state->tilebank = data & 0x07;
	tilemap_mark_all_tiles_dirty(state->zz_tilemap);
}

static TILE_GET_INFO( get_zerozone_tile_info )
{
	zerozone_state *state = machine->driver_data<zerozone_state>();
	int tileno = state->videoram[tile_index] & 0x07ff;
	int colour = state->videoram[tile_index] & 0xf000;

	if (state->videoram[tile_index] & 0x0800)
		tileno += state->tilebank * 0x800;

	SET_TILE_INFO(0, tileno, colour >> 12, 0);
}

VIDEO_START( zerozone )
{
	zerozone_state *state = machine->driver_data<zerozone_state>();

	// i'm not 100% sure it should be opaque, pink title screen looks strange in las vegas girls
	// but if its transparent other things look incorrect
	state->zz_tilemap = tilemap_create(machine, get_zerozone_tile_info, tilemap_scan_cols, 8, 8, 64, 32);
}

SCREEN_UPDATE( zerozone )
{
	zerozone_state *state = screen->machine->driver_data<zerozone_state>();

	tilemap_draw(bitmap, cliprect, state->zz_tilemap, 0, 0);
	return 0;
}
