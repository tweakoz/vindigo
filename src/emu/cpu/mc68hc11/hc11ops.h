typedef struct _hc11_opcode_list_struct hc11_opcode_list_struct;
struct _hc11_opcode_list_struct
{
	int page;
	int opcode;
	void (*handler)(hc11_state *cpustate);
};

static const hc11_opcode_list_struct hc11_opcode_list[] =
{
	/*  page    opcode          handler                     */
	{	0,		0x1b,			HC11OP(aba)					},
	{	0,		0x3a,			HC11OP(abx)					},
	{	0x18,	0x3a,			HC11OP(aby)					},
	{	0,		0x89,			HC11OP(adca_imm)			},
	{	0,		0x99,			HC11OP(adca_dir)			},
	{	0,		0xb9,			HC11OP(adca_ext)			},
	{	0,		0xa9,			HC11OP(adca_indx)			},
	{	0x18,	0xa9,			HC11OP(adca_indy)			},
	{	0,		0xc9,			HC11OP(adcb_imm)			},
	{	0,		0xd9,			HC11OP(adcb_dir)			},
	{	0,		0xf9,			HC11OP(adcb_ext)			},
	{	0,		0xe9,			HC11OP(adcb_indx)			},
	{	0x18,	0xe9,			HC11OP(adcb_indy)			},
	{	0,		0x8b,			HC11OP(adda_imm)			},
	{	0,		0x9b,			HC11OP(adda_dir)			},
	{	0,		0xbb,			HC11OP(adda_ext)			},
	{	0,		0xab,			HC11OP(adda_indx)			},
	{	0x18,	0xab,			HC11OP(adda_indy)			},
	{	0,		0xcb,			HC11OP(addb_imm)			},
	{	0,		0xdb,			HC11OP(addb_dir)			},
	{	0,		0xfb,			HC11OP(addb_ext)			},
	{	0,		0xeb,			HC11OP(addb_indx)			},
	{	0x18,	0xeb,			HC11OP(addb_indy)			},
	{	0,		0xc3,			HC11OP(addd_imm)			},
	{	0,		0xd3,			HC11OP(addd_dir)			},
	{	0,		0xf3,			HC11OP(addd_ext)			},
	{	0,		0xe3,			HC11OP(addd_indx)			},
	{	0x18,	0xe3,			HC11OP(addd_indy)			},
	{	0,		0x84,			HC11OP(anda_imm)			},
	{	0,		0x94,			HC11OP(anda_dir)			},
	{	0,		0xb4,			HC11OP(anda_ext)			},
	{	0,		0xa4,			HC11OP(anda_indx)			},
	{	0x18,	0xa4,			HC11OP(anda_indy)			},
	{	0,		0xc4,			HC11OP(andb_imm)			},
	{	0,		0xd4,			HC11OP(andb_dir)			},
	{	0,		0xf4,			HC11OP(andb_ext)			},
	{	0,		0xe4,			HC11OP(andb_indx)			},
	{	0x18,	0xe4,			HC11OP(andb_indy)			},
	{	0,		0x48,			HC11OP(asla)				},
	{	0,		0x58,			HC11OP(aslb)				},
//  {   0,      0x78,           HC11OP(asl_ext)             },
//  {   0,      0x68,           HC11OP(asl_indx)            },
//  {   0x18,   0x68,           HC11OP(asl_indy)            },
//  {   0,      0x47,           HC11OP(asra)                },
//  {   0,      0x57,           HC11OP(asrb)                },
//  {   0,      0x77,           HC11OP(asr_ext)             },
//  {   0,      0x67,           HC11OP(asr_indx)            },
//  {   0x18,   0x67,           HC11OP(asr_indy)            },
	{	0,		0x24,			HC11OP(bcc)					},
//  {   0,      0x15,           HC11OP(bclr_dir)            },
	{	0,		0x1d,			HC11OP(bclr_indx)			},
//  {   0x18,   0x1d,           HC11OP(bclr_indy)           },
	{	0,		0x25,			HC11OP(bcs)					},
	{	0,		0x27,			HC11OP(beq)					},
//  {   0,      0x2c,           HC11OP(bge)                 },
//  {   0,      0x2e,           HC11OP(bgt)                 },
	{	0,		0x22,			HC11OP(bhi)					},
	{	0,		0x85,			HC11OP(bita_imm)			},
	{	0,		0x95,			HC11OP(bita_dir)			},
	{	0,		0xb5,			HC11OP(bita_ext)			},
	{	0,		0xa5,			HC11OP(bita_indx)			},
	{	0x18,	0xa5,			HC11OP(bita_indy)			},
	{	0,		0xc5,			HC11OP(bitb_imm)			},
	{	0,		0xd5,			HC11OP(bitb_dir)			},
	{	0,		0xf5,			HC11OP(bitb_ext)			},
	{	0,		0xe5,			HC11OP(bitb_indx)			},
	{	0x18,	0xe5,			HC11OP(bitb_indy)			},
	{	0,		0x2f,			HC11OP(ble)					},
	{	0,		0x23,			HC11OP(bls)					},
//  {   0,      0x2d,           HC11OP(blt)                 },
	{	0,		0x2b,			HC11OP(bmi)					},
	{	0,		0x26,			HC11OP(bne)					},
	{	0,		0x2a,			HC11OP(bpl)					},
	{	0,		0x20,			HC11OP(bra)					},
	{	0,		0x13,			HC11OP(brclr_dir)			},
	{	0,		0x1f,			HC11OP(brclr_indx)			},
//  {   0x18,   0x1f,           HC11OP(brclr_indy)          },
	{	0,		0x21,			HC11OP(brn)					},
	{	0,		0x12,			HC11OP(brset_dir)			},
	{	0,		0x1e,			HC11OP(brset_indx)			},
//  {   0x18,   0x1e,           HC11OP(brset_indy)          },
//  {   0,      0x14,           HC11OP(bset_dir)            },
	{	0,		0x1c,			HC11OP(bset_indx)			},
//  {   0x18,   0x1c,           HC11OP(bset_indy)           },
	{	0,		0x8d,			HC11OP(bsr)					},
	{	0,		0x28,			HC11OP(bvc)					},
	{	0,		0x29,			HC11OP(bvs)					},
	{	0,		0x11,			HC11OP(cba)					},
	{	0,		0x0c,			HC11OP(clc)					},
	{	0,		0x0e,			HC11OP(cli)					},
	{	0,		0x4f,			HC11OP(clra)				},
	{	0,		0x5f,			HC11OP(clrb)				},
	{	0,		0x7f,			HC11OP(clr_ext)				},
	{	0,		0x6f,			HC11OP(clr_indx)			},
	{	0x18,	0x6f,			HC11OP(clr_indy)			},
	{	0,		0x0a,			HC11OP(clv)					},
	{	0,		0x81,			HC11OP(cmpa_imm)			},
	{	0,		0x91,			HC11OP(cmpa_dir)			},
	{	0,		0xb1,			HC11OP(cmpa_ext)			},
	{	0,		0xa1,			HC11OP(cmpa_indx)			},
	{	0x18,	0xa1,			HC11OP(cmpa_indy)			},
	{	0,		0xc1,			HC11OP(cmpb_imm)			},
	{	0,		0xd1,			HC11OP(cmpb_dir)			},
	{	0,		0xf1,			HC11OP(cmpb_ext)			},
	{	0,		0xe1,			HC11OP(cmpb_indx)			},
	{	0x18,	0xe1,			HC11OP(cmpb_indy)			},
	{	0,		0x43,			HC11OP(coma)				},
	{	0,		0x53,			HC11OP(comb)				},
//  {   0,      0x73,           HC11OP(com_ext)             },
//  {   0,      0x63,           HC11OP(com_indx)            },
//  {   0x18,   0x63,           HC11OP(com_indy)            },
	{	0x1a,	0x83,			HC11OP(cpd_imm)				},
	{	0x1a,	0x93,			HC11OP(cpd_dir)				},
	{	0x1a,	0xb3,			HC11OP(cpd_ext)				},
	{	0x1a,	0xa3,			HC11OP(cpd_indx)			},
	{	0xcd,	0xa3,			HC11OP(cpd_indy)			},
	{	0,		0x8c,			HC11OP(cpx_imm)				},
	{	0,		0x9c,			HC11OP(cpx_dir)				},
	{	0,		0xbc,			HC11OP(cpx_ext)				},
	{	0,		0xac,			HC11OP(cpx_indx)			},
	{	0xcd,	0xac,			HC11OP(cpx_indy)			},
	{	0x18,	0x8c,			HC11OP(cpy_imm)				},
	{	0x18,	0x9c,			HC11OP(cpy_dir)				},
	{	0x18,	0xbc,			HC11OP(cpy_ext)				},
	{	0x1a,	0xac,			HC11OP(cpy_indx)			},
	{	0x18,	0xac,			HC11OP(cpy_indy)			},
//  {   0,      0x19,           HC11OP(daa)                 },
	{	0,		0x4a,			HC11OP(deca)				},
	{	0,		0x5a,			HC11OP(decb)				},
	{	0,		0x7a,			HC11OP(dec_ext)				},
	{	0,		0x6a,			HC11OP(dec_indx)			},
	{	0x18,	0x6a,			HC11OP(dec_indy)			},
//  {   0,      0x34,           HC11OP(des)                 },
	{	0,		0x09,			HC11OP(dex)					},
	{	0x18,	0x09,			HC11OP(dey)					},
	{	0,		0x88,			HC11OP(eora_imm)			},
	{	0,		0x98,			HC11OP(eora_dir)			},
	{	0,		0xb8,			HC11OP(eora_ext)			},
	{	0,		0xa8,			HC11OP(eora_indx)			},
	{	0x18,	0xa8,			HC11OP(eora_indy)			},
	{	0,		0xc8,			HC11OP(eorb_imm)			},
	{	0,		0xd8,			HC11OP(eorb_dir)			},
	{	0,		0xf8,			HC11OP(eorb_ext)			},
	{	0,		0xe8,			HC11OP(eorb_indx)			},
	{	0x18,	0xe8,			HC11OP(eorb_indy)			},
//  {   0,      0x03,           HC11OP(fdiv)                },
	{	0,		0x02,			HC11OP(idiv)				},
	{	0,		0x4c,			HC11OP(inca)				},
	{	0,		0x5c,			HC11OP(incb)				},
	{	0,		0x7c,			HC11OP(inc_ext)				},
	{	0,		0x6c,			HC11OP(inc_indx)			},
	{	0x18,	0x6c,			HC11OP(inc_indy)			},
//  {   0,      0x31,           HC11OP(ins)                 },
	{	0,		0x08,			HC11OP(inx)					},
	{	0x18,	0x08,			HC11OP(iny)					},
	{	0,		0x7e,			HC11OP(jmp_ext)				},
	{	0,		0x6e,			HC11OP(jmp_indx)			},
	{	0x18,	0x6e,			HC11OP(jmp_indy)			},
	{	0,		0x9d,			HC11OP(jsr_dir)				},
	{	0,		0xbd,			HC11OP(jsr_ext)				},
	{	0,		0xad,			HC11OP(jsr_indx)			},
	{	0x18,	0xad,			HC11OP(jsr_indy)			},
	{	0,		0x86,			HC11OP(ldaa_imm)			},
	{	0,		0x96,			HC11OP(ldaa_dir)			},
	{	0,		0xb6,			HC11OP(ldaa_ext)			},
	{	0,		0xa6,			HC11OP(ldaa_indx)			},
	{	0x18,	0xa6,			HC11OP(ldaa_indy)			},
	{	0,		0xc6,			HC11OP(ldab_imm)			},
	{	0,		0xd6,			HC11OP(ldab_dir)			},
	{	0,		0xf6,			HC11OP(ldab_ext)			},
	{	0,		0xe6,			HC11OP(ldab_indx)			},
	{	0x18,	0xe6,			HC11OP(ldab_indy)			},
	{	0,		0xcc,			HC11OP(ldd_imm)				},
	{	0,		0xdc,			HC11OP(ldd_dir)				},
	{	0,		0xfc,			HC11OP(ldd_ext)				},
	{	0,		0xec,			HC11OP(ldd_indx)			},
	{	0x18,	0xec,			HC11OP(ldd_indy)			},
	{	0,		0x8e,			HC11OP(lds_imm)				},
	{	0,		0x9e,			HC11OP(lds_dir)				},
	{	0,		0xbe,			HC11OP(lds_ext)				},
	{	0,		0xae,			HC11OP(lds_indx)			},
	{	0x18,	0xae,			HC11OP(lds_indy)			},
	{	0,		0xce,			HC11OP(ldx_imm)				},
	{	0,		0xde,			HC11OP(ldx_dir)				},
	{	0,		0xfe,			HC11OP(ldx_ext)				},
	{	0,		0xee,			HC11OP(ldx_indx)			},
	{	0xcd,	0xee,			HC11OP(ldx_indy)			},
	{	0x18,	0xce,			HC11OP(ldy_imm)				},
	{	0x18,	0xde,			HC11OP(ldy_dir)				},
	{	0x18,	0xfe,			HC11OP(ldy_ext)				},
	{	0x1a,	0xee,			HC11OP(ldy_indx)			},
	{	0x18,	0xee,			HC11OP(ldy_indy)			},
	{	0,		0x05,			HC11OP(lsld)				},
	{	0,		0x44,			HC11OP(lsra)				},
	{	0,		0x54,			HC11OP(lsrb)				},
//  {   0,      0x74,           HC11OP(lsr_ext)             },
//  {   0,      0x64,           HC11OP(lsr_indx)            },
//  {   0x18,   0x64,           HC11OP(lsr_indy)            },
	{	0,		0x04,			HC11OP(lsrd)				},
	{	0,		0x3d,			HC11OP(mul)					},
	{	0,		0x40,			HC11OP(nega)				},
	{	0,		0x50,			HC11OP(negb)				},
	{	0,		0x70,			HC11OP(neg_ext)				},
	{	0,		0x60,			HC11OP(neg_indx)			},
	{	0x18,	0x60,			HC11OP(neg_indy)			},
	{	0,		0x01,			HC11OP(nop)					},
	{	0,		0x8a,			HC11OP(oraa_imm)			},
	{	0,		0x9a,			HC11OP(oraa_dir)			},
	{	0,		0xba,			HC11OP(oraa_ext)			},
	{	0,		0xaa,			HC11OP(oraa_indx)			},
	{	0x18,	0xaa,			HC11OP(oraa_indy)			},
	{	0,		0xca,			HC11OP(orab_imm)			},
	{	0,		0xda,			HC11OP(orab_dir)			},
	{	0,		0xfa,			HC11OP(orab_ext)			},
	{	0,		0xea,			HC11OP(orab_indx)			},
	{	0x18,	0xea,			HC11OP(orab_indy)			},
	{	0,		0x36,			HC11OP(psha)				},
	{	0,		0x37,			HC11OP(pshb)				},
	{	0,		0x3c,			HC11OP(pshx)				},
	{	0x18,	0x3c,			HC11OP(pshy)				},
	{	0,		0x32,			HC11OP(pula)				},
	{	0,		0x33,			HC11OP(pulb)				},
	{	0,		0x38,			HC11OP(pulx)				},
	{	0x18,	0x38,			HC11OP(puly)				},
	{	0,		0x49,			HC11OP(rola)				},
	{	0,		0x59,			HC11OP(rolb)				},
//  {   0,      0x79,           HC11OP(rol_ext)             },
//  {   0,      0x69,           HC11OP(rol_indx)            },
//  {   0x18,   0x69,           HC11OP(rol_indy)            },
	{	0,		0x46,			HC11OP(rora)				},
	{	0,		0x56,			HC11OP(rorb)				},
//  {   0,      0x76,           HC11OP(ror_ext)             },
//  {   0,      0x66,           HC11OP(ror_indx)            },
//  {   0x18,   0x66,           HC11OP(ror_indy)            },
	{	0,		0x3b,			HC11OP(rti)					},
	{	0,		0x39,			HC11OP(rts)					},
	{	0,		0x10,			HC11OP(sba)					},
	{	0,		0x82,			HC11OP(sbca_imm)			},
//  {   0,      0x92,           HC11OP(sbca_dir)            },
//  {   0,      0xb2,           HC11OP(sbca_ext)            },
	{	0,		0xa2,			HC11OP(sbca_indx)			},
	{	0x18,	0xa2,			HC11OP(sbca_indy)			},
	{	0,		0xc2,			HC11OP(sbcb_imm)			},
//  {   0,      0xd2,           HC11OP(sbcb_dir)            },
//  {   0,      0xf2,           HC11OP(sbcb_ext)            },
	{	0,		0xe2,			HC11OP(sbcb_indx)			},
	{	0x18,	0xe2,			HC11OP(sbcb_indy)			},
	{	0,		0x0d,			HC11OP(sec)					},
	{	0,		0x0f,			HC11OP(sei)					},
	{	0,		0x0b,			HC11OP(sev)					},
	{	0,		0x97,			HC11OP(staa_dir)			},
	{	0,		0xb7,			HC11OP(staa_ext)			},
	{	0,		0xa7,			HC11OP(staa_indx)			},
	{	0x18,	0xa7,			HC11OP(staa_indy)			},
	{	0,		0xd7,			HC11OP(stab_dir)			},
	{	0,		0xf7,			HC11OP(stab_ext)			},
	{	0,		0xe7,			HC11OP(stab_indx)			},
	{	0x18,	0xe7,			HC11OP(stab_indy)			},
	{	0,		0xdd,			HC11OP(std_dir)				},
	{	0,		0xfd,			HC11OP(std_ext)				},
	{	0,		0xed,			HC11OP(std_indx)			},
	{	0x18,	0xed,			HC11OP(std_indy)			},
//  {   0,      0x9f,           HC11OP(sts_dir)             },
//  {   0,      0xbf,           HC11OP(sts_ext)             },
//  {   0,      0xaf,           HC11OP(sts_indx)            },
//  {   0x18,   0xaf,           HC11OP(sts_indy)            },
	{   0,      0xdf,           HC11OP(stx_dir)             },
	{	0,		0xff,			HC11OP(stx_ext)				},
	{	0,		0xef,			HC11OP(stx_indx)			},
	{	0xcd,	0xef,			HC11OP(stx_indy)			},
	{   0x18,   0xdf,           HC11OP(sty_dir)             },
	{	0x18,	0xff,			HC11OP(sty_ext)				},
	{	0x1a,	0xef,			HC11OP(sty_indx)			},
	{	0x18,	0xef,			HC11OP(sty_indy)			},
	{	0,		0xcf,			HC11OP(stop)				},
	{	0,		0x80,			HC11OP(suba_imm)			},
	{	0,		0x90,			HC11OP(suba_dir)			},
	{	0,		0xb0,			HC11OP(suba_ext)			},
	{	0,		0xa0,			HC11OP(suba_indx)			},
	{	0x18,	0xa0,			HC11OP(suba_indy)			},
	{	0,		0xc0,			HC11OP(subb_imm)			},
	{	0,		0xd0,			HC11OP(subb_dir)			},
	{	0,		0xf0,			HC11OP(subb_ext)			},
	{	0,		0xe0,			HC11OP(subb_indx)			},
	{	0x18,	0xe0,			HC11OP(subb_indy)			},
	{	0,		0x83,			HC11OP(subd_imm)			},
//  {   0,      0x93,           HC11OP(subd_dir)            },
//  {   0,      0xb3,           HC11OP(subd_ext)            },
	{	0,		0xa3,			HC11OP(subd_indx)			},
	{	0x18,	0xa3,			HC11OP(subd_indy)			},
	{	0,		0x3f,			HC11OP(swi)					},
	{	0,		0x16,			HC11OP(tab)					},
	{	0,		0x06,			HC11OP(tap)					},
	{	0,		0x17,			HC11OP(tba)					},
	{	0,		0x00,			HC11OP(test)				},
	{	0,		0x07,			HC11OP(tpa)					},
	{	0,		0x4d,			HC11OP(tsta)				},
	{	0,		0x5d,			HC11OP(tstb)				},
	{	0,		0x7d,			HC11OP(tst_ext)				},
	{	0,		0x6d,			HC11OP(tst_indx)			},
	{	0x18,	0x6d,			HC11OP(tst_indy)			},
	{	0,		0x30,			HC11OP(tsx)					},
	{	0x18,	0x30,			HC11OP(tsy)					},
	{	0,		0x35,			HC11OP(txs)					},
	{	0x18,	0x35,			HC11OP(tys)					},
	{	0,		0x3e,			HC11OP(wai)					},
	{	0,		0x8f,			HC11OP(xgdx)				},
	{	0x18,	0x8f,			HC11OP(xgdy)				},

	{	0,		0x18,			HC11OP(page2)				},
	{	0,		0x1a,			HC11OP(page3)				},
	{	0,		0xcd,			HC11OP(page4)				},
};
