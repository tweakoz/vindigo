/*
 * nlbase.c
 *
 */

#include "nl_base.h"
#include "devices/nld_system.h"
#include "analog/nld_solver.h"
#include "pstring.h"
#include "nl_util.h"

const netlist_time netlist_time::zero = netlist_time::from_raw(0);

netlist_logic_family_desc_t netlist_family_ttl =
{
        0.8, // m_low_thresh_V
        2.0, // m_high_thresh_V
        0.3, // m_low_V  - these depend on sinked/sourced current. Values should be suitable for typical applications.
        3.7, // m_high_V
        1.0, // m_R_low;
        130.0, //  m_R_high;
};

// ----------------------------------------------------------------------------------------
// netlist_queue_t
// ----------------------------------------------------------------------------------------

netlist_queue_t::netlist_queue_t(netlist_base_t &nl)
	: netlist_timed_queue<netlist_net_t *, netlist_time, 512>(), pstate_callback_t(),
		m_netlist(nl),
		m_qsize(0)
{  }

void netlist_queue_t::register_state(pstate_manager_t &manager, const pstring &module)
{
	NL_VERBOSE_OUT(("register_state\n"));
	manager.save_item(m_qsize, module + "." + "qsize");
	manager.save_item(m_times, module + "." + "times");
	manager.save_item(&(m_name[0][0]), module + "." + "names", sizeof(m_name));
}

void netlist_queue_t::on_pre_save()
{
	NL_VERBOSE_OUT(("on_pre_save\n"));
	m_qsize = this->count();
	NL_VERBOSE_OUT(("current time %f qsize %d\n", m_netlist->time().as_double(), qsize));
	for (int i = 0; i < m_qsize; i++ )
	{
		m_times[i] =  this->listptr()[i].time().as_raw();
		const char *p = this->listptr()[i].object()->name().cstr();
		int n = MIN(63, strlen(p));
		strncpy(&(m_name[i][0]), p, n);
		m_name[i][n] = 0;
	}
}


void netlist_queue_t::on_post_load()
{
	this->clear();
	NL_VERBOSE_OUT(("current time %f qsize %d\n", m_netlist->time().as_double(), qsize));
	for (int i = 0; i < m_qsize; i++ )
	{
		netlist_net_t *n = m_netlist.find_net(&(m_name[i][0]));
		NL_VERBOSE_OUT(("Got %s ==> %p\n", qtemp[i].m_name, n));
		NL_VERBOSE_OUT(("schedule time %f (%f)\n", n->time().as_double(), qtemp[i].m_time.as_double()));
		this->push(netlist_queue_t::entry_t(netlist_time::from_raw(m_times[i]), n));
	}
}

// ----------------------------------------------------------------------------------------
// netlist_object_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_object_t::netlist_object_t(const type_t atype, const family_t afamily)
: m_objtype(atype)
, m_family(afamily)
, m_netlist(NULL)
{}

ATTR_COLD netlist_object_t::~netlist_object_t()
{
	//delete m_name;
}

ATTR_COLD void netlist_object_t::init_object(netlist_base_t &nl, const pstring &aname)
{
	m_netlist = &nl;
	m_name = aname;
	save_register();
}

ATTR_COLD const pstring &netlist_object_t::name() const
{
	if (m_name == "")
		netlist().error("object not initialized");
	return m_name;
}

// ----------------------------------------------------------------------------------------
// netlist_owned_object_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_owned_object_t::netlist_owned_object_t(const type_t atype,
		const family_t afamily)
: netlist_object_t(atype, afamily)
, m_netdev(NULL)
{
}

ATTR_COLD void netlist_owned_object_t::init_object(netlist_core_device_t &dev,
		const pstring &aname)
{
	netlist_object_t::init_object(dev.netlist(), aname);
	m_netdev = &dev;
}

// ----------------------------------------------------------------------------------------
// netlist_base_t
// ----------------------------------------------------------------------------------------

netlist_base_t::netlist_base_t()
	:   netlist_object_t(NETLIST, GENERIC),
		m_time(netlist_time::zero),
		m_queue(*this),
		m_mainclock(NULL),
		m_solver(NULL)
{
}

