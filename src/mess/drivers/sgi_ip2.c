/****************************************************************************

    drivers/sgi_ip2.c
    SGI IRIS 3130 skeleton driver with some meat on its bones

    by Harmony

        0x00000000 - ?              RAM (?)
        0x30000000 - 0x30017fff     ROM (3x32k)
        0x30800000 - 0x30800000     Mouse Buttons (1)
        0x31000000 - 0x31000001     Mouse Quadrature (2)
        0x32000000 - 0x3200000f     DUART0 (?)
        0x32800000 - 0x3280000f     DUART1 (?)
        0x33000000 - 0x330007ff     SRAM (2k)
        0x34000000 - 0x34000000     Clock Control (1)
        0x35000000 - 0x35000000     Clock Data (1)
        0x36000000 - 0x36000000     OS Base (1)
        0x38000000 - 0x38000001     Status Register (2)
        0x39000000 - 0x39000000     Parity control (1)
        0x3a000000 - 0x3a000000     Multibus Protection (1)
        0x3b000000 - 0x3b000003     Page Table Map Base (4)
        0x3c000000 - 0x3c000001     Text/Data Base (2)
        0x3d000000 - 0x3d000001     Text/Data Limit (2)
        0x3e000000 - 0x3e000001     Stack Base (2)
        0x3f000000 - 0x3f000001     Stack Limit (2)

    TODO:
        Finish incomplete MC68681 DUART emulation
        Hook up keyboard
        Hook up mouse
        Hook up graphics
        Hook up mass storage

    Interrupts:
        M68K:
            6 - DUART

    It is unlikely that this driver will ever be fully brought to life, as
    it will require the M68020 core to be updated to support having opcodes
    paused and resumed mid-instruction in order to service exceptions.

****************************************************************************/

#include "emu.h"
#include "sound/dac.h"
#include "cpu/m68000/m68000.h"
#include "machine/mc146818.h" /* TOD clock */
#include "machine/68681.h" /* DUART0, DUART1 */
#include "machine/terminal.h"


class sgi_ip2_state : public driver_device
{
public:
	sgi_ip2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		  m_terminal(*this, TERMINAL_TAG) ,
		m_mainram(*this, "mainram"),
		m_bss(*this, "bss"),
		m_ptmap(*this, "ptmap"){ }

	required_device<generic_terminal_device> m_terminal;
	required_shared_ptr<UINT32> m_mainram;
	required_shared_ptr<UINT32> m_bss;
	required_shared_ptr<UINT32> m_ptmap;
	UINT8 m_mbut;
	UINT16 m_mquad;
	UINT16 m_tdbase;
	UINT16 m_tdlmt;
	UINT16 m_stkbase;
	UINT16 m_stklmt;
	UINT8 m_parctl;
	UINT8 m_mbp;

