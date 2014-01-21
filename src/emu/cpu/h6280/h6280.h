/*****************************************************************************

    h6280.h Portable Hu6280 emulator interface

    Copyright Bryan McPhail, mish@tendril.co.uk

    This source code is based (with permission!) on the 6502 emulator by
    Juergen Buchmueller.  It is released as part of the Mame emulator project.
    Let me know if you intend to use this code in any other project.

******************************************************************************/

#pragma once

#ifndef __H6280_H__
#define __H6280_H__

#include "emu.h"

#pragma push_macro("BIT")
#undef BIT

#define LAZY_FLAGS  0

/***************************************************************************
    REGISTER ENUMERATION
***************************************************************************/

enum
{
	H6280_PC = 1,
	H6280_S,
	H6280_P,
	H6280_A,
	H6280_X,
	H6280_Y,
	H6280_IRQ_MASK,
	H6280_TIMER_STATE,
	H6280_NMI_STATE,
	H6280_IRQ1_STATE,
	H6280_IRQ2_STATE,
	H6280_IRQT_STATE,
	H6280_M1,
	H6280_M2,
	H6280_M3,
	H6280_M4,
	H6280_M5,
	H6280_M6,
	H6280_M7,
	H6280_M8
};

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> h6280_device

// Used by core CPU interface
class h6280_device : public cpu_device
{
public:
	// construction/destruction
	h6280_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// public interfaces
	void set_irq_line(int irqline, int state);

	DECLARE_READ8_MEMBER( irq_status_r );
	DECLARE_WRITE8_MEMBER( irq_status_w );

	DECLARE_READ8_MEMBER( timer_r );
	DECLARE_WRITE8_MEMBER( timer_w );

	/* functions for use by the PSG and joypad port only! */
	UINT8 io_get_buffer();
	void io_set_buffer(UINT8);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();
	virtual void device_stop();

	// device_execute_interface overrides
	virtual UINT32 execute_min_cycles() const;
	virtual UINT32 execute_max_cycles() const;
	virtual UINT32 execute_input_lines() const;
	virtual void execute_run();
	virtual void execute_set_input(int inputnum, int state);

	// device_memory_interface overrides
	virtual const address_space_config *memory_space_config(address_spacenum spacenum = AS_0) const { return (spacenum == AS_PROGRAM) ? &m_program_config : ( (spacenum == AS_IO) ? &m_io_config : NULL ); }
	virtual bool memory_translate(address_spacenum spacenum, int intention, offs_t &address);

	// device_disasm_interface overrides
	virtual UINT32 disasm_min_opcode_bytes() const;
	virtual UINT32 disasm_max_opcode_bytes() const;
	virtual offs_t disasm_disassemble(char *buffer, offs_t pc, const UINT8 *oprom, const UINT8 *opram, UINT32 options);

	// device_state_interface overrides
	virtual void state_string_export(const device_state_entry &entry, astring &string);

