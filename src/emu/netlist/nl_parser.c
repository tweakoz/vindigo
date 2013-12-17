// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nl_parser.c
 *
 */

#include "nl_parser.h"

#undef NL_VERBOSE_OUT
#define NL_VERBOSE_OUT(x) printf x

// ----------------------------------------------------------------------------------------
// A netlist parser
// ----------------------------------------------------------------------------------------

void netlist_parser::parse(char *buf)
{
    char c;
	m_px = buf;
	c = getc();

	while (c)
	{
		pstring n;
		skipws();
		if (eof()) break;
		n = getname('(');
		NL_VERBOSE_OUT(("Parser: Device: %s\n", n.cstr()));
		if (n == "NET_ALIAS")
			net_alias();
        else if (n == "NET_C")
            net_c();
		else if (n == "NETDEV_PARAM")
			netdev_param();
        else if (n == "NETDEV_R")
            netdev_device(n, "R");
        else if (n == "NETDEV_C")
            netdev_device(n, "C");
        else if (n == "NETDEV_POT")
            netdev_device(n, "R");
        else if (n == "NETDEV_D")
            netdev_device(n, "model", true);
		else if ((n == "NETDEV_TTL_CONST") || (n == "NETDEV_ANALOG_CONST"))
			netdev_const(n);
		else
			netdev_device(n);
		c = getc();
	}
}

void netlist_parser::net_alias()
{
	pstring alias;
	pstring out;
	skipws();
	alias = getname(',');
	skipws();
	out = getname(')');
	NL_VERBOSE_OUT(("Parser: Alias: %s %s\n", alias.cstr(), out.cstr()));
	m_setup.register_alias(alias, out);
}

void netlist_parser::net_c()
{
    pstring t1;
    pstring t2;
    skipws();
    t1 = getname(',');
    skipws();
    t2 = getname(')');
    NL_VERBOSE_OUT(("Parser: Connect: %s %s\n", t1.cstr(), t2.cstr()));
    m_setup.register_link(t1 , t2);
}

void netlist_parser::netdev_param()
{
	pstring param;
	double val;
	skipws();
	param = getname(',');
	skipws();
	val = eval_param();
	NL_VERBOSE_OUT(("Parser: Param: %s %f\n", param.cstr(), val));
	m_setup.register_param(param, val);
	//m_setup.find_param(param).initial(val);
	check_char(')');
}

void netlist_parser::netdev_const(const pstring &dev_name)
{
	pstring name;
	netlist_device_t *dev;
	pstring paramfq;
	double val;

	skipws();
	name = getname(',');
	dev = m_setup.factory().new_device_by_name(dev_name, m_setup);
	m_setup.register_dev(dev, name);
	skipws();
	val = eval_param();
	paramfq = name + ".CONST";
	NL_VERBOSE_OUT(("Parser: Const: %s %f\n", name.cstr(), val));
    check_char(')');
	//m_setup.find_param(paramfq).initial(val);
	m_setup.register_param(paramfq, val);
}

void netlist_parser::netdev_device(const pstring &dev_type)
{
	pstring devname;
	netlist_device_t *dev;
	int cnt;

	skipws();
	devname = getname2(',', ')');
	dev = m_setup.factory().new_device_by_name(dev_type, m_setup);
	m_setup.register_dev(dev, devname);
	NL_VERBOSE_OUT(("Parser: IC: %s\n", devname.cstr()));
	cnt = 0;
	while (getc() != ')')
	{
		skipws();
		pstring output_name = getname2(',', ')');
		pstring alias = pstring::sprintf("%s.[%d]", devname.cstr(), cnt);
		NL_VERBOSE_OUT(("Parser: ID: %s %s\n", output_name.cstr(), alias.cstr()));
		m_setup.register_link(alias, output_name);
		skipws();
		cnt++;
	}
/*
    if (cnt != dev->m_terminals.count() && !dev->variable_input_count())
		fatalerror("netlist: input count mismatch for %s - expected %d found %d\n", devname.cstr(), dev->m_terminals.count(), cnt);
	if (dev->variable_input_count())
	{
		NL_VERBOSE_OUT(("variable inputs %s: %d\n", dev->name().cstr(), cnt));
	}
	*/
}

