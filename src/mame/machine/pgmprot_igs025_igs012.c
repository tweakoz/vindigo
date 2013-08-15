/***********************************************************************
 PGM 012 + 025 PGM protection emulation

 these are simulations of the IGS 012 and 025 protection combination
 used on the following PGM games

 Dragon World 2

 ----

 IGS012 provides ROM overlay???

 IGS025 is some kind of state machine / logic device which the game
 uses for various security checks bitswap checks.

 ***********************************************************************/

#include "emu.h"
#include "includes/pgm.h"

/* Dragon World 2 */

void pgm_012_025_state::pgm_drgw2_decrypt()
{
	int i;
	UINT16 *src = (UINT16 *) (memregion("maincpu")->base()+0x100000);

	int rom_size = 0x80000;

	for (i = 0; i < rom_size / 2; i++)
	{
		UINT16 x = src[i];

		if (((i & 0x20890) == 0) || ((i & 0x20000) == 0x20000 && (i & 0x01500) != 0x01400))
			x ^= 0x0002;

		if (((i & 0x20400) == 0 && (i & 0x02010) != 0x02010) || ((i & 0x20000) == 0x20000 && (i & 0x00148) != 0x00140))
			x ^= 0x0400;

		src[i] = x;
	}
}

// All tables all xored by 'warning' information at $1354ee (drgw2)
// tables are the same as drgw3 and olds
static const UINT8 drgw2_source_data[0x08][0xec] =
{
	{ 0, }, // Region 0, not used
	{   // Region 1, $13A886
		0x67, 0x51, 0xF3, 0x19, 0xA0, 0x11, 0xB1, 0x11, 0xB0, 0xEE, 0xE3, 0xF6, 0xBE, 0x81, 0x35, 0xE3,
		0xFB, 0xE6, 0xEF, 0xDF, 0x61, 0x01, 0xFA, 0x22, 0x5D, 0x43, 0x01, 0xA5, 0x3B, 0x17, 0xD4, 0x74,
		0xF0, 0xF4, 0xF3, 0x43, 0xB5, 0x19, 0x04, 0xD5, 0x84, 0xCE, 0x87, 0xFE, 0x35, 0x3E, 0xC4, 0x3C,
		0xC7, 0x85, 0x2A, 0x33, 0x00, 0x86, 0xD0, 0x4D, 0x65, 0x4B, 0xF9, 0xE9, 0xC0, 0xBA, 0xAA, 0x77,
		0x9E, 0x66, 0xF6, 0x0F, 0x4F, 0x3A, 0xB6, 0xF1, 0x64, 0x9A, 0xE9, 0x25, 0x1A, 0x5F, 0x22, 0xA3,
		0xA2, 0xBF, 0x4B, 0x77, 0x3F, 0x34, 0xC9, 0x6E, 0xDB, 0x12, 0x5C, 0x33, 0xA5, 0x8B, 0x6C, 0xB1,
		0x74, 0xC8, 0x40, 0x4E, 0x2F, 0xE7, 0x46, 0xAE, 0x99, 0xFC, 0xB0, 0x55, 0x54, 0xDF, 0xA7, 0xA1,
		0x0F, 0x5E, 0x49, 0xCF, 0x56, 0x3C, 0x90, 0x2B, 0xAC, 0x65, 0x6E, 0xDB, 0x58, 0x3E, 0xC9, 0x00,
		0xAE, 0x53, 0x4D, 0x92, 0xFA, 0x40, 0xB2, 0x6B, 0x65, 0x4B, 0x90, 0x8A, 0x0C, 0xE2, 0xA5, 0x9A,
		0xD0, 0x20, 0x29, 0x55, 0xA4, 0x44, 0xAC, 0x51, 0x87, 0x54, 0x53, 0x34, 0x24, 0x4B, 0x81, 0x67,
		0x34, 0x4C, 0x5F, 0x31, 0x4E, 0xF2, 0xF1, 0x19, 0x18, 0x1C, 0x34, 0x38, 0xE1, 0x81, 0x17, 0xCF,
		0x24, 0xB9, 0x9A, 0xCB, 0x34, 0x51, 0x50, 0x59, 0x44, 0xB1, 0x0B, 0x50, 0x95, 0x6C, 0x48, 0x7E,
		0x14, 0xA4, 0xC6, 0xD9, 0xD3, 0xA5, 0xD6, 0xD0, 0xC5, 0x97, 0xF0, 0x45, 0xD0, 0x98, 0x51, 0x91,
		0x9F, 0xA3, 0x43, 0x51, 0x05, 0x90, 0xEE, 0xCA, 0x7E, 0x5F, 0x72, 0x53, 0xB1, 0xD3, 0xAF, 0x36,
		0x08, 0x75, 0xB0, 0x9B, 0xE0, 0x0D, 0x43, 0x88, 0xAA, 0x27, 0x44, 0x11
	},
	{ 0, }, // Region 2, not used
	{ 0, }, // Region 3, not used
	{ 0, }, // Region 4, not used
	{   // Region 5, $13ab42 (drgw2c)
		0x7F, 0x41, 0xF3, 0x39, 0xA0, 0x11, 0xA1, 0x11, 0xB0, 0xA2, 0x4C, 0x23, 0x13, 0xE9, 0x25, 0x3D,
		0x0F, 0x72, 0x3A, 0x9D, 0xB5, 0x96, 0xD1, 0xDA, 0x07, 0x29, 0x41, 0x9A, 0xAD, 0x70, 0xBA, 0x46,
		0x63, 0x2B, 0x7F, 0x3D, 0xBE, 0x40, 0xAD, 0xD4, 0x4C, 0x73, 0x27, 0x58, 0xA7, 0x65, 0xDC, 0xD6,
		0xFD, 0xDE, 0xB5, 0x6E, 0xD6, 0x6C, 0x75, 0x1A, 0x32, 0x45, 0xD5, 0xE3, 0x6A, 0x14, 0x6D, 0x80,
		0x84, 0x15, 0xAF, 0xCC, 0x7B, 0x61, 0x51, 0x82, 0x40, 0x53, 0x7F, 0x38, 0xA0, 0xD6, 0x8F, 0x61,
		0x79, 0x19, 0xE5, 0x99, 0x84, 0xD8, 0x78, 0x27, 0x3F, 0x16, 0x97, 0x78, 0x4F, 0x7B, 0x0C, 0xA6,
		0x37, 0xDB, 0xC6, 0x0C, 0x24, 0xB4, 0xC7, 0x94, 0x9D, 0x92, 0xD2, 0x3B, 0xD5, 0x11, 0x6F, 0x0A,
		0xDB, 0x76, 0x66, 0xE7, 0xCD, 0x18, 0x2B, 0x66, 0xD8, 0x41, 0x40, 0x58, 0xA2, 0x01, 0x1E, 0x6D,
		0x44, 0x75, 0xE7, 0x19, 0x4F, 0xB2, 0xE8, 0xC4, 0x96, 0x77, 0x62, 0x02, 0xC9, 0xDC, 0x59, 0xF3,
		0x43, 0x8D, 0xC8, 0xFE, 0x9E, 0x2A, 0xBA, 0x32, 0x3B, 0x62, 0xE3, 0x92, 0x6E, 0xC2, 0x08, 0x4D,
		0x51, 0xCD, 0xF9, 0x3A, 0x3E, 0xC9, 0x50, 0x27, 0x21, 0x25, 0x97, 0xD7, 0x0E, 0xF8, 0x39, 0x38,
		0xF5, 0x86, 0x94, 0x93, 0xBF, 0xEB, 0x18, 0xA8, 0xFC, 0x24, 0xF5, 0xF9, 0x99, 0x20, 0x3D, 0xCD,
		0x2C, 0x94, 0x25, 0x79, 0x28, 0x77, 0x8F, 0x2F, 0x10, 0x69, 0x86, 0x30, 0x43, 0x01, 0xD7, 0x9A,
		0x17, 0xE3, 0x47, 0x37, 0xBD, 0x62, 0x75, 0x42, 0x78, 0xF4, 0x2B, 0x57, 0x4C, 0x0A, 0xDB, 0x53,
		0x4D, 0xA1, 0x0A, 0xD6, 0x3A, 0x16, 0x15, 0xAA, 0x2C, 0x6C, 0x39, 0x42
	},
	{   // Region 6, $13ab42 (drgw2), $13ab2e (dw2v100x)
		0x12, 0x09, 0xF3, 0x29, 0xA0, 0x11, 0xA0, 0x11, 0xB0, 0xD5, 0x66, 0xA1, 0x28, 0x4A, 0x21, 0xC0,
		0xD3, 0x9B, 0x86, 0x80, 0x57, 0x6F, 0x41, 0xC2, 0xE4, 0x2F, 0x0B, 0x91, 0xBD, 0x3A, 0x7A, 0xBA,
		0x00, 0xE5, 0x35, 0x02, 0x74, 0x7D, 0x8B, 0x21, 0x57, 0x10, 0x0F, 0xAE, 0x44, 0xBB, 0xE2, 0x37,
		0x18, 0x7B, 0x52, 0x3D, 0x8C, 0x59, 0x9E, 0x20, 0x1F, 0x0A, 0xCC, 0x1C, 0x8E, 0x6A, 0xD7, 0x95,
		0x2B, 0x34, 0xB0, 0x82, 0x6D, 0xFD, 0x25, 0x33, 0xAA, 0x3B, 0x2B, 0x70, 0x15, 0x87, 0x31, 0x5D,
		0xBB, 0x29, 0x19, 0x95, 0xD5, 0x8E, 0x24, 0x28, 0x5E, 0xD0, 0x20, 0x83, 0x46, 0x4A, 0x21, 0x70,
		0x5B, 0xCD, 0xAE, 0x7B, 0x61, 0xA1, 0xFA, 0xF4, 0x2B, 0x84, 0x15, 0x6E, 0x36, 0x5D, 0x1B, 0x24,
		0x0F, 0x09, 0x3A, 0x61, 0x38, 0x0F, 0x18, 0x35, 0x11, 0x38, 0xB4, 0xBD, 0xEE, 0xF7, 0xEC, 0x0F,
		0x1D, 0xB7, 0x48, 0x01, 0xAA, 0x09, 0x8F, 0x61, 0xB5, 0x0F, 0x1D, 0x26, 0x39, 0x2E, 0x8C, 0xD6,
		0x26, 0x5C, 0x3D, 0x23, 0x63, 0xE9, 0x6B, 0x97, 0xB4, 0x9F, 0x7B, 0xB6, 0xBA, 0xA0, 0x7C, 0xC6,
		0x25, 0xA1, 0x73, 0x36, 0x67, 0x7F, 0x74, 0x1E, 0x1D, 0xDA, 0x70, 0xBF, 0xA5, 0x63, 0x35, 0x39,
		0x24, 0x8C, 0x9F, 0x85, 0x16, 0xD8, 0x50, 0x95, 0x71, 0xC0, 0xF6, 0x1E, 0x6D, 0x80, 0xED, 0x15,
		0xEB, 0x63, 0xE9, 0x1B, 0xF6, 0x78, 0x31, 0xC6, 0x5C, 0xDD, 0x19, 0xBD, 0xDF, 0xA7, 0xEC, 0x50,
		0x22, 0xAD, 0xBB, 0xF6, 0xEB, 0xD6, 0xA3, 0x20, 0xC9, 0xE6, 0x9F, 0xCB, 0xF2, 0x97, 0xB9, 0x54,
		0x12, 0x66, 0xA6, 0xBE, 0x4A, 0x12, 0x43, 0xEC, 0x00, 0xEA, 0x49, 0x02
	},
	{ 0, }  // Region 7, not used
};

