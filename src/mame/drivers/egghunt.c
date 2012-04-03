/*

Egg Hunt

hardware seems close to mitchell.c
--
palette format is different
has a sound cpu
ports shuffled around a bit



the following string is at the start of the roms

INVI IMAGE COPYWRITE 1995 OCT SEOUL IN KOREA
TEL 02-569-5830
PROGRMED BY JANG-K.Y
GRAPHIC DESIGN  KIM-Y.H & LEE-H.M

(and in the sound program)
SOUND DIRECTOR  LEE-S.O

the screen says VH-K October 1995, are VH-K another company involved?

---------------------

Egghunt by Invi Image

PCB marked "Invi Image Co. 1995 IZ80B-1"
The pcb has poor quality and resemble a bootleg.

2x Z80
1x AD65 (oki 6295 probably)
1x OSc 12mhz
1x OSC 30mhz
1x FPGA
2x Dipswitch

Note: rom 3 has the 16th pin overlapped and soldered to 15th....it seems it's a manufacturer choice since pin 16 on the socket is not connected with any trace on the pcb
I dumped it with this configuration. In case I'll redump it desoldering pin 16 from 15

*/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "sound/okim6295.h"

class egghunt_state : public driver_device
{
public:
	egghunt_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }

	/* video-related */
	tilemap_t   *m_bg_tilemap;
	UINT8     m_vidram_bank;

	/* misc */
	UINT8     m_okibanking;
	UINT8     m_gfx_banking;

	/* devices */
	device_t *m_audiocpu;

	/* memory */
	UINT8 *   m_atram;
	UINT8     m_bgram[0x1000];
	UINT8     m_spram[0x1000];
	DECLARE_READ8_MEMBER(egghunt_bgram_r);
	DECLARE_WRITE8_MEMBER(egghunt_bgram_w);
	DECLARE_WRITE8_MEMBER(egghunt_atram_w);
	DECLARE_WRITE8_MEMBER(egghunt_gfx_banking_w);
	DECLARE_WRITE8_MEMBER(egghunt_vidram_bank_w);
	DECLARE_WRITE8_MEMBER(egghunt_soundlatch_w);
};


static void draw_sprites( running_machine &machine, bitmap_ind16 &bitmap,const rectangle &cliprect )
{
	egghunt_state *state = machine.driver_data<egghunt_state>();
	int flipscreen = 0;
	int offs, sx, sy;

	for (offs = 0x1000 - 0x40; offs >= 0; offs -= 0x20)
	{
		int code = state->m_spram[offs];
		int attr = state->m_spram[offs + 1];
		int color = attr & 0x0f;
		sx = state->m_spram[offs + 3] + ((attr & 0x10) << 4);
		sy = ((state->m_spram[offs + 2] + 8) & 0xff) - 8;
		code += (attr & 0xe0) << 3;

		if (attr & 0xe0)
		{
			switch(state->m_gfx_banking & 0x30)
			{
	//          case 0x00:
	//          case 0x10: code += 0; break;
				case 0x20: code += 0x400; break;
				case 0x30: code += 0x800; break;
			}
		}

		if (flipscreen)
		{
			sx = 496 - sx;
			sy = 240 - sy;
		}
		drawgfx_transpen(bitmap,cliprect,machine.gfx[1],
				 code,
				 color,
				 flipscreen,flipscreen,
				 sx,sy,15);
	}
}

static TILE_GET_INFO( get_bg_tile_info )
{
	egghunt_state *state = machine.driver_data<egghunt_state>();
	int code = ((state->m_bgram[tile_index * 2 + 1] << 8) | state->m_bgram[tile_index * 2]) & 0x3fff;
	int colour = state->m_atram[tile_index] & 0x3f;

	if(code & 0x2000)
	{
		if((state->m_gfx_banking & 3) == 2)
			code += 0x2000;
		else if((state->m_gfx_banking & 3) == 3)
			code += 0x4000;
//      else if((state->m_gfx_banking & 3) == 1)
//          code += 0;
	}

	SET_TILE_INFO(0, code, colour, 0);
}

READ8_MEMBER(egghunt_state::egghunt_bgram_r)
{
	if (m_vidram_bank)
	{
		return m_spram[offset];
	}
	else
	{
		return m_bgram[offset];
	}
}

WRITE8_MEMBER(egghunt_state::egghunt_bgram_w)
{
	if (m_vidram_bank)
	{
		m_spram[offset] = data;
	}
	else
	{
		m_bgram[offset] = data;
		m_bg_tilemap->mark_tile_dirty(offset / 2);
	}
}

WRITE8_MEMBER(egghunt_state::egghunt_atram_w)
{
	m_atram[offset] = data;
	m_bg_tilemap->mark_tile_dirty(offset);
}


