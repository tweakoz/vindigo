/***************************************************************************

    netlist.c

    Discrete netlist implementation.

****************************************************************************

    Couriersud reserves the right to license the code under a less restrictive
    license going forward.

    Copyright Nicola Salmoria and the MAME team
    All rights reserved.

    Redistribution and use of this code or any derivative works are permitted
    provided that the following conditions are met:

    * Redistributions may not be sold, nor may they be used in a commercial
    product or activity.

    * Redistributions that are modified from the original source must include the
    complete source code, including the source code for all components used by a
    binary built from the modified sources. However, as a special exception, the
    source code distributed need not include anything that is normally distributed
    (in either source or binary form) with the major components (compiler, kernel,
    and so on) of the operating system on which the executable runs, unless that
    component itself accompanies the executable.

    * Redistributions must reproduce the above copyright notice, this list of
    conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

#include "netlist.h"
#include "netlist/net_lib.h"

//============================================================
//  DEBUGGING
//============================================================

#define VERBOSE                 (0)
#define KEEP_STATISTICS         (0)
#define FATAL_ERROR_AFTER_NS	 (0) //(1000)

#if (VERBOSE)
	#define VERBOSE_OUT(x)      printf x
#else
	#define VERBOSE_OUT(x)
#endif

//============================================================
//  MACROS
//============================================================

#if KEEP_STATISTICS
#define add_to_stat(v,x)        do { v += (x); } while (0)
#define inc_stat(v)             add_to_stat(v, 1)
#define begin_timing(v)         do { (v) -= get_profile_ticks(); } while (0)
#define end_timing(v)           do { (v) += get_profile_ticks(); } while (0)
#else
#define add_to_stat(v,x)        do { } while (0)
#define inc_stat(v)             add_to_stat(v, 1)
#define begin_timing(v)         do { } while (0)
#define end_timing(v)           do { } while (0)
#endif

const netlist_time netlist_time::zero = netlist_time::from_raw(0);

// ----------------------------------------------------------------------------------------
// A netlist parser
// ----------------------------------------------------------------------------------------

class netlist_parser
{
public:
	netlist_parser(netlist_setup_t &setup)
	: m_setup(setup) {}

	void parse(char *buf)
	{
		m_p = buf;
		while (*m_p)
		{
			astring n;
			skipws();
			if (!*m_p) break;
			n = getname('(');
			VERBOSE_OUT(("Parser: Device: %s\n", n.cstr()));
			if (n == "NET_ALIAS")
				net_alias();
			else if (n == "NETDEV_PARAM")
				netdev_param();
			else if ((n == "NETDEV_TTL_CONST") || (n == "NETDEV_ANALOG_CONST"))
				netdev_const(n);
			else
				netdev_device(n);
		}
	}

	void net_alias()
	{
		astring alias;
		astring out;
		skipws();
		alias = getname(',');
		skipws();
		out = getname(')');
		VERBOSE_OUT(("Parser: Alias: %s %s\n", alias.cstr(), out.cstr()));
		m_setup.register_alias(alias, out);
	}

	void netdev_param()
	{
		astring param;
		double val;
		skipws();
		param = getname(',');
		skipws();
		val = eval_param();
		VERBOSE_OUT(("Parser: Param: %s %f\n", param.cstr(), val));
		m_setup.find_param(param).initial(val);
		check_char(')');
	}

	void netdev_const(const astring &dev_name)
	{
		astring name;
		net_device_t *dev;
		astring paramfq;
		double val;

		skipws();
		name = getname(',');
		dev = net_create_device_by_name(dev_name, m_setup, name);
		m_setup.register_dev(dev);
		skipws();
		val = eval_param();
		check_char(')');
		paramfq = name;
		paramfq.cat(".CONST");
		VERBOSE_OUT(("Parser: Const: %s %f\n", name.cstr(), val));
		m_setup.find_param(paramfq).initial(val);
	}

	void netdev_device(const astring &dev_type)
	{
		astring devname;
		net_device_t *dev;
		int cnt;

		skipws();
		devname = getname2(',', ')');
		dev = net_create_device_by_name(dev_type, m_setup, devname);
		m_setup.register_dev(dev);
		skipws();
		VERBOSE_OUT(("Parser: IC: %s\n", devname.cstr()));
		cnt = 0;
		while (*m_p != ')')
		{
			m_p++;
			skipws();
			astring output_name = getname2(',', ')');
			VERBOSE_OUT(("Parser: ID: %s %s\n", output_name.cstr(), dev->m_inputs.item(cnt)->cstr()));
			m_setup.register_link(*dev->m_inputs.item(cnt), output_name);
			skipws();
			cnt++;
		}
		if (cnt != dev->m_inputs.count() && !dev->variable_input_count())
			fatalerror("netlist: input count mismatch for %s - expected %d found %d\n", devname.cstr(), dev->m_inputs.count(), cnt);
		if (dev->variable_input_count())
		{
			VERBOSE_OUT(("variable inputs %s: %d\n", dev->name().cstr(), cnt));
		}
		check_char(')');
	}

private:

	void skipeol()
	{
		while (*m_p)
		{
			if (*m_p == 10)
			{
				m_p++;
				if (*m_p && *m_p == 13)
					m_p++;
				return;
			}
			m_p++;
		}
	}

	void skipws()
	{
		while (*m_p)
		{
			switch (*m_p)
			{
			case ' ':
			case 9:
			case 10:
			case 13:
				m_p++;
				break;
			case '/':
				if (*(m_p+1) == '/')
					skipeol();
				break;
			default:
				return;
			}
		}
	}

	astring getname(char sep)
	{
		char buf[300];
		char *p1 = buf;

		while (*m_p != sep)
			*p1++ = *m_p++;
		*p1 = 0;
		m_p++;
		return astring(buf);
	}

	astring getname2(char sep1, char sep2)
	{
		char buf[300];
		char *p1 = buf;

		while ((*m_p != sep1) && (*m_p != sep2))
			*p1++ = *m_p++;
		*p1 = 0;
		return astring(buf);
	}

	void check_char(char ctocheck)
	{
		skipws();
		if (*m_p == ctocheck)
		{
			m_p++;
			return;
		}
		fatalerror("Parser: expected '%c' found '%c'\n", ctocheck, *m_p);
	}

	double eval_param()
	{
		static const char *macs[6] = {"", "RES_K(", "RES_M(", "CAP_U(", "CAP_N(", "CAP_P("};
		static double facs[6] = {1, 1e3, 1e6, 1e-6, 1e-9, 1e-12};
		int i;
		int f=0;
		char *e;
		double ret;
		char *s = m_p;

		for (i=1; i<6;i++)
			if (strncmp(s, macs[i], strlen(macs[i])) == 0)
				f = i;
		ret = strtod(s+strlen(macs[f]), &e);
		if ((f>0) && (*e != ')'))
			fatalerror("Parser: Error with parameter ...\n");
		if (f>0)
			e++;
		m_p = e;
		return ret * facs[f];
	}

	char * m_p;
	netlist_setup_t &m_setup;

};


// ----------------------------------------------------------------------------------------
// netdev_mainclock
// ----------------------------------------------------------------------------------------

ATTR_HOT inline void netdev_mainclock::mc_update(net_output_t &Q, const netlist_time curtime)
{
	Q.m_new_Q = !Q.m_new_Q;
	Q.set_time(curtime);
	Q.update_devs();
}

ATTR_COLD NETLIB_START(netdev_mainclock)
{
	register_output("Q", m_Q);

	register_param("FREQ", m_freq, 7159000.0 * 5);
	m_inc = netlist_time::from_hz(m_freq.Value()*2);

}

ATTR_HOT NETLIB_UPDATE_PARAM(netdev_mainclock)
{
	m_inc = netlist_time::from_hz(m_freq.Value()*2);
}

ATTR_HOT NETLIB_UPDATE(netdev_mainclock)
{
	// this is only called during setup ...
	m_Q.m_new_Q = !m_Q.m_new_Q;
	m_Q.set_time(m_netlist->time() + m_inc);
}


// ----------------------------------------------------------------------------------------
// netdev_a_to_d
// ----------------------------------------------------------------------------------------

class netdev_a_to_d_proxy : public net_device_t
{
public:
	netdev_a_to_d_proxy(net_input_t &in_proxied)
			: net_device_t()
	{
		assert(in_proxied.object_type(SIGNAL_MASK) == SIGNAL_DIGITAL);
		m_I.m_high_thresh_V = in_proxied.m_high_thresh_V;
		m_I.m_low_thresh_V = in_proxied.m_low_thresh_V;
	}

	virtual ~netdev_a_to_d_proxy() {}

	analog_input_t m_I;
	ttl_output_t m_Q;

protected:
	void start()
	{
		m_I.init_input(this);

		m_Q.init_terminal(this);
		m_Q.initial(1);
	}

	ATTR_HOT ATTR_ALIGN void update()
	{
		if (m_I.Q_Analog() > m_I.m_high_thresh_V)
			OUTLOGIC(m_Q, 1, NLTIME_FROM_NS(1));
		else if (m_I.Q_Analog() < m_I.m_low_thresh_V)
			OUTLOGIC(m_Q, 0, NLTIME_FROM_NS(1));
	}

};

// ----------------------------------------------------------------------------------------
// netdev_d_to_a
// ----------------------------------------------------------------------------------------

class netdev_d_to_a_proxy : public net_device_t
{
public:
	netdev_d_to_a_proxy(net_output_t &out_proxied)
			: net_device_t()
	{
		assert(out_proxied.object_type(SIGNAL_MASK) == SIGNAL_DIGITAL);
		m_low_V = out_proxied.m_low_V;
		m_high_V = out_proxied.m_high_V;
	}

	virtual ~netdev_d_to_a_proxy() {}

	ttl_input_t m_I;
	analog_output_t m_Q;

protected:
	void start()
	{
		m_I.init_input(this);
		m_Q.init_terminal(this);
		m_Q.initial(0);
	}

	ATTR_HOT ATTR_ALIGN void update()
	{
		OUTANALOG(m_Q, INPLOGIC(m_I) ? m_high_V : m_low_V, NLTIME_FROM_NS(1));
	}

private:
	double m_low_V;
	double m_high_V;
};

// ----------------------------------------------------------------------------------------
// netdev_const
// ----------------------------------------------------------------------------------------

NETLIB_START(netdev_ttl_const)
{
	register_output("Q", m_Q);
	register_param("CONST", m_const, 0.0);
}

NETLIB_UPDATE(netdev_ttl_const)
{
}

NETLIB_UPDATE_PARAM(netdev_ttl_const)
{
	OUTLOGIC(m_Q, m_const.ValueInt(), NLTIME_IMMEDIATE);
}

NETLIB_START(netdev_analog_const)
{
	register_output("Q", m_Q);
	register_param("CONST", m_const, 0.0);
}

NETLIB_UPDATE(netdev_analog_const)
{
}

NETLIB_UPDATE_PARAM(netdev_analog_const)
{
	m_Q.initial(m_const.Value());
}

// ----------------------------------------------------------------------------------------
// netlist_base_t
// ----------------------------------------------------------------------------------------


NETLIB_UPDATE(netdev_analog_callback)
{
	// FIXME: Remove after device cleanup
	if (!m_callback.isnull())
		m_callback(INPANALOG(m_in));
}

netlist_base_t::netlist_base_t()
	: m_mainclock(NULL),
	  m_time_ps(netlist_time::zero),
	  m_rem(0),
	  m_div(NETLIST_DIV)
{
}

netlist_base_t::~netlist_base_t()
{
}

ATTR_COLD void netlist_base_t::set_mainclock_dev(netdev_mainclock *dev)
{
	m_mainclock = dev;
}

ATTR_COLD void netlist_base_t::reset()
{
	  m_time_ps = netlist_time::zero;
	  m_rem = 0;
	  m_queue.clear();
	  if (m_mainclock != NULL)
		  m_mainclock->m_Q.set_time(netlist_time::zero);
}


void netlist_base_t::set_clock_freq(UINT64 clockfreq)
{
	m_div = netlist_time::from_hz(clockfreq).as_raw();
	m_rem = 0;
	assert_always(m_div == NETLIST_DIV, "netlist: illegal clock!");
	VERBOSE_OUT(("Setting clock %" I64FMT "d and divisor %d\n", clockfreq, m_div));
}

ATTR_HOT ATTR_ALIGN inline void netlist_base_t::update_time(const netlist_time t, INT32 &atime)
{
	if (NETLIST_DIV_BITS == 0)
	{
		const netlist_time delta = t - m_time_ps;
		m_time_ps = t;
		atime -= delta.as_raw();
	} else {
		const netlist_time delta = t - m_time_ps + netlist_time::from_raw(m_rem);
		m_time_ps = t;
		m_rem = delta.as_raw() & NETLIST_MASK;
		atime -= (delta.as_raw() >> NETLIST_DIV_BITS);

		// The folling is suitable for non-power of 2 m_divs ...
		// atime -= divu_64x32_rem(delta.as_raw(), m_div, &m_rem);
	}
}

ATTR_HOT ATTR_ALIGN void netlist_base_t::process_list(INT32 &atime)
{
	if (m_mainclock == NULL)
	{
		while ( (atime > 0) && (m_queue.is_not_empty()))
		{
			const queue_t::entry_t e = m_queue.pop();
			update_time(e.time(), atime);

			if (FATAL_ERROR_AFTER_NS)
				printf("%s\n", e.object().netdev()->name().cstr());

			e.object().update_devs();

			add_to_stat(m_perf_out_processed, 1);

			if (FATAL_ERROR_AFTER_NS)
				if (time() > NLTIME_FROM_NS(FATAL_ERROR_AFTER_NS))
					fatalerror("Stopped");
		}

		if (atime > 0)
		{
			m_time_ps += netlist_time::from_raw(atime * m_div);
			atime = 0;
		}
	} else {
		net_output_t &mcQ = m_mainclock->m_Q;
		const netlist_time inc = m_mainclock->m_inc;

		while (atime > 0)
		{
			if (m_queue.is_not_empty())
			{
				while (m_queue.peek().time() > mcQ.time())
				{
					update_time(mcQ.time(), atime);

					netdev_mainclock::mc_update(mcQ, time() + inc);

				}
				const queue_t::entry_t e = m_queue.pop();

				update_time(e.time(), atime);

				e.object().update_devs();

			} else {
				update_time(mcQ.time(), atime);

				netdev_mainclock::mc_update(mcQ, time() + inc);
			}
			if (FATAL_ERROR_AFTER_NS)
				if (time() > NLTIME_FROM_NS(FATAL_ERROR_AFTER_NS))
					fatalerror("Stopped");

			add_to_stat(m_perf_out_processed, 1);
		}

		if (atime > 0)
		{
			m_time_ps += netlist_time::from_raw(atime * m_div);
			atime = 0;
		}
	}
}

// ----------------------------------------------------------------------------------------
// Default netlist elements ...
// ----------------------------------------------------------------------------------------


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
{
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
	tagmap_free_entries<tagmap_astring_t>(m_links);
	tagmap_free_entries<tagmap_astring_t>(m_alias);
	m_params.reset();
	m_terminals.reset();
}

net_device_t *netlist_setup_t::register_dev(net_device_t *dev)
{
	if (!(m_devices.add(dev->name(), dev, false)==TMERR_NONE))
		fatalerror("Error adding %s to device list\n", dev->name().cstr());
	return dev;
}

template <class T>
static void remove_start_with(T &hm, astring &sw)
{
	typename T::entry_t *entry = hm.first();
	while (entry != NULL)
	{
		typename T::entry_t *next = hm.next(entry);
		if (sw.cmpsubstr(entry->tag(), 0, sw.len()) == 0)
		{
			VERBOSE_OUT(("removing %s\n", entry->tag().cstr()));
			hm.remove(entry->object());
		}
		entry = next;
	}
}

void netlist_setup_t::remove_dev(const astring &name)
{
	net_device_t *dev = m_devices.find(name);
	astring temp = name;
	if (dev == NULL)
		fatalerror("Device %s does not exist\n", name.cstr());

	temp.cat(".");

	//remove_start_with<tagmap_input_t>(m_inputs, temp);
	remove_start_with<tagmap_terminal_t>(m_terminals, temp);
	remove_start_with<tagmap_param_t>(m_params, temp);
	remove_start_with<tagmap_astring_t>(m_links, temp);
	m_devices.remove(name);
}

void netlist_setup_t::register_callback(const astring &devname, net_output_delegate delegate)
{
	netdev_analog_callback *dev = (netdev_analog_callback *) m_devices.find(devname);
	if (dev == NULL)
		fatalerror("did not find device %s\n", devname.cstr());
	dev->register_callback(delegate);
}

void netlist_setup_t::register_alias(const astring &alias, const astring &out)
{
	if (!(m_alias.add(alias, new astring(out), false)==TMERR_NONE))
		fatalerror("Error adding alias %s to alias list\n", alias.cstr());
}

void netlist_setup_t::register_output(net_core_device_t &dev, net_core_device_t &upd_dev, const astring &name, net_output_t &out)
{
	VERBOSE_OUT(("out %s\n", name.cstr()));
	astring temp = dev.name();
	temp.cat(".");
	temp.cat(name);
	out.init_terminal(&upd_dev);
	if (!(m_terminals.add(temp, &out, false)==TMERR_NONE))
		fatalerror("Error adding output %s to output list\n", name.cstr());
}

void netlist_setup_t::register_input(net_device_t &dev, net_core_device_t &upd_dev, const astring &name, net_input_t &inp, net_input_t::net_input_state type)
{
	VERBOSE_OUT(("input %s\n", name.cstr()));
	astring temp = dev.name();
	temp.cat(".");
	temp.cat(name);
	inp.init_input(&upd_dev, type);
	dev.m_inputs.add(temp);
	if (!(m_terminals.add(temp, &inp, false) == TMERR_NONE))
		fatalerror("Error adding input %s to input list\n", name.cstr());
}

void netlist_setup_t::register_link(const astring &sin, const astring &sout)
{
	const astring *temp = new astring(sout);
	VERBOSE_OUT(("link %s <== %s\n", sin.cstr(), sout.cstr()));
	if (!(m_links.add(sin, temp, false)==TMERR_NONE))
		fatalerror("Error adding link %s<==%s to link list\n", sin.cstr(), sout.cstr());
}


void netlist_setup_t::register_param(const astring &name, net_param_t *param)
{
	astring temp = param->netdev().name();
	temp.cat(".");
	temp.cat(name);
	if (!(m_params.add(temp, param, false)==TMERR_NONE))
		fatalerror("Error adding parameter %s to parameter list\n", name.cstr());
}


const astring &netlist_setup_t::resolve_alias(const astring &name) const
{
	const astring *ret = m_alias.find(name);
	if (ret != NULL)
		return *ret;
	return name;
}

net_output_t *netlist_setup_t::find_output_exact(const astring &outname_in)
{
	net_terminal_t *term = m_terminals.find(outname_in);
	return dynamic_cast<net_output_t *>(term);
}

net_output_t &netlist_setup_t::find_output(const astring &outname_in)
{
	const astring &outname = resolve_alias(outname_in);
	net_output_t *ret;

	ret = find_output_exact(outname);
	/* look for default */
	if (ret == NULL)
	{
		/* look for ".Q" std output */
		astring s = outname;
		s.cat(".Q");
		ret = find_output_exact(s);
	}
	if (ret == NULL)
		fatalerror("output %s(%s) not found!\n", outname_in.cstr(), outname.cstr());
	VERBOSE_OUT(("Found input %s\n", outname.cstr()));
	return *ret;
}