void pgm_012_025_state::drgw2_protection_calculate_hold(int y, int z)
{
	unsigned short old = m_drgw2_prot_hold;

	m_drgw2_prot_hold = ((old << 1) | (old >> 15));

	m_drgw2_prot_hold ^= 0x2bad;
	m_drgw2_prot_hold ^= BIT(z, y);
	m_drgw2_prot_hold ^= BIT( old,  7) <<  0;
	m_drgw2_prot_hold ^= BIT(~old, 13) <<  4;
	m_drgw2_prot_hold ^= BIT( old,  3) << 11;

	m_drgw2_prot_hold ^= (m_drgw2_prot_hilo & ~0x0408) << 1;
}

void pgm_012_025_state::drgw2_protection_calculate_hilo()
{
	UINT8 source;

	m_drgw2_prot_hilo_select++;
	if (m_drgw2_prot_hilo_select > 0xeb) {
		m_drgw2_prot_hilo_select = 0;
	}

	source = m_drgw2_source_data[m_drgw2_protection_region][m_drgw2_prot_hilo_select];

	if (m_drgw2_prot_hilo_select & 1)
	{
		m_drgw2_prot_hilo = (m_drgw2_prot_hilo & 0x00ff) | (source << 8);
	}
	else
	{
		m_drgw2_prot_hilo = (m_drgw2_prot_hilo & 0xff00) | (source << 0);
	}
}

