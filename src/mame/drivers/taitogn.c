/*

GNET Motherboard
Taito, 1998

The Taito GNET System comprises the following main parts....
- Sony ZN-2 Motherboard (Main CPU/GPU/SPU, RAM, BIOS, EEPROM & peripheral interfaces)
- Taito FC PCB (Sound hardware & FLASHROMs for storage of PCMCIA cart contents)
- Taito CD PCB (PCMCIA cart interface)

Also available are...
- Optional Communication Interface PCB
- Optional Save PCB

On power-up, the system checks for a PCMCIA cart. If the cart matches the contents of the flashROMs,
the game boots immediately with no delay. If the cart doesn't match, it re-flashes the flashROMs with _some_
of the information contained in the cart, which takes approximately 2-3 minutes. The game then resets
and boots up.

If no cart is present on power-up, the Taito GNET logo is displayed, then a message 'SYSTEM ERROR'
Since the logo is shown on boot even without a cart, there must be another sub-BIOS for the initial booting,
which I suspect is one of the flashROMs that is acting like a standard ROM and is not flashed at all.
Upon inspecting the GNET top board, it appears flash.u30 is the sub-BIOS and perhaps U27 is something sound related.
The flashROMs at U55, U56 & U29 appear to be the ones that are re-flashed when swapping game carts.

PCB Layouts
-----------
(Standard ZN2 Motherboard)

ZN-2 COH-3000 (sticker says COH-3002T denoting Taito GNET BIOS version)
|--------------------------------------------------------|
|  LA4705             |---------------------------|      |
|                     |---------------------------|      |
|    AKM_AK4310VM      AT28C16                           |
|  VOL                                                   |
|       S301           COH3002T.353                      |
|                                                        |
|                                                        |
|J                                                       |
|                                                        |
|A              814260    CXD2925Q     EPM7064           |
|                                                        |
|M                                     67.73MHz          |
|                                                        |
|M                                                       |
|            S551    KM4132G271BQ-8                      |
|A                                                       |
|                                CXD8654Q    CXD8661R    |
|                    KM4132G271BQ-8                      |
|CN505  CN506                   53.693MHz    100MHz      |
|            CAT702                                      |
|                                                        |
|CN504  CN503                                            |
|                                                        |
|            MC44200FT                                   |
|  NEC_78081G503        KM416V1204BT-L5  KM416V1204BT-L5 |
|                                                        |
|CN651  CN652                 *                 *        |
|                CN654                                   |
|--------------------------------------------------------|
Notes:
      CN506 - Connector for optional 3rd player controls
      CN505 - Connector for optional 4th player controls
      CN503 - Connector for optional 15kHz external video output (R,G,B,Sync, GND)
      CN504 - Connector for optional 2nd speaker (for stereo output)
      CN652 - Connector for optional trackball
      CN651 - Connector for optional analog controls
      CN654 - Connector for optional memory card
      S301  - Slide switch for stereo or mono sound output
      S551  - Dip switch (4 position, defaults all OFF)

      COH3002T.353   - GNET BIOS 4MBit MaskROM type M534002 (SOP40)
      AT28C16        - Atmel AT28C16 2K x8 EEPROM
      814260-70      - 256K x16 (4MBit) DRAM
      KM4132G271BQ-8 - 128K x 32Bit x 2 Banks SGRAM
      KM416V1204BT-L5- 1M x16 EDO DRAM
      EPM7064        - Altera EPM7064QC100 CPLD (QFP100)
      CAT702         - Protection chip labelled 'TT10' (DIP20)
      *              - Unpopulated position for additional KM416V1204BT-L5 RAMs


FC PCB  K91X0721B  M43X0337B
|--------------------------------------------|
|   |---------------------------|            |
|   |---------------------------|            |
| NJM2100  NJM2100                           |
| MB87078                                    |
| *MB3773     XC95108         DIP40   CAT702 |
| *ADM708AR                                  |
| *UPD6379GR                                 |
|             FLASH.U30                      |
|                                            |
| DIP24                                      |
|                  *RF5C296                  |
| -------CD-PCB------- _                     |
| |                   | |                    |
| |                   | |                    |
| |                   | |                    |
| |                   | |                    |
| |                   | |                    |
| |                   | |                    |
| |                   | |                    |
| |                   |-|                    |
| --------------------                       |
|          M66220FP   FLASH.U55   FLASH16.U29|
|      FLASH.U27             FLASH.U56       |
|*LC321664                                   |
| TMS57002DPHA                *ZSG-2         |
|           LH52B256      25MHz              |
|   MN1020012A                               |
|--------------------------------------------|
Notes:
      DIP40           - Unpopulated socket for 8MBit DIP40 EPROM type AM27C800
      DIP24           - Unpopulated position for FM1208 DIP24 IC
      FLASH.U30       - Intel TE28F160 16MBit FLASHROM (TSOP56)
      FLASH.U29/55/56 - Intel TE28F160 16MBit FLASHROM (TSOP56)
      FLASH.U27       - Intel E28F400 4MBit FLASHROM (TSOP48)
      LH52B256        - Sharp 32K x8 SRAM (SOP28)
      LC321664        - Sanyo 64K x16 EDO DRAM (SOP40)
      XC95108         - XILINX XC95108 CPLD labelled 'E65-01' (QFP100)
      MN1020012A      - Panasonic MN1020012A Sound CPU (QFP128)
      ZSG-2           - Zoom Corp ZSG-2 Sound DSP (QFP100)
      TMS57002DPHA    - Texas Instruments TMS57002DPHA Sound DSP (QFP80)
      RF5C296         - Ricoh RF5C296 PCMCIA controller (TQFP144)
      M66220FP        - 256 x8bit Mail-Box (Inter-MPU data transfer)
      CAT702          - Protection chip labelled 'TT16' (DIP20)
      CD PCB          - A PCMCIA cart slot connector mounted onto a small daughterboard
      *               - These parts located under the PCB

*/

