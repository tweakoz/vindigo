/**************************************************************************
 *                      Intel 8039 Portable Emulator                      *
 *                                                                        *
 *                   Copyright Mirko Buffoni                              *
 *  Based on the original work Copyright Dan Boris, an 8048 emulator      *
 *     You are not allowed to distribute this software commercially       *
 *        Please, notify me, if you make any changes to this file         *
 **************************************************************************/

#ifndef _I8039_H
#define _I8039_H

#ifndef INLINE
#define INLINE static inline
#endif

#include "cpuintrf.h"


/**************************************************************************
    Internal Clock divisor

    External Clock is divided internally by 3 to produce the machine state
    generator. This is then divided by 5 for the instruction cycle times.
    (Each instruction cycle passes through 5 machine states).
*/


enum { I8039_PC=1, I8039_SP, I8039_PSW, I8039_A,  I8039_TC,
	   I8039_P1,   I8039_P2, I8039_R0,  I8039_R1, I8039_R2,
	   I8039_R3,   I8039_R4, I8039_R5,  I8039_R6, I8039_R7,
	   I8039_EA
};

extern void i8039_get_info(UINT32 state, cpuinfo *info);

/*   This handling of special I/O ports should be better for actual MAME
 *   architecture.  (i.e., define access to ports { I8039_p1, I8039_p1, dkong_out_w })
 */

#define  I8039_p0	0x100   /* Not used */
#define  I8039_p1	0x101
#define  I8039_p2	0x102
#define  I8039_p4	0x104
#define  I8039_p5	0x105
#define  I8039_p6	0x106
#define  I8039_p7	0x107
#define  I8039_t0	0x110
#define  I8039_t1	0x111
/* EA is activelow input on real hardware. It is treated
 * as active high here to remain compatible with existing drivers! */
#define  I8039_ea	0x112
#define  I8039_bus	0x120

/**************************************************************************
 * I8035 section
 **************************************************************************/
#if (HAS_I8035)
#define I8035_PC				I8039_PC
#define I8035_SP				I8039_SP
#define I8035_PSW				I8039_PSW
#define I8035_A 				I8039_A
#define I8035_TC				I8039_TC
#define I8035_P1				I8039_P1
#define I8035_P2				I8039_P2
#define I8035_R0				I8039_R0
#define I8035_R1				I8039_R1
#define I8035_R2				I8039_R2
#define I8035_R3				I8039_R3
#define I8035_R4				I8039_R4
#define I8035_R5				I8039_R5
#define I8035_R6				I8039_R6
#define I8035_R7				I8039_R7

#define i8035_ICount			i8039_ICount

extern void i8035_get_info(UINT32 state, cpuinfo *info);
#endif

/**************************************************************************
 * I8048 section
 **************************************************************************/
#if (HAS_I8048)
#define I8048_PC				I8039_PC
#define I8048_SP				I8039_SP
#define I8048_PSW				I8039_PSW
#define I8048_A 				I8039_A
#define I8048_TC				I8039_TC
#define I8048_P1				I8039_P1
#define I8048_P2				I8039_P2
#define I8048_R0				I8039_R0
#define I8048_R1				I8039_R1
#define I8048_R2				I8039_R2
#define I8048_R3				I8039_R3
#define I8048_R4				I8039_R4
#define I8048_R5				I8039_R5
#define I8048_R6				I8039_R6
#define I8048_R7				I8039_R7

#define i8048_ICount			i8039_ICount

extern void i8048_get_info(UINT32 state, cpuinfo *info);
#endif

/**************************************************************************
 * I8749 section
 **************************************************************************/
#if (HAS_I8749)
#define I8749_PC				I8039_PC
#define I8749_SP				I8039_SP
#define I8749_PSW				I8039_PSW
#define I8749_A 				I8039_A
#define I8749_TC				I8039_TC
#define I8749_P1				I8039_P1
#define I8749_P2				I8039_P2
#define I8749_R0				I8039_R0
#define I8749_R1				I8039_R1
#define I8749_R2				I8039_R2
#define I8749_R3				I8039_R3
#define I8749_R4				I8039_R4
#define I8749_R5				I8039_R5
#define I8749_R6				I8039_R6
#define I8749_R7				I8039_R7

