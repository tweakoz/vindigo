/* Double Wing */

/*

the most protected of the DE102 games?

Protection TODO:
- 3rd Boss (with Red Plane) causes a crash to the main CPU if you don't beat him on time;
- What is the $330 related for? Appears to be read when you collect a power-up.
- Check the remaining unmapped read/writes effect;
- Boss BGM might be wrong / variable;
- Haven't yet checked if bonus life and difficulty DIP-SW are rightly tested;
- Demo Sounds DIP-SW doesn't work?
- Clean-up the whole routine;


-- Dip locations verified with Japanese manual
*/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "cpu/m68000/m68000.h"
#include "includes/decocrpt.h"
#include "sound/2151intf.h"
#include "sound/okim6295.h"
#include "video/deco16ic.h"
#include "video/decospr.h"
#include "machine/deco104.h"

class dblewing_state : public driver_device
{
public:
	dblewing_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_deco104(*this, "ioprot104"),
		m_pf1_rowscroll(*this, "pf1_rowscroll"),
		m_pf2_rowscroll(*this, "pf2_rowscroll"),
		m_spriteram(*this, "spriteram"),
		m_sprgen(*this, "spritegen"),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_deco_tilegen1(*this, "tilegen1")
	{ }
	optional_device<deco104_device> m_deco104;
	/* memory pointers */
	required_shared_ptr<UINT16> m_pf1_rowscroll;
	required_shared_ptr<UINT16> m_pf2_rowscroll;
	required_shared_ptr<UINT16> m_spriteram;
	optional_device<decospr_device> m_sprgen;


	/* misc */
	UINT8 m_sound_irq;

	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	required_device<deco16ic_device> m_deco_tilegen1;
	DECLARE_WRITE_LINE_MEMBER(sound_irq);
	DECLARE_READ16_MEMBER(dblewing_prot_r);
	DECLARE_WRITE16_MEMBER(dblewing_prot_w);
	DECLARE_READ8_MEMBER(irq_latch_r);
	DECLARE_DRIVER_INIT(dblewing);
	virtual void machine_start();
	virtual void machine_reset();
	UINT32 screen_update_dblewing(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
};

UINT16 dblwings_pri_callback(UINT16 x)
{
	UINT16 pri = (x & 0xc000); // 2 bits or 1?
	switch (pri & 0xc000)
	{
		case 0x0000: pri = 0; break;
		case 0x4000: pri = 0xf0; break;
		case 0x8000: pri = 0xf0 | 0xcc; break;
		case 0xc000: pri = 0xf0 | 0xcc; break; /*  or 0xf0|0xcc|0xaa ? */
	}

	return pri;
}



UINT32 dblewing_state::screen_update_dblewing(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	address_space &space = generic_space();
	UINT16 flip = m_deco_tilegen1->pf_control_r(space, 0, 0xffff);

	flip_screen_set(BIT(flip, 7));
	m_deco_tilegen1->pf_update(m_pf1_rowscroll, m_pf2_rowscroll);

	bitmap.fill(0, cliprect); /* not Confirmed */
	machine().priority_bitmap.fill(0);

	m_deco_tilegen1->tilemap_2_draw(bitmap, cliprect, 0, 2);
	m_deco_tilegen1->tilemap_1_draw(bitmap, cliprect, 0, 4);
	m_sprgen->draw_sprites(bitmap, cliprect, m_spriteram, 0x400);
	return 0;
}


static ADDRESS_MAP_START( dblewing_map, AS_PROGRAM, 16, dblewing_state )
	AM_RANGE(0x000000, 0x07ffff) AM_ROM

	AM_RANGE(0x100000, 0x100fff) AM_DEVREADWRITE("tilegen1", deco16ic_device, pf1_data_r, pf1_data_w)
	AM_RANGE(0x102000, 0x102fff) AM_DEVREADWRITE("tilegen1", deco16ic_device, pf2_data_r, pf2_data_w)
	AM_RANGE(0x104000, 0x104fff) AM_RAM AM_SHARE("pf1_rowscroll")
	AM_RANGE(0x106000, 0x106fff) AM_RAM AM_SHARE("pf2_rowscroll")

