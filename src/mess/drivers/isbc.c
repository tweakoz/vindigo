// license:MAME
// copyright-holders:Robbbert
/***************************************************************************

        Intel iSBC series

        09/12/2009 Skeleton driver.

Notes:

isbc86 commands: BYTE WORD REAL EREAL ROMTEST. ROMTEST works, the others hang.

****************************************************************************/

#include "emu.h"
#include "cpu/i86/i86.h"
#include "cpu/i86/i286.h"
#include "machine/terminal.h"
#include "machine/pic8259.h"
#include "machine/pit8253.h"
#include "machine/i8255.h"
#include "machine/i8251.h"
#include "machine/z80dart.h"
#include "machine/serial.h"
#include "bus/centronics/ctronics.h"
#include "bus/isbx/isbx.h"
#include "machine/isbc_215g.h"

class isbc_state : public driver_device
{
public:
	isbc_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
	m_maincpu(*this, "maincpu"),
	m_uart8251(*this, "uart8251"),
	m_uart8274(*this, "uart8274"),
	m_pic_0(*this, "pic_0"),
	m_pic_1(*this, "pic_1"),
	m_centronics(*this, "centronics")
	{ }

	required_device<cpu_device> m_maincpu;
	optional_device<i8251_device> m_uart8251;
	optional_device<i8274_device> m_uart8274;
	required_device<pic8259_device> m_pic_0;
	optional_device<pic8259_device> m_pic_1;
	optional_device<centronics_device> m_centronics;

	DECLARE_WRITE_LINE_MEMBER(lpt_ack);
	DECLARE_WRITE_LINE_MEMBER(isbc86_tmr2_w);
	DECLARE_WRITE_LINE_MEMBER(isbc286_tmr2_w);
	DECLARE_WRITE_LINE_MEMBER(isbc_uart8274_irq);
	DECLARE_READ8_MEMBER(get_slave_ack);
	DECLARE_READ8_MEMBER(ppi_b_r);
	DECLARE_WRITE8_MEMBER(ppi_c_w);
	IRQ_CALLBACK_MEMBER( irq_callback ) { return m_pic_0->inta_r(); }
	void driver_start() { m_maincpu->set_irq_acknowledge_callback(device_irq_acknowledge_delegate(FUNC(isbc_state::irq_callback),this)); }
};

static ADDRESS_MAP_START(rpc86_mem, AS_PROGRAM, 16, isbc_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00000, 0x0ffff) AM_RAM
	AM_RANGE(0xfc000, 0xfffff) AM_ROM AM_REGION("user1",0)
ADDRESS_MAP_END

static ADDRESS_MAP_START(rpc86_io, AS_IO, 16, isbc_state)
	ADDRESS_MAP_UNMAP_HIGH
ADDRESS_MAP_END

static ADDRESS_MAP_START(isbc86_mem, AS_PROGRAM, 16, isbc_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00000, 0xfbfff) AM_RAM
	AM_RANGE(0xfc000, 0xfffff) AM_ROM AM_REGION("user1",0)
ADDRESS_MAP_END

static ADDRESS_MAP_START(isbc_io, AS_IO, 16, isbc_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00c0, 0x00c3) AM_DEVREADWRITE8("pic_0", pic8259_device, read, write, 0x00ff)
	AM_RANGE(0x00c4, 0x00c7) AM_DEVREADWRITE8("pic_0", pic8259_device, read, write, 0x00ff)
	AM_RANGE(0x00c8, 0x00cf) AM_DEVREADWRITE8("ppi", i8255_device, read, write, 0x00ff)
	AM_RANGE(0x00d0, 0x00d7) AM_DEVREADWRITE8("pit", pit8253_device, read, write, 0x00ff)
	AM_RANGE(0x00d8, 0x00d9) AM_DEVREADWRITE8("uart8251", i8251_device, data_r, data_w, 0x00ff)
	AM_RANGE(0x00da, 0x00db) AM_DEVREADWRITE8("uart8251", i8251_device, status_r, control_w, 0x00ff)
	AM_RANGE(0x00dc, 0x00dd) AM_DEVREADWRITE8("uart8251", i8251_device, data_r, data_w, 0x00ff)
	AM_RANGE(0x00de, 0x00df) AM_DEVREADWRITE8("uart8251", i8251_device, status_r, control_w, 0x00ff)
