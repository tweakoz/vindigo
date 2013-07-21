/* Dream Ball

PCB DE-0386-2
(also has DEC-22V0 like many Data East PCBS)

Customs
104 (I/O, Protection)
59 (68000 CPU)
141 (Tilemap GFX)
71 (usually sprites? or mixer?)
HD63B50P in I/O section (maybe hopper comms?)

--
todo:
emulate hopper
lamps?


*/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "cpu/m68000/m68000.h"
#include "includes/decocrpt.h"
#include "sound/okim6295.h"
#include "video/deco16ic.h"
#include "video/decospr.h"
#include "machine/eeprom.h"
#include "machine/deco104.h"

class dreambal_state : public driver_device
{
public:
	dreambal_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_eeprom(*this, "eeprom"),
		m_maincpu(*this, "maincpu"),
		m_deco104(*this, "ioprot104"),
		m_deco_tilegen1(*this, "tilegen1")
	{ }

	required_device<eeprom_device> m_eeprom;

	/* devices */
	required_device<cpu_device> m_maincpu;
	optional_device<deco104_device> m_deco104;
	required_device<deco16ic_device> m_deco_tilegen1;

	DECLARE_DRIVER_INIT(dreambal);
	virtual void machine_start();
	virtual void machine_reset();
	UINT32 screen_update_dreambal(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	DECLARE_WRITE16_HANDLER( dreambal_eeprom_w )
	{
		if (data&0xfff8)
		{
			logerror("dreambal_eeprom_w unhandled data %04x %04x\n",data&0x0fff8, mem_mask);
		}

		if (mem_mask&0x00ff)
		{
			m_eeprom->set_clock_line(data &0x2 ? ASSERT_LINE : CLEAR_LINE);
			m_eeprom->write_bit(data &0x1);
			m_eeprom->set_cs_line(data&0x4 ? CLEAR_LINE : ASSERT_LINE);
		}
	}
};


UINT32 dreambal_state::screen_update_dreambal(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	address_space &space = generic_space();
	UINT16 flip = m_deco_tilegen1->pf_control_r(space, 0, 0xffff);

	flip_screen_set(BIT(flip, 7));
	m_deco_tilegen1->pf_update(NULL, NULL);

	bitmap.fill(0, cliprect); /* not Confirmed */
	machine().priority_bitmap.fill(0);

	m_deco_tilegen1->tilemap_2_draw(bitmap, cliprect, 0, 2);
	m_deco_tilegen1->tilemap_1_draw(bitmap, cliprect, 0, 4);
	return 0;
}


static ADDRESS_MAP_START( dreambal_map, AS_PROGRAM, 16, dreambal_state )
//ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x000000, 0x07ffff) AM_ROM
	AM_RANGE(0x100000, 0x100fff) AM_DEVREADWRITE("tilegen1", deco16ic_device, pf1_data_r, pf1_data_w)
	AM_RANGE(0x101000, 0x101fff) AM_RAM
	AM_RANGE(0x102000, 0x102fff) AM_DEVREADWRITE("tilegen1", deco16ic_device, pf2_data_r, pf2_data_w)
	AM_RANGE(0x103000, 0x103fff) AM_RAM

	AM_RANGE(0x120000, 0x123fff) AM_RAM
	AM_RANGE(0x140000, 0x1403ff) AM_RAM_WRITE(paletteram_xxxxBBBBGGGGRRRR_word_w) AM_SHARE("paletteram")
	AM_RANGE(0x161000, 0x16100f) AM_DEVWRITE("tilegen1", deco16ic_device, pf_control_w)

	AM_RANGE(0x160088, 0x160089) AM_READ_PORT("INP")
	AM_RANGE(0x160292, 0x160293) AM_READ_PORT("SYS")
	AM_RANGE(0x16036C, 0x16036D) AM_READ_PORT("COIN")


	AM_RANGE(0x180000, 0x180001) AM_DEVREADWRITE8("oki", okim6295_device, read, write, 0x00ff)

	AM_RANGE(0x162000, 0x162001) AM_WRITENOP // writes 0003 on startup
	AM_RANGE(0x163000, 0x163001) AM_WRITENOP // something on bit 1
	AM_RANGE(0x164000, 0x164001) AM_WRITENOP // something on bit 1

	AM_RANGE(0x165000, 0x165001) AM_WRITE( dreambal_eeprom_w ) // EEP Write?

