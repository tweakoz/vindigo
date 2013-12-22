// license:BSD-3-Clause

#ifndef ISBC_215G_H_
#define ISBC_215G_H_

#include "emu.h"
#include "cpu/i8089/i8089.h"
#include "bus/isbx/isbx.h"
#include "imagedev/harddriv.h"

class isbc_215g_device : public device_t
{
public:
	isbc_215g_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual machine_config_constructor device_mconfig_additions() const;
	const rom_entry *device_rom_region() const;

	DECLARE_WRITE8_MEMBER(write);
	DECLARE_READ16_MEMBER(io_r);
	DECLARE_WRITE16_MEMBER(io_w);
	DECLARE_READ16_MEMBER(mem_r);
	DECLARE_WRITE16_MEMBER(mem_w);

	static void static_set_wakeup_addr(device_t &device, UINT32 wakeup) { downcast<isbc_215g_device &>(device).m_wakeup = wakeup; }
	static void static_set_maincpu_tag(device_t &device, const char *maincpu_tag) { downcast<isbc_215g_device &>(device).m_maincpu_tag = maincpu_tag; }
	template<class _Object> static devcb2_base &static_set_irq_callback(device_t &device, _Object object) { return downcast<isbc_215g_device &>(device).m_out_irq_func.set_callback(object); }

protected:
	virtual void device_start();
	virtual void device_reset();

private:
	void find_sector();
	UINT16 read_sector();

	required_device<i8089_device> m_dmac;
	required_device<harddisk_image_device> m_hdd0;
	required_device<harddisk_image_device> m_hdd1;
	devcb2_write_line m_out_irq_func;

	int m_reset;
	UINT16 m_wakeup, m_secoffset, m_sector[512];
	const char *m_maincpu_tag;
	address_space *m_maincpu_mem;
	UINT16 m_cyl[2];
	UINT8 m_idcompare[4], m_drive, m_head;
	bool m_idfound, m_index, m_stepdir, m_wrgate, m_rdgate, m_amsrch;

	const struct hard_disk_info* m_geom[2];
};

#define MCFG_ISBC_215_ADD(_tag, _wakeup, _maincpu_tag) \
	MCFG_DEVICE_ADD(_tag, ISBC_215G, 0) \
	isbc_215g_device::static_set_wakeup_addr(*device, _wakeup); \
	isbc_215g_device::static_set_maincpu_tag(*device, _maincpu_tag);

#define MCFG_ISBC_215_IRQ(_irq_line) \
	devcb = &isbc_215g_device::static_set_irq_callback(*device, DEVCB2_##_irq_line);

extern const device_type ISBC_215G;

#endif /* ISBC_215G_H_ */
