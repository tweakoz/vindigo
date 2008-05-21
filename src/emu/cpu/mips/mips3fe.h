/***************************************************************************

    mips3fe.h

    Front-end for MIPS3 recompiler

    Copyright Aaron Giles
    Released for general non-commercial use under the MAME license
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#ifndef __MIPS3FE_H__
#define __MIPS3FE_H__

#include "cpu/drcfe.h"


/***************************************************************************
    CONSTANTS
***************************************************************************/

/* register flags */
#define REGFLAG_R(n)					(((n) == 0) ? 0 : ((UINT64)1 << (n)))
#define REGFLAG_LO						(REGFLAG_R(REG_LO))
#define REGFLAG_HI						(REGFLAG_R(REG_HI))
#define REGFLAG_CPR1(n)					((UINT64)1 << (n))
#define REGFLAG_FCC						(REGFLAG_CPR1(32))



/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

int mips3fe_describe(void *param, opcode_desc *desc);

#endif
