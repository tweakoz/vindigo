// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Sinclair ZX8302 emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

**********************************************************************
                            _____   _____
                 ERASE   1 |*    \_/     | 40  Vdd
               EXTINTL   2 |             | 39  DB4
                MDRDWL   3 |             | 38  DB5
                NETOUT   4 |             | 37  DB0
                BAUDX4   5 |             | 36  DB1
                 DTR1L   6 |             | 35  COMDATA
                 CTS2L   7 |             | 34  MDSELDH
                 DCSML   8 |             | 33  MDSELCKN
                  ROWL   9 |             | 32  VSYNCH
                 PCENL  10 |    ZX8302   | 31  XTAL2
                   Vdd  11 |     ULA     | 30  XTAL1
                   DB2  12 |             | 29
                  TXD1  13 |             | 28  RESETOUTL
                  TXD2  14 |             | 27  DB7
                    A5  15 |             | 26  IPL1L
                 NETIN  16 |             | 25  CLKCPU
                    A1  17 |             | 24  DB3
                    A0  18 |             | 23  DB6
                  RAW2  19 |             | 22  COMCTL
                   GND  20 |_____________| 21  RAW1

**********************************************************************/

#pragma once

#ifndef __ZX8302__
#define __ZX8302__

#include "emu.h"



///*************************************************************************
//  MACROS / CONSTANTS
///*************************************************************************




///*************************************************************************
//  INTERFACE CONFIGURATION MACROS
///*************************************************************************

#define MCFG_ZX8302_ADD(_tag, _clock, _intrf) \
	MCFG_DEVICE_ADD(_tag, ZX8302, _clock) \
	MCFG_DEVICE_CONFIG(_intrf)

#define ZX8302_INTERFACE(name) \
	const zx8302_interface (name)=



///*************************************************************************
//  TYPE DEFINITIONS
///*************************************************************************

// ======================> zx8302_interface

struct zx8302_interface
{
	int rtc_clock;              // the RTC clock (pin 30) of the chip

	// serial
	devcb_write_line    out_ipl1l_cb;
	devcb_write_line    out_baudx4_cb;
	devcb_write_line    out_comdata_cb;
	devcb_write_line    out_txd1_cb;
	devcb_write_line    out_txd2_cb;
	devcb_write_line    out_netout_cb;

	// microdrive
	devcb_write_line    out_mdselck_cb;
	devcb_write_line    out_mdseld_cb;
	devcb_write_line    out_mdrdw_cb;
	devcb_write_line    out_erase_cb;
	devcb_write_line    out_raw1_cb;
	devcb_read_line     in_raw1_cb;
	devcb_write_line    out_raw2_cb;
	devcb_read_line     in_raw2_cb;
};

// ======================> zx8302_device

class zx8302_device :  public device_t,
						public device_serial_interface,
						public zx8302_interface
{
public:
	// construction/destruction
	zx8302_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	DECLARE_READ8_MEMBER( rtc_r );
	DECLARE_WRITE8_MEMBER( rtc_w );
	DECLARE_WRITE8_MEMBER( control_w );
	DECLARE_READ8_MEMBER( mdv_track_r );
	DECLARE_READ8_MEMBER( status_r );
	DECLARE_WRITE8_MEMBER( ipc_command_w );
	DECLARE_WRITE8_MEMBER( mdv_control_w );
	DECLARE_READ8_MEMBER( irq_status_r );
	DECLARE_WRITE8_MEMBER( irq_acknowledge_w );
	DECLARE_WRITE8_MEMBER( data_w );
	DECLARE_WRITE_LINE_MEMBER( vsync_w );
	DECLARE_WRITE_LINE_MEMBER( comctl_w );
	DECLARE_WRITE_LINE_MEMBER( comdata_w );
	DECLARE_WRITE_LINE_MEMBER( extint_w );

	DECLARE_WRITE_LINE_MEMBER( write_netin );
	DECLARE_WRITE_LINE_MEMBER( write_dtr1 );
	DECLARE_WRITE_LINE_MEMBER( write_cts2 );

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
	virtual void device_config_complete();

	// device_serial_interface overrides
	virtual void tra_callback();
	virtual void tra_complete();
	virtual void rcv_callback();
	virtual void rcv_complete();
	virtual void input_callback(UINT8 state);

	inline void trigger_interrupt(UINT8 line);
	inline void transmit_ipc_data();

private:
	enum
	{
		TIMER_BAUDX4 = 0,
		TIMER_RTC,
		TIMER_GAP,
		TIMER_IPC
	};

	enum
	{
		IPC_START,
		IPC_DATA,
		IPC_STOP
	};

	enum
	{
		BAUD_19200 = 0,
		BAUD_9600,
		BAUD_4800,
		BAUD_2400,
		BAUD_1200,
		BAUD_600,
		BAUD_300,
		BAUD_75,
		BAUD_MASK = 0x07
	};

	enum
	{
		MODE_SER1               = 0x00,
		MODE_SER2               = 0x08,
		MODE_MDV                = 0x10,
		MODE_NET                = 0x18,
		MODE_MASK               = 0x18,
	};

	enum
	{
		INT_GAP                 = 0x01,
		INT_INTERFACE           = 0x02,
		INT_TRANSMIT            = 0x04,
		INT_FRAME               = 0x08,
		INT_EXTERNAL            = 0x10,
	};

	enum
	{
		STATUS_NETWORK_PORT     = 0x01,
		STATUS_TX_BUFFER_FULL   = 0x02,
		STATUS_RX_BUFFER_FULL   = 0x04,
		STATUS_MICRODRIVE_GAP   = 0x08,
	};

	devcb_resolved_write_line   m_out_ipl1l_func;
	devcb_resolved_write_line   m_out_baudx4_func;
	devcb_resolved_write_line   m_out_comdata_func;
	devcb_resolved_write_line   m_out_txd1_func;
	devcb_resolved_write_line   m_out_txd2_func;
	devcb_resolved_write_line   m_out_netout_func;

	devcb_resolved_write_line   m_out_mdselck_func;
	devcb_resolved_write_line   m_out_mdseld_func;
	devcb_resolved_write_line   m_out_mdrdw_func;
	devcb_resolved_write_line   m_out_erase_func;
	devcb_resolved_write_line   m_out_raw1_func;
	devcb_resolved_read_line    m_in_raw1_func;
	devcb_resolved_write_line   m_out_raw2_func;
	devcb_resolved_read_line    m_in_raw2_func;

	int m_dtr1;
	int m_cts2;

	// registers
	UINT8 m_idr;                    // IPC data register
	UINT8 m_tcr;                    // transfer control register
	UINT8 m_tdr;                    // transfer data register
	UINT8 m_irq;                    // interrupt register
	UINT32 m_ctr;                   // counter register
	UINT8 m_status;                 // status register

	// IPC communication state
	int m_comdata;                  // communication data
	int m_comctl;                   // communication control
	int m_ipc_state;                // communication state
	int m_ipc_rx;                   // receiving data from IPC
	int m_ipc_busy;                 // IPC busy
	int m_baudx4;                   // IPC baud x4

	// microdrive state
	UINT8 m_mdv_data[2];            // track data register
	int m_track;                    // current track

	// timers
	emu_timer *m_baudx4_timer;      // baud x4 timer
	emu_timer *m_rtc_timer;         // real time clock timer
	emu_timer *m_gap_timer;         // microdrive gap timer
	emu_timer *m_ipc_timer;         // delayed IPC command timer
};


// device type definition
extern const device_type ZX8302;



#endif
