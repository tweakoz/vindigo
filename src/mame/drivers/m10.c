/***************************************************************************

    Irem M10/M11/M15 hardware

****************************************************************************

  (c) 12/2/1998 Lee Taylor

Notes:
- Colors are close to screen shots for IPM Invader. The other games have not
  been verified.
- The bitmap strips in IPM Invader might be slightly misplaced

TODO:
- Dip switches

Head On
-------
Irem, 1979? / 1980?

PCB Layout
----------

    M-15L
   |---------------------------------------------------------------------------------|
   |                                                                                 |
   | DSW(8)  74175   74175   7400  74LS08   74121   M53214     |-------|      E1.9A  |
   |                                                           | 6502  |             |
   |          7432   74175   7404    7427    7442  74LS241     |-------|             |
   |                                                                          E2.9B  |
   |                                                                                 |
   |        74LS74    7432  74161   74161    7442  74LS241  74LS367  74LS367         |
 |-|                                                                          E3.9C  |
 |          M53214 74LS367   7442    7486    8216     2114  74LS157  74LS367         |
 |                                                                                   |
 |4         M53214 74LS367  74161    7486    8216     2114  74LS157     2111  E4.9D  |
 |4                                                                                  |
 |W         M53214 74LS367  74161    7486    8216     2114  74LS157     2111         |
 |A                                                                           E5.9F  |
 |Y         M53214 74LS367  74161    7486    8216    74166     2114  74LS157         |
 |                         11.73MHz                                                  |
 |            7400    7432  7404    74161    8216    74166     2114  74LS157  E6.9G  |
 |-|                                                                                 |
   |   VR3 VR2 VR1    7432  7404     7400  *74173     7400  74LS139  74LS157         |
   |                                       *74S04                                    |
   |                                                                                 |
   |---------------------------------------------------------------------------------|
Notes:
      All IC's are listed
      All ROMs type 2708 (1K x8)

      6502 clock: 1.173MHz
               *: These 2 IC's piggybacked. 74S04 on top
         VR1/2/3: 5K potentiometers
            2114: 1K x4 SRAM
            2111: 256bytes x4 SRAM
            8216: 256bytes x1 SRAM

Sound PCB
---------

M-15S
|---------------------------|
|                           |
|  NE555  NE555             |
|                           |
|  NE555  NE555             |
|               LM3900   VR1|
|                           |
|  c1815x9               VR2|
|                           |
|               LM3900   VR3|
|                           |
|                        VR4|
|                           |
|                        VR5|
|    TA7222                 |
|---------------------------|
Notes:
      PCB contains lots of resistors, capacitors, transistors etc.

      VR1/2/3/4/5: Potentiometers for volume of each sound
            C1815: Transistor (x9)
           TA7222: Power Amp

***************************************************************************/

/***************************************************************************
Notes (couriersud)

    From http://www.crazykong.com/tech/IremBoardList.txt

    skychut:        M-11 (?)
    andromed:       N/A
    ipminvad:       N/A
    spacbeam:       not listed
    headon:         not listed
    greenber:       N/A

    M10-Board: Has SN76477

    ipminva1
    ========

    This is from an incomplete dump without documentation.
    The filename contained m10 and with a hack to work
    around the missing rom you get some action.

    The files are all different from ipminvad. Either this has
    been a prototype or eventually the famous "capsule invader".

***************************************************************************/
#include "emu.h"
#include "cpu/m6502/m6502.h"
#include "machine/rescap.h"
#include "sound/samples.h"
#include "machine/74123.h"
#include "includes/m10.h"


/*************************************
 *
 *  Defines
 *
 *************************************/

#define DEBUG		(0)

#define LOG(x) do { if (DEBUG) printf x; } while (0)

static WRITE8_DEVICE_HANDLER( ic8j1_output_changed )
{
	m10_state *state = device->machine->driver_data<m10_state>();
	LOG(("ic8j1: %d %d\n", data, device->machine->primary_screen->vpos()));
	cpu_set_input_line(state->maincpu, 0, !data ? CLEAR_LINE : ASSERT_LINE);
}

static WRITE8_DEVICE_HANDLER( ic8j2_output_changed )
{
	m10_state *state = device->machine->driver_data<m10_state>();

	/* written from /Q to A with slight delight */
	LOG(("ic8j2: %d\n", data));
	ttl74123_a_w(device, 0, data);
	ttl74123_a_w(state->ic8j1, 0, data);
}

static const ttl74123_interface ic8j1_intf =
{
	/* completely illegible */
	TTL74123_NOT_GROUNDED_DIODE,	/* the hook up type */
	RES_K(1),				/* resistor connected to RCext */
	CAP_U(1),				/* capacitor connected to Cext and RCext */
	1,					/* A pin - driven by the CRTC */
	1,					/* B pin - pulled high */
	1,					/* Clear pin - pulled high */
	ic8j1_output_changed
};

static const ttl74123_interface ic8j2_intf =
{
	TTL74123_NOT_GROUNDED_DIODE,	/* the hook up type */
	/* 10k + 20k variable resistor */
	RES_K(22),				/* resistor connected to RCext */
	CAP_U(2.2),				/* capacitor connected to Cext and RCext */
	1,					/* A pin - driven by the CRTC */
	1,					/* B pin - pulled high */
	1,					/* Clear pin - pulled high */
	ic8j2_output_changed
};

/*************************************
 *
 *  Initialization
 *
 *************************************/

