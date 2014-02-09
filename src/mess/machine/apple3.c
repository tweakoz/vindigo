/***************************************************************************

    machine/apple3.c

    Apple ///

    VIA #0 (D VIA)
	    CA1: IRQ from the MM58167 RTC
    	CA2: 1 if key pressed, 0 otherwise
    	CB1/CB2: connected to VBL
 
		Port A: Environment register (all bits out)
			bit 7: 1 for 1 MHz, 0 for 2 MHz
			bit 6: 1 for I/O at C000-CFFF
			bit 5: 1 to enable video 
			bit 4: 1 to enable NMI/Reset 
			bit 3: 1 to write-protect RAM in system bank C000-FFFF
			bit 2: 1 to force primary stack at 0100-01FF
			bit 1: 1 for primary ROM, 0 for secondary (Apple III doesn't have a secondary ROM, so this should always be '1' when bit 0 is)
			bit 0: 1 to enable ROM in F000-FFFF
 
		Port B: Zero page high 8 address bits, also MM58167 RTC register select (all bits out)
 
    VIA #1 (E VIA)
    	CA1: OR of all 4 slots' IRQ status
    	CA2: SW1 (Open Apple key?)
    	CB1: SW3/SCO
    	CB2: SER
 
    	Port A:
			bits 0-2: bank select for $2000-$9FFF range
			bit 3: n/c
			bit 4: slot 4 IRQ (in)
			bit 5: slot 3 IRQ (in)
			bit 6: Apple II mode trap output (out)
			bit 7: IRQ status (in) (0 = IRQ, 1 = no IRQ)
 
		Port B:
			bits 0-5: 6-bit audio DAC output
			bit 6: screen blank
			bit 7: OR of NMI from slots
 
***************************************************************************/

#include "emu.h"
#include "includes/apple3.h"
#include "includes/apple2.h"
#include "machine/applefdc.h"
#include "machine/appldriv.h"

static void apple3_update_drives(device_t *device);

#define LOG_MEMORY      1
#define LOG_INDXADDR    1

READ8_MEMBER(apple3_state::apple3_c0xx_r)
{
	UINT8 result = 0xFF;

	switch(offset)
	{
		/* keystrobe */
		case 0x00: case 0x01: case 0x02: case 0x03:
		case 0x04: case 0x05: case 0x06: case 0x07:
			result = (m_transchar & 0x7f) | m_strobe;
			break;

		/* modifier keys */
		case 0x08: case 0x09: case 0x0A: case 0x0B:
		case 0x0C: case 0x0D: case 0x0E: case 0x0F:
			{
				UINT8 tmp = m_kbspecial->read();

				result = 0x7c | (m_transchar & 0x80);

				if (m_strobe)
				{
					result |= 1;
				}

				if (tmp & 0x06)
				{
					result |= 0x02;
				}
				if (tmp & 0x08)
				{
					result &= ~0x04;
				}
				if (tmp & 0x01)
				{
					result &= ~0x08;
				}
				if (tmp & 0x10)
				{
					result &= ~0x10;
				}
				if (tmp & 0x20)
				{
					result &= ~0x20;
				}
			}
//			printf("modifier = %02x\n", result);
			break;

		case 0x10: case 0x11: case 0x12: case 0x13:
		case 0x14: case 0x15: case 0x16: case 0x17:
		case 0x18: case 0x19: case 0x1A: case 0x1B:
		case 0x1C: case 0x1D: case 0x1E: case 0x1F:
			m_strobe = 0;
			break;

		case 0x30: case 0x31: case 0x32: case 0x33:
		case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x38: case 0x39: case 0x3A: case 0x3B:
		case 0x3C: case 0x3D: case 0x3E: case 0x3F:
			m_speaker_state ^= 1;
			m_speaker->level_w(m_speaker_state);
			result = 0xff;
			break;

		case 0x40: case 0x41: case 0x42: case 0x43:
		case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4A: case 0x4B:
		case 0x4C: case 0x4D: case 0x4E: case 0x4F:
			m_c040_time = 200;
			result = 0xff;
			break;

		case 0x50: case 0x51: case 0x52: case 0x53:
		case 0x54: case 0x55: case 0x56: case 0x57:
			/* graphics softswitches */
			if (offset & 1)
				m_flags |= 1 << ((offset - 0x50) / 2);
			else
				m_flags &= ~(1 << ((offset - 0x50) / 2));
			break;

		case 0x60: case 0x61: case 0x62: case 0x63:
		case 0x64: case 0x65: case 0x66: case 0x67:
		case 0x68: case 0x69: case 0x6A: case 0x6B:
		case 0x6C: case 0x6D: case 0x6E: case 0x6F:
			/* unsure what these are */
			result = 0x00;
			break;

		case 0xD0: case 0xD1: case 0xD2: case 0xD3:
		case 0xD4: case 0xD5: case 0xD6: case 0xD7:
			/* external drive stuff */
			if (offset & 1)
				m_flags |= VAR_EXTA0 << ((offset - 0xD0) / 2);
			else
				m_flags &= ~(VAR_EXTA0 << ((offset - 0xD0) / 2));
			apple3_update_drives(machine().device("fdc"));
			result = 0x00;
			break;

		case 0xDB:
			apple3_write_charmem();
			break;

		case 0xE0: case 0xE1: case 0xE2: case 0xE3:
		case 0xE4: case 0xE5: case 0xE6: case 0xE7:
		case 0xE8: case 0xE9: case 0xEA: case 0xEB:
		case 0xEC: case 0xED: case 0xEE: case 0xEF:
			result = m_fdc->read(offset);
			break;

		case 0xF0:
		case 0xF1:
		case 0xF2:
		case 0xF3:
			result = m_acia->read(space, offset & 0x03);
			break;
	}
	return result;
}



