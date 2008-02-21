/******************************************************************

    Double Dragon 3                     Technos Japan Corp 1990
    The Combatribes                     Technos Japan Corp 1990


    Notes:

    Both games have original and bootleg versions supported.
    Double Dragon 3 bootleg has some misplaced graphics, but I
    think this is how the real thing would look.
    Combatribes has sprite lag but it seems to be caused by poor
    programming and I think the original does the same.

******************************************************************/

/*

    TODO:

    - coin counters/lockouts

*/


#include "driver.h"
#include "deprecat.h"
#include "cpu/z80/z80.h"
#include "cpu/m68000/m68000.h"
#include "sound/2151intf.h"
#include "sound/okim6295.h"


extern UINT16 *ddragon3_bg_videoram16;
extern UINT16 *ddragon3_fg_videoram16;
extern UINT16 ddragon3_vreg;

extern WRITE16_HANDLER( ddragon3_bg_videoram16_w );
extern WRITE16_HANDLER( ddragon3_fg_videoram16_w );
extern WRITE16_HANDLER( ddragon3_scroll16_w );
extern READ16_HANDLER( ddragon3_scroll16_r );

extern VIDEO_START( ddragon3 );
extern VIDEO_UPDATE( ddragon3 );
extern VIDEO_UPDATE( ctribe );

/* Read/Write Handlers */

static WRITE8_HANDLER( oki_bankswitch_w )
{
	OKIM6295_set_bank_base(0, (data & 1) * 0x40000);
}

static WRITE16_HANDLER( ddragon3_io16_w )
{
	static UINT16 reg[8];

	COMBINE_DATA(&reg[offset]);

	switch (offset)
	{
		case 0:
		ddragon3_vreg = reg[0];
		break;

		case 1: /* soundlatch_w */
		soundlatch_w(1,reg[1]&0xff);
		cpunum_set_input_line(Machine, 1, INPUT_LINE_NMI, PULSE_LINE );
		break;

		case 2:
		/*  this gets written to on startup and at the end of IRQ6
        **  possibly trigger IRQ on sound CPU
        */
		break;

		case 3:
		/*  this gets written to on startup,
        **  and at the end of IRQ5 (input port read) */
		break;

		case 4:
		/* this gets written to at the end of IRQ6 only */
		break;

		default:
		logerror("OUTPUT 1400[%02x] %08x, pc=%06x \n", offset,(unsigned)data, activecpu_get_pc() );
		break;
	}
}

/* Memory Maps */