	DECLARE_READ8_MEMBER(sgi_ip2_m_but_r);
	DECLARE_WRITE8_MEMBER(sgi_ip2_m_but_w);
	DECLARE_READ16_MEMBER(sgi_ip2_m_quad_r);
	DECLARE_WRITE16_MEMBER(sgi_ip2_m_quad_w);
	DECLARE_READ16_MEMBER(sgi_ip2_swtch_r);
	DECLARE_READ8_MEMBER(sgi_ip2_clock_ctl_r);
	DECLARE_WRITE8_MEMBER(sgi_ip2_clock_ctl_w);
	DECLARE_READ8_MEMBER(sgi_ip2_clock_data_r);
	DECLARE_WRITE8_MEMBER(sgi_ip2_clock_data_w);
	DECLARE_READ8_MEMBER(sgi_ip2_os_base_r);
	DECLARE_WRITE8_MEMBER(sgi_ip2_os_base_w);
	DECLARE_READ16_MEMBER(sgi_ip2_status_r);
	DECLARE_WRITE16_MEMBER(sgi_ip2_status_w);
	DECLARE_READ8_MEMBER(sgi_ip2_parctl_r);
	DECLARE_WRITE8_MEMBER(sgi_ip2_parctl_w);
	DECLARE_READ8_MEMBER(sgi_ip2_mbp_r);
	DECLARE_WRITE8_MEMBER(sgi_ip2_mbp_w);
	DECLARE_READ32_MEMBER(sgi_ip2_ptmap_r);
	DECLARE_WRITE32_MEMBER(sgi_ip2_ptmap_w);
	DECLARE_READ16_MEMBER(sgi_ip2_tdbase_r);
	DECLARE_WRITE16_MEMBER(sgi_ip2_tdbase_w);
	DECLARE_READ16_MEMBER(sgi_ip2_tdlmt_r);
	DECLARE_WRITE16_MEMBER(sgi_ip2_tdlmt_w);
	DECLARE_READ16_MEMBER(sgi_ip2_stkbase_r);
	DECLARE_WRITE16_MEMBER(sgi_ip2_stkbase_w);
	DECLARE_READ16_MEMBER(sgi_ip2_stklmt_r);
	DECLARE_WRITE16_MEMBER(sgi_ip2_stklmt_w);
	DECLARE_DRIVER_INIT(sgi_ip2);
	virtual void machine_start();
	virtual void machine_reset();
	INTERRUPT_GEN_MEMBER(sgi_ip2_vbl);
	DECLARE_WRITE8_MEMBER(sgi_kbd_put);
};


#define VERBOSE_LEVEL ( 0 )

#define ENABLE_VERBOSE_LOG (0)

#if ENABLE_VERBOSE_LOG
INLINE void ATTR_PRINTF(3,4) verboselog( running_machine &machine, int n_level, const char *s_fmt, ... )
{
	if( VERBOSE_LEVEL >= n_level )
	{
		va_list v;
		char buf[ 32768 ];
		va_start( v, s_fmt );
		vsprintf( buf, s_fmt, v );
		va_end( v );
		logerror("%08x: %s", machine.device("maincpu")->safe_pc(), buf);
	}
}
#else
#define verboselog(x,y,z,...)
#endif

/***************************************************************************
    MACHINE FUNCTIONS
***************************************************************************/

#define MBUT_RIGHT		0x01	/* Right button */
#define MBUT_MIDDLE		0x02	/* Middle button */
#define MBUT_LEFT		0x04	/* Left button */
#define BOARD_REV1		0x60	/* Board revision - #1 */
#define BOARD_REV2		0x50	/* Board revision - #2 */


READ8_MEMBER(sgi_ip2_state::sgi_ip2_m_but_r)
{
	verboselog(machine(), 0, "sgi_ip2_m_but_r: %02x\n", m_mbut | BOARD_REV1);
	return m_mbut | BOARD_REV1;
}

WRITE8_MEMBER(sgi_ip2_state::sgi_ip2_m_but_w)
{
	verboselog(machine(), 0, "sgi_ip2_m_but_w: %02x\n", data);
	m_mbut = data;
}

#define MOUSE_XFIRE		0x01	/* X Quadrature Fired, active low */
#define MOUSE_XCHANGE	0x02	/* MOUSE_XCHANGE ? x-- : x++ */
#define MOUSE_YFIRE		0x04	/* Y Quadrature Fired, active low */
#define MOUSE_YCHANGE	0x08	/* MOUSE_YCHANGE ? y-- : y++ */


READ16_MEMBER(sgi_ip2_state::sgi_ip2_m_quad_r)
{
	verboselog(machine(), 0, "sgi_ip2_m_quad_r: %04x\n", m_mquad);
	return m_mquad;
}

WRITE16_MEMBER(sgi_ip2_state::sgi_ip2_m_quad_w)
{
	verboselog(machine(), 0, "sgi_ip2_m_quad_w = %04x & %04x\n", data, mem_mask);
	COMBINE_DATA(&m_mquad);
}

READ16_MEMBER(sgi_ip2_state::sgi_ip2_swtch_r)
{
	verboselog(machine(), 0, "sgi_ip2_swtch_r: %04x\n", ioport("SWTCH")->read());
	return ioport("SWTCH")->read();
}

