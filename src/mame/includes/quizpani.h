#include "machine/nmk112.h"

class quizpani_state : public driver_device
{
public:
	quizpani_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_nmk112(*this, "nmk112"),
		m_scrollreg(*this, "scrollreg"),
		m_bg_videoram(*this, "bg_videoram"),
		m_txt_videoram(*this, "txt_videoram") { }

	required_device<cpu_device> m_maincpu;
	required_device<nmk112_device> m_nmk112;
	required_shared_ptr<UINT16> m_scrollreg;
	required_shared_ptr<UINT16> m_bg_videoram;
	required_shared_ptr<UINT16> m_txt_videoram;
	tilemap_t *m_bg_tilemap;
	tilemap_t *m_txt_tilemap;
	int m_bgbank;
	int m_txtbank;
	DECLARE_WRITE16_MEMBER(quizpani_bg_videoram_w);
	DECLARE_WRITE16_MEMBER(quizpani_txt_videoram_w);
	DECLARE_WRITE16_MEMBER(quizpani_tilesbank_w);
	TILEMAP_MAPPER_MEMBER(bg_scan);
	TILE_GET_INFO_MEMBER(bg_tile_info);
	TILE_GET_INFO_MEMBER(txt_tile_info);
	virtual void video_start();
	UINT32 screen_update_quizpani(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
};
