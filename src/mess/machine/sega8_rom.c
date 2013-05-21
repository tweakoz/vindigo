/***********************************************************************************************************

 Sega 8-bit cart emulation (for Master System, GameGear and SG-1000)

 ***********************************************************************************************************/


#include "emu.h"
#include "machine/sega8_rom.h"


//-------------------------------------------------
//  constructors
//-------------------------------------------------

const device_type SEGA8_ROM_STD = &device_creator<sega8_rom_device>;
const device_type SEGA8_ROM_EEPROM = &device_creator<sega8_eeprom_device>;
const device_type SEGA8_ROM_TEREBI = &device_creator<sega8_terebi_device>;
const device_type SEGA8_ROM_CODEMASTERS = &device_creator<sega8_codemasters_device>;
const device_type SEGA8_ROM_4PAK = &device_creator<sega8_4pak_device>;
const device_type SEGA8_ROM_ZEMINA = &device_creator<sega8_zemina_device>;
const device_type SEGA8_ROM_NEMESIS = &device_creator<sega8_nemesis_device>;
const device_type SEGA8_ROM_JANGGUN = &device_creator<sega8_janggun_device>;
const device_type SEGA8_ROM_KOREAN = &device_creator<sega8_korean_device>;
const device_type SEGA8_ROM_KOREAN_NB = &device_creator<sega8_korean_nb_device>;


sega8_rom_device::sega8_rom_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source)
					: device_t(mconfig, type, name, tag, owner, clock, shortname, source),
						device_sega8_cart_interface( mconfig, *this )
{
}

sega8_rom_device::sega8_rom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: device_t(mconfig, SEGA8_ROM_STD, "SMS Carts", tag, owner, clock, "sega8_rom", __FILE__),
						device_sega8_cart_interface( mconfig, *this )
{
}


sega8_eeprom_device::sega8_eeprom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: device_t(mconfig, SEGA8_ROM_EEPROM, "SMS EEPROM Carts", tag, owner, clock, "sega8_eeprom", __FILE__),
						device_sega8_cart_interface( mconfig, *this ),
						m_eeprom(*this, "eeprom")
{
}


sega8_terebi_device::sega8_terebi_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: sega8_rom_device(mconfig, SEGA8_ROM_TEREBI, "SMS Terebi Oekaki Cart", tag, owner, clock, "sega8_terebi", __FILE__),
						m_tvdraw_x(*this, "TVDRAW_X"),
						m_tvdraw_y(*this, "TVDRAW_Y"),
						m_tvdraw_pen(*this, "TVDRAW_PEN")
{
}


sega8_codemasters_device::sega8_codemasters_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: device_t(mconfig, SEGA8_ROM_CODEMASTERS, "SMS Codemasters Carts", tag, owner, clock, "sega8_codemasters", __FILE__),
						device_sega8_cart_interface( mconfig, *this )
{
}


sega8_4pak_device::sega8_4pak_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: sega8_rom_device(mconfig, SEGA8_ROM_4PAK, "SMS 4-Pak Cart", tag, owner, clock, "sega8_4pak", __FILE__)
{
}


sega8_zemina_device::sega8_zemina_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source)
					: device_t(mconfig, type, name, tag, owner, clock, shortname, source),
						device_sega8_cart_interface( mconfig, *this )
{
}

sega8_zemina_device::sega8_zemina_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: device_t(mconfig, SEGA8_ROM_ZEMINA, "SMS Zemina Carts", tag, owner, clock, "sega8_zemina", __FILE__),
						device_sega8_cart_interface( mconfig, *this )
{
}


sega8_nemesis_device::sega8_nemesis_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: sega8_zemina_device(mconfig, SEGA8_ROM_NEMESIS, "SMS Nemesis Cart", tag, owner, clock, "sega8_nemesis", __FILE__)
{
}


sega8_janggun_device::sega8_janggun_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: device_t(mconfig, SEGA8_ROM_JANGGUN, "SMS Janggun Cart", tag, owner, clock, "sega8_janggun", __FILE__),
						device_sega8_cart_interface( mconfig, *this )
{
}


