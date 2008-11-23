#pragma once

#ifndef __SPC700_H__
#define __SPC700_H__

/* ======================================================================== */
/* ============================= Configuration ============================ */
/* ======================================================================== */

#ifndef INLINE
#define INLINE static
#endif

/* Turn on optimizations for SNES since it doesn't hook up the interrupt lines */
#define SPC700_OPTIMIZE_SNES 1


/* ======================================================================== */
/* ============================== PROTOTYPES ============================== */
/* ======================================================================== */

enum
{
	SPC700_PC=1, SPC700_S, SPC700_P, SPC700_A, SPC700_X, SPC700_Y
};

#define SPC700_INT_NONE			0
#define SPC700_INT_IRQ			1
#define SPC700_INT_NMI			2



/* ======================================================================== */
/* ============================= INTERFACE API ============================ */
/* ======================================================================== */

/* This is the interface, not the implementation.  Please call the  */
/* implementation APIs below.                                       */


CPU_INIT( spc700 );

/* Pulse the RESET pin on the CPU */
CPU_RESET( spc700 );

/* Set the RESET line on the CPU */
void spc700_set_reset_line(int state, void* param);

/* Clean up after the emulation core - Not used in this core - */
CPU_EXIT( spc700 );

/* Get the current CPU context */
unsigned spc700_get_context(void *dst);

/* Set the current CPU context */
CPU_SET_CONTEXT( spc700 );

/* Get the current Program Counter */
unsigned spc700_get_pc(void);

/* Set the current Program Counter */
void spc700_set_pc(unsigned val);

/* Get the current Stack Pointer */
unsigned spc700_get_sp(void);

/* Set the current Stack Pointer */
void spc700_set_sp(unsigned val);

/* Get a register from the core */
unsigned spc700_get_reg(int regnum);

/* Set a register in the core */
void spc700_set_reg(int regnum, unsigned val);

/* Note about NMI:
 *   NMI is a one-shot trigger.  In order to trigger NMI again, you must
 *   clear NMI and then assert it again.
 */
void spc700_set_nmi_line(int state);

/* Assert or clear the IRQ pin */
void spc700_set_irq_line(int line, int state);

/* Set the callback that will be called when an interrupt is serviced */
void spc700_set_irq_callback(cpu_irq_callback callback);

/* Save the current CPU state to disk */
void spc700_state_save(void *file);

/* Load a CPU state from disk */
void spc700_state_load(void *file);

/* Get a formatted string representing a register and its contents */
const char *spc700_info(void *context, int regnum);


/* Pulse the SO (Set Overflow) pin on the CPU */
void spc700_pulse_so(void);

int spc700_execute(int clocks);


/* ======================================================================== */
/* =================== Functions Implemented by the Host ================== */
/* ======================================================================== */

/* Read data from RAM */
unsigned int spc700_read_8(unsigned int address);

/* Read data from the direct page */
unsigned int spc700_read_8_direct(unsigned int address);

/* Read data from ROM */
unsigned int spc700_read_8_immediate(unsigned int address);
unsigned int spc700_read_8_instruction(unsigned int address);

/* Write data to RAM */
void spc700_write_8(unsigned int address, unsigned int value);
void spc700_write_8_direct(unsigned int address, unsigned int value);

void spc700_jumping(unsigned int new_pc);
void spc700_branching(unsigned int new_pc);



/* ======================================================================== */
/* ================================= MAME ================================= */
/* ======================================================================== */

#include "cpuintrf.h"

extern CPU_GET_INFO( spc700 );

#define spc700_read_8(addr) memory_read_byte_8le(spc700i_cpu.program,addr)
#define spc700_write_8(addr,data) memory_write_byte_8le(spc700i_cpu.program,addr,data)

#define spc700_read_8_direct(A)     spc700_read_8(A)
#define spc700_write_8_direct(A, V) spc700_write_8(A, V)
//#define spc700_read_instruction(A)    memory_decrypted_read_byte(spc700i_cpu.program,A)
//#define spc700_read_8_immediate(A)    memory_raw_read_byte(spc700i_cpu.program,A)
#define spc700_read_instruction(A)    memory_read_byte_8le(spc700i_cpu.program,A)
#define spc700_read_8_immediate(A)    memory_read_byte_8le(spc700i_cpu.program,A)
#define spc700_jumping(A)             change_pc(A)
#define spc700_branching(A)	      change_pc(A)



/* ======================================================================== */
/* ============================== END OF FILE ============================= */
/* ======================================================================== */

#endif /* __SPC700_H__ */
