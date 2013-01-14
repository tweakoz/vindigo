/*********************************************************************

    midioutport.c

    MIDI Out serial port - glues the image device to the pluggable serial port

*********************************************************************/

#include "machine/midioutport.h"

const device_type MIDIOUT_PORT = &device_creator<midiout_port_device>;

midiout_port_device::midiout_port_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, MIDIOUT_PORT, "MIDI Out port", tag, owner, clock),
		device_serial_port_interface(mconfig, *this),
		m_midiout(*this, "midiout")
{
}

static MACHINE_CONFIG_FRAGMENT(midiout_port_config)
	MCFG_MIDIOUT_ADD("midiout")
MACHINE_CONFIG_END

machine_config_constructor midiout_port_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME(midiout_port_config);
}