static PALETTE_INIT( m10 )
{
	int i;

	for (i = 0; i < 0x10; i++)
	{
		rgb_t color;

		if (i & 0x01)
			color = MAKE_RGB(pal1bit(~i >> 3), pal1bit(~i >> 2), pal1bit(~i >> 1));
		else
			color = RGB_BLACK;

		palette_set_color(machine, i, color);
	}
}

static MACHINE_START( m10 )
{
	m10_state *state = machine->driver_data<m10_state>();

	state->maincpu = machine->device("maincpu");
	state->ic8j1 = machine->device("ic8j1");
	state->ic8j2 = machine->device("ic8j2");
	state->samples = machine->device("samples");

	state_save_register_global(machine, state->bottomline);
	state_save_register_global(machine, state->flip);
	state_save_register_global(machine, state->last);
}

static MACHINE_RESET( m10 )
{
	m10_state *state = machine->driver_data<m10_state>();

	state->bottomline = 0;
	state->flip = 0;
	state->last = 0;
}

/*************************************
 *
 *  I/O handling
 *
 *************************************/

/*
 * M10 Ctrl Port
 *
 * 76543210
 * ========
 * e-------     ACTIVE LOW  Demo mode
 * -?------     ????
 * --b-----     ACTIVE LOW  Bottom line
 * ---f----     ACTIVE LOW  Flip screen
 * ----u---     ACTIVE LOW  Ufo sound enable (SN76477)
 * -----sss     Sound #sss start
 *              0x01: MISSILE
 *              0x02: EXPLOSION
 *              0x03: INVADER HIT
 *              0x04: BONUS BASE
 *              0x05: FLEET MOVE
 *              0x06: SAUCER HIT
 */

static WRITE8_HANDLER( m10_ctrl_w )
{
	m10_state *state = space->machine->driver_data<m10_state>();

#if DEBUG
	if (data & 0x40)
		popmessage("ctrl: %02x",data);
#endif

	/* I have NO IDEA if this is correct or not */
	state->bottomline = ~data & 0x20;

	if (input_port_read(space->machine, "CAB") & 0x01)
		state->flip = ~data & 0x10;

	if (!(input_port_read(space->machine, "CAB") & 0x02))
		sound_global_enable(space->machine, ~data & 0x80);

	/* sound command in lower 4 bytes */
	switch (data & 0x07)
	{
		case 0x00:
			/* no sound mapped */
			break;
		case 0x01:
			/* MISSILE sound */
			sample_start(state->samples, 0, 0, 0);
			break;
		case 0x02:
			/* EXPLOSION sound */
			sample_start(state->samples, 1, 1, 0);
			break;
		case 0x03:
			/* INVADER HIT sound */
			sample_start(state->samples, 2, 2, 0);
			break;
		case 0x04:
			/* BONUS BASE sound */
			sample_start(state->samples, 3, 8, 0);
			break;
		case 0x05:
			/* FLEET MOVE sound */
			sample_start(state->samples, 3, 3, 0);
			break;
		case 0x06:
			/* SAUCER HIT SOUND */
			sample_start(state->samples, 2, 7, 0);
			break;
		default:
			popmessage("Unknown sound M10: %02x\n", data & 0x07);
			break;
	}
	/* UFO SOUND */
	if (data & 0x08)
		sample_stop(state->samples, 4);
	else
		sample_start(state->samples, 4, 9, 1);

}

/*
 * M11 Ctrl Port
 *
 * 76543210
 * ========
 * e-------     ACTIVE LOW  Demo mode
 * -?------     ????
 * --b-----     ACTIVE LOW  Bottom line
 * ---f----     ACTIVE LOW  Flip screen
 * ----??--     ????
 * ------cc     Credits indicator ?
 *              0x03: 0 Credits
 *              0x02: 1 Credit
 *              0x00: 2 or more credits
 *              Will be updated only in attract mode
 */

static WRITE8_HANDLER( m11_ctrl_w )
{
	m10_state *state = space->machine->driver_data<m10_state>();

#if DEBUG
	if (data & 0x4c)
		popmessage("M11 ctrl: %02x",data);
#endif

	state->bottomline = ~data & 0x20;

	if (input_port_read(space->machine, "CAB") & 0x01)
		state->flip = ~data & 0x10;

	if (!(input_port_read(space->machine, "CAB") & 0x02))
		sound_global_enable(space->machine, ~data & 0x80);
}

/*
 * M15 Ctrl Port
 *
 * 76543210
 * ========
 * ????----     ????
 * ----e---     ACTIVE LOW  Demo mode
 * -----f--     ACTIVE LOW  Flip screen
 * ------cc     Credits indicator ?
 *              0x03: 0 Credits
 *              0x02: 1 Credit
 *              0x00: 2 or more credits
 *              Will be updated only in attract mode
 */

static WRITE8_HANDLER( m15_ctrl_w )
{
	m10_state *state = space->machine->driver_data<m10_state>();

#if DEBUG
	if (data & 0xf0)
		popmessage("M15 ctrl: %02x",data);
#endif
	if (input_port_read(space->machine, "CAB") & 0x01)
		state->flip = ~data & 0x04;
	if (!(input_port_read(space->machine, "CAB") & 0x02))
		sound_global_enable(space->machine, ~data & 0x08);
}


/*
 * M10 A500
 *
 * 76543210
 * ========
 * ??????--     Always 111111
 * ------cc     Credits indicator ?
 *              0x03: 0 Credits
 *              0x02: 1 Credit
 *              0x00: 2 or more credits
 *              Will be updated only in attract mode
 */