READ8_MEMBER(sgi_ip2_state::sgi_ip2_clock_ctl_r)
{
	mc146818_device *rtc = machine().device<mc146818_device>("rtc");
	UINT8 ret = rtc->read(space, 1);
	verboselog(machine(), 1, "sgi_ip2_clock_ctl_r: %02x\n", ret);
	return ret;
}

WRITE8_MEMBER(sgi_ip2_state::sgi_ip2_clock_ctl_w)
{
	verboselog(machine(), 1, "sgi_ip2_clock_ctl_w: %02x\n", data);
	mc146818_device *rtc = machine().device<mc146818_device>("rtc");
	rtc->write(space, 1, data);
}

READ8_MEMBER(sgi_ip2_state::sgi_ip2_clock_data_r)
{
	mc146818_device *rtc = machine().device<mc146818_device>("rtc");
	UINT8 ret = rtc->read(space, 0);

	verboselog(machine(), 1, "sgi_ip2_clock_data_r: %02x\n", ret);
	return ret;
}

WRITE8_MEMBER(sgi_ip2_state::sgi_ip2_clock_data_w)
{
	verboselog(machine(), 1, "sgi_ip2_clock_data_w: %02x\n", data);
	mc146818_device *rtc = machine().device<mc146818_device>("rtc");
	rtc->write(space, 0, data);
}


READ8_MEMBER(sgi_ip2_state::sgi_ip2_os_base_r)
{
	switch(offset)
	{
		default:
			verboselog(machine(), 0, "sgi_ip2_os_base_r: Unknown Register %08x\n", 0x36000000 + offset);
			break;
	}
	return 0;
}

WRITE8_MEMBER(sgi_ip2_state::sgi_ip2_os_base_w)
{
	switch(offset)
	{
		default:
			verboselog(machine(), 0, "sgi_ip2_os_base_w: Unknown Register %08x = %02x\n", 0x36000000 + offset, data);
			break;
	}
}

READ16_MEMBER(sgi_ip2_state::sgi_ip2_status_r)
{
	switch(offset)
	{
		default:
			verboselog(machine(), 0, "sgi_ip2_status_r: Unknown Register %08x & %04x\n", 0x38000000 + (offset << 1), mem_mask);
			break;
	}
	return 0;
}

WRITE16_MEMBER(sgi_ip2_state::sgi_ip2_status_w)
{
	switch(offset)
	{
		default:
			verboselog(machine(), 0, "sgi_ip2_status_w: Unknown Register %08x = %04x & %04x\n", 0x38000000 + (offset << 1), data, mem_mask);
			break;
	}
}

#define PAR_UR		0x01	/* Check parity on user-mode reads */
#define PAR_UW		0x02	/* Check parity on user-mode writes */
#define PAR_KR		0x04	/* Check parity on kernel-mode reads */
#define PAR_KW		0x08	/* Check parity on kernel-mode writes */
#define PAR_DIS0	0x10	/* Disable access to DUART0 and LEDs */
#define PAR_DIS1	0x20	/* Disable access to DUART1 */
#define PAR_MBR		0x40	/* Check parity on multibus reads */
#define PAR_MBW		0x80	/* Check parity on multibus writes */


READ8_MEMBER(sgi_ip2_state::sgi_ip2_parctl_r)
{
	verboselog(machine(), 0, "sgi_ip2_parctl_r: %02x\n", m_parctl);
	return m_parctl;
}

WRITE8_MEMBER(sgi_ip2_state::sgi_ip2_parctl_w)
{
	verboselog(machine(), 0, "sgi_ip2_parctl_w: %02x\n", data);
	m_parctl = data;
}

#define MBP_DCACC	0x01	/* Display controller access (I/O page 4) */
#define MBP_UCACC	0x02	/* Update controller access (I/O page 3) */
#define MBP_GFACC	0x04	/* Allow GF access (I/O page 1) */
#define MBP_DMACC	0x08	/* Allow GL2 DMA access (0x8nnnnn - x0bnnnnn) */
#define MBP_LIOACC	0x10	/* Allow lower I/O access (0x0nnnnn - 0x7nnnnn) */
#define MBP_HIOACC	0x20	/* Allow upper I/O access (0x8nnnnn - 0xfnnnnn) */
#define MBP_LMACC	0x40	/* Allow lower memory access (0x0nnnnn - 0x7nnnnn) */
#define MBP_HMACC	0x80	/* Allow upper memory access (0x8nnnnn - 0xfnnnnn) */


