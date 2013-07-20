/****************************************************************************

    Data East 104 based protection/IO chips


    Game                        Custom chip number
    ------------------------------------------------
    Caveman Ninja                   104
    Wizard Fire                     104
    Pocket Gal DX                   104
    Boogie Wings                    104
    Rohga                           104
    Diet GoGo                       104
    Tattoo Assassins                104?
	Dream Ball                      104
	Night Slashers                  104
	Double Wings                    104
	Schmeiser Robo                  104

	for more modern 60/66/75/146 knowledge see deco146.c


    This series of chips is used for I/O and copy protection.  They are all
    16 bit word based chips, with 0x400 write addresses, and 0x400 read
    addresses.  The basic idea of the protection is that read & write
    addresses don't match.  So if you write to write address 0, you might
    read that data back at read address 10, and something else will be at 0.
    In addition, the data read back may be bit shifted in various ways.
    Games can be very well protected by writing variables to the chip, and
    expecting certain values back from certain read addresses.  With care,
    it can be impossible to tell from the game code what values go where,
    and what shifting goes on.  Eg, writing 10 values to the chip, of which
    7 are dummy values, then reading back 3 particular values, and using them
    in a multiplication and add calculation to get a jump address for the
    program.  Even if you can guess one of many possible legal jump addresses
    it's hard to tell what values should be bit shifted in what way.

    It's also been found some chips contain a hardwired XOR port and hardwired
    NAND port which affects only certain read values.

    The chips also handle the interface to the sound cpu via a write address,
    and game inputs via several read addresses.  The I/O input data is also
    mirrored to several locations, some with bit shifting.

    Although several games use chip 104, each seems to be different, the
    address lines leading to the chip on each game are probably arranged
    differently, to further scramble reads/writes.  From hardware tests
    chips 60 && 66 appear to be identical.

    Update January - April 2006:
        Further work on examining the 146 chip has revealed that if you
        read an address immediately after writing it, you always get
        the written value returned.  This behaviour is confirmed
        to only exist for one read/write 'tick' - any other read will
        return that location to its usual state - ie, bit shifter
        or input port.  This has been emulated for the 146 chip in
        Nitroball and Fighters History but the behaviour probably
        also exists in earlier chips as it explains the 'one-shot'
        ports in Mutant Fighter.

        The 'double buffer' feature seen in the 104 chip is also
        confirmed to exist in the 146 chip.  Again, this may well
        be present in the earlier chip too.

    Emulation by Bryan McPhail, mish@tendril.co.uk

***************************************************************************/

#include "emu.h"
#include "machine/eeprom.h"
#include "deco104.h"

// the same way some 146 games require an address XOR of 0x44a at some point during the read chain
// there are 104 games requiring a xor of 0x2a4
#define DECO104_MAGIC_XOR 0x2a4

#define DECO_PORT(p) (prot_ram[p/2])
#define DECO_NEW_PORT(p) (prot_ram[p/2])


static UINT8 decoprot_buffer_ram_selected=0;
static UINT16 deco16_xor=0;
static UINT16 deco16_mask;
static int decoprot_last_write=0, decoprot_last_write_val=0;
static UINT16 decoprot_buffer_ram[0x800];
static UINT16 decoprot_buffer_ram2[0x800];

static UINT16 *deco16_prot_ram;
static UINT32 *deco32_prot_ram;




const device_type DECO104PROT = &device_creator<deco104_device>;



deco104_device::deco104_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, DECO104PROT, "DECO104PROT", tag, owner, clock, "deco104", __FILE__)
{
}


void deco104_device::device_config_complete()
{
}

void deco104_device::device_start()
{
	// double wing
	save_item(NAME(m_008_data));
	save_item(NAME(m_104_data));
	save_item(NAME(m_406_data));
	save_item(NAME(m_608_data));
	save_item(NAME(m_70c_data));
	save_item(NAME(m_78a_data));
	save_item(NAME(m_088_data));
	save_item(NAME(m_58c_data));
	save_item(NAME(m_408_data));
	save_item(NAME(m_40e_data));
	save_item(NAME(m_080_data));
	save_item(NAME(m_788_data));
	save_item(NAME(m_38e_data));
	save_item(NAME(m_580_data));
	save_item(NAME(m_60a_data));
	save_item(NAME(m_200_data));
	save_item(NAME(m_28c_data));
	save_item(NAME(m_18a_data));
	save_item(NAME(m_280_data));
	save_item(NAME(m_384_data));

	save_item(NAME(m_boss_move));
	save_item(NAME(m_boss_shoot_type));
	save_item(NAME(m_boss_3_data));
	save_item(NAME(m_boss_4_data));
	save_item(NAME(m_boss_5_data));
	save_item(NAME(m_boss_5sx_data));
	save_item(NAME(m_boss_6_data));
}

void deco104_device::device_reset()
{
	// double wing
	m_008_data = 0;
	m_104_data = 0;
	m_406_data = 0;
	m_608_data = 0;
	m_70c_data = 0;
	m_78a_data = 0;
	m_088_data = 0;
	m_58c_data = 0;
	m_408_data = 0;
	m_40e_data = 0;
	m_080_data = 0;
	m_788_data = 0;
	m_38e_data = 0;
	m_580_data = 0;
	m_60a_data = 0;
	m_200_data = 0;
	m_28c_data = 0;
	m_18a_data = 0;
	m_280_data = 0;
	m_384_data = 0;

	m_boss_move = 0;
	m_boss_shoot_type = 0;
	m_boss_3_data = 0;
	m_boss_4_data = 0;
	m_boss_5_data = 0;
	m_boss_5sx_data = 0;
	m_boss_6_data = 0;
}


/***************************************************************************/

void decoprot104_reset(running_machine &machine)
{
	deco16_xor=0;
//	deco16_mask=0xffff;
	deco16_mask=0x0; // wizard fire doesn't initialize it, but accesses addresses rohga needs the mask applied on

	decoprot_last_write=decoprot_last_write_val=0;
	decoprot_buffer_ram_selected=0;

	deco16_prot_ram = reinterpret_cast<UINT16 *>(machine.root_device().memshare("prot16ram")->ptr());
	deco32_prot_ram = reinterpret_cast<UINT32 *>(machine.root_device().memshare("prot32ram")->ptr());

	machine.save().save_item(NAME(deco16_xor));
	machine.save().save_item(NAME(deco16_mask));
	machine.save().save_item(NAME(decoprot_last_write));
	machine.save().save_item(NAME(decoprot_last_write_val));
	machine.save().save_item(NAME(decoprot_buffer_ram_selected));
	machine.save().save_item(NAME(decoprot_buffer_ram));
	machine.save().save_item(NAME(decoprot_buffer_ram2));
}