static WRITE8_HANDLER( m10_a500_w )
{
#if DEBUG
	if (data & 0xfc)
		popmessage("a500: %02x",data);
#endif
}

static WRITE8_HANDLER( m11_a100_w )
{
	m10_state *state = space->machine->driver_data<m10_state>();
	int raising_bits = data & ~state->last;
	//int falling_bits = ~data & state->last;

	// should a falling bit stop a sample?
	// This port is written to about 20x per vblank
#if DEBUG
	if ((state->last & 0xe8) != (data & 0xe8))
		popmessage("A100: %02x\n", data);
#endif
	state->last = data;

	// audio control!
	/* MISSILE sound */
	if (raising_bits & 0x01)
		sample_start(state->samples, 0, 0, 0);

	/* EXPLOSION sound */
	if (raising_bits & 0x02)
		sample_start(state->samples, 1, 1, 0);

	/* Rapidly falling parachute */
	if (raising_bits & 0x04)
		sample_start(state->samples, 3, 8, 0);

	/* Background sound ? */
	if (data & 0x10)
		sample_start(state->samples, 4, 9, 1);
	else
		sample_stop(state->samples, 4);

}

static WRITE8_HANDLER( m15_a100_w )
{
	m10_state *state = space->machine->driver_data<m10_state>();
	//int raising_bits = data & ~state->last;
	int falling_bits = ~data & state->last;

	// should a falling bit stop a sample?
	// Bit 4 is used
	// Bit 5 is used 0xef
	// Bit 7 is used

	// headoni
	// 0x01: Acceleration
	// 0x04: background (motor) ?
	// 0x08: explosion
	// 0x10: player changes lane
	// 0x20: computer car changes lane
	// 0x40: dot

#if DEBUG
	if ((state->last & 0x82) != (data & 0x82))
		popmessage("A100: %02x\n", data);
#endif
	/* DOT sound */
	if (falling_bits & 0x40)
		sample_start(state->samples, 0, 0, 0);
#if 0
	if (raising_bits & 0x40)
		sample_stop(state->samples, 0);
#endif

	/* EXPLOSION sound */
	if (falling_bits & 0x08)
		sample_start(state->samples, 1, 1, 0);
#if 0
	if (raising_bits & 0x08)
		sample_stop(state->samples, 1);
#endif

	/* player changes lane */
	if (falling_bits & 0x10)
		sample_start(state->samples, 3, 3, 0);
#if 0
	if (raising_bits & 0x10)
		sample_stop(state->samples, 3);
#endif

	/* computer car changes lane */
	if (falling_bits & 0x20)
		sample_start(state->samples, 4, 4, 0);
#if 0
	if (raising_bits & 0x20)
		sample_stop(state->samples, 4);
#endif

	state->last = data;
}

static READ8_HANDLER( m10_a700_r )
{
	m10_state *state = space->machine->driver_data<m10_state>();
	//LOG(("rd:%d\n",space->machine->primary_screen->vpos()));
	LOG(("clear\n"));
	ttl74123_clear_w(state->ic8j1, 0, 0);
	ttl74123_clear_w(state->ic8j1, 0, 1);
	return 0x00;
}

static READ8_HANDLER( m11_a700_r )
{
	m10_state *state = space->machine->driver_data<m10_state>();
	//LOG(("rd:%d\n",space->machine->primary_screen->vpos()));
	//cpu_set_input_line(state->maincpu, 0, CLEAR_LINE);
	LOG(("clear\n"));
	ttl74123_clear_w(state->ic8j1, 0, 0);
	ttl74123_clear_w(state->ic8j1, 0, 1);
	return 0x00;
}

/*************************************
 *
 *  Interrupt handling
 *
 *************************************/

static INPUT_CHANGED( coin_inserted )
{
	m10_state *state = field->port->machine->driver_data<m10_state>();
	/* coin insertion causes an NMI */
	cpu_set_input_line(state->maincpu, INPUT_LINE_NMI, newval ? ASSERT_LINE : CLEAR_LINE);
}


static TIMER_CALLBACK( interrupt_callback )
{
	m10_state *state = machine->driver_data<m10_state>();
	if (param == 0)
	{
		cpu_set_input_line(state->maincpu, 0, ASSERT_LINE);
		timer_set(machine, machine->primary_screen->time_until_pos(IREMM10_VBSTART + 16), NULL, 1, interrupt_callback);
	}
	if (param == 1)
	{
		cpu_set_input_line(state->maincpu, 0, ASSERT_LINE);
		timer_set(machine, machine->primary_screen->time_until_pos(IREMM10_VBSTART + 24), NULL, 2, interrupt_callback);
	}
	if (param == -1)
		cpu_set_input_line(state->maincpu, 0, CLEAR_LINE);

}

#if 0
static INTERRUPT_GEN( m11_interrupt )
{
	cpu_set_input_line(device, 0, ASSERT_LINE);
	//timer_set(device->machine, machine->primary_screen->time_until_pos(IREMM10_VBEND), NULL, -1, interrupt_callback);
}

static INTERRUPT_GEN( m10_interrupt )
{
	cpu_set_input_line(device, 0, ASSERT_LINE);
}
#endif

