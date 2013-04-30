/***************************************************************************

    savquest.c

    "Savage Quest" (c) 1999 Interactive Light, developed by Angel Studios.
    Skeleton by R. Belmont

    H/W is a white-box PC consisting of:
    Pentium II 450 CPU
    DFI P2XBL motherboard (i440BX chipset)
    128 MB RAM
    Guillemot Maxi Gamer 3D2 Voodoo II
    Sound Blaster AWE64

    Protected by a HASP brand parallel port dongle.
    I/O board has a PIC17C43 which is not readable.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

- update by Peter Ferrie:
- split BIOS region into 16kb blocks and implement missing PAM registers


***************************************************************************/


#include "emu.h"
#include "cpu/i386/i386.h"
#include "machine/8237dma.h"
#include "machine/pic8259.h"
#include "machine/pit8253.h"
#include "machine/mc146818.h"
#include "machine/pci.h"
#include "machine/8042kbdc.h"
#include "machine/pckeybrd.h"
#include "machine/idectrl.h"
#include "video/pc_vga.h"


class savquest_state : public driver_device
{
public:
	savquest_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, "maincpu"),
			m_pit8254(*this, "pit8254"),
			m_dma8237_1(*this, "dma8237_1"),
			m_dma8237_2(*this, "dma8237_2"),
			m_pic8259_1(*this, "pic8259_1"),
			m_pic8259_2(*this, "pic8259_2")
	{ }

	UINT32 *m_bios_f0000_ram;
	UINT32 *m_bios_e0000_ram;
	UINT32 *m_bios_e4000_ram;
	UINT32 *m_bios_e8000_ram;
	UINT32 *m_bios_ec000_ram;
	int m_dma_channel;
	UINT8 m_dma_offset[2][4];
	UINT8 m_at_pages[0x10];
	UINT8 m_mxtc_config_reg[256];
	UINT8 m_piix4_config_reg[8][256];

	// devices
	required_device<cpu_device> m_maincpu;
	required_device<pit8254_device> m_pit8254;
	required_device<i8237_device> m_dma8237_1;
	required_device<i8237_device> m_dma8237_2;
	required_device<pic8259_device> m_pic8259_1;
	required_device<pic8259_device> m_pic8259_2;

	DECLARE_READ8_MEMBER( get_slave_ack );
	DECLARE_WRITE32_MEMBER( bios_f0000_ram_w );
	DECLARE_WRITE32_MEMBER( bios_e0000_ram_w );
	DECLARE_WRITE32_MEMBER( bios_e4000_ram_w );
	DECLARE_WRITE32_MEMBER( bios_e8000_ram_w );
	DECLARE_WRITE32_MEMBER( bios_ec000_ram_w );

protected:


	// driver_device overrides
//  virtual void video_start();
public:
	DECLARE_READ8_MEMBER(at_page8_r);
	DECLARE_WRITE8_MEMBER(at_page8_w);
	DECLARE_READ8_MEMBER(pc_dma_read_byte);
	DECLARE_WRITE8_MEMBER(pc_dma_write_byte);
	DECLARE_READ32_MEMBER(ide_r);
	DECLARE_WRITE32_MEMBER(ide_w);
	DECLARE_READ32_MEMBER(fdc_r);
	DECLARE_WRITE32_MEMBER(fdc_w);
	DECLARE_READ8_MEMBER(at_dma8237_2_r);
	DECLARE_WRITE8_MEMBER(at_dma8237_2_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dma_hrq_changed);
	DECLARE_WRITE_LINE_MEMBER(pc_dack0_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dack1_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dack2_w);
	DECLARE_WRITE_LINE_MEMBER(pc_dack3_w);
	DECLARE_WRITE_LINE_MEMBER(savquest_pic8259_1_set_int_line);
	DECLARE_READ8_MEMBER(get_out2);
	virtual void machine_start();
	virtual void machine_reset();
	IRQ_CALLBACK_MEMBER(irq_callback);
	void intel82439tx_init();
};

// Intel 82439TX System Controller (MXTC)

