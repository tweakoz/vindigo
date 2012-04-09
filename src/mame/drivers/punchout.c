/***************************************************************************

Punch Out / Super Punch Out / Arm Wrestling

Arm Wrestling runs on about the same hardware, but the video board is different.
The most significant changes are that Punchout has a larger bottom tilemap,
with scrolling, while Arm Wrestling has an additional FG tilemap displayed on
the bottom screen.

- money bag placement might not be 100% correct in Arm Wrestling, however
  the more serious part of armwrest35b9yel (unplayable bonus round after rounds
  5 and 9) is now fixed.

driver by Nicola Salmoria

TODO:
- Finish emulation of RP5C01 and RP5H01 for spnchout. The RP5C01 features don't
  seem to be used at all except for very basic protection e.g. relying on the
  masking done by the internal registers.


main CPU:

0000-bfff ROM
c000-c3ff NVRAM
d000-d7ff RAM
d800-dfff Video RAM (info screen)
e000-e7ff Video RAM (opponent)
e800-efff Video RAM (player)
f000-f03f Background row scroll (low/high couples)
f000-ffff Video RAM (background)

memory mapped ports:
write:
dfe0-dfef ??

dff0      big sprite #1 zoom low 8 bits
dff1      big sprite #1 zoom high 4 bits
dff2      big sprite #1 x pos low 8 bits
dff3      big sprite #1 x pos high 4 bits
dff4      big sprite #1 y pos low 8 bits
dff5      big sprite #1 y pos high bit
dff6      big sprite #1 x flip (bit 0)
dff7      big sprite #1 bit 0: show on top monitor; bit 1: show on bottom monitor

dff8      big sprite #2 x pos low 8 bits
dff9      big sprite #2 x pos high bit
dffa      big sprite #2 y pos low 8 bits
dffb      big sprite #2 y pos high bit
dffc      big sprite #2 x flip (bit 0)
dffd      palette bank (bit 0 = bottom monitor bit 1 = top monitor)

I/O
read:
00        IN0
01        IN1
02        DSW0
03        DSW1 (bit 4: VLM5030 busy signal)

write:
00        to 2A03 #1 IN0 (unpopulated)
01        to 2A03 #1 IN1 (unpopulated)
02        to 2A03 #2 IN0
03        to 2A03 #2 IN1
04        to VLM5030
08        NMI enable + watchdog reset
09        watchdog reset
0a        ? latched into Z80 BUS RQ
0b        to 2A03 #1 and #2 RESET
0c        to VLM5030 RESET
0d        to VLM5030 START
0e        to VLM5030 VCU
0f        enable NVRAM ?

sound CPU:
the sound CPU is a 2A03, which is a modified 6502 with built-in input ports
and two (analog?) outputs. The input ports are memory mapped at 4016-4017;
the outputs are more complicated. The only thing I have found is that 4011
goes straight into a DAC and produces the crowd sounds, but several addresses
in the range 4000-4017 are written to. There are probably three tone generators.

0000-07ff RAM
e000-ffff ROM

read:
4016      IN0
4017      IN1

write:
4000      ? is usually ORed with 90 or 50
4001      ? usually 7f, could be associated with 4000
4002-4003 ? tone #1 freq? (bit 3 of 4003 is always 1, bits 4-7 always 0)
4004      ? is usually ORed with 90 or 50
4005      ? usually 7f, could be associated with 4004
4006-4007 ? tone #2 freq? (bit 3 of 4007 is always 1, bits 4-7 always 0)
4008      ? at one point the max value is cut at 38
400a-400b ? tone #3 freq? (bit 3 of 400b is always 1, bits 4-7 always 0)
400c      ?
400e-400f ?
4011      DAC crowd noise
4015      ?? 00 or 0f
4017      ?? always c0

***************************************************************************

DIP locations verified for:
    -punchout (manual)
    -spnchout (manual)

***************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "cpu/m6502/m6502.h"
#include "sound/vlm5030.h"
#include "sound/nes_apu.h"
#include "machine/nvram.h"
#include "rendlay.h"
#include "includes/punchout.h"


CUSTOM_INPUT_MEMBER(punchout_state::punchout_vlm5030_busy_r)
{
	/* bit 4 of DSW1 is busy pin level */
	return (vlm5030_bsy(machine().device("vlm"))) ? 0x00 : 0x01;
}

static WRITE8_DEVICE_HANDLER( punchout_speech_reset_w )
{
	vlm5030_rst( device, data & 0x01 );
}

static WRITE8_DEVICE_HANDLER( punchout_speech_st_w )
{
	vlm5030_st( device, data & 0x01 );
}

static WRITE8_DEVICE_HANDLER( punchout_speech_vcu_w )
{
	vlm5030_vcu( device, data & 0x01 );
}

WRITE8_MEMBER(punchout_state::punchout_2a03_reset_w)
{
	if (data & 1)
		cputag_set_input_line(machine(), "audiocpu", INPUT_LINE_RESET, ASSERT_LINE);
	else
		cputag_set_input_line(machine(), "audiocpu", INPUT_LINE_RESET, CLEAR_LINE);
}


READ8_MEMBER(punchout_state::spunchout_rp5c01_r)
{
	logerror("%04x: prot_r %x\n", cpu_get_previouspc(&space.device()), offset);

	if (offset <= 0x0c)
	{
		switch (m_rp5c01_mode_sel & 3)
		{
			case 0:	// time
				switch ( offset )
				{
					case 0x00:	// 1-second counter
						return m_rp5c01_mem[0x00];

					case 0x01:	// 10-second counter
						return m_rp5c01_mem[0x01] & 0x7;

					case 0x02:	// 1-minute counter
						return m_rp5c01_mem[0x02];

					case 0x03:	// 10-minute counter
						return m_rp5c01_mem[0x03] & 0x07;

					case 0x04:	// 1-hour counter
						return m_rp5c01_mem[0x04];

					case 0x05:	// 10-hour counter
						return m_rp5c01_mem[0x05] & 0x03;

					case 0x06:	// day-of-the-week counter
						return m_rp5c01_mem[0x06] & 0x07;

					case 0x07:	// 1-day counter
						return m_rp5c01_mem[0x07];

					case 0x08:	// 10-day counter
						return m_rp5c01_mem[0x08] & 0x03;

					case 0x09:	// 1-month counter
						return m_rp5c01_mem[0x09];

					case 0x0a:	// 10-month counter
						return m_rp5c01_mem[0x0a] & 0x01;

					case 0x0b:	// 1-year counter
						return m_rp5c01_mem[0x0b];

					case 0x0c:	// 10-year counter
						return m_rp5c01_mem[0x0c];
				}
				break;

			case 1:	// alarm
				switch ( offset )
				{
					case 0x00:	// n/a
						return 0x00;

					case 0x01:	// n/a
						return 0x00;

					case 0x02:	// 1-minute alarm register
						return m_rp5c01_mem[0x12];

					case 0x03:	// 10-minute alarm register
						return m_rp5c01_mem[0x13] & 0x07;

					case 0x04:	// 1-hour alarm register
						return m_rp5c01_mem[0x14];

					case 0x05:	// 10-hour alarm register
						return m_rp5c01_mem[0x15] & 0x03;

					case 0x06:	// day-of-the-week alarm register
						return m_rp5c01_mem[0x16] & 0x07;

					case 0x07:	// 1-day alarm register
						return m_rp5c01_mem[0x17];

					case 0x08:	// 10-day alarm register
						return m_rp5c01_mem[0x18] & 0x03;

					case 0x09:	// n/a
						return 0x00;

					case 0x0a:	// /12/24 select register
						return m_rp5c01_mem[0x1a] & 0x01;

					case 0x0b:	// leap year count
						return m_rp5c01_mem[0x1b] & 0x03;

					case 0x0c:	// n/a
						return 0x00;
				}
				break;

			case 2:	// RAM BLOCK 10
			case 3:	// RAM BLOCK 11
				return m_rp5c01_mem[0x10 * (m_rp5c01_mode_sel & 3) + offset];
		}
	}
	else if (offset == 0x0d)
	{
		return m_rp5c01_mode_sel;
	}

	logerror("Read from unknown protection? port %02x ( selector = %02x )\n", offset, m_rp5c01_mode_sel );
	return 0;
}

WRITE8_MEMBER(punchout_state::spunchout_rp5c01_w)
{
	data &= 0x0f;

	logerror("%04x: prot_w %x = %02x\n",cpu_get_previouspc(&space.device()),offset,data);

	if (offset <= 0x0c)
	{
		m_rp5c01_mem[0x10 * (m_rp5c01_mode_sel & 3) + offset] = data;
	}
	else if (offset == 0x0d)
	{
		m_rp5c01_mode_sel = data;
		logerror("MODE: Timer EN = %d  Alarm EN = %d  MODE %d\n",BIT(data,3),BIT(data,2),data&3);
	}
	else if (offset == 0x0e)
	{
		logerror("TEST = %d",data);
	}
	else if (offset == 0x0f)
	{
		logerror("RESET: /1Hz = %d  /16Hz = %d  Timer = %d  Timer = %d\n",BIT(data,3),BIT(data,2),BIT(data,1),BIT(data,0));
	}
}

READ8_MEMBER(punchout_state::spunchout_exp_r)
{
	// bit 7 = DATA OUT from RP5H01
	// bit 6 = COUNTER OUT from RP5H01
	// bit 5 = /ALARM from RP5C01
	// bit 4 = n.c.
	// bits 3-0 = D3-D0 from RP5C01

	UINT8 ret = spunchout_rp5c01_r( space, offset >> 4 );

	// FIXME hack
	/* PC = 0x0313 */
	/* (ret or 0x10) -> (D7DF),(D7A0) - (D7DF),(D7A0) = 0d0h(ret nc) */

	if (cpu_get_previouspc(&space.device()) == 0x0313)
		ret |= 0xc0;

	return ret;
}

