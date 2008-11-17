/*** t11: Portable DEC T-11 emulator ******************************************

    Copyright Aaron Giles

    System dependencies:    long must be at least 32 bits
                            word must be 16 bit unsigned int
                            byte must be 8 bit unsigned int
                            long must be more than 16 bits
                            arrays up to 65536 bytes must be supported
                            machine must be twos complement

*****************************************************************************/

#include "debugger.h"
#include "t11.h"


/*************************************
 *
 *  Internal state representation
 *
 *************************************/

typedef struct
{
	PAIR	ppc;	/* previous program counter */
    PAIR	reg[8];
    PAIR	psw;
    UINT16	op;
    UINT16	initial_pc;
    UINT8	wait_state;
    UINT8	irq_state;
    INT32	interrupt_cycles;
	cpu_irq_callback irq_callback;
	const device_config *device;
} t11_Regs;

static t11_Regs t11;



/*************************************
 *
 *  Global variables
 *
 *************************************/

static int	t11_ICount;



/*************************************
 *
 *  Macro shortcuts
 *
 *************************************/

/* registers of various sizes */
#define REGD(x) t11.reg[x].d
#define REGW(x) t11.reg[x].w.l
#define REGB(x) t11.reg[x].b.l

/* PC, SP, and PSW definitions */
#define SP REGW(6)
#define PC REGW(7)
#define SPD REGD(6)
#define PCD REGD(7)
#define PSW t11.psw.b.l



/*************************************
 *
 *  Low-level memory operations
 *
 *************************************/

INLINE int ROPCODE(void)
{
	int val = program_decrypted_read_word(PC);
	PC += 2;
	return val;
}


INLINE int RBYTE(int addr)
{
	return T11_RDMEM(addr);
}


INLINE void WBYTE(int addr, int data)
{
	T11_WRMEM(addr, data);
}


INLINE int RWORD(int addr)
{
	return T11_RDMEM_WORD(addr & 0xfffe);
}


INLINE void WWORD(int addr, int data)
{
	T11_WRMEM_WORD(addr & 0xfffe, data);
}



/*************************************
 *
 *  Low-level stack operations
 *
 *************************************/

INLINE void PUSH(int val)
{
	SP -= 2;
	WWORD(SPD, val);
}


INLINE int POP(void)
{
	int result = RWORD(SPD);
	SP += 2;
	return result;
}



/*************************************
 *
 *  Flag definitions and operations
 *
 *************************************/

/* flag definitions */
#define CFLAG 1
#define VFLAG 2
#define ZFLAG 4
#define NFLAG 8

/* extracts flags */
#define GET_C (PSW & CFLAG)
#define GET_V (PSW & VFLAG)
#define GET_Z (PSW & ZFLAG)
#define GET_N (PSW & NFLAG)

/* clears flags */
#define CLR_C (PSW &= ~CFLAG)
#define CLR_V (PSW &= ~VFLAG)
#define CLR_Z (PSW &= ~ZFLAG)
#define CLR_N (PSW &= ~NFLAG)

/* sets flags */
#define SET_C (PSW |= CFLAG)
#define SET_V (PSW |= VFLAG)
#define SET_Z (PSW |= ZFLAG)
#define SET_N (PSW |= NFLAG)



/*************************************
 *
 *  Interrupt handling
 *
 *************************************/

struct irq_table_entry
{
	UINT8	priority;
	UINT8	vector;
};

static const struct irq_table_entry irq_table[] =
{
	{ 0<<5, 0x00 },
	{ 4<<5, 0x38 },
	{ 4<<5, 0x34 },
	{ 4<<5, 0x30 },
	{ 5<<5, 0x5c },
	{ 5<<5, 0x58 },
	{ 5<<5, 0x54 },
	{ 5<<5, 0x50 },
	{ 6<<5, 0x4c },
	{ 6<<5, 0x48 },
	{ 6<<5, 0x44 },
	{ 6<<5, 0x40 },
	{ 7<<5, 0x6c },
	{ 7<<5, 0x68 },
	{ 7<<5, 0x64 },
	{ 7<<5, 0x60 }
};