ADDRESS_MAP_END

static ADDRESS_MAP_START(isbc286_io, AS_IO, 16, isbc_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0080, 0x008f) AM_DEVREADWRITE8("sbx1", isbx_slot_device, mcs0_r, mcs0_w, 0x00ff)
	AM_RANGE(0x0080, 0x008f) AM_DEVREADWRITE8("sbx1", isbx_slot_device, mcs0_r, mcs0_w, 0xff00)
	AM_RANGE(0x0090, 0x009f) AM_DEVREADWRITE8("sbx1", isbx_slot_device, mcs1_r, mcs1_w, 0x00ff)
	AM_RANGE(0x0090, 0x009f) AM_DEVREADWRITE8("sbx1", isbx_slot_device, mcs1_r, mcs1_w, 0xff00)
	AM_RANGE(0x00a0, 0x00af) AM_DEVREADWRITE8("sbx2", isbx_slot_device, mcs0_r, mcs0_w, 0x00ff)
	AM_RANGE(0x00a0, 0x00af) AM_DEVREADWRITE8("sbx2", isbx_slot_device, mcs0_r, mcs0_w, 0xff00)
	AM_RANGE(0x00b0, 0x00bf) AM_DEVREADWRITE8("sbx2", isbx_slot_device, mcs1_r, mcs1_w, 0x00ff)
	AM_RANGE(0x00b0, 0x00bf) AM_DEVREADWRITE8("sbx2", isbx_slot_device, mcs1_r, mcs1_w, 0xff00)
	AM_RANGE(0x00c0, 0x00c3) AM_DEVREADWRITE8("pic_0", pic8259_device, read, write, 0x00ff)
	AM_RANGE(0x00c4, 0x00c7) AM_DEVREADWRITE8("pic_1", pic8259_device, read, write, 0x00ff)
	AM_RANGE(0x00c8, 0x00cf) AM_DEVREADWRITE8("ppi", i8255_device, read, write, 0x00ff)
	AM_RANGE(0x00d0, 0x00d7) AM_DEVREADWRITE8("pit", pit8254_device, read, write, 0x00ff)
	AM_RANGE(0x00d8, 0x00df) AM_DEVREADWRITE8("uart8274", i8274_device, cd_ba_r, cd_ba_w, 0x00ff)
	AM_RANGE(0x0100, 0x0101) AM_DEVWRITE8("isbc_215g", isbc_215g_device, write, 0x00ff)
ADDRESS_MAP_END

static ADDRESS_MAP_START(isbc286_mem, AS_PROGRAM, 16, isbc_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00000, 0xdffff) AM_RAM
	AM_RANGE(0xe0000, 0xfffff) AM_ROM AM_REGION("user1",0)
	AM_RANGE(0xfe0000, 0xffffff) AM_ROM AM_REGION("user1",0)
ADDRESS_MAP_END

static ADDRESS_MAP_START(isbc2861_mem, AS_PROGRAM, 16, isbc_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00000, 0xdffff) AM_RAM
	AM_RANGE(0xf0000, 0xfffff) AM_ROM AM_REGION("user1",0)
	AM_RANGE(0xff0000, 0xffffff) AM_ROM AM_REGION("user1",0)
ADDRESS_MAP_END

/* Input ports */
static INPUT_PORTS_START( isbc )
INPUT_PORTS_END

