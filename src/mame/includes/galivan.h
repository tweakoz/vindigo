/***************************************************************************

    Galivan - Cosmo Police

***************************************************************************/

class galivan_state : public driver_device
{
public:
	galivan_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_videoram(*this, "videoram"),
		m_spriteram(*this, "spriteram"){ }

	/* memory pointers */
	required_shared_ptr<UINT8> m_videoram;
//  UINT8 *     m_colorram;
	required_shared_ptr<UINT8> m_spriteram;

	/* video-related */
	tilemap_t     *m_bg_tilemap;
	tilemap_t     *m_tx_tilemap;
	UINT16       m_scrollx;
	UINT16       m_scrolly;
	UINT8		m_galivan_scrollx[2],m_galivan_scrolly[2];
	UINT8       m_flipscreen;
	UINT8       m_write_layers;
	UINT8       m_layers;
	UINT8       m_ninjemak_dispdisable;

	UINT8       m_shift_scroll; //youmab
	UINT32		m_shift_val;
	DECLARE_WRITE8_MEMBER(galivan_sound_command_w);
	DECLARE_READ8_MEMBER(soundlatch_clear_r);
	DECLARE_READ8_MEMBER(IO_port_c0_r);
	DECLARE_WRITE8_MEMBER(blit_trigger_w);
	DECLARE_WRITE8_MEMBER(youmab_extra_bank_w);
	DECLARE_READ8_MEMBER(youmab_8a_r);
	DECLARE_WRITE8_MEMBER(youmab_81_w);
	DECLARE_WRITE8_MEMBER(youmab_84_w);
	DECLARE_WRITE8_MEMBER(youmab_86_w);
	DECLARE_WRITE8_MEMBER(galivan_videoram_w);
	DECLARE_WRITE8_MEMBER(galivan_gfxbank_w);
	DECLARE_WRITE8_MEMBER(ninjemak_gfxbank_w);
	DECLARE_WRITE8_MEMBER(galivan_scrollx_w);
	DECLARE_WRITE8_MEMBER(galivan_scrolly_w);
	DECLARE_DRIVER_INIT(youmab);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_tx_tile_info);
	TILE_GET_INFO_MEMBER(ninjemak_get_bg_tile_info);
	TILE_GET_INFO_MEMBER(ninjemak_get_tx_tile_info);
	virtual void palette_init();
	DECLARE_MACHINE_START(galivan);
	DECLARE_MACHINE_RESET(galivan);
	DECLARE_VIDEO_START(galivan);
	DECLARE_MACHINE_START(ninjemak);
	DECLARE_MACHINE_RESET(ninjemak);
	DECLARE_VIDEO_START(ninjemak);
	UINT32 screen_update_galivan(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_ninjemak(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
};



/*----------- defined in video/galivan.c -----------*/

DECLARE_WRITE8_HANDLER( ninjemak_scrollx_w );
DECLARE_WRITE8_HANDLER( ninjemak_scrolly_w );







