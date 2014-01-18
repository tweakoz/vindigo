/*
 * nld_bjt.h
 *
 */

#ifndef NLD_BJT_H_
#define NLD_BJT_H_

#include "../nl_base.h"
#include "nld_twoterm.h"

// ----------------------------------------------------------------------------------------
// Macros
// ----------------------------------------------------------------------------------------

#define NETDEV_QBJT_SW(_name, _model)                                           \
        NET_REGISTER_DEV(QBJT_switch, _name)                                    \
        NETDEV_PARAMI(_name,  model,   _model)

#define NETDEV_QBJT_EB(_name, _model)                                           \
        NET_REGISTER_DEV(QBJT_EB, _name)                                        \
        NETDEV_PARAMI(_name,  model,   _model)


// ----------------------------------------------------------------------------------------
// nld_Q - Base classes
// ----------------------------------------------------------------------------------------

// Have a common start for transistors

class NETLIB_NAME(Q) : public netlist_device_t
{
public:
    enum q_type {
        BJT_NPN,
        BJT_PNP
    };

    ATTR_COLD NETLIB_NAME(Q)(const family_t afamily)
    : netlist_device_t(afamily)
    , m_qtype(BJT_NPN) { }

    inline q_type qtype() const { return m_qtype; }
    inline bool is_qtype(q_type atype) const { return m_qtype == atype; }
    inline void set_qtype(q_type atype) { m_qtype = atype; }
protected:
    ATTR_COLD virtual void start();
    ATTR_COLD virtual void reset();
    ATTR_HOT ATTR_ALIGN void update();

    netlist_param_model_t m_model;
private:
    q_type m_qtype;
};

class NETLIB_NAME(QBJT) : public NETLIB_NAME(Q)
{
public:

    ATTR_COLD NETLIB_NAME(QBJT)(const family_t afamily)
    : NETLIB_NAME(Q)(afamily) { }

protected:

private:
};




// ----------------------------------------------------------------------------------------
// nld_QBJT_switch
// ----------------------------------------------------------------------------------------


/*
 *         + -              C
 *   B ----VVV----+         |
 *                |         |
 *                Rb        Rc
 *                Rb        Rc
 *                Rb        Rc
 *                |         |
 *                +----+----+
 *                     |
 *                     E
 */

#define USE_OLD_S (0)

#if USE_OLD_S
class NETLIB_NAME(QBJT_switch) : public NETLIB_NAME(QBJT)
{
public:
    ATTR_COLD NETLIB_NAME(QBJT_switch)()
    : NETLIB_NAME(QBJT)(BJT_SWITCH), m_gB(NETLIST_GMIN), m_gC(NETLIST_GMIN), m_V(0.0), m_state_on(0) { }


    NETLIB_UPDATEI()
    {
        double vE = INPANALOG(m_EV);
        double vB = INPANALOG(m_BV);
        double m = (is_qtype( BJT_NPN) ? 1 : -1);

        int new_state = ((vB - vE) * m > m_V ) ? 1 : 0;
        if (m_state_on ^ new_state)
        {
            double gb = m_gB;
            double gc = m_gC;
            double v  = m_V * m;
            if (!new_state )
            {
                // not conducting
                gb = NETLIST_GMIN;
                v = 0;
                gc = NETLIST_GMIN;
            }
            m_RB.set(gb, v,   0.0);
            m_RC.set(gc, 0.0, 0.0);
            m_state_on = new_state;
            // Don't update m_RB - could cause an infinite loop
            m_RB.update_dev();
            m_RC.update_dev();
        }

    }

    NETLIB_NAME(R) m_RB;
    NETLIB_NAME(R) m_RC;

    netlist_analog_input_t m_BV;
    netlist_analog_input_t m_EV;

protected:

    ATTR_COLD virtual void start();
    ATTR_HOT void update_param();

    double m_gB; // base conductance / switch on
    double m_gC; // collector conductance / switch on
    double m_V; // internal voltage source
    UINT8 m_state_on;

private:
};
#else
class NETLIB_NAME(QBJT_switch) : public NETLIB_NAME(QBJT)
{
public:
    ATTR_COLD NETLIB_NAME(QBJT_switch)()
    : NETLIB_NAME(QBJT)(BJT_SWITCH),
      m_RB(netlist_object_t::ANALOG),
      m_RC(netlist_object_t::ANALOG),
      m_BC_dummy(netlist_object_t::ANALOG),
      m_gB(NETLIST_GMIN), m_gC(NETLIST_GMIN), m_V(0.0), m_state_on(0) { }

    NETLIB_UPDATE_TERMINALS()
    {
        double m = (is_qtype( BJT_NPN) ? 1 : -1);

        int new_state = (m_RB.deltaV() * m > m_V ) ? 1 : 0;
        if (m_state_on ^ new_state)
        {
            double gb = m_gB;
            double gc = m_gC;
            double v  = m_V * m;
            if (!new_state )
            {
                // not conducting
                gb = NETLIST_GMIN;
                v = 0;
                gc = NETLIST_GMIN;
            }
            m_RB.set(gb, v,   0.0);
            m_RC.set(gc, 0.0, 0.0);
            m_state_on = new_state;
        }
    }