WRITE8_MEMBER(apple3_state::apple3_c0xx_w)
{
	switch(offset)
	{
		case 0x10: case 0x11: case 0x12: case 0x13:
		case 0x14: case 0x15: case 0x16: case 0x17:
		case 0x18: case 0x19: case 0x1A: case 0x1B:
		case 0x1C: case 0x1D: case 0x1E: case 0x1F:
			m_strobe = 0;
			break;

		case 0x30: case 0x31: case 0x32: case 0x33:
		case 0x34: case 0x35: case 0x36: case 0x37:
		case 0x38: case 0x39: case 0x3A: case 0x3B:
		case 0x3C: case 0x3D: case 0x3E: case 0x3F:
			m_speaker_state ^= 1;
			m_speaker->level_w(m_speaker_state);
			break;

		case 0x40: case 0x41: case 0x42: case 0x43:
		case 0x44: case 0x45: case 0x46: case 0x47:
		case 0x48: case 0x49: case 0x4A: case 0x4B:
		case 0x4C: case 0x4D: case 0x4E: case 0x4F:
			m_c040_time = 200;
			break;

		case 0x50: case 0x51: case 0x52: case 0x53:
		case 0x54: case 0x55: case 0x56: case 0x57:
			/* graphics softswitches */
			if (offset & 1)
				m_flags |= 1 << ((offset - 0x50) / 2);
			else
				m_flags &= ~(1 << ((offset - 0x50) / 2));
			break;

		case 0xD0: case 0xD1: case 0xD2: case 0xD3:
		case 0xD4: case 0xD5: case 0xD6: case 0xD7:
			/* external drive stuff */
			if (offset & 1)
				m_flags |= VAR_EXTA0 << ((offset - 0xD0) / 2);
			else
				m_flags &= ~(VAR_EXTA0 << ((offset - 0xD0) / 2));
			apple3_update_drives(machine().device("fdc"));
			break;

		case 0xDB:
			apple3_write_charmem();
			break;

		case 0xE0: case 0xE1: case 0xE2: case 0xE3:
		case 0xE4: case 0xE5: case 0xE6: case 0xE7:
		case 0xE8: case 0xE9: case 0xEA: case 0xEB:
		case 0xEC: case 0xED: case 0xEE: case 0xEF:
			m_fdc->write(offset, data);
			break;

		case 0xF0:
		case 0xF1:
		case 0xF2:
		case 0xF3:
			m_acia->write(space, offset & 0x03, data);
			break;
	}
}

