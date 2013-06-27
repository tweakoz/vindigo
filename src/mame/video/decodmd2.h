/*
 *  Data East Pinball DMD Type 2 Display
 */

#ifndef DECODMD_H_
#define DECODMD_H_

#include "emu.h"
#include "cpu/m6809/m6809.h"
#include "video/mc6845.h"
#include "machine/ram.h"

#define MCFG_DECODMD_TYPE2_ADD(_tag, _intrf) \
	MCFG_DEVICE_ADD(_tag, DECODMD2, 0) \
	MCFG_DEVICE_CONFIG(_intrf)

#define START_ADDRESS       (((m_crtc_reg[0x0c]<<8) & 0x3f00) | (m_crtc_reg[0x0d] & 0xff))

struct decodmd_intf
{
	const char* m_romregion;  // region containing display ROM
};

class decodmd_type2_device : public device_t,
							 public decodmd_intf
{
public:
	decodmd_type2_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	required_device<cpu_device> m_cpu;
	required_device<mc6845_device> m_mc6845;
	required_memory_bank m_rombank1;
	required_memory_bank m_rombank2;
	required_memory_bank m_rambank;
	required_device<ram_device> m_ram;
	memory_region* m_rom;

	UINT32 screen_update( screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect );

	DECLARE_WRITE8_MEMBER(bank_w);
	DECLARE_WRITE8_MEMBER(crtc_address_w);
	DECLARE_WRITE8_MEMBER(crtc_register_w);
	DECLARE_READ8_MEMBER(crtc_status_r);
	DECLARE_READ8_MEMBER(latch_r);
	DECLARE_WRITE8_MEMBER(data_w);
	DECLARE_READ8_MEMBER(busy_r);
	DECLARE_WRITE8_MEMBER(ctrl_w);
	DECLARE_READ8_MEMBER(ctrl_r);
	DECLARE_READ8_MEMBER(status_r);
	DECLARE_WRITE8_MEMBER(status_w);
	TIMER_DEVICE_CALLBACK_MEMBER(dmd_firq);

protected:
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual void device_start();
	virtual void device_reset();
	virtual void device_config_complete();

private:
	UINT8 m_crtc_index;
	UINT8 m_crtc_reg[0x100];
	UINT8 m_latch;
	UINT8 m_status;
	UINT8 m_ctrl;
	UINT8 m_busy;
	UINT8 m_command;
};

extern const device_type DECODMD2;

#endif /* DECODMD_H_ */
