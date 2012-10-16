/***************************************************************************
Double Crown
(C) 1994, or maybe 1995
cards gambling game

dfinal.c ish, but newer?


Excellent System
boardlabel: ES-9411B

28.6363 xtal
ES-9409 QFP is 208 pins.. for graphics only?
Z0840006PSC Zilog z80, is rated 6.17 MHz
OKI M82C55A-2
65764H-5 .. 64kbit ram CMOS
2 * N341256P-25 - CMOS SRAM 256K-BIT(32KX8)
4 * dipsw 8pos
YMZ284-D (ay8910, but without i/o ports)
MAXIM MAX693ACPE is a "Microprocessor Supervisory Circuit", for watchdog? and for keeping nvram stable?

***************************************************************************/


#include "emu.h"
#include "cpu/z80/z80.h"
#include "sound/ay8910.h"
#include "machine/nvram.h"

#define MAIN_CLOCK XTAL_28_63636MHz

#define DEBUG_VRAM

class dblcrown_state : public driver_device
{
public:
	dblcrown_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		  m_maincpu(*this, "maincpu")
	{ }

	// devices
	required_device<cpu_device> m_maincpu;

	// screen updates
	UINT32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	UINT8 m_bank;
	UINT8 m_irq_src;
	UINT8 *m_pal_ram;
	UINT8 *m_vram;
	UINT8 m_vram_bank[2];

	DECLARE_READ8_MEMBER(bank_r);
	DECLARE_WRITE8_MEMBER(bank_w);
	DECLARE_READ8_MEMBER(irq_source_r);
	DECLARE_WRITE8_MEMBER(irq_source_w);
	DECLARE_READ8_MEMBER(palette_r);
	DECLARE_WRITE8_MEMBER(palette_w);
	DECLARE_READ8_MEMBER(vram_r);
	DECLARE_WRITE8_MEMBER(vram_w);
	DECLARE_READ8_MEMBER(vram_bank_r);
	DECLARE_WRITE8_MEMBER(vram_bank_w);

	TIMER_DEVICE_CALLBACK_MEMBER(dblcrown_irq_scanline);

protected:
	// driver_device overrides
	virtual void machine_start();
	virtual void machine_reset();

	virtual void video_start();
	virtual void palette_init();
};

void dblcrown_state::video_start()
{
	m_pal_ram = auto_alloc_array(machine(), UINT8, 0x200*2);
	m_vram = auto_alloc_array(machine(), UINT8, 0x1000*0x10);

	state_save_register_global_pointer(machine(), m_vram, 0x1000*0x10);
}

UINT32 dblcrown_state::screen_update( screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	return 0;
}

READ8_MEMBER( dblcrown_state::bank_r)
{
	return m_bank;
}

WRITE8_MEMBER( dblcrown_state::bank_w)
{
	m_bank = data;
	membank("rom_bank")->set_entry(m_bank & 0x1f);
}

READ8_MEMBER( dblcrown_state::irq_source_r)
{
	return m_irq_src;
}

WRITE8_MEMBER( dblcrown_state::irq_source_w)
{
	m_irq_src = data; // this effectively acks the irq, by writing 0
}

READ8_MEMBER( dblcrown_state::palette_r)
{
	if(m_bank & 8) /* TODO: verify this */
		offset+=0x200;

	return m_pal_ram[offset];
}

WRITE8_MEMBER( dblcrown_state::palette_w)
{
	int r,g,b,datax;

	if(m_bank & 8) /* TODO: verify this */
		offset+=0x200;

	m_pal_ram[offset] = data;
	offset>>=1;
	datax = m_pal_ram[offset*2] + 256*m_pal_ram[offset*2 + 1];

	r = ((datax)&0x000f)>>0;
	g = ((datax)&0x00f0)>>4;
	b = ((datax)&0x0f00)>>8;

	palette_set_color_rgb(machine(), offset, pal4bit(r), pal4bit(g), pal4bit(b));
}


READ8_MEMBER( dblcrown_state::vram_r)
{
	UINT32 hi_offs;
	hi_offs = m_vram_bank[offset & 0x1000 >> 12] << 12;

	return m_vram[(offset & 0xfff) | hi_offs];
}

WRITE8_MEMBER( dblcrown_state::vram_w)
{
	UINT32 hi_offs;
	hi_offs = m_vram_bank[(offset & 0x1000) >> 12] << 12;

	m_vram[(offset & 0xfff) | hi_offs] = data;

	#ifdef DEBUG_VRAM
	{
		UINT8 *VRAM = memregion("vram")->base();

		VRAM[(offset & 0xfff) | hi_offs] = data;
		machine().gfx[0]->mark_dirty(((offset & 0xfff) | hi_offs) / 32);
	}
	#endif
}

READ8_MEMBER( dblcrown_state::vram_bank_r)
{
	return m_vram_bank[offset];
}

WRITE8_MEMBER( dblcrown_state::vram_bank_w)
{
	m_vram_bank[offset] = data & 0xf;

	if(data & 0xf0)
		printf("vram bank = %02x\n",data);
}