TIMER_DEVICE_CALLBACK_MEMBER(apple3_state::apple3_interrupt)
{
	m_via_1->write_cb1(machine().primary_screen->vblank()); 
	m_via_1->write_cb2(machine().primary_screen->vblank());
}

UINT8 *apple3_state::apple3_bankaddr(UINT16 bank, offs_t offset)
{
	if (bank != (UINT16) ~0)
	{
		bank %= m_ram->size() / 0x8000;
		if ((bank + 1) == (m_ram->size() / 0x8000))
			bank = 0x02;
	}
	offset += ((offs_t) bank) * 0x8000;
	offset %= m_ram->size();
	return &m_ram->pointer()[offset];
}

UINT8 *apple3_state::apple3_get_zpa_addr(offs_t offset)
{
	m_zpa = (((offs_t) m_via_0_b) * 0x100) + offset;

	if (m_via_0_b < 0x20)
		return apple3_bankaddr(~0, m_zpa);
	else if (m_via_0_b > 0x9F)
		return apple3_bankaddr(~0, m_zpa - 0x8000);
	else
		return apple3_bankaddr(m_via_1_a, m_zpa - 0x2000);
}

void apple3_state::apple3_update_memory()
{
	UINT16 bank;
	UINT8 page;

	if (LOG_MEMORY)
	{
		logerror("apple3_update_memory(): via_0_b=0x%02x via_1_a=0x0x%02x\n", m_via_0_b, m_via_1_a);
	}

	machine().device("maincpu")->set_unscaled_clock((m_via_0_a & 0x80) ? 1000000 : 2000000);

	/* bank 2 (0100-01FF) */
	if (!(m_via_0_a & 0x04))
	{
		if (m_via_0_b < 0x20)
		{
			bank = ~0;  /* system bank */
			page = m_via_0_b ^ 0x01;
		}
		else if (m_via_0_b >= 0xA0)
		{
			bank = ~0;  /* system bank */
			page = (m_via_0_b ^ 0x01) - 0x80;
		}
		else
		{
			bank = m_via_1_a;
			page = (m_via_0_b ^ 0x01) - 0x20;
		}
	}
	else
	{
		bank = ~0;
		page = 0x01;
	}
	m_bank2 = apple3_bankaddr(bank, ((offs_t) page) * 0x100); 

	/* bank 3 (0200-1FFF) */
	m_bank3 = apple3_bankaddr(~0, 0x0200); 

	/* bank 4 (2000-9FFF) */
	m_bank4 = apple3_bankaddr(m_via_1_a, 0x0000); 

	/* bank 5 (A000-BFFF) */
	m_bank5 = apple3_bankaddr(~0, 0x2000); 

	/* bank 8 (C000-C0FF) */
	if (!(m_via_0_a & 0x40))
	{
		m_bank8 = apple3_bankaddr(~0, 0x4000); 
	}

	/* bank 9 (C100-C4FF) */
	if (!(m_via_0_a & 0x40))
	{
		m_bank9 = apple3_bankaddr(~0, 0x4100);                                  
	}

	/* bank 10 (C500-C7FF) */
	m_bank10 = apple3_bankaddr(~0, 0x4500); 

	/* bank 11 (C800-CFFF) */
	if (!(m_via_0_a & 0x40))
	{
		m_bank11 = apple3_bankaddr(~0, 0x4800);
	}

	/* install bank 6 (D000-EFFF) */
	m_bank6 = apple3_bankaddr(~0, 0x5000);

	/* install bank 7 (F000-FFFF) */
	if (m_via_0_a & 0x01)
		m_bank7 = memregion("maincpu")->base();
	else
		m_bank7 = apple3_bankaddr(~0, 0x7000);
}



void apple3_state::apple3_via_out(UINT8 *var, UINT8 data)
{
	if (*var != data)
	{
		*var = data;
		apple3_update_memory();
	}
}


WRITE8_MEMBER(apple3_state::apple3_via_0_out_a)
{
	apple3_via_out(&m_via_0_a, data);
}

