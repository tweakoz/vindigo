/*************************************************************************

    Ginga NinkyouDen

*************************************************************************/

class ginganin_state : public driver_device
{
public:
	ginganin_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	/* memory pointers */
	UINT16 *    fgram;
	UINT16 *    txtram;
	UINT16 *    vregs;
	UINT16 *    spriteram;
//  UINT16 *    paletteram; // currently this uses generic palette handling
	size_t      spriteram_size;

	/* video-related */
	tilemap_t     *bg_tilemap, *fg_tilemap, *tx_tilemap;
	int         layers_ctrl, flipscreen;
#ifdef MAME_DEBUG
	int         posx, posy;
#endif
	/* sound-related */
	UINT8       MC6840_index0;
	UINT8       MC6840_register0;
	UINT8       MC6840_index1;
	UINT8       MC6840_register1;
	int         S_TEMPO;
	int         S_TEMPO_OLD;
	int         MC6809_CTR;
	int         MC6809_FLAG;

	/* devices */
	device_t *audiocpu;
};



/*----------- defined in video/ginganin.c -----------*/

WRITE16_HANDLER( ginganin_fgram16_w );
WRITE16_HANDLER( ginganin_txtram16_w );
WRITE16_HANDLER( ginganin_vregs16_w );

VIDEO_START( ginganin );
VIDEO_UPDATE( ginganin );