static DEVICE_INPUT_DEFAULTS_START( isbc86_terminal )
	DEVICE_INPUT_DEFAULTS( "TERM_TXBAUD", 0xff, 0x01 ) // 300
	DEVICE_INPUT_DEFAULTS( "TERM_RXBAUD", 0xff, 0x01 ) // 300
	DEVICE_INPUT_DEFAULTS( "TERM_STARTBITS", 0xff, 0x01 ) // 1
	DEVICE_INPUT_DEFAULTS( "TERM_DATABITS", 0xff, 0x03 ) // 8
	DEVICE_INPUT_DEFAULTS( "TERM_PARITY", 0xff, 0x00 ) // N
	DEVICE_INPUT_DEFAULTS( "TERM_STOPBITS", 0xff, 0x03 ) // 2
DEVICE_INPUT_DEFAULTS_END

static DEVICE_INPUT_DEFAULTS_START( rpc86_terminal )
	// No UART hooked up yet
DEVICE_INPUT_DEFAULTS_END

static DEVICE_INPUT_DEFAULTS_START( isbc286_terminal )
	DEVICE_INPUT_DEFAULTS( "TERM_TXBAUD", 0xff, 0x06 ) // 9600
	DEVICE_INPUT_DEFAULTS( "TERM_RXBAUD", 0xff, 0x06 ) // 9600
	DEVICE_INPUT_DEFAULTS( "TERM_STARTBITS", 0xff, 0x01 ) // 1
	DEVICE_INPUT_DEFAULTS( "TERM_DATABITS", 0xff, 0x02 ) // 7
	DEVICE_INPUT_DEFAULTS( "TERM_PARITY", 0xff, 0x02 ) // E
	DEVICE_INPUT_DEFAULTS( "TERM_STOPBITS", 0xff, 0x01 ) // 1
DEVICE_INPUT_DEFAULTS_END

static const struct pit8253_interface isbc86_pit_config =
{
	{
		{
			XTAL_22_1184MHz/18,
			DEVCB_NULL,
			DEVCB_DEVICE_LINE_MEMBER("pic_0", pic8259_device, ir0_w)
		}, {
			XTAL_22_1184MHz/18,
			DEVCB_NULL,
			DEVCB_NULL
		}, {
			XTAL_22_1184MHz/18,
			DEVCB_NULL,
			DEVCB_DRIVER_LINE_MEMBER(isbc_state, isbc86_tmr2_w)
		}
	}
};

WRITE_LINE_MEMBER( isbc_state::isbc86_tmr2_w )
{
	m_uart8251->rxc_w(state);
	m_uart8251->txc_w(state);
}

static const i8255_interface isbc86_ppi_interface =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};

static const i8251_interface isbc86_uart8251_interface =
{
	DEVCB_DEVICE_LINE_MEMBER("rs232", serial_port_device, tx),
	DEVCB_DEVICE_LINE_MEMBER("rs232", rs232_port_device, dtr_w),
	DEVCB_DEVICE_LINE_MEMBER("rs232", rs232_port_device, rts_w),
	DEVCB_DEVICE_LINE_MEMBER("pic_0", pic8259_device, ir6_w),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};

READ8_MEMBER( isbc_state::get_slave_ack )
{
	if (offset == 7)
		return m_pic_1->inta_r();

	return 0x00;
}

static const struct pit8253_interface isbc286_pit_config =
{
	{
		{
			XTAL_22_1184MHz/18,
			DEVCB_NULL,
			DEVCB_DEVICE_LINE_MEMBER("pic_0", pic8259_device, ir0_w)
		}, {
			XTAL_22_1184MHz/18,
			DEVCB_NULL,
			DEVCB_DEVICE_LINE_MEMBER("uart8274", z80dart_device, rxtxcb_w)
		}, {
			XTAL_22_1184MHz/18,
			DEVCB_NULL,
			DEVCB_DRIVER_LINE_MEMBER(isbc_state, isbc286_tmr2_w)
		}
	}
};

WRITE_LINE_MEMBER( isbc_state::isbc286_tmr2_w )
{
	m_uart8274->rxca_w(state);
	m_uart8274->txca_w(state);
}

