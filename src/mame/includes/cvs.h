/***************************************************************************

    Century CVS System

****************************************************************************/

extern mame_bitmap *cvs_collision_bitmap;
extern mame_bitmap *cvs_collision_background;
extern int cvs_collision_register;


PALETTE_INIT( cvs );
VIDEO_UPDATE( cvs );
VIDEO_START( cvs );

WRITE8_HANDLER( cvs_scroll_w );
WRITE8_HANDLER( cvs_video_fx_w );

READ8_HANDLER( cvs_collision_r );
READ8_HANDLER( cvs_collision_clear );


/*----------- defined in driver/cvs.c -----------*/

extern UINT8 *cvs_color_ram;
extern UINT8 *cvs_video_ram;
extern UINT8 *cvs_bullet_ram;
extern UINT8 *cvs_palette_ram;
extern UINT8 *cvs_character_ram;

READ8_HANDLER( cvs_video_or_color_ram_r );
WRITE8_HANDLER( cvs_video_or_color_ram_w );

READ8_HANDLER( cvs_bullet_ram_or_palette_r );
WRITE8_HANDLER( cvs_bullet_ram_or_palette_w );

READ8_HANDLER( cvs_s2636_1_or_character_ram_r );
WRITE8_HANDLER( cvs_s2636_1_or_character_ram_w );
READ8_HANDLER( cvs_s2636_2_or_character_ram_r );
WRITE8_HANDLER( cvs_s2636_2_or_character_ram_w );
READ8_HANDLER( cvs_s2636_3_or_character_ram_r );
WRITE8_HANDLER( cvs_s2636_3_or_character_ram_w );

UINT8 cvs_get_character_banking_mode(void);



/*----------- defined in video/cvs.c -----------*/

void cvs_scroll_stars(void);

