/***************************************************************************

                          -= Fuuki 32 Bit Games (FG-3) =-

                driver by Paul Priest and David Haywood
                based on fuukifg2 by Luca Elia

Hardware is similar to FG-2 used for :
"Go Go! Mile Smile", "Susume! Mile Smile (Japan)" & "Gyakuten!! Puzzle Bancho (Japan)"
See fuukifg2.c

Main  CPU   :   M68020

---------------------------------------------------------------------------
Year + Game
---------------------------------------------------------------------------
98  Asura Blade - Sword of Dynasty (Japan)
00  Asura Buster - Eternal Warriors (Japan)

English versions exist, but are not dumped

---------------------------------------------------------------------------

--
Notes so far:

- Dips, need DIP sheet.

- Raster Effects are imperfect, bad frames when lots of new sprites.

- The scroll values are generally wrong when flip screen is on and rasters are often incorrect

Asura Blade
Fuuki Co. Ltd., 1998

PCB Layout
----------

Top Board

FG-3J MAIN-J Revision:1.1
|-----------------------------------------------------|
|  YAC516  YMF278 N341256(x4) N341028 (x4)    FI-002K |
|  33.8688MHz                 N341512(x4)             |
|   PAL N341256                                       |
|   Z80                     N341256                   |
|                           N341256                   |
|J DSW1                                               |
|A                                                    |
|M       12MHz                   FI-003K  N341256(x2) |
|M DSW2                N341256(x3)                    |
|A               PAL                                  |
|         40MHz  PAL                                  |
|  DSW3          PAL  N341256                         |
|         68020  PAL  N341256                         |
|        N341256      28.432MHz    M60067-0901FP      |
|  DSW4  N341256 PAL                                  |
|-----------------------------------------------------|

Notes:
      68020 clock: 20.000MHz
        Z80 clock: 6.000MHz
      YM278 clock: 33.8688MHz
            VSync: 60Hz
            Hsync: 15.81kHz


Bottom Board

FG-3J ROM-J 507KA0301P04       Rev:1.3
|--------------------------------|
|                          SROM  |
|                                |
|  SP01*      SP89         PCM   |
|                                |
|  SP23       SPAB               |
|                                |
|  SP45       SPCD         MAP   |
|                                |
|  SP67       SPEF*        PGM3  |
|                                |
|                          PGM2  |
|                                |
|  BG2123     BG1113       PGM1  |
|                                |
|  BG2022     BG1012       PGM0  |
|--------------------------------|

* = Not populated

****************************************************************************

Asura Buster
Fuuki Co. Ltd.

PCB Layout
----------

Top Board

FG-3J MAIN-J Revision:1.1
|-----------------------------------------------------|
|  YAC516  YMF278 N341256(x4) N341028 (x4)    FI-002K |
|  33.8688MHz                 N341512(x4)             |
|   PAL N341256                                       |
|   Z80                     N341256                   |
|                           N341256                   |
|J DSW1                                               |
|A                                                    |
|M       12MHz                   FI-003K  N341256(x2) |
|M DSW2                N341256(x3)                    |
|A               PAL                                  |
|         40MHz  PAL                                  |
|  DSW3          PAL  N341256                         |
|         68020  PAL  N341256                         |
|        N341256      28.432MHz    M60067-0901FP      |
|  DSW4  N341256 PAL                                  |
|-----------------------------------------------------|

Notes:
      68020 clock: 20.000MHz [40/2]
        Z80 clock: 6.000MHz [12/2]
      YM278 clock: 33.8688MHz
            VSync: 60Hz
            Hsync: 15.81kHz


Bottom Board

FG-3J ROM-J 507KA0301P04       Rev:1.3
|--------------------------------|
|                          SROM  |
|                                |
|  SP01       SP89         OPM   |
|                                |
|  SP23       SPAB               |
|                                |
|  SP45       SPCD         MAP   |
|                                |
|  SP67       SPEF         PGM3  |
|                                |
|                          PGM2  |
|                                |
|  BG2123     BG1113       PGM1  |
|                                |
|  BG2022     BG1012       PGM0  |
|--------------------------------|

***************************************************************************/

/* Define clocks based on actual OSC on the PCB */

#define CPU_CLOCK		(XTAL_40MHz / 2)		/* clock for 68020 */
#define SOUND_CPU_CLOCK		(XTAL_12MHz / 2)		/* clock for Z80 sound CPU */
#define FM_SOUND_CLOCK		(XTAL_33_8688MHz / 2)		/* FM clock */

