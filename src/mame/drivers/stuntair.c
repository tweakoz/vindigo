/*

Stunt Air - is this a bootleg of something? (it's not Star Jacker / Star Force)

Stunt Air by Nuova Videotron 1983

Finally i've found this one,too.This romset comes from a rare italian pcb.Game is a Sega's Star Jacker clone with different code,graphics,sound and hardware.

Hardware info (complete):
Main cpu Z80A
Sound cpu Z80A
Sound ic AY-3-8910 x2
Note: stereo sound output.Op amps LM3900 x3, audio amps TDA2002 x2, many discrete components
Osc: 18.432 Mhz
Ram:
Work 2kb (6116)
Sound 1kb (2114 x2)
Chars 1kb (2114 x2)
Bg 1,5kb (2114 x3)
Sprites 1kb (2148 x2)
color 320byte (27ls00 x10)

Rom definiton:
-top pcb-
stuntair.a0,a1,a3,a4,a6 main program
stuntair.e14 sound program
stuntair.a9 character gfx
stuntair.a11,a12 background gfx
stuntair.a13,a15 obj/sprites gfx
82s123.a7 encryption key? (removing it results in garbage boot screen with high score table music)
-bottom pcb-
82s129.l11 green,blue colors
82s129.m11 red color

Eproms are 2764
Bproms are 82s123,82s129

Dip switches (by direct test,no manuals present):
-DIP A-
SW 1   2   3   5   6   7   8  
   unknown
SW 4
   OFF infinite lives (test)
   ON  normal game
-DIP B-
SW 1   2  (coin 1)
   OFF OFF 1 coin 1 play,1 coin 2 play (alternate)
   ON  OFF 2 coin 1 play
   OFF ON  1 coin 2 play
   ON  ON  1 coin 1 play
SW 3   4  (coin 2)
   OFF OFF 1 coin 1 play,1 coin 2 play (alternate)
   ON  OFF 2 coin 1 play
   OFF ON  1 coin 2 play
   ON  ON  1 coin 1 play
SW 5   6
   OFF OFF bonus 50000 pts,100000 every
   ON  OFF bonus 30000 pts,50000 every
   OFF ON  bonus 20000 pts,30000 every
   ON  ON  bonus 10000 pts,20000 every
SW 7   8
   OFF OFF 5 lives
   ON  OFF 4 lives
   OFF ON  3 lives
   ON  ON  2 lives
Note: no table-upright mode sw,upright fixed.No picture flip sw too
   

Eprom dump,hw and dip info by tirino73 >isolani1973@libero.it<
Bprom dump by f205v

*/

#include "emu.h"
#include "cpu/z80/z80.h"

class stuntair_state : public driver_device
{
public:
	stuntair_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu") { }

	required_device<cpu_device> m_maincpu;

	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	UINT32 screen_update_stuntair(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
};


static ADDRESS_MAP_START( stuntair_map, AS_PROGRAM, 8, stuntair_state )
	AM_RANGE(0x0000, 0x9fff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( stuntair_soound_map, AS_PROGRAM, 8, stuntair_state )
	AM_RANGE(0x0000, 0x1fff) AM_ROM
ADDRESS_MAP_END

static INPUT_PORTS_START( stuntair )
INPUT_PORTS_END

static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,1),
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8 },
	8*8
};

static const gfx_layout tiles8x8x2_layout =
{
	8,8,
	RGN_FRAC(1,2),
	2,
	{ RGN_FRAC(0,2), RGN_FRAC(1,2) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8 },
	8*8
};


static GFXDECODE_START( stuntair )
	GFXDECODE_ENTRY( "gfx1", 0, tiles8x8_layout, 0, 16 )
	GFXDECODE_ENTRY( "gfx2", 0, tiles8x8x2_layout, 0, 16 )
	GFXDECODE_ENTRY( "gfx3", 0, tiles8x8x2_layout, 0, 16 )
GFXDECODE_END



void stuntair_state::machine_start()
{
}

void stuntair_state::machine_reset()
{
}

void stuntair_state::video_start()
{
}