UINT16 deco104_read_core(address_space &space, int offset, UINT16 mem_mask)
{
	const UINT16* prot_ram=decoprot_buffer_ram_selected ? decoprot_buffer_ram : deco16_prot_ram;


	switch (offset) {
		case 0x088/2: /* Player 1 & 2 input ports */		return space.machine().root_device().ioport("IN0")->read(); // also caveman ninja + wizard fire
		case 0x36c/2:									return space.machine().root_device().ioport("IN1")->read(); // also caveman ninja + wizard fire
		case 0x44c/2:									return ((space.machine().root_device().ioport("IN1")->read() & 0x7)<<13)|((space.machine().root_device().ioport("IN1")->read() & 0x8)<<9);
		case 0x292/2: /* Dips */						return space.machine().root_device().ioport("DSW")->read(); // also wizard fire
		case 0x044/2:									return ((((DECO_PORT(0x2c)&0x000f)<<12)) ^ deco16_xor) & (~deco16_mask);
		case 0x282/2:									return ((DECO_PORT(0x26)&0x000f)<<12) & (~deco16_mask);
		case 0x0d4/2:									return ((DECO_PORT(0x6e)&0x0ff0)<<4) | ((DECO_PORT(0x6e)&0x000e)<<3) | ((DECO_PORT(0x6e)&0x0001)<<7);
		case 0x5a2/2:									return (((DECO_PORT(0x24)&0xff00)>>4) | ((DECO_PORT(0x24)&0x000f)<<0) | ((DECO_PORT(0x24)&0x00f0)<<8)) & (~deco16_mask);
		case 0x570/2:									return (((DECO_PORT(0x24)&0xf0f0)>>0) | ((DECO_PORT(0x24)&0x000f)<<8)) ^ deco16_xor;
		case 0x32e/2:									return (((DECO_PORT(0x46)&0xf000)>>0) | ((DECO_PORT(0x46)&0x00ff)<<4)) & (~deco16_mask);
		case 0x4dc/2:									return ((DECO_PORT(0x62)&0x00ff)<<8);
		case 0x1be/2:									return ((((DECO_PORT(0xc2)&0x0ff0)<<4) | ((DECO_PORT(0xc2)&0x0003)<<6) | ((DECO_PORT(0xc2)&0x000c)<<2)) ^ deco16_xor) & (~deco16_mask);
		case 0x420/2:									return ((DECO_PORT(0x2e)&0xf000)>>4) | ((DECO_PORT(0x2e)&0x0f00)<<4) | ((DECO_PORT(0x2e)&0x00f0)>>4) | ((DECO_PORT(0x2e)&0x000f)<<4);
		case 0x390/2:									return DECO_PORT(0x2c);
		case 0x756/2:									return ((DECO_PORT(0x60)&0xfff0)>>4) | ((DECO_PORT(0x60)&0x0007)<<13) | ((DECO_PORT(0x60)&0x0008)<<9);
		case 0x424/2:									return ((DECO_PORT(0x60)&0xf000)>>4) | ((DECO_PORT(0x60)&0x0f00)<<4) | ((DECO_PORT(0x60)&0x00f0)>>0) | ((DECO_PORT(0x60)&0x000f)<<0);
		case 0x156/2:									return (((DECO_PORT(0xde)&0xff00)<<0) | ((DECO_PORT(0xde)&0x000f)<<4) | ((DECO_PORT(0xde)&0x00f0)>>4)) & (~deco16_mask);
		case 0x0a8/2:									return (((DECO_PORT(0xde)&0xff00)>>4) | ((DECO_PORT(0xde)&0x000f)<<0) | ((DECO_PORT(0xde)&0x00f0)<<8)) & (~deco16_mask);
		case 0x64a/2:									return (((DECO_PORT(0xde)&0xfff0)>>4) | ((DECO_PORT(0xde)&0x000c)<<10) | ((DECO_PORT(0xde)&0x0003)<<14)) & (~deco16_mask);
		case 0x16e/2:									return DECO_PORT(0x6a);
		case 0x39c/2:									return (DECO_PORT(0x6a)&0x00ff) | ((DECO_PORT(0x6a)&0xf000)>>4) | ((DECO_PORT(0x6a)&0x0f00)<<4);
		case 0x212/2:									return (((DECO_PORT(0x6e)&0xff00)>>4) | ((DECO_PORT(0x6e)&0x00f0)<<8) | ((DECO_PORT(0x6e)&0x000f)<<0)) ^ deco16_xor;
		case 0x70a/2:									return (((DECO_PORT(0xde)&0x00f0)<<8) | ((DECO_PORT(0xde)&0x0007)<<9) | ((DECO_PORT(0xde)&0x0008)<<5)) ^ deco16_xor;
		case 0x7a0/2:									return (DECO_PORT(0x6e)&0x00ff) | ((DECO_PORT(0x6e)&0xf000)>>4) | ((DECO_PORT(0x6e)&0x0f00)<<4);
		case 0x162/2:									return DECO_PORT(0x6e);
		case 0x384/2:									return ((DECO_PORT(0xdc)&0xf000)>>12) | ((DECO_PORT(0xdc)&0x0ff0)<<4) | ((DECO_PORT(0xdc)&0x000c)<<2) | ((DECO_PORT(0xdc)&0x0003)<<6);
		case 0x302/2:									return DECO_PORT(0x24);
		case 0x334/2:									return DECO_PORT(0x30);
		case 0x34c/2:									return DECO_PORT(0x3c);
		case 0x514/2:									return (((DECO_PORT(0x32)&0x0ff0)<<4) | ((DECO_PORT(0x32)&0x000c)<<2) | ((DECO_PORT(0x32)&0x0003)<<6)) & (~deco16_mask);
		case 0x34e/2:									return ((DECO_PORT(0xde)&0x0ff0)<<4) | ((DECO_PORT(0xde)&0xf000)>>8) | ((DECO_PORT(0xde)&0x000f)<<0);
		case 0x722/2:									return (((DECO_PORT(0xdc)&0x0fff)<<4) ^ deco16_xor) & (~deco16_mask);
		case 0x574/2:									return ((((DECO_PORT(0xdc)&0xfff0)>>0) | ((DECO_PORT(0xdc)&0x0003)<<2) | ((DECO_PORT(0xdc)&0x000c)>>2)) ^ deco16_xor) & (~deco16_mask);
		case 0x5ae/2:									return DECO_PORT(0xdc); // also caveman ninja
		case 0x410/2:									return DECO_PORT(0xde); // also caveman ninja
		case 0x340/2:									return ((DECO_PORT(0x90)&0xfff0) | ((DECO_PORT(0x90)&0x7)<<1) | ((DECO_PORT(0x90)&0x8)>>3)) ^ deco16_xor;
		case 0x4a4/2:									return (((DECO_PORT(0xce)&0x0ff0) | ((DECO_PORT(0xce)&0xf000)>>12) | ((DECO_PORT(0xce)&0x000f)<<12)) ^ deco16_xor) & (~deco16_mask);
		case 0x256/2:									return ((((DECO_PORT(0xce)&0xf000)>>12) | ((DECO_PORT(0xce)&0x0fff)<<4))) & (~deco16_mask);
		case 0x79a/2:									return (((DECO_PORT(0xc8)&0xfff0)>>4) | ((DECO_PORT(0xc8)&0x0008)<<9) | ((DECO_PORT(0xc8)&0x0007)<<13)) ^ deco16_xor;
		case 0x65e/2:									return DECO_PORT(0x9c); // also caveman ninja
		case 0x79c/2:									return ((DECO_PORT(0xc6)&0xf000) | ((DECO_PORT(0xc6)&0x00ff)<<4) | ((DECO_PORT(0xc6)&0x0f00)>>8)) & (~deco16_mask);
		case 0x15e/2:									return (((DECO_PORT(0x98)&0x0ff0)<<4) | ((DECO_PORT(0x98)&0xf000)>>12) | ((DECO_PORT(0x98)&0x0003)<<6) | ((DECO_PORT(0x98)&0x000c)<<2)) ^ deco16_xor;
		case 0x6e4/2:									return DECO_PORT(0x98); // also caveman ninja
		case 0x01e/2:									return ((((DECO_PORT(0xc4)&0xf000)>>4) | ((DECO_PORT(0xc4)&0x0f00)<<4) | ((DECO_PORT(0xc4)&0x00ff)<<0)) ^ deco16_xor) & (~deco16_mask);
		case 0x23a/2:									return ((((DECO_PORT(0x86)&0xfff0)>>0) | ((DECO_PORT(0x86)&0x0003)<<2) | ((DECO_PORT(0x86)&0x000c)>>2)) ^ deco16_xor);
		case 0x06e/2:									return ((((DECO_PORT(0x96)&0xf000)>>8) | ((DECO_PORT(0x96)&0x0f0f)<<0) | ((DECO_PORT(0x96)&0x00f0)<<8)) ^ deco16_xor);
		case 0x3a2/2:									return ((((DECO_PORT(0x94)&0xf000)>>8) | ((DECO_PORT(0x94)&0x0f00)>>8) | ((DECO_PORT(0x94)&0x00f0)<<8) | ((DECO_PORT(0x94)&0x000e)<<7) | ((DECO_PORT(0x94)&0x0001)<<11)) ^ deco16_xor);// & (~deco16_mask);
		case 0x4a6/2:									return ((DECO_PORT(0x8c)&0xff00)>>0) | ((DECO_PORT(0x8c)&0x00f0)>>4) | ((DECO_PORT(0x8c)&0x000f)<<4);
		case 0x7b0/2:									return DECO_PORT(0x80); // also caveman ninja
		case 0x5aa/2:									return ((((DECO_PORT(0x98)&0x0f00)>>8) | ((DECO_PORT(0x98)&0xf000)>>8) | ((DECO_PORT(0x98)&0x00f0)<<8) | ((DECO_PORT(0x98)&0x000e)<<7) | ((DECO_PORT(0x98)&0x0001)<<11)) ^ deco16_xor) & (~deco16_mask);
		case 0x662/2:									return DECO_PORT(0x8c); // also caveman ninja
		case 0x624/2:									return DECO_PORT(0x9a); // also caveman ninja
		case 0x02c/2:									return (((DECO_PORT(0x82)&0x0f0f)>>0) | ((DECO_PORT(0x82)&0xf000)>>8) | ((DECO_PORT(0x82)&0x00f0)<<8)) & (~deco16_mask);
		case 0x1b4/2:									return ((DECO_PORT(0xcc)&0x00f0)<<4) | ((DECO_PORT(0xcc)&0x000f)<<12);
		case 0x7ce/2:									return ((DECO_PORT(0x80)&0x000e)<<11) | ((DECO_PORT(0x80)&0x0001)<<15);
		case 0x41a/2:									return ((((DECO_PORT(0x84)&0x00f0)<<8) | ((DECO_PORT(0x84)&0xf000)>>8) | ((DECO_PORT(0x84)&0x0f00)>>8) | ((DECO_PORT(0x84)&0x0003)<<10) | ((DECO_PORT(0x84)&0x000c)<<6)) ^ deco16_xor);
		case 0x168/2:									return ((((DECO_PORT(0x84)&0x0ff0)<<4) | ((DECO_PORT(0x84)&0x000e)<<3) | ((DECO_PORT(0x84)&0x0001)<<5))) & (~deco16_mask);
		case 0x314/2:									return ((((DECO_PORT(0x84)&0x0ff0)<<4) | ((DECO_PORT(0x84)&0x000e)<<3) | ((DECO_PORT(0x84)&0x0001)<<5)));
		case 0x5e2/2:									return ((((DECO_PORT(0x84)&0x00f0)<<8) | ((DECO_PORT(0x84)&0x000e)<<7) | ((DECO_PORT(0x84)&0x0001)<<9)));
		case 0x72a/2:									return ((((DECO_PORT(0x86)&0xfff0)>>4) | ((DECO_PORT(0x86)&0x0003)<<14) | ((DECO_PORT(0x86)&0x000c)<<10)) ^ deco16_xor) & (~deco16_mask);
		case 0x178/2:									return (((DECO_PORT(0x88)&0x00ff)<<8) | ((DECO_PORT(0x88)&0xff00)>>8)) & (~deco16_mask); // also wizard fire
		case 0x40e/2:									return ((((DECO_PORT(0x8a)&0xf000)>>0) | ((DECO_PORT(0x8a)&0x00ff)<<4)) ^ deco16_xor) & (~deco16_mask);
		case 0x248/2:									return ((((DECO_PORT(0x8c)&0xff00)>>8) | ((DECO_PORT(0x8c)&0x00f0)<<4) | ((DECO_PORT(0x8c)&0x000f)<<12)) ^ deco16_xor) & (~deco16_mask);
		case 0x27e/2:									return ((((DECO_PORT(0x94)&0x00f0)<<8)) ^ deco16_xor) & (~deco16_mask);
		case 0x22c/2:									return ((DECO_PORT(0xc4)&0x00f0)<<8);
		case 0x77e/2:									return ((DECO_PORT(0x62)&0xf000)>>12) | ((DECO_PORT(0x62)&0x0ff0)<<0) | ((DECO_PORT(0x62)&0x000f)<<12);
		case 0x00c/2:										return ((DECO_PORT(0xd6)&0xf000)>>12) | ((DECO_PORT(0xd6)&0x0fff)<<4);
		case 0x090/2:									return DECO_PORT(0x44);
		case 0x246/2:									return ((((DECO_PORT(0x48)&0xff00)>>8) | ((DECO_PORT(0x48)&0x00f0)<<8) | ((DECO_PORT(0x48)&0x0f00)>>8) | ((DECO_PORT(0x48)&0x0003)<<10) | ((DECO_PORT(0x48)&0x000c)<<6)) ^ deco16_xor);
		case 0x546/2:									return (((DECO_PORT(0x62)&0xf0f0)>>0) | ((DECO_PORT(0x62)&0x000f)<<8)) & (~deco16_mask);
		case 0x2e2/2:									return ((DECO_PORT(0xc6)&0x000e)<<11) | ((DECO_PORT(0xc6)&0x0001)<<15);
		case 0x3c0/2:									return DECO_PORT(0x22);
		case 0x4b8/2:									return (((DECO_PORT(0x46)&0xf000)>>12) | ((DECO_PORT(0x46)&0x0f00)>>4) | ((DECO_PORT(0x46)&0x00ff)<<8)) ^ deco16_xor;
		case 0x65c/2:									return ((((DECO_PORT(0x44)&0xf000)>>12) | ((DECO_PORT(0x44)&0x0fff)<<4)) ^ deco16_xor) & (~deco16_mask);
		case 0x32a/2:									return ((((DECO_PORT(0xc0)&0x0ff0)<<4) | ((DECO_PORT(0xc0)&0x000e)<<3) | ((DECO_PORT(0xc0)&0x0001)<<7))) & (~deco16_mask);// ^ deco16_xor;
		case 0x008/2:										return ((((DECO_PORT(0x94)&0xfff0)<<0) | ((DECO_PORT(0x94)&0x000e)>>1) | ((DECO_PORT(0x94)&0x0001)<<3))) & (~deco16_mask);// ^ deco16_xor;
		case 0x456/2:									return (((DECO_PORT(0x26)&0xfff0)<<0) | ((DECO_PORT(0x26)&0x0007)<<1) | ((DECO_PORT(0x26)&0x0008)>>3));// ^ deco16_xor;
		case 0x190/2:									return ((((DECO_PORT(0x44)&0xf000)<<0) | ((DECO_PORT(0x44)&0x00ff)<<4))) & (~deco16_mask);// ^ deco16_xor;
		case 0x3f2/2:									return ((((DECO_PORT(0x48)&0x000f)<<12) | ((DECO_PORT(0x48)&0x00f0)<<4))) & (~deco16_mask);// ^ deco16_xor;
		case 0x2be/2:									return ((DECO_PORT(0x40)&0x00ff)<<8);
		case 0x19e/2:									return ((((DECO_PORT(0x3c)&0xf000)>>12) | ((DECO_PORT(0x3c)&0x0f00)<<4) | ((DECO_PORT(0x3c)&0x00f0)>>0) | ((DECO_PORT(0x3c)&0x000f)<<8)) ^ deco16_xor) & (~deco16_mask);
		case 0x2a2/2:									return ((((DECO_PORT(0x44)&0xff00)>>8) | ((DECO_PORT(0x44)&0x00f0)<<8) | ((DECO_PORT(0x44)&0x000e)<<7) | ((DECO_PORT(0x44)&0x0001)<<11)) ^ deco16_xor) & (~deco16_mask);
		case 0x748/2:									return (((DECO_PORT(0x44)&0xfff0)<<0) | ((DECO_PORT(0x44)&0x000e)>>1) | ((DECO_PORT(0x44)&0x0001)<<3));// & (~deco16_mask);
		case 0x686/2:									return (((DECO_PORT(0x46)&0xf000)>>4) | ((DECO_PORT(0x46)&0x0f00)>>8) | ((DECO_PORT(0x46)&0x00f0)<<8) | ((DECO_PORT(0x46)&0x000f)<<4));// & (~deco16_mask);
		case 0x4c4/2:									return ((DECO_PORT(0x3c)&0x000f)<<12) & (~deco16_mask);
		case 0x538/2:									return ((DECO_PORT(0x3c)&0x000f)<<12);
		case 0x63a/2:									return ((DECO_PORT(0x3c)&0x000f)<<12);
		case 0x348/2:									return ((((DECO_PORT(0x44)&0xf000)>>12) | ((DECO_PORT(0x44)&0x0ff0)<<4) | ((DECO_PORT(0x44)&0x000e)<<3) | ((DECO_PORT(0x44)&0x0001)<<7))) ^ deco16_xor;// & (~deco16_mask);
		case 0x200/2:									return (((DECO_PORT(0xa0)&0xfff0)>>4) | ((DECO_PORT(0xa0)&0x0007)<<13) | ((DECO_PORT(0xa0)&0x0008)<<9));// & (~deco16_mask);
		case 0x254/2:									return ((((DECO_PORT(0x7e)&0x0ff0)<<4) | ((DECO_PORT(0x7e)&0x000c)<<2) | ((DECO_PORT(0x7e)&0x0003)<<6))) ^ deco16_xor;// & (~deco16_mask);
		case 0x182/2:									return ((DECO_PORT(0x46)&0xf000)<<0) | ((DECO_PORT(0x46)&0x0f00)>>8) | ((DECO_PORT(0x46)&0x00f0)>>0) | ((DECO_PORT(0x46)&0x000f)<<8);
		case 0x058/2:									return DECO_PORT(0x46);
		case 0x48e/2:									return ((((DECO_PORT(0x46)&0xf000)>>12) | ((DECO_PORT(0x46)&0x0f00)>>4) | ((DECO_PORT(0x46)&0x00f0)<<4) | ((DECO_PORT(0x46)&0x000f)<<12)));// /*^ deco16_xor*/) & (~deco16_mask);
		case 0x4ba/2:									return (((DECO_PORT(0x24)&0xf000)>>12) | ((DECO_PORT(0x24)&0x0ff0)<<4) | ((DECO_PORT(0x24)&0x000c)<<2) | ((DECO_PORT(0x24)&0x0003)<<6)) & (~deco16_mask);
		case 0x092/2:									return (((DECO_PORT(0x3c)&0xfff0)>>0) | ((DECO_PORT(0x3c)&0x0007)<<1) | ((DECO_PORT(0x3c)&0x0008)>>3));
		case 0x1f0/2:									return ((((DECO_PORT(0xa2)&0xf000)>>12) | ((DECO_PORT(0xa2)&0x0f00)>>4) | ((DECO_PORT(0xa2)&0x00ff)<<8)) ^ deco16_xor) & (~deco16_mask);
		case 0x24e/2:									return ((((DECO_PORT(0x46)&0xf000)>>8) | ((DECO_PORT(0x46)&0x0f00)>>0) | ((DECO_PORT(0x46)&0x00f0)>>4) | ((DECO_PORT(0x46)&0x000f)<<12)) ^ deco16_xor);// & (~deco16_mask);
		case 0x594/2:									return ((((DECO_PORT(0x40)&0x00f0)<<8) | ((DECO_PORT(0x40)&0x000c)<<6) | ((DECO_PORT(0x40)&0x0003)<<10)) ^ deco16_xor);// & (~deco16_mask);
		case 0x7e2/2:									return ((((DECO_PORT(0x96)&0xf000)<<0) | ((DECO_PORT(0x96)&0x00f0)<<4) | ((DECO_PORT(0x96)&0x000f)<<4))) ^ deco16_xor;// | ((DECO_PORT(0x96)&0x0001)<<7));// ^ deco16_xor);// & (~deco16_mask);
		case 0x18c/2:									return (((DECO_PORT(0x22)&0xfff0)>>4) | ((DECO_PORT(0x22)&0x000e)<<11) | ((DECO_PORT(0x22)&0x0001)<<15));// ^ deco16_xor);// & (~deco16_mask);
		case 0x1fa/2:									return ((((DECO_PORT(0x26)&0xf000)>>8) | ((DECO_PORT(0x26)&0x0f00)<<0) | ((DECO_PORT(0x26)&0x00f0)>>4) | ((DECO_PORT(0x26)&0x000f)<<12))) ^ deco16_xor;// & (~deco16_mask);
		case 0x70e/2:									return ((((DECO_PORT(0x26)&0x0ff0)<<4) | ((DECO_PORT(0x26)&0x000c)<<2) | ((DECO_PORT(0x26)&0x0003)<<6))) ^ deco16_xor;// & (~deco16_mask);
		case 0x33a/2:									return DECO_PORT(0x60) & (~deco16_mask);
		case 0x1e2/2:									return ((DECO_PORT(0xd0)&0xf000)>>12) | ((DECO_PORT(0xd0)&0x0f00)>>4) | ((DECO_PORT(0xd0)&0x00ff)<<8);
		case 0x3f4/2:									return DECO_PORT(0x6e)<<4;
		case 0x2ae/2:									return ((DECO_PORT(0x9c)&0xf000)<<0) | ((DECO_PORT(0x9c)&0x0ff0)>>4) | ((DECO_PORT(0x9c)&0x000f)<<8);// & (~deco16_mask);
		case 0x096/2:									return ((((DECO_PORT(0x22)&0xff00)>>8) | ((DECO_PORT(0x22)&0x00f0)<<8) | ((DECO_PORT(0x22)&0x000e)<<7) | ((DECO_PORT(0x22)&0x0001)<<11)) ^ deco16_xor) & (~deco16_mask);
		case 0x33e/2:									return (((DECO_PORT(0x0)&0xf000)>>12) | ((DECO_PORT(0x0)&0x0f00)>>4) | ((DECO_PORT(0x0)&0x00f0)<<4) | ((DECO_PORT(0x0)&0x000f)<<12)) & (~deco16_mask); // also wizard fire
		case 0x6c4/2: /* Reads from here flip buffers */decoprot_buffer_ram_selected^=1;/* Flip occurs AFTER this data has been calculated*/return ((DECO_PORT(0x66)&0xf0f0) | ((DECO_PORT(0x66)&0x000f)<<8)) & (~deco16_mask);
		case 0x700/2: /* Reads from here flip buffers */decoprot_buffer_ram_selected^=1;return (((DECO_PORT(0x66)&0xf000)>>4) | ((DECO_PORT(0x66)&0x00f0)<<8)) ^ deco16_xor;
		case 0x444/2:									decoprot_buffer_ram_selected^=1;return ((DECO_PORT(0x66)&0x00f0)<<8) | ((DECO_PORT(0x66)&0x0007)<<9)  | ((DECO_PORT(0x66)&0x0008)<<5);
		case 0x2d0/2:									decoprot_buffer_ram_selected^=1;return (((DECO_PORT(0x66)&0xf000)>>4) | ((DECO_PORT(0x66)&0x00f0)<<8)) ^ deco16_xor;
		case 0x2b8/2:									decoprot_buffer_ram_selected^=1;return ((DECO_PORT(0x66)&0x00f0)<<8) ^ deco16_xor;
		case 0x294/2:									decoprot_buffer_ram_selected^=1;return ((DECO_PORT(0x66)&0x000f)<<12);
		case 0x1e8/2:									decoprot_buffer_ram_selected^=1;return 0; // todo
		case 0x49c/2:									return (((DECO_PORT(0x6c)&0x00f0)<<8) ^ deco16_xor) & (~deco16_mask);
		case 0x44e/2:									return (((DECO_PORT(0x44)&0x00f0)<<4) | ((DECO_PORT(0x44)&0x000f)<<12)) ^ deco16_xor;
		case 0x3ca/2:									return (((DECO_PORT(0x1e)&0xfff0)>>4) | ((DECO_PORT(0x1e)&0x0003)<<14) | ((DECO_PORT(0x1e)&0x000c)<<10)) ^ deco16_xor;
		case 0x2ac/2:									return DECO_PORT(0x1e); // also caveman ninja
		case 0x03c/2:									return (((DECO_PORT(0x1e)&0x0003)<<14) | ((DECO_PORT(0x1e)&0x000c)<<10)) & (~deco16_mask);
		case 0x174/2:									return (((DECO_PORT(0x1e)&0xff00)>>8) | ((DECO_PORT(0x1e)&0x00f0)<<8) | ((DECO_PORT(0x1e)&0x0007)<<9) | ((DECO_PORT(0x1e)&0x0008)<<5)) & (~deco16_mask);
		case 0x34a/2:									return (((DECO_PORT(0x4)&0xff00)>>0) | ((DECO_PORT(0x4)&0x00f0)>>4) | ((DECO_PORT(0x4)&0x000f)<<4)) & (~deco16_mask);
		case 0x324/2:									return (((DECO_PORT(0x6)&0xf000)>>12) | ((DECO_PORT(0x6)&0x0ff0)<<4) | ((DECO_PORT(0x6)&0x0007)<<5) | ((DECO_PORT(0x6)&0x0008)<<1));
		case 0x344/2:									return (((DECO_PORT(0x8)&0xf000)>>8) | ((DECO_PORT(0x8)&0x0f00)>>8) | ((DECO_PORT(0x8)&0x00f0)<<4) | ((DECO_PORT(0x8)&0x000f)<<12));
		case 0x072/2:									return ((((DECO_PORT(0xa)&0xf000)>>8) | ((DECO_PORT(0xa)&0x0ff0)<<4) | ((DECO_PORT(0xa)&0x000f)>>0))) & (~deco16_mask);
		case 0x36e/2:									return ((((DECO_PORT(0xc)&0xf000)>>0) | ((DECO_PORT(0xc)&0x0ff0)>>4) | ((DECO_PORT(0xc)&0x000f)<<8))) & (~deco16_mask);
		case 0x590/2:									return ((((DECO_PORT(0xe)&0xfff0)>>4) | ((DECO_PORT(0xe)&0x000e)<<11) | ((DECO_PORT(0xe)&0x0001)<<15))) ^ deco16_xor;
		case 0x7b6/2:									return ((((DECO_PORT(0x2)&0xf000)>>8) | ((DECO_PORT(0x2)&0x0ff0)<<4) | ((DECO_PORT(0x2)&0x000f)<<0)) ^ deco16_xor) & (~deco16_mask);
		case 0x588/2:									return ((((DECO_PORT(0x4)&0xff00)>>4) | ((DECO_PORT(0x4)&0x00f0)<<8) | ((DECO_PORT(0x4)&0x000f)<<0)) ^ deco16_xor) & (~deco16_mask);
		case 0x1f6/2:									return (((DECO_PORT(0x6)&0xf000)>>12) | ((DECO_PORT(0x6)&0x0ff0)<<4) | ((DECO_PORT(0x6)&0x0007)<<5) | ((DECO_PORT(0x6)&0x0008)<<1)) ^ deco16_xor;
		case 0x4c0/2:									return (((DECO_PORT(0x8)&0xf000)>>4) | ((DECO_PORT(0x8)&0x0f00)<<4) | ((DECO_PORT(0x8)&0x00f0)>>4) | ((DECO_PORT(0x8)&0x000f)<<4)) & (~deco16_mask);
		case 0x63e/2:									return ((((DECO_PORT(0xa)&0x0ff0)<<4) | ((DECO_PORT(0xa)&0xf000)>>12) | ((DECO_PORT(0xa)&0x0003)<<6) | ((DECO_PORT(0xa)&0x000c)<<2)));
		case 0x7cc/2:									return ((((DECO_PORT(0xc)&0xfff0)>>4) | ((DECO_PORT(0xc)&0x000e)<<11) | ((DECO_PORT(0xc)&0x0001)<<15)) ^ deco16_xor) & (~deco16_mask);
		case 0x1bc/2:									return (((DECO_PORT(0xe)&0xf000)>>12) | ((DECO_PORT(0xe)&0x0f00)>>4) | ((DECO_PORT(0xe)&0x00ff)<<8)) & (~deco16_mask);
		case 0x780/2:									return DECO_PORT(0xb8); // also caveman ninja
		case 0x454/2:									return (((DECO_PORT(0x82)&0xf000)>>8) | ((DECO_PORT(0x82)&0x0f00)>>0) | ((DECO_PORT(0x82)&0x00f0)>>4) | ((DECO_PORT(0x82)&0x000f)<<12)) ^ deco16_xor;
		case 0x53e/2:									return ((DECO_PORT(0x9e)&0x0003)<<14) | ((DECO_PORT(0x9e)&0x000c)<<10);
		case 0x250/2:									return (((DECO_PORT(0x62)&0xf0f0)<<0) | ((DECO_PORT(0x62)&0x0f00)>>8)  | ((DECO_PORT(0x62)&0x000f)<<8)) & (~deco16_mask);
		case 0x150/2: /* Shared */						return DECO_PORT(0x7e);
		case 0x10e/2: /* Schmeizr Robo only */			return DECO_PORT(0x7c);
		case 0x56a/2: /* Schmeizr Robo only */			return (((DECO_PORT(0x7c)&0xfff0)>>4) | ((DECO_PORT(0x7c)&0x000e)<<11) | ((DECO_PORT(0x7c)&0x0001)<<15)) & (~deco16_mask);
		case 0x39a/2: /* Schmeizr Robo only */			return ((((DECO_PORT(0x7e)&0xfff0)>>4) | ((DECO_PORT(0x7e)&0x000e)<<11) | ((DECO_PORT(0x7e)&0x0001)<<15)) ^ deco16_xor) & (~deco16_mask);
		case 0x188/2: /* Schmeizr Robo only */			return (((deco16_mask&0x0003)<<6) | ((deco16_mask&0x000c)<<2) | ((deco16_mask&0x00f0)<<4) | ((deco16_mask&0x0f00)<<4)) & (~deco16_mask);
		case 0x3cc/2: /* Schmeizr Robo only */			return deco16_mask;
		case 0x04a/2: /* Schmeizr Robo only */			return DECO_PORT(0x9e) & (~deco16_mask);
		case 0x7e8/2: /* Schmeizr Robo only */			return DECO_PORT(0x4a) ^ deco16_xor;
		case 0x0fc/2: /* Schmeizr Robo only */			return DECO_PORT(0x4a);
		case 0x38c/2: /* Schmeizr Robo only */			return DECO_PORT(0x28);
		case 0x028/2: /* Schmeizr Robo only  */			return DECO_PORT(0x58);

		// caveman ninja cases
		case 0x224/2: /* was 0x080 */ /* Master level control */			return prot_ram[0x0/2];
		case 0x27a/2: /* was 0x0de */ /* Restart position control */			return prot_ram[0x2/2];
		case 0x242/2: /* was 0x0e6 */ /* The number of credits in the system. */			return prot_ram[0x4/2];
		case 0x222/2: /* was 0x086 */ /* End of game check.  See 0x1814 */			return prot_ram[0x6/2];
		case 0x2fe/2: /* was 0x05a */ /* Moved to 0x140000 on int */			return prot_ram[0x10/2];
		case 0x220/2: /* was 0x084 */ /* Moved to 0x14000a on int */			return prot_ram[0x12/2];
		case 0x284/2: /* was 0x020 */ /* Moved to 0x14000c on int */			return prot_ram[0x14/2];
		case 0x2d6/2: /* was 0x072 */ /* Moved to 0x14000e on int */			return prot_ram[0x16/2];
		case 0x278/2: /* was 0x0dc */ /* Moved to 0x150000 on int */			return prot_ram[0x18/2];
		case 0x2ca/2: /* was 0x06e */ /* Moved to 0x15000a on int */			return prot_ram[0x1a/2]; /* Not used on bootleg */
		case 0x2c8/2: /* was 0x06c */ /* Moved to 0x15000c on int */			return prot_ram[0x1c/2];
		//case 0x2ac/2: /* was 0x008 */ /* Moved to 0x15000e on int */			return prot_ram[0x1e/2];
		//case 0x292/2: /* was 0x036 */ /* Dip switches */			return space.machine().root_device().ioport("DSW")->read();
		//case 0x36c/2: /* was 0x1c8 */ /* Coins */			return space.machine().root_device().ioport("IN1")->read();
		//case 0x088/2: /* was 0x22c */ /* Player 1 & 2 input ports */			return space.machine().root_device().ioport("IN0")->read();
		case 0x016/2: /* was 0x2b2 */ return prot_ram[0x0fc/2]; // 0xad65
		case 0x68e/2: /* was 0x42a */ return prot_ram[0x092/2]; // 0xb2b7
		case 0x692/2: /* was 0x436 */ return prot_ram[0x088/2]; // 0xea5a
		//case 0x6e4/2: /* was 0x440 */ return prot_ram[0x098/2]; // 0x7aa0
		case 0x6e2/2: /* was 0x446 */ return prot_ram[0x090/2]; // 0x5fdf
		case 0x6fc/2: /* was 0x458 */ return prot_ram[0x082/2]; // 0x108d
		//case 0x624/2: /* was 0x480 */ return prot_ram[0x09a/2]; // 0xfbe4
		case 0x62a/2: /* was 0x48e */ return prot_ram[0x08a/2]; // 0x67a2
		case 0x638/2: /* was 0x49c */ return prot_ram[0x094/2]; // 0xb62a
		case 0x614/2: /* was 0x4b0 */ return prot_ram[0x096/2]; // 0x3555
		case 0x664/2: /* was 0x4c0 */ return prot_ram[0x09e/2]; // 0x6a34
		//case 0x662/2: /* was 0x4c6 */ return prot_ram[0x08c/2]; // 0xc6ef
		case 0x64e/2: /* was 0x4ea */ return prot_ram[0x086/2]; // 0x9feb
		//case 0x65e/2: /* was 0x4fa */ return prot_ram[0x09c/2]; // 0x1dcb
		case 0x7ac/2: /* was 0x508 */ return prot_ram[0x08e/2]; // 0xed9c
		//case 0x7b0/2: /* was 0x514 */ return prot_ram[0x080/2]; // 0xf6e2
		case 0x7b8/2: /* was 0x51c */ return prot_ram[0x084/2]; // 0xbdb9
		//case 0x780/2: /* was 0x524 */ return prot_ram[0x0b8/2]; // 0x5d26
		case 0x782/2: /* was 0x526 */ return prot_ram[0x0a2/2]; // 0x4770
		case 0x7f6/2: /* was 0x552 */ return prot_ram[0x0ac/2]; // 0x0843
		case 0x7f0/2: /* was 0x554 */ return prot_ram[0x0ba/2]; // 0x9b79
		case 0x7ca/2: /* was 0x56e */ return prot_ram[0x0a0/2]; // 0xd1be
		case 0x7d4/2: /* was 0x570 */ return prot_ram[0x0b4/2]; // 0xc950
		case 0x724/2: /* was 0x580 */ return prot_ram[0x0b2/2]; // 0xa600
		case 0x73e/2: /* was 0x59a */ return prot_ram[0x0ae/2]; // 0x2b24
		case 0x77c/2: /* was 0x5d8 */ return prot_ram[0x0b6/2]; // 0x17c1
		case 0x750/2: /* was 0x5f4 */ return prot_ram[0x0aa/2]; // 0xf152
		case 0x752/2: /* was 0x5f6 */ return prot_ram[0x0be/2]; // 0x97ce
		case 0x75c/2: /* was 0x5f8 */ return prot_ram[0x0bc/2]; // 0xa485
		case 0x4a0/2: /* was 0x604 */ return prot_ram[0x0a4/2]; // 0x0e88
		case 0x4a8/2: /* was 0x60c */ return prot_ram[0x0b0/2]; // 0xab89
		case 0x4be/2: /* was 0x61a */ return prot_ram[0x0a6/2]; // 0x64ba
		case 0x4ee/2: /* was 0x64a */ return prot_ram[0x0cc/2]; // 0x0ae9
		case 0x4d4/2: /* was 0x670 */ return prot_ram[0x0d4/2]; // 0x4ec2
		case 0x4da/2: /* was 0x67e */ return prot_ram[0x0ca/2]; // 0x61c3
		case 0x430/2: /* was 0x694 */ return prot_ram[0x0d2/2]; // 0x3b4f
		case 0x40c/2: /* was 0x6a8 */ return prot_ram[0x0d6/2]; // 0x0c27
		case 0x40a/2: /* was 0x6ae */ return prot_ram[0x0da/2]; // 0x3a72
		//case 0x410/2: /* was 0x6b4 */ return prot_ram[0x0de/2]; // 0x15d2
		case 0x460/2: /* was 0x6c4 */ return prot_ram[0x0c8/2]; // 0x5849
		case 0x46c/2: /* was 0x6c8 */ return prot_ram[0x0d0/2]; // 0x186c
		case 0x468/2: /* was 0x6cc */ return prot_ram[0x0c0/2]; // 0x713e
		case 0x47a/2: /* was 0x6de */ return prot_ram[0x0c2/2]; // 0xa87e
		case 0x45c/2: /* was 0x6f8 */ return prot_ram[0x0d8/2]; // 0x9a31
		case 0x45a/2: /* was 0x6fe */ return prot_ram[0x0c6/2]; // 0xec69
		case 0x5a4/2: /* was 0x700 */ return prot_ram[0x0ce/2]; // 0x82d9
		//case 0x5ae/2: /* was 0x70a */ return prot_ram[0x0dc/2]; // 0x628c
		case 0x5b0/2: /* was 0x714 */ return prot_ram[0x0c4/2]; // 0xda45
		case 0x5e8/2: /* was 0x74c */ return prot_ram[0x0e4/2]; // 0x8e3d
		case 0x5c0/2: /* was 0x764 */ return prot_ram[0x0fe/2]; // 0xdef7
		case 0x5d4/2: /* was 0x770 */ return prot_ram[0x0f4/2]; // 0xe7fe
		case 0x5d6/2: /* was 0x772 */ return prot_ram[0x0ec/2]; // 0x23ca
		case 0x5d0/2: /* was 0x774 */ return prot_ram[0x0e2/2]; // 0xe62c
		case 0x5da/2: /* was 0x77e */ return prot_ram[0x0e8/2]; // 0x6683
		case 0x52c/2: /* was 0x788 */ return prot_ram[0x0e0/2]; // 0xd60b
		case 0x53c/2: /* was 0x798 */ return prot_ram[0x0fa/2]; // 0x9e1a
		case 0x500/2: /* was 0x7a4 */ return prot_ram[0x0f0/2]; // 0x578f
		case 0x566/2: /* was 0x7c2 */ return prot_ram[0x0f8/2]; // 0x0503
		case 0x54e/2: /* was 0x7ea */ return prot_ram[0x0e6/2]; // 0x8654
		case 0x548/2: /* was 0x7ec */ return prot_ram[0x0f6/2]; // 0xa1e1
		case 0x55e/2: /* was 0x7fa */ return prot_ram[0x0ea/2]; // 0x5146
		case 0x55a/2: /* was 0x7fe */ return prot_ram[0x0f2/2]; // 0x91d4

		// wizard fire cases
		//case 0x088/2: /* was 0x110*/ /* Player input */		return space.machine().root_device().ioport("IN0")->read(); // also used in rohga sim
		//case 0x36c/2: /* was 0x36c*/							return space.machine().root_device().ioport("IN1")->read(); // also used in rohga sim
		case 0x2cc/2: /* was 0x334*/							return space.machine().root_device().ioport("IN1")->read();
		case 0x3b0/2: /* was 0x0dc*/							return space.machine().root_device().ioport("IN1")->read()<<4;
		//case 0x292/2: /* was 0x494*/ /* Dips */				return space.machine().root_device().ioport("DSW1_2")->read(); // also used in rohga sim // also caveman ninja
		//case 0x224/2: /* was 0x244*/							return DECO_NEW_PORT(0x00); // also caveman ninja
		//case 0x33e/2: /* was 0x7cc*/							return ((DECO_NEW_PORT(0x00)&0x000f)<<12) | ((DECO_NEW_PORT(0x00)&0x00f0)<<4) | ((DECO_NEW_PORT(0x00)&0x0f00)>>4) | ((DECO_NEW_PORT(0x00)&0xf000)>>12); // also used in rohga sim (NOTE, ROHGA APPLIES MASK, CHECK!)
		case 0x030/2: /* was 0x0c0*/							return (((DECO_NEW_PORT(0x00)&0x000e)>>1) | ((DECO_NEW_PORT(0x00)&0x0001)<<3))<<12;
		case 0x118/2: /* was 0x188*/							return (((DECO_NEW_PORT(0x00)&0x000e)>>1) | ((DECO_NEW_PORT(0x00)&0x0001)<<3))<<12;
		case 0x7a6/2: /* was 0x65e*/							return (((DECO_NEW_PORT(0x00)&0x000c)>>2) | ((DECO_NEW_PORT(0x00)&0x0003)<<2))<<12;
		case 0x73a/2: /* was 0x5ce*/							return ((DECO_NEW_PORT(0x00)<<8)&0xf000) | ((DECO_NEW_PORT(0x00)&0xe)<<7) | ((DECO_NEW_PORT(0x00)&0x1)<<11);
		case 0x586/2: /* was 0x61a*/							return (DECO_NEW_PORT(0x00)<<8)&0xff00;
		//case 0x692/2: /* was 0x496*/							return DECO_NEW_PORT(0x88); // also caveman ninja
		case 0x502/2: /* was 0x40a*/							return ((DECO_NEW_PORT(0x88)&0x000f)<<12) | ((DECO_NEW_PORT(0x88)&0x00f0)>>4) | ((DECO_NEW_PORT(0x88)&0x0f00)<<0) | ((DECO_NEW_PORT(0x88)&0xf000)>>8);
		//case 0x178/2: /* was 0x1e8*/							return ((DECO_NEW_PORT(0x88)&0x00ff)<<8) | ((DECO_NEW_PORT(0x88)&0xff00)>>8); // also used in rohga sim  (NOTE, ROHGA APPLIES MASK, CHECK!)
		case 0x3d2/2: /* was 0x4bc*/							return ((DECO_NEW_PORT(0x88)&0x0ff0)<<4) | ((DECO_NEW_PORT(0x88)&0x0003)<<6) | ((DECO_NEW_PORT(0x88)&0x000c)<<2);
		case 0x762/2: /* was 0x46e*/							return ((DECO_NEW_PORT(0x88)&0xfff0)<<0) | ((DECO_NEW_PORT(0x88)&0x0007)<<1) | ((DECO_NEW_PORT(0x88)&0x0008)>>3);
		case 0x264/2: /* was 0x264*/							return ((DECO_NEW_PORT(0x88)&0x000f)<<8) | ((DECO_NEW_PORT(0x88)&0x00f0)>>0) | ((DECO_NEW_PORT(0x88)&0x0f00)<<4);
		case 0x4e8/2: /* was 0x172*/							return ((DECO_NEW_PORT(0x88)&0x000f)<<4) | ((DECO_NEW_PORT(0x88)&0x00f0)<<4) | ((DECO_NEW_PORT(0x88)&0xf000)<<0);
		//case 0x284/2: /* was 0x214*/							return DECO_NEW_PORT(0x14); // also caveman ninja
		case 0x74a/2: /* was 0x52e*/							return ((DECO_NEW_PORT(0x14)&0x000f)<<8) | ((DECO_NEW_PORT(0x14)&0x00f0)>>0) | ((DECO_NEW_PORT(0x14)&0x0f00)>>8) | ((DECO_NEW_PORT(0x14)&0xf000)>>0);
		case 0x5e0/2: /* was 0x07a*/							return ((DECO_NEW_PORT(0x14)&0x000f)<<8) | ((DECO_NEW_PORT(0x14)&0x00f0)>>0) | ((DECO_NEW_PORT(0x14)&0x0f00)>>8) | ((DECO_NEW_PORT(0x14)&0xf000)>>0);
		case 0x06c/2: /* was 0x360*/							return ((DECO_NEW_PORT(0x14)&0x000f)<<8) | ((DECO_NEW_PORT(0x14)&0x00f0)>>0) | ((DECO_NEW_PORT(0x14)&0x0f00)>>8) | ((DECO_NEW_PORT(0x14)&0xf000)>>0);
		case 0x3b2/2: /* was 0x4dc*/							return ((DECO_NEW_PORT(0x14)&0x0ff0)<<4) | ((DECO_NEW_PORT(0x14)&0x0007)<<5) | ((DECO_NEW_PORT(0x14)&0x0008)<<1);
		case 0x15c/2: /* was 0x3a8*/							return ((DECO_NEW_PORT(0x14)&0x000e)<<3) | ((DECO_NEW_PORT(0x14)&0x0001)<<7) | ((DECO_NEW_PORT(0x14)&0x0ff0)<<4) | ((DECO_NEW_PORT(0x14)&0xf000)>>12);
		case 0x6f4/2: /* was 0x2f6*/							return ((DECO_NEW_PORT(0x14)&0xff00)>>8) | ((DECO_NEW_PORT(0x14)&0x00f0)<<8) | ((DECO_NEW_PORT(0x14)&0x000c)<<6) | ((DECO_NEW_PORT(0x14)&0x0003)<<10);
		//case 0x27e/2: /* was 0x7e4*/							return (DECO_NEW_PORT(0x94)&0x00f0)<<8; // also used in rohga sim (NOTE, ROHGA APPLIES XOR and MASK, CHECK!)
		case 0x6ca/2: /* was 0x536*/							return ((DECO_NEW_PORT(0xd4)&0x000f)<<8) | ((DECO_NEW_PORT(0xd4)&0x00f0)<<0) | ((DECO_NEW_PORT(0xd4)&0x0f00)<<4) | ((DECO_NEW_PORT(0xd4)&0xf000)>>12);
		case 0x7d0/2: /* was 0x0be*/							return ((DECO_NEW_PORT(0xec)&0x000f)<<4) | ((DECO_NEW_PORT(0xec)&0x00f0)<<4) | ((DECO_NEW_PORT(0xec)&0x0f00)>>8) | ((DECO_NEW_PORT(0xec)&0xf000)>>0);
		//case 0x092/2: /* was 0x490*/							return (DECO_NEW_PORT(0x3c)&0xfff0) | ((DECO_NEW_PORT(0x3c)&0x0007)<<1) | ((DECO_NEW_PORT(0x3c)&0x0008)>>3); // also used in rohga sim
		case 0x08e/2: /* was 0x710*/							return (DECO_NEW_PORT(0xc2)&0xfff0) | ((DECO_NEW_PORT(0xc2)&0x0007)<<1) | ((DECO_NEW_PORT(0xc2)&0x0008)>>3);
		case 0x544/2: /* was 0x22a*/							return ((DECO_NEW_PORT(0x5a)&0xff00)>>8) | ((DECO_NEW_PORT(0x5a)&0x00f0)<<8) | ((DECO_NEW_PORT(0x5a)&0x0001)<<11) | ((DECO_NEW_PORT(0x5a)&0x000e)<<7);
		case 0x646/2: /* was 0x626*/							return ((DECO_NEW_PORT(0xda)&0x000f)<<8) | ((DECO_NEW_PORT(0xda)&0x00f0)<<8) | ((DECO_NEW_PORT(0xda)&0x0f00)>>4) | ((DECO_NEW_PORT(0xda)&0xf000)>>12);
		//case 0x222/2: /* was 0x444*/							return DECO_NEW_PORT(0x06); // (old comment was 'rohga') /* this CAN'T be right (port addr > 0x100), is it even used by this game or some c+p error? */
		case 0x35a/2: /* was 0x5ac*/							return ((DECO_NEW_PORT(0x76)&0xfff0)>>4) | ((DECO_NEW_PORT(0x76)&0x0007)<<13) | ((DECO_NEW_PORT(0x76)&0x0008)<<9);
		case 0x0a6/2: /* was 0x650*/							return ((DECO_NEW_PORT(0xbe)&0xfff0)>>4) | ((DECO_NEW_PORT(0xbe)&0x000f)<<12); // also used in rohga sim
		case 0x352/2: /* was 0x4ac*/							return ((DECO_NEW_PORT(0x62)&0x0007)<<13) | ((DECO_NEW_PORT(0x62)&0x0008)<<9);

	}
	logerror("Protection PC %06x: warning - read unmapped memory address %04x\n",space.device().safe_pc(),offset);

	return 0x0000;
}
/***************************************************************************/

