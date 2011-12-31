/*
Funai / Gakken Interstellar Laser Fantasy laserdisc hardware
Driver by Andrew Gardner with help from Daphne Source

Notes:
    Holding down the TEST switch (T) while hitting reset will bring up the Self Test.
    Hit T twice more for color and monitor calibration.
    This is somewhat strange hardware : More z80's than necessary
                                        3 bpp sprites
                                        6-pin dip switches with odd handling
                                        inverted DIP logic?
                                        CPU2 maps RAM over where its ROM lives

Todo:
    How does one best make one DIP switch bit from address 0x02 tie to two bits from address 0x03?
    Get real ROM labels!  The current labels are unfortunately a bit odd.
    Add sprite drawing.
    Convert to tilemaps.
    Make it work - this one should be close right now :/.
*/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "render.h"
#include "machine/laserdsc.h"


class istellar_state : public driver_device
{
public:
	istellar_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }

	device_t *m_laserdisc;
	UINT8 *m_tile_ram;
	UINT8 *m_tile_control_ram;
	UINT8 *m_sprite_ram;
	UINT8 m_ldp_latch1;
	UINT8 m_ldp_latch2;
	UINT8 m_z80_2_nmi_enable;
};


/* There is only 1 crystal on the stack of 3 boards - speed is unknown, the following is Daphne's guess */
#define GUESSED_CLOCK (3072000)


/* VIDEO GOODS */
static SCREEN_UPDATE( istellar )
{
	istellar_state *state = screen.machine().driver_data<istellar_state>();
	int charx, chary;

	/* clear */
	bitmap->fill(0, cliprect);

	/* DEBUG */
	/*
    for (charx = 0; charx < 0x400; charx ++)
    {
        printf ("%x ", state->m_sprite_ram[charx]) ;
    }
    printf("\n\n\n");
    */

	/* Draw tiles */
	for (charx = 0; charx < 32; charx++)
	{
		for (chary = 0; chary < 32; chary++)
		{
			int current_screen_character = (chary*32) + charx;

			drawgfx_transpen(bitmap, cliprect, screen.machine().gfx[0],
					state->m_tile_ram[current_screen_character],
					(state->m_tile_control_ram[current_screen_character] & 0x0f),
					0, 0, charx*8, chary*8, 0);
		}
	}


	/* Draw sprites */

	return 0;
}


static MACHINE_START( istellar )
{
	istellar_state *state = machine.driver_data<istellar_state>();
	state->m_laserdisc = machine.device("laserdisc");
}



/* MEMORY HANDLERS */
/* Z80 0 R/W */
static READ8_HANDLER(z80_0_latch1_read)
{
	istellar_state *state = space->machine().driver_data<istellar_state>();
	/*logerror("CPU0 : reading LDP status latch (%x)\n", state->m_ldp_latch1);*/
	return state->m_ldp_latch1;
}

static WRITE8_HANDLER(z80_0_latch2_write)
{
	istellar_state *state = space->machine().driver_data<istellar_state>();
	/*logerror("CPU0 : writing cpu_latch2 (%x).  Potentially followed by an IRQ.\n", data);*/
	state->m_ldp_latch2 = data;

	/* A CPU2 NMI */
	if (state->m_z80_2_nmi_enable)
	{
		logerror("Executing an NMI on CPU2\n");
		cputag_set_input_line(space->machine(), "sub", INPUT_LINE_NMI, PULSE_LINE);		/* Maybe this is a ASSERT_LINE, CLEAR_LINE combo? */
		state->m_z80_2_nmi_enable = 0;
	}
}


/* Z80 1 R/W */


/* Z80 2 R/W */
static READ8_HANDLER(z80_2_ldp_read)
{
	istellar_state *state = space->machine().driver_data<istellar_state>();
	UINT8 readResult = laserdisc_data_r(state->m_laserdisc);
	logerror("CPU2 : reading LDP : %x\n", readResult);
	return readResult;
}

static READ8_HANDLER(z80_2_latch2_read)
{
	istellar_state *state = space->machine().driver_data<istellar_state>();
	logerror("CPU2 : reading latch2 (%x)\n", state->m_ldp_latch2);
	return state->m_ldp_latch2;
}

static READ8_HANDLER(z80_2_nmienable)
{
	istellar_state *state = space->machine().driver_data<istellar_state>();
	logerror("CPU2 : ENABLING NMI\n");
	state->m_z80_2_nmi_enable = 1;
	return 0x00;
}

static READ8_HANDLER(z80_2_unknown_read)
{
	logerror("CPU2 : c000!\n");
	return 0x00;
}

