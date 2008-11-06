#include <stdio.h>
/* ======================================================================== */
/* ========================= LICENSING & COPYRIGHT ======================== */
/* ======================================================================== */
/*
 *                                  MUSASHI
 *                                Version 3.32
 *
 * A portable Motorola M680x0 processor emulation engine.
 * Copyright Karl Stenerud.  All rights reserved.
 *
 * This code may be freely used for non-commercial purposes as long as this
 * copyright notice remains unaltered in the source code and any binary files
 * containing this code in compiled form.
 *
 * All other licensing terms must be negotiated with the author
 * (Karl Stenerud).
 *
 * The latest version of this code can be obtained at:
 * http://kstenerud.cjb.net
 */


#pragma once

#ifndef __M68KCPU_H__
#define __M68KCPU_H__

typedef struct _m68ki_cpu_core m68ki_cpu_core;

#include "m68k.h"
#include <limits.h>
#include <setjmp.h>

/* ======================================================================== */
/* ==================== ARCHITECTURE-DEPENDANT DEFINES ==================== */
/* ======================================================================== */

/* Check for > 32bit sizes */
#define M68K_INT_GT_32_BIT  0

#define MAKE_INT_8(A) (INT8)(A)
#define MAKE_INT_16(A) (INT16)(A)
#define MAKE_INT_32(A) (INT32)(A)


/* ======================================================================== */
/* ============================ GENERAL DEFINES =========================== */
/* ======================================================================== */

/* Exception Vectors handled by emulation */
#define EXCEPTION_BUS_ERROR                2 /* This one is not emulated! */
#define EXCEPTION_ADDRESS_ERROR            3 /* This one is partially emulated (doesn't stack a proper frame yet) */
#define EXCEPTION_ILLEGAL_INSTRUCTION      4
#define EXCEPTION_ZERO_DIVIDE              5
#define EXCEPTION_CHK                      6
#define EXCEPTION_TRAPV                    7
#define EXCEPTION_PRIVILEGE_VIOLATION      8
#define EXCEPTION_TRACE                    9
#define EXCEPTION_1010                    10
#define EXCEPTION_1111                    11
#define EXCEPTION_FORMAT_ERROR            14
#define EXCEPTION_UNINITIALIZED_INTERRUPT 15
#define EXCEPTION_SPURIOUS_INTERRUPT      24
#define EXCEPTION_INTERRUPT_AUTOVECTOR    24
#define EXCEPTION_TRAP_BASE               32

/* Function codes set by CPU during data/address bus activity */
#define FUNCTION_CODE_USER_DATA          1
#define FUNCTION_CODE_USER_PROGRAM       2
#define FUNCTION_CODE_SUPERVISOR_DATA    5
#define FUNCTION_CODE_SUPERVISOR_PROGRAM 6
#define FUNCTION_CODE_CPU_SPACE          7

/* CPU types for deciding what to emulate */
#define CPU_TYPE_000   1
#define CPU_TYPE_008   2
#define CPU_TYPE_010   4
#define CPU_TYPE_EC020 8
#define CPU_TYPE_020   16
#define CPU_TYPE_040   32

/* Different ways to stop the CPU */
#define STOP_LEVEL_STOP 1
#define STOP_LEVEL_HALT 2

/* Used for 68000 address error processing */
#define INSTRUCTION_YES 0
#define INSTRUCTION_NO  0x08
#define MODE_READ       0x10
#define MODE_WRITE      0

#define RUN_MODE_NORMAL          0
#define RUN_MODE_BERR_AERR_RESET 1


/* ======================================================================== */
/* ================================ MACROS ================================ */
/* ======================================================================== */


/* ---------------------------- General Macros ---------------------------- */

/* Bit Isolation Macros */
#define BIT_0(A)  ((A) & 0x00000001)
#define BIT_1(A)  ((A) & 0x00000002)
#define BIT_2(A)  ((A) & 0x00000004)
#define BIT_3(A)  ((A) & 0x00000008)
#define BIT_4(A)  ((A) & 0x00000010)
#define BIT_5(A)  ((A) & 0x00000020)
#define BIT_6(A)  ((A) & 0x00000040)
#define BIT_7(A)  ((A) & 0x00000080)
#define BIT_8(A)  ((A) & 0x00000100)
#define BIT_9(A)  ((A) & 0x00000200)
#define BIT_A(A)  ((A) & 0x00000400)
#define BIT_B(A)  ((A) & 0x00000800)
#define BIT_C(A)  ((A) & 0x00001000)
#define BIT_D(A)  ((A) & 0x00002000)
#define BIT_E(A)  ((A) & 0x00004000)
#define BIT_F(A)  ((A) & 0x00008000)
#define BIT_10(A) ((A) & 0x00010000)
#define BIT_11(A) ((A) & 0x00020000)
#define BIT_12(A) ((A) & 0x00040000)
#define BIT_13(A) ((A) & 0x00080000)
#define BIT_14(A) ((A) & 0x00100000)
#define BIT_15(A) ((A) & 0x00200000)
#define BIT_16(A) ((A) & 0x00400000)
#define BIT_17(A) ((A) & 0x00800000)
#define BIT_18(A) ((A) & 0x01000000)
#define BIT_19(A) ((A) & 0x02000000)
#define BIT_1A(A) ((A) & 0x04000000)
#define BIT_1B(A) ((A) & 0x08000000)
#define BIT_1C(A) ((A) & 0x10000000)
#define BIT_1D(A) ((A) & 0x20000000)
#define BIT_1E(A) ((A) & 0x40000000)
#define BIT_1F(A) ((A) & 0x80000000)

/* Get the most significant bit for specific sizes */
#define GET_MSB_8(A)  ((A) & 0x80)
#define GET_MSB_9(A)  ((A) & 0x100)
#define GET_MSB_16(A) ((A) & 0x8000)
#define GET_MSB_17(A) ((A) & 0x10000)
#define GET_MSB_32(A) ((A) & 0x80000000)
#if M68K_USE_64_BIT
#define GET_MSB_33(A) ((A) & 0x100000000)
#endif /* M68K_USE_64_BIT */

/* Isolate nibbles */
#define LOW_NIBBLE(A)  ((A) & 0x0f)
#define HIGH_NIBBLE(A) ((A) & 0xf0)

/* These are used to isolate 8, 16, and 32 bit sizes */
#define MASK_OUT_ABOVE_2(A)  ((A) & 3)
#define MASK_OUT_ABOVE_8(A)  ((A) & 0xff)
#define MASK_OUT_ABOVE_16(A) ((A) & 0xffff)
#define MASK_OUT_BELOW_2(A)  ((A) & ~3)
#define MASK_OUT_BELOW_8(A)  ((A) & ~0xff)
#define MASK_OUT_BELOW_16(A) ((A) & ~0xffff)

/* No need to mask if we are 32 bit */
#if M68K_INT_GT_32_BIT || M68K_USE_64_BIT
	#define MASK_OUT_ABOVE_32(A) ((A) & 0xffffffff)
	#define MASK_OUT_BELOW_32(A) ((A) & ~0xffffffff)
#else
	#define MASK_OUT_ABOVE_32(A) (A)
	#define MASK_OUT_BELOW_32(A) 0
#endif /* M68K_INT_GT_32_BIT || M68K_USE_64_BIT */

/* Simulate address lines of 68k family */
#define ADDRESS_68K(A) ((A)&m68k->address_mask)


/* Shift & Rotate Macros. */
#define LSL(A, C) ((A) << (C))
#define LSR(A, C) ((A) >> (C))

/* Some > 32-bit optimizations */
#if M68K_INT_GT_32_BIT
	/* Shift left and right */
	#define LSR_32(A, C) ((A) >> (C))
	#define LSL_32(A, C) ((A) << (C))
#else
	/* We have to do this because the morons at ANSI decided that shifts
     * by >= data size are undefined.
     */
	#define LSR_32(A, C) ((C) < 32 ? (A) >> (C) : 0)
	#define LSL_32(A, C) ((C) < 32 ? (A) << (C) : 0)
#endif /* M68K_INT_GT_32_BIT */

#if M68K_USE_64_BIT
	#define LSL_32_64(A, C) ((A) << (C))
	#define LSR_32_64(A, C) ((A) >> (C))
	#define ROL_33_64(A, C) (LSL_32_64(A, C) | LSR_32_64(A, 33-(C)))
	#define ROR_33_64(A, C) (LSR_32_64(A, C) | LSL_32_64(A, 33-(C)))
#endif /* M68K_USE_64_BIT */

#define ROL_8(A, C)      MASK_OUT_ABOVE_8(LSL(A, C) | LSR(A, 8-(C)))
#define ROL_9(A, C)                      (LSL(A, C) | LSR(A, 9-(C)))
#define ROL_16(A, C)    MASK_OUT_ABOVE_16(LSL(A, C) | LSR(A, 16-(C)))
#define ROL_17(A, C)                     (LSL(A, C) | LSR(A, 17-(C)))
#define ROL_32(A, C)    MASK_OUT_ABOVE_32(LSL_32(A, C) | LSR_32(A, 32-(C)))
#define ROL_33(A, C)                     (LSL_32(A, C) | LSR_32(A, 33-(C)))

#define ROR_8(A, C)      MASK_OUT_ABOVE_8(LSR(A, C) | LSL(A, 8-(C)))
#define ROR_9(A, C)                      (LSR(A, C) | LSL(A, 9-(C)))
#define ROR_16(A, C)    MASK_OUT_ABOVE_16(LSR(A, C) | LSL(A, 16-(C)))
#define ROR_17(A, C)                     (LSR(A, C) | LSL(A, 17-(C)))
#define ROR_32(A, C)    MASK_OUT_ABOVE_32(LSR_32(A, C) | LSL_32(A, 32-(C)))
#define ROR_33(A, C)                     (LSR_32(A, C) | LSL_32(A, 33-(C)))



/* ------------------------------ CPU Access ------------------------------ */

/* Access the CPU registers */
#define REG_DA           m68k->dar /* easy access to data and address regs */
#define REG_D            m68k->dar
#define REG_A            (m68k->dar+8)
#define REG_PPC 		 m68k->ppc
#define REG_PC           m68k->pc
#define REG_SP_BASE      m68k->sp
#define REG_USP          m68k->sp[0]
#define REG_ISP          m68k->sp[4]
#define REG_MSP          m68k->sp[6]
#define REG_SP           m68k->dar[15]
#define REG_VBR          m68k->vbr
#define REG_SFC          m68k->sfc
#define REG_DFC          m68k->dfc
#define REG_CACR         m68k->cacr
#define REG_CAAR         m68k->caar
#define REG_IR           m68k->ir

