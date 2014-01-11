/*
 * nld_twoterm.c
 *
 */

#include "nld_twoterm.h"
#include "nld_solver.h"

// ----------------------------------------------------------------------------------------
// nld_twoterm
// ----------------------------------------------------------------------------------------

ATTR_COLD NETLIB_NAME(twoterm)::NETLIB_NAME(twoterm)(const family_t afamily) :
		netlist_device_t(afamily)
{
	m_P.m_otherterm = &m_N;
	m_N.m_otherterm = &m_P;
}

NETLIB_START(twoterm)
{
}

NETLIB_UPDATE(twoterm)
{
	/* only called if connected to a rail net ==> notify the solver to recalculate */
#if !(USE_ALTERNATE_SCHEDULING)
	netlist().solver()->schedule1();
#else
    /* we only need to call the non-rail terminal */
    m_P.net().solve();
    m_N.net().solve();
#endif
}

// ----------------------------------------------------------------------------------------
// nld_R
// ----------------------------------------------------------------------------------------

NETLIB_START(R_base)
{
	register_terminal("1", m_P);
	register_terminal("2", m_N);
}

NETLIB_UPDATE(R_base)
{
	NETLIB_NAME(twoterm)::update();
}

NETLIB_START(R)
{
	NETLIB_NAME(R_base)::start();
	register_param("R", m_R, 1.0 / NETLIST_GMIN);
}

NETLIB_UPDATE(R)
{
	NETLIB_NAME(twoterm)::update();
}

NETLIB_UPDATE_PARAM(R)
{
	//printf("updating %s to %f\n", name().cstr(), m_R.Value());
	set_R(m_R.Value());
    if (USE_ALTERNATE_SCHEDULING)
        update_dev();
}

// ----------------------------------------------------------------------------------------
// nld_POT
// ----------------------------------------------------------------------------------------

NETLIB_START(POT)
{
	register_sub(m_R1, "R1");
	register_sub(m_R2, "R2");

	register_subalias("1", m_R1.m_P);
	register_subalias("2", m_R1.m_N);
	register_subalias("3", m_R2.m_N);

	setup().connect(m_R2.m_P, m_R1.m_N);

	register_param("R", m_R, 1.0 / NETLIST_GMIN);
	register_param("DIAL", m_Dial, 0.5);
	register_param("DIALLOG", m_DialIsLog, 0);

}

NETLIB_UPDATE(POT)
{
	m_R1.update_dev();
	m_R2.update_dev();
}

NETLIB_UPDATE_PARAM(POT)
{
	double v = m_Dial.Value();
	if (m_DialIsLog.Value())
		v = (exp(v) - 1.0) / (exp(1.0) - 1.0);
	m_R1.set_R(MAX(m_R.Value() * v, NETLIST_GMIN));
	m_R2.set_R(MAX(m_R.Value() * (1.0 - v), NETLIST_GMIN));
	// force a schedule all
	if (USE_ALTERNATE_SCHEDULING)
	{
	    m_R1.update_dev();
	    m_R2.update_dev();
	}
}

// ----------------------------------------------------------------------------------------
// nld_C
// ----------------------------------------------------------------------------------------

NETLIB_START(C)
{
	register_terminal("1", m_P);
	register_terminal("2", m_N);

	register_param("C", m_C, 1e-6);

	// set up the element
    set(m_C.Value() / 1e-9, 0.0, 0.0);
}

NETLIB_UPDATE_PARAM(C)
{
	//step_time(1e-9);
}

NETLIB_UPDATE(C)
{
	NETLIB_NAME(twoterm)::update();
}

// ----------------------------------------------------------------------------------------
// nld_D
// ----------------------------------------------------------------------------------------

NETLIB_START(D)
{
	register_terminal("A", m_P);
	register_terminal("K", m_N);
	register_param("model", m_model, "");

	m_D.save("m_D", *this);

}


NETLIB_UPDATE_PARAM(D)
{
	double Is = m_model.model_value("Is", 1e-15);
	double n = m_model.model_value("N", 1);

	m_D.set_param(Is, n);
}

NETLIB_UPDATE(D)
{
	NETLIB_NAME(twoterm)::update();
}