WRITE8_MEMBER(punchout_state::spunchout_exp_w)
{
	spunchout_rp5c01_w( space, offset >> 4, data );
}



static ADDRESS_MAP_START( punchout_map, AS_PROGRAM, 8, punchout_state )
	AM_RANGE(0x0000, 0xbfff) AM_ROM
	AM_RANGE(0xc000, 0xc3ff) AM_RAM AM_SHARE("nvram")
	AM_RANGE(0xd000, 0xd7ff) AM_RAM
	AM_RANGE(0xd800, 0xdfff) AM_RAM_WRITE(punchout_bg_top_videoram_w) AM_BASE(m_bg_top_videoram)
	AM_RANGE(0xdff0, 0xdff7) AM_BASE(m_spr1_ctrlram)
	AM_RANGE(0xdff8, 0xdffc) AM_BASE(m_spr2_ctrlram)
	AM_RANGE(0xdffd, 0xdffd) AM_BASE(m_palettebank)
	AM_RANGE(0xe000, 0xe7ff) AM_RAM_WRITE(punchout_spr1_videoram_w) AM_BASE(m_spr1_videoram)
	AM_RANGE(0xe800, 0xefff) AM_RAM_WRITE(punchout_spr2_videoram_w) AM_BASE(m_spr2_videoram)
	AM_RANGE(0xf000, 0xffff) AM_RAM_WRITE(punchout_bg_bot_videoram_w) AM_BASE(m_bg_bot_videoram)	// also contains scroll RAM
ADDRESS_MAP_END


static ADDRESS_MAP_START( armwrest_map, AS_PROGRAM, 8, punchout_state )
	AM_RANGE(0x0000, 0xbfff) AM_ROM
	AM_RANGE(0xc000, 0xc3ff) AM_RAM AM_SHARE("nvram")
	AM_RANGE(0xd000, 0xd7ff) AM_RAM
	AM_RANGE(0xd800, 0xdfff) AM_RAM_WRITE(armwrest_fg_videoram_w) AM_BASE(m_armwrest_fg_videoram)
	AM_RANGE(0xdff0, 0xdff7) AM_BASE(m_spr1_ctrlram)
	AM_RANGE(0xdff8, 0xdffc) AM_BASE(m_spr2_ctrlram)
	AM_RANGE(0xdffd, 0xdffd) AM_BASE(m_palettebank)
	AM_RANGE(0xe000, 0xe7ff) AM_RAM_WRITE(punchout_spr1_videoram_w) AM_BASE(m_spr1_videoram)
	AM_RANGE(0xe800, 0xefff) AM_RAM_WRITE(punchout_spr2_videoram_w) AM_BASE(m_spr2_videoram)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM_WRITE(punchout_bg_bot_videoram_w) AM_BASE(m_bg_bot_videoram)
	AM_RANGE(0xf800, 0xffff) AM_RAM_WRITE(punchout_bg_top_videoram_w) AM_BASE(m_bg_top_videoram)
ADDRESS_MAP_END

WRITE8_MEMBER(punchout_state::nmi_mask_w)
{

	m_nmi_mask = data & 1;
}

static ADDRESS_MAP_START( punchout_io_map, AS_IO, 8, punchout_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_READ_PORT("IN0")
	AM_RANGE(0x01, 0x01) AM_READ_PORT("IN1")
	AM_RANGE(0x00, 0x01) AM_WRITENOP	/* the 2A03 #1 is not present */
	AM_RANGE(0x02, 0x02) AM_READ_PORT("DSW2") AM_WRITE(soundlatch_w)
	AM_RANGE(0x03, 0x03) AM_READ_PORT("DSW1") AM_WRITE(soundlatch2_w)
	AM_RANGE(0x04, 0x04) AM_DEVWRITE_LEGACY("vlm", vlm5030_data_w)	/* VLM5030 */
//  AM_RANGE(0x05, 0x05) AM_WRITENOP  /* unused */
//  AM_RANGE(0x06, 0x06) AM_WRITENOP
	AM_RANGE(0x08, 0x08) AM_WRITE(nmi_mask_w)
	AM_RANGE(0x09, 0x09) AM_WRITENOP	/* watchdog reset, seldom used because 08 clears the watchdog as well */
	AM_RANGE(0x0a, 0x0a) AM_WRITENOP	/* ?? */
	AM_RANGE(0x0b, 0x0b) AM_WRITE(punchout_2a03_reset_w)
	AM_RANGE(0x0c, 0x0c) AM_DEVWRITE_LEGACY("vlm", punchout_speech_reset_w)	/* VLM5030 */
	AM_RANGE(0x0d, 0x0d) AM_DEVWRITE_LEGACY("vlm", punchout_speech_st_w)	/* VLM5030 */
	AM_RANGE(0x0e, 0x0e) AM_DEVWRITE_LEGACY("vlm", punchout_speech_vcu_w)	/* VLM5030 */
	AM_RANGE(0x0f, 0x0f) AM_WRITENOP	/* enable NVRAM ? */

	/* protection ports - Super Punchout only (move to install handler?) */
	AM_RANGE(0x07, 0x07) AM_MIRROR(0xf0) AM_MASK(0xf0) AM_READWRITE(spunchout_exp_r, spunchout_exp_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( punchout_sound_map, AS_PROGRAM, 8, punchout_state )
	AM_RANGE(0x0000, 0x07ff) AM_RAM
	AM_RANGE(0x4016, 0x4016) AM_READ(soundlatch_r)
	AM_RANGE(0x4017, 0x4017) AM_READ(soundlatch2_r)
	AM_RANGE(0x4000, 0x4017) AM_DEVREADWRITE_LEGACY("nes", nes_psg_r,nes_psg_w)
	AM_RANGE(0xe000, 0xffff) AM_ROM
ADDRESS_MAP_END



static INPUT_PORTS_START( punchout )
	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_4WAY
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_4WAY
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_COIN1 )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Difficulty ) )	PORT_DIPLOCATION("SW2:!1,!2")
	PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x03, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x0c, 0x00, "Time" )					PORT_DIPLOCATION("SW2:!3,!4")
	PORT_DIPSETTING(    0x00, "Longest" )
	PORT_DIPSETTING(    0x04, "Long" )
	PORT_DIPSETTING(    0x08, "Short" )
	PORT_DIPSETTING(    0x0c, "Shortest" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Demo_Sounds ) )	PORT_DIPLOCATION("SW2:!5")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, "Rematch At A Discount" )	PORT_DIPLOCATION("SW2:!6")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPUNUSED_DIPLOC( 0x40, 0x00, "SW2:!7" )		/* Listed as "Unused" */
	PORT_SERVICE_DIPLOC( 0x80, IP_ACTIVE_HIGH, "SW2:!8" )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x0f, 0x00, DEF_STR( Coinage ) )		PORT_DIPLOCATION("SW1:!1,!2,!3,!4")
	PORT_DIPSETTING(    0x0e, DEF_STR( 5C_1C ) )		/* Not documented */
	PORT_DIPSETTING(    0x0b, DEF_STR( 4C_1C ) )		/* Not documented */
	PORT_DIPSETTING(    0x0c, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) )        /* Not documented */
	PORT_DIPSETTING(    0x08, "1 Coin/2 Credits (2 Credits/1 Play)" ) /* Not documented */
	PORT_DIPSETTING(    0x0d, "1 Coin/3 Credits (2 Credits/1 Play)" ) /* Not documented */
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
//  PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )        /* Not documented */
//  PORT_DIPSETTING(    0x09, DEF_STR( 1C_2C ) )        /* Not documented */
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( Free_Play ) )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, punchout_state,punchout_vlm5030_busy_r, NULL)	/* VLM5030 busy signal */
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_DIPUNUSED_DIPLOC( 0x40, 0x00, "R18:!1" )		/* Not documented, R18 resistor */
	PORT_DIPNAME( 0x80, 0x00, "Copyright" )				PORT_DIPLOCATION("R19:!1") /* Not documented, R19 resistor */
	PORT_DIPSETTING(    0x00, "Nintendo" )
	PORT_DIPSETTING(    0x80, "Nintendo of America Inc." )
INPUT_PORTS_END

/* same as punchout with additional duck button */
static INPUT_PORTS_START( spnchout )
	PORT_INCLUDE( punchout )

	PORT_MODIFY("IN0")
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_BUTTON4 )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x0f, 0x00, DEF_STR( Coinage ) )		PORT_DIPLOCATION("SW1:!1,!2,!3,!4")
	PORT_DIPSETTING(    0x08, DEF_STR( 6C_1C ) )		/* Not documented */
	PORT_DIPSETTING(    0x04, DEF_STR( 5C_1C ) )		/* Not documented */
	PORT_DIPSETTING(    0x03, DEF_STR( 4C_1C ) )		/* Not documented */
//  PORT_DIPSETTING(    0x09, DEF_STR( 4C_1C ) )        /* Not documented */
	PORT_DIPSETTING(    0x0c, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
//  PORT_DIPSETTING(    0x0e, DEF_STR( 1C_1C ) )        /* Not documented */
	PORT_DIPSETTING(    0x0d, "1 Coin/3 Credits (2 Credits/1 Play)" ) /* Not documented */
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0b, "1 Coin/2 Credits (3 Credits/1 Play)" ) /* Not documented */
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( Free_Play ) )
INPUT_PORTS_END

