/***************************************************************************

    Ms.Pac-Man/Galaga - 20 Year Reunion hardware

    driver by Nicola Salmoria

***************************************************************************/


class _20pacgal_state : public driver_data_t
{
public:
	static driver_data_t *alloc(running_machine &machine) { return auto_alloc_clear(&machine, _20pacgal_state(machine)); }

	_20pacgal_state(running_machine &machine)
		: driver_data_t(machine) { }

	/* memory pointers */
	UINT8 *char_gfx_ram;
	UINT8 *sprite_gfx_ram;
	UINT8 *video_ram;
	UINT8 *sprite_ram;
	UINT8 *sprite_color_lookup;
	UINT8 *flip;
	UINT8 *stars_seed;
	UINT8 *stars_ctrl;

	/* machine state */
	UINT8 game_selected;	/* 0 = Ms. Pac-Man, 1 = Galaga */

	/* devices */
	running_device *maincpu;
	running_device *eeprom;

	/* bank support */
	UINT8 *ram_48000;

	/* differs between 25pacman and 20pacgal, might be a register */
	UINT8 sprite_pal_base;
};



/*----------- defined in video/20pacgal.c -----------*/

MACHINE_DRIVER_EXTERN( 20pacgal_video );