    ATTR_HOT ATTR_ALIGN void virtual update()
    {
        if (!m_RB.m_P.net().isRailNet())
            m_RB.m_P.net().solve();   // Basis
        else if (!m_RB.m_N.net().isRailNet())
            m_RB.m_N.net().solve();   // Emitter
        else if (!m_RC.m_P.net().isRailNet())
            m_RB.m_P.net().solve();   // Collector
    }

    nld_twoterm m_RB;
    nld_twoterm m_RC;

    // FIXME: the matrix solvers should be devices so we can properly
    //        schedule them. This is a workaround and blows netgroup size

    nld_twoterm m_BC_dummy;

protected:

    ATTR_COLD virtual void start();
    ATTR_HOT void update_param();

    double m_gB; // base conductance / switch on
    double m_gC; // collector conductance / switch on
    double m_V; // internal voltage source
    UINT8 m_state_on;

private:
};
#endif

// ----------------------------------------------------------------------------------------
// nld_QBJT_EB
// ----------------------------------------------------------------------------------------

#define USE_OLD_B (0)

#if USE_OLD_B
class NETLIB_NAME(QBJT_EB) : public NETLIB_NAME(QBJT)
{
public:
    ATTR_COLD NETLIB_NAME(QBJT_EB)()
    : NETLIB_NAME(QBJT)(BJT_EB),
      m_D_BC(netlist_object_t::ANALOG),
      m_D_BE(netlist_object_t::ANALOG),
      m_I_BC(netlist_object_t::ANALOG),
      m_I_BE(netlist_object_t::ANALOG)
      { }

    NETLIB_UPDATE_TERMINALS()
    {
        m_gD_BE.update_diode(m_D_BE.deltaV());
        m_gD_BC.update_diode(m_D_BC.deltaV());

        m_D_BE.set(m_gD_BE.G(), 0.0, m_gD_BE.Ieq());
        m_D_BC.set(m_gD_BC.G(), 0.0, m_gD_BC.Ieq());

        m_I_BE.set(0.0, 0.0, - m_alpha_r * m_gD_BC.I());
        m_I_BC.set(0.0, 0.0, - m_alpha_f * m_gD_BE.I());
    }

protected:

    ATTR_COLD virtual void start();
    ATTR_COLD virtual void reset();
    ATTR_HOT void update_param();
    ATTR_HOT ATTR_ALIGN void virtual update();

    netlist_generic_diode m_gD_BC;
    netlist_generic_diode m_gD_BE;

    nld_twoterm m_D_BC;
    nld_twoterm m_D_BE;

    nld_twoterm m_I_BC;
    nld_twoterm m_I_BE;

    double m_alpha_f;
    double m_alpha_r;

private:
};
#else
class NETLIB_NAME(QBJT_EB) : public NETLIB_NAME(QBJT)
{
public:
    ATTR_COLD NETLIB_NAME(QBJT_EB)()
    : NETLIB_NAME(QBJT)(BJT_EB),
      m_D_CB(netlist_object_t::ANALOG),
      m_D_EB(netlist_object_t::ANALOG),
      m_D_EC(netlist_object_t::ANALOG)
      { }

    NETLIB_UPDATE_TERMINALS()
    {
        m_gD_BE.update_diode(-m_D_EB.deltaV());
        m_gD_BC.update_diode(-m_D_CB.deltaV());

        double gee = m_gD_BE.G();
        double gcc = m_gD_BC.G();
        double gec =  m_alpha_r * gcc;
        double gce =  m_alpha_f * gee;
        double sIe = -m_gD_BE.I() + m_alpha_r * m_gD_BC.I();
        double sIc = m_alpha_f * m_gD_BE.I() - m_gD_BC.I();
        double Ie = sIe + gee * m_gD_BE.Vd() - gec * m_gD_BC.Vd();
        double Ic = sIc - gce * m_gD_BE.Vd() + gcc * m_gD_BC.Vd();
        //double Ie = sIe + gee * -m_D_EB.deltaV() - gec * -m_D_CB.deltaV();
        //double Ic = sIc - gce * -m_D_EB.deltaV() + gcc * -m_D_CB.deltaV();
        //printf("EB %f sIe %f sIc %f\n", m_D_BE.deltaV(), sIe, sIc);

        m_D_EB.set_mat(gee, gec - gee, gce - gee, gee - gec, Ie, -Ie);
        m_D_CB.set_mat(gcc, gce - gcc, gec - gcc, gcc - gce, Ic, -Ic);
        m_D_EC.set_mat( 0,    -gec,      -gce,        0,       0,   0);
    }

protected:

    ATTR_COLD virtual void start();
    ATTR_COLD virtual void reset();
    ATTR_HOT void update_param();
    ATTR_HOT ATTR_ALIGN void virtual update();

    netlist_generic_diode m_gD_BC;
    netlist_generic_diode m_gD_BE;

    nld_twoterm m_D_CB;  // gcc, gce - gcc, gec - gcc, gcc - gce | Ic
    nld_twoterm m_D_EB;  // gee, gec - gee, gce - gee, gee - gec | Ie
    nld_twoterm m_D_EC;  // 0, -gec, -gcc, 0 | 0

    double m_alpha_f;
    double m_alpha_r;

private:
};
#endif
#endif /* NLD_BJT_H_ */
