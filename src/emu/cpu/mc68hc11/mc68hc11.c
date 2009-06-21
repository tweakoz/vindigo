/*
   Motorola MC68HC11 emulator

   Written by Ville Linde
 */

#include "debugger.h"
#include "mc68hc11.h"

enum
{
	HC11_PC = 1,
	HC11_SP,
	HC11_A,
	HC11_B,
	HC11_IX,
	HC11_IY
};

#define CC_S	0x80
#define CC_X	0x40
#define CC_H	0x20
#define CC_I	0x10
#define CC_N	0x08
#define CC_Z	0x04
#define CC_V	0x02
#define CC_C	0x01

typedef struct _hc11_state hc11_state;
struct _hc11_state
{
	union {
		struct {
#ifdef LSB_FIRST
			UINT8 b;
			UINT8 a;
#else
			UINT8 a;
			UINT8 b;
#endif
		} d8;
		UINT16 d16;
	} d;

	UINT16 ix;
	UINT16 iy;
	UINT16 sp;
	UINT16 pc;
	UINT16 ppc;
	UINT8 ccr;

	UINT8 adctl;
	int ad_channel;

	cpu_irq_callback irq_callback;
	const device_config *device;
	const address_space *program;
	const address_space *io;
	int icount;
	int ram_position;
	int reg_position;
	UINT8 *internal_ram;
	int internal_ram_size;
};

INLINE hc11_state *get_safe_token(const device_config *device)
{
	assert(device != NULL);
	assert(device->token != NULL);
	assert(device->type == CPU);
	assert(cpu_get_type(device) == CPU_MC68HC11);
	return (hc11_state *)device->token;
}

#define HC11OP(XX)		hc11_##XX

/*****************************************************************************/
/* Internal registers */

static UINT8 hc11_regs_r(hc11_state *cpustate, UINT32 address)
{
	int reg = address & 0xff;

	switch(reg)
	{
		case 0x00:		/* PORTA */
			return memory_read_byte(cpustate->io, MC68HC11_IO_PORTA);
		case 0x01:		/* DDRA */
			return 0;
		case 0x02:		/* PIOC */
			return 0;
		case 0x09:		/* DDRD */
			return 0;
		case 0x28:		/* SPCR1 */
			return 0;
		case 0x30:		/* ADCTL */
			return 0x80;
		case 0x31:		/* ADR1 */
		{
			if (cpustate->adctl & 0x10)
			{
				return memory_read_byte(cpustate->io, (cpustate->adctl & 0x4) + MC68HC11_IO_AD0);
			}
			else
			{
				return memory_read_byte(cpustate->io, (cpustate->adctl & 0x7) + MC68HC11_IO_AD0);
			}
		}
		case 0x32:		/* ADR2 */
		{
			if (cpustate->adctl & 0x10)
			{
				return memory_read_byte(cpustate->io, (cpustate->adctl & 0x4) + MC68HC11_IO_AD1);
			}
			else
			{
				return memory_read_byte(cpustate->io, (cpustate->adctl & 0x7) + MC68HC11_IO_AD0);
			}
		}
		case 0x33:		/* ADR3 */
		{
			if (cpustate->adctl & 0x10)
			{
				return memory_read_byte(cpustate->io, (cpustate->adctl & 0x4) + MC68HC11_IO_AD2);
			}
			else
			{
				return memory_read_byte(cpustate->io, (cpustate->adctl & 0x7) + MC68HC11_IO_AD0);
			}
		}
		case 0x34:		/* ADR4 */
		{
			if (cpustate->adctl & 0x10)
			{
				return memory_read_byte(cpustate->io, (cpustate->adctl & 0x4) + MC68HC11_IO_AD3);
			}
			else
			{
				return memory_read_byte(cpustate->io, (cpustate->adctl & 0x7) + MC68HC11_IO_AD0);
			}
		}
		case 0x38:		/* OPT2 */
			return 0;
		case 0x70:		/* SCBDH */
			return 0;
		case 0x71:		/* SCBDL */
			return 0;
		case 0x72:		/* SCCR1 */
			return 0;
		case 0x73:		/* SCCR2 */
			return 0;
		case 0x74:		/* SCSR1 */
			return 0x40;
		case 0x7c:		/* PORTH */
			return memory_read_byte(cpustate->io, MC68HC11_IO_PORTH);
		case 0x7e:		/* PORTG */
			return memory_read_byte(cpustate->io, MC68HC11_IO_PORTG);
		case 0x7f:		/* DDRG */
			return 0;

		case 0x88:		/* SPCR2 */
			return 0;
		case 0x89:		/* SPSR2 */
			return 0x80;
		case 0x8a:		/* SPDR2 */
			return memory_read_byte(cpustate->io, MC68HC11_IO_SPI2_DATA);

		case 0x8b:		/* OPT4 */
			return 0;
	}

	logerror("HC11: regs_r %02X\n", reg);
	return 0; // Dummy
}