	AM_RANGE(0x280000, 0x2807ff) AM_DEVREADWRITE("ioprot104", deco104_device, dblewing_prot_r, dblewing_prot_w)


	AM_RANGE(0x284000, 0x284001) AM_RAM
	AM_RANGE(0x288000, 0x288001) AM_RAM
	AM_RANGE(0x28c000, 0x28c00f) AM_RAM_DEVWRITE("tilegen1", deco16ic_device, pf_control_w)
	AM_RANGE(0x300000, 0x3007ff) AM_RAM AM_SHARE("spriteram")
	AM_RANGE(0x320000, 0x3207ff) AM_RAM_WRITE(paletteram_xxxxBBBBGGGGRRRR_word_w) AM_SHARE("paletteram")
	AM_RANGE(0xff0000, 0xff3fff) AM_MIRROR(0xc000) AM_RAM
ADDRESS_MAP_END

READ8_MEMBER(dblewing_state::irq_latch_r)
{
	/* bit 1 of dblewing_sound_irq specifies IRQ command writes */
	m_sound_irq &= ~0x02;
	m_audiocpu->set_input_line(0, (m_sound_irq != 0) ? ASSERT_LINE : CLEAR_LINE);
	return m_sound_irq;
}

static ADDRESS_MAP_START( sound_map, AS_PROGRAM, 8, dblewing_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x87ff) AM_RAM
	AM_RANGE(0xa000, 0xa001) AM_DEVREADWRITE("ymsnd", ym2151_device, status_r, write)
	AM_RANGE(0xb000, 0xb000) AM_DEVREADWRITE("oki", okim6295_device, read, write)
	AM_RANGE(0xc000, 0xc000) AM_READ(soundlatch_byte_r)
	AM_RANGE(0xd000, 0xd000) AM_READ(irq_latch_r) //timing? sound latch?
	AM_RANGE(0xf000, 0xf000) AM_DEVREADWRITE("oki", okim6295_device, read, write)
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_io, AS_IO, 8, dblewing_state )
	AM_RANGE(0x0000, 0xffff)  AM_ROM AM_REGION("audio_data", 0)
ADDRESS_MAP_END



static const gfx_layout tile_8x8_layout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0,RGN_FRAC(0,2)+8,RGN_FRAC(0,2)+0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	8*16
};

static const gfx_layout tile_16x16_layout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+0,RGN_FRAC(0,2)+8,RGN_FRAC(0,2)+0 },
	{ 256,257,258,259,260,261,262,263,0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,8*16,9*16,10*16,11*16,12*16,13*16,14*16,15*16 },
	32*16
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 24,8,16,0 },
	{ 512,513,514,515,516,517,518,519, 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
		8*32, 9*32,10*32,11*32,12*32,13*32,14*32,15*32},
	32*32
};


static GFXDECODE_START( dblewing )
	GFXDECODE_ENTRY( "gfx1", 0, tile_8x8_layout,     0x000, 32 )    /* Tiles (8x8) */
	GFXDECODE_ENTRY( "gfx1", 0, tile_16x16_layout,   0x000, 32 )    /* Tiles (16x16) */
	GFXDECODE_ENTRY( "gfx2", 0, spritelayout,        0x200, 32 )    /* Sprites (16x16) */
GFXDECODE_END

