// license: ?
// copyright-holders: Angelo Salese
/***************************************************************************

Template for skeleton device

***************************************************************************/

#pragma once

#ifndef __MB_VCUDEV_H__
#define __MB_VCUDEV_H__



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_MB_VCU_ADD(_tag,_freq,_config) \
	MCFG_DEVICE_ADD(_tag, MB_VCU, _freq) \
	MCFG_DEVICE_CONFIG(_config)

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> mb_vcu_interface

struct mb_vcu_interface
{
	const char         *m_screen_tag;
};

// ======================> mb_vcu_device

class mb_vcu_device : public device_t,
 					  public device_memory_interface,
					  public device_video_interface,
					  public mb_vcu_interface
{
public:
	// construction/destruction
	mb_vcu_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// I/O operations
	DECLARE_WRITE8_MEMBER( write_vregs );
	DECLARE_READ8_MEMBER( read_ram );
	DECLARE_WRITE8_MEMBER( write_ram );
	DECLARE_READ8_MEMBER( load_params );
	DECLARE_READ8_MEMBER( load_gfx );
	DECLARE_READ8_MEMBER( load_clr );
	DECLARE_WRITE8_MEMBER( background_color_w );
	DECLARE_READ8_MEMBER( status_r );
	DECLARE_WRITE8_MEMBER( vbank_w );

	UINT32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_validity_check(validity_checker &valid) const;
	virtual void device_start();
	virtual void device_reset();
	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const;
private:
	inline UINT16 read_byte(offs_t address);
	inline void write_byte(offs_t address, UINT8 data);

	const address_space_config      m_space_config;
};


// device type definition
extern const device_type MB_VCU;



//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************



#endif
