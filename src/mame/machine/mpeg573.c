#include "emu.h"
#include "mpeg573.h"

const device_type MPEG573 = &device_creator<mpeg573_device>;

DEVICE_ADDRESS_MAP_START(amap, 16, mpeg573_device)
	AM_RANGE(0x00, 0x01) AM_READ(a00_r)
	AM_RANGE(0x02, 0x03) AM_READ(a02_r)
	AM_RANGE(0x04, 0x05) AM_READ(a04_r)
	AM_RANGE(0x06, 0x07) AM_READ(a06_r)
	AM_RANGE(0x0a, 0x0b) AM_READ(a0a_r)
	AM_RANGE(0x80, 0x81) AM_READ(a80_r)
	AM_RANGE(0xa0, 0xa1) AM_WRITE(mpeg_start_adr_high_w)
	AM_RANGE(0xa2, 0xa3) AM_WRITE(mpeg_start_adr_low_w)
	AM_RANGE(0xa4, 0xa5) AM_WRITE(mpeg_end_adr_high_w)
	AM_RANGE(0xa6, 0xa7) AM_WRITE(mpeg_end_adr_low_w)
	AM_RANGE(0xa8, 0xa9) AM_WRITE(mpeg_key_1_w)
	AM_RANGE(0xac, 0xad) AM_READWRITE(mas_i2c_r, mas_i2c_w)
	AM_RANGE(0xae, 0xaf) AM_WRITE(mpeg_ctrl_w)
	AM_RANGE(0xb0, 0xb1) AM_WRITE(ram_write_adr_high_w)
	AM_RANGE(0xb2, 0xb3) AM_WRITE(ram_write_adr_low_w)
	AM_RANGE(0xb4, 0xb5) AM_READWRITE(ram_r, ram_w)
	AM_RANGE(0xb6, 0xb7) AM_WRITE(ram_read_adr_high_w)
	AM_RANGE(0xb8, 0xb9) AM_WRITE(ram_read_adr_low_w)
	AM_RANGE(0xe0, 0xe1) AM_WRITE(output_1_w)
	AM_RANGE(0xe2, 0xe3) AM_WRITE(output_0_w)
	AM_RANGE(0xe4, 0xe5) AM_WRITE(output_3_w)
	AM_RANGE(0xe6, 0xe7) AM_WRITE(output_7_w)
	AM_RANGE(0xea, 0xeb) AM_WRITE(mpeg_key_2_w)
	AM_RANGE(0xec, 0xed) AM_WRITE(mpeg_key_3_w)
	AM_RANGE(0xee, 0xef) AM_READWRITE(digital_id_r, digital_id_w)
	AM_RANGE(0xf6, 0xf7) AM_READ(fpga_status_r)
	AM_RANGE(0xf8, 0xf9) AM_WRITE(fpga_firmware_w)
	AM_RANGE(0xfa, 0xfb) AM_WRITE(output_4_w)
	AM_RANGE(0xfc, 0xfd) AM_WRITE(output_5_w)
	AM_RANGE(0xfe, 0xff) AM_WRITE(output_2_w)
ADDRESS_MAP_END

static MACHINE_CONFIG_FRAGMENT( mpeg573 )
	MCFG_MAS3507D_ADD( "mpeg" )
	MCFG_DS2401_ADD( "digital_id" )
MACHINE_CONFIG_END

mpeg573_device::mpeg573_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
  : device_t(mconfig, MPEG573, "Konami 573 digital audio board", tag, owner, clock, "mpeg573", __FILE__),
	mas3507d(*this, "mpeg"),
	digital_id(*this, "digital_id"),
    output_cb(*this)
{
}

void mpeg573_device::device_start()
{
	output_cb.resolve_safe();
	ram = auto_alloc_array( machine(), UINT16, 12 * 1024 * 1024 );
	save_pointer( NAME(ram), 12 * 1024 * 1024 );
}

void mpeg573_device::device_reset()
{
	ram_adr = 0;
	memset(output_data, 0, sizeof(output_data));
}

ROM_START( mpeg573 )
	ROM_REGION( 0x000008, "digital_id", 0 )
	ROM_LOAD( "digital-id.bin",   0x000000, 0x000008, CRC(2b977f4d) SHA1(2b108a56653f91cb3351718c45dfcf979bc35ef1) )
ROM_END

const rom_entry *mpeg573_device::device_rom_region() const
{
	return ROM_NAME(mpeg573);
}

machine_config_constructor mpeg573_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( mpeg573 );
}

void mpeg573_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
}

READ16_MEMBER(mpeg573_device::a00_r)
{
	logerror("%s: a00_r (%s)\n", tag(), machine().describe_context());
	return 0x0000;
}

READ16_MEMBER(mpeg573_device::a02_r)
{
	logerror("%s: a02_r (%s)\n", tag(), machine().describe_context());
	return 0x0001;
}

READ16_MEMBER(mpeg573_device::a04_r)
{
	logerror("%s: a04_r (%s)\n", tag(), machine().describe_context());
	return 0x0000;
}

READ16_MEMBER(mpeg573_device::a06_r)
{
	logerror("%s: a06_r (%s)\n", tag(), machine().describe_context());
	return 0x0000;
}