static void hc11_regs_w(hc11_state *cpustate, UINT32 address, UINT8 value)
{
	int reg = address & 0xff;

	switch(reg)
	{
		case 0x00:		/* PORTA */
			memory_write_byte(cpustate->io, MC68HC11_IO_PORTA, value);
			return;
		case 0x01:		/* DDRA */
			//mame_printf_debug("HC11: ddra = %02X\n", value);
			return;
		case 0x08:		/* PORTD */
			memory_write_byte(cpustate->io, MC68HC11_IO_PORTD, value);
			return;
		case 0x09:		/* DDRD */
			//mame_printf_debug("HC11: ddrd = %02X\n", value);
			return;
		case 0x22:		/* TMSK1 */
			return;
		case 0x24:		/* TMSK2 */
			return;
		case 0x28:		/* SPCR1 */
			return;
		case 0x30:		/* ADCTL */
			cpustate->adctl = value;
			return;
		case 0x38:		/* OPT2 */
			return;
		case 0x39:		/* OPTION */
			return;

		case 0x3d:		/* INIT */
		{
			int reg_page = value & 0xf;
			int ram_page = (value >> 4) & 0xf;

			if (reg_page == ram_page) {
				cpustate->reg_position = reg_page << 12;
				cpustate->ram_position = (ram_page << 12) + 0x100;
			} else {
				cpustate->reg_position = reg_page << 12;
				cpustate->ram_position = ram_page << 12;
			}
			return;
		}

		case 0x3f:		/* CONFIG */
			return;

		case 0x70:		/* SCBDH */
			return;
		case 0x71:		/* SCBDL */
			return;
		case 0x72:		/* SCCR1 */
			return;
		case 0x73:		/* SCCR2 */
			return;
		case 0x77:		/* SCDRL */
			return;
		case 0x7c:		/* PORTH */
			memory_write_byte(cpustate->io, MC68HC11_IO_PORTH, value);
			return;
		case 0x7d:		/* DDRH */
			//mame_printf_debug("HC11: ddrh = %02X at %04X\n", value, cpustate->pc);
			return;
		case 0x7e:		/* PORTG */
			memory_write_byte(cpustate->io, MC68HC11_IO_PORTG, value);
			return;
		case 0x7f:		/* DDRG */
			//mame_printf_debug("HC11: ddrg = %02X at %04X\n", value, cpustate->pc);
			return;

		case 0x88:		/* SPCR2 */
			return;
		case 0x89:		/* SPSR2 */
			return;
		case 0x8a:		/* SPDR2 */
			memory_write_byte(cpustate->io, MC68HC11_IO_SPI2_DATA, value);
			return;

		case 0x8b:		/* OPT4 */
			return;

	}

	logerror("HC11: regs_w %02X, %02X\n", reg, value);
}

/*****************************************************************************/

INLINE UINT8 FETCH(hc11_state *cpustate)
{
	return memory_decrypted_read_byte(cpustate->program, cpustate->pc++);
}

INLINE UINT16 FETCH16(hc11_state *cpustate)
{
	UINT16 w;
	w = (memory_decrypted_read_byte(cpustate->program, cpustate->pc) << 8) | (memory_decrypted_read_byte(cpustate->program, cpustate->pc+1));
	cpustate->pc += 2;
	return w;
}

INLINE UINT8 READ8(hc11_state *cpustate, UINT32 address)
{
	if(address >= cpustate->reg_position && address < cpustate->reg_position+0x100)
	{
		return hc11_regs_r(cpustate, address);
	}
	else if(address >= cpustate->ram_position && address < cpustate->ram_position+cpustate->internal_ram_size)
	{
		return cpustate->internal_ram[address-cpustate->ram_position];
	}
	return memory_read_byte(cpustate->program, address);
}

INLINE void WRITE8(hc11_state *cpustate, UINT32 address, UINT8 value)
{
	if(address >= cpustate->reg_position && address < cpustate->reg_position+0x100)
	{
		hc11_regs_w(cpustate, address, value);
		return;
	}
	else if(address >= cpustate->ram_position && address < cpustate->ram_position+cpustate->internal_ram_size)
	{
		cpustate->internal_ram[address-cpustate->ram_position] = value;
		return;
	}
	memory_write_byte(cpustate->program, address, value);
}

INLINE UINT16 READ16(hc11_state *cpustate, UINT32 address)
{
	return (READ8(cpustate, address) << 8) | (READ8(cpustate, address+1));
}

