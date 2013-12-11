// license:MAME|LGPL-2.1+
// copyright-holders:Michael Zapf
/*******************************************************************************
    SNUG BwG Disk Controller
    Based on WD1773
    Double Density, Double-sided

    * Supports Double Density.
    * As this card includes its own RAM, it does not need to allocate a portion
      of VDP RAM to store I/O buffers.
    * Includes a MM58274C RTC.
    * Support an additional floppy drive, for a total of 4 floppies.

    Reference:
    * BwG Disketten-Controller: Beschreibung der DSR (Description of the DSR)
        <http://home.t-online.de/home/harald.glaab/snug/bwg.pdf>

    +------------------------+
    |   32 KiB EPROM         | --- 1 of 4 pages--> 4000  +------------------+
    |                        |                           |   DSR space      |
    +------------------------+                           |   (Driver)       |
    |   2 KiB  RAM           | --- 1 of 2 pages--> 5c00  +------------------+
    +------------------------+                           |   RAM buffer     |
                                                   5fe0  +------------------+
                                                         |   RTC or WD1773  |
                                                   5fff  +------------------+

    Michael Zapf, September 2010
    January 2012: rewritten as class (MZ)

    Known issues (Dec 2013):

  1. The BwG controller may fail to read files from single density disks. This
     is because the DSR tries to read the disk as double density first, which
     usually fails for a single density disk image - unless we are in track 0.
     The fix would be to enhance disk images in a way to tell the controller
     whether they are single or double density and to refuse delivering data
     when accessed wrongly.

  2. The BwG controller cannot run with the Geneve or other non-9900 computers.
     The reason for that is the wait state logic. It assumes that when
     executing MOVB @>5FF6,*R2, first a value from 5FF7 is attempted to be read,
     just as the TI console does. In that case, wait states are inserted if
     necessary. The Geneve, however, will try to read a single byte from 5FF6
     only and therefore circumvent the wait state generation. This is in fact
     not an emulation glitch but the behavior of the real expansion card.

*******************************************************************************/

#include "emu.h"
#include "peribox.h"
#include "bwg.h"
#include "formats/ti99_dsk.h"
#include "imagedev/flopdrv.h"

// ----------------------------------
// Flags for debugging

// Show read and write accesses
#define TRACE_RW 0

// Show CRU bit accesses
#define TRACE_CRU 0

// Show ready line activity
#define TRACE_READY 0

// Show detailed signal activity
#define TRACE_SIGNALS 0

// Show sector data
#define TRACE_DATA 0

// Show address bus operations
#define TRACE_ADDRESS 0
// ----------------------------------

#define MOTOR_TIMER 1
#define FDC_TAG "wd1773"
#define CLOCK_TAG "mm58274c"

#define BUFFER "ram"

snug_bwg_device::snug_bwg_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
			: ti_expansion_card_device(mconfig, TI99_BWG, "SNUG BwG Floppy Controller", tag, owner, clock, "ti99_bwg", __FILE__),
				m_wd1773(*this, FDC_TAG),
				m_clock(*this, CLOCK_TAG) { }

/*
    Callback called at the end of DVENA pulse
*/
void snug_bwg_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	m_DVENA = CLEAR_LINE;
	set_ready_line();
}


/*
    Operate the wait state logic.
*/
void snug_bwg_device::set_ready_line()
{
	// This is the wait state logic
	if (TRACE_SIGNALS) logerror("bwg: address=%04x, DRQ=%d, INTRQ=%d, MOTOR=%d\n", m_address & 0xffff, m_DRQ, m_IRQ, m_DVENA);
	line_state nready = (m_dataregLB &&         // Are we accessing 5ff7
			m_WAITena &&                        // and the wait state generation is active (SBO 2)
			(m_DRQ==CLEAR_LINE) &&              // and we are waiting for a byte
			(m_IRQ==CLEAR_LINE) &&              // and there is no interrupt yet
			(m_DVENA==ASSERT_LINE)              // and the motor is turning?
			)? ASSERT_LINE : CLEAR_LINE;        // In that case, clear READY and thus trigger wait states

	if (TRACE_READY) if (nready==ASSERT_LINE) logerror("bwg: READY line = %d\n", (nready==CLEAR_LINE)? 1:0);
	m_slot->set_ready((nready==CLEAR_LINE)? ASSERT_LINE : CLEAR_LINE);
}

