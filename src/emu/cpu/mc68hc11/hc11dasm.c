/*
   Motorola M68HC11 disassembler

   Written by Ville Linde
*/

#include "cpuintrf.h"
#include <stdarg.h>

enum
{
	EA_IMM8 = 1,
	EA_IMM16,
	EA_EXT,
	EA_REL,
	EA_DIRECT,
	EA_DIRECT_IMM8,
	EA_DIRECT_IMM8_REL,
	EA_IND_X,
	EA_IND_X_IMM8,
	EA_IND_X_IMM8_REL,
	EA_IND_Y,
	EA_IND_Y_IMM8,
	EA_IND_Y_IMM8_REL,
	PAGE2,
	PAGE3,
	PAGE4,
};

typedef struct {
	char mnemonic[32];
	int address_mode;
} M68HC11_OPCODE;

static const M68HC11_OPCODE opcode_table[256] =
{
	/* 0x00 - 0x0f */
	{ "test",			0,						},
	{ "nop",			0,						},
	{ "idiv",			0,						},
	{ "fdiv",			0,						},
	{ "lsrd",			0,						},
	{ "asld",			0,						},
	{ "tap",			0,						},
	{ "tpa",			0,						},
	{ "inx",			0,						},
	{ "dex",			0,						},
	{ "clv",			0,						},
	{ "sev",			0,						},
	{ "clc",			0,						},
	{ "sec",			0,						},
	{ "cli",			0,						},
	{ "sei",			0,						},
	/* 0x10 - 0x1f */
	{ "sba",			0,						},
	{ "cba",			0,						},
	{ "brset",			EA_DIRECT_IMM8_REL,		},
	{ "brclr",			EA_DIRECT_IMM8_REL,		},
	{ "bset",			EA_DIRECT_IMM8,			},
	{ "bclr",			EA_DIRECT_IMM8,			},
	{ "tab",			0,						},
	{ "tba",			0,						},
	{ "page2",			PAGE2,					},
	{ "daa",			0,						},
	{ "page3",			PAGE3,					},
	{ "aba",			0,						},
	{ "bset",			EA_IND_X_IMM8,			},
	{ "bclr",			EA_IND_X_IMM8,			},
	{ "brset",			EA_IND_X_IMM8_REL,		},
	{ "brclr",			EA_IND_X_IMM8_REL,		},
	/* 0x20 - 0x2f */
	{ "bra",			EA_REL,					},
	{ "brn",			EA_REL,					},
	{ "bhi",			EA_REL,					},
	{ "bls",			EA_REL,					},
	{ "bcc",			EA_REL,					},
	{ "bcs",			EA_REL,					},
	{ "bne",			EA_REL,					},
	{ "beq",			EA_REL,					},
	{ "bvc",			EA_REL,					},
	{ "bvs",			EA_REL,					},
	{ "bpl",			EA_REL,					},
	{ "bmi",			EA_REL,					},
	{ "bge",			EA_REL,					},
	{ "blt",			EA_REL,					},
	{ "bgt",			EA_REL,					},
	{ "ble",			EA_REL,					},
	/* 0x30 - 0x3f */
	{ "tsx",			0,						},
	{ "ins",			0,						},
	{ "pula",			0,						},
	{ "pulb",			0,						},
	{ "des",			0,						},
	{ "txs",			0,						},
	{ "psha",			0,						},
	{ "pshb",			0,						},
	{ "pulx",			0,						},
	{ "rts",			0,						},
	{ "abx",			0,						},
	{ "rti",			0,						},
	{ "pshx",			0,						},
	{ "mul",			0,						},
	{ "wai",			0,						},
	{ "swi",			0,						},
	/* 0x40 - 0x4f */
	{ "nega",			0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "coma",			0,						},
	{ "lsra",			0,						},
	{ "?",				0,						},
	{ "rora",			0,						},
	{ "asra",			0,						},
	{ "asla",			0,						},
	{ "rola",			0,						},
	{ "deca",			0,						},
	{ "?",				0,						},
	{ "inca",			0,						},
	{ "tsta",			0,						},
	{ "?",				0,						},
	{ "clra",			0,						},
	/* 0x50 - 0x5f */
	{ "negb",			0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "comb",			0,						},
	{ "lsrb",			0,						},
	{ "?",				0,						},
	{ "rorb",			0,						},
	{ "asrb",			0,						},
	{ "aslb",			0,						},
	{ "rolb",			0,						},
	{ "decb",			0,						},
	{ "?",				0,						},
	{ "incb",			0,						},
	{ "tstb",			0,						},
	{ "?",				0,						},
	{ "clrb",			0,						},
	/* 0x60 - 0x6f */
	{ "neg",			EA_IND_X,				},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "com",			EA_IND_X,				},
	{ "lsr",			EA_IND_X,				},
	{ "?",				0,						},
	{ "ror",			EA_IND_X,				},
	{ "asr",			EA_IND_X,				},
	{ "asl",			EA_IND_X,				},
	{ "rol",			EA_IND_X,				},
	{ "dec",			EA_IND_X,				},
	{ "?",				0,						},
	{ "inc",			EA_IND_X,				},
	{ "tst",			EA_IND_X,				},
	{ "jmp",			EA_IND_X,				},
	{ "clr",			EA_IND_X,				},
	/* 0x70 - 0x7f */
	{ "neg",			EA_EXT,					},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "com",			EA_EXT,					},
	{ "lsr",			EA_EXT,					},
	{ "?",				0,						},
	{ "ror",			EA_EXT,					},
	{ "asr",			EA_EXT,					},
	{ "asl",			EA_EXT,					},
	{ "rol",			EA_EXT,					},
	{ "dec",			EA_EXT,					},
	{ "?",				0,						},
	{ "inc",			EA_EXT,					},
	{ "tst",			EA_EXT,					},
	{ "jmp",			EA_EXT,					},
	{ "clr",			EA_EXT,					},
	/* 0x80 - 0x8f */
	{ "suba",			EA_IMM8,				},
	{ "cmpa",			EA_IMM8,				},
	{ "sbca",			EA_IMM8,				},
	{ "subd",			EA_IMM16,				},
	{ "anda",			EA_IMM8,				},
	{ "bita",			EA_IMM8,				},
	{ "ldaa",			EA_IMM8,				},
	{ "?",				0,						},
	{ "eora",			EA_IMM8,				},
	{ "adca",			EA_IMM8,				},
	{ "oraa",			EA_IMM8,				},
	{ "adda",			EA_IMM8,				},
	{ "cpx",			EA_IMM16,				},
	{ "bsr",			EA_REL,					},
	{ "lds",			EA_IMM16,				},
	{ "xgdx",			0,						},
	/* 0x90 - 0x9f */
	{ "suba",			EA_DIRECT,				},
	{ "cmpa",			EA_DIRECT,				},
	{ "sbca",			EA_DIRECT,				},
	{ "subd",			EA_DIRECT,				},
	{ "anda",			EA_DIRECT,				},
	{ "bita",			EA_DIRECT,				},
	{ "ldaa",			EA_DIRECT,				},
	{ "staa",			EA_DIRECT,				},
	{ "eora",			EA_DIRECT,				},
	{ "adca",			EA_DIRECT,				},
	{ "oraa",			EA_DIRECT,				},
	{ "adda",			EA_DIRECT,				},
	{ "cpx",			EA_DIRECT,				},
	{ "jsr",			EA_DIRECT,				},
	{ "lds",			EA_DIRECT,				},
	{ "sts",			EA_DIRECT,				},
	/* 0xa0 - 0xaf */
	{ "suba",			EA_IND_X,				},
	{ "cmpa",			EA_IND_X,				},
	{ "sbca",			EA_IND_X,				},
	{ "subd",			EA_IND_X,				},
	{ "anda",			EA_IND_X,				},
	{ "bita",			EA_IND_X,				},
	{ "ldaa",			EA_IND_X,				},
	{ "staa",			EA_IND_X,				},
	{ "eora",			EA_IND_X,				},
	{ "adca",			EA_IND_X,				},
	{ "oraa",			EA_IND_X,				},
	{ "adda",			EA_IND_X,				},
	{ "cpx",			EA_IND_X,				},
	{ "jsr",			EA_IND_X,				},
	{ "lds",			EA_IND_X,				},
	{ "sts",			EA_IND_X,				},
	/* 0xb0 - 0xbf */
	{ "suba",			EA_EXT,					},
	{ "cmpa",			EA_EXT,					},
	{ "sbca",			EA_EXT,					},
	{ "subd",			EA_EXT,					},
	{ "anda",			EA_EXT,					},
	{ "bita",			EA_EXT,					},
	{ "ldaa",			EA_EXT,					},
	{ "staa",			EA_EXT,					},
	{ "eora",			EA_EXT,					},
	{ "adca",			EA_EXT,					},
	{ "oraa",			EA_EXT,					},
	{ "adda",			EA_EXT,					},
	{ "cpx",			EA_EXT,					},
	{ "jsr",			EA_EXT,					},
	{ "lds",			EA_EXT,					},
	{ "sts",			EA_EXT,					},
	/* 0xc0 - 0xcf */
	{ "subb",			EA_IMM8,				},
	{ "cmpb",			EA_IMM8,				},
	{ "sbcb",			EA_IMM8,				},
	{ "addd",			EA_IMM16,				},
	{ "andb",			EA_IMM8,				},
	{ "bitb",			EA_IMM8,				},
	{ "ldab",			EA_IMM8,				},
	{ "?",				0,						},
	{ "eorb",			EA_IMM8,				},
	{ "adcb",			EA_IMM8,				},
	{ "orab",			EA_IMM8,				},
	{ "addb",			EA_IMM8,				},
	{ "ldd",			EA_IMM16,				},
	{ "page4",			PAGE4,					},
	{ "ldx",			EA_IMM16,				},
	{ "stop",			0,						},
	/* 0xd0 - 0xdf */
	{ "subb",			EA_DIRECT,				},
	{ "cmpb",			EA_DIRECT,				},
	{ "sbcb",			EA_DIRECT,				},
	{ "addd",			EA_DIRECT,				},
	{ "andb",			EA_DIRECT,				},
	{ "bitb",			EA_DIRECT,				},
	{ "ldab",			EA_DIRECT,				},
	{ "stab",			EA_DIRECT,				},
	{ "eorb",			EA_DIRECT,				},
	{ "adcb",			EA_DIRECT,				},
	{ "orab",			EA_DIRECT,				},
	{ "addb",			EA_DIRECT,				},
	{ "ldd",			EA_DIRECT,				},
	{ "std",			EA_DIRECT,				},
	{ "ldx",			EA_DIRECT,				},
	{ "stx",			EA_DIRECT,				},
	/* 0xe0 - 0xef */
	{ "subb",			EA_IND_X,				},
	{ "cmpb",			EA_IND_X,				},
	{ "sbcb",			EA_IND_X,				},
	{ "addd",			EA_IND_X,				},
	{ "andb",			EA_IND_X,				},
	{ "bitb",			EA_IND_X,				},
	{ "ldab",			EA_IND_X,				},
	{ "stab",			EA_IND_X,				},
	{ "eorb",			EA_IND_X,				},
	{ "adcb",			EA_IND_X,				},
	{ "orab",			EA_IND_X,				},
	{ "addb",			EA_IND_X,				},
	{ "ldd",			EA_IND_X,				},
	{ "std",			EA_IND_X,				},
	{ "ldx",			EA_IND_X,				},
	{ "stx",			EA_IND_X,				},
	/* 0xf0 - 0xff */
	{ "subb",			EA_EXT,					},
	{ "cmpb",			EA_EXT,					},
	{ "sbcb",			EA_EXT,					},
	{ "addd",			EA_EXT,					},
	{ "andb",			EA_EXT,					},
	{ "bitb",			EA_EXT,					},
	{ "ldab",			EA_EXT,					},
	{ "stab",			EA_EXT,					},
	{ "eorb",			EA_EXT,					},
	{ "adcb",			EA_EXT,					},
	{ "orab",			EA_EXT,					},
	{ "addb",			EA_EXT,					},
	{ "ldd",			EA_EXT,					},
	{ "std",			EA_EXT,					},
	{ "ldx",			EA_EXT,					},
	{ "stx",			EA_EXT,					},
};


