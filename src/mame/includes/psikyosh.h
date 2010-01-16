
#define MASTER_CLOCK 57272700	// main oscillator frequency

/* Psikyo PS6406B */
#define FLIPSCREEN (((state->vidregs[3] & 0x0000c000) == 0x0000c000) ? 1:0)
#define DISPLAY_DISABLE (((state->vidregs[2] & 0x0000000f) == 0x00000006) ? 1:0)
#define BG_LARGE(n) (((state->vidregs[7] << (4*n)) & 0x00001000 ) ? 1:0)
#define BG_DEPTH_8BPP(n) (((state->vidregs[7] << (4*n)) & 0x00004000 ) ? 1:0)
#define BG_LAYER_ENABLE(n) (((state->vidregs[7] << (4*n)) & 0x00008000 ) ? 1:0)

#define BG_TYPE(n) (((state->vidregs[6] << (8*n)) & 0x7f000000 ) >> 24)
#define BG_LINE(n) (((state->vidregs[6] << (8*n)) & 0x80000000 ) ? 1:0)

#define BG_TRANSPEN MAKE_ARGB(0x00,0xff,0x00,0xff) // used for representing transparency in temporary bitmaps

/* All below have BG_LINE() set, row/linescroll enable/toggle */
#define BG_SCROLL_0C   0x0c /* 224 v/h scroll values in bank 0x0c; Used in daraku, for text */
#define BG_SCROLL_0D   0x0d /* 224 v/h scroll values in bank 0x0d; Used in daraku, for alternate characters of text */

#define SPRITE_PRI(n) (((state->vidregs[2] << (4*n)) & 0xf0000000 ) >> 28)


typedef struct _psikyosh_state psikyosh_state;
struct _psikyosh_state
{
	/* memory pointers */
	UINT32 *       bgram;
	UINT32 *       zoomram;
	UINT32 *       vidregs;
	UINT32 *       ram;
	UINT32 *       paletteram;
//  UINT32 *       spriteram;   // currently this uses generic buffered spriteram
//  size_t         spriteram_size;

	/* video-related */
	bitmap_t       *zoom_bitmap, *z_bitmap, *bg_bitmap;
//  UINT8          alphatable[256];

	/* misc */
	UINT32         sample_offs;	// only used if ROMTEST = 1

	/* devices */
	const device_config *maincpu;
};

/*----------- defined in video/psikyosh.c -----------*/

VIDEO_START( psikyosh );
VIDEO_UPDATE( psikyosh );
VIDEO_EOF( psikyosh );