sega8_korean_device::sega8_korean_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: sega8_rom_device(mconfig, SEGA8_ROM_KOREAN, "SMS Korean Carts", tag, owner, clock, "sega8_korean", __FILE__)
{
}


sega8_korean_nb_device::sega8_korean_nb_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
					: sega8_rom_device(mconfig, SEGA8_ROM_KOREAN_NB, "SMS Korean No-Bank Mapper Carts", tag, owner, clock, "sega8_korean_nb", __FILE__)
{
}


void sega8_rom_device::device_start()
{
	save_item(NAME(m_rom_bank_base));
	save_item(NAME(m_ram_base));
	save_item(NAME(m_ram_enabled));
}

void sega8_rom_device::device_reset()
{
	m_ram_base = 0;
	m_ram_enabled = 0;
}


void sega8_eeprom_device::device_start()
{
	save_item(NAME(m_rom_bank_base));
	save_item(NAME(m_93c46_enabled));
	save_item(NAME(m_93c46_lines));
}

void sega8_eeprom_device::device_reset()
{
	m_93c46_lines = 0;
	m_93c46_enabled = 0;
}


void sega8_terebi_device::device_start()
{
	save_item(NAME(m_rom_bank_base));
	save_item(NAME(m_tvdraw_data));
}

void sega8_terebi_device::device_reset()
{
	m_tvdraw_data = 0;
}

void sega8_codemasters_device::device_start()
{
	save_item(NAME(m_rom_bank_base));
	save_item(NAME(m_ram_base));
	save_item(NAME(m_ram_enabled));
}

void sega8_codemasters_device::device_reset()
{
	m_ram_base = 0;
	m_ram_enabled = 0;
}

void sega8_4pak_device::device_start()
{
	save_item(NAME(m_rom_bank_base));
	save_item(NAME(m_reg));
}

void sega8_4pak_device::device_reset()
{
	memset(m_reg, 0, sizeof(m_reg));
}


void sega8_zemina_device::device_start()
{
	save_item(NAME(m_rom_bank_base));
	save_item(NAME(m_ram_base));
	save_item(NAME(m_ram_enabled));
}

void sega8_zemina_device::device_reset()
{
	m_ram_base = 0;
	m_ram_enabled = 0;
}


void sega8_janggun_device::device_start()
{
	save_item(NAME(m_rom_bank_base));
}


// initial bank setup needs to know how many 16K banks are available, so it needs to be called during cart loading...

void sega8_rom_device::late_bank_setup()
{
	m_rom_bank_base[0] = 0;
	m_rom_bank_base[1] = 1 & (m_rom_page_count - 1);
	m_rom_bank_base[2] = 2 & (m_rom_page_count - 1);
}

void sega8_eeprom_device::late_bank_setup()
{
	m_rom_bank_base[0] = 0;
	m_rom_bank_base[1] = 1 & (m_rom_page_count - 1);
	m_rom_bank_base[2] = 2 & (m_rom_page_count - 1);
}

void sega8_codemasters_device::late_bank_setup()
{
	m_rom_bank_base[0] = 0;
	m_rom_bank_base[1] = 1 & (m_rom_page_count - 1);
	m_rom_bank_base[2] = 0;
}

void sega8_zemina_device::late_bank_setup()
{
	m_rom_bank_base[0] = 0;
	m_rom_bank_base[1] = 1 & (m_rom_page_count * 2 - 1);
	m_rom_bank_base[2] = 2 & (m_rom_page_count * 2 - 1);
	m_rom_bank_base[3] = 3 & (m_rom_page_count * 2 - 1);
	m_rom_bank_base[4] = 4 & (m_rom_page_count * 2 - 1);
	m_rom_bank_base[5] = 5 & (m_rom_page_count * 2 - 1);
}

void sega8_nemesis_device::late_bank_setup()
{
	// Nemesis starts with last 8kb bank in page 0 (m_rom_page_count is for 16kb pages)
	m_rom_bank_base[0] = m_rom_page_count * 2 - 1;
	m_rom_bank_base[1] = 1 & (m_rom_page_count * 2 - 1);
	m_rom_bank_base[2] = 2 & (m_rom_page_count * 2 - 1);
	m_rom_bank_base[3] = 3 & (m_rom_page_count * 2 - 1);
	m_rom_bank_base[4] = 4 & (m_rom_page_count * 2 - 1);
	m_rom_bank_base[5] = 5 & (m_rom_page_count * 2 - 1);
}