static INPUT_PORTS_START( dblewing )
	PORT_START("P1_P2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START("SYSTEM")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_VBLANK("screen")

	PORT_START("DSW")
		/* 16bit - These values are for Dip Switch #1 */
	PORT_DIPNAME( 0x0007, 0x0007, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0007, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0005, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0003, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0038, 0x0038, DEF_STR( Coin_B ) ) PORT_DIPLOCATION("SW1:4,5,6")
	PORT_DIPSETTING(      0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0038, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0030, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0028, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0018, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Region ) ) PORT_DIPLOCATION("SW1:8") /*Manual says "don't change this" */
	PORT_DIPSETTING(      0x0080, DEF_STR( Japan ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Korea ) )
	/* 16bit - These values are for Dip Switch #2 */
	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(      0x0100, "1" )
	PORT_DIPSETTING(      0x0000, "2" )
	PORT_DIPSETTING(      0x0300, "3" )
	PORT_DIPSETTING(      0x0200, "5" )
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION("SW2:3,4")
	PORT_DIPSETTING(      0x0800, DEF_STR( Easy ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( Normal ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x3000, 0x3000, DEF_STR( Bonus_Life ) ) PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(      0x2000, "Every 100,000" )
	PORT_DIPSETTING(      0x3000, "Every 150,000" )
	PORT_DIPSETTING(      0x1000, "Every 300,000" )
	PORT_DIPSETTING(      0x0000, "250,000 Only" )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Allow_Continue ) ) PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x0000, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START("UNK")
	PORT_DIPNAME( 0x0001, 0x0001, "2" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END

WRITE_LINE_MEMBER(dblewing_state::sound_irq)
{
	/* bit 0 of dblewing_sound_irq specifies IRQ from sound chip */
	if (state)
		m_sound_irq |= 0x01;
	else
		m_sound_irq &= ~0x01;
	m_audiocpu->set_input_line(0, (m_sound_irq != 0) ? ASSERT_LINE : CLEAR_LINE);
}

static int dblewing_bank_callback( const int bank )
{
	return ((bank >> 4) & 0x7) * 0x1000;
}

static const deco16ic_interface dblewing_deco16ic_tilegen1_intf =
{
	"screen",
	0, 1,
	0x0f, 0x0f,     /* trans masks (default values) */
	0, 16, /* color base (default values) */
	0x0f, 0x0f, /* color masks (default values) */
	dblewing_bank_callback,
	dblewing_bank_callback,
	0,1,
};

void dblewing_state::machine_start()
{

	save_item(NAME(m_sound_irq));
}

void dblewing_state::machine_reset()
{

	m_sound_irq = 0;
}

static MACHINE_CONFIG_START( dblewing, dblewing_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, 14000000)   /* DE102 */
	MCFG_CPU_PROGRAM_MAP(dblewing_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", dblewing_state,  irq6_line_hold)

	MCFG_CPU_ADD("audiocpu", Z80, 4000000)
	MCFG_CPU_PROGRAM_MAP(sound_map)
	MCFG_CPU_IO_MAP(sound_io)

	MCFG_QUANTUM_TIME(attotime::from_hz(6000))


	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(58)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(dblewing_state, screen_update_dblewing)

	MCFG_PALETTE_LENGTH(4096)
	MCFG_GFXDECODE(dblewing)

	MCFG_DECO16IC_ADD("tilegen1", dblewing_deco16ic_tilegen1_intf)
	MCFG_DEVICE_ADD("spritegen", DECO_SPRITE, 0)
	decospr_device::set_gfx_region(*device, 2);
	decospr_device::set_pri_callback(*device, dblwings_pri_callback);

	MCFG_DECO104_ADD("ioprot104")

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_YM2151_ADD("ymsnd", 32220000/9)
	MCFG_YM2151_IRQ_HANDLER(WRITELINE(dblewing_state, sound_irq))
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)

	MCFG_OKIM6295_ADD("oki", 32220000/32, OKIM6295_PIN7_HIGH)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)
MACHINE_CONFIG_END