INLINE void WRITE16(hc11_state *cpustate, UINT32 address, UINT16 value)
{
	WRITE8(cpustate, address+0, (value >> 8) & 0xff);
	WRITE8(cpustate, address+1, (value >> 0) & 0xff);
}

/*****************************************************************************/

static void (*hc11_optable[256])(hc11_state *cpustate);
static void (*hc11_optable_page2[256])(hc11_state *cpustate);
static void (*hc11_optable_page3[256])(hc11_state *cpustate);
static void (*hc11_optable_page4[256])(hc11_state *cpustate);

#include "hc11ops.c"
#include "hc11ops.h"

static CPU_INIT( hc11 )
{
	hc11_state *cpustate = get_safe_token(device);
	int i;

	/* clear the opcode tables */
	for(i=0; i < 256; i++) {
		hc11_optable[i] = HC11OP(invalid);
		hc11_optable_page2[i] = HC11OP(invalid);
		hc11_optable_page3[i] = HC11OP(invalid);
		hc11_optable_page4[i] = HC11OP(invalid);
	}
	/* fill the opcode tables */
	for(i=0; i < sizeof(hc11_opcode_list)/sizeof(hc11_opcode_list_struct); i++)
	{
		switch(hc11_opcode_list[i].page)
		{
			case 0x00:
				hc11_optable[hc11_opcode_list[i].opcode] = hc11_opcode_list[i].handler;
				break;
			case 0x18:
				hc11_optable_page2[hc11_opcode_list[i].opcode] = hc11_opcode_list[i].handler;
				break;
			case 0x1A:
				hc11_optable_page3[hc11_opcode_list[i].opcode] = hc11_opcode_list[i].handler;
				break;
			case 0xCD:
				hc11_optable_page4[hc11_opcode_list[i].opcode] = hc11_opcode_list[i].handler;
				break;
		}
	}

	cpustate->internal_ram_size = 1280;		/* FIXME: this is for MC68HC11M0 */
	cpustate->internal_ram = auto_alloc_array(device->machine, UINT8, cpustate->internal_ram_size);

	cpustate->reg_position = 0;
	cpustate->ram_position = 0x100;
	cpustate->irq_callback = irqcallback;
	cpustate->device = device;
	cpustate->program = memory_find_address_space(device, ADDRESS_SPACE_PROGRAM);
	cpustate->io = memory_find_address_space(device, ADDRESS_SPACE_IO);
}

static CPU_RESET( hc11 )
{
	hc11_state *cpustate = get_safe_token(device);
	cpustate->pc = READ16(cpustate, 0xfffe);
}

static CPU_EXIT( hc11 )
{

}

static CPU_EXECUTE( hc11 )
{
	hc11_state *cpustate = get_safe_token(device);

	cpustate->icount = cycles;

	while(cpustate->icount > 0)
	{
		UINT8 op;

		cpustate->ppc = cpustate->pc;
		debugger_instruction_hook(device, cpustate->pc);

		op = FETCH(cpustate);
		hc11_optable[op](cpustate);
	}

	return cycles-cpustate->icount;
}

/*****************************************************************************/

static CPU_SET_INFO( mc68hc11 )
{
	hc11_state *cpustate = get_safe_token(device);

	switch (state)
	{
		/* --- the following bits of info are set as 64-bit signed integers --- */
		case CPUINFO_INT_PC:							cpustate->pc = info->i;						break;
		case CPUINFO_INT_REGISTER + HC11_PC:			cpustate->pc = info->i; 					break;
		case CPUINFO_INT_REGISTER + HC11_SP:			cpustate->sp = info->i;						break;
		case CPUINFO_INT_REGISTER + HC11_A:				cpustate->d.d8.a = info->i;					break;
		case CPUINFO_INT_REGISTER + HC11_B:				cpustate->d.d8.b = info->i;					break;
		case CPUINFO_INT_REGISTER + HC11_IX:			cpustate->ix = info->i;						break;
		case CPUINFO_INT_REGISTER + HC11_IY:			cpustate->iy = info->i;						break;
	}
}

