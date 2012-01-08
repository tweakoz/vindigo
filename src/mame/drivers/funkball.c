// dgPIX 'VRender 2 Beta Rev4' hardware
// MEDIAGX CPU + 3dFX VooDoo chipset

/***************************************************************************

Notes:
- currently fails DAC r/w in Voodoo registers (FPU fault?);
(note: some of those are redundant)
bp 0x18390,ax = 1;bp 0x1b5cb,ax = 1;

Funky Ball
dgPIX, 1998

PCB Layout
----------

VRender 2Beta Rev4
  |--------------------------------------------------------------------|
  |TDA2005  14.31818MHz       |---------|   KM416C254 KM416C254        |
  |VOL KDA0340D    |-------|  |3DFX     |                              |
  |4558            |GENDAC |  |500-0004-02                             |
  |                |ICS5342|  |F004221.1|                              |
  |                |       |  |TMU      |                    KM416S1020|
  |                |-------|  |---------|   KM416C254 KM416C254        |
  |SERVICE_SW |---SUB---|                                              |
  |           |         |                                              |
  |           |FLASH.U3 |     |-----------|                            |
|-|           |         |     |3DFX       |                  KM416S1020|
|             |         |     |500-0003-03| KM416C254 KM416C254        |
|             |         |     |F006531.1  |                            |
|             |         |     |FBI        |                            |
|J            |         |     |           | KM416C254 KM416C254        |
|A            |         |     |-----------|                            |
|M            |         |RESET_SW                                      |
|M            |         |                                              |
|A            |---------|                     |-------------|          |
|      512K-EPR.U62       14.31818MHz         |Cyrix        |KM416S1020|
|                            |---------|      |GX MEDIA     |          |
|    |-------|  |------|     |LSI      |      |GXm-233GP    |          |
|    |XILINX |  |KS0164|     |L2A0788  |      |             |          |
|-|  |XCS05  |  |      |     |Cyrix    |      |             |          |
  |  |       |  |------|     |CX5520   |      |             |          |
  |  |-------| 16.9344MHz    |---------|      |-------------|KM416S1020|
  | LED               DIP20                                            |
  | |--------------FLASH-DAUGHTERBOARD----------------|                |
  | |                                                 |                |
  | |           FLASH.U30 FLASH.U29              DIP20|                |
  | |-------------------------------------------------|                |
  |--------------------------------------------------------------------|
Notes:
      Cyrix GXm233 - Main CPU; Cyrix GX Media running at 233MHz. Clock is generated by the Cyrix CX5520
                     and a 14.31818MHz xtal. That gives a 66.6MHz bus clock with a 3.5X multiplier for 233MHz
      Cyrix CX5220 - CPU-support chipset (BGA IC)
      FLASH.U29/30 - Intel Strata-Flash DA28F320J5 SSOP56 contained on a plug-in daughterboard; graphics ROMs
      FLASH.U3     - Intel Strata-Flash DA28F320J5 SSOP56 contained on a plug-in daughterboard; main program
      KS0164       - Samsung Electronics KS0164 General Midi compliant 32-voice Wavetable Synthesizer chip
                     with built-in 16bit CPU and MPU-401 compatibility (QFP100)
      512K-EPR     - 512k EPROM, boot-loader program. EPROM is tied to the KS0164 and the XCS05
      DIP20        - not-populated sockets
      KDA0340D     - Samsung KDA0340D CMOS low-power two-channel digital-to-analog converter (SOP28)
      KM416S1020   - Samsung 1M x16 SDRAM (x4, TSSOP50)
      KM416C254    - Samsung 256k x16 DRAM (x8, SOJ40)
      ICS5342      - combination dual programmable clock generator, 256bytes x18-bit RAM and a triple 8-bit video DAC (PLCC68)
      XCS05        - Xilinx Spartan XCS05 FPGA (PLCC84)

***************************************************************************/

#define ADDRESS_MAP_MODERN


