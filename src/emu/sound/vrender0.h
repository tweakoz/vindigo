#pragma once

#ifndef __VRENDER0_H__
#define __VRENDER0_H__


//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_SOUND_VRENDER0_ADD(_tag, _clock) \
	MCFG_DEVICE_ADD(_tag, VRENDER0, _clock) \

#define MCFG_SOUND_VRENDER0_REPLACE(_tag, _clock) \
	MCFG_DEVICE_REPLACE(_tag, VRENDER0, _clock) \



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


// ======================> vrender0_device

struct vr0_interface
{
	UINT32 RegBase;
};

void vr0_snd_set_areas(device_t *device, UINT32 *texture, UINT32 *frame);

class vrender0_device : public device_t,
						public device_sound_interface
{
public:
	vrender0_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~vrender0_device() { }

protected:
	// device-level overrides
	virtual void device_start();

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);

public:
	DECLARE_READ32_MEMBER( vr0_snd_read );
	DECLARE_WRITE32_MEMBER( vr0_snd_write );

	void set_areas(UINT32 *texture, UINT32 *frame);

private:
	UINT32 *m_TexBase;
	UINT32 *m_FBBase;
	UINT32 m_SOUNDREGS[0x10000/4];
	sound_stream *m_stream;
	vr0_interface m_Intf;

	void VR0_RenderAudio(int nsamples, stream_sample_t *l, stream_sample_t *r);
};

extern const device_type VRENDER0;


#endif /* __VRENDER0_H__ */