static WRITE8_HANDLER(z80_2_latch1_write)
{
	istellar_state *state = space->machine().driver_data<istellar_state>();
	logerror("CPU2 : writing latch1 (%x)\n", data);
	state->m_ldp_latch1 = data;
}

static WRITE8_HANDLER(z80_2_ldp_write)
{
	istellar_state *state = space->machine().driver_data<istellar_state>();
	logerror("CPU2 : writing LDP : 0x%x\n", data);
	laserdisc_data_w(state->m_laserdisc,data);
}



/* PROGRAM MAPS */
static ADDRESS_MAP_START( z80_0_mem, AS_PROGRAM, 8 )
	AM_RANGE(0x0000,0x9fff) AM_ROM
	AM_RANGE(0xa000,0xa7ff) AM_RAM
	AM_RANGE(0xa800,0xabff) AM_RAM AM_BASE_MEMBER(istellar_state, m_tile_ram)
	AM_RANGE(0xac00,0xafff) AM_RAM AM_BASE_MEMBER(istellar_state, m_tile_control_ram)
	AM_RANGE(0xb000,0xb3ff) AM_RAM AM_BASE_MEMBER(istellar_state, m_sprite_ram)
ADDRESS_MAP_END

static ADDRESS_MAP_START( z80_1_mem, AS_PROGRAM, 8 )
	AM_RANGE(0x0000,0x1fff) AM_ROM
	AM_RANGE(0x4000,0x47ff) AM_RAM
ADDRESS_MAP_END

static ADDRESS_MAP_START( z80_2_mem, AS_PROGRAM, 8 )
	AM_RANGE(0x0000,0x17ff) AM_ROM
	AM_RANGE(0x1800,0x1fff) AM_RAM
	AM_RANGE(0xc000,0xc000) AM_READ(z80_2_unknown_read)		/* Seems to be thrown away every time it's read - maybe interrupt related? */
ADDRESS_MAP_END


/* IO MAPS */
static ADDRESS_MAP_START( z80_0_io, AS_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00,0x00) AM_READ_PORT("IN0")
	AM_RANGE(0x02,0x02) AM_READ_PORT("DSW1")
	AM_RANGE(0x03,0x03) AM_READ_PORT("DSW2")
	/*AM_RANGE(0x04,0x04) AM_WRITE(volatile_palette_write)*/
	AM_RANGE(0x05,0x05) AM_READWRITE(z80_0_latch1_read,z80_0_latch2_write)
ADDRESS_MAP_END

static ADDRESS_MAP_START( z80_1_io, AS_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00,0x00) AM_NOP /*AM_READWRITE(z80_1_slatch_read,z80_1_slatch_write)*/
	AM_RANGE(0x01,0x01) AM_NOP /*AM_READWRITE(z80_1_nmienable,z80_1_soundwrite_front)*/
	AM_RANGE(0x02,0x02) AM_NOP /*AM_WRITE(z80_1_soundwrite_rear)*/
ADDRESS_MAP_END

static ADDRESS_MAP_START( z80_2_io, AS_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00,0x00) AM_READWRITE(z80_2_ldp_read,z80_2_ldp_write)
	AM_RANGE(0x01,0x01) AM_READWRITE(z80_2_latch2_read,z80_2_latch1_write)
	AM_RANGE(0x02,0x02) AM_READ(z80_2_nmienable)
/*  AM_RANGE(0x03,0x03) AM_WRITE(z80_2_ldtrans_write)*/
ADDRESS_MAP_END


/* PORTS */
static INPUT_PORTS_START( istellar )
	/* TEST MODE might display a 0 for a short and a 1 for an open circuit?  If so, everything below is inverted. */
	PORT_START("DSW1")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION("SW1:!1")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x0e, 0x00, DEF_STR( Coin_A ) ) PORT_DIPLOCATION("SW1:!2,!3,!4")
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Lives ) ) PORT_DIPLOCATION("SW1:!5,!6")
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x10, "4" )
	PORT_DIPSETTING(    0x20, "5" )
	PORT_DIPSETTING(    0x30, "10" )
	PORT_DIPNAME( 0x40, 0x00, "Barrier UFO" ) PORT_DIPLOCATION("SW2:!1")
	PORT_DIPSETTING(    0x40, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unused ) ) PORT_DIPLOCATION("SW2:!2")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	/* NOTE - bit 0x80 in the above read is combined with bits 0x03 in the below read to form the Coin_B
              settings.  I'm unaware of what mechanism MAME will use to make this work right? */

	/* "In case of inter-stellar upright type the coin switch 2 is not used."  Quoth the manual. */
	PORT_START("DSW2")
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unused ) ) PORT_DIPLOCATION("SW2:!3")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unused ) ) PORT_DIPLOCATION("SW2:!4")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unused ) ) PORT_DIPLOCATION("SW2:!5")		/* Maybe SERVICE? */
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER ) PORT_NAME( "TEST" ) PORT_CODE( KEYCODE_T )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_START1 )

	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	/* SERVICE might be hanging out back here */