/* NOTE: YMF278B_STD_CLOCK is defined in /src/emu/sound/ymf278b.h */


#include "driver.h"
#include "sound/262intf.h"
#include "sound/ymf278b.h"

static emu_timer *raster_interrupt_timer;
static UINT8 fuuki32_shared_ram[16];

// Described in src/video/fuuki32.c
extern UINT32 *fuuki32_vram_0, *fuuki32_vram_1;
extern UINT32 *fuuki32_vram_2, *fuuki32_vram_3;
extern UINT32 *fuuki32_vregs, *fuuki32_priority, *fuuki32_tilebank;


/* Functions defined in video: */

WRITE32_HANDLER( fuuki32_vram_0_w );
WRITE32_HANDLER( fuuki32_vram_1_w );
WRITE32_HANDLER( fuuki32_vram_2_w );
WRITE32_HANDLER( fuuki32_vram_3_w );

VIDEO_START( fuuki32 );
VIDEO_UPDATE( fuuki32 );
VIDEO_EOF( fuuki32 );

//UINT32 *fuuki32_shared_ram;

static WRITE32_HANDLER( paletteram32_xRRRRRGGGGGBBBBB_dword_w )
{
	if(ACCESSING_BITS_16_31)
	{
		int r,g,b;
		COMBINE_DATA(&paletteram32[offset]);

		r = (paletteram32[offset] & 0x7c000000) >> (10+16);
		g = (paletteram32[offset] & 0x03e00000) >> (5+16);
		b = (paletteram32[offset] & 0x001f0000) >> (0+16);

		palette_set_color_rgb(machine,offset*2,pal5bit(r),pal5bit(g),pal5bit(b));
	}

	if(ACCESSING_BITS_0_15)
	{
		int r,g,b;
		COMBINE_DATA(&paletteram32[offset]);

		r = (paletteram32[offset] & 0x00007c00) >> (10);
		g = (paletteram32[offset] & 0x000003e0) >> (5);
		b = (paletteram32[offset] & 0x0000001f) >> (0);

		palette_set_color_rgb(machine,offset*2+1,pal5bit(r),pal5bit(g),pal5bit(b));
	}
}

/***************************************************************************


                            Memory Maps - Main CPU


***************************************************************************/

static const char *portnames[] = { "IN0", "IN1", "DSW0", "DSW1" };