static UINT8 mxtc_config_r(device_t *busdevice, device_t *device, int function, int reg)
{
	savquest_state *state = busdevice->machine().driver_data<savquest_state>();
//  mame_printf_debug("MXTC: read %d, %02X\n", function, reg);

	return state->m_mxtc_config_reg[reg];
}

static void mxtc_config_w(device_t *busdevice, device_t *device, int function, int reg, UINT8 data)
{
	savquest_state *state = busdevice->machine().driver_data<savquest_state>();
//  mame_printf_debug("%s:MXTC: write %d, %02X, %02X\n", machine.describe_context(), function, reg, data);

	#if 1
	switch(reg)
	{
		case 0x59:      // PAM0
		{
			if (data & 0x10)        // enable RAM access to region 0xf0000 - 0xfffff
			{
				state->membank("bios_f0000")->set_base(state->m_bios_f0000_ram);
			}
			else                    // disable RAM access (reads go to BIOS ROM)
			{
				state->membank("bios_f0000")->set_base(state->memregion("bios")->base() + 0x30000);
			}
			break;
		}

		case 0x5e:      // PAM5
		{
			if (data & 0x10)        // enable RAM access to region 0xe4000 - 0xe7fff
			{
				state->membank("bios_e4000")->set_base(state->m_bios_e4000_ram);
			}
			else                    // disable RAM access (reads go to BIOS ROM)
			{
				state->membank("bios_e4000")->set_base(state->memregion("bios")->base() + 0x24000);
			}

			if (data & 1)       // enable RAM access to region 0xe0000 - 0xe3fff
			{
				state->membank("bios_e0000")->set_base(state->m_bios_e0000_ram);
			}
			else                    // disable RAM access (reads go to BIOS ROM)
			{
				state->membank("bios_e0000")->set_base(state->memregion("bios")->base() + 0x20000);
			}
			break;
		}

		case 0x5f:      // PAM6
		{
			if (data & 0x10)        // enable RAM access to region 0xec000 - 0xeffff
			{
				state->membank("bios_ec000")->set_base(state->m_bios_ec000_ram);
			}
			else                    // disable RAM access (reads go to BIOS ROM)
			{
				state->membank("bios_ec000")->set_base(state->memregion("bios")->base() + 0x2c000);
			}

			if (data & 1)       // enable RAM access to region 0xe8000 - 0xebfff
			{
				state->membank("bios_e8000")->set_base(state->m_bios_e8000_ram);
			}
			else                    // disable RAM access (reads go to BIOS ROM)
			{
				state->membank("bios_e8000")->set_base(state->memregion("bios")->base() + 0x28000);
			}
			break;
		}
	}
	#endif

	state->m_mxtc_config_reg[reg] = data;
}

void savquest_state::intel82439tx_init()
{
	m_mxtc_config_reg[0x60] = 0x02;
	m_mxtc_config_reg[0x61] = 0x02;
	m_mxtc_config_reg[0x62] = 0x02;
	m_mxtc_config_reg[0x63] = 0x02;
	m_mxtc_config_reg[0x64] = 0x02;
	m_mxtc_config_reg[0x65] = 0x02;
}

static UINT32 intel82439tx_pci_r(device_t *busdevice, device_t *device, int function, int reg, UINT32 mem_mask)
{
	UINT32 r = 0;
	if (ACCESSING_BITS_24_31)
	{
		r |= mxtc_config_r(busdevice, device, function, reg + 3) << 24;
	}
	if (ACCESSING_BITS_16_23)
	{
		r |= mxtc_config_r(busdevice, device, function, reg + 2) << 16;
	}
	if (ACCESSING_BITS_8_15)
	{
		r |= mxtc_config_r(busdevice, device, function, reg + 1) << 8;
	}
	if (ACCESSING_BITS_0_7)
	{
		r |= mxtc_config_r(busdevice, device, function, reg + 0) << 0;
	}
	return r;
}

