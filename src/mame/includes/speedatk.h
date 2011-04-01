class speedatk_state : public driver_device
{
public:
	speedatk_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	UINT8 *m_videoram;
	UINT8 *m_colorram;
	UINT8 m_crtc_vreg[0x100];
	UINT8 m_crtc_index;
	UINT8 m_flip_scr;

	UINT8 m_mux_data;
	UINT8 m_km_status;
	UINT8 m_coin_settings;
	UINT8 m_coin_impulse;
};


/*----------- defined in video/speedatk.c -----------*/

WRITE8_HANDLER( speedatk_videoram_w );
WRITE8_HANDLER( speedatk_colorram_w );
WRITE8_HANDLER( speedatk_6845_w );
PALETTE_INIT( speedatk );
VIDEO_START( speedatk );
SCREEN_UPDATE( speedatk );