READ16_MEMBER(mpeg573_device::a0a_r)
{
	logerror("%s: a0a_r (%s)\n", tag(), machine().describe_context());
	return 0x0000;
}

READ16_MEMBER(mpeg573_device::a80_r)
{
	logerror("%s: a80_r (%s)\n", tag(), machine().describe_context());
	return 0x1234;
}

WRITE16_MEMBER(mpeg573_device::mpeg_start_adr_high_w)
{
	logerror("FPGA MPEG start address high %04x\n", data);
}

WRITE16_MEMBER(mpeg573_device::mpeg_start_adr_low_w)
{
	logerror("FPGA MPEG start address low %04x\n", data);
}

WRITE16_MEMBER(mpeg573_device::mpeg_end_adr_high_w)
{
	logerror("FPGA MPEG end address high %04x\n", data);
}

WRITE16_MEMBER(mpeg573_device::mpeg_end_adr_low_w)
{
	logerror("FPGA MPEG end address low %04x\n", data);
}

WRITE16_MEMBER(mpeg573_device::mpeg_key_1_w)
{
	logerror("FPGA MPEG key 1/3 %04x\n", data);
}

READ16_MEMBER(mpeg573_device::mas_i2c_r)
{
	return (mas3507d->i2c_scl_r() << 13) | (mas3507d->i2c_sda_r() << 12);
}

WRITE16_MEMBER(mpeg573_device::mas_i2c_w)
{
	mas3507d->i2c_scl_w(data & 0x2000);
	mas3507d->i2c_sda_w(data & 0x1000);
}

WRITE16_MEMBER(mpeg573_device::mpeg_ctrl_w)
{
	logerror("FPGA MPEG control %c%c%c\n",
			 data & 0x8000 ? '#' : '.',
			 data & 0x4000 ? '#' : '.',
			 data & 0x2000 ? '#' : '.');
}

WRITE16_MEMBER(mpeg573_device::ram_write_adr_high_w)
{
	// read and write address are shared
	ram_adr = (ram_adr & 0x0000ffff) | (data << 16);
}

WRITE16_MEMBER(mpeg573_device::ram_write_adr_low_w)
{
	// read and write address are shared
	ram_adr = (ram_adr & 0xffff0000) | data;
}

READ16_MEMBER(mpeg573_device::ram_r)
{
	UINT16 res = ram[ram_adr >> 1];
	ram_adr += 2;
	return res;
}

WRITE16_MEMBER(mpeg573_device::ram_w)
{
	ram[ram_adr >> 1] = data;
	ram_adr += 2;
}

WRITE16_MEMBER(mpeg573_device::ram_read_adr_high_w)
{
	// read and write address are shared
	ram_adr = (ram_adr & 0x0000ffff) | (data << 16);
}

WRITE16_MEMBER(mpeg573_device::ram_read_adr_low_w)
{
	// read and write address are shared
	ram_adr = (ram_adr & 0xffff0000) | data;
}

WRITE16_MEMBER(mpeg573_device::output_1_w)
{
	output(1, data);
}

WRITE16_MEMBER(mpeg573_device::output_0_w)
{
	output(0, data);
}

WRITE16_MEMBER(mpeg573_device::output_3_w)
{
	output(3, data);
}

WRITE16_MEMBER(mpeg573_device::output_7_w)
{
	output(7, data);
}

WRITE16_MEMBER(mpeg573_device::mpeg_key_2_w)
{
	logerror("FPGA MPEG key 2/3 %04x\n", data);
}

WRITE16_MEMBER(mpeg573_device::mpeg_key_3_w)
{
	logerror("FPGA MPEG key 3/3 %04x\n", data);
}

READ16_MEMBER(mpeg573_device::digital_id_r)
{
	return digital_id->read() << 12;
}

WRITE16_MEMBER(mpeg573_device::digital_id_w)
{
	digital_id->write( !( ( data >> 12 ) & 1 ) );
}

READ16_MEMBER(mpeg573_device::fpga_status_r)
{
	logerror("%s: fpga_status_r (%s)\n", tag(), machine().describe_context());

	// fpga/digital board status checks
	// wants & c000 = 8000 (just after program upload?)
	// write 0000 to +f4.w
	// write 8000 to +f6.w

	/* fails if !8000 */
	/* fails if  4000 */
	/* fails if !2000 */
	/* fails if !1000 */
	return 0x8000 | 0x2000 | 0x1000;
}

WRITE16_MEMBER(mpeg573_device::fpga_firmware_w)
{
	// Firmware bits in bit 15, always the same firmware
}

WRITE16_MEMBER(mpeg573_device::output_4_w)
{
	output(4, data);
}

WRITE16_MEMBER(mpeg573_device::output_5_w)
{
	output(5, data);
}

WRITE16_MEMBER(mpeg573_device::output_2_w)
{
	output(2, data);
}

void mpeg573_device::output(int offset, UINT16 data)
{
	data = (data >> 12) & 0x0f;
	static const int shift[] = { 0, 2, 3, 1 };
	for(int i = 0; i < 4; i++) {
		int oldbit = (output_data[offset] >> shift[i]) & 1;
		int newbit = (data                >> shift[i]) & 1;
		if(oldbit != newbit)
			output_cb(4*offset + i, newbit, 0xff);
	}
	output_data[offset] = data;
}
