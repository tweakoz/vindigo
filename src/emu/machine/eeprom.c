/***************************************************************************

    eeprom.c

    Base class for EEPROM devices.

****************************************************************************

    Copyright Aaron Giles
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

#include "emu.h"
#include "machine/eeprom.h"



//**************************************************************************
//  DEBUGGING
//**************************************************************************

#define VERBOSE 0
#define LOG(x) do { if (VERBOSE) logerror x; } while (0)



//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

static ADDRESS_MAP_START( eeprom_map8, AS_PROGRAM, 8, base_eeprom_device )
	AM_RANGE(0x0000, 0x0fff) AM_RAM
ADDRESS_MAP_END


static ADDRESS_MAP_START( eeprom_map16, AS_PROGRAM, 16, base_eeprom_device )
	AM_RANGE(0x0000, 0x07ff) AM_RAM
ADDRESS_MAP_END



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  base_eeprom_device - constructor
//-------------------------------------------------

base_eeprom_device::base_eeprom_device(const machine_config &mconfig, device_type devtype, const char *name, const char *tag, device_t *owner, const char *shortname, const char *file)
	: device_t(mconfig, devtype, name, tag, owner, 0, shortname, file),
		device_memory_interface(mconfig, *this),
		device_nvram_interface(mconfig, *this),
		m_cells(0),
		m_address_bits(0),
		m_data_bits(0),
		m_default_data(0),
		m_default_data_size(0),
		m_default_value(0),
		m_default_value_set(false)
{
}


//-------------------------------------------------
//  static_set_default_data - configuration helpers
//  to set the default data
//-------------------------------------------------

void base_eeprom_device::static_set_size(device_t &device, int cells, int cellbits)
{
	base_eeprom_device &eeprom = downcast<base_eeprom_device &>(device);
	eeprom.m_cells = cells;
	eeprom.m_data_bits = cellbits;

	// compute address bits (validation checks verify cells was an even power of 2)
	cells--;
	eeprom.m_address_bits = 0;
	while (cells != 0)
	{
		cells >>= 1;
		eeprom.m_address_bits++;
	}

	// describe our address space
	if (eeprom.m_data_bits == 8)
		eeprom.m_space_config = address_space_config("eeprom", ENDIANNESS_BIG, 8,  eeprom.m_address_bits, 0, *ADDRESS_MAP_NAME(eeprom_map8));
	else
		eeprom.m_space_config = address_space_config("eeprom", ENDIANNESS_BIG, 16, eeprom.m_address_bits * 2, 0, *ADDRESS_MAP_NAME(eeprom_map16));
}


//-------------------------------------------------
//  static_set_default_data - configuration helpers
//  to set the default data
//-------------------------------------------------

void base_eeprom_device::static_set_default_data(device_t &device, const UINT8 *data, UINT32 size)
{
	base_eeprom_device &eeprom = downcast<base_eeprom_device &>(device);
	assert(eeprom.m_cellbits == 8);
	eeprom.m_default_data.u8 = const_cast<UINT8 *>(data);
	eeprom.m_default_data_size = size;
}

void base_eeprom_device::static_set_default_data(device_t &device, const UINT16 *data, UINT32 size)
{
	base_eeprom_device &eeprom = downcast<base_eeprom_device &>(device);
	assert(eeprom.m_cellbits == 16);
	eeprom.m_default_data.u16 = const_cast<UINT16 *>(data);
	eeprom.m_default_data_size = size / 2;
}


//-------------------------------------------------
//  static_set_default_value - configuration helper
//  to set the default value
//-------------------------------------------------

void base_eeprom_device::static_set_default_value(device_t &device, UINT32 value)
{
	base_eeprom_device &eeprom = downcast<base_eeprom_device &>(device);
	eeprom.m_default_value = value;
	eeprom.m_default_value_set = true;
}


//-------------------------------------------------
//  read_data - read data at the given address
//-------------------------------------------------

UINT32 base_eeprom_device::read_data(offs_t address)
{
	if (m_data_bits == 16)
		return m_addrspace[0]->read_word(address * 2);
	else
		return m_addrspace[0]->read_byte(address);
}


//-------------------------------------------------
//  write_data - write data at the given address
//-------------------------------------------------

void base_eeprom_device::write_data(offs_t address, UINT32 data)
{
	if (m_data_bits == 16)
		m_addrspace[0]->write_word(address * 2, data);
	else
		m_addrspace[0]->write_byte(address, data);
}


//-------------------------------------------------
//  device_validity_check - perform validity checks
//  on this device
//-------------------------------------------------

void base_eeprom_device::device_validity_check(validity_checker &valid) const
{
	// ensure the number of cells is an even power of 2
	if (m_cells != (1 << m_address_bits))
		mame_printf_error("Invalid EEPROM size %d specified\n", m_cells);

	// ensure only the sizes we support are requested
	if (m_data_bits != 8 && m_data_bits != 16)
		mame_printf_error("Invalid EEPROM data width %d specified\n", m_data_bits);
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void base_eeprom_device::device_start()
{
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void base_eeprom_device::device_reset()
{
}


//-------------------------------------------------
//  memory_space_config - return a description of
//  any address spaces owned by this device
//-------------------------------------------------

const address_space_config *base_eeprom_device::memory_space_config(address_spacenum spacenum) const
{
	return (spacenum == 0) ? &m_space_config : NULL;
}


//-------------------------------------------------
//  nvram_default - called to initialize NVRAM to
//  its default state
//-------------------------------------------------

void base_eeprom_device::nvram_default()
{
	UINT32 eeprom_length = 1 << m_address_bits;
	UINT32 eeprom_bytes = eeprom_length * m_data_bits / 8;

	// initialize to the default value
	UINT32 default_value = m_default_value_set ? m_default_value : ~0;
	for (offs_t offs = 0; offs < eeprom_length; offs++)
		if (m_data_bits == 8)
			m_addrspace[0]->write_byte(offs, default_value);
		else
			m_addrspace[0]->write_word(offs * 2, default_value);

	// handle hard-coded data from the driver
	if (m_default_data.u8 != NULL)
		for (offs_t offs = 0; offs < m_default_data_size; offs++) 
		{
			if (m_data_bits == 8)
				m_addrspace[0]->write_byte(offs, m_default_data.u8[offs]);
			else
				m_addrspace[0]->write_word(offs * 2, m_default_data.u16[offs]);
		}

	// populate from a memory region if present
	if (m_region != NULL)
	{
		if (m_region->bytes() != eeprom_bytes)
			fatalerror("eeprom region '%s' wrong size (expected size = 0x%X)\n", tag(), eeprom_bytes);
		if (m_data_bits == 8 && m_region->width() != 1)
			fatalerror("eeprom region '%s' needs to be an 8-bit region\n", tag());
		if (m_data_bits == 16 && (m_region->width() != 2 || m_region->endianness() != ENDIANNESS_BIG))
			fatalerror("eeprom region '%s' needs to be a 16-bit big-endian region\n", tag());

		for (offs_t offs = 0; offs < eeprom_length; offs++)
			if (m_data_bits == 8)
				m_addrspace[0]->write_byte(offs, m_region->u8(offs));
			else
				m_addrspace[0]->write_word(offs * 2, m_region->u16(offs));
	}
}


//-------------------------------------------------
//  nvram_read - called to read NVRAM from the
//  .nv file
//-------------------------------------------------

void base_eeprom_device::nvram_read(emu_file &file)
{
	UINT32 eeprom_length = 1 << m_address_bits;
	UINT32 eeprom_bytes = eeprom_length * m_data_bits / 8;

	dynamic_buffer buffer(eeprom_bytes);
	file.read(buffer, eeprom_bytes);
	for (offs_t offs = 0; offs < eeprom_bytes; offs++)
		m_addrspace[0]->write_byte(offs, buffer[offs]);
}


//-------------------------------------------------
//  nvram_write - called to write NVRAM to the
//  .nv file
//-------------------------------------------------

void base_eeprom_device::nvram_write(emu_file &file)
{
	UINT32 eeprom_length = 1 << m_address_bits;
	UINT32 eeprom_bytes = eeprom_length * m_data_bits / 8;

	dynamic_buffer buffer(eeprom_bytes);
	for (offs_t offs = 0; offs < eeprom_bytes; offs++)
		buffer[offs] = m_addrspace[0]->read_byte(offs);
	file.write(buffer, eeprom_bytes);
}