INPUT_PORTS_END

static PALETTE_INIT( istellar )
{
	int i;

	/* Oddly enough, the top 4 bits of each byte is 0 */
	for (i = 0; i < machine.total_colors(); i++)
	{
		int r,g,b;
		int bit0,bit1,bit2,bit3;

		/* Daphne says "TODO: get the real interstellar resistor values" */

		/* red component */
		bit0 = (color_prom[i+0x000] >> 0) & 0x01;
		bit1 = (color_prom[i+0x000] >> 1) & 0x01;
		bit2 = (color_prom[i+0x000] >> 2) & 0x01;
		bit3 = (color_prom[i+0x000] >> 3) & 0x01;
		r = (0x8f * bit3) + (0x43 * bit2) + (0x1f * bit1) + (0x0e * bit0);

		/* green component */
		bit0 = (color_prom[i+0x100] >> 0) & 0x01;
		bit1 = (color_prom[i+0x100] >> 1) & 0x01;
		bit2 = (color_prom[i+0x100] >> 2) & 0x01;
		bit3 = (color_prom[i+0x100] >> 3) & 0x01;
		g = (0x8f * bit3) + (0x43 * bit2) + (0x1f * bit1) + (0x0e * bit0);

		/* blue component */
		bit0 = (color_prom[i+0x200] >> 0) & 0x01;
		bit1 = (color_prom[i+0x200] >> 1) & 0x01;
		bit2 = (color_prom[i+0x200] >> 2) & 0x01;
		bit3 = (color_prom[i+0x200] >> 3) & 0x01;
		b = (0x8f * bit3) + (0x43 * bit2) + (0x1f * bit1) + (0x0e * bit0);

		palette_set_color(machine,i,MAKE_RGB(r,g,b));
	}

	/* make color 0 transparent */
	palette_set_color(machine, 0, MAKE_ARGB(0,0,0,0));
}