WRITE16_HANDLER( deco16_104_prot_w ) /* Wizard Fire */
{
	int deco104_addr = BITSWAP32(offset*2, /* NC */31,30,29,28,27,26,25,24,23,22,21,20,19,18, 13,12,11,/**/      17,16,15,14,    1,2,3, 4,5,6,7, 8,9,10,0) & 0x7fff;

	deco104_addr &=0xff;

	if (decoprot_buffer_ram_selected)
		COMBINE_DATA(&decoprot_buffer_ram[deco104_addr/2]);
	else
		COMBINE_DATA(&deco16_prot_ram[deco104_addr/2]);


	driver_device *state = space.machine().driver_data<driver_device>();
	if (deco104_addr == (0xa8))
	{
		state->soundlatch_byte_w(space, 0, data & 0xff);
		space.machine().device("audiocpu")->execute().set_input_line(0, HOLD_LINE);
		return;
	}

	if (deco104_addr != (0x00))
	if (deco104_addr != (0x88))
	if (deco104_addr != (0xa8))
	if (deco104_addr != (0x14))
	if (deco104_addr != (0x94))
	if (deco104_addr != (0xd4))
	if (deco104_addr != (0xec))
	if (deco104_addr != (0x3c))
	if (deco104_addr != (0xc2))
	if (deco104_addr != (0x5a))
	if (deco104_addr != (0xda))
	//if (deco104_addr != (0x206))
	if (deco104_addr != (0x76))
	if (deco104_addr != (0xbe))
	if (deco104_addr != (0x62))
		printf("CONTROL PC %06x: warning - write protection memory address %04x %04x\n", space.device().safe_pc(), deco104_addr, data);



}