#include "emu.h"
#include "cpu/i386/i386.h"
#include "memconv.h"
#include "devconv.h"
#include "machine/8237dma.h"
#include "machine/pic8259.h"
#include "machine/pit8253.h"
#include "machine/mc146818.h"
#include "machine/pcshare.h"
#include "machine/pci.h"
#include "machine/8042kbdc.h"
#include "machine/pckeybrd.h"
#include "machine/idectrl.h"
#include "video/voodoo.h"


class funkball_state : public driver_device
{
public:
	funkball_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		  m_maincpu(*this, "maincpu"),
		  m_pit8254(*this, "pit8254"),
		  m_dma8237_1(*this, "dma8237_1"),
		  m_dma8237_2(*this, "dma8237_2"),
		  m_pic8259_1(*this, "pic8259_1"),
		  m_pic8259_2(*this, "pic8259_2"),
		  m_voodoo(*this, "voodoo_0")
		  { }

	int m_dma_channel;
	UINT8 m_dma_offset[2][4];
	UINT8 m_at_pages[0x10];

	UINT8 m_funkball_config_reg_sel;
	UINT8 m_funkball_config_regs[256];
	UINT32 m_cx5510_regs[256/4];
	UINT16 m_flash_addr;
	UINT32 *m_unk_ram;
	UINT8 *m_bios_ram;
	UINT8 m_flash_cmd;
	UINT8 m_flash_data_cmd;

	UINT32 m_biu_ctrl_reg[256/4];

	// devices
	required_device<cpu_device> m_maincpu;
	required_device<pit8254_device> m_pit8254;
	required_device<i8237_device> m_dma8237_1;
	required_device<i8237_device> m_dma8237_2;
	required_device<pic8259_device> m_pic8259_1;
	required_device<pic8259_device> m_pic8259_2;
	required_device<voodoo_device> m_voodoo;

	DECLARE_READ8_MEMBER( get_slave_ack );
	DECLARE_WRITE8_MEMBER( flash_w );
	DECLARE_READ8_MEMBER( flash_data_r );
	DECLARE_WRITE8_MEMBER( flash_data_w );
//  DECLARE_WRITE8_MEMBER( bios_ram_w );
	DECLARE_READ32_MEMBER( test_r );

	DECLARE_READ8_MEMBER( fdc_r );
	DECLARE_WRITE8_MEMBER( fdc_w );

	virtual void video_start();
	virtual bool screen_update(screen_device &screen, bitmap_t &bitmap, const rectangle &cliprect);

	struct
	{
		/* PCI */
		UINT32 command;
		UINT32 base_addr;

		UINT32 init_enable;
	} m_voodoo_pci_regs;
};

void funkball_state::video_start()
{

}

bool funkball_state::screen_update( screen_device &screen, bitmap_t &bitmap, const rectangle &cliprect )
{
	return voodoo_update(m_voodoo, bitmap, cliprect) ? 0 : UPDATE_HAS_NOT_CHANGED;
}

static UINT32 voodoo_0_pci_r(device_t *busdevice, device_t *device, int function, int reg, UINT32 mem_mask)
{
	funkball_state* state = device->machine().driver_data<funkball_state>();
	UINT32 val = 0;

	printf("Voodoo PCI R: %x\n", reg);

	switch (reg)
	{
		case 0:
			val = 0x0001121a;
			break;
		case 0x10:
			val = state->m_voodoo_pci_regs.base_addr;
			break;
		case 0x40:
			val = state->m_voodoo_pci_regs.init_enable;
			break;
	}
	return val;
}

static void voodoo_0_pci_w(device_t *busdevice, device_t *device, int function, int reg, UINT32 data, UINT32 mem_mask)
{
	funkball_state* state = device->machine().driver_data<funkball_state>();

	printf("Voodoo [%x]: %x\n", reg, data);

	switch (reg)
	{
		case 0x04:
			state->m_voodoo_pci_regs.command = data & 0x3;
			break;
		case 0x10:
			if (data == 0xffffffff)
				state->m_voodoo_pci_regs.base_addr = 0xff000000;
			else
				state->m_voodoo_pci_regs.base_addr = data;
			break;
		case 0x40:
			state->m_voodoo_pci_regs.init_enable = data;
			voodoo_set_init_enable(state->m_voodoo, data);
			break;
	}
}