CPU_GET_INFO( mc68hc11 )
{
	hc11_state *cpustate = (device != NULL && device->token != NULL) ? get_safe_token(device) : NULL;

	switch(state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case CPUINFO_INT_CONTEXT_SIZE:					info->i = sizeof(hc11_state);			break;
		case CPUINFO_INT_INPUT_LINES:					info->i = 1;							break;
		case CPUINFO_INT_DEFAULT_IRQ_VECTOR:			info->i = 0;							break;
		case CPUINFO_INT_ENDIANNESS:					info->i = ENDIANNESS_BIG;					break;
		case CPUINFO_INT_CLOCK_MULTIPLIER:				info->i = 1;							break;
		case CPUINFO_INT_CLOCK_DIVIDER:					info->i = 1;							break;
		case CPUINFO_INT_MIN_INSTRUCTION_BYTES:			info->i = 1;							break;
		case CPUINFO_INT_MAX_INSTRUCTION_BYTES:			info->i = 5;							break;
		case CPUINFO_INT_MIN_CYCLES:					info->i = 1;							break;
		case CPUINFO_INT_MAX_CYCLES:					info->i = 41;							break;

		case CPUINFO_INT_DATABUS_WIDTH_PROGRAM:	info->i = 8;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH_PROGRAM: info->i = 16;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT_PROGRAM: info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH_DATA:	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT_DATA: 	info->i = 0;					break;
		case CPUINFO_INT_DATABUS_WIDTH_IO:		info->i = 8;					break;
		case CPUINFO_INT_ADDRBUS_WIDTH_IO: 		info->i = 8;					break;
		case CPUINFO_INT_ADDRBUS_SHIFT_IO: 		info->i = 0;					break;

		case CPUINFO_INT_INPUT_STATE:					info->i = CLEAR_LINE;					break;

		case CPUINFO_INT_PREVIOUSPC:					/* not implemented */					break;

		case CPUINFO_INT_PC:	/* intentional fallthrough */
		case CPUINFO_INT_REGISTER + HC11_PC:			info->i = cpustate->pc;						break;
		case CPUINFO_INT_REGISTER + HC11_SP:			info->i = cpustate->sp;						break;
		case CPUINFO_INT_REGISTER + HC11_A:				info->i = cpustate->d.d8.a;					break;
		case CPUINFO_INT_REGISTER + HC11_B:				info->i = cpustate->d.d8.b;					break;
		case CPUINFO_INT_REGISTER + HC11_IX:			info->i = cpustate->ix;						break;
		case CPUINFO_INT_REGISTER + HC11_IY:			info->i = cpustate->iy;						break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case CPUINFO_FCT_SET_INFO:						info->setinfo = CPU_SET_INFO_NAME(mc68hc11);		break;
		case CPUINFO_FCT_INIT:							info->init = CPU_INIT_NAME(hc11);					break;
		case CPUINFO_FCT_RESET:							info->reset = CPU_RESET_NAME(hc11);				break;
		case CPUINFO_FCT_EXIT:							info->exit = CPU_EXIT_NAME(hc11);					break;
		case CPUINFO_FCT_EXECUTE:						info->execute = CPU_EXECUTE_NAME(hc11);			break;
		case CPUINFO_FCT_BURN:							info->burn = NULL;						break;
		case CPUINFO_FCT_DISASSEMBLE:					info->disassemble = hc11_disasm;		break;
		case CPUINFO_PTR_INSTRUCTION_COUNTER:			info->icount = &cpustate->icount;			break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case CPUINFO_STR_NAME:							strcpy(info->s, "MC68HC11");			break;
		case CPUINFO_STR_CORE_FAMILY:					strcpy(info->s, "Motorola MC68HC11");	break;
		case CPUINFO_STR_CORE_VERSION:					strcpy(info->s, "1.0");					break;
		case CPUINFO_STR_CORE_FILE:						strcpy(info->s, __FILE__);				break;
		case CPUINFO_STR_CORE_CREDITS:					strcpy(info->s, "Copyright Ville Linde"); break;

		case CPUINFO_STR_FLAGS:
			sprintf(info->s, "%c%c%c%c%c%c%c%c",
				(cpustate->ccr & CC_S) ? 'S' : '.',
				(cpustate->ccr & CC_X) ? 'X' : '.',
				(cpustate->ccr & CC_H) ? 'H' : '.',
				(cpustate->ccr & CC_I) ? 'I' : '.',
				(cpustate->ccr & CC_N) ? 'N' : '.',
				(cpustate->ccr & CC_Z) ? 'Z' : '.',
				(cpustate->ccr & CC_V) ? 'V' : '.',
				(cpustate->ccr & CC_C) ? 'C' : '.');
			break;

		case CPUINFO_STR_REGISTER + HC11_PC:			sprintf(info->s, "PC: %04X", cpustate->pc);	break;
		case CPUINFO_STR_REGISTER + HC11_SP:			sprintf(info->s, "SP: %04X", cpustate->sp);	break;
		case CPUINFO_STR_REGISTER + HC11_A:				sprintf(info->s, "A: %02X", cpustate->d.d8.a); break;
		case CPUINFO_STR_REGISTER + HC11_B:				sprintf(info->s, "B: %02X", cpustate->d.d8.b); break;
		case CPUINFO_STR_REGISTER + HC11_IX:			sprintf(info->s, "IX: %04X", cpustate->ix);	break;
		case CPUINFO_STR_REGISTER + HC11_IY:			sprintf(info->s, "IY: %04X", cpustate->iy);	break;
	}
}