WRITE8_MEMBER(apple3_state::apple3_via_0_out_b)
{
	apple3_via_out(&m_via_0_b, data);
}

WRITE8_MEMBER(apple3_state::apple3_via_1_out_a)
{
	apple3_via_out(&m_via_1_a, data);
}

WRITE8_MEMBER(apple3_state::apple3_via_1_out_b)
{
	m_dac->write_unsigned8(data<<2);
	apple3_via_out(&m_via_1_b, data);
}

void apple3_state::apple3_irq_update()
{
	if (m_via_1_irq || m_via_0_irq)
	{
//		printf("   asserting IRQ\n");
		m_maincpu->set_input_line(M6502_IRQ_LINE, ASSERT_LINE);
		m_via_1->write_pa7(0);	// this is active low
	}
	else
	{
//		printf("   clearing IRQ\n");
		m_maincpu->set_input_line(M6502_IRQ_LINE, CLEAR_LINE);
		m_via_1->write_pa7(1);
	}
}

WRITE_LINE_MEMBER(apple3_state::apple3_via_1_irq_func)
{
//	printf("via 1 IRQ: %d\n", state);
	m_via_1_irq = state;
	apple3_irq_update();
}


WRITE_LINE_MEMBER(apple3_state::apple3_via_0_irq_func)
{
//	printf("via 0 IRQ: %d\n", state);
	m_via_0_irq = state;
	apple3_irq_update();
}

MACHINE_RESET_MEMBER(apple3_state,apple3)
{
	m_indir_count = 0;
	m_sync = false;
	m_speaker_state = 0;
	m_speaker->level_w(0);
	m_c040_time = 0;
	m_strobe = 0;
	m_lastchar = 0x0d;
}



UINT8 *apple3_state::apple3_get_indexed_addr(offs_t offset)
{
	UINT8 n;
	UINT8 *result = NULL;

	if ((m_via_0_b >= 0x18) && (m_via_0_b <= 0x1F))
	{
		n = *apple3_bankaddr(~0, m_zpa ^ 0x0C00);

		if (LOG_INDXADDR)
		{
			if (m_last_n != n)
			{
				logerror("indxaddr: zpa=0x%04x n=0x%02x\n", m_zpa, n);
				m_last_n = n;
			}
		}

		if (n == 0x8F)
		{
			/* get at that special ram under the VIAs */
			if ((offset >= 0xFFD0) && (offset <= 0xFFEF))
				result = apple3_bankaddr(~0, offset & 0x7FFF);
			else if (offset < 0x2000)
				result = apple3_bankaddr(~0, offset - 0x2000);
			else if (offset > 0x9FFF)
				result = apple3_bankaddr(~0, offset - 0x8000);
			else
				result = &m_ram->pointer()[offset - 0x2000];
		}
		else if ((n >= 0x80) && (n <= 0x8E))
		{
			if (offset < 0x0100)
				result = apple3_bankaddr(~0, ((offs_t) m_via_0_b) * 0x100 + offset);
			else
				result = apple3_bankaddr(n, offset);
		}
		else if (n == 0xFF)
		{
			if (offset < 0x2000)
				result = apple3_bankaddr(~0, offset - 0x2000);
			else if (offset < 0xA000)
				result = apple3_bankaddr(m_via_1_a, offset - 0x2000);
			else if (offset < 0xC000)
				result = apple3_bankaddr(~0, offset - 0x8000);
			else if (offset < 0xD000)
				result = NULL;
			else if (offset < 0xF000)
				result = apple3_bankaddr(~0, offset - 0x8000);
			else
				result = (UINT8 *) ~0;
		}
		else if (offset < 0x0100)
		{
			result = apple3_bankaddr(~0, ((offs_t) m_via_0_b) * 0x100 + offset);
		}
	}
	else if ((offset >= 0xF000) && (m_via_0_a & 0x01))
	{
		/* The Apple /// Diagnostics seems to expect that indexed writes
		 * always write to RAM.  That image jumps to an address that is
		 * undefined unless this code is enabled. 
		 *  
		 * The confidence test and the diagnostics together indicates 
		 * that this *doesn't* apply to the VIA region, however. 
		 */ 

		if (offset < 0xffd0 || offset > 0xffef)
		{
			result = apple3_bankaddr(~0, offset - 0x8000); 
		}
	}

	return result;
}

