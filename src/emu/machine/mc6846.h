/**********************************************************************

  Copyright (C) Antoine Mine' 2006

  Motorola 6846 timer emulation.

**********************************************************************/

#ifndef MC6846_H
#define MC6846_H


/* ---------- configuration ------------ */

struct mc6846_interface
{
	/* CPU write to the outside through chip */
	devcb_write8 m_out_port_cb;  /* 8-bit output */
	devcb_write8 m_out_cp1_cb;   /* 1-bit output */
	devcb_write8 m_out_cp2_cb;   /* 1-bit output */

	/* CPU read from the outside through chip */
	devcb_read8 m_in_port_cb; /* 8-bit input */

	/* asynchronous timer output to outside world */
	devcb_write8 m_out_cto_cb; /* 1-bit output */

	/* timer interrupt */
	devcb_write_line m_irq_cb;
};


class mc6846_device : public device_t,
									public mc6846_interface
{
public:
	mc6846_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~mc6846_device() {}

	/* interface to CPU via address/data bus*/
	DECLARE_READ8_MEMBER(read);
	DECLARE_WRITE8_MEMBER(write);

	/* asynchronous write from outside world into interrupt-generating pins */
	void set_input_cp1(int data);
	void set_input_cp2(int data);

	/* polling from outside world */
	UINT8  get_output_port();
	UINT8  get_output_cto();
	UINT8  get_output_cp2();

	/* partial access to internal state */
	UINT16 get_preset(); /* timer interval - 1 in us */
	
protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();

private:
	// internal state
	
	/* registers */
	UINT8    m_csr;      /* 0,4: combination status register */
	UINT8    m_pcr;      /* 1:   peripheral control register */
	UINT8    m_ddr;      /* 2:   data direction register */
	UINT8    m_pdr;      /* 3:   peripheral data register (last cpu write) */
	UINT8    m_tcr;      /* 5:   timer control register */

	/* lines */
	UINT8 m_cp1;         /* 1-bit input */
	UINT8 m_cp2;         /* 1-bit input/output: last external write */
	UINT8 m_cp2_cpu;     /* last cpu write */
	UINT8 m_cto;         /* 1-bit timer output (unmasked) */

	/* internal state */
	UINT8  m_time_MSB; /* MSB buffer register */
	UINT8  m_csr0_to_be_cleared;
	UINT8  m_csr1_to_be_cleared;
	UINT8  m_csr2_to_be_cleared;
	UINT16 m_latch;   /* timer latch */
	UINT16 m_preset;  /* preset value */
	UINT8  m_timer_started;

	/* timers */
	emu_timer *m_interval; /* interval programmable timer */
	emu_timer *m_one_shot; /* 1-us x factor one-shot timer */

	/* CPU write to the outside through chip */
	devcb_resolved_write8 m_out_port_func;  /* 8-bit output */
	devcb_resolved_write8 m_out_cp1_func;   /* 1-bit output */
	devcb_resolved_write8 m_out_cp2_func;   /* 1-bit output */

	/* CPU read from the outside through chip */
	devcb_resolved_read8 m_in_port_func; /* 8-bit input */

	/* asynchronous timer output to outside world */
	devcb_resolved_write8 m_out_cto_func; /* 1-bit output */

	/* timer interrupt */
	devcb_resolved_write_line m_irq_func;

	int m_old_cif;
	int m_old_cto;
	
	inline UINT16 counter();
	inline void update_irq();
	inline void update_cto();
	inline void timer_launch();
	
	TIMER_CALLBACK_MEMBER(timer_expire);
	TIMER_CALLBACK_MEMBER(timer_one_shot);
};

extern const device_type MC6846;

#define MCFG_MC6846_ADD(_tag, _intrf) \
	MCFG_DEVICE_ADD(_tag, MC6846, 0)          \
	MCFG_DEVICE_CONFIG(_intrf)

#define MCFG_MC6846_MODIFY(_tag, _intrf) \
	MCFG_DEVICE_MODIFY(_tag)          \
	MCFG_DEVICE_CONFIG(_intrf)

#define MCFG_MC6846_REMOVE(_tag)        \
	MCFG_DEVICE_REMOVE(_tag)

#endif
