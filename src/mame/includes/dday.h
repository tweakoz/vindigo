/*************************************************************************

    D-Day

*************************************************************************/


class dday_state : public driver_device
{
public:
	dday_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	/* memory pointers */
	UINT8 *        bgvideoram;
	UINT8 *        fgvideoram;
	UINT8 *        textvideoram;
	UINT8 *        colorram;

	/* video-related */
	tilemap_t        *fg_tilemap, *bg_tilemap, *text_tilemap, *sl_tilemap;
	bitmap_t       *main_bitmap;
	int            control;
	int            sl_image;
	int            sl_enable;
	int            timer_value;

	/* devices */
	device_t *ay1;
};


/*----------- defined in video/dday.c -----------*/

PALETTE_INIT( dday );
VIDEO_START( dday );
SCREEN_UPDATE( dday );

WRITE8_HANDLER( dday_bgvideoram_w );
WRITE8_HANDLER( dday_fgvideoram_w );
WRITE8_HANDLER( dday_textvideoram_w );
WRITE8_HANDLER( dday_colorram_w );
READ8_HANDLER( dday_colorram_r );
WRITE8_HANDLER( dday_control_w );
WRITE8_HANDLER( dday_sl_control_w );
READ8_HANDLER( dday_countdown_timer_r );