static const i8255_interface isbc286_ppi_interface =
{
	DEVCB_NULL,
	DEVCB_DEVICE_MEMBER("centronics", centronics_device, write),
	DEVCB_DRIVER_MEMBER(isbc_state, ppi_b_r),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_DRIVER_MEMBER(isbc_state, ppi_c_w),
};

READ8_MEMBER( isbc_state::ppi_b_r )
{
	UINT8 data = 0;
	data |= m_centronics->ack_r() ? 0x10 : 0;
	data |= m_centronics->fault_r() ? 0x40 : 0;
	data |= m_centronics->busy_r() ? 0x80 : 0;
	return data;
}

WRITE8_MEMBER( isbc_state::ppi_c_w )
{
	m_centronics->strobe_w(data & 1);

	if(data & 0x80)
		m_pic_1->ir7_w(0);
}

static I8274_INTERFACE(isbc286_uart8274_interface)
{
	0, 0, 0, 0,

	DEVCB_DEVICE_LINE_MEMBER("rs232a", serial_port_device, tx),
	DEVCB_DEVICE_LINE_MEMBER("rs232a", rs232_port_device, dtr_w),
	DEVCB_DEVICE_LINE_MEMBER("rs232a", rs232_port_device, rts_w),
	DEVCB_NULL,
	DEVCB_NULL,

	DEVCB_DEVICE_LINE_MEMBER("rs232b", serial_port_device, tx),
	DEVCB_DEVICE_LINE_MEMBER("rs232b", rs232_port_device, dtr_w),
	DEVCB_DEVICE_LINE_MEMBER("rs232b", rs232_port_device, rts_w),
	DEVCB_NULL,
	DEVCB_NULL,

	DEVCB_DRIVER_LINE_MEMBER(isbc_state, isbc_uart8274_irq),
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL
};

WRITE_LINE_MEMBER(isbc_state::isbc_uart8274_irq)
{
	m_uart8274->m1_r(); // always set
	m_pic_0->ir6_w(state);
}

static const centronics_interface isbc286_centronics =
{
	DEVCB_DRIVER_LINE_MEMBER(isbc_state, lpt_ack),
	DEVCB_NULL,
	DEVCB_NULL
};

WRITE_LINE_MEMBER( isbc_state::lpt_ack )
{
	if(state)
		m_pic_1->ir7_w(1);
}

