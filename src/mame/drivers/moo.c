/***************************************************************************

 Wild West C.O.W.boys of Moo Mesa
 Bucky O'Hare
 (c) 1992 Konami
 Driver by R. Belmont and Acho A. Tang based on xexex.c by Olivier Galibert.
 Moo Mesa protection information thanks to ElSemi and OG.

 These are the final Xexex hardware games before the pre-GX/Mystic Warriors
 hardware took over.

Bug Fixes and Outstanding Issues
--------------------------------
Moo:
 - 54338 color blender support. Works fine with Bucky but needs a correct
   enable/disable bit in Moo. (intro gfx missing and fog blocking view)
 - Enemies coming out of the jail cells in the last stage have wrong priority.
   Could be tile priority or the typical "small Z, big pri" sprite masking
   trick currently not supported by K053247_sprites_draw().

Moo (bootleg):
 - No sprites appear, and the game never enables '246 interrupts.  Of course,
   they're using some copy of a '246 on FPGAs, so who knows what's going on...

Bucky:
 - Shadows sometimes have wrong priority. (unsupported priority modes)
 - Gaps between zoomed sprites. (fraction round-off)
 - Rogue sprites keep popping on screen after stage 2. They can usually be
   found near 950000 with sprite code around 5e40 or f400. The GFX viewer
   only shows blanks at 5e40, however. Are they invalid data from bad
   sprite ROMs or markers that aren't supposed to be displayed? These
   artifacts have one thing in common: they all have zero zcode. In fact
   no other sprites in Bucky seems to have zero zcode.

   Update: More garbages seen in later stages with a great variety.
   There's enough indication to assume Bucky simply ignores sprites with
   zero Z. I wonder why nobody reported this.

***************************************************************************/

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "cpu/z80/z80.h"
#include "machine/eeprom.h"
#include "sound/2151intf.h"
#include "sound/okim6295.h"
#include "sound/k054539.h"
#include "video/konicdev.h"
#include "machine/k053252.h"
#include "includes/konamipt.h"
#include "includes/moo.h"

#define MOO_DEBUG 0
#define MOO_DMADELAY (100)


static const eeprom_interface eeprom_intf =
{
	7,			/* address bits */
	8,			/* data bits */
	"011000",		/* read command */
	"011100",		/* write command */
	"0100100000000",	/* erase command */
	"0100000000000",	/* lock command */
	"0100110000000" 	/* unlock command */
};

READ16_MEMBER(moo_state::control2_r)
{
	return m_cur_control2;
}

WRITE16_MEMBER(moo_state::control2_w)
{
	/* bit 0  is data */
	/* bit 1  is cs (active low) */
	/* bit 2  is clock (active high) */
	/* bit 5  is enable irq 5 (unconfirmed) */
	/* bit 8  is enable sprite ROM reading */
	/* bit 10 is watchdog */
	/* bit 11 is enable irq 4 (unconfirmed) */

	COMBINE_DATA(&m_cur_control2);

	input_port_write(machine(), "EEPROMOUT", m_cur_control2, 0xff);

	if (data & 0x100)
		k053246_set_objcha_line(m_k053246, ASSERT_LINE);
	else
		k053246_set_objcha_line(m_k053246, CLEAR_LINE);
}


static void moo_objdma( running_machine &machine, int type )
{
	moo_state *state = machine.driver_data<moo_state>();
	int num_inactive;
	UINT16 *src, *dst, zmask;
	int counter = k053247_get_dy(state->m_k053246);

	k053247_get_ram(state->m_k053246, &dst);
	src = state->m_spriteram;
	num_inactive = counter = 256;

	zmask = (type) ? 0x00ff : 0xffff;

	do {
		if ((*src & 0x8000) && (*src & zmask))
		{
			memcpy(dst, src, 0x10);
			dst += 8;
			num_inactive--;
		}
		src += 0x80;
	}
	while (--counter);

	if (num_inactive) do { *dst = 0; dst += 8; } while (--num_inactive);
}

static TIMER_CALLBACK( dmaend_callback )
{
	moo_state *state = machine.driver_data<moo_state>();
	if (state->m_cur_control2 & 0x800)
		device_set_input_line(state->m_maincpu, 4, HOLD_LINE);
}

static INTERRUPT_GEN( moo_interrupt )
{
	moo_state *state = device->machine().driver_data<moo_state>();
	if (k053246_is_irq_enabled(state->m_k053246))
	{
		moo_objdma(device->machine(), state->m_game_type);

		// schedule DMA end interrupt (delay shortened to catch up with V-blank)
        state->m_dmaend_timer->adjust(attotime::from_usec(MOO_DMADELAY));
	}

	// trigger V-blank interrupt
	if (state->m_cur_control2 & 0x20)
		device_set_input_line(device, 5, HOLD_LINE);
}

static INTERRUPT_GEN( moobl_interrupt )
{
	moo_state *state = device->machine().driver_data<moo_state>();
	moo_objdma(device->machine(), state->m_game_type);

	// schedule DMA end interrupt (delay shortened to catch up with V-blank)
    state->m_dmaend_timer->adjust(attotime::from_usec(MOO_DMADELAY));

	// trigger V-blank interrupt
	device_set_input_line(device, 5, HOLD_LINE);
}

WRITE16_MEMBER(moo_state::sound_cmd1_w)
{
	if ((data & 0x00ff0000) == 0)
	{
		data &= 0xff;
		soundlatch_byte_w(space, 0, data);
	}
}

WRITE16_MEMBER(moo_state::sound_cmd2_w)
{
	if ((data & 0x00ff0000) == 0)
		soundlatch2_byte_w(space, 0, data & 0xff);
}

WRITE16_MEMBER(moo_state::sound_irq_w)
{
	device_set_input_line(m_audiocpu, 0, HOLD_LINE);
}

READ16_MEMBER(moo_state::sound_status_r)
{
	return soundlatch3_byte_r(space, 0);
}

WRITE8_MEMBER(moo_state::sound_bankswitch_w)
{
	membank("bank1")->set_base(machine().root_device().memregion("soundcpu")->base() + 0x10000 + (data&0xf)*0x4000);
}


#if 0 // (for reference; do not remove)

/* the interface with the 053247 is weird. The chip can address only 0x1000 bytes */
/* of RAM, but they put 0x10000 there. The CPU can access them all. */
READ16_MEMBER(moo_state::K053247_scattered_word_r)
{

	if (offset & 0x0078)
		return m_spriteram[offset];
	else
	{
		offset = (offset & 0x0007) | ((offset & 0x7f80) >> 4);
		return k053247_word_r(m_k053246, offset, mem_mask);
	}
}

WRITE16_MEMBER(moo_state::K053247_scattered_word_w)
{

	if (offset & 0x0078)
		COMBINE_DATA(m_spriteram + offset);
	else
	{
		offset = (offset & 0x0007) | ((offset & 0x7f80) >> 4);

		k053247_word_w(m_k053246, offset, data, mem_mask);
	}
}

#endif


WRITE16_MEMBER(moo_state::moo_prot_w)
{
	UINT32 src1, src2, dst, length, a, b, res;

	COMBINE_DATA(&m_protram[offset]);

	if (offset == 0xc)	// trigger operation
	{
		src1 = (m_protram[1] & 0xff) << 16 | m_protram[0];
		src2 = (m_protram[3] & 0xff) << 16 | m_protram[2];
		dst = (m_protram[5] & 0xff) << 16 | m_protram[4];
		length = m_protram[0xf];

		while (length)
		{
			a = space.read_word(src1);
			b = space.read_word(src2);
			res = a + 2 * b;

			space.write_word(dst, res);

			src1 += 2;
			src2 += 2;
			dst += 2;
			length--;
		}
	}
}


