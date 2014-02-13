// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nllists.h
 *
 */

#ifndef NLLISTS_H_
#define NLLISTS_H_

#include "nl_config.h"

// ----------------------------------------------------------------------------------------
// netlist_list_t: a simple list
// ----------------------------------------------------------------------------------------


template <class _ListClass, int _NumElem = 128>
class netlist_list_t
{
public:

	ATTR_COLD netlist_list_t(int numElements = _NumElem)
	{
		m_num_elements = numElements;
		m_list = new _ListClass[m_num_elements];
        m_count = 0;
	}

	ATTR_COLD netlist_list_t(const netlist_list_t &rhs)
	{
	    m_num_elements = rhs.capacity();
		m_list = new _ListClass[m_num_elements];
		m_count = 0;
		for (int i=0; i<rhs.count(); i++)
		{
			this->add(rhs[i]);
		}
	}

	ATTR_COLD netlist_list_t &operator=(const netlist_list_t &rhs)
	{
		for (int i=0; i<rhs.count(); i++)
		{
			this->add(rhs[i]);
		}
		return *this;
	}


	ATTR_COLD ~netlist_list_t()
	{
		delete[] m_list;
	}

	ATTR_HOT inline void add(const _ListClass &elem)
	{
		if (m_count >= m_num_elements)
			resize(m_num_elements * 2);

		m_list[m_count++] = elem;
	}

	ATTR_HOT inline void resize(const int new_size)
	{
		int cnt = count();
		_ListClass *m_new = new _ListClass[new_size];
		_ListClass *pd = m_new;

		for (_ListClass *ps = m_list; ps < m_list + cnt; ps++, pd++)
			*pd = *ps;
		delete[] m_list;
		m_list = m_new;
		m_count = cnt;
		m_num_elements = new_size;
	}

	ATTR_HOT inline void remove(const _ListClass &elem)
	{
		for (int i = 0; i < m_count; i++)
		{
			if (m_list[i] == elem)
			{
			    m_count --;
				while (i < m_count)
				{
					m_list[i] = m_list[i+1];
					i++;
				}
				return;
			}
		}
	}

    ATTR_HOT inline void remove_at(const int pos)
    {
        assert((pos>=0) && (pos<m_count));
        m_count--;
        for (int i = pos; i < m_count; i++)
        {
            m_list[i] = m_list[i+1];
        }
    }

	ATTR_HOT inline bool contains(const _ListClass &elem) const
	{
		for (_ListClass *i = m_list; i < m_list + m_count; i++)
		{
			if (*i == elem)
				return true;
		}
		return false;
	}

    ATTR_HOT inline int indexof(const _ListClass &elem) const
    {
        for (int i = 0; i < m_count; i++)
        {
            if (m_list[i] == elem)
                return i;
        }
        return -1;
    }

	ATTR_HOT inline const _ListClass *first() const { return ((m_count > 0) ? &m_list[0] : NULL ); }
	ATTR_HOT inline const _ListClass *next(const _ListClass *lc) const { return ((lc < last()) ? lc + 1 : NULL ); }
	ATTR_HOT inline const _ListClass *last() const { return &m_list[m_count -1]; }
	ATTR_HOT inline int count() const { return m_count; }
	ATTR_HOT inline bool empty() const { return (m_count == 0); }
	ATTR_HOT inline void reset() { m_count = 0; }
    ATTR_HOT inline int capacity() const { return m_num_elements; }

	ATTR_COLD void reset_and_free()
	{
		for (_ListClass *i = m_list; i < m_list + m_count; i++)
		{
			delete *i;
		}
		reset();
	}

	ATTR_HOT inline _ListClass& operator[](const int & index) { return m_list[index]; }
	ATTR_HOT inline const _ListClass& operator[](const int & index) const { return m_list[index]; }

private:
	int m_count;
	_ListClass * m_list;
	int m_num_elements;
	//_ListClass m_list[_NumElements];
};

