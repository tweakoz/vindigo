// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Commodore VIC-20 User Port emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************

                    GND       1      A       GND
                    +5V       2      B       CB1
                 /RESET       3      C       PB0
                   JOY0       4      D       PB1
                   JOY1       5      E       PB2
                   JOY2       6      F       PB3
              LIGHT PEN       7      H       PB4
        CASSETTE SWITCH       8      J       PB5
                    ATN       9      K       PB6
                  +9VAC      10      L       PB7
                  +9VAC      11      M       CB2
                    GND      12      N       GND

**********************************************************************/

#pragma once

#ifndef __VIC20_USER_PORT__
#define __VIC20_USER_PORT__

#include "emu.h"



//**************************************************************************
//  CONSTANTS
//**************************************************************************

#define VIC20_USER_PORT_TAG     "user"



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_VIC20_USER_PORT_ADD(_tag, _slot_intf, _def_slot) \
	MCFG_DEVICE_ADD(_tag, VIC20_USER_PORT, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, false)

#define MCFG_VIC20_USER_PORT_3_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_3_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_4_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_4_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_5_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_5_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_6_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_6_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_7_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_7_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_8_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_8_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_B_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_b_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_C_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_c_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_D_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_d_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_E_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_e_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_F_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_f_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_H_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_h_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_J_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_j_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_K_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_k_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_L_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_l_handler(*device, DEVCB2_##_devcb);

#define MCFG_VIC20_USER_PORT_M_HANDLER(_devcb) \
	devcb = &vic20_user_port_device::set_m_handler(*device, DEVCB2_##_devcb);


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> vic20_user_port_device

class device_vic20_user_port_interface;

class vic20_user_port_device : public device_t,
	public device_slot_interface
{
public:
	// construction/destruction
	vic20_user_port_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	template<class _Object> static devcb2_base &set_3_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_3_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_4_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_4_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_5_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_5_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_6_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_6_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_7_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_7_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_8_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_8_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_b_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_b_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_c_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_c_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_d_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_d_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_e_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_e_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_f_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_f_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_h_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_h_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_j_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_j_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_k_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_k_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_l_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_l_handler.set_callback(object); }
	template<class _Object> static devcb2_base &set_m_handler(device_t &device, _Object object) { return downcast<vic20_user_port_device &>(device).m_m_handler.set_callback(object); }

	// computer interface
	DECLARE_WRITE_LINE_MEMBER( write_7 );
	DECLARE_WRITE_LINE_MEMBER( write_9 );
	DECLARE_WRITE_LINE_MEMBER( write_b );
	DECLARE_WRITE_LINE_MEMBER( write_c );
	DECLARE_WRITE_LINE_MEMBER( write_d );
	DECLARE_WRITE_LINE_MEMBER( write_e );
	DECLARE_WRITE_LINE_MEMBER( write_f );
	DECLARE_WRITE_LINE_MEMBER( write_h );
	DECLARE_WRITE_LINE_MEMBER( write_j );
	DECLARE_WRITE_LINE_MEMBER( write_k );
	DECLARE_WRITE_LINE_MEMBER( write_l );
	DECLARE_WRITE_LINE_MEMBER( write_m );

	// device interface
	devcb2_write_line m_3_handler;
	devcb2_write_line m_4_handler;
	devcb2_write_line m_5_handler;
	devcb2_write_line m_6_handler;
	devcb2_write_line m_7_handler;
	devcb2_write_line m_8_handler;
	devcb2_write_line m_b_handler;
	devcb2_write_line m_c_handler;
	devcb2_write_line m_d_handler;
	devcb2_write_line m_e_handler;
	devcb2_write_line m_f_handler;
	devcb2_write_line m_h_handler;
	devcb2_write_line m_j_handler;
	devcb2_write_line m_k_handler;
	devcb2_write_line m_l_handler;
	devcb2_write_line m_m_handler;

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	device_vic20_user_port_interface *m_card;
};


// ======================> device_vic20_user_port_interface

// class representing interface-specific live vic20_expansion card
class device_vic20_user_port_interface : public device_slot_card_interface
{
public:
	device_vic20_user_port_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_vic20_user_port_interface();

	DECLARE_WRITE_LINE_MEMBER( write_7 ) {}
	DECLARE_WRITE_LINE_MEMBER( write_9 ) {}
	DECLARE_WRITE_LINE_MEMBER( write_b ) {}
	DECLARE_WRITE_LINE_MEMBER( write_c ) {}
	DECLARE_WRITE_LINE_MEMBER( write_d ) {}
	DECLARE_WRITE_LINE_MEMBER( write_e ) {}
	DECLARE_WRITE_LINE_MEMBER( write_f ) {}
	DECLARE_WRITE_LINE_MEMBER( write_h ) {}
	DECLARE_WRITE_LINE_MEMBER( write_j ) {}
	DECLARE_WRITE_LINE_MEMBER( write_k ) {}
	DECLARE_WRITE_LINE_MEMBER( write_l ) {}
	DECLARE_WRITE_LINE_MEMBER( write_m ) {}

protected:
	vic20_user_port_device *m_slot;
};


// device type definition
extern const device_type VIC20_USER_PORT;

SLOT_INTERFACE_EXTERN( vic20_user_port_cards );

#endif