static void apple3_update_drives(device_t *device)
{
	apple3_state *state = device->machine().driver_data<apple3_state>();
	int enable_mask = 0x00;

	if (state->m_enable_mask & 0x01)
		enable_mask |= 0x01;

	if (state->m_enable_mask & 0x02)
	{
		switch(state->m_flags & (VAR_EXTA0 | VAR_EXTA1))
		{
			case VAR_EXTA0:
				enable_mask |= 0x02;
				break;
			case VAR_EXTA1:
				enable_mask |= 0x04;
				break;
			case VAR_EXTA1|VAR_EXTA0:
				enable_mask |= 0x08;
				break;
		}
	}

	apple525_set_enable_lines(device,enable_mask);
}



static void apple3_set_enable_lines(device_t *device,int enable_mask)
{
	apple3_state *state = device->machine().driver_data<apple3_state>();
	state->m_enable_mask = enable_mask;
	apple3_update_drives(device);
}



const applefdc_interface apple3_fdc_interface =
{
	apple525_set_lines,
	apple3_set_enable_lines,
	apple525_read_data,
	apple525_write_data
};



DRIVER_INIT_MEMBER(apple3_state,apple3)
{
	m_enable_mask = 0;
	apple3_update_drives(machine().device("fdc"));

	m_flags = 0;
	m_via_0_a = ~0;
	m_via_1_a = ~0;
	m_via_0_irq = 0;
	m_via_1_irq = 0;

	/* these are here to appease the Apple /// confidence tests */
	m_via_1->write_pa0(1);
	m_via_1->write_pa1(1);
	m_via_1->write_pa2(1);
	m_via_1->write_pa3(1);
	m_via_1->write_pa4(1);
	m_via_1->write_pa5(1);
	m_via_1->write_pa6(1);
	m_via_1->write_pa7(1);

	m_via_1->write_pb0(1);
	m_via_1->write_pb1(1);
	m_via_1->write_pb2(1);
	m_via_1->write_pb3(1);
	m_via_1->write_pb4(1);
	m_via_1->write_pb5(1);
	m_via_1->write_pb6(1);
	m_via_1->write_pb7(1);

	apple3_update_memory();
}

READ8_MEMBER(apple3_state::apple3_memory_r)
{
	UINT8 rv = 0xff;

	// (zp), y or (zp,x) read
	if (!space.debugger_access())
	{
		if (((m_indir_count == 4) && (m_indir_opcode & 0x10)) ||
			((m_indir_count == 5) && !(m_indir_opcode & 0x10)))
		{
			UINT8 *test;
			test = apple3_get_indexed_addr(offset);

			if (test)
			{
				return *test;
			}
		}
	}

	if (offset < 0x100)
	{
		rv = *apple3_get_zpa_addr(offset); 
	}
	else if (offset < 0x200)
	{
		rv = m_bank2[offset-0x100];
	}
	else if (offset < 0x2000)
	{
		rv = m_bank3[offset-0x200];
	}
	else if (offset < 0xa000)
	{
		rv = m_bank4[offset-0x2000];
	}
	else if (offset < 0xc000)
	{
		rv = m_bank5[offset-0xa000];
	}
	else if (offset < 0xc100)
	{
		if (m_via_0_a & 0x40)
		{
			if (!space.debugger_access())
			{
				rv = apple3_c0xx_r(space, offset-0xc000);
			}
		}
		else
		{
			rv = m_bank8[offset - 0xc000];
		}
	}
	else if (offset < 0xc500)
	{
		if (!(m_via_0_a & 0x40))
		{
			rv = m_bank9[offset - 0xc100];
		}
	}
	else if (offset < 0xc800)
	{
		rv = m_bank10[offset - 0xc500];
	}
	else if (offset < 0xd000)
	{
		if (!(m_via_0_a & 0x40))
		{
			rv = m_bank11[offset - 0xc800];
		}
	}
	else if (offset < 0xf000)
	{
		rv = m_bank6[offset - 0xd000];
	}
	else
	{
		if (offset >= 0xffd0 && offset <= 0xffdf)
		{
			rv = m_via_0->read(space, offset);
		}
		else if (offset >= 0xffe0 && offset <= 0xffef)
		{
			rv = m_via_1->read(space, offset);
		}
		else
		{
			rv = m_bank7[offset - 0xf000]; 
		}
	}

	if (!space.debugger_access())
	{
		if (m_indir_count > 0) 
		{
			m_indir_count++;
		}

		// capture last opcode for indirect mode shenanigans
		if (m_sync)
		{
			// 0xN1 is a (zp),y or (zp, x) opcode
			if ((rv & 0x0f) == 0x1)
			{
//				printf("(zp),y or (zp,x) %02x at %x\n", rv, offset);
				m_indir_count = 1;
				m_indir_opcode = rv;
			}
		}
	}

	return rv;
}

