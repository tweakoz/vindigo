#pragma once

#ifndef __EMU_H__
#error Dont include this file directly; include emu.h instead.
#endif

#ifndef __DISERIAL_H__
#define __DISERIAL_H__

// Windows headers are crap, let me count the ways
#undef PARITY_NONE
#undef PARITY_ODD
#undef PARITY_EVEN
#undef PARITY_MARK
#undef PARITY_SPACE

// ======================> device_serial_interface
class device_serial_interface : public device_interface
{
public:
	/* parity selections */
	/* if all the bits are added in a byte, if the result is:
	   even -> parity is even
	   odd -> parity is odd
	*/
	enum
	{
		PARITY_NONE,     /* no parity. a parity bit will not be in the transmitted/received data */
		PARITY_ODD,      /* odd parity */
		PARITY_EVEN,     /* even parity */
		PARITY_MARK,     /* one parity */
		PARITY_SPACE     /* zero parity */
	};

	/* Communication lines.  Beware, everything is active high */
	enum
	{
		CTS = 0x0001, /* Clear to Send.       (INPUT)  Other end of connection is ready to accept data */
		RTS = 0x0002, /* Request to Send.     (OUTPUT) This end is ready to send data, and requests if the other */
		              /*                               end is ready to accept it */
		DSR = 0x0004, /* Data Set ready.      (INPUT)  Other end of connection has data */
		DTR = 0x0008, /* Data terminal Ready. (OUTPUT) TX contains new data. */
		RX  = 0x0010, /* Recieve data.        (INPUT)  */
		TX  = 0x0020  /* TX = Transmit data.  (OUTPUT) */
	};

	// construction/destruction
	device_serial_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_serial_interface();

	virtual void input_callback(UINT8 state) = 0;

	void set_data_frame(int num_data_bits, int stop_bit_count, int parity_code, bool synchronous);

	void receive_register_reset();
	void receive_register_update_bit(int bit);
	void receive_register_extract();

	void set_rcv_rate(attotime rate);
	void set_tra_rate(attotime rate);
	void set_rcv_rate(UINT32 clock, int div) { set_rcv_rate((clock && div) ? (attotime::from_hz(clock) * div) : attotime::never); }
	void set_tra_rate(UINT32 clock, int div) { set_tra_rate((clock && div) ? (attotime::from_hz(clock) * div) : attotime::never); }
	void set_rcv_rate(int baud) { set_rcv_rate(baud ? attotime::from_hz(baud) : attotime::never); }
	void set_tra_rate(int baud) { set_tra_rate(baud ? attotime::from_hz(baud) : attotime::never); }
	void set_rate(attotime rate) { set_rcv_rate(rate); set_tra_rate(rate); }
	void set_rate(UINT32 clock, int div) { set_rcv_rate(clock, div); set_tra_rate(clock, div); }
	void set_rate(int baud) { set_rcv_rate(baud); set_tra_rate(baud); }

	DECLARE_WRITE_LINE_MEMBER(tx_clock_w);
	DECLARE_WRITE_LINE_MEMBER(rx_clock_w);
	DECLARE_WRITE_LINE_MEMBER(clock_w);

	void transmit_register_reset();
	void transmit_register_add_bit(int bit);
	void transmit_register_setup(UINT8 data_byte);
	UINT8 transmit_register_get_data_bit();
	UINT8 transmit_register_send_bit();

	UINT8 serial_helper_get_parity(UINT8 data) { return m_serial_parity_table[data]; }

	UINT8 get_in_data_bit()  { return ((m_input_state & RX)>>4) & 1; }
	void set_out_data_bit(UINT8 data)  { m_connection_state &= ~TX; m_connection_state |=(data<<5); }

	void serial_connection_out();

	bool is_receive_register_full();
	bool is_transmit_register_empty();

	UINT8 get_received_char() { return m_rcv_byte_received; }

	void set_other_connection(device_serial_interface *other_connection);

	void connect(device_serial_interface *other_connection);
	DECLARE_WRITE_LINE_MEMBER(rx_w);
protected:
	UINT8 m_input_state;
	UINT8 m_connection_state;
	virtual void tra_callback() { }
	virtual void rcv_callback() { receive_register_update_bit(m_rcv_line); }
	virtual void tra_complete() { }
	virtual void rcv_complete() { }

	// interface-level overrides
	virtual void interface_pre_start();

	// Must be called from device_timer in the underlying device
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

private:
	enum { TRA_TIMER_ID = 10000, RCV_TIMER_ID };

	UINT8 m_serial_parity_table[256];

	// Data frame
	// length of word in bits
	UINT8 m_df_word_length;
	// parity state
	UINT8 m_df_parity;
	// number of stop bits
	UINT8 m_df_stop_bit_count;
	// synchronous or not
	bool m_synchronous;

	// Receive register
	/* data */
	UINT16 m_rcv_register_data;
	/* flags */
	UINT8 m_rcv_flags;
	/* bit count received */
	UINT8 m_rcv_bit_count_received;
	/* length of data to receive - includes data bits, parity bit and stop bit */
	UINT8 m_rcv_bit_count;
	/* the byte of data received */
	UINT8 m_rcv_byte_received;

	// Transmit register
	/* data */
	UINT16 m_tra_register_data;
	/* flags */
	UINT8 m_tra_flags;
	/* number of bits transmitted */
	UINT8 m_tra_bit_count_transmitted;
	/* length of data to send */
	UINT8 m_tra_bit_count;

	emu_timer *m_rcv_clock;
	emu_timer *m_tra_clock;
	attotime m_rcv_rate;
	attotime m_tra_rate;
	UINT8 m_rcv_line;

	bool m_tra_clock_state, m_rcv_clock_state;

	device_serial_interface *m_other_connection;

	void tra_edge();
	void rcv_edge();
};


class serial_source_device :  public device_t,
							  public device_serial_interface
{
public:
	// construction/destruction
	serial_source_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual void input_callback(UINT8 state);
	void send_bit(UINT8 data);
protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
};

extern const device_type SERIAL_SOURCE;

#define MCFG_SERIAL_SOURCE_ADD(_tag)    \
	MCFG_DEVICE_ADD((_tag), SERIAL_SOURCE, 0)

#endif  /* __DISERIAL_H__ */