static VIDEO_START(egghunt)
{
	egghunt_state *state = machine.driver_data<egghunt_state>();

	state->m_bg_tilemap = tilemap_create(machine, get_bg_tile_info, tilemap_scan_rows, 8, 8, 64, 32);

	state->save_item(NAME(state->m_bgram));
	state->save_item(NAME(state->m_spram));
}

static SCREEN_UPDATE_IND16(egghunt)
{
	egghunt_state *state = screen.machine().driver_data<egghunt_state>();
	state->m_bg_tilemap->draw(bitmap, cliprect, 0, 0);
	draw_sprites(screen.machine(), bitmap, cliprect);
	return 0;
}

WRITE8_MEMBER(egghunt_state::egghunt_gfx_banking_w)
{
	// data & 0x03 is used for tile banking
	// data & 0x30 is used for sprites banking
	m_gfx_banking = data & 0x33;

	m_bg_tilemap->mark_all_dirty();
}

WRITE8_MEMBER(egghunt_state::egghunt_vidram_bank_w)
{
	m_vidram_bank = data & 1;
}

WRITE8_MEMBER(egghunt_state::egghunt_soundlatch_w)
{
	soundlatch_w(space, 0, data);
	device_set_input_line(m_audiocpu, 0, HOLD_LINE);
}

static READ8_DEVICE_HANDLER( egghunt_okibanking_r )
{
	egghunt_state *state = device->machine().driver_data<egghunt_state>();
	return state->m_okibanking;
}

static WRITE8_DEVICE_HANDLER( egghunt_okibanking_w )
{
	egghunt_state *state = device->machine().driver_data<egghunt_state>();
	state->m_okibanking = data;
	okim6295_device *oki = downcast<okim6295_device *>(device);
	oki->set_bank_base((data & 0x10) ? 0x40000 : 0);
}

static ADDRESS_MAP_START( egghunt_map, AS_PROGRAM, 8, egghunt_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0xc000, 0xc7ff) AM_RAM_WRITE_LEGACY(paletteram_xRRRRRGGGGGBBBBB_le_w) AM_BASE_GENERIC(paletteram)
	AM_RANGE(0xc800, 0xcfff) AM_RAM_WRITE(egghunt_atram_w) AM_BASE(m_atram)
	AM_RANGE(0xd000, 0xdfff) AM_READWRITE(egghunt_bgram_r, egghunt_bgram_w)
	AM_RANGE(0xe000, 0xffff) AM_RAM
ADDRESS_MAP_END


static ADDRESS_MAP_START( io_map, AS_IO, 8, egghunt_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_READ_PORT("DSW1") AM_WRITE(egghunt_vidram_bank_w)
	AM_RANGE(0x01, 0x01) AM_READ_PORT("SYSTEM") AM_WRITE(egghunt_gfx_banking_w)
	AM_RANGE(0x02, 0x02) AM_READ_PORT("P1")
	AM_RANGE(0x03, 0x03) AM_READ_PORT("P2") AM_WRITE(egghunt_soundlatch_w)
	AM_RANGE(0x04, 0x04) AM_READ_PORT("DSW2")
	AM_RANGE(0x06, 0x06) AM_READ_PORT("UNK") AM_WRITENOP
	AM_RANGE(0x07, 0x07) AM_WRITENOP
ADDRESS_MAP_END

static ADDRESS_MAP_START( sound_map, AS_PROGRAM, 8, egghunt_state )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
	AM_RANGE(0xe000, 0xe000) AM_READ_LEGACY(soundlatch_r)
	AM_RANGE(0xe001, 0xe001) AM_DEVREADWRITE_LEGACY("oki", egghunt_okibanking_r, egghunt_okibanking_w)
	AM_RANGE(0xe004, 0xe004) AM_DEVREADWRITE("oki", okim6295_device, read, write)
	AM_RANGE(0xf000, 0xffff) AM_RAM
ADDRESS_MAP_END


static INPUT_PORTS_START( egghunt )
	PORT_START("DSW1")
	PORT_DIPNAME( 0x01, 0x01, "Debug Mode" ) // Run all the animations
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
	PORT_DIPNAME( 0x60, 0x60, "Credits per Player" )
	PORT_DIPSETTING(    0x60, "1" )
//  PORT_DIPSETTING(    0x20, "2" ) /* One of these maybe 2 to start 1 to continue */
	PORT_DIPSETTING(    0x40, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )


	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN1 )

	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)

	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
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
	PORT_DIPNAME( 0x80, 0x00, "Censor Pictures" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ) )

	PORT_START("UNK")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
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



static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(2,4),RGN_FRAC(3,4),RGN_FRAC(0,4),RGN_FRAC(1,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout tiles16x16_layout =
{
	16,16,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(2,4),RGN_FRAC(3,4),RGN_FRAC(0,4),RGN_FRAC(1,4) },
	{  4, 5, 6, 7,0, 1, 2, 3, 16*8+4,16*8+5,16*8+6,16*8+7,16*8+0,16*8+1,16*8+2,16*8+3 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,8*8,9*8,10*8,11*8,12*8,13*8,14*8,15*8 },
	16*16
};