READ16_MEMBER(pgm_012_025_state::drgw2_d80000_protection_r )
{
	switch (m_drgw2_cmd)
	{
		case 0x05:
		{
			switch (m_drgw2_ptr)
			{
				case 1: return 0x3f00 | ((m_drgw2_protection_region >> 0) & 0xff);

				case 2:
					return 0x3f00 | ((m_drgw2_protection_region >> 8) & 0xff);

				case 3:
					return 0x3f00 | ((m_drgw2_protection_region >> 16) & 0xff);

				case 4:
					return 0x3f00 | ((m_drgw2_protection_region >> 24) & 0xff);

				case 5:
				default:
					return 0x3f00 | BITSWAP8(m_drgw2_prot_hold, 5,2,9,7,10,13,12,15);
			}

			return 0x3f00;
		}

		case 0x40:
			drgw2_protection_calculate_hilo();
			return 0;

	//  case 0x13: // Read to $80eeb8
	//  case 0x1f: // Read to $80eeb8
	//  case 0xf4: // Read to $80eeb8
	//  case 0xf6: // Read to $80eeb8
	//  case 0xf8: // Read to $80eeb8
	//      return 0;

	//  default:
	//      logerror("%06x: warning, reading with igs003_reg = %02x\n", space.device().safe_pc(), m_drgw2_cmd);
	}

	return 0;
}