/*

*/

READ16_HANDLER( deco16_104_prot_r ) /* Wizard Fire */
{
	int deco104_addr = BITSWAP32(offset*2, /* NC */31,30,29,28,27,26,25,24,23,22,21,20,19,18, 13,12,11,/**/      17,16,15,14,    1,2,3, 4,5,6,7, 8,9,10,0) & 0x7fff;
	return deco104_read_core(space, deco104_addr/2, mem_mask);
}

/***************************************************************************/

/***************************************************************************/

WRITE16_HANDLER( deco16_104_cninja_prot_w )
{
	driver_device *state = space.machine().driver_data<driver_device>();

	offset &=0x7f;

	if (decoprot_buffer_ram_selected)
		COMBINE_DATA(&decoprot_buffer_ram[offset]);
	else
		COMBINE_DATA(&deco16_prot_ram[offset]);

	if (offset == (0xa8 / 2))
	{
		state->soundlatch_byte_w(space, 0, data & 0xff);
		space.machine().device("audiocpu")->execute().set_input_line(0, HOLD_LINE);
		return;
	}


}






READ16_HANDLER( deco16_104_cninja_prot_r )
{
	int real_offset = offset << 1;
	real_offset ^= DECO104_MAGIC_XOR;

	return deco104_read_core(space, real_offset/2, mem_mask);
}