static GFXDECODE_START( egghunt )
	GFXDECODE_ENTRY( "gfx1", 0, tiles8x8_layout, 0, 64 )
	GFXDECODE_ENTRY( "gfx2", 0, tiles16x16_layout, 0, 64 )
GFXDECODE_END


static MACHINE_START( egghunt )
{
	egghunt_state *state = machine.driver_data<egghunt_state>();

	state->m_audiocpu = machine.device("audiocpu");

	state->save_item(NAME(state->m_gfx_banking));
	state->save_item(NAME(state->m_okibanking));
	state->save_item(NAME(state->m_vidram_bank));
}

static MACHINE_RESET( egghunt )
{
	egghunt_state *state = machine.driver_data<egghunt_state>();
	state->m_gfx_banking = 0;
	state->m_okibanking = 0;
	state->m_vidram_bank = 0;
}

static MACHINE_CONFIG_START( egghunt, egghunt_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80,12000000/2)		 /* 6 MHz ?*/
	MCFG_CPU_PROGRAM_MAP(egghunt_map)
	MCFG_CPU_IO_MAP(io_map)
	MCFG_CPU_VBLANK_INT("screen", irq0_line_hold) // or 2 like mitchell.c?

	MCFG_CPU_ADD("audiocpu", Z80,12000000/2)		 /* 6 MHz ?*/
	MCFG_CPU_PROGRAM_MAP(sound_map)

	MCFG_MACHINE_START(egghunt)
	MCFG_MACHINE_RESET(egghunt)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(64*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(8*8, 56*8-1, 1*8, 31*8-1)
	MCFG_SCREEN_UPDATE_STATIC(egghunt)

	MCFG_GFXDECODE(egghunt)
	MCFG_PALETTE_LENGTH(0x400)

	MCFG_VIDEO_START(egghunt)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_OKIM6295_ADD("oki", 1056000, OKIM6295_PIN7_HIGH) // clock frequency & pin 7 not verified
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_CONFIG_END

ROM_START( egghunt )
	ROM_REGION( 0x20000, "maincpu", 0 )
	ROM_LOAD( "prg.bin", 0x00000, 0x20000, CRC(eb647145) SHA1(792951b76b5fac01e72ae34a2fe2108e373c5b62) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "rom2.bin", 0x00000, 0x10000, CRC(88a71bc3) SHA1(cf5acccfda9fda0d55af91a415a54391d0d0b7a2) )

	ROM_REGION( 0x100000, "gfx1", ROMREGION_INVERT )
	ROM_LOAD( "rom3.bin", 0x00000, 0x40000, CRC(9d51ac49) SHA1(b0785d746fb2872a04386016ffdee80e6174dfc0) )
	ROM_LOAD( "rom4.bin", 0x40000, 0x40000, CRC(41c63041) SHA1(24e9a21d448c144db2356329cf87dc99598c96dc) )
	ROM_LOAD( "rom5.bin", 0x80000, 0x40000, CRC(6f96cb97) SHA1(7dde7d2aec6b5f9929b98d06c07bb07bf7bd59dd) )
	ROM_LOAD( "rom6.bin", 0xc0000, 0x40000, CRC(b5a41d4b) SHA1(1b4cf9c944e3eb7dc2d26d8a73bf5efb7b53253a) )

	ROM_REGION( 0x80000, "gfx2", ROMREGION_INVERT )
	ROM_LOAD( "rom7.bin", 0x00000, 0x20000, CRC(1b43fb57) SHA1(f06e186bf514f2ad655df23636eab72e6fafd815) )
	ROM_LOAD( "rom8.bin", 0x20000, 0x20000, CRC(f8122d0d) SHA1(78551c689b9e4eeed5e1ae97d8c7a907a388a9ff) )
	ROM_LOAD( "rom9.bin", 0x40000, 0x20000, CRC(dbfa0ffe) SHA1(2aa759c0bd3945473a6d8fa48226ce6c6c94d740) )
	ROM_LOAD( "rom10.bin",0x60000, 0x20000, CRC(14f5fc74) SHA1(769bccf9c1b42c35c3aee3866ed015de7c83b710) )

	ROM_REGION( 0x80000, "oki", 0 )
	ROM_LOAD( "rom1.bin", 0x00000, 0x80000, CRC(f03589bc) SHA1(4d9c8422ac3c4c3ecba3bcf0ed47b8c7d5903f8c) )
ROM_END

GAME( 1995, egghunt, 0, egghunt, egghunt, 0, ROT0, "Invi Image", "Egg Hunt", GAME_SUPPORTS_SAVE )