net_param_t &netlist_setup_t::find_param(const astring &param_in)
{
	const astring &outname = resolve_alias(param_in);
	net_param_t *ret;

	ret = m_params.find(outname);
	if (ret == NULL)
		fatalerror("parameter %s(%s) not found!\n", param_in.cstr(), outname.cstr());
	VERBOSE_OUT(("Found parameter %s\n", outname.cstr()));
	return *ret;
}

void netlist_setup_t::resolve_inputs(void)
{
	VERBOSE_OUT(("Resolving ...\n"));
	int proxy_cnt = 0;
	for (tagmap_astring_t::entry_t *entry = m_links.first(); entry != NULL; entry = m_links.next(entry))
	{
		const astring *sout = entry->object();
		astring sin = entry->tag();
		net_input_t *in = dynamic_cast<net_input_t *>(m_terminals.find(sin));

		if (in == NULL)
			fatalerror("Unable to find %s\n", sin.cstr());

		net_output_t  &out = find_output(sout->cstr());
		if (out.object_type(net_output_t::SIGNAL_MASK) == net_output_t::SIGNAL_ANALOG
				&& in->object_type(net_output_t::SIGNAL_MASK) == net_output_t::SIGNAL_DIGITAL)
		{
			netdev_a_to_d_proxy *proxy = new netdev_a_to_d_proxy(*in);
			astring x = "";
			x.printf("proxy_ad_%d", proxy_cnt++);

			proxy->init(*this, x.cstr());
			register_dev(proxy);

			in->set_output(proxy->m_Q);
			proxy->m_Q.register_con(*in);
			proxy->m_I.set_output(out);
			out.register_con(proxy->m_I);

		}
		else if (out.object_type(net_output_t::SIGNAL_MASK) == net_output_t::SIGNAL_DIGITAL
				&& in->object_type(net_output_t::SIGNAL_MASK) == net_output_t::SIGNAL_ANALOG)
		{
			//printf("here 1\n");
			netdev_d_to_a_proxy *proxy = new netdev_d_to_a_proxy(out);
			astring x = "";
			x.printf("proxy_da_%d", proxy_cnt++);
			proxy->init(*this, x.cstr());
			register_dev(proxy);

			in->set_output(proxy->m_Q);
			proxy->m_Q.register_con(*in);
			proxy->m_I.set_output(out);
			out.register_con(proxy->m_I);
			//printf("here 2\n");
		}
		else
		{
			in->set_output(out);
			out.register_con(*in);
		}
	}

	/* find the main clock ... */
	for (tagmap_devices_t::entry_t *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
	{
		net_device_t *dev = entry->object();
		if (dynamic_cast<netdev_mainclock*>(dev) != NULL)
		{
			m_netlist.set_mainclock_dev(dynamic_cast<netdev_mainclock*>(dev));
		}
	}

#if 1

#else
	/* make sure all outputs are triggered once */
	for (tagmap_output_t::entry_t *entry = m_outputs.first(); entry != NULL; entry = m_outputs.next(entry))
	{
		net_output_t *out = entry->object();
		//if (dynamic_cast<const netdev_clock *>(out->netdev()) == NULL )
		{
			out->update_devs_force();
			INT32 time = 10000;
			m_netlist.process_list(time);
		}
	}
	//m_netlist.m_queue.clear();
#endif

	/* print all outputs */
	for (tagmap_terminal_t::entry_t *entry = m_terminals.first(); entry != NULL; entry = m_terminals.next(entry))
	{
		ATTR_UNUSED net_output_t *out = dynamic_cast<net_output_t *>(entry->object());
		//if (out != NULL)
			//VERBOSE_OUT(("%s %d\n", out->netdev()->name(), *out->Q_ptr()));
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
		net_device_t *dev = entry->object();
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
	if (KEEP_STATISTICS)
	{
		for (netlist_setup_t::tagmap_devices_t::entry_t *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
		{
			//entry->object()->s
			printf("Device %20s : %12d %15ld\n", entry->object()->name().cstr(), entry->object()->stat_count, (long int) entry->object()->total_time / (entry->object()->stat_count + 1));
		}
		printf("Queue Start %15d\n", m_netlist.m_queue.m_prof_start);
		printf("Queue End   %15d\n", m_netlist.m_queue.m_prof_end);
		printf("Queue Sort  %15d\n", m_netlist.m_queue.m_prof_sort);
		printf("Queue Move  %15d\n", m_netlist.m_queue.m_prof_sortmove);
	}
}


// ----------------------------------------------------------------------------------------
// net_core_device_t
// ----------------------------------------------------------------------------------------

net_core_device_t::net_core_device_t()
: net_object_t(DEVICE)
{
}

ATTR_COLD void net_core_device_t::init(netlist_setup_t &setup, const astring &name)
{
	m_netlist = &setup.netlist();
	m_name = name;

#if USE_DELEGATES
#if USE_PMFDELEGATES
	void (net_core_device_t::* pFunc)() = &net_core_device_t::update;
	static_update = reinterpret_cast<net_update_delegate>((this->*pFunc));
#else
	static_update = net_update_delegate(&net_core_device_t::update, "update", this);
	// get the pointer to the member function
#endif
#endif

}

ATTR_COLD void net_device_t::init(netlist_setup_t &setup, const astring &name)
{
	net_core_device_t::init(setup, name);
	m_setup = &setup;
	start();
}


net_core_device_t::~net_core_device_t()
{
}

// ----------------------------------------------------------------------------------------
// net_device_t
// ----------------------------------------------------------------------------------------

ATTR_HOT ATTR_ALIGN const net_sig_t net_core_device_t::INPLOGIC_PASSIVE(logic_input_t &inp)
{
	if (inp.state() == net_input_t::INP_STATE_PASSIVE)
	{
		inp.activate();
		const net_sig_t ret = inp.Q();
		inp.inactivate();
		return ret;
	}
	else
		return inp.Q();

}

net_device_t::net_device_t()
	: net_core_device_t(),
	  m_setup(NULL),
	  m_variable_input_count(false)
{
}

net_device_t::~net_device_t()
{
	//printf("~net_device_t\n");
}

ATTR_COLD void net_device_t::register_sub(net_core_device_t &dev, const astring &name)
{
	dev.init(*m_setup, name);
}

void net_device_t::register_output(net_core_device_t &dev, const astring &name, net_output_t &port)
{
	m_setup->register_output(*this, dev, name, port);
}

void net_device_t::register_output(const astring &name, net_output_t &port)
{
	m_setup->register_output(*this,*this,name, port);
}

void net_device_t::register_input(net_core_device_t &dev, const astring &name, net_input_t &inp, net_input_t::net_input_state type)
{
	m_setup->register_input(*this, dev, name, inp, type);
}

void net_device_t::register_input(const astring &name, net_input_t &inp, net_input_t::net_input_state type)
{
	register_input(*this, name, inp, type);
}

void net_device_t::register_link_internal(net_core_device_t &dev, net_input_t &in, net_output_t &out, net_input_t::net_input_state aState)
{
	in.set_output(out);
	in.init_input(&dev, aState);
	//if (in.state() != net_input_t::INP_STATE_PASSIVE)
		out.register_con(in);
}

void net_device_t::register_link_internal(net_input_t &in, net_output_t &out, net_input_t::net_input_state aState)
{
	register_link_internal(*this, in, out, aState);
}

void net_device_t::register_param(net_core_device_t &dev, const astring &name, net_param_t &param, double initialVal)
{
	param.set_netdev(dev);
	param.initial(initialVal);
	m_setup->register_param(name, &param);
}

void net_device_t::register_param(const astring &name, net_param_t &param, double initialVal)
{
	register_param(*this,name, param, initialVal);
}

// ----------------------------------------------------------------------------------------
// net_terminal_t
// ----------------------------------------------------------------------------------------

ATTR_COLD void net_terminal_t::init_terminal(net_core_device_t *dev)
{
	m_netdev = dev;
	m_netlist = dev->netlist();
}

// ----------------------------------------------------------------------------------------
// net_input_t
// ----------------------------------------------------------------------------------------

ATTR_COLD void net_input_t::init_input(net_core_device_t *dev, net_input_state astate)
{
	init_terminal(dev);
	m_state = astate;
}

// ----------------------------------------------------------------------------------------
// net_output_t
// ----------------------------------------------------------------------------------------

net_output_t::net_output_t(int atype)
	: net_terminal_t(atype)
	, m_low_V(0.0)
	, m_high_V(0.0)
	, m_last_Q(0)
	, m_Q(0)
	, m_new_Q(0)
	, m_Q_analog(0.0)
	, m_new_Q_analog(0.0)
	, m_num_cons(0)
	, m_time(netlist_time::zero)
	, m_active(0)
	, m_in_queue(2)
{
	//m_cons = global_alloc_array(net_input_t *, OUTPUT_MAX_CONNECTIONS);
}

ATTR_HOT inline void net_output_t::update_dev(const net_input_t *inp, const UINT32 mask)
{
	if ((inp->state() & mask) != 0)
	{
		ATTR_UNUSED net_core_device_t *netdev = inp->netdev();
		begin_timing(netdev->total_time);
		inc_stat(netdev->stat_count);
		netdev->update_dev();
		end_timing(netdev()->total_time);
	}
}

ATTR_HOT inline void net_output_t::update_devs()
{

	assert(m_num_cons != 0);

	const UINT32 masks[4] = { 1, 5, 3, 1 };
	m_Q = m_new_Q;
	m_Q_analog = m_new_Q_analog;
	m_in_queue = 2; /* mark as taken ... */

	const UINT32 mask = masks[ (m_last_Q  << 1) | m_Q ];

	switch (m_num_cons)
	{
	case 2:
		update_dev(m_cons[1], mask);
	case 1:
		update_dev(m_cons[0], mask);
		break;
	default:
		{
			for (int i=0; i < m_num_cons; i++)
				update_dev(m_cons[i], mask);
		}
		break;
	}

	m_last_Q = m_Q;
}

ATTR_COLD void net_output_t::register_con(net_input_t &input)
{
	int i;
	if (m_num_cons >= OUTPUT_MAX_CONNECTIONS)
		fatalerror("Connections exceeded for %s\n", netdev()->name().cstr());

	/* keep similar devices together */
	for (i = 0; i < m_num_cons; i++)
		if (m_cons[i]->netdev() == input.netdev())
			break;

	for (int j = m_num_cons; j > i; j--)
		m_cons[j] = m_cons[j - 1];

	m_cons[i] = &input;
	m_num_cons++;
	if (input.state() != net_input_t::INP_STATE_PASSIVE)
		m_active++;
}

// ----------------------------------------------------------------------------------------
// netlist_mame_device
// ----------------------------------------------------------------------------------------

const device_type NETLIST = &device_creator<netlist_mame_device>;

netlist_mame_device::netlist_mame_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, NETLIST, "netlist", tag, owner, clock, "netlist_mame", __FILE__),
		device_execute_interface(mconfig, *this)
{
}

void netlist_mame_device::static_set_constructor(device_t &device, void (*setup_func)(netlist_setup_t &))
{
	netlist_mame_device &netlist = downcast<netlist_mame_device &>(device);
	netlist.m_setup_func = setup_func;
}

void netlist_mame_device::device_config_complete()
{
}

void netlist_mame_device::device_start()
{
	//double dt = clocks_to_attotime(1).as_double();
	m_netlist = global_alloc_clear(netlist_t(*this));
	m_netlist->set_clock_freq(this->clock());

	m_setup = global_alloc_clear(netlist_setup_t(*m_netlist));

	m_setup_func(*m_setup);

	bool allok = true;
	for (on_device_start **ods = m_device_start_list.first(); ods <= m_device_start_list.last(); ods++)
		allok &= (*ods)->OnDeviceStart();

	if (!allok)
		fatalerror("required elements not found\n");

	m_setup->resolve_inputs();

	save_state();
	/* TODO: we have to save the round robin queue as well */

	// set our instruction counter
	m_icountptr = &m_icount;
}

void netlist_mame_device::device_reset()
{
	m_netlist->reset();
	m_setup->step_devices_once();
}

void netlist_mame_device::device_stop()
{
	m_setup->print_stats();

	global_free(m_setup);
	m_setup = NULL;
	global_free(m_netlist);
	m_netlist = NULL;
}

void netlist_mame_device::device_post_load()
{
}

void netlist_mame_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
}

void netlist_mame_device::save_state()
{
#if 0
	for (netlist_setup_t::tagmap_output_t::entry_t *entry = m_setup->m_outputs.first(); entry != NULL; entry = m_setup->m_outputs.next(entry))
	{
		save_item(*entry->object()->Q_ptr(), entry->tag().cstr(), 0);
		save_item(*entry->object()->new_Q_ptr(), entry->tag().cstr(), 1);
	}
#endif
}

UINT64 netlist_mame_device::execute_clocks_to_cycles(UINT64 clocks) const
{
	return clocks;
}

UINT64 netlist_mame_device::execute_cycles_to_clocks(UINT64 cycles) const
{
	return cycles;
}

ATTR_HOT void netlist_mame_device::execute_run()
{
	//bool check_debugger = ((device_t::machine().debug_flags & DEBUG_FLAG_ENABLED) != 0);

	// debugging
	//m_ppc = m_pc; // copy PC to previous PC
	//if (check_debugger)
	//  debugger_instruction_hook(this, 0); //m_pc);

	m_netlist->process_list(m_icount);

}