READ8_MEMBER(sgi_ip2_state::sgi_ip2_mbp_r)
{
	verboselog(machine(), 0, "sgi_ip2_mbp_r: %02x\n", m_mbp);
	return m_mbp;
}

WRITE8_MEMBER(sgi_ip2_state::sgi_ip2_mbp_w)
{
	verboselog(machine(), 0, "sgi_ip2_mbp_w: %02x\n", data);
	m_mbp = data;
}


READ32_MEMBER(sgi_ip2_state::sgi_ip2_ptmap_r)
{
	verboselog(machine(), 0, "sgi_ip2_ptmap_r: %08x = %08x & %08x\n", 0x3b000000 + (offset << 2), m_ptmap[offset], mem_mask);
	return m_ptmap[offset];
}

WRITE32_MEMBER(sgi_ip2_state::sgi_ip2_ptmap_w)
{
	verboselog(machine(), 0, "sgi_ip2_ptmap_w: %08x = %08x & %08x\n", 0x3b000000 + (offset << 2), data, mem_mask);
	COMBINE_DATA(&m_ptmap[offset]);
}


READ16_MEMBER(sgi_ip2_state::sgi_ip2_tdbase_r)
{
	verboselog(machine(), 0, "sgi_ip2_tdbase_r: %04x\n", m_tdbase);
	return m_tdbase;
}

WRITE16_MEMBER(sgi_ip2_state::sgi_ip2_tdbase_w)
{
	verboselog(machine(), 0, "sgi_ip2_tdbase_w: %04x & %04x\n", data, mem_mask);
	COMBINE_DATA(&m_tdbase);
}


READ16_MEMBER(sgi_ip2_state::sgi_ip2_tdlmt_r)
{
	verboselog(machine(), 0, "sgi_ip2_tdlmt_r: %04x\n", m_tdlmt);
	return m_tdlmt;
}

WRITE16_MEMBER(sgi_ip2_state::sgi_ip2_tdlmt_w)
{
	verboselog(machine(), 0, "sgi_ip2_tdlmt_w: %04x & %04x\n", data, mem_mask);
	COMBINE_DATA(&m_tdlmt);
}


READ16_MEMBER(sgi_ip2_state::sgi_ip2_stkbase_r)
{
	verboselog(machine(), 0, "sgi_ip2_stkbase_r: %04x\n", m_stkbase);
	return m_stkbase;
}

WRITE16_MEMBER(sgi_ip2_state::sgi_ip2_stkbase_w)
{
	verboselog(machine(), 0, "sgi_ip2_stkbase_w: %04x & %04x\n", data, mem_mask);
	COMBINE_DATA(&m_stkbase);
}


READ16_MEMBER(sgi_ip2_state::sgi_ip2_stklmt_r)
{
	verboselog(machine(), 0, "sgi_ip2_stklmt_r: %04x\n", m_stklmt);
	return m_stklmt;
}

WRITE16_MEMBER(sgi_ip2_state::sgi_ip2_stklmt_w)
{
	verboselog(machine(), 0, "sgi_ip2_stklmt_w: %04x & %04x\n", data, mem_mask);
	COMBINE_DATA(&m_stklmt);
}

WRITE8_MEMBER(sgi_ip2_state::sgi_kbd_put)
{
	duart68681_rx_data(machine().device("duart68681a"), 1, data);
}

static GENERIC_TERMINAL_INTERFACE( sgi_terminal_intf )
{
	DEVCB_DRIVER_MEMBER(sgi_ip2_state,sgi_kbd_put)
};


INTERRUPT_GEN_MEMBER(sgi_ip2_state::sgi_ip2_vbl)
{
}

void sgi_ip2_state::machine_start()
{
}

