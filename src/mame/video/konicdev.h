/*************************************************************************

    konicdev.h

    Implementation of various Konami custom video ICs

**************************************************************************/

#pragma once
#ifndef __KONICDEV_H__
#define __KONICDEV_H__

#include "konami_helper.h"

/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

typedef void (*k007342_callback)(running_machine &machine, int tmap, int bank, int *code, int *color, int *flags);
typedef void (*k007420_callback)(running_machine &machine, int *code, int *color);
typedef void (*k052109_callback)(running_machine &machine, int layer, int bank, int *code, int *color, int *flags, int *priority);
typedef void (*k051960_callback)(running_machine &machine, int *code, int *color, int *priority, int *shadow);
typedef void (*k051316_callback)(running_machine &machine, int *code, int *color, int *flags);


struct k007342_interface
{
	int                m_gfxnum;
	k007342_callback   m_callback;
};

struct k007420_interface
{
	int                m_banklimit;
	k007420_callback   m_callback;
};

struct k052109_interface
{
	const char         *m_gfx_memory_region;
	int                m_gfx_num;
	int                m_plane_order;
	int                m_deinterleave;
	k052109_callback   m_callback;
};

struct k051960_interface
{
	const char         *m_gfx_memory_region;
	int                m_gfx_num;
	int                m_plane_order;
	int                m_deinterleave;
	k051960_callback   m_callback;
};


struct k051316_interface
{
	const char         *m_gfx_memory_region_tag;
	int                m_gfx_num;
	int                m_bpp, m_pen_is_mask, m_transparent_pen;
	int                m_wrap, m_xoffs, m_yoffs;
	k051316_callback   m_callback;
};




struct k001006_interface
{
	const char     *gfx_region;
};

struct k001005_interface
{
	const char     *screen;
	const char     *cpu;
	const char     *dsp;
	const char     *k001006_1;
	const char     *k001006_2;

	const char     *gfx_memory_region;
	int            gfx_index;
};

struct k001604_interface
{
	int            gfx_index_1;
	int            gfx_index_2;
	int            layer_size;
	int            roz_size;
	int            txt_mem_offset;
	int            roz_mem_offset;
};

struct k037122_interface
{
	const char     *m_screen_tag;
	int            m_gfx_index;
};

class k007121_device : public device_t
{
public:
	k007121_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k007121_device() {}
	
	DECLARE_READ8_MEMBER( ctrlram_r );
	DECLARE_WRITE8_MEMBER( ctrl_w );

	/* shall we move source in the interface? */
	/* also notice that now we directly pass *gfx[chip] instead of **gfx !! */
	void sprites_draw( bitmap_ind16 &bitmap, const rectangle &cliprect, gfx_element *gfx, colortable_t *ctable,	const UINT8 *source, int base_color, int global_x_offset, int bank_base, UINT32 pri_mask );

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
	
private:
	// internal state
	UINT8    m_ctrlram[8];
	int      m_flipscreen;
};

extern const device_type K007121;

class k007342_device : public device_t,
										public k007342_interface
{
public:
	k007342_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k007342_device() {}

	DECLARE_READ8_MEMBER( read );
	DECLARE_WRITE8_MEMBER( write );
	DECLARE_READ8_MEMBER( scroll_r );
	DECLARE_WRITE8_MEMBER( scroll_w );
	DECLARE_WRITE8_MEMBER( vreg_w );

	void tilemap_update();
	void tilemap_draw(bitmap_ind16 &bitmap, const rectangle &cliprect, int num, int flags, UINT32 priority);
	int is_int_enabled();

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:
	// internal state
	UINT8    *m_ram;
	UINT8    *m_scroll_ram;
	UINT8    *m_videoram_0;
	UINT8    *m_videoram_1;
	UINT8    *m_colorram_0;
	UINT8    *m_colorram_1;

	tilemap_t  *m_tilemap[2];
	int      m_flipscreen, m_int_enabled;
	UINT8    m_regs[8];
	UINT16   m_scrollx[2];
	UINT8    m_scrolly[2];

	TILEMAP_MAPPER_MEMBER(scan);
	TILE_GET_INFO_MEMBER(get_tile_info0);
	TILE_GET_INFO_MEMBER(get_tile_info1);
	void get_tile_info( tile_data &tileinfo, int tile_index, int layer, UINT8 *cram, UINT8 *vram );
};