static ADDRESS_MAP_START( readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x080000, 0x080fff) AM_READ(MRA16_RAM)	/* Foreground (32x32 Tiles - 4 by per tile) */
	AM_RANGE(0x082000, 0x0827ff) AM_READ(MRA16_RAM)	/* Background (32x32 Tiles - 2 by per tile) */
	AM_RANGE(0x100000, 0x100001) AM_READ(input_port_0_word_r)
	AM_RANGE(0x100002, 0x100003) AM_READ(input_port_1_word_r)
	AM_RANGE(0x100004, 0x100005) AM_READ(input_port_2_word_r)
	AM_RANGE(0x100006, 0x100007) AM_READ(input_port_3_word_r)
	AM_RANGE(0x140000, 0x1405ff) AM_READ(MRA16_RAM)	/* Palette RAM */
	AM_RANGE(0x180000, 0x180fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x1c0000, 0x1c3fff) AM_READ(MRA16_RAM)	/* working RAM */
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x080000, 0x080fff) AM_WRITE(ddragon3_fg_videoram16_w) AM_BASE(&ddragon3_fg_videoram16)
	AM_RANGE(0x082000, 0x0827ff) AM_WRITE(ddragon3_bg_videoram16_w) AM_BASE(&ddragon3_bg_videoram16)
	AM_RANGE(0x0c0000, 0x0c000f) AM_WRITE(ddragon3_scroll16_w)
	AM_RANGE(0x100000, 0x10000f) AM_WRITE(ddragon3_io16_w)
	AM_RANGE(0x140000, 0x1405ff) AM_WRITE(paletteram16_xBBBBBGGGGGRRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x180000, 0x180fff) AM_WRITE(MWA16_RAM) AM_BASE(&spriteram16) /* Sprites (16 bytes per sprite) */
	AM_RANGE(0x1c0000, 0x1c3fff) AM_WRITE(MWA16_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( dd3b_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x080000, 0x080fff) AM_READ(MRA16_RAM)	/* Foreground (32x32 Tiles - 4 by per tile) */
	AM_RANGE(0x081000, 0x081fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x082000, 0x0827ff) AM_READ(MRA16_RAM)	/* Background (32x32 Tiles - 2 by per tile) */
	AM_RANGE(0x100000, 0x1005ff) AM_READ(MRA16_RAM)	/* Palette RAM */
	AM_RANGE(0x180000, 0x180001) AM_READ(input_port_0_word_r)
	AM_RANGE(0x180002, 0x180003) AM_READ(input_port_1_word_r)
	AM_RANGE(0x180004, 0x180005) AM_READ(input_port_2_word_r)
	AM_RANGE(0x180006, 0x180007) AM_READ(input_port_3_word_r)
	AM_RANGE(0x1c0000, 0x1c3fff) AM_READ(MRA16_RAM)	/* working RAM */
ADDRESS_MAP_END

static ADDRESS_MAP_START( dd3b_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x080000, 0x080fff) AM_WRITE(ddragon3_fg_videoram16_w) AM_BASE(&ddragon3_fg_videoram16)
	AM_RANGE(0x081000, 0x081fff) AM_WRITE(MWA16_RAM) AM_BASE(&spriteram16) /* Sprites (16 bytes per sprite) */
	AM_RANGE(0x082000, 0x0827ff) AM_WRITE(ddragon3_bg_videoram16_w) AM_BASE(&ddragon3_bg_videoram16)
	AM_RANGE(0x0c0000, 0x0c000f) AM_WRITE(ddragon3_scroll16_w)
	AM_RANGE(0x100000, 0x1005ff) AM_WRITE(paletteram16_xBBBBBGGGGGRRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x140000, 0x14000f) AM_WRITE(ddragon3_io16_w)
	AM_RANGE(0x1c0000, 0x1c3fff) AM_WRITE(MWA16_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( ctribe_readmem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_READ(MRA16_ROM)
	AM_RANGE(0x080000, 0x080fff) AM_READ(MRA16_RAM)	/* Foreground (32x32 Tiles - 4 by per tile) */
	AM_RANGE(0x081000, 0x081fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x082000, 0x0827ff) AM_READ(MRA16_RAM)	/* Background (32x32 Tiles - 2 by per tile) */
	AM_RANGE(0x082800, 0x082fff) AM_READ(MRA16_RAM)
	AM_RANGE(0x0c0000, 0x0c000f) AM_READ(ddragon3_scroll16_r)
	AM_RANGE(0x100000, 0x1005ff) AM_READ(MRA16_RAM)	/* Palette RAM */
	AM_RANGE(0x180000, 0x180001) AM_READ(input_port_0_word_r)
	AM_RANGE(0x180002, 0x180003) AM_READ(input_port_1_word_r)
	AM_RANGE(0x180004, 0x180005) AM_READ(input_port_2_word_r)
	AM_RANGE(0x180006, 0x180007) AM_READ(input_port_3_word_r)
	AM_RANGE(0x1c0000, 0x1c3fff) AM_READ(MRA16_RAM)	/* working RAM */
ADDRESS_MAP_END

static ADDRESS_MAP_START( ctribe_writemem, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x07ffff) AM_WRITE(MWA16_ROM)
	AM_RANGE(0x080000, 0x080fff) AM_WRITE(ddragon3_fg_videoram16_w) AM_BASE(&ddragon3_fg_videoram16)
	AM_RANGE(0x081000, 0x081fff) AM_WRITE(MWA16_RAM) AM_BASE(&spriteram16) /* Sprites (16 bytes per sprite) */
	AM_RANGE(0x082000, 0x0827ff) AM_WRITE(ddragon3_bg_videoram16_w) AM_BASE(&ddragon3_bg_videoram16)
	AM_RANGE(0x082800, 0x082fff) AM_WRITE(MWA16_RAM)
	AM_RANGE(0x0c0000, 0x0c000f) AM_WRITE(ddragon3_scroll16_w)
	AM_RANGE(0x100000, 0x1005ff) AM_WRITE(paletteram16_xxxxBBBBGGGGRRRR_word_w) AM_BASE(&paletteram16)
	AM_RANGE(0x140000, 0x14000f) AM_WRITE(ddragon3_io16_w)
	AM_RANGE(0x1c0000, 0x1c3fff) AM_WRITE(MWA16_RAM)
ADDRESS_MAP_END

static ADDRESS_MAP_START( readmem_sound, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_READ(MRA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_READ(MRA8_RAM)
	AM_RANGE(0xc801, 0xc801) AM_READ(YM2151_status_port_0_r)
	AM_RANGE(0xd800, 0xd800) AM_READ(OKIM6295_status_0_r)
	AM_RANGE(0xe000, 0xe000) AM_READ(soundlatch_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( writemem_sound, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0xbfff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0xc000, 0xc7ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0xc800, 0xc800) AM_WRITE(YM2151_register_port_0_w)
	AM_RANGE(0xc801, 0xc801) AM_WRITE(YM2151_data_port_0_w)
	AM_RANGE(0xd800, 0xd800) AM_WRITE(OKIM6295_data_0_w)
	AM_RANGE(0xe800, 0xe800) AM_WRITE(oki_bankswitch_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( ctribe_readmem_sound, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_READ(MRA8_ROM)
	AM_RANGE(0x8000, 0x87ff) AM_READ(MRA8_RAM)
	AM_RANGE(0x8801, 0x8801) AM_READ(YM2151_status_port_0_r)
	AM_RANGE(0x9800, 0x9800) AM_READ(OKIM6295_status_0_r)
	AM_RANGE(0xa000, 0xa000) AM_READ(soundlatch_r)
ADDRESS_MAP_END

static ADDRESS_MAP_START( ctribe_writemem_sound, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_WRITE(MWA8_ROM)
	AM_RANGE(0x8000, 0x87ff) AM_WRITE(MWA8_RAM)
	AM_RANGE(0x8800, 0x8800) AM_WRITE(YM2151_register_port_0_w)
	AM_RANGE(0x8801, 0x8801) AM_WRITE(YM2151_data_port_0_w)
	AM_RANGE(0x9800, 0x9800) AM_WRITE(OKIM6295_data_0_w)
ADDRESS_MAP_END

/* Input Ports */

static INPUT_PORTS_START( ddragon3 )
	PORT_START_TAG("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 )	// punch
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 )	// jump
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 )	// kick
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )

	PORT_START_TAG("IN2")
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x00f8, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START // DSW1&2
	PORT_DIPNAME( 0x0003, 0x0003, DEF_STR( Coinage ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(	  0x0001, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	  0x0003, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	  0x0002, DEF_STR( 1C_2C ) )
	PORT_BIT( 0x000c, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x0010, 0x0010, "Continue Discount" )
	PORT_DIPSETTING(	  0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(	  0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(	  0x0200, DEF_STR( Easy ) )
	PORT_DIPSETTING(	  0x0300, DEF_STR( Normal ) )
	PORT_DIPSETTING(	  0x0100, DEF_STR( Hard ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0400, 0x0400, "Player Vs. Player Damage" )
	PORT_DIPSETTING(	  0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE( 0x1000, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x2000, 0x2000, "Stage Clear Energy" )
	PORT_DIPSETTING(	  0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x2000, "50" )
	PORT_DIPNAME( 0x4000, 0x4000, "Starting Energy" )
	PORT_DIPSETTING(	  0x0000, "200" )
	PORT_DIPSETTING(	  0x4000, "230" )
	PORT_DIPNAME( 0x8000, 0x0000, DEF_STR( Players ) )
	PORT_DIPSETTING(	  0x8000, "2" )
	PORT_DIPSETTING(	  0x0000, "3" )

	PORT_START_TAG("IN3")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( ddrago3b )
	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 )	// punch
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 )	// jump
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 )	// kick
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(	  0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )
	PORT_BIT( 0xe000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START2 )

	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Coinage ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(	  0x0100, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	  0x0300, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	  0x0200, DEF_STR( 1C_2C ) )
	PORT_BIT( 0x0c00, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x1000, 0x1000, "Continue Discount" )
	PORT_DIPSETTING(	  0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x2000, DEF_STR( On ) )
	PORT_BIT( 0xc000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(3)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START3 )

	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(	  0x0200, DEF_STR( Easy ) )
	PORT_DIPSETTING(	  0x0300, DEF_STR( Normal ) )
	PORT_DIPSETTING(	  0x0100, DEF_STR( Hard ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0400, 0x0400, "Player Vs. Player Damage" )
	PORT_DIPSETTING(	  0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE( 0x1000, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x2000, 0x2000, "Stage Clear Energy" )
	PORT_DIPSETTING(	  0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x2000, "50" )
	PORT_BIT( 0xc000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_DIPNAME( 0x0100, 0x0100, "Starting Energy" )
	PORT_DIPSETTING(	0x0000, "200" )
	PORT_DIPSETTING(	0x0100, "230" )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Players ) )
	PORT_DIPSETTING(	0x0200, "2" )
	PORT_DIPSETTING(	0x0000, "3" )
	PORT_BIT( 0xfc00, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( ctribe )
	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 )	// punch
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 )	// jump
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )

	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_VBLANK )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(	  0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )
	PORT_BIT( 0xe000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START2 )

	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Coinage ) )     PORT_DIPLOCATION("SW1:1,2")
	PORT_DIPSETTING(	  0x0000, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(	  0x0100, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	  0x0300, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	  0x0200, DEF_STR( 1C_2C ) )
	PORT_BIT( 0x0c00, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x1000, 0x1000, "Continue Discount" )    PORT_DIPLOCATION("SW1:5")
	PORT_DIPSETTING(	  0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(	  0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x2000, DEF_STR( On ) )
	PORT_BIT( 0xc000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START3 )

	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Difficulty ) )  PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(	  0x0200, DEF_STR( Easy ) )
	PORT_DIPSETTING(	  0x0300, DEF_STR( Normal ) )
	PORT_DIPSETTING(	  0x0100, "Less than Difficult" )
	PORT_DIPSETTING(	  0x0000, "Difficult" )
	PORT_DIPNAME( 0x0400, 0x0400, "Timer Speed" )          PORT_DIPLOCATION("SW2:3")
	PORT_DIPSETTING(	  0x0400, DEF_STR( Normal ) )
	PORT_DIPSETTING(	  0x0000, "Fast" )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x1000, 0x1000, "Test Mode" )            PORT_DIPLOCATION("SW2:5")
	PORT_DIPSETTING(	  0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(	  0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x6000, 0x6000, "Stage Clear Energy" )   PORT_DIPLOCATION("SW2:6,7")
	PORT_DIPSETTING(	  0x6000, "0" )
	PORT_DIPSETTING(	  0x4000, "50" )
	PORT_DIPSETTING(	  0x2000, "100" ) /* According to manual but doesn't work on real pcb. */
	PORT_DIPSETTING(	  0x0000, "150" ) /* According to manual but doesn't work on real pcb. */
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x00ff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Players ) )     PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(	0x0200, "2" )
	PORT_DIPSETTING(	0x0000, "3" )
	PORT_BIT( 0xc000, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

/* Graphics Layouts */

static const gfx_layout tile_layout =
{
	16,16,	/* 16*16 tiles */
	8192,	/* 8192 tiles */
	4,	/* 4 bits per pixel */
	{ 0, 0x40000*8, 2*0x40000*8 , 3*0x40000*8 },	/* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7,
			16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	32*8	/* every tile takes 32 consecutive bytes */
};

static const gfx_layout sprite_layout = {
	16,16,	/* 16*16 tiles */
	0x90000/32, /* 4096 tiles */
	4,	/* 4 bits per pixel */
	{ 0, 0x100000*8, 2*0x100000*8 , 3*0x100000*8 }, /* the bitplanes are separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7,
		16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
	32*8	/* every tile takes 32 consecutive bytes */
};

/* Graphics Decode Info */

static GFXDECODE_START( ddragon3 )
	GFXDECODE_ENTRY( REGION_GFX1, 0, tile_layout,   256, 32 )
	GFXDECODE_ENTRY( REGION_GFX2, 0, sprite_layout,	0, 16 )
GFXDECODE_END

/* Sound Interfaces */

static void dd3_ymirq_handler(int irq)
{
	cpunum_set_input_line(Machine, 1, 0 , irq ? ASSERT_LINE : CLEAR_LINE );
}

static const struct YM2151interface ym2151_interface =
{
	dd3_ymirq_handler
};

/* Interrupt Generators */

static INTERRUPT_GEN( ddragon3_cpu_interrupt ) { /* 6:0x177e - 5:0x176a */
	if( cpu_getiloops() == 0 ){
		cpunum_set_input_line(machine, 0, 6, HOLD_LINE);  /* VBlank */
	}
	else {
		cpunum_set_input_line(machine, 0, 5, HOLD_LINE); /* Input Ports */
	}
}

/* Machine Drivers */

static MACHINE_DRIVER_START( ddragon3 )
	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", M68000, 12000000) // Guess
	MDRV_CPU_PROGRAM_MAP(readmem, writemem)
	MDRV_CPU_VBLANK_INT(ddragon3_cpu_interrupt, 2)

	MDRV_CPU_ADD_TAG("audio", Z80, 3579545) // Guess
	/* audio CPU */
	MDRV_CPU_PROGRAM_MAP(readmem_sound, writemem_sound)

	/* video hardware */
	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_SCREEN_VISIBLE_AREA(0*8, 40*8-1, 1*8, 31*8-1)

	MDRV_GFXDECODE(ddragon3)
	MDRV_PALETTE_LENGTH(768)

	MDRV_VIDEO_START(ddragon3)
	MDRV_VIDEO_UPDATE(ddragon3)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_STEREO("left", "right")

	MDRV_SOUND_ADD(YM2151, 3579545)
	MDRV_SOUND_CONFIG(ym2151_interface)
	MDRV_SOUND_ROUTE(0, "left", 1.00)	/* music */
	MDRV_SOUND_ROUTE(1, "right", 1.00)

	MDRV_SOUND_ADD(OKIM6295, 1122000)
	MDRV_SOUND_CONFIG(okim6295_interface_region_1_pin7high) // clock frequency & pin 7 not verified
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "left", 1.00)	/* sound fx */
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "right", 1.00)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( ddrago3b )
	MDRV_IMPORT_FROM(ddragon3)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(dd3b_readmem, dd3b_writemem)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( ctribe )
	MDRV_IMPORT_FROM(ddragon3)

	MDRV_CPU_MODIFY("main")
	MDRV_CPU_PROGRAM_MAP(ctribe_readmem,ctribe_writemem)

	MDRV_CPU_MODIFY("audio")
	MDRV_CPU_PROGRAM_MAP(ctribe_readmem_sound,ctribe_writemem_sound)

	MDRV_SCREEN_MODIFY("main")
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500) /* not accurate */)

	MDRV_VIDEO_UPDATE(ctribe)
MACHINE_DRIVER_END

/* ROMs */

ROM_START( ddragon3 )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )	/* 64k for cpu code */
	ROM_LOAD16_BYTE( "30a14-0.ic78", 0x00001, 0x40000, CRC(f42fe016) SHA1(11511aa43caa12b36a795bfaefee824821282523) )
	ROM_LOAD16_BYTE( "30a15-0.ic79", 0x00000, 0x20000, CRC(ad50e92c) SHA1(facac5bbe11716d076a40eacbb67f7caab7a4a27) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for sound cpu code */
	ROM_LOAD( "30a13-0.ic43", 0x00000, 0x10000, CRC(1e974d9b) SHA1(8e54ff747efe587a2e971c15e729445c4e232f0f) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "30j-7.ic4",    0x000000, 0x40000, CRC(89d58d32) SHA1(54cfc154024e014f537c7ae0c2275ece50413bc5) ) /* Background */
	ROM_LOAD( "30j-6.ic5",    0x040000, 0x40000, CRC(9bf1538e) SHA1(c7cb96c6b1ac73ec52f46b2a6687bfcfd375ab44) )
	ROM_LOAD( "30j-5.ic6",    0x080000, 0x40000, CRC(8f671a62) SHA1(b5dba61ad6ed39440bb98f7b2dc1111779d6c4a1) )
	ROM_LOAD( "30j-4.ic7",    0x0c0000, 0x40000, CRC(0f74ea1c) SHA1(6bd8dd89bd22b29038cf502a898336e95e50a9cc) )

	ROM_REGION( 0x400000, REGION_GFX2, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "30j-3.ic9",    0x000000, 0x80000, CRC(b3151871) SHA1(a647b4d9bddd6b8715a1d24641391a2e2d0f8867) )
	ROM_LOAD( "30a12-0.ic8",  0x080000, 0x10000, CRC(20d64bea) SHA1(c2bd86bc5310f13f158ca2f93cfc57e5dbf01f7e) )
	ROM_LOAD( "30j-2.ic11",   0x100000, 0x80000, CRC(41c6fb08) SHA1(9fb6105bdc9ff8eeaacf378d208cf6d32a09401b) )
	ROM_LOAD( "30a11-0.ic10", 0x180000, 0x10000, CRC(785d71b0) SHA1(e3f63f6984589d4d6ec6200ae33ce12610d27774) )
	ROM_LOAD( "30j-1.ic13",   0x200000, 0x80000, CRC(67a6f114) SHA1(7d0f3cd6376128ddfcd13f2ec683ec270e95c19c) )
	ROM_LOAD( "30a10-0.ic12", 0x280000, 0x10000, CRC(15e43d12) SHA1(b51cbd0c4c38b802e60616e11795b1ac43bfcb01) )
	ROM_LOAD( "30j-0.ic15",   0x300000, 0x80000, CRC(f15dafbe) SHA1(68049c4542e1c7119bbf1be1fa44e3eea9c11b6e) )
	ROM_LOAD( "30a9-0.ic14",  0x380000, 0x10000, CRC(5a47e7a4) SHA1(74b9dff6e3d5fe22ea505dc439121ff64889769c) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )	/* ADPCM Samples */
	ROM_LOAD( "30j-8.ic73",   0x000000, 0x80000, CRC(c3ad40f3) SHA1(6f3f5fc5b1050fc9a366e02e8e507183a624494d) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "mb7114h.ic38", 0x000000, 0x00100, CRC(113c7443) SHA1(7b0b13e9f0c219f6d436aeec06494734d1f4a599) )
ROM_END

ROM_START( ddrago3j )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )	/* 64k for cpu code */
	ROM_LOAD16_BYTE( "30j15.ic78", 0x00001, 0x40000, CRC(40618cbc) SHA1(cb05498003a45d773983501d3a0f1584a25dcdd3) )
	ROM_LOAD16_BYTE( "30j14.ic79", 0x00000, 0x20000, CRC(96827e80) SHA1(499c0c67d55ff4816ad8832d1d8836eb7432bd13) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for sound cpu code */
	ROM_LOAD( "30j13.ic43",   0x00000, 0x10000, CRC(1e974d9b) SHA1(8e54ff747efe587a2e971c15e729445c4e232f0f) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "30j-7.ic4",    0x000000, 0x40000, CRC(89d58d32) SHA1(54cfc154024e014f537c7ae0c2275ece50413bc5) ) /* Background */
	ROM_LOAD( "30j-6.ic5",    0x040000, 0x40000, CRC(9bf1538e) SHA1(c7cb96c6b1ac73ec52f46b2a6687bfcfd375ab44) )
	ROM_LOAD( "30j-5.ic6",    0x080000, 0x40000, CRC(8f671a62) SHA1(b5dba61ad6ed39440bb98f7b2dc1111779d6c4a1) )
	ROM_LOAD( "30j-4.ic7",    0x0c0000, 0x40000, CRC(0f74ea1c) SHA1(6bd8dd89bd22b29038cf502a898336e95e50a9cc) )

	ROM_REGION( 0x400000, REGION_GFX2, ROMREGION_DISPOSE ) /* sprites */
	ROM_LOAD( "30j-3.ic9",    0x000000, 0x80000, CRC(b3151871) SHA1(a647b4d9bddd6b8715a1d24641391a2e2d0f8867) )
	ROM_LOAD( "30j12-0.ic8",  0x080000, 0x10000, CRC(1e9290d7) SHA1(77e660d2dc9a0e2c4c8ceb3e47b7ce674bceb34a) )
	ROM_LOAD( "30j-2.ic11",   0x100000, 0x80000, CRC(41c6fb08) SHA1(9fb6105bdc9ff8eeaacf378d208cf6d32a09401b) )
	ROM_LOAD( "30j11-0.ic10", 0x180000, 0x10000, CRC(99195b2a) SHA1(d1c0e1855aed22f169717f94d78bc326e68e3064) )
	ROM_LOAD( "30j-1.ic13",   0x200000, 0x80000, CRC(67a6f114) SHA1(7d0f3cd6376128ddfcd13f2ec683ec270e95c19c) )
	ROM_LOAD( "30a10-0.ic12", 0x280000, 0x10000, CRC(e3879b5d) SHA1(fc87aedb0f4964a8d261d86121fe8544b330bed9) )
	ROM_LOAD( "30j-0.ic15",   0x300000, 0x80000, CRC(f15dafbe) SHA1(68049c4542e1c7119bbf1be1fa44e3eea9c11b6e) )
	ROM_LOAD( "30j9-0.ic14",  0x380000, 0x10000, CRC(2759ae84) SHA1(02c70958259f56174ce2ba2db56040dad72be02b) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )	/* ADPCM Samples */
	ROM_LOAD( "30j-8.ic73",   0x000000, 0x80000, CRC(c3ad40f3) SHA1(6f3f5fc5b1050fc9a366e02e8e507183a624494d) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "mb7114h.ic38", 0x000000, 0x00100, CRC(113c7443) SHA1(7b0b13e9f0c219f6d436aeec06494734d1f4a599) )
ROM_END

ROM_START( ddrago3b )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )	/* 64k for cpu code */
	ROM_LOAD16_BYTE( "dd3.01",   0x00001, 0x20000, CRC(68321d8b) SHA1(bd34d361e8ef18ef2b7e8bfe438b1b098c3151b5) )
	ROM_LOAD16_BYTE( "dd3.03",   0x00000, 0x20000, CRC(bc05763b) SHA1(49f661fdc98bd43a6622945e9aa8d8e7a7dc1ce6) )
	ROM_LOAD16_BYTE( "dd3.02",   0x40001, 0x20000, CRC(38d9ae75) SHA1(d42e1d9c704c66bad94e14d14f5e0b7209cc938e) )
	/* No EVEN rom! */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for sound cpu code */
	ROM_LOAD( "dd3.06",    0x00000, 0x10000, CRC(1e974d9b) SHA1(8e54ff747efe587a2e971c15e729445c4e232f0f) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE )
	/* Background */
	ROM_LOAD( "dd3.f",   0x000000, 0x40000, CRC(89d58d32) SHA1(54cfc154024e014f537c7ae0c2275ece50413bc5) )
	ROM_LOAD( "dd3.e",   0x040000, 0x40000, CRC(9bf1538e) SHA1(c7cb96c6b1ac73ec52f46b2a6687bfcfd375ab44) )
	ROM_LOAD( "dd3.b",   0x080000, 0x40000, CRC(8f671a62) SHA1(b5dba61ad6ed39440bb98f7b2dc1111779d6c4a1) )
	ROM_LOAD( "dd3.a",   0x0c0000, 0x40000, CRC(0f74ea1c) SHA1(6bd8dd89bd22b29038cf502a898336e95e50a9cc) )

	ROM_REGION( 0x400000, REGION_GFX2, ROMREGION_DISPOSE )
	/* sprites  */
	ROM_LOAD( "dd3.3e",   0x000000, 0x20000, CRC(726c49b7) SHA1(dbafad47bb6b717c409fdc5d81c413f1282f2bbb) ) //4a
	ROM_LOAD( "dd3.3d",   0x020000, 0x20000, CRC(37a1c335) SHA1(de70ba51788b601591c3aff71cb94aae349b272d) ) //3a
	ROM_LOAD( "dd3.3c",   0x040000, 0x20000, CRC(2bcfe63c) SHA1(678ef0e7cc38e4df1e1d1e3f5cba6601aa520ec6) ) //2a
	ROM_LOAD( "dd3.3b",   0x060000, 0x20000, CRC(b864cf17) SHA1(39a5155f40ba500bf201acca6f7d230cb0ea8309) ) //1a
	ROM_LOAD( "dd3.3a",   0x080000, 0x10000, CRC(20d64bea) SHA1(c2bd86bc5310f13f158ca2f93cfc57e5dbf01f7e) ) //5a

	ROM_LOAD( "dd3.2e",   0x100000, 0x20000, CRC(8c71eb06) SHA1(e47acf9e2d5eeec0cff9654210a43c690a45d447) ) //4b
	ROM_LOAD( "dd3.2d",   0x120000, 0x20000, CRC(3e134be9) SHA1(0a75b56353bed2743f7ce8f3f74379fc9f0d3cb9) ) //3b
	ROM_LOAD( "dd3.2c",   0x140000, 0x20000, CRC(b4115ef0) SHA1(d90943f75051c7590a0effcc30fa813890c9ad11) ) //2b
	ROM_LOAD( "dd3.2b",   0x160000, 0x20000, CRC(4639333d) SHA1(8e3c982d6fa38cbec42e8de780f165547b5b0271) ) //1b
	ROM_LOAD( "dd3.2a",   0x180000, 0x10000, CRC(785d71b0) SHA1(e3f63f6984589d4d6ec6200ae33ce12610d27774) ) //5b

	ROM_LOAD( "dd3.1e",   0x200000, 0x20000, CRC(04420cc8) SHA1(ed148c52374bbd0d29c12070ea1499333fc04449) ) //4c
	ROM_LOAD( "dd3.1d",   0x220000, 0x20000, CRC(33f97b2f) SHA1(40dc5357caa17ed6673588422332966ee97752b7) ) //3c
	ROM_LOAD( "dd3.1c",   0x240000, 0x20000, CRC(0f9a8f2a) SHA1(d7e46d32067d3f8b3bacbf96ea313645a9a48410) ) //2c
	ROM_LOAD( "dd3.1b",   0x260000, 0x20000, CRC(15c91772) SHA1(8578b6c501e3af64863bd6b28ef59c6884dfe028) ) //1c
	ROM_LOAD( "dd3.1a",   0x280000, 0x10000, CRC(15e43d12) SHA1(b51cbd0c4c38b802e60616e11795b1ac43bfcb01) ) //5c

	ROM_LOAD( "dd3.0e",   0x300000, 0x20000, CRC(894734b3) SHA1(46fa174a303e85f439254976252835626c4b2ddc) ) //4d
	ROM_LOAD( "dd3.0d",   0x320000, 0x20000, CRC(cd504584) SHA1(674481b524853dbfcb7d173d58250b1be8464313) ) //3d
	ROM_LOAD( "dd3.0c",   0x340000, 0x20000, CRC(38e8a9ad) SHA1(1c66acde8f72fa7c6415a7aadc2dbf4300446c88) ) //2d
	ROM_LOAD( "dd3.0b",   0x360000, 0x20000, CRC(80c1cb74) SHA1(5558fa36b238cff1bee9df921e77d7de2062bf15) ) //1d
	ROM_LOAD( "dd3.0a",   0x380000, 0x10000, CRC(5a47e7a4) SHA1(74b9dff6e3d5fe22ea505dc439121ff64889769c) ) //5d

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )	/* ADPCM Samples */
	ROM_LOAD( "dd3.j7",   0x000000, 0x40000, CRC(3af21dbe) SHA1(295d0b7f33c55ef37a71382a22edd8fc97fa5353) )
	ROM_LOAD( "dd3.j8",   0x040000, 0x40000, CRC(c28b53cd) SHA1(93d29669ec899fd5852f61b1d91d0a90cc30e192) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "mb7114h.38", 0x0000, 0x0100, CRC(113c7443) SHA1(7b0b13e9f0c219f6d436aeec06494734d1f4a599) )
