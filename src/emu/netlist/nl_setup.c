// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nlsetup.c
 *
 */

#include "nl_base.h"
#include "nl_setup.h"
#include "nl_parser.h"
#include "nl_util.h"
#include "devices/nld_system.h"
#include "devices/nld_solver.h"

static NETLIST_START(base)
	NETDEV_TTL_CONST(ttlhigh, 1)
	NETDEV_TTL_CONST(ttllow, 0)
	NETDEV_ANALOG_CONST(NC, NETLIST_HIGHIMP_V)
NETLIST_END


// ----------------------------------------------------------------------------------------
// netlist_setup_t
// ----------------------------------------------------------------------------------------

netlist_setup_t::netlist_setup_t(netlist_base_t &netlist)
	: m_netlist(netlist)
    , m_proxy_cnt(0)
{
    m_factory.initialize();
	NETLIST_NAME(base)(*this);
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

netlist_setup_t::~netlist_setup_t()
{
	tagmap_free_entries<tagmap_devices_t>(m_devices);
	//tagmap_free_entries<tagmap_link_t>(m_links);
	//tagmap_free_entries<tagmap_nstring_t>(m_alias);
	m_links.reset();
	m_alias.reset();
	m_params.reset();
	m_terminals.reset();
	m_params_temp.reset();
	pstring::resetmem();
}

netlist_device_t *netlist_setup_t::register_dev(netlist_device_t *dev, const pstring &name)
{
	if (!(m_devices.add(name, dev, false)==TMERR_NONE))
		fatalerror("Error adding %s to device list\n", name.cstr());
	return dev;
}

template <class T>
static void remove_start_with(T &hm, pstring &sw)
{
	typename T::entry_t *entry = hm.first();
	while (entry != NULL)
	{
		typename T::entry_t *next = hm.next(entry);
		pstring x = entry->tag().cstr();
		if (sw.equals(x.substr(0, sw.len())))
		{
			NL_VERBOSE_OUT(("removing %s\n", entry->tag().cstr()));
			hm.remove(entry->object());
		}
		entry = next;
	}
}

void netlist_setup_t::remove_dev(const pstring &name)
{
	netlist_device_t *dev = m_devices.find(name);
	pstring temp = name + ".";
	if (dev == NULL)
		fatalerror("Device %s does not exist\n", name.cstr());

	//remove_start_with<tagmap_input_t>(m_inputs, temp);
	remove_start_with<tagmap_terminal_t>(m_terminals, temp);
	remove_start_with<tagmap_param_t>(m_params, temp);

	tagmap_link_t::entry_t *p = m_links.first();
	while (p != NULL)
	{
	    tagmap_link_t::entry_t *n = m_links.next(p);
	    if (temp.equals(p->object().e1.substr(0,temp.len())) || temp.equals(p->object().e2.substr(0,temp.len())))
	        m_links.remove(p->object());
	    p = n;
	}
	m_devices.remove(name);
}

void netlist_setup_t::register_callback(const pstring &devname, netlist_output_delegate delegate)
{
	NETLIB_NAME(analog_callback) *dev = (NETLIB_NAME(analog_callback) *) m_devices.find(devname);
	if (dev == NULL)
		fatalerror("did not find device %s\n", devname.cstr());
	dev->register_callback(delegate);
}

void netlist_setup_t::register_alias(const pstring &alias, const pstring &out)
{
    //if (!(m_alias.add(alias, new nstring(out), false)==TMERR_NONE))
	if (!(m_alias.add(alias, out, false)==TMERR_NONE))
		fatalerror("Error adding alias %s to alias list\n", alias.cstr());
}

pstring netlist_setup_t::objtype_as_astr(netlist_object_t &in)
{
    switch (in.type())
    {
        case netlist_terminal_t::TERMINAL:
            return "TERMINAL";
            break;
        case netlist_terminal_t::INPUT:
            return "INPUT";
            break;
        case netlist_terminal_t::OUTPUT:
            return "OUTPUT";
            break;
        case netlist_terminal_t::NET:
            return "NET";
            break;
        case netlist_terminal_t::PARAM:
            return "PARAM";
            break;
        case netlist_terminal_t::DEVICE:
            return "DEVICE";
            break;
    }
    fatalerror("Unknown object type %d\n", in.type());
}

void netlist_setup_t::register_object(netlist_device_t &dev, netlist_core_device_t &upd_dev, const pstring &name, netlist_object_t &obj, const netlist_input_t::state_e state)
{
    switch (obj.type())
    {
        case netlist_terminal_t::TERMINAL:
        case netlist_terminal_t::INPUT:
        case netlist_terminal_t::OUTPUT:
            {
                netlist_core_terminal_t &term = dynamic_cast<netlist_core_terminal_t &>(obj);
                if (obj.isType(netlist_terminal_t::OUTPUT))
                    dynamic_cast<netlist_output_t &>(term).init_object(upd_dev, dev.name() + "." + name);
                else
                    term.init_object(upd_dev, dev.name() + "." + name, state);

                if (!(m_terminals.add(term.name(), &term, false)==TMERR_NONE))
                    fatalerror("Error adding %s %s to terminal list\n", objtype_as_astr(term).cstr(), term.name().cstr());
                NL_VERBOSE_OUT(("%s %s\n", objtype_as_astr(term).cstr(), name.cstr()));
            }
            break;
        case netlist_terminal_t::NET:
            break;
        case netlist_terminal_t::PARAM:
            {

                netlist_param_t &param = dynamic_cast<netlist_param_t &>(obj);
                pstring temp = param.netdev().name() + "." + name;
                const pstring val = m_params_temp.find(temp);
                if (val != "")
                {
                    switch (param.param_type())
                    {
                        case netlist_param_t::DOUBLE:
                        {
                            //printf("Found parameter ... %s : %s\n", temp.cstr(), val->cstr());
                            double vald = 0;
                            if (sscanf(val.cstr(), "%lf", &vald) != 1)
                                fatalerror("Invalid number conversion %s : %s\n", temp.cstr(), val.cstr());
                            dynamic_cast<netlist_param_double_t &>(param).initial(vald);
                        }
                        break;
                        case netlist_param_t::INTEGER:
                        case netlist_param_t::LOGIC:
                        {
                            //printf("Found parameter ... %s : %s\n", temp.cstr(), val->cstr());
                            int vald = 0;
                            if (sscanf(val.cstr(), "%d", &vald) != 1)
                                fatalerror("Invalid number conversion %s : %s\n", temp.cstr(), val.cstr());
                            dynamic_cast<netlist_param_int_t &>(param).initial(vald);
                        }
                        break;
                        case netlist_param_t::STRING:
                        {
                            dynamic_cast<netlist_param_str_t &>(param).initial(val);
                        }
                        break;
                        default:
                            fatalerror("Parameter is not supported %s : %s\n", temp.cstr(), val.cstr());
                    }
                }
                if (!(m_params.add(temp, &param, false)==TMERR_NONE))
                    fatalerror("Error adding parameter %s to parameter list\n", name.cstr());
            }
            break;
        case netlist_terminal_t::DEVICE:
            break;
    }
}

void netlist_setup_t::register_link(const pstring &sin, const pstring &sout)
{
	link_t temp = link_t(sin, sout);
	NL_VERBOSE_OUT(("link %s <== %s\n", sin.cstr(), sout.cstr()));
    m_links.add(temp);
	//if (!(m_links.add(sin + "." + sout, temp, false)==TMERR_NONE))
	//	fatalerror("Error adding link %s<==%s to link list\n", sin.cstr(), sout.cstr());
}

void netlist_setup_t::register_param(const pstring &param, const double value)
{
    // FIXME: there should be a better way
    register_param(param, pstring::sprintf("%.9e", value));
}

void netlist_setup_t::register_param(const pstring &param, const pstring &value)
{
    //if (!(m_params_temp.add(param, new nstring(value), false)==TMERR_NONE))
    if (!(m_params_temp.add(param, value, false)==TMERR_NONE))
        fatalerror("Error adding parameter %s to parameter list\n", param.cstr());
}

const pstring netlist_setup_t::resolve_alias(const pstring &name) const
{
    pstring temp = name;
    pstring ret;

    /* FIXME: Detect endless loop */
    do {
        ret = temp;
        temp = m_alias.find(ret);
    } while (temp != "");

    int p = ret.find(".[");
	if (p > 0)
	{
	    pstring dname = ret;
	    netlist_device_t *dev = m_devices.find(dname.substr(0,p));
	    if (dev == NULL)
	        fatalerror("Device for %s not found\n", name.cstr());
	    int c = atoi(ret.substr(p+2,ret.len()-p-3));
	    temp = dev->name() + "." + dev->m_terminals[c];
	    // reresolve ....
	    do {
	        ret = temp;
	        temp = m_alias.find(ret);
	    } while (temp != "");
	}

    //printf("%s==>%s\n", name.cstr(), ret.cstr());
	return ret;
}

netlist_core_terminal_t *netlist_setup_t::find_terminal(const pstring &terminal_in, bool required)
{
    const pstring &tname = resolve_alias(terminal_in);
    netlist_core_terminal_t *ret;

    ret = m_terminals.find(tname);
    /* look for default */
    if (ret == NULL)
    {
        /* look for ".Q" std output */
        pstring s = tname + ".Q";
        ret = m_terminals.find(s);
    }
    if (ret == NULL && required)
        fatalerror("terminal %s(%s) not found!\n", terminal_in.cstr(), tname.cstr());
    if (ret != NULL)
        NL_VERBOSE_OUT(("Found input %s\n", tname.cstr()));
    return ret;
}

netlist_core_terminal_t *netlist_setup_t::find_terminal(const pstring &terminal_in, netlist_object_t::type_t atype, bool required)
{
	const pstring &tname = resolve_alias(terminal_in);
	netlist_core_terminal_t *ret;

	ret = m_terminals.find(tname);
	/* look for default */
	if (ret == NULL && atype == netlist_object_t::OUTPUT)
	{
		/* look for ".Q" std output */
		pstring s = tname + ".Q";
		ret = m_terminals.find(s);
	}
	if (ret == NULL && required)
		fatalerror("terminal %s(%s) not found!\n", terminal_in.cstr(), tname.cstr());
    if (ret != NULL && ret->type() != atype)
    {
        if (required)
            fatalerror("object %s(%s) found but wrong type\n", terminal_in.cstr(), tname.cstr());
        else
            ret = NULL;
    }
    if (ret != NULL)
        NL_VERBOSE_OUT(("Found input %s\n", tname.cstr()));
	return ret;
}

netlist_param_t *netlist_setup_t::find_param(const pstring &param_in, bool required)
{
	const pstring &outname = resolve_alias(param_in);
	netlist_param_t *ret;

	ret = m_params.find(outname);
	if (ret == NULL && required)
		fatalerror("parameter %s(%s) not found!\n", param_in.cstr(), outname.cstr());
	if (ret != NULL)
	    NL_VERBOSE_OUT(("Found parameter %s\n", outname.cstr()));
	return ret;
}


void netlist_setup_t::connect_input_output(netlist_input_t &in, netlist_output_t &out)
{
    if (out.isFamily(netlist_terminal_t::ANALOG) && in.isFamily(netlist_terminal_t::LOGIC))
    {
        nld_a_to_d_proxy *proxy = new nld_a_to_d_proxy(in);
        pstring x = pstring::sprintf("proxy_ad_%d", m_proxy_cnt);
        m_proxy_cnt++;

        proxy->init(*this, x);
        register_dev(proxy, x);

        proxy->m_Q.net().register_con(in);
        out.net().register_con(proxy->m_I);

    }
    else if (out.isFamily(netlist_terminal_t::LOGIC) && in.isFamily(netlist_terminal_t::ANALOG))
    {
        //printf("here 1\n");
        nld_d_to_a_proxy *proxy = new nld_d_to_a_proxy(out);
        pstring x = pstring::sprintf("proxy_da_%d", m_proxy_cnt);
        m_proxy_cnt++;

        proxy->init(*this, x);
        register_dev(proxy, x);

        proxy->m_Q.net().register_con(in);
        out.net().register_con(proxy->m_I);
    }
    else
    {
        out.net().register_con(in);
    }
}

void netlist_setup_t::connect_terminal_input(netlist_terminal_t &term, netlist_input_t &inp)
{
    if (inp.isFamily(netlist_terminal_t::ANALOG))
    {
        connect_terminals(inp, term);
    }
    else if (inp.isFamily(netlist_terminal_t::LOGIC))
    {
        NL_VERBOSE_OUT(("connect_terminal_input: connecting proxy\n"));
        nld_a_to_d_proxy *proxy = new nld_a_to_d_proxy(inp);
        pstring x = pstring::sprintf("proxy_da_%d", m_proxy_cnt);
        m_proxy_cnt++;

        proxy->init(*this, x);
        register_dev(proxy, x);

        connect_terminals(term, proxy->m_I);

        if (inp.has_net())
            //fatalerror("logic inputs can only belong to one net!\n");
            proxy->m_Q.net().merge_net(&inp.net());
        else
            proxy->m_Q.net().register_con(inp);
    }
    else
    {
        fatalerror("Netlist: Severe Error");
    }
}

// FIXME: optimize code  ...
void netlist_setup_t::connect_terminal_output(netlist_terminal_t &in, netlist_output_t &out)
{
    if (out.isFamily(netlist_terminal_t::ANALOG))
    {
        /* no proxy needed, just merge existing terminal net */
        if (in.has_net())
            out.net().merge_net(&in.net());
        else
            out.net().register_con(in);

    }
    else if (out.isFamily(netlist_terminal_t::LOGIC))
    {
        NL_VERBOSE_OUT(("connect_terminal_output: connecting proxy\n"));
        nld_d_to_a_proxy *proxy = new nld_d_to_a_proxy(out);
        pstring x = pstring::sprintf("proxy_da_%d", m_proxy_cnt);
        m_proxy_cnt++;

        proxy->init(*this, x);
        register_dev(proxy, x);

        out.net().register_con(proxy->m_I);

        if (in.has_net())
            proxy->m_Q.net().merge_net(&in.net());
        else
            proxy->m_Q.net().register_con(in);
    }
    else
    {
        fatalerror("Netlist: Severe Error");
    }
}

void netlist_setup_t::connect_terminals(netlist_core_terminal_t &t1, netlist_core_terminal_t &t2)
{
    //assert(in.isType(netlist_terminal_t::TERMINAL));
    //assert(out.isType(netlist_terminal_t::TERMINAL));

    if (t1.has_net() && t2.has_net())
    {
        NL_VERBOSE_OUT(("T2 and T1 have net\n"));
        t1.net().merge_net(&t2.net());
    }
    else if (t2.has_net())
    {
        NL_VERBOSE_OUT(("T2 has net\n"));
        t2.net().register_con(t1);
    }
    else if (t1.has_net())
    {
        NL_VERBOSE_OUT(("T1 has net\n"));
        t1.net().register_con(t2);
    }
    else
    {
        NL_VERBOSE_OUT(("adding net ...\n"));
        netlist_net_t *anet =  new netlist_net_t(netlist_object_t::NET, netlist_object_t::ANALOG);
        t1.set_net(*anet);
        m_netlist.solver()->m_nets.add(anet);
        // FIXME: Nets should have a unique name
        t1.net().init_object(netlist(),"some net");
        t1.net().register_con(t2);
        t1.net().register_con(t1);
    }
}

void netlist_setup_t::connect(netlist_core_terminal_t &t1, netlist_core_terminal_t &t2)
{
    NL_VERBOSE_OUT(("Connecting %s to %s\n", t1.name().cstr(), t2.name().cstr()));
    // FIXME: amend device design so that warnings can be turned into errors
    //        Only variable inputs have this issue
    if (t1.isType(netlist_core_terminal_t::OUTPUT) && t2.isType(netlist_core_terminal_t::INPUT))
    {
        if (t2.has_net())
            mame_printf_warning("Input %s already connected\n", t2.name().cstr());
        connect_input_output(dynamic_cast<netlist_input_t &>(t2), dynamic_cast<netlist_output_t &>(t1));
    }
    else if (t1.isType(netlist_core_terminal_t::INPUT) && t2.isType(netlist_core_terminal_t::OUTPUT))
    {
        if (t1.has_net())
            mame_printf_warning("Input %s already connected\n", t1.name().cstr());
        connect_input_output(dynamic_cast<netlist_input_t &>(t1), dynamic_cast<netlist_output_t &>(t2));
    }
    else if (t1.isType(netlist_core_terminal_t::OUTPUT) && t2.isType(netlist_core_terminal_t::TERMINAL))
    {
        connect_terminal_output(dynamic_cast<netlist_terminal_t &>(t2), dynamic_cast<netlist_output_t &>(t1));
    }
    else if (t1.isType(netlist_core_terminal_t::TERMINAL) && t2.isType(netlist_core_terminal_t::OUTPUT))
    {
        connect_terminal_output(dynamic_cast<netlist_terminal_t &>(t1), dynamic_cast<netlist_output_t &>(t2));
    }
    else if (t1.isType(netlist_core_terminal_t::INPUT) && t2.isType(netlist_core_terminal_t::TERMINAL))
    {
        connect_terminal_input(dynamic_cast<netlist_terminal_t &>(t2), dynamic_cast<netlist_input_t &>(t1));
    }
    else if (t1.isType(netlist_core_terminal_t::TERMINAL) && t2.isType(netlist_core_terminal_t::INPUT))
    {
        connect_terminal_input(dynamic_cast<netlist_terminal_t &>(t1), dynamic_cast<netlist_input_t &>(t2));
    }
    else if (t1.isType(netlist_core_terminal_t::TERMINAL) && t2.isType(netlist_core_terminal_t::TERMINAL))
    {
        connect_terminals(dynamic_cast<netlist_terminal_t &>(t1), dynamic_cast<netlist_terminal_t &>(t2));
    }
    else
        fatalerror("Connecting %s to %s not supported!\n", t1.name().cstr(), t2.name().cstr());
}

void netlist_setup_t::resolve_inputs(void)
{

    NL_VERBOSE_OUT(("Resolving ...\n"));
    for (tagmap_link_t::entry_t *entry = m_links.first(); entry != NULL; entry = m_links.next(entry))
    {
        const pstring t1s = entry->object().e1;
        const pstring t2s = entry->object().e2;
        netlist_core_terminal_t *t1 = find_terminal(t1s);
        netlist_core_terminal_t *t2 = find_terminal(t2s);

        connect(*t1, *t2);
    }

    /* print all outputs */
    for (tagmap_terminal_t::entry_t *entry = m_terminals.first(); entry != NULL; entry = m_terminals.next(entry))
    {
        ATTR_UNUSED netlist_output_t *out = dynamic_cast<netlist_output_t *>(entry->object());
        //if (out != NULL)
            //VERBOSE_OUT(("%s %d\n", out->netdev()->name(), *out->Q_ptr()));
    }

    if (m_netlist.solver() != NULL)
        m_netlist.solver()->post_start();


}

void netlist_setup_t::start_devices(void)
{

    if (getenv("NL_LOGS"))
    {
        NL_VERBOSE_OUT(("Creating dynamic logs ...\n"));
        nl_util::pstring_list ll = nl_util::split(getenv("NL_LOGS"), ":");
        for (int i=0; i < ll.count(); i++)
        {
            printf("%d: <%s>\n",i, ll[i].cstr());
            netlist_device_t *nc = factory().new_device_by_classname("nld_log", *this);
            pstring name = "log" + ll[i];
            register_dev(nc, name);
            register_link(name + ".I", ll[i]);
        }
    }


    NL_VERBOSE_OUT(("Searching for mainclock and solver ...\n"));
    /* find the main clock ... */
    for (tagmap_devices_t::entry_t *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
    {
        netlist_device_t *dev = entry->object();
        if (dynamic_cast<NETLIB_NAME(mainclock)*>(dev) != NULL)
        {
            m_netlist.set_mainclock_dev(dynamic_cast<NETLIB_NAME(mainclock)*>(dev));
        }
        if (dynamic_cast<NETLIB_NAME(solver)*>(dev) != NULL)
        {
            m_netlist.set_solver_dev(dynamic_cast<NETLIB_NAME(solver)*>(dev));
        }
    }

    NL_VERBOSE_OUT(("Initializing devices ...\n"));
    for (tagmap_devices_t::entry_t *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
    {
        netlist_device_t *dev = entry->object();
        dev->init(*this, entry->tag().cstr());
    }
}

void netlist_setup_t::step_devices_once(void)
{
	/* make sure params are set now .. */
	for (tagmap_param_t::entry_t *entry = m_params.first(); entry != NULL; entry = m_params.next(entry))
	{
		entry->object()->netdev().update_param();
	}

	for (tagmap_devices_t::entry_t *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
	{
		netlist_device_t *dev = entry->object();
		dev->update_dev();
	}
}


void netlist_setup_t::parse(char *buf)
{
	netlist_parser parser(*this);
	parser.parse(buf);
}

void netlist_setup_t::print_stats()
{
#if (NL_KEEP_STATISTICS)
	{
		for (netlist_setup_t::tagmap_devices_t::entry_t *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
		{
			//entry->object()->s
			printf("Device %20s : %12d %15ld\n", entry->object()->name().cstr(), entry->object()->stat_count, (long int) entry->object()->total_time / (entry->object()->stat_count + 1));
		}
		printf("Queue Start %15d\n", m_netlist.queue().m_prof_start);
		printf("Queue End   %15d\n", m_netlist.queue().m_prof_end);
		printf("Queue Sort  %15d\n", m_netlist.queue().m_prof_sort);
		printf("Queue Move  %15d\n", m_netlist.queue().m_prof_sortmove);
	}
#endif
}
