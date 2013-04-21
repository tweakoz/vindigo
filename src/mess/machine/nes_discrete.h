#ifndef __NES_DISCRETE_H
#define __NES_DISCRETE_H

#include "machine/nes_nxrom.h"


// ======================> nes_74x161x161x32_device

class nes_74x161x161x32_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_74x161x161x32_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();
};


// ======================> nes_74x139x74_device

class nes_74x139x74_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_74x139x74_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_m);

	virtual void pcb_reset();
};


// ======================> nes_74x377_device

class nes_74x377_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_74x377_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();
};


// ======================> nes_74x161x138_device

class nes_74x161x138_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_74x161x138_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_m);

	virtual void pcb_reset();
};



// device type definition
extern const device_type NES_74X161X161X32;
extern const device_type NES_74X139X74;
extern const device_type NES_74X377;
extern const device_type NES_74X161X138;

#endif