static void intel82439tx_pci_w(device_t *busdevice, device_t *device, int function, int reg, UINT32 data, UINT32 mem_mask)
{
	if (ACCESSING_BITS_24_31)
	{
		mxtc_config_w(busdevice, device, function, reg + 3, (data >> 24) & 0xff);
	}
	if (ACCESSING_BITS_16_23)
	{
		mxtc_config_w(busdevice, device, function, reg + 2, (data >> 16) & 0xff);
	}
	if (ACCESSING_BITS_8_15)
	{
		mxtc_config_w(busdevice, device, function, reg + 1, (data >> 8) & 0xff);
	}
	if (ACCESSING_BITS_0_7)
	{
		mxtc_config_w(busdevice, device, function, reg + 0, (data >> 0) & 0xff);
	}
}

// Intel 82371AB PCI-to-ISA / IDE bridge (PIIX4)

static UINT8 piix4_config_r(device_t *busdevice, device_t *device, int function, int reg)
{
	savquest_state *state = busdevice->machine().driver_data<savquest_state>();
//  mame_printf_debug("PIIX4: read %d, %02X\n", function, reg);
	return state->m_piix4_config_reg[function][reg];
}

static void piix4_config_w(device_t *busdevice, device_t *device, int function, int reg, UINT8 data)
{
	savquest_state *state = busdevice->machine().driver_data<savquest_state>();
//  mame_printf_debug("%s:PIIX4: write %d, %02X, %02X\n", machine.describe_context(), function, reg, data);
	state->m_piix4_config_reg[function][reg] = data;
}

static UINT32 intel82371ab_pci_r(device_t *busdevice, device_t *device, int function, int reg, UINT32 mem_mask)
{
	UINT32 r = 0;
	if (ACCESSING_BITS_24_31)
	{
		r |= piix4_config_r(busdevice, device, function, reg + 3) << 24;
	}
	if (ACCESSING_BITS_16_23)
	{
		r |= piix4_config_r(busdevice, device, function, reg + 2) << 16;
	}
	if (ACCESSING_BITS_8_15)
	{
		r |= piix4_config_r(busdevice, device, function, reg + 1) << 8;
	}
	if (ACCESSING_BITS_0_7)
	{
		r |= piix4_config_r(busdevice, device, function, reg + 0) << 0;
	}
	return r;
}

static void intel82371ab_pci_w(device_t *busdevice, device_t *device, int function, int reg, UINT32 data, UINT32 mem_mask)
{
	if (ACCESSING_BITS_24_31)
	{
		piix4_config_w(busdevice, device, function, reg + 3, (data >> 24) & 0xff);
	}
	if (ACCESSING_BITS_16_23)
	{
		piix4_config_w(busdevice, device, function, reg + 2, (data >> 16) & 0xff);
	}
	if (ACCESSING_BITS_8_15)
	{
		piix4_config_w(busdevice, device, function, reg + 1, (data >> 8) & 0xff);
	}
	if (ACCESSING_BITS_0_7)
	{
		piix4_config_w(busdevice, device, function, reg + 0, (data >> 0) & 0xff);
	}
}

WRITE32_MEMBER(savquest_state::bios_f0000_ram_w)
{
	//if (m_mxtc_config_reg[0x59] & 0x20)       // write to RAM if this region is write-enabled
	#if 1
	if (m_mxtc_config_reg[0x59] & 0x20)     // write to RAM if this region is write-enabled
	{
		COMBINE_DATA(m_bios_f0000_ram + offset);
	}
	#endif
}

WRITE32_MEMBER(savquest_state::bios_e0000_ram_w)
{
	//if (m_mxtc_config_reg[0x5e] & 2)       // write to RAM if this region is write-enabled
	#if 1
	if (m_mxtc_config_reg[0x5e] & 2)        // write to RAM if this region is write-enabled
	{
		COMBINE_DATA(m_bios_e0000_ram + offset);
	}
	#endif
}

WRITE32_MEMBER(savquest_state::bios_e4000_ram_w)
{
	//if (m_mxtc_config_reg[0x5e] & 0x20)       // write to RAM if this region is write-enabled
	#if 1
	if (m_mxtc_config_reg[0x5e] & 0x20)     // write to RAM if this region is write-enabled
	{
		COMBINE_DATA(m_bios_e4000_ram + offset);
	}
	#endif
}

