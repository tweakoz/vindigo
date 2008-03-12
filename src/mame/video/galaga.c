/***************************************************************************

  video.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "driver.h"
#include "includes/galaga.h"


#define MAX_STARS 252
#define STARS_COLOR_BASE (64*4+64*4)

UINT8 *galaga_videoram;
UINT8 *galaga_ram1,*galaga_ram2,*galaga_ram3;
UINT8 galaga_starcontrol[6];
static UINT32 stars_scrollx,stars_scrolly;

static INT32 galaga_gfxbank; // used by catsbee

static tilemap *tx_tilemap;

/*
Galaga star line and pixel locations pulled directly from
a clocked stepping of the 05 starfield. The chip was clocked
on a test rig with hblank and vblank simulated, each X & Y
location of a star being recorded along with it's color value.

Because the starfield begins generating stars at the point
in time it's enabled the exact horiz location of the stars
on Galaga depends on the length of time of the POST for the
original board.

Two control bits determine which of two sets are displayed
set 0 or 1 and simultaneously 2 or 3.

There are 63 stars in each set, 126 displayed at any one time

*/

const struct star star_seed_tab[252]=
{

/* also shared by Bosconian */

/* star set 0 */

{0x0085,0x0006,0x35, 0x00},
{0x008F,0x0008,0x30, 0x00},
{0x00E5,0x001B,0x07, 0x00},
{0x0022,0x001C,0x31, 0x00},
{0x00E5,0x0025,0x1D, 0x00},
{0x0015,0x0026,0x29, 0x00},
{0x0080,0x002D,0x3B, 0x00},
{0x0097,0x002E,0x1C, 0x00},
{0x00BA,0x003B,0x05, 0x00},
{0x0036,0x003D,0x36, 0x00},
{0x0057,0x0044,0x09, 0x00},
{0x00CF,0x0044,0x3D, 0x00},
{0x0061,0x004E,0x27, 0x00},
{0x0087,0x0064,0x1A, 0x00},
{0x00D6,0x0064,0x17, 0x00},
{0x000B,0x006C,0x3C, 0x00},
{0x0006,0x006D,0x24, 0x00},
{0x0018,0x006E,0x3A, 0x00},
{0x00A9,0x0079,0x23, 0x00},
{0x008A,0x007B,0x11, 0x00},
{0x00D6,0x0080,0x0C, 0x00},
{0x0067,0x0082,0x3F, 0x00},
{0x0039,0x0083,0x38, 0x00},
{0x0072,0x0083,0x14, 0x00},
{0x00EC,0x0084,0x16, 0x00},
{0x008E,0x0085,0x10, 0x00},
{0x0020,0x0088,0x25, 0x00},
{0x0095,0x008A,0x0F, 0x00},
{0x000E,0x008D,0x00, 0x00},
{0x0006,0x0091,0x2E, 0x00},
{0x0007,0x0094,0x0D, 0x00},
{0x00AE,0x0097,0x0B, 0x00},
{0x0000,0x0098,0x2D, 0x00},
{0x0086,0x009B,0x01, 0x00},
{0x0058,0x00A1,0x34, 0x00},
{0x00FE,0x00A1,0x3E, 0x00},
{0x00A2,0x00A8,0x1F, 0x00},
{0x0041,0x00AA,0x0A, 0x00},
{0x003F,0x00AC,0x32, 0x00},
{0x00DE,0x00AC,0x03, 0x00},
{0x00D4,0x00B9,0x26, 0x00},
{0x006D,0x00BB,0x1B, 0x00},
{0x0062,0x00BD,0x39, 0x00},
{0x00C9,0x00BE,0x18, 0x00},
{0x006C,0x00C1,0x04, 0x00},
{0x0059,0x00C3,0x21, 0x00},
{0x0060,0x00CC,0x0E, 0x00},
{0x0091,0x00CC,0x12, 0x00},
{0x003F,0x00CF,0x06, 0x00},
{0x00F7,0x00CF,0x22, 0x00},
{0x0044,0x00D0,0x33, 0x00},
{0x0034,0x00D2,0x08, 0x00},
{0x00D3,0x00D9,0x20, 0x00},
{0x0071,0x00DD,0x37, 0x00},
{0x0073,0x00E1,0x2C, 0x00},
{0x00B9,0x00E3,0x2F, 0x00},
{0x00A9,0x00E4,0x13, 0x00},
{0x00D3,0x00E7,0x19, 0x00},
{0x0037,0x00ED,0x02, 0x00},
{0x00BD,0x00F4,0x15, 0x00},
{0x000F,0x00F6,0x28, 0x00},
{0x004F,0x00F7,0x2B, 0x00},
{0x00FB,0x00FF,0x2A, 0x00},

/* star set 1 */

{0x00FE,0x0004,0x3D, 0x01},
{0x00C4,0x0006,0x10, 0x01},
{0x001E,0x0007,0x2D, 0x01},
{0x0083,0x000B,0x1F, 0x01},
{0x002E,0x000D,0x3C, 0x01},
{0x001F,0x000E,0x00, 0x01},
{0x00D8,0x000E,0x2C, 0x01},
{0x0003,0x000F,0x17, 0x01},
{0x0095,0x0011,0x3F, 0x01},
{0x006A,0x0017,0x35, 0x01},
{0x00CC,0x0017,0x02, 0x01},
{0x0000,0x0018,0x32, 0x01},
{0x0092,0x001D,0x36, 0x01},
{0x00E3,0x0021,0x04, 0x01},
{0x002F,0x002D,0x37, 0x01},
{0x00F0,0x002F,0x0C, 0x01},
{0x009B,0x003E,0x06, 0x01},
{0x00A4,0x004C,0x07, 0x01},
{0x00EA,0x004D,0x13, 0x01},
{0x0084,0x004E,0x21, 0x01},
{0x0033,0x0052,0x0F, 0x01},
{0x0070,0x0053,0x0E, 0x01},
{0x0006,0x0059,0x08, 0x01},
{0x0081,0x0060,0x28, 0x01},
{0x0037,0x0061,0x29, 0x01},
{0x008F,0x0067,0x2F, 0x01},
{0x001B,0x006A,0x1D, 0x01},
{0x00BF,0x007C,0x12, 0x01},
{0x0051,0x007F,0x31, 0x01},
{0x0061,0x0086,0x25, 0x01},
{0x006A,0x008F,0x0D, 0x01},
{0x006A,0x0091,0x19, 0x01},
{0x0090,0x0092,0x05, 0x01},
{0x003B,0x0096,0x24, 0x01},
{0x008C,0x0097,0x0A, 0x01},
{0x0006,0x0099,0x03, 0x01},
{0x0038,0x0099,0x38, 0x01},
{0x00A8,0x0099,0x18, 0x01},
{0x0076,0x00A6,0x20, 0x01},
{0x00AD,0x00A6,0x1C, 0x01},
{0x00EC,0x00A6,0x1E, 0x01},
{0x0086,0x00AC,0x15, 0x01},
{0x0078,0x00AF,0x3E, 0x01},
{0x007B,0x00B3,0x09, 0x01},
{0x0027,0x00B8,0x39, 0x01},
{0x0088,0x00C2,0x23, 0x01},
{0x0044,0x00C3,0x3A, 0x01},
{0x00CF,0x00C5,0x34, 0x01},
{0x0035,0x00C9,0x30, 0x01},
{0x006E,0x00D1,0x3B, 0x01},
{0x00D6,0x00D7,0x16, 0x01},
{0x003A,0x00D9,0x2B, 0x01},
{0x00AB,0x00E0,0x11, 0x01},
{0x00E0,0x00E2,0x1B, 0x01},
{0x006F,0x00E6,0x0B, 0x01},
{0x00B8,0x00E8,0x14, 0x01},
{0x00D9,0x00E8,0x1A, 0x01},
{0x00F9,0x00E8,0x22, 0x01},
{0x0004,0x00F1,0x2E, 0x01},
{0x0049,0x00F8,0x26, 0x01},
{0x0010,0x00F9,0x01, 0x01},
{0x0039,0x00FB,0x33, 0x01},
{0x0028,0x00FC,0x27, 0x01},

/* star set 2 */

{0x00FA,0x0006,0x19, 0x02},
{0x00E4,0x0007,0x2D, 0x02},
{0x0072,0x000A,0x03, 0x02},
{0x0084,0x001B,0x00, 0x02},
{0x00BA,0x001D,0x29, 0x02},
{0x00E3,0x0022,0x04, 0x02},
{0x00D1,0x0026,0x2A, 0x02},
{0x0089,0x0032,0x30, 0x02},
{0x005B,0x0036,0x27, 0x02},
{0x0084,0x003A,0x36, 0x02},
{0x0053,0x003F,0x0D, 0x02},
{0x0008,0x0040,0x1D, 0x02},
{0x0055,0x0040,0x1A, 0x02},
{0x00AA,0x0041,0x31, 0x02},
{0x00FB,0x0041,0x2B, 0x02},
{0x00BC,0x0046,0x16, 0x02},
{0x0093,0x0052,0x39, 0x02},
{0x00B9,0x0057,0x10, 0x02},
{0x0054,0x0059,0x28, 0x02},
{0x00E6,0x005A,0x01, 0x02},
{0x00A7,0x005D,0x1B, 0x02},
{0x002D,0x005E,0x35, 0x02},
{0x0014,0x0062,0x21, 0x02},
{0x0069,0x006D,0x1F, 0x02},
{0x00CE,0x006F,0x0B, 0x02},
{0x00DF,0x0075,0x2F, 0x02},
{0x00CB,0x0077,0x12, 0x02},
{0x004E,0x007C,0x23, 0x02},
{0x004A,0x0084,0x0F, 0x02},
{0x0012,0x0086,0x25, 0x02},
{0x0068,0x008C,0x32, 0x02},
{0x0003,0x0095,0x20, 0x02},
{0x000A,0x009C,0x17, 0x02},
{0x005B,0x00A3,0x08, 0x02},
{0x005F,0x00A4,0x3E, 0x02},
{0x0072,0x00A4,0x2E, 0x02},
{0x00CC,0x00A6,0x06, 0x02},
{0x008A,0x00AB,0x0C, 0x02},
{0x00E0,0x00AD,0x26, 0x02},
{0x00F3,0x00AF,0x0A, 0x02},
{0x0075,0x00B4,0x13, 0x02},
{0x0068,0x00B7,0x11, 0x02},
{0x006D,0x00C2,0x2C, 0x02},
{0x0076,0x00C3,0x14, 0x02},
{0x00CF,0x00C4,0x1E, 0x02},
{0x0004,0x00C5,0x1C, 0x02},
{0x0013,0x00C6,0x3F, 0x02},
{0x00B9,0x00C7,0x3C, 0x02},
{0x0005,0x00D7,0x34, 0x02},
{0x0095,0x00D7,0x3A, 0x02},
{0x00FC,0x00D8,0x02, 0x02},
{0x00E7,0x00DC,0x09, 0x02},
{0x001D,0x00E1,0x05, 0x02},
{0x0005,0x00E6,0x33, 0x02},
{0x001C,0x00E9,0x3B, 0x02},
{0x00A2,0x00ED,0x37, 0x02},
{0x0028,0x00EE,0x07, 0x02},
{0x00DD,0x00EF,0x18, 0x02},
{0x006D,0x00F0,0x38, 0x02},
{0x00A1,0x00F2,0x0E, 0x02},
{0x0074,0x00F7,0x3D, 0x02},
{0x0069,0x00F9,0x22, 0x02},
{0x003F,0x00FF,0x24, 0x02},

/* star set 3 */

{0x0071,0x0010,0x34, 0x03},
{0x00AF,0x0011,0x23, 0x03},
{0x00A0,0x0014,0x26, 0x03},
{0x0002,0x0017,0x02, 0x03},
{0x004B,0x0019,0x31, 0x03},
{0x0093,0x001C,0x0E, 0x03},
{0x001B,0x001E,0x25, 0x03},
{0x0032,0x0020,0x2E, 0x03},
{0x00EE,0x0020,0x3A, 0x03},
{0x0079,0x0022,0x2F, 0x03},
{0x006C,0x0023,0x17, 0x03},
{0x00BC,0x0025,0x11, 0x03},
{0x0041,0x0029,0x30, 0x03},
{0x001C,0x002E,0x32, 0x03},
{0x00B9,0x0031,0x01, 0x03},
{0x0083,0x0032,0x05, 0x03},
{0x0095,0x003A,0x12, 0x03},
{0x000D,0x003F,0x07, 0x03},
{0x0020,0x0041,0x33, 0x03},
{0x0092,0x0045,0x2C, 0x03},
{0x00D4,0x0047,0x08, 0x03},
{0x00A1,0x004B,0x2D, 0x03},
{0x00D2,0x004B,0x3B, 0x03},
{0x00D6,0x0052,0x24, 0x03},
{0x009A,0x005F,0x1C, 0x03},
{0x0016,0x0060,0x3D, 0x03},
{0x001A,0x0063,0x1F, 0x03},
{0x00CD,0x0066,0x28, 0x03},
{0x00FF,0x0067,0x10, 0x03},
{0x0035,0x0069,0x20, 0x03},
{0x008F,0x006C,0x04, 0x03},
{0x00CA,0x006C,0x2A, 0x03},
{0x005A,0x0074,0x09, 0x03},
{0x0060,0x0078,0x38, 0x03},
{0x0072,0x0079,0x1E, 0x03},
{0x0037,0x007F,0x29, 0x03},
{0x0012,0x0080,0x14, 0x03},
{0x0029,0x0082,0x2B, 0x03},
{0x0084,0x0098,0x36, 0x03},
{0x0032,0x0099,0x37, 0x03},
{0x00BB,0x00A0,0x19, 0x03},
{0x003E,0x00A3,0x3E, 0x03},
{0x004A,0x00A6,0x1A, 0x03},
{0x0029,0x00A7,0x21, 0x03},
{0x009D,0x00B7,0x22, 0x03},
{0x006C,0x00B9,0x15, 0x03},
{0x000C,0x00C0,0x0A, 0x03},
{0x00C2,0x00C3,0x0F, 0x03},
{0x002F,0x00C9,0x0D, 0x03},
{0x00D2,0x00CE,0x16, 0x03},
{0x00F3,0x00CE,0x0B, 0x03},
{0x0075,0x00CF,0x27, 0x03},
{0x001A,0x00D5,0x35, 0x03},
{0x0026,0x00D6,0x39, 0x03},
{0x0080,0x00DA,0x3C, 0x03},
{0x00A9,0x00DD,0x00, 0x03},
{0x00BC,0x00EB,0x03, 0x03},
{0x0032,0x00EF,0x1B, 0x03},
{0x0067,0x00F0,0x3F, 0x03},
{0x00EF,0x00F1,0x18, 0x03},
{0x00A8,0x00F3,0x0C, 0x03},
{0x00DE,0x00F9,0x1D, 0x03},
{0x002C,0x00FA,0x13, 0x03}
};