extern const device_type K007342;

class k007420_device : public device_t,
										public k007420_interface
{
public:
	k007420_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k007420_device() {}

	DECLARE_READ8_MEMBER( read );
	DECLARE_WRITE8_MEMBER( write );
	void sprites_draw(bitmap_ind16 &bitmap, const rectangle &cliprect, gfx_element *gfx);

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:
	// internal state
	UINT8        *m_ram;

	int          m_flipscreen;    // current code uses the 7342 flipscreen!!
	UINT8        m_regs[8];   // current code uses the 7342 regs!! (only [2])
};

extern const device_type K007420;

class k052109_device : public device_t,
										public k052109_interface
{
public:
	k052109_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k052109_device() {}

	/*
	You don't have to decode the graphics: the vh_start() routines will do that
	for you, using the plane order passed.
	Of course the ROM data must be in the correct order. This is a way to ensure
	that the ROM test will pass.
	The konami_rom_deinterleave() function above will do the reorganization for
	you in most cases (but see tmnt.c for additional bit rotations or byte
	permutations which may be required).
	*/
	#define NORMAL_PLANE_ORDER 0x0123
	#define REVERSE_PLANE_ORDER 0x3210
	#define GRADIUS3_PLANE_ORDER 0x1111
	#define TASMAN_PLANE_ORDER 0x1616
	
	/*
	The callback is passed:
	- layer number (0 = FIX, 1 = A, 2 = B)
	- bank (range 0-3, output of the pins CAB1 and CAB2)
	- code (range 00-FF, output of the pins VC3-VC10)
	NOTE: code is in the range 0000-FFFF for X-Men, which uses extra RAM
	- color (range 00-FF, output of the pins COL0-COL7)
	The callback must put:
	- in code the resulting tile number
	- in color the resulting color index
	- if necessary, put flags and/or priority for the TileMap code in the tile_info
	structure (e.g. TILE_FLIPX). Note that TILE_FLIPY is handled internally by the
	chip so it must not be set by the callback.
	*/

	DECLARE_READ8_MEMBER( read );
	DECLARE_WRITE8_MEMBER( write );	
	DECLARE_READ16_MEMBER( word_r );
	DECLARE_WRITE16_MEMBER( word_w );
	DECLARE_READ16_MEMBER( lsb_r );
	DECLARE_WRITE16_MEMBER( lsb_w );
	
	void set_rmrd_line(int state);
	int get_rmrd_line();
	void tilemap_update();
	int is_irq_enabled();
	void set_layer_offsets(int layer, int dx, int dy);
	void tilemap_mark_dirty(int tmap_num);
	void tilemap_draw(bitmap_ind16 &bitmap, const rectangle &cliprect, int tmap_num, UINT32 flags, UINT8 priority);

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:
	// internal state
	UINT8    *m_ram;
	UINT8    *m_videoram_F;
	UINT8    *m_videoram_A;
	UINT8    *m_videoram_B;
	UINT8    *m_videoram2_F;
	UINT8    *m_videoram2_A;
	UINT8    *m_videoram2_B;
	UINT8    *m_colorram_F;
	UINT8    *m_colorram_A;
	UINT8    *m_colorram_B;

	tilemap_t  *m_tilemap[3];
	int      m_tileflip_enable;
	UINT8    m_charrombank[4];
	UINT8    m_charrombank_2[4];
	UINT8    m_has_extra_video_ram;
	INT32    m_rmrd_line;
	UINT8    m_irq_enabled;
	INT32    m_dx[3], m_dy[3];
	UINT8    m_romsubbank, m_scrollctrl;

	TILE_GET_INFO_MEMBER(get_tile_info0);
	TILE_GET_INFO_MEMBER(get_tile_info1);
	TILE_GET_INFO_MEMBER(get_tile_info2);
	
	void get_tile_info( tile_data &tileinfo, int tile_index, int layer, UINT8 *cram, UINT8 *vram1, UINT8 *vram2 );
	void tileflip_reset();
};

