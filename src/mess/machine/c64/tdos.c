/**********************************************************************

    Triton QD TDOS cartridge emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************/

/*

    PCB Layout
    ----------

    XM-2206-A (top)
    XM-2205-A (bottom)

                |===========================|
                |            CN4            |
                |                           |
                |      ULA                  |
                |                           |
                |      6.5MHz               |
                |                           |
                |      SSDA                 |
                |                 LS175     |
                |                 LS367     |
                |            CN3            |
                |=========|||||||||=========|
                          |||||||||
            |=============|||||||||============|
            |                CN2               |
            |       LS00     LS02     LS138    |
    |=======|                                  |
    |=|             LS245        ROM           |
    |=|                                        |
    |=|                                        |
    |=|                                     CN1|
    |=|                                        |
    |=|                                        |
    |=|                                        |
    |=|                             SW1        |
    |==========================================|

    ROM    - Hitachi HN482764G 8Kx8 EPROM "TDOS 1.2"
    ULA    - Ferranti ULA5RB073E1 XZ-2085-1 40-pin custom ULA
    SSDA   - Motorola MC68A52P SSDA
    CN1    - C64 expansion connector (pass-thru)
    CN2,3  - 19x1 flat ribbon cable to other PCB
    CN4    - 9 wires to 3" drive
    SW1    - cartridge on/off switch


	Drive cable pinout
	------------------
    1    WP
    2    WD
    3    WG
    4    MO
    5    RD
    6    RY
    7    MS
    8    RS
    9    +5V
    10   GND

*/

#include "tdos.h"



//**************************************************************************
//  MACROS/CONSTANTS
//**************************************************************************

#define MC68A52P_TAG        "mc6852"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

const device_type C64_TDOS = &device_creator<c64_tdos_cartridge_device>;


//-------------------------------------------------
//  MC6852_INTERFACE( ssda_intf )
//-------------------------------------------------

static MC6852_INTERFACE( ssda_intf )
{
	0,
	0,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_LINE_GND,
	DEVCB_LINE_GND,
	DEVCB_NULL,
	DEVCB_NULL
};


//-------------------------------------------------
//  MACHINE_CONFIG_FRAGMENT( c64_multiscreen )
//-------------------------------------------------

static MACHINE_CONFIG_FRAGMENT( c64_tdos )
	MCFG_MC6852_ADD(MC68A52P_TAG, XTAL_6_5MHz, ssda_intf)

	MCFG_C64_PASSTHRU_EXPANSION_SLOT_ADD()
MACHINE_CONFIG_END


//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor c64_tdos_cartridge_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( c64_tdos );
}


//-------------------------------------------------
//  INPUT_PORTS( c64_tdos )
//-------------------------------------------------

static INPUT_PORTS_START( c64_tdos )
	PORT_START("SW1")
	PORT_DIPNAME( 0x01, 0x01, "Enabled" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
INPUT_PORTS_END


//-------------------------------------------------
//  input_ports - device-specific input ports
//-------------------------------------------------

ioport_constructor c64_tdos_cartridge_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( c64_tdos );
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  c64_tdos_cartridge_device - constructor
//-------------------------------------------------

c64_tdos_cartridge_device::c64_tdos_cartridge_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	device_t(mconfig, C64_TDOS, "C64 TDOS cartridge", tag, owner, clock, "c64_tdos", __FILE__),
	device_c64_expansion_card_interface(mconfig, *this),
	m_ssda(*this, MC68A52P_TAG),
	m_exp(*this, C64_EXPANSION_SLOT_TAG),
	m_sw1(*this, "SW1")
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void c64_tdos_cartridge_device::device_start()
{
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void c64_tdos_cartridge_device::device_reset()
{
	m_ssda->reset();
	//m_ula->reset();
	//flip-flop reset

	m_enabled = m_sw1->read();
}


//-------------------------------------------------
//  c64_cd_r - cartridge data read
//-------------------------------------------------

UINT8 c64_tdos_cartridge_device::c64_cd_r(address_space &space, offs_t offset, UINT8 data, int sphi2, int ba, int roml, int romh, int io1, int io2)
{
	data = m_exp->cd_r(space, offset, data, sphi2, ba, roml, romh, io1, io2);

	if (m_enabled && !roml)
	{
		data = m_roml[offset & 0x1fff];
	}

	if (m_enabled && !io2)
	{
		switch (offset >> 1)
		{
		case 0:
			data = m_ssda->read(space, offset & 0x01);
			break;

		case 1:
			break;

		case 2:
			/*
			
			    bit     description
			
			    0       
			    1       
			    2       
			    3       
			    4       
			    5       drive MS
			    6       drive WP
			    7       drive RY
			
			*/
			break;
		}
	}

	return data;
}


//-------------------------------------------------
//  c64_cd_w - cartridge data write
//-------------------------------------------------

void c64_tdos_cartridge_device::c64_cd_w(address_space &space, offs_t offset, UINT8 data, int sphi2, int ba, int roml, int romh, int io1, int io2)
{
	m_exp->cd_w(space, offset, data, sphi2, ba, roml, romh, io1, io2);

	if (m_enabled && !io2)
	{
		switch (offset >> 1)
		{
		case 0:
			m_ssda->write(space, offset & 0x01, data);
			break;

		case 1:
			/*
			
			    bit     description
			
			    0       
			    1       
			    2       
			    3       
			    4       
			    5       ULA pin 8, inverted
			    6       drive MO
			    7       ULA pin 15
			
			*/
			break;

		case 2:
			break;
		}
	}
}


//-------------------------------------------------
//  c64_game_r - GAME read
//-------------------------------------------------

int c64_tdos_cartridge_device::c64_game_r(offs_t offset, int sphi2, int ba, int rw)
{
	return m_enabled ? 1 : m_exp->game_r(offset, sphi2, ba, rw, m_slot->hiram());
}


//-------------------------------------------------
//  c64_exrom_r - EXROM read
//-------------------------------------------------

int c64_tdos_cartridge_device::c64_exrom_r(offs_t offset, int sphi2, int ba, int rw)
{
	return m_enabled ? 0 : m_exp->exrom_r(offset, sphi2, ba, rw, m_slot->hiram());
}