void sega8_janggun_device::late_bank_setup()
{
	m_rom_bank_base[0] = 0;
	m_rom_bank_base[1] = 1;
	m_rom_bank_base[2] = 2;
	m_rom_bank_base[3] = 3;
	m_rom_bank_base[4] = 4;
	m_rom_bank_base[5] = 5;
}

void sega8_korean_device::late_bank_setup()
{
	m_rom_bank_base[0] = 0;
	m_rom_bank_base[1] = 1;
	m_rom_bank_base[2] = 2;
}



/*-------------------------------------------------
 mapper specific handlers
 -------------------------------------------------*/

/*-------------------------------------------------
 
 Base Sega 8bit carts, possibly with bankswitch
 
 -------------------------------------------------*/

READ8_MEMBER(sega8_rom_device::read_cart)
{
	int bank = offset / 0x4000;
	
	if (bank == 2 && m_ram && m_ram_enabled)
		return m_ram[m_ram_base * 0x4000 + (offset & 0x3fff)];

	if (offset < 0x400)	// first 1k is hardcoded
		return m_rom[offset];

	return m_rom[m_rom_bank_base[bank] * 0x4000 + (offset & 0x3fff)];
}

WRITE8_MEMBER(sega8_rom_device::write_cart)
{
	int bank = offset / 0x4000;
	
	if (bank == 2 && m_ram && m_ram_enabled)
		m_ram[m_ram_base * 0x4000 + (offset & 0x3fff)] = data;
}

WRITE8_MEMBER(sega8_rom_device::write_mapper)
{
	switch (offset)
	{
		case 0:
			if (data & 0x08)
			{
				// hack: if the RAM is ever enabled by the game, then we save it at exit...
				has_battery = 1;
				m_ram_enabled = 1;
				m_ram_base = (data & 0x04) >> 2;
			}
			else
				m_ram_enabled = 0;
			break;
			
		case 1: // Select 16k ROM bank for 0000-3fff
		case 2: // Select 16k ROM bank for 4000-7fff
		case 3: // Select 16k ROM bank for 8000-bfff
			m_rom_bank_base[offset - 1] = data  & (m_rom_page_count - 1);
			break;
	}	 
}

/*-------------------------------------------------
 
 Sega carts + EEPROM, used for some GameGear baseball
 games
 
 -------------------------------------------------*/


READ8_MEMBER(sega8_eeprom_device::read_cart)
{
	int bank = offset / 0x4000;
	
	if (offset == 0x8000 && m_93c46_enabled)
	{
		UINT8 value = (m_93c46_lines & 0xfc) | 0x02;
		value |= m_eeprom->read_bit() ? 1 : 0;
		return value;
	}
	
	if (offset < 0x400)	// first 1k is hardcoded
		return m_rom[offset];
	
	return m_rom[m_rom_bank_base[bank] * 0x4000 + (offset & 0x3fff)];
}

WRITE8_MEMBER(sega8_eeprom_device::write_cart)
{
	if (offset == 0x8000 && m_93c46_enabled)
	{
		m_93c46_lines = data;
		m_eeprom->write_bit((data & 0x01) ? ASSERT_LINE : CLEAR_LINE);
		m_eeprom->set_cs_line(!(data & 0x04) ? ASSERT_LINE : CLEAR_LINE);
		m_eeprom->set_clock_line((data & 0x02) ? ASSERT_LINE : CLEAR_LINE);
	}
}

WRITE8_MEMBER(sega8_eeprom_device::write_mapper)
{
	switch (offset)
	{
		case 0:
			if (data & 0x80)
			{
				m_eeprom->reset();
				logerror("eeprom CS = 1\n");
				m_eeprom->set_cs_line(ASSERT_LINE);
			}
			m_93c46_enabled = BIT(data, 3);
			logerror("eeprom %s\n", m_93c46_enabled ? "enabled" : "disabled");
			break;
			
		case 1: // Select 16k ROM bank for 0000-3fff
		case 2: // Select 16k ROM bank for 4000-7fff
		case 3: // Select 16k ROM bank for 8000-bfff
			m_rom_bank_base[offset - 1] = data  & (m_rom_page_count - 1);
			break;
	}	 
}