#include "driver.h"
#include "cpu/mips/psx.h"
#include "includes/psx.h"
#include "machine/at28c16.h"
#include "machine/intelfsh.h"
#include "machine/znsec.h"
#include "machine/idectrl.h"
#include "sound/psx.h"


static unsigned char cis[512];
static int locked;

// rf5c296 is very inaccurate at that point, it hardcodes the gnet config

static unsigned char rf5c296_reg = 0;

static void rf5c296_reg_w(ATTR_UNUSED running_machine *machine, UINT8 reg, UINT8 data)
{
	//	fprintf(stderr, "rf5c296_reg_w %02x, %02x (%s)\n", reg, data, cpuexec_describe_context(machine));
}

static UINT8 rf5c296_reg_r(ATTR_UNUSED running_machine *machine, UINT8 reg)
{
	//	fprintf(stderr, "rf5c296_reg_r %02x (%s)\n", reg, cpuexec_describe_context(machine));
	return 0x00;
}

static WRITE32_HANDLER(rf5c296_io_w)
{
	if(offset < 2) {
		ide_controller32_pcmcia_w(devtag_get_device(space->machine, "card"), offset, data, mem_mask);
		return;
	}

	if(offset == 0x3e0/4) {
		if(ACCESSING_BITS_0_7)
			rf5c296_reg = data;
		if(ACCESSING_BITS_8_15)
			rf5c296_reg_w(space->machine, rf5c296_reg, data >> 8);
	}
}

static READ32_HANDLER(rf5c296_io_r)
{
	if(offset < 2)
		return ide_controller32_pcmcia_r(devtag_get_device(space->machine, "card"), offset, mem_mask);

	offset *= 4;

	if(offset == 0x3e0/4) {
		UINT32 res = 0xffff0000;
		if(ACCESSING_BITS_0_7)
			res |= rf5c296_reg;
		if(ACCESSING_BITS_8_15)
			res |= rf5c296_reg_r(space->machine, rf5c296_reg) << 8;
		return res;
	}

	return 0xffffffff;
}

// Hardcoded to reach the pcmcia CIS

static READ32_HANDLER(rf5c296_mem_r)
{
	if(offset < 0x80)
		return (cis[offset*2+1] << 16) | cis[offset*2];

	switch(offset) {
	case 0x080: return 0x00800041;
	case 0x081: return 0x0000002e;
	case 0x100: return locked ? 0x00010000 : 0;
	default:
		return 0;
	}
}

static WRITE32_HANDLER(rf5c296_mem_w)
{
	if(offset >= 0x140 && offset <= 0x144) {
		UINT8 key[5];
		int pos = (offset - 0x140)*2;
		UINT8 v, k;
		if(ACCESSING_BITS_16_23) {
			v = data >> 16;
			pos++;
		} else
			v = data;
		chd_get_metadata(get_disk_handle("card"), HARD_DISK_KEY_METADATA_TAG, 0, key, 5, 0, 0, 0);
		k = pos < 5 ? key[pos] : 0;
		if(v == k)
			locked &= ~(1 << pos);
		else
			locked |= 1 << pos;
	}
}