WRITE32_MEMBER(savquest_state::bios_e8000_ram_w)
{
	//if (m_mxtc_config_reg[0x5f] & 2)       // write to RAM if this region is write-enabled
	#if 1
	if (m_mxtc_config_reg[0x5f] & 2)        // write to RAM if this region is write-enabled
	{
		COMBINE_DATA(m_bios_e8000_ram + offset);
	}
	#endif
}

WRITE32_MEMBER(savquest_state::bios_ec000_ram_w)
{
	//if (m_mxtc_config_reg[0x5f] & 0x20)       // write to RAM if this region is write-enabled
	#if 1
	if (m_mxtc_config_reg[0x5f] & 0x20)     // write to RAM if this region is write-enabled
	{
		COMBINE_DATA(m_bios_ec000_ram + offset);
	}
	#endif
}

READ32_MEMBER(savquest_state::ide_r)
{
	device_t *device = machine().device("ide");
	return ide_controller32_r(device, space, 0x1f0/4 + offset, mem_mask);
}

WRITE32_MEMBER(savquest_state::ide_w)
{
	device_t *device = machine().device("ide");
	ide_controller32_w(device, space, 0x1f0/4 + offset, data, mem_mask);
}

READ32_MEMBER(savquest_state::fdc_r)
{
	device_t *device = machine().device("ide");
	return ide_controller32_r(device, space, 0x3f0/4 + offset, mem_mask);
}

WRITE32_MEMBER(savquest_state::fdc_w)
{
	device_t *device = machine().device("ide");
	//mame_printf_debug("FDC: write %08X, %08X, %08X\n", data, offset, mem_mask);
	ide_controller32_w(device, space, 0x3f0/4 + offset, data, mem_mask);
}

READ8_MEMBER(savquest_state::at_page8_r)
{
	UINT8 data = m_at_pages[offset % 0x10];

	switch(offset % 8) {
	case 1:
		data = m_dma_offset[(offset / 8) & 1][2];
		break;
	case 2:
		data = m_dma_offset[(offset / 8) & 1][3];
		break;
	case 3:
		data = m_dma_offset[(offset / 8) & 1][1];
		break;
	case 7:
		data = m_dma_offset[(offset / 8) & 1][0];
		break;
	}
	return data;
}


WRITE8_MEMBER(savquest_state::at_page8_w)
{
	m_at_pages[offset % 0x10] = data;

	switch(offset % 8) {
	case 1:
		m_dma_offset[(offset / 8) & 1][2] = data;
		break;
	case 2:
		m_dma_offset[(offset / 8) & 1][3] = data;
		break;
	case 3:
		m_dma_offset[(offset / 8) & 1][1] = data;
		break;
	case 7:
		m_dma_offset[(offset / 8) & 1][0] = data;
		break;
	}
}


READ8_MEMBER(savquest_state::at_dma8237_2_r)
{
	return m_dma8237_2->i8237_r(space, offset / 2);
}

WRITE8_MEMBER(savquest_state::at_dma8237_2_w)
{
	m_dma8237_2->i8237_w(space, offset / 2, data);
}

WRITE_LINE_MEMBER(savquest_state::pc_dma_hrq_changed)
{
	m_maincpu->set_input_line(INPUT_LINE_HALT, state ? ASSERT_LINE : CLEAR_LINE);

	/* Assert HLDA */
	m_dma8237_1->i8237_hlda_w( state );
}


READ8_MEMBER(savquest_state::pc_dma_read_byte)
{
	offs_t page_offset = (((offs_t) m_dma_offset[0][m_dma_channel]) << 16)
		& 0xFF0000;

	return space.read_byte(page_offset + offset);
}


WRITE8_MEMBER(savquest_state::pc_dma_write_byte)
{
	offs_t page_offset = (((offs_t) m_dma_offset[0][m_dma_channel]) << 16)
		& 0xFF0000;

	space.write_byte(page_offset + offset, data);
}