/*

Double Wings (JPN Ver.)
(c)1993 Mitchell
DEC-22V0 (S-NK-3220)

Software is by Mitchell, but the PCB is pure Data East.

Data East ROM code = KP


CPU     :DE102 - Encrypted 68000
Sound   :TMPZ84C00AP-6,YM2151,OKI M6295, YM3014B
OSC     :28.0000MHz,32.2200MHz
RAM     :LH6168 x 1, CXK5814 x 6, CXK5864 x 4
DIP     :2 x 8 position
Other   :DATA EAST Chips (numbers scratched)
         --------------------------------------
         DATA EAST #?  9235EV 205941  VC5259-0001 JAPAN (confirmed #52) - 128 pin PQFP
         DATA EAST #?  DATA EAST 250 JAPAN (#102, the CPU) - 128 Pin PQFP
         DATA EAST #?  24220F008 (confirmed #141) - 160 pin PQFP
         DATA EAST #?  L7A0717   9143  (confirmed #104, IO/Protection) - 100 pin PQFP

         PALs: PAL16L8 (x 2, VG-00, VG-01) between program ROMs and CPU
               PAL16L8 (x 1, VG-02) next to #52

         Small surface-mounted chip with number scratched off (28 pin SOP), but has number 9303K9200.
         A similar chip exists on Capt. America PCB and has the number 77 on it. Possibly the same chip?

KP_00-.3D    [547dc83e]
KP_01-.5D    [7a210c33]

KP_02-.10H   [def035fa]

KP_03-.16H   [5d7f930d]

MBE-00.14A   [e33f5c93]
MBE-01.16A   [ef452ad7]
MBE-02.8H    [5a6d3ac5]

*/

ROM_START( dblewing )
	ROM_REGION( 0x80000, "maincpu", 0 ) /* DE102 code (encrypted) */
	ROM_LOAD16_BYTE( "kp_00-.3d",    0x000001, 0x040000, CRC(547dc83e) SHA1(f6f96bd4338d366f06df718093f035afabc073d1) )
	ROM_LOAD16_BYTE( "kp_01-.5d",    0x000000, 0x040000, CRC(7a210c33) SHA1(ced89140af6d6a1bc0ffb7728afca428ed007165) )

	ROM_REGION( 0x18000, "audiocpu", 0 ) // sound cpu
	ROM_LOAD( "kp_02-.10h",    0x00000, 0x08000, CRC(def035fa) SHA1(fd50314e5c94c25df109ee52c0ce701b0ff2140c) )
	ROM_CONTINUE(              0x10000, 0x08000 )

	ROM_REGION( 0x10000, "audio_data", 0 ) // sound data
	ROM_COPY( "audiocpu" ,  0x00000, 0x00000, 0x8000 )
	ROM_COPY( "audiocpu" ,  0x10000, 0x08000, 0x8000 )

	ROM_REGION( 0x100000, "gfx1", 0 )
	ROM_LOAD( "mbe-02.8h",    0x00000, 0x100000, CRC(5a6d3ac5) SHA1(738bb833e2c5d929ac75fe4e69ee0af88197d8a6) )

	ROM_REGION( 0x200000, "gfx2", 0 )
	ROM_LOAD16_BYTE( "mbe-00.14a",    0x000000, 0x100000, CRC(e33f5c93) SHA1(720904b54d02dace2310ac6bd07d5ed4bc4fd69c) )
	ROM_LOAD16_BYTE( "mbe-01.16a",    0x000001, 0x100000, CRC(ef452ad7) SHA1(7fe49123b5c2778e46104eaa3a2104ce09e05705) )

	ROM_REGION( 0x80000, "oki", 0 ) /* Oki samples */
	ROM_LOAD( "kp_03-.16h",    0x00000, 0x20000, CRC(5d7f930d) SHA1(ad23aa804ea3ccbd7630ade9b53fc3ea2718a6ec) )
	ROM_RELOAD(                0x20000, 0x20000 )
	ROM_RELOAD(                0x40000, 0x20000 )
	ROM_RELOAD(                0x60000, 0x20000 )

ROM_END

DRIVER_INIT_MEMBER(dblewing_state,dblewing)
{
	deco56_decrypt_gfx(machine(), "gfx1");
	deco102_decrypt_cpu(machine(), "maincpu", 0x399d, 0x25, 0x3d);
}


GAME( 1993, dblewing, 0,     dblewing, dblewing, dblewing_state,  dblewing,  ROT90, "Mitchell", "Double Wings", GAME_UNEMULATED_PROTECTION | GAME_IMPERFECT_SOUND | GAME_NOT_WORKING | GAME_SUPPORTS_SAVE )