/*
    Callback, called from the controller chip whenever DRQ/IRQ state change
*/
WRITE_LINE_MEMBER( snug_bwg_device::intrq_w )
{
	if (TRACE_SIGNALS) logerror("bwg: set intrq = %d\n", state);
	m_IRQ = (line_state)state;

	// Note that INTB is actually not used in the TI-99 family. But the
	// controller asserts the line nevertheless, probably intended for
	// use in another planned TI system
	m_slot->set_intb(state==ASSERT_LINE);

	// We need to explicitly set the READY line to release the datamux
	set_ready_line();
}

WRITE_LINE_MEMBER( snug_bwg_device::drq_w )
{
	if (TRACE_SIGNALS) logerror("bwg: set drq = %d\n", state);
	m_DRQ = (line_state)state;

	// We need to explicitly set the READY line to release the datamux
	set_ready_line();
}

SETADDRESS_DBIN_MEMBER( snug_bwg_device::setaddress_dbin )
{
	// Selection login in the PAL and some circuits on the board

	// Is the card being selected?
	m_address = offset;
	m_inDsrArea = ((m_address & m_select_mask)==m_select_value);

	if (!m_inDsrArea) return;

	if (TRACE_ADDRESS) logerror("bwg: set address = %04x\n", offset & 0xffff);

	// Is the WD chip on the card being selected?
	// We need the even and odd addresses for the wait state generation,
	// but only the even addresses when we access it
	m_WDsel0 = m_inDsrArea && !m_rtc_enabled
			&& ((state==ASSERT_LINE && ((m_address & 0x1ff8)==0x1ff0))    // read
				|| (state==CLEAR_LINE && ((m_address & 0x1ff8)==0x1ff8)));  // write

	m_WDsel = m_WDsel0 && ((m_address & 1)==0);

	// Is the RTC selected on the card? (even addr)
	m_RTCsel = m_inDsrArea && m_rtc_enabled && ((m_address & 0x1fe1)==0x1fe0);

	// RTC disabled:
	// 5c00 - 5fef: RAM
	// 5ff0 - 5fff: Controller (f0 = status, f2 = track, f4 = sector, f6 = data)

	// RTC enabled:
	// 5c00 - 5fdf: RAM
	// 5fe0 - 5fff: Clock (even addr)

	// Is RAM selected? We just check for the last 1K and let the RTC or WD
	// just take control before
	m_lastK = m_inDsrArea && ((m_address & 0x1c00)==0x1c00);

	// Is the data register port of the WD being selected?
	// In fact, the address to read the data from is 5FF6, but the TI-99 datamux
	// fetches both bytes from 5FF7 and 5FF6, the odd one first. The BwG uses
	// the odd address to operate the READY line
	m_dataregLB = m_WDsel0 && ((m_address & 0x07)==0x07);

	// Clear or assert the outgoing READY line
	set_ready_line();
}

/*
    Read a byte from ROM, RAM, FDC, or RTC. See setaddress_dbin for selection
    logic.
*/
READ8Z_MEMBER(snug_bwg_device::readz)
{
	if (m_inDsrArea && m_selected)
	{
		// 010x xxxx xxxx xxxx
		if (m_lastK)
		{
			// ...1 11xx xxxx xxxx
			if (m_rtc_enabled)
			{
				if (m_RTCsel)
				{
					// .... ..11 111x xxx0
					if (!space.debugger_access()) *value = m_clock->read(space, (m_address & 0x001e) >> 1);
					if (TRACE_RW) logerror("bwg: read RTC: %04x -> %02x\n", m_address & 0xffff, *value);
				}
				else
				{
					*value = m_buffer_ram[(m_ram_page<<10) | (m_address & 0x03ff)];
					if (TRACE_RW) logerror("bwg: read ram: %04x (page %d)-> %02x\n", m_address & 0xffff, m_ram_page, *value);
				}
			}
			else
			{
				if (m_WDsel)
				{
					// .... ..11 1111 0xx0
					// Note that the value is inverted again on the board,
					// so we can drop the inversion
					if (!space.debugger_access()) *value = wd17xx_r(m_wd1773, space, (m_address >> 1)&0x03);
					if (TRACE_RW) logerror("bwg: read FDC: %04x -> %02x\n", m_address & 0xffff, *value);
					if (TRACE_DATA)
					{
						if ((m_address & 0xffff)==0x5ff6) logerror("%02x ", *value);
						else logerror("\n%04x: %02x", m_address&0xffff, *value);
					}
				}
				else
				{
					*value = m_buffer_ram[(m_ram_page<<10) | (m_address & 0x03ff)];
					if (TRACE_RW) logerror("bwg: read ram: %04x (page %d)-> %02x\n", m_address & 0xffff, m_ram_page, *value);
				}
			}
		}
		else
		{
			*value = m_dsrrom[(m_rom_page<<13) | (m_address & 0x1fff)];
			if (TRACE_RW) logerror("bwg: read dsr: %04x (page %d)-> %02x\n", m_address & 0xffff, m_rom_page, *value);
		}
	}
}

