/***************************************************************************

Template for skeleton device

***************************************************************************/

#pragma once

#ifndef __seibu_copDEV_H__
#define __seibu_copDEV_H__



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_SEIBU_COP_ADD(_tag,_freq) \
	MCFG_DEVICE_ADD(_tag, SEIBU_COP, _freq) \


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> seibu_cop_device

class seibu_cop_device :	public device_t
{
public:
	// construction/destruction
	seibu_cop_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// I/O operations
	DECLARE_WRITE8_MEMBER( write );
	DECLARE_READ8_MEMBER( read );

protected:
	// device-level overrides
	virtual void device_validity_check(validity_checker &valid) const;
	virtual void device_start();
	virtual void device_reset();
};


// device type definition
extern const device_type SEIBU_COP;



//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************



#endif