// Flash handling

static UINT32 gen_flash_r(running_machine *machine, int chip, offs_t offset, UINT32 mem_mask)
{
	UINT32 res = 0;
	offset *= 2;
	if(ACCESSING_BITS_0_15)
		res |= intelflash_read(chip, offset);
	if(ACCESSING_BITS_16_31)
		res |= intelflash_read(chip, offset+1) << 16;
	return res;
}

static void gen_flash_w(running_machine *machine, int chip, offs_t offset, UINT32 data, UINT32 mem_mask)
{
	offset *= 2;
	if(ACCESSING_BITS_0_15)
		intelflash_write(chip, offset, data);
	if(ACCESSING_BITS_16_31)
	    intelflash_write(chip, offset+1, data >> 16);
}


static READ32_HANDLER(flash_subbios_r)
{
	return gen_flash_r(space->machine, 0, offset, mem_mask);
}

static WRITE32_HANDLER(flash_subbios_w)
{
	gen_flash_w(space->machine, 0, offset, data, mem_mask);
}

static READ32_HANDLER(flash_mn102_r)
{
	return gen_flash_r(space->machine, 1, offset, mem_mask);
}

static WRITE32_HANDLER(flash_mn102_w)
{
	gen_flash_w(space->machine, 1, offset, data, mem_mask);
}

static READ32_HANDLER(flash_s1_r)
{
	return gen_flash_r(space->machine, 2, offset, mem_mask);
}

static WRITE32_HANDLER(flash_s1_w)
{
	gen_flash_w(space->machine, 2, offset, data, mem_mask);
}

static READ32_HANDLER(flash_s2_r)
{
	return gen_flash_r(space->machine, 3, offset, mem_mask);
}

static WRITE32_HANDLER(flash_s2_w)
{
	gen_flash_w(space->machine, 3, offset, data, mem_mask);
}

static READ32_HANDLER(flash_s3_r)
{
	return gen_flash_r(space->machine, 4, offset, mem_mask);
}

static WRITE32_HANDLER(flash_s3_w)
{
	gen_flash_w(space->machine, 4, offset, data, mem_mask);
}



static void install_handlers(running_machine *machine, int mode)
{
	const address_space *a = cpu_get_address_space(machine->cpu[0], ADDRESS_SPACE_PROGRAM);
	if(mode == 0) {
		// Mode 0 has access to the subbios, the mn102 flash and the rf5c296 mem zone
		memory_install_readwrite32_handler(a, 0x1f000000, 0x1f1fffff, 0, 0, flash_subbios_r, flash_subbios_w);
		memory_install_readwrite32_handler(a, 0x1f200000, 0x1f2fffff, 0, 0, rf5c296_mem_r, rf5c296_mem_w);
		memory_install_readwrite32_handler(a, 0x1f300000, 0x1f37ffff, 0, 0, flash_mn102_r, flash_mn102_w);
		memory_install_readwrite32_handler(a, 0x1f380000, 0x1f5fffff, 0, 0, SMH_NOP, SMH_NOP);

	} else {
		// Mode 1 has access to the 3 samples flashes
		memory_install_readwrite32_handler(a, 0x1f000000, 0x1f1fffff, 0, 0, flash_s1_r, flash_s1_w);
		memory_install_readwrite32_handler(a, 0x1f200000, 0x1f3fffff, 0, 0, flash_s2_r, flash_s2_w);
		memory_install_readwrite32_handler(a, 0x1f400000, 0x1f5fffff, 0, 0, flash_s3_r, flash_s3_w);
	}
}

// Misc. controls

static UINT32 control = 0, control2 = 0, control3;

static READ32_HANDLER(control_r)
{
	//      fprintf(stderr, "gn_r %08x @ %08x (%s)\n", 0x1fb00000+4*offset, mem_mask, cpuexec_describe_context(space->machine));
	return control;
}

static WRITE32_HANDLER(control_w)
{
	// 20 = watchdog
	// 04 = select bank

	// According to the rom code, bits 1-0 may be part of the bank
	// selection too, but they're always 0.

	UINT32 p = control;
	COMBINE_DATA(&control);

#if 0
	if((p ^ control) & ~0x20)
		fprintf(stderr, "control = %c%c.%c %c%c%c%c (%s)\n",
				control & 0x80 ? '1' : '0',
				control & 0x40 ? '1' : '0',
				control & 0x10 ? '1' : '0',
				control & 0x08 ? '1' : '0',
				control & 0x04 ? 'f' : '-',
				control & 0x02 ? '1' : '0',
				control & 0x01 ? '1' : '0',
				cpuexec_describe_context(space->machine));
#endif

	if((p ^ control) & 0x04)
		install_handlers(space->machine, control & 4 ? 1 : 0);
}