#define i8749_ICount			i8039_ICount

extern void i8749_get_info(UINT32 state, cpuinfo *info);
#endif

/**************************************************************************
 * N7751 section
 **************************************************************************/
#if (HAS_N7751)
#define N7751_PC				I8039_PC
#define N7751_SP				I8039_SP
#define N7751_PSW				I8039_PSW
#define N7751_A 				I8039_A
#define N7751_TC				I8039_TC
#define N7751_P1				I8039_P1
#define N7751_P2				I8039_P2
#define N7751_R0				I8039_R0
#define N7751_R1				I8039_R1
#define N7751_R2				I8039_R2
#define N7751_R3				I8039_R3
#define N7751_R4				I8039_R4
#define N7751_R5				I8039_R5
#define N7751_R6				I8039_R6
#define N7751_R7				I8039_R7

#define n7751_ICount			i8039_ICount

extern void n7751_get_info(UINT32 state, cpuinfo *info);
#endif

/**************************************************************************
 * MB8884 section
 **************************************************************************/
#if (HAS_MB8884)
#define MB8884_PC				I8039_PC
#define MB8884_SP				I8039_SP
#define MB8884_PSW				I8039_PSW
#define MB8884_A 				I8039_A
#define MB8884_TC				I8039_TC
#define MB8884_P1				I8039_P1
#define MB8884_P2				I8039_P2
#define MB8884_R0				I8039_R0
#define MB8884_R1				I8039_R1
#define MB8884_R2				I8039_R2
#define MB8884_R3				I8039_R3
#define MB8884_R4				I8039_R4
#define MB8884_R5				I8039_R5
#define MB8884_R6				I8039_R6
#define MB8884_R7				I8039_R7

#define mb8884_ICount			i8039_ICount

extern void mb8884_get_info(UINT32 state, cpuinfo *info);
#endif

/**************************************************************************
 * M58715 section
 **************************************************************************/
#if (HAS_M58715)
#define M58715_PC				I8039_PC
#define M58715_SP				I8039_SP
#define M58715_PSW				I8039_PSW
#define M58715_A 				I8039_A
#define M58715_TC				I8039_TC
#define M58715_P1				I8039_P1
#define M58715_P2				I8039_P2
#define M58715_R0				I8039_R0
#define M58715_R1				I8039_R1
#define M58715_R2				I8039_R2
#define M58715_R3				I8039_R3
#define M58715_R4				I8039_R4
#define M58715_R5				I8039_R5
#define M58715_R6				I8039_R6
#define M58715_R7				I8039_R7

#define m58715_ICount			i8039_ICount

extern void m58715_get_info(UINT32 state, cpuinfo *info);
#endif

/*
 *   Input a UINT8 from given I/O port
 */
#define I8039_In(Port) ((UINT8)io_read_byte_8(Port))


/*
 *   Output a UINT8 to given I/O port
 */
#define I8039_Out(Port,Value) (io_write_byte_8(Port,Value))


/*
 *   Read a UINT8 from given memory location
 */
#define I8039_RDMEM(A) ((unsigned)program_read_byte_8(A))


/*
 *   Write a UINT8 to given memory location
 */
#define I8039_WRMEM(A,V) (program_write_byte_8(A,V))


/*
 *   I8039_RDOP() is identical to I8039_RDMEM() except it is used for reading
 *   opcodes. In case of system with memory mapped I/O, this function can be
 *   used to greatly speed up emulation
 */
#define I8039_RDOP(A) ((unsigned)cpu_readop(A))


/*
 *   I8039_RDOP_ARG() is identical to I8039_RDOP() except it is used for reading
 *   opcode arguments. This difference can be used to support systems that
 *   use different encoding mechanisms for opcodes and opcode arguments
 */
#define I8039_RDOP_ARG(A) ((unsigned)cpu_readop_arg(A))

#ifdef  ENABLE_DEBUGGER
offs_t i8039_dasm(char *buffer, offs_t pc, const UINT8 *oprom, const UINT8 *opram);
#endif

#endif  /* _I8039_H */
