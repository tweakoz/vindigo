/*
 * nl_string.c
 *
 */

#include "pstring.h"
#include <cstdio>


pblockpool *pstring::m_pool = new pblockpool;
pstring::str_t *pstring::m_zero = new(*pstring::m_pool, 0) pstring::str_t(0);

#define IMMEDIATE_MODE  (0)
#define DEBUG_MODE      (0)

pstring::~pstring()
{
   sfree(m_ptr);
}

void pstring::pcat(const char *s)
{
    int slen = strlen(s);
    str_t *n = salloc(m_ptr->len() + slen);
    if (m_ptr->len() > 0)
        memcpy(n->str(), m_ptr->str(), m_ptr->len());
    if (slen > 0)
        memcpy(n->str() + m_ptr->len(), s, slen);
    *(n->str() + n->len()) = 0;
    sfree(m_ptr);
    m_ptr = n;
}

void pstring::pcopy(const char *from, int size)
{
    str_t *n = salloc(size);
    if (size > 0)
        memcpy(n->str(), from, size);
    *(n->str() + size) = 0;
    sfree(m_ptr);
    m_ptr = n;
}

pstring pstring::substr(unsigned int start, int count) const
{
    int alen = len();
    if (start >= alen)
        return pstring();
    if (count <0 || start + count > alen)
        count = alen - start;
    pstring ret;
    ret.pcopy(cstr() + start, count);
    return ret;
}

//-------------------------------------------------
//  pcmpi - compare a character array to an nstring
//-------------------------------------------------

int pstring::pcmpi(const char *lhs, const char *rhs, int count) const
{
    // loop while equal until we hit the end of strings
    int index;
    for (index = 0; index < count; index++)
        if (lhs[index] == 0 || tolower(lhs[index]) != tolower(rhs[index]))
            break;

    // determine the final result
    if (index < count)
        return tolower(lhs[index]) - tolower(rhs[index]);
    if (lhs[index] == 0)
        return 0;
    return 1;
}

pstring pstring::vprintf(va_list args) const
{
    // sprintf into the temporary buffer
    char tempbuf[4096];
    vsprintf(tempbuf, cstr(), args);

    return pstring(tempbuf);
}

// ----------------------------------------------------------------------------------------
// static stuff ...
// ----------------------------------------------------------------------------------------

void pstring::sfree(str_t *s)
{
    s->m_ref_count--;
    if (s->m_ref_count == 0)
        m_pool->dealloc(s);
}

pstring::str_t *pstring::salloc(int n)
{
    str_t *ret = new(*m_pool, n) str_t(n);
    return ret;
}

pstring pstring::sprintf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    pstring ret = pstring(format).vprintf(ap);
    va_end(ap);
    return ret;
}


void pstring::resetmem()
{
    // Release the 0 string
    if (m_zero != NULL)
        sfree(m_zero);
    m_zero = NULL;
    m_pool->m_shutdown = true;
    m_pool->resetmem();
}

// ----------------------------------------------------------------------------------------
// block allocation pool
// ----------------------------------------------------------------------------------------


pblockpool::pblockpool()
    : m_shutdown(false)
    , m_first(NULL)
    , m_blocksize((DEBUG_MODE) ? 0 : 16384)
    , m_align(8)
{
}


void *pblockpool::alloc(const std::size_t n)
{
    if (IMMEDIATE_MODE)
        return (char *) malloc(n);
    else
    {
        int min_alloc = MAX(m_blocksize, n+sizeof(memblock));
        char *ret = NULL;
        int memsize = ((n + m_align - 1) / m_align) * m_align;
        //std::printf("m_first %p\n", m_first);
        for (memblock *p = m_first; p != NULL && ret == NULL; p = p->next)
        {
            if (p->remaining > memsize)
            {
                ret = p->cur;
                p->cur += memsize;
                p->allocated += 1;
                p->remaining -= memsize;
            }
        }

        if (ret == NULL)
        {
            // need to allocate a new block
            memblock *p = (memblock *) malloc(min_alloc); //new char[min_alloc];
            p->allocated = 0;
            p->cur = &p->data[0];
            p->size = p->remaining = min_alloc - sizeof(memblock);
            p->next = m_first;
            //std::printf("allocated block size %d\n", p->size);

            ret = p->cur;
            p->cur += memsize;
            p->allocated += 1;
            p->remaining -= memsize;

            m_first = p;
        }

        return ret;
    }
}

void pblockpool::dealloc(void *ptr)
{
    if (IMMEDIATE_MODE)
        free(ptr);
    else
    {
        for (memblock *p = m_first; p != NULL; p = p->next)
        {
            if (ptr >= &p->data[0] && ptr < &p->data[p->size])
            {
                p->allocated -= 1;
                if (p->allocated < 0)
                    fatalerror("nstring: memory corruption\n");
                if (p->allocated == 0)
                {
                    //std::printf("Block entirely freed\n");
                    p->remaining = p->size;
                    p->cur = &p->data[0];
                }
                // shutting down ?
                if (m_shutdown)
                    resetmem(); // try to free blocks
                return;
            }
        }
        fatalerror("nstring: string <%p> not found\n", ptr);
    }
}

void pblockpool::resetmem()
{
    if (!IMMEDIATE_MODE)
    {
        memblock **p = &m_first;
        int totalblocks = 0;
        int freedblocks = 0;

        while (*p != NULL)
        {
            totalblocks++;
            memblock **next = &((*p)->next);
            if ((*p)->allocated == 0)
            {
                //std::printf("freeing block %p\n", *p);
                memblock *freeme = *p;
                *p = *next;
                free(freeme); //delete[] *p;
                freedblocks++;
            }
            else
            {
                //if (DEBUG_MODE)
                //    std::printf("Allocated: <%s>\n", ((str_t *)(&(*p)->data[0]))->str());

                p = next;
            }
        }
        if (DEBUG_MODE)
            std::printf("Freed %d out of total %d blocks\n", freedblocks, totalblocks);
    }
}