static MACHINE_CONFIG_START( isbc86, isbc_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", I8086, XTAL_5MHz)
	MCFG_CPU_PROGRAM_MAP(isbc86_mem)
	MCFG_CPU_IO_MAP(isbc_io)
	MCFG_PIC8259_ADD("pic_0", INPUTLINE(":maincpu", 0), VCC, NULL)
	MCFG_PIT8253_ADD("pit", isbc86_pit_config)
	MCFG_I8255A_ADD("ppi", isbc86_ppi_interface)
	MCFG_I8251_ADD("uart8251", isbc86_uart8251_interface)

	/* video hardware */
	MCFG_RS232_PORT_ADD("rs232", default_rs232_devices, "serial_terminal")
	MCFG_SERIAL_OUT_RX_HANDLER(DEVWRITELINE("uart8251", i8251_device, write_rx))
	MCFG_RS232_OUT_CTS_HANDLER(DEVWRITELINE("uart8251", i8251_device, write_cts))
	MCFG_RS232_OUT_DSR_HANDLER(DEVWRITELINE("uart8251", i8251_device, write_dsr))
	MCFG_DEVICE_CARD_DEVICE_INPUT_DEFAULTS("serial_terminal", isbc86_terminal)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( rpc86, isbc_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", I8086, XTAL_5MHz)
	MCFG_CPU_PROGRAM_MAP(rpc86_mem)
	MCFG_CPU_IO_MAP(rpc86_io)
	MCFG_PIC8259_ADD("pic_0", INPUTLINE(":maincpu", 0), VCC, NULL)

	/* video hardware */
	MCFG_RS232_PORT_ADD("rs232", default_rs232_devices, "serial_terminal")
	MCFG_DEVICE_CARD_DEVICE_INPUT_DEFAULTS("serial_terminal", rpc86_terminal)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( isbc286, isbc_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", I80286, XTAL_16MHz/2)
	MCFG_CPU_PROGRAM_MAP(isbc286_mem)
	MCFG_CPU_IO_MAP(isbc286_io)
	MCFG_PIC8259_ADD("pic_0", INPUTLINE(":maincpu", 0), VCC, READ8(isbc_state, get_slave_ack))
	MCFG_PIC8259_ADD("pic_1", DEVWRITELINE("pic_0", pic8259_device, ir7_w), GND, NULL)
	MCFG_PIT8254_ADD("pit", isbc286_pit_config)
	MCFG_I8255A_ADD("ppi", isbc286_ppi_interface)
	MCFG_CENTRONICS_PRINTER_ADD("centronics", isbc286_centronics)
	MCFG_I8274_ADD("uart8274", XTAL_16MHz/4, isbc286_uart8274_interface)

	MCFG_RS232_PORT_ADD("rs232a", default_rs232_devices, NULL)
	MCFG_SERIAL_OUT_RX_HANDLER(DEVWRITELINE("uart8274", z80dart_device, rxa_w))
	MCFG_RS232_OUT_DCD_HANDLER(DEVWRITELINE("uart8274", z80dart_device, dcda_w))
	MCFG_RS232_OUT_CTS_HANDLER(DEVWRITELINE("uart8274", z80dart_device, ctsa_w))

	MCFG_ISBX_SLOT_ADD("sbx1", 0, isbx_cards, NULL)
	MCFG_ISBX_SLOT_MINTR0_CALLBACK(DEVWRITELINE("pic_1", pic8259_device, ir3_w))
	MCFG_ISBX_SLOT_MINTR1_CALLBACK(DEVWRITELINE("pic_1", pic8259_device, ir4_w))
	MCFG_ISBX_SLOT_ADD("sbx2", 0, isbx_cards, NULL)
	MCFG_ISBX_SLOT_MINTR0_CALLBACK(DEVWRITELINE("pic_1", pic8259_device, ir5_w))
	MCFG_ISBX_SLOT_MINTR1_CALLBACK(DEVWRITELINE("pic_1", pic8259_device, ir6_w))

	MCFG_ISBC_215_ADD("isbc_215g", 0x100, "maincpu")
	MCFG_ISBC_215_IRQ(DEVWRITELINE("pic_0", pic8259_device, ir5_w))

	/* video hardware */
	MCFG_RS232_PORT_ADD("rs232b", default_rs232_devices, "serial_terminal")
	MCFG_SERIAL_OUT_RX_HANDLER(DEVWRITELINE("uart8274", z80dart_device, rxb_w))
	MCFG_RS232_OUT_DCD_HANDLER(DEVWRITELINE("uart8274", z80dart_device, dcdb_w))
	MCFG_RS232_OUT_CTS_HANDLER(DEVWRITELINE("uart8274", z80dart_device, ctsb_w))
	MCFG_DEVICE_CARD_DEVICE_INPUT_DEFAULTS("serial_terminal", isbc286_terminal)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( isbc2861, isbc286 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(isbc2861_mem)
MACHINE_CONFIG_END

/* ROM definition */
ROM_START( isbc86 )
	ROM_REGION( 0x4000, "user1", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "8612_2u.bin", 0x0001, 0x1000, CRC(84fa14cf) SHA1(783e1459ab121201fd49368d4bf769c1bab6447a))
	ROM_LOAD16_BYTE( "8612_2l.bin", 0x0000, 0x1000, CRC(922bda5f) SHA1(15743e69f3aba56425fa004d19b82ec20532fd72))
	ROM_LOAD16_BYTE( "8612_3u.bin", 0x2001, 0x1000, CRC(68d47c3e) SHA1(16c17f26b33daffa84d065ff7aefb581544176bd))
	ROM_LOAD16_BYTE( "8612_3l.bin", 0x2000, 0x1000, CRC(17f27ad2) SHA1(c3f379ac7d67dc4a0a7a611a0bc6323b8a3d4840))
ROM_END

ROM_START( isbc286 )
	ROM_REGION( 0x20000, "user1", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "u79.bin", 0x00001, 0x10000, CRC(144182ea) SHA1(4620ca205a6ac98fe2636183eaead7c4bfaf7a72))
	ROM_LOAD16_BYTE( "u36.bin", 0x00000, 0x10000, CRC(22db075f) SHA1(fd29ea77f5fc0697c8f8b66aca549aad5b9db3ea))
//  ROM_REGION( 0x4000, "isbc215", ROMREGION_ERASEFF )
//  ROM_LOAD16_BYTE( "174581.001.bin", 0x0000, 0x2000, CRC(ccdbc7ab) SHA1(5c2ebdde1b0252124177221ba9cacdb6d925a24d))
//  ROM_LOAD16_BYTE( "174581.002.bin", 0x0001, 0x2000, CRC(6190fa67) SHA1(295dd4e75f699aaf93227cc4876cee8accae383a))
ROM_END

ROM_START( isbc2861 )
	ROM_REGION( 0x10000, "user1", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "174894-001.bin", 0x0000, 0x4000, CRC(79e4f7af) SHA1(911a4595d35e6e82b1149e75bb027927cd1c1658))
	ROM_LOAD16_BYTE( "174894-002.bin", 0x0001, 0x4000, CRC(66747d21) SHA1(4094b1f10a8bc7db8d6dd48d7128e14e875776c7))
	ROM_LOAD16_BYTE( "174894-003.bin", 0x8000, 0x4000, CRC(c98c7f17) SHA1(6e9a14aedd630824dccc5eb6052867e73b1d7db6))
	ROM_LOAD16_BYTE( "174894-004.bin", 0x8001, 0x4000, CRC(61bc1dc9) SHA1(feed5a5f0bb4630c8f6fa0d5cca30654a80b4ee5))
ROM_END

ROM_START( rpc86 )
	ROM_REGION( 0x4000, "user1", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "145068-001.bin", 0x0001, 0x1000, CRC(0fa9db83) SHA1(4a44f8683c263c9ef6850cbe05aaa73f4d4d4e06))
	ROM_LOAD16_BYTE( "145069-001.bin", 0x2001, 0x1000, CRC(1692a076) SHA1(0ce3a4a867cb92340871bb8f9c3e91ce2984c77c))
	ROM_LOAD16_BYTE( "145070-001.bin", 0x0000, 0x1000, CRC(8c8303ef) SHA1(60f94daa76ab9dea6e309ac580152eb212b847a0))
	ROM_LOAD16_BYTE( "145071-001.bin", 0x2000, 0x1000, CRC(a49681d8) SHA1(e81f8b092cfa2d1737854b1fa270a4ce07d61a9f))
ROM_END
/* Driver */

/*    YEAR  NAME    PARENT  COMPAT   MACHINE    INPUT    INIT COMPANY   FULLNAME       FLAGS */
COMP( 19??, rpc86,    0,       0,    rpc86,      isbc, driver_device,    0,   "Intel",   "RPC 86",GAME_NOT_WORKING | GAME_NO_SOUND)
COMP( 1978, isbc86,   0,       0,    isbc86,     isbc, driver_device,    0,   "Intel",   "iSBC 86/12A",GAME_NOT_WORKING | GAME_NO_SOUND)
COMP( 19??, isbc286,  0,       0,    isbc286,    isbc, driver_device,    0,   "Intel",   "iSBC 286",GAME_NOT_WORKING | GAME_NO_SOUND)
COMP( 1983, isbc2861, 0,       0,    isbc2861,    isbc, driver_device,    0,   "Intel",   "iSBC 286/10",GAME_NOT_WORKING | GAME_NO_SOUND)