extern const device_type K052109;

class k051960_device : public device_t,
										public k051960_interface
{
public:
	k051960_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k051960_device() {}

	/*
	The callback is passed:
	- code (range 00-1FFF, output of the pins CA5-CA17)
	- color (range 00-FF, output of the pins OC0-OC7). Note that most of the
	  time COL7 seems to be "shadow", but not always (e.g. Aliens).
	The callback must put:
	- in code the resulting sprite number
	- in color the resulting color index
	- if necessary, in priority the priority of the sprite wrt tilemaps
	- if necessary, alter shadow to indicate whether the sprite has shadows enabled.
	  shadow is preloaded with color & 0x80 so it doesn't need to be changed unless
	  the game has special treatment (Aliens)
	*/

	DECLARE_READ8_MEMBER( k051960_r );
	DECLARE_WRITE8_MEMBER( k051960_w );
	DECLARE_READ16_MEMBER( k051960_word_r );
	DECLARE_WRITE16_MEMBER( k051960_word_w );

	DECLARE_READ8_MEMBER( k051937_r );
	DECLARE_WRITE8_MEMBER( k051937_w );
	DECLARE_READ16_MEMBER( k051937_word_r );
	DECLARE_WRITE16_MEMBER( k051937_word_w );

	void k051960_sprites_draw(bitmap_ind16 &bitmap, const rectangle &cliprect, int min_priority, int max_priority);
	int k051960_is_irq_enabled();
	int k051960_is_nmi_enabled();
	void k051960_set_sprite_offsets(int dx, int dy);

	#if 0 // to be moved in the specific drivers!
	/* special handling for the chips sharing address space */
	DECLARE_READ8_HANDLER( k052109_051960_r );
	DECLARE_WRITE8_HANDLER( k052109_051960_w );
	#endif

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:
	// internal state
	UINT8    *m_ram;

	gfx_element *m_gfx;

	UINT8    m_spriterombank[3];
	int      m_dx, m_dy;
	int      m_romoffset;
	int      m_spriteflip, m_readroms;
	int      m_irq_enabled, m_nmi_enabled;

	int      m_k051937_counter;
	
	int k051960_fetchromdata( int byte );
};

extern const device_type K051960;



class k051316_device : public device_t,
										public k051316_interface
{
public:
	k051316_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k051316_device() {}

	/*
	The callback is passed:
	- code (range 00-FF, contents of the first tilemap RAM byte)
	- color (range 00-FF, contents of the first tilemap RAM byte). Note that bit 6
	  seems to be hardcoded as flip X.
	The callback must put:
	- in code the resulting tile number
	- in color the resulting color index
	- if necessary, put flags for the TileMap code in the tile_info
	  structure (e.g. TILE_FLIPX)
	*/

	DECLARE_READ8_MEMBER( read );
	DECLARE_WRITE8_MEMBER( write );
	DECLARE_READ8_MEMBER( rom_r );
	DECLARE_WRITE8_MEMBER( ctrl_w );
	void zoom_draw(bitmap_ind16 &bitmap,const rectangle &cliprect,int flags,UINT32 priority);
	void wraparound_enable(int status);

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:
	// internal state
	UINT8    *m_ram;
	tilemap_t  *m_tmap;
	int      m_bpp;
	UINT8    m_ctrlram[16];
		
	TILE_GET_INFO_MEMBER(get_tile_info0);
	void get_tile_info( tile_data &tileinfo, int tile_index );
};