static void set_dma_channel(device_t *device, int channel, int state)
{
	savquest_state *drvstate = device->machine().driver_data<savquest_state>();
	if (!state) drvstate->m_dma_channel = channel;
}

WRITE_LINE_MEMBER(savquest_state::pc_dack0_w){ set_dma_channel(m_dma8237_1, 0, state); }
WRITE_LINE_MEMBER(savquest_state::pc_dack1_w){ set_dma_channel(m_dma8237_1, 1, state); }
WRITE_LINE_MEMBER(savquest_state::pc_dack2_w){ set_dma_channel(m_dma8237_1, 2, state); }
WRITE_LINE_MEMBER(savquest_state::pc_dack3_w){ set_dma_channel(m_dma8237_1, 3, state); }

static I8237_INTERFACE( dma8237_1_config )
{
	DEVCB_DRIVER_LINE_MEMBER(savquest_state,pc_dma_hrq_changed),
	DEVCB_NULL,
	DEVCB_DRIVER_MEMBER(savquest_state, pc_dma_read_byte),
	DEVCB_DRIVER_MEMBER(savquest_state, pc_dma_write_byte),
	{ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL },
	{ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL },
	{ DEVCB_DRIVER_LINE_MEMBER(savquest_state,pc_dack0_w), DEVCB_DRIVER_LINE_MEMBER(savquest_state,pc_dack1_w), DEVCB_DRIVER_LINE_MEMBER(savquest_state,pc_dack2_w), DEVCB_DRIVER_LINE_MEMBER(savquest_state,pc_dack3_w) }
};

static I8237_INTERFACE( dma8237_2_config )
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	{ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL },
	{ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL },
	{ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL }
};


static ADDRESS_MAP_START(savquest_map, AS_PROGRAM, 32, savquest_state)
	AM_RANGE(0x00000000, 0x0009ffff) AM_RAM
	AM_RANGE(0x000a0000, 0x000bffff) AM_DEVREADWRITE8("vga", vga_device, mem_r, mem_w, 0xffffffff)
	AM_RANGE(0x000c0000, 0x000c7fff) AM_ROM AM_REGION("video_bios", 0)
	AM_RANGE(0x000f0000, 0x000fffff) AM_ROMBANK("bios_f0000") AM_WRITE(bios_f0000_ram_w)
	AM_RANGE(0x000e0000, 0x000e3fff) AM_ROMBANK("bios_e0000") AM_WRITE(bios_e0000_ram_w)
	AM_RANGE(0x000e4000, 0x000e7fff) AM_ROMBANK("bios_e4000") AM_WRITE(bios_e4000_ram_w)
	AM_RANGE(0x000e8000, 0x000ebfff) AM_ROMBANK("bios_e8000") AM_WRITE(bios_e8000_ram_w)
	AM_RANGE(0x000ec000, 0x000effff) AM_ROMBANK("bios_ec000") AM_WRITE(bios_ec000_ram_w)
	AM_RANGE(0x00100000, 0x01ffffff) AM_RAM
//  AM_RANGE(0x02000000, 0x02000003) // protection dongle lies there?
	AM_RANGE(0xfffc0000, 0xffffffff) AM_ROM AM_REGION("bios", 0)    /* System BIOS */
ADDRESS_MAP_END