WRITE8_MEMBER(apple3_state::apple3_memory_w)
{
	if ((!space.debugger_access()) && (m_indir_count > 0))
	{
		UINT8 *test;
		test = apple3_get_indexed_addr(offset);

		if (test)
		{
			*test = data;
			return;
		}
	}

	if (offset < 0x100)
	{
		*apple3_get_zpa_addr(offset) = data;
		return;
	}
	else if (offset < 0x200)
	{
		m_bank2[offset-0x100] = data;
	}
	else if (offset < 0x2000)
	{
		m_bank3[offset-0x200] = data;
	}
	else if (offset < 0xa000)
	{
		m_bank4[offset-0x2000] = data;
	}
	else if (offset < 0xc000)
	{
		m_bank5[offset-0xa000] = data;
	}
	else if (offset < 0xc100)
	{
		if (m_via_0_a & 0x40)
		{
			if (!space.debugger_access())
			{
				apple3_c0xx_w(space, offset-0xc000, data);
			}
		}
		else
		{
			// is this page write protected?
			if (!(m_via_0_a & 0x08))
			{
				m_bank8[offset - 0xc000] = data;
			}
		}
	}
	else if (offset < 0xc500)
	{
		if (!(m_via_0_a & 0x40))
		{
			if (!(m_via_0_a & 0x08))
			{
				m_bank9[offset - 0xc100] = data;
			}
		}
	}
	else if (offset < 0xc800)
	{
		if (!(m_via_0_a & 0x08))
		{
			m_bank10[offset - 0xc500] = data;
		}
	}
	else if (offset < 0xd000)
	{
		if (!(m_via_0_a & 0x40))
		{
			if (!(m_via_0_a & 0x08))
			{
				m_bank11[offset - 0xc800] = data;
			}
		}
	}
	else if (offset < 0xf000)
	{
		if (!(m_via_0_a & 0x08))
		{
			m_bank6[offset - 0xd000] = data;
		}
	}
	else
	{
		if (offset >= 0xffd0 && offset <= 0xffdf)
		{
			if (!space.debugger_access())
			{
				m_via_0->write(space, offset, data);
			}
		}
		else if (offset >= 0xffe0 && offset <= 0xffef)
		{
			if (!space.debugger_access())
			{
				m_via_1->write(space, offset, data);
			}
		}
		else
		{
			if (!(m_via_0_a & 0x09))
			{
				m_bank7[offset - 0xf000] = data;
			}
		}
	}
}

WRITE_LINE_MEMBER(apple3_state::apple3_sync_w)
{
//	printf("sync: %d\n", state);
	m_sync = (state == ASSERT_LINE) ? true : false;

	if (m_sync)
	{
		m_indir_count = 0; 
	}
}

TIMER_DEVICE_CALLBACK_MEMBER(apple3_state::apple3_c040_tick)
{
	if (m_c040_time > 0)
	{
		m_speaker_state ^= 1;
		m_speaker->level_w(m_speaker_state);
		m_c040_time--;
	}
}

READ_LINE_MEMBER(apple3_state::ay3600_shift_r)
{
	// either shift key
	if (m_kbspecial->read() & 0x06)
	{
		return ASSERT_LINE;
	}

	return CLEAR_LINE;
}