static WRITE32_HANDLER(control2_w)
{
	COMBINE_DATA(&control2);
}

static READ32_HANDLER(control3_r)
{
	return control3;
}

static WRITE32_HANDLER(control3_w)
{
	COMBINE_DATA(&control3);

	// card reset, maybe
	if(control3 & 2) {
		devtag_reset(space->machine, "card");
		locked = 0x1ff;
	}
}

static READ32_HANDLER(gn_1fb70000_r)
{
	// (1328) 1348 tests mask 0002, 8 times.
	// Called by 1434, exit at 143c
	// f -> 4/1
	// end with 4x1 -> ok
	// end with 4x0 -> configid error
	// so returning 2 always works, strange.

	return 2;
}

static WRITE32_HANDLER(gn_1fb70000_w)
{
	// Writes 0 or 1 all the time, it *may* have somthing to do with
	// i/o port width, but then maybe not
}

static READ32_HANDLER(hack1_r)
{
	static int v = 0;
	v = v ^ 8;
	// Probably something to do with sound
	return v;
}



// Lifted from zn.c

static const UINT8 tt10[ 8 ] = { 0x80, 0x20, 0x38, 0x08, 0xf1, 0x03, 0xfe, 0xfc };
static const UINT8 tt16[ 8 ] = { 0xc0, 0x04, 0xf9, 0xe1, 0x60, 0x70, 0xf2, 0x02 };

static UINT32 m_n_znsecsel;
static UINT32 m_b_znsecport;
static int m_n_dip_bit;
static int m_b_lastclock;
static emu_timer *dip_timer;

static READ32_HANDLER( znsecsel_r )
{
	return m_n_znsecsel;
}

static void sio_znsec0_handler( running_machine *machine, int n_data )
{
	if( ( n_data & PSX_SIO_OUT_CLOCK ) == 0 )
        {
			if( m_b_lastclock )
				psx_sio_input( machine, 0, PSX_SIO_IN_DATA, ( znsec_step( 0, ( n_data & PSX_SIO_OUT_DATA ) != 0 ) != 0 ) * PSX_SIO_IN_DATA );
			m_b_lastclock = 0;
        }
	else
        {
			m_b_lastclock = 1;
        }
}

static void sio_znsec1_handler( running_machine *machine, int n_data )
{
	if( ( n_data & PSX_SIO_OUT_CLOCK ) == 0 )
        {
			if( m_b_lastclock )
				psx_sio_input( machine, 0, PSX_SIO_IN_DATA, ( znsec_step( 1, ( n_data & PSX_SIO_OUT_DATA ) != 0 ) != 0 ) * PSX_SIO_IN_DATA );
			m_b_lastclock = 0;
        }
	else
        {
			m_b_lastclock = 1;
        }
}

static void sio_pad_handler( running_machine *machine, int n_data )
{
	if( ( n_data & PSX_SIO_OUT_DTR ) != 0 )
        {
			m_b_znsecport = 1;
        }
	else
        {
			m_b_znsecport = 0;
        }

	psx_sio_input( machine, 0, PSX_SIO_IN_DATA | PSX_SIO_IN_DSR, PSX_SIO_IN_DATA | PSX_SIO_IN_DSR );
}

static void sio_dip_handler( running_machine *machine, int n_data )
{
	if( ( n_data & PSX_SIO_OUT_CLOCK ) == 0 )
	{
		if( m_b_lastclock )
		{
			int bit = ( ( input_port_read(machine, "DSW") >> m_n_dip_bit ) & 1 );
			psx_sio_input( machine, 0, PSX_SIO_IN_DATA, bit * PSX_SIO_IN_DATA );
			m_n_dip_bit++;
			m_n_dip_bit &= 7;
		}
		m_b_lastclock = 0;
	}
	else
	{
		m_b_lastclock = 1;
	}
}

