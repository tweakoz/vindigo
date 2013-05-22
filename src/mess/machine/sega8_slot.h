#ifndef __SEGA8_SLOT_H
#define __SEGA8_SLOT_H

/***************************************************************************
 TYPE DEFINITIONS
 ***************************************************************************/


/* PCB */
enum
{
	SEGA8_BASE_ROM = 0,
	SEGA8_EEPROM,
	SEGA8_TEREBIOEKAKI,
	SEGA8_4PAK,
	SEGA8_CODEMASTERS,
	SEGA8_ZEMINA,
	SEGA8_NEMESIS,
	SEGA8_JANGGUN,
	SEGA8_KOREAN,
	SEGA8_KOREAN_NOBANK
};


// ======================> device_sega8_cart_interface

class device_sega8_cart_interface : public device_slot_card_interface
{
public:
	// construction/destruction
	device_sega8_cart_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_sega8_cart_interface();

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_cart) { return 0xff; }
	virtual DECLARE_WRITE8_MEMBER(write_cart) {}
	virtual DECLARE_WRITE8_MEMBER(write_mapper) {}
	
	void rom_alloc(running_machine &machine, UINT32 size);
	void ram_alloc(running_machine &machine, UINT32 size);
	
	virtual void late_bank_setup() {}
	
	void set_has_battery(bool val) { has_battery = val; }
	bool get_has_battery() { return has_battery; }
	void set_lphaser_xoffs(int val) { m_lphaser_xoffs = val; }
	int get_lphaser_xoffs() { return m_lphaser_xoffs; }
	void set_sms_mode(int val) { m_sms_mode = val; }
	int get_sms_mode() { return m_sms_mode; }
	
//protected:
	UINT8* get_rom_base() { return m_rom; }
	UINT8* get_ram_base() { return m_ram; }
	UINT32 get_rom_size() { return m_rom_size; }
	UINT32 get_ram_size() { return m_ram_size; }

	void rom_map_setup(UINT32 size);
	void ram_map_setup(UINT8 banks);

//private:
	// internal state
	UINT8  *m_rom;
	UINT8  *m_ram;
	UINT32 m_rom_size;
	UINT32 m_ram_size;
	int m_rom_page_count;

	bool has_battery;
	int m_lphaser_xoffs;
	int m_sms_mode;
};


// ======================> sega8_cart_slot_device

class sega8_cart_slot_device : public device_t,
								public device_image_interface,
								public device_slot_interface
{
public:
	// construction/destruction
	sega8_cart_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	virtual ~sega8_cart_slot_device();

	// device-level overrides
	virtual void device_start();
	virtual void device_config_complete();

	// image-level overrides
	virtual bool call_load();
	virtual void call_unload();
	virtual bool call_softlist_load(char *swlist, char *swname, rom_entry *start_entry);

	int get_type() { return m_type; }
	int get_cart_type(UINT8 *ROM, UINT32 len);

	void internal_header_logging(UINT8 *ROM, UINT32 len, UINT32 nvram_len);
	int verify_cart(UINT8 *magic, int size);
	void set_lphaser_xoffset(UINT8 *rom, int size);

	void set_mandatory(bool val) { m_must_be_loaded = val; }
	void set_intf(const char * interface) { m_interface = interface; }
	void set_ext(const char * extensions) { m_extensions = extensions; }

	virtual iodevice_t image_type() const { return IO_CARTSLOT; }
	virtual bool is_readable()  const { return 1; }
	virtual bool is_writeable() const { return 0; }
	virtual bool is_creatable() const { return 0; }
	virtual bool must_be_loaded() const { return m_must_be_loaded; }
	virtual bool is_reset_on_load() const { return 1; }
	virtual const option_guide *create_option_guide() const { return NULL; }
	virtual const char *image_interface() const { return m_interface; }
	virtual const char *file_extensions() const { return m_extensions; }

	// slot interface overrides
	virtual const char * get_default_card_software(const machine_config &config, emu_options &options);

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_cart);
	virtual DECLARE_WRITE8_MEMBER(write_cart);
	virtual DECLARE_WRITE8_MEMBER(write_mapper);


//protected:
	int m_type;
	bool m_must_be_loaded;
	const char *m_interface;
	const char *m_extensions;
	device_sega8_cart_interface*       m_cart;
};


extern const device_type SEGA8_CART_SLOT;

/***************************************************************************
 DEVICE CONFIGURATION MACROS
 ***************************************************************************/

#define MCFG_SMS_CARTRIDGE_ADD(_tag,_slot_intf,_def_slot,_def_inp) \
	MCFG_DEVICE_ADD(_tag, SEGA8_CART_SLOT, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, _def_inp, false) \
	static_cast<sega8_cart_slot_device *>(device)->set_mandatory(FALSE); \
	static_cast<sega8_cart_slot_device *>(device)->set_intf("sms_cart"); \
	static_cast<sega8_cart_slot_device *>(device)->set_ext("bin,sms");

#define MCFG_SMS_CARD_ADD(_tag,_slot_intf,_def_slot,_def_inp) \
	MCFG_DEVICE_ADD(_tag, SEGA8_CART_SLOT, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, _def_inp, false) \
	static_cast<sega8_cart_slot_device *>(device)->set_mandatory(FALSE); \
	static_cast<sega8_cart_slot_device *>(device)->set_intf("sms_card"); \
	static_cast<sega8_cart_slot_device *>(device)->set_ext("bin,sc");

#define MCFG_GG_CARTRIDGE_ADD(_tag,_slot_intf,_def_slot,_def_inp) \
	MCFG_DEVICE_ADD(_tag, SEGA8_CART_SLOT, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, _def_inp, false) \
	static_cast<sega8_cart_slot_device *>(device)->set_mandatory(TRUE); \
	static_cast<sega8_cart_slot_device *>(device)->set_intf("gamegear_cart"); \
	static_cast<sega8_cart_slot_device *>(device)->set_ext("bin,gg");

#define MCFG_SG1000MK3_CARTRIDGE_ADD(_tag,_slot_intf,_def_slot,_def_inp) \
	MCFG_DEVICE_ADD(_tag, SEGA8_CART_SLOT, 0) \
	MCFG_DEVICE_SLOT_INTERFACE(_slot_intf, _def_slot, _def_inp, false) \
	static_cast<sega8_cart_slot_device *>(device)->set_mandatory(TRUE); \
	static_cast<sega8_cart_slot_device *>(device)->set_intf("sms_cart"); \
	static_cast<sega8_cart_slot_device *>(device)->set_ext("bin,sms,sg");


#endif