WRITE16_MEMBER(pgm_012_025_state::drgw2_d80000_protection_w )
{
	if (offset == 0)
	{
		m_drgw2_cmd = data;
		return;
	}

	switch (m_drgw2_cmd)
	{
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
			m_drgw2_ptr++;
			drgw2_protection_calculate_hold(m_drgw2_cmd & 0x0f, data & 0xff);
		break;

	//  case 0x08: // Used only on init..
	//  case 0x09:
	//  case 0x0a:
	//  case 0x0b:
	//  case 0x0c:
	//  break;

	//  case 0x15: // ????
	//  case 0x17:
	//  case 0xf2:
	//  break;

	//  default:
	//      logerror("%06x: warning, writing to igs003_reg %02x = %02x\n", space.device().safe_pc(), m_drgw2_cmd, data);
	}
}

MACHINE_RESET_MEMBER(pgm_012_025_state,drgw2)
{
	MACHINE_RESET_CALL_MEMBER(pgm);

	m_drgw2_cmd = 0;
	m_drgw2_ptr = 0;
	m_drgw2_prot_hilo = 0;
	m_drgw2_prot_hilo_select = 0;
	m_drgw2_prot_hold = 0;
}

void pgm_012_025_state::drgw2_common_init()
{
	m_drgw2_source_data = drgw2_source_data;

	pgm_basic_init();
	pgm_drgw2_decrypt();

	save_item(NAME(m_drgw2_cmd));
	save_item(NAME(m_drgw2_ptr));
	save_item(NAME(m_drgw2_prot_hilo));
	save_item(NAME(m_drgw2_prot_hilo_select));
	save_item(NAME(m_drgw2_prot_hold));
}