UINT32 stuntair_state::screen_update_stuntair(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	return 0;
}



static MACHINE_CONFIG_START( stuntair, stuntair_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80,  18432000/4)         /* ? MHz */
	MCFG_CPU_PROGRAM_MAP(stuntair_map)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", stuntair_state,  irq0_line_hold)

	MCFG_CPU_ADD("audiocpu", Z80,  18432000/4)         /* ? MHz */
	MCFG_CPU_PROGRAM_MAP(stuntair_soound_map)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(256, 256)
	MCFG_SCREEN_VISIBLE_AREA(0, 256-1, 16, 256-16-1)
	MCFG_SCREEN_UPDATE_DRIVER(stuntair_state, screen_update_stuntair)

	MCFG_GFXDECODE(stuntair)
	MCFG_PALETTE_LENGTH(0x100)
MACHINE_CONFIG_END



ROM_START( stuntair )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "stuntair.a0", 0x0000, 0x2000, CRC(f61c4a1d) SHA1(29a227b447866e27e7619a0a676c9ad66364c323) )
	ROM_LOAD( "stuntair.a1", 0x2000, 0x2000, CRC(1546f041) SHA1(6a2346edf39700f7b1e609f19e0f2e46b3a78a2a) )
	ROM_LOAD( "stuntair.a3", 0x4000, 0x2000, CRC(63d00b97) SHA1(63efa151147a3c0ac33e226d38aecfd06b36ad38) )
	ROM_LOAD( "stuntair.a4", 0x6000, 0x2000, CRC(01fe2697) SHA1(f7efc6af8047245ad92dff0e62f61abc71a2e9d1) )
	ROM_LOAD( "stuntair.a6", 0x8000, 0x2000, CRC(6704d05c) SHA1(5b1af8be86ffc44ae0207397b33769556ab456df) )

	ROM_REGION( 0x2000, "audiocpu", 0 )
	ROM_LOAD( "stuntair.e14", 0x0000, 0x2000, CRC(641fc9db) SHA1(959a4b6617f840f52d1856e12a9fad8e12293387) )

	ROM_REGION( 0x2000, "gfx1", 0 )
	ROM_LOAD( "stuntair.a9", 0x0000, 0x2000, CRC(bfd861f5) SHA1(ff089ec2e98b21202aeefc31158961e3b1d1ccca) )

	ROM_REGION( 0x4000, "gfx2", 0 )
	ROM_LOAD( "stuntair.a11", 0x0000, 0x2000, CRC(421fef4c) SHA1(e46abead8cd44253cf6da74326f6f3bdd3aa26e5) )
	ROM_LOAD( "stuntair.a12", 0x2000, 0x2000, CRC(e6ee7489) SHA1(6eeea137fc8968e84d2aadbbac982fd9cd161b16) )

	ROM_REGION( 0x4000, "gfx3", 0 )
	ROM_LOAD( "stuntair.a13", 0x0000, 0x2000, CRC(bfdc0d38) SHA1(ea0a22971e9cf1b1682c35facc9c4e30607faed7) )
	ROM_LOAD( "stuntair.a15", 0x2000, 0x2000, CRC(4531cab5) SHA1(35271555377ec3454a5d74bf8c21d7e8acc05782) )

	ROM_REGION( 0x04000, "proms", 0 )
	ROM_LOAD( "dm74s287n.11l", 0x000, 0x100, CRC(6c98f964) SHA1(abf7bdeccd33e62fa106d2056d1949cf278483a7) )
	ROM_LOAD( "dm74s287n.11m", 0x000, 0x100, CRC(d330ff90) SHA1(e223935464109a3c4c7b29641b3736484c22c47a) )
	ROM_LOAD( "dm74s288n.7a",  0x000, 0x020, CRC(5779e751) SHA1(89c955ef8635ad3e9d699f33ec0e4d6c9205d01c) )
ROM_END


GAME( 1983, stuntair,  0,    stuntair, stuntair, driver_device,  0, ROT90, "Nuova Videotron", "Stunt Air", GAME_NO_SOUND | GAME_NOT_WORKING )