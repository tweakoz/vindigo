#include "cpu/mn10200/mn10200.h"
#include "cpu/tms57002/tms57002.h"
#include "sound/zsg2.h"

class taito_zoom_device : public device_t

{
public:
	taito_zoom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~taito_zoom_device() {}

	DECLARE_WRITE16_MEMBER(sound_irq_w);
	DECLARE_READ16_MEMBER(sound_irq_r);
	DECLARE_WRITE16_MEMBER(global_volume_w);
	DECLARE_WRITE16_MEMBER(reset_control_w);

	DECLARE_READ8_MEMBER(shared_ram_r);
	DECLARE_WRITE8_MEMBER(shared_ram_w);
	DECLARE_READ8_MEMBER(tms_ctrl_r);
	DECLARE_WRITE8_MEMBER(tms_ctrl_w);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

private:
	
	// devices/pointers
	required_device<mn10200_device> m_soundcpu;
	
	// internal state
	UINT8 m_tms_ctrl;
	UINT8* m_snd_shared_ram;
};

extern const device_type TAITO_ZOOM;

MACHINE_CONFIG_EXTERN( taito_zoom_sound );

#define MCFG_TAITO_ZOOM_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, TAITO_ZOOM, 0)