static ADDRESS_MAP_START( dblcrown_map, AS_PROGRAM, 8, dblcrown_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0x8000, 0x9fff) AM_ROMBANK("rom_bank")
	AM_RANGE(0xa000, 0xb7ff) AM_RAM // work ram
	AM_RANGE(0xb800, 0xbfff) AM_RAM AM_SHARE("nvram")
	AM_RANGE(0xc000, 0xdfff) AM_READWRITE(vram_r, vram_w)
	AM_RANGE(0xf000, 0xf1ff) AM_READWRITE(palette_r, palette_w) //AM_RAM_WRITE(paletteram_xBBBBBGGGGGRRRRR_byte_le_w) AM_SHARE("paletteram") // TODO: correct bit order
	AM_RANGE(0xfe00, 0xfeff) AM_RAM // ???
	AM_RANGE(0xff00, 0xff01) AM_READWRITE(vram_bank_r, vram_bank_w)
	AM_RANGE(0xff04, 0xff04) AM_READWRITE(irq_source_r,irq_source_w)

	AM_RANGE(0xff00, 0xffff) AM_RAM // ???, intentional fall-through
ADDRESS_MAP_END

static ADDRESS_MAP_START( dblcrown_io, AS_IO, 8, dblcrown_state )
   ADDRESS_MAP_GLOBAL_MASK(0xff)
   AM_RANGE(0x11, 0x11) AM_READWRITE(bank_r,bank_w)
ADDRESS_MAP_END

static INPUT_PORTS_START( dblcrown )
	/* dummy active high structure */
	PORT_START("SYSA")
	PORT_DIPNAME( 0x01, 0x00, "SYSA" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	/* dummy active low structure */
	PORT_START("DSWA")
	PORT_DIPNAME( 0x01, 0x01, "DSWA" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static const gfx_layout char_8x8_layout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 4,0, 12,8, 20,16, 28,24 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static const gfx_layout char_16x16_layout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 4,0, 12,8, 20,16, 28,24, 36,32, 44,40, 52,48, 60,56 },
	{ STEP16(0,8*8) },
	8*8*16
};


static GFXDECODE_START( dblcrown )
#ifdef DEBUG_VRAM
	GFXDECODE_ENTRY( "vram", 0, char_8x8_layout, 0, 0x20 )
#endif
	GFXDECODE_ENTRY( "gfx1", 0, char_16x16_layout, 0, 0x20 )
GFXDECODE_END



void dblcrown_state::machine_start()
{
	UINT8 *ROM = memregion("maincpu")->base();
	membank("rom_bank")->configure_entries(0, 0x20, &ROM[0], 0x2000);
}

void dblcrown_state::machine_reset()
{
}


void dblcrown_state::palette_init()
{
}

TIMER_DEVICE_CALLBACK_MEMBER(dblcrown_state::dblcrown_irq_scanline)
{
	int scanline = param;

	if (scanline == 240)
	{
		m_maincpu->set_input_line(0, HOLD_LINE);
		m_irq_src = 2;
	}

	/* TODO: unknown source */
	if (scanline == 128)
	{
		m_maincpu->set_input_line(0, HOLD_LINE);
		m_irq_src = 4;
	}
}


static MACHINE_CONFIG_START( dblcrown, dblcrown_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",Z80,MAIN_CLOCK/6)
	MCFG_CPU_PROGRAM_MAP(dblcrown_map)
	MCFG_CPU_IO_MAP(dblcrown_io)
	MCFG_TIMER_DRIVER_ADD_SCANLINE("scantimer", dblcrown_state, dblcrown_irq_scanline, "screen", 0, 1)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MCFG_SCREEN_UPDATE_DRIVER(dblcrown_state, screen_update)
	MCFG_SCREEN_SIZE(32*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 0*8, 32*8-1)

	MCFG_GFXDECODE(dblcrown)

	MCFG_PALETTE_LENGTH(0x200)

	MCFG_NVRAM_ADD_0FILL("nvram")

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("aysnd", AY8910, MAIN_CLOCK/12)
    MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.30)
MACHINE_CONFIG_END


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( dblcrown )
	ROM_REGION( 0x40000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD("1.u33", 0x00000, 0x40000, CRC(5df95a9c) SHA1(799333206089989c25ff9f167363073d4cf64bd2) )

	ROM_REGION( 0x80000, "gfx1", ROMREGION_ERASE00 )
	ROM_LOAD("2.u43", 0x00000, 0x80000, CRC(58200bd4) SHA1(2795cfc41056111f66bfb82916343d1c733baa83) )

#ifdef DEBUG_VRAM
	ROM_REGION( 0x1000*0x10, "vram", ROMREGION_ERASE00 )
#endif

	ROM_REGION( 0x0bf1, "pals", 0 ) // in Jedec format
	ROM_LOAD("palce16v8h.u39", 0x0000, 0x0bf1, CRC(997b0ba9) SHA1(1c121ab74f33d5162b619740b08cc7bc694c257d) )
ROM_END

GAME( 199?, dblcrown,  0,   dblcrown,  dblcrown,  driver_device, 0,       ROT0, "Excellent System",      "Double Crown", GAME_IS_SKELETON ) // 1997 DYNA copyright in tile GFX
