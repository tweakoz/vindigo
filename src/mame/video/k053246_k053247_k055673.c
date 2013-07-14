/***************************************************************************/
/*                                                                         */
/*                                 053246                                  */
/*                          with 053247 or 055673                          */
/*  is the 053247 / 055673 choice just a BPP change like the tilemaps?     */
/*                                                                         */
/*                                                                         */
/***************************************************************************/
/* later Konami GX board replaces the 053246 with a 058142 */

/*

053247/053246
-------------
Sprite generators. Nothing is known about their external interface.
The sprite RAM format is very similar to the 053245.

053246 memory map (but the 053247 sees and processes them too):
000-001 W  global X offset
002-003 W  global Y offset
004     W  low 8 bits of the ROM address to read
005     W  bit 0 = flip screen X
           bit 1 = flip screen Y
           bit 2 = unknown
           bit 4 = interrupt enable
           bit 5 = unknown
006-007 W  high 16 bits of the ROM address to read

???-??? R  reads data from the gfx ROMs (16 bits in total). The address of the
           data is determined by the registers above


*/

#include "emu.h"
#include "k053246_k053247_k055673.h"
#include "konami_helper.h"
#include "devlegcy.h"

#define VERBOSE 0
#define LOG(x) do { if (VERBOSE) logerror x; } while (0)









/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/


void k053247_device::clear_all()
{
	m_ram = 0;
	m_gfx = 0;

	for (int i=0;i<8;i++)
		m_kx46_regs[i] = 0;

	for (int i=0;i<16;i++)
		m_kx47_regs[i] = 0;

	m_dx = m_dy = m_wraparound = 0;
	m_objcha_line = 0;
	m_z_rejection = 0;

	m_callback = 0;

	m_memory_region = 0;
	m_screen = 0;

	m_intf_screen = 0;
	m_intf_gfx_memory_region = 0;
	m_intf_gfx_num = -1;
	m_intf_plane_order = 0;
	m_intf_dx = m_intf_dy = 0;
	m_intf_deinterleave = 0;
	m_intf_callback = 0;
}

void k053247_device::k053247_get_ram( UINT16 **ram )
{
	*ram = m_ram;
}

int k053247_device::k053247_get_dx( void )
{
	return m_dx;
}

int k053247_device::k053247_get_dy( void )
{
	return m_dy;
}

int k053247_device::k053246_read_register( int regnum )
{
	return(m_kx46_regs[regnum]);
}

int k053247_device::k053247_read_register( int regnum )
{
	return(m_kx47_regs[regnum]);
}

void k053247_device::k053247_set_sprite_offs( int offsx, int offsy )
{
	m_dx = offsx;
	m_dy = offsy;
}

void k053247_device::k053247_wraparound_enable( int status )
{
	m_wraparound = status;
}

WRITE16_MEMBER( k053247_device::k053247_reg_word_w ) // write-only OBJSET2 registers (see p.43 table 6.1)
{
	COMBINE_DATA(m_kx47_regs + offset);
}

WRITE32_MEMBER( k053247_device::k053247_reg_long_w )
{
	offset <<= 1;
	COMBINE_DATA(m_kx47_regs + offset + 1);
	mem_mask >>= 16;
	data >>= 16;
	COMBINE_DATA(m_kx47_regs + offset);
}

READ16_MEMBER( k053247_device::k053247_word_r )
{
	return m_ram[offset];
}

WRITE16_MEMBER( k053247_device::k053247_word_w )
{
	COMBINE_DATA(m_ram + offset);
}

READ32_MEMBER( k053247_device::k053247_long_r )
{
	return m_ram[offset * 2 + 1] | (m_ram[offset * 2] << 16);
}

WRITE32_MEMBER( k053247_device::k053247_long_w )
{
	offset <<= 1;
	COMBINE_DATA(m_ram + offset + 1);
	mem_mask >>= 16;
	data >>= 16;
	COMBINE_DATA(m_ram + offset);
}

READ8_MEMBER( k053247_device::k053247_r )
{
	int offs = offset >> 1;

	if (offset & 1)
		return(m_ram[offs] & 0xff);
	else
		return(m_ram[offs] >> 8);
}

WRITE8_MEMBER( k053247_device::k053247_w )
{
	int offs = offset >> 1;

	if (offset & 1)
		m_ram[offs] = (m_ram[offs] & 0xff00) | data;
	else
		m_ram[offs] = (m_ram[offs] & 0x00ff) | (data << 8);
}