static WRITE32_HANDLER( znsecsel_w )
{
	COMBINE_DATA( &m_n_znsecsel );

	if( ( m_n_znsecsel & 0x80 ) == 0 )
        {
			psx_sio_install_handler( 0, sio_pad_handler );
			psx_sio_input( space->machine, 0, PSX_SIO_IN_DSR, 0 );
        }
	else if( ( m_n_znsecsel & 0x08 ) == 0 )
        {
			znsec_start( 1 );
			psx_sio_install_handler( 0, sio_znsec1_handler );
			psx_sio_input( space->machine, 0, PSX_SIO_IN_DSR, 0 );
        }
	else if( ( m_n_znsecsel & 0x04 ) == 0 )
        {
			znsec_start( 0 );
			psx_sio_install_handler( 0, sio_znsec0_handler );
			psx_sio_input( space->machine, 0, PSX_SIO_IN_DSR, 0 );
        }
	else
        {
			m_n_dip_bit = 0;
			m_b_lastclock = 1;

			psx_sio_install_handler( 0, sio_dip_handler );
			psx_sio_input( space->machine, 0, PSX_SIO_IN_DSR, 0 );

			timer_adjust_oneshot( dip_timer, cpu_clocks_to_attotime( space->cpu, 100 ), 1 );
        }
}

static TIMER_CALLBACK( dip_timer_fired )
{
	psx_sio_input( machine, 0, PSX_SIO_IN_DSR, param * PSX_SIO_IN_DSR );

	if( param )
	{
		timer_adjust_oneshot( dip_timer, cpu_clocks_to_attotime( machine->cpu[0], 50 ), 0 );
	}
}


static READ32_HANDLER( boardconfig_r )
{
	/*
    ------00 mem=4M
    ------01 mem=4M
    ------10 mem=8M
    ------11 mem=16M
    -----0-- smem=hM
    -----1-- smem=2M
    ----0--- vmem=1M
    ----1--- vmem=2M
    000----- rev=-2
    001----- rev=-1
    010----- rev=0
    011----- rev=1
    100----- rev=2
    101----- rev=3
    110----- rev=4
    111----- rev=5
    */

	return 64|32|8;
}


static UINT32 coin_info;

static WRITE32_HANDLER( coin_w )
{
	/* 0x01=counter
       0x02=coin lock 1
       0x08=??
       0x20=coin lock 2
       0x80=??
    */
	COMBINE_DATA (&coin_info);
}

static READ32_HANDLER( coin_r )
{
	return coin_info;
}


// Init and reset


static NVRAM_HANDLER( coh3002t )
{
	nvram_handler_intelflash(machine, 0, file, read_or_write);
	nvram_handler_intelflash(machine, 1, file, read_or_write);
	nvram_handler_intelflash(machine, 2, file, read_or_write);
	nvram_handler_intelflash(machine, 3, file, read_or_write);
	nvram_handler_intelflash(machine, 4, file, read_or_write);

	if(!file) {
		// Only the subbios needs to preexist for the board to work
		memcpy(intelflash_getmemptr(0), memory_region(machine, "subbios"), 0x200000);
	}
}

static DRIVER_INIT( coh3002t )
{
	// Sub-bios (u30)
	intelflash_init(machine, 0, FLASH_INTEL_TE28F160, 0);

	// mn102 program flash (u27)
	intelflash_init(machine, 1, FLASH_INTEL_E28F400, 0);

	// Samples (u29, u55, u56)
	intelflash_init(machine, 2, FLASH_INTEL_TE28F160, 0);
	intelflash_init(machine, 3, FLASH_INTEL_TE28F160, 0);
	intelflash_init(machine, 4, FLASH_INTEL_TE28F160, 0);

	psx_driver_init(machine);
	znsec_init(0, tt10);
	znsec_init(1, tt16);
	psx_sio_install_handler(0, sio_pad_handler);
	dip_timer = timer_alloc(machine,  dip_timer_fired, NULL );

	memset(cis, 0xff, 512);
	chd_get_metadata(get_disk_handle("card"), PCMCIA_CIS_METADATA_TAG, 0, cis, 512, 0, 0, 0);
}

static MACHINE_RESET( coh3002t )
{
	m_b_lastclock = 1;
	locked = 0x1ff;
	install_handlers(machine, 0);
	control = 0;
	psx_machine_init(machine);
	devtag_reset(machine, "card");
}

