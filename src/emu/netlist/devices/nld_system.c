/*
 * nld_system.c
 *
 */

#include "nld_system.h"
//#include "nld_twoterm.h"

// ----------------------------------------------------------------------------------------
// netdev_const
// ----------------------------------------------------------------------------------------

NETLIB_START(ttl_const)
{
	register_output("Q", m_Q);
	register_param("CONST", m_const, 0);
}

NETLIB_UPDATE(ttl_const)
{
    OUTLOGIC(m_Q, m_const.Value(), NLTIME_IMMEDIATE);
}

NETLIB_UPDATE_PARAM(ttl_const)
{
    OUTLOGIC(m_Q, m_const.Value(), NLTIME_IMMEDIATE);
}

NETLIB_START(analog_const)
{
	register_output("Q", m_Q);
	register_param("CONST", m_const, 0.0);
}

NETLIB_UPDATE(analog_const)
{
    OUTANALOG(m_Q, m_const.Value(), NLTIME_IMMEDIATE);
}

NETLIB_UPDATE_PARAM(analog_const)
{
	//m_Q.initial(m_const.Value());
    OUTANALOG(m_Q, m_const.Value(), NLTIME_IMMEDIATE);
}

// ----------------------------------------------------------------------------------------
// clock
// ----------------------------------------------------------------------------------------

NETLIB_START(clock)
{
	register_output("Q", m_Q);
	//register_input("FB", m_feedback);

	register_param("FREQ", m_freq, 7159000.0 * 5.0);
	m_inc = netlist_time::from_hz(m_freq.Value()*2);

	register_link_internal(m_feedback, m_Q, netlist_input_t::STATE_INP_ACTIVE);

}

NETLIB_UPDATE_PARAM(clock)
{
	m_inc = netlist_time::from_hz(m_freq.Value()*2);
}

NETLIB_UPDATE(clock)
{
	OUTLOGIC(m_Q, !m_Q.net().new_Q(), m_inc  );
}

// ----------------------------------------------------------------------------------------
// logic_input
// ----------------------------------------------------------------------------------------

NETLIB_START(logic_input)
{
	register_output("Q", m_Q);
	register_param("OUT", m_OUT, 0);
}

NETLIB_UPDATE(logic_input)
{
    OUTLOGIC(m_Q, m_OUT.Value() & 1, NLTIME_IMMEDIATE);
}

NETLIB_UPDATE_PARAM(logic_input)
{
	OUTLOGIC(m_Q, m_OUT.Value() & 1, NLTIME_IMMEDIATE);
}

// ----------------------------------------------------------------------------------------
// analog_input
// ----------------------------------------------------------------------------------------

NETLIB_START(analog_input)
{
	register_output("Q", m_Q);
    register_param("OUT", m_OUT, 0.0);
}

NETLIB_UPDATE(analog_input)
{
    OUTANALOG(m_Q, m_OUT.Value(), NLTIME_IMMEDIATE);
}

NETLIB_UPDATE_PARAM(analog_input)
{
    OUTANALOG(m_Q, m_OUT.Value(), NLTIME_IMMEDIATE);
}