static INPUT_PORTS_START( armwrest )
	PORT_INCLUDE( punchout )

	PORT_MODIFY("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY

	PORT_MODIFY("IN1")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_SERVICE1 )

	/* Coinage:

    R18 (Coin Slots setting) determines which table is used.

    L = number of credits per coin for left coin slot
    R = number of credits per coin for right coin slot
    C = number of credits needed for one play

Table 1 (for 2 Coin Slots):

    DSW1 DSW2
bit 3210 5432  L  R  C

    0000 0000  1  1  1
    0000 0001  8  1  1
    0000 0010  2  1  1
    0000 0011  8  3  3
    0000 0100  3  1  2
    0000 0101  3  12 4
    0000 0110  1  2  2
    0000 0111  3  24 2
    0000 1000  1  3  3
    0000 1001  4  1  3
    0000 1010  1  4  4
    0000 1011  3  3  4
    0000 1100  1  5  5
    0000 1101  1  1  1
    0000 1110  2  3  3
    0000 1111  5  5  3

    0001 0000  1  2  1
    0001 0001  1  8  1
    0001 0010  2  2  1
    0001 0011  3  8  3
    0001 0100  3  2  1
    0001 0101  12 3  4
    0001 0110  1  4  2
    0001 0111  24 3  2
    0001 1000  1  6  3
    0001 1001  1  4  3
    0001 1010  1  8  4
    0001 1011  3  3  4
    0001 1100  1  10 5
    0001 1101  1  1  2
    0001 1110  2  6  3
    0001 1111  5  5  4

    0010 0000  1  3  1
    0010 0001  9  1  1
    0010 0010  2  3  1
    0010 0011  3  4  4
    0010 0100  3  3  1
    0010 0101  4  4  1
    0010 0110  1  6  2
    0010 0111  3  1  2
    0010 1000  1  9  3
    0010 1001  10 1  3
    0010 1010  1  12 4
    0010 1011  1  1  6
    0010 1100  1  15 5
    0010 1101  2  2  1
    0010 1110  2  9  3
    0010 1111  2  2  5

    0011 0000  1  4  1
    0011 0001  1  9  1
    0011 0010  2  4  1
    0011 0011  4  3  4
    0011 0100  3  4  1
    0011 0101  4  4  1
    0011 0110  1  8  2
    0011 0111  1  3  2
    0011 1000  1  12 3
    0011 1001  1  10 3
    0011 1010  1  16 4
    0011 1011  1  1  6
    0011 1100  1  20 5
    0011 1101  1  1  4
    0011 1110  2  12 3
    0011 1111  8  8  1

    0100 0000  1  5  1
    0100 0001  10 1  1
    0100 0010  2  5  1
    0100 0011  2  5  5
    0100 0100  3  5  1
    0100 0101  4  4  1
    0100 0110  1  10 2
    0100 0111  10 1  2
    0100 1000  1  15 3
    0100 1001  20 1  3
    0100 1010  1  20 4
    0100 1011  16 1  2
    0100 1100  1  25 5
    0100 1101  1  1  5
    0100 1110  2  15 3
    0100 1111  8  8  3

    0101 0000  1  6  1
    0101 0001  1  10 1
    0101 0010  2  6  1
    0101 0011  5  2  5
    0101 0100  3  6  1
    0101 0101  4  5  1
    0101 0110  1  12 2
    0101 0111  1  10 2
    0101 1000  1  18 3
    0101 1001  1  20 3
    0101 1010  1  24 4
    0101 1011  20 20 0  Not a "Freeplay": you MUST insert a coin!
    0101 1100  1  30 5
    0101 1101  3  3  3
    0101 1110  2  18 1
    0101 1111  9  9  1

    0110 0000  2  1  2
    0110 0001  12 1  1
    0110 0010  4  1  2
    0110 0011  10 2  1
    0110 0100  6  1  2
    0110 0101  3  8  2
    0110 0110  1  1  2
    0110 0111  3  2  4
    0110 1000  2  3  6
    0110 1001  9  4  12
    0110 1010  1  2  4
    0110 1011  1  1  1
    0110 1100  2  5  10
    0110 1101  4  4  1
    0110 1110  4  3  6
    0110 1111  9  9  2

    0111 0000  3  1  3
    0111 0001  1  12 1
    0111 0010  6  1  3
    0111 0011  2  10 1
    0111 0100  9  1  3
    0111 0101  8  3  2
    0111 0110  3  2  6
    0111 0111  2  3  4
    0111 1000  1  1  3
    0111 1001  4  9  12
    0111 1010  3  4  12
    0111 1011  1  1  1
    0111 1100  3  5  15
    0111 1101  6  6  1
    0111 1110  2  2  3
    0111 1111  10 10 1

    1000 0000  4  1  4
    1000 0001  3  2  2
    1000 0010  8  1  4
    1000 0011  12 2  1
    1000 0100  12 1  4
    1000 0101  5  5  1
    1000 0110  2  1  4
    1000 0111  3  3  2
    1000 1000  4  3  12
    1000 1001  4  2  3
    1000 1010  1  1  4
    1000 1011  1  1  1
    1000 1100  4  5  20
    1000 1101  1  1  6
    1000 1110  8  3  12
    1000 1111  10 10 3

    1001 0000  5  1  5
    1001 0001  2  3  2
    1001 0010  10 1  5
    1001 0011  2  12 1
    1001 0100  15 1  5
    1001 0101  5  5  1
    1001 0110  5  2  10
    1001 0111  3  3  2
    1001 1000  5  3  15
    1001 1001  2  4  3
    1001 1010  5  4  20
    1001 1011  1  1  1
    1001 1100  1  1  5
    1001 1101  2  2  3
    1001 1110  10 3  15
    1001 1111  11 11 1

    1010 0000  3  2  3
    1010 0001  5  2  2
    1010 0010  6  2  3
    1010 0011  3  4  2
    1010 0100  9  2  3
    1010 0101  6  6  1
    1010 0110  3  4  6
    1010 0111  4  9  6
    1010 1000  1  2  3
    1010 1001  10 2  3
    1010 1010  3  8 12
    1010 1011  1  1  1
    1010 1100  2  10 15
    1010 1101  5  5  1
    1010 1110  2  2  3
    1010 1111  11 11 3

    1011 0000  3  5  3
    1011 0001  2  5  2
    1011 0010  6  5  3
    1011 0011  4  3  2
    1011 0100  9  5  3
    1011 0101  6  6  1
    1011 0110  3  10 6
    1011 0111  9  4  6
    1011 1000  1  5  3
    1011 1001  2  10 3
    1011 1010  3  20 12
    1011 1011  1  1  1
    1011 1100  3  25 15
    1011 1101  3  3  2
    1011 1110  2  5  3
    1011 1111  12 12 1

    1100 0000  4  5  4
    1100 0001  9  2  2
    1100 0010  8  5  4
    1100 0011  3  8  4
    1100 0100  12 5  4
    1100 0101  3  12 2
    1100 0110  2  5  4
    1100 0111  8  9  6
    1100 1000  4  15 12
    1100 1001  11 2  3
    1100 1010  1  5  4
    1100 1011  1  1  1
    1100 1100  4  25 20
    1100 1101  1  1  3
    1100 1110  8  15 12
    1100 1111  20 20 3

    1101 0000  4  1  1
    1101 0001  2  9  2
    1101 0010  4  2  1
    1101 0011  8  3  4
    1101 0100  4  3  1
    1101 0101  12 3  2
    1101 0110  8  1  2
    1101 0111  9  8  6
    1101 1000  12 1  3
    1101 1001  2  11 3
    1101 1010  12 2  3
    1101 1011  1  1  1
    1101 1100  20 1  5
    1101 1101  5  5  2
    1101 1110  18 2  3
    1101 1111  3  3  4

    1110 0000  5  5  1
    1110 0001  4  4  3
    1110 0010  5  2  1
    1110 0011  11 2  1
    1110 0100  5  3  1
    1110 0101  3  24 4
    1110 0110  10 1  2
    1110 0111  1  6  4
    1110 1000  15 1  3
    1110 1001  9  8  12
    1110 1010  20 1  4
    1110 1011  1  1  1
    1110 1100  25 1  5
    1110 1101  4  4  3
    1110 1110  20 4  4
    1110 1111  20 20 0  Not a "Freeplay": you MUST insert a coin!

    1111 0000  6  1  1
    1111 0001  3  4  3
    1111 0010  6  2  1
    1111 0011  2  11 1
    1111 0100  6  3  1
    1111 0101  24 3  1
    1111 0110  12 1  2
    1111 0111  6  1  4
    1111 1000  18 1  3
    1111 1001  8  9  12
    1111 1010  24 1  4
    1111 1011  1  1  1
    1111 1100  15 2  3
    1111 1101  3  3  4
    1111 1110  "Freeplay"
    1111 1111  "Freeplay"


Table 2 (for 1 Coin Slot):

    DSW1 DSW2
bit 3210 5432  L  R  C

    0000 0xxx  1  1  1
    0000 1xxx  5  5  3

    0001 0xxx  1  1  2
    0001 1xxx  5  5  4

    0010 0xxx  2  2  1
    0010 1xxx  2  2  5

    0011 0xxx  1  1  4
    0011 1xxx  8  8  1

    0100 0xxx  1  1  5
    0100 1xxx  8  8  3

    0101 0xxx  3  3  1
    0101 1xxx  9  9  1

    0110 0xxx  4  4  1
    0110 1xxx  9  9  2

    0111 0xxx  6  6  1
    0111 1xxx  10 10 1

    1000 0xxx  1  1  6
    1000 1xxx  10 10 3

    1001 0xxx  5  1  5
    1001 1xxx  11 11 1

    1010 0xxx  5  5  1
    1010 1xxx  11 11 3

    1011 0xxx  3  3  2
    1011 1xxx  12 12 1

    1100 0xxx  1  1  3
    1100 1xxx  20 20 3

    1101 0xxx  5  5  2
    1101 1xxx  3  3  4

    1110 0xxx  4  4  3
    1110 1xxx  20 20 0  Not a "Freeplay": you MUST insert a coin!

    1111 xxxx  "Freeplay"

    */

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x3c, 0x00, "Coinage 2" )				PORT_DIPLOCATION("SW2:!3,!4,!5,!6") // K,L,M,N
	PORT_DIPSETTING(    0x00, "0000" )
	PORT_DIPSETTING(    0x04, "0001" )
	PORT_DIPSETTING(    0x08, "0010" )
	PORT_DIPSETTING(    0x0c, "0011" )
	PORT_DIPSETTING(    0x10, "0100" )
	PORT_DIPSETTING(    0x14, "0101" )
	PORT_DIPSETTING(    0x18, "0110" )
	PORT_DIPSETTING(    0x1c, "0111" )
	PORT_DIPSETTING(    0x20, "1000" )
	PORT_DIPSETTING(    0x24, "1001" )
	PORT_DIPSETTING(    0x28, "1010" )
	PORT_DIPSETTING(    0x2c, "1011" )
	PORT_DIPSETTING(    0x30, "1100" )
	PORT_DIPSETTING(    0x34, "1101" )
	PORT_DIPSETTING(    0x38, "1110" )
	PORT_DIPSETTING(    0x3c, "1111" )
	PORT_DIPNAME( 0x40, 0x00, "Rematches" )				PORT_DIPLOCATION("SW2:!7")
	PORT_DIPSETTING(    0x40, "3" )
	PORT_DIPSETTING(    0x00, "7" )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x0f, 0x00, "Coinage 1" )				PORT_DIPLOCATION("SW1:!1,!2,!3,!4") // A,B,C,D
	PORT_DIPSETTING(    0x00, "0000" )
	PORT_DIPSETTING(    0x01, "0001" )
	PORT_DIPSETTING(    0x02, "0010" )
	PORT_DIPSETTING(    0x03, "0011" )
	PORT_DIPSETTING(    0x04, "0100" )
	PORT_DIPSETTING(    0x05, "0101" )
	PORT_DIPSETTING(    0x06, "0110" )
	PORT_DIPSETTING(    0x07, "0111" )
	PORT_DIPSETTING(    0x08, "1000" )
	PORT_DIPSETTING(    0x09, "1001" )
	PORT_DIPSETTING(    0x0a, "1010" )
	PORT_DIPSETTING(    0x0b, "1011" )
	PORT_DIPSETTING(    0x0c, "1100" )
	PORT_DIPSETTING(    0x0d, "1101" )
	PORT_DIPSETTING(    0x0e, "1110" )
	PORT_DIPSETTING(    0x0f, "1111" )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_SPECIAL ) PORT_CUSTOM_MEMBER(DEVICE_SELF, punchout_state,punchout_vlm5030_busy_r, NULL)	/* VLM5030 busy signal */
	PORT_DIPNAME( 0x40, 0x00, "Coin Slots" )			PORT_DIPLOCATION("R18:!1") /* R18 resistor */
	PORT_DIPSETTING(    0x40, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPUNUSED_DIPLOC( 0x80, 0x00, "R19:!1" )		/* R19 resistor */
INPUT_PORTS_END


static const gfx_layout charlayout_2bpp =
{
	8,8,
	RGN_FRAC(1,2),
	2,
	{ RGN_FRAC(1,2), 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static const gfx_layout charlayout_3bpp =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(2,3), RGN_FRAC(1,3), 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static GFXDECODE_START( punchout )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout_2bpp, 0x000, 0x100/4 )	// bg chars (top monitor only)
	GFXDECODE_ENTRY( "gfx2", 0, charlayout_2bpp, 0x100, 0x100/4 )	// bg chars (bottom monitor only)
	GFXDECODE_ENTRY( "gfx3", 0, charlayout_3bpp, 0x000, 0x200/8 )	// big sprite #1 (top and bottom monitor)
	GFXDECODE_ENTRY( "gfx4", 0, charlayout_2bpp, 0x100, 0x100/4 )	// big sprite #2 (bottom monitor only)
GFXDECODE_END

static GFXDECODE_START( armwrest )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout_2bpp, 0x000, 0x200/4 )	// bg chars (top and bottom monitor)
	GFXDECODE_ENTRY( "gfx2", 0, charlayout_3bpp, 0x100, 0x100/8 )	// fg chars (bottom monitor only)
	GFXDECODE_ENTRY( "gfx3", 0, charlayout_3bpp, 0x000, 0x200/8 )	// big sprite #1 (top and bottom monitor)
	GFXDECODE_ENTRY( "gfx4", 0, charlayout_2bpp, 0x100, 0x100/4 )	// big sprite #2 (bottom monitor only)
GFXDECODE_END



static const nes_interface nes_config =
{
	"audiocpu"
};

static MACHINE_RESET( punchout )
{
	punchout_state *state = machine.driver_data<punchout_state>();
	state->m_rp5c01_mode_sel = 0;
	memset(state->m_rp5c01_mem, 0, sizeof(state->m_rp5c01_mem));
}

static INTERRUPT_GEN( vblank_irq )
{
	punchout_state *state = device->machine().driver_data<punchout_state>();

	if(state->m_nmi_mask)
		device_set_input_line(device, INPUT_LINE_NMI, PULSE_LINE);
}



static MACHINE_CONFIG_START( punchout, punchout_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80, 8000000/2)	/* 4 MHz */
	MCFG_CPU_PROGRAM_MAP(punchout_map)
	MCFG_CPU_IO_MAP(punchout_io_map)
	MCFG_CPU_VBLANK_INT("top", vblank_irq)

	MCFG_CPU_ADD("audiocpu", N2A03, N2A03_DEFAULTCLOCK)
	MCFG_CPU_PROGRAM_MAP(punchout_sound_map)
	MCFG_CPU_VBLANK_INT("top", nmi_line_pulse)

	MCFG_MACHINE_RESET(punchout)
	MCFG_NVRAM_ADD_0FILL("nvram")

	/* video hardware */
	MCFG_GFXDECODE(punchout)
	MCFG_PALETTE_LENGTH(0x200)
	MCFG_DEFAULT_LAYOUT(layout_dualhovu)

	MCFG_SCREEN_ADD("top", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(32*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MCFG_SCREEN_UPDATE_STATIC(punchout_top)

	MCFG_SCREEN_ADD("bottom", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MCFG_SCREEN_SIZE(32*8, 32*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MCFG_SCREEN_UPDATE_STATIC(punchout_bottom)

	MCFG_VIDEO_START(punchout)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")

	MCFG_SOUND_ADD("nes", NES, N2A03_DEFAULTCLOCK)
	MCFG_SOUND_CONFIG(nes_config)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	MCFG_SOUND_ADD("vlm", VLM5030, 3580000)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( armwrest, punchout )

	/* basic machine hardware */

	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(armwrest_map)

	/* video hardware */
	MCFG_GFXDECODE(armwrest)

	MCFG_VIDEO_START(armwrest)
	MCFG_SCREEN_MODIFY("top")
	MCFG_SCREEN_UPDATE_STATIC(armwrest_top)
	MCFG_SCREEN_MODIFY("bottom")
	MCFG_SCREEN_UPDATE_STATIC(armwrest_bottom)
MACHINE_CONFIG_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( punchout )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "chp1-c.8l",    0x0000, 0x2000, CRC(a4003adc) SHA1(a8026eb39aa883993a0c9cb4400bf1a7e5898a2b) )
	ROM_LOAD( "chp1-c.8k",    0x2000, 0x2000, CRC(745ecf40) SHA1(430f80b688a515953fab177a3ec2eb31c886df22) )
	ROM_LOAD( "chp1-c.8j",    0x4000, 0x2000, CRC(7a7f870e) SHA1(76bb9f3ef0a2fd514db63fb77f35bde12c15c29c) )
	ROM_LOAD( "chp1-c.8h",    0x6000, 0x2000, CRC(5d8123d7) SHA1(04ddfcde969db93ff31e9c8a2af4dde285b82e2e) )
	ROM_LOAD( "chp1-c.8f",    0x8000, 0x4000, CRC(c8a55ddb) SHA1(f91fb368542c50969a086f01a2e70ecce7f2697b) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 64k for the sound CPU */
	ROM_LOAD( "chp1-c.4k",    0xe000, 0x2000, CRC(cb6ef376) SHA1(503dbcc1b18a497311bf129689d5650860bf96c7) )

	ROM_REGION( 0x04000, "gfx1", ROMREGION_ERASEFF | ROMREGION_INVERT )
	ROM_LOAD( "chp1-b.4c",    0x00000, 0x2000, CRC(e26dc8b3) SHA1(a704d39ef6f5cbad64a478e5c109b18aae427cbc) )	/* chars #1 */
	ROM_LOAD( "chp1-b.4d",    0x02000, 0x2000, CRC(dd1310ca) SHA1(918d2eda000244b692f1da7ac57d7a0edaef95fb) )

	ROM_REGION( 0x04000, "gfx2", ROMREGION_ERASEFF | ROMREGION_INVERT )
	ROM_LOAD( "chp1-b.4a",    0x00000, 0x2000, CRC(20fb4829) SHA1(9f0ce9379eb31c19bfacdc514ac6a28aa4217cbb) )	/* chars #2 */
	ROM_LOAD( "chp1-b.4b",    0x02000, 0x2000, CRC(edc34594) SHA1(fbb4a8b979d60b183dc23bdbb7425100b9325287) )

	ROM_REGION( 0x30000, "gfx3", ROMREGION_ERASEFF )
	ROM_LOAD( "chp1-v.2r",    0x00000, 0x4000, CRC(bd1d4b2e) SHA1(492ae301a9890c2603d564c9048b1b67895052dd) )	/* chars #3 */
	ROM_LOAD( "chp1-v.2t",    0x04000, 0x4000, CRC(dd9a688a) SHA1(fbb98eebfbaab445928da939846a2d07a8046afb) )
	ROM_LOAD( "chp1-v.2u",    0x08000, 0x2000, CRC(da6a3c4b) SHA1(e03469fb6f552f41a9b7f4b3e51c15a52b61cf84) )
	/* 0a000-0bfff empty (space for 16k ROM) */
	ROM_LOAD( "chp1-v.2v",    0x0c000, 0x2000, CRC(8c734a67) SHA1(d59b5a2517e4890e7ca7da52ca2813a6abc484a3) )
	/* 0e000-0ffff empty (space for 16k ROM) */
	ROM_LOAD( "chp1-v.3r",    0x10000, 0x4000, CRC(2e74ad1d) SHA1(538b3f9273699106a50887c927f0251537bf0f42) )
	ROM_LOAD( "chp1-v.3t",    0x14000, 0x4000, CRC(630ba9fb) SHA1(36cec8658597239385cada3bc947b940ab66954b) )
	ROM_LOAD( "chp1-v.3u",    0x18000, 0x2000, CRC(6440321d) SHA1(c8c084ad408cb6bf65959ed4db03c4b4cf9b1c1a) )
	/* 1a000-1bfff empty (space for 16k ROM) */
	ROM_LOAD( "chp1-v.3v",    0x1c000, 0x2000, CRC(bb7b7198) SHA1(64572668d30e008daf4ccaa5689518ecc41f1091) )
	/* 1e000-1ffff empty (space for 16k ROM) */
	ROM_LOAD( "chp1-v.4r",    0x20000, 0x4000, CRC(4e5b0fe9) SHA1(c5c4fb735cc232b43c49442e62af0ebe99eaab0c) )
	ROM_LOAD( "chp1-v.4t",    0x24000, 0x4000, CRC(37ffc940) SHA1(d555807a6a1025c81637c5db0184b48306aa01ac) )
	ROM_LOAD( "chp1-v.4u",    0x28000, 0x2000, CRC(1a7521d4) SHA1(4e8a8298f2ff8257d2058e5133ad295f92c7deb8) )
	/* 2a000-2bfff empty (space for 16k ROM) */
	/* 2c000-2ffff empty (4v doesn't exist, it is seen as a 0xff fill) */

	ROM_REGION( 0x10000, "gfx4", ROMREGION_ERASEFF | ROMREGION_INVERT )
	ROM_LOAD( "chp1-v.6p",    0x00000, 0x2000, CRC(16588f7a) SHA1(1aeaaa5cc2477c3aa4bf80df7d9474cc9ded9f15) )	/* chars #4 */
	ROM_LOAD( "chp1-v.6n",    0x02000, 0x2000, CRC(dc743674) SHA1(660582c76ee68a7267d5686a2f8ea0fd6c2b25fc) )
	/* 04000-07fff empty (space for 6l and 6k) */
	ROM_LOAD( "chp1-v.8p",    0x08000, 0x2000, CRC(c2db5b4e) SHA1(39d009af597fa28d34af31aec111aa6fe09fea39) )
	ROM_LOAD( "chp1-v.8n",    0x0a000, 0x2000, CRC(e6af390e) SHA1(73984cbdc8fbf667126ada63ab9500609eb25c61) )
	/* 0c000-0ffff empty (space for 8l and 8k) */

	ROM_REGION( 0x0d00, "proms", 0 )
	ROM_LOAD( "chp1-b.6e",    0x0000, 0x0200, CRC(e9ca3ac6) SHA1(68d9739d8a0dadc6fe3b3767437526096ca5db98) )	/* R (top monitor) */
	ROM_LOAD( "chp1-b.6f",    0x0200, 0x0200, CRC(02be56ab) SHA1(a88f332cb26928350ed20ab5f4c04d5324bb516a) )	/* G */
	ROM_LOAD( "chp1-b.7f",    0x0400, 0x0200, CRC(11de55f1) SHA1(269b82f4bc73fac197e0bb6d9a90a220e77ce478) )	/* B */
	ROM_LOAD( "chp1-b.7e",    0x0600, 0x0200, CRC(47adf7a2) SHA1(1d37d5207cd37a9c122251c60cc8f43dd680f484) )	/* R (bottom monitor) */
	ROM_LOAD( "chp1-b.8e",    0x0800, 0x0200, CRC(b0fc15a8) SHA1(a1af09cfea81231240bd94f3b98de1be8235ebe7) )	/* G */
	ROM_LOAD( "chp1-b.8f",    0x0a00, 0x0200, CRC(1ffd894a) SHA1(9e8c1c28b4c12acf42f814bc109d353729a25652) )	/* B */
	ROM_LOAD( "chp1-v.2d",    0x0c00, 0x0100, CRC(71dc0d48) SHA1(dd6609f547d74887f520d7e71a1a00317ff181d0) )	/* timing - not used */

	ROM_REGION( 0x4000, "vlm", 0 )	/* 16k for the VLM5030 data */
	ROM_LOAD( "chp1-c.6p",    0x0000, 0x4000, CRC(ea0bbb31) SHA1(b1da024cb688341d39791a78d1144fe09acb00cf) )
ROM_END

/* Italian bootleg set from an original board found in Italy,
   uses new program roms, 2 new gfx roms, and a mix of PunchOut and Super PunchOut graphic roms
   Service mode is diaabled
*/

ROM_START( punchita )
	/* Unique to this set */
	ROM_REGION( 0x10000, "maincpu", 0 )	/* 64k for code */
	ROM_LOAD( "chp1-c(__ita).8l",    0x0000, 0x2000, CRC(1d595ce2) SHA1(affd43bef96c68f953e66cfa14ad4e9c304dc022) )
	ROM_LOAD( "chp1-c(__ita).8k",    0x2000, 0x2000, CRC(c062fa5c) SHA1(8ebd6fd76f1fd1b85216a4e21d8a13be8317b9e2) )
	ROM_LOAD( "chp1-c(__ita).8j",    0x4000, 0x2000, CRC(48d453ef) SHA1(145f3ace8bec87e83b64c6472e2b71f1ebea13ea) )
	ROM_LOAD( "chp1-c(__ita).8h",    0x6000, 0x2000, CRC(67f5aedc) SHA1(c63a8b0696eec87bb147d435c18ee7e26d19e2a4) )
	ROM_LOAD( "chp1-c(__ita).8f",    0x8000, 0x4000, CRC(761de4f3) SHA1(66754bc762c14fea620fabf408f85e6e3acb89ad) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 64k for the sound CPU */
	ROM_LOAD( "chp1-c.4k",    0xe000, 0x2000, CRC(cb6ef376) SHA1(503dbcc1b18a497311bf129689d5650860bf96c7) )

	/* Unique to this set */
	ROM_REGION( 0x04000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD( "chp1-b(__ita).4c",    0x00000, 0x0800, CRC(9a9ff1d3) SHA1(d91adf69acb717f238cd5954909701a8748f2185) )	/* chars #1 */
	ROM_CONTINUE(             0x01000, 0x0800 )
	ROM_CONTINUE(             0x00800, 0x0800 )
	ROM_CONTINUE(             0x01800, 0x0800 )
	ROM_LOAD( "chp1-b(__ita).4d",    0x02000, 0x0800, CRC(4c23350f) SHA1(70a76002db9209699cdf1f092b2b5ef32d0b7b75) )
	ROM_CONTINUE(             0x03000, 0x0800 )
	ROM_CONTINUE(             0x02800, 0x0800 )
	ROM_CONTINUE(             0x03800, 0x0800 )

	/* These match SUPER PunchOut */
	ROM_REGION( 0x04000, "gfx2", ROMREGION_ERASEFF )
	ROM_LOAD( "chp1-b(__ita).4a",    0x00000, 0x0800, CRC(c075f831) SHA1(f22d9e415637599420c443ce08e7e70d1eb1c6f5) )	/* chars #2 */
	ROM_CONTINUE(             0x01000, 0x0800 )
	ROM_CONTINUE(             0x00800, 0x0800 )
	ROM_CONTINUE(             0x01800, 0x0800 )
	ROM_LOAD( "chp1-b(__ita).4b",    0x02000, 0x0800, CRC(c4cc2b5a) SHA1(7b9d4dcecc67271980c3c44561fc25a6f6c93ee3) )
	ROM_CONTINUE(             0x03000, 0x0800 )
	ROM_CONTINUE(             0x02800, 0x0800 )
	ROM_CONTINUE(             0x03800, 0x0800 )

	ROM_REGION( 0x30000, "gfx3", ROMREGION_ERASEFF )
	ROM_LOAD( "chp1-v.2r",    0x00000, 0x4000, CRC(bd1d4b2e) SHA1(492ae301a9890c2603d564c9048b1b67895052dd) )	/* chars #3 */
	ROM_LOAD( "chp1-v.2t",    0x04000, 0x4000, CRC(dd9a688a) SHA1(fbb98eebfbaab445928da939846a2d07a8046afb) )
	ROM_LOAD( "chp1-v.2u",    0x08000, 0x2000, CRC(da6a3c4b) SHA1(e03469fb6f552f41a9b7f4b3e51c15a52b61cf84) )
	/* 0a000-0bfff empty (space for 16k ROM) */
	ROM_LOAD( "chp1-v.2v",    0x0c000, 0x2000, CRC(8c734a67) SHA1(d59b5a2517e4890e7ca7da52ca2813a6abc484a3) )
	/* 0e000-0ffff empty (space for 16k ROM) */
	ROM_LOAD( "chp1-v.3r",    0x10000, 0x4000, CRC(2e74ad1d) SHA1(538b3f9273699106a50887c927f0251537bf0f42) )
	ROM_LOAD( "chp1-v.3t",    0x14000, 0x4000, CRC(630ba9fb) SHA1(36cec8658597239385cada3bc947b940ab66954b) )
	ROM_LOAD( "chp1-v.3u",    0x18000, 0x2000, CRC(6440321d) SHA1(c8c084ad408cb6bf65959ed4db03c4b4cf9b1c1a) )
	/* 1a000-1bfff empty (space for 16k ROM) */
	ROM_LOAD( "chp1-v.3v",    0x1c000, 0x2000, CRC(bb7b7198) SHA1(64572668d30e008daf4ccaa5689518ecc41f1091) )
	/* 1e000-1ffff empty (space for 16k ROM) */
	ROM_LOAD( "chp1-v.4r",    0x20000, 0x4000, CRC(4e5b0fe9) SHA1(c5c4fb735cc232b43c49442e62af0ebe99eaab0c) )
	ROM_LOAD( "chp1-v.4t",    0x24000, 0x4000, CRC(37ffc940) SHA1(d555807a6a1025c81637c5db0184b48306aa01ac) )
	ROM_LOAD( "chp1-v.4u",    0x28000, 0x2000, CRC(1a7521d4) SHA1(4e8a8298f2ff8257d2058e5133ad295f92c7deb8) )
	/* 2a000-2bfff empty (space for 16k ROM) */
	/* 2c000-2ffff empty (4v doesn't exist, it is seen as a 0xff fill) */

	/* These match SUPER PunchOut */
	ROM_REGION( 0x10000, "gfx4", ROMREGION_ERASEFF )
	ROM_LOAD( "chp1-v(__ita).6p",    0x00000, 0x0800, CRC(75be7aae) SHA1(396bc1d301b99e064de4dad699882618b1b9c958) )	/* chars #4 */
	ROM_CONTINUE(             0x01000, 0x0800 )
	ROM_CONTINUE(             0x00800, 0x0800 )
	ROM_CONTINUE(             0x01800, 0x0800 )
	ROM_LOAD( "chp1-v(__ita).6n",    0x02000, 0x0800, CRC(daf74de0) SHA1(9373d4527b675b3128a5a830f42e1dc5dcb85307) )
	ROM_CONTINUE(             0x03000, 0x0800 )
	ROM_CONTINUE(             0x02800, 0x0800 )
	ROM_CONTINUE(             0x03800, 0x0800 )
	/* 04000-07fff empty (space for 6l and 6k) */
	ROM_LOAD( "chp1-v(__ita).8p",    0x08000, 0x0800, CRC(4cb7ea82) SHA1(213b7c1431f4c92e5519a8771035bda28b3bab8a) )
	ROM_CONTINUE(             0x09000, 0x0800 )
	ROM_CONTINUE(             0x08800, 0x0800 )
	ROM_CONTINUE(             0x09800, 0x0800 )
	ROM_LOAD( "chp1-v(__ita).8n",    0x0a000, 0x0800, CRC(1c0d09aa) SHA1(3276bae7400453f3612f53d7b47fb199cbe53e6d) )
	ROM_CONTINUE(             0x0b000, 0x0800 )
	ROM_CONTINUE(             0x0a800, 0x0800 )
	ROM_CONTINUE(             0x0b800, 0x0800 )
	/* 0c000-0ffff empty (space for 8l and 8k) */

	ROM_REGION( 0x0d00, "proms", 0 )
	ROM_LOAD( "chp1-b.6e",    0x0000, 0x0200, CRC(e9ca3ac6) SHA1(68d9739d8a0dadc6fe3b3767437526096ca5db98) )	/* R (top monitor) */
	ROM_LOAD( "chp1-b.6f",    0x0200, 0x0200, CRC(02be56ab) SHA1(a88f332cb26928350ed20ab5f4c04d5324bb516a) )	/* G */
	ROM_LOAD( "chp1-b.7f",    0x0400, 0x0200, CRC(11de55f1) SHA1(269b82f4bc73fac197e0bb6d9a90a220e77ce478) )	/* B */
	ROM_LOAD( "chp1-b.7e",    0x0600, 0x0200, CRC(47adf7a2) SHA1(1d37d5207cd37a9c122251c60cc8f43dd680f484) )	/* R (bottom monitor) */
	ROM_LOAD( "chp1-b.8e",    0x0800, 0x0200, CRC(b0fc15a8) SHA1(a1af09cfea81231240bd94f3b98de1be8235ebe7) )	/* G */
	ROM_LOAD( "chp1-b.8f",    0x0a00, 0x0200, CRC(1ffd894a) SHA1(9e8c1c28b4c12acf42f814bc109d353729a25652) )	/* B */
	ROM_LOAD( "chp1-v.2d",    0x0c00, 0x0100, CRC(71dc0d48) SHA1(dd6609f547d74887f520d7e71a1a00317ff181d0) )	/* timing - not used */

	ROM_REGION( 0x4000, "vlm", 0 )	/* 16k for the VLM5030 data */
	ROM_LOAD( "chp1-c.6p",    0x0000, 0x4000, CRC(ea0bbb31) SHA1(b1da024cb688341d39791a78d1144fe09acb00cf) )
ROM_END

ROM_START( spnchout )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "chs1-c.8l",    0x0000, 0x2000, CRC(703b9780) SHA1(93b2fd8392ef094413330cd2474ac406c3db426e) )
	ROM_LOAD( "chs1-c.8k",    0x2000, 0x2000, CRC(e13719f6) SHA1(d0f08a0999801dd5d55f2f4ae3e76f25b765b8d6) )
	ROM_LOAD( "chs1-c.8j",    0x4000, 0x2000, CRC(1fa629e8) SHA1(e0c37883e65c77e9f25e323fb4dc05f7dcdc6347) )
	ROM_LOAD( "chs1-c.8h",    0x6000, 0x2000, CRC(15a6c068) SHA1(3f42697a6d79c6fd4b638feb366c80e98a7f02e2) )
	ROM_LOAD( "chs1-c.8f",    0x8000, 0x4000, CRC(4ff3cdd9) SHA1(282edf9a3fa085bc82523249a519f2a3fe04e87e) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 64k for the sound CPU */
	ROM_LOAD( "chp1-c.4k",    0xe000, 0x2000, CRC(cb6ef376) SHA1(503dbcc1b18a497311bf129689d5650860bf96c7) )

	ROM_REGION( 0x04000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD( "chs1-b.4c",    0x00000, 0x0800, CRC(9f2ede2d) SHA1(58a0f8c34ff9ec425c846c1eb6c6ccd99c2d0132) )	/* chars #1 */
	ROM_CONTINUE(             0x01000, 0x0800 )
	ROM_CONTINUE(             0x00800, 0x0800 )
	ROM_CONTINUE(             0x01800, 0x0800 )
	ROM_LOAD( "chs1-b.4d",    0x02000, 0x0800, CRC(143ae5c6) SHA1(4c8426ba336941ac3341b1dd65c0d68b9aae56de) )
	ROM_CONTINUE(             0x03000, 0x0800 )
	ROM_CONTINUE(             0x02800, 0x0800 )
	ROM_CONTINUE(             0x03800, 0x0800 )

	ROM_REGION( 0x04000, "gfx2", ROMREGION_ERASEFF )
	ROM_LOAD( "chp1-b.4a",    0x00000, 0x0800, CRC(c075f831) SHA1(f22d9e415637599420c443ce08e7e70d1eb1c6f5) )	/* chars #2 */
	ROM_CONTINUE(             0x01000, 0x0800 )
	ROM_CONTINUE(             0x00800, 0x0800 )
	ROM_CONTINUE(             0x01800, 0x0800 )
	ROM_LOAD( "chp1-b.4b",    0x02000, 0x0800, CRC(c4cc2b5a) SHA1(7b9d4dcecc67271980c3c44561fc25a6f6c93ee3) )
	ROM_CONTINUE(             0x03000, 0x0800 )
	ROM_CONTINUE(             0x02800, 0x0800 )
	ROM_CONTINUE(             0x03800, 0x0800 )

	ROM_REGION( 0x30000, "gfx3", ROMREGION_ERASEFF )
	ROM_LOAD( "chs1-v.2r",    0x00000, 0x4000, CRC(ff33405d) SHA1(31b892d184d24a0ec05fd6facec61a532ce8535b) )	/* chars #3 */
	ROM_LOAD( "chs1-v.2t",    0x04000, 0x4000, CRC(f507818b) SHA1(fb99c5c88e829d7e81c53ead21554a614b6fdcf9) )
	ROM_LOAD( "chs1-v.2u",    0x08000, 0x4000, CRC(0995fc95) SHA1(d056fc61ad2409525622b4db69796668c3145460) )
	ROM_LOAD( "chs1-v.2v",    0x0c000, 0x2000, CRC(f44d9878) SHA1(327a8bbc8f1a33fcf95ebc75db97406feb6435d9) )
	/* 0e000-0ffff empty (space for 16k ROM) */
	ROM_LOAD( "chs1-v.3r",    0x10000, 0x4000, CRC(09570945) SHA1(c3e2a8f76eebacc9042d087db2dfdc8ea267d46a) )
	ROM_LOAD( "chs1-v.3t",    0x14000, 0x4000, CRC(42c6861c) SHA1(2b160cde3cc3ee7adb276fe719f7919c9295ba38) )
	ROM_LOAD( "chs1-v.3u",    0x18000, 0x4000, CRC(bf5d02dd) SHA1(f1f4932fc258c087783450e7c964902fa45c4568) )
	ROM_LOAD( "chs1-v.3v",    0x1c000, 0x2000, CRC(5673f4fc) SHA1(682a81b60494b2c77d1da312c97bc807021eac67) )
	/* 1e000-1ffff empty (space for 16k ROM) */
	ROM_LOAD( "chs1-v.4r",    0x20000, 0x4000, CRC(8e155758) SHA1(d21ce2d81b2d47e5ff091e48cf46d41d01ea6314) )
	ROM_LOAD( "chs1-v.4t",    0x24000, 0x4000, CRC(b4e43448) SHA1(1ed6bf913c15851cf86554713c122b55c18c5d67) )
	ROM_LOAD( "chs1-v.4u",    0x28000, 0x4000, CRC(74e0d956) SHA1(b172cdcc5d26f3be06a7f0f9e19879957e87f992) )
	/* 2c000-2ffff empty (4v doesn't exist, it is seen as a 0xff fill) */

	ROM_REGION( 0x10000, "gfx4", ROMREGION_ERASEFF )
	ROM_LOAD( "chp1-v.6p",    0x00000, 0x0800, CRC(75be7aae) SHA1(396bc1d301b99e064de4dad699882618b1b9c958) )	/* chars #4 */
	ROM_CONTINUE(             0x01000, 0x0800 )
	ROM_CONTINUE(             0x00800, 0x0800 )
	ROM_CONTINUE(             0x01800, 0x0800 )
	ROM_LOAD( "chp1-v.6n",    0x02000, 0x0800, CRC(daf74de0) SHA1(9373d4527b675b3128a5a830f42e1dc5dcb85307) )
	ROM_CONTINUE(             0x03000, 0x0800 )
	ROM_CONTINUE(             0x02800, 0x0800 )
	ROM_CONTINUE(             0x03800, 0x0800 )
	/* 04000-07fff empty (space for 6l and 6k) */
	ROM_LOAD( "chp1-v.8p",    0x08000, 0x0800, CRC(4cb7ea82) SHA1(213b7c1431f4c92e5519a8771035bda28b3bab8a) )
	ROM_CONTINUE(             0x09000, 0x0800 )
	ROM_CONTINUE(             0x08800, 0x0800 )
	ROM_CONTINUE(             0x09800, 0x0800 )
	ROM_LOAD( "chp1-v.8n",    0x0a000, 0x0800, CRC(1c0d09aa) SHA1(3276bae7400453f3612f53d7b47fb199cbe53e6d) )
	ROM_CONTINUE(             0x0b000, 0x0800 )
	ROM_CONTINUE(             0x0a800, 0x0800 )
	ROM_CONTINUE(             0x0b800, 0x0800 )
	/* 0c000-0ffff empty (space for 8l and 8k) */

	ROM_REGION( 0x0d00, "proms", 0 )
	ROM_LOAD( "chs1-b.6e",    0x0000, 0x0200, CRC(0ad4d727) SHA1(5fa4247d58d10b4644f0a7492efb22b7a9ce7b62) )	/* R (top monitor) */
	ROM_LOAD( "chs1-b.6f",    0x0200, 0x0200, CRC(86f5cfdb) SHA1(a2a3a4e9ca15826fe8c86650d50c8ce203d57eae) )	/* G */
	ROM_LOAD( "chs1-b.7f",    0x0400, 0x0200, CRC(8bd406f8) SHA1(eaf0b62eccf1f47452bf983b3ffc6cacc25d4585) )	/* B */
	ROM_LOAD( "chs1-b.7e",    0x0600, 0x0200, CRC(9e170f64) SHA1(9548bfec2f5b7d222e91562b5459aef8c107b3ec) )	/* R (bottom monitor) */
	ROM_LOAD( "chs1-b.8e",    0x0800, 0x0200, CRC(3a2e333b) SHA1(5cf0324cc07ac4af63598c5c6acc61d24215b233) )	/* G */
	ROM_LOAD( "chs1-b.8f",    0x0a00, 0x0200, CRC(1663eed7) SHA1(90ff876a6b885f8a80c17531cde8b91864f1a6a5) )	/* B */
	ROM_LOAD( "chs1-v.2d",    0x0c00, 0x0100, CRC(71dc0d48) SHA1(dd6609f547d74887f520d7e71a1a00317ff181d0) )	/* timing - not used */

	ROM_REGION( 0x10000, "vlm", 0 )	/* 64k for the VLM5030 data */
	ROM_LOAD( "chs1-c.6p",    0x0000, 0x4000, CRC(ad8b64b8) SHA1(0f1232a10faf71b782f9f6653cca8570243c17e0) )
ROM_END

ROM_START( spnchoutj )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "chs1c8la.bin", 0x0000, 0x2000, CRC(dc2a592b) SHA1(a8a7fc5c836e2723ba6abcb1137f4c4f79e21c87) )
	ROM_LOAD( "chs1c8ka.bin", 0x2000, 0x2000, CRC(ce687182) SHA1(f07d930d90eda199b089f9023b51fd4456c87bdf) )
	ROM_LOAD( "chs1-c.8j",    0x4000, 0x2000, CRC(1fa629e8) SHA1(e0c37883e65c77e9f25e323fb4dc05f7dcdc6347) )
	ROM_LOAD( "chs1-c.8h",    0x6000, 0x2000, CRC(15a6c068) SHA1(3f42697a6d79c6fd4b638feb366c80e98a7f02e2) )
	ROM_LOAD( "chs1c8fa.bin", 0x8000, 0x4000, CRC(f745b5d5) SHA1(8130b5be011848625ebe6691fbb76dc338979b60) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 64k for the sound CPU */
	ROM_LOAD( "chp1-c.4k",    0xe000, 0x2000, CRC(cb6ef376) SHA1(503dbcc1b18a497311bf129689d5650860bf96c7) )

	ROM_REGION( 0x04000, "gfx1", ROMREGION_ERASEFF | ROMREGION_INVERT )
	ROM_LOAD( "b_4c_01a.bin", 0x00000, 0x2000, CRC(b017e1e9) SHA1(39e98f48bff762a674a2506efa39b3619337a1e0) )	/* chars #1 */
	ROM_LOAD( "b_4d_01a.bin", 0x02000, 0x2000, CRC(e3de9d18) SHA1(f55b6f522e127e6239197dd7eb1564e6f275df74) )

	ROM_REGION( 0x04000, "gfx2", ROMREGION_ERASEFF )
	ROM_LOAD( "chp1-b.4a",    0x00000, 0x0800, CRC(c075f831) SHA1(f22d9e415637599420c443ce08e7e70d1eb1c6f5) )	/* chars #2 */
	ROM_CONTINUE(             0x01000, 0x0800 )
	ROM_CONTINUE(             0x00800, 0x0800 )
	ROM_CONTINUE(             0x01800, 0x0800 )
	ROM_LOAD( "chp1-b.4b",    0x02000, 0x0800, CRC(c4cc2b5a) SHA1(7b9d4dcecc67271980c3c44561fc25a6f6c93ee3) )
	ROM_CONTINUE(             0x03000, 0x0800 )
	ROM_CONTINUE(             0x02800, 0x0800 )
	ROM_CONTINUE(             0x03800, 0x0800 )

	ROM_REGION( 0x30000, "gfx3", ROMREGION_ERASEFF )
	ROM_LOAD( "chs1-v.2r",    0x00000, 0x4000, CRC(ff33405d) SHA1(31b892d184d24a0ec05fd6facec61a532ce8535b) )	/* chars #3 */
	ROM_LOAD( "chs1-v.2t",    0x04000, 0x4000, CRC(f507818b) SHA1(fb99c5c88e829d7e81c53ead21554a614b6fdcf9) )
	ROM_LOAD( "chs1-v.2u",    0x08000, 0x4000, CRC(0995fc95) SHA1(d056fc61ad2409525622b4db69796668c3145460) )
	ROM_LOAD( "chs1-v.2v",    0x0c000, 0x2000, CRC(f44d9878) SHA1(327a8bbc8f1a33fcf95ebc75db97406feb6435d9) )
	/* 0e000-0ffff empty (space for 16k ROM) */
	ROM_LOAD( "chs1-v.3r",    0x10000, 0x4000, CRC(09570945) SHA1(c3e2a8f76eebacc9042d087db2dfdc8ea267d46a) )
	ROM_LOAD( "chs1-v.3t",    0x14000, 0x4000, CRC(42c6861c) SHA1(2b160cde3cc3ee7adb276fe719f7919c9295ba38) )
	ROM_LOAD( "chs1-v.3u",    0x18000, 0x4000, CRC(bf5d02dd) SHA1(f1f4932fc258c087783450e7c964902fa45c4568) )
	ROM_LOAD( "chs1-v.3v",    0x1c000, 0x2000, CRC(5673f4fc) SHA1(682a81b60494b2c77d1da312c97bc807021eac67) )
	/* 1e000-1ffff empty (space for 16k ROM) */
	ROM_LOAD( "chs1-v.4r",    0x20000, 0x4000, CRC(8e155758) SHA1(d21ce2d81b2d47e5ff091e48cf46d41d01ea6314) )
	ROM_LOAD( "chs1-v.4t",    0x24000, 0x4000, CRC(b4e43448) SHA1(1ed6bf913c15851cf86554713c122b55c18c5d67) )
	ROM_LOAD( "chs1-v.4u",    0x28000, 0x4000, CRC(74e0d956) SHA1(b172cdcc5d26f3be06a7f0f9e19879957e87f992) )
	/* 2c000-2ffff empty (4v doesn't exist, it is seen as a 0xff fill) */

	ROM_REGION( 0x10000, "gfx4", ROMREGION_ERASEFF )
	ROM_LOAD( "chp1-v.6p",    0x00000, 0x0800, CRC(75be7aae) SHA1(396bc1d301b99e064de4dad699882618b1b9c958) )	/* chars #4 */
	ROM_CONTINUE(             0x01000, 0x0800 )
	ROM_CONTINUE(             0x00800, 0x0800 )
	ROM_CONTINUE(             0x01800, 0x0800 )
	ROM_LOAD( "chp1-v.6n",    0x02000, 0x0800, CRC(daf74de0) SHA1(9373d4527b675b3128a5a830f42e1dc5dcb85307) )
	ROM_CONTINUE(             0x03000, 0x0800 )
	ROM_CONTINUE(             0x02800, 0x0800 )
	ROM_CONTINUE(             0x03800, 0x0800 )
	/* 04000-07fff empty (space for 6l and 6k) */
	ROM_LOAD( "chp1-v.8p",    0x08000, 0x0800, CRC(4cb7ea82) SHA1(213b7c1431f4c92e5519a8771035bda28b3bab8a) )
	ROM_CONTINUE(             0x09000, 0x0800 )
	ROM_CONTINUE(             0x08800, 0x0800 )
	ROM_CONTINUE(             0x09800, 0x0800 )
	ROM_LOAD( "chp1-v.8n",    0x0a000, 0x0800, CRC(1c0d09aa) SHA1(3276bae7400453f3612f53d7b47fb199cbe53e6d) )
	ROM_CONTINUE(             0x0b000, 0x0800 )
	ROM_CONTINUE(             0x0a800, 0x0800 )
	ROM_CONTINUE(             0x0b800, 0x0800 )
	/* 0c000-0ffff empty (space for 8l and 8k) */

	ROM_REGION( 0x0d00, "proms", 0 )
	ROM_LOAD( "chs1b_6e.bpr", 0x0000, 0x0200, CRC(8efd867f) SHA1(d5f2bfe750bb5d472922bdb7e915ee28a3eec9bd) )	/* R (top monitor) */
	ROM_LOAD( "chs1b_6f.bpr", 0x0200, 0x0200, CRC(279d6cbc) SHA1(aea56970801908b4d51be0c15043c7b315d2637f) )	/* G */
	ROM_LOAD( "chs1b_7f.bpr", 0x0400, 0x0200, CRC(cad6b7ad) SHA1(62b61d5fa47ca6e2dd15295674dff62e4e69471a) )	/* B */
	ROM_LOAD( "chs1-b.7e",    0x0600, 0x0200, CRC(9e170f64) SHA1(9548bfec2f5b7d222e91562b5459aef8c107b3ec) )	/* R (bottom monitor) */
	ROM_LOAD( "chs1-b.8e",    0x0800, 0x0200, CRC(3a2e333b) SHA1(5cf0324cc07ac4af63598c5c6acc61d24215b233) )	/* G */
	ROM_LOAD( "chs1-b.8f",    0x0a00, 0x0200, CRC(1663eed7) SHA1(90ff876a6b885f8a80c17531cde8b91864f1a6a5) )	/* B */
	ROM_LOAD( "chs1-v.2d",    0x0c00, 0x0100, CRC(71dc0d48) SHA1(dd6609f547d74887f520d7e71a1a00317ff181d0) )	/* timing - not used */

	ROM_REGION( 0x10000, "vlm", 0 )	/* 64k for the VLM5030 data */
	ROM_LOAD( "chs1c6pa.bin", 0x0000, 0x4000, CRC(d05fb730) SHA1(9f4c4c7e5113739312558eff4d3d3e42d513aa31) )
ROM_END

ROM_START( armwrest )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "chv1-c.8l",    0x0000, 0x2000, CRC(b09764c1) SHA1(2f32acd689ef70ec81fe958c7a604855ae39cf5e) )
	ROM_LOAD( "chv1-c.8k",    0x2000, 0x2000, CRC(0e147ff7) SHA1(7ea8b7b5562d9432c6cace2ee13377f91543975d) )
	ROM_LOAD( "chv1-c.8j",    0x4000, 0x2000, CRC(e7365289) SHA1(9d4ed5ce73b93c3917b1411ed902974e2a4f3d35) )
	ROM_LOAD( "chv1-c.8h",    0x6000, 0x2000, CRC(a2118eec) SHA1(93e1b19819352f88888b3caf67ed27cd50f866a9) )
	ROM_LOAD( "chpv-c.8f",    0x8000, 0x4000, CRC(664a07c4) SHA1(a8a049be5beeab3940079465fb0c80382f3860f0) )

	ROM_REGION( 0x10000, "audiocpu", 0 )	/* 64k for the sound CPU */
	ROM_LOAD( "chp1-c.4k",    0xe000, 0x2000, CRC(cb6ef376) SHA1(503dbcc1b18a497311bf129689d5650860bf96c7) )	/* same as Punch Out */

	ROM_REGION( 0x08000, "gfx1", ROMREGION_ERASEFF )
	ROM_LOAD( "chpv-b.2e",    0x00000, 0x4000, CRC(8b45f365) SHA1(15fadccc9afe26672fbbb8eaeaa7d3ee70bcb056) )	/* chars #1 */
	ROM_LOAD( "chpv-b.2d",    0x04000, 0x4000, CRC(b1a2850c) SHA1(e3aec428bb52443921fb7ceb5eb21b5f9ee9edcb) )

	ROM_REGION( 0x0c000, "gfx2", ROMREGION_ERASEFF )
	ROM_LOAD( "chpv-b.2m",    0x00000, 0x4000, CRC(19245b37) SHA1(711e263d487661afca09f731e9333a84eb8d1541) )	/* chars #2 */
	ROM_LOAD( "chpv-b.2l",    0x04000, 0x4000, CRC(46797941) SHA1(e21fcec8e19702f9765205a4dc89105b4e98dcdd) )
	ROM_LOAD( "chpv-b.2k",    0x0a000, 0x2000, CRC(de189b00) SHA1(62b38d5f95bb4f0a0d04947c7c2031e07f95cbe4) )

	ROM_REGION( 0x30000, "gfx3", ROMREGION_ERASEFF )
	ROM_LOAD( "chv1-v.2r",    0x00000, 0x4000, CRC(d86056d9) SHA1(decedf6b54e5990ff14d8049791b2d06c33ae71b) )	/* chars #3 */
	ROM_LOAD( "chv1-v.2t",    0x04000, 0x4000, CRC(5ad77059) SHA1(05a1c7957982fa695bca62a05dc593c7913ccd7f) )
	/* 08000-0bfff empty */
	ROM_LOAD( "chv1-v.2v",    0x0c000, 0x4000, CRC(a0fd7338) SHA1(afd8d78661c3b7149f4c491ba930a8ce66d29977) )
	ROM_LOAD( "chv1-v.3r",    0x10000, 0x4000, CRC(690e26fb) SHA1(6c20daabf5db633482b288c8020130a80cc939fc) )
	ROM_LOAD( "chv1-v.3t",    0x14000, 0x4000, CRC(ea5d7759) SHA1(4d72d7b602455349be4a9cbf34127952aa2a99ea) )
	/* 18000-1bfff empty */
	ROM_LOAD( "chv1-v.3v",    0x1c000, 0x4000, CRC(ceb37c05) SHA1(9d0e3d52e018901c2f26a9de7aa9858b106487d3) )
	ROM_LOAD( "chv1-v.4r",    0x20000, 0x4000, CRC(e291cba0) SHA1(a03ff7eea3a7a841000b67a8baeca6e82e8496ef) )
	ROM_LOAD( "chv1-v.4t",    0x24000, 0x4000, CRC(e01f3b59) SHA1(9f47507094e03735adaf033f3b99e17dd9dfd5d0) )
	/* 28000-2bfff empty */
	/* 2c000-2ffff empty (4v doesn't exist, it is seen as a 0xff fill) */

	ROM_REGION( 0x10000, "gfx4", ROMREGION_ERASEFF | ROMREGION_INVERT )
	ROM_LOAD( "chv1-v.6p",    0x00000, 0x2000, CRC(d834e142) SHA1(e7d654145b695147b744af2284173f90749fbf0e) )	/* chars #4 */
	/* 02000-03fff empty (space for 16k ROM) */
	/* 04000-07fff empty (space for 6l and 6k) */
	ROM_LOAD( "chv1-v.8p",    0x08000, 0x2000, CRC(a2f531db) SHA1(c9be180fbc608135c892e8ee396b138f058edf24) )
	/* 0a000-0bfff empty (space for 16k ROM) */
	/* 0c000-0ffff empty (space for 8l and 8k) */

	ROM_REGION( 0x0e00, "proms", 0 )
	ROM_LOAD( "chpv-b.7b",    0x0000, 0x0200, CRC(df6fdeb3) SHA1(7766d420cb95377104e26d96afddc83b67553c2f) )	/* R (top monitor) */
	ROM_LOAD( "chpv-b.7c",    0x0200, 0x0200, CRC(b1da5f42) SHA1(55e744da70bbaa855cb1403eef028771a97578a1) )	/* G */
	ROM_LOAD( "chpv-b.7d",    0x0400, 0x0200, CRC(4ede813e) SHA1(6603465dae7d869c483d66768fab16f282caaa8b) )	/* B */
	ROM_LOAD( "chpv-b.4b",    0x0600, 0x0200, CRC(9d51416e) SHA1(ae933786c5fc19311144b2094305b4253dc8b75b) )	/* R (bottom monitor) */
	ROM_LOAD( "chpv-b.4c",    0x0800, 0x0200, CRC(b8a25795) SHA1(8e41baa796fd8f00739a95b2e07066d68193bd76) )	/* G */
	ROM_LOAD( "chpv-b.4d",    0x0a00, 0x0200, CRC(474fc3b1) SHA1(9cda1d1626285310524d048b60b1cf89e197a26d) )	/* B */
	ROM_LOAD( "chv1-b.3c",    0x0c00, 0x0100, CRC(c3f92ea2) SHA1(1a82cca1b9a8d9bd4a1d121d8c131a7d0be554bc) )	/* priority encoder - not used */
	ROM_LOAD( "chpv-v.2d",    0x0d00, 0x0100, CRC(71dc0d48) SHA1(dd6609f547d74887f520d7e71a1a00317ff181d0) )	/* timing - not used */

	ROM_REGION( 0x10000, "vlm", 0 )	/* 64k for the VLM5030 data */
	ROM_LOAD( "chv1-c.6p",    0x0000, 0x4000, CRC(31b52896) SHA1(395f59ac38b46042f79e9224ac6bc7d3dc299906) )
ROM_END



GAME( 1984, punchout, 0,        punchout, punchout, punchout, ROT0, "Nintendo", "Punch-Out!!", 0 )
GAME( 1984, punchita, punchout, punchout, punchout, spnchout, ROT0, "bootleg",  "Punch-Out!! (Italian bootleg)", 0 )
GAME( 1984, spnchout, 0,        punchout, spnchout, spnchout, ROT0, "Nintendo", "Super Punch-Out!!", 0 )
GAME( 1984, spnchoutj,spnchout, punchout, spnchout, spnchotj, ROT0, "Nintendo", "Super Punch-Out!! (Japan)", 0 )
GAME( 1985, armwrest, 0,        armwrest, armwrest, armwrest, ROT0, "Nintendo", "Arm Wrestling", 0 )