// Mystic Warriors hardware games support a non-objcha based ROM readback
// write the address to the 246 as usual, but there's a completely separate ROM
// window that works without needing an objcha line.
// in this window, +0 = 32 bits from one set of ROMs, and +8 = 32 bits from another set
READ16_MEMBER( k053247_device::k055673_rom_word_r ) // 5bpp
{
	UINT8 *ROM8 = (UINT8 *)space.machine().root_device().memregion(m_memory_region)->base();
	UINT16 *ROM = (UINT16 *)space.machine().root_device().memregion(m_memory_region)->base();
	int size4 = (space.machine().root_device().memregion(m_memory_region)->bytes() / (1024 * 1024)) / 5;
	int romofs;

	size4 *= 4 * 1024 * 1024;   // get offset to 5th bit
	ROM8 += size4;

	romofs = m_kx46_regs[6] << 16 | m_kx46_regs[7] << 8 | m_kx46_regs[4];

	switch (offset)
	{
		case 0: // 20k / 36u
			return ROM[romofs + 2];
		case 1: // 17k / 36y
			return ROM[romofs + 3];
		case 2: // 10k / 32y
		case 3:
			romofs /= 2;
			return ROM8[romofs + 1];
		case 4: // 22k / 34u
			return ROM[romofs];
		case 5: // 19k / 34y
			return ROM[romofs + 1];
		case 6: // 12k / 29y
		case 7:
			romofs /= 2;
			return ROM8[romofs];
		default:
			LOG(("55673_rom_word_r: Unknown read offset %x\n", offset));
			break;
	}

	return 0;
}

READ16_MEMBER( k053247_device::k055673_GX6bpp_rom_word_r )
{
	UINT16 *ROM = (UINT16 *)space.machine().root_device().memregion(m_memory_region)->base();
	int romofs;

	romofs = m_kx46_regs[6] << 16 | m_kx46_regs[7] << 8 | m_kx46_regs[4];

	romofs /= 4;    // romofs increments 4 at a time
	romofs *= 12 / 2;   // each increment of romofs = 12 new bytes (6 new words)

	switch (offset)
	{
		case 0:
			return ROM[romofs + 3];
		case 1:
			return ROM[romofs + 4];
		case 2:
		case 3:
			return ROM[romofs + 5];
		case 4:
			return ROM[romofs];
		case 5:
			return ROM[romofs + 1];
		case 6:
		case 7:
			return ROM[romofs + 2];
		default:
//          LOG(("55673_rom_word_r: Unknown read offset %x (PC=%x)\n", offset, space.device().safe_pc()));
			break;
	}

	return 0;
}

READ8_MEMBER( k053247_device::k053246_r )
{
	if (m_objcha_line == ASSERT_LINE)
	{
		int addr;

		addr = (m_kx46_regs[6] << 17) | (m_kx46_regs[7] << 9) | (m_kx46_regs[4] << 1) | ((offset & 1) ^ 1);
		addr &= space.machine().root_device().memregion(m_memory_region)->bytes() - 1;
//      if (VERBOSE)
//          popmessage("%04x: offset %02x addr %06x", space.device().safe_pc(), offset, addr);
		return space.machine().root_device().memregion(m_memory_region)->base()[addr];
	}
	else
	{
//      LOG(("%04x: read from unknown 053246 address %x\n", space.device().safe_pc(), offset));
		return 0;
	}
}

WRITE8_MEMBER( k053247_device::k053246_w )
{
	m_kx46_regs[offset] = data;
}

READ16_MEMBER( k053247_device::k053246_word_r )
{
	offset <<= 1;
	return k053246_r( space, offset + 1) | (k053246_r( space, offset) << 8);
}

WRITE16_MEMBER( k053247_device::k053246_word_w )
{
	if (ACCESSING_BITS_8_15)
		k053246_w( space, offset << 1,(data >> 8) & 0xff);
	if (ACCESSING_BITS_0_7)
		k053246_w( space, (offset << 1) + 1,data & 0xff);
}

READ32_MEMBER( k053247_device::k053246_long_r )
{
	offset <<= 1;
	return (k053246_word_r( space, offset + 1, 0xffff) | k053246_word_r( space, offset, 0xffff) << 16);
}

WRITE32_MEMBER( k053247_device::k053246_long_w )
{
	offset <<= 1;
	k053246_word_w( space, offset, data >> 16, mem_mask >> 16);
	k053246_word_w( space, offset + 1, data, mem_mask);
}

void k053247_device::k053246_set_objcha_line( int state )
{
	m_objcha_line = state;
}

int k053247_device::k053246_is_irq_enabled( void )
{
	// This bit enables obj DMA rather than obj IRQ even though the two functions usually coincide.
	return m_kx46_regs[5] & 0x10;
}

/*
 * Sprite Format
 * ------------------
 *
 * Word | Bit(s)           | Use
 * -----+-fedcba9876543210-+----------------
 *   0  | x--------------- | active (show this sprite)
 *   0  | -x-------------- | maintain aspect ratio (when set, zoom y acts on both axis)
 *   0  | --x------------- | flip y
 *   0  | ---x------------ | flip x
 *   0  | ----xxxx-------- | sprite size (see below)
 *   0  | --------xxxxxxxx | zcode
 *   1  | xxxxxxxxxxxxxxxx | sprite code
 *   2  | ------xxxxxxxxxx | y position
 *   3  | ------xxxxxxxxxx | x position
 *   4  | xxxxxxxxxxxxxxxx | zoom y (0x40 = normal, <0x40 = enlarge, >0x40 = reduce)
 *   5  | xxxxxxxxxxxxxxxx | zoom x (0x40 = normal, <0x40 = enlarge, >0x40 = reduce)
 *   6  | x--------------- | mirror y (top half is drawn as mirror image of the bottom)
 *   6  | -x-------------- | mirror x (right half is drawn as mirror image of the left)
 *   6  | --xx------------ | reserved (sprites with these two bits set don't seem to be graphics data at all)
 *   6  | ----xx---------- | shadow code: 0=off, 0x400=preset1, 0x800=preset2, 0xc00=preset3
 *   6  | ------xx-------- | effect code: flicker, upper palette, full shadow...etc. (game dependent)
 *   6  | --------xxxxxxxx | "color", but depends on external connections (implies priority)
 *   7  | xxxxxxxxxxxxxxxx | game dependent
 *
 * shadow enables transparent shadows. Note that it applies to the last sprite pen ONLY.
 * The rest of the sprite remains normal.
 */