void sgi_ip2_state::machine_reset()
{
}

/***************************************************************************
    ADDRESS MAPS
***************************************************************************/


static ADDRESS_MAP_START(sgi_ip2_map, AS_PROGRAM, 32, sgi_ip2_state )
	AM_RANGE(0x00000000, 0x00ffffff) AM_RAM AM_SHARE("mainram")
	AM_RANGE(0x02100000, 0x0210ffff) AM_RAM AM_SHARE("bss") // ??? I don't understand the need for this...
	AM_RANGE(0x30000000, 0x30017fff) AM_ROM AM_REGION("user1", 0)
	AM_RANGE(0x30800000, 0x30800003) AM_READWRITE8(sgi_ip2_m_but_r, 		sgi_ip2_m_but_w,		0xffffffff)
	AM_RANGE(0x31000000, 0x31000003) AM_READWRITE16(sgi_ip2_m_quad_r,		sgi_ip2_m_quad_w,		0xffffffff)
	AM_RANGE(0x31800000, 0x31800003) AM_READ16(sgi_ip2_swtch_r,										0xffffffff)
	AM_RANGE(0x32000000, 0x3200000f) AM_DEVREADWRITE8_LEGACY("duart68681a",		duart68681_r,		duart68681_w, 0xffffffff)
	AM_RANGE(0x32800000, 0x3280000f) AM_DEVREADWRITE8_LEGACY("duart68681b",		duart68681_r,		duart68681_w, 0xffffffff)
	AM_RANGE(0x33000000, 0x330007ff) AM_RAM
	AM_RANGE(0x34000000, 0x34000003) AM_READWRITE8(sgi_ip2_clock_ctl_r, 	sgi_ip2_clock_ctl_w,	0xffffffff)
	AM_RANGE(0x35000000, 0x35000003) AM_READWRITE8(sgi_ip2_clock_data_r,	sgi_ip2_clock_data_w,	0xffffffff)
	AM_RANGE(0x36000000, 0x36000003) AM_READWRITE8(sgi_ip2_os_base_r,		sgi_ip2_os_base_w,		0xffffffff)
	AM_RANGE(0x38000000, 0x38000003) AM_READWRITE16(sgi_ip2_status_r,		sgi_ip2_status_w,		0xffffffff)
	AM_RANGE(0x39000000, 0x39000003) AM_READWRITE8(sgi_ip2_parctl_r,		sgi_ip2_parctl_w,		0xffffffff)
	AM_RANGE(0x3a000000, 0x3a000003) AM_READWRITE8(sgi_ip2_mbp_r,			sgi_ip2_mbp_w,			0xffffffff)
	AM_RANGE(0x3b000000, 0x3b003fff) AM_READWRITE(sgi_ip2_ptmap_r, sgi_ip2_ptmap_w) AM_SHARE("ptmap")
	AM_RANGE(0x3c000000, 0x3c000003) AM_READWRITE16(sgi_ip2_tdbase_r,		sgi_ip2_tdbase_w,		0xffffffff)
	AM_RANGE(0x3d000000, 0x3d000003) AM_READWRITE16(sgi_ip2_tdlmt_r,		sgi_ip2_tdlmt_w,		0xffffffff)
	AM_RANGE(0x3e000000, 0x3e000003) AM_READWRITE16(sgi_ip2_stkbase_r,		sgi_ip2_stkbase_w,		0xffffffff)
	AM_RANGE(0x3f000000, 0x3f000003) AM_READWRITE16(sgi_ip2_stklmt_r,		sgi_ip2_stklmt_w,		0xffffffff)
ADDRESS_MAP_END

/***************************************************************************
    MACHINE DRIVERS
***************************************************************************/

static void duarta_irq_handler(device_t *device, int state, UINT8 vector)
{
	verboselog(device->machine(), 0, "duarta_irq_handler\n");
	device->machine().device("maincpu")->execute().set_input_line_and_vector(M68K_IRQ_6, state, M68K_INT_ACK_AUTOVECTOR);
};

static UINT8 duarta_input(device_t *device)
{
	verboselog(device->machine(), 0, "duarta_input\n");
	return 0;
}