#define REG_FP           m68k->fpr
#define REG_FPCR         m68k->fpcr
#define REG_FPSR         m68k->fpsr
#define REG_FPIAR        m68k->fpiar

#define FLAG_T1          m68k->t1_flag
#define FLAG_T0          m68k->t0_flag
#define FLAG_S           m68k->s_flag
#define FLAG_M           m68k->m_flag
#define FLAG_X           m68k->x_flag
#define FLAG_N           m68k->n_flag
#define FLAG_Z           m68k->not_z_flag
#define FLAG_V           m68k->v_flag
#define FLAG_C           m68k->c_flag
#define FLAG_INT_MASK    m68k->int_mask


/* ----------------------------- Configuration ---------------------------- */

/* These defines are dependant on the configuration defines in m68kconf.h */

/* Disable certain comparisons if we're not using all CPU types */
#if HAS_M68040
	#define CPU_TYPE_IS_040_PLUS(A)    ((A) & CPU_TYPE_040)
	#define CPU_TYPE_IS_040_LESS(A)    1
#else
	#define CPU_TYPE_IS_040_PLUS(A)    0
	#define CPU_TYPE_IS_040_LESS(A)    1
#endif

#if HAS_M68020
	#define CPU_TYPE_IS_020_PLUS(A)    ((A) & (CPU_TYPE_020 | CPU_TYPE_040))
	#define CPU_TYPE_IS_020_LESS(A)    1
#else
	#define CPU_TYPE_IS_020_PLUS(A)    0
	#define CPU_TYPE_IS_020_LESS(A)    1
#endif

#if HAS_M68EC020
	#define CPU_TYPE_IS_EC020_PLUS(A)  ((A) & (CPU_TYPE_EC020 | CPU_TYPE_020 | CPU_TYPE_040))
	#define CPU_TYPE_IS_EC020_LESS(A)  ((A) & (CPU_TYPE_000 | CPU_TYPE_008 | CPU_TYPE_010 | CPU_TYPE_EC020))
#else
	#define CPU_TYPE_IS_EC020_PLUS(A)  CPU_TYPE_IS_020_PLUS(A)
	#define CPU_TYPE_IS_EC020_LESS(A)  CPU_TYPE_IS_020_LESS(A)
#endif

#if HAS_M68010
	#define CPU_TYPE_IS_010(A)         ((A) == CPU_TYPE_010)
	#define CPU_TYPE_IS_010_PLUS(A)    ((A) & (CPU_TYPE_010 | CPU_TYPE_EC020 | CPU_TYPE_020 | CPU_TYPE_040))
	#define CPU_TYPE_IS_010_LESS(A)    ((A) & (CPU_TYPE_000 | CPU_TYPE_008 | CPU_TYPE_010))
#else
	#define CPU_TYPE_IS_010(A)         0
	#define CPU_TYPE_IS_010_PLUS(A)    CPU_TYPE_IS_EC020_PLUS(A)
	#define CPU_TYPE_IS_010_LESS(A)    CPU_TYPE_IS_EC020_LESS(A)
#endif

#if HAS_M68020 || HAS_M68EC020
	#define CPU_TYPE_IS_020_VARIANT(A) ((A) & (CPU_TYPE_EC020 | CPU_TYPE_020))
#else
	#define CPU_TYPE_IS_020_VARIANT(A) 0
#endif

#if HAS_M68040 || HAS_M68020 || HAS_M68EC020 || HAS_M68010
	#define CPU_TYPE_IS_000(A)         ((A) == CPU_TYPE_000 || (A) == CPU_TYPE_008)
#else
	#define CPU_TYPE_IS_000(A)         1
#endif


#if !M68K_SEPARATE_READS
#define m68k_read_immediate_16(M, A) m68ki_read_program_16(M, A)
#define m68k_read_immediate_32(M, A) m68ki_read_program_32(M, A)

#define m68k_read_pcrelative_8(M, A) m68ki_read_program_8(M, A)
#define m68k_read_pcrelative_16(M, A) m68ki_read_program_16(M, A)
#define m68k_read_pcrelative_32(M, A) m68ki_read_program_32(M, A)
#endif /* M68K_SEPARATE_READS */


/* Enable or disable function code emulation */
#if M68K_EMULATE_FC
	#if M68K_EMULATE_FC == OPT_SPECIFY_HANDLER
		#define m68ki_set_fc(A) M68K_SET_FC_CALLBACK(A)
	#else
		#define m68ki_set_fc(A) m68k->set_fc_callback(A)
	#endif
	#define m68ki_use_data_space(M) (M)->address_space = FUNCTION_CODE_USER_DATA
	#define m68ki_use_program_space(M) (M)->address_space = FUNCTION_CODE_USER_PROGRAM
	#define m68ki_get_address_space(M) (M)->address_space
#else
	#define m68ki_set_fc(A)
	#define m68ki_use_data_space(M)
	#define m68ki_use_program_space(M)
	#define m68ki_get_address_space(M) FUNCTION_CODE_USER_DATA
#endif /* M68K_EMULATE_FC */


/* Enable or disable trace emulation */
#if M68K_EMULATE_TRACE
	/* Initiates trace checking before each instruction (t1) */
	#define m68ki_trace_t1() m68ki_tracing = FLAG_T1
	/* adds t0 to trace checking if we encounter change of flow */
	#define m68ki_trace_t0() m68ki_tracing |= FLAG_T0
	/* Clear all tracing */
	#define m68ki_clear_trace() m68ki_tracing = 0
	/* Cause a trace exception if we are tracing */
	#define m68ki_exception_if_trace() if(m68ki_tracing) m68ki_exception_trace(m68k)
#else
	#define m68ki_trace_t1()
	#define m68ki_trace_t0()
	#define m68ki_clear_trace()
	#define m68ki_exception_if_trace()
#endif /* M68K_EMULATE_TRACE */



/* Address error */
#define m68ki_set_address_error_trap(m68k) \
	if(setjmp(m68k->aerr_trap) != 0) \
	{ \
		m68ki_exception_address_error(m68k); \
		if(m68k->stopped) \
		{ \
			SET_CYCLES(m68k, 0); \
			return m68k->initial_cycles; \
		} \
	}

#define m68ki_check_address_error(m68k, ADDR, WRITE_MODE, FC) \
	if((ADDR)&1) \
	{ \
		m68k->aerr_address = ADDR; \
		m68k->aerr_write_mode = WRITE_MODE; \
		m68k->aerr_fc = FC; \
		longjmp(m68k->aerr_trap, 1); \
	}

#define m68ki_check_address_error_010_less(m68k, ADDR, WRITE_MODE, FC) \
	if (CPU_TYPE_IS_010_LESS(m68k->cpu_type)) \
	{ \
		m68ki_check_address_error(m68k, ADDR, WRITE_MODE, FC) \
	}

/* Logging */
#if M68K_LOG_ENABLE
	#include <stdio.h>
	extern FILE* M68K_LOG_FILEHANDLE
	extern const char *const m68ki_cpu_names[];

	#define M68K_DO_LOG(A) if(M68K_LOG_FILEHANDLE) fprintf A
	#if M68K_LOG_1010_1111
		#define M68K_DO_LOG_EMU(A) if(M68K_LOG_FILEHANDLE) fprintf A
	#else
		#define M68K_DO_LOG_EMU(A)
	#endif
#else
	#define M68K_DO_LOG(A)
	#define M68K_DO_LOG_EMU(A)
#endif



/* -------------------------- EA / Operand Access ------------------------- */

/*
 * The general instruction format follows this pattern:
 * .... XXX. .... .YYY
 * where XXX is register X and YYY is register Y
 */
/* Data Register Isolation */
#define DX (REG_D[(REG_IR >> 9) & 7])
#define DY (REG_D[REG_IR & 7])
/* Address Register Isolation */
#define AX (REG_A[(REG_IR >> 9) & 7])
#define AY (REG_A[REG_IR & 7])


/* Effective Address Calculations */
#define EA_AY_AI_8(m68k)   AY                                    /* address register indirect */
#define EA_AY_AI_16(m68k)  EA_AY_AI_8(m68k)
#define EA_AY_AI_32(m68k)  EA_AY_AI_8(m68k)
#define EA_AY_PI_8(m68k)   (AY++)                                /* postincrement (size = byte) */
#define EA_AY_PI_16(m68k)  ((AY+=2)-2)                           /* postincrement (size = word) */
#define EA_AY_PI_32(m68k)  ((AY+=4)-4)                           /* postincrement (size = long) */
#define EA_AY_PD_8(m68k)   (--AY)                                /* predecrement (size = byte) */
#define EA_AY_PD_16(m68k)  (AY-=2)                               /* predecrement (size = word) */
#define EA_AY_PD_32(m68k)  (AY-=4)                               /* predecrement (size = long) */
#define EA_AY_DI_8(m68k)   (AY+MAKE_INT_16(m68ki_read_imm_16(m68k))) /* displacement */
#define EA_AY_DI_16(m68k)  EA_AY_DI_8(m68k)
#define EA_AY_DI_32(m68k)  EA_AY_DI_8(m68k)
#define EA_AY_IX_8(m68k)   m68ki_get_ea_ix(m68k, AY)                   /* indirect + index */
#define EA_AY_IX_16(m68k)  EA_AY_IX_8(m68k)
#define EA_AY_IX_32(m68k)  EA_AY_IX_8(m68k)

#define EA_AX_AI_8(m68k)   AX
#define EA_AX_AI_16(m68k)  EA_AX_AI_8(m68k)
#define EA_AX_AI_32(m68k)  EA_AX_AI_8(m68k)
#define EA_AX_PI_8(m68k)   (AX++)
#define EA_AX_PI_16(m68k)  ((AX+=2)-2)
#define EA_AX_PI_32(m68k)  ((AX+=4)-4)
#define EA_AX_PD_8(m68k)   (--AX)
#define EA_AX_PD_16(m68k)  (AX-=2)
#define EA_AX_PD_32(m68k)  (AX-=4)
#define EA_AX_DI_8(m68k)   (AX+MAKE_INT_16(m68ki_read_imm_16(m68k)))
#define EA_AX_DI_16(m68k)  EA_AX_DI_8(m68k)
#define EA_AX_DI_32(m68k)  EA_AX_DI_8(m68k)
#define EA_AX_IX_8(m68k)   m68ki_get_ea_ix(m68k, AX)
#define EA_AX_IX_16(m68k)  EA_AX_IX_8(m68k)
#define EA_AX_IX_32(m68k)  EA_AX_IX_8(m68k)