/*
    Resets the drive geometry. This is required because the heuristic of
    the default implementation sets the drive geometry to the geometry
    of the medium.
*/
void snug_bwg_device::set_geometry(device_t *drive, floppy_type_t type)
{
	// This assertion may fail when the names of the floppy devices change.
	// Unfortunately, the wd17xx device assumes the floppy drives at root
	// level, so we use an explicitly qualified tag. See peribox.h.
	assert(drive != NULL);
	floppy_drive_set_geometry(drive, type);
}

void snug_bwg_device::set_all_geometries(floppy_type_t type)
{
	set_geometry(machine().device(PFLOPPY_0), type);
	set_geometry(machine().device(PFLOPPY_1), type);
	set_geometry(machine().device(PFLOPPY_2), type);
}

/*
    Write a byte
    4000 - 5bff: ROM, ignore write (4 banks)

    rtc disabled:
    5c00 - 5fef: RAM
    5ff0 - 5fff: Controller (f8 = command, fa = track, fc = sector, fe = data)

    rtc enabled:
    5c00 - 5fdf: RAM
    5fe0 - 5fff: Clock (even addr)
*/
WRITE8_MEMBER(snug_bwg_device::write)
{
	if (m_inDsrArea && m_selected)
	{
		if (m_lastK)
		{
			if (m_rtc_enabled)
			{
				if (m_RTCsel)
				{
					// .... ..11 111x xxx0
					if (TRACE_RW) logerror("bwg: write RTC: %04x <- %02x\n", m_address & 0xffff, data);
					if (!space.debugger_access()) m_clock->write(space, (m_address & 0x001e) >> 1, data);
				}
				else
				{
					if (TRACE_RW) logerror("bwg: write ram: %04x (page %d) <- %02x\n", m_address & 0xffff, m_ram_page, data);
					m_buffer_ram[(m_ram_page<<10) | (m_address & 0x03ff)] = data;
				}
			}
			else
			{
				if (m_WDsel)
				{
					// .... ..11 1111 1xx0
					// Note that the value is inverted again on the board,
					// so we can drop the inversion
					if (TRACE_RW) logerror("bwg: write FDC: %04x <- %02x\n", m_address & 0xffff, data);
					if (!space.debugger_access()) wd17xx_w(m_wd1773, space, (m_address >> 1)&0x03, data);
				}
				else
				{
					if (TRACE_RW) logerror("bwg: write ram: %04x (page %d) <- %02x\n", m_address & 0xffff, m_ram_page, data);
					m_buffer_ram[(m_ram_page<<10) | (m_address & 0x03ff)] = data;
				}
			}
		}
	}
}

/*
    CRU read handler. *=inverted.
    bit 0: DSK4 connected*
    bit 1: DSK1 connected*
    bit 2: DSK2 connected*
    bit 3: DSK3 connected*
    bit 4: Dip 1
    bit 5: Dip 2
    bit 6: Dip 3
    bit 7: Dip 4
*/
READ8Z_MEMBER(snug_bwg_device::crureadz)
{
	UINT8 reply = 0;

	if ((offset & 0xff00)==m_cru_base)
	{
		if ((offset & 0x00ff)==0)
		{
			// Assume that we have 4 drives connected
			// If we want to do that properly, we need to check the actually
			// available drives (not the images!). But why should we connect less?
			reply = 0x00;

			// DIP switches. Note that a closed switch means 0
			// xx01 1111   11 = only dsk1; 10 = 1+2, 01=1/2/3, 00=1-4

			if (m_dip1 != 0) reply |= 0x10;
			if (m_dip2 != 0) reply |= 0x20;
			reply |= (m_dip34 << 6);
			*value = ~reply;
		}
		else
			*value = 0;
		if (TRACE_CRU) logerror("bwg: Read CRU = %02x\n", *value);
	}
}