/***************************************************************************/

/***************************************************************************/

WRITE16_HANDLER( deco16_104_rohga_prot_w )
{

	offset &=0x7f;
	/*
	    Schmeizr uses a feature of this chip that other games don't seem to exploit.  There
	    appear to be two banks of data ports (referred to here as front and back buffers).
	    These banks are switched when certain addresses are read - also very good protection
	    against trojans as it is non-obvious when data ports are switched.

	    It appears to work as follows..  When the front buffer is active (the state upon
	    reset) any writes to data ports push the existing front buffer word into the back
	    buffer (ie, a FIFO latch type of system).  When the back buffer is active, any writes
	    to data ports affect the back buffer only - the front buffer data is not affected, and
	    there is no FIFO action.

	    By default the read ports return a scrambled address and scrambled data from a
	    front buffer port.  When the back buffer is active most read ports return scrambled
	    data from the back buffer, however some ports work exclusively on front buffer data
	    even if the back buffer is selected.

	    There doesn't appear to be any way to detect what bank is currently selected - it
	    seems game code must maintain this state (if it matters).
	*/
	offset &=0x7f;

	if (decoprot_buffer_ram_selected)
		COMBINE_DATA(&decoprot_buffer_ram[offset]);
	else
		COMBINE_DATA(&deco16_prot_ram[offset]);
	driver_device *state = space.machine().driver_data<driver_device>();
	if (offset == (0xa8 / 2))
	{
		state->soundlatch_byte_w(space, 0, data & 0xff);
		space.machine().device("audiocpu")->execute().set_input_line(0, HOLD_LINE);
		return;
	}

	// These are set regardless of bank
	if (offset==0x42/2)
		COMBINE_DATA(&deco16_xor);
	if (offset==0xee/2)
		COMBINE_DATA(&deco16_mask);

}