template<class _BitmapClass>
void k053247_device::k053247_sprites_draw_common( _BitmapClass &bitmap, const rectangle &cliprect )
{
#define NUM_SPRITES 256

	/* sprites can be grouped up to 8x8. The draw order is
	     0  1  4  5 16 17 20 21
	     2  3  6  7 18 19 22 23
	     8  9 12 13 24 25 28 29
	    10 11 14 15 26 27 30 31
	    32 33 36 37 48 49 52 53
	    34 35 38 39 50 51 54 55
	    40 41 44 45 56 57 60 61
	    42 43 46 47 58 59 62 63
	*/
	static const int xoffset[8] = { 0, 1, 4, 5, 16, 17, 20, 21 };
	static const int yoffset[8] = { 0, 2, 8, 10, 32, 34, 40, 42 };

	int sortedlist[NUM_SPRITES];
	int offs,zcode;
	int ox, oy, color, code, size, w, h, x, y, xa, ya, flipx, flipy, mirrorx, mirrory, shadow, zoomx, zoomy, primask;
	int shdmask, nozoom, count, temp;

	int flipscreenx = m_kx46_regs[5] & 0x01;
	int flipscreeny = m_kx46_regs[5] & 0x02;
	int offx = (short)((m_kx46_regs[0] << 8) | m_kx46_regs[1]);
	int offy = (short)((m_kx46_regs[2] << 8) | m_kx46_regs[3]);

	int screen_width = m_screen->width();
	UINT8 drawmode_table[256];
	UINT8 shadowmode_table[256];
	UINT8 *whichtable;

	memset(drawmode_table, DRAWMODE_SOURCE, sizeof(drawmode_table));
	drawmode_table[0] = DRAWMODE_NONE;
	memset(shadowmode_table, DRAWMODE_SHADOW, sizeof(shadowmode_table));
	shadowmode_table[0] = DRAWMODE_NONE;

	/*
	    safeguard older drivers missing any of the following video attributes:

	    VIDEO_HAS_SHADOWS | VIDEO_HAS_HIGHLIGHTS
	*/
	if (machine().config().m_video_attributes & VIDEO_HAS_SHADOWS)
	{
		if (sizeof(typename _BitmapClass::pixel_t) == 4 && (machine().config().m_video_attributes & VIDEO_HAS_HIGHLIGHTS))
			shdmask = 3; // enable all shadows and highlights
		else
			shdmask = 0; // enable default shadows
	}
	else
		shdmask = -1; // disable everything

	/*
	    The k053247 does not draw pixels on top of those with equal or smaller Z-values
	    regardless of priority. Embedded shadows inherit Z-values from their host sprites
	    but do not assume host priorities unless explicitly told. In other words shadows
	    can have priorities different from that of normal pens in the same sprite,
	    in addition to the ability of masking themselves from specific layers or pixels
	    on the other sprites.

	    In front-to-back rendering, sprites cannot sandwich between alpha blended layers
	    or the draw code will have to figure out the percentage opacities of what is on
	    top and beneath each sprite pixel and blend the target accordingly. The process
	    is overly demanding for realtime software and is thus another shortcoming of
	    pdrawgfx and pixel based mixers. Even mahjong games with straight forward video
	    subsystems are feeling the impact by which the girls cannot appear under
	    translucent dialogue boxes.

	    These are a small part of the k053247's feature set but many games expect them
	    to be the minimum compliances. The specification will undoubtedly require
	    redesigning the priority system from the ground up. Drawgfx.c and tilemap.c must
	    also undergo heavy facelifts but in the end the changes could hurt simpler games
	    more than they help complex systems; therefore the new engine should remain
	    completely stand alone and self-contained. Implementation details are being
	    hammered down but too early to make propositions.
	*/

	// Prebuild a sorted table by descending Z-order.
	zcode = m_z_rejection;
	offs = count = 0;

	if (zcode == -1)
	{
		for (; offs < 0x800; offs += 8)
			if (m_ram[offs] & 0x8000)
				sortedlist[count++] = offs;
	}
	else
	{
		for (; offs < 0x800; offs += 8)
			if ((m_ram[offs] & 0x8000) && ((m_ram[offs] & 0xff) != zcode))
				sortedlist[count++] = offs;
	}

	w = count;
	count--;
	h = count;

	if (!(m_kx47_regs[0xc / 2] & 0x10))
	{
		// sort objects in decending order(smaller z closer) when OPSET PRI is clear
		for (y = 0; y < h; y++)
		{
			offs = sortedlist[y];
			zcode = m_ram[offs] & 0xff;
			for (x = y + 1; x < w; x++)
			{
				temp = sortedlist[x];
				code = m_ram[temp] & 0xff;
				if (zcode <= code)
				{
					zcode = code;
					sortedlist[x] = offs;
					sortedlist[y] = offs = temp;
				}
			}
		}
	}
	else
	{
		// sort objects in ascending order(bigger z closer) when OPSET PRI is set
		for (y = 0; y < h; y++)
		{
			offs = sortedlist[y];
			zcode = m_ram[offs] & 0xff;
			for (x = y + 1; x < w; x++)
			{
				temp = sortedlist[x];
				code = m_ram[temp] & 0xff;
				if (zcode >= code)
				{
					zcode = code;
					sortedlist[x] = offs;
					sortedlist[y] = offs = temp;
				}
			}
		}
	}

	for (; count >= 0; count--)
	{
		offs = sortedlist[count];

		code = m_ram[offs + 1];
		shadow = color = m_ram[offs + 6];
		primask = 0;

		m_callback(machine(), &code, &color, &primask);

		temp = m_ram[offs];

		size = (temp & 0x0f00) >> 8;
		w = 1 << (size & 0x03);
		h = 1 << ((size >> 2) & 0x03);

		/* the sprite can start at any point in the 8x8 grid. We have to */
		/* adjust the offsets to draw it correctly. Simpsons does this all the time. */
		xa = 0;
		ya = 0;
		if (code & 0x01) xa += 1;
		if (code & 0x02) ya += 1;
		if (code & 0x04) xa += 2;
		if (code & 0x08) ya += 2;
		if (code & 0x10) xa += 4;
		if (code & 0x20) ya += 4;
		code &= ~0x3f;

		oy = (short)m_ram[offs + 2];
		ox = (short)m_ram[offs + 3];

		if (m_wraparound)
		{
			offx &= 0x3ff;
			offy &= 0x3ff;
			oy &= 0x3ff;
			ox &= 0x3ff;
		}

		/* zoom control:
		   0x40 = normal scale
		  <0x40 enlarge (0x20 = double size)
		  >0x40 reduce (0x80 = half size)
		*/
		y = zoomy = m_ram[offs + 4] & 0x3ff;
		if (zoomy)
			zoomy = (0x400000 + (zoomy >> 1)) / zoomy;
		else
			zoomy = 0x800000;

		if (!(temp & 0x4000))
		{
			x = zoomx = m_ram[offs + 5] & 0x3ff;
			if (zoomx)
				zoomx = (0x400000 + (zoomx >> 1)) / zoomx;
			else
				zoomx = 0x800000;
		}
		else
		{
			zoomx = zoomy;
			x = y;
		}

// ************************************************************************************
//  for Escape Kids (GX975)
// ************************************************************************************
//    Escape Kids use 053246 #5 register's UNKNOWN Bit #5, #3 and #2.
//    Bit #5, #3, #2 always set "1".
//    Maybe, Bit #5 or #3 or #2 or combination means "FIX SPRITE WIDTH TO HALF" ?????
//    Below 7 lines supports this 053246's(???) function.
//    Don't rely on it, Please.  But, Escape Kids works correctly!
// ************************************************************************************
		if ( m_kx46_regs[5] & 0x08 ) // Check only "Bit #3 is '1'?" (NOTE: good guess)
		{
			zoomx >>= 1;        // Fix sprite width to HALF size
			ox = (ox >> 1) + 1; // Fix sprite draw position
			if (flipscreenx)
				ox += screen_width;
			nozoom = 0;
		}
		else
			nozoom = (x == 0x40 && y == 0x40);

		flipx = temp & 0x1000;
		flipy = temp & 0x2000;
		mirrorx = shadow & 0x4000;
		if (mirrorx)
			flipx = 0; // documented and confirmed
		mirrory = shadow & 0x8000;

		whichtable = drawmode_table;
		if (color == -1)
		{
			// drop the entire sprite to shadow unconditionally
			if (shdmask < 0) continue;
			color = 0;
			shadow = -1;
			whichtable = shadowmode_table;
			palette_set_shadow_mode(machine(), 0);
		}
		else
		{
			if (shdmask >= 0)
			{
				shadow = (color & K053247_CUSTOMSHADOW) ? (color >> K053247_SHDSHIFT) : (shadow >> 10);
				if (shadow &= 3) palette_set_shadow_mode(machine(), (shadow - 1) & shdmask);
			}
			else
				shadow = 0;
		}

		color &= 0xffff; // strip attribute flags

		if (flipscreenx)
		{
			ox = -ox;
			if (!mirrorx) flipx = !flipx;
		}
		if (flipscreeny)
		{
			oy = -oy;
			if (!mirrory) flipy = !flipy;
		}

		// apply wrapping and global offsets
		if (m_wraparound)
		{
			ox = ( ox - offx) & 0x3ff;
			oy = (-oy - offy) & 0x3ff;
			if (ox >= 0x300) ox -= 0x400;
			if (oy >= 0x280) oy -= 0x400;
		}
		else
		{
			ox =  ox - offx;
			oy = -oy - offy;
		}
		ox += m_dx;
		oy -= m_dy;

		// apply global and display window offsets

		/* the coordinates given are for the *center* of the sprite */
		ox -= (zoomx * w) >> 13;
		oy -= (zoomy * h) >> 13;

		drawmode_table[m_gfx->granularity() - 1] = shadow ? DRAWMODE_SHADOW : DRAWMODE_SOURCE;

		for (y = 0; y < h; y++)
		{
			int sx, sy, zw, zh;

			sy = oy + ((zoomy * y + (1 << 11)) >> 12);
			zh = (oy + ((zoomy * (y + 1) + (1 << 11)) >> 12)) - sy;

			for (x = 0; x < w; x++)
			{
				int c, fx, fy;

				sx = ox + ((zoomx * x + (1 << 11)) >> 12);
				zw = (ox + ((zoomx * (x+1) + (1 << 11)) >> 12)) - sx;
				c = code;
				if (mirrorx)
				{
					if ((flipx == 0) ^ ((x << 1) < w))
					{
						/* mirror left/right */
						c += xoffset[(w - 1 - x + xa) & 7];
						fx = 1;
					}
					else
					{
						c += xoffset[(x + xa) & 7];
						fx = 0;
					}
				}
				else
				{
					if (flipx) c += xoffset[(w - 1 - x + xa) & 7];
					else c += xoffset[(x + xa) & 7];
					fx = flipx;
				}
				if (mirrory)
				{
					if ((flipy == 0) ^ ((y<<1) >= h))
					{
						/* mirror top/bottom */
						c += yoffset[(h - 1 - y + ya) & 7];
						fy = 1;
					}
					else
					{
						c += yoffset[(y + ya) & 7];
						fy = 0;
					}
				}
				else
				{
					if (flipy) c += yoffset[(h - 1 - y + ya) & 7];
					else c += yoffset[(y + ya) & 7];
					fy = flipy;
				}

				if (nozoom)
				{
					pdrawgfx_transtable(bitmap,cliprect,m_gfx,
							c,
							color,
							fx,fy,
							sx,sy,
							machine().priority_bitmap,primask,
							whichtable,machine().shadow_table);
				}
				else
				{
					pdrawgfxzoom_transtable(bitmap,cliprect,m_gfx,
							c,
							color,
							fx,fy,
							sx,sy,
							(zw << 16) >> 4,(zh << 16) >> 4,
							machine().priority_bitmap,primask,
							whichtable,machine().shadow_table);
				}

				if (mirrory && h == 1)  /* Simpsons shadows */
				{
					if (nozoom)
					{
						pdrawgfx_transtable(bitmap,cliprect,m_gfx,
								c,
								color,
								fx,!fy,
								sx,sy,
								machine().priority_bitmap,primask,
								whichtable,machine().shadow_table);
					}
					else
					{
						pdrawgfxzoom_transtable(bitmap,cliprect,m_gfx,
								c,
								color,
								fx,!fy,
								sx,sy,
								(zw << 16) >> 4,(zh << 16) >> 4,
								machine().priority_bitmap,primask,
								whichtable,machine().shadow_table);
					}
				}
			} // end of X loop
		} // end of Y loop

	} // end of sprite-list loop
#undef NUM_SPRITES
}