	// opcode accessors
	UINT8 program_read8(offs_t addr);
	void program_write8(offs_t addr, UINT8 data);
	UINT8 program_read8z(offs_t addr);
	void program_write8z(offs_t addr, UINT8 data);
	UINT16 program_read16(offs_t addr);
	UINT16 program_read16z(offs_t addr);
	void push(UINT8 value);
	void pull(UINT8 &value);
	UINT8 read_opcode();
	UINT8 read_opcode_arg();

#undef PROTOTYPES
#define PROTOTYPES(prefix) \
	void prefix##_00(); void prefix##_01(); void prefix##_02(); void prefix##_03(); \
	void prefix##_04(); void prefix##_05(); void prefix##_06(); void prefix##_07(); \
	void prefix##_08(); void prefix##_09(); void prefix##_0a(); void prefix##_0b(); \
	void prefix##_0c(); void prefix##_0d(); void prefix##_0e(); void prefix##_0f(); \
	void prefix##_10(); void prefix##_11(); void prefix##_12(); void prefix##_13(); \
	void prefix##_14(); void prefix##_15(); void prefix##_16(); void prefix##_17(); \
	void prefix##_18(); void prefix##_19(); void prefix##_1a(); void prefix##_1b(); \
	void prefix##_1c(); void prefix##_1d(); void prefix##_1e(); void prefix##_1f(); \
	void prefix##_20(); void prefix##_21(); void prefix##_22(); void prefix##_23(); \
	void prefix##_24(); void prefix##_25(); void prefix##_26(); void prefix##_27(); \
	void prefix##_28(); void prefix##_29(); void prefix##_2a(); void prefix##_2b(); \
	void prefix##_2c(); void prefix##_2d(); void prefix##_2e(); void prefix##_2f(); \
	void prefix##_30(); void prefix##_31(); void prefix##_32(); void prefix##_33(); \
	void prefix##_34(); void prefix##_35(); void prefix##_36(); void prefix##_37(); \
	void prefix##_38(); void prefix##_39(); void prefix##_3a(); void prefix##_3b(); \
	void prefix##_3c(); void prefix##_3d(); void prefix##_3e(); void prefix##_3f(); \
	void prefix##_40(); void prefix##_41(); void prefix##_42(); void prefix##_43(); \
	void prefix##_44(); void prefix##_45(); void prefix##_46(); void prefix##_47(); \
	void prefix##_48(); void prefix##_49(); void prefix##_4a(); void prefix##_4b(); \
	void prefix##_4c(); void prefix##_4d(); void prefix##_4e(); void prefix##_4f(); \
	void prefix##_50(); void prefix##_51(); void prefix##_52(); void prefix##_53(); \
	void prefix##_54(); void prefix##_55(); void prefix##_56(); void prefix##_57(); \
	void prefix##_58(); void prefix##_59(); void prefix##_5a(); void prefix##_5b(); \
	void prefix##_5c(); void prefix##_5d(); void prefix##_5e(); void prefix##_5f(); \
	void prefix##_60(); void prefix##_61(); void prefix##_62(); void prefix##_63(); \
	void prefix##_64(); void prefix##_65(); void prefix##_66(); void prefix##_67(); \
	void prefix##_68(); void prefix##_69(); void prefix##_6a(); void prefix##_6b(); \
	void prefix##_6c(); void prefix##_6d(); void prefix##_6e(); void prefix##_6f(); \
	void prefix##_70(); void prefix##_71(); void prefix##_72(); void prefix##_73(); \
	void prefix##_74(); void prefix##_75(); void prefix##_76(); void prefix##_77(); \
	void prefix##_78(); void prefix##_79(); void prefix##_7a(); void prefix##_7b(); \
	void prefix##_7c(); void prefix##_7d(); void prefix##_7e(); void prefix##_7f(); \
	void prefix##_80(); void prefix##_81(); void prefix##_82(); void prefix##_83(); \
	void prefix##_84(); void prefix##_85(); void prefix##_86(); void prefix##_87(); \
	void prefix##_88(); void prefix##_89(); void prefix##_8a(); void prefix##_8b(); \
	void prefix##_8c(); void prefix##_8d(); void prefix##_8e(); void prefix##_8f(); \
	void prefix##_90(); void prefix##_91(); void prefix##_92(); void prefix##_93(); \
	void prefix##_94(); void prefix##_95(); void prefix##_96(); void prefix##_97(); \
	void prefix##_98(); void prefix##_99(); void prefix##_9a(); void prefix##_9b(); \
	void prefix##_9c(); void prefix##_9d(); void prefix##_9e(); void prefix##_9f(); \
	void prefix##_a0(); void prefix##_a1(); void prefix##_a2(); void prefix##_a3(); \
	void prefix##_a4(); void prefix##_a5(); void prefix##_a6(); void prefix##_a7(); \
	void prefix##_a8(); void prefix##_a9(); void prefix##_aa(); void prefix##_ab(); \
	void prefix##_ac(); void prefix##_ad(); void prefix##_ae(); void prefix##_af(); \
	void prefix##_b0(); void prefix##_b1(); void prefix##_b2(); void prefix##_b3(); \
	void prefix##_b4(); void prefix##_b5(); void prefix##_b6(); void prefix##_b7(); \
	void prefix##_b8(); void prefix##_b9(); void prefix##_ba(); void prefix##_bb(); \
	void prefix##_bc(); void prefix##_bd(); void prefix##_be(); void prefix##_bf(); \
	void prefix##_c0(); void prefix##_c1(); void prefix##_c2(); void prefix##_c3(); \
	void prefix##_c4(); void prefix##_c5(); void prefix##_c6(); void prefix##_c7(); \
	void prefix##_c8(); void prefix##_c9(); void prefix##_ca(); void prefix##_cb(); \
	void prefix##_cc(); void prefix##_cd(); void prefix##_ce(); void prefix##_cf(); \
	void prefix##_d0(); void prefix##_d1(); void prefix##_d2(); void prefix##_d3(); \
	void prefix##_d4(); void prefix##_d5(); void prefix##_d6(); void prefix##_d7(); \
	void prefix##_d8(); void prefix##_d9(); void prefix##_da(); void prefix##_db(); \
	void prefix##_dc(); void prefix##_dd(); void prefix##_de(); void prefix##_df(); \
	void prefix##_e0(); void prefix##_e1(); void prefix##_e2(); void prefix##_e3(); \
	void prefix##_e4(); void prefix##_e5(); void prefix##_e6(); void prefix##_e7(); \
	void prefix##_e8(); void prefix##_e9(); void prefix##_ea(); void prefix##_eb(); \
	void prefix##_ec(); void prefix##_ed(); void prefix##_ee(); void prefix##_ef(); \
	void prefix##_f0(); void prefix##_f1(); void prefix##_f2(); void prefix##_f3(); \
	void prefix##_f4(); void prefix##_f5(); void prefix##_f6(); void prefix##_f7(); \
	void prefix##_f8(); void prefix##_f9(); void prefix##_fa(); void prefix##_fb(); \
	void prefix##_fc(); void prefix##_fd(); void prefix##_fe(); void prefix##_ff();