static WRITE16_DEVICE_HANDLER( moobl_oki_bank_w )
{
	logerror("%x to OKI bank\n", data);

	downcast<okim6295_device *>(device)->set_bank_base((data & 0x0f) * 0x40000);
}

static ADDRESS_MAP_START( moo_map, AS_PROGRAM, 16, moo_state )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x0c0000, 0x0c003f) AM_DEVWRITE_LEGACY("k056832", k056832_word_w)
	AM_RANGE(0x0c2000, 0x0c2007) AM_DEVWRITE_LEGACY("k053246", k053246_word_w)

	AM_RANGE(0x0c4000, 0x0c4001) AM_DEVREAD_LEGACY("k053246", k053246_word_r)
	AM_RANGE(0x0ca000, 0x0ca01f) AM_DEVWRITE_LEGACY("k054338", k054338_word_w)		/* K054338 alpha blending engine */
	AM_RANGE(0x0cc000, 0x0cc01f) AM_DEVWRITE_LEGACY("k053251", k053251_lsb_w)
	AM_RANGE(0x0ce000, 0x0ce01f) AM_WRITE(moo_prot_w)
	AM_RANGE(0x0d0000, 0x0d001f) AM_DEVREADWRITE8_LEGACY("k053252",k053252_r,k053252_w,0x00ff)					/* CCU regs (ignored) */
	AM_RANGE(0x0d4000, 0x0d4001) AM_WRITE(sound_irq_w)
	AM_RANGE(0x0d600c, 0x0d600d) AM_WRITE(sound_cmd1_w)
	AM_RANGE(0x0d600e, 0x0d600f) AM_WRITE(sound_cmd2_w)
	AM_RANGE(0x0d6014, 0x0d6015) AM_READ(sound_status_r)
	AM_RANGE(0x0d6000, 0x0d601f) AM_RAM							/* sound regs fall through */
	AM_RANGE(0x0d8000, 0x0d8007) AM_DEVWRITE_LEGACY("k056832", k056832_b_word_w)		/* VSCCS regs */
	AM_RANGE(0x0da000, 0x0da001) AM_READ_PORT("P1_P3")
	AM_RANGE(0x0da002, 0x0da003) AM_READ_PORT("P2_P4")
	AM_RANGE(0x0dc000, 0x0dc001) AM_READ_PORT("IN0")
	AM_RANGE(0x0dc002, 0x0dc003) AM_READ_PORT("IN1")
	AM_RANGE(0x0de000, 0x0de001) AM_READWRITE(control2_r, control2_w)
	AM_RANGE(0x100000, 0x17ffff) AM_ROM
	AM_RANGE(0x180000, 0x18ffff) AM_RAM	AM_SHARE("workram")		/* Work RAM */
	AM_RANGE(0x190000, 0x19ffff) AM_RAM AM_SHARE("spriteram")	/* Sprite RAM */
	AM_RANGE(0x1a0000, 0x1a1fff) AM_DEVREADWRITE_LEGACY("k056832", k056832_ram_word_r, k056832_ram_word_w)	/* Graphic planes */
	AM_RANGE(0x1a2000, 0x1a3fff) AM_DEVREADWRITE_LEGACY("k056832", k056832_ram_word_r, k056832_ram_word_w)	/* Graphic planes mirror */
	AM_RANGE(0x1b0000, 0x1b1fff) AM_DEVREAD_LEGACY("k056832", k056832_rom_word_r)	/* Passthrough to tile roms */
	AM_RANGE(0x1c0000, 0x1c1fff) AM_RAM_WRITE(paletteram_xrgb_word_be_w) AM_SHARE("paletteram")
#if MOO_DEBUG
	AM_RANGE(0x0c0000, 0x0c003f) AM_DEVREAD_LEGACY("k056832", k056832_word_r)
	AM_RANGE(0x0c2000, 0x0c2007) AM_DEVREAD_LEGACY("k053246", k053246_reg_word_r)
	AM_RANGE(0x0ca000, 0x0ca01f) AM_DEVREAD_LEGACY("k054338", k054338_word_r)
	AM_RANGE(0x0cc000, 0x0cc01f) AM_DEVREAD_LEGACY("k053251", k053251_lsb_r)
	AM_RANGE(0x0d8000, 0x0d8007) AM_DEVREAD_LEGACY("k056832", k056832_b_word_r)
#endif
ADDRESS_MAP_END

static ADDRESS_MAP_START( moobl_map, AS_PROGRAM, 16, moo_state )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x0c0000, 0x0c003f) AM_DEVWRITE_LEGACY("k056832", k056832_word_w)
	AM_RANGE(0x0c2000, 0x0c2007) AM_DEVWRITE_LEGACY("k053246", k053246_word_w)
	AM_RANGE(0x0c2f00, 0x0c2f01) AM_READNOP						/* heck if I know, but it's polled constantly */
	AM_RANGE(0x0c4000, 0x0c4001) AM_DEVREAD_LEGACY("k053246", k053246_word_r)
	AM_RANGE(0x0ca000, 0x0ca01f) AM_DEVWRITE_LEGACY("k054338", k054338_word_w)       /* K054338 alpha blending engine */
	AM_RANGE(0x0cc000, 0x0cc01f) AM_DEVWRITE_LEGACY("k053251", k053251_lsb_w)
	AM_RANGE(0x0d0000, 0x0d001f) AM_WRITEONLY		            /* CCU regs (ignored) */
	AM_RANGE(0x0d6ffc, 0x0d6ffd) AM_DEVWRITE_LEGACY("oki", moobl_oki_bank_w)
	AM_RANGE(0x0d6ffe, 0x0d6fff) AM_DEVREADWRITE8("oki", okim6295_device, read, write, 0x00ff)
	AM_RANGE(0x0d8000, 0x0d8007) AM_DEVWRITE_LEGACY("k056832", k056832_b_word_w)     /* VSCCS regs */
	AM_RANGE(0x0da000, 0x0da001) AM_READ_PORT("P1_P3")
	AM_RANGE(0x0da002, 0x0da003) AM_READ_PORT("P2_P4")
	AM_RANGE(0x0dc000, 0x0dc001) AM_READ_PORT("IN0")
	AM_RANGE(0x0dc002, 0x0dc003) AM_READ_PORT("IN1")
	AM_RANGE(0x0de000, 0x0de001) AM_READWRITE(control2_r, control2_w)
	AM_RANGE(0x100000, 0x17ffff) AM_ROM
	AM_RANGE(0x180000, 0x18ffff) AM_RAM AM_SHARE("workram")		 /* Work RAM */
	AM_RANGE(0x190000, 0x19ffff) AM_RAM AM_SHARE("spriteram")	 /* Sprite RAM */
	AM_RANGE(0x1a0000, 0x1a1fff) AM_DEVREADWRITE_LEGACY("k056832", k056832_ram_word_r, k056832_ram_word_w) /* Graphic planes */
	AM_RANGE(0x1a2000, 0x1a3fff) AM_DEVREADWRITE_LEGACY("k056832", k056832_ram_word_r, k056832_ram_word_w)	/* Graphic planes mirror */
	AM_RANGE(0x1b0000, 0x1b1fff) AM_DEVREAD_LEGACY("k056832", k056832_rom_word_r)	/* Passthrough to tile roms */
	AM_RANGE(0x1c0000, 0x1c1fff) AM_RAM_WRITE(paletteram_xrgb_word_be_w) AM_SHARE("paletteram")
