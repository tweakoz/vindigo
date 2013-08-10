/***************************************************************************

    Intel 8089 I/O Processor

***************************************************************************/

#pragma once

#ifndef __I8089_H__
#define __I8089_H__

#include "emu.h"


//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_I8089_ADD(_tag, _clock, _cputag) \
	MCFG_DEVICE_ADD(_tag, I8089, _clock) \
	i8089_device::static_set_cputag(*device, _cputag);

#define MCFG_I8089_SINTR1(_sintr1) \
	downcast<i8089_device *>(device)->set_sintr1_callback(DEVCB2_##_sintr1);

#define MCFG_I8089_SINTR2(_sintr2) \
	downcast<i8089_device *>(device)->set_sintr2_callback(DEVCB2_##_sintr2);


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// forward declaration
class i8089_channel;

// ======================> i8089_device

class i8089_device : public device_t
{
public:
	// construction/destruction
	i8089_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// callbacks
	template<class _sintr1> void set_sintr1_callback(_sintr1 sintr1) { m_write_sintr1.set_callback(sintr1); }
	template<class _sintr2> void set_sintr2_callback(_sintr2 sintr2) { m_write_sintr2.set_callback(sintr2); }

	// input lines
	DECLARE_WRITE_LINE_MEMBER( ca_w );
	DECLARE_WRITE_LINE_MEMBER( sel_w );
	DECLARE_WRITE_LINE_MEMBER( drq1_w );
	DECLARE_WRITE_LINE_MEMBER( drq2_w );
	DECLARE_WRITE_LINE_MEMBER( ext1_w );
	DECLARE_WRITE_LINE_MEMBER( ext2_w );

	// inline configuration
	static void static_set_cputag(device_t &device, const char *tag);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;

private:
	required_device<i8089_channel> m_ch1;
	required_device<i8089_channel> m_ch2;

	devcb2_write_line m_write_sintr1;
	devcb2_write_line m_write_sintr2;

	void initialize();
	UINT16 read_word(offs_t address);

	// internal state
	const char *m_cputag;

	address_space *m_mem;
	address_space *m_io;

	// system configuration
	bool m_initialized;
	bool m_16bit_system;
	bool m_16bit_remote;
	bool m_master;
	bool m_request_grant;

	// control block location
	offs_t m_control_block;

	// state of input pins
	int m_ca;
	int m_sel;
};


// device type definition
extern const device_type I8089;


#endif  /* __I8089_H__ */