static ADDRESS_MAP_START(savquest_io, AS_IO, 32, savquest_state)
	AM_RANGE(0x0000, 0x001f) AM_DEVREADWRITE8("dma8237_1", i8237_device, i8237_r, i8237_w, 0xffffffff)
	AM_RANGE(0x0020, 0x003f) AM_DEVREADWRITE8_LEGACY("pic8259_1", pic8259_r, pic8259_w, 0xffffffff)
	AM_RANGE(0x0040, 0x005f) AM_DEVREADWRITE8_LEGACY("pit8254", pit8253_r, pit8253_w, 0xffffffff)
	AM_RANGE(0x0060, 0x006f) AM_DEVREADWRITE8("kbdc", kbdc8042_device, data_r, data_w, 0xffffffff)
	AM_RANGE(0x0070, 0x007f) AM_DEVREADWRITE8("rtc", mc146818_device, read, write, 0xffffffff) /* todo: nvram (CMOS Setup Save)*/
	AM_RANGE(0x0080, 0x009f) AM_READWRITE8(at_page8_r, at_page8_w, 0xffffffff)
	AM_RANGE(0x00a0, 0x00bf) AM_DEVREADWRITE8_LEGACY("pic8259_2", pic8259_r, pic8259_w, 0xffffffff)
	AM_RANGE(0x00c0, 0x00df) AM_READWRITE8(at_dma8237_2_r, at_dma8237_2_w, 0xffffffff)
	AM_RANGE(0x00e8, 0x00ef) AM_NOP

	AM_RANGE(0x01f0, 0x01f7) AM_READWRITE(ide_r, ide_w)
	AM_RANGE(0x03b0, 0x03bf) AM_DEVREADWRITE8("vga", vga_device, port_03b0_r, port_03b0_w, 0xffffffff)
	AM_RANGE(0x03c0, 0x03cf) AM_DEVREADWRITE8("vga", vga_device, port_03c0_r, port_03c0_w, 0xffffffff)
	AM_RANGE(0x03d0, 0x03df) AM_DEVREADWRITE8("vga", vga_device, port_03d0_r, port_03d0_w, 0xffffffff)
	AM_RANGE(0x03f0, 0x03f7) AM_READWRITE(fdc_r, fdc_w)

	AM_RANGE(0x0cf8, 0x0cff) AM_DEVREADWRITE("pcibus", pci_bus_legacy_device, read, write)

//  AM_RANGE(0x5000, 0x5007) // routes to port $eb
ADDRESS_MAP_END

static INPUT_PORTS_START( savquest )
INPUT_PORTS_END

static const struct pit8253_config savquest_pit8254_config =
{
	{
		{
			4772720/4,              /* heartbeat IRQ */
			DEVCB_NULL,
			DEVCB_DEVICE_LINE("pic8259_1", pic8259_ir0_w)
		}, {
			4772720/4,              /* dram refresh */
			DEVCB_NULL,
			DEVCB_NULL
		}, {
			4772720/4,              /* pio port c pin 4, and speaker polling enough */
			DEVCB_NULL,
			DEVCB_NULL
		}
	}
};

WRITE_LINE_MEMBER(savquest_state::savquest_pic8259_1_set_int_line)
{
	m_maincpu->set_input_line(0, state ? HOLD_LINE : CLEAR_LINE);
}

READ8_MEMBER( savquest_state::get_slave_ack )
{
	if (offset==2) { // IRQ = 2
		logerror("pic8259_slave_ACK!\n");
		return pic8259_acknowledge(m_pic8259_2);
	}
	return 0x00;
}

static const struct pic8259_interface savquest_pic8259_1_config =
{
	DEVCB_DRIVER_LINE_MEMBER(savquest_state,savquest_pic8259_1_set_int_line),
	DEVCB_LINE_VCC,
	DEVCB_DRIVER_MEMBER(savquest_state,get_slave_ack)
};

static const struct pic8259_interface savquest_pic8259_2_config =
{
	DEVCB_DEVICE_LINE("pic8259_1", pic8259_ir2_w),
	DEVCB_LINE_GND,
	DEVCB_NULL
};

READ8_MEMBER(savquest_state::get_out2)
{
	return pit8253_get_output( m_pit8254, 2 );
}

static const struct kbdc8042_interface at8042 =
{
	KBDC8042_AT386,
	DEVCB_CPU_INPUT_LINE("maincpu", INPUT_LINE_RESET),
	DEVCB_CPU_INPUT_LINE("maincpu", INPUT_LINE_A20),
	DEVCB_DEVICE_LINE_MEMBER("pic8259_1", pic8259_device, ir1_w),
	DEVCB_NULL,

	DEVCB_NULL,
	DEVCB_DRIVER_MEMBER(savquest_state,get_out2)
};

IRQ_CALLBACK_MEMBER(savquest_state::irq_callback)
{
	return pic8259_acknowledge(m_pic8259_1);
}