// ----------------------------------------------------------------------------------------
// timed queue
// ----------------------------------------------------------------------------------------

template <class _Element, class _Time, int _Size>
class netlist_timed_queue
{
	NETLIST_PREVENT_COPYING(netlist_timed_queue)
public:

	struct entry_t
	{
	public:
		ATTR_HOT inline entry_t()
		: m_time(), m_object() {}
		ATTR_HOT inline entry_t(const _Time atime, _Element elem) : m_time(atime), m_object(elem) {}
		ATTR_HOT inline _Time time() const { return m_time; }
		ATTR_HOT inline _Element object() const { return m_object; }

	private:
		_Time m_time;
		_Element m_object;
	};

	netlist_timed_queue()
	{
		//m_list = global_alloc_array(entry_t, SIZE);
		clear();
	}

	ATTR_HOT inline int capacity() const { return _Size; }
	ATTR_HOT inline bool is_empty() const { return (m_end == &m_list[0]); }
	ATTR_HOT inline bool is_not_empty() const { return (m_end > &m_list[0]); }

	ATTR_HOT ATTR_ALIGN void push(const entry_t &e)
	{
        entry_t * RESTRICT i = m_end++;
        while ((i > &m_list[0]) && (e.time() > (i - 1)->time()) )
        {
            i--;
            *(i+1) = *i;
            inc_stat(m_prof_sortmove);
        }
        *i = e;
        inc_stat(m_prof_sort);
		assert(m_end - m_list < _Size);
	}

	ATTR_HOT inline entry_t pop()
	{
		return *--m_end;
	}

	ATTR_HOT inline const entry_t &peek() const
	{
		return *(m_end-1);
	}

	ATTR_COLD void clear()
	{
		m_end = &m_list[0];
	}

	// save state support & mame disasm

	ATTR_COLD inline const entry_t *listptr() const { return &m_list[0]; }
	ATTR_HOT inline int count() const { return m_end - m_list; }
	ATTR_HOT inline const entry_t & operator[](const int & index) const { return m_list[index]; }

#if (NL_KEEP_STATISTICS)
	// profiling
	INT32   m_prof_start;
	INT32   m_prof_end;
	INT32   m_prof_sortmove;
	INT32   m_prof_sort;
#endif

private:

	entry_t * RESTRICT m_end;
	//entry_t *m_list;
	entry_t m_list[_Size];

};

// ----------------------------------------------------------------------------------------
// netlist_stack_t: a simple stack
// ----------------------------------------------------------------------------------------


template <class _StackClass, int _NumElem = 128>
class netlist_stack_t
{
public:

    ATTR_COLD netlist_stack_t(int numElements = _NumElem)
    : m_list(numElements)
    {
    }

    ATTR_COLD netlist_stack_t(const netlist_stack_t &rhs)
    : m_list(rhs.m_list)
    {
    }

    ATTR_COLD netlist_stack_t &operator=(const netlist_stack_t &rhs)
    {
        m_list = rhs.m_list;
        return *this;
    }


    ATTR_COLD ~netlist_stack_t()
    {
    }

    ATTR_HOT inline void push(const _StackClass &elem)
    {
        m_list.add(elem);
    }

    ATTR_HOT inline _StackClass peek() const
    {
        return m_list[m_list.count() - 1];
    }

    ATTR_HOT inline _StackClass pop()
    {
        _StackClass ret = peek();
        m_list.remove_at(m_list.count() - 1);
        return ret;
    }

    ATTR_HOT inline int count() const { return m_list.count(); }
    ATTR_HOT inline bool empty() const { return (m_list.count() == 0); }
    ATTR_HOT inline void reset() { m_list.reset(); }
    ATTR_HOT inline int capacity() const { return m_list.capacity(); }

private:
    netlist_list_t<_StackClass, _NumElem> m_list;
};


#endif /* NLLISTS_H_ */