WRITE8_MEMBER(snug_bwg_device::cruwrite)
{
	int drive, drivebit;

	if ((offset & 0xff00)==m_cru_base)
	{
		int bit = (offset >> 1) & 0x0f;
		switch (bit)
		{
		case 0:
			/* (De)select the card. Indicated by a LED on the board. */
			m_selected = (data != 0);
			if (TRACE_CRU) logerror("bwg: Map DSR (bit 0) = %d\n", m_selected);
			break;

		case 1:
			/* Activate motor */
			if (data && !m_strobe_motor)
			{   /* on rising edge, set motor_running for 4.23s */
				if (TRACE_CRU) logerror("bwg: trigger motor (bit 1)\n");
				m_DVENA = ASSERT_LINE;
				m_motor_on_timer->adjust(attotime::from_msec(4230));
			}
			m_strobe_motor = (data != 0);
			break;

		case 2:
			/* Set disk ready/hold (bit 2) */
			// 0: ignore IRQ and DRQ
			// 1: TMS9900 is stopped until IRQ or DRQ are set
			// OR the motor stops rotating - rotates for 4.23s after write
			// to CRU bit 1
			if (TRACE_CRU) logerror("bwg: arm wait state logic (bit 2) = %d\n", data);
			m_WAITena = (data != 0);
			break;

		case 4:
		case 5:
		case 6:
		case 8:
			/* Select drive 0-2 (DSK1-DSK3) (bits 4-6) */
			/* Select drive 3 (DSK4) (bit 8) */
			drive = (bit == 8) ? 3 : (bit - 4);     /* drive # (0-3) */
			if (TRACE_CRU) logerror("bwg: set drive (bit %d) = %d\n", bit, data);
			drivebit = 1<<drive;

			if (data != 0)
			{
				if ((m_DSEL & drivebit) == 0)           /* select drive */
				{
					if (m_DSEL != 0)
						logerror("bwg: Multiple drives selected, %02x\n", m_DSEL);
					m_DSEL |= drivebit;
					wd17xx_set_drive(m_wd1773, drive);
				}
			}
			else
				m_DSEL &= ~drivebit;
			break;

		case 7:
			/* Select side of disk (bit 7) */
			m_SIDE = data;
			if (TRACE_CRU) logerror("bwg: set side (bit 7) = %d\n", data);
			wd17xx_set_side(m_wd1773, m_SIDE);
			break;

		case 10:
			/* double density enable (active low) */
			if (TRACE_CRU) logerror("bwg: set double density (bit 10) = %d\n", data);
			wd17xx_dden_w(m_wd1773, (data != 0) ? ASSERT_LINE : CLEAR_LINE);
			break;

		case 11:
			/* EPROM A13 */
			if (data != 0)
				m_rom_page |= 1;
			else
				m_rom_page &= 0xfe;  // 11111110
			if (TRACE_CRU) logerror("bwg: set ROM page (bit 11) = %d, page = %d\n", bit, m_rom_page);
			break;

		case 13:
			/* RAM A10 */
			m_ram_page = data;
			if (TRACE_CRU) logerror("bwg: set RAM page (bit 13) = %d, page = %d\n", bit, m_ram_page);
			break;

		case 14:
			/* Override FDC with RTC (active high) */
			if (TRACE_CRU) logerror("bwg: turn on RTC (bit 14) = %d\n", data);
			m_rtc_enabled = (data != 0);
			break;

		case 15:
			/* EPROM A14 */
			if (data != 0)
				m_rom_page |= 2;
			else
				m_rom_page &= 0xfd; // 11111101
			if (TRACE_CRU) logerror("bwg: set ROM page (bit 15) = %d, page = %d\n", bit, m_rom_page);
			break;

		case 3:
			if (TRACE_CRU) logerror("bwg: set head load (bit 3) = %d\n", data);
			break;
		case 9:
		case 12:
			/* Unused (bit 3, 9 & 12) */
			if (TRACE_CRU) logerror("bwg: set unknown bit %d = %d\n", bit, data);
			break;
		}
	}
}

