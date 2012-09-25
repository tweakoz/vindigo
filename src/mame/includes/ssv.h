#include "cpu/upd7725/upd7725.h"
#include "video/st0020.h"

class ssv_state : public driver_device
{
public:
	ssv_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_dsp(*this, "dsp"),
		m_mainram(*this, "mainram"),
		m_spriteram(*this, "spriteram"),
		m_paletteram(*this, "paletteram"),
		m_scroll(*this, "scroll"),
		m_irq_vectors(*this, "irq_vectors"),
		m_gdfs_tmapram(*this, "gdfs_tmapram"),
		m_gdfs_tmapscroll(*this, "gdfs_tmapscroll"),
		m_gdfs_st0020(*this, "st0020_spr"),
		m_input_sel(*this, "input_sel"){ }

	optional_device<upd96050_device> m_dsp;

	required_shared_ptr<UINT16> m_mainram;
	required_shared_ptr<UINT16> m_spriteram;
	required_shared_ptr<UINT16> m_paletteram;
	required_shared_ptr<UINT16> m_scroll;
	required_shared_ptr<UINT16> m_irq_vectors;
	optional_shared_ptr<UINT16> m_gdfs_tmapram;
	optional_shared_ptr<UINT16> m_gdfs_tmapscroll;
	optional_device<st0020_device> m_gdfs_st0020;
	optional_shared_ptr<UINT16> m_input_sel;

	int m_tile_code[16];

	int m_enable_video;
	int m_shadow_pen_mask;
	int m_shadow_pen_shift;

	UINT8 m_requested_int;
	UINT16 m_irq_enable;

	UINT16 *m_dsp_ram;

	UINT16 *m_eaglshot_gfxram;

	tilemap_t *m_gdfs_tmap;

	int m_interrupt_ultrax;

	int m_gdfs_gfxram_bank;
	int m_gdfs_lightgun_select;

	UINT16 m_sxyreact_serial;
	int m_sxyreact_dial;
	UINT16 m_gdfs_eeprom_old;

	UINT32 m_latches[8];

	UINT8 m_trackball_select;
	UINT8 m_gfxrom_select;
	DECLARE_WRITE16_MEMBER(ssv_irq_ack_w);
	DECLARE_WRITE16_MEMBER(ssv_irq_enable_w);
	DECLARE_WRITE16_MEMBER(ssv_lockout_w);
	DECLARE_WRITE16_MEMBER(ssv_lockout_inv_w);
	DECLARE_READ16_MEMBER(dsp_dr_r);
	DECLARE_WRITE16_MEMBER(dsp_dr_w);
	DECLARE_READ16_MEMBER(dsp_r);
	DECLARE_WRITE16_MEMBER(dsp_w);
	DECLARE_READ16_MEMBER(fake_r);
	DECLARE_READ16_MEMBER(drifto94_rand_r);
	DECLARE_READ16_MEMBER(gdfs_gfxram_r);
	DECLARE_WRITE16_MEMBER(gdfs_gfxram_w);
	DECLARE_READ16_MEMBER(gdfs_blitram_r);
	DECLARE_WRITE16_MEMBER(gdfs_blitram_w);
	DECLARE_READ16_MEMBER(hypreact_input_r);
	DECLARE_READ16_MEMBER(ssv_mainram_r);
	DECLARE_WRITE16_MEMBER(ssv_mainram_w);
	DECLARE_READ16_MEMBER(srmp4_input_r);
	DECLARE_READ16_MEMBER(srmp7_irqv_r);
	DECLARE_WRITE16_MEMBER(srmp7_sound_bank_w);
	DECLARE_READ16_MEMBER(srmp7_input_r);
	DECLARE_READ16_MEMBER(sxyreact_ballswitch_r);
	DECLARE_READ16_MEMBER(sxyreact_dial_r);
	DECLARE_WRITE16_MEMBER(sxyreact_dial_w);
	DECLARE_WRITE16_MEMBER(sxyreact_motor_w);
	DECLARE_READ32_MEMBER(latch32_r);
	DECLARE_WRITE32_MEMBER(latch32_w);
	DECLARE_READ16_MEMBER(latch16_r);
	DECLARE_WRITE16_MEMBER(latch16_w);
	DECLARE_READ16_MEMBER(eaglshot_gfxrom_r);
	DECLARE_WRITE16_MEMBER(eaglshot_gfxrom_w);
	DECLARE_READ16_MEMBER(eaglshot_trackball_r);
	DECLARE_WRITE16_MEMBER(eaglshot_trackball_w);
	DECLARE_READ16_MEMBER(eaglshot_gfxram_r);
	DECLARE_WRITE16_MEMBER(eaglshot_gfxram_w);
	DECLARE_WRITE16_MEMBER(gdfs_tmapram_w);
	DECLARE_READ16_MEMBER(ssv_vblank_r);
	DECLARE_WRITE16_MEMBER(ssv_scroll_w);
	DECLARE_WRITE16_MEMBER(paletteram16_xrgb_swap_word_w);
	DECLARE_READ16_MEMBER(gdfs_eeprom_r);
	DECLARE_WRITE16_MEMBER(gdfs_eeprom_w);
	DECLARE_DRIVER_INIT(gdfs);
	DECLARE_DRIVER_INIT(sxyreac2);
	DECLARE_DRIVER_INIT(hypreac2);
	DECLARE_DRIVER_INIT(hypreact);
	DECLARE_DRIVER_INIT(dynagear);
	DECLARE_DRIVER_INIT(eaglshot);
	DECLARE_DRIVER_INIT(srmp4);
	DECLARE_DRIVER_INIT(srmp7);
	DECLARE_DRIVER_INIT(keithlcy);
	DECLARE_DRIVER_INIT(meosism);
	DECLARE_DRIVER_INIT(vasara);
	DECLARE_DRIVER_INIT(cairblad);
	DECLARE_DRIVER_INIT(sxyreact);
	DECLARE_DRIVER_INIT(janjans1);
	DECLARE_DRIVER_INIT(ryorioh);
	DECLARE_DRIVER_INIT(drifto94);
	DECLARE_DRIVER_INIT(survarts);
	DECLARE_DRIVER_INIT(ultrax);
	DECLARE_DRIVER_INIT(stmblade);
	DECLARE_DRIVER_INIT(jsk);
	DECLARE_DRIVER_INIT(twineag2);
	DECLARE_DRIVER_INIT(mslider);
	TILE_GET_INFO_MEMBER(get_tile_info_0);
	virtual void machine_reset();
	virtual void video_start();
	DECLARE_VIDEO_START(gdfs);
	DECLARE_VIDEO_START(eaglshot);
	UINT32 screen_update_ssv(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_gdfs(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_eaglshot(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_DEVICE_CALLBACK_MEMBER(ssv_interrupt);
	TIMER_DEVICE_CALLBACK_MEMBER(gdfs_interrupt);
};

/*----------- defined in video/ssv.c -----------*/
void ssv_enable_video(running_machine &machine, int enable);