ADDRESS_MAP_END

static ADDRESS_MAP_START( bucky_map, AS_PROGRAM, 16, moo_state )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x080000, 0x08ffff) AM_RAM
	AM_RANGE(0x090000, 0x09ffff) AM_RAM AM_SHARE("spriteram")	/* Sprite RAM */
	AM_RANGE(0x0a0000, 0x0affff) AM_RAM							/* extra sprite RAM? */
	AM_RANGE(0x0c0000, 0x0c003f) AM_DEVWRITE_LEGACY("k056832", k056832_word_w)
	AM_RANGE(0x0c2000, 0x0c2007) AM_DEVWRITE_LEGACY("k053246", k053246_word_w)
	AM_RANGE(0x0c4000, 0x0c4001) AM_DEVREAD_LEGACY("k053246", k053246_word_r)
	AM_RANGE(0x0ca000, 0x0ca01f) AM_DEVWRITE_LEGACY("k054338", k054338_word_w)		/* K054338 alpha blending engine */
	AM_RANGE(0x0cc000, 0x0cc01f) AM_DEVWRITE_LEGACY("k053251", k053251_lsb_w)
	AM_RANGE(0x0ce000, 0x0ce01f) AM_WRITE(moo_prot_w)
	AM_RANGE(0x0d0000, 0x0d001f) AM_DEVREADWRITE8_LEGACY("k053252",k053252_r,k053252_w,0x00ff)					/* CCU regs (ignored) */
	AM_RANGE(0x0d2000, 0x0d20ff) AM_DEVREADWRITE_LEGACY("k054000", k054000_lsb_r, k054000_lsb_w)
	AM_RANGE(0x0d4000, 0x0d4001) AM_WRITE(sound_irq_w)
	AM_RANGE(0x0d600c, 0x0d600d) AM_WRITE(sound_cmd1_w)
	AM_RANGE(0x0d600e, 0x0d600f) AM_WRITE(sound_cmd2_w)
	AM_RANGE(0x0d6014, 0x0d6015) AM_READ(sound_status_r)
	AM_RANGE(0x0d6000, 0x0d601f) AM_RAM							/* sound regs fall through */
	AM_RANGE(0x0d8000, 0x0d8007) AM_DEVWRITE_LEGACY("k056832", k056832_b_word_w)		/* VSCCS regs */
	AM_RANGE(0x0da000, 0x0da001) AM_READ_PORT("P1_P3")
	AM_RANGE(0x0da002, 0x0da003) AM_READ_PORT("P2_P4")
	AM_RANGE(0x0dc000, 0x0dc001) AM_READ_PORT("IN0")
	AM_RANGE(0x0dc002, 0x0dc003) AM_READ_PORT("IN1")
	AM_RANGE(0x0de000, 0x0de001) AM_READWRITE(control2_r, control2_w)
	AM_RANGE(0x180000, 0x181fff) AM_DEVREADWRITE_LEGACY("k056832", k056832_ram_word_r, k056832_ram_word_w)	/* Graphic planes */
	AM_RANGE(0x182000, 0x183fff) AM_DEVREADWRITE_LEGACY("k056832", k056832_ram_word_r, k056832_ram_word_w)	/* Graphic planes mirror */
	AM_RANGE(0x184000, 0x187fff) AM_RAM							/* extra tile RAM? */
	AM_RANGE(0x190000, 0x191fff) AM_DEVREAD_LEGACY("k056832", k056832_rom_word_r)	/* Passthrough to tile roms */
	AM_RANGE(0x1b0000, 0x1b3fff) AM_RAM_WRITE(paletteram_xrgb_word_be_w) AM_SHARE("paletteram")
	AM_RANGE(0x200000, 0x23ffff) AM_ROM							/* data */
#if MOO_DEBUG
	AM_RANGE(0x0c0000, 0x0c003f) AM_DEVREAD_LEGACY("k056832", k056832_word_r)
	AM_RANGE(0x0c2000, 0x0c2007) AM_DEVREAD_LEGACY("k053246", k053246_reg_word_r)
	AM_RANGE(0x0ca000, 0x0ca01f) AM_DEVREAD_LEGACY("k054338", k054338_word_r)
	AM_RANGE(0x0cc000, 0x0cc01f) AM_DEVREAD_LEGACY("k053251", k053251_lsb_r)
	AM_RANGE(0x0d8000, 0x0d8007) AM_DEVREAD_LEGACY("k056832", k056832_b_word_r)
#endif
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_map, AS_PROGRAM, 8, moo_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0xbfff) AM_ROMBANK("bank1")
	AM_RANGE(0xc000, 0xdfff) AM_RAM
	AM_RANGE(0xe000, 0xe22f) AM_DEVREADWRITE("k054539", k054539_device, read, write)
	AM_RANGE(0xec00, 0xec01) AM_DEVREADWRITE_LEGACY("ymsnd", ym2151_r,ym2151_w)
	AM_RANGE(0xf000, 0xf000) AM_WRITE(soundlatch3_byte_w)
	AM_RANGE(0xf002, 0xf002) AM_READ(soundlatch_byte_r)
	AM_RANGE(0xf003, 0xf003) AM_READ(soundlatch2_byte_r)
	AM_RANGE(0xf800, 0xf800) AM_WRITE(sound_bankswitch_w)
ADDRESS_MAP_END

static INPUT_PORTS_START( moo )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SERVICE3 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE4 )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_READ_LINE_DEVICE_MEMBER("eeprom", eeprom_device, read_bit)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SPECIAL )	/* EEPROM ready (always 1) */
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_SERVICE_NO_TOGGLE(0x08, IP_ACTIVE_LOW)
	PORT_DIPNAME( 0x10, 0x00, "Sound Output")		PORT_DIPLOCATION("SW1:1")
	PORT_DIPSETTING(    0x10, DEF_STR( Mono ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Stereo ) )
	PORT_DIPNAME( 0x20, 0x20, "Coin Mechanism") 		PORT_DIPLOCATION("SW1:2")
	PORT_DIPSETTING(    0x20, "Common")
	PORT_DIPSETTING(    0x00, "Independent")
	PORT_DIPNAME( 0xc0, 0x80, "Number of Players")		PORT_DIPLOCATION("SW1:3,4")
	PORT_DIPSETTING(    0xc0, "2")
	PORT_DIPSETTING(    0x40, "3")
	PORT_DIPSETTING(    0x80, "4")

	PORT_START( "EEPROMOUT" )
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_device, write_bit)
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_device, set_cs_line)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("eeprom", eeprom_device, set_clock_line)

	PORT_START("P1_P3")
	KONAMI16_LSB( 1, IPT_UNKNOWN, IPT_START1 )
	KONAMI16_MSB( 3, IPT_UNKNOWN, IPT_START3 )

	PORT_START("P2_P4")
	KONAMI16_LSB( 2, IPT_UNKNOWN, IPT_START2 )
	KONAMI16_MSB( 4, IPT_UNKNOWN, IPT_START4 )
INPUT_PORTS_END