static INTERRUPT_GEN( m15_interrupt )
{
	cpu_set_input_line(device, 0, ASSERT_LINE);
	timer_set(device->machine, device->machine->primary_screen->time_until_pos(IREMM10_VBSTART + 1, 80), NULL, -1, interrupt_callback);
}

/*************************************
 *
 *  Main CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( m10_main, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x02ff) AM_RAM AM_BASE_MEMBER(m10_state, memory) /* scratch ram */
	AM_RANGE(0x1000, 0x2fff) AM_ROM AM_BASE_MEMBER(m10_state, rom)
	AM_RANGE(0x4000, 0x43ff) AM_RAM AM_BASE_SIZE_MEMBER(m10_state, videoram, videoram_size)
	AM_RANGE(0x4800, 0x4bff) AM_RAM_WRITE(m10_colorram_w) AM_BASE_MEMBER(m10_state, colorram) /* foreground colour  */
	AM_RANGE(0x5000, 0x53ff) AM_RAM_WRITE(m10_chargen_w) AM_BASE_MEMBER(m10_state, chargen) /* background ????? */
	AM_RANGE(0xa200, 0xa200) AM_READ_PORT("DSW")
	AM_RANGE(0xa300, 0xa300) AM_READ_PORT("INPUTS")
	AM_RANGE(0xa400, 0xa400) AM_WRITE(m10_ctrl_w)	/* line at bottom of screen?, sound, flip screen */
	AM_RANGE(0xa500, 0xa500) AM_WRITE(m10_a500_w)	/* ??? */
	AM_RANGE(0xa700, 0xa700) AM_READ(m10_a700_r)
	AM_RANGE(0xfc00, 0xffff) AM_ROM	/* for the reset / interrupt vectors */
ADDRESS_MAP_END

static ADDRESS_MAP_START( m11_main, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x02ff) AM_RAM AM_BASE_MEMBER(m10_state, memory) /* scratch ram */
	AM_RANGE(0x1000, 0x2fff) AM_ROM AM_BASE_MEMBER(m10_state, rom)
	AM_RANGE(0x4000, 0x43ff) AM_RAM AM_BASE_SIZE_MEMBER(m10_state, videoram, videoram_size)
	AM_RANGE(0x4800, 0x4bff) AM_RAM_WRITE(m10_colorram_w) AM_BASE_MEMBER(m10_state, colorram) /* foreground colour  */
	AM_RANGE(0x5000, 0x53ff) AM_RAM AM_BASE_MEMBER(m10_state, chargen) /* background ????? */
	AM_RANGE(0xa100, 0xa100) AM_WRITE(m11_a100_w) /* sound writes ???? */
	AM_RANGE(0xa200, 0xa200) AM_READ_PORT("DSW")
	AM_RANGE(0xa300, 0xa300) AM_READ_PORT("INPUTS")
	AM_RANGE(0xa400, 0xa400) AM_WRITE(m11_ctrl_w)	/* line at bottom of screen?, sound, flip screen */
	AM_RANGE(0xa700, 0xa700) AM_READ(m11_a700_r)
	AM_RANGE(0xfc00, 0xffff) AM_ROM	/* for the reset / interrupt vectors */
ADDRESS_MAP_END

static ADDRESS_MAP_START( m15_main, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x02ff) AM_RAM AM_BASE_MEMBER(m10_state, memory) /* scratch ram */
	AM_RANGE(0x1000, 0x33ff) AM_ROM AM_BASE_MEMBER(m10_state, rom)
	AM_RANGE(0x4000, 0x43ff) AM_RAM AM_BASE_SIZE_MEMBER(m10_state, videoram, videoram_size)
	AM_RANGE(0x4800, 0x4bff) AM_RAM_WRITE(m10_colorram_w) AM_BASE_MEMBER(m10_state, colorram) /* foreground colour  */
	AM_RANGE(0x5000, 0x57ff) AM_RAM_WRITE(m15_chargen_w) AM_BASE_MEMBER(m10_state, chargen) /* background ????? */
	AM_RANGE(0xa000, 0xa000) AM_READ_PORT("P2")
	AM_RANGE(0xa100, 0xa100) AM_WRITE(m15_a100_w) /* sound writes ???? */
	AM_RANGE(0xa200, 0xa200) AM_READ_PORT("DSW")
	AM_RANGE(0xa300, 0xa300) AM_READ_PORT("P1")
	AM_RANGE(0xa400, 0xa400) AM_WRITE(m15_ctrl_w)	/* sound, flip screen */
	AM_RANGE(0xfc00, 0xffff) AM_ROM	/* for the reset / interrupt vectors */
ADDRESS_MAP_END

/*************************************
 *
 *  Port definitions
 *
 *************************************/

#define CAB_PORTENV \
	/* fake port for cabinet type */					\
	PORT_START("CAB")								\
	PORT_CONFNAME( 0x01, 0x00, DEF_STR( Cabinet ) )		\
	PORT_CONFSETTING(    0x00, DEF_STR( Upright ) )		\
	PORT_CONFSETTING(    0x01, DEF_STR( Cocktail ) )	\
	PORT_CONFNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )		\
	PORT_CONFSETTING(    0x00, DEF_STR( Off ) )		\
	PORT_CONFSETTING(    0x02, DEF_STR( On ) )	\
	PORT_BIT( 0xfc, IP_ACTIVE_HIGH, IPT_UNUSED )


