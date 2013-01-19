/* PAD emulation */

#include "machine/psxcport.h"
#include "machine/psxanalog.h"

const device_type PSX_CONTROLLER_PORT = &device_creator<psx_controller_port_device>;

psx_controller_port_device::psx_controller_port_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
		device_t(mconfig, PSX_CONTROLLER_PORT, "Playstation Controller Port", tag, owner, clock),
		device_slot_interface(mconfig, *this),
		m_card(*this, "card")
{
}

void psx_controller_port_device::device_config_complete()
{
	m_dev = dynamic_cast<device_psx_controller_interface *>(get_card_device());
}

static MACHINE_CONFIG_FRAGMENT( psx_memory_card )
	MCFG_PSXCARD_ADD("card")
MACHINE_CONFIG_END

machine_config_constructor psx_controller_port_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( psx_memory_card );
}

const device_type PSXCONTROLLERPORTS = &device_creator<psxcontrollerports_device>;

psxcontrollerports_device::psxcontrollerports_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
		psxsiodev_device(mconfig, PSXCONTROLLERPORTS, "PSXCONTROLLERPORTS", tag, owner, clock)
{
}

void psxcontrollerports_device::device_start()
{
	m_port0 = machine().device<psx_controller_port_device>("port1");
	m_port1 = machine().device<psx_controller_port_device>("port2");
	m_port0->setup_ack_cb(psx_controller_port_device::void_cb(FUNC(psxcontrollerports_device::ack), this));
	m_port1->setup_ack_cb(psx_controller_port_device::void_cb(FUNC(psxcontrollerports_device::ack), this));
	psxsiodev_device::device_start();
}

SLOT_INTERFACE_START(psx_controllers)
	SLOT_INTERFACE("digital_pad", PSX_STANDARD_CONTROLLER)
	SLOT_INTERFACE("analog_pad", PSX_ANALOG_CONTROLLER)
SLOT_INTERFACE_END

void psxcontrollerports_device::data_in( int data, int mask )
{
	m_port0->sel_w((data & PSX_SIO_OUT_DTR)?1:0);
	m_port0->tx_w((data & PSX_SIO_OUT_DATA)?1:0);
	m_port0->clock_w((data & PSX_SIO_OUT_CLOCK)?1:0); // clock must be last

	m_port1->tx_w((data & PSX_SIO_OUT_DATA)?1:0);
	m_port1->sel_w((data & PSX_SIO_OUT_DTR)?0:1); // not dtr
	m_port1->clock_w((data & PSX_SIO_OUT_CLOCK)?1:0);

	data_out(((m_port0->rx_r() && m_port1->rx_r()) * PSX_SIO_IN_DATA), PSX_SIO_IN_DATA);
}

void psxcontrollerports_device::ack()
{
	data_out((!(m_port0->ack_r() && m_port1->ack_r()) * PSX_SIO_IN_DSR), PSX_SIO_IN_DSR);
}

device_psx_controller_interface::device_psx_controller_interface(const machine_config &mconfig, device_t &device) :
		device_slot_card_interface(mconfig, device)
{
}

device_psx_controller_interface::~device_psx_controller_interface()
{
}

void device_psx_controller_interface::interface_pre_reset()
{
	m_bit = 0;
	m_count = 0;
	m_idata = 0;
	m_memcard = false;

	m_clock = true;
	m_sel = true;
	m_rx = true;
	m_ack = true;
	m_owner->ack();
}

void device_psx_controller_interface::interface_pre_start()
{
	m_owner = dynamic_cast<psx_controller_port_device *>(device().owner());
	m_ack_timer = device().machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(device_psx_controller_interface::ack_timer), this));
}

void device_psx_controller_interface::ack_timer(void *ptr, int param)
{
	m_ack = param;
	m_owner->ack();

	if(!param)
		m_ack_timer->adjust(attotime::from_usec(2), 1);
}

void device_psx_controller_interface::do_pad()
{
	if(!m_bit)
	{
		if(!m_count)
			m_odata = 0xff;
		m_idata = 0;
	}

	m_rx = (m_odata & (1 << m_bit)) ? true : false;
	m_idata |= (m_owner->tx_r()?1:0) << m_bit;
	m_bit = (m_bit + 1) % 8;

	if(!m_bit)
	{
		if((!m_count) && (m_idata & 0xf0))
		{
				m_memcard = true;
				return;
		}

		if(get_pad(m_count++, &m_odata, m_idata))
			m_ack_timer->adjust(attotime::from_usec(10), 0);
		else
			m_count = 0;
	}
}

void device_psx_controller_interface::sel_w(bool state) {
	if(state && !m_sel)
		interface_pre_reset(); // don't reset the controller, just the interface
	m_sel = state;
}

const device_type PSX_STANDARD_CONTROLLER = &device_creator<psx_standard_controller_device>;

psx_standard_controller_device::psx_standard_controller_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
		device_t(mconfig, PSX_STANDARD_CONTROLLER, "Playstation Standard Controller", tag, owner, clock),
		device_psx_controller_interface(mconfig, *this)
{
}

bool psx_standard_controller_device::get_pad(int count, UINT8 *odata, UINT8 idata)
{
	switch(count)
	{
		case 0:
			*odata = 0x41;
			break;
		case 1:
			if(idata != QUERY_PAD_STATE)
				return false;
			*odata = 0x5a;
			break;
		case 2:
			*odata = ioport("PSXPAD0")->read();
			break;
		case 3:
			*odata = ioport("PSXPAD1")->read();
			break;
		case 4:
			return false;
	}
	return true;
}

static INPUT_PORTS_START( psx_standard_controller )
	PORT_START("PSXPAD0")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SELECT )

	PORT_START("PSXPAD1")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Square")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Cross")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Circle")
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Triangle")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("R1")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("L1")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON7 ) PORT_NAME("R2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON8 ) PORT_NAME("L2")
INPUT_PORTS_END

ioport_constructor psx_standard_controller_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(psx_standard_controller);
}