static UINT32 cx5510_pci_r(device_t *busdevice, device_t *device, int function, int reg, UINT32 mem_mask)
{
	funkball_state *state = busdevice->machine().driver_data<funkball_state>();

	//mame_printf_debug("CX5510: PCI read %d, %02X, %08X\n", function, reg, mem_mask);
	switch (reg)
	{
		case 0:		return 0x00001078;
	}

	return state->m_cx5510_regs[reg/4];
}

static void cx5510_pci_w(device_t *busdevice, device_t *device, int function, int reg, UINT32 data, UINT32 mem_mask)
{
	funkball_state *state = busdevice->machine().driver_data<funkball_state>();

	//mame_printf_debug("CX5510: PCI write %d, %02X, %08X, %08X\n", function, reg, data, mem_mask);
	COMBINE_DATA(state->m_cx5510_regs + (reg/4));
}

#if 0
static READ32_DEVICE_HANDLER( ide_r )
{
	return ide_controller32_r(device, 0x1f0/4 + offset, mem_mask);
}

static WRITE32_DEVICE_HANDLER( ide_w )
{
	ide_controller32_w(device, 0x1f0/4 + offset, data, mem_mask);
}

static READ32_DEVICE_HANDLER( fdc_r )
{
	return ide_controller32_r(device, 0x3f0/4 + offset, mem_mask);
}

static WRITE32_DEVICE_HANDLER( fdc_w )
{
	//mame_printf_debug("FDC: write %08X, %08X, %08X\n", data, offset, mem_mask);
	ide_controller32_w(device, 0x3f0/4 + offset, data, mem_mask);
}
#endif

READ8_MEMBER( funkball_state::fdc_r )
{
	//printf("%02x\n",offset);
	if(offset == 0xd)
		return 0x20;

	return 0;
}

WRITE8_MEMBER( funkball_state::fdc_w )
{
	if(offset == 8)
	{
		if(data == 0x0d)
			printf("\n");
		else
			printf("%c",data);
	}
}


static READ8_HANDLER(at_page8_r)
{
	funkball_state *state = space->machine().driver_data<funkball_state>();
	UINT8 data = state->m_at_pages[offset % 0x10];

	switch(offset % 8) {
	case 1:
		data = state->m_dma_offset[(offset / 8) & 1][2];
		break;
	case 2:
		data = state->m_dma_offset[(offset / 8) & 1][3];
		break;
	case 3:
		data = state->m_dma_offset[(offset / 8) & 1][1];
		break;
	case 7:
		data = state->m_dma_offset[(offset / 8) & 1][0];
		break;
	}
	return data;
}


static WRITE8_HANDLER(at_page8_w)
{
	funkball_state *state = space->machine().driver_data<funkball_state>();
	state->m_at_pages[offset % 0x10] = data;

	switch(offset % 8) {
	case 1:
		state->m_dma_offset[(offset / 8) & 1][2] = data;
		break;
	case 2:
		state->m_dma_offset[(offset / 8) & 1][3] = data;
		break;
	case 3:
		state->m_dma_offset[(offset / 8) & 1][1] = data;
		break;
	case 7:
		state->m_dma_offset[(offset / 8) & 1][0] = data;
		break;
	}
}

static READ32_HANDLER(at_page32_r)
{
	return read32le_with_read8_handler(at_page8_r, space, offset, mem_mask);
}


static WRITE32_HANDLER(at_page32_w)
{
	write32le_with_write8_handler(at_page8_w, space, offset, data, mem_mask);
}

static READ8_DEVICE_HANDLER(at_dma8237_2_r)
{
	return i8237_r(device, offset / 2);
}

static WRITE8_DEVICE_HANDLER(at_dma8237_2_w)
{
	i8237_w(device, offset / 2, data);
}

