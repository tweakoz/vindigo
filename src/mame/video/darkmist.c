#include "driver.h"

/*defined in drivers/darkmist.c */
extern int darkmist_hw;


UINT8 *darkmist_scroll;
UINT8 *darkmist_spritebank;

/* vis. flags */

#define DISPLAY_SPR		1
#define DISPLAY_FG		2 /* 2 or 8 */
#define DISPLAY_BG		4
#define DISPLAY_TXT		16

static tilemap *bgtilemap, *fgtilemap, *txtilemap;

static TILE_GET_INFO( get_bgtile_info )
{
	int code,attr,pal;

	code=memory_region(REGION_USER1)[tile_index]; /* TTTTTTTT */
	attr=memory_region(REGION_USER2)[tile_index]; /* -PPP--TT - FIXED BITS (0xxx00xx) */
	code+=(attr&3)<<8;
	pal=(attr>>4);

	SET_TILE_INFO(
		1,
        code,
        pal,
        0);
}

static TILE_GET_INFO( get_fgtile_info )
{
	int code,attr,pal;

	code=memory_region(REGION_USER3)[tile_index]; /* TTTTTTTT */
	attr=memory_region(REGION_USER4)[tile_index]; /* -PPP--TT - FIXED BITS (0xxx00xx) */
	pal=attr>>4;

	code+=(attr&3)<<8;

	code+=0x400;

	pal+=16;

	SET_TILE_INFO(
		1,
        code,
        pal,
        0);
}

static TILE_GET_INFO( get_txttile_info )
{
	int code,attr,pal;

	code=videoram[tile_index];
	attr=videoram[tile_index+0x400];
	pal=(attr>>1);

	code+=(attr&1)<<8;

	pal+=48;

	SET_TILE_INFO(
		0,
        code,
        pal,
        0);
}

PALETTE_INIT(darkmist)
{
	int i;

	/* allocate the colortable */
	machine->colortable = colortable_alloc(machine, 0x101);

	for (i = 0; i < 0x400; i++)
	{
		int ctabentry;

		if (color_prom[i] & 0x40)
			ctabentry = 0x100;
		else
		{
			ctabentry = (color_prom[i] & 0x3f);

			switch (i & 0x300)
			{
			case 0x000:  ctabentry = ctabentry | 0x80; break;
			case 0x100:  ctabentry = ctabentry | 0x00; break;
			case 0x200:  ctabentry = ctabentry | 0x40; break;
			case 0x300:  ctabentry = ctabentry | 0xc0; break;
			}
		}

		colortable_entry_set_value(machine->colortable, i, ctabentry);
	}
}


static void set_pens(running_machine *machine)
{
	int i;

	for (i = 0; i < 0x100; i++)
	{
		int r = pal4bit(paletteram[i | 0x200] >> 0);
		int g = pal4bit(paletteram[i | 0x000] >> 4);
		int b = pal4bit(paletteram[i | 0x000] >> 0);

		colortable_palette_set_color(machine->colortable, i, MAKE_RGB(r, g, b));
	}

	colortable_palette_set_color(machine->colortable, 0x100, RGB_BLACK);
}


VIDEO_START(darkmist)
{
	bgtilemap = tilemap_create( get_bgtile_info,tilemap_scan_rows,16,16,512,64 );
	fgtilemap = tilemap_create( get_fgtile_info,tilemap_scan_rows,16,16,64,256 );
	txtilemap = tilemap_create( get_txttile_info,tilemap_scan_rows,8,8,32,32 );
	tilemap_set_transparent_pen(fgtilemap, 0);
	tilemap_set_transparent_pen(txtilemap, 0);
}

VIDEO_UPDATE( darkmist)
{

#define DM_GETSCROLL(n) (((darkmist_scroll[(n)]<<1)&0xff) + ((darkmist_scroll[(n)]&0x80)?1:0) +( ((darkmist_scroll[(n)-1]<<4) | (darkmist_scroll[(n)-1]<<12) )&0xff00))

	set_pens(machine);

	tilemap_set_scrollx(bgtilemap, 0, DM_GETSCROLL(0x2));
	tilemap_set_scrolly(bgtilemap, 0, DM_GETSCROLL(0x6));
	tilemap_set_scrollx(fgtilemap, 0, DM_GETSCROLL(0xa));
	tilemap_set_scrolly(fgtilemap, 0, DM_GETSCROLL(0xe));

	fillbitmap(bitmap, get_black_pen(machine), cliprect);

	if(darkmist_hw & DISPLAY_BG)
		tilemap_draw(bitmap,cliprect,bgtilemap, 0,0);

	if(darkmist_hw & DISPLAY_FG)
		tilemap_draw(bitmap,cliprect,fgtilemap, 0,0);

	if(darkmist_hw & DISPLAY_SPR)
	{
/*
    Sprites

    76543210
0 - TTTT TTTT - tile
1 - xyBP PPP? - palette (P), flips (x,y), B - use spritebank,
                ? - unknown, according to gamecode top bit of one of coords(y/x)
2 - YYYY YYYY - y coord
3 - XXXX XXXX - x coord

*/
	int i,fx,fy,tile,palette;
	for(i=0;i<spriteram_size;i+=32)
	{
		fy=spriteram[i+1]&0x40;
		fx=spriteram[i+1]&0x80;

		tile=spriteram[i+0];

		if(spriteram[i+1]&0x20)
			tile += (*darkmist_spritebank << 8);

		palette=((spriteram[i+1])>>1)&0xf;

		if(spriteram[i+1]&0x1)
			palette=mame_rand(machine)&15;

		palette+=32;

		drawgfx(
               bitmap,machine->gfx[2],
               tile,
               palette,
               fx,fy,
               spriteram[i+3],spriteram[i+2],
               cliprect,
               TRANSPARENCY_PEN,0 );
		}

	}

	if(darkmist_hw & DISPLAY_TXT)
	{
		tilemap_mark_all_tiles_dirty(txtilemap);
		tilemap_draw(bitmap,cliprect,txtilemap, 0,0);
	}


	return 0;
}
