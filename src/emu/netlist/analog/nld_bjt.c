/*
 * nld_bjt.c
 *
 */

#include "nld_bjt.h"
#include "../nl_setup.h"
#include "nld_solver.h"

class diode
{
public:
    diode() : m_Is(1e-15), m_VT(0.0258), m_VT_inv(1.0 / m_VT) {}
    diode(const double Is, const double n)
    {
        m_Is = Is;
        m_VT = 0.0258 * n;
        m_VT_inv = 1.0 / m_VT;
    }
    void set(const double Is, const double n)
    {
        m_Is = Is;
        m_VT = 0.0258 * n;
        m_VT_inv = 1.0 / m_VT;
    }
    double I(const double V) const { return m_Is * exp(V * m_VT_inv) - m_Is; }
    double g(const double V) const { return m_Is * m_VT_inv * exp(V * m_VT_inv); }
    double V(const double I) const { return log(1.0 + I / m_Is) * m_VT; }
    double gI(const double I) const { return m_VT_inv * (I + m_Is); }

private:
    double m_Is;
    double m_VT;
    double m_VT_inv;
};



// ----------------------------------------------------------------------------------------
// nld_Q
// ----------------------------------------------------------------------------------------

NETLIB_START(Q)
{
    register_param("model", m_model, "");
}

template <NETLIB_NAME(Q)::q_type _type>
NETLIB_START(QBJT_switch<_type>)
{
    NETLIB_NAME(Q)::start();

    register_sub(m_RB, "RB");
    register_sub(m_RC, "RC");
    register_input("BV", m_BV);
    register_input("EV", m_EV);

    register_subalias("B", m_RB.m_P);
    register_subalias("E", m_RB.m_N);
    register_subalias("C", m_RC.m_P);

    setup().connect(m_RB.m_N, m_RC.m_N);
    setup().connect(m_RB.m_P, m_BV);
    setup().connect(m_RB.m_N, m_EV);

    save(NAME(m_state_on));

    m_RB.set(NETLIST_GMIN, 0.0, 0.0);
    m_RC.set(NETLIST_GMIN, 0.0, 0.0);

    m_state_on = 0;

}

NETLIB_UPDATE(Q)
{
    netlist().solver()->schedule();
}

template <NETLIB_NAME(Q)::q_type _type>
NETLIB_UPDATE_PARAM(QBJT_switch<_type>)
{
    double IS = m_model.dValue("IS", 1e-15);
    double BF = m_model.dValue("BF", 100);
    double NF = m_model.dValue("NF", 1);
    //double VJE = m_model.dValue("VJE", 0.75);

    double alpha = BF / (1.0 + BF);

    diode d(IS, NF);

    // Assume 5mA Collector current for switch operation

    m_V = d.V(0.005 / alpha);

    /* Base current is 0.005 / beta
     * as a rough estimate, we just scale the conductance down */

    m_gB = d.gI(0.005 / alpha) / BF;

    if (m_gB < NETLIST_GMIN)
        m_gB = NETLIST_GMIN;
    m_gC = BF * m_gB; // very rough estimate
    //printf("%f %f \n", m_V, m_gB);
    m_RB.set(NETLIST_GMIN, 0.0, 0.0);
    m_RC.set(NETLIST_GMIN, 0.0, 0.0);
}

template NETLIB_START(QBJT_switch<NETLIB_NAME(Q)::BJT_NPN>);
template NETLIB_START(QBJT_switch<NETLIB_NAME(Q)::BJT_PNP>);
template NETLIB_UPDATE_PARAM(QBJT_switch<NETLIB_NAME(Q)::BJT_NPN>);
template NETLIB_UPDATE_PARAM(QBJT_switch<NETLIB_NAME(Q)::BJT_PNP>);

// ----------------------------------------------------------------------------------------
// nld_Q - Ebers Moll
// ----------------------------------------------------------------------------------------

template <NETLIB_NAME(Q)::q_type _type>
NETLIB_START(QBJT_EB<_type>)
{
    NETLIB_NAME(Q)::start();

    register_terminal("B", m_D_BE.m_P);   // Anode
    register_terminal("E", m_D_BE.m_N);   // Cathode

    register_terminal("_B1", m_D_BC.m_P); // Anode
    register_terminal("C", m_D_BC.m_N);   // Cathode

    register_terminal("_B2", m_I_BE.m_P);
    register_terminal("_E2", m_I_BE.m_N);

    register_terminal("_B3", m_I_BC.m_P);
    register_terminal("_C1", m_I_BC.m_N);

    setup().connect(m_D_BE.m_P, m_D_BC.m_P);
    setup().connect(m_D_BE.m_P, m_I_BE.m_P);
    setup().connect(m_D_BE.m_P, m_I_BC.m_P);

    setup().connect(m_D_BE.m_N, m_I_BE.m_N);
    setup().connect(m_D_BC.m_N, m_I_BC.m_N);

    m_gD_BE.save("m_D_BE", *this);
    m_gD_BC.save("m_D_BC", *this);

}


template <NETLIB_NAME(Q)::q_type _type>
NETLIB_UPDATE_PARAM(QBJT_EB<_type>)
{
    double IS = m_model.dValue("IS", 1e-15);
    double BF = m_model.dValue("BF", 100);
    double NF = m_model.dValue("NF", 1);
    double BR = m_model.dValue("BR", 1);
    double NR = m_model.dValue("NR", 1);
    //double VJE = m_model.dValue("VJE", 0.75);

    m_alpha_f = BF / (1.0 + BF);
    m_alpha_r = BR / (1.0 + BR);

    m_gD_BE.set_param(IS / m_alpha_f, NF);
    m_gD_BC.set_param(IS / m_alpha_r, NR);

}

template NETLIB_START(QBJT_EB<NETLIB_NAME(Q)::BJT_NPN>);
template NETLIB_START(QBJT_EB<NETLIB_NAME(Q)::BJT_PNP>);
template NETLIB_UPDATE_PARAM(QBJT_EB<NETLIB_NAME(Q)::BJT_NPN>);
template NETLIB_UPDATE_PARAM(QBJT_EB<NETLIB_NAME(Q)::BJT_PNP>);