MACHINE_CONFIG_FRAGMENT( gg_eeprom )
	MCFG_EEPROM_93C46_ADD("eeprom")
MACHINE_CONFIG_END

machine_config_constructor sega8_eeprom_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( gg_eeprom );
}


/*-------------------------------------------------
 
 Sega 8-bit cart + Tablet input device, used for 
 SG-1000 Terebi Oekaki (compatible with Mark III)
 
 -------------------------------------------------*/

READ8_MEMBER(sega8_terebi_device::read_cart)
{
	int bank = offset / 0x4000;
	
	if (offset == 0x8000)
		return m_tvdraw_pen->read();
	if (offset == 0xa000)
		return m_tvdraw_data;
	
	return m_rom[m_rom_bank_base[bank] * 0x4000 + (offset & 0x3fff)];
}

WRITE8_MEMBER(sega8_terebi_device::write_cart)
{
	switch (offset)
	{
		case 0x6000:
			if (data & 0x01)
			{
				m_tvdraw_data = m_tvdraw_x->read();
				
				if (m_tvdraw_data < 4) m_tvdraw_data = 4;
				if (m_tvdraw_data > 251) m_tvdraw_data = 251;
			}
			else
				m_tvdraw_data = m_tvdraw_y->read() + 0x20;
			break;
		case 0xa000:
			// effect unknown
			break;
	}
}

static INPUT_PORTS_START( tvdraw )
	PORT_START("TVDRAW_X")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_NAME("Tablet - X Axis") PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("TVDRAW_Y")
	PORT_BIT( 0xff, 0x60, IPT_LIGHTGUN_Y ) PORT_NAME("Tablet - Y Axis") PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_MINMAX(0, 191) PORT_SENSITIVITY(50) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("TVDRAW_PEN")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("Tablet - Pen")
INPUT_PORTS_END

ioport_constructor sega8_terebi_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( tvdraw );
}


/*-------------------------------------------------
 
 Codemasters carts, possibly having on cart RAM
 (Ernie Els Golf)
 
 -------------------------------------------------*/

READ8_MEMBER(sega8_codemasters_device::read_cart)
{
	int bank = offset / 0x2000;
	
	if (bank == 5 && m_ram && m_ram_enabled)
		return m_ram[m_ram_base * 0x2000 + (offset & 0x1fff)];
	
	return m_rom[m_rom_bank_base[bank/2] * 0x4000 + (offset & 0x3fff)];
}

WRITE8_MEMBER(sega8_codemasters_device::write_cart)
{
	int bank = offset / 0x2000;

	switch (offset)
	{
		case 0x0000:
			m_rom_bank_base[0] = data & (m_rom_page_count - 1);
			break;
		case 0x4000:
			if (data & 0x80)
			{
				m_ram_enabled = 1;
				m_ram_base = data & 0x07;
			}
			else
			{
				m_ram_enabled = 0;
				m_rom_bank_base[1] = data & (m_rom_page_count - 1);
			}
			break;
		case 0x8000:
			m_rom_bank_base[2] = data & (m_rom_page_count - 1);
			break;
	}

	if (bank == 5 && m_ram && m_ram_enabled)
		m_ram[m_ram_base * 0x2000 + (offset & 0x1fff)] = data;
}

/*-------------------------------------------------
 
 HES 4 PAK All Action cart
 
 -------------------------------------------------*/


WRITE8_MEMBER(sega8_4pak_device::write_cart)
{
	switch (offset)
	{
		case 0x3ffe:
			m_reg[0] = data;
			m_rom_bank_base[0] = data & (m_rom_page_count - 1);
			m_rom_bank_base[2] = ((m_reg[0] & 0x30) + m_reg[2])  & (m_rom_page_count - 1);
			break;
		case 0x7fff:
			m_reg[1] = data;
			m_rom_bank_base[1] = data & (m_rom_page_count - 1);
			break;
		case 0xbfff:
			m_reg[2] = data;
			m_rom_bank_base[2] = ((m_reg[0] & 0x30) + m_reg[2])  & (m_rom_page_count - 1);
			break;
	}
}