/***************************************************************************

  Convert the color PROMs.

  Galaga has one 32x8 palette PROM and two 256x4 color lookup table PROMs
  (one for characters, one for sprites). Only the first 128 bytes of the
  lookup tables seem to be used.
  The palette PROM is connected to the RGB output this way:

  bit 7 -- 220 ohm resistor  -- BLUE
        -- 470 ohm resistor  -- BLUE
        -- 220 ohm resistor  -- GREEN
        -- 470 ohm resistor  -- GREEN
        -- 1  kohm resistor  -- GREEN
        -- 220 ohm resistor  -- RED
        -- 470 ohm resistor  -- RED
  bit 0 -- 1  kohm resistor  -- RED

***************************************************************************/

PALETTE_INIT( galaga )
{
	int i;

	machine->colortable = colortable_alloc(machine, 32+64);

	/* core palette */
	for (i = 0;i < 32;i++)
	{
		int bit0,bit1,bit2,r,g,b;

		bit0 = ((*color_prom) >> 0) & 0x01;
		bit1 = ((*color_prom) >> 1) & 0x01;
		bit2 = ((*color_prom) >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		bit0 = ((*color_prom) >> 3) & 0x01;
		bit1 = ((*color_prom) >> 4) & 0x01;
		bit2 = ((*color_prom) >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;
		bit0 = 0;
		bit1 = ((*color_prom) >> 6) & 0x01;
		bit2 = ((*color_prom) >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		colortable_palette_set_color(machine->colortable,i,MAKE_RGB(r,g,b));
		color_prom++;
	}

	/* palette for the stars */
	for (i = 0;i < 64;i++)
	{
		int bits,r,g,b;
		static const int map[4] = { 0x00, 0x47, 0x97 ,0xde };

		bits = (i >> 0) & 0x03;
		r = map[bits];
		bits = (i >> 2) & 0x03;
		g = map[bits];
		bits = (i >> 4) & 0x03;
		b = map[bits];

		colortable_palette_set_color(machine->colortable,32 + i,MAKE_RGB(r,g,b));
	}

	/* characters */
	for (i = 0;i < 64*4;i++)
		colortable_entry_set_value(machine->colortable, i, (*(color_prom++) & 0x0f) + 0x10);	/* chars */

	/* sprites */
	for (i = 0;i < 64*4;i++)
		colortable_entry_set_value(machine->colortable, 64*4+i, (*(color_prom++) & 0x0f));

	/* now the stars */
	for (i = 0;i < 64;i++)
		colortable_entry_set_value(machine->colortable, 64*4+64*4+i, 32 + i);
}



/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

/* convert from 32x32 to 36x28 */
static TILEMAP_MAPPER( tilemap_scan )
{
	int offs;

	row += 2;
	col -= 2;
	if (col & 0x20)
		offs = row + ((col & 0x1f) << 5);
	else
		offs = col + (row << 5);

	return offs;
}


static TILE_GET_INFO( get_tile_info )
{
	/* the hardware has two character sets, one normal and one x-flipped. When
       screen is flipped, character y flip is done by the hardware inverting the
       timing signals, while x flip is done by selecting the 2nd character set.
       We reproduce this here, but since the tilemap system automatically flips
       characters when screen is flipped, we have to flip them back. */
    int color = galaga_videoram[tile_index + 0x400] & 0x3f;
	SET_TILE_INFO(
			0,
			(galaga_videoram[tile_index] & 0x7f) | (flip_screen_get() ? 0x80 : 0) | (galaga_gfxbank << 8),
			color,
			flip_screen_get() ? TILE_FLIPX : 0);
	tileinfo->group = color;
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

VIDEO_START( galaga )
{
	tx_tilemap = tilemap_create(get_tile_info,tilemap_scan,8,8,36,28);
	colortable_configure_tilemap_groups(machine->colortable, tx_tilemap, machine->gfx[0], 0x1f);

	galaga_gfxbank = 0;

	spriteram   = galaga_ram1 + 0x380;
	spriteram_2 = galaga_ram2 + 0x380;
	spriteram_3 = galaga_ram3 + 0x380;


	state_save_register_global_array(galaga_starcontrol);
	state_save_register_global(stars_scrollx);
	state_save_register_global(stars_scrolly);
	state_save_register_global(galaga_gfxbank);
}



/***************************************************************************

  Memory handlers

***************************************************************************/

READ8_HANDLER( galaga_videoram_r )
{
	return galaga_videoram[offset];
}

WRITE8_HANDLER( galaga_videoram_w )
{
	galaga_videoram[offset] = data;
	tilemap_mark_tile_dirty(tx_tilemap,offset & 0x3ff);
}

WRITE8_HANDLER( galaga_starcontrol_w )
{
	galaga_starcontrol[offset] = data & 1;
}

WRITE8_HANDLER ( gatsbee_bank_w )
{
	galaga_gfxbank = data & 0x1;
	tilemap_mark_all_tiles_dirty(tx_tilemap);
}



/***************************************************************************

  Display refresh

***************************************************************************/

static void draw_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect )
{
	int offs;


	for (offs = 0;offs < 0x80;offs += 2)
	{
		static const int gfx_offs[2][2] =
		{
			{ 0, 1 },
			{ 2, 3 }
		};
		int sprite = spriteram[offs] & 0x7f;
		int color = spriteram[offs+1] & 0x3f;
		int sx = spriteram_2[offs+1] - 40 + 0x100*(spriteram_3[offs+1] & 3);
		int sy = 256 - spriteram_2[offs] + 1;	// sprites are buffered and delayed by one scanline
		int flipx = (spriteram_3[offs] & 0x01);
		int flipy = (spriteram_3[offs] & 0x02) >> 1;
		int sizex = (spriteram_3[offs] & 0x04) >> 2;
		int sizey = (spriteram_3[offs] & 0x08) >> 3;
		int x,y;

		if (flip_screen_get())
		{
			flipx ^= 1;
			flipy ^= 1;
		}

		sy -= 16 * sizey;
		sy = (sy & 0xff) - 32;	// fix wraparound

		for (y = 0;y <= sizey;y++)
		{
			for (x = 0;x <= sizex;x++)
			{
				drawgfx(bitmap,machine->gfx[1],
					sprite + gfx_offs[y ^ (sizey * flipy)][x ^ (sizex * flipx)],
					color,
					flipx,flipy,
					sx + 16*x, sy + 16*y,
					cliprect,TRANSPARENCY_PENS,
					colortable_get_transpen_mask(machine->colortable, machine->gfx[1], color, 0x0f));
			}
		}
	}
}


static void draw_stars(bitmap_t *bitmap, const rectangle *cliprect )
{
	/* draw the stars */

	/* $a005 controls the stars ON/OFF */


	if ( galaga_starcontrol[5] == 1 )
	{
  		int star_cntr;
		int set_a, set_b;

		/* two sets of stars controlled by these bits */

		set_a = galaga_starcontrol[3];
		set_b = galaga_starcontrol[4] | 0x2;


		for (star_cntr = 0;star_cntr < MAX_STARS ;star_cntr++)
		{
			int x,y;

			if   ( (set_a == star_seed_tab[star_cntr].set) ||  ( set_b == star_seed_tab[star_cntr].set) )
			{
				x = (star_seed_tab[star_cntr].x + stars_scrollx) % 256 + 16;
				y = (112 + star_seed_tab[star_cntr].y + stars_scrolly) % 256;
			   /* 112 is a tweak to get alignment about perfect */

				if (y >= cliprect->min_y && y <= cliprect->max_y)
					*BITMAP_ADDR16(bitmap, y, x) = STARS_COLOR_BASE + star_seed_tab[ star_cntr ].col;
			}

		}
	}
}

VIDEO_UPDATE( galaga )
{
	fillbitmap(bitmap,get_black_pen(screen->machine),cliprect);
	draw_stars(bitmap,cliprect);
	draw_sprites(screen->machine,bitmap,cliprect);
	tilemap_draw(bitmap,cliprect,tx_tilemap,0,0);
	return 0;
}



VIDEO_EOF( galaga )
{
	/* this function is called by galaga_interrupt_1() */
	int s0,s1,s2;
	static const int speeds[8] = { -1, -2, -3, 0, 3, 2, 1, 0 };


	s0 = galaga_starcontrol[0];
	s1 = galaga_starcontrol[1];
	s2 = galaga_starcontrol[2];

	stars_scrollx += speeds[s0 + s1*2 + s2*4];
}


