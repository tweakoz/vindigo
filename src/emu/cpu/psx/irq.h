// license:MAME
// copyright-holders:smf
/*
 * PlayStation IRQ emulator
 *
 * Copyright 2003-2011 smf
 *
 */

#pragma once

#ifndef __PSXIRQ_H__
#define __PSXIRQ_H__

#include "emu.h"

extern const device_type PSX_IRQ;

#define MCFG_PSX_IRQ_HANDLER(_devcb) \
	devcb = &psxirq_device::set_irq_handler(*device, DEVCB2_##_devcb);

class psxirq_device : public device_t
{
public:
	psxirq_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// static configuration helpers
	template<class _Object> static devcb2_base &set_irq_handler(device_t &device, _Object object) { return downcast<psxirq_device &>(device).m_irq_handler.set_callback(object); }

	DECLARE_READ32_MEMBER( read );
	DECLARE_WRITE32_MEMBER( write );

	DECLARE_WRITE_LINE_MEMBER( intin0 );
	DECLARE_WRITE_LINE_MEMBER( intin1 );
	DECLARE_WRITE_LINE_MEMBER( intin2 );
	DECLARE_WRITE_LINE_MEMBER( intin3 );
	DECLARE_WRITE_LINE_MEMBER( intin4 );
	DECLARE_WRITE_LINE_MEMBER( intin5 );
	DECLARE_WRITE_LINE_MEMBER( intin6 );
	DECLARE_WRITE_LINE_MEMBER( intin7 );
	DECLARE_WRITE_LINE_MEMBER( intin8 );
	DECLARE_WRITE_LINE_MEMBER( intin9 );
	DECLARE_WRITE_LINE_MEMBER( intin10 );

protected:
	virtual void device_start();
	virtual void device_reset();
	virtual void device_post_load();

private:
	void psx_irq_update( void );
	void set( UINT32 bitmask );

	UINT32 n_irqdata;
	UINT32 n_irqmask;

	devcb2_write_line m_irq_handler;
};

#endif
