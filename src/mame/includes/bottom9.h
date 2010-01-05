/*************************************************************************

    Bottom of the Ninth

*************************************************************************/

typedef struct _bottom9_state bottom9_state;
struct _bottom9_state
{
	/* memory pointers */
//  UINT8 *    paletteram;    // currently this uses generic palette handling

	/* video-related */
	int        layer_colorbase[3], sprite_colorbase, zoom_colorbase;

	/* misc */
	int        video_enable;
	int        zoomreadroms, k052109_selected;
	int        nmienable;

	/* devices */
	const device_config *maincpu;
	const device_config *audiocpu;
	const device_config *k007232_1;
	const device_config *k007232_2;
	const device_config *k052109;
	const device_config *k051960;
	const device_config *k051316;
};

/*----------- defined in video/bottom9.c -----------*/

extern void bottom9_tile_callback(running_machine *machine, int layer,int bank,int *code,int *color,int *flags,int *priority);
extern void bottom9_sprite_callback(running_machine *machine, int *code,int *color,int *priority,int *shadow);
extern void bottom9_zoom_callback(running_machine *machine, int *code,int *color,int *flags);

VIDEO_START( bottom9 );
VIDEO_UPDATE( bottom9 );