/*****************************************************************************/


static const M68HC11_OPCODE opcode_table_page2[256] =
{
	/* 0x00 - 0x0f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "iny",			0,						},
	{ "dey",			0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x10 - 0x1f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "bset",			EA_IND_Y_IMM8,			},
	{ "bclr",			EA_IND_Y_IMM8,			},
	{ "brset",			EA_IND_Y_IMM8_REL,		},
	{ "brlcr",			EA_IND_Y_IMM8_REL,		},
	/* 0x20 - 0x2f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x30 - 0x3f */
	{ "tsy",			0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "tys",			0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "puly",			0,						},
	{ "?",				0,						},
	{ "aby",			0,						},
	{ "?",				0,						},
	{ "pshy",			0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x40 - 0x4f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x50 - 0x5f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x60 - 0x6f */
	{ "neg",			EA_IND_Y,				},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "com",			EA_IND_Y,				},
	{ "lsr",			EA_IND_Y,				},
	{ "?",				0,						},
	{ "ror",			EA_IND_Y,				},
	{ "asr",			EA_IND_Y,				},
	{ "asl",			EA_IND_Y,				},
	{ "rol",			EA_IND_Y,				},
	{ "dec",			EA_IND_Y,				},
	{ "?",				0,						},
	{ "inc",			EA_IND_Y,				},
	{ "tst",			EA_IND_Y,				},
	{ "jmp",			EA_IND_Y,				},
	{ "clr",			EA_IND_Y,				},
	/* 0x70 - 0x7f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x80 - 0x8f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "cpy",			EA_IMM16,				},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "xgdy",			0,						},
	/* 0x90 - 0x9f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "cpy",			EA_DIRECT,				},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xa0 - 0xaf */
	{ "suba",			EA_IND_Y,				},
	{ "cmpa",			EA_IND_Y,				},
	{ "sbca",			EA_IND_Y,				},
	{ "subd",			EA_IND_Y,				},
	{ "anda",			EA_IND_Y,				},
	{ "bita",			EA_IND_Y,				},
	{ "ldaa",			EA_IND_Y,				},
	{ "staa",			EA_IND_Y,				},
	{ "eora",			EA_IND_Y,				},
	{ "adca",			EA_IND_Y,				},
	{ "oraa",			EA_IND_Y,				},
	{ "adda",			EA_IND_Y,				},
	{ "cpy",			EA_IND_Y,				},
	{ "jsr",			EA_IND_Y,				},
	{ "lds",			EA_IND_Y,				},
	{ "sts",			EA_IND_Y,				},
	/* 0xb0 - 0xbf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "cpy",			EA_EXT,					},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xc0 - 0xcf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "ldy",			EA_IMM16,				},
	{ "?",				0,						},
	/* 0xd0 - 0xdf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "ldy",			EA_DIRECT,				},
	{ "sty",			EA_DIRECT,				},
	/* 0xe0 - 0xef */
	{ "subb",			EA_IND_Y,				},
	{ "cmpb",			EA_IND_Y,				},
	{ "sbcb",			EA_IND_Y,				},
	{ "addd",			EA_IND_Y,				},
	{ "andb",			EA_IND_Y,				},
	{ "bitb",			EA_IND_Y,				},
	{ "ldab",			EA_IND_Y,				},
	{ "stab",			EA_IND_Y,				},
	{ "eorb",			EA_IND_Y,				},
	{ "adcb",			EA_IND_Y,				},
	{ "orab",			EA_IND_Y,				},
	{ "addb",			EA_IND_Y,				},
	{ "ldd",			EA_IND_Y,				},
	{ "std",			EA_IND_Y,				},
	{ "ldy",			EA_IND_Y,				},
	{ "sty",			EA_IND_Y,				},
	/* 0xf0 - 0xff */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "ldy",			EA_EXT,					},
	{ "sty",			EA_EXT,					},
};