/* Same as 'moo', but additional "Button 3" for all players */
static INPUT_PORTS_START( bucky )
	PORT_INCLUDE( moo )

	PORT_MODIFY("P1_P3")
	KONAMI16_LSB( 1, IPT_BUTTON3, IPT_START1 )
	KONAMI16_MSB( 3, IPT_BUTTON3, IPT_START3 )

	PORT_MODIFY("P2_P4")
	KONAMI16_LSB( 2, IPT_BUTTON3, IPT_START2 )
	KONAMI16_MSB( 4, IPT_BUTTON3, IPT_START4 )
INPUT_PORTS_END


static MACHINE_START( moo )
{
	moo_state *state = machine.driver_data<moo_state>();

	state->m_maincpu = machine.device("maincpu");
	state->m_audiocpu = machine.device("soundcpu");
	state->m_k054539 = machine.device("k054539");
	state->m_k053246 = machine.device("k053246");
	state->m_k053251 = machine.device("k053251");
	state->m_k056832 = machine.device("k056832");
	state->m_k054338 = machine.device("k054338");

	state->save_item(NAME(state->m_cur_control2));
	state->save_item(NAME(state->m_alpha_enabled));
	state->save_item(NAME(state->m_sprite_colorbase));
	state->save_item(NAME(state->m_layer_colorbase));
	state->save_item(NAME(state->m_layerpri));
	state->save_item(NAME(state->m_protram));

    state->m_dmaend_timer = machine.scheduler().timer_alloc(FUNC(dmaend_callback));
}

static MACHINE_RESET( moo )
{
	moo_state *state = machine.driver_data<moo_state>();
	int i;

	for (i = 0; i < 16; i++)
		state->m_protram[i] = 0;

	for (i = 0; i < 4; i++)
		state->m_layer_colorbase[i] = 0;

	for (i = 0; i < 3; i++)
		state->m_layerpri[i] = 0;

	state->m_cur_control2 = 0;
	state->m_alpha_enabled = 0;
	state->m_sprite_colorbase = 0;
}

static const k056832_interface moo_k056832_intf =
{
	"gfx1", 0,
	K056832_BPP_4,
	1, 0,
	KONAMI_ROM_DEINTERLEAVE_2,
	moo_tile_callback, "none"
};

static const k053247_interface moo_k053247_intf =
{
	"screen",
	"gfx2", 1,
	NORMAL_PLANE_ORDER,
	-48+1, 23,
	KONAMI_ROM_DEINTERLEAVE_4,
	moo_sprite_callback
};

static const k053247_interface bucky_k053247_intf =
{
	"screen",
	"gfx2", 1,
	NORMAL_PLANE_ORDER,
	-48, 23,
	KONAMI_ROM_DEINTERLEAVE_4,
	moo_sprite_callback
};

static const k054338_interface moo_k054338_intf =
{
	"screen",
	0,
	"none"
};

static const k053252_interface moo_k053252_intf =
{
	"screen",
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	40, 16
};

static k054539_interface k054539_config;

static MACHINE_CONFIG_START( moo, moo_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, 16000000)
	MCFG_CPU_PROGRAM_MAP(moo_map)
	MCFG_CPU_VBLANK_INT("screen", moo_interrupt)

	MCFG_CPU_ADD("soundcpu", Z80, 8000000)
	MCFG_CPU_PROGRAM_MAP(sound_map)

	MCFG_MACHINE_START(moo)
	MCFG_MACHINE_RESET(moo)

	MCFG_EEPROM_ADD("eeprom", eeprom_intf)

	MCFG_K053252_ADD("k053252", 16000000/2, moo_k053252_intf)

	/* video hardware */
	MCFG_VIDEO_ATTRIBUTES(VIDEO_HAS_SHADOWS | VIDEO_HAS_HIGHLIGHTS | VIDEO_UPDATE_AFTER_VBLANK)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(1200))	 // should give IRQ4 sufficient time to update scroll registers
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(40, 40+384-1, 16, 16+224-1)
	MCFG_SCREEN_UPDATE_STATIC(moo)

	MCFG_PALETTE_LENGTH(2048)

	MCFG_VIDEO_START(moo)

	MCFG_K053247_ADD("k053246", moo_k053247_intf)
	MCFG_K056832_ADD("k056832", moo_k056832_intf)
	MCFG_K053251_ADD("k053251")
	MCFG_K054338_ADD("k054338", moo_k054338_intf)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_SOUND_ADD("ymsnd", YM2151, 4000000)
	MCFG_SOUND_ROUTE(0, "lspeaker", 0.50)
	MCFG_SOUND_ROUTE(1, "rspeaker", 0.50)

	MCFG_K054539_ADD("k054539", 48000, k054539_config)
	MCFG_SOUND_ROUTE(0, "lspeaker", 0.75)
	MCFG_SOUND_ROUTE(1, "rspeaker", 0.75)
MACHINE_CONFIG_END

static MACHINE_CONFIG_START( moobl, moo_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, 16100000)
	MCFG_CPU_PROGRAM_MAP(moobl_map)
	MCFG_CPU_VBLANK_INT("screen", moobl_interrupt)

	MCFG_MACHINE_START(moo)
	MCFG_MACHINE_RESET(moo)

	MCFG_EEPROM_ADD("eeprom", eeprom_intf)

	/* video hardware */
	MCFG_VIDEO_ATTRIBUTES(VIDEO_HAS_SHADOWS | VIDEO_HAS_HIGHLIGHTS | VIDEO_UPDATE_AFTER_VBLANK)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(1200)) // should give IRQ4 sufficient time to update scroll registers
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(40, 40+384-1, 16, 16+224-1)
	MCFG_SCREEN_UPDATE_STATIC(moo)

	MCFG_PALETTE_LENGTH(2048)

	MCFG_VIDEO_START(moo)

	MCFG_K053247_ADD("k053246", moo_k053247_intf)
	MCFG_K056832_ADD("k056832", moo_k056832_intf)
	MCFG_K053251_ADD("k053251")
	MCFG_K054338_ADD("k054338", moo_k054338_intf)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_OKIM6295_ADD("oki", 1056000, OKIM6295_PIN7_HIGH) // clock frequency & pin 7 not verified
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 1.0)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( bucky, moo )

	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(bucky_map)

	MCFG_K054000_ADD("k054000")

	MCFG_DEVICE_REMOVE("k053246")
	MCFG_K053247_ADD("k053246", bucky_k053247_intf)		// diff x offset

	/* video hardware */
	MCFG_PALETTE_LENGTH(4096)
MACHINE_CONFIG_END