	PROTOTYPES(op);

	UINT32 TRANSLATED(UINT16 addr);
	void H6280_CYCLES(int cyc);
	void SET_NZ(UINT8 n);
	void CLEAR_T();
	void DO_INTERRUPT(UINT16 vector);
	void CHECK_AND_TAKE_IRQ_LINES();
	void CHECK_IRQ_LINES();
	void CHECK_VDC_VCE_PENALTY(UINT16 addr);
	void BRA(bool cond);
	void EA_ZPG();
	void EA_TFLG();
	void EA_ZPX();
	void EA_ZPY();
	void EA_ABS();
	void EA_ABX();
	void EA_ABY();
	void EA_ZPI();
	void EA_IDX();
	void EA_IDY();
	void EA_IND();
	void EA_IAX();
	UINT8 RD_IMM();
	UINT8 RD_ZPG();
	UINT8 RD_ZPX();
	UINT8 RD_ZPY();
	UINT8 RD_ABS();
	UINT8 RD_ABX();
	UINT8 RD_ABY();
	UINT8 RD_ZPI();
	UINT8 RD_IDX();
	UINT8 RD_IDY();
	UINT8 RD_TFL();
	void WR_ZPG(UINT8 tmp);
	void WR_ZPX(UINT8 tmp);
	void WR_ZPY(UINT8 tmp);
	void WR_ABS(UINT8 tmp);
	void WR_ABX(UINT8 tmp);
	void WR_ABY(UINT8 tmp);
	void WR_ZPI(UINT8 tmp);
	void WR_IDX(UINT8 tmp);
	void WR_IDY(UINT8 tmp);
	void WB_EA(UINT8 tmp);
	void WB_EAZ(UINT8 tmp);
	void COMPOSE_P(UINT8 SET, UINT8 CLR);
	void TADC(UINT8 tmp);
	void ADC(UINT8 tmp);
	void TAND(UINT8 tmp);
	void AND(UINT8 tmp);
	UINT8 ASL(UINT8 tmp);
	void BBR(int bit, UINT8 tmp);
	void BBS(int bit, UINT8 tmp);
	void BCC();
	void BCS();
	void BEQ();
	void BIT(UINT8 tmp);
	void BMI();
	void BNE();
	void BPL();
	void BRK();
	void BSR();
	void BVC();
	void BVS();
	void CLA();
	void CLC();
	void CLD();
	void CLI();
	void CLV();
	void CLX();
	void CLY();
	void CMP(UINT8 tmp);
	void CPX(UINT8 tmp);
	void CPY(UINT8 tmp);
	UINT8 DEC(UINT8 tmp);
	void DEX();
	void DEY();
	void TEOR(UINT8 tmp);
	void EOR(UINT8 tmp);
	UINT8 INC(UINT8 tmp);
	void INX();
	void INY();
	void JMP();
	void JSR();
	void LDA(UINT8 tmp);
	void LDX(UINT8 tmp);
	void LDY(UINT8 tmp);
	UINT8 LSR(UINT8 tmp);
	void NOP();
	void TORA(UINT8 tmp);
	void ORA(UINT8 tmp);
	void PHA();
	void PHP();
	void PHX();
	void PHY();
	void PLA();
	void PLP();
	void PLX();
	void PLY();
	UINT8 RMB(int bit, UINT8 tmp);
	UINT8 ROL(UINT8 tmp);
	UINT8 ROR(UINT8 tmp);
	void RTI();
	void RTS();
	void SAX();
	void SAY();
	void TSBC(UINT8 tmp);
	void SBC(UINT8 tmp);
	void SEC();
	void SED();
	void SEI();
	void SET();
	UINT8 SMB(int bit, UINT8 tmp);
	void ST0(UINT8 tmp);
	void ST1(UINT8 tmp);
	void ST2(UINT8 tmp);
	UINT8 STA();
	UINT8 STX();
	UINT8 STY();
	UINT8 STZ();
	void SXY();
	void TAI();
	void TAM(UINT8 tmp);
	void TAX();
	void TAY();
	void TDD();
	void TIA();
	void TII();
	void TIN();
	void TMA(UINT8 tmp);
	UINT8 TRB(UINT8 tmp);
	UINT8 TSB(UINT8 tmp);
	void TSX();
	void TST(UINT8 imm, UINT8 tmp);
	void TXA();
	void TXS();
	void TYA();
	void CSH();
	void CSL();