void snug_bwg_device::device_start(void)
{
	logerror("bwg: BWG start\n");
	m_dsrrom = memregion(DSRROM)->base();
	m_buffer_ram = memregion(BUFFER)->base();
	m_motor_on_timer = timer_alloc(MOTOR_TIMER);
	m_cru_base = 0x1100;
}

void snug_bwg_device::device_reset()
{
	logerror("bwg: BWG reset\n");

	if (m_genmod)
	{
		m_select_mask = 0x1fe000;
		m_select_value = 0x174000;
	}
	else
	{
		m_select_mask = 0x7e000;
		m_select_value = 0x74000;
	}

	m_strobe_motor = false;
	m_DVENA = CLEAR_LINE;
	m_DSEL = 0;
	m_SIDE = 0;
	ti99_set_80_track_drives(FALSE);
	floppy_type_t type = FLOPPY_STANDARD_5_25_DSDD_40;
	set_all_geometries(type);
	m_DRQ = CLEAR_LINE;
	m_IRQ = CLEAR_LINE;
	m_WAITena = false;
	m_rtc_enabled = false;
	m_selected = false;

	m_dip1 = ioport("BWGDIP1")->read();
	m_dip2 = ioport("BWGDIP2")->read();
	m_dip34 = ioport("BWGDIP34")->read();

	m_rom_page = 0;
	m_ram_page = 0;
}

const wd17xx_interface bwg_wd17xx_interface =
{
	DEVCB_NULL,
	DEVCB_DEVICE_LINE_MEMBER(DEVICE_SELF_OWNER, snug_bwg_device, intrq_w),
	DEVCB_DEVICE_LINE_MEMBER(DEVICE_SELF_OWNER, snug_bwg_device, drq_w),
	{ PFLOPPY_0, PFLOPPY_1, PFLOPPY_2, PFLOPPY_3 }
};

static const mm58274c_interface floppy_mm58274c_interface =
{
	1,  /*  mode 24*/
	0   /*  first day of week */
};

INPUT_PORTS_START( bwg_fdc )
	PORT_START( "BWGDIP1" )
	PORT_DIPNAME( 0x01, 0x00, "BwG step rate" )
		PORT_DIPSETTING( 0x00, "6 ms")
		PORT_DIPSETTING( 0x01, "20 ms")

	PORT_START( "BWGDIP2" )
	PORT_DIPNAME( 0x01, 0x00, "BwG date/time display" )
		PORT_DIPSETTING( 0x00, "Hide")
		PORT_DIPSETTING( 0x01, "Show")

	PORT_START( "BWGDIP34" )
	PORT_DIPNAME( 0x03, 0x03, "BwG drives" )
		PORT_DIPSETTING( 0x00, "DSK1 only")
		PORT_DIPSETTING( 0x01, "DSK1-DSK2")
		PORT_DIPSETTING( 0x02, "DSK1-DSK3")
		PORT_DIPSETTING( 0x03, "DSK1-DSK4")
INPUT_PORTS_END

MACHINE_CONFIG_FRAGMENT( bwg_fdc )
	MCFG_WD1773_ADD(FDC_TAG, bwg_wd17xx_interface )
	MCFG_MM58274C_ADD(CLOCK_TAG, floppy_mm58274c_interface)
MACHINE_CONFIG_END

ROM_START( bwg_fdc )
	ROM_REGION(0x8000, DSRROM, 0)
	ROM_LOAD("bwg.bin", 0x0000, 0x8000, CRC(06f1ec89) SHA1(6ad77033ed268f986d9a5439e65f7d391c4b7651)) /* BwG disk DSR ROM */
	ROM_REGION(0x0800, BUFFER, 0)  /* BwG RAM buffer */
	ROM_FILL(0x0000, 0x0400, 0x00)
ROM_END

machine_config_constructor snug_bwg_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( bwg_fdc );
}

const rom_entry *snug_bwg_device::device_rom_region() const
{
	return ROM_NAME( bwg_fdc );
}

ioport_constructor snug_bwg_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(bwg_fdc);
}

const device_type TI99_BWG = &device_creator<snug_bwg_device>;