ROM_START( moomesa ) /* Version EA */
	ROM_REGION( 0x180000, "maincpu", 0 )
	/* main program */
	ROM_LOAD16_BYTE( "151b01.q5",   0x000000, 0x40000, CRC(fb2fa298) SHA1(f03b24681a2b329ba797fd2780ac9a3cf862ebcb) )	/* B */
	ROM_LOAD16_BYTE( "151eab02.q6", 0x000001, 0x40000, CRC(37b30c01) SHA1(cb91739097a4a36f8f8d92998d822ffc851e1279) )	/* EAB */

	/* data */
	ROM_LOAD16_BYTE( "151a03.t5", 0x100000, 0x40000, CRC(c896d3ea) SHA1(ea83c63e2c3dbc4f1e1d49f1852a78ffc1f0ea4b) )
	ROM_LOAD16_BYTE( "151a04.t6", 0x100001, 0x40000, CRC(3b24706a) SHA1(c2a77944284e35ff57f0774fa7b67e53d3b63e1f) )

	ROM_REGION( 0x050000, "soundcpu", 0 )
	/* Z80 sound program */
	ROM_LOAD( "151a07.f5",  0x000000, 0x040000, CRC(cde247fc) SHA1(cdee0228db55d53ae43d7cd2d9001dadd20c2c61) )
	ROM_RELOAD(             0x010000, 0x040000 )

	ROM_REGION( 0x200000, "gfx1", 0 )
	/* tilemaps */
	ROM_LOAD( "151a05.t8",  0x000000, 0x100000, CRC(bc616249) SHA1(58c1f1a03ce9bead8f79d12ce4b2d342432b24b5) )
	ROM_LOAD( "151a06.t10", 0x100000, 0x100000, CRC(38dbcac1) SHA1(c357779733921695b20ac586db5b475f5b2b8f4c) )

	ROM_REGION( 0x800000, "gfx2", 0 )
	/* sprites */
	ROM_LOAD( "151a10.b8",  0x000000, 0x200000, CRC(376c64f1) SHA1(eb69c5a27f9795e28f04a503955132f0a9e4de12) )
	ROM_LOAD( "151a11.a8",  0x200000, 0x200000, CRC(e7f49225) SHA1(1255b214f29b6507540dad5892c60a7ae2aafc5c) )
	ROM_LOAD( "151a12.b10", 0x400000, 0x200000, CRC(4978555f) SHA1(d9871f21d0c8a512b408e137e2e80e9392c2bf6f) )
	ROM_LOAD( "151a13.a10", 0x600000, 0x200000, CRC(4771f525) SHA1(218d86b6230919b5db0304dac00513eb6b27ba9a) )

	ROM_REGION( 0x200000, "k054539", 0 )
	/* K054539 samples */
	ROM_LOAD( "151a08.b6",  0x000000, 0x200000, CRC(962251d7) SHA1(32dccf515d2ca8eeffb45cada3dcc60089991b77) )

	ROM_REGION( 0x80, "eeprom", 0 ) // default eeprom to prevent game booting upside down with error
	ROM_LOAD( "moomesa.nv", 0x0000, 0x080, CRC(7bd904a8) SHA1(8747c5c62d1832e290be8ace73c61b1f228c0bec) )
ROM_END

ROM_START( moomesauac ) /* Version UA */
	ROM_REGION( 0x180000, "maincpu", 0 )
	/* main program */
	ROM_LOAD16_BYTE( "151c01.q5",   0x000000, 0x40000, CRC(10555732) SHA1(b67cb756c250ddd6f3291683b3f3449e13a2ee83) )	/* C */
	ROM_LOAD16_BYTE( "151uac02.q6", 0x000001, 0x40000, CRC(52ae87b0) SHA1(552d41a2ddd040f92c6a3cfdc07f9d6e751ac9c1) )	/* UAC */

	/* data */
	ROM_LOAD16_BYTE( "151a03.t5", 0x100000, 0x40000, CRC(c896d3ea) SHA1(ea83c63e2c3dbc4f1e1d49f1852a78ffc1f0ea4b) )
	ROM_LOAD16_BYTE( "151a04.t6", 0x100001, 0x40000, CRC(3b24706a) SHA1(c2a77944284e35ff57f0774fa7b67e53d3b63e1f) )

	ROM_REGION( 0x050000, "soundcpu", 0 )
	/* Z80 sound program */
	ROM_LOAD( "151a07.f5",  0x000000, 0x040000, CRC(cde247fc) SHA1(cdee0228db55d53ae43d7cd2d9001dadd20c2c61) )
	ROM_RELOAD(             0x010000, 0x040000 )

	ROM_REGION( 0x200000, "gfx1", 0 )
	/* tilemaps */
	ROM_LOAD( "151a05.t8",  0x000000, 0x100000, CRC(bc616249) SHA1(58c1f1a03ce9bead8f79d12ce4b2d342432b24b5) )
	ROM_LOAD( "151a06.t10", 0x100000, 0x100000, CRC(38dbcac1) SHA1(c357779733921695b20ac586db5b475f5b2b8f4c) )

	ROM_REGION( 0x800000, "gfx2", 0 )
	/* sprites */
	ROM_LOAD( "151a10.b8",  0x000000, 0x200000, CRC(376c64f1) SHA1(eb69c5a27f9795e28f04a503955132f0a9e4de12) )
	ROM_LOAD( "151a11.a8",  0x200000, 0x200000, CRC(e7f49225) SHA1(1255b214f29b6507540dad5892c60a7ae2aafc5c) )
	ROM_LOAD( "151a12.b10", 0x400000, 0x200000, CRC(4978555f) SHA1(d9871f21d0c8a512b408e137e2e80e9392c2bf6f) )
	ROM_LOAD( "151a13.a10", 0x600000, 0x200000, CRC(4771f525) SHA1(218d86b6230919b5db0304dac00513eb6b27ba9a) )

	ROM_REGION( 0x200000, "k054539", 0 )
	/* K054539 samples */
	ROM_LOAD( "151a08.b6",  0x000000, 0x200000, CRC(962251d7) SHA1(32dccf515d2ca8eeffb45cada3dcc60089991b77) )

	ROM_REGION( 0x80, "eeprom", 0 ) // default eeprom to prevent game booting upside down with error
	ROM_LOAD( "moomesauac.nv", 0x0000, 0x080, CRC(a5cb137a) SHA1(119df859d6b5c366481305b1433eea0deadc3fa9) )
ROM_END

ROM_START( moomesauab ) /* Version UA */
	ROM_REGION( 0x180000, "maincpu", 0 )
	/* main program */
	ROM_LOAD16_BYTE( "151b01.q5",   0x000000, 0x40000, CRC(fb2fa298) SHA1(f03b24681a2b329ba797fd2780ac9a3cf862ebcb) )	/* B */
	ROM_LOAD16_BYTE( "151uab02.q6", 0x000001, 0x40000, CRC(3d9f4d59) SHA1(db47044bd4935fce94ec659242c9819c30eb6d0f) )	/* UAB */

	/* data */
	ROM_LOAD16_BYTE( "151a03.t5", 0x100000, 0x40000, CRC(c896d3ea) SHA1(ea83c63e2c3dbc4f1e1d49f1852a78ffc1f0ea4b) )
	ROM_LOAD16_BYTE( "151a04.t6", 0x100001, 0x40000, CRC(3b24706a) SHA1(c2a77944284e35ff57f0774fa7b67e53d3b63e1f) )

	ROM_REGION( 0x050000, "soundcpu", 0 )
	/* Z80 sound program */
	ROM_LOAD( "151a07.f5",  0x000000, 0x040000, CRC(cde247fc) SHA1(cdee0228db55d53ae43d7cd2d9001dadd20c2c61) )
	ROM_RELOAD(             0x010000, 0x040000 )

	ROM_REGION( 0x200000, "gfx1", 0 )
	/* tilemaps */
	ROM_LOAD( "151a05.t8",  0x000000, 0x100000, CRC(bc616249) SHA1(58c1f1a03ce9bead8f79d12ce4b2d342432b24b5) )
	ROM_LOAD( "151a06.t10", 0x100000, 0x100000, CRC(38dbcac1) SHA1(c357779733921695b20ac586db5b475f5b2b8f4c) )

	ROM_REGION( 0x800000, "gfx2", 0 )
	/* sprites */
	ROM_LOAD( "151a10.b8",  0x000000, 0x200000, CRC(376c64f1) SHA1(eb69c5a27f9795e28f04a503955132f0a9e4de12) )
	ROM_LOAD( "151a11.a8",  0x200000, 0x200000, CRC(e7f49225) SHA1(1255b214f29b6507540dad5892c60a7ae2aafc5c) )
	ROM_LOAD( "151a12.b10", 0x400000, 0x200000, CRC(4978555f) SHA1(d9871f21d0c8a512b408e137e2e80e9392c2bf6f) )
	ROM_LOAD( "151a13.a10", 0x600000, 0x200000, CRC(4771f525) SHA1(218d86b6230919b5db0304dac00513eb6b27ba9a) )

	ROM_REGION( 0x200000, "k054539", 0 )
	/* K054539 samples */
	ROM_LOAD( "151a08.b6",  0x000000, 0x200000, CRC(962251d7) SHA1(32dccf515d2ca8eeffb45cada3dcc60089991b77) )

	ROM_REGION( 0x80, "eeprom", 0 ) // default eeprom to prevent game booting upside down with error
	ROM_LOAD( "moomesauab.nv", 0x0000, 0x080, CRC(a5cb137a) SHA1(119df859d6b5c366481305b1433eea0deadc3fa9) )