static ADDRESS_MAP_START( zn_map, ADDRESS_SPACE_PROGRAM, 32 )
	AM_RANGE(0x00000000, 0x003fffff) AM_RAM AM_SHARE(1) AM_BASE(&g_p_n_psxram) AM_SIZE(&g_n_psxramsize) /*
 ram */
	AM_RANGE(0x00400000, 0x007fffff) AM_RAM AM_SHARE(1) /* ram mirror */
	AM_RANGE(0x1f000000, 0x1f1fffff) AM_READWRITE(flash_s1_r, flash_s1_w)
	AM_RANGE(0x1f200000, 0x1f3fffff) AM_READWRITE(flash_s2_r, flash_s2_w)
	AM_RANGE(0x1f400000, 0x1f5fffff) AM_READWRITE(flash_s3_r, flash_s3_w)
	AM_RANGE(0x1f800000, 0x1f8003ff) AM_RAM /* scratchpad */
	AM_RANGE(0x1f801000, 0x1f80100f) AM_RAM /* ?? */
	AM_RANGE(0x1f801014, 0x1f801017) AM_DEVREADWRITE("spu", psx_spu_delay_r, psx_spu_delay_w)
	AM_RANGE(0x1f801020, 0x1f801023) AM_READWRITE(psx_com_delay_r, psx_com_delay_w)
	AM_RANGE(0x1f801040, 0x1f80105f) AM_READWRITE(psx_sio_r, psx_sio_w)
	AM_RANGE(0x1f801070, 0x1f801077) AM_READWRITE(psx_irq_r, psx_irq_w)
	AM_RANGE(0x1f801080, 0x1f8010ff) AM_READWRITE(psx_dma_r, psx_dma_w)
	AM_RANGE(0x1f801100, 0x1f80112f) AM_READWRITE(psx_counter_r, psx_counter_w)
	AM_RANGE(0x1f801810, 0x1f801817) AM_READWRITE(psx_gpu_r, psx_gpu_w)
	AM_RANGE(0x1f801820, 0x1f801827) AM_READWRITE(psx_mdec_r, psx_mdec_w)
	AM_RANGE(0x1f801c00, 0x1f801dff) AM_DEVREADWRITE("spu", psx_spu_r, psx_spu_w)
	AM_RANGE(0x1fa00000, 0x1fa00003) AM_READ_PORT("P1")
	AM_RANGE(0x1fa00100, 0x1fa00103) AM_READ_PORT("P2")
	AM_RANGE(0x1fa00200, 0x1fa00203) AM_READ_PORT("SERVICE")
	AM_RANGE(0x1fa00300, 0x1fa00303) AM_READ_PORT("SYSTEM")
	AM_RANGE(0x1fa10000, 0x1fa10003) AM_READ_PORT("P3")
	AM_RANGE(0x1fa10100, 0x1fa10103) AM_READ_PORT("P4")
	AM_RANGE(0x1fa10200, 0x1fa10203) AM_READ(boardconfig_r)
	AM_RANGE(0x1fa10300, 0x1fa10303) AM_READWRITE(znsecsel_r, znsecsel_w)
	AM_RANGE(0x1fa20000, 0x1fa20003) AM_READWRITE(coin_r, coin_w)
	AM_RANGE(0x1fa30000, 0x1fa30003) AM_READWRITE(control3_r, control3_w)
	AM_RANGE(0x1fa51c00, 0x1fa51dff) AM_DEVREADWRITE("spu", psx_spu_r, psx_spu_w) // systematic read at spu_address + 250000, result dropped, maybe other accesses
	AM_RANGE(0x1fa60000, 0x1fa60003) AM_READ(hack1_r)
	AM_RANGE(0x1faf0000, 0x1faf07ff) AM_DEVREADWRITE8("at28c16", at28c16_r, at28c16_w, 0xffffffff) /* eeprom */
	AM_RANGE(0x1fb00000, 0x1fb0ffff) AM_READWRITE(rf5c296_io_r, rf5c296_io_w)
	AM_RANGE(0x1fb40000, 0x1fb40003) AM_READWRITE(control_r, control_w)
	AM_RANGE(0x1fb60000, 0x1fb60003) AM_WRITE(control2_w)
	AM_RANGE(0x1fb70000, 0x1fb70003) AM_READWRITE(gn_1fb70000_r, gn_1fb70000_w)
	AM_RANGE(0x1fbe0000, 0x1fbe01ff) AM_RAM // 256 bytes com zone with the mn102, low bytes of words only, with additional comm at 1fb80000
	AM_RANGE(0x1fc00000, 0x1fc7ffff) AM_ROM AM_SHARE(2) AM_REGION("mainbios", 0) /* bios */
	AM_RANGE(0x80000000, 0x803fffff) AM_RAM AM_SHARE(1) /* ram mirror */
	AM_RANGE(0x80400000, 0x807fffff) AM_RAM AM_SHARE(1) /* ram mirror */
	AM_RANGE(0x9fc00000, 0x9fc7ffff) AM_ROM AM_SHARE(2) /* bios mirror */
	AM_RANGE(0xa0000000, 0xa03fffff) AM_RAM AM_SHARE(1) /* ram mirror */
	AM_RANGE(0xbfc00000, 0xbfc7ffff) AM_WRITENOP AM_ROM AM_SHARE(2) /* bios mirror */
	AM_RANGE(0xfffe0130, 0xfffe0133) AM_WRITENOP
