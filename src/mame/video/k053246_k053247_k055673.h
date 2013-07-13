/* */

#pragma once
#ifndef __K05324x_H__
#define __K05324x_H__

#define NORMAL_PLANE_ORDER 0x0123
#define TASMAN_PLANE_ORDER 0x1616

typedef void (*k05324x_callback)(running_machine &machine, int *code, int *color, int *priority);


/**  Konami 053246 / 053247 / 055673  **/
#define K055673_LAYOUT_GX  0
#define K055673_LAYOUT_RNG 1
#define K055673_LAYOUT_LE2 2
#define K055673_LAYOUT_GX6 3


/*
Callback procedures for non-standard shadows:

1) translate shadow code to the correct 2-bit form (0=off, 1-3=style)
2) shift shadow code left by K053247_SHDSHIFT and add the K053247_CUSTOMSHADOW flag
3) combine the result with sprite color
*/
#define K053247_CUSTOMSHADOW    0x20000000
#define K053247_SHDSHIFT        20


struct k053247_interface
{
	const char         *m_intf_screen;
	const char         *m_intf_gfx_memory_region;
	int                m_intf_gfx_num;
	int                m_intf_plane_order;
	int                m_intf_dx, m_intf_dy;
	int                m_intf_deinterleave;
	k05324x_callback   m_intf_callback;
};

class k053247_device : public device_t,
					   public k053247_interface
{
public:
	k053247_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	k053247_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	
	~k053247_device() { }

	void clear_all();

	DECLARE_READ16_MEMBER( k055673_rom_word_r );
	DECLARE_READ16_MEMBER( k055673_GX6bpp_rom_word_r );

	DECLARE_READ8_MEMBER( k053247_r );
	DECLARE_WRITE8_MEMBER( k053247_w );
	DECLARE_READ16_MEMBER( k053247_word_r );
	DECLARE_WRITE16_MEMBER( k053247_word_w );
	DECLARE_READ32_MEMBER( k053247_long_r );
	DECLARE_WRITE32_MEMBER( k053247_long_w );
	DECLARE_WRITE16_MEMBER( k053247_reg_word_w ); // "OBJSET2" registers
	DECLARE_WRITE32_MEMBER( k053247_reg_long_w );

	void k053247_sprites_draw( bitmap_ind16 &bitmap,const rectangle &cliprect);
	void k053247_sprites_draw( bitmap_rgb32 &bitmap,const rectangle &cliprect);
	int k053247_read_register( int regnum);
	void k053247_set_sprite_offs( int offsx, int offsy);
	void k053247_wraparound_enable( int status);
	void k053247_set_z_rejection( int zcode); // common to k053246/7
	void k053247_get_ram( UINT16 **ram);
	int k053247_get_dx( void );
	int k053247_get_dy( void );

	DECLARE_READ8_MEMBER( k053246_r );
	DECLARE_WRITE8_MEMBER( k053246_w );
	DECLARE_READ16_MEMBER( k053246_word_r );
	DECLARE_WRITE16_MEMBER( k053246_word_w );
	DECLARE_READ32_MEMBER( k053246_long_r );
	DECLARE_WRITE32_MEMBER( k053246_long_w );

	void k053246_set_objcha_line( int state);
	int k053246_is_irq_enabled(void);
	int k053246_read_register( int regnum);
	
	DECLARE_READ16_MEMBER( k053246_reg_word_r );    // OBJSET1
	DECLARE_READ16_MEMBER( k053247_reg_word_r );    // OBJSET2
	DECLARE_READ32_MEMBER( k053247_reg_long_r );    // OBJSET2

	UINT16    *m_ram;

	gfx_element *m_gfx;

	UINT8    m_kx46_regs[8];
	UINT16   m_kx47_regs[16];
	int      m_dx, m_dy, m_wraparound;
	UINT8    m_objcha_line;
	int      m_z_rejection;

	k05324x_callback m_callback;

	const char *m_memory_region;
	screen_device *m_screen;

	template<class _BitmapClass>
	void k053247_sprites_draw_common( _BitmapClass &bitmap, const rectangle &cliprect );

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:


};

extern const device_type K053246;

#define K053247 K053246
class k055673_device : public k053247_device
{
public:
	k055673_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k055673_device() { }

protected:
	// device-level overrides
//	virtual void device_config_complete();
	virtual void device_start();
//	virtual void device_reset();
private:

};

extern const device_type K055673;


#define MCFG_K053246_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K053246, 0) \
	MCFG_DEVICE_CONFIG(_interface)

#define MCFG_K053247_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K053247, 0) \
	MCFG_DEVICE_CONFIG(_interface)
	
#define MCFG_K055673_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, K055673, 0) \
	MCFG_DEVICE_CONFIG(_interface)




/* old non-device stuff */

void K055673_vh_start(running_machine &machine, const char *gfx_memory_region, int alt_layout, int dx, int dy,
		void (*callback)(running_machine &machine, int *code,int *color,int *priority));
DECLARE_READ16_HANDLER( K055673_rom_word_r );
DECLARE_READ16_HANDLER( K055673_GX6bpp_rom_word_r );

/*
Callback procedures for non-standard shadows:

1) translate shadow code to the correct 2-bit form (0=off, 1-3=style)
2) shift shadow code left by K053247_SHDSHIFT and add the K053247_CUSTOMSHADOW flag
3) combine the result with sprite color
*/
#define K053247_CUSTOMSHADOW    0x20000000
#define K053247_SHDSHIFT        20

DECLARE_READ16_HANDLER( K053247_word_r );
DECLARE_WRITE16_HANDLER( K053247_word_w );
DECLARE_READ32_HANDLER( K053247_long_r );
DECLARE_WRITE32_HANDLER( K053247_long_w );
DECLARE_WRITE16_HANDLER( K053247_reg_word_w ); // "OBJSET2" registers
DECLARE_WRITE32_HANDLER( K053247_reg_long_w );

int K053247_read_register(int regnum);
void K053247_set_SpriteOffset(int offsx, int offsy);
void K053247_export_config(UINT16 **ram, gfx_element **gfx, void (**callback)(running_machine &, int *, int *, int *), int *dx, int *dy);

DECLARE_WRITE16_HANDLER( K053246_word_w );
DECLARE_WRITE32_HANDLER( K053246_long_w );
void K053246_set_OBJCHA_line(int state);
int K053246_is_IRQ_enabled(void);
int K053246_read_register(int regnum);





#endif