static READ32_DEVICE_HANDLER(at32_dma8237_2_r)
{
	return read32le_with_read8_device_handler(at_dma8237_2_r, device, offset, mem_mask);
}

static WRITE32_DEVICE_HANDLER(at32_dma8237_2_w)
{
	write32le_with_write8_device_handler(at_dma8237_2_w, device, offset, data, mem_mask);
}




static WRITE_LINE_DEVICE_HANDLER( pc_dma_hrq_changed )
{
	cputag_set_input_line(device->machine(), "maincpu", INPUT_LINE_HALT, state ? ASSERT_LINE : CLEAR_LINE);

	/* Assert HLDA */
	i8237_hlda_w( device, state );
}


static READ8_HANDLER( pc_dma_read_byte )
{
	funkball_state *state = space->machine().driver_data<funkball_state>();
	offs_t page_offset = (((offs_t) state->m_dma_offset[0][state->m_dma_channel]) << 16)
		& 0xFF0000;

	return space->read_byte(page_offset + offset);
}


static WRITE8_HANDLER( pc_dma_write_byte )
{
	funkball_state *state = space->machine().driver_data<funkball_state>();
	offs_t page_offset = (((offs_t) state->m_dma_offset[0][state->m_dma_channel]) << 16)
		& 0xFF0000;

	space->write_byte(page_offset + offset, data);
}

static void set_dma_channel(device_t *device, int channel, int state)
{
	funkball_state *drvstate = device->machine().driver_data<funkball_state>();
	if (!state) drvstate->m_dma_channel = channel;
}

static WRITE_LINE_DEVICE_HANDLER( pc_dack0_w ) { set_dma_channel(device, 0, state); }
static WRITE_LINE_DEVICE_HANDLER( pc_dack1_w ) { set_dma_channel(device, 1, state); }
static WRITE_LINE_DEVICE_HANDLER( pc_dack2_w ) { set_dma_channel(device, 2, state); }
static WRITE_LINE_DEVICE_HANDLER( pc_dack3_w ) { set_dma_channel(device, 3, state); }