void netlist_parser::netdev_device(const pstring &dev_type, const pstring &default_param, bool isString)
{
    netlist_device_t *dev;

    skipws();
    pstring devname = getname2(',', ')');
    pstring defparam = devname + "." + default_param;
    dev = m_setup.factory().new_device_by_name(dev_type, m_setup);
    m_setup.register_dev(dev, devname);
    NL_VERBOSE_OUT(("Parser: IC: %s\n", devname.cstr()));
    if (getc() != ')')
    {
        // have a default param
        skipws();
        if (isString)
        {
            pstring val = getname(')');
            ungetc();
            NL_VERBOSE_OUT(("Parser: Default param: %s %s\n", defparam.cstr(), val.cstr()));
            m_setup.register_param(defparam, val);
        }
        else
        {
            double val = eval_param();
            NL_VERBOSE_OUT(("Parser: Default param: %s %f\n", defparam.cstr(), val));
            m_setup.register_param(defparam, val);
        }
    }
    check_char(')');
}

// ----------------------------------------------------------------------------------------
// private
// ----------------------------------------------------------------------------------------

void netlist_parser::skipeol()
{
    char c = getc();
	while (c)
	{
		if (c == 10)
		{
			c = getc();
			if (c != 13)
				ungetc();
			return;
		}
		c = getc();
	}
}

void netlist_parser::skipws()
{
	while (unsigned char c = getc())
	{
		switch (c)
		{
		case ' ':
		case 9:
		case 10:
		case 13:
			break;
		case '/':
		    c = getc();
			if (c == '/')
			{
                skipeol();
			}
			else if (c == '*')
			{
			    int f=0;
			    while ((c = getc()) != 0 )
			    {
			        if (f == 0 && c == '*')
			            f=1;
			        else if (f == 1 && c== '/' )
			            break;
			        else
			            f=0;
			    }
			}
			break;
		default:
		    ungetc();
			return;
		}
	}
}

pstring netlist_parser::getname(char sep)
{
	char buf[300];
	char *p1 = buf;
	char c;

	while ((c=getc()) != sep)
		*p1++ = c;
	*p1 = 0;
	return pstring(buf);
}

pstring netlist_parser::getname2(char sep1, char sep2)
{
	char buf[300];
	char *p1 = buf;
	char c=getc();

	while ((c != sep1) && (c != sep2))
	{
        *p1++ = c;
        c = getc();
	}
	*p1 = 0;
	ungetc();
	return pstring(buf);
}

void netlist_parser::check_char(char ctocheck)
{
	skipws();
	char c = getc();
	if ( c == ctocheck)
	{
		return;
	}
	m_setup.netlist().xfatalerror("Parser: expected '%c' found '%c'\n", ctocheck, c);
}

double netlist_parser::eval_param()
{
	static const char *macs[6] = {"", "RES_K(", "RES_M(", "CAP_U(", "CAP_N(", "CAP_P("};
	static double facs[6] = {1, 1e3, 1e6, 1e-6, 1e-9, 1e-12};
	int i;
	int f=0;
	char *e;
	double ret;

	pstring s = getname2(')',',');

	printf("Got %s\n", s.cstr());
	for (i=1; i<6;i++)
		if (strncmp(s.cstr(), macs[i], strlen(macs[i])) == 0)
			f = i;
	ret = strtod(s.substr(strlen(macs[f])).cstr(), &e);
	if ((f>0) && (*e != 0))
	    m_setup.netlist().xfatalerror("Parser: Error with parameter ...\n");
    if (f>0)
        check_char(')');
	//if (f == 0)
	//    ungetc();
	//if (f>0)
	//	e++;
	//m_p = e;
	return ret * facs[f];
}

unsigned char netlist_parser::getc()
{
    return *(m_px++);
}

void netlist_parser::ungetc()
{
    m_px--;
}