static void duarta_output(device_t *device, UINT8 data)
{
	verboselog(device->machine(), 0, "duarta_output: RTS: %d, DTR: %d\n", data & 1, (data & 4) >> 2);
}

static void duarta_tx(device_t *device, int channel, UINT8 data)
{
	device_t *devconf = device->machine().device(TERMINAL_TAG);
	verboselog(device->machine(), 0, "duarta_tx: %02x\n", data);
	dynamic_cast<generic_terminal_device *>(devconf)->write(devconf->machine().driver_data()->generic_space(), 0, data);
}

static const duart68681_config sgi_ip2_duart68681a_config =
{
	duarta_irq_handler,
	duarta_tx,
	duarta_input,
	duarta_output
};

static void duartb_irq_handler(device_t *device, int state, UINT8 vector)
{
	verboselog(device->machine(), 0, "duartb_irq_handler\n");
	device->machine().device("maincpu")->execute().set_input_line_and_vector(M68K_IRQ_6, state, M68K_INT_ACK_AUTOVECTOR);
};

static UINT8 duartb_input(device_t *device)
{
	verboselog(device->machine(), 0, "duartb_input\n");
	return 0;
}

static void duartb_output(device_t *device, UINT8 data)
{
	verboselog(device->machine(), 0, "duartb_output: RTS: %d, DTR: %d\n", data & 1, (data & 4) >> 2);
}

static void duartb_tx(device_t *device, int channel, UINT8 data)
{
	verboselog(device->machine(), 0, "duartb_tx: %02x\n", data);
}

static const duart68681_config sgi_ip2_duart68681b_config =
{
	duartb_irq_handler,
	duartb_tx,
	duartb_input,
	duartb_output
};

static MACHINE_CONFIG_START( sgi_ip2, sgi_ip2_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68020, 16000000)
	MCFG_CPU_PROGRAM_MAP(sgi_ip2_map)
	MCFG_CPU_VBLANK_INT_DRIVER(TERMINAL_TAG ":" TERMINAL_SCREEN_TAG, sgi_ip2_state,  sgi_ip2_vbl)


	/* video hardware */
	MCFG_GENERIC_TERMINAL_ADD(TERMINAL_TAG,sgi_terminal_intf)

    MCFG_DUART68681_ADD( "duart68681a", XTAL_3_6864MHz, sgi_ip2_duart68681a_config ) /* Y3 3.6864MHz Xtal ??? copy-over from dectalk */
    MCFG_DUART68681_ADD( "duart68681b", XTAL_3_6864MHz, sgi_ip2_duart68681b_config ) /* Y3 3.6864MHz Xtal ??? copy-over from dectalk */
	MCFG_MC146818_ADD( "rtc", MC146818_IGNORE_CENTURY )

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD( "dac", DAC, 0 )
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.5)
MACHINE_CONFIG_END

