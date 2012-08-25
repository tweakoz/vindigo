/***************************************************************************

Mitsubishi M50458 OSD chip

preliminary device by Angelo Salese

***************************************************************************/

#include "emu.h"
#include "video/m50458.h"



//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

// device type definition
const device_type M50458 = &device_creator<m50458_device>;

static ADDRESS_MAP_START( m50458_vram, AS_0, 16, m50458_device )
	AM_RANGE(0x0000, 0x023f) AM_RAM // vram
	AM_RANGE(0x0240, 0x0241) AM_WRITE(vreg_120_w)
	AM_RANGE(0x024c, 0x024d) AM_WRITE(vreg_126_w)
	AM_RANGE(0x024e, 0x024f) AM_WRITE(vreg_127_w)
ADDRESS_MAP_END

// internal GFX ROM (TODO: GFXs in here should be 12x18, not 16x18)
ROM_START( m50458 )
	ROM_REGION( 0x1200, "m50458", 0 )
	ROM_LOAD("m50458_char.bin",     0x0000, 0x1200, BAD_DUMP CRC(011cc342) SHA1(d5b9f32d6e251b4b25945267d7c68c099bd83e96) )
ROM_END

WRITE16_MEMBER( m50458_device::vreg_120_w)
{
	// ...
}

WRITE16_MEMBER( m50458_device::vreg_126_w)
{
	// ...
}


WRITE16_MEMBER( m50458_device::vreg_127_w)
{
	if(data & 0x20) // RAMERS, display RAM is erased
	{
		int i;

		for(i=0;i<0x120;i++)
			write_word(i,0);
	}
}

//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const rom_entry *m50458_device::device_rom_region() const
{
	return ROM_NAME( m50458 );
}

//-------------------------------------------------
//  memory_space_config - return a description of
//  any address spaces owned by this device
//-------------------------------------------------

const address_space_config *m50458_device::memory_space_config(address_spacenum spacenum) const
{
	return (spacenum == AS_0) ? &m_space_config : NULL;
}

//**************************************************************************
//  INLINE HELPERS
//**************************************************************************

//-------------------------------------------------
//  readbyte - read a byte at the given address
//-------------------------------------------------

inline UINT16 m50458_device::read_word(offs_t address)
{
	return space()->read_word(address << 1);
}

//-------------------------------------------------
//  writebyte - write a byte at the given address
//-------------------------------------------------

inline void m50458_device::write_word(offs_t address, UINT16 data)
{
	space()->write_word(address << 1, data);
}


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  m50458_device - constructor
//-------------------------------------------------

m50458_device::m50458_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, M50458, "m50458", tag, owner, clock),
	  device_memory_interface(mconfig, *this),
	  m_space_config("videoram", ENDIANNESS_LITTLE, 16, 16, 0, NULL, *ADDRESS_MAP_NAME(m50458_vram))
{
	m_shortname = "m50458";
}


//-------------------------------------------------
//  device_validity_check - perform validity checks
//  on this device
//-------------------------------------------------

void m50458_device::device_validity_check(validity_checker &valid) const
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void m50458_device::device_start()
{

}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void m50458_device::device_reset()
{
}


//**************************************************************************
//  READ/WRITE HANDLERS
//**************************************************************************

WRITE_LINE_MEMBER( m50458_device::write_bit )
{
	m_latch = state;
}

WRITE_LINE_MEMBER( m50458_device::set_cs_line )
{
	m_reset_line = state;

	if(m_reset_line != CLEAR_LINE)
	{
		//printf("Reset asserted\n");
		m_cmd_stream_pos = 0;
		m_current_cmd = 0;
		m_osd_state = OSD_SET_ADDRESS;
	}
}

/*
0x7e: "JUST A MOMENT"
0x81: "NON SLOT"
*/

WRITE_LINE_MEMBER( m50458_device::set_clock_line )
{
	if (m_reset_line == CLEAR_LINE)
	{
		if(state == 1)
		{
			//printf("%d\n",m_latch);

			m_current_cmd = (m_current_cmd >> 1) | ((m_latch<<15)&0x8000);
			m_cmd_stream_pos++;

			if(m_cmd_stream_pos == 16)
			{
				switch(m_osd_state)
				{
					case OSD_SET_ADDRESS:
						m_osd_addr = m_current_cmd;
						m_osd_state = OSD_SET_DATA;
						break;
					case OSD_SET_DATA:
						//if(m_osd_addr >= 0x120)
						//printf("%04x %04x\n",m_osd_addr,m_current_cmd);
						write_word(m_osd_addr,m_current_cmd);
						m_osd_addr++;
						/* Presumably wraps at 0x127? */
						if(m_osd_addr > 0x127) { m_osd_addr = 0; }
						break;
				}

				m_cmd_stream_pos = 0;
				m_current_cmd = 0;
			}
		}
	}
}

//-------------------------------------------------
//  update_screen -
//-------------------------------------------------

UINT32 m50458_device::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	int x,y;
	UINT8 *pcg = memregion("m50458")->base();

	for(y=0;y<12;y++)
	{
		for(x=0;x<24;x++)
		{
			int xi,yi;
			UINT16 tile;

			tile = read_word(x+y*24);

			for(yi=0;yi<18;yi++)
			{
				for(xi=4;xi<16;xi++) /* TODO: remove 4 / 16 / -4 offset once that the ROM is fixed */
				{
					UINT8 pix;
					UINT8 r,g,b;
					UINT16 offset = ((tile & 0x7f)*36+yi*2);

					if(xi>=8)
						pix = (pcg[offset+1] >> (7-(xi & 0x7))) & 1;
					else
						pix = (pcg[offset+0] >> (7-(xi & 0x7))) & 1;

					r = (tile & 0x100 && pix) ? 0xff : 0x00;
					g = (tile & 0x200 && pix) ? 0xff : 0x00;
					b = (tile & 0x400 && pix) ? 0xff : 0x00;

					bitmap.pix32(y*18+yi,x*12+(xi-4)) = r << 16 | g << 8 | b;
				}
			}
		}
	}

	return 0;
}