static INPUT_PORTS_START( skychut )
	PORT_START("INPUTS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_COCKTAIL

	PORT_START("DSW")
	PORT_DIPNAME(0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING (  0x00, "3" )
	PORT_DIPSETTING (  0x01, "4" )
	PORT_DIPSETTING (  0x02, "5" )
	PORT_DIPNAME( 0x04, 0x00, "Unknown 1" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Unknown 2" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Unknown 3" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, "Unknown 4" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, "Unknown 5" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, "Unknown 6" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START("FAKE")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_CHANGED(coin_inserted, 0)

	CAB_PORTENV
INPUT_PORTS_END

static INPUT_PORTS_START( ipminvad )
	PORT_START("INPUTS")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_COCKTAIL

	PORT_START("DSW")
	PORT_DIPNAME(0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING (  0x00, "3" )
	PORT_DIPSETTING (  0x01, "4" )
	PORT_DIPSETTING (  0x02, "5" )
	PORT_DIPNAME( 0x04, 0x00, "Capsules" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "1500" )
	PORT_DIPSETTING(    0x08, "1000" )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Unused ) )  // Verified with debugger
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START("FAKE")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_CHANGED(coin_inserted, 0)

	CAB_PORTENV
INPUT_PORTS_END

static INPUT_PORTS_START( spacbeam )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )

	PORT_START("P2")
	PORT_BIT( 0x03, 0, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_COCKTAIL

	PORT_START("DSW")
	PORT_DIPNAME(0x03, 0x01, DEF_STR( Lives ) )
	PORT_DIPSETTING (  0x00, "2" )
	PORT_DIPSETTING (  0x01, "3" )
	PORT_DIPSETTING (  0x02, "4" )
	PORT_DIPSETTING (  0x03, "5" )
	PORT_DIPNAME(0x08, 0x00, "Replay" )
	PORT_DIPSETTING (  0x00, "30000" )
	PORT_DIPSETTING (  0x08, DEF_STR( None ) )
	PORT_DIPNAME(0x30, 0x10, DEF_STR( Coinage ) )
	PORT_DIPSETTING (  0x00, "Testmode" )
	PORT_DIPSETTING (  0x10, "1 Coin 1 Play" )
	PORT_DIPSETTING (  0x20, "1 Coin 2 Plays" )

	PORT_START("FAKE")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_CHANGED(coin_inserted, 0)

	CAB_PORTENV
INPUT_PORTS_END

static INPUT_PORTS_START( headoni )
	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0x03, 0x00, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_COCKTAIL
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_COCKTAIL
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_COCKTAIL
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_COCKTAIL
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_COCKTAIL
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME(0x03, 0x01, DEF_STR( Lives ) )
	PORT_DIPSETTING (  0x00, "2" )
	PORT_DIPSETTING (  0x01, "3" )
	PORT_DIPSETTING (  0x02, "4" )
	PORT_DIPSETTING (  0x03, "5" )
	PORT_DIPNAME(0x08, 0x00, "Replay" )
	PORT_DIPSETTING (  0x00, "30000" )
	PORT_DIPSETTING (  0x08, DEF_STR( None ) )
	PORT_DIPNAME(0x30, 0x10, DEF_STR( Coinage ) )
	PORT_DIPSETTING (  0x00, "Testmode" )
	PORT_DIPSETTING (  0x10, "1 Coin 1 Play" )
	PORT_DIPSETTING (  0x20, "1 Coin 2 Plays" )

	PORT_START("FAKE")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) PORT_CHANGED(coin_inserted, 0)

	CAB_PORTENV
INPUT_PORTS_END



/*************************************
 *
 *  Graphics definitions
 *
 *************************************/


static const gfx_layout charlayout =
{
	8,8,	/* 8*8 characters */
	256,	/* 256 characters */
	1,	/* 1 bits per pixel */
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8	/* every char takes 8 consecutive bytes */
};


static GFXDECODE_START( m10 )
	GFXDECODE_ENTRY( "gfx1", 0x0000, charlayout, 0, 8 )
GFXDECODE_END

/*************************************
 *
 *  Sound definitions
 *
 *************************************/

static const char *const m10_sample_names[] =
{
	"*ipminvad",
	"1.wav",		/* shot/missle */
	"2.wav",		/* base hit/explosion */
	"3.wav",		/* invader hit */
	"4.wav",		/* fleet move 1 */
	"5.wav",		/* fleet move 2 */
	"6.wav",		/* fleet move 3 */
	"7.wav",		/* fleet move 4 */
	"8.wav",		/* UFO/saucer hit */
	"9.wav",		/* bonus base */
	"0.wav",		/* UFO sound */
	0
};


static const samples_interface m10_samples_interface =
{
	6,	/* 6 channels */
	m10_sample_names
};

/*************************************
 *
 *  Machine driver
 *
 *************************************/

static MACHINE_DRIVER_START( m10 )

	MDRV_DRIVER_DATA(m10_state)

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M6502,IREMM10_CPU_CLOCK)
	MDRV_CPU_PROGRAM_MAP(m10_main)

	MDRV_MACHINE_START(m10)
	MDRV_MACHINE_RESET(m10)

	//MDRV_CPU_VBLANK_INT("screen", m10_interrupt)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS(IREMM10_PIXEL_CLOCK, IREMM10_HTOTAL, IREMM10_HBEND, IREMM10_HBSTART, IREMM10_VTOTAL, IREMM10_VBEND, IREMM10_VBSTART)

	MDRV_GFXDECODE(m10)
	MDRV_PALETTE_LENGTH(2*8)

	MDRV_PALETTE_INIT(m10)
	MDRV_VIDEO_START(m10)
	MDRV_VIDEO_UPDATE(m10)

	/* 74LS123 */

	MDRV_TTL74123_ADD("ic8j1", ic8j1_intf)
	MDRV_TTL74123_ADD("ic8j2", ic8j2_intf)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("samples", SAMPLES, 0)
	MDRV_SOUND_CONFIG(m10_samples_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

MACHINE_DRIVER_END

static MACHINE_DRIVER_START( m11 )

	MDRV_DRIVER_DATA(m10_state)

	/* basic machine hardware */
	MDRV_IMPORT_FROM(m10)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(m11_main)
	//MDRV_CPU_VBLANK_INT("screen", m11_interrupt)

	/* sound hardware */
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( m15 )

	MDRV_DRIVER_DATA(m10_state)

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", M6502,IREMM15_CPU_CLOCK)
	MDRV_CPU_PROGRAM_MAP(m15_main)

	MDRV_MACHINE_START(m10)
	MDRV_MACHINE_RESET(m10)

	MDRV_CPU_VBLANK_INT("screen", m15_interrupt)

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_RAW_PARAMS(IREMM15_PIXEL_CLOCK, IREMM15_HTOTAL, IREMM15_HBEND, IREMM15_HBSTART, IREMM15_VTOTAL, IREMM15_VBEND, IREMM15_VBSTART)

	MDRV_PALETTE_LENGTH(2*8)

	MDRV_PALETTE_INIT(m10)
	MDRV_VIDEO_START( m15 )
	MDRV_VIDEO_UPDATE(m15)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD("samples", SAMPLES, 0)
	MDRV_SOUND_CONFIG(m10_samples_interface)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

MACHINE_DRIVER_END


static MACHINE_DRIVER_START( headoni )
	MDRV_IMPORT_FROM(m15)
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_CLOCK(11730000/16)
MACHINE_DRIVER_END

/*************************************
 *
 *  Driver Initialization
 *
 *************************************/

/*
 * Hacks to work around missing roms to get at least some
 * video output
 */
static DRIVER_INIT( andromed )
{
	int i;
	m10_state *state = machine->driver_data<m10_state>();

	for (i = 0x1c00; i < 0x2000; i++)
		state->rom[i] = 0x60;
}

static DRIVER_INIT( ipminva1 )
{
	int i;
	m10_state *state = machine->driver_data<m10_state>();

	for (i = 0x1400; i < 0x17ff; i++)
		state->rom[i] = 0x60;
}

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( andromed )//Jumps to an unmapped sub-routine at $2fc9
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "am1",  0x1000, 0x0400, CRC(53df0152) SHA1(d27113740094d219b0e05a930d8daa4c22129183) )
	ROM_LOAD( "am2",  0x1400, 0x0400, CRC(dab64957) SHA1(77ced520f8e78bb08ddab4213646cf55d834e63e) )
	ROM_LOAD( "am3",  0x1800, 0x0400, CRC(f983f35c) SHA1(1bfee6cf7d18b56594831f2efa7dcc53b47d7e30) )
	ROM_LOAD( "am4",  0x1c00, 0x0400, CRC(09f20717) SHA1(c54c9b7d16b40a7ab49eac255906b43b03939d2b) )
	ROM_RELOAD(       0xfc00, 0x0400 )	/* for the reset and interrupt vectors */
	ROM_LOAD( "am5",  0x2000, 0x0400, CRC(518a3b88) SHA1(5e20c905c2190b381a105327e112fcc0a127bb2f) )
	ROM_LOAD( "am6",  0x2400, 0x0400, CRC(ce3d5fff) SHA1(c34178aca9ffb8b2dd468d9e3369a985f52daf9a) )
	ROM_LOAD( "am7",  0x2800, 0x0400, CRC(30d3366f) SHA1(aa73bba194fa6d1f3909f8df517a0bff07583ea9) )
	ROM_LOAD( "am8",  0x2c00, 0x0400, NO_DUMP ) // $60 entries

	ROM_REGION( 0x0800, "gfx1", 0 )
	ROM_LOAD( "am9",  0x0000, 0x0400, CRC(a1c8f4db) SHA1(bedf5d7126c7e9b91ad595188c69aa2c043c71e8) )
	ROM_LOAD( "am10", 0x0400, 0x0400, CRC(be2de8f3) SHA1(7eb3d1eb88b4481b0dcb7d001207f516a5db32b3) )