ROM_END

ROM_START( ctribe )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )	/* 64k for cpu code */
	ROM_LOAD16_BYTE( "28a16-2.ic26", 0x00001, 0x20000, CRC(c46b2e63) SHA1(86ace715dca48c78a46da1d102de47e5f948a86c) )
	ROM_LOAD16_BYTE( "28a15-2.ic25", 0x00000, 0x20000, CRC(3221c755) SHA1(0f6fe5cd6947f6547585eedb7fc5e6af8544b1f7) )
	ROM_LOAD16_BYTE( "ct_ep2.ic104", 0x40001, 0x10000, CRC(8c2c6dbd) SHA1(b99b9be6e0bdc8340fedd258819c4df587926a84) )
	/* No EVEN rom! */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for sound cpu code */
	ROM_LOAD( "28a10-0.ic89", 0x00000, 0x8000, CRC(4346de13) SHA1(67c6de90ba31a325f03e64d28c9391a315ee359c) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "28j7-0.ic11",  0x000000, 0x40000, CRC(a8b773f1) SHA1(999e41dfeb3fb937da769c4a33bb29bf4076dc63) )    /* Background */
	ROM_LOAD( "28j6-0.ic13",  0x040000, 0x40000, CRC(617530fc) SHA1(b9155ed0ae1437bf4d0b7a95e769bc05a820ecec) )
	ROM_LOAD( "28j5-0.ic12",  0x080000, 0x40000, CRC(cef0a821) SHA1(c7a35048d5ebf3f09abf9d27f91d12adc03befeb) )
	ROM_LOAD( "28j4-0.ic14",  0x0c0000, 0x40000, CRC(b84fda09) SHA1(3ae0c0ec6c398dea17e248b017ea3e2f6c3571e1) )

	ROM_REGION( 0x400000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "28j3-0.ic77",  0x000000, 0x80000, CRC(1ac2a461) SHA1(17436f5dcf29041ca5f470dfae538e4fc12153cc) )    /* Sprites */
	ROM_LOAD( "28a14-0.ic60", 0x080000, 0x10000, CRC(972faddb) SHA1(f2b211e8f8301667e6c9a3ce9612e39b16e66a67) )
	ROM_LOAD( "28j2-0.ic78",  0x100000, 0x80000, CRC(8c796707) SHA1(7417ad0413083876ed65a8612845ccb0d2717530) )
	ROM_LOAD( "28a13-0.ic61", 0x180000, 0x10000, CRC(eb3ab374) SHA1(db66cb7976c111fa76a3a211e96ad1d7b78ce0ad) )
	ROM_LOAD( "28j1-0.ic97",  0x200000, 0x80000, CRC(1c9badbd) SHA1(d28f6d684d88448eaa3feae0bba2f5a836d89bd7) )
	ROM_LOAD( "28a12-0.ic85", 0x280000, 0x10000, CRC(c602ac97) SHA1(44440739636b684c6dcac837f59664120c9ba5f3) )
	ROM_LOAD( "28j0-0.ic98",  0x300000, 0x80000, CRC(ba73c49e) SHA1(830099027ede1f7c56bb0bf3cdef3018b92e0b87) )
	ROM_LOAD( "28a11-0.ic86", 0x380000, 0x10000, CRC(4da1d8e5) SHA1(568e9e8d00f1b1ca27c28df5fc0ffc74ad91da7e) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 ) /* ADPCM Samples */
	ROM_LOAD( "28j9-0.ic83", 0x00000, 0x20000, CRC(f92a7f4a) SHA1(3717ef64876be9ada378b449749918ce9072073a) )
	ROM_LOAD( "28j8-0.ic82", 0x20000, 0x20000, CRC(1a3a0b39) SHA1(8847530027cf4be03ffbc6d78dee97b459d03a04) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "28.ic44", 0x0000, 0x0100, CRC(964329ef) SHA1(f26846571a16d27b726f689049deb0188103aadb) )