ROM_END

ROM_START( moomesaaab ) /* Version AA */
	ROM_REGION( 0x180000, "maincpu", 0 )
	/* main program */
	ROM_LOAD16_BYTE( "151b01.q5",   0x000000, 0x40000, CRC(fb2fa298) SHA1(f03b24681a2b329ba797fd2780ac9a3cf862ebcb) )	/* B */
	ROM_LOAD16_BYTE( "151aab02.q6", 0x000001, 0x40000, CRC(2162d593) SHA1(a6cfe4a57b3f22b2aa0f04f91acefe3b7bea9e76) )	/* AAB */

	/* data */
	ROM_LOAD16_BYTE( "151a03.t5", 0x100000, 0x40000, CRC(c896d3ea) SHA1(ea83c63e2c3dbc4f1e1d49f1852a78ffc1f0ea4b) )
	ROM_LOAD16_BYTE( "151a04.t6", 0x100001, 0x40000, CRC(3b24706a) SHA1(c2a77944284e35ff57f0774fa7b67e53d3b63e1f) )

	ROM_REGION( 0x050000, "soundcpu", 0 )
	/* Z80 sound program */
	ROM_LOAD( "151a07.f5",  0x000000, 0x040000, CRC(cde247fc) SHA1(cdee0228db55d53ae43d7cd2d9001dadd20c2c61) )
	ROM_RELOAD(             0x010000, 0x040000 )

	ROM_REGION( 0x200000, "gfx1", 0 )
	/* tilemaps */
	ROM_LOAD( "151a05.t8",  0x000000, 0x100000, CRC(bc616249) SHA1(58c1f1a03ce9bead8f79d12ce4b2d342432b24b5) )
	ROM_LOAD( "151a06.t10", 0x100000, 0x100000, CRC(38dbcac1) SHA1(c357779733921695b20ac586db5b475f5b2b8f4c) )

	ROM_REGION( 0x800000, "gfx2", 0 )
	/* sprites */
	ROM_LOAD( "151a10.b8",  0x000000, 0x200000, CRC(376c64f1) SHA1(eb69c5a27f9795e28f04a503955132f0a9e4de12) )
	ROM_LOAD( "151a11.a8",  0x200000, 0x200000, CRC(e7f49225) SHA1(1255b214f29b6507540dad5892c60a7ae2aafc5c) )
	ROM_LOAD( "151a12.b10", 0x400000, 0x200000, CRC(4978555f) SHA1(d9871f21d0c8a512b408e137e2e80e9392c2bf6f) )
	ROM_LOAD( "151a13.a10", 0x600000, 0x200000, CRC(4771f525) SHA1(218d86b6230919b5db0304dac00513eb6b27ba9a) )

	ROM_REGION( 0x200000, "k054539", 0 )
	/* K054539 samples */
	ROM_LOAD( "151a08.b6",  0x000000, 0x200000, CRC(962251d7) SHA1(32dccf515d2ca8eeffb45cada3dcc60089991b77) )

	ROM_REGION( 0x80, "eeprom", 0 ) // default eeprom to prevent game booting upside down with error
	ROM_LOAD( "moomesaaab.nv", 0x0000, 0x080, CRC(7bd904a8) SHA1(8747c5c62d1832e290be8ace73c61b1f228c0bec) )
ROM_END

ROM_START( bucky ) /* Version EA */
	ROM_REGION( 0x240000, "maincpu", 0 )
	/* main program */
	ROM_LOAD16_BYTE( "173eab01.q5", 0x000000, 0x40000, CRC(7785ac8a) SHA1(ef78d14f54d3a0b724b9702a18c67891e2d366a7) )	/* EAB */
	ROM_LOAD16_BYTE( "173eab02.q6", 0x000001, 0x40000, CRC(9b45f122) SHA1(325af1612e6f90ef9ae9353c43dc645be1f3465c) )	/* EAB */

	/* data */
	ROM_LOAD16_BYTE( "173a03.t5", 0x200000, 0x20000, CRC(cd724026) SHA1(525445499604b713da4d8bc0a88e428654ceab95) )
	ROM_LOAD16_BYTE( "173a04.t6", 0x200001, 0x20000, CRC(7dd54d6f) SHA1(b0ee8ec445b92254bca881eefd4449972fed506a) )

	ROM_REGION( 0x050000, "soundcpu", 0 )
	/* Z80 sound program */
	ROM_LOAD( "173a07.f5",  0x000000, 0x040000, CRC(4cdaee71) SHA1(bdc05d4475415f6fac65d7cdbc48df398e57845e) )
	ROM_RELOAD(             0x010000, 0x040000 )

	ROM_REGION( 0x200000, "gfx1", 0 )
	/* tilemaps */
	ROM_LOAD( "173a05.t8",  0x000000, 0x100000, CRC(d14333b4) SHA1(d1a15ead2d156e1fceca0bf202ab3962411caf11) )
	ROM_LOAD( "173a06.t10", 0x100000, 0x100000, CRC(6541a34f) SHA1(15cf481498e3b7e0b2f7bfe5434121cc3bd65662) )

	ROM_REGION( 0x800000, "gfx2", 0 )
	/* sprites */
	ROM_LOAD( "173a10.b8",  0x000000, 0x200000, CRC(42fb0a0c) SHA1(d68c932cfabdec7896698b433525fe47ef4698d0) )
	ROM_LOAD( "173a11.a8",  0x200000, 0x200000, CRC(b0d747c4) SHA1(0cf1ee1b9a35ded31a81c321df2a076f7b588971) )
	ROM_LOAD( "173a12.b10", 0x400000, 0x200000, CRC(0fc2ad24) SHA1(6eda1043ee1266b8ba938a03a90bc7787210a936) )
	ROM_LOAD( "173a13.a10", 0x600000, 0x200000, CRC(4cf85439) SHA1(8c298bf0e659a830a1830a1180f4ce71215ade45) )

	ROM_REGION( 0x400000, "k054539", 0 )
	/* K054539 samples */
	ROM_LOAD( "173a08.b6",  0x000000, 0x200000, CRC(dcdded95) SHA1(8eeb546a0b60a35a6dce36c5ee872e6c93c577c9) )
	ROM_LOAD( "173a09.a6",  0x200000, 0x200000, CRC(c93697c4) SHA1(0528a604868267a30d281b822c187df118566691) )

	ROM_REGION( 0x80, "eeprom", 0 ) // default eeprom to prevent game booting upside down with error
	ROM_LOAD( "bucky.nv", 0x0000, 0x080, CRC(6a5986f3) SHA1(3efddeed261b09031c582e12318f00c2cbb214ea) )