READ16_HANDLER( deco16_104_rohga_prot_r )
{
	return deco104_read_core(space, offset, mem_mask);
}

/**********************************************************************************/


/**********************************************************************************/

READ16_HANDLER( dietgo_104_prot_r )
{
	switch (offset * 2)
	{
	case 0x298: return space.machine().root_device().ioport("IN0")->read();
	case 0x342: return space.machine().root_device().ioport("IN1")->read();
	case 0x506: return space.machine().root_device().ioport("DSW")->read();
	}

	logerror("Protection PC %06x: warning - read unmapped memory address %04x\n", space.device().safe_pc(), offset<<1);

	return 0;
}

WRITE16_HANDLER( dietgo_104_prot_w )
{
	driver_device *state = space.machine().driver_data<driver_device>();
	if (offset == (0x380 / 2))
	{
		state->soundlatch_byte_w(space, 0, data & 0xff);
		space.machine().device("audiocpu")->execute().set_input_line(0, HOLD_LINE);
		return;
	}
	logerror("Protection PC %06x: warning - write unmapped memory address %04x %04x\n", space.device().safe_pc(), offset << 1, data);
}

/**********************************************************************************/

READ16_HANDLER( deco16_104_pktgaldx_prot_r )
{
	const UINT16* prot_ram=deco16_prot_ram;
	switch (offset * 2)
	{
	case 0x5b2: return space.machine().root_device().ioport("SYSTEM")->read();
	case 0x44c: return space.machine().root_device().ioport("DSW")->read();
	case 0x042: return space.machine().root_device().ioport("INPUTS")->read();

	case 0x510: return DECO_PORT(0);
	case 0x51a: return DECO_PORT(2);
	}

	logerror("Protection PC %06x: warning - read unmapped memory address %04x\n", space.device().safe_pc(), offset<<1);

	return 0;
}