#define EA_A7_PI_8(m68k)   ((REG_A[7]+=2)-2)
#define EA_A7_PD_8(m68k)   (REG_A[7]-=2)

#define EA_AW_8(m68k)      MAKE_INT_16(m68ki_read_imm_16(m68k))      /* absolute word */
#define EA_AW_16(m68k)     EA_AW_8(m68k)
#define EA_AW_32(m68k)     EA_AW_8(m68k)
#define EA_AL_8(m68k)      m68ki_read_imm_32(m68k)                   /* absolute long */
#define EA_AL_16(m68k)     EA_AL_8(m68k)
#define EA_AL_32(m68k)     EA_AL_8(m68k)
#define EA_PCDI_8(m68k)    m68ki_get_ea_pcdi(m68k)                   /* pc indirect + displacement */
#define EA_PCDI_16(m68k)   EA_PCDI_8(m68k)
#define EA_PCDI_32(m68k)   EA_PCDI_8(m68k)
#define EA_PCIX_8(m68k)    m68ki_get_ea_pcix(m68k)                   /* pc indirect + index */
#define EA_PCIX_16(m68k)   EA_PCIX_8(m68k)
#define EA_PCIX_32(m68k)   EA_PCIX_8(m68k)


#define OPER_I_8(m68k)     m68ki_read_imm_8(m68k)
#define OPER_I_16(m68k)    m68ki_read_imm_16(m68k)
#define OPER_I_32(m68k)    m68ki_read_imm_32(m68k)



/* --------------------------- Status Register ---------------------------- */

/* Flag Calculation Macros */
#define CFLAG_8(A) (A)
#define CFLAG_16(A) ((A)>>8)

#if M68K_INT_GT_32_BIT
	#define CFLAG_ADD_32(S, D, R) ((R)>>24)
	#define CFLAG_SUB_32(S, D, R) ((R)>>24)
#else
	#define CFLAG_ADD_32(S, D, R) (((S & D) | (~R & (S | D)))>>23)
	#define CFLAG_SUB_32(S, D, R) (((S & R) | (~D & (S | R)))>>23)
#endif /* M68K_INT_GT_32_BIT */

#define VFLAG_ADD_8(S, D, R) ((S^R) & (D^R))
#define VFLAG_ADD_16(S, D, R) (((S^R) & (D^R))>>8)
#define VFLAG_ADD_32(S, D, R) (((S^R) & (D^R))>>24)

#define VFLAG_SUB_8(S, D, R) ((S^D) & (R^D))
#define VFLAG_SUB_16(S, D, R) (((S^D) & (R^D))>>8)
#define VFLAG_SUB_32(S, D, R) (((S^D) & (R^D))>>24)

#define NFLAG_8(A) (A)
#define NFLAG_16(A) ((A)>>8)
#define NFLAG_32(A) ((A)>>24)
#define NFLAG_64(A) ((A)>>56)

#define ZFLAG_8(A) MASK_OUT_ABOVE_8(A)
#define ZFLAG_16(A) MASK_OUT_ABOVE_16(A)
#define ZFLAG_32(A) MASK_OUT_ABOVE_32(A)


/* Flag values */
#define NFLAG_SET   0x80
#define NFLAG_CLEAR 0
#define CFLAG_SET   0x100
#define CFLAG_CLEAR 0
#define XFLAG_SET   0x100
#define XFLAG_CLEAR 0
#define VFLAG_SET   0x80
#define VFLAG_CLEAR 0
#define ZFLAG_SET   0
#define ZFLAG_CLEAR 0xffffffff

#define SFLAG_SET   4
#define SFLAG_CLEAR 0
#define MFLAG_SET   2
#define MFLAG_CLEAR 0

/* Turn flag values into 1 or 0 */
#define XFLAG_AS_1() ((FLAG_X>>8)&1)
#define NFLAG_AS_1() ((FLAG_N>>7)&1)
#define VFLAG_AS_1() ((FLAG_V>>7)&1)
#define ZFLAG_AS_1() (!FLAG_Z)
#define CFLAG_AS_1() ((FLAG_C>>8)&1)


/* Conditions */
#define COND_CS() (FLAG_C&0x100)
#define COND_CC() (!COND_CS())
#define COND_VS() (FLAG_V&0x80)
#define COND_VC() (!COND_VS())
#define COND_NE() FLAG_Z
#define COND_EQ() (!COND_NE())
#define COND_MI() (FLAG_N&0x80)
#define COND_PL() (!COND_MI())
#define COND_LT() ((FLAG_N^FLAG_V)&0x80)
#define COND_GE() (!COND_LT())
#define COND_HI() (COND_CC() && COND_NE())
#define COND_LS() (COND_CS() || COND_EQ())
#define COND_GT() (COND_GE() && COND_NE())
#define COND_LE() (COND_LT() || COND_EQ())

/* Reversed conditions */
#define COND_NOT_CS() COND_CC()
#define COND_NOT_CC() COND_CS()
#define COND_NOT_VS() COND_VC()
#define COND_NOT_VC() COND_VS()
#define COND_NOT_NE() COND_EQ()
#define COND_NOT_EQ() COND_NE()
#define COND_NOT_MI() COND_PL()
#define COND_NOT_PL() COND_MI()
#define COND_NOT_LT() COND_GE()
#define COND_NOT_GE() COND_LT()
#define COND_NOT_HI() COND_LS()
#define COND_NOT_LS() COND_HI()
#define COND_NOT_GT() COND_LE()
#define COND_NOT_LE() COND_GT()

/* Not real conditions, but here for convenience */
#define COND_XS() (FLAG_X&0x100)
#define COND_XC() (!COND_XS)


/* Get the condition code register */
#define m68ki_get_ccr(m68k) ((COND_XS() >> 4) | \
						     (COND_MI() >> 4) | \
						     (COND_EQ() << 2) | \
						     (COND_VS() >> 6) | \
						     (COND_CS() >> 8))

/* Get the status register */
#define m68ki_get_sr(m68k) ( FLAG_T1              | \
						     FLAG_T0              | \
						    (FLAG_S        << 11) | \
						    (FLAG_M        << 11) | \
						     FLAG_INT_MASK        | \
						     m68ki_get_ccr(m68k))



/* ---------------------------- Cycle Counting ---------------------------- */

#define ADD_CYCLES(M, A)			(M)->remaining_cycles += (A)
#define USE_CYCLES(M, A)			(M)->remaining_cycles -= (A)
#define SET_CYCLES(M, A)			(M)->remaining_cycles = A
#define GET_CYCLES(M)				(M)->remaining_cycles
#define USE_ALL_CYCLES(M) 			(M)->remaining_cycles = 0



/* ----------------------------- Read / Write ----------------------------- */

/* Read from the current address space */
#define m68ki_read_8(M, A)			m68ki_read_8_fc (M, A, FLAG_S | m68ki_get_address_space(M))
#define m68ki_read_16(M, A)			m68ki_read_16_fc(M, A, FLAG_S | m68ki_get_address_space(M))
#define m68ki_read_32(M, A)			m68ki_read_32_fc(M, A, FLAG_S | m68ki_get_address_space(M))

/* Write to the current data space */
#define m68ki_write_8(M, A, V)		m68ki_write_8_fc (M, A, FLAG_S | FUNCTION_CODE_USER_DATA, V)
#define m68ki_write_16(M, A, V)		m68ki_write_16_fc(M, A, FLAG_S | FUNCTION_CODE_USER_DATA, V)
#define m68ki_write_32(M, A, V)		m68ki_write_32_fc(M, A, FLAG_S | FUNCTION_CODE_USER_DATA, V)
#define m68ki_write_32_pd(M, A, V)	m68ki_write_32_pd_fc(M, A, FLAG_S | FUNCTION_CODE_USER_DATA, V)

/* map read immediate 8 to read immediate 16 */
#define m68ki_read_imm_8(M)			MASK_OUT_ABOVE_8(m68ki_read_imm_16(M))

/* Map PC-relative reads */
#define m68ki_read_pcrel_8(M, A) 	m68k_read_pcrelative_8(M, A)
#define m68ki_read_pcrel_16(M, A) 	m68k_read_pcrelative_16(M, A)
#define m68ki_read_pcrel_32(M, A) 	m68k_read_pcrelative_32(M, A)

/* Read from the program space */
#define m68ki_read_program_8(M, A) 	m68ki_read_8_fc(M, A, FLAG_S | FUNCTION_CODE_USER_PROGRAM)
#define m68ki_read_program_16(M, A) m68ki_read_16_fc(M, A, FLAG_S | FUNCTION_CODE_USER_PROGRAM)
#define m68ki_read_program_32(M, A) m68ki_read_32_fc(M, A, FLAG_S | FUNCTION_CODE_USER_PROGRAM)

/* Read from the data space */
#define m68ki_read_data_8(M, A) 	m68ki_read_8_fc(M, A, FLAG_S | FUNCTION_CODE_USER_DATA)
#define m68ki_read_data_16(M, A) 	m68ki_read_16_fc(M, A, FLAG_S | FUNCTION_CODE_USER_DATA)
#define m68ki_read_data_32(M, A) 	m68ki_read_32_fc(M, A, FLAG_S | FUNCTION_CODE_USER_DATA)



/* ======================================================================== */
/* =============================== PROTOTYPES ============================= */
/* ======================================================================== */

typedef union _fp_reg fp_reg;
union _fp_reg
{
	UINT64 i;
	double f;
};


struct _m68ki_cpu_core
{
	UINT32 cpu_type;     /* CPU Type: 68000, 68008, 68010, 68EC020, or 68020 */
	UINT32 dar[16];      /* Data and Address Registers */
	UINT32 ppc;		   /* Previous program counter */
	UINT32 pc;           /* Program Counter */
	UINT32 sp[7];        /* User, Interrupt, and Master Stack Pointers */
	UINT32 vbr;          /* Vector Base Register (m68010+) */
	UINT32 sfc;          /* Source Function Code Register (m68010+) */
	UINT32 dfc;          /* Destination Function Code Register (m68010+) */
	UINT32 cacr;         /* Cache Control Register (m68020, unemulated) */
	UINT32 caar;         /* Cache Address Register (m68020, unemulated) */
	UINT32 ir;           /* Instruction Register */
    fp_reg fpr[8];     /* FPU Data Register (m68040) */
	UINT32 fpiar;        /* FPU Instruction Address Register (m68040) */
	UINT32 fpsr;         /* FPU Status Register (m68040) */
	UINT32 fpcr;         /* FPU Control Register (m68040) */
	UINT32 t1_flag;      /* Trace 1 */
	UINT32 t0_flag;      /* Trace 0 */
	UINT32 s_flag;       /* Supervisor */
	UINT32 m_flag;       /* Master/Interrupt state */
	UINT32 x_flag;       /* Extend */
	UINT32 n_flag;       /* Negative */
	UINT32 not_z_flag;   /* Zero, inverted for speedups */
	UINT32 v_flag;       /* Overflow */
	UINT32 c_flag;       /* Carry */
	UINT32 int_mask;     /* I0-I2 */
	UINT32 int_level;    /* State of interrupt pins IPL0-IPL2 -- ASG: changed from ints_pending */
	UINT32 stopped;      /* Stopped state */
	UINT32 pref_addr;    /* Last prefetch address */
	UINT32 pref_data;    /* Data in the prefetch queue */
	UINT32 address_mask; /* Available address pins */
	UINT32 sr_mask;      /* Implemented status register bits */
	UINT32 instr_mode;   /* Stores whether we are in instruction mode or group 0/1 exception mode */
	UINT32 run_mode;     /* Stores whether we are processing a reset, bus error, address error, or something else */