/*****************************************************************************/


static const M68HC11_OPCODE opcode_table_page3[256] =
{
	/* 0x00 - 0x0f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x10 - 0x1f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x20 - 0x2f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x30 - 0x3f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x40 - 0x4f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x50 - 0x5f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x60 - 0x6f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x70 - 0x7f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x80 - 0x8f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "cpd",			EA_IMM16,				},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x90 - 0x9f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "cpd",			EA_DIRECT,				},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xa0 - 0xaf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "cpd",			EA_IND_X,				},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "cpy",			EA_IND_X,				},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xb0 - 0xbf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "cpd",			EA_EXT,					},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xc0 - 0xcf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xd0 - 0xdf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xe0 - 0xef */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "ldy",			EA_IND_X,				},
	{ "sty",			EA_IND_X,				},
	/* 0xf0 - 0xff */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
};


/*****************************************************************************/


static const M68HC11_OPCODE opcode_table_page4[256] =
{
	/* 0x00 - 0x0f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x10 - 0x1f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x20 - 0x2f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x30 - 0x3f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x40 - 0x4f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x50 - 0x5f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x60 - 0x6f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x70 - 0x7f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x80 - 0x8f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0x90 - 0x9f */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xa0 - 0xaf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "cpd",			EA_IND_Y,				},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "cpx",			EA_IND_Y,				},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xb0 - 0xbf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xc0 - 0xcf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xd0 - 0xdf */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	/* 0xe0 - 0xef */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "ldx",			EA_IND_Y,				},
	{ "stx",			EA_IND_Y,				},
	/* 0xf0 - 0xff */
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
	{ "?",				0,						},
};