WRITE16_HANDLER( deco16_104_pktgaldx_prot_w )
{
	COMBINE_DATA(&deco16_prot_ram[offset]);
//  logerror("Protection PC %06x: warning - write unmapped memory address %04x %04x\n",space.device().safe_pc(),offset<<1,data);
}

/**********************************************************************************/





/* protection.. involves more addresses than this .. */
/* this is going to be typical deco '104' protection...
 writes one place, reads back data shifted in another
 the addresses below are the ones seen accessed by the
 game so far...

 we need to log the PC of each read/write and check to
 see if the code makes any of them move obvious




 
 	// protection 
//  AM_RANGE(0x280104, 0x280105) AM_WRITENOP              // ??
//  AM_RANGE(0x2800ac, 0x2800ad) AM_READ_PORT("DSW")            // dips
//  AM_RANGE(0x280298, 0x280299) AM_READ_PORT("SYSTEM")         // vbl
//  AM_RANGE(0x280506, 0x280507) AM_READ_PORT("UNK")
//  AM_RANGE(0x2802b4, 0x2802b5) AM_READ_PORT("P1_P2")          // inverted?
//  AM_RANGE(0x280330, 0x280331) AM_READNOP               // sound?
//  AM_RANGE(0x280380, 0x280381) AM_WRITENOP              // sound
 */