	/* Clocks required for instructions / exceptions */
	UINT32 cyc_bcc_notake_b;
	UINT32 cyc_bcc_notake_w;
	UINT32 cyc_dbcc_f_noexp;
	UINT32 cyc_dbcc_f_exp;
	UINT32 cyc_scc_r_true;
	UINT32 cyc_movem_w;
	UINT32 cyc_movem_l;
	UINT32 cyc_shift;
	UINT32 cyc_reset;

	int  initial_cycles;
	int  remaining_cycles;                     /* Number of clocks remaining */
	UINT32 tracing;
	UINT32 address_space;

	jmp_buf aerr_trap;
	UINT32    aerr_address;
	UINT32    aerr_write_mode;
	UINT32    aerr_fc;

	/* Virtual IRQ lines state */
	UINT32 virq_state;
	UINT32 nmi_pending;

	const UINT8* cyc_instruction;
	const UINT8* cyc_exception;

	/* Callbacks to host */
	int  (*int_ack_callback)(void *param, int int_line);/* Interrupt Acknowledge */
	void *int_ack_param;
	void (*bkpt_ack_callback)(unsigned int data);     /* Breakpoint Acknowledge */
	void (*reset_instr_callback)(void);               /* Called when a RESET instruction is encountered */
	void (*cmpild_instr_callback)(unsigned int, int); /* Called when a CMPI.L #v, Dn instruction is encountered */
	void (*rte_instr_callback)(void);                 /* Called when a RTE instruction is encountered */
	int  (*tas_instr_callback)(void);                 /* Called when a TAS instruction is encountered, allows / disallows writeback */
	void (*pc_changed_callback)(unsigned int new_pc); /* Called when the PC changes by a large amount */
	void (*set_fc_callback)(unsigned int new_fc);     /* Called when the CPU function code changes */
	void (*instr_hook_callback)(unsigned int pc);     /* Called every instruction cycle prior to execution */

};


extern const UINT8    m68ki_shift_8_table[];
extern const UINT16   m68ki_shift_16_table[];
extern const UINT32     m68ki_shift_32_table[];
extern const UINT8    m68ki_exception_cycle_table[][256];
extern const UINT8    m68ki_ea_idx_cycle_table[];

/* Read data immediately after the program counter */
INLINE UINT32 m68ki_read_imm_16(m68ki_cpu_core *m68k);
INLINE UINT32 m68ki_read_imm_32(m68ki_cpu_core *m68k);

/* Read data with specific function code */
INLINE UINT32 m68ki_read_8_fc  (m68ki_cpu_core *m68k, UINT32 address, UINT32 fc);
INLINE UINT32 m68ki_read_16_fc (m68ki_cpu_core *m68k, UINT32 address, UINT32 fc);
INLINE UINT32 m68ki_read_32_fc (m68ki_cpu_core *m68k, UINT32 address, UINT32 fc);

/* Write data with specific function code */
INLINE void m68ki_write_8_fc (m68ki_cpu_core *m68k, UINT32 address, UINT32 fc, UINT32 value);
INLINE void m68ki_write_16_fc(m68ki_cpu_core *m68k, UINT32 address, UINT32 fc, UINT32 value);
INLINE void m68ki_write_32_fc(m68ki_cpu_core *m68k, UINT32 address, UINT32 fc, UINT32 value);
INLINE void m68ki_write_32_pd_fc(m68ki_cpu_core *m68k, UINT32 address, UINT32 fc, UINT32 value);

/* Indexed and PC-relative ea fetching */
INLINE UINT32 m68ki_get_ea_pcdi(m68ki_cpu_core *m68k);
INLINE UINT32 m68ki_get_ea_pcix(m68ki_cpu_core *m68k);
INLINE UINT32 m68ki_get_ea_ix(m68ki_cpu_core *m68k, UINT32 An);

/* Operand fetching */
INLINE UINT32 OPER_AY_AI_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_AI_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_AI_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_PI_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_PI_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_PI_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_PD_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_PD_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_PD_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_DI_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_DI_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_DI_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_IX_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_IX_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AY_IX_32(m68ki_cpu_core *m68k);

INLINE UINT32 OPER_AX_AI_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_AI_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_AI_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_PI_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_PI_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_PI_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_PD_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_PD_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_PD_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_DI_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_DI_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_DI_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_IX_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_IX_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AX_IX_32(m68ki_cpu_core *m68k);

INLINE UINT32 OPER_A7_PI_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_A7_PD_8(m68ki_cpu_core *m68k);

