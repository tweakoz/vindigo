#pragma once
#ifndef __DECO146_H__
#define __DECO146_H__

typedef device_delegate<UINT16 (int unused)> deco146_port_read_cb;
typedef device_delegate<void (address_space &space, UINT16 data, UINT16 mem_mask)> deco146_port_write_cb;


#define MCFG_DECO146_SET_PORTA_CALLBACK( _class, _method) \
	deco_146_base_device::set_port_a_cb(*device, deco146_port_read_cb(&_class::_method, #_class "::" #_method, NULL, (_class *)0));

#define MCFG_DECO146_SET_PORTB_CALLBACK( _class, _method) \
	deco_146_base_device::set_port_b_cb(*device, deco146_port_read_cb(&_class::_method, #_class "::" #_method, NULL, (_class *)0));

#define MCFG_DECO146_SET_PORTC_CALLBACK( _class, _method) \
	deco_146_base_device::set_port_c_cb(*device, deco146_port_read_cb(&_class::_method, #_class "::" #_method, NULL, (_class *)0));

#define MCFG_DECO146_SET_SOUNDLATCH_CALLBACK( _class, _method) \
	deco_146_base_device::set_soundlatch_cb(*device, deco146_port_write_cb(&_class::_method, #_class "::" #_method, NULL, (_class *)0));


// there are some standard ways the chip gets hooked up, so have them here ready to use
#define MCFG_DECO146_SET_INTERFACE_SCRAMBLE( a9,a8,a7,a6,a5,a4,a3,a2,a1,a0 ) \
	deco_146_base_device::set_interface_scramble(*device, a9,a8,a7,a6,a5,a4,a3,a2,a1,a0);

#define MCFG_DECO146_SET_INTERFACE_SCRAMBLE_REVERSE \
	deco_146_base_device::set_interface_scramble(*device, 0,1,2,3,4,5,6,7,8,9);

#define MCFG_DECO146_SET_INTERFACE_SCRAMBLE_INTERLEAVE \
	deco_146_base_device::set_interface_scramble(*device, 4,5,3,6,2,7,1,8,0,9 );

#define MCFG_DECO146_SET_USE_MAGIC_ADDRESS_XOR \
	deco_146_base_device::set_use_magic_read_address_xor(*device, 1 );

                                                                                                                                                                   


/* Data East 146 protection chip */

class deco_146_base_device : public device_t
{
public:
	//deco_146_base_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	deco_146_base_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);

	void write_data(address_space &space, UINT16 address, UINT16 data, UINT16 mem_mask, UINT8 &csflags);
	UINT16 read_data(UINT16 address, UINT16 mem_mask, UINT8 &csflags);

	static void set_port_a_cb(device_t &device,deco146_port_read_cb port_cb);
	static void set_port_b_cb(device_t &device,deco146_port_read_cb port_cb);
	static void set_port_c_cb(device_t &device,deco146_port_read_cb port_cb);
	static void set_soundlatch_cb(device_t &device,deco146_port_write_cb port_cb);
	static void set_interface_scramble(device_t &device,UINT8 a9, UINT8 a8, UINT8 a7, UINT8 a6, UINT8 a5, UINT8 a4, UINT8 a3,UINT8 a2,UINT8 a1,UINT8 a0);
	static void set_use_magic_read_address_xor(device_t &device, int use_xor);

	// legacy stuff
	DECLARE_READ32_MEMBER(captaven_prot_r);
	DECLARE_READ16_MEMBER(lemmings_prot_r);



	deco146_port_read_cb m_port_a_r;
	deco146_port_read_cb m_port_b_r;
	deco146_port_read_cb m_port_c_r;
	deco146_port_write_cb m_soundlatch_w;

	UINT16 port_a_default(int unused);
	UINT16 port_b_default(int unused);
	UINT16 port_c_default(int unused);
	UINT16 port_dummy_cb(int unused);
	void soundlatch_default(address_space &space, UINT16 data, UINT16 mem_mask);
	void soundlatch_dummy(address_space &space, UINT16 data, UINT16 mem_mask);

	UINT8 m_bankswitch_swap_read_address;
	UINT16 m_magic_read_address_xor;
	int m_magic_read_address_xor_enabled;
	UINT8 m_xor_port;
	UINT8 m_mask_port;
	UINT8 m_soundlatch_port;


	UINT8 m_external_addrswap[10];
	virtual UINT16 read_data_getloc(UINT16 address, int& location) = 0;





// for older handlers
#define DECO146__PORT(p) (prot_ram[p/2])





protected:
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();

	UINT16 read_protport(UINT16 address, UINT16 mem_mask);
	virtual void write_protport(address_space &space, UINT16 address, UINT16 data, UINT16 mem_mask);

	UINT16 m_rambank0[0x80];
	UINT16 m_rambank1[0x80];

	UINT16* m_current_rambank;


	UINT16 m_nand;
	UINT16 m_xor;
	UINT16 m_soundlatch;

	UINT16 m_latchaddr;
	UINT16 m_latchdata;
	int m_latchflag;


	int m_strobe;

private:
	UINT8 region_selects[6];

};

extern const device_type DECO146BASE;

class deco146_device : public deco_146_base_device
{
public:
	deco146_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	virtual UINT16 read_data_getloc(UINT16 address, int& location);
};

extern const device_type DECO146PROT;

#define MCFG_DECO146_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, DECO146PROT, 0)







#endif