READ16_MEMBER(deco104_device::dblewing_prot_r)
{
	int deco104_addrxx = BITSWAP32(offset*2,  31,30,29,28,27,26,25,24,23,22,21,20,19,18, 13,12,11,      17,16,15,14,   5,  6,  4,  7,  3,   8,  2,  9,  1,  10,  0) & 0x7fff;

	deco104_addrxx ^= DECO104_MAGIC_XOR;

	switch (deco104_addrxx)
	{
		//case 0x4c0: /* was 0x664 */ /* was 0x16a*/ return m_boss_move;          // boss 1 movement  // in shared sim
		case 0x13a: /* was 0x39e */ /* was 0x6d6*/ return m_boss_move;          // boss 1 2nd pilot
		case 0x0ce: /* was 0x26a */ /* was 0x748*/ return m_boss_move;          // boss 1 3rd pilot

		case 0x492: /* was 0x636 */ /* was 0x566*/ return 0x0009;             // boss BGM,might be a variable one (read->write to the sound latch)
		case 0x440: /* was 0x6e4 */ /* was 0x1ea*/ return m_boss_shoot_type;    // boss 1 shoot type
		case 0x312: /* was 0x1b6 */ /* was 0x596*/ return m_boss_3_data;          // boss 3 appearing
		//case 0x32a: /* was 0x18e */ /* was 0x692*/ return m_boss_4_data; // in shared sim
		case 0x72e: /* was 0x58a */ /* was 0x6b0*/ return m_boss_5_data;
		//case 0x3d2: /* was 0x176 */ /* was 0x51e*/ return m_boss_5sx_data;  // in shared sim
		case 0x21e: /* was 0x0ba */ /* was 0x784*/ return m_boss_6_data;

		case 0x78c: /* was 0x528 */ /* was 0x330*/ return 0; // controls bonuses such as shoot type,bombs etc.
		case 0x114: /* was 0x3b0 */ /* was 0x1d4*/ return m_70c_data;  //controls restart points

		case 0x674: /* was 0x4d0 */ /* was 0x0ac*/ return (ioport(":DSW")->read() & 0x40) << 4;//flip screen
		case 0x726: /* was 0x582 */ /* was 0x4b0*/return m_608_data;//coinage
		case 0x4e4: /* was 0x640 */ /* was 0x068*/
		{
			switch (ioport(":DSW")->read() & 0x0300) //I don't know how to relationate this...
			{
				case 0x0000: return 0x000;//0
				case 0x0100: return 0x060;//3
				case 0x0200: return 0x0d0;//6
				case 0x0300: return 0x160;//b
			}
		}
		//case 0x334: /* was 0x190 */ /* was 0x094*/ return m_104_data;// p1 inputs select screen  OK // in shared sim
		//case 0x0fc: /* was 0x258 */ /* was 0x24c*/return m_008_data;//read DSW (mirror for coinage/territory) // in shared sim
		//case 0x36c: /* was 0x1c8 */ /* was 0x298*/return ioport(":IN1")->read();//vblank // in shared sim
		case 0x5b2: /* was 0x716 */ /* was 0x476*/return ioport(":IN1")->read();//mirror for coins
		//case 0x292: /* was 0x036 */ /* was 0x506*/return ioport(":DSW")->read(); // in shared sim
		case 0x146: /* was 0x3e2 */ /* was 0x5d8*/return m_406_data;
		case 0x73c: /* was 0x598 */ /* was 0x2b4*/return ioport(":IN0")->read();
		case 0x644: /* was 0x4e0 */ /* was 0x1a8*/return (ioport(":DSW")->read() & 0x4000) >> 12;//allow continue
		// case 0x45c: /* was 0x6f8 */ /* was 0x3ec*/return m_70c_data; //score entry // in shared sim
		case 0x0b8: /* was 0x21c */ /* was 0x246*/return m_580_data; // these three controls "perfect bonus" I suppose...
		case 0x6d2: /* was 0x476 */ /* was 0x52e*/return m_580_data;
		//case 0x782: /* was 0x526 */ /* was 0x532*/return m_580_data; // in shared sim


		case 0x564: /* was 0x7c0 */ /* was 0x0f8*/ return 0; // m_080_data;
		//case 0x294: /* was 0x030 */ /* was 0x104*/ return 0; // in shared sim (ram buffer change!)
		//case 0x2d0: /* was 0x074 */ /* was 0x10e*/ return 0;// in shared sim (ram buffer change!)
		//case 0x2b8: /* was 0x01c */ /* was 0x206*/ return 0; // m_70c_data;// in shared sim (ram buffer change!)
		case 0x1fc: /* was 0x358 */ /* was 0x25c*/return 0;
		case 0x23c: /* was 0x098 */ /* was 0x284*/ return 0; // 3rd player 2nd boss
		case 0x7a2: /* was 0x506 */ /* was 0x432*/return 0; // boss on water level?
		case 0x0c2: /* was 0x266 */ /* was 0x54a*/ return 0; // 3rd player 2nd boss
		case 0x21a: /* was 0x0be */ /* was 0x786*/return 0;

	}

//  printf("dblewing prot r %08x, %04x, %04x\n", space.device().safe_pc(), offset * 2, mem_mask);

	mame_printf_debug("dblewing prot r %08x, %04x, %04x\n", space.device().safe_pc(), offset * 2, mem_mask);

	return deco104_read_core(space, deco104_addrxx/2, mem_mask);

	return 0;//machine().rand();
}


 








WRITE16_MEMBER(deco104_device::dblewing_prot_w)
{
//  if (offset * 2 != 0x380)
//  printf("dblewing prot w %08x, %04x, %04x %04x\n", space.device().safe_pc(), offset * 2, mem_mask, data);
	driver_device *drvstate = machine().driver_data<driver_device>();
	cpu_device* cpudev = 0;

	int deco104_addrxx = BITSWAP32(offset *2,  31,30,29,28,27,26,25,24,23,22,21,20,19,18, 13,12,11,      17,16,15,14,  5,  6,  4,  7,  3,   8,  2,  9,  1,  10,    0) & 0x7fff;

	deco104_addrxx &=0xff;

	if (decoprot_buffer_ram_selected)
		COMBINE_DATA(&decoprot_buffer_ram[deco104_addrxx/2]);
	else
		COMBINE_DATA(&deco16_prot_ram[deco104_addrxx/2]);
	driver_device *state = space.machine().driver_data<driver_device>();
	if (deco104_addrxx == (0xa8))
	{
		state->soundlatch_byte_w(space, 0, data & 0xff);
		space.machine().device("audiocpu")->execute().set_input_line(0, HOLD_LINE);
	}

	// These are set regardless of bank
	if (deco104_addrxx==0x42)
		COMBINE_DATA(&deco16_xor);
	if (deco104_addrxx==0xee)
		COMBINE_DATA(&deco16_mask);


	switch (deco104_addrxx)
	{
		case 0x0c0: /* was 0x088*/
			m_088_data = data;
			if(m_088_data == 0)          { m_boss_4_data = 0;    }
			else if(m_088_data & 0x8000) { m_boss_4_data = 0x50; }
			else                                { m_boss_4_data = 0x40; }

			return;

		case 0x030: /* was 0x104*/
			m_104_data = data;
			return; // p1 inputs select screen  OK

		case 0x0e4: /* was 0x18a*/
			m_18a_data = data;
			switch (m_18a_data)
			{
				case 0x6b94: m_boss_5_data = 0x10; break; //initialize
				case 0x7c68: m_boss_5_data = 0x60; break; //go up
				case 0xfb1d: m_boss_5_data = 0x50; break;
				case 0x977c: m_boss_5_data = 0x50; break;
				case 0x8a49: m_boss_5_data = 0x60; break;
			}
			return;
		case 0x008: /* was 0x200*/
			m_200_data = data;
			switch (m_200_data)
			{
				case 0x5a19: m_boss_move = 1; break;
				case 0x3b28: m_boss_move = 2; break;
				case 0x1d4d: m_boss_move = 1; break;
			}
			//popmessage("%04x",m_200_data);
			return;
		case 0x088: /* was 0x280*/
			m_280_data = data;
			switch (m_280_data)
			{
				case 0x6b94: m_boss_5sx_data = 0x10; break;
				case 0x7519: m_boss_5sx_data = 0x60; break;
				case 0xfc68: m_boss_5sx_data = 0x50; break;
				case 0x02dd: m_boss_5sx_data = 0x50; break;
				case 0x613c: m_boss_5sx_data = 0x50; break;
			}
			//printf("%04x\n",m_280_data);
			return;
		case 0x0a8: /* was 0x380*/
			drvstate->soundlatch_byte_w(space, 0, data & 0xff);
			cpudev = (cpu_device*)machine().device(":audiocpu");
			if (cpudev) cpudev->set_input_line(0, HOLD_LINE);


		//	m_sound_irq |= 0x02;
		//	m_audiocpu->set_input_line(0, (m_sound_irq != 0) ? ASSERT_LINE : CLEAR_LINE);
			return;
		case 0x0b8: /* was 0x384*/
			m_384_data = data;
			switch(m_384_data)
			{
				case 0xaa41: m_boss_6_data = 1; break;
				case 0x5a97: m_boss_6_data = 2; break;
				case 0xbac5: m_boss_6_data = 3; break;
				case 0x0afb: m_boss_6_data = 4; break;
				case 0x6a99: m_boss_6_data = 5; break;
				case 0xda8f: m_boss_6_data = 6; break;
			}
			return;
		case 0x0fc: /* was 0x38e*/
			m_38e_data = data;
			switch(m_38e_data)
			{
				case 0x6c13: m_boss_shoot_type = 3; break;
				case 0xc311: m_boss_shoot_type = 0; break;
				case 0x1593: m_boss_shoot_type = 1; break;
				case 0xf9db: m_boss_shoot_type = 2; break;
				case 0xf742: m_boss_shoot_type = 3; break;

				case 0xeff5: m_boss_move = 1; break;
				case 0xd2f1: m_boss_move = 2; break;
				//default:   printf("%04x\n",m_38e_data); break;
				//case 0xe65a: m_boss_shoot_type = 0; break;
			}
			return;
		case 0x0f2: /* was 0x58c*/ // 3rd player 1st level
			m_58c_data = data;
			if(m_58c_data == 0)     { m_boss_move = 5; }
			else                           { m_boss_move = 2; }

			return;
		case 0x04e: /* was 0x60a*/
			m_60a_data = data;
			if(m_60a_data & 0x8000) { m_boss_3_data = 2; }
			else                           { m_boss_3_data = 9; }

			return;
		case 0x0a2: /* was 0x580*/
			m_580_data = data;
			return;
		case 0x016: /* was 0x406*/
			m_406_data = data;
			return;  // p2 inputs select screen  OK

		case 0x040: /* was 0x008*/ { m_008_data = data; return; }
		case 0x080: /* was 0x080*/ { m_080_data = data; return; } // p3 3rd boss?
		case 0x0d8: /* was 0x28c*/ { m_28c_data = data; return; }
		case 0x042: /* was 0x408*/ { m_408_data = data; return; } // 3rd player 1st level?
		case 0x056: /* was 0x40e*/ { m_40e_data = data; return; } // 3rd player 2nd level?
		case 0x04a: /* was 0x608*/ { m_608_data = data; return; }
		case 0x07a: /* was 0x70c*/ { m_70c_data = data; return; }
		case 0x0ee: /* was 0x78a*/ { m_78a_data = data; return; }
		case 0x0ea: /* was 0x788*/ { m_788_data = data; return; }
	}

//  printf("dblewing prot w %08x, %04x, %04x %04x\n", space.device().safe_pc(), offset * 2, mem_mask, data);


}