ROM_END

ROM_START( ipminvad )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "b1r",  0x1000, 0x0400, CRC(f9a7eb9b) SHA1(93ac65d3ac725d3e4c2fb769816ee808ab609911) )
	ROM_LOAD( "b2r",  0x1400, 0x0400, CRC(af11c1aa) SHA1(6a74fcc7cb1627b1c427a77da89b69ccf3175800) )
	ROM_LOAD( "b3r",  0x1800, 0x0400, CRC(ed49e481) SHA1(8771a34f432e6d88acc5f7529f16c980a77485db) )
	ROM_LOAD( "b4r",  0x1c00, 0x0400, CRC(6d5db95b) SHA1(135500fc17524e8608c3bcfe26321144aa0afb91) )
	ROM_RELOAD(       0xfc00, 0x0400 )	/* for the reset and interrupt vectors */
	ROM_LOAD( "b5r",  0x2000, 0x0400, CRC(eabba7aa) SHA1(75e47eacd429f48f0a3a4539e5ecb4b1ea7281b1) )
	ROM_LOAD( "b6r",  0x2400, 0x0400, CRC(3d0e7fa6) SHA1(14903bfc9506cb8e37807fb397be79f5eab99e3b) )
	ROM_LOAD( "b7r",  0x2800, 0x0400, CRC(cf04864f) SHA1(6fe3ce208334321b63ada779fed69ec7cf4051ad) )

	ROM_REGION( 0x0800, "gfx1", 0 )
	ROM_LOAD( "b9r",  0x0000, 0x0400, CRC(56942cab) SHA1(ba13a856477fc6cf7fd36996e47a3724f862f888) )
	ROM_LOAD( "b10r", 0x0400, 0x0400, CRC(be4b8585) SHA1(0154eae62585e154cf20edcf4599bda8bd333aa9) )
