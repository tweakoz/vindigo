/*************************************************************************

    Atari Basketball hardware

*************************************************************************/

#include "sound/discrete.h"

/* Discrete Sound Input Nodes */
#define BSKTBALL_NOTE_DATA		NODE_01
#define BSKTBALL_CROWD_DATA		NODE_02
#define BSKTBALL_NOISE_EN		NODE_03
#define BSKTBALL_BOUNCE_EN		NODE_04


class bsktball_state : public driver_device
{
public:
	bsktball_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) ,
		m_videoram(*this, "videoram"),
		m_motion(*this, "motion"){ }

	/* memory pointers */
	required_shared_ptr<UINT8> m_videoram;
	required_shared_ptr<UINT8> m_motion;

	/* video-related */
	tilemap_t  *m_bg_tilemap;

	/* misc */
	UINT32   m_nmi_on;
//  int      m_i256v;

	/* input-related */
	int m_ld1;
	int m_ld2;
	int m_dir0;
	int m_dir1;
	int m_dir2;
	int m_dir3;
	int m_last_p1_horiz;
	int m_last_p1_vert;
	int m_last_p2_horiz;
	int m_last_p2_vert;
	DECLARE_WRITE8_MEMBER(bsktball_nmion_w);
	DECLARE_WRITE8_MEMBER(bsktball_ld1_w);
	DECLARE_WRITE8_MEMBER(bsktball_ld2_w);
	DECLARE_READ8_MEMBER(bsktball_in0_r);
	DECLARE_WRITE8_MEMBER(bsktball_led1_w);
	DECLARE_WRITE8_MEMBER(bsktball_led2_w);
	DECLARE_WRITE8_MEMBER(bsktball_videoram_w);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	virtual void palette_init();
	UINT32 screen_update_bsktball(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
};



/*----------- defined in machine/bsktball.c -----------*/

TIMER_DEVICE_CALLBACK( bsktball_scanline );


/*----------- defined in audio/bsktball.c -----------*/

DECLARE_WRITE8_DEVICE_HANDLER( bsktball_bounce_w );
DECLARE_WRITE8_DEVICE_HANDLER( bsktball_note_w );
DECLARE_WRITE8_DEVICE_HANDLER( bsktball_noise_reset_w );

DISCRETE_SOUND_EXTERN( bsktball );

/*----------- defined in video/bsktball.c -----------*/