static void t11_check_irqs(void)
{
	const struct irq_table_entry *irq = &irq_table[t11.irq_state & 15];
	int priority = PSW & 0xe0;

	/* compare the priority of the interrupt to the PSW */
	if (irq->priority > priority)
	{
		int vector = irq->vector;
		int new_pc, new_psw;

		/* call the callback; if we don't get -1 back, use the return value as our vector */
		if (t11.irq_callback != NULL)
		{
			int new_vector = (*t11.irq_callback)(t11.device, t11.irq_state & 15);
			if (new_vector != -1)
				vector = new_vector;
		}

		/* fetch the new PC and PSW from that vector */
		assert((vector & 3) == 0);
		new_pc = RWORD(vector);
		new_psw = RWORD(vector + 2);

		/* push the old state, set the new one */
		PUSH(PSW);
		PUSH(PC);
		PCD = new_pc;
		PSW = new_psw;
		change_pc(PC);
		t11_check_irqs();

		/* count cycles and clear the WAIT flag */
		t11.interrupt_cycles += 114;
		t11.wait_state = 0;
	}
}



/*************************************
 *
 *  Core opcodes
 *
 *************************************/

/* includes the static function prototypes and the master opcode table */
#include "t11table.c"

/* includes the actual opcode implementations */
#include "t11ops.c"



/*************************************
 *
 *  Fetch current context into buffer
 *
 *************************************/

static CPU_GET_CONTEXT( t11 )
{
	if (dst)
		*(t11_Regs *)dst = t11;
}



/*************************************
 *
 *  Retrieve context from buffer
 *
 *************************************/

static CPU_SET_CONTEXT( t11 )
{
	if (src)
		t11 = *(t11_Regs *)src;
	change_pc(PC);
	t11_check_irqs();
}



/*************************************
 *
 *  Low-level initialization/cleanup
 *
 *************************************/

static CPU_INIT( t11 )
{
	static const UINT16 initial_pc[] =
	{
		0xc000, 0x8000, 0x4000, 0x2000,
		0x1000, 0x0000, 0xf600, 0xf400
	};
	const struct t11_setup *setup = device->static_config;

	t11.initial_pc = initial_pc[setup->mode >> 13];
	t11.irq_callback = irqcallback;
	t11.device = device;

	state_save_register_item("t11", device->tag, 0, t11.ppc.w.l);
	state_save_register_item("t11", device->tag, 0, t11.reg[0].w.l);
	state_save_register_item("t11", device->tag, 0, t11.reg[1].w.l);
	state_save_register_item("t11", device->tag, 0, t11.reg[2].w.l);
	state_save_register_item("t11", device->tag, 0, t11.reg[3].w.l);
	state_save_register_item("t11", device->tag, 0, t11.reg[4].w.l);
	state_save_register_item("t11", device->tag, 0, t11.reg[5].w.l);
	state_save_register_item("t11", device->tag, 0, t11.reg[6].w.l);
	state_save_register_item("t11", device->tag, 0, t11.reg[7].w.l);
	state_save_register_item("t11", device->tag, 0, t11.psw.w.l);
	state_save_register_item("t11", device->tag, 0, t11.op);
	state_save_register_item("t11", device->tag, 0, t11.initial_pc);
	state_save_register_item("t11", device->tag, 0, t11.wait_state);
	state_save_register_item("t11", device->tag, 0, t11.irq_state);
	state_save_register_item("t11", device->tag, 0, t11.interrupt_cycles);
}


static CPU_EXIT( t11 )
{
	/* nothing to do */
}



/*************************************
 *
 *  CPU reset
 *
 *************************************/

static CPU_RESET( t11 )
{
	/* initial SP is 376 octal, or 0xfe */
	SP = 0x00fe;

	/* initial PC comes from the setup word */
	PC = t11.initial_pc;
	change_pc(PC);

	/* PSW starts off at highest priority */
	PSW = 0xe0;

	/* initialize the IRQ state */
	t11.irq_state = 0;

	/* reset the remaining state */
	REGD(0) = 0;
	REGD(1) = 0;
	REGD(2) = 0;
	REGD(3) = 0;
	REGD(4) = 0;
	REGD(5) = 0;
	t11.ppc.d = 0;
	t11.wait_state = 0;
	t11.interrupt_cycles = 0;
}



/*************************************
 *
 *  Interrupt handling
 *
 *************************************/

static void set_irq_line(int irqline, int state)
{
	/* set the appropriate bit */
	if (state == CLEAR_LINE)
		t11.irq_state &= ~(1 << irqline);
	else
		t11.irq_state |= 1 << irqline;

	/* recheck for interrupts */
   	t11_check_irqs();
}



/*************************************
 *
 *  Core execution
 *
 *************************************/