static ADDRESS_MAP_START( drgw2_mem, AS_PROGRAM, 16, pgm_012_025_state )
	AM_IMPORT_FROM(pgm_mem)
	AM_RANGE(0x100000, 0x1fffff) AM_ROMBANK("bank1") /* Game ROM */
	AM_RANGE(0xd00000, 0xd00fff) AM_NOP // Written, but never read back? Related to the protection device?
	AM_RANGE(0xd80000, 0xd80003) AM_READWRITE(drgw2_d80000_protection_r, drgw2_d80000_protection_w)
ADDRESS_MAP_END

MACHINE_CONFIG_START( pgm_012_025_drgw2, pgm_012_025_state )
	MCFG_FRAGMENT_ADD(pgmbase)

	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(drgw2_mem)

	MCFG_MACHINE_RESET_OVERRIDE(pgm_012_025_state,drgw2)
MACHINE_CONFIG_END


DRIVER_INIT_MEMBER(pgm_012_025_state,drgw2)
{
	/* incomplete? */
	UINT16 *mem16 = (UINT16 *)memregion("maincpu")->base();

	drgw2_common_init();

	m_drgw2_protection_region = 0x00000006;

	/* These ROM patches are not hacks, the protection device
	   overlays the normal ROM code, this has been confirmed on a real PCB
	   although some addresses may be missing */
	mem16[0x131098 / 2] = 0x4e93;
	mem16[0x13113e / 2] = 0x4e93;
	mem16[0x1311ce / 2] = 0x4e93;
}

DRIVER_INIT_MEMBER(pgm_012_025_state,dw2v100x)
{
	UINT16 *mem16 = (UINT16 *)memregion("maincpu")->base();

	drgw2_common_init();

	m_drgw2_protection_region = 0x00000006;

	/* These ROM patches are not hacks, the protection device
	   overlays the normal ROM code, this has been confirmed on a real PCB
	   although some addresses may be missing */
	mem16[0x131084 / 2] = 0x4e93;
	mem16[0x13112a / 2] = 0x4e93;
	mem16[0x1311ba / 2] = 0x4e93;
}

DRIVER_INIT_MEMBER(pgm_012_025_state,drgw2c)
{
	UINT16 *mem16 = (UINT16 *)memregion("maincpu")->base();

	drgw2_common_init();

	m_drgw2_protection_region = 0x00000005;

	/* These ROM patches are not hacks, the protection device
	   overlays the normal ROM code, this has been confirmed on a real PCB
	   although some addresses may be missing */
	mem16[0x1303bc / 2] = 0x4e93;
	mem16[0x130462 / 2] = 0x4e93;
	mem16[0x1304f2 / 2] = 0x4e93;
}

DRIVER_INIT_MEMBER(pgm_012_025_state,drgw2j)
{
	UINT16 *mem16 = (UINT16 *)memregion("maincpu")->base();

	drgw2_common_init();

	m_drgw2_protection_region = 0x00000001;

	/* These ROM patches are not hacks, the protection device
	   overlays the normal ROM code, this has been confirmed on a real PCB
	   although some addresses may be missing */
	mem16[0x1302c0 / 2] = 0x4e93;
	mem16[0x130366 / 2] = 0x4e93;
	mem16[0x1303f6 / 2] = 0x4e93;
}