INLINE UINT32 OPER_AW_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AW_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AW_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AL_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AL_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_AL_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_PCDI_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_PCDI_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_PCDI_32(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_PCIX_8(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_PCIX_16(m68ki_cpu_core *m68k);
INLINE UINT32 OPER_PCIX_32(m68ki_cpu_core *m68k);

/* Stack operations */
INLINE void m68ki_push_16(m68ki_cpu_core *m68k, UINT32 value);
INLINE void m68ki_push_32(m68ki_cpu_core *m68k, UINT32 value);
INLINE UINT32 m68ki_pull_16(m68ki_cpu_core *m68k);
INLINE UINT32 m68ki_pull_32(m68ki_cpu_core *m68k);

/* Program flow operations */
INLINE void m68ki_jump(m68ki_cpu_core *m68k, UINT32 new_pc);
INLINE void m68ki_jump_vector(m68ki_cpu_core *m68k, UINT32 vector);
INLINE void m68ki_branch_8(m68ki_cpu_core *m68k, UINT32 offset);
INLINE void m68ki_branch_16(m68ki_cpu_core *m68k, UINT32 offset);
INLINE void m68ki_branch_32(m68ki_cpu_core *m68k, UINT32 offset);

/* Status register operations. */
INLINE void m68ki_set_s_flag(m68ki_cpu_core *m68k, UINT32 value);            /* Only bit 2 of value should be set (i.e. 4 or 0) */
INLINE void m68ki_set_sm_flag(m68ki_cpu_core *m68k, UINT32 value);           /* only bits 1 and 2 of value should be set */
INLINE void m68ki_set_ccr(m68ki_cpu_core *m68k, UINT32 value);               /* set the condition code register */
INLINE void m68ki_set_sr(m68ki_cpu_core *m68k, UINT32 value);                /* set the status register */
INLINE void m68ki_set_sr_noint(m68ki_cpu_core *m68k, UINT32 value);          /* set the status register */

/* Exception processing */
INLINE UINT32 m68ki_init_exception(m68ki_cpu_core *m68k);              /* Initial exception processing */

INLINE void m68ki_stack_frame_3word(m68ki_cpu_core *m68k, UINT32 pc, UINT32 sr); /* Stack various frame types */
INLINE void m68ki_stack_frame_buserr(m68ki_cpu_core *m68k, UINT32 sr);

INLINE void m68ki_stack_frame_0000(m68ki_cpu_core *m68k, UINT32 pc, UINT32 sr, UINT32 vector);
INLINE void m68ki_stack_frame_0001(m68ki_cpu_core *m68k, UINT32 pc, UINT32 sr, UINT32 vector);
INLINE void m68ki_stack_frame_0010(m68ki_cpu_core *m68k, UINT32 sr, UINT32 vector);
INLINE void m68ki_stack_frame_1000(m68ki_cpu_core *m68k, UINT32 pc, UINT32 sr, UINT32 vector);
INLINE void m68ki_stack_frame_1010(m68ki_cpu_core *m68k, UINT32 sr, UINT32 vector, UINT32 pc);
INLINE void m68ki_stack_frame_1011(m68ki_cpu_core *m68k, UINT32 sr, UINT32 vector, UINT32 pc);

INLINE void m68ki_exception_trap(m68ki_cpu_core *m68k, UINT32 vector);
INLINE void m68ki_exception_trapN(m68ki_cpu_core *m68k, UINT32 vector);
INLINE void m68ki_exception_trace(m68ki_cpu_core *m68k);
INLINE void m68ki_exception_privilege_violation(m68ki_cpu_core *m68k);
INLINE void m68ki_exception_1010(m68ki_cpu_core *m68k);
INLINE void m68ki_exception_1111(m68ki_cpu_core *m68k);
INLINE void m68ki_exception_illegal(m68ki_cpu_core *m68k);
INLINE void m68ki_exception_format_error(m68ki_cpu_core *m68k);
INLINE void m68ki_exception_address_error(m68ki_cpu_core *m68k);
INLINE void m68ki_exception_interrupt(m68ki_cpu_core *m68k, UINT32 int_level);
INLINE void m68ki_check_interrupts(m68ki_cpu_core *m68k);            /* ASG: check for interrupts */

/* quick disassembly (used for logging) */
char* m68ki_disassemble_quick(unsigned int pc, unsigned int cpu_type);


/* ======================================================================== */
/* =========================== UTILITY FUNCTIONS ========================== */
/* ======================================================================== */


/* ---------------------------- Read Immediate ---------------------------- */

/* Handles all immediate reads, does address error check, function code setting,
 * and prefetching if they are enabled in m68kconf.h
 */
INLINE UINT32 m68ki_read_imm_16(m68ki_cpu_core *m68k)
{
	UINT32 result;

	m68ki_set_fc(FLAG_S | FUNCTION_CODE_USER_PROGRAM); /* auto-disable (see m68kcpu.h) */
	m68ki_check_address_error(m68k, REG_PC, MODE_READ, FLAG_S | FUNCTION_CODE_USER_PROGRAM); /* auto-disable (see m68kcpu.h) */

	if(REG_PC != m68k->pref_addr)
	{
		m68k->pref_addr = REG_PC;
		m68k->pref_data = m68k_read_immediate_16(m68k, ADDRESS_68K(m68k->pref_addr));
	}
	result = MASK_OUT_ABOVE_16(m68k->pref_data);
	REG_PC += 2;
	m68k->pref_addr = REG_PC;
	m68k->pref_data = m68k_read_immediate_16(m68k, ADDRESS_68K(m68k->pref_addr));
	return result;
}

INLINE UINT32 m68ki_read_imm_32(m68ki_cpu_core *m68k)
{
	UINT32 temp_val;

	m68ki_set_fc(FLAG_S | FUNCTION_CODE_USER_PROGRAM); /* auto-disable (see m68kcpu.h) */
	m68ki_check_address_error(m68k, REG_PC, MODE_READ, FLAG_S | FUNCTION_CODE_USER_PROGRAM); /* auto-disable (see m68kcpu.h) */

	if(REG_PC != m68k->pref_addr)
	{
		m68k->pref_addr = REG_PC;
		m68k->pref_data = m68k_read_immediate_16(m68k, ADDRESS_68K(m68k->pref_addr));
	}
	temp_val = MASK_OUT_ABOVE_16(m68k->pref_data);
	REG_PC += 2;
	m68k->pref_addr = REG_PC;
	m68k->pref_data = m68k_read_immediate_16(m68k, ADDRESS_68K(m68k->pref_addr));

	temp_val = MASK_OUT_ABOVE_32((temp_val << 16) | MASK_OUT_ABOVE_16(m68k->pref_data));
	REG_PC += 2;
	m68k->pref_addr = REG_PC;
	m68k->pref_data = m68k_read_immediate_16(m68k, ADDRESS_68K(m68k->pref_addr));

	return temp_val;
}



/* ------------------------- Top level read/write ------------------------- */

/* Handles all memory accesses (except for immediate reads if they are
 * configured to use separate functions in m68kconf.h).
 * All memory accesses must go through these top level functions.
 * These functions will also check for address error and set the function
 * code if they are enabled in m68kconf.h.
 */
INLINE UINT32 m68ki_read_8_fc(m68ki_cpu_core *m68k, UINT32 address, UINT32 fc)
{
	m68ki_set_fc(fc); /* auto-disable (see m68kcpu.h) */
	return m68k_read_memory_8(m68k, ADDRESS_68K(address));
}
INLINE UINT32 m68ki_read_16_fc(m68ki_cpu_core *m68k, UINT32 address, UINT32 fc)
{
	m68ki_set_fc(fc); /* auto-disable (see m68kcpu.h) */
	m68ki_check_address_error_010_less(m68k, address, MODE_READ, fc); /* auto-disable (see m68kcpu.h) */
	return m68k_read_memory_16(m68k, ADDRESS_68K(address));
}
INLINE UINT32 m68ki_read_32_fc(m68ki_cpu_core *m68k, UINT32 address, UINT32 fc)
{
	m68ki_set_fc(fc); /* auto-disable (see m68kcpu.h) */
	m68ki_check_address_error_010_less(m68k, address, MODE_READ, fc); /* auto-disable (see m68kcpu.h) */
	return m68k_read_memory_32(m68k, ADDRESS_68K(address));
}

INLINE void m68ki_write_8_fc(m68ki_cpu_core *m68k, UINT32 address, UINT32 fc, UINT32 value)
{
	m68ki_set_fc(fc); /* auto-disable (see m68kcpu.h) */
	m68k_write_memory_8(m68k, ADDRESS_68K(address), value);
}
INLINE void m68ki_write_16_fc(m68ki_cpu_core *m68k, UINT32 address, UINT32 fc, UINT32 value)
{
	m68ki_set_fc(fc); /* auto-disable (see m68kcpu.h) */
	m68ki_check_address_error_010_less(m68k, address, MODE_WRITE, fc); /* auto-disable (see m68kcpu.h) */
	m68k_write_memory_16(m68k, ADDRESS_68K(address), value);
}
INLINE void m68ki_write_32_fc(m68ki_cpu_core *m68k, UINT32 address, UINT32 fc, UINT32 value)
{
	m68ki_set_fc(fc); /* auto-disable (see m68kcpu.h) */
	m68ki_check_address_error_010_less(m68k, address, MODE_WRITE, fc); /* auto-disable (see m68kcpu.h) */
	m68k_write_memory_32(m68k, ADDRESS_68K(address), value);
}

INLINE void m68ki_write_32_pd_fc(m68ki_cpu_core *m68k, UINT32 address, UINT32 fc, UINT32 value)
{
	m68ki_set_fc(fc); /* auto-disable (see m68kcpu.h) */
	m68ki_check_address_error_010_less(m68k, address, MODE_WRITE, fc); /* auto-disable (see m68kcpu.h) */
	m68k_write_memory_32_pd(m68k, ADDRESS_68K(address), value);
}


/* --------------------- Effective Address Calculation -------------------- */

/* The program counter relative addressing modes cause operands to be
 * retrieved from program space, not data space.
 */
INLINE UINT32 m68ki_get_ea_pcdi(m68ki_cpu_core *m68k)
{
	UINT32 old_pc = REG_PC;
	m68ki_use_program_space(m68k); /* auto-disable */
	return old_pc + MAKE_INT_16(m68ki_read_imm_16(m68k));
}


INLINE UINT32 m68ki_get_ea_pcix(m68ki_cpu_core *m68k)
{
	m68ki_use_program_space(m68k); /* auto-disable */
	return m68ki_get_ea_ix(m68k, REG_PC);
}

/* Indexed addressing modes are encoded as follows:
 *
 * Base instruction format:
 * F E D C B A 9 8 7 6 | 5 4 3 | 2 1 0
 * x x x x x x x x x x | 1 1 0 | BASE REGISTER      (An)
 *
 * Base instruction format for destination EA in move instructions:
 * F E D C | B A 9    | 8 7 6 | 5 4 3 2 1 0
 * x x x x | BASE REG | 1 1 0 | X X X X X X       (An)
 *
 * Brief extension format:
 *  F  |  E D C   |  B  |  A 9  | 8 | 7 6 5 4 3 2 1 0
 * D/A | REGISTER | W/L | SCALE | 0 |  DISPLACEMENT
 *
 * Full extension format:
 *  F     E D C      B     A 9    8   7    6    5 4       3   2 1 0
 * D/A | REGISTER | W/L | SCALE | 1 | BS | IS | BD SIZE | 0 | I/IS
 * BASE DISPLACEMENT (0, 16, 32 bit)                (bd)
 * OUTER DISPLACEMENT (0, 16, 32 bit)               (od)
 *
 * D/A:     0 = Dn, 1 = An                          (Xn)
 * W/L:     0 = W (sign extend), 1 = L              (.SIZE)
 * SCALE:   00=1, 01=2, 10=4, 11=8                  (*SCALE)
 * BS:      0=add base reg, 1=suppress base reg     (An suppressed)
 * IS:      0=add index, 1=suppress index           (Xn suppressed)
 * BD SIZE: 00=reserved, 01=NULL, 10=Word, 11=Long  (size of bd)
 *
 * IS I/IS Operation
 * 0  000  No Memory Indirect
 * 0  001  indir prex with null outer
 * 0  010  indir prex with word outer
 * 0  011  indir prex with long outer
 * 0  100  reserved
 * 0  101  indir postx with null outer
 * 0  110  indir postx with word outer
 * 0  111  indir postx with long outer
 * 1  000  no memory indirect
 * 1  001  mem indir with null outer
 * 1  010  mem indir with word outer
 * 1  011  mem indir with long outer
 * 1  100-111  reserved
 */
INLINE UINT32 m68ki_get_ea_ix(m68ki_cpu_core *m68k, UINT32 An)
{
	/* An = base register */
	UINT32 extension = m68ki_read_imm_16(m68k);
	UINT32 Xn = 0;                        /* Index register */
	UINT32 bd = 0;                        /* Base Displacement */
	UINT32 od = 0;                        /* Outer Displacement */

	if(CPU_TYPE_IS_010_LESS(m68k->cpu_type))
	{
		/* Calculate index */
		Xn = REG_DA[extension>>12];     /* Xn */
		if(!BIT_B(extension))           /* W/L */
			Xn = MAKE_INT_16(Xn);

		/* Add base register and displacement and return */
		return An + Xn + MAKE_INT_8(extension);
	}

	/* Brief extension format */
	if(!BIT_8(extension))
	{
		/* Calculate index */
		Xn = REG_DA[extension>>12];     /* Xn */
		if(!BIT_B(extension))           /* W/L */
			Xn = MAKE_INT_16(Xn);
		/* Add scale if proper CPU type */
		if(CPU_TYPE_IS_EC020_PLUS(m68k->cpu_type))
			Xn <<= (extension>>9) & 3;  /* SCALE */

		/* Add base register and displacement and return */
		return An + Xn + MAKE_INT_8(extension);
	}

	/* Full extension format */

	USE_CYCLES(m68k, m68ki_ea_idx_cycle_table[extension&0x3f]);

	/* Check if base register is present */
	if(BIT_7(extension))                /* BS */
		An = 0;                         /* An */

	/* Check if index is present */
	if(!BIT_6(extension))               /* IS */
	{
		Xn = REG_DA[extension>>12];     /* Xn */
		if(!BIT_B(extension))           /* W/L */
			Xn = MAKE_INT_16(Xn);
		Xn <<= (extension>>9) & 3;      /* SCALE */
	}

	/* Check if base displacement is present */
	if(BIT_5(extension))                /* BD SIZE */
		bd = BIT_4(extension) ? m68ki_read_imm_32(m68k) : MAKE_INT_16(m68ki_read_imm_16(m68k));

	/* If no indirect action, we are done */
	if(!(extension&7))                  /* No Memory Indirect */
		return An + bd + Xn;

	/* Check if outer displacement is present */
	if(BIT_1(extension))                /* I/IS:  od */
		od = BIT_0(extension) ? m68ki_read_imm_32(m68k) : MAKE_INT_16(m68ki_read_imm_16(m68k));

	/* Postindex */
	if(BIT_2(extension))                /* I/IS:  0 = preindex, 1 = postindex */
		return m68ki_read_32(m68k, An + bd) + Xn + od;

	/* Preindex */
	return m68ki_read_32(m68k, An + bd + Xn) + od;
}


/* Fetch operands */
INLINE UINT32 OPER_AY_AI_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_AY_AI_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AY_AI_16(m68ki_cpu_core *m68k) {UINT32 ea = EA_AY_AI_16(m68k); return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AY_AI_32(m68ki_cpu_core *m68k) {UINT32 ea = EA_AY_AI_32(m68k); return m68ki_read_32(m68k, ea);}
INLINE UINT32 OPER_AY_PI_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_AY_PI_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AY_PI_16(m68ki_cpu_core *m68k) {UINT32 ea = EA_AY_PI_16(m68k); return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AY_PI_32(m68ki_cpu_core *m68k) {UINT32 ea = EA_AY_PI_32(m68k); return m68ki_read_32(m68k, ea);}
INLINE UINT32 OPER_AY_PD_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_AY_PD_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AY_PD_16(m68ki_cpu_core *m68k) {UINT32 ea = EA_AY_PD_16(m68k); return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AY_PD_32(m68ki_cpu_core *m68k) {UINT32 ea = EA_AY_PD_32(m68k); return m68ki_read_32(m68k, ea);}
INLINE UINT32 OPER_AY_DI_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_AY_DI_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AY_DI_16(m68ki_cpu_core *m68k) {UINT32 ea = EA_AY_DI_16(m68k); return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AY_DI_32(m68ki_cpu_core *m68k) {UINT32 ea = EA_AY_DI_32(m68k); return m68ki_read_32(m68k, ea);}
INLINE UINT32 OPER_AY_IX_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_AY_IX_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AY_IX_16(m68ki_cpu_core *m68k) {UINT32 ea = EA_AY_IX_16(m68k); return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AY_IX_32(m68ki_cpu_core *m68k) {UINT32 ea = EA_AY_IX_32(m68k); return m68ki_read_32(m68k, ea);}

INLINE UINT32 OPER_AX_AI_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_AX_AI_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AX_AI_16(m68ki_cpu_core *m68k) {UINT32 ea = EA_AX_AI_16(m68k); return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AX_AI_32(m68ki_cpu_core *m68k) {UINT32 ea = EA_AX_AI_32(m68k); return m68ki_read_32(m68k, ea);}
INLINE UINT32 OPER_AX_PI_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_AX_PI_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AX_PI_16(m68ki_cpu_core *m68k) {UINT32 ea = EA_AX_PI_16(m68k); return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AX_PI_32(m68ki_cpu_core *m68k) {UINT32 ea = EA_AX_PI_32(m68k); return m68ki_read_32(m68k, ea);}
INLINE UINT32 OPER_AX_PD_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_AX_PD_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AX_PD_16(m68ki_cpu_core *m68k) {UINT32 ea = EA_AX_PD_16(m68k); return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AX_PD_32(m68ki_cpu_core *m68k) {UINT32 ea = EA_AX_PD_32(m68k); return m68ki_read_32(m68k, ea);}
INLINE UINT32 OPER_AX_DI_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_AX_DI_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AX_DI_16(m68ki_cpu_core *m68k) {UINT32 ea = EA_AX_DI_16(m68k); return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AX_DI_32(m68ki_cpu_core *m68k) {UINT32 ea = EA_AX_DI_32(m68k); return m68ki_read_32(m68k, ea);}
INLINE UINT32 OPER_AX_IX_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_AX_IX_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AX_IX_16(m68ki_cpu_core *m68k) {UINT32 ea = EA_AX_IX_16(m68k); return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AX_IX_32(m68ki_cpu_core *m68k) {UINT32 ea = EA_AX_IX_32(m68k); return m68ki_read_32(m68k, ea);}

INLINE UINT32 OPER_A7_PI_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_A7_PI_8(m68k);  return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_A7_PD_8(m68ki_cpu_core *m68k)  {UINT32 ea = EA_A7_PD_8(m68k);  return m68ki_read_8(m68k, ea); }

INLINE UINT32 OPER_AW_8(m68ki_cpu_core *m68k)     {UINT32 ea = EA_AW_8(m68k);     return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AW_16(m68ki_cpu_core *m68k)    {UINT32 ea = EA_AW_16(m68k);    return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AW_32(m68ki_cpu_core *m68k)    {UINT32 ea = EA_AW_32(m68k);    return m68ki_read_32(m68k, ea);}
INLINE UINT32 OPER_AL_8(m68ki_cpu_core *m68k)     {UINT32 ea = EA_AL_8(m68k);     return m68ki_read_8(m68k, ea); }
INLINE UINT32 OPER_AL_16(m68ki_cpu_core *m68k)    {UINT32 ea = EA_AL_16(m68k);    return m68ki_read_16(m68k, ea);}
INLINE UINT32 OPER_AL_32(m68ki_cpu_core *m68k)    {UINT32 ea = EA_AL_32(m68k);    return m68ki_read_32(m68k, ea);}
INLINE UINT32 OPER_PCDI_8(m68ki_cpu_core *m68k)   {UINT32 ea = EA_PCDI_8(m68k);   return m68ki_read_pcrel_8(m68k, ea); }
INLINE UINT32 OPER_PCDI_16(m68ki_cpu_core *m68k)  {UINT32 ea = EA_PCDI_16(m68k);  return m68ki_read_pcrel_16(m68k, ea);}
INLINE UINT32 OPER_PCDI_32(m68ki_cpu_core *m68k)  {UINT32 ea = EA_PCDI_32(m68k);  return m68ki_read_pcrel_32(m68k, ea);}
INLINE UINT32 OPER_PCIX_8(m68ki_cpu_core *m68k)   {UINT32 ea = EA_PCIX_8(m68k);   return m68ki_read_pcrel_8(m68k, ea); }
INLINE UINT32 OPER_PCIX_16(m68ki_cpu_core *m68k)  {UINT32 ea = EA_PCIX_16(m68k);  return m68ki_read_pcrel_16(m68k, ea);}
INLINE UINT32 OPER_PCIX_32(m68ki_cpu_core *m68k)  {UINT32 ea = EA_PCIX_32(m68k);  return m68ki_read_pcrel_32(m68k, ea);}



/* ---------------------------- Stack Functions --------------------------- */

/* Push/pull data from the stack */
INLINE void m68ki_push_16(m68ki_cpu_core *m68k, UINT32 value)
{
	REG_SP = MASK_OUT_ABOVE_32(REG_SP - 2);
	m68ki_write_16(m68k, REG_SP, value);
}

INLINE void m68ki_push_32(m68ki_cpu_core *m68k, UINT32 value)
{
	REG_SP = MASK_OUT_ABOVE_32(REG_SP - 4);
	m68ki_write_32(m68k, REG_SP, value);
}

INLINE UINT32 m68ki_pull_16(m68ki_cpu_core *m68k)
{
	REG_SP = MASK_OUT_ABOVE_32(REG_SP + 2);
	return m68ki_read_16(m68k, REG_SP-2);
}

INLINE UINT32 m68ki_pull_32(m68ki_cpu_core *m68k)
{
	REG_SP = MASK_OUT_ABOVE_32(REG_SP + 4);
	return m68ki_read_32(m68k, REG_SP-4);
}


/* Increment/decrement the stack as if doing a push/pull but
 * don't do any memory access.
 */
INLINE void m68ki_fake_push_16(m68ki_cpu_core *m68k)
{
	REG_SP = MASK_OUT_ABOVE_32(REG_SP - 2);
}

INLINE void m68ki_fake_push_32(m68ki_cpu_core *m68k)
{
	REG_SP = MASK_OUT_ABOVE_32(REG_SP - 4);
}

INLINE void m68ki_fake_pull_16(m68ki_cpu_core *m68k)
{
	REG_SP = MASK_OUT_ABOVE_32(REG_SP + 2);
}

INLINE void m68ki_fake_pull_32(m68ki_cpu_core *m68k)
{
	REG_SP = MASK_OUT_ABOVE_32(REG_SP + 4);
}


/* ----------------------------- Program Flow ----------------------------- */

/* Jump to a new program location or vector.
 * These functions will also call the pc_changed callback if it was enabled
 * in m68kconf.h.
 */
INLINE void m68ki_jump(m68ki_cpu_core *m68k, UINT32 new_pc)
{
	REG_PC = new_pc;
	change_pc(ADDRESS_68K(REG_PC));
}

INLINE void m68ki_jump_vector(m68ki_cpu_core *m68k, UINT32 vector)
{
	REG_PC = (vector<<2) + REG_VBR;
	REG_PC = m68ki_read_data_32(m68k, REG_PC);
	change_pc(ADDRESS_68K(REG_PC));
}


/* Branch to a new memory location.
 * The 32-bit branch will call pc_changed if it was enabled in m68kconf.h.
 * So far I've found no problems with not calling pc_changed for 8 or 16
 * bit branches.
 */
INLINE void m68ki_branch_8(m68ki_cpu_core *m68k, UINT32 offset)
{
	REG_PC += MAKE_INT_8(offset);
}

INLINE void m68ki_branch_16(m68ki_cpu_core *m68k, UINT32 offset)
{
	REG_PC += MAKE_INT_16(offset);
}

INLINE void m68ki_branch_32(m68ki_cpu_core *m68k, UINT32 offset)
{
	REG_PC += offset;
	change_pc(ADDRESS_68K(REG_PC));
}



/* ---------------------------- Status Register --------------------------- */

/* Set the S flag and change the active stack pointer.
 * Note that value MUST be 4 or 0.
 */
INLINE void m68ki_set_s_flag(m68ki_cpu_core *m68k, UINT32 value)
{
	/* Backup the old stack pointer */
	REG_SP_BASE[FLAG_S | ((FLAG_S>>1) & FLAG_M)] = REG_SP;
	/* Set the S flag */
	FLAG_S = value;
	/* Set the new stack pointer */
	REG_SP = REG_SP_BASE[FLAG_S | ((FLAG_S>>1) & FLAG_M)];
}

/* Set the S and M flags and change the active stack pointer.
 * Note that value MUST be 0, 2, 4, or 6 (bit2 = S, bit1 = M).
 */
INLINE void m68ki_set_sm_flag(m68ki_cpu_core *m68k, UINT32 value)
{
	/* Backup the old stack pointer */
	REG_SP_BASE[FLAG_S | ((FLAG_S>>1) & FLAG_M)] = REG_SP;
	/* Set the S and M flags */
	FLAG_S = value & SFLAG_SET;
	FLAG_M = value & MFLAG_SET;
	/* Set the new stack pointer */
	REG_SP = REG_SP_BASE[FLAG_S | ((FLAG_S>>1) & FLAG_M)];
}

/* Set the S and M flags.  Don't touch the stack pointer. */
INLINE void m68ki_set_sm_flag_nosp(m68ki_cpu_core *m68k, UINT32 value)
{
	/* Set the S and M flags */
	FLAG_S = value & SFLAG_SET;
	FLAG_M = value & MFLAG_SET;
}


/* Set the condition code register */
INLINE void m68ki_set_ccr(m68ki_cpu_core *m68k, UINT32 value)
{
	FLAG_X = BIT_4(value)  << 4;
	FLAG_N = BIT_3(value)  << 4;
	FLAG_Z = !BIT_2(value);
	FLAG_V = BIT_1(value)  << 6;
	FLAG_C = BIT_0(value)  << 8;
}

/* Set the status register but don't check for interrupts */
INLINE void m68ki_set_sr_noint(m68ki_cpu_core *m68k, UINT32 value)
{
	/* Mask out the "unimplemented" bits */
	value &= m68k->sr_mask;

	/* Now set the status register */
	FLAG_T1 = BIT_F(value);
	FLAG_T0 = BIT_E(value);
	FLAG_INT_MASK = value & 0x0700;
	m68ki_set_ccr(m68k, value);
	m68ki_set_sm_flag(m68k, (value >> 11) & 6);
}

/* Set the status register but don't check for interrupts nor
 * change the stack pointer
 */
INLINE void m68ki_set_sr_noint_nosp(m68ki_cpu_core *m68k, UINT32 value)
{
	/* Mask out the "unimplemented" bits */
	value &= m68k->sr_mask;

	/* Now set the status register */
	FLAG_T1 = BIT_F(value);
	FLAG_T0 = BIT_E(value);
	FLAG_INT_MASK = value & 0x0700;
	m68ki_set_ccr(m68k, value);
	m68ki_set_sm_flag_nosp(m68k, (value >> 11) & 6);
}

/* Set the status register and check for interrupts */
INLINE void m68ki_set_sr(m68ki_cpu_core *m68k, UINT32 value)
{
	m68ki_set_sr_noint(m68k, value);
	m68ki_check_interrupts(m68k);
}


/* ------------------------- Exception Processing ------------------------- */

/* Initiate exception processing */
INLINE UINT32 m68ki_init_exception(m68ki_cpu_core *m68k)
{
	/* Save the old status register */
	UINT32 sr = m68ki_get_sr(m68k);

	/* Turn off trace flag, clear pending traces */
	FLAG_T1 = FLAG_T0 = 0;
	m68ki_clear_trace();
	/* Enter supervisor mode */
	m68ki_set_s_flag(m68k, SFLAG_SET);

	return sr;
}

/* 3 word stack frame (68000 only) */
INLINE void m68ki_stack_frame_3word(m68ki_cpu_core *m68k, UINT32 pc, UINT32 sr)
{
	m68ki_push_32(m68k, pc);
	m68ki_push_16(m68k, sr);
}

/* Format 0 stack frame.
 * This is the standard stack frame for 68010+.
 */
INLINE void m68ki_stack_frame_0000(m68ki_cpu_core *m68k, UINT32 pc, UINT32 sr, UINT32 vector)
{
	/* Stack a 3-word frame if we are 68000 */
	if(m68k->cpu_type == CPU_TYPE_000 || m68k->cpu_type == CPU_TYPE_008)
	{
		m68ki_stack_frame_3word(m68k, pc, sr);
		return;
	}
	m68ki_push_16(m68k, vector<<2);
	m68ki_push_32(m68k, pc);
	m68ki_push_16(m68k, sr);
}

/* Format 1 stack frame (68020).
 * For 68020, this is the 4 word throwaway frame.
 */
INLINE void m68ki_stack_frame_0001(m68ki_cpu_core *m68k, UINT32 pc, UINT32 sr, UINT32 vector)
{
	m68ki_push_16(m68k, 0x1000 | (vector<<2));
	m68ki_push_32(m68k, pc);
	m68ki_push_16(m68k, sr);
}

/* Format 2 stack frame.
 * This is used only by 68020 for trap exceptions.
 */
INLINE void m68ki_stack_frame_0010(m68ki_cpu_core *m68k, UINT32 sr, UINT32 vector)
{
	m68ki_push_32(m68k, REG_PPC);
	m68ki_push_16(m68k, 0x2000 | (vector<<2));
	m68ki_push_32(m68k, REG_PC);
	m68ki_push_16(m68k, sr);
}


/* Bus error stack frame (68000 only).
 */
INLINE void m68ki_stack_frame_buserr(m68ki_cpu_core *m68k, UINT32 sr)
{
	m68ki_push_32(m68k, REG_PC);
	m68ki_push_16(m68k, sr);
	m68ki_push_16(m68k, REG_IR);
	m68ki_push_32(m68k, m68k->aerr_address);	/* access address */
	/* 0 0 0 0 0 0 0 0 0 0 0 R/W I/N FC
     * R/W  0 = write, 1 = read
     * I/N  0 = instruction, 1 = not
     * FC   3-bit function code
     */
	m68ki_push_16(m68k, m68k->aerr_write_mode | m68k->instr_mode | m68k->aerr_fc);
}

/* Format 8 stack frame (68010).
 * 68010 only.  This is the 29 word bus/address error frame.
 */
void m68ki_stack_frame_1000(m68ki_cpu_core *m68k, UINT32 pc, UINT32 sr, UINT32 vector)
{
	/* VERSION
     * NUMBER
     * INTERNAL INFORMATION, 16 WORDS
     */
	m68ki_fake_push_32(m68k);
	m68ki_fake_push_32(m68k);
	m68ki_fake_push_32(m68k);
	m68ki_fake_push_32(m68k);
	m68ki_fake_push_32(m68k);
	m68ki_fake_push_32(m68k);
	m68ki_fake_push_32(m68k);
	m68ki_fake_push_32(m68k);

	/* INSTRUCTION INPUT BUFFER */
	m68ki_push_16(m68k, 0);

	/* UNUSED, RESERVED (not written) */
	m68ki_fake_push_16(m68k);

	/* DATA INPUT BUFFER */
	m68ki_push_16(m68k, 0);

	/* UNUSED, RESERVED (not written) */
	m68ki_fake_push_16(m68k);

	/* DATA OUTPUT BUFFER */
	m68ki_push_16(m68k, 0);

	/* UNUSED, RESERVED (not written) */
	m68ki_fake_push_16(m68k);

	/* FAULT ADDRESS */
	m68ki_push_32(m68k, 0);

	/* SPECIAL STATUS WORD */
	m68ki_push_16(m68k, 0);

	/* 1000, VECTOR OFFSET */
	m68ki_push_16(m68k, 0x8000 | (vector<<2));

	/* PROGRAM COUNTER */
	m68ki_push_32(m68k, pc);

	/* STATUS REGISTER */
	m68ki_push_16(m68k, sr);
}

/* Format A stack frame (short bus fault).
 * This is used only by 68020 for bus fault and address error
 * if the error happens at an instruction boundary.
 * PC stacked is address of next instruction.
 */
void m68ki_stack_frame_1010(m68ki_cpu_core *m68k, UINT32 sr, UINT32 vector, UINT32 pc)
{
	/* INTERNAL REGISTER */
	m68ki_push_16(m68k, 0);

	/* INTERNAL REGISTER */
	m68ki_push_16(m68k, 0);

	/* DATA OUTPUT BUFFER (2 words) */
	m68ki_push_32(m68k, 0);

	/* INTERNAL REGISTER */
	m68ki_push_16(m68k, 0);

	/* INTERNAL REGISTER */
	m68ki_push_16(m68k, 0);

	/* DATA CYCLE FAULT ADDRESS (2 words) */
	m68ki_push_32(m68k, 0);

	/* INSTRUCTION PIPE STAGE B */
	m68ki_push_16(m68k, 0);

	/* INSTRUCTION PIPE STAGE C */
	m68ki_push_16(m68k, 0);

	/* SPECIAL STATUS REGISTER */
	m68ki_push_16(m68k, 0);

	/* INTERNAL REGISTER */
	m68ki_push_16(m68k, 0);

	/* 1010, VECTOR OFFSET */
	m68ki_push_16(m68k, 0xa000 | (vector<<2));

	/* PROGRAM COUNTER */
	m68ki_push_32(m68k, pc);

	/* STATUS REGISTER */
	m68ki_push_16(m68k, sr);
}

/* Format B stack frame (long bus fault).
 * This is used only by 68020 for bus fault and address error
 * if the error happens during instruction execution.
 * PC stacked is address of instruction in progress.
 */
void m68ki_stack_frame_1011(m68ki_cpu_core *m68k, UINT32 sr, UINT32 vector, UINT32 pc)
{
	/* INTERNAL REGISTERS (18 words) */
	m68ki_push_32(m68k, 0);
	m68ki_push_32(m68k, 0);
	m68ki_push_32(m68k, 0);
	m68ki_push_32(m68k, 0);
	m68ki_push_32(m68k, 0);
	m68ki_push_32(m68k, 0);
	m68ki_push_32(m68k, 0);
	m68ki_push_32(m68k, 0);
	m68ki_push_32(m68k, 0);

	/* VERSION# (4 bits), INTERNAL INFORMATION */
	m68ki_push_16(m68k, 0);

	/* INTERNAL REGISTERS (3 words) */
	m68ki_push_32(m68k, 0);
	m68ki_push_16(m68k, 0);

	/* DATA INTPUT BUFFER (2 words) */
	m68ki_push_32(m68k, 0);

	/* INTERNAL REGISTERS (2 words) */
	m68ki_push_32(m68k, 0);

	/* STAGE B ADDRESS (2 words) */
	m68ki_push_32(m68k, 0);

	/* INTERNAL REGISTER (4 words) */
	m68ki_push_32(m68k, 0);
	m68ki_push_32(m68k, 0);

	/* DATA OUTPUT BUFFER (2 words) */
	m68ki_push_32(m68k, 0);

	/* INTERNAL REGISTER */
	m68ki_push_16(m68k, 0);

	/* INTERNAL REGISTER */
	m68ki_push_16(m68k, 0);

	/* DATA CYCLE FAULT ADDRESS (2 words) */
	m68ki_push_32(m68k, 0);

	/* INSTRUCTION PIPE STAGE B */
	m68ki_push_16(m68k, 0);

	/* INSTRUCTION PIPE STAGE C */
	m68ki_push_16(m68k, 0);

	/* SPECIAL STATUS REGISTER */
	m68ki_push_16(m68k, 0);

	/* INTERNAL REGISTER */
	m68ki_push_16(m68k, 0);

	/* 1011, VECTOR OFFSET */
	m68ki_push_16(m68k, 0xb000 | (vector<<2));

	/* PROGRAM COUNTER */
	m68ki_push_32(m68k, pc);

	/* STATUS REGISTER */
	m68ki_push_16(m68k, sr);
}


/* Used for Group 2 exceptions.
 * These stack a type 2 frame on the 020.
 */
INLINE void m68ki_exception_trap(m68ki_cpu_core *m68k, UINT32 vector)
{
	UINT32 sr = m68ki_init_exception(m68k);

	if(CPU_TYPE_IS_010_LESS(m68k->cpu_type))
		m68ki_stack_frame_0000(m68k, REG_PC, sr, vector);
	else
		m68ki_stack_frame_0010(m68k, sr, vector);

	m68ki_jump_vector(m68k, vector);

	/* Use up some clock cycles */
	USE_CYCLES(m68k, m68k->cyc_exception[vector]);
}

/* Trap#n stacks a 0 frame but behaves like group2 otherwise */
INLINE void m68ki_exception_trapN(m68ki_cpu_core *m68k, UINT32 vector)
{
	UINT32 sr = m68ki_init_exception(m68k);
	m68ki_stack_frame_0000(m68k, REG_PC, sr, vector);
	m68ki_jump_vector(m68k, vector);

	/* Use up some clock cycles */
	USE_CYCLES(m68k, m68k->cyc_exception[vector]);
}

/* Exception for trace mode */
INLINE void m68ki_exception_trace(m68ki_cpu_core *m68k)
{
	UINT32 sr = m68ki_init_exception(m68k);

	if(CPU_TYPE_IS_010_LESS(m68k->cpu_type))
	{
		if(CPU_TYPE_IS_000(m68k->cpu_type))
		{
			m68k->instr_mode = INSTRUCTION_NO;
		}
		m68ki_stack_frame_0000(m68k, REG_PC, sr, EXCEPTION_TRACE);
	}
	else
		m68ki_stack_frame_0010(m68k, sr, EXCEPTION_TRACE);

	m68ki_jump_vector(m68k, EXCEPTION_TRACE);

	/* Trace nullifies a STOP instruction */
	m68k->stopped &= ~STOP_LEVEL_STOP;

	/* Use up some clock cycles */
	USE_CYCLES(m68k, m68k->cyc_exception[EXCEPTION_TRACE]);
}

/* Exception for privilege violation */
INLINE void m68ki_exception_privilege_violation(m68ki_cpu_core *m68k)
{
	UINT32 sr = m68ki_init_exception(m68k);

	if(CPU_TYPE_IS_000(m68k->cpu_type))
	{
		m68k->instr_mode = INSTRUCTION_NO;
	}

	m68ki_stack_frame_0000(m68k, REG_PPC, sr, EXCEPTION_PRIVILEGE_VIOLATION);
	m68ki_jump_vector(m68k, EXCEPTION_PRIVILEGE_VIOLATION);

	/* Use up some clock cycles and undo the instruction's cycles */
	USE_CYCLES(m68k, m68k->cyc_exception[EXCEPTION_PRIVILEGE_VIOLATION] - m68k->cyc_instruction[REG_IR]);
}

/* Exception for A-Line instructions */
INLINE void m68ki_exception_1010(m68ki_cpu_core *m68k)
{
	UINT32 sr;
#if M68K_LOG_1010_1111 == OPT_ON
	M68K_DO_LOG_EMU((M68K_LOG_FILEHANDLE "%s at %08x: called 1010 instruction %04x (%s)\n",
					 m68ki_cpu_names[m68k->cpu_type], ADDRESS_68K(REG_PPC), REG_IR,
					 m68ki_disassemble_quick(ADDRESS_68K(REG_PPC))));
#endif

	sr = m68ki_init_exception(m68k);
	m68ki_stack_frame_0000(m68k, REG_PPC, sr, EXCEPTION_1010);
	m68ki_jump_vector(m68k, EXCEPTION_1010);

	/* Use up some clock cycles and undo the instruction's cycles */
	USE_CYCLES(m68k, m68k->cyc_exception[EXCEPTION_1010] - m68k->cyc_instruction[REG_IR]);
}

/* Exception for F-Line instructions */
INLINE void m68ki_exception_1111(m68ki_cpu_core *m68k)
{
	UINT32 sr;

#if M68K_LOG_1010_1111 == OPT_ON
	M68K_DO_LOG_EMU((M68K_LOG_FILEHANDLE "%s at %08x: called 1111 instruction %04x (%s)\n",
					 m68ki_cpu_names[m68k->cpu_type], ADDRESS_68K(REG_PPC), REG_IR,
					 m68ki_disassemble_quick(ADDRESS_68K(REG_PPC))));
#endif

	sr = m68ki_init_exception(m68k);
	m68ki_stack_frame_0000(m68k, REG_PPC, sr, EXCEPTION_1111);
	m68ki_jump_vector(m68k, EXCEPTION_1111);

	/* Use up some clock cycles and undo the instruction's cycles */
	USE_CYCLES(m68k, m68k->cyc_exception[EXCEPTION_1111] - m68k->cyc_instruction[REG_IR]);
}

/* Exception for illegal instructions */
INLINE void m68ki_exception_illegal(m68ki_cpu_core *m68k)
{
	UINT32 sr;

	M68K_DO_LOG((M68K_LOG_FILEHANDLE "%s at %08x: illegal instruction %04x (%s)\n",
				 m68ki_cpu_names[m68k->cpu_type], ADDRESS_68K(REG_PPC), REG_IR,
				 m68ki_disassemble_quick(ADDRESS_68K(REG_PPC))));

	sr = m68ki_init_exception(m68k);

	if(CPU_TYPE_IS_000(m68k->cpu_type))
	{
		m68k->instr_mode = INSTRUCTION_NO;
	}

	m68ki_stack_frame_0000(m68k, REG_PPC, sr, EXCEPTION_ILLEGAL_INSTRUCTION);
	m68ki_jump_vector(m68k, EXCEPTION_ILLEGAL_INSTRUCTION);

	/* Use up some clock cycles and undo the instruction's cycles */
	USE_CYCLES(m68k, m68k->cyc_exception[EXCEPTION_ILLEGAL_INSTRUCTION] - m68k->cyc_instruction[REG_IR]);
}

/* Exception for format errror in RTE */
INLINE void m68ki_exception_format_error(m68ki_cpu_core *m68k)
{
	UINT32 sr = m68ki_init_exception(m68k);
	m68ki_stack_frame_0000(m68k, REG_PC, sr, EXCEPTION_FORMAT_ERROR);
	m68ki_jump_vector(m68k, EXCEPTION_FORMAT_ERROR);

	/* Use up some clock cycles and undo the instruction's cycles */
	USE_CYCLES(m68k, m68k->cyc_exception[EXCEPTION_FORMAT_ERROR] - m68k->cyc_instruction[REG_IR]);
}

/* Exception for address error */
INLINE void m68ki_exception_address_error(m68ki_cpu_core *m68k)
{
	UINT32 sr = m68ki_init_exception(m68k);

	/* If we were processing a bus error, address error, or reset,
     * this is a catastrophic failure.
     * Halt the CPU
     */
	if(m68k->run_mode == RUN_MODE_BERR_AERR_RESET)
	{
		m68k_read_memory_8(m68k, 0x00ffff01);
		m68k->stopped = STOP_LEVEL_HALT;
		return;
	}
	m68k->run_mode = RUN_MODE_BERR_AERR_RESET;

	/* Note: This is implemented for 68000 only! */
	m68ki_stack_frame_buserr(m68k, sr);

	m68ki_jump_vector(m68k, EXCEPTION_ADDRESS_ERROR);

	/* Use up some clock cycles and undo the instruction's cycles */
	USE_CYCLES(m68k, m68k->cyc_exception[EXCEPTION_ADDRESS_ERROR] - m68k->cyc_instruction[REG_IR]);
}


/* Service an interrupt request and start exception processing */
void m68ki_exception_interrupt(m68ki_cpu_core *m68k, UINT32 int_level)
{
	UINT32 vector;
	UINT32 sr;
	UINT32 new_pc;

	if(CPU_TYPE_IS_000(m68k->cpu_type))
	{
		m68k->instr_mode = INSTRUCTION_NO;
	}

	/* Turn off the stopped state */
	m68k->stopped &= ~STOP_LEVEL_STOP;

	/* If we are halted, don't do anything */
	if(m68k->stopped)
		return;

	/* Acknowledge the interrupt */
	vector = (*m68k->int_ack_callback)(m68k->int_ack_param, int_level);

	/* Get the interrupt vector */
	if(vector == M68K_INT_ACK_AUTOVECTOR)
		/* Use the autovectors.  This is the most commonly used implementation */
		vector = EXCEPTION_INTERRUPT_AUTOVECTOR+int_level;
	else if(vector == M68K_INT_ACK_SPURIOUS)
		/* Called if no devices respond to the interrupt acknowledge */
		vector = EXCEPTION_SPURIOUS_INTERRUPT;
	else if(vector > 255)
	{
		M68K_DO_LOG_EMU((M68K_LOG_FILEHANDLE "%s at %08x: Interrupt acknowledge returned invalid vector $%x\n",
				 m68ki_cpu_names[m68k->cpu_type], ADDRESS_68K(REG_PC), vector));
		return;
	}

	/* Start exception processing */
	sr = m68ki_init_exception(m68k);

	/* Set the interrupt mask to the level of the one being serviced */
	FLAG_INT_MASK = int_level<<8;

	/* Get the new PC */
	new_pc = m68ki_read_data_32(m68k, (vector<<2) + REG_VBR);

	/* If vector is uninitialized, call the uninitialized interrupt vector */
	if(new_pc == 0)
		new_pc = m68ki_read_data_32(m68k, (EXCEPTION_UNINITIALIZED_INTERRUPT<<2) + REG_VBR);

	/* Generate a stack frame */
	m68ki_stack_frame_0000(m68k, REG_PC, sr, vector);
	if(FLAG_M && CPU_TYPE_IS_EC020_PLUS(m68k->cpu_type))
	{
		/* Create throwaway frame */
		m68ki_set_sm_flag(m68k, FLAG_S);	/* clear M */
		sr |= 0x2000; /* Same as SR in master stack frame except S is forced high */
		m68ki_stack_frame_0001(m68k, REG_PC, sr, vector);
	}

	m68ki_jump(m68k, new_pc);

	/* Defer cycle counting until later */
	USE_CYCLES(m68k, m68k->cyc_exception[vector]);
}


/* ASG: Check for interrupts */
INLINE void m68ki_check_interrupts(m68ki_cpu_core *m68k)
{
	if(m68k->nmi_pending)
	{
		m68k->nmi_pending = FALSE;
		m68ki_exception_interrupt(m68k, 7);
	}
	else if(m68k->int_level > FLAG_INT_MASK)
		m68ki_exception_interrupt(m68k, m68k->int_level>>8);
}



/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */

#endif /* __M68KCPU_H__ */
