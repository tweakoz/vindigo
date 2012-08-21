/**********************************************************************

    RCA VIP ASCII Keyboard Interface VP-620 emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

#include "vp620.h"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type VP620 = &device_creator<vp620_device>;


//-------------------------------------------------
//  ASCII_KEYBOARD_INTERFACE( kb_intf )
//-------------------------------------------------

WRITE8_MEMBER( vp620_device::kb_w )
{
	m_keydata = data;

	m_keystb = ASSERT_LINE;
}

static ASCII_KEYBOARD_INTERFACE( kb_intf )
{
	DEVCB_DEVICE_MEMBER(DEVICE_SELF_OWNER, vp620_device, kb_w)
};


//-------------------------------------------------
//  MACHINE_CONFIG_FRAGMENT( vp620 )
//-------------------------------------------------

static MACHINE_CONFIG_FRAGMENT( vp620 )
	MCFG_ASCII_KEYBOARD_ADD("keyboard", kb_intf)
MACHINE_CONFIG_END


//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor vp620_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( vp620 );
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  vp620_device - constructor
//-------------------------------------------------

vp620_device::vp620_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	device_t(mconfig, VP620, "VP620", tag, owner, clock),
	device_vip_byteio_port_interface(mconfig, *this),
	m_keydata(0),
	m_keystb(CLEAR_LINE)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void vp620_device::device_start()
{
}


//-------------------------------------------------
//  vip_in_r - byte input read
//-------------------------------------------------

UINT8 vp620_device::vip_in_r()
{
	return m_keydata;
}


//-------------------------------------------------
//  vip_ef3_r - EF3 flag read
//-------------------------------------------------

int vp620_device::vip_ef3_r()
{
	int state = m_keystb;

	m_keystb = CLEAR_LINE;

	return state;
}