extern const device_type K051316;

	enum
	{
		K053251_CI0 = 0,
		K053251_CI1,
		K053251_CI2,
		K053251_CI3,
		K053251_CI4
	};

class k053251_device : public device_t
{
public:
	k053251_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k053251_device() {}

	/*
	Note: k053251_w() automatically does a ALL_TILEMAPS->mark_all_dirty()
	when some palette index changes. If ALL_TILEMAPS is too expensive, use
	k053251_set_tilemaps() to indicate which tilemap is associated with each index.
	*/

	DECLARE_WRITE8_MEMBER( write );
	DECLARE_WRITE16_MEMBER( lsb_w );
	DECLARE_WRITE16_MEMBER( msb_w );
	int get_priority(int ci);
	int get_palette_index(int ci);
	int get_tmap_dirty(int tmap_num);
	void set_tmap_dirty(int tmap_num, int data);

	DECLARE_READ16_MEMBER( lsb_r );         // PCU1
	DECLARE_READ16_MEMBER( msb_r );         // PCU1

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:
	// internal state
	int      m_dirty_tmap[5];

	UINT8    m_ram[16];
	int      m_tilemaps_set;
	int      m_palette_index[5];
	
	void reset_indexes();
};

extern const device_type K053251;

class k051733_device : public device_t
{
public:
	k051733_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k051733_device() {}

	DECLARE_WRITE8_MEMBER( write );
	DECLARE_READ8_MEMBER( read );

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:
	// internal state
	UINT8    m_ram[0x20];
	UINT8    m_rng;
};

extern const device_type K051733;



class k001006_device : public device_t
{
public:
	k001006_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k001006_device() { global_free(m_token); }

	// access to legacy token
	void *token() const { assert(m_token != NULL); return m_token; }
protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:
	// internal state
	void *m_token;
};

extern const device_type K001006;

class k001005_device : public device_t
{
public:
	k001005_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k001005_device() { global_free(m_token); }

	// access to legacy token
	void *token() const { assert(m_token != NULL); return m_token; }
protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_stop();
	virtual void device_reset();
private:
	// internal state
	void *m_token;
};

extern const device_type K001005;

class k001604_device : public device_t
{
public:
	k001604_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k001604_device() { global_free(m_token); }

	// access to legacy token
	void *token() const { assert(m_token != NULL); return m_token; }
protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:
	// internal state
	void *m_token;

	TILEMAP_MAPPER_MEMBER(k001604_scan_layer_8x8_0_size0);
	TILEMAP_MAPPER_MEMBER(k001604_scan_layer_8x8_0_size1);
	TILEMAP_MAPPER_MEMBER(k001604_scan_layer_8x8_1_size0);
	TILEMAP_MAPPER_MEMBER(k001604_scan_layer_8x8_1_size1);
	TILEMAP_MAPPER_MEMBER(k001604_scan_layer_roz_256);
	TILEMAP_MAPPER_MEMBER(k001604_scan_layer_roz_128);
	TILE_GET_INFO_MEMBER(k001604_tile_info_layer_8x8);
	TILE_GET_INFO_MEMBER(k001604_tile_info_layer_roz);
};

extern const device_type K001604;

class k037122_device : public device_t,
										public k037122_interface
{
public:
	k037122_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k037122_device() {}

	void tile_draw( bitmap_rgb32 &bitmap, const rectangle &cliprect );
	DECLARE_READ32_MEMBER( sram_r );
	DECLARE_WRITE32_MEMBER( sram_w );
	DECLARE_READ32_MEMBER( char_r );
	DECLARE_WRITE32_MEMBER( char_w );
	DECLARE_READ32_MEMBER( reg_r );
	DECLARE_WRITE32_MEMBER( reg_w );

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();

private:
	// internal state
	screen_device *m_screen;
	tilemap_t     *m_layer[2];
	
	UINT32 *       m_tile_ram;
	UINT32 *       m_char_ram;
	UINT32 *       m_reg;

	TILE_GET_INFO_MEMBER(tile_info_layer0);
	TILE_GET_INFO_MEMBER(tile_info_layer1);
	void update_palette_color( UINT32 palette_base, int color );
};