	enum
	{
		H6280_RESET_VEC = 0xfffe,
		H6280_NMI_VEC =  0xfffc,
		H6280_TIMER_VEC = 0xfffa,
		H6280_IRQ1_VEC = 0xfff8,
		H6280_IRQ2_VEC = 0xfff6 /* Aka BRK vector */
	};

	// address spaces
	const address_space_config m_program_config;
	const address_space_config m_io_config;

	// CPU registers
	PAIR  m_ppc;            /* previous program counter */
	PAIR  m_pc;             /* program counter */
	PAIR  m_sp;             /* stack pointer (always 100 - 1FF) */
	PAIR  m_zp;             /* zero page address */
	PAIR  m_ea;             /* effective address */
	UINT8 m_a;              /* Accumulator */
	UINT8 m_x;              /* X index register */
	UINT8 m_y;              /* Y index register */
	UINT8 m_p;              /* Processor status */
	UINT8 m_mmr[8];         /* Hu6280 memory mapper registers */
	UINT8 m_irq_mask;       /* interrupt enable/disable */
	UINT8 m_timer_status;   /* timer status */
	UINT8 m_timer_ack;      /* timer acknowledge */
	UINT8 m_clocks_per_cycle; /* 4 = low speed mode, 1 = high speed mode */
	INT32 m_timer_value;    /* timer interrupt */
	INT32 m_timer_load;     /* reload value */
	UINT8 m_nmi_state;
	UINT8 m_irq_state[3];
	UINT8 m_irq_pending;
#if LAZY_FLAGS
	INT32 m_nz;         /* last value (lazy N and Z flag) */
#endif
	UINT8 m_io_buffer;  /* last value written to the PSG, timer, and interrupt pages */

	// other internal states
	int m_icount;

	// address spaces
	address_space *m_program;
	address_space *m_io;
	direct_read_data *m_direct;

	typedef void (h6280_device::*ophandler)();

	ophandler m_opcode[256];

	static const ophandler s_opcodetable[256];
};

extern const device_type H6280;

#pragma pop_macro("BIT")

#endif /* __H6280_H__ */