/*-------------------------------------------------
 
 base Zemina carts, allowing for 8K bankswitch
 
 -------------------------------------------------*/

READ8_MEMBER(sega8_zemina_device::read_cart)
{
	int bank = offset / 0x2000;
	
	if (bank >= 4 && m_ram && m_ram_enabled)
		return m_ram[m_ram_base * 0x2000 + (offset & 0x1fff)];
	
	return m_rom[m_rom_bank_base[bank] * 0x2000 + (offset & 0x1fff)];
}

WRITE8_MEMBER(sega8_zemina_device::write_cart)
{
	int bank = offset / 0x2000;
	
	if (bank >= 4 && m_ram && m_ram_enabled)
		m_ram[m_ram_base * 0x2000 + (offset & 0x1fff)] = data;

	if (offset < 4)
	{
		switch (offset & 3)
		{
			case 0:
				m_rom_bank_base[4] = data & (m_rom_page_count * 2 - 1);
				break;
			case 1:
				m_rom_bank_base[5] = data & (m_rom_page_count * 2 - 1);
				break;
			case 2:
				m_rom_bank_base[2] = data & (m_rom_page_count * 2 - 1);
				break;
			case 3:
				m_rom_bank_base[3] = data & (m_rom_page_count * 2 - 1);
				break;
		}
	}
}

/*-------------------------------------------------
 
 Zemina cart used for Nemesis: same as above, but
 different bank layout at start (see late_bank_setup)
 
 -------------------------------------------------*/

/*-------------------------------------------------
 
 Daiou cart used for SMS Janggun-ui Adeul
 
 the game expects to access 256 x 8K banks:
 first 64 are just the game, second 64 are a mirror of the first ones
 upper 128 are the same as the previous but with bytes in reverse order
 probably as a shortcut to get sprite flipping for free from hw
 so if bit7 of current bank page is set, we swap the result
 
 -------------------------------------------------*/

READ8_MEMBER(sega8_janggun_device::read_cart)
{
	int bank = offset / 0x2000;
	
	if (m_rom_bank_base[bank] < 0x80)
		return m_rom[(m_rom_bank_base[bank] & 0x3f) * 0x2000 + (offset & 0x1fff)];
	else
		return BITSWAP8(m_rom[(m_rom_bank_base[bank] & 0x3f) * 0x2000 + (offset & 0x1fff)], 0, 1, 2, 3, 4, 5, 6, 7);
}


WRITE8_MEMBER(sega8_janggun_device::write_cart)
{
	switch (offset)
	{
		case 0x4000:
			m_rom_bank_base[2] = data;
			break;
		case 0x6000:
			m_rom_bank_base[3] = data;
			break;
		case 0x8000:
			m_rom_bank_base[4] = data;
			break;
		case 0xa000:
			m_rom_bank_base[5] = data;
			break;
	}
}

WRITE8_MEMBER(sega8_janggun_device::write_mapper)
{
	switch (offset)
	{
		case 0:
			break;
			
		case 1: // Select 16k ROM bank for 0000-3fff
		case 2: // Select 16k ROM bank for 4000-7fff
		case 3: // Select 16k ROM bank for 8000-bfff
			m_rom_bank_base[(offset - 1) * 2] = (data  & (m_rom_page_count - 1)) * 2;
			m_rom_bank_base[(offset - 1) * 2 + 1] = (data  & (m_rom_page_count - 1)) * 2 + 1;
			break;
	}	 
}


/*-------------------------------------------------
 
 Korean cart, used e.g. in Dodgeball King, 
 uses writes to 0xa000 for bankswitch
 
 -------------------------------------------------*/

WRITE8_MEMBER(sega8_korean_device::write_cart)
{
	int bank = offset / 0x4000;
	
	if (bank == 2 && m_ram && m_ram_enabled)
		m_ram[m_ram_base * 0x4000 + (offset & 0x3fff)] = data;

	if (offset == 0xa000)
		m_rom_bank_base[2] = data % m_rom_page_count;
}
