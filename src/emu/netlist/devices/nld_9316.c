/*
 * nld_9316.c
 *
 */

#include "nld_9316.h"

NETLIB_START(9316)
{
    register_sub(sub, "sub");

    register_subalias("CLK", sub.m_clk);

    register_input("ENP", m_ENP);
    register_input("ENT", m_ENT);
    register_input("CLRQ", m_CLRQ);
    register_input("LOADQ", m_LOADQ);

    register_subalias("A", sub.m_A);
    register_subalias("B", sub.m_B);
    register_subalias("C", sub.m_C);
    register_subalias("D", sub.m_D);

    register_subalias("QA", sub.m_QA);
    register_subalias("QB", sub.m_QB);
    register_subalias("QC", sub.m_QC);
    register_subalias("QD", sub.m_QD);
    register_subalias("RC", sub.m_RC);

}

NETLIB_START(9316_sub)
{
    m_cnt = 0;
    m_loadq = 1;
    m_ent = 1;

    register_input("CLK", m_clk, netlist_input_t::STATE_INP_LH);

    register_input("A", m_A, netlist_input_t::STATE_INP_PASSIVE);
    register_input("B", m_B, netlist_input_t::STATE_INP_PASSIVE);
    register_input("C", m_C, netlist_input_t::STATE_INP_PASSIVE);
    register_input("D", m_D, netlist_input_t::STATE_INP_PASSIVE);

    register_output("QA", m_QA);
    register_output("QB", m_QB);
    register_output("QC", m_QC);
    register_output("QD", m_QD);
    register_output("RC", m_RC);

}

NETLIB_UPDATE(9316_sub)
{
    if (m_loadq)
    {
        m_cnt = ( m_cnt + 1) & 0x0f;
        update_outputs();
    }
    else
    {
        m_cnt = (INPLOGIC_PASSIVE(m_D) << 3) | (INPLOGIC_PASSIVE(m_C) << 2) | (INPLOGIC_PASSIVE(m_B) << 1) | (INPLOGIC_PASSIVE(m_A) << 0);
        update_outputs_all();
    }
    OUTLOGIC(m_RC, m_ent & (m_cnt == 0x0f), NLTIME_FROM_NS(20));
}

NETLIB_UPDATE(9316)
{
    sub.m_loadq = INPLOGIC(m_LOADQ);
    sub.m_ent = INPLOGIC(m_ENT);
    const netlist_sig_t clrq = INPLOGIC(m_CLRQ);

    if ((!sub.m_loadq || (sub.m_ent & INPLOGIC(m_ENP))) & clrq)
    {
        sub.m_clk.activate_lh();
    }
    else
    {
        sub.m_clk.inactivate();
        if (!clrq & (sub.m_cnt>0))
        {
            sub.m_cnt = 0;
            sub.update_outputs();
            OUTLOGIC(sub.m_RC, 0, NLTIME_FROM_NS(20));
            return;
        }
    }
    OUTLOGIC(sub.m_RC, sub.m_ent & (sub.m_cnt == 0x0f), NLTIME_FROM_NS(20));
}

NETLIB_FUNC_VOID(9316_sub, update_outputs_all, (void))
{
    const netlist_time out_delay = NLTIME_FROM_NS(20);
    OUTLOGIC(m_QA, (m_cnt >> 0) & 1, out_delay);
    OUTLOGIC(m_QB, (m_cnt >> 1) & 1, out_delay);
    OUTLOGIC(m_QC, (m_cnt >> 2) & 1, out_delay);
    OUTLOGIC(m_QD, (m_cnt >> 3) & 1, out_delay);
}

NETLIB_FUNC_VOID(9316_sub, update_outputs, (void))
{
    const netlist_time out_delay = NLTIME_FROM_NS(20);
#if 0
    OUTLOGIC(m_QA, (m_cnt >> 0) & 1, out_delay);
    OUTLOGIC(m_QB, (m_cnt >> 1) & 1, out_delay);
    OUTLOGIC(m_QC, (m_cnt >> 2) & 1, out_delay);
    OUTLOGIC(m_QD, (m_cnt >> 3) & 1, out_delay);
#else
    if ((m_cnt & 1) == 1)
        OUTLOGIC(m_QA, 1, out_delay);
    else
    {
        OUTLOGIC(m_QA, 0, out_delay);
        switch (m_cnt)
        {
        case 0x00:
            OUTLOGIC(m_QB, 0, out_delay);
            OUTLOGIC(m_QC, 0, out_delay);
            OUTLOGIC(m_QD, 0, out_delay);
            break;
        case 0x02:
        case 0x06:
        case 0x0A:
        case 0x0E:
            OUTLOGIC(m_QB, 1, out_delay);
            break;
        case 0x04:
        case 0x0C:
            OUTLOGIC(m_QB, 0, out_delay);
            OUTLOGIC(m_QC, 1, out_delay);
            break;
        case 0x08:
            OUTLOGIC(m_QB, 0, out_delay);
            OUTLOGIC(m_QC, 0, out_delay);
            OUTLOGIC(m_QD, 1, out_delay);
            break;
        }

    }
#endif
}

