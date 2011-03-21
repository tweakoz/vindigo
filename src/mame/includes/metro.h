/*************************************************************************

    Metro Games

*************************************************************************/

#include "sound/okim6295.h"
#include "sound/2151intf.h"
#include "video/konicdev.h"

class metro_state : public driver_device
{
public:
	metro_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config),
		  maincpu(*this, "maincpu"),
		  audiocpu(*this, "audiocpu"),
		  oki(*this, "oki"),
		  ymsnd(*this, "ymsnd"),
		  k053936(*this, "k053936") { }

	/* memory pointers */
	UINT16 *    vram_0;
	UINT16 *    vram_1;
	UINT16 *    vram_2;
	UINT16 *    spriteram;
	UINT16 *    tiletable;
	UINT16 *    tiletable_old;
	UINT16 *    blitter_regs;
	UINT16 *    scroll;
	UINT16 *    window;
	UINT16 *    irq_enable;
	UINT16 *    irq_levels;
	UINT16 *    irq_vectors;
	UINT16 *    rombank;
	UINT16 *    videoregs;
	UINT16 *    screenctrl;
	UINT16 *    input_sel;
	UINT16 *    k053936_ram;

	size_t      spriteram_size;
	size_t      tiletable_size;

	int         flip_screen;

	/* video-related */
	tilemap_t   *k053936_tilemap;
	int         bg_tilemap_enable[3];
	int         bg_tilemap_enable16[3];
	int         bg_tilemap_scrolldx[3];

	int         support_8bpp, support_16x16;
	int         has_zoom;
	int         sprite_xoffs, sprite_yoffs;

	/* blitter */
	int         blitter_bit;

	/* irq_related */
	int         irq_line;
	UINT8       requested_int[8];
	emu_timer   *mouja_irq_timer;

	/* sound related */
	UINT16      soundstatus;
	int         porta, portb, busy_sndcpu;

	/* misc */
	int         gakusai_oki_bank_lo, gakusai_oki_bank_hi;

	/* devices */
	required_device<cpu_device> maincpu;
	optional_device<cpu_device> audiocpu;
	optional_device<okim6295_device> oki;
	optional_device<device_t> ymsnd;
	optional_device<k053936_device> k053936;
};


/*----------- defined in video/metro.c -----------*/

WRITE16_HANDLER( metro_window_w );
WRITE16_HANDLER( metro_vram_0_w );
WRITE16_HANDLER( metro_vram_1_w );
WRITE16_HANDLER( metro_vram_2_w );
WRITE16_HANDLER( metro_k053936_w );

VIDEO_START( metro_14100 );
VIDEO_START( metro_14220 );
VIDEO_START( metro_14300 );
VIDEO_START( blzntrnd );
VIDEO_START( gstrik2 );

SCREEN_UPDATE( metro );

void metro_draw_sprites(running_machine *machine, bitmap_t *bitmap, const rectangle *cliprect);