ROM_END

ROM_START( ipminvad1 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "b1g",  0x1000, 0x0400, CRC(069102e2) SHA1(90affe384a688b0d42154633e80b708371117fc2) )
	ROM_LOAD( "b2f",  0x1400, 0x0400, CRC(a6aa5879) SHA1(959ab207110785c03e57ca69c0e62356dd974085) )
	ROM_LOAD( "b3f",  0x1800, 0x0400, CRC(0c09feb9) SHA1(0db43f480162f8e3fb8b61fcceb2884d19ff115b) )
	ROM_LOAD( "b4f",  0x1c00, 0x0400, CRC(a4d32207) SHA1(ea9a01d09d82b8c27701601f03989735558d975c) )
	ROM_RELOAD(       0xfc00, 0x0400 )	/* for the reset and interrupt vectors */
	ROM_LOAD( "b5f",  0x2000, 0x0400, CRC(192361c7) SHA1(b13e80429a9183ce78c4df52a32070416d4ec988) )
	ROM_LOAD( "b6f",  0x2400, 0x0400, NO_DUMP )
	ROM_LOAD( "b7f",  0x2800, 0x0400, CRC(0f5115ab) SHA1(3bdd3fc1cfe6bfacb5820ee12c15f2909d2f58d1) )

	ROM_REGION( 0x0800, "gfx1", 0 )
	ROM_LOAD( "b9",  0x0000, 0x0400, CRC(f6cfa53c) SHA1(ec1076982edee95efb24a1bb08e733bcccacb922) )
	ROM_LOAD( "b10", 0x0400, 0x0400, CRC(63672cd2) SHA1(3d9fa15509a363e1a32e58a2242b266b1162e9a6) )
ROM_END

ROM_START( skychut )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "sc1d",  0x1000, 0x0400, CRC(30b5ded1) SHA1(3a8b4fa344522404661b062808a2ea1d5858fdd0) )
	ROM_LOAD( "sc2d",  0x1400, 0x0400, CRC(fd1f4b9e) SHA1(e5606979abe1fa4cc9eae0c4f61516769db35c39) )
	ROM_LOAD( "sc3d",  0x1800, 0x0400, CRC(67ed201e) SHA1(589b1efdc1bbccff296f6420e2b320cd54b4ac8e) )
	ROM_LOAD( "sc4d",  0x1c00, 0x0400, CRC(9b23a679) SHA1(a101f9b0fdde927a43e53e9b7d0dffb9dcca9e16) )
	ROM_RELOAD(        0xfc00, 0x0400 )	/* for the reset and interrupt vectors */
	ROM_LOAD( "sc5a",  0x2000, 0x0400, CRC(51d975e6) SHA1(7d345025ef28c8a81f599cde445eeb336c368fce) )
	ROM_LOAD( "sc6e",  0x2400, 0x0400, CRC(617f302f) SHA1(4277ef97279eb63fc68b6c40f8545b31abaab474) )
	ROM_LOAD( "sc7",   0x2800, 0x0400, CRC(dd4c8e1a) SHA1(b5a141d8ac256ba6522308e5f194bfaf5c75fa5b) )
	ROM_LOAD( "sc8d",  0x2c00, 0x0400, CRC(aca8b798) SHA1(d9048d060314d8f20ab1967fee846d35c22ac693) )

	ROM_REGION( 0x0800, "gfx1", 0 )
	ROM_LOAD( "sc9d",  0x0000, 0x0400, CRC(2101029e) SHA1(34cddf076d3d860aa03043db14837f42449aefe7) )
	ROM_LOAD( "sc10d", 0x0400, 0x0400, CRC(2f81c70c) SHA1(504935c89a4158a067cbf1dcdb27f7421678915d) )
ROM_END

ROM_START( spacbeam )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "m1b", 0x1000, 0x0400, CRC(5a1c3e0b) SHA1(1c9c58359d74b14ce96934fcc6acefbdfaf1e1be) )
	ROM_LOAD( "m2b", 0x1400, 0x0400, CRC(a02bd9d7) SHA1(d25dfa66b422bdbb29b1922007c84f1947fe9be1) )
	ROM_LOAD( "m3b", 0x1800, 0x0400, CRC(78040843) SHA1(0b8a3ab09dff951aa527649f82b8877cf01126c1) )
	ROM_LOAD( "m4b", 0x1c00, 0x0400, CRC(74705a44) SHA1(8fa9d22a58f08086bf2d89e3d92eca097cdd2cbf) )
	ROM_RELOAD(      0xfc00, 0x0400 )	/* for the reset and interrupt vectors */
	ROM_LOAD( "m5b", 0x2000, 0x0400, CRC(afdf1242) SHA1(e26a8e91edb3d8ba96b3d225813760f42238b003) )
	ROM_LOAD( "m6b", 0x2400, 0x0400, CRC(12afb0c2) SHA1(bf6ed90cf4815f0fb41d435954d4c346a55098f5) )