template <class T>
static void tagmap_free_entries(T &tm)
{
	for (typename T::entry_t *entry = tm.first(); entry != NULL; entry = tm.next(entry))
	{
		delete entry->object();
	}
	tm.reset();
}

netlist_base_t::~netlist_base_t()
{
	tagmap_free_entries<tagmap_devices_t>(m_devices);

	netlist_net_t * const *p = m_nets.first();
	while (p != NULL)
	{
		netlist_net_t * const *pn = m_nets.next(p);
		if (!(*p)->isRailNet())
			delete (*p);
		p = pn;
	}

	m_nets.reset();
	pstring::resetmem();
}

ATTR_COLD void netlist_base_t::start()
{
    /* find the main clock and solver ... */

    m_mainclock = get_single_device<NETLIB_NAME(mainclock)>("mainclock");
    m_solver = get_single_device<NETLIB_NAME(solver)>("solver");
    m_gnd = get_single_device<NETLIB_NAME(gnd)>("gnd");

    NL_VERBOSE_OUT(("Initializing devices ...\n"));
    for (tagmap_devices_t::entry_t *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
    {
        netlist_device_t *dev = entry->object();
        dev->init(*this, entry->tag().cstr());
    }

}

ATTR_COLD netlist_net_t *netlist_base_t::find_net(const pstring &name)
{
	for (int i = 0; i < m_nets.count(); i++)
	{
		if (m_nets[i]->name() == name)
			return m_nets[i];
	}
	return NULL;
}

ATTR_COLD void netlist_base_t::reset()
{
	m_time = netlist_time::zero;
	m_queue.clear();
	if (m_mainclock != NULL)
		m_mainclock->m_Q.net().set_time(netlist_time::zero);
    if (m_solver != NULL)
        m_solver->do_reset();

    // Reset all nets once !
    for (int i = 0; i < m_nets.count(); i++)
        m_nets[i]->do_reset();

    // Reset all devices once !
    for (tagmap_devices_t::entry_t *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
    {
        netlist_device_t *dev = entry->object();
        dev->do_reset();
    }

	// Step all devices once !
	for (tagmap_devices_t::entry_t *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
	{
		netlist_device_t *dev = entry->object();
		dev->update_dev();
	}

	// FIXME: some const devices rely on this
    /* make sure params are set now .. */
    for (tagmap_devices_t::entry_t *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
    {
        entry->object()->update_param();
    }
}


ATTR_HOT ATTR_ALIGN void netlist_base_t::process_queue(const netlist_time delta)
{
    m_stop = m_time + delta;

    if (m_mainclock == NULL)
    {
        while ( (m_time < m_stop) && (m_queue.is_not_empty()))
        {
            const netlist_queue_t::entry_t &e = m_queue.pop();
            m_time = e.time();
            e.object()->update_devs();

            add_to_stat(m_perf_out_processed, 1);
            if (FATAL_ERROR_AFTER_NS)
                if (time() > NLTIME_FROM_NS(FATAL_ERROR_AFTER_NS))
                    error("Stopped");
        }
        if (m_queue.is_empty())
            m_time = m_stop;

    } else {
        netlist_net_t &mcQ = m_mainclock->m_Q.net();
        const netlist_time inc = m_mainclock->m_inc;

        while (m_time < m_stop)
        {
            if (m_queue.is_not_empty())
            {
                while (m_queue.peek().time() > mcQ.time())
                {
                    m_time = mcQ.time();
                    NETLIB_NAME(mainclock)::mc_update(mcQ, m_time + inc);
                }

                const netlist_queue_t::entry_t &e = m_queue.pop();
                m_time = e.time();
                e.object()->update_devs();

            } else {
                m_time = mcQ.time();
                NETLIB_NAME(mainclock)::mc_update(mcQ, m_time + inc);
            }
            if (FATAL_ERROR_AFTER_NS)
                if (time() > NLTIME_FROM_NS(FATAL_ERROR_AFTER_NS))
                    error("Stopped");

            add_to_stat(m_perf_out_processed, 1);
        }
    }
}

ATTR_COLD void netlist_base_t::error(const char *format, ...) const
{
	va_list ap;
	va_start(ap, format);
	vfatalerror(NL_ERROR, format, ap);
	va_end(ap);
}

ATTR_COLD void netlist_base_t::warning(const char *format, ...) const
{
    va_list ap;
    va_start(ap, format);
    vfatalerror(NL_WARNING, format, ap);
    va_end(ap);
}

ATTR_COLD void netlist_base_t::log(const char *format, ...) const
{
    va_list ap;
    va_start(ap, format);
    vfatalerror(NL_LOG, format, ap);
    va_end(ap);
}


// ----------------------------------------------------------------------------------------
// Default netlist elements ...
// ----------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------
// net_core_device_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_core_device_t::netlist_core_device_t(const family_t afamily)
: netlist_object_t(DEVICE, afamily), m_family_desc(NULL)
{
}

ATTR_COLD netlist_core_device_t::netlist_core_device_t(const netlist_logic_family_desc_t *family_desc)
: netlist_object_t(DEVICE, GENERIC), m_family_desc(family_desc)
{
}

ATTR_COLD void netlist_core_device_t::init(netlist_base_t &anetlist, const pstring &name)
{
	init_object(anetlist, name);

#if USE_PMFDELEGATES
	void (netlist_core_device_t::* pFunc)() = &netlist_core_device_t::update;
	static_update = reinterpret_cast<net_update_delegate>((this->*pFunc));
#endif

}

ATTR_COLD netlist_core_device_t::~netlist_core_device_t()
{
}

ATTR_HOT ATTR_ALIGN const netlist_sig_t netlist_core_device_t::INPLOGIC_PASSIVE(netlist_logic_input_t &inp)
{
	if (inp.state() == netlist_input_t::STATE_INP_PASSIVE)
	{
		inp.activate();
		const netlist_sig_t ret = inp.Q();
		inp.inactivate();
		return ret;
	}
	else
		return inp.Q();

}


// ----------------------------------------------------------------------------------------
// net_device_t
// ----------------------------------------------------------------------------------------

netlist_device_t::netlist_device_t()
	: netlist_core_device_t(&netlist_family_ttl),
		m_terminals(20)
{
}

netlist_device_t::netlist_device_t(const family_t afamily)
	: netlist_core_device_t(afamily),
		m_terminals(20)
{
}

netlist_device_t::~netlist_device_t()
{
	//NL_VERBOSE_OUT(("~net_device_t\n");
}

ATTR_COLD netlist_setup_t &netlist_device_t::setup()
{
	return netlist().setup();
}

ATTR_COLD void netlist_device_t::init(netlist_base_t &anetlist, const pstring &name)
{
	netlist_core_device_t::init(anetlist, name);
	start();
}


ATTR_COLD void netlist_device_t::register_sub(netlist_device_t &dev, const pstring &name)
{
	dev.init(netlist(), this->name() + "." + name);
}

ATTR_COLD void netlist_device_t::register_subalias(const pstring &name, netlist_core_terminal_t &term)
{
	pstring alias = this->name() + "." + name;

	setup().register_alias(alias, term.name());

	if (term.isType(netlist_terminal_t::INPUT) || term.isType(netlist_terminal_t::TERMINAL))
		m_terminals.add(alias);
}

ATTR_COLD void netlist_device_t::register_terminal(const pstring &name, netlist_terminal_t &port)
{
	setup().register_object(*this, name, port);
    if (port.isType(netlist_terminal_t::INPUT) || port.isType(netlist_terminal_t::TERMINAL))
        m_terminals.add(port.name());
}

ATTR_COLD void netlist_device_t::register_output(const pstring &name, netlist_output_t &port)
{
    port.m_family_desc = this->m_family_desc;
	setup().register_object(*this, name, port);
}

ATTR_COLD void netlist_device_t::register_input(const pstring &name, netlist_input_t &inp)
{
    // FIXME: change register_object as well
    inp.m_family_desc = this->m_family_desc;
	setup().register_object(*this, name, inp);
    m_terminals.add(inp.name());
}

ATTR_COLD void netlist_device_t::connect(netlist_core_terminal_t &t1, netlist_core_terminal_t &t2)
{
    setup().connect(t1, t2);
}


template <class C, class T>
ATTR_COLD void netlist_device_t::register_param(const pstring &sname, C &param, const T initialVal)
{
	pstring fullname = this->name() + "." + sname;
	param.init_object(*this, fullname);
	param.initial(initialVal);
	//FIXME: pass fullname from above
	setup().register_object(*this, fullname, param);
}

template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_double_t &param, const double initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_int_t &param, const int initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_logic_t &param, const int initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_str_t &param, const char * const initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_str_t &param, const pstring &initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_model_t &param, const char * const initialVal);


// ----------------------------------------------------------------------------------------
// net_net_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_net_t::netlist_net_t(const type_t atype, const family_t afamily)
	: netlist_object_t(atype, afamily)
    , m_solver(NULL)
	, m_head(NULL)
	, m_num_cons(0)
	, m_time(netlist_time::zero)
	, m_active(0)
	, m_in_queue(2)
	, m_railterminal(NULL)
{
    m_last.Analog = 0.0;
    m_new.Analog = 0.0;
    m_cur.Analog = 0.0;
};

ATTR_COLD void netlist_net_t::reset()
{
    m_last.Analog = 0.0;
    m_cur.Analog = 0.0;
    m_new.Analog = 0.0;
    m_last.Q = 0; // set to something we will never hit.
    m_new.Q = 0;
    m_cur.Q = 0;
    m_time = netlist_time::zero;
    m_active = 0;
    m_in_queue = 2;

    for (netlist_core_terminal_t *t = m_head; t != NULL; t = t->m_update_list_next)
    {
        t->do_reset();
        if (t->state() != netlist_input_t::STATE_INP_PASSIVE)
            m_active++;
    }
}

ATTR_COLD void netlist_net_t::init_object(netlist_base_t &nl, const pstring &aname)
{
	netlist_object_t::init_object(nl, aname);
	nl.m_nets.add(this);
}

ATTR_COLD void netlist_net_t::register_railterminal(netlist_output_t &mr)
{
	assert(m_railterminal == NULL);
	m_railterminal = &mr;
}

ATTR_COLD void netlist_net_t::merge_net(netlist_net_t *othernet)
{
	NL_VERBOSE_OUT(("merging nets ...\n"));
	if (othernet == NULL)
		return; // Nothing to do

	if (this->isRailNet() && othernet->isRailNet())
		netlist().error("Trying to merge to rail nets\n");

	if (othernet->isRailNet())
	{
		NL_VERBOSE_OUT(("othernet is railnet\n"));
		othernet->merge_net(this);
	}
	else
	{
		netlist_core_terminal_t *p = othernet->m_head;
		while (p != NULL)
		{
			netlist_core_terminal_t *pn = p->m_update_list_next;
			register_con(*p);
			p = pn;
		}

		othernet->m_head = NULL; // FIXME: othernet needs to be free'd from memory
	}
}

ATTR_COLD void netlist_net_t::register_con(netlist_core_terminal_t &terminal)
{
	terminal.set_net(*this);

	terminal.m_update_list_next = m_head;
	m_head = &terminal;
	m_num_cons++;

	if (terminal.state() != netlist_input_t::STATE_INP_PASSIVE)
		m_active++;
}

ATTR_HOT inline void netlist_net_t::update_dev(const netlist_core_terminal_t *inp, const UINT32 mask) const
{
	if ((inp->state() & mask) != 0)
	{
		netlist_core_device_t &netdev = inp->netdev();
		begin_timing(netdev.total_time);
		inc_stat(netdev.stat_count);
		netdev.update_dev();
		end_timing(netdev().total_time);
	}
}

ATTR_HOT inline void netlist_net_t::update_devs()
{
	assert(m_num_cons != 0);

	assert(this->isRailNet());

	static const UINT32 masks[4] = { 1, 5, 3, 1 };
    const UINT32 mask = masks[ (m_last.Q  << 1) | m_new.Q ];

    m_cur = m_new;
    m_in_queue = 2; /* mark as taken ... */

    netlist_core_terminal_t *p = m_head;

    switch (m_num_cons)
    {
    case 2:
        update_dev(p, mask);
        p = p->m_update_list_next;
    case 1:
        update_dev(p, mask);
        break;
    default:
        do
        {
            update_dev(p, mask);
            p = p->m_update_list_next;
        } while (p != NULL);
        break;
    }

    m_last = m_cur;
}

ATTR_HOT void netlist_net_t::solve()
{
    if (m_solver != NULL)
        m_solver->schedule();
}

// ----------------------------------------------------------------------------------------
// netlist_terminal_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_core_terminal_t::netlist_core_terminal_t(const type_t atype, const family_t afamily)
: netlist_owned_object_t(atype, afamily)
, m_family_desc(NULL)
, m_update_list_next(NULL)
, m_net(NULL)
, m_state(STATE_NONEX)
{
}

ATTR_COLD netlist_terminal_t::netlist_terminal_t()
: netlist_core_terminal_t(TERMINAL, ANALOG)
, m_Idr(0.0)
, m_go(NETLIST_GMIN)
, m_gt(NETLIST_GMIN)
, m_otherterm(NULL)
{
}


ATTR_COLD void netlist_terminal_t::reset()
{
    //netlist_terminal_core_terminal_t::reset();
    set_state(STATE_INP_ACTIVE);
    m_Idr = 0.0;
    m_go = NETLIST_GMIN;
    m_gt = NETLIST_GMIN;
}

ATTR_COLD void netlist_core_terminal_t::set_net(netlist_net_t &anet)
{
	m_net = &anet;
}

// ----------------------------------------------------------------------------------------
// net_input_t
// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// net_output_t
// ----------------------------------------------------------------------------------------

netlist_output_t::netlist_output_t(const type_t atype, const family_t afamily)
	: netlist_core_terminal_t(atype, afamily)
	, m_my_net(NET, afamily)
{
	//m_net = new net_net_t(NET_DIGITAL);
    set_state(STATE_OUT);
	this->set_net(m_my_net);
}

ATTR_COLD void netlist_output_t::init_object(netlist_core_device_t &dev, const pstring &aname)
{
	netlist_core_terminal_t::init_object(dev, aname);
	net().init_object(dev.netlist(), aname + ".net");
	net().register_railterminal(*this);
}

// ----------------------------------------------------------------------------------------
// netlist_logic_output_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_logic_output_t::netlist_logic_output_t()
    : netlist_output_t(OUTPUT, LOGIC), m_proxy(NULL)
{
}

ATTR_COLD void netlist_logic_output_t::initial(const netlist_sig_t val)
{
	net().m_cur.Q = val;
	net().m_new.Q = val;
    net().m_last.Q = val;
}

// ----------------------------------------------------------------------------------------
// netlist_ttl_output_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_ttl_output_t::netlist_ttl_output_t()
	: netlist_logic_output_t()
{
}

// ----------------------------------------------------------------------------------------
// netlist_analog_output_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_analog_output_t::netlist_analog_output_t()
	: netlist_output_t(OUTPUT, ANALOG)
{
    net().m_last.Analog = 0.97;
	net().m_cur.Analog = 0.98;
	net().m_new.Analog = 0.99;
}

ATTR_COLD void netlist_analog_output_t::initial(const double val)
{
    net().m_cur.Analog = val * 0.98;
	net().m_cur.Analog = val * 0.99;
	net().m_new.Analog = val * 1.0;
}

// ----------------------------------------------------------------------------------------
// netlist_param_t & friends
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_param_t::netlist_param_t(const param_type_t atype)
	: netlist_owned_object_t(PARAM, ANALOG)
	, m_param_type(atype)
{
}

ATTR_COLD netlist_param_double_t::netlist_param_double_t()
	: netlist_param_t(DOUBLE)
	, m_param(0.0)
{
}

ATTR_COLD netlist_param_int_t::netlist_param_int_t()
	: netlist_param_t(INTEGER)
	, m_param(0)
{
}

ATTR_COLD netlist_param_logic_t::netlist_param_logic_t()
	: netlist_param_int_t()
{
}

ATTR_COLD netlist_param_str_t::netlist_param_str_t()
	: netlist_param_t(STRING)
	, m_param("")
{
}

ATTR_COLD netlist_param_model_t::netlist_param_model_t()
	: netlist_param_t(MODEL)
	, m_param("")
{
}

ATTR_COLD const pstring netlist_param_model_t::model_type() const
{
    pstring tmp = this->Value();
    // .model 1N914 D(Is=2.52n Rs=.568 N=1.752 Cjo=4p M=.4 tt=20n Iave=200m Vpk=75 mfg=OnSemi type=silicon)
    int p = tmp.find("(");
    int p1 = p;
    while (--p >= 0 && tmp[p] != ' ')
        ;

    return tmp.substr(p+1, p1-p-1).ucase();
}


ATTR_COLD double netlist_param_model_t::model_value(const pstring &entity, const double defval) const
{
	pstring tmp = this->Value();
	// .model 1N914 D(Is=2.52n Rs=.568 N=1.752 Cjo=4p M=.4 tt=20n Iave=200m Vpk=75 mfg=OnSemi type=silicon)
	int p = tmp.ucase().find(entity.ucase() + "=");
	if (p>=0)
	{
		int pblank = tmp.find(" ", p);
		if (pblank < 0) pblank = tmp.len() + 1;
		tmp = tmp.substr(p, pblank - p);
		int pequal = tmp.find("=", 0);
		if (pequal < 0)
			netlist().error("parameter %s misformat in model %s temp %s\n", entity.cstr(), Value().cstr(), tmp.cstr());
		tmp = tmp.substr(pequal+1);
		double factor = 1.0;
		switch (*(tmp.right(1).cstr()))
		{
			case 'm': factor = 1e-3; break;
			case 'u': factor = 1e-6; break;
			case 'n': factor = 1e-9; break;
			case 'p': factor = 1e-12; break;
			case 'f': factor = 1e-15; break;
			case 'a': factor = 1e-18; break;

		}
		if (factor != 1.0)
			tmp = tmp.left(tmp.len() - 1);
		return atof(tmp.cstr()) * factor;
	}
	else
		return defval;
}


// ----------------------------------------------------------------------------------------
// mainclock
// ----------------------------------------------------------------------------------------

ATTR_HOT inline void NETLIB_NAME(mainclock)::mc_update(netlist_net_t &net, const netlist_time curtime)
{
	net.m_new.Q ^= 1;
	net.set_time(curtime);
	net.update_devs();
}

NETLIB_START(mainclock)
{
	register_output("Q", m_Q);

	register_param("FREQ", m_freq, 7159000.0 * 5);
	m_inc = netlist_time::from_hz(m_freq.Value()*2);
}

NETLIB_RESET(mainclock)
{
    m_Q.net().set_time(netlist_time::zero);
}

NETLIB_UPDATE_PARAM(mainclock)
{
	m_inc = netlist_time::from_hz(m_freq.Value()*2);
}

NETLIB_UPDATE(mainclock)
{
	netlist_net_t &net = m_Q.net();
	// this is only called during setup ...
	net.m_new.Q = !net.m_new.Q;
	net.set_time(netlist().time() + m_inc);
}

// ----------------------------------------------------------------------------------------
// net_device_t_base_factory
// ----------------------------------------------------------------------------------------

ATTR_COLD const nl_util::pstring_list net_device_t_base_factory::term_param_list()
{
    if (m_def_param.startsWith("+"))
        return nl_util::split(m_def_param.substr(1), ",");
    else
        return nl_util::pstring_list();
}

ATTR_COLD const nl_util::pstring_list net_device_t_base_factory::def_params()
{
    if (m_def_param.startsWith("+") || m_def_param.equals("-"))
        return nl_util::pstring_list();
    else
        return nl_util::split(m_def_param, ",");
}