static const gfx_layout istellar_gfx_layout =
{
	8,8,
	0x2000/8,
	3,
	{ 0, 0x2000*8, 0x4000*8 },
	{ 0,1,2,3,4,5,6,7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static GFXDECODE_START( istellar )
	GFXDECODE_ENTRY( "gfx1", 0, istellar_gfx_layout, 0x0, 0x100 )
GFXDECODE_END

static INTERRUPT_GEN( vblank_callback_istellar )
{
	/* Interrupt presumably comes from VBlank */
	device_set_input_line(device, 0, ASSERT_LINE);

	/* Interrupt presumably comes from the LDP's status strobe */
	cputag_set_input_line(device->machine(), "sub", 0, ASSERT_LINE);
}


/* DRIVER */
static MACHINE_CONFIG_START( istellar, istellar_state )
	/* main cpu */
	MCFG_CPU_ADD("maincpu", Z80, GUESSED_CLOCK)
	MCFG_CPU_PROGRAM_MAP(z80_0_mem)
	MCFG_CPU_IO_MAP(z80_0_io)
	MCFG_CPU_VBLANK_INT("screen", vblank_callback_istellar)

	/* sound cpu */
	MCFG_CPU_ADD("audiocpu", Z80, GUESSED_CLOCK)
	MCFG_CPU_PROGRAM_MAP(z80_1_mem)
	MCFG_CPU_IO_MAP(z80_1_io)

	/* ldp comm cpu */
	MCFG_CPU_ADD("sub", Z80, GUESSED_CLOCK)
	MCFG_CPU_PROGRAM_MAP(z80_2_mem)
	MCFG_CPU_IO_MAP(z80_2_io)

	MCFG_MACHINE_START(istellar)

	MCFG_LASERDISC_ADD("laserdisc", PIONEER_LDV1000, "screen", "ldsound")
	MCFG_LASERDISC_OVERLAY(istellar, 256, 256, BITMAP_FORMAT_INDEXED16)

	/* video hardware */
	MCFG_LASERDISC_SCREEN_ADD_NTSC("screen", BITMAP_FORMAT_INDEXED16)

	MCFG_PALETTE_LENGTH(256)
	MCFG_PALETTE_INIT(istellar)

	MCFG_GFXDECODE(istellar)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_SOUND_ADD("ldsound", LASERDISC_SOUND, 0)
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(1, "rspeaker", 1.0)
MACHINE_CONFIG_END


/* There is a photo of the PCB with blurry IC locations and labels.  Comments reflect what I can (barely) see. */
ROM_START( istellar )
	/* Main program CPU */
	ROM_REGION( 0xa000, "maincpu", 0 )
	ROM_LOAD( "rom2.top", 0x0000, 0x2000, CRC(5d643381) SHA1(75ca52c28a52f534eda00c18b0db97e9923ff670) )	/* At IC location C63 (top board) - label ? */
	ROM_LOAD( "rom3.top", 0x2000, 0x2000, CRC(ce5a2b09) SHA1(2de6a6e993c3411577ac0c834db8aaf16fb007ed) )	/* At IC location C64 (top board) - label ? */
	ROM_LOAD( "rom4.top", 0x4000, 0x2000, CRC(7c2cb1f1) SHA1(ffd92510c03c2d35a59d233883c2b9f57394a51c) )	/* At IC location C65 (top board) - label ? */
	ROM_LOAD( "rom5.top", 0x6000, 0x2000, CRC(354377f6) SHA1(bcf95b7ee1b47854e10baf24b0d8af3d56738b99) )	/* At IC location C66 (top board) - label ? */
	ROM_LOAD( "rom6.top", 0x8000, 0x2000, CRC(0319bf40) SHA1(f324626e457c3eb7d6b74bc6afbfcc3aab2b3c72) )	/* At IC location C67 (top board) - label ? */

	/* Sound CPU */
	ROM_REGION( 0x2000, "audiocpu", 0 )
	ROM_LOAD( "rom1.top", 0x0000, 0x2000, CRC(4f34fb1d) SHA1(56ca19344c84c5989d0be797e2759f84760310be) )	/* At IC location C62 (top board) - label ? */

	/* LDP Communications CPU */
	ROM_REGION( 0x2000, "sub", 0 )
	ROM_LOAD( "rom11.bot", 0x0000, 0x2000, CRC(165cbc57) SHA1(39463888f22ec3125f0686066d923a9aae79a8f7) )	/* At IC location C12 (bottom board) - label IS11 */

	/* Tiles */
	ROM_REGION( 0x6000, "gfx1", 0 )
	ROM_LOAD( "rom9.bot", 0x0000, 0x2000, CRC(9d79acb6) SHA1(72af972695face0016afce8a26c629d963e86d48) )	/* At IC location C47? (bottom board) - label ? */
	ROM_LOAD( "rom8.bot", 0x2000, 0x2000, CRC(e9c9e490) SHA1(79aa35552b984018bc723adece5c40a0833a313c) )	/* At IC location C48? (bottom board) - label ? */
	ROM_LOAD( "rom7.bot", 0x4000, 0x2000, CRC(1447ce3a) SHA1(8545cec108df6adab303802b1407c89b2dceba21) )	/* At IC location C49? (bottom board) - label ? */

	/* Color PROMs */
	ROM_REGION( 0x300, "proms", 0 )
	ROM_LOAD( "red6b.bot",   0x000, 0x100, CRC(5c52f844) SHA1(a8a3d91f3247ad13c805d8d8288b07f3cdaf1189) )	/* At IC location C63? (bottom board) - label ? */
	ROM_LOAD( "green6c.bot", 0x100, 0x100, CRC(7d8c845c) SHA1(04ae2ca0cc6679e21346ce34e9e01aa5bf4e2067) )	/* At IC location C62? (bottom board) - label ? */
	ROM_LOAD( "blue6d.bot",  0x200, 0x100, CRC(5ebb81f9) SHA1(285d60f2894c524ca80fc68ad7c2dfd9093a67ea) )	/* At IC location C61? (bottom board) - label ? */

	DISK_REGION( "laserdisc" )
	DISK_IMAGE_READONLY( "istellar", 0, NO_DUMP )
ROM_END


static DRIVER_INIT( istellar )
{
	istellar_state *state = machine.driver_data<istellar_state>();
	state->m_z80_2_nmi_enable = 0;
}

/*    YEAR  NAME    PARENT   MACHINE  INPUT    INIT    MONITOR  COMPANY          FULLNAME                       FLAGS) */
GAME( 1983, istellar, 0,       istellar,  istellar,  istellar, ROT0,    "Funai/Gakken",  "Interstellar Laser Fantasy",  GAME_NOT_WORKING|GAME_NO_SOUND)
