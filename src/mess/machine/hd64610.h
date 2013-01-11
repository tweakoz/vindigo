/**********************************************************************

    Hitachi HD64610 Real Time Clock

**********************************************************************
                            _____   _____
                   GND   1 |*    \_/     | 24  Vcc
          H-Start/Stop   2 |             | 23  OSC2
                  _IRQ   3 |             | 22  OSC1
                   1Hz   4 |   HD64610   | 21  GND
                    A3   5 |             | 20  _WE
                    A2   6 |             | 19  _OE
                    A1   7 |             | 18  _CS
                    A0   8 |             | 17  I/O8
                  I/O1   9 |             | 16  I/O7
                  I/O2  10 |             | 15  I/O6
                  I/O3  11 |             | 14  I/O5
                   GND  12 |_____________| 13  I/O4

**********************************************************************/

#pragma once

#ifndef __HD64610__
#define __HD64610__

#include "emu.h"



//**************************************************************************
//  INTERFACE CONFIGURATION MACROS
//**************************************************************************

#define MCFG_HD64610_ADD(_tag, _clock, _config) \
	MCFG_DEVICE_ADD((_tag), HD64610, _clock)    \
	MCFG_DEVICE_CONFIG(_config)


#define HD64610_INTERFACE(name) \
	const hd64610_interface (name) =



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> hd64610_interface

struct hd64610_interface
{
	devcb_write_line        m_out_irq_cb;
	devcb_write_line        m_out_1hz_cb;
};



// ======================> hd64610_device

class hd64610_device :  public device_t,
						public device_rtc_interface,
						public device_nvram_interface,
						public hd64610_interface
{
public:
	// construction/destruction
	hd64610_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	DECLARE_READ8_MEMBER( read );
	DECLARE_WRITE8_MEMBER( write );

	// hardware start/stop line
	DECLARE_WRITE_LINE_MEMBER( h_w );

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

	// device_rtc_interface overrides
	virtual void rtc_clock_updated(int year, int month, int day, int day_of_week, int hour, int minute, int second);

	// device_nvram_interface overrides
	virtual void nvram_default();
	virtual void nvram_read(emu_file &file);
	virtual void nvram_write(emu_file &file);

private:
	inline void set_irq_line();
	inline UINT8 read_counter(int counter);
	inline void write_counter(int counter, UINT8 value);
	inline void check_alarm();

	static const device_timer_id TIMER_UPDATE_COUNTER = 0;

	devcb_resolved_write_line   m_out_irq_func;
	devcb_resolved_write_line   m_out_1hz_func;

	UINT8   m_regs[0x10];       // Internal registers
	int     m_hline_state;      // H-Start/Stop line
	int     m_irq_out;          // alarm output

	// timers
	emu_timer *m_counter_timer;
};


// device type definition
extern const device_type HD64610;


#endif
