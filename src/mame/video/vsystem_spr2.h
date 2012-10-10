// Video System Sprites


class vsystem_spr2_device : public device_t
{
public:
	vsystem_spr2_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	
	template<class _BitmapClass>
	void turbofrc_draw_sprites_common( UINT16* spriteram3,  int spriteram3_bytes, UINT16* spriteram1, int spriteram1_bytes, UINT16* spriteram2, int spriteram2_bytes, int sprite_gfx, int spritepalettebank, running_machine &machine, _BitmapClass &bitmap, const rectangle &cliprect, int chip, int chip_disabled_pri );
	
	void turbofrc_draw_sprites( UINT16* spriteram3,  int spriteram3_bytes, UINT16* spriteram1, int spriteram1_bytes, UINT16* spriteram2, int spriteram2_bytes, int sprite_gfx, int spritepalettebank, running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect, int chip, int chip_disabled_pri );
	void turbofrc_draw_sprites( UINT16* spriteram3,  int spriteram3_bytes, UINT16* spriteram1, int spriteram1_bytes, UINT16* spriteram2, int spriteram2_bytes, int sprite_gfx, int spritepalettebank, running_machine &machine, bitmap_rgb32 &bitmap, const rectangle &cliprect, int chip, int chip_disabled_pri );

	template<class _BitmapClass>
	void spinlbrk_draw_sprites_common( UINT16* spriteram3,  int spriteram3_bytes, UINT16* spriteram1, int spriteram1_bytes, UINT16* spriteram2, int spriteram2_bytes, int sprite_gfx, int spritepalettebank, running_machine &machine, _BitmapClass &bitmap, const rectangle &cliprect, int chip, int chip_disabled_pri );
	
	void spinlbrk_draw_sprites( UINT16* spriteram3,  int spriteram3_bytes, UINT16* spriteram1, int spriteram1_bytes, UINT16* spriteram2, int spriteram2_bytes, int sprite_gfx, int spritepalettebank, running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect, int chip, int chip_disabled_pri );
	void spinlbrk_draw_sprites( UINT16* spriteram3,  int spriteram3_bytes, UINT16* spriteram1, int spriteram1_bytes, UINT16* spriteram2, int spriteram2_bytes, int sprite_gfx, int spritepalettebank, running_machine &machine, bitmap_rgb32 &bitmap, const rectangle &cliprect, int chip, int chip_disabled_pri );


	void welltris_draw_sprites( UINT16* spriteram, int spritepalettebank, running_machine &machine, bitmap_ind16 &bitmap,const rectangle &cliprect);
	void f1gp_draw_sprites( UINT16* spr1vram, UINT16* spr2vram, UINT16* spr1cgram, int spr1cgram_bytes, UINT16* spr2cgram, int spr2cgram_bytes, running_machine &machine, bitmap_ind16 &bitmap, const rectangle &cliprect, int chip, int primask );
	void draw_sprites_pipedrm( UINT8* spriteram, int spriteram_bytes, int flipscreen, screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int draw_priority );

protected:
	virtual void device_start();
	virtual void device_reset();


private:


};


extern const device_type VSYSTEM_SPR2;