READ_LINE_MEMBER(apple3_state::ay3600_control_r)
{
	if (m_kbspecial->read() & 0x08)
	{
		return ASSERT_LINE;
	}

	return CLEAR_LINE;
}

static const UINT8 key_remap[0x50][4] =
{
/*	  norm shft ctrl both */
	{ 0x9b,0x9b,0x9b,0x9b },    /* Escape  00     */
	{ 0x31,0x21,0x31,0x31 },    /* 1 !     01     */
	{ 0x32,0x40,0x32,0x00 },    /* 2 @     02     */
	{ 0x33,0x23,0x33,0x23 },    /* 3 #     03     */
	{ 0x34,0x24,0x34,0x24 },    /* 4 $     04     */
	{ 0x35,0x25,0x35,0x25 },    /* 5 %     05     */
	{ 0x36,0x5e,0x35,0x53 },    /* 6 ^     06     */
	{ 0x37,0x26,0x37,0x26 },    /* 7 &     07     */
	{ 0x38,0x2a,0x38,0x2a },    /* 8 *     08     */
	{ 0x39,0x28,0x39,0x28 },    /* 9 (     09     */
	{ 0x89,0x89,0x89,0x89 },    /* Tab     0a     */
	{ 0x51,0x51,0x11,0x11 },    /* q Q     0b     */
	{ 0x57,0x57,0x17,0x17 },    /* w W     0c     */
	{ 0x45,0x45,0x05,0x05 },    /* e E     0d     */
	{ 0x52,0x52,0x12,0x12 },    /* r R     0e     */
	{ 0x54,0x54,0x14,0x14 },    /* t T     0f     */
	{ 0x59,0x59,0x19,0x19 },    /* y Y     10     */
	{ 0x55,0x55,0x15,0x15 },    /* u U     11     */
	{ 0x49,0x49,0x09,0x09 },    /* i I     12     */
	{ 0x4f,0x4f,0x0f,0x0f },    /* o O     13     */
	{ 0x41,0x41,0x01,0x01 },    /* a A     14     */
	{ 0x53,0x53,0x13,0x13 },    /* s S     15     */
	{ 0x44,0x44,0x04,0x04 },    /* d D     16     */
	{ 0x46,0x46,0x06,0x06 },    /* f F     17     */
	{ 0x48,0x48,0x08,0x08 },    /* h H     18     */
	{ 0x47,0x47,0x07,0x07 },    /* g G     19     */
	{ 0x4a,0x4a,0x0a,0x0a },    /* j J     1a     */
	{ 0x4b,0x4b,0x0b,0x0b },    /* k K     1b     */
	{ 0x3b,0x3a,0x3b,0x3a },    /* ; :     1c     */
	{ 0x4c,0x4c,0x0c,0x0c },    /* l L     1d     */
	{ 0x5a,0x5a,0x1a,0x1a },    /* z Z     1e     */
	{ 0x58,0x58,0x18,0x18 },    /* x X     1f     */
	{ 0x43,0x43,0x03,0x03 },    /* c C     20     */
	{ 0x56,0x56,0x16,0x16 },    /* v V     21     */
	{ 0x42,0x42,0x02,0x02 },    /* b B     22     */
	{ 0x4e,0x4e,0x0e,0x0e },    /* n N     23     */
	{ 0x4d,0x4d,0x0d,0x0d },    /* m M     24     */
	{ 0x2c,0x3c,0x2c,0x3c },    /* , <     25     */
	{ 0x2e,0x3e,0x2e,0x3e },    /* . >     26     */
	{ 0x2f,0x3f,0x2f,0x3f },    /* / ?     27     */
	{ 0x00,0x00,0x00,0x00 },    /* 0x28 unused    */
	{ 0xb9,0xb9,0xb9,0xb9 },    /* 9 (KP)  29     */
	{ 0x00,0x00,0x00,0x00 },    /* 0x2a unused    */
	{ 0xb8,0xb8,0xb8,0xb8 },    /* 8 (KP)  2b     */
	{ 0x00,0x00,0x00,0x00 },    /* 0x2c unused    */
	{ 0xb7,0xb7,0xb7,0xb7 },    /* 7 (KP)  2d     */
	{ 0x5c,0x7c,0x7f,0x1c },    /* \ |     2e     */
	{ 0x3d,0x2b,0x3d,0x2b },    /* = +     2f     */
	{ 0x30,0x29,0x30,0x29 },    /* 0 )     30     */
	{ 0x2d,0x5f,0x2d,0x1f },    /* - _     31     */
	{ 0x00,0x00,0x00,0x00 },    /* 0x32 unused    */
	{ 0xb6,0xb6,0xb6,0xb6 },    /* 6 (KP)  33     */
	{ 0x00,0x00,0x00,0x00 },    /* 0x34 unused    */
	{ 0xb5,0xb5,0xb5,0xb5 },    /* 5 (KP)  35     */
	{ 0x00,0x00,0x00,0x00 },    /* 0x36 unused    */
	{ 0xb4,0xb4,0xb4,0xb4 },    /* 4 (KP)  37     */
	{ 0x60,0x7e,0x60,0x7e },    /* ` ~     38     */
	{ 0x5d,0x7d,0x1d,0x1d },    /* ] }     39     */
	{ 0x50,0x50,0x10,0x10 },    /* p P     3a     */
	{ 0x5b,0x7b,0x1b,0x1b },    /* [ {     3b     */
	{ 0x00,0x00,0x00,0x00 },    /* 0x3c unused    */
	{ 0xb3,0xb3,0xb3,0xb3 },    /* 3 (KP)  3d     */
	{ 0xae,0xae,0xae,0xae },    /* . (KP)  3e     */
	{ 0xb2,0xb2,0xb2,0xb2 },    /* 2 (KP)  3f     */
	{ 0xb0,0xb0,0xb0,0xb0 },    /* 0 (KP)  40     */
	{ 0xb1,0xb1,0xb1,0xb1 },    /* 1 (KP)  41     */
	{ 0x0d,0x0d,0x0d,0x0d },    /* Enter   42     */
	{ 0x8b,0x8b,0x8b,0x8b },    /* Up      43     */
	{ 0x00,0x00,0x00,0x00 },    /* 0x44 unused    */
	{ 0x27,0x22,0x27,0x22 },    /* ' "     45     */
	{ 0x00,0x00,0x00,0x00 },    /* 0x46 unused    */
	{ 0x00,0x00,0x00,0x00 },    /* 0x47 unused    */
	{ 0x8d,0x8d,0x8d,0x8d },    /* Ent(KP) 48     */
	{ 0xa0,0xa0,0xa0,0xa0 },    /* Space   49     */
	{ 0x00,0x00,0x00,0x00 },    /* 0x4a unused    */
	{ 0xad,0xad,0xad,0xad },    /* - (KP)  4b     */
	{ 0x95,0x95,0x95,0x95 },    /* Right   4c     */
	{ 0x8a,0x8a,0x8a,0x8a },    /* Down    4d     */
	{ 0x88,0x88,0x88,0x88 },    /* Left    4e     */
	{ 0x00,0x00,0x00,0x00 }     /* 0x4f unused    */
};

WRITE_LINE_MEMBER(apple3_state::ay3600_data_ready_w)
{
	m_via_1->write_ca2(state);

	if (state == ASSERT_LINE)
	{
		UINT16 trans;
		int mod = 0;
		m_lastchar = m_ay3600->b_r(); 

		trans = m_lastchar & ~(0x1c0);	// clear the 3600's control/shift stuff
		trans |= (m_lastchar & 0x100)>>2;	// bring the 0x100 bit down to the 0x40 place

		mod = (m_kbspecial->read() & 0x06) ? 0x01 : 0x00;
		mod |= (m_kbspecial->read() & 0x08) ? 0x02 : 0x00;

		m_transchar = key_remap[trans][mod];

		if (m_transchar != 0)
		{
			m_strobe = 0x80;
//			printf("new char = %04x (%02x)\n", trans, m_transchar);
		}
	}
}