void savquest_state::machine_start()
{
	m_bios_f0000_ram = auto_alloc_array(machine(), UINT32, 0x10000/4);
	m_bios_e0000_ram = auto_alloc_array(machine(), UINT32, 0x4000/4);
	m_bios_e4000_ram = auto_alloc_array(machine(), UINT32, 0x4000/4);
	m_bios_e8000_ram = auto_alloc_array(machine(), UINT32, 0x4000/4);
	m_bios_ec000_ram = auto_alloc_array(machine(), UINT32, 0x4000/4);

	m_maincpu->set_irq_acknowledge_callback(device_irq_acknowledge_delegate(FUNC(savquest_state::irq_callback),this));
	intel82439tx_init();
}

void savquest_state::machine_reset()
{
	membank("bios_f0000")->set_base(memregion("bios")->base() + 0x30000);
	membank("bios_e0000")->set_base(memregion("bios")->base() + 0x20000);
	membank("bios_e4000")->set_base(memregion("bios")->base() + 0x24000);
	membank("bios_e8000")->set_base(memregion("bios")->base() + 0x28000);
	membank("bios_ec000")->set_base(memregion("bios")->base() + 0x2c000);
}

static MACHINE_CONFIG_START( savquest, savquest_state )
	MCFG_CPU_ADD("maincpu", PENTIUM, 450000000) // actually Pentium II 450
	MCFG_CPU_PROGRAM_MAP(savquest_map)
	MCFG_CPU_IO_MAP(savquest_io)


	MCFG_PIT8254_ADD( "pit8254", savquest_pit8254_config )
	MCFG_I8237_ADD( "dma8237_1", XTAL_14_31818MHz/3, dma8237_1_config )
	MCFG_I8237_ADD( "dma8237_2", XTAL_14_31818MHz/3, dma8237_2_config )
	MCFG_PIC8259_ADD( "pic8259_1", savquest_pic8259_1_config )
	MCFG_PIC8259_ADD( "pic8259_2", savquest_pic8259_2_config )

	MCFG_MC146818_ADD( "rtc", MC146818_STANDARD )

	MCFG_PCI_BUS_LEGACY_ADD("pcibus", 0)
	MCFG_PCI_BUS_LEGACY_DEVICE(0, NULL, intel82439tx_pci_r, intel82439tx_pci_w)
	MCFG_PCI_BUS_LEGACY_DEVICE(7, NULL, intel82371ab_pci_r, intel82371ab_pci_w)

	MCFG_IDE_CONTROLLER_ADD("ide", ide_devices, "hdd", NULL, true)
	MCFG_IDE_CONTROLLER_IRQ_HANDLER(DEVWRITELINE("pic8259_2", pic8259_device, ir6_w))

	/* video hardware */
	MCFG_FRAGMENT_ADD( pcvideo_vga )

	MCFG_KBDC8042_ADD("kbdc", at8042)
MACHINE_CONFIG_END

ROM_START( savquest )
	ROM_REGION32_LE(0x40000, "bios", 0)
	ROM_LOAD( "sq-aflash.bin", 0x00000, 0x040000, CRC(0b4f406f) SHA1(4003b0e6d46dcb47012acc118837f0f7cf529faf) ) // first half is 1-filled

	ROM_REGION( 0x8000, "video_bios", 0 ) // TODO: needs proper video BIOS dumped
	ROM_LOAD16_BYTE( "trident_tgui9680_bios.bin", 0x0000, 0x4000, BAD_DUMP CRC(1eebde64) SHA1(67896a854d43a575037613b3506aea6dae5d6a19) )
	ROM_CONTINUE(                                 0x0001, 0x4000 )

	DISK_REGION( "drive_0" )
	DISK_IMAGE( "savquest", 0, SHA1(b20cacf45e093b533c538bf4fc08f05f9475d640) )
ROM_END


GAME(1999, savquest, 0, savquest, savquest, driver_device, 0, ROT0, "Interactive Light", "Savage Quest", GAME_IS_SKELETON)