ROM_END

ROM_START( buckyuab ) /* Version UA */
	ROM_REGION( 0x240000, "maincpu", 0 )
	/* main program */
	ROM_LOAD16_BYTE( "173uab01.q5", 0x000000, 0x40000, CRC(dcaecca0) SHA1(c41847c9d89cdaf7cfa81ad9cc018c32592a882f) )	/* UAB */
	ROM_LOAD16_BYTE( "173uab02.q6", 0x000001, 0x40000, CRC(e3c856a6) SHA1(33cc8a29643e44b31ee280015c0c994bed72a0e3) )	/* UAB */

	/* data */
	ROM_LOAD16_BYTE( "173a03.t5", 0x200000, 0x20000, CRC(cd724026) SHA1(525445499604b713da4d8bc0a88e428654ceab95) )
	ROM_LOAD16_BYTE( "173a04.t6", 0x200001, 0x20000, CRC(7dd54d6f) SHA1(b0ee8ec445b92254bca881eefd4449972fed506a) )

	ROM_REGION( 0x050000, "soundcpu", 0 )
	/* Z80 sound program */
	ROM_LOAD( "173a07.f5",  0x000000, 0x040000, CRC(4cdaee71) SHA1(bdc05d4475415f6fac65d7cdbc48df398e57845e) )
	ROM_RELOAD(             0x010000, 0x040000 )

	ROM_REGION( 0x200000, "gfx1", 0 )
	/* tilemaps */
	ROM_LOAD( "173a05.t8",  0x000000, 0x100000, CRC(d14333b4) SHA1(d1a15ead2d156e1fceca0bf202ab3962411caf11) )
	ROM_LOAD( "173a06.t10", 0x100000, 0x100000, CRC(6541a34f) SHA1(15cf481498e3b7e0b2f7bfe5434121cc3bd65662) )

	ROM_REGION( 0x800000, "gfx2", 0 )
	/* sprites */
	ROM_LOAD( "173a10.b8",  0x000000, 0x200000, CRC(42fb0a0c) SHA1(d68c932cfabdec7896698b433525fe47ef4698d0) )
	ROM_LOAD( "173a11.a8",  0x200000, 0x200000, CRC(b0d747c4) SHA1(0cf1ee1b9a35ded31a81c321df2a076f7b588971) )
	ROM_LOAD( "173a12.b10", 0x400000, 0x200000, CRC(0fc2ad24) SHA1(6eda1043ee1266b8ba938a03a90bc7787210a936) )
	ROM_LOAD( "173a13.a10", 0x600000, 0x200000, CRC(4cf85439) SHA1(8c298bf0e659a830a1830a1180f4ce71215ade45) )

	ROM_REGION( 0x400000, "k054539", 0 )
	/* K054539 samples */
	ROM_LOAD( "173a08.b6",  0x000000, 0x200000, CRC(dcdded95) SHA1(8eeb546a0b60a35a6dce36c5ee872e6c93c577c9) )
	ROM_LOAD( "173a09.a6",  0x200000, 0x200000, CRC(c93697c4) SHA1(0528a604868267a30d281b822c187df118566691) )

	ROM_REGION( 0x80, "eeprom", 0 ) // default eeprom to prevent game booting upside down with error
	ROM_LOAD( "buckyuab.nv", 0x0000, 0x080, CRC(a5cb137a) SHA1(119df859d6b5c366481305b1433eea0deadc3fa9) )
ROM_END

ROM_START( buckyaab ) /* Version AA */
	ROM_REGION( 0x240000, "maincpu", 0 )
	/* main program */
	ROM_LOAD16_BYTE( "173aab01.q5", 0x000000, 0x40000, CRC(9193e89f) SHA1(574d6eb4097cd10c3dea99060ee09f220d41f1dc) )	/* AAB */
	ROM_LOAD16_BYTE( "173aab02.q6", 0x000001, 0x40000, CRC(2567f3eb) SHA1(ccdb2a4b3ad1464f70d1442df8a3a7a7e34f6cd7) )	/* AAB */

	/* data */
	ROM_LOAD16_BYTE( "173a03.t5", 0x200000, 0x20000, CRC(cd724026) SHA1(525445499604b713da4d8bc0a88e428654ceab95) )
	ROM_LOAD16_BYTE( "173a04.t6", 0x200001, 0x20000, CRC(7dd54d6f) SHA1(b0ee8ec445b92254bca881eefd4449972fed506a) )

	ROM_REGION( 0x050000, "soundcpu", 0 )
	/* Z80 sound program */
	ROM_LOAD( "173a07.f5",  0x000000, 0x040000, CRC(4cdaee71) SHA1(bdc05d4475415f6fac65d7cdbc48df398e57845e) )
	ROM_RELOAD(             0x010000, 0x040000 )

	ROM_REGION( 0x200000, "gfx1", 0 )
	/* tilemaps */
	ROM_LOAD( "173a05.t8",  0x000000, 0x100000, CRC(d14333b4) SHA1(d1a15ead2d156e1fceca0bf202ab3962411caf11) )
	ROM_LOAD( "173a06.t10", 0x100000, 0x100000, CRC(6541a34f) SHA1(15cf481498e3b7e0b2f7bfe5434121cc3bd65662) )

	ROM_REGION( 0x800000, "gfx2", 0 )
	/* sprites */
	ROM_LOAD( "173a10.b8",  0x000000, 0x200000, CRC(42fb0a0c) SHA1(d68c932cfabdec7896698b433525fe47ef4698d0) )
	ROM_LOAD( "173a11.a8",  0x200000, 0x200000, CRC(b0d747c4) SHA1(0cf1ee1b9a35ded31a81c321df2a076f7b588971) )
	ROM_LOAD( "173a12.b10", 0x400000, 0x200000, CRC(0fc2ad24) SHA1(6eda1043ee1266b8ba938a03a90bc7787210a936) )
	ROM_LOAD( "173a13.a10", 0x600000, 0x200000, CRC(4cf85439) SHA1(8c298bf0e659a830a1830a1180f4ce71215ade45) )

	ROM_REGION( 0x400000, "k054539", 0 )
	/* K054539 samples */
	ROM_LOAD( "173a08.b6",  0x000000, 0x200000, CRC(dcdded95) SHA1(8eeb546a0b60a35a6dce36c5ee872e6c93c577c9) )
	ROM_LOAD( "173a09.a6",  0x200000, 0x200000, CRC(c93697c4) SHA1(0528a604868267a30d281b822c187df118566691) )

	ROM_REGION( 0x80, "eeprom", 0 ) // default eeprom to prevent game booting upside down with error
	ROM_LOAD( "buckyaab.nv", 0x0000, 0x080, CRC(6a5986f3) SHA1(3efddeed261b09031c582e12318f00c2cbb214ea) )
ROM_END