ROM_END

ROM_START( ctribe1 )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )	/* 64k for cpu code */
	ROM_LOAD16_BYTE( "28a16-2.ic26", 0x00001, 0x20000, CRC(f00f8443) SHA1(3c099b6bea9956cc60ce4a9a5d790ac2bf7d77bd) )
	ROM_LOAD16_BYTE( "28a15-2.ic25", 0x00000, 0x20000, CRC(dd70079f) SHA1(321b523fefec2a962d0afa20b33428e7caea8958) )
	ROM_LOAD16_BYTE( "ct_ep2.ic104", 0x40001, 0x10000, CRC(8c2c6dbd) SHA1(b99b9be6e0bdc8340fedd258819c4df587926a84) )
	/* No EVEN rom! */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for sound cpu code */
	ROM_LOAD( "28a10-0.ic89", 0x00000, 0x8000, CRC(4346de13) SHA1(67c6de90ba31a325f03e64d28c9391a315ee359c) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "28j7-0.ic11",  0x000000, 0x40000, CRC(a8b773f1) SHA1(999e41dfeb3fb937da769c4a33bb29bf4076dc63) )    /* Background */
	ROM_LOAD( "28j6-0.ic13",  0x040000, 0x40000, CRC(617530fc) SHA1(b9155ed0ae1437bf4d0b7a95e769bc05a820ecec) )
	ROM_LOAD( "28j5-0.ic12",  0x080000, 0x40000, CRC(cef0a821) SHA1(c7a35048d5ebf3f09abf9d27f91d12adc03befeb) )
	ROM_LOAD( "28j4-0.ic14",  0x0c0000, 0x40000, CRC(b84fda09) SHA1(3ae0c0ec6c398dea17e248b017ea3e2f6c3571e1) )

	ROM_REGION( 0x400000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "28j3-0.ic77",  0x000000, 0x80000, CRC(1ac2a461) SHA1(17436f5dcf29041ca5f470dfae538e4fc12153cc) )    /* Sprites */
	ROM_LOAD( "28a14-0.ic60", 0x080000, 0x10000, CRC(972faddb) SHA1(f2b211e8f8301667e6c9a3ce9612e39b16e66a67) )
	ROM_LOAD( "28j2-0.ic78",  0x100000, 0x80000, CRC(8c796707) SHA1(7417ad0413083876ed65a8612845ccb0d2717530) )
	ROM_LOAD( "28a13-0.ic61", 0x180000, 0x10000, CRC(eb3ab374) SHA1(db66cb7976c111fa76a3a211e96ad1d7b78ce0ad) )
	ROM_LOAD( "28j1-0.ic97",  0x200000, 0x80000, CRC(1c9badbd) SHA1(d28f6d684d88448eaa3feae0bba2f5a836d89bd7) )
	ROM_LOAD( "28a12-0.ic85", 0x280000, 0x10000, CRC(c602ac97) SHA1(44440739636b684c6dcac837f59664120c9ba5f3) )
	ROM_LOAD( "28j0-0.ic98",  0x300000, 0x80000, CRC(ba73c49e) SHA1(830099027ede1f7c56bb0bf3cdef3018b92e0b87) )
	ROM_LOAD( "28a11-0.ic86", 0x380000, 0x10000, CRC(4da1d8e5) SHA1(568e9e8d00f1b1ca27c28df5fc0ffc74ad91da7e) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 ) /* ADPCM Samples */
	ROM_LOAD( "28j9-0.ic83", 0x00000, 0x20000, CRC(f92a7f4a) SHA1(3717ef64876be9ada378b449749918ce9072073a) )
	ROM_LOAD( "28j8-0.ic82", 0x20000, 0x20000, CRC(1a3a0b39) SHA1(8847530027cf4be03ffbc6d78dee97b459d03a04) )

	ROM_REGION( 0x0100, REGION_PROMS, 0 )
	ROM_LOAD( "28.ic44", 0x0000, 0x0100, CRC(964329ef) SHA1(f26846571a16d27b726f689049deb0188103aadb) )