static CPU_EXECUTE( t11 )
{
	t11_ICount = cycles;
	t11_ICount -= t11.interrupt_cycles;
	t11.interrupt_cycles = 0;

	if (t11.wait_state)
	{
		t11_ICount = 0;
		goto getout;
	}

	do
	{
		t11.ppc = t11.reg[7];	/* copy PC to previous PC */

		debugger_instruction_hook(device->machine, PCD);

		t11.op = ROPCODE();
		(*opcode_table[t11.op >> 3])();

	} while (t11_ICount > 0);

getout:

	t11_ICount -= t11.interrupt_cycles;
	t11.interrupt_cycles = 0;

	return cycles - t11_ICount;
}



/**************************************************************************
 * Generic set_info
 **************************************************************************/

static CPU_SET_INFO( t11 )
{
	switch (state)
	{
		/* --- the following bits of info are set as 64-bit signed integers --- */
		case CPUINFO_INT_INPUT_STATE + T11_IRQ0:		set_irq_line(T11_IRQ0, info->i);		break;
		case CPUINFO_INT_INPUT_STATE + T11_IRQ1:		set_irq_line(T11_IRQ1, info->i);		break;
		case CPUINFO_INT_INPUT_STATE + T11_IRQ2:		set_irq_line(T11_IRQ2, info->i);		break;
		case CPUINFO_INT_INPUT_STATE + T11_IRQ3:		set_irq_line(T11_IRQ3, info->i);		break;

		case CPUINFO_INT_PC:
		case CPUINFO_INT_REGISTER + T11_PC:				PC = info->i; change_pc(PC); 			break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + T11_SP:				SP = info->i;							break;
		case CPUINFO_INT_REGISTER + T11_PSW:			PSW = info->i;							break;
		case CPUINFO_INT_REGISTER + T11_R0:				REGW(0) = info->i;						break;
		case CPUINFO_INT_REGISTER + T11_R1:				REGW(1) = info->i;						break;
		case CPUINFO_INT_REGISTER + T11_R2:				REGW(2) = info->i;						break;
		case CPUINFO_INT_REGISTER + T11_R3:				REGW(3) = info->i;						break;
		case CPUINFO_INT_REGISTER + T11_R4:				REGW(4) = info->i;						break;
		case CPUINFO_INT_REGISTER + T11_R5:				REGW(5) = info->i;						break;
	}
}



/**************************************************************************
 * Generic get_info
 **************************************************************************/