ROM_END

ROM_START( headoni )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "e1.9a", 0x1000, 0x0400, CRC(05da5265) SHA1(17e0c9261978770325a0befdcdd8a1b07ed39df0) )
	ROM_LOAD( "e2.9b", 0x1400, 0x0400, CRC(dada26a8) SHA1(1368ade1c0c57d33d15594370cf1edf95fc44fd1) )
	ROM_LOAD( "e3.9c", 0x1800, 0x0400, CRC(61ff24f5) SHA1(0e68aedd01b765fb2af76f914b3d287ecf30f716) )
	ROM_LOAD( "e4.9d", 0x1c00, 0x0400, CRC(ce4c5a67) SHA1(8db493d43f311a29127405aad7693bc08b570b14) )
	ROM_RELOAD(        0xfc00, 0x0400 )	/* for the reset and interrupt vectors */
	ROM_LOAD( "e5.9f", 0x2000, 0x0400, CRC(b5232439) SHA1(39b8fb4bbd00a73b9a2b68bc3e88fb45d3f62d7c) )
	ROM_LOAD( "e6.9g", 0x2400, 0x0400, CRC(99acd1a6) SHA1(799382c1b079aad3034a1cc738dc06954978a0ac) )
ROM_END

ROM_START( greenber )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "gb1", 0x1000, 0x0400, CRC(018ff672) SHA1(54d082a332831afc28b47704a5656da0a8a902fa) ) // ok
	ROM_LOAD( "gb2", 0x1400, 0x0400, CRC(ea8f2267) SHA1(ad5bb38a80fbc7c70c8fa6f41086a7ade81655bc) ) // ok
	ROM_LOAD( "gb3", 0x1800, 0x0400, CRC(8f337920) SHA1(ac3d76eb368645ba23f5823b39c04fae49d481e1) ) // ok
	ROM_LOAD( "gb4", 0x1c00, 0x0400, CRC(7eeac4eb) SHA1(c668ad45ebc4aca558371539031efc4ec3990e44) ) // ok
	ROM_RELOAD(      0xfc00, 0x0400 )	/* for the reset and interrupt vectors */
	ROM_LOAD( "gb5", 0x2000, 0x0400, CRC(b2f8e69a) SHA1(44295e58da890a8c4aba6fe90defe9c578c95592) )
	ROM_LOAD( "gb6", 0x2400, 0x0400, CRC(50ea8bd3) SHA1(a816c5fcc603b28c2ae59f217871a7e85fb794e1) )
	ROM_LOAD( "gb7", 0x2800, 0x0400, NO_DUMP ) // 2be8 entry
	ROM_LOAD( "gb8", 0x2c00, 0x0400, CRC(34700b31) SHA1(c148e2475eaaa0e9d1e2412eea359a7ba744e563) )
	ROM_LOAD( "gb9", 0x3000, 0x0400, CRC(c27b9ba3) SHA1(a2f4f0c4b61eb03bba13ae5d25dc01009a4f86ee) ) // ok ?
ROM_END

GAME( 1979, andromed,  0,        m11,     skychut,  andromed, ROT270, "IPM",  "Andromeda (Japan?)", GAME_NO_COCKTAIL | GAME_NO_SOUND | GAME_IMPERFECT_COLORS | GAME_NOT_WORKING | GAME_SUPPORTS_SAVE )
GAME( 1979, ipminvad,  0,        m10,     ipminvad, 0,        ROT270, "IPM",  "IPM Invader", GAME_IMPERFECT_SOUND | GAME_NO_COCKTAIL | GAME_IMPERFECT_COLORS | GAME_SUPPORTS_SAVE )
GAME( 1979, ipminvad1, ipminvad, m10,     ipminvad, ipminva1, ROT270, "IPM",  "IPM Invader (Incomplete Dump)", GAME_IMPERFECT_SOUND | GAME_NOT_WORKING | GAME_SUPPORTS_SAVE )
GAME( 1980, skychut,   0,        m11,     skychut,  0,        ROT270, "Irem", "Sky Chuter", GAME_NO_COCKTAIL | GAME_NO_SOUND | GAME_IMPERFECT_COLORS | GAME_SUPPORTS_SAVE )
GAME( 1979, spacbeam,  0,        m15,     spacbeam, 0,        ROT270, "Irem", "Space Beam", GAME_NO_COCKTAIL | GAME_NO_SOUND | GAME_IMPERFECT_COLORS | GAME_SUPPORTS_SAVE ) // IPM or Irem?
GAME( 1979, headoni,   0,        headoni, headoni,  0,        ROT270, "Irem", "Head On (Irem, M-15 Hardware)", GAME_NO_COCKTAIL | GAME_NO_SOUND | GAME_IMPERFECT_COLORS | GAME_SUPPORTS_SAVE )
GAME( 1980, greenber,  0,        m15,     spacbeam, 0,        ROT270, "Irem", "Green Beret (Irem)", GAME_NO_COCKTAIL | GAME_NO_SOUND | GAME_IMPERFECT_COLORS | GAME_NOT_WORKING | GAME_SUPPORTS_SAVE )