void k053247_device::k053247_sprites_draw( bitmap_ind16 &bitmap, const rectangle &cliprect )
{ k053247_sprites_draw_common( bitmap, cliprect); }

void k053247_device::k053247_sprites_draw( bitmap_rgb32 &bitmap, const rectangle &cliprect )
{ k053247_sprites_draw_common( bitmap, cliprect); }


/*****************************************************************************
    DEVICE INTERFACE
*****************************************************************************/




const device_type K055673 = &device_creator<k055673_device>;

k055673_device::k055673_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: k053247_device(mconfig, K055673, "Konami 055673", tag, owner, clock, "k055673", __FILE__)
{

}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k055673_device::device_start()
{

	/* early out for the non-interface cases for now */
	if (m_intf_gfx_num == -1)
		return;

	UINT32 total;
	UINT8 *s1, *s2, *d;
	long i;
	UINT16 *k055673_rom;
	int size4;

	static const gfx_layout spritelayout =  /* System GX sprite layout */
	{
		16,16,
		0,
		5,
		{ 32, 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 40, 41, 42, 43, 44, 45, 46, 47 },
		{ 0, 10*8, 10*8*2, 10*8*3, 10*8*4, 10*8*5, 10*8*6, 10*8*7, 10*8*8,
			10*8*9, 10*8*10, 10*8*11, 10*8*12, 10*8*13, 10*8*14, 10*8*15 },
		16*16*5
	};
	static const gfx_layout spritelayout2 = /* Run and Gun sprite layout */
	{
		16,16,
		0,
		4,
		{ 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 32, 33, 34, 35, 36, 37, 38, 39 },
		{ 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960 },
		16*16*4
	};
	static const gfx_layout spritelayout3 = /* Lethal Enforcers II sprite layout */
	{
		16,16,
		0,
		8,
		{ 8*1,8*0,8*3,8*2,8*5,8*4,8*7,8*6 },
		{  0,1,2,3,4,5,6,7,64+0,64+1,64+2,64+3,64+4,64+5,64+6,64+7 },
		{ 128*0, 128*1, 128*2,  128*3,  128*4,  128*5,  128*6,  128*7,
			128*8, 128*9, 128*10, 128*11, 128*12, 128*13, 128*14, 128*15 },
		128*16
	};
	static const gfx_layout spritelayout4 = /* System GX 6bpp sprite layout */
	{
		16,16,
		0,
		6,
		{ 40, 32, 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 48, 49, 50, 51, 52, 53, 54, 55 },
		{ 0, 12*8, 12*8*2, 12*8*3, 12*8*4, 12*8*5, 12*8*6, 12*8*7, 12*8*8,
			12*8*9, 12*8*10, 12*8*11, 12*8*12, 12*8*13, 12*8*14, 12*8*15 },
		16*16*6
	};

	m_screen = machine().device<screen_device>(m_intf_screen);

	k055673_rom = (UINT16 *)machine().root_device().memregion(m_intf_gfx_memory_region)->base();

	/* decode the graphics */
	switch (m_intf_plane_order)  /* layout would be more correct than plane_order, but we use k053247_interface */
	{
	case K055673_LAYOUT_GX:
		size4 = (machine().root_device().memregion(m_intf_gfx_memory_region)->bytes() / (1024 * 1024)) / 5;
		size4 *= 4 * 1024 * 1024;
		/* set the # of tiles based on the 4bpp section */
		k055673_rom = auto_alloc_array(machine(), UINT16, size4 * 5 / 2);
		d = (UINT8 *)k055673_rom;
		// now combine the graphics together to form 5bpp
		s1 = machine().root_device().memregion(m_intf_gfx_memory_region)->base(); // 4bpp area
		s2 = s1 + (size4);   // 1bpp area
		for (i = 0; i < size4; i+= 4)
		{
			*d++ = *s1++;
			*d++ = *s1++;
			*d++ = *s1++;
			*d++ = *s1++;
			*d++ = *s2++;
		}

		total = size4 / 128;
		konami_decode_gfx(machine(), m_intf_gfx_num, (UINT8 *)k055673_rom, total, &spritelayout, 4);
		break;

	case K055673_LAYOUT_RNG:
		total = machine().root_device().memregion(m_intf_gfx_memory_region)->bytes() / (16 * 16 / 2);
		konami_decode_gfx(machine(), m_intf_gfx_num, (UINT8 *)k055673_rom, total, &spritelayout2, 4);
		break;

	case K055673_LAYOUT_LE2:
		total = machine().root_device().memregion(m_intf_gfx_memory_region)->bytes() / (16 * 16);
		konami_decode_gfx(machine(), m_intf_gfx_num, (UINT8 *)k055673_rom, total, &spritelayout3, 4);
		break;

	case K055673_LAYOUT_GX6:
		total = machine().root_device().memregion(m_intf_gfx_memory_region)->bytes() / (16 * 16 * 6 / 8);
		konami_decode_gfx(machine(), m_intf_gfx_num, (UINT8 *)k055673_rom, total, &spritelayout4, 4);
		break;

	default:
		fatalerror("Unsupported layout\n");
	}

	if (VERBOSE && !(machine().config().m_video_attributes & VIDEO_HAS_SHADOWS))
		popmessage("driver should use VIDEO_HAS_SHADOWS");

	m_dx = m_intf_dx;
	m_dy = m_intf_dy;
	m_memory_region = m_intf_gfx_memory_region;
	m_gfx = machine().gfx[m_intf_gfx_num];
	m_callback = m_intf_callback;

	m_ram = auto_alloc_array(machine(), UINT16, 0x1000 / 2);

	save_pointer(NAME(m_ram), 0x800);
	save_item(NAME(m_kx46_regs));
	save_item(NAME(m_kx47_regs));
	save_item(NAME(m_objcha_line));
	save_item(NAME(m_wraparound));
	save_item(NAME(m_z_rejection));

}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------