ROM_END

ROM_START( ctribeb )
	ROM_REGION( 0x80000, REGION_CPU1, 0 )	/* 64k for cpu code */
	ROM_LOAD16_BYTE( "ct_ep1.rom", 0x00001, 0x20000, CRC(9cfa997f) SHA1(ee49b4b9e9cd29616f244fdf3912ef743e2404ce) )
	ROM_LOAD16_BYTE( "ct_ep3.rom", 0x00000, 0x20000, CRC(2ece8681) SHA1(17ee2ceb893e2eb08fa4cabcdebcec02bee16cda) )
	ROM_LOAD16_BYTE( "ct_ep2.rom", 0x40001, 0x10000, CRC(8c2c6dbd) SHA1(b99b9be6e0bdc8340fedd258819c4df587926a84) )
	/* No EVEN rom! */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64k for sound cpu code */
	ROM_LOAD( "ct_ep4.rom",   0x00000, 0x8000, CRC(4346de13) SHA1(67c6de90ba31a325f03e64d28c9391a315ee359c) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ct_mr7.rom",  0x000000, 0x40000, CRC(a8b773f1) SHA1(999e41dfeb3fb937da769c4a33bb29bf4076dc63) )    /* Background */
	ROM_LOAD( "ct_mr6.rom",  0x040000, 0x40000, CRC(617530fc) SHA1(b9155ed0ae1437bf4d0b7a95e769bc05a820ecec) )
	ROM_LOAD( "ct_mr5.rom",  0x080000, 0x40000, CRC(cef0a821) SHA1(c7a35048d5ebf3f09abf9d27f91d12adc03befeb) )
	ROM_LOAD( "ct_mr4.rom",  0x0c0000, 0x40000, CRC(b84fda09) SHA1(3ae0c0ec6c398dea17e248b017ea3e2f6c3571e1) )

	ROM_REGION( 0x400000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "ct_mr3.rom",  0x000000, 0x80000, CRC(1ac2a461) SHA1(17436f5dcf29041ca5f470dfae538e4fc12153cc) )    /* Sprites */
	ROM_LOAD( "ct_ep5.rom",  0x080000, 0x10000, CRC(972faddb) SHA1(f2b211e8f8301667e6c9a3ce9612e39b16e66a67) )
	ROM_LOAD( "ct_mr2.rom",  0x100000, 0x80000, CRC(8c796707) SHA1(7417ad0413083876ed65a8612845ccb0d2717530) )
	ROM_LOAD( "ct_ep6.rom",  0x180000, 0x10000, CRC(eb3ab374) SHA1(db66cb7976c111fa76a3a211e96ad1d7b78ce0ad) )
	ROM_LOAD( "ct_mr1.rom",  0x200000, 0x80000, CRC(1c9badbd) SHA1(d28f6d684d88448eaa3feae0bba2f5a836d89bd7) )
	ROM_LOAD( "ct_ep7.rom",  0x280000, 0x10000, CRC(c602ac97) SHA1(44440739636b684c6dcac837f59664120c9ba5f3) )
	ROM_LOAD( "ct_mr0.rom",  0x300000, 0x80000, CRC(ba73c49e) SHA1(830099027ede1f7c56bb0bf3cdef3018b92e0b87) )
	ROM_LOAD( "ct_ep8.rom",  0x380000, 0x10000, CRC(4da1d8e5) SHA1(568e9e8d00f1b1ca27c28df5fc0ffc74ad91da7e) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 )	/* ADPCM Samples */
	ROM_LOAD( "ct_mr8.rom",   0x020000, 0x20000, CRC(9963a6be) SHA1(b09b8f52b7fe5ceac34bc7d70c235d60d808fcbf) )
	ROM_CONTINUE(			  0x000000, 0x20000 )
ROM_END

/* Game Drivers */

GAME( 1990, ddragon3, 0,        ddragon3, ddragon3, 0, ROT0, "Technos", "Double Dragon 3 - The Rosetta Stone (US)", GAME_SUPPORTS_SAVE )
GAME( 1990, ddrago3j, ddragon3,	ddragon3, ddragon3, 0, ROT0, "Technos", "Double Dragon 3 - The Rosetta Stone (Japan)", GAME_SUPPORTS_SAVE )
GAME( 1990, ddrago3b, ddragon3, ddrago3b, ddrago3b, 0, ROT0, "bootleg", "Double Dragon 3 - The Rosetta Stone (bootleg)", GAME_SUPPORTS_SAVE )
GAME( 1990, ctribe,   0,        ctribe,   ctribe,   0, ROT0, "Technos", "The Combatribes (US)", GAME_SUPPORTS_SAVE )
GAME( 1990, ctribe1,  ctribe,   ctribe,   ctribe,   0, ROT0, "Technos", "The Combatribes (US) - Set 1?", GAME_SUPPORTS_SAVE )
GAME( 1990, ctribeb,  ctribe,   ctribe,   ctribe,   0, ROT0, "bootleg", "The Combatribes (bootleg)", GAME_SUPPORTS_SAVE )
