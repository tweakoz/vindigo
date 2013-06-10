/***********************************************************************************************************
 
 Super Game Boy emulation (for SNES/SFC)
 
 Copyright MESS Team.
 Visit http://mamedev.org for licensing and usage restrictions.

 TODO: almost everything
 
 ***********************************************************************************************************/


#include "emu.h"
#include "machine/sns_sgb.h"

#include "cpu/lr35902/lr35902.h"

//-------------------------------------------------
//  sns_rom_sgb_device - constructor
//-------------------------------------------------

const device_type SNS_LOROM_SUPERGB = &device_creator<sns_rom_sgb_device>;


sns_rom_sgb_device::sns_rom_sgb_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: sns_rom_device(mconfig, SNS_LOROM_SUPERGB, "SNES Super Game Boy Cart", tag, owner, clock, "sns_rom_sgb", __FILE__),
					m_gb_cpu(*this, "supergb"),
					m_cartslot(*this, "gb_slot")
{
}


void sns_rom_sgb_device::device_start()
{
}

void sns_rom_sgb_device::device_reset()
{
}



// SuperGB emulation

//-------------------------------------------------
//  ADDRESS_MAP( supergb_map )
//-------------------------------------------------

READ8_MEMBER(sns_rom_sgb_device::gb_cart_r)
{
	return m_cartslot->read_rom(space, offset);
}

WRITE8_MEMBER(sns_rom_sgb_device::gb_bank_w)
{
	m_cartslot->write_bank(space, offset, data);
}

READ8_MEMBER(sns_rom_sgb_device::gb_ram_r)
{
	return m_cartslot->read_ram(space, offset);
}

WRITE8_MEMBER(sns_rom_sgb_device::gb_ram_w)
{
	m_cartslot->write_ram(space, offset, data);
}


static ADDRESS_MAP_START(supergb_map, AS_PROGRAM, 8, sns_rom_sgb_device )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x7fff) AM_READWRITE(gb_cart_r, gb_bank_w)
	AM_RANGE(0xa000, 0xbfff) AM_READWRITE(gb_ram_r, gb_ram_w )   /* 8k switched RAM bank (cartridge) */
	AM_RANGE(0xc000, 0xfdff) AM_RAM                     /* 8k low RAM, echo RAM */
	AM_RANGE(0xff27, 0xff2f) AM_NOP                     /* unused */
	AM_RANGE(0xff80, 0xfffe) AM_RAM                     /* High RAM */
ADDRESS_MAP_END



WRITE8_MEMBER( sns_rom_sgb_device::gb_timer_callback )
{
}


static SLOT_INTERFACE_START(supergb_cart)
SLOT_INTERFACE_END

static MACHINE_CONFIG_FRAGMENT( supergb )
	MCFG_CPU_ADD("supergb", LR35902, 4295454)   /* 4.295454 MHz */
	MCFG_CPU_PROGRAM_MAP(supergb_map)
	MCFG_LR35902_TIMER_CB(WRITE8(sns_rom_sgb_device, gb_timer_callback))
	MCFG_LR35902_HALT_BUG

	MCFG_GB_CARTRIDGE_ADD("gb_slot", supergb_cart, NULL)
MACHINE_CONFIG_END


machine_config_constructor sns_rom_sgb_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( supergb );
}


/*-------------------------------------------------
 mapper specific handlers
 -------------------------------------------------*/


READ8_MEMBER(sns_rom_sgb_device::read_l)
{
	return read_h(space, offset);
}

READ8_MEMBER(sns_rom_sgb_device::read_h)
{
	int bank = offset / 0x10000;
	return m_rom[rom_bank_map[bank] * 0x8000 + (offset & 0x7fff)];
}

READ8_MEMBER( sns_rom_sgb_device::chip_read )
{
	UINT16 address = offset & 0xffff;

	//LY counter
	if (address == 0x6000) 
	{
		m_sgb_ly = 0;// GameBoy PPU LY here
		m_sgb_row = m_lcd_row;
		return m_sgb_ly;
	}
	
	//command ready port
	if (address == 0x6002) 
	{
		bool data = (m_packetsize > 0);
		if (data) 
		{
			for (int i = 0; i < 16; i++) 
				m_joy_pckt[i] = m_packet_data[0][i];
			m_packetsize--;	

			//hack because we still don't emulate input packets!
			if (!m_packetsize) m_packetsize = 64;

			// shift packet
			for (int i = 0; i < m_packetsize; i++) 
				for (int j = 0; j < 16; j++)
					m_packet_data[i][j] = m_packet_data[i + 1][j];
		}
		return data;
	}
	
	//ICD2 revision
	if (address == 0x600f) 
		return 0x21;
	
	//command port
	if ((address & 0xfff0) == 0x7000) 
		return m_joy_pckt[address & 0x0f];
	
	//VRAM port
	if (address == 0x7800) 
	{
		UINT8 data = m_lcd_output[m_vram_offs];
		m_vram_offs = (m_vram_offs + 1) % 320;
		return data;
	}

	return 0x00;	// this should never happen?
}

void sns_rom_sgb_device::lcd_render(UINT32 *source) 
{
	memset(m_lcd_output, 0x00, 320 * sizeof(UINT16));
	
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 160; x++) 
		{
			UINT32 pixel = *source++;
			UINT16 addr = y * 2 + (x / 8 * 16);
			m_lcd_output[addr + 0] |= ((pixel & 1) >> 0) << (7 - (x & 7));
			m_lcd_output[addr + 1] |= ((pixel & 2) >> 1) << (7 - (x & 7));
		}
	}
}

WRITE8_MEMBER( sns_rom_sgb_device::chip_write )
{
	UINT16 address = offset & 0xffff;
	
	//VRAM port
	if (address == 0x6001) 
	{
		m_vram = data;
		m_vram_offs = 0;
		
		UINT8 offset = (m_sgb_row - (4 - (m_vram - (m_sgb_ly & 3)))) & 3;
		lcd_render(m_lcd_buffer + offset * 160 * 8);
		
		return;
	}
	
	//control port
	if (address == 0x6003) 
	{
		if ((m_port & 0x80) == 0x00 && (data & 0x80) == 0x80) 
		{
			//reset
		}

		switch (data & 3) 
		{
			//change CPU frequency
		}
		m_port = data;
		return;
	}
	
	if (address == 0x6004) 
	{
		//joypad 1
		m_joy1 = data; 
		return; 
	}
	if (address == 0x6005) 
	{
		//joypad 2
		m_joy2 = data; 
		return; 
	}
	if (address == 0x6006) 
	{
		//joypad 3
		m_joy3 = data; 
		return; 
	}
	if (address == 0x6007) 
	{
		//joypad 4
		m_joy4 = data; 
		return; 
	}
	
}