const device_type K053246 = &device_creator<k053247_device>;

k053247_device::k053247_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K053246, "Konami 053246 & 053247", tag, owner, clock, "k053247", __FILE__)
{
	clear_all();
}

k053247_device::k053247_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source)
	: device_t(mconfig, type, name, tag, owner, clock, shortname, source)
{
	clear_all();
}


//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k053247_device::device_config_complete()
{
	// inherit a copy of the static data
	const k053247_interface *intf = reinterpret_cast<const k053247_interface *>(static_config());
	if (intf != NULL)
	*static_cast<k053247_interface *>(this) = *intf;
	
	// or initialize to defaults if none provided
	else
	{
	}

}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k053247_device::device_start()
{
	UINT32 total;
	static const gfx_layout spritelayout =
	{
		16,16,
		0,
		4,
		{ 0, 1, 2, 3 },
		{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4,
				10*4, 11*4, 8*4, 9*4, 14*4, 15*4, 12*4, 13*4 },
		{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
				8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
		128*8
	};
	static const gfx_layout tasman_16x16_layout =
	{
		16,16,
		RGN_FRAC(1,2),
		8,
		{ 0,8,16,24, RGN_FRAC(1,2)+0,RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+16,RGN_FRAC(1,2)+24 },
		{ 0,1,2,3,4,5,6,7, 32,33,34,35,36,37,38,39 },
		{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64, 8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
		16*64
	};

	m_screen = machine().device<screen_device>(m_intf_screen);

	/* decode the graphics */
	switch (m_intf_plane_order)
	{
	case NORMAL_PLANE_ORDER:
		total = machine().root_device().memregion(m_intf_gfx_memory_region)->bytes() / 128;
		konami_decode_gfx(machine(), m_intf_gfx_num, machine().root_device().memregion(m_intf_gfx_memory_region)->base(), total, &spritelayout, 4);
		break;

	case TASMAN_PLANE_ORDER:
		total = machine().root_device().memregion(m_intf_gfx_memory_region)->bytes() / 128;
		konami_decode_gfx(machine(), m_intf_gfx_num, machine().root_device().memregion(m_intf_gfx_memory_region)->base(), total, &tasman_16x16_layout, 4);
		break;

	default:
		fatalerror("Unsupported plane_order\n");
	}

	if (VERBOSE)
	{
		if (m_screen->format() == BITMAP_FORMAT_RGB32)
		{
			if ((machine().config().m_video_attributes & (VIDEO_HAS_SHADOWS|VIDEO_HAS_HIGHLIGHTS)) != VIDEO_HAS_SHADOWS+VIDEO_HAS_HIGHLIGHTS)
				popmessage("driver missing SHADOWS or HIGHLIGHTS flag");
		}
		else
		{
			if (!(machine().config().m_video_attributes & VIDEO_HAS_SHADOWS))
				popmessage("driver should use VIDEO_HAS_SHADOWS");
		}
	}

	/* deinterleave the graphics, if needed */
	konami_deinterleave_gfx(machine(), m_intf_gfx_memory_region, m_intf_deinterleave);

	m_dx = m_intf_dx;
	m_dy = m_intf_dy;
	m_memory_region = m_intf_gfx_memory_region;
	m_gfx = machine().gfx[m_intf_gfx_num];
	m_callback = m_intf_callback;

	m_ram = auto_alloc_array_clear(machine(), UINT16, 0x1000 / 2);

	save_pointer(NAME(m_ram), 0x1000 / 2);
	save_item(NAME(m_kx46_regs));
	save_item(NAME(m_kx47_regs));
	save_item(NAME(m_objcha_line));
	save_item(NAME(m_wraparound));
	save_item(NAME(m_z_rejection));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k053247_device::device_reset()
{
	m_wraparound = 1;
	m_z_rejection = -1;
	m_objcha_line = CLEAR_LINE;

	memset(m_kx46_regs, 0, 8);
	memset(m_kx47_regs, 0, 32);
}


/*
    In a K053247+K055555 setup objects with Z-code 0x00 should be ignored
    when PRFLIP is cleared, while objects with Z-code 0xff should be
    ignored when PRFLIP is set.

    These behaviors can also be seen in older K053245(6)+K053251 setups.
    Bucky'O Hare, The Simpsons and Sunset Riders rely on their implications
    to prepare and retire sprites. They probably apply to many other Konami
    games but it's hard to tell because most artifacts have been filtered
    by exclusion sort.

    A driver may call K05324x_set_z_rejection() to set which zcode to ignore.
    Parameter:
               -1 = accept all(default)
        0x00-0xff = zcode to ignore
*/

void k053247_device::k053247_set_z_rejection( int zcode )
{
	m_z_rejection = zcode;
}


READ16_MEMBER( k053247_device::k053246_reg_word_r )
{
	return(m_kx46_regs[offset * 2] << 8 | m_kx46_regs[offset * 2 + 1]);
}   // OBJSET1

READ16_MEMBER( k053247_device::k053247_reg_word_r )
{
	return(m_kx47_regs[offset]);
}   // OBJSET2

READ32_MEMBER( k053247_device::k053247_reg_long_r )
{
	offset <<= 1;
	return (k053247_reg_word_r( space, offset + 1, 0xffff) | k053247_reg_word_r( space, offset, 0xffff) << 16);
}


/***************************************************************************/
/*                                                                         */
/*                                 053246/053247                           */
/* stuff GX still relies on                                                */
/*                                                                         */
/***************************************************************************/


void k053247_device::alt_k053247_export_config(UINT16 **ram, gfx_element **gfx, void (**callback)(running_machine &, int *, int *, int *), int *dx, int *dy)
{
	if(ram)
		*ram = m_ram;
	if(gfx)
		*gfx = m_gfx;
	if(callback)
		*callback = m_callback;
	if(dx)
		*dx = m_dx;
	if(dy)
		*dy = m_dy;
}

/* alt_K055673 used with the 54246 in PreGX/Run and Gun/System GX games */
void k053247_device::alt_k055673_vh_start(running_machine &machine, const char *gfx_memory_region, int layout, int dx, int dy, void (*callback)(running_machine &machine, int *code,int *color,int *priority))
{
	int gfx_index;
	UINT32 total;

	static const gfx_layout spritelayout =  /* System GX sprite layout */
	{
		16,16,
		0,
		5,
		{ 32, 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 40, 41, 42, 43, 44, 45, 46, 47 },
		{ 0, 10*8, 10*8*2, 10*8*3, 10*8*4, 10*8*5, 10*8*6, 10*8*7, 10*8*8,
			10*8*9, 10*8*10, 10*8*11, 10*8*12, 10*8*13, 10*8*14, 10*8*15 },
		16*16*5
	};
	static const gfx_layout spritelayout2 = /* Run and Gun sprite layout */
	{
		16,16,
		0,
		4,
		{ 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 32, 33, 34, 35, 36, 37, 38, 39 },
		{ 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960 },
		16*16*4
	};
	static const gfx_layout spritelayout3 = /* Lethal Enforcers II sprite layout */
	{
		16,16,
		0,
		8,
		{ 8*1,8*0,8*3,8*2,8*5,8*4,8*7,8*6 },
		{  0,1,2,3,4,5,6,7,64+0,64+1,64+2,64+3,64+4,64+5,64+6,64+7 },
		{ 128*0, 128*1, 128*2,  128*3,  128*4,  128*5,  128*6,  128*7,
			128*8, 128*9, 128*10, 128*11, 128*12, 128*13, 128*14, 128*15 },
		128*16
	};
	static const gfx_layout spritelayout4 = /* System GX 6bpp sprite layout */
	{
		16,16,
		0,
		6,
		{ 40, 32, 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 48, 49, 50, 51, 52, 53, 54, 55 },
		{ 0, 12*8, 12*8*2, 12*8*3, 12*8*4, 12*8*5, 12*8*6, 12*8*7, 12*8*8,
			12*8*9, 12*8*10, 12*8*11, 12*8*12, 12*8*13, 12*8*14, 12*8*15 },
		16*16*6
	};
	UINT8 *s1, *s2, *d;
	long i;
	UINT16 *alt_k055673_rom;
	int size4;

	/* find first empty slot to decode gfx */
	for (gfx_index = 0; gfx_index < MAX_GFX_ELEMENTS; gfx_index++)
		if (machine.gfx[gfx_index] == 0)
			break;
	assert(gfx_index != MAX_GFX_ELEMENTS);

	alt_k055673_rom = (UINT16 *)machine.root_device().memregion(gfx_memory_region)->base();

	/* decode the graphics */
	switch(layout)
	{
	case K055673_LAYOUT_GX:
		size4 = (machine.root_device().memregion(gfx_memory_region)->bytes()/(1024*1024))/5;
		size4 *= 4*1024*1024;
		/* set the # of tiles based on the 4bpp section */
		alt_k055673_rom = auto_alloc_array(machine, UINT16, size4 * 5 / 2);
		d = (UINT8 *)alt_k055673_rom;
		// now combine the graphics together to form 5bpp
		s1 = machine.root_device().memregion(gfx_memory_region)->base(); // 4bpp area
		s2 = s1 + (size4);   // 1bpp area
		for (i = 0; i < size4; i+= 4)
		{
			*d++ = *s1++;
			*d++ = *s1++;
			*d++ = *s1++;
			*d++ = *s1++;
			*d++ = *s2++;
		}

		total = size4 / 128;
		konami_decode_gfx(machine, gfx_index, (UINT8 *)alt_k055673_rom, total, &spritelayout, 4);
		break;

	case K055673_LAYOUT_RNG:
		total = machine.root_device().memregion(gfx_memory_region)->bytes() / (16*16/2);
		konami_decode_gfx(machine, gfx_index, (UINT8 *)alt_k055673_rom, total, &spritelayout2, 4);
		break;

	case K055673_LAYOUT_LE2:
		total = machine.root_device().memregion(gfx_memory_region)->bytes() / (16*16);
		konami_decode_gfx(machine, gfx_index, (UINT8 *)alt_k055673_rom, total, &spritelayout3, 4);
		break;

	case K055673_LAYOUT_GX6:
		total = machine.root_device().memregion(gfx_memory_region)->bytes() / (16*16*6/8);
		konami_decode_gfx(machine, gfx_index, (UINT8 *)alt_k055673_rom, total, &spritelayout4, 4);
		break;

	default:
		fatalerror("Unsupported layout\n");
	}

	if (VERBOSE && !(machine.config().m_video_attributes & VIDEO_HAS_SHADOWS))
		popmessage("driver should use VIDEO_HAS_SHADOWS");

	m_dx = dx;
	m_dy = dy;
	m_wraparound = 1;
	m_z_rejection = -1;
	m_memory_region = gfx_memory_region;
	m_gfx = machine.gfx[gfx_index];
	m_callback = callback;
	m_objcha_line = CLEAR_LINE;
	m_ram = auto_alloc_array(machine, UINT16, 0x1000/2);

	memset(m_ram,  0, 0x1000);
	memset(m_kx46_regs, 0, 8);
	memset(m_kx47_regs, 0, 32);

	machine.save().save_pointer(NAME(m_ram), 0x800);
	machine.save().save_item(NAME(m_kx46_regs));
	machine.save().save_item(NAME(m_kx47_regs));
	machine.save().save_item(NAME(m_objcha_line));
}