static I8237_INTERFACE( dma8237_1_config )
{
	DEVCB_LINE(pc_dma_hrq_changed),
	DEVCB_NULL,
	DEVCB_MEMORY_HANDLER("maincpu", PROGRAM, pc_dma_read_byte),
	DEVCB_MEMORY_HANDLER("maincpu", PROGRAM, pc_dma_write_byte),
	{ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL },
	{ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL },
	{ DEVCB_LINE(pc_dack0_w), DEVCB_LINE(pc_dack1_w), DEVCB_LINE(pc_dack2_w), DEVCB_LINE(pc_dack3_w) }
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

static UINT8 funkball_config_reg_r(device_t *device)
{
	funkball_state *state = device->machine().driver_data<funkball_state>();

	//mame_printf_debug("funkball_config_reg_r %02X\n", funkball_config_reg_sel);
	return state->m_funkball_config_regs[state->m_funkball_config_reg_sel];
}

static void funkball_config_reg_w(device_t *device, UINT8 data)
{
	funkball_state *state = device->machine().driver_data<funkball_state>();

	//mame_printf_debug("funkball_config_reg_w %02X, %02X\n", funkball_config_reg_sel, data);
	state->m_funkball_config_regs[state->m_funkball_config_reg_sel] = data;
}

static READ8_DEVICE_HANDLER( io20_r )
{
	UINT8 r = 0;

	// 0x22, 0x23, Cyrix configuration registers
	if (offset == 0x02)
	{
	}
	else if (offset == 0x03)
	{
		r = funkball_config_reg_r(device);
	}
	else
	{
		r = pic8259_r(device, offset);
	}
	return r;
}

static WRITE8_DEVICE_HANDLER( io20_w )
{
	funkball_state *state = device->machine().driver_data<funkball_state>();

	// 0x22, 0x23, Cyrix configuration registers
	if (offset == 0x02)
	{
		state->m_funkball_config_reg_sel = data;
	}
	else if (offset == 0x03)
	{
		funkball_config_reg_w(device, data);
	}
	else
	{
		pic8259_w(device, offset, data);
	}
}

WRITE8_MEMBER( funkball_state::flash_w )
{
	if(!(offset & 0x2))
	{
		m_flash_addr = (offset & 1) ? ((m_flash_addr & 0xff) | (data << 8)) : ((m_flash_addr & 0xff00) | (data));
		//printf("%08x ADDR\n",m_flash_addr << 16);
	}
	else if(offset == 2)
	{
		/* 0x83: read from u29/u30
           0x03: read from u3
           0x81: init device
        */
		m_flash_cmd = data;
		printf("%02x CMD\n",data);
	}
	else
		printf("%02x %02x\n",offset,data);
}

READ8_MEMBER( funkball_state::flash_data_r )
{
	if(m_flash_data_cmd == 0x90)
	{
		if(offset == 0 && (m_flash_addr == 0))
			return 0x89; // manufacturer code

		if(offset == 2 && (m_flash_addr == 0))
			return 0x14; // device code, 32 MBit in both cases

		if(offset > 3)
			printf("%02x FLASH DATA 0x90\n",offset);

		return 0;
	}

	if(m_flash_data_cmd == 0xff)
	{
		UINT8 *ROM = machine().region(m_flash_cmd & 0x80 ? "prg_flash" : "data_flash")->base();

		return ROM[offset + (m_flash_addr << 16)];
	}

	printf("%02x %08x %02x %02x\n",offset,m_flash_addr << 16,m_flash_cmd,m_flash_data_cmd);

	return 0;
}

WRITE8_MEMBER( funkball_state::flash_data_w )
{
	if(offset == 0)
	{
		m_flash_data_cmd = data;
	}
	else
		printf("%08x %02x FLASH DATA W %08x\n",offset,data,m_flash_addr << 16);
}

static READ32_HANDLER( biu_ctrl_r )
{
	funkball_state *state = space->machine().driver_data<funkball_state>();

	if (offset == 0)
	{
		return 0xffffff;
	}
	return state->m_biu_ctrl_reg[offset];
}

static WRITE32_HANDLER( biu_ctrl_w )
{
	funkball_state *state = space->machine().driver_data<funkball_state>();

	//mame_printf_debug("biu_ctrl_w %08X, %08X, %08X\n", data, offset, mem_mask);
	COMBINE_DATA(state->m_biu_ctrl_reg + offset);

	if (offset == 0x0c/4)		// BC_XMAP_3 register
	{
		const char *const banknames[8] = { "bios_ext1", "bios_ext2", "bios_ext3","bios_ext4", "bios_bank1", "bios_bank2", "bios_bank3", "bios_bank4" };
		int i;

		for(i=0;i<8;i++)
		{
			if (data & 0x1 << i*4)		// enable RAM access to region 0xe0000 - 0xfffff
				memory_set_bankptr(space->machine(), banknames[i], state->m_bios_ram + (0x4000 * i));
			else					// disable RAM access (reads go to BIOS ROM)
				memory_set_bankptr(space->machine(), banknames[i], space->machine().region("bios")->base() + (0x4000 * i));
		}
	}
}

static WRITE8_HANDLER( bios_ram_w )
{
	funkball_state *state = space->machine().driver_data<funkball_state>();

	if(state->m_biu_ctrl_reg[0x0c/4] & (2 << ((offset & 0x4000)>>14)*4)) // memory is write-able
	{
		state->m_bios_ram[offset] = data;
	}
}

READ32_MEMBER( funkball_state::test_r )
{
	return -1;
}

static ADDRESS_MAP_START(funkball_map, AS_PROGRAM, 32, funkball_state)
	AM_RANGE(0x00000000, 0x0009ffff) AM_RAM
	AM_RANGE(0x000a0000, 0x000affff) AM_RAM
	AM_RANGE(0x000b0000, 0x000bffff) AM_READWRITE8(flash_data_r,flash_data_w,0xffffffff)
	AM_RANGE(0x000c0000, 0x000cffff) AM_RAM
	AM_RANGE(0x000d0000, 0x000dffff) AM_RAM
	AM_RANGE(0x000e0000, 0x000e3fff) AM_ROMBANK("bios_ext1")
	AM_RANGE(0x000e4000, 0x000e7fff) AM_ROMBANK("bios_ext2")
	AM_RANGE(0x000e8000, 0x000ebfff) AM_ROMBANK("bios_ext3")
	AM_RANGE(0x000ec000, 0x000effff) AM_ROMBANK("bios_ext4")
	AM_RANGE(0x000f0000, 0x000f3fff) AM_ROMBANK("bios_bank1")
	AM_RANGE(0x000f4000, 0x000f7fff) AM_ROMBANK("bios_bank2")
	AM_RANGE(0x000f8000, 0x000fbfff) AM_ROMBANK("bios_bank3")
	AM_RANGE(0x000fc000, 0x000fffff) AM_ROMBANK("bios_bank4")
	AM_RANGE(0x000e0000, 0x000fffff) AM_WRITE8_LEGACY(bios_ram_w,0xffffffff)
	AM_RANGE(0x00100000, 0x07ffffff) AM_RAM
//  AM_RANGE(0x08000000, 0x0fffffff) AM_NOP
	AM_RANGE(0x40008000, 0x400080ff) AM_READWRITE_LEGACY(biu_ctrl_r, biu_ctrl_w)
	AM_RANGE(0x40010e00, 0x40010eff) AM_RAM AM_BASE(m_unk_ram)
	AM_RANGE(0xff000000, 0xffffdfff) AM_DEVREADWRITE_LEGACY("voodoo_0", voodoo_r, voodoo_w)
	AM_RANGE(0xfffe0000, 0xffffffff) AM_ROM AM_REGION("bios", 0)	/* System BIOS */
ADDRESS_MAP_END

static ADDRESS_MAP_START(funkball_io, AS_IO, 32, funkball_state)
	AM_RANGE(0x0000, 0x001f) AM_DEVREADWRITE8_LEGACY("dma8237_1", i8237_r, i8237_w, 0xffffffff)
	AM_RANGE(0x0020, 0x003f) AM_DEVREADWRITE8_LEGACY("pic8259_1", io20_r, io20_w, 0xffffffff)
	AM_RANGE(0x0040, 0x005f) AM_DEVREADWRITE8_LEGACY("pit8254", pit8253_r, pit8253_w, 0xffffffff)
	AM_RANGE(0x0060, 0x006f) AM_READWRITE_LEGACY(kbdc8042_32le_r,			kbdc8042_32le_w)
	AM_RANGE(0x0070, 0x007f) AM_DEVREADWRITE8("rtc", mc146818_device, read, write, 0xffffffff) /* todo: nvram (CMOS Setup Save)*/
	AM_RANGE(0x0080, 0x009f) AM_READWRITE_LEGACY(at_page32_r,				at_page32_w)
	AM_RANGE(0x00a0, 0x00bf) AM_DEVREADWRITE8_LEGACY("pic8259_2", pic8259_r, pic8259_w, 0xffffffff)
	AM_RANGE(0x00c0, 0x00df) AM_DEVREADWRITE_LEGACY("dma8237_2", at32_dma8237_2_r, at32_dma8237_2_w)
	AM_RANGE(0x00e8, 0x00ef) AM_NOP

//  AM_RANGE(0x01f0, 0x01f7) AM_DEVREADWRITE_LEGACY("ide", ide_r, ide_w)
//  AM_RANGE(0x03f0, 0x03ff) AM_DEVREADWRITE_LEGACY("ide", fdc_r, fdc_w)
	AM_RANGE(0x03f0, 0x03ff) AM_READWRITE8(fdc_r,fdc_w,0xffffffff)

	AM_RANGE(0x0cf8, 0x0cff) AM_DEVREADWRITE_LEGACY("pcibus", pci_32le_r,	pci_32le_w)

	AM_RANGE(0x0360, 0x0363) AM_WRITE8(flash_w,0xffffffff)

//  AM_RANGE(0x0320, 0x0323) AM_READ(test_r)
	AM_RANGE(0x036c, 0x036f) AM_READ(test_r)
ADDRESS_MAP_END

static INPUT_PORTS_START( funkball )
INPUT_PORTS_END

static const struct pit8253_config funkball_pit8254_config =
{
	{
		{
			4772720/4,				/* heartbeat IRQ */
			DEVCB_NULL,
			DEVCB_DEVICE_LINE("pic8259_1", pic8259_ir0_w)
		}, {
			4772720/4,				/* dram refresh */
			DEVCB_NULL,
			DEVCB_NULL
		}, {
			4772720/4,				/* pio port c pin 4, and speaker polling enough */
			DEVCB_NULL,
			DEVCB_NULL
		}
	}
};

static WRITE_LINE_DEVICE_HANDLER( funkball_pic8259_1_set_int_line )
{
	funkball_state *drvstate = device->machine().driver_data<funkball_state>();
	device_set_input_line(drvstate->m_maincpu, 0, state ? HOLD_LINE : CLEAR_LINE);
}

READ8_MEMBER( funkball_state::get_slave_ack )
{
	if (offset==2) { // IRQ = 2
		logerror("pic8259_slave_ACK!\n");
		return pic8259_acknowledge(m_pic8259_2);
	}
	return 0x00;
}

static const struct pic8259_interface funkball_pic8259_1_config =
{
	DEVCB_LINE(funkball_pic8259_1_set_int_line),
	DEVCB_LINE_VCC,
	DEVCB_MEMBER(funkball_state,get_slave_ack)
};

static const struct pic8259_interface funkball_pic8259_2_config =
{
	DEVCB_DEVICE_LINE("pic8259_1", pic8259_ir2_w),
	DEVCB_LINE_GND,
	DEVCB_NULL
};

static void set_gate_a20(running_machine &machine, int a20)
{
	funkball_state *state = machine.driver_data<funkball_state>();

	device_set_input_line(state->m_maincpu, INPUT_LINE_A20, a20);
}

static void keyboard_interrupt(running_machine &machine, int state)
{
	funkball_state *drvstate = machine.driver_data<funkball_state>();
	pic8259_ir1_w(drvstate->m_pic8259_1, state);
}

static int funkball_get_out2(running_machine &machine)
{
	funkball_state *state = machine.driver_data<funkball_state>();
	return pit8253_get_output(state->m_pit8254, 2 );
}

static const struct kbdc8042_interface at8042 =
{
	KBDC8042_AT386, set_gate_a20, keyboard_interrupt, NULL, funkball_get_out2
};

static void funkball_set_keyb_int(running_machine &machine, int state)
{
	funkball_state *drvstate = machine.driver_data<funkball_state>();
	pic8259_ir1_w(drvstate->m_pic8259_1, state);
}

static IRQ_CALLBACK(irq_callback)
{
	funkball_state *state = device->machine().driver_data<funkball_state>();
	return pic8259_acknowledge( state->m_pic8259_1);
}

static void ide_interrupt(device_t *device, int state)
{
	funkball_state *drvstate = device->machine().driver_data<funkball_state>();
	pic8259_ir6_w(drvstate->m_pic8259_2, state);
}

static MACHINE_START( funkball )
{
	funkball_state *state = machine.driver_data<funkball_state>();

	state->m_bios_ram = auto_alloc_array(machine, UINT8, 0x20000);

	init_pc_common(machine, PCCOMMON_KEYBOARD_AT, funkball_set_keyb_int);

	device_set_irq_callback(state->m_maincpu, irq_callback);

	kbdc8042_init(machine, &at8042);

	/* defaults, otherwise it won't boot */
	state->m_unk_ram[0x010/4] = 0x2f8d85ff;
	state->m_unk_ram[0x018/4] = 0x000018c5;
}

static MACHINE_RESET( funkball )
{
	funkball_state *state = machine.driver_data<funkball_state>();
	memory_set_bankptr(machine, "bios_ext1", machine.region("bios")->base() + 0x00000);
	memory_set_bankptr(machine, "bios_ext2", machine.region("bios")->base() + 0x04000);
	memory_set_bankptr(machine, "bios_ext3", machine.region("bios")->base() + 0x08000);
	memory_set_bankptr(machine, "bios_ext4", machine.region("bios")->base() + 0x0c000);
	memory_set_bankptr(machine, "bios_bank1", machine.region("bios")->base() + 0x10000);
	memory_set_bankptr(machine, "bios_bank2", machine.region("bios")->base() + 0x14000);
	memory_set_bankptr(machine, "bios_bank3", machine.region("bios")->base() + 0x18000);
	memory_set_bankptr(machine, "bios_bank4", machine.region("bios")->base() + 0x1c000);
	state->m_voodoo_pci_regs.base_addr = 0xff000000;
}

static MACHINE_CONFIG_START( funkball, funkball_state )
	MCFG_CPU_ADD("maincpu", MEDIAGX, 66666666*3.5/16) // 66,6 MHz x 3.5
	MCFG_CPU_PROGRAM_MAP(funkball_map)
	MCFG_CPU_IO_MAP(funkball_io)

	MCFG_MACHINE_START(funkball)
	MCFG_MACHINE_RESET(funkball)

	MCFG_PIT8254_ADD( "pit8254", funkball_pit8254_config )
	MCFG_I8237_ADD( "dma8237_1", XTAL_14_31818MHz/3, dma8237_1_config )
	MCFG_I8237_ADD( "dma8237_2", XTAL_14_31818MHz/3, dma8237_2_config )
	MCFG_PIC8259_ADD( "pic8259_1", funkball_pic8259_1_config )
	MCFG_PIC8259_ADD( "pic8259_2", funkball_pic8259_2_config )

	MCFG_MC146818_ADD( "rtc", MC146818_STANDARD )

	MCFG_PCI_BUS_ADD("pcibus", 0)
	MCFG_PCI_BUS_DEVICE(7, "voodoo_0", voodoo_0_pci_r, voodoo_0_pci_w)
	MCFG_PCI_BUS_DEVICE(18, NULL, cx5510_pci_r, cx5510_pci_w)

	MCFG_IDE_CONTROLLER_ADD("ide", ide_interrupt)

	/* video hardware */
	MCFG_3DFX_VOODOO_1_ADD("voodoo_0", STD_VOODOO_1_CLOCK, 2, "screen")
	MCFG_3DFX_VOODOO_CPU("maincpu")
	MCFG_3DFX_VOODOO_TMU_MEMORY(0, 4)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_FORMAT(BITMAP_FORMAT_RGB32)
	MCFG_SCREEN_SIZE(1024, 1024)
	MCFG_SCREEN_VISIBLE_AREA(0, 511, 16, 447)
MACHINE_CONFIG_END

ROM_START( funkball )
	ROM_REGION32_LE(0x20000, "bios", ROMREGION_ERASEFF)
	ROM_LOAD( "512k-epr.u62", 0x010000, 0x010000, CRC(cced894a) SHA1(298c81716e375da4b7215f3e588a45ca3ea7e35c) )

	ROM_REGION(0x8000000, "prg_flash", ROMREGION_ERASE00)
	ROM_LOAD( "flash.u3", 0x0000000, 0x400000, CRC(fb376abc) SHA1(ea4c48bb6cd2055431a33f5c426e52c7af6997eb) )

	ROM_REGION(0x8000000, "data_flash", ROMREGION_ERASE00)
	ROM_LOAD( "flash.u29",0x0000000, 0x400000, CRC(7cf6ff4b) SHA1(4ccdd4864ad92cc218998f3923997119a1a9dd1d) )
	ROM_LOAD( "flash.u30",0x0400000, 0x400000, CRC(1d46717a) SHA1(acfbd0a2ccf4d717779733c4a9c639296c3bbe0e) )
ROM_END


GAME(1998, funkball, 0, funkball, funkball, 0, ROT0, "dgPIX Entertainment Inc.", "Funky Ball", GAME_IS_SKELETON)