ROM_START( moomesabl )
	ROM_REGION( 0x180000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "moo03.rom", 0x000000, 0x80000, CRC(fed6a1cb) SHA1(be58e266973930d643b5e15dcc974a82e1a3ae35) )
	ROM_LOAD16_WORD_SWAP( "moo04.rom", 0x100000, 0x80000, CRC(ec45892a) SHA1(594330cbbfbca87e61ddf519e565018b6eaf5a20) )

	ROM_REGION( 0x100000, "user1", 0 )
	ROM_LOAD16_WORD_SWAP( "moo03.rom", 0x000000, 0x80000, CRC(fed6a1cb) SHA1(be58e266973930d643b5e15dcc974a82e1a3ae35) )
	ROM_LOAD16_WORD_SWAP( "moo04.rom", 0x080000, 0x80000, CRC(ec45892a) SHA1(594330cbbfbca87e61ddf519e565018b6eaf5a20) )

	ROM_REGION( 0x200000, "gfx1", 0 )
	ROM_LOAD( "moo05.rom", 0x000000, 0x080000, CRC(8c045f9c) SHA1(cde81a722a4bc2efac09a26d7e300664059ec7bb) )
	ROM_LOAD( "moo06.rom", 0x080000, 0x080000, CRC(1261aa89) SHA1(b600916911bc0d8b6348e2ad4a16ed1a1c528261) )
	ROM_LOAD( "moo07.rom", 0x100000, 0x080000, CRC(b9e29f50) SHA1(c2af095df0af45064d49210085370425b319b82b) )
	ROM_LOAD( "moo08.rom", 0x180000, 0x080000, CRC(e6937229) SHA1(089b3d4af33e8d8fbc1f3abb81e047a7a590567c) )

	// sprites from bootleg not included in dump, taken from original game
	ROM_REGION( 0x800000, "gfx2", 0 )
	ROM_LOAD( "151a10", 0x000000, 0x200000, CRC(376c64f1) SHA1(eb69c5a27f9795e28f04a503955132f0a9e4de12) )
	ROM_LOAD( "151a11", 0x200000, 0x200000, CRC(e7f49225) SHA1(1255b214f29b6507540dad5892c60a7ae2aafc5c) )
	ROM_LOAD( "151a12", 0x400000, 0x200000, CRC(4978555f) SHA1(d9871f21d0c8a512b408e137e2e80e9392c2bf6f) )
	ROM_LOAD( "151a13", 0x600000, 0x200000, CRC(4771f525) SHA1(218d86b6230919b5db0304dac00513eb6b27ba9a) )

	ROM_REGION( 0x340000, "oki", 0 )
	ROM_LOAD( "moo01.rom", 0x000000, 0x040000, CRC(3311338a) SHA1(c0b5cd16f0275b5b93a2ea4fc64013c848c5fa43) )//bank 0 lo & hi
	ROM_CONTINUE(          0x040000+0x30000, 0x010000)//bank 1 hi
	ROM_CONTINUE(          0x080000+0x30000, 0x010000)//bank 2 hi
	ROM_CONTINUE(          0x0c0000+0x30000, 0x010000)//bank 3 hi
	ROM_CONTINUE(          0x100000+0x30000, 0x010000)//bank 4 hi
	ROM_RELOAD(            0x040000, 0x30000 )//bank 1 lo
	ROM_RELOAD(            0x080000, 0x30000 )//bank 2 lo
	ROM_RELOAD(            0x0c0000, 0x30000 )//bank 3 lo
	ROM_RELOAD(            0x100000, 0x30000 )//bank 4 lo
	ROM_RELOAD(            0x140000, 0x30000 )//bank 5 lo
	ROM_RELOAD(            0x180000, 0x30000 )//bank 6 lo
	ROM_RELOAD(            0x1c0000, 0x30000 )//bank 7 lo
	ROM_RELOAD(            0x200000, 0x30000 )//bank 8 lo
	ROM_RELOAD(            0x240000, 0x30000 )//bank 9 lo
	ROM_RELOAD(            0x280000, 0x30000 )//bank a lo
	ROM_RELOAD(            0x2c0000, 0x30000 )//bank b lo
	ROM_RELOAD(            0x300000, 0x30000 )//bank c lo

	ROM_LOAD( "moo02.rom", 0x140000+0x30000, 0x010000, CRC(2cf3a7c6) SHA1(06f495ba8250b34c32569d49c8b84e6edef562d3) )//bank 5 hi
	ROM_CONTINUE(          0x180000+0x30000, 0x010000)//bank 6 hi
	ROM_CONTINUE(          0x1c0000+0x30000, 0x010000)//bank 7 hi
	ROM_CONTINUE(          0x200000+0x30000, 0x010000)//bank 8 hi
	ROM_CONTINUE(          0x240000+0x30000, 0x010000)//bank 9 hi
	ROM_CONTINUE(          0x280000+0x30000, 0x010000)//bank a hi
	ROM_CONTINUE(          0x2c0000+0x30000, 0x010000)//bank b hi
	ROM_CONTINUE(          0x300000+0x30000, 0x010000)//bank c hi

	ROM_REGION( 0x80, "eeprom", 0 ) // default eeprom to prevent game booting upside down with error
	ROM_LOAD( "moo.nv", 0x0000, 0x080, CRC(7bd904a8) SHA1(8747c5c62d1832e290be8ace73c61b1f228c0bec) )
ROM_END

static DRIVER_INIT( moo )
{
	moo_state *state = machine.driver_data<moo_state>();
	state->m_game_type = (!strcmp(machine.system().name, "bucky") || !strcmp(machine.system().name, "buckyua"));
}


GAME( 1992, moomesa,    0,       moo,     moo,     moo,      ROT0, "Konami", "Wild West C.O.W.-Boys of Moo Mesa (ver EAB)", GAME_IMPERFECT_GRAPHICS | GAME_SUPPORTS_SAVE )
GAME( 1992, moomesauac, moomesa, moo,     moo,     moo,      ROT0, "Konami", "Wild West C.O.W.-Boys of Moo Mesa (ver UAC)", GAME_IMPERFECT_GRAPHICS | GAME_SUPPORTS_SAVE )
GAME( 1992, moomesauab, moomesa, moo,     moo,     moo,      ROT0, "Konami", "Wild West C.O.W.-Boys of Moo Mesa (ver UAB)", GAME_IMPERFECT_GRAPHICS | GAME_SUPPORTS_SAVE )
GAME( 1992, moomesaaab, moomesa, moo,     moo,     moo,      ROT0, "Konami", "Wild West C.O.W.-Boys of Moo Mesa (ver AAB)", GAME_IMPERFECT_GRAPHICS | GAME_SUPPORTS_SAVE )
GAME( 1992, moomesabl,  moomesa, moobl,   moo,     moo,      ROT0, "bootleg", "Wild West C.O.W.-Boys of Moo Mesa (bootleg)", GAME_NOT_WORKING | GAME_SUPPORTS_SAVE )	// based on Version AA
GAME( 1992, bucky,      0,       bucky,   bucky,   moo,      ROT0, "Konami", "Bucky O'Hare (ver EAB)", GAME_SUPPORTS_SAVE )
GAME( 1992, buckyuab,   bucky,   bucky,   bucky,   moo,      ROT0, "Konami", "Bucky O'Hare (ver UAB)", GAME_SUPPORTS_SAVE )
GAME( 1992, buckyaab,   bucky,   bucky,   bucky,   moo,      ROT0, "Konami", "Bucky O'Hare (ver AAB)", GAME_SUPPORTS_SAVE )