static INPUT_PORTS_START( sgi_ip2 )
    PORT_START("SWTCH")
	PORT_DIPNAME( 0x8000, 0x8000, "Master/Slave" )
	PORT_DIPSETTING(	0x0000, "Slave" )
	PORT_DIPSETTING(	0x8000, "Master" )
	PORT_BIT( 0x6000, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_DIPNAME( 0x1800, 0x1800, "RS232 Console Speed" )
	PORT_DIPSETTING(	0x0000, "9600 Baud" )
	PORT_DIPSETTING(	0x0800, "300 Baud" )
	PORT_DIPSETTING(	0x1000, "1200 Baud" )
	PORT_DIPSETTING(	0x1800, "19200 Baud" )
	PORT_DIPNAME( 0x0700, 0x0000, "Display Setting" )
	PORT_DIPSETTING(	0x0000, "60Hz Non-Interlaced / 60Hz Non-Interlaced" )
	PORT_DIPSETTING(	0x0100, "60Hz Non-Interlaced / 30Hz Interlaced" )
	PORT_DIPSETTING(	0x0200, "60Hz Non-Interlaced / NTSC RS 170A" )
	PORT_DIPSETTING(	0x0300, "60Hz Non-Interlaced / PAL" )
	PORT_DIPSETTING(	0x0400, "30Hz Interlaced / 60Hz Non-Interlaced" )
	PORT_DIPSETTING(	0x0500, "30Hz Interlaced / 30Hz Interlaced" )
	PORT_DIPSETTING(	0x0600, "30Hz Interlaced / NTSC RS 170A" )
	PORT_DIPSETTING(	0x0700, "30Hz Interlaced / PAL" )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_DIPNAME( 0x0040, 0x0000, "Enable Dual-Head Display" )
	PORT_DIPSETTING(	0x0000, "Use Primary Display" )
	PORT_DIPSETTING(	0x0040, "Use Secondary Display" )
	PORT_DIPNAME( 0x0020, 0x0000, "Verbose Boot" )
	PORT_DIPSETTING(	0x0000, "Be Verbose" )
	PORT_DIPSETTING(	0x0020, "Be Quiet" )
	PORT_DIPNAME( 0x0010, 0x0000, "Auto-Boot" )
	PORT_DIPSETTING(	0x0000, "Enter PROM Monitor" )
	PORT_DIPSETTING(	0x0010, "Auto-Boot" )
	PORT_DIPNAME( 0x000f, 0x0005, "Boot Media" )
	PORT_DIPSETTING(	0x0000, "Hard Disk (IP, SD, MD)" )
	PORT_DIPSETTING(	0x0001, "Cartridge Tape" )
	PORT_DIPSETTING(	0x0002, "Floppy Disk (SF, MF)" )
	PORT_DIPSETTING(	0x0003, "Ethernet using XNS" )
	PORT_DIPSETTING(	0x0005, "Enter PROM Monitor" )
	PORT_DIPSETTING(	0x0006, "Boot from PROM Board" )
	PORT_DIPSETTING(	0x0007, "TCP/UDP Netboot" )
	PORT_DIPSETTING(	0x0009, "Interphase SMD Disk Boot" )
	PORT_DIPSETTING(	0x000a, "Storager Tape Boot (1)" )
	PORT_DIPSETTING(	0x000b, "Storager Tape Boot (2)" )
	PORT_DIPSETTING(	0x000c, "Stoarger Hard Disk Boot" )
	PORT_DIPSETTING(	0x000d, "DSD Tape Boot (1)" )
	PORT_DIPSETTING(	0x000e, "DSD Tape Boot (2)" )
	PORT_DIPSETTING(	0x000f, "DSD Hard Disk Boot" )

INPUT_PORTS_END

DRIVER_INIT_MEMBER(sgi_ip2_state,sgi_ip2)
{
	UINT32 *src = (UINT32*)(*memregion("user1"));
	UINT32 *dst = m_mainram;
	memcpy(dst, src, 8);

	machine().device("maincpu")->reset();
}

/***************************************************************************

  ROM definition(s)

***************************************************************************/

ROM_START( sgi_ip2 )
	ROM_REGION32_BE(0x18000, "user1", 0)
	ROM_LOAD( "sgi-ip2-u91.nolabel.od",    0x00000, 0x8000, CRC(32e1f6b5) SHA1(2bd928c3fe2e364b9a38189158e9bad0e5271a59) )
	ROM_LOAD( "sgi-ip2-u92.nolabel.od",    0x08000, 0x8000, CRC(13dbfdb3) SHA1(3361fb62f7a8c429653700bccfc3e937f7508182) )
	ROM_LOAD( "sgi-ip2-u93.ip2.2-008.od",  0x10000, 0x8000, CRC(bf967590) SHA1(1aac48e4f5531a25c5482f64de5cd3c7a9931f11) )
ROM_END

/*    YEAR  NAME      PARENT    COMPAT    MACHINE  INPUT     INIT     COMPANY                   FULLNAME */
COMP( 1985, sgi_ip2,  0,        0,        sgi_ip2, sgi_ip2, sgi_ip2_state,  sgi_ip2, "Silicon Graphics Inc", "IRIS 3130 (IP2)", GAME_NOT_WORKING )