	AM_RANGE(0x16c002, 0x16c00d) AM_WRITENOP // writes 0000 to 0005 on startup
	AM_RANGE(0x1a0000, 0x1a0001) AM_WRITENOP // writes 33 and 1a on startup

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


static GFXDECODE_START( dreambal )
	GFXDECODE_ENTRY( "gfx1", 0, tile_8x8_layout,     0x000, 32 )    /* Tiles (8x8) */
	GFXDECODE_ENTRY( "gfx1", 0, tile_16x16_layout,   0x000, 32 )    /* Tiles (16x16) */
GFXDECODE_END

static INPUT_PORTS_START( dreambal )
	PORT_START("INP")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_GAMBLE_TAKE )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_GAMBLE_PAYOUT ) // currently causes hopper error
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_POKER_CANCEL ) // fold?
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_POKER_HOLD1 ) PORT_NAME("Hold 1 / Double Up")
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_POKER_HOLD3 ) PORT_NAME("Hold 3 / Small")
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_POKER_HOLD5 ) PORT_NAME("Hold 5")
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_GAMBLE_BET )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_START1) PORT_NAME("Start / Proceed")
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_POKER_HOLD2 ) PORT_NAME("Hold 2 / Big")
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_POKER_HOLD4 ) PORT_NAME("Hold 4")
	PORT_SERVICE( 0x2000, IP_ACTIVE_LOW ) // only works if there are 0 credits
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START("SYS")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_READ_LINE_DEVICE_MEMBER("eeprom", eeprom_device, read_bit)
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

	PORT_START("COIN")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_DIPNAME( 0x0004, 0x0004, "3" ) // freeze / vbl?
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

static int dreambal_bank_callback( const int bank )
{
	return ((bank >> 4) & 0x7) * 0x1000;
}

static const deco16ic_interface dreambal_deco16ic_tilegen1_intf =
{
	"screen",
	0, 1,
	0x0f, 0x0f,     /* trans masks (default values) */
	0, 16, /* color base (default values) */
	0x0f, 0x0f, /* color masks (default values) */
	dreambal_bank_callback,
	dreambal_bank_callback,
	0,1,
};

void dreambal_state::machine_start()
{
}

void dreambal_state::machine_reset()
{
}

// xtals = 28.000, 9.8304
static MACHINE_CONFIG_START( dreambal, dreambal_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M68000, 28000000/2)
	MCFG_CPU_PROGRAM_MAP(dreambal_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", dreambal_state,  irq6_line_hold) // 5 valid too?

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(58)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(dreambal_state, screen_update_dreambal)

	MCFG_PALETTE_LENGTH(0x400/2)
	MCFG_GFXDECODE(dreambal)

	MCFG_EEPROM_93C46_ADD("eeprom")  // 93lc46b

	MCFG_DECO104_ADD("ioprot104")

	MCFG_DECO16IC_ADD("tilegen1", dreambal_deco16ic_tilegen1_intf)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_OKIM6295_ADD("oki", 9830400/8, OKIM6295_PIN7_HIGH)

	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)
MACHINE_CONFIG_END



ROM_START( dreambal )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD16_WORD_SWAP( "mm_00-2.1c",    0x000000, 0x020000, CRC(257f6ad1) SHA1(7b232ce2d503e6f21286176974f6b74052f76d07) )

	ROM_REGION( 0x80000, "gfx1", 0 )
	ROM_LOAD( "mm_01-1.12b",    0x00000, 0x80000, CRC(dc9cc708) SHA1(03b8e6aa37e0107514a2498849208d2bd51a4163) )

	ROM_REGION( 0x80000, "oki", 0 ) /* Oki samples */
	ROM_LOAD( "mm_01-1.12f",    0x00000, 0x20000, CRC(4f134be7) SHA1(b83230cc62bde55be736fd604af23f927706a770) )
ROM_END

DRIVER_INIT_MEMBER(dreambal_state,dreambal)
{
	deco56_decrypt_gfx(machine(), "gfx1");
}

// Ver 2.4 JPN 93.12.02
GAME( 1993, dreambal, 0,     dreambal, dreambal, dreambal_state,  dreambal,  ROT0, "NDK / Data East", "Dream Ball (Japan V2.4)", 0 ) // copyright shows NDK, board is Data East, code seems Data East-like too
