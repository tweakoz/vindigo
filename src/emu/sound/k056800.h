/*********************************************************

    Konami 056800 MIRAC sound interface

*********************************************************/

#ifndef __K056800_H__
#define __K056800_H__


/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/

#define MCFG_K056800_ADD(_tag, _clock) \
	MCFG_DEVICE_ADD(_tag, K056800, _clock) \

#define MCFG_K056800_INT_HANDLER(_devcb) \
	devcb = &k056800_device::set_int_handler(*device, DEVCB2_##_devcb);



/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

class k056800_device : public device_t
{
public:
	// construction/destruction
	k056800_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// static configuration helpers
	template<class _Object> static devcb2_base &set_int_handler(device_t &device, _Object object) { return downcast<k056800_device &>(device).m_int_handler.set_callback(object); }

	DECLARE_READ8_MEMBER( host_r );
	DECLARE_WRITE8_MEMBER( host_w );
	DECLARE_READ8_MEMBER( sound_r );
	DECLARE_WRITE8_MEMBER( sound_w );

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

private:
	// internal state
	bool				m_int_pending;
	bool				m_int_enabled;
	UINT8				m_host_to_snd_regs[4];
	UINT8				m_snd_to_host_regs[2];

	devcb2_write_line	m_int_handler;
};

extern const device_type K056800;



#endif /* __K056800_H__ */