#define FUUKI32_INPUT( _N_ ) \
\
static READ32_HANDLER( io32_##_N_##_r ) \
{ \
	return (input_port_read(machine, portnames[ _N_ ]) << 16) | input_port_read(machine, portnames[ _N_ ]); \
}

FUUKI32_INPUT( 0 ) /* $800000.l Coins Inputs */
FUUKI32_INPUT( 1 ) /* $810000.l Player Inputs */
FUUKI32_INPUT( 2 ) /* $880000.l Dipswitches + Service */
FUUKI32_INPUT( 3 ) /* $890000.l More Dipswitches */

/* Sound comms */
static READ32_HANDLER( snd_020_r )
{
	UINT32 retdata = fuuki32_shared_ram[offset*2]<<16 | fuuki32_shared_ram[(offset*2)+1];
	return retdata;
}

static WRITE32_HANDLER( snd_020_w )
{
	if (ACCESSING_BITS_16_23)
	{
		fuuki32_shared_ram[offset*2] = data>>16;
	}

	if (ACCESSING_BITS_0_7)
	{
		fuuki32_shared_ram[(offset*2)+1] = data & 0xff;
	}
}

static WRITE32_HANDLER( fuuki32_vregs_w )
{
	if (fuuki32_vregs[offset] != data)
	{
		COMBINE_DATA(&fuuki32_vregs[offset]);
		if (offset == 0x1c/4)
		{
			const rectangle *visarea = video_screen_get_visible_area(machine->primary_screen);
			attotime period = video_screen_get_frame_period(machine->primary_screen);
			timer_adjust_periodic(raster_interrupt_timer, video_screen_get_time_until_pos(machine->primary_screen, fuuki32_vregs[0x1c/4]>>16, visarea->max_x + 1), 0, period);
		}
	}
}

// Lines with empty comment are for debug only

static ADDRESS_MAP_START( fuuki32_readmem, ADDRESS_SPACE_PROGRAM, 32 )
	AM_RANGE(0x000000, 0x1fffff) AM_READ(SMH_ROM)
	AM_RANGE(0x400000, 0x40ffff) AM_READ(SMH_RAM) // Work RAM
	AM_RANGE(0x410000, 0x41ffff) AM_READ(SMH_RAM) // Work RAM (used by asurabus)
	AM_RANGE(0x500000, 0x501fff) AM_READ(SMH_RAM) // Tilemap 1
	AM_RANGE(0x502000, 0x503fff) AM_READ(SMH_RAM) // Tilemap 2
	AM_RANGE(0x504000, 0x505fff) AM_READ(SMH_RAM) // Tilemap bg
	AM_RANGE(0x506000, 0x507fff) AM_READ(SMH_RAM) // Tilemap bg2
	AM_RANGE(0x508000, 0x517fff) AM_READ(SMH_RAM) // More tilemap, or linescroll? Seems to be empty all of the time
	AM_RANGE(0x600000, 0x601fff) AM_READ(SMH_RAM) // Sprites

	AM_RANGE(0x700000, 0x703fff) AM_READ(SMH_RAM) // Palette

	AM_RANGE(0x800000, 0x800003) AM_READ(io32_0_r) // Coin
	AM_RANGE(0x810000, 0x810003) AM_READ(io32_1_r) // Player Inputs
	AM_RANGE(0x880000, 0x880003) AM_READ(io32_2_r) // Service + DIPS
	AM_RANGE(0x890000, 0x890003) AM_READ(io32_3_r) // More DIPS

	AM_RANGE(0x8c0000, 0x8c001f) AM_READ(SMH_RAM)// Video Registers

/**/AM_RANGE(0x8d0000, 0x8d0003) AM_READ(SMH_RAM) // Flipscreen Related
/**/AM_RANGE(0x8e0000, 0x8e0003) AM_READ(SMH_RAM) // Controls layer order

	AM_RANGE(0x903fe0, 0x903fff) AM_READ(snd_020_r) // Shared with Z80
//  AM_RANGE(0x903fe0, 0x903fe3) AM_READ(fuuki32_sound_command_r) // Shared with Z80
//  AM_RANGE(0x903fe4, 0x903fff) AM_READ(SMH_RAM) // ??
ADDRESS_MAP_END

static ADDRESS_MAP_START( fuuki32_writemem, ADDRESS_SPACE_PROGRAM, 32 )
	AM_RANGE(0x000000, 0x1fffff) AM_WRITE(SMH_ROM)
	AM_RANGE(0x400000, 0x40ffff) AM_WRITE(SMH_RAM) // Work RAM
	AM_RANGE(0x410000, 0x41ffff) AM_WRITE(SMH_RAM) // Work RAM
	AM_RANGE(0x500000, 0x501fff) AM_WRITE(fuuki32_vram_0_w) AM_BASE(&fuuki32_vram_0) // Tilemap 1
	AM_RANGE(0x502000, 0x503fff) AM_WRITE(fuuki32_vram_1_w) AM_BASE(&fuuki32_vram_1) // Tilemap 2
	AM_RANGE(0x504000, 0x505fff) AM_WRITE(fuuki32_vram_2_w) AM_BASE(&fuuki32_vram_2) // Tilemap bg
	AM_RANGE(0x506000, 0x507fff) AM_WRITE(fuuki32_vram_3_w) AM_BASE(&fuuki32_vram_3) // Tilemap bg2
	AM_RANGE(0x508000, 0x517fff) AM_WRITE(SMH_RAM) // More tilemap, or linescroll? Seems to be empty all of the time

	AM_RANGE(0x600000, 0x601fff) AM_WRITE(SMH_RAM) AM_BASE(&spriteram32) AM_SIZE(&spriteram_size	)	// Sprites
	AM_RANGE(0x700000, 0x703fff) AM_WRITE(paletteram32_xRRRRRGGGGGBBBBB_dword_w) AM_BASE(&paletteram32) // Palette

	AM_RANGE(0x800000, 0x800003) AM_WRITE(SMH_NOP) // Clear buffered inputs
	AM_RANGE(0x810000, 0x810003) AM_WRITE(SMH_NOP) // Clear buffered inputs

	AM_RANGE(0x8c0000, 0x8c001f) AM_WRITE(fuuki32_vregs_w) AM_BASE(&fuuki32_vregs)	// Video Registers

	AM_RANGE(0x8d0000, 0x8d0003) AM_WRITE(SMH_RAM) // Flipscreen Related
	AM_RANGE(0x8e0000, 0x8e0003) AM_WRITE(SMH_RAM) AM_BASE(&fuuki32_priority) // Controls layer order

	AM_RANGE(0x903fe0, 0x903fff) AM_WRITE(snd_020_w) // z80 comms

	AM_RANGE(0xa00000, 0xa00003) AM_WRITE(SMH_RAM) AM_BASE(&fuuki32_tilebank)
ADDRESS_MAP_END


/***************************************************************************

                            Memory Maps - Sound CPU

***************************************************************************/

static WRITE8_HANDLER ( fuuki32_sound_bw_w )
{
	UINT8 *rom = memory_region(machine, "sound");

	memory_set_bankptr(1, rom + 0x10000 + (data * 0x8000));
}

static READ8_HANDLER( snd_z80_r )
{
	UINT8 retdata = fuuki32_shared_ram[offset];
	return retdata;
}

static WRITE8_HANDLER( snd_z80_w )
{
	fuuki32_shared_ram[offset] = data;
}

static ADDRESS_MAP_START( fuuki32_sound_readmem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_READ(SMH_ROM		)	// ROM
	AM_RANGE(0x6000, 0x6fff) AM_READ(SMH_RAM		)	// RAM
	AM_RANGE(0x7ff0, 0x7fff) AM_READ(snd_z80_r)
	AM_RANGE(0x8000, 0xffff) AM_READ(SMH_BANK1		)	// ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( fuuki32_sound_writemem, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x5fff) AM_WRITE(SMH_ROM		)	// ROM
	AM_RANGE(0x6000, 0x6fff) AM_WRITE(SMH_RAM		)	// RAM
	AM_RANGE(0x7ff0, 0x7fff) AM_WRITE(snd_z80_w)
	AM_RANGE(0x8000, 0xffff) AM_WRITE(SMH_ROM		)	// ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( fuuki32_sound_readport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x40, 0x40) AM_READ(ymf262_status_0_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( fuuki32_sound_writeport, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_WRITE(fuuki32_sound_bw_w)
	AM_RANGE(0x30, 0x30) AM_WRITE(SMH_NOP)
	AM_RANGE(0x40, 0x40) AM_WRITE(ymf262_register_a_0_w)
	AM_RANGE(0x41, 0x41) AM_WRITE(ymf262_data_a_0_w)
	AM_RANGE(0x42, 0x42) AM_WRITE(ymf262_register_b_0_w)
	AM_RANGE(0x43, 0x43) AM_WRITE(ymf262_data_b_0_w)
	AM_RANGE(0x44, 0x44) AM_WRITE(ymf278b_control_port_0_c_w)
	AM_RANGE(0x45, 0x45) AM_WRITE(ymf278b_data_port_0_c_w)
ADDRESS_MAP_END


/***************************************************************************


                                Input Ports


***************************************************************************/

static INPUT_PORTS_START( asurabld )
	PORT_START("IN0")	// IN0 - $800000.w/$800002.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_COIN1    )
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_COIN2    )
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_START1   )
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_START2   )
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN  )
	PORT_BIT(  0x0100, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT(  0xfe00, IP_ACTIVE_LOW, IPT_UNKNOWN  )

	PORT_START("IN1")	// IN1 - $810000.w/$810002.w
	PORT_BIT(  0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT(  0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT(  0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT(  0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT(  0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT(  0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
	PORT_BIT(  0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT(  0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN                      )
	PORT_BIT(  0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT(  0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT(  0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT(  0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT(  0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT(  0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT(  0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT(  0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN                      )

	PORT_START("DSW0")	// IN2 - $880000.w/$880002.w
	PORT_SERVICE( 0x0001, 0x0001 )
	PORT_DIPNAME( 0x0002, 0x0002, "Blood Colour" ) // Any other censorship? (Tested in 3 locations)
	PORT_DIPSETTING(      0x0002, "Red" )
	PORT_DIPSETTING(      0x0000, "Green" )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) ) // Tested @ 0917AC
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) ) // Tested @ 0917AC
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0030, 0x0030, "Timer" )
	PORT_DIPSETTING(      0x0000, "Slow" )
	PORT_DIPSETTING(      0x0030, DEF_STR( Medium ) )
	PORT_DIPSETTING(      0x0010, "Fast" )
	PORT_DIPSETTING(      0x0020, "Very Fast" )
	PORT_DIPNAME( 0x00c0, 0x0000, "Coinage Mode" )
	PORT_DIPSETTING(      0x00c0, "Split" )
	PORT_DIPSETTING(      0x0000, "Joint" )
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW1")	// IN3 - $890000.w/$890002.w
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x000e, 0x0000, "Computer Level" ) // See @ 0917CC
	PORT_DIPSETTING(      0x0000, "0" )
	PORT_DIPSETTING(      0x0008, "1" )
	PORT_DIPSETTING(      0x0004, "2" )
	PORT_DIPSETTING(      0x000c, "3" )
	PORT_DIPSETTING(      0x000e, "4" )
	PORT_DIPSETTING(      0x0002, "5" )
	PORT_DIPSETTING(      0x000a, "6" )
	PORT_DIPSETTING(      0x0006, "7" )
	PORT_DIPNAME( 0x0030, 0x0010, "Damage" )
	PORT_DIPSETTING(      0x0020, "Lowest" )
	PORT_DIPSETTING(      0x0030, DEF_STR( Low ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Medium ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( High ) )
	PORT_DIPNAME( 0x00c0, 0x0040, "Max Rounds" )
	PORT_DIPSETTING(      0x0000, "1" )
	PORT_DIPSETTING(      0x0040, "3" )
	PORT_DIPSETTING(      0x0080, "5" )
	PORT_DIPNAME( 0xf000, 0xf000, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( 8C_1C ) )
	PORT_DIPSETTING(      0x9000, DEF_STR( 7C_1C ) )
	PORT_DIPSETTING(      0xa000, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING(      0xb000, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(      0xc000, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0xd000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0xe000, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0xf000, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) ) // Set both for Free Play
	PORT_DIPNAME( 0x0f00, 0x0f00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( 8C_1C ) )
	PORT_DIPSETTING(      0x0900, DEF_STR( 7C_1C ) )
	PORT_DIPSETTING(      0x0a00, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING(      0x0b00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(      0x0c00, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(      0x0d00, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0e00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0f00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) ) // Set both for Free Play

INPUT_PORTS_END


/***************************************************************************


                            Graphics Layouts


***************************************************************************/

/* 8x8x4 */
static const gfx_layout layout_8x8x4 =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ STEP4(0,1) },
	{ 2*4,3*4,   0*4,1*4,   6*4,7*4, 4*4,5*4 },
	{ STEP8(0,8*4) },
	8*8*4
};

/* 16x16x4 */
static const gfx_layout layout_16x16x4 =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ STEP4(0,1) },
	{	2*4,3*4,   0*4,1*4,   6*4,7*4, 4*4,5*4,
		10*4,11*4, 8*4,9*4, 14*4,15*4, 12*4,13*4	},
	{ STEP16(0,16*4) },
	16*16*4
};

/* 16x16x8 */
static const gfx_layout layout_16x16x8 =
{
	16,16,
	RGN_FRAC(1,2),
	8,
	{ STEP4(RGN_FRAC(1,2),1), STEP4(0,1) },
	{	2*4,3*4,   0*4,1*4,   6*4,7*4, 4*4,5*4,
		10*4,11*4, 8*4,9*4, 14*4,15*4, 12*4,13*4	},
	{ STEP16(0,16*4) },
	16*16*4
};

static GFXDECODE_START( fuuki32 )
	GFXDECODE_ENTRY( "gfx1", 0, layout_16x16x4, 0x400*2, 0x40 ) // [0] Sprites
	GFXDECODE_ENTRY( "gfx2", 0, layout_16x16x8, 0x400*0, 0x40 ) // [1] Layer 1
	GFXDECODE_ENTRY( "gfx3", 0, layout_16x16x8, 0x400*1, 0x40 ) // [2] Layer 2
	GFXDECODE_ENTRY( "gfx4", 0, layout_8x8x4,   0x400*3, 0x40 ) // [3] BG Layer
	GFXDECODE_ENTRY( "gfx4", 0, layout_8x8x4,   0x400*3, 0x40 ) // [4] BG Layer 2 (GFX4!)
GFXDECODE_END


/***************************************************************************


                                Machine Drivers


***************************************************************************/

static TIMER_CALLBACK( level_1_interrupt_callback )
{
	cpunum_set_input_line(machine, 0, 1, PULSE_LINE);
	timer_set(video_screen_get_time_until_pos(machine->primary_screen, 248, 0), NULL, 0, level_1_interrupt_callback);
}


static TIMER_CALLBACK( vblank_interrupt_callback )
{
	cpunum_set_input_line(machine, 0, 3, PULSE_LINE);	// VBlank IRQ
	timer_set(video_screen_get_time_until_vblank_start(machine->primary_screen), NULL, 0, vblank_interrupt_callback);
}


static TIMER_CALLBACK( raster_interrupt_callback )
{
	cpunum_set_input_line(machine, 0, 5, PULSE_LINE);	// Raster Line IRQ
	video_screen_update_partial(machine->primary_screen, video_screen_get_vpos(machine->primary_screen));
	timer_adjust_oneshot(raster_interrupt_timer, video_screen_get_frame_period(machine->primary_screen), 0);
}


static MACHINE_START( fuuki32 )
{
	raster_interrupt_timer = timer_alloc(raster_interrupt_callback, NULL);
}


static MACHINE_RESET( fuuki32 )
{
	const rectangle *visarea = video_screen_get_visible_area(machine->primary_screen);

	timer_set(video_screen_get_time_until_pos(machine->primary_screen, 248, 0), NULL, 0, level_1_interrupt_callback);
	timer_set(video_screen_get_time_until_vblank_start(machine->primary_screen), NULL, 0, vblank_interrupt_callback);
	timer_adjust_oneshot(raster_interrupt_timer, video_screen_get_time_until_pos(machine->primary_screen, 0, visarea->max_x + 1), 0);
}


static void irqhandler(running_machine *machine, int irq)
{
	cpunum_set_input_line(machine, 1, 0, irq ? ASSERT_LINE : CLEAR_LINE);
}

static const ymf262_interface fuuki32_ymf262_interface =
{
	irqhandler		/* irq */
};

static MACHINE_DRIVER_START( fuuki32 )

	/* basic machine hardware */
	MDRV_CPU_ADD("main", M68EC020, CPU_CLOCK) /* 20MHz verified */
	MDRV_CPU_PROGRAM_MAP(fuuki32_readmem,fuuki32_writemem)

	MDRV_CPU_ADD("sound", Z80, SOUND_CPU_CLOCK) /* 6MHz verified */
	MDRV_CPU_PROGRAM_MAP(fuuki32_sound_readmem,fuuki32_sound_writemem)
	MDRV_CPU_IO_MAP(fuuki32_sound_readport,fuuki32_sound_writeport)

	MDRV_MACHINE_START(fuuki32)
	MDRV_MACHINE_RESET(fuuki32)

	/* video hardware */
	//MDRV_VIDEO_ATTRIBUTES(VIDEO_BUFFERS_SPRITERAM) // Buffered by 2 frames

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0, 40*8-1, 0, 30*8-1)

	MDRV_GFXDECODE(fuuki32)
	MDRV_PALETTE_LENGTH(0x4000/2)

	MDRV_VIDEO_START(fuuki32)
	MDRV_VIDEO_UPDATE(fuuki32)
	MDRV_VIDEO_EOF(fuuki32)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD("ymf1", YMF262, FM_SOUND_CLOCK) /* 33.8688MHz OSC divided by 2 is 16.9344MHz */
	MDRV_SOUND_CONFIG(fuuki32_ymf262_interface)
	MDRV_SOUND_ROUTE(0, "left", 0.50)
	MDRV_SOUND_ROUTE(1, "right", 0.50)
	MDRV_SOUND_ROUTE(2, "left", 0.50)
	MDRV_SOUND_ROUTE(3, "right", 0.50)

	MDRV_SOUND_ADD("ymf2", YMF278B, YMF278B_STD_CLOCK) /* YMF278B_STD_CLOCK = OSC 33.8688MHz */
	MDRV_SOUND_ROUTE(0, "left", 0.50)
	MDRV_SOUND_ROUTE(1, "right", 0.50)
MACHINE_DRIVER_END

/***************************************************************************


                                ROMs Loading


***************************************************************************/

/***************************************************************************

                 Asura Blade - Sword of Dynasty (Japan)

Fuuki, 1999   Consists of a FG-3J MAIN-J mainboard &  FG-3J ROM-J combo

***************************************************************************/

ROM_START( asurabld )
	ROM_REGION( 0x200000, "main", 0 ) /* M68020 */
	ROM_LOAD32_BYTE( "pgm3.u1", 0x000000, 0x80000, CRC(053e9758) SHA1(c2754d3f0c607c81c8fa33b667b576eb0474fd0b) )
	ROM_LOAD32_BYTE( "pgm2.u2", 0x000001, 0x80000, CRC(16b656ca) SHA1(5ffb551ce7dec462d3896f0fed693454496894bc) )
	ROM_LOAD32_BYTE( "pgm1.u3", 0x000002, 0x80000, CRC(35104452) SHA1(03cfd81429f8a945d5419c9750925bfa997d0607) )
	ROM_LOAD32_BYTE( "pgm0.u4", 0x000003, 0x80000, CRC(68615497) SHA1(de93751f151f195a863dc6fe83b6e7ed8f99430a) )

	ROM_REGION( 0x090000, "sound", 0 ) /* Z80 */
	ROM_LOAD( "srom.u7", 0x00000, 0x80000, CRC(bb1deb89) SHA1(b1c70abddc0b9a88beb69a592376ff69a7e091eb) )
	ROM_RELOAD(          0x10000, 0x80000) /* for banks */

	ROM_REGION( 0x2000000, "gfx1", ROMREGION_DISPOSE )
	/* 0x0000000 - 0x03fffff empty */ /* spXX.uYY - XX is the bank number! */
	ROM_LOAD( "sp23.u14", 0x0400000, 0x400000, CRC(7df492eb) SHA1(30b88a3cd025ffc8c28fef06e0784755be37ef8e) )
	ROM_LOAD( "sp45.u15", 0x0800000, 0x400000, CRC(1890f42a) SHA1(22254fe38fd83f4602a25e1ccba32df16edaf3f9) )
	ROM_LOAD( "sp67.u16", 0x0c00000, 0x400000, CRC(a48f1ef0) SHA1(bf8787f293793291a503af662d3738c007654726) )
	ROM_LOAD( "sp89.u17", 0x1000000, 0x400000, CRC(6b024362) SHA1(8be5cc3c7306d28b75acd970bb3be6d3c9825367) )
	ROM_LOAD( "spab.u18", 0x1400000, 0x400000, CRC(803d2d8c) SHA1(25df30689e576a0620656c721d92bcc3fbd84844) )
	ROM_LOAD( "spcd.u19", 0x1800000, 0x400000, CRC(42e5c26e) SHA1(b68875d353bdc5d49113bbac02fd83508bce66a5) )

	ROM_REGION( 0x0800000, "gfx2", ROMREGION_DISPOSE )
	ROM_LOAD( "bg1012.u22", 0x0000000, 0x400000, CRC(d717a0a1) SHA1(007df309dc0650ca07e077b983a2b05730349d0b) )
	ROM_LOAD( "bg1113.u23", 0x0400000, 0x400000, CRC(94338267) SHA1(7848bc57cb0eac216100a508763451eb57a0a082) )

	ROM_REGION( 0x0800000, "gfx3", ROMREGION_DISPOSE )
	ROM_LOAD( "bg2022.u25", 0x0000000, 0x400000, CRC(ee312cd3) SHA1(2ef9d51928d80375daf8e6b204bb66a8b9cbaee7) )
	ROM_LOAD( "bg2123.u24", 0x0400000, 0x400000, CRC(4acfc469) SHA1(a98d06b967ebb3fa3b4c8aa3d7a05063ec981fb2) )

	ROM_REGION( 0x200000, "gfx4", ROMREGION_DISPOSE ) // background tiles
	ROM_LOAD( "map.u5", 0x00000, 0x200000, CRC(e681155e) SHA1(458845b9c86df72685d92d0d4052aacc2fa7d1bd) )

	ROM_REGION( 0x400000, "ymf1", 0 ) // OPL4 samples
	ROM_LOAD( "pcm.u6", 0x00000, 0x400000, CRC(ac72225a) SHA1(8d16399ed34ac5bd69dbf43b2de2b0db9ac1c610) )

	ROM_REGION( 0x400000, "ymf2", 0 ) // OPL4 samples
	ROM_LOAD( "pcm.u6", 0x00000, 0x400000, CRC(ac72225a) SHA1(8d16399ed34ac5bd69dbf43b2de2b0db9ac1c610) )
ROM_END

/***************************************************************************

                 Asura Buster - Eternal Warriors (Japan)

Fuuki, 2000   Consists of a FG-3J MAIN-J mainboard &  FG-3J ROM-J combo

***************************************************************************/

ROM_START( asurabus )
	ROM_REGION( 0x200000, "main", 0 ) /* M68020 */
	ROM_LOAD32_BYTE( "pgm3.u1", 0x000000, 0x80000, CRC(2c6b5271) SHA1(188371f1f003823ac719e962e048719d76696b2f) )
	ROM_LOAD32_BYTE( "pgm2.u2", 0x000001, 0x80000, CRC(8f8694ec) SHA1(3334df4aecc5ab2f8914ef6748c027a99b39ce26) )
	ROM_LOAD32_BYTE( "pgm1.u3", 0x000002, 0x80000, CRC(0a040f0f) SHA1(d5e86d33efcbbde7ee62cfc8dfe867f250a33415) )
	ROM_LOAD32_BYTE( "pgm0.u4", 0x000003, 0x80000, CRC(9b71e9d8) SHA1(9b705b5b6fff549f5679890422b481b5cf1d7bd7) )

	ROM_REGION( 0x090000, "sound", 0 ) /* Z80 */
	ROM_LOAD( "srom.u7", 0x00000, 0x80000, CRC(368da389) SHA1(1423b709da40bf3033c9032c4bd07658f1a969de) )
	ROM_RELOAD(          0x10000, 0x80000) /* for banks */

	ROM_REGION( 0x2000000, "gfx1", ROMREGION_DISPOSE )
	ROM_LOAD( "sp01.u13", 0x0000000, 0x400000, CRC(5edea463) SHA1(22a780912f060bae0c9a403a7bfd4d27f25b76e3) )
	ROM_LOAD( "sp23.u14", 0x0400000, 0x400000, CRC(91b1b0de) SHA1(341367966559ef2027415b673eb0db704680c81f) )
	ROM_LOAD( "sp45.u15", 0x0800000, 0x400000, CRC(96c69aac) SHA1(cf053523026651427f884b9dd7c095af362dd24e) )
	ROM_LOAD( "sp67.u16", 0x0c00000, 0x400000, CRC(7c3d83bf) SHA1(7188dd923c6c7eb6aee3323e7ab54aa240c35ea3) )
	ROM_LOAD( "sp89.u17", 0x1000000, 0x400000, CRC(cb1e14f8) SHA1(941cea1887d7ceb52222adcf1d6913969e6163aa) )
	ROM_LOAD( "spab.u18", 0x1400000, 0x400000, CRC(e5a4608d) SHA1(b8e39f53e0b7ad1e16ae9c3726597776b404be1c) )
	ROM_LOAD( "spcd.u19", 0x1800000, 0x400000, CRC(99bfbe32) SHA1(926a8afc4a175874f22f53300e76f59331d3b9ba) )
	ROM_LOAD( "spef.u20", 0x1c00000, 0x400000, CRC(c9c799cc) SHA1(01373316700d8688deeea2e9e8f831d5f86c7f17) )

	ROM_REGION( 0x0800000, "gfx2", ROMREGION_DISPOSE )
	ROM_LOAD( "bg1012.u22", 0x0000000, 0x400000, CRC(e3fb9af0) SHA1(11900cc2873337692f66fb4f1eb9c574e5a967de) )
	ROM_LOAD( "bg1113.u23", 0x0400000, 0x400000, CRC(5f8657e6) SHA1(7c2854dc5d2d4efe55bda01e329da051350e0031) )

	ROM_REGION( 0x0800000, "gfx3", ROMREGION_DISPOSE )
	ROM_LOAD( "bg2022.u25", 0x0000000, 0x400000, CRC(f46eda52) SHA1(46530016b32a164bd76c4f53e7b53b2beb28db06) )
	ROM_LOAD( "bg2123.u24", 0x0400000, 0x400000, CRC(c4ebb86b) SHA1(a7093e6e02b64566d277cbbd5fa90cd430e7c8a0) )

	ROM_REGION( 0x200000, "gfx4", ROMREGION_DISPOSE ) // background tiles
	ROM_LOAD( "map.u5", 0x00000, 0x200000, CRC(bd179dc5) SHA1(ce3fcac573b14fd5365eb5dcec3257e439d2c129) )

	ROM_REGION( 0x400000, "ymf1", 0 ) // OPL4 samples
	ROM_LOAD( "opm.u6", 0x00000, 0x400000, CRC(31b05be4) SHA1(d0f4f387f84a74591224b0f42b7f5c538a3dc498) )

	ROM_REGION( 0x400000, "ymf2", 0 ) // OPL4 samples
	ROM_LOAD( "opm.u6", 0x00000, 0x400000, CRC(31b05be4) SHA1(d0f4f387f84a74591224b0f42b7f5c538a3dc498) )
ROM_END



/***************************************************************************


                                Game Drivers


***************************************************************************/

GAME( 1998, asurabld,	0, fuuki32, asurabld, 0, ROT0, "Fuuki", "Asura Blade - Sword of Dynasty (Japan)", GAME_IMPERFECT_GRAPHICS )
GAME( 2000, asurabus,	0, fuuki32, asurabld, 0, ROT0, "Fuuki", "Asura Buster - Eternal Warriors (Japan)", GAME_IMPERFECT_GRAPHICS|GAME_IMPERFECT_SOUND ) // sounds loop forever?