extern const device_type K037122;



/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/

#define MCFG_K007121_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, K007121, 0)

#define MCFG_K007342_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K007342, 0) \
	MCFG_DEVICE_CONFIG(_interface)

#define MCFG_K007420_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K007420, 0) \
	MCFG_DEVICE_CONFIG(_interface)

#define MCFG_K052109_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K052109, 0) \
	MCFG_DEVICE_CONFIG(_interface)

#define MCFG_K051960_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K051960, 0) \
	MCFG_DEVICE_CONFIG(_interface)

#define MCFG_K053244_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K053244, 0) \
	MCFG_DEVICE_CONFIG(_interface)

#define MCFG_K053245_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K053245, 0) \
	MCFG_DEVICE_CONFIG(_interface)

#define MCFG_K053246_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K053246, 0) \
	MCFG_DEVICE_CONFIG(_interface)

#define MCFG_K053247_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K053247, 0) \
	MCFG_DEVICE_CONFIG(_interface)

#define MCFG_K055673_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K055673, 0) \
	MCFG_DEVICE_CONFIG(_interface)

#define MCFG_K051316_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K051316, 0) \
	MCFG_DEVICE_CONFIG(_interface)



#define MCFG_K053251_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, K053251, 0)



#define MCFG_K051733_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, K051733, 0)



#define MCFG_K001006_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K001006, 0) \
	MCFG_DEVICE_CONFIG(_interface)


#define MCFG_K001005_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K001005, 0) \
	MCFG_DEVICE_CONFIG(_interface)


#define MCFG_K001604_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K001604, 0) \
	MCFG_DEVICE_CONFIG(_interface)


#define MCFG_K037122_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K037122, 0) \
	MCFG_DEVICE_CONFIG(_interface)


/***************************************************************************
    HELPERS FOR DRIVERS
***************************************************************************/


/* helper function to sort three tile layers by priority order */
void konami_sortlayers3(int *layer, int *pri);
/* helper function to sort four tile layers by priority order */
void konami_sortlayers4(int *layer, int *pri);
/* helper function to sort five tile layers by priority order */
void konami_sortlayers5(int *layer, int *pri);

/***************************************************************************
    DEVICE I/O FUNCTIONS
***************************************************************************/


/**  Konami 001006  **/
UINT32 k001006_get_palette(device_t *device, int index);

DECLARE_READ32_DEVICE_HANDLER( k001006_r );
DECLARE_WRITE32_DEVICE_HANDLER( k001006_w );


/**  Konami 001005  **/
void k001005_draw(device_t *device, bitmap_ind16 &bitmap, const rectangle &cliprect);
void k001005_swap_buffers(device_t *device);
void k001005_preprocess_texture_data(UINT8 *rom, int length, int gticlub);

DECLARE_READ32_DEVICE_HANDLER( k001005_r );
DECLARE_WRITE32_DEVICE_HANDLER( k001005_w );


/**  Konami 001604  **/
void k001604_draw_back_layer( device_t *device, bitmap_rgb32 &bitmap, const rectangle &cliprect );
void k001604_draw_front_layer( device_t *device, bitmap_rgb32 &bitmap, const rectangle &cliprect );
DECLARE_WRITE32_DEVICE_HANDLER( k001604_tile_w );
DECLARE_READ32_DEVICE_HANDLER( k001604_tile_r );
DECLARE_WRITE32_DEVICE_HANDLER( k001604_char_w );
DECLARE_READ32_DEVICE_HANDLER( k001604_char_r );
DECLARE_WRITE32_DEVICE_HANDLER( k001604_reg_w );
DECLARE_READ32_DEVICE_HANDLER( k001604_reg_r );



#endif