ADDRESS_MAP_END


static void psx_spu_irq(const device_config *device, UINT32 data)
{
	psx_irq_set(device->machine, data);
}

static const psx_spu_interface psxspu_interface =
{
	&g_p_n_psxram,
	psx_spu_irq,
	psx_dma_install_read_handler,
	psx_dma_install_write_handler
};

static MACHINE_DRIVER_START( coh3002t )
	/* basic machine hardware */
	MDRV_CPU_ADD( "maincpu", PSXCPU, XTAL_100MHz )
	MDRV_CPU_PROGRAM_MAP( zn_map, 0 )
	MDRV_CPU_VBLANK_INT("screen", psx_vblank)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE( 60 )
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE( 1024, 1024 )
	MDRV_SCREEN_VISIBLE_AREA( 0, 639, 0, 479 )

	MDRV_PALETTE_LENGTH( 65536 )

	MDRV_PALETTE_INIT( psx )
	MDRV_VIDEO_START( psx_type2 )
	MDRV_VIDEO_UPDATE( psx )

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MDRV_SOUND_ADD( "spu", PSXSPU, 0 )
	MDRV_SOUND_CONFIG( psxspu_interface )
	MDRV_SOUND_ROUTE(0, "lspeaker", 0.35)
	MDRV_SOUND_ROUTE(1, "rspeaker", 0.35)

	MDRV_MACHINE_RESET( coh3002t )

	MDRV_AT28C16_ADD( "at28c16", 0 )
	MDRV_IDE_CONTROLLER_ADD( "card", 0 )
	MDRV_NVRAM_HANDLER( coh3002t )
MACHINE_DRIVER_END

static INPUT_PORTS_START( coh3002t )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SERVICE")
	PORT_SERVICE_NO_TOGGLE( 0x01, IP_ACTIVE_LOW )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0xf8, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START4 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN4 )

	PORT_START("P3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(4)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x01, 0x01, "Freeze" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Service_Mode ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

//    ROM_LOAD( "flashv2.u30",    0x000000, 0x200000, CRC(f624ebf8) SHA1(be84ef1b083f819f3ab13c983c52bca97bb908ef) )

#define TAITOGNET_BIOS \
	ROM_REGION32_LE( 0x080000, "mainbios", 0 ) \
	ROM_LOAD( "coh-3002t.353", 0x000000, 0x080000, CRC(03967fa7) SHA1(0e17fec2286e4e25deb23d40e41ce0986f373d49) ) \
\
	ROM_REGION32_LE( 0x200000, "subbios", 0 ) \
    ROM_LOAD( "flash.u30",     0x000000, 0x200000, CRC(c48c8236) SHA1(c6dad60266ce2ff635696bc0d91903c543273559) ) \
	ROM_REGION32_LE( 0x80000,  "soundcpu", 0) \
	ROM_FILL( 0, 0x80000, 0xff) \
	ROM_REGION32_LE( 0x600000, "samples", 0) \
	ROM_FILL( 0, 0x600000, 0xff)

ROM_START( taitogn )
	TAITOGNET_BIOS
ROM_END

/* Taito */

ROM_START(raycris)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "raycris", 0, SHA1(015cb0e6c4421cc38809de28c4793b4491386aee))
ROM_END


ROM_START(gobyrc)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "gobyrc", 0, SHA1(0bee1f495fc8b033fd56aad9260ae94abb35eb58))
ROM_END

ROM_START(rcdego)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "rcdego", 0, SHA1(9e177f2a3954cfea0c8c5a288e116324d10f5dd1))
ROM_END

ROM_START(chaoshea)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "chaoshea", 0, SHA1(2f211ac08675ea8ec33c7659a13951db94eaa627))
ROM_END


ROM_START(spuzbobl)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "spuzbobl", 0, SHA1(1b1c72fb7e5656021485fefaef8f2ba48e2b4ea8))
ROM_END


ROM_START(soutenry)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "soutenry", 0, SHA1(9204d0be833d29f37b8cd3fbdf09da69b622254b))
ROM_END

