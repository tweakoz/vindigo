/***************************************************************************

    Uses Data East custom chip 55 for backgrounds, with a special 8bpp mode
    2 times custom chips 52/71 for sprites.

***************************************************************************/

#include "emu.h"
#include "includes/sshangha.h"
#include "video/decospr.h"

/******************************************************************************/

WRITE16_HANDLER (sshangha_video_w)
{
	sshangha_state *state = space->machine->driver_data<sshangha_state>();
	/* 0x4: Special video mode, other bits unknown */
	state->video_control=data;
//  popmessage("%04x",data);
}

/******************************************************************************/

VIDEO_START( sshangha )
{
	machine->device<decospr_device>("spritegen1")->alloc_sprite_bitmap(machine);
	machine->device<decospr_device>("spritegen2")->alloc_sprite_bitmap(machine);
}

/******************************************************************************/

SCREEN_UPDATE( sshangha )
{
	sshangha_state *state = screen->machine->driver_data<sshangha_state>();

	screen->machine->device<decospr_device>("spritegen1")->draw_sprites(screen->machine, bitmap, cliprect, screen->machine->generic.spriteram.u16, 0x800, true);

	// I'm pretty sure only the original has the 2nd spriteram, used for the Japanese text on the 2nd scene (non-scrolling text) in the intro of the quest (3rd in JPN) mode
	if (screen->machine->generic.spriteram2.u16)
		screen->machine->device<decospr_device>("spritegen2")->draw_sprites(screen->machine, bitmap, cliprect, screen->machine->generic.spriteram2.u16, 0x800, true);

	tilemap_set_flip_all(screen->machine,flip_screen_x_get(screen->machine) ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);

	bitmap_fill(bitmap,cliprect,get_black_pen(screen->machine));

	deco16ic_pf_update(state->deco_tilegen1, state->pf1_rowscroll, state->pf2_rowscroll);
	
	/* the tilemap 4bpp + 4bpp = 8bpp mixing actually seems external to the tilemap, note video_control is not part of the tilemap chip */
	if ((state->video_control&4)==0) {
		deco16ic_tilemap_12_combine_draw(state->deco_tilegen1, bitmap, cliprect, 0, 0, 1);
		screen->machine->device<decospr_device>("spritegen1")->inefficient_copy_sprite_bitmap(screen->machine, bitmap, cliprect, 0x0200, 0x0200, 0x100, 0x1ff);
	}
	else {

		deco16ic_tilemap_2_draw(state->deco_tilegen1, bitmap, cliprect, 0, 0);
		screen->machine->device<decospr_device>("spritegen1")->inefficient_copy_sprite_bitmap(screen->machine, bitmap, cliprect, 0x0200, 0x0200, 0x100, 0x1ff);
		deco16ic_tilemap_1_draw(state->deco_tilegen1, bitmap, cliprect, 0, 0);
	}

	if (screen->machine->generic.spriteram2.u16)
		screen->machine->device<decospr_device>("spritegen2")->inefficient_copy_sprite_bitmap(screen->machine, bitmap, cliprect, 0x0000, 0x0000, 0, 0x1ff);

	screen->machine->device<decospr_device>("spritegen1")->inefficient_copy_sprite_bitmap(screen->machine, bitmap, cliprect, 0x0000, 0x0200, 0, 0x1ff);
	return 0;
}