/*****************************************************************************/

static char *output;
static const UINT8 *rombase;

static void ATTR_PRINTF(1,2) print(const char *fmt, ...)
{
	va_list vl;

	va_start(vl, fmt);
	output += vsprintf(output, fmt, vl);
	va_end(vl);
}

static UINT8 fetch(void)
{
	return *rombase++;
}

static UINT16 fetch16(void)
{
	UINT16 w;
	w = (rombase[0] << 8) | rombase[1];
	rombase+=2;
	return w;
}

static UINT32 decode_opcode(UINT32 pc, const M68HC11_OPCODE *op_table)
{
	UINT8 imm8, mask;
	INT8 rel8;
	UINT16 imm16;
	UINT8 op2;
	UINT32 flags = 0;

	if (!strcmp(op_table->mnemonic, "jsr") || !strcmp(op_table->mnemonic, "bsr"))
		flags = DASMFLAG_STEP_OVER;
	else if (!strcmp(op_table->mnemonic, "rts") || !strcmp(op_table->mnemonic, "rti"))
		flags = DASMFLAG_STEP_OUT;

	switch(op_table->address_mode)
	{
		case EA_IMM8:
			imm8 = fetch();
			print("%s 0x%02X", op_table->mnemonic, imm8);
			break;

		case EA_IMM16:
			imm16 = fetch16();
			print("%s 0x%04X", op_table->mnemonic, imm16);
			break;

		case EA_DIRECT:
			imm8 = fetch();
			print("%s (0x%04X)", op_table->mnemonic, imm8);
			break;

		case EA_EXT:
			imm16 = fetch16();
			print("%s (0x%04X)", op_table->mnemonic, imm16);
			break;

		case EA_IND_X:
			imm8 = fetch();
			print("%s (X+0x%02X)", op_table->mnemonic, imm8);
			break;

		case EA_REL:
			rel8 = fetch();
			print("%s [0x%04X]", op_table->mnemonic, pc+2+rel8);
			break;

		case EA_DIRECT_IMM8:
			imm8 = fetch();
			mask = fetch();
			print("%s (0x%04X), 0x%02X", op_table->mnemonic, imm8, mask);
			break;

		case EA_IND_X_IMM8:
			imm8 = fetch();
			mask = fetch();
			print("%s (X+0x%02X), 0x%02X", op_table->mnemonic, imm8, mask);
			break;

		case EA_DIRECT_IMM8_REL:
			imm8 = fetch();
			mask = fetch();
			rel8 = fetch();
			print("%s (0x%04X), 0x%02X, [0x%04X]", op_table->mnemonic, imm8, mask, pc+4+rel8);
			break;

		case EA_IND_X_IMM8_REL:
			imm8 = fetch();
			mask = fetch();
			rel8 = fetch();
			print("%s (X+0x%02X), 0x%02X, [0x%04X]", op_table->mnemonic, imm8, mask, pc+4+rel8);
			break;

		case EA_IND_Y:
			imm8 = fetch();
			print("%s (Y+0x%02X)", op_table->mnemonic, imm8);
			break;

		case EA_IND_Y_IMM8:
			imm8 = fetch();
			mask = fetch();
			print("%s (Y+0x%02X), 0x%02X", op_table->mnemonic, imm8, mask);
			break;

		case EA_IND_Y_IMM8_REL:
			imm8 = fetch();
			mask = fetch();
			rel8 = fetch();
			print("%s (Y+0x%02X), 0x%02X, [0x%04X]", op_table->mnemonic, imm8, mask, pc+2+rel8);
			break;

		case PAGE2:
			op2 = fetch();
			return decode_opcode(pc, &opcode_table_page2[op2]);

		case PAGE3:
			op2 = fetch();
			return decode_opcode(pc, &opcode_table_page3[op2]);

		case PAGE4:
			op2 = fetch();
			return decode_opcode(pc, &opcode_table_page4[op2]);

		default:
			print("%s", op_table->mnemonic);
	}
	return flags;
}

CPU_DISASSEMBLE( hc11 )
{
	UINT32 flags = 0;
	UINT8 opcode;

	output = buffer;
	rombase = oprom;

	opcode = fetch();
	flags = decode_opcode(pc, &opcode_table[opcode]);

	return (rombase-oprom) | flags | DASMFLAG_SUPPORTED;
}