ROM_START(shanghss)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "shanghss", 0, SHA1(7964f71ec5c81d2120d83b63a82f97fbad5a8e6d))
ROM_END

/* Success */

ROM_START(psyvaria)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "psyvaria", 0,  SHA1(a80e164c09d795085b65659d1df60710530549c3))
ROM_END

ROM_START(psyvarrv)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "psyvarrv", 0, SHA1(c12f0d5cbb6ec7f906e3dcdc3047c22de36457a5))
ROM_END


ROM_START(zokuoten)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "zokuoten", 0, SHA1(5ce13db00518f96af64935176c71ec68d2a51938))
ROM_END

/* Takumi */

ROM_START(nightrai)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "nightrai", 0, SHA1(7a4d62018fd19ccbdec12b6d24052d49f8a8359e))
ROM_END

/* Warashi */

ROM_START(usagi)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "usagi", 0, SHA1(edf9dd271957f6cb06feed238ae21100514bef8e))
ROM_END

ROM_START(mahjngoh)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "mahjngoh", 0, SHA1(3ef1110d15582d7c0187438d7ad61765dd121cff))
ROM_END

/* Triangle Service */

ROM_START(xiistag)
	TAITOGNET_BIOS

	DISK_REGION( "card" )
	DISK_IMAGE( "xiistag", 0, SHA1(8e0b2ee88edf0b45b3b4063d65eae236c12ee8ff))
ROM_END


/* A dummy driver, so that the bios can be debugged, and to serve as */
/* parent for the coh-3002t.353 file, so that we do not have to include */
/* it in every zip file */
GAME( 1997, taitogn,  0, coh3002t, coh3002t, coh3002t, ROT0, "Sony/Taito", "Taito GNET", GAME_IS_BIOS_ROOT )

GAME( 199?, raycris,  taitogn, coh3002t, coh3002t, coh3002t, ROT0, "Taito", "Ray Crisis (V2.03J)", 0 )
GAME( 199?, gobyrc, taitogn, coh3002t, coh3002t, coh3002t, ROT0, "Taito", "Go By RC (V2.03O)", GAME_NOT_WORKING ) // custom inputs need calibrating
GAME( 199?, rcdego, gobyrc,  coh3002t, coh3002t, coh3002t, ROT0, "Taito", "RC De Go (V2.03J)", GAME_NOT_WORKING ) // custom inputs need calibrating
GAME( 1998, chaoshea, taitogn, coh3002t, coh3002t, coh3002t, ROT0, "Taito", "Chaos Heat (V2.08J)", 0 )
GAME( 1999, spuzbobl, taitogn, coh3002t, coh3002t, coh3002t, ROT0, "Taito", "Super Puzzle Bobble (V2.05O)", 0 )
GAME( 2000, soutenry, taitogn, coh3002t, coh3002t, coh3002t, ROT0, "Taito", "Soutenryu (V2.07J)", 0 )
GAME( 2000, shanghss, taitogn, coh3002t, coh3002t, coh3002t, ROT0, "Taito", "Shanghai Shoryu Sairin (V2.03J)", 0 )

GAME( 2000, psyvaria, taitogn, coh3002t, coh3002t, coh3002t, ROT270, "Success", "Psyvariar -Medium Unit- (V2.04J)", 0 )
GAME( 2000, psyvarrv, taitogn, coh3002t, coh3002t, coh3002t, ROT270, "Success", "Psyvariar -Revision- (V2.04J)", 0 )
GAME( 2000, zokuoten, taitogn, coh3002t, coh3002t, coh3002t, ROT0,   "Success", "Zoku Otenamihaiken (V2.03J)", 0 )

GAME( 199?, usagi,    taitogn, coh3002t, coh3002t, coh3002t, ROT0, "Warashi/Mahjong Kobo/Taito", "Usagi (V2.02J)", GAME_NOT_WORKING ) // no inputs (mahjong panel?)
GAME( 1999, mahjngoh, taitogn, coh3002t, coh3002t, coh3002t, ROT0, "Warashi/Mahjong Kobo/Taito", "Mahjong Oh (V2.06J)", GAME_NOT_WORKING ) // needs mahjong panel

GAME( 2001, nightrai, taitogn, coh3002t, coh3002t, coh3002t, ROT0, "Takumi", "Night Raid (V2.03J)", GAME_NOT_WORKING ) // no background / enemy sprites

GAME( 2002, xiistag,  taitogn, coh3002t, coh3002t, coh3002t, ROT270, "Triangle Service", "XII Stag (V2.01J)", 0 )

