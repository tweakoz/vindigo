// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_system.h
 *
 * netlist devices defined in the core
 */

#ifndef NLD_SYSTEM_H_
#define NLD_SYSTEM_H_

#include "../nl_setup.h"
#include "../nl_base.h"

// ----------------------------------------------------------------------------------------
// Macros
// ----------------------------------------------------------------------------------------

#define TTL_INPUT(_name, _v)                                                 \
		NET_REGISTER_DEV(ttl_input, _name)                                          \
		PARAM(_name.IN, _v)

#define ANALOG_INPUT(_name, _v)                                              \
		NET_REGISTER_DEV(analog_input, _name)                                       \
		PARAM(_name.IN, _v)

#define MAINCLOCK(_name)                                                     \
		NET_REGISTER_DEV(mainclock, _name)

#define CLOCK(_name)                                                         \
		NET_REGISTER_DEV(clock, _name)

#define NETDEV_GND()                                                                \
        NET_REGISTER_DEV(gnd, GND)

// ----------------------------------------------------------------------------------------
// mainclock
// ----------------------------------------------------------------------------------------

NETLIB_DEVICE_WITH_PARAMS(mainclock,
public:
	netlist_ttl_output_t m_Q;

	netlist_param_double_t m_freq;
	netlist_time m_inc;

	ATTR_HOT inline static void mc_update(netlist_net_t &net, const netlist_time curtime);
);

// ----------------------------------------------------------------------------------------
// clock
// ----------------------------------------------------------------------------------------

NETLIB_DEVICE_WITH_PARAMS(clock,
	netlist_ttl_input_t m_feedback;
	netlist_ttl_output_t m_Q;

	netlist_param_double_t m_freq;
	netlist_time m_inc;
);


// ----------------------------------------------------------------------------------------
// Special support devices ...
// ----------------------------------------------------------------------------------------

NETLIB_DEVICE_WITH_PARAMS(ttl_input,
	netlist_ttl_output_t m_Q;

	netlist_param_logic_t m_IN;
);

NETLIB_DEVICE_WITH_PARAMS(analog_input,
	netlist_analog_output_t m_Q;

    netlist_param_double_t m_IN;
);

// ----------------------------------------------------------------------------------------
// nld_gnd
// ----------------------------------------------------------------------------------------

class nld_gnd : public netlist_device_t
{
public:
    ATTR_COLD nld_gnd()
            : netlist_device_t(GND) { }

    ATTR_COLD virtual ~nld_gnd() {}

protected:

    ATTR_COLD void start()
    {
        register_output("Q", m_Q);
    }

    ATTR_COLD void reset()
    {
        m_Q.initial(0.001); // Make sure update outputs something
    }

    ATTR_HOT ATTR_ALIGN void update()
    {
        OUTANALOG(m_Q, 0.0, NLTIME_IMMEDIATE);
    }

private:
    netlist_analog_output_t m_Q;

};


// ----------------------------------------------------------------------------------------
// netdev_a_to_d
// ----------------------------------------------------------------------------------------

class nld_a_to_d_proxy : public netlist_device_t
{
public:
    ATTR_COLD nld_a_to_d_proxy(netlist_input_t &in_proxied)
            : netlist_device_t()
    {
        assert(in_proxied.family() == LOGIC);
        m_I.m_family_desc = in_proxied.m_family_desc;
    }

    ATTR_COLD virtual ~nld_a_to_d_proxy() {}

    netlist_analog_input_t m_I;
    netlist_ttl_output_t m_Q;

protected:
    ATTR_COLD void start()
    {
        register_input("I", m_I);
        register_output("Q", m_Q);
    }

    ATTR_COLD void reset()
    {
        m_Q.initial(1);
    }

    ATTR_HOT ATTR_ALIGN void update()
    {
        if (m_I.Q_Analog() > m_I.m_family_desc->m_high_thresh_V)
            OUTLOGIC(m_Q, 1, NLTIME_FROM_NS(1));
        else if (m_I.Q_Analog() < m_I.m_family_desc->m_low_thresh_V)
            OUTLOGIC(m_Q, 0, NLTIME_FROM_NS(1));
        //else
        //  OUTLOGIC(m_Q, m_Q.net().last_Q(), NLTIME_FROM_NS(1));
    }

};

// ----------------------------------------------------------------------------------------
// DIODE_to_a
// ----------------------------------------------------------------------------------------

class nld_base_d_to_a_proxy : public netlist_device_t
{
public:
    ATTR_COLD nld_base_d_to_a_proxy(netlist_output_t &out_proxied)
            : netlist_device_t()
    {
        assert(out_proxied.family() == LOGIC);
        m_family_desc = out_proxied.m_family_desc;
    }

    ATTR_COLD virtual ~nld_base_d_to_a_proxy() {}

    ATTR_COLD virtual netlist_core_terminal_t &out() = 0;

    netlist_ttl_input_t m_I;

protected:
    ATTR_COLD void start()
    {
        register_input("I", m_I);
    }

private:
};


class nld_d_to_a_proxy : public nld_base_d_to_a_proxy
{
public:
    ATTR_COLD nld_d_to_a_proxy(netlist_output_t &out_proxied)
            : nld_base_d_to_a_proxy(out_proxied)
    {
    }

    ATTR_COLD virtual ~nld_d_to_a_proxy() {}

protected:
    ATTR_COLD void start()
    {
        nld_base_d_to_a_proxy::start();
        register_output("Q", m_Q);
    }

    ATTR_COLD void reset()
    {
        m_Q.initial(0);
    }

    ATTR_COLD virtual netlist_core_terminal_t &out()
    {
        return m_Q;
    }

    ATTR_HOT ATTR_ALIGN void update()
    {
        OUTANALOG(m_Q, INPLOGIC(m_I) ? m_family_desc->m_high_V : m_family_desc->m_low_V, NLTIME_FROM_NS(1));
    }

private:
    netlist_analog_output_t m_Q;
};

#endif /* NLD_SYSTEM_H_ */
