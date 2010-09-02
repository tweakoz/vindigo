/*************************************************************************

    Rock'n Rage

*************************************************************************/

class rockrage_state : public driver_device
{
public:
	rockrage_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	/* memory pointers */
	UINT8 *    paletteram;

	/* video-related */
	int        layer_colorbase[2];
	int        vreg;

	/* devices */
	running_device *audiocpu;
	running_device *k007342;
	running_device *k007420;
};


/*----------- defined in video/rockrage.c -----------*/

WRITE8_HANDLER( rockrage_vreg_w );

VIDEO_UPDATE( rockrage );
PALETTE_INIT( rockrage );

void rockrage_tile_callback(running_machine *machine, int layer, int bank, int *code, int *color, int *flags);
void rockrage_sprite_callback(running_machine *machine, int *code, int *color);
