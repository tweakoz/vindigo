/*************************************************************************

    Asterix

*************************************************************************/

class asterix_state : public driver_device
{
public:
	asterix_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	/* memory pointers */
//  UINT16 *    m_paletteram;    // currently this uses generic palette handling

	/* video-related */
	int         m_sprite_colorbase;
	int         m_layer_colorbase[4];
	int         m_layerpri[3];
	UINT16      m_spritebank;
	int         m_tilebanks[4];
	int         m_spritebanks[4];

	/* misc */
	UINT8       m_cur_control2;
	UINT16      m_prot[2];

	/* devices */
	device_t *m_maincpu;
	device_t *m_audiocpu;
	device_t *m_k053260;
	device_t *m_k056832;
	device_t *m_k053244;
	device_t *m_k053251;
};



/*----------- defined in video/asterix.c -----------*/

SCREEN_UPDATE( asterix );
WRITE16_HANDLER( asterix_spritebank_w );

extern void asterix_tile_callback(running_machine &machine, int layer, int *code, int *color, int *flags);
extern void asterix_sprite_callback(running_machine &machine, int *code, int *color, int *priority_mask);
