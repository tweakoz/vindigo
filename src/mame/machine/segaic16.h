/***************************************************************************

    Sega 16-bit common hardware

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

#pragma once

#ifndef __SEGAIC16_H__
#define __SEGAIC16_H__

#include "cpu/m68000/m68000.h"
#include "machine/fd1089.h"
#include "machine/fd1094.h"


//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_SEGA_315_5195_MAPPER_ADD(_tag, _cputag, _class, _mapper, _read, _write) \
	MCFG_DEVICE_ADD(_tag, SEGA_MEM_MAPPER, 0) \
	sega_315_5195_mapper_device::static_set_cputag(*device, _cputag); \
	sega_315_5195_mapper_device::static_set_mapper(*device, sega_315_5195_mapper_device::mapper_delegate(&_class::_mapper, #_class "::" #_mapper, NULL, (_class *)0)); \
	sega_315_5195_mapper_device::static_set_sound_readwrite(*device, sega_315_5195_mapper_device::sound_read_delegate(&_class::_read, #_class "::" #_read, NULL, (_class *)0), sega_315_5195_mapper_device::sound_write_delegate(&_class::_write, #_class "::" #_write, NULL, (_class *)0)); \



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


// ======================> sega_315_5195_mapper_device

class sega_315_5195_mapper_device : public device_t
{
public:
	typedef device_delegate<void (sega_315_5195_mapper_device &, UINT8)> mapper_delegate;
	typedef device_delegate<UINT8 ()> sound_read_delegate;
	typedef device_delegate<void (UINT8)> sound_write_delegate;

	// construction/destruction
	sega_315_5195_mapper_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// inline configuration helpers
	static void static_set_cputag(device_t &device, const char *cpu);
	static void static_set_mapper(device_t &device, mapper_delegate callback);
	static void static_set_sound_readwrite(device_t &device, sound_read_delegate read, sound_write_delegate write);

	// public interface
	DECLARE_READ8_MEMBER( read );
	DECLARE_WRITE8_MEMBER( write );

	// mapping helpers
	void map_as_rom(UINT32 offset, UINT32 length, offs_t mirror, const char *bank_name, offs_t rgnoffset, write16_delegate whandler);
	void map_as_ram(UINT32 offset, UINT32 length, offs_t mirror, const char *bank_share_name, write16_delegate whandler);
	void map_as_handler(UINT32 offset, UINT32 length, offs_t mirror, read16_delegate rhandler, write16_delegate whandler);

	// perform an explicit configuration (for bootlegs with hard-coded mappings)
	void configure_explicit(const UINT8 *map_data);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

private:
	// internal region struct
	struct region_info
	{
		offs_t	size_mask;
		offs_t	base;
		offs_t	mirror;
		offs_t	start;
		offs_t	end;
	};

	// helper class for tracking banks mapped to ROM regions
	class decrypt_bank
	{
	public:
		// construction/destruction
		decrypt_bank();
		~decrypt_bank();
	
		// configuration
		void set_decrypt(fd1089_base_device *fd1089);
		void set_decrypt(fd1094_device *fd1094);
		void clear() { set(NULL, 0, 0, ~0, NULL); }
		void set(memory_bank *bank, offs_t start, offs_t end, offs_t rgnoffs, UINT8 *src);

		// updating
		void update();
		void reset() { m_fd1089_decrypted.reset(); if (m_fd1094_cache != NULL) m_fd1094_cache->reset(); }

	private:
		// internal state
		memory_bank *			m_bank;
		offs_t					m_start;
		offs_t					m_end;
		offs_t					m_rgnoffs;
		UINT8 *					m_srcptr;
		fd1089_base_device *	m_fd1089;
		dynamic_array<UINT16>	m_fd1089_decrypted;
		fd1094_decryption_cache	*m_fd1094_cache;
	};

	// internal helpers
	void compute_region(region_info &info, UINT8 index, UINT32 length, UINT32 mirror, UINT32 offset = 0);
	void update_mapping();
	void fd1094_state_change(UINT8 state);

	// configuration
	const char *				m_cputag;
	mapper_delegate				m_mapper;
	sound_read_delegate			m_sound_read;
	sound_write_delegate		m_sound_write;

	// internal state
	m68000_device *				m_cpu;
	address_space *				m_space;
	UINT8						m_regs[0x20];
	UINT8						m_curregion;
	decrypt_bank				m_banks[8];
};


// device type definition
extern const device_type SEGA_MEM_MAPPER;




#include "devlegcy.h"

/* open bus read helpers */
READ16_HANDLER( segaic16_open_bus_r );

/*** Sega 16-bit Devices ***/

#include "devcb.h"

/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

typedef void (*_315_5250_sound_callback)(running_machine &, UINT8);
typedef void (*_315_5250_timer_ack_callback)(running_machine &);

typedef struct _ic_315_5250_interface ic_315_5250_interface;
struct _ic_315_5250_interface
{
	_315_5250_sound_callback          sound_write_callback;
	_315_5250_timer_ack_callback      timer_ack_callback;
};


/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/

DECLARE_LEGACY_DEVICE(_315_5248, ic_315_5248);
DECLARE_LEGACY_DEVICE(_315_5249, ic_315_5249);
DECLARE_LEGACY_DEVICE(_315_5250, ic_315_5250);

#define MCFG_315_5248_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, _315_5248, 0)

#define MCFG_315_5249_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, _315_5249, 0)

#define MCFG_315_5250_ADD(_tag, _interface) \
	MCFG_DEVICE_ADD(_tag, _315_5250, 0) \
	MCFG_DEVICE_CONFIG(_interface)


/***************************************************************************
    DEVICE I/O FUNCTIONS
***************************************************************************/

/* multiply chip */
READ16_DEVICE_HANDLER( segaic16_multiply_r );
WRITE16_DEVICE_HANDLER( segaic16_multiply_w );

/* divide chip */
READ16_DEVICE_HANDLER( segaic16_divide_r );
WRITE16_DEVICE_HANDLER( segaic16_divide_w );

/* compare/timer chip */
int segaic16_compare_timer_clock( device_t *device );
READ16_DEVICE_HANDLER( segaic16_compare_timer_r );
WRITE16_DEVICE_HANDLER( segaic16_compare_timer_w );


#endif
