/*
 * nld_system.c
 *
 */

#include "nld_system.h"
#include "nld_twoterm.h"

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
}

NETLIB_UPDATE_PARAM(analog_const)
{
    m_Q.initial(m_const.Value());
}

// ----------------------------------------------------------------------------------------
// analog_callback
// ----------------------------------------------------------------------------------------

NETLIB_UPDATE(analog_callback)
{
    // FIXME: Remove after device cleanup
    if (!m_callback.isnull())
        m_callback(INPANALOG(m_in));
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
// solver
// ----------------------------------------------------------------------------------------

NETLIB_START(solver)
{
    register_output("Q_sync", m_Q_sync);
    register_output("Q_step", m_Q_step);
    //register_input("FB", m_feedback);

    register_param("SYNC_DELAY", m_sync_delay, NLTIME_FROM_NS(10).as_double());
    m_nt_sync_delay = m_sync_delay.Value();

    register_param("FREQ", m_freq, 48000.0);
    m_inc = netlist_time::from_hz(m_freq.Value());

    register_param("ACCURACY", m_accuracy, 1e-3);

    register_link_internal(m_fb_sync, m_Q_sync, netlist_input_t::STATE_INP_ACTIVE);
    register_link_internal(m_fb_step, m_Q_step, netlist_input_t::STATE_INP_ACTIVE);
    m_last_step = netlist_time::zero;

}

NETLIB_UPDATE_PARAM(solver)
{
    m_inc = netlist_time::from_hz(m_freq.Value());
}

NETLIB_NAME(solver)::~NETLIB_NAME(solver)()
{
    net_list_t::entry_t *p = m_nets.first();
    while (p != NULL)
    {
        net_list_t::entry_t *pn = m_nets.next(p);
        delete p->object();
        p = pn;
    }
}

NETLIB_FUNC_VOID(solver, post_start, ())
{

    NL_VERBOSE_OUT(("post start solver ...\n"));
    for (net_list_t::entry_t *pn = m_nets.first(); pn != NULL; pn = m_nets.next(pn))
    {
        NL_VERBOSE_OUT(("setting up net\n"));
        for (netlist_core_terminal_t *p = pn->object()->m_head; p != NULL; p = p->m_update_list_next)
        {
            switch (p->type())
            {
                case netlist_terminal_t::TERMINAL:
                    switch (p->netdev().family())
                    {
                        case CAPACITOR:
                            if (!m_steps.contains(&p->netdev()))
                                m_steps.add(&p->netdev());
                            break;
                        case DIODE:
                        case BJT_SWITCH:
                            if (!m_dynamic.contains(&p->netdev()))
                                m_dynamic.add(&p->netdev());
                            break;
                        default:
                            break;
                    }
                    pn->object()->m_terms.add(static_cast<netlist_terminal_t *>(p));
                    NL_VERBOSE_OUT(("Added terminal\n"));
                    break;
                case netlist_terminal_t::INPUT:
                    if (!m_inps.contains(&p->netdev()))
                        m_inps.add(&p->netdev());
                    NL_VERBOSE_OUT(("Added input\n"));
                    break;
                default:
                    fatalerror("unhandled element found\n");
                    break;
            }
        }
        if (pn->object()->m_head == NULL)
        {
            NL_VERBOSE_OUT(("Deleting net ...\n"));
            netlist_net_t *to_delete = pn->object();
            m_nets.remove(to_delete);
            delete to_delete;
            pn--;
        }
    }
}

NETLIB_UPDATE(solver)
{
    //m_Q.setToNoCheck(!m_Q.new_Q(), m_inc  );
    //OUTLOGIC(m_Q, !m_Q.net().new_Q(), m_inc  );

    bool resched = false;
    int  resched_cnt = 0;
    netlist_time now = netlist().time();
    netlist_time delta = now - m_last_step;

    if (delta >= m_inc)
    {
        NL_VERBOSE_OUT(("Step!\n"));
        /* update all terminals for new time step */
        m_last_step = now;
        for (dev_list_t::entry_t *p = m_steps.first(); p != NULL; p = m_steps.next(p))
            p->object()->step_time(delta.as_double());
    }
    do {
        /* update all non-linear devices  */
        for (dev_list_t::entry_t *p = m_dynamic.first(); p != NULL; p = m_dynamic.next(p))
            p->object()->update_terminals();

        resched = false;

        for (net_list_t::entry_t *pn = m_nets.first(); pn != NULL; pn = m_nets.next(pn))
        {
            netlist_net_t *net = pn->object();

            double gtot = 0;
            double iIdr = 0;

            for (netlist_net_t::terminal_list_t::entry_t *e = net->m_terms.first(); e != NULL; e = net->m_terms.next(e))
            {
                netlist_terminal_t *pt = e->object();
                gtot += pt->m_g;
                iIdr += pt->m_Idr + pt->m_g * pt->m_otherterm->net().Q_Analog();
            }

            double new_val = iIdr / gtot;
            if (fabs(new_val - net->m_cur.Analog) > m_accuracy.Value())
                resched = true;
            resched_cnt++;
            net->m_cur.Analog = net->m_new.Analog = new_val;

            NL_VERBOSE_OUT(("Info: %d\n", pn->object()->m_num_cons));
            NL_VERBOSE_OUT(("New: %lld %f %f\n", netlist().time().as_raw(), netlist().time().as_double(), new_val));
        }
    } while (resched && (resched_cnt < 5));
    //if (resched_cnt >= 5)
    //    printf("rescheduled\n");
    if (resched)
    {
        schedule();
    }
    else
    {
        /* update all inputs connected */
        for (dev_list_t::entry_t *p = m_inps.first(); p != NULL; p = m_inps.next(p))
            p->object()->update_dev();

        /* step circuit */
        if (!m_Q_step.net().is_queued())
            m_Q_step.net().push_to_queue(m_inc);
    }

        /* only inputs and terminals connected
         * approach:
         *
         * a) Update voltage on this net
         * b) Update devices
         * c) If difference old - new > trigger schedule immediate update
         *    of number of updates < max_update_count
         *    else clear number of updates
         */

}
