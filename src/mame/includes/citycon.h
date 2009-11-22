/*************************************************************************

    City Connection

*************************************************************************/

typedef struct _citycon_state citycon_state;
struct _citycon_state
{
	/* memory pointers */
	UINT8 *        videoram;
	UINT8 *        linecolor;
	UINT8 *        scroll;
	UINT8 *        spriteram;
//	UINT8 *        paletteram;	// currently this uses generic palette handling

	/* video-related */
	tilemap        *bg_tilemap,*fg_tilemap;
	int            bg_image;
};


/*----------- defined in video/citycon.c -----------*/

WRITE8_HANDLER( citycon_videoram_w );
WRITE8_HANDLER( citycon_linecolor_w );
WRITE8_HANDLER( citycon_background_w );

VIDEO_UPDATE( citycon );
VIDEO_START( citycon );