CPU_GET_INFO( t11 )
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_CONTEXT_SIZE:					info->i = sizeof(t11);					break;
		case CPUINFO_INT_INPUT_LINES:					info->i = 4;							break;
		case CPUINFO_INT_DEFAULT_IRQ_VECTOR:			info->i = -1;							break;
		case CPUINFO_INT_ENDIANNESS:					info->i = CPU_IS_LE;					break;
		case CPUINFO_INT_CLOCK_MULTIPLIER:				info->i = 1;							break;
		case CPUINFO_INT_CLOCK_DIVIDER:					info->i = 1;							break;
		case CPUINFO_INT_MIN_INSTRUCTION_BYTES:			info->i = 2;							break;
		case CPUINFO_INT_MAX_INSTRUCTION_BYTES:			info->i = 6;							break;
		case CPUINFO_INT_MIN_CYCLES:					info->i = 12;							break;
		case CPUINFO_INT_MAX_CYCLES:					info->i = 110;							break;

		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_PROGRAM:	info->i = 16;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_PROGRAM: info->i = 16;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_PROGRAM: info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_DATA:	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH + ADDRESS_SPACE_IO:		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH + ADDRESS_SPACE_IO: 		info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT + ADDRESS_SPACE_IO: 		info->i = 0;					break;

		case CPUINFO_INT_INPUT_STATE + T11_IRQ0:		info->i = (t11.irq_state & 1) ? ASSERT_LINE : CLEAR_LINE; break;
		case CPUINFO_INT_INPUT_STATE + T11_IRQ1:		info->i = (t11.irq_state & 2) ? ASSERT_LINE : CLEAR_LINE; break;
		case CPUINFO_INT_INPUT_STATE + T11_IRQ2:		info->i = (t11.irq_state & 4) ? ASSERT_LINE : CLEAR_LINE; break;
		case CPUINFO_INT_INPUT_STATE + T11_IRQ3:		info->i = (t11.irq_state & 8) ? ASSERT_LINE : CLEAR_LINE; break;

		case CPUINFO_INT_PREVIOUSPC:					info->i = t11.ppc.w.l;					break;

		case CPUINFO_INT_PC:
		case CPUINFO_INT_REGISTER + T11_PC:				info->i = PCD;							break;
		case CPUINFO_INT_SP:
		case CPUINFO_INT_REGISTER + T11_SP:				info->i = SPD;							break;
		case CPUINFO_INT_REGISTER + T11_PSW:			info->i = PSW;							break;
		case CPUINFO_INT_REGISTER + T11_R0:				info->i = REGD(0);						break;
		case CPUINFO_INT_REGISTER + T11_R1:				info->i = REGD(1);						break;
		case CPUINFO_INT_REGISTER + T11_R2:				info->i = REGD(2);						break;
		case CPUINFO_INT_REGISTER + T11_R3:				info->i = REGD(3);						break;
		case CPUINFO_INT_REGISTER + T11_R4:				info->i = REGD(4);						break;
		case CPUINFO_INT_REGISTER + T11_R5:				info->i = REGD(5);						break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_PTR_SET_INFO:						info->setinfo = CPU_SET_INFO_NAME(t11);			break;
		case CPUINFO_PTR_GET_CONTEXT:					info->getcontext = CPU_GET_CONTEXT_NAME(t11);		break;
		case CPUINFO_PTR_SET_CONTEXT:					info->setcontext = CPU_SET_CONTEXT_NAME(t11);		break;
		case CPUINFO_PTR_INIT:							info->init = CPU_INIT_NAME(t11);					break;
		case CPUINFO_PTR_RESET:							info->reset = CPU_RESET_NAME(t11);				break;
		case CPUINFO_PTR_EXIT:							info->exit = CPU_EXIT_NAME(t11);					break;
		case CPUINFO_PTR_EXECUTE:						info->execute = CPU_EXECUTE_NAME(t11);			break;
		case CPUINFO_PTR_BURN:							info->burn = NULL;						break;
		case CPUINFO_PTR_DISASSEMBLE:					info->disassemble = CPU_DISASSEMBLE_NAME(t11);			break;
		case CPUINFO_PTR_INSTRUCTION_COUNTER:			info->icount = &t11_ICount;				break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case CPUINFO_STR_NAME:							strcpy(info->s, "T11");					break;
		case CPUINFO_STR_CORE_FAMILY:					strcpy(info->s, "DEC T-11");			break;
		case CPUINFO_STR_CORE_VERSION:					strcpy(info->s, "1.0");					break;
		case CPUINFO_STR_CORE_FILE:						strcpy(info->s, __FILE__);				break;
		case CPUINFO_STR_CORE_CREDITS:					strcpy(info->s, "Copyright Aaron Giles"); break;

		case CPUINFO_STR_FLAGS:
			sprintf(info->s, "%c%c%c%c%c%c%c%c",
				t11.psw.b.l & 0x80 ? '?':'.',
				t11.psw.b.l & 0x40 ? 'I':'.',
				t11.psw.b.l & 0x20 ? 'I':'.',
				t11.psw.b.l & 0x10 ? 'T':'.',
				t11.psw.b.l & 0x08 ? 'N':'.',
				t11.psw.b.l & 0x04 ? 'Z':'.',
				t11.psw.b.l & 0x02 ? 'V':'.',
				t11.psw.b.l & 0x01 ? 'C':'.');
			break;

		case CPUINFO_STR_REGISTER + T11_PC:				sprintf(info->s, "PC:%04X", t11.reg[7].w.l); break;
		case CPUINFO_STR_REGISTER + T11_SP:				sprintf(info->s, "SP:%04X", t11.reg[6].w.l); break;
		case CPUINFO_STR_REGISTER + T11_PSW:			sprintf(info->s, "PSW:%02X", t11.psw.b.l);   break;
		case CPUINFO_STR_REGISTER + T11_R0:				sprintf(info->s, "R0:%04X", t11.reg[0].w.l); break;
		case CPUINFO_STR_REGISTER + T11_R1:				sprintf(info->s, "R1:%04X", t11.reg[1].w.l); break;
		case CPUINFO_STR_REGISTER + T11_R2:				sprintf(info->s, "R2:%04X", t11.reg[2].w.l); break;
		case CPUINFO_STR_REGISTER + T11_R3:				sprintf(info->s, "R3:%04X", t11.reg[3].w.l); break;
		case CPUINFO_STR_REGISTER + T11_R4:				sprintf(info->s, "R4:%04X", t11.reg[4].w.l); break;
		case CPUINFO_STR_REGISTER + T11_R5:				sprintf(info->s, "R5:%04X", t11.reg[5].w.l); break;
	}
}
