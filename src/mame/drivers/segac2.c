/***********************************************************************************************

    Sega System C/C2 Driver
    driver by David Haywood and Aaron Giles
    ---------------------------------------
    Last Update 15 Nov 2005


    Sega's C2 was used between 1989 and 1994, the hardware being very similar to that
    used by the Sega MegaDrive/Genesis Home Console Sega produced around the same time.

    todo: fill in protection chip data

    Year  Game                       Developer         Protection Chip  Board
    ====  ====================       ================  ===============  =====
    1989  Bloxeed                    Sega / Elorg      n/a              C
    1990  Columns                    Sega              317-0149         C
    1990  Columns II                 Sega              317-0160         C
    1990  ThunderForce AC            Sega / Technosoft 317-0172         C2
    1990  Borench                    Sega              317-0173         C2
    1991  Twin Squash                Sega              317-0193         C2
    1992  Ribbit!                    Sega              317-0178         C2
    1992  Puyo Puyo                  Sega / Compile    317-0203         C2
    1992  Tant-R (Japan)             Sega              317-0211         C2
    1992  Tant-R (Korea)             Sega              ?                C2
    1994  PotoPoto (Japan)           Sega              317-0218         C2
    1994  Stack Columns (Japan)      Sega              317-0219         C2
    1994  Stack Columns (World)      Sega              317-0223         C2
    1994  Ichidant-R (Japan)         Sega              317-0224         C2
    1994  Ichidant-R (World)         Sega              ?                C2
    1994  Ichidant-R (Korea)         Sega              ?                C2
    1994  Puyo Puyo 2                Compile           317-0228         C2
    1994  Zunzunkyou No Yabou        Sega              ?                C2

    1995  Print Club (Vol.1)         Atlus             ?                C2
    1995  Print Club (Vol.2)         Atlus             ?                C2
    1996  Print Club (Vol.4)         Atlus             ?                C2
    1996  Print Club (Vol.5)         Atlus             ?                C2


     Notes:
            Bloxeed doesn't Read from the Protection Chip at all; all of the other games do.
            Currently the protection chip is mostly understood, and needs a table of 256
            4-bit values for each game. In all cases except for Poto Poto and Puyo Puyo 2,
            the table is embedded in the code. Workarounds for the other 2 cases are
            provided.

            I'm assuming System-C was the Board without the uPD7759 chip and System-C2 was the
            version of the board with it, this could be completely wrong but it doesn't really
            matter anyway.

    Bugs:   Puyo Puyo ends up with a black screen after doing memory tests
            Battery-backed RAM needs to be figured out

    Thanks: (in no particular order) to any MameDev that helped me out .. (OG, Mish etc.)
            Charles MacDonald for his C2Emu .. without it working out what were bugs in my code
                and issues due to protection would have probably killed the driver long ago :p
            Razoola & Antiriad .. for helping teach me some 68k ASM needed to work out just why
                the games were crashing :)
            Sega for producing some Fantastic Games...
            and anyone else who knows they've contributed :)

************************************************************************************************

    Hiscores:

    Bloxeed  @ f400-????            [key = ???]
    Columns  @ fc00-ffff            [key = '(C) SEGA 1990.JAN BY.TAKOSUKEZOU' @ fc00,ffe0]
    Columns2 @ fc00-ffff            [key = '(C) SEGA 1990.SEP.COLUMNS2 JAPAN' @ fc00,fd00,fe00,ffe0]
    Borench  @ f400-f5ff            [key = 'EIJI' in last word]
    TForceAC @ 8100-817f/8180-81ff  [key = '(c)Tehcno soft90' @ 8070 and 80f0]
    TantR    @ fc00-fcff/fd00-fdff  [key = 0xd483 in last word]
    PuyoPuyo @ fc00-fdff/fe00-ffff  [key = 0x28e1 in first word]
    Ichidant @ fc00-fcff/fd00-fdff  [key = 0x85a9 in last word]
    StkClmns @ fc00-fc7f/fc80-fcff  [key = ???]
    PuyoPuy2
    PotoPoto
    ZunkYou

***********************************************************************************************/


#include "driver.h"
#include "deprecat.h"
#include "cpu/m68000/m68000.h"
#include "genesis.h"

#define XL1_CLOCK			640000
#define XL2_CLOCK			53693175


#define LOG_PROTECTION		1
#define LOG_PALETTE			0
#define LOG_IOCHIP			0




/******************************************************************************
    Global variables
******************************************************************************/

/* internal states */
static UINT8 		misc_io_data[0x10];	/* holds values written to the I/O chip */

/* protection-related tracking */
static int (*prot_func)(int in);		/* emulation of protection chip */
static UINT8 		prot_write_buf;		/* remembers what was written */
static UINT8		prot_read_buf;		/* remembers what was returned */

/* palette-related variables */
static UINT8		alt_palette_mode;
static UINT8		palbank;
static UINT8		bg_palbase;
static UINT8		sp_palbase;

/* sound-related variables */
static UINT8		sound_banks;		/* number of sound banks */
static UINT8		bloxeed_sound;		/* use kludge for bloxeed sound? */



/******************************************************************************
    Machine init
*******************************************************************************

    This is called at init time, when it's safe to create a timer. We use
    it to prime the scanline interrupt timer.

******************************************************************************/

static MACHINE_START( segac2 )
{
	state_save_register_global_array(misc_io_data);
	state_save_register_global(prot_write_buf);
	state_save_register_global(prot_read_buf);

	MACHINE_START_CALL(genesis);
}


static MACHINE_RESET( segac2 )
{
	/* set up interrupts and such */
	MACHINE_RESET_CALL(genesis);

	/* determine how many sound banks */
	sound_banks = 0;
	if (memory_region(REGION_SOUND1))
		sound_banks = memory_region_length(REGION_SOUND1) / 0x20000;

	/* reset the protection */
	prot_write_buf = 0;
	prot_read_buf = 0;
	alt_palette_mode = 0;

	palbank = 0;
	bg_palbase = 0;
	sp_palbase = 0;
}




/******************************************************************************
    Sound handlers
*******************************************************************************

    These handlers are responsible for communicating with the (genenerally)
    8-bit sound chips. All accesses are via the low byte.

    The Sega C/C2 system uses a YM3438 (compatible with the YM2612) for FM-
    based music generation, and an SN76489 for PSG and noise effects. The
    C2 board also appears to have a UPD7759 for sample playback.

******************************************************************************/

/* handle reads from the YM3438 */
static READ16_HANDLER( ym3438_r )
{
	switch (offset)
	{
		case 0: return YM3438_status_port_0_A_r(0);
		case 1: return YM3438_read_port_0_r(0);
		case 2: return YM3438_status_port_0_B_r(0);
	}
	return 0xff;
}


/* handle writes to the YM3438 */
static WRITE16_HANDLER( ym3438_w )
{
	/* only works if we're accessing the low byte */
	if (ACCESSING_LSB)
	{
		static UINT8 last_port;

		/* kludge for Bloxeed - it seems to accidentally trip timer 2  */
		/* and has no recourse for clearing the interrupt; until we    */
		/* find more documentation on the 2612/3438, it's unknown what */
		/* to do here */
		if (bloxeed_sound && last_port == 0x27 && (offset & 1))
			data &= ~0x08;

		switch (offset)
		{
			case 0: YM3438_control_port_0_A_w(0, data & 0xff);	last_port = data;	break;
			case 1: YM3438_data_port_0_A_w(0, data & 0xff);							break;
			case 2: YM3438_control_port_0_B_w(0, data & 0xff);	last_port = data;	break;
			case 3: YM3438_data_port_0_B_w(0, data & 0xff);							break;
		}
	}
}


/* handle writes to the UPD7759 */
static WRITE16_HANDLER( segac2_upd7759_w )
{
	/* make sure we have a UPD chip */
	if (!sound_banks)
		return;

	/* only works if we're accessing the low byte */
	if (ACCESSING_LSB)
	{
		upd7759_port_w(0, data & 0xff);
		upd7759_start_w(0, 0);
		upd7759_start_w(0, 1);
	}
}



/******************************************************************************
    Palette RAM Read / Write Handlers
*******************************************************************************

    The following Read / Write Handlers are used when accessing Palette RAM.
    The C2 Hardware appears to use 4 Banks of Colours 1 of which can be Mapped
    to 0x8C0000 - 0x8C03FF at any given time by writes to 0x84000E (This same
    address also looks to be used for things like Sample Banking)

    Each Colour uses 15-bits (from a 16-bit word) in the Format
        xBGRBBBB GGGGRRRR  (x = unused, B = Blue, G = Green, R = Red)

    As this works out the Palette RAM Stores 2048 from a Possible 4096 Colours
    at any given time.

******************************************************************************/

/* handle reads from the paletteram */
static READ16_HANDLER( palette_r )
{
	offset &= 0x1ff;
	if (alt_palette_mode)
		offset = ((offset << 1) & 0x100) | ((offset << 2) & 0x80) | ((~offset >> 2) & 0x40) | ((offset >> 1) & 0x20) | (offset & 0x1f);
	return paletteram16[offset + palbank * 0x200];
}


/* handle writes to the paletteram */
static WRITE16_HANDLER( palette_w )
{
	int r,g,b,newword;

	/* adjust for the palette bank */
	offset &= 0x1ff;
	if (alt_palette_mode)
		offset = ((offset << 1) & 0x100) | ((offset << 2) & 0x80) | ((~offset >> 2) & 0x40) | ((offset >> 1) & 0x20) | (offset & 0x1f);
	offset += palbank * 0x200;

	/* combine data */
	COMBINE_DATA(&paletteram16[offset]);
	newword = paletteram16[offset];

	/* up to 8 bits */
	r = ((newword << 1) & 0x1e) | ((newword >> 12) & 0x01);
	g = ((newword >> 3) & 0x1e) | ((newword >> 13) & 0x01);
	b = ((newword >> 7) & 0x1e) | ((newword >> 14) & 0x01);

	/* set the color */
	palette_set_color_rgb(Machine, offset, pal5bit(r), pal5bit(g), pal5bit(b));
}



/******************************************************************************
    Palette Tables
*******************************************************************************

    Both the background and sprites within the VDP have 4 possible palettes
    to select from. External hardware on the C2 boards, controlled by the
    EPM5032 chip and the I/O chip, allows for more complex palette selection.
    The actual palette entry comes from:

        Bits 10-9 = output from I/O port H
        Bits  8-5 = output from EPM5032
        Bits  4-0 = direct from the VDP

    In order to compute bits 8-5, the EPM5032 gets the sprite/background
    output line along with the two bits of palette info. For most games, the
    two bits of palette info map straight through as follows:

        Bits 10-9 = output from I/O port H
        Bits    8 = sprite/background select
        Bits  7-6 = palette selected by writing to prot_w
        Bits  5-4 = direct from the VDP palette select
        Bits  3-0 = direct from the VDP

    However, because there are 4 bits completely controlled by the EPM5032,
    it doesn't have to always map this way. An alternate palette mode can
    be selected which alters the output palette by swizzling the color
    RAM address bits.

******************************************************************************/

static void recompute_palette_tables(void)
{
	int i;

	for (i = 0; i < 4; i++)
	{
		int bgpal = 0x000 + bg_palbase * 0x40 + i * 0x10;
		int sppal = 0x100 + sp_palbase * 0x40 + i * 0x10;

		if (!alt_palette_mode)
		{
			genesis_bg_pal_lookup[i] = palbank * 0x200 + bgpal;
			genesis_sp_pal_lookup[i] = palbank * 0x200 + sppal;
		}
		else
		{
			genesis_bg_pal_lookup[i] = palbank * 0x200 + ((bgpal << 1) & 0x180) + ((~bgpal >> 2) & 0x40) + (bgpal & 0x30);
			genesis_sp_pal_lookup[i] = palbank * 0x200 + ((~sppal << 2) & 0x100) + ((sppal << 2) & 0x80) + ((~sppal >> 2) & 0x40) + ((sppal >> 2) & 0x20) + (sppal & 0x10);
		}
	}
}



/******************************************************************************
    I/O Read & Write Handlers
*******************************************************************************

    Controls, and Poto Poto reads 'S' 'E' 'G' and 'A' (SEGA) from this area
    as a form of protection.

    Lots of unknown writes however offset 0E certainly seems to be banking,
    both colours and sound sample banks.

******************************************************************************/

static READ16_HANDLER( io_chip_r )
{
	offset &= 0x1f/2;

	switch (offset)
	{
		/* I/O ports */
		case 0x00/2:
		case 0x02/2:
		case 0x04/2:
		case 0x06/2:
		case 0x08/2:
		case 0x0a/2:
		case 0x0c/2:
		case 0x0e/2:
			/* if the port is configured as an output, return the last thing written */
			if (misc_io_data[0x1e/2] & (1 << offset))
				return misc_io_data[offset];

			/* otherwise, return an input port */
			if (offset == 0x04/2 && sound_banks)
				return (readinputport(offset) & 0xbf) | (upd7759_0_busy_r(0) << 6);
			return readinputport(offset);

		/* 'SEGA' protection */
		case 0x10/2:
			return 'S';
		case 0x12/2:
			return 'E';
		case 0x14/2:
			return 'G';
		case 0x16/2:
			return 'A';

		/* CNT register & mirror */
		case 0x18/2:
		case 0x1c/2:
			return misc_io_data[0x1c/2];

		/* port direction register & mirror */
		case 0x1a/2:
		case 0x1e/2:
			return misc_io_data[0x1e/2];
	}
	return 0xffff;
}


static WRITE16_HANDLER( io_chip_w )
{
	UINT8 newbank;
	UINT8 old;

	/* generic implementation */
	offset &= 0x1f/2;
	old = misc_io_data[offset];
	misc_io_data[offset] = data;

	switch (offset)
	{
		/* I/O ports */
		case 0x00/2:
		case 0x02/2:
		case 0x04/2:
		case 0x08/2:
		case 0x0a/2:
		case 0x0c/2:
			break;

		/* miscellaneous output */
		case 0x06/2:
			/*
             D7 : To pin 3 of JP15. (Watchdog clock control)
             D6 : To MUTE input pin on TDA1518BQ amplifier.
             D5 : To CN2 pin 10. (Unknown purpose)
             D4 : To CN2 pin 11. (Unknown purpose)
             D3 : To CN1 pin K. (Coin lockout 2)
             D2 : To CN1 pin 9. (Coin lockout 1)
             D1 : To CN1 pin J. (Coin meter 2)
             D0 : To CN1 pin 8. (Coin meter 1)
            */
/*          coin_lockout_w(1, data & 0x08);
            coin_lockout_w(0, data & 0x04); */
			coin_counter_w(1, data & 0x02);
			coin_counter_w(0, data & 0x01);
			break;

		/* banking */
		case 0x0e/2:
			/*
             D7 : To pin A19 of CN4
             D6 : To pin B19 of CN4
             D5 : ?
             D4 : ?
             D3 : To pin 31 of uPD7759 sample ROM (A18 on a 27C040)
             D2 : To pin 30 of uPD7759 sample ROM (A17 on a 27C040)
             D1 : To A10 of color RAM
             D0 : To A9 of color RAM
            */
			newbank = data & 3;
			if (newbank != palbank)
			{
				video_screen_update_partial(0, video_screen_get_vpos(0) + 1);
				palbank = newbank;
				recompute_palette_tables();
			}
			if (sound_banks > 1)
			{
				newbank = (data >> 2) & (sound_banks - 1);
				upd7759_set_bank_base(0, newbank * 0x20000);
			}
			break;

		/* CNT register */
		case 0x1c/2:
			if (sound_banks > 1)
				upd7759_reset_w(0, (data >> 1) & 1);
			break;
	}
}



/******************************************************************************
    Control Write Handler
*******************************************************************************

    Seems to control some global states. The most important bit is the low
    one, which enables/disables the display. This is used while tiles are
    being modified in Bloxeed.

******************************************************************************/

static WRITE16_HANDLER( control_w )
{
	/* skip if not LSB */
	if (!ACCESSING_LSB)
		return;
	data &= 0x0f;

	/* bit 0 controls display enable */
	segac2_enable_display(~data & 1);

	/* bit 1 resets the protection */
	if (!(data & 2))
		prot_write_buf = prot_read_buf = 0;

	/* bit 2 controls palette shuffling; only ribbit and twinsqua use this feature */
	alt_palette_mode = ((~data & 4) >> 2);
	recompute_palette_tables();
}



/******************************************************************************
    Protection Read / Write Handlers
*******************************************************************************

    The protection chip is fairly simple. Writes to it control the palette
    banking for the sprites and backgrounds. The low 4 bits are also
    remembered in a 2-stage FIFO buffer. A read from this chip should
    return a value from a 256x4-bit table. The index into this table is
    computed by taking the second-to-last value written in the upper 4 bits,
    and the previously-fetched table value in the lower 4 bits.

******************************************************************************/

/* protection chip reads */
static READ16_HANDLER( prot_r )
{
	if (LOG_PROTECTION) logerror("%06X:protection r=%02X\n", activecpu_get_previouspc(), prot_func ? prot_read_buf : 0xff);
	return prot_read_buf | 0xf0;
}


/* protection chip writes */
static WRITE16_HANDLER( prot_w )
{
	int new_sp_palbase = (data >> 2) & 3;
	int new_bg_palbase = data & 3;
	int table_index;

	/* only works for the LSB */
	if (!ACCESSING_LSB)
		return;

	/* compute the table index */
	table_index = (prot_write_buf << 4) | prot_read_buf;

	/* keep track of the last write for the next table lookup */
	prot_write_buf = data & 0x0f;

	/* determine the value to return, should a read occur */
	if (prot_func)
		prot_read_buf = prot_func(table_index);
	if (LOG_PROTECTION) logerror("%06X:protection w=%02X, new result=%02X\n", activecpu_get_previouspc(), data & 0x0f, prot_read_buf);

	/* if the palette changed, force an update */
	if (new_sp_palbase != sp_palbase || new_bg_palbase != bg_palbase)
	{
		video_screen_update_partial(0, video_screen_get_vpos(0) + 1);
		sp_palbase = new_sp_palbase;
		bg_palbase = new_bg_palbase;
		recompute_palette_tables();
		if (LOG_PALETTE) logerror("Set palbank: %d/%d (scan=%d)\n", bg_palbase, sp_palbase, video_screen_get_vpos(0));
	}
}



/******************************************************************************
    Counter/timer I/O
*******************************************************************************

    There appears to be a chip that is used to count coins and track time
    played, or at the very least the current status of the game. All games
    except Puyo Puyo 2 and Poto Poto access this in a mostly consistent
    manner.

******************************************************************************/

static WRITE16_HANDLER( counter_timer_w )
{
	/* only LSB matters */
	if (ACCESSING_LSB)
	{
		/*int value = data & 1;*/
		switch (data & 0x1e)
		{
			case 0x00:	/* player 1 start/stop */
			case 0x02:	/* player 2 start/stop */
			case 0x04:	/* ??? */
			case 0x06:	/* ??? */
			case 0x08:	/* player 1 game timer? */
			case 0x0a:	/* player 2 game timer? */
			case 0x0c:	/* ??? */
			case 0x0e:	/* ??? */
				break;

			case 0x10:	/* coin counter */
//              coin_counter_w(0,1);
//              coin_counter_w(0,0);
				break;

			case 0x12:	/* set coinage info -- followed by two 4-bit values */
				break;

			case 0x14:	/* game timer? (see Tant-R) */
			case 0x16:	/* intro timer? (see Tant-R) */
			case 0x18:	/* ??? */
			case 0x1a:	/* ??? */
			case 0x1c:	/* ??? */
				break;

			case 0x1e:	/* reset */
				break;
		}
	}
}



/******************************************************************************
    Print Club camera handling
*******************************************************************************

    Just some fake stuff to get us to boot.

******************************************************************************/

static int cam_data;

static READ16_HANDLER( printer_r )
{
	return cam_data;
}

static WRITE16_HANDLER( print_club_camera_w )
{
	cam_data = data;
}




/******************************************************************************
    Memory Maps
*******************************************************************************

    The System C/C2 68k Memory map is fairly similar to the Genesis in terms
    of RAM, ROM, VDP access locations, although the differences between the
    arcade system and the Genesis means its not same.

******************************************************************************/

static ADDRESS_MAP_START( main_map, ADDRESS_SPACE_PROGRAM, 16 )
	AM_RANGE(0x000000, 0x1fffff) AM_ROM
	AM_RANGE(0x800000, 0x800001) AM_MIRROR(0x13fdfe) AM_READWRITE(prot_r, prot_w)
	AM_RANGE(0x800200, 0x800201) AM_MIRROR(0x13fdfe) AM_WRITE(control_w)
	AM_RANGE(0x840000, 0x84001f) AM_MIRROR(0x13fee0) AM_READWRITE(io_chip_r, io_chip_w)
	AM_RANGE(0x840100, 0x840107) AM_MIRROR(0x13fef8) AM_READWRITE(ym3438_r, ym3438_w)
	AM_RANGE(0x880000, 0x880001) AM_MIRROR(0x13fefe) AM_WRITE(segac2_upd7759_w)
	AM_RANGE(0x880100, 0x880101) AM_MIRROR(0x13fefe) AM_WRITE(counter_timer_w)
	AM_RANGE(0x8c0000, 0x8c0fff) AM_MIRROR(0x13f000) AM_READWRITE(palette_r, palette_w) AM_BASE(&paletteram16)
	AM_RANGE(0xc00000, 0xc0001f) AM_MIRROR(0x18ff00) AM_READWRITE(genesis_vdp_r, genesis_vdp_w)
	AM_RANGE(0xe00000, 0xe0ffff) AM_MIRROR(0x1f0000) AM_RAM AM_BASE(&generic_nvram16) AM_SIZE(&generic_nvram_size)
ADDRESS_MAP_END



/******************************************************************************
    Input Ports
*******************************************************************************

    The input ports on the C2 games always consist of 1 Coin Port, 2 Player
    Input ports and 2 Dipswitch Ports, 1 of those Dipswitch Ports being used
    for coinage, the other for Game Options.

    Most of the Games List the Dipswitchs and Inputs in the Test Menus, adding
    them is just a tedious task.  I think Columnns & Bloxeed are Exceptions
    and will need their Dipswitches working out by observation.  The Coin Part
    of the DSW's seems fairly common to all games.

******************************************************************************/

static INPUT_PORTS_START( systemc_generic )
    PORT_START_TAG("P1")
    PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1)
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
    PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
    PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
    PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
    PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)

    PORT_START_TAG("P2")
    PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2)
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
    PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
    PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
    PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
    PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)

	PORT_START_TAG("PORTC")
	PORT_BIT( 0x3f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_SPECIAL )	/* From uPD7759 pin 18. (/BUSY output) */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SPECIAL )	/* From MB3773P pin 1. (/RESET output) */

	PORT_START_TAG("PORTD")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START_TAG("SERVICE")
    PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_SERVICE_NO_TOGGLE( 0x04, IP_ACTIVE_LOW )
    PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_SERVICE1 )
    PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START1 )
    PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START2 )
    PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
    PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START_TAG("COINAGE")
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x05, "2 Coins/1 Credit 5/3 6/4" )
	PORT_DIPSETTING(    0x04, "2 Coins/1 Credit 4/3" )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, "1 Coin/1 Credit 2/3" )
	PORT_DIPSETTING(    0x02, "1 Coin/1 Credit 4/5" )
	PORT_DIPSETTING(    0x03, "1 Coin/1 Credit 5/6" )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin B too) or 1/1" )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x50, "2 Coins/1 Credit 5/3 6/4" )
	PORT_DIPSETTING(    0x40, "2 Coins/1 Credit 4/3" )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x10, "1 Coin/1 Credit 2/3" )
	PORT_DIPSETTING(    0x20, "1 Coin/1 Credit 4/5" )
	PORT_DIPSETTING(    0x30, "1 Coin/1 Credit 5/6" )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin A too) or 1/1" )

	PORT_START_TAG("DSW")
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

	PORT_START_TAG("PORTH")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


static INPUT_PORTS_START( columns )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    /* The first level increase (from 0 to 1) is always after destroying
       35 jewels. Then, the level gets 1 level more every : */
    PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
    PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )     // 50 jewels
    PORT_DIPSETTING(    0x10, DEF_STR( Medium ) )   // 40 jewels
    PORT_DIPSETTING(    0x30, DEF_STR( Hard ) )     // 35 jewels
    PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )  // 25 jewels
INPUT_PORTS_END

static INPUT_PORTS_START( columnsu )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    /* The first level increase (from 0 to 1) is always after destroying
       35 jewels. Then, the level gets 1 level more every : */
    PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
    PORT_DIPSETTING(    0x00, DEF_STR( Easy ) )     // 50 jewels
    PORT_DIPSETTING(    0x10, DEF_STR( Medium ) )   // 40 jewels
    PORT_DIPSETTING(    0x30, DEF_STR( Hard ) )     // 35 jewels
    PORT_DIPSETTING(    0x20, DEF_STR( Hardest ) )  // 25 jewels
INPUT_PORTS_END

static INPUT_PORTS_START( columns2 )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
    PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x0c, "VS. Mode Credits/Match" )
	PORT_DIPSETTING(    0x0c, "1" )
	PORT_DIPSETTING(    0x08, "2" )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPNAME( 0x30, 0x30, "Flash Mode Difficulty" )
	PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
    PORT_DIPSETTING(    0x30, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


static INPUT_PORTS_START( borench )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x01, "Credits to Start" )
	PORT_DIPSETTING(    0x01, "1" )
	PORT_DIPSETTING(    0x00, "2" )
    PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x0c, 0x0c, "Lives 1P Mode" )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x0c, "2" )
	PORT_DIPSETTING(    0x08, "3" )
	PORT_DIPSETTING(    0x04, "4" )
    PORT_DIPNAME( 0x30, 0x30, "Lives 2P Mode" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x30, "3" )
	PORT_DIPSETTING(    0x20, "4" )
	PORT_DIPSETTING(    0x10, "5" )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
    PORT_DIPSETTING(    0xc0, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


static INPUT_PORTS_START( tfrceac )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x01, "Credits to Start" )
	PORT_DIPSETTING(    0x01, "1" )
	PORT_DIPSETTING(    0x00, "2" )
    PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x08, "4" )
	PORT_DIPSETTING(    0x04, "5" )
	PORT_DIPNAME( 0x30, 0x30,  DEF_STR( Bonus_Life ) )
    PORT_DIPSETTING(    0x10, "10k, 70k, 150k" )
    PORT_DIPSETTING(    0x30, "20k, 100k, 200k" )
    PORT_DIPSETTING(    0x20, "40k, 150k, 300k" )
    PORT_DIPSETTING(    0x00, DEF_STR( None ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Easy ) )
    PORT_DIPSETTING(    0xc0, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


static INPUT_PORTS_START( twinsqua )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(15) PORT_PLAYER(1)

	PORT_MODIFY("P2")
	PORT_BIT( 0xff, 0x00, IPT_DIAL ) PORT_SENSITIVITY(30) PORT_KEYDELTA(15) PORT_PLAYER(2)

	PORT_MODIFY("SERVICE")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1)
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x01, "Credits to Start" )
	PORT_DIPSETTING(    0x01, "1" )
	PORT_DIPSETTING(    0x00, "2" )
    PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x04, 0x04, "Buy In" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x18, 0x18, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
    PORT_DIPSETTING(    0x18, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
    PORT_DIPNAME( 0x20, 0x20, "Seat Type" )
	PORT_DIPSETTING(    0x20, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x00, "Moving" )
INPUT_PORTS_END


static INPUT_PORTS_START( ribbit )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 1 Unused */
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 1 Unused */
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x01, "Credits to Start" )
	PORT_DIPSETTING(    0x01, "1" )
	PORT_DIPSETTING(    0x00, "2" )
    PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x0c, 0x04, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x08, "1" )
	PORT_DIPSETTING(    0x0c, "2" )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Difficulty ) )
    PORT_DIPSETTING(    0x20, DEF_STR( Easy ) )
    PORT_DIPSETTING(    0x30, DEF_STR( Normal ) )
    PORT_DIPSETTING(    0x10, DEF_STR( Hard ) )
    PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


static INPUT_PORTS_START( puyo )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
    PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "VS. Mode Credits/Match" )
	PORT_DIPSETTING(    0x04, "1" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPNAME( 0x18, 0x18, "1P Mode Difficulty" )
	PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
    PORT_DIPSETTING(    0x18, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x80, 0x80, "Moving Seat" )
	PORT_DIPSETTING(    0x80, "No Use" )
	PORT_DIPSETTING(    0x00, "In Use" )
INPUT_PORTS_END


static INPUT_PORTS_START( stkclmns )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Easy ) )
    PORT_DIPSETTING(    0x03, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
    PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "Match Mode Price" )
	PORT_DIPSETTING(    0x08, "1" )
	PORT_DIPSETTING(    0x00, "2" )
INPUT_PORTS_END


static INPUT_PORTS_START( potopoto )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x01, "Credits to Start" )
	PORT_DIPSETTING(    0x01, "1" )
	PORT_DIPSETTING(    0x00, "2" )
    PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "Coin Chute Type" )
	PORT_DIPSETTING(    0x04, "Common" )
	PORT_DIPSETTING(    0x00, "Individual" )
	PORT_DIPNAME( 0x08, 0x08, "Credits to Continue" )
	PORT_DIPSETTING(    0x08, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPNAME( 0x10, 0x10, "Buy-In" )
	PORT_DIPSETTING(    0x10, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x60, 0x60, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Easy ) )
    PORT_DIPSETTING(    0x60, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPNAME( 0x80, 0x80, "Moving Seat" )
	PORT_DIPSETTING(    0x80, "No Use" )
	PORT_DIPSETTING(    0x00, "In Use" )
INPUT_PORTS_END


static INPUT_PORTS_START( zunkyou )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
	PORT_DIPNAME( 0x01, 0x01, "Game Difficulty 1" )
    PORT_DIPSETTING(    0x01, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
	PORT_DIPNAME( 0x02, 0x02, "Game Difficulty 2" )
    PORT_DIPSETTING(    0x02, DEF_STR( Medium ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hard ) )
    PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x08, "1" )
	PORT_DIPSETTING(    0x04, "2" )
	PORT_DIPSETTING(    0x0c, "3" )
	PORT_DIPSETTING(    0x00, "5" )
    PORT_DIPNAME( 0x10, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


static INPUT_PORTS_START( ichir )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
    PORT_DIPNAME( 0x01, 0x00, DEF_STR( Demo_Sounds ) )
    PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x06, 0x06, DEF_STR( Difficulty ) )
    PORT_DIPSETTING(    0x04, DEF_STR( Easy ) )
    PORT_DIPSETTING(    0x06, DEF_STR( Medium ) )
    PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
INPUT_PORTS_END


static INPUT_PORTS_START( bloxeedc )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 2 Unused */
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
    PORT_DIPNAME( 0x01, 0x01, "VS Mode Price" )
    PORT_DIPSETTING(    0x00, "Same as Ordinary" )
    PORT_DIPSETTING(    0x01, "Double as Ordinary" )
    PORT_DIPNAME( 0x02, 0x02, "Credits to Start" )
    PORT_DIPSETTING(    0x02, "1" )
	PORT_DIPSETTING(    0x00, "2" )
    PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )
    PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


static INPUT_PORTS_START( puyopuy2 )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("P2")
    PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )		/* Button 3 Unused */

	PORT_MODIFY("DSW")
    PORT_DIPNAME( 0x01, 0x01, "Rannyu Off Button" )
    PORT_DIPSETTING(    0x01, "Use" )
    PORT_DIPSETTING(    0x00, "No Use" )
    PORT_DIPNAME( 0x02, 0x00, DEF_STR( Demo_Sounds ) )
    PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x04, 0x04, "Turn Direction" )
    PORT_DIPSETTING(    0x04, "1:Right  2:Left" )
    PORT_DIPSETTING(    0x00, "1:Left  2:Right")
    PORT_DIPNAME( 0x18, 0x18, DEF_STR( Difficulty ) )
    PORT_DIPSETTING(    0x10, DEF_STR( Easy ) )
    PORT_DIPSETTING(    0x18, DEF_STR( Medium ) )
    PORT_DIPSETTING(    0x08, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
    PORT_DIPNAME( 0x60, 0x60, "VS Mode Match/1 Play" )
    PORT_DIPSETTING(    0x60, "1" )
    PORT_DIPSETTING(    0x40, "2" )
    PORT_DIPSETTING(    0x20, "3" )
    PORT_DIPSETTING(    0x00, "4" )
    PORT_DIPNAME( 0x80, 0x80, "Battle Start credit" )
    PORT_DIPSETTING(    0x00, "1" )
    PORT_DIPSETTING(    0x80, "2" )
INPUT_PORTS_END


static INPUT_PORTS_START( pclub )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNKNOWN ) 	/* Probably Unused */
    PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Ok")
    PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Cancel")
    PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY
    PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_2WAY

	PORT_MODIFY("P2")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN ) 	/* Probably Unused */

	PORT_MODIFY("SERVICE")
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* Probably Unused */

	PORT_MODIFY("COINAGE")
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 7C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 4C_1C ) )
    PORT_DIPSETTING(    0x07, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_1C ) )
    PORT_DIPSETTING(    0x06, DEF_STR( Free_Play ) )
    PORT_DIPNAME( 0x08, 0x08, "Unknown 4-4" )
    PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x10, 0x10, "Unknown 4-5" )
    PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x20, 0x20, "Unknown 4-6" )
    PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x40, 0x40, "Unknown 4-7" )
    PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x80, 0x80, "Unknown 4-8" )
    PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_MODIFY("DSW")
    PORT_DIPNAME( 0x01, 0x01, "Unknown 5-1" )
    PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
    PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x02, 0x02, "Unknown 5-2" )
    PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x04, 0x04, "Unknown 5-3" )
    PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x08, 0x08, "Unknown 5-4" )
    PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x10, 0x10, "Unknown 5-5" )
    PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x20, 0x20, DEF_STR( Demo_Sounds ))
    PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
    PORT_DIPNAME( 0x40, 0x40, "Unknown 5-7" )
    PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x80, 0x80, "Unknown 5-8" )
    PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


static INPUT_PORTS_START( pclubjv2 )
	PORT_INCLUDE( systemc_generic )

	PORT_MODIFY("P1")
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNKNOWN ) 	/* Probably Unused */
    PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Ok")
    PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("Cancel")
    PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_2WAY
    PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_2WAY

	PORT_MODIFY("P2")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNKNOWN ) 	/* Probably Unused */

	PORT_MODIFY("SERVICE")
	PORT_BIT( 0xf0, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* Probably Unused */

	PORT_MODIFY("COINAGE")
	PORT_DIPNAME( 0x07, 0x07, "Coins per Credit (Normal / Alternate)" )
	PORT_DIPSETTING(    0x00, "25 / 30" ) //7C1C
	PORT_DIPSETTING(    0x01, "6 / 7" ) //6C1C
	PORT_DIPSETTING(    0x02, "10 / 12" ) //5C1C
	PORT_DIPSETTING(    0x03, "1 / 2" ) //4C1C
    PORT_DIPSETTING(    0x07, "3 / Free Play" ) //3C1C
	PORT_DIPSETTING(    0x04, "15 / 20" ) //2C1C
	PORT_DIPSETTING(    0x05, "4 / 5" ) //1C1C
    PORT_DIPSETTING(    0x06, "8 / 9" ) //FP
    PORT_DIPNAME( 0x08, 0x08, "Alternate Coinage" )
    PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x10, 0x10, "Unknown 4-5" )
    PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x20, 0x20, "Unknown 4-6" )
    PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x40, 0x40, "Unknown 4-7" )
    PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x80, 0x80, "Unknown 4-8" )
    PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_MODIFY("DSW")
    PORT_DIPNAME( 0x01, 0x01, "Unknown 5-1" )
    PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
    PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x02, 0x02, "Unknown 5-2" )
    PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x04, 0x04, "Unknown 5-3" )
    PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x08, 0x08, "Unknown 5-4" )
    PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x10, 0x10, "Unknown 5-5" )
    PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x20, 0x20, DEF_STR( Demo_Sounds ))
    PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
    PORT_DIPNAME( 0x40, 0x40, "Unknown 5-7" )
    PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    PORT_DIPNAME( 0x80, 0x80, "Unknown 5-8" )
    PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

/******************************************************************************
    Sound interfaces
******************************************************************************/

static const struct upd7759_interface upd7759_intf =
{
	REGION_SOUND1				/* Memory pointer (gen.h) */
};

static const struct YM3438interface ym3438_intf =
{
	genesis_irq2_interrupt		/* IRQ handler */
};



/******************************************************************************
    Machine Drivers
*******************************************************************************

    General Overview
        M68000 @ 10MHz (Main Processor)
        YM3438 (Fm Sound)
        SN76489 (PSG, Noise, Part of the VDP)
        UPD7759 (Sample Playback, C-2 Only)

******************************************************************************/

static MACHINE_DRIVER_START( segac )

	/* basic machine hardware */
	MDRV_CPU_ADD_TAG("main", M68000, XL2_CLOCK/6)
	MDRV_CPU_PROGRAM_MAP(main_map,0)
	MDRV_CPU_VBLANK_INT("main", genesis_vblank_interrupt)

	MDRV_MACHINE_START(segac2)
	MDRV_MACHINE_RESET(segac2)
	MDRV_NVRAM_HANDLER(generic_randfill)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_HAS_SHADOWS | VIDEO_HAS_HIGHLIGHTS)

	MDRV_SCREEN_ADD("main", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(342,262)
	MDRV_SCREEN_VISIBLE_AREA(0, 319, 0, 223)

	MDRV_PALETTE_LENGTH(2048)

	MDRV_VIDEO_START(segac2)
	MDRV_VIDEO_UPDATE(segac2)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")

	MDRV_SOUND_ADD(YM3438, XL2_CLOCK/7)
	MDRV_SOUND_CONFIG(ym3438_intf)
	MDRV_SOUND_ROUTE(0, "mono", 0.50)
	/* right channel not connected */

	MDRV_SOUND_ADD(SN76496, XL2_CLOCK/15)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( segac2 )

	/* basic machine hardware */
	MDRV_IMPORT_FROM( segac )

	/* sound hardware */
	MDRV_SOUND_ADD(UPD7759, XL1_CLOCK)
	MDRV_SOUND_CONFIG(upd7759_intf)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_DRIVER_END



/******************************************************************************
    Rom Definitions
*******************************************************************************

    All the known System C/C2 Dumps are listed here with the exception of
    the version of Puzzle & Action (I believe its actually Ichidant-R) which
    was credited to SpainDumps in the included text file.  This appears to be
    a bad dump (half sized roms) however the roms do not match up exactly with
    the good dump of the game.  English language sets are assumed to be the
    parent where they exist.  Hopefully some more alternate version dumps will
    turn up sometime soon for example English Language version of Tant-R or
    Japanese Language versions of Borench (if of course these games were
    released in other locations.

    Games are in Order of Date (Year) with System-C titles coming first.

******************************************************************************/


/* ----- System C Games ----- */

ROM_START( bloxeedc ) /* Bloxeed (C System Version)  (c)1989 Sega / Elorg */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr12908.32", 0x000000, 0x020000, CRC(fc77cb91) SHA1(248a462e3858ffdc171af7d806e57deecb5dae50) )
	ROM_LOAD16_BYTE( "epr12907.31", 0x000001, 0x020000, CRC(e5fcbac6) SHA1(a1adec5ef5574bff96a3d66619a24a6715097bb9) )
	ROM_LOAD16_BYTE( "epr12993.34", 0x040000, 0x020000, CRC(487bc8fc) SHA1(3fb205bf56f35443e993e08b39c1a08c13ca5e3b) )
	ROM_LOAD16_BYTE( "epr12992.33", 0x040001, 0x020000, CRC(19b0084c) SHA1(b3ba0f3d8d39a19aa66edb24885ea21192e22704) )
ROM_END

ROM_START( bloxeedu ) /* Bloxeed USA (C System Version)  (c)1989 Sega / Elorg */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr12997a.32", 0x000000, 0x020000, CRC(23655bc9) SHA1(32fc1f75a43aa49dc656d40d34ec10f3f0a2bdb3) )
	ROM_LOAD16_BYTE( "epr12996a.31", 0x000001, 0x020000, CRC(83c83f0c) SHA1(ca8e2ad7cceabd8de7a91b91cb92eafb6dd3171f) )
	ROM_LOAD16_BYTE( "epr12993.34", 0x040000, 0x020000, CRC(487bc8fc) SHA1(3fb205bf56f35443e993e08b39c1a08c13ca5e3b) )
	ROM_LOAD16_BYTE( "epr12992.33", 0x040001, 0x020000, CRC(19b0084c) SHA1(b3ba0f3d8d39a19aa66edb24885ea21192e22704) )
ROM_END


ROM_START( columns ) /* Columns (World) (c)1990 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr13114.32", 0x000000, 0x020000, CRC(ff78f740) SHA1(0a034103a4b942f43e62f6e717f5dbf1bfb0b613) )
	ROM_LOAD16_BYTE( "epr13113.31", 0x000001, 0x020000, CRC(9a426d9b) SHA1(3322e65ebf8d0a6047f7d408387c63ea401b8973) )
ROM_END

ROM_START( columnsu ) /* Columns (US) (c)1990 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr13116a.32", 0x000000, 0x020000, CRC(a0284b16) SHA1(a72c8567ab2386ef6bc7bb83cc1612f4c6bf8461) )
	ROM_LOAD16_BYTE( "epr13115a.31", 0x000001, 0x020000, CRC(e37496f3) SHA1(30ebeed76613ae8d6d3ce9fca282124685067b27) )
ROM_END

ROM_START( columnsj ) /* Columns (Jpn) (c)1990 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr13112.32", 0x000000, 0x020000, CRC(bae6e53e) SHA1(2c2fd621eecd55591f22d076323972a7d0314615) )
	ROM_LOAD16_BYTE( "epr13111.31", 0x000001, 0x020000, CRC(aa5ccd6d) SHA1(480e29e3112282d1790f1fb68075453325ba4336) )
ROM_END


ROM_START( columns2 ) /* Columns II - The Voyage Through Time  (c)1990 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr13363.bin", 0x000000, 0x020000, CRC(c99e4ffd) SHA1(67981aa08c8a625af35dd7689011364159cf9194) )
	ROM_LOAD16_BYTE( "epr13362.bin", 0x000001, 0x020000, CRC(394e2419) SHA1(d4f726b32cf301d0d52611237b83177e69bfaf71) )
ROM_END

ROM_START( column2j ) /* Columns II - The Voyage Through Time (Jpn)  (c)1990 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr13361.rom", 0x000000, 0x020000, CRC(b54b5f12) SHA1(4d7fbae7d9bcadd433ebc25aef255dc43df611bc) )
	ROM_LOAD16_BYTE( "epr13360.rom", 0x000001, 0x020000, CRC(a59b1d4f) SHA1(e9ee315677782e1c61ae8f11260101cc03176188) )
ROM_END


ROM_START( tantrbl2 ) /* Tant-R (Puzzle & Action) (Alt Bootleg Running on C Board?, No Samples) */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "trb2_2.32",    0x000000, 0x080000, CRC(8fc99c48) SHA1(d90ed673fe1f6e1f878c0d8fc62f5439b56d0a47) )
	ROM_LOAD16_BYTE( "trb2_1.31",    0x000001, 0x080000, CRC(c318d00d) SHA1(703760d4ddc45bc0921ae96a27d9a8fbf12a1e96) )
	ROM_LOAD16_BYTE( "mpr15616.34",  0x100000, 0x080000, CRC(17b80202) SHA1(f47bf2aa0c5972647438619b8453c7dede5c422f) )
	ROM_LOAD16_BYTE( "mpr15615.33",  0x100001, 0x080000, CRC(36a88bd4) SHA1(cc7f6a947d1b79bb86957c43035b53d6d2bcfa28) )
ROM_END

ROM_START( tantrbl3 ) /* Tant-R (Puzzle & Action) (Alt Bootleg Running on C Board?, No Samples) */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "2.u29",  0x000000, 0x080000, CRC(faecb7b1) SHA1(ddca61996dbd0d66e9ecc04647e61a6d0af25807) )
	ROM_LOAD16_BYTE( "1.u28",  0x000001, 0x080000, CRC(3304556d) SHA1(586265c846bf6404dcb33404e961bf0733e24bf5) )
	ROM_LOAD16_BYTE( "4.u31",  0x100000, 0x080000, CRC(17b80202) SHA1(f47bf2aa0c5972647438619b8453c7dede5c422f) )
	ROM_LOAD16_BYTE( "3.u30",  0x100001, 0x080000, CRC(36a88bd4) SHA1(cc7f6a947d1b79bb86957c43035b53d6d2bcfa28) )
ROM_END

ROM_START( ichirjbl ) /* Ichident-R (Puzzle & Action 2) (Bootleg Running on C Board?, No Samples) */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "27c4000.2",0x000000, 0x080000, CRC(5a194f44) SHA1(67a4d21b91704f8c2210b5106e82e22ba3366f4c) )
	ROM_LOAD16_BYTE( "27c4000.1",0x000001, 0x080000, CRC(de209f84) SHA1(0860d0ebfab2952e82fc1e292bf9410d673d9322) )
	ROM_LOAD16_BYTE( "epr16888", 0x100000, 0x080000, CRC(85d73722) SHA1(7ebe81b4d6c89f87f60200a3a8cddb07d581adef) )
	ROM_LOAD16_BYTE( "epr16887", 0x100001, 0x080000, CRC(bc3bbf25) SHA1(e760ad400bc183b38e9787d88c8ac084fbe2ae21) )
ROM_END


/* ----- System C-2 Games ----- */

ROM_START( borench ) /* Borench  (c)1990 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "ic32.bin", 0x000000, 0x040000, CRC(2c54457d) SHA1(adf3ea5393d2633ec6215e64f0cd89ad4567e765) )
	ROM_LOAD16_BYTE( "ic31.bin", 0x000001, 0x040000, CRC(b46445fc) SHA1(24e85ef5abbc5376a854b13ed90f08f0c30d7f25) )

	ROM_REGION( 0x020000, REGION_SOUND1, 0 )
	ROM_LOAD( "ic4.bin", 0x000000, 0x020000, CRC(62b85e56) SHA1(822ab733c87938bb70a9e32cc5dd36bbf6f21d11) )
ROM_END


ROM_START( tfrceac ) /* ThunderForce AC  (c)1990 Technosoft / Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "ic32.bin", 0x000000, 0x040000, CRC(95ecf202) SHA1(92b0f351f2bee7d59873a4991615f14f1afe4da7) )
	ROM_LOAD16_BYTE( "ic31.bin", 0x000001, 0x040000, CRC(e63d7f1a) SHA1(a40d0a5a96f379a467048dc8fddd8aaaeb94da1d) )
	/* 0x080000 - 0x100000 Empty */
	ROM_LOAD16_BYTE( "ic34.bin", 0x100000, 0x040000, CRC(29f23461) SHA1(032a7125fef5a660b85654d595aafc46812cdde6) )
	ROM_LOAD16_BYTE( "ic33.bin", 0x100001, 0x040000, CRC(9e23734f) SHA1(64d27dc53f0ffc3513345a26ed077751b25d15f1) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 )
	ROM_LOAD( "ic4.bin", 0x000000, 0x040000, CRC(e09961f6) SHA1(e109b5f41502b765d191f22e3bbcff97d6defaa1) )
ROM_END

ROM_START( tfrceacj ) /* ThunderForce AC (Jpn)  (c)1990 Technosoft / Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr13657.32", 0x000000, 0x040000, CRC(a0f38ffd) SHA1(da548e7f61aed0e82a460553a119941da8857bc4) )
	ROM_LOAD16_BYTE( "epr13656.31", 0x000001, 0x040000, CRC(b9438d1e) SHA1(598209c9fec3527fde720af09e5bebd7379f5b2b) )
	/* 0x080000 - 0x100000 Empty */
	ROM_LOAD16_BYTE( "ic34.bin",    0x100000, 0x040000, CRC(29f23461) SHA1(032a7125fef5a660b85654d595aafc46812cdde6) )
	ROM_LOAD16_BYTE( "ic33.bin",    0x100001, 0x040000, CRC(9e23734f) SHA1(64d27dc53f0ffc3513345a26ed077751b25d15f1) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 )
	ROM_LOAD( "ic4.bin", 0x000000, 0x040000, CRC(e09961f6) SHA1(e109b5f41502b765d191f22e3bbcff97d6defaa1) )
ROM_END

ROM_START( tfrceacb ) /* ThunderForce AC (Bootleg)  (c)1990 Technosoft / Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "4.bin",    0x000000, 0x040000, CRC(eba059d3) SHA1(7bc04401f9a138fa151ac09a528b70acfb2021e3) )
	ROM_LOAD16_BYTE( "3.bin",    0x000001, 0x040000, CRC(3e5dc542) SHA1(4a66dc842afaa145dab82b232738eea107bdf0f8) )
	/* 0x080000 - 0x100000 Empty */
	ROM_LOAD16_BYTE( "ic34.bin", 0x100000, 0x040000, CRC(29f23461) SHA1(032a7125fef5a660b85654d595aafc46812cdde6) )
	ROM_LOAD16_BYTE( "ic33.bin", 0x100001, 0x040000, CRC(9e23734f) SHA1(64d27dc53f0ffc3513345a26ed077751b25d15f1) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 )
	ROM_LOAD( "ic4.bin", 0x000000, 0x040000, CRC(e09961f6) SHA1(e109b5f41502b765d191f22e3bbcff97d6defaa1) )
ROM_END


ROM_START( twinsqua ) /* Twin Squash  (c)1991 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "ep14657.32", 0x000000, 0x040000, CRC(becbb1a1) SHA1(787b1a4bf420186d05b5448582f6492e40d394fa) )
	ROM_LOAD16_BYTE( "ep14656.31", 0x000001, 0x040000, CRC(411906e7) SHA1(68a4e66b9e18499d77cdb584470f35f67edec6fd) )

	ROM_REGION( 0x020000, REGION_SOUND1, 0 )
	ROM_LOAD( "ep14588.4", 0x000000, 0x020000, CRC(5a9b6881) SHA1(d86ec7f569fae5a1ce93a1cf40998cbb13726e0c) )
ROM_END


ROM_START( ribbit ) /* Ribbit  (c)1991 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "ep13833.32", 0x000000, 0x040000, CRC(5347f8ce) SHA1(b95b99536157edfbf0d74a42f64235f47dca7ee1) )
	ROM_LOAD16_BYTE( "ep13832.31", 0x000001, 0x040000, CRC(889c42c2) SHA1(0839a50a68b64a66d995f1bfaff42fcb60bb4d45) )
	ROM_COPY( REGION_CPU1, 0x000000, 0x080000, 0x080000 )
	ROM_LOAD16_BYTE( "ep13838.34", 0x100000, 0x080000, CRC(a5d62ac3) SHA1(8d83a7bc4017e125ef4231278f766b2368d5fc1f) )
	ROM_LOAD16_BYTE( "ep13837.33", 0x100001, 0x080000, CRC(434de159) SHA1(cf2973131cabf2bc0ebb2bfe9f804ad3d7d0a733) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )
	ROM_LOAD( "ep13834.4", 0x000000, 0x020000, CRC(ab0c1833) SHA1(f864e12ecf6c0524da20fc66747a4fa4280e67e9) )
ROM_END


ROM_START( tantr ) /* Tant-R (Puzzle & Action)  (c)1992 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr15614.32", 0x000000, 0x080000, CRC(557782bc) SHA1(1546a999ab97c380dc87f6c95d5687722206740d) )
	ROM_LOAD16_BYTE( "epr15613.31", 0x000001, 0x080000, CRC(14bbb235) SHA1(8dbfec5fb1d7a695acbb2fc0e78e4bdf76eb8d9d) )
	ROM_LOAD16_BYTE( "mpr15616.34", 0x100000, 0x080000, CRC(17b80202) SHA1(f47bf2aa0c5972647438619b8453c7dede5c422f) )
	ROM_LOAD16_BYTE( "mpr15615.33", 0x100001, 0x080000, CRC(36a88bd4) SHA1(cc7f6a947d1b79bb86957c43035b53d6d2bcfa28) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr15617.4", 0x000000, 0x040000, CRC(338324a1) SHA1(79e2782d0d4764dd723886f846c852a6f6c1fb64) )
ROM_END

ROM_START( tantrkor ) /* Tant-R (Puzzle & Action) (Korea) (c)1993 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	/* strange names, but this is what was printed on the (original) chips */
	ROM_LOAD16_BYTE( "m15592b.32", 0x000000, 0x080000, CRC(7efe26b3) SHA1(958420b9b400eafe392745af90bff729463427c7) )
	ROM_LOAD16_BYTE( "m15592b.31", 0x000001, 0x080000, CRC(af5a860f) SHA1(cb0011f420721d035e9f0e43bb72cf286982fd32) )
	ROM_LOAD16_BYTE( "m15992b.34", 0x100000, 0x080000, CRC(6282a5d4) SHA1(9220e119e79d969d7d70e8a25c75dd3d9bc340ae) )
	ROM_LOAD16_BYTE( "m15592b.33", 0x100001, 0x080000, CRC(82d78413) SHA1(9ff9c2b1632e280444965110bab90c0fc98cd6da) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr15617.4", 0x000000, 0x040000, CRC(338324a1) SHA1(79e2782d0d4764dd723886f846c852a6f6c1fb64) )
ROM_END

ROM_START( tantrbl ) /* Tant-R (Puzzle & Action) (Bootleg)  (c)1992 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "pa_e10.bin",  0x000000, 0x080000, CRC(6c3f711f) SHA1(55aa2d50422134b95d9a7c5cbdc453b207b91b4c) )
	ROM_LOAD16_BYTE( "pa_f10.bin",  0x000001, 0x080000, CRC(75526786) SHA1(8f5aa7f6918b71a79e6fca18194beec2aef15844) )
	ROM_LOAD16_BYTE( "mpr15616.34", 0x100000, 0x080000, CRC(17b80202) SHA1(f47bf2aa0c5972647438619b8453c7dede5c422f) )
	ROM_LOAD16_BYTE( "mpr15615.33", 0x100001, 0x080000, CRC(36a88bd4) SHA1(cc7f6a947d1b79bb86957c43035b53d6d2bcfa28) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 )
	ROM_LOAD( "pa_e03.bin", 0x000000, 0x020000, CRC(72918c58) SHA1(cb42363b163727a887a0b762519c72dcdf0a6460) )
	ROM_LOAD( "pa_e02.bin", 0x020000, 0x020000, CRC(4e85b2a3) SHA1(3f92fb931d315c5a2d6c54b3204718574928cb7b) )
ROM_END


ROM_START( puyo ) /* Puyo Puyo  (c)1992 Sega / Compile */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr-15198.32", 0x000000, 0x020000, CRC(9610d80c) SHA1(1ffad09d3369c1942d4db611c41bae47d08c7564) )
	ROM_LOAD16_BYTE( "epr-15197.31", 0x000001, 0x020000, CRC(7b1f3229) SHA1(13d0905291e748973d7d17eb404a286ffb94de03) )
	/* 0x040000 - 0x100000 Empty */
	ROM_LOAD16_BYTE( "epr-15200.34", 0x100000, 0x020000, CRC(0a0692e5) SHA1(d4ecc5b1791a91e3b33a5d4d0dd305f1623483d9) )
	ROM_LOAD16_BYTE( "epr-15199.33", 0x100001, 0x020000, CRC(353109b8) SHA1(92440987add3124b758e7eaa77a3a6f54ca61bb8) )

	ROM_REGION( 0x020000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr-15196.4", 0x000000, 0x020000, CRC(79112b3b) SHA1(fc3a202e1e2ff39950d4af689b7fcca86c301805) )
ROM_END

ROM_START( puyoj ) /* Puyo Puyo  (c)1992 Sega / Compile */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr15036", 0x000000, 0x020000, CRC(5310ca1b) SHA1(dcfe2bf7476b640dfb790e8716e75b483d535e48) )
	ROM_LOAD16_BYTE( "epr15035", 0x000001, 0x020000, CRC(bc62e400) SHA1(12bb6031574838a28889f6edb31dbb689265287c) )
	/* 0x040000 - 0x100000 Empty */
	ROM_LOAD16_BYTE( "epr15038", 0x100000, 0x020000, CRC(3b9eea0c) SHA1(e3e6148c1769834cc0061932eb035daa79673144) )
	ROM_LOAD16_BYTE( "epr15037", 0x100001, 0x020000, CRC(be2f7974) SHA1(77027ced7a62f94e9fc6e8a0a4ac0c62f7ea813b) )

	ROM_REGION( 0x020000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr15034", 0x000000, 0x020000, CRC(5688213b) SHA1(f3f234e482871ca903a782e51008f3bfed04ee63) )
ROM_END

ROM_START( puyoja ) /* Puyo Puyo (Rev A)  (c)1992 Sega / Compile */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "ep15036a.32", 0x000000, 0x020000, CRC(61b35257) SHA1(e09a7e992999befc88fc7928a478d1e2d14d7b08) )
	ROM_LOAD16_BYTE( "ep15035a.31", 0x000001, 0x020000, CRC(dfebb6d9) SHA1(6f685729ef4660c2eba409c5236c6d2f313eef5b) )
	/* 0x040000 - 0x100000 Empty */
	ROM_LOAD16_BYTE( "epr15038",    0x100000, 0x020000, CRC(3b9eea0c) SHA1(e3e6148c1769834cc0061932eb035daa79673144) )
	ROM_LOAD16_BYTE( "epr15037",    0x100001, 0x020000, CRC(be2f7974) SHA1(77027ced7a62f94e9fc6e8a0a4ac0c62f7ea813b) )

	ROM_REGION( 0x020000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr15034", 0x000000, 0x020000, CRC(5688213b) SHA1(f3f234e482871ca903a782e51008f3bfed04ee63) )
ROM_END

ROM_START( puyobl ) /* Puyo Puyo  (c)1992 Sega / Compile  Bootleg */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "puyopuyb.4bo", 0x000000, 0x020000, CRC(89ea4d33) SHA1(bef9d011524e71c072d309f6da3c2ebc38878e0e) )
	ROM_LOAD16_BYTE( "puyopuyb.3bo", 0x000001, 0x020000, CRC(c002e545) SHA1(7a59ac764d60e9955830d9617b0bd122b44e7b2f) )
	/* 0x040000 - 0x100000 Empty */
	ROM_LOAD16_BYTE( "puyopuyb.6bo", 0x100000, 0x020000, CRC(0a0692e5) SHA1(d4ecc5b1791a91e3b33a5d4d0dd305f1623483d9) )
	ROM_LOAD16_BYTE( "puyopuyb.5bo", 0x100001, 0x020000, CRC(353109b8) SHA1(92440987add3124b758e7eaa77a3a6f54ca61bb8) )

	ROM_REGION( 0x020000, REGION_SOUND1, 0 )
	ROM_LOAD( "puyopuyb.abo", 0x000000, 0x020000, CRC(79112b3b) SHA1(fc3a202e1e2ff39950d4af689b7fcca86c301805) )
ROM_END

ROM_START( ichir ) /* Ichident-R (Puzzle & Action 2)  (c)1994 Sega (World) */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "pa2_32.bin", 0x000000, 0x080000, CRC(7ba0c025) SHA1(855e9bb2a20c6f51b26381233c57c26aa96ad1f6) )
	ROM_LOAD16_BYTE( "pa2_31.bin", 0x000001, 0x080000, CRC(5f86e5cc) SHA1(44e201de00dfbf7c66d0e0d40d17b162c6f0625b) )
	ROM_LOAD16_BYTE( "epr16888",   0x100000, 0x080000, CRC(85d73722) SHA1(7ebe81b4d6c89f87f60200a3a8cddb07d581adef) )
	ROM_LOAD16_BYTE( "epr16887",   0x100001, 0x080000, CRC(bc3bbf25) SHA1(e760ad400bc183b38e9787d88c8ac084fbe2ae21) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )
	ROM_LOAD( "pa2_02.bin", 0x000000, 0x080000, CRC(fc7b0da5) SHA1(46770aa7e19b4f8a183be3f433c48ad677b552b1) )
ROM_END

ROM_START( ichirk ) /* Ichident-R (Puzzle & Action 2)  (c)1994 Sega (Korea) */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	/* Again the part numbers are quite strange for the Korean verison */
	ROM_LOAD16_BYTE( "epr_ichi.32", 0x000000, 0x080000, CRC(804dea11) SHA1(40bf8cbd40969a5880df10914252b7f64d5ce8e9) )
	ROM_LOAD16_BYTE( "epr_ichi.31", 0x000001, 0x080000, CRC(92452353) SHA1(d2e1da5b139965611cd8d707d23396b5d4c07d12) )
	ROM_LOAD16_BYTE( "epr16888",   0x100000, 0x080000, CRC(85d73722) SHA1(7ebe81b4d6c89f87f60200a3a8cddb07d581adef) )  // m17235a.34
	ROM_LOAD16_BYTE( "epr16887",   0x100001, 0x080000, CRC(bc3bbf25) SHA1(e760ad400bc183b38e9787d88c8ac084fbe2ae21) )  // m17220a.33

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )
	ROM_LOAD( "pa2_02.bin", 0x000000, 0x080000, CRC(fc7b0da5) SHA1(46770aa7e19b4f8a183be3f433c48ad677b552b1) ) // m17220a.4
ROM_END

ROM_START( ichirj ) /* Ichident-R (Puzzle & Action 2)  (c)1994 Sega (Japan) */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr16886", 0x000000, 0x080000, CRC(38208e28) SHA1(07fc634bdf2d3e25274c9c374b3506dec765114c) )
	ROM_LOAD16_BYTE( "epr16885", 0x000001, 0x080000, CRC(1ce4e837) SHA1(16600600e12e3f35e3da89524f7f51f019b5ad17) )
	ROM_LOAD16_BYTE( "epr16888", 0x100000, 0x080000, CRC(85d73722) SHA1(7ebe81b4d6c89f87f60200a3a8cddb07d581adef) )
	ROM_LOAD16_BYTE( "epr16887", 0x100001, 0x080000, CRC(bc3bbf25) SHA1(e760ad400bc183b38e9787d88c8ac084fbe2ae21) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr16884", 0x000000, 0x080000, CRC(fd9dcdd6) SHA1(b8053a2e68072e7664ffc3c53f983f3ba72a892b) )
ROM_END

ROM_START( stkclmns ) /* Stack Columns  (c)1994 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr16874.32", 0x000000, 0x080000, CRC(d78a871c) SHA1(7efcd5d07b089442be5170a3cf9e09579527252f) )
	ROM_LOAD16_BYTE( "epr16873.31", 0x000001, 0x080000, CRC(1def1da4) SHA1(da534a971b40277b2d58ef22c07ca468250d23ca) )
	ROM_LOAD16_BYTE( "mpr16797.34", 0x100000, 0x080000, CRC(b28e9bd5) SHA1(227eb591d10c9dbc52b35954ebd322e2a4451df2) )
	ROM_LOAD16_BYTE( "mpr16796.33", 0x100001, 0x080000, CRC(ec7de52d) SHA1(85bc48cef15e615ad9059500808d17916c854a87) )

	ROM_REGION( 0x020000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr16793.4", 0x000000, 0x020000, CRC(ebb2d057) SHA1(4a19ee5d71e4aabe7d9b9b968ab5ee4bc6262aad) )
ROM_END

ROM_START( stkclmnj ) /* Stack Columns  (c)1994 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr16795.32", 0x000000, 0x080000, CRC(b478fd02) SHA1(aaf9d9f9f4dc900b4e8ff6f258f26e782e5c3166) )
	ROM_LOAD16_BYTE( "epr16794.31", 0x000001, 0x080000, CRC(6d0e8c56) SHA1(8f98d9fd98a1faa70b173cfd72f15102d11e79ae) )
	ROM_LOAD16_BYTE( "mpr16797.34", 0x100000, 0x080000, CRC(b28e9bd5) SHA1(227eb591d10c9dbc52b35954ebd322e2a4451df2) )
	ROM_LOAD16_BYTE( "mpr16796.33", 0x100001, 0x080000, CRC(ec7de52d) SHA1(85bc48cef15e615ad9059500808d17916c854a87) )

	ROM_REGION( 0x020000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr16793.4", 0x000000, 0x020000, CRC(ebb2d057) SHA1(4a19ee5d71e4aabe7d9b9b968ab5ee4bc6262aad) )
ROM_END


ROM_START( puyopuy2 ) /* Puyo Puyo 2  (c)1994 Compile */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr17241.32", 0x000000, 0x080000, CRC(1cad1149) SHA1(77fb0482fa35e615c0bed65f4d7f4dd89b241f23) )
	ROM_LOAD16_BYTE( "epr17240.31", 0x000001, 0x080000, CRC(beecf96d) SHA1(c2bdad4b6184c11f81f2a5db409cb4ea186205a7) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr17239.4", 0x000000, 0x080000, CRC(020ff6ef) SHA1(6095b8277b47a6fd7a9721f15a70ae5bf6be9b1a) )
ROM_END


ROM_START( potopoto ) /* Poto Poto  (c)1994 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr16662", 0x000000, 0x040000, CRC(bbd305d6) SHA1(1a4f4869fefac188c69bc67df0b625e43a0c3f1f) )
	ROM_LOAD16_BYTE( "epr16661", 0x000001, 0x040000, CRC(5a7d14f4) SHA1(a615b5f481256366db7b1c6302a8dcb69708102b) )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr16660", 0x000000, 0x040000, CRC(8251c61c) SHA1(03eef3aa0bdde2c1d93128648f54fd69278d85dd) )
ROM_END


ROM_START( zunkyou ) /* Zunzunkyou No Yabou  (c)1994 Sega */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr16812.32", 0x000000, 0x080000, CRC(eb088fb0) SHA1(69089a3516ad50f35e81971ef3c33eb3f5d52374) )
	ROM_LOAD16_BYTE( "epr16811.31", 0x000001, 0x080000, CRC(9ac7035b) SHA1(1803ffbadc1213e04646d483e27da1591e22cd06) )
	ROM_LOAD16_BYTE( "epr16814.34", 0x100000, 0x080000, CRC(821b3b77) SHA1(c45c7393a792ce8306a52f83f8ed8f6b0d7c11e9) )
	ROM_LOAD16_BYTE( "epr16813.33", 0x100001, 0x080000, CRC(3cba9e8f) SHA1(208819bc1a205eaab089542afc7a59f69ce5bb81) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr16810.4", 0x000000, 0x080000, CRC(d542f0fe) SHA1(23ea50110dfe1cd9f286a535d15e0c3bcba73b00) )
ROM_END


ROM_START( pclubj ) /* Print Club (c)1995 Atlus */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "epr18171.32", 0x000000, 0x080000, CRC(6c8eb8e2) SHA1(bbd885a83269524215c1d8470544086e3e82c05c) )
	ROM_LOAD16_BYTE( "epr18170.31", 0x000001, 0x080000, CRC(72c631e6) SHA1(77c4ed793db6cb75346998f38a637db64fd258bd) )
	ROM_LOAD16_BYTE( "epr18173.34", 0x100000, 0x080000, CRC(9809dc72) SHA1(6dbe6b7d4e525aa9b6174f8dc5aee12a5e00a009) )
	ROM_LOAD16_BYTE( "epr18172.33", 0x100001, 0x080000, CRC(c61d819b) SHA1(4813ed3161e16099f482e0cf8df3cbe6c01c619c) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr18169.4", 0x000000, 0x080000, CRC(5c00ccfb) SHA1(d043ffa6528bb9b76774c96df4edf8222a1878a4) )
ROM_END


ROM_START( pclubjv2 ) /* Print Club vol.2 (c)1995 Atlus */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "p2jwn.u32", 0x000000, 0x080000, CRC(dfc0f7f1) SHA1(d2399f3ff05006590903f943cd77a9c709b9b5b1) )
	ROM_LOAD16_BYTE( "p2jwn.u31", 0x000001, 0x080000, CRC(6ab4c694) SHA1(d8cfaa1a49e86842079c6e3800a95c5afaf76ab6) )
	ROM_LOAD16_BYTE( "p2jwn.u34", 0x100000, 0x080000, CRC(854fd456) SHA1(eff7413a7acd8ee37cb73bc8dfd4f4ae53c04836) )
	ROM_LOAD16_BYTE( "p2jwn.u33", 0x100001, 0x080000, CRC(64428a69) SHA1(e2c5ead4b35db76fda1db03adcd020bde5ca1dd2) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr18169.4", 0x000000, 0x080000, CRC(5c00ccfb) SHA1(d043ffa6528bb9b76774c96df4edf8222a1878a4) )
ROM_END


ROM_START( pclubjv4 ) /* Print Club vol.4 (c)1996 Atlus */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "p4jsm.u32", 0x000000, 0x080000, CRC(36ff5f80) SHA1(33872aa00c8ca3f54dd7503a44562fbdad92df7d) )
	ROM_LOAD16_BYTE( "p4jsm.u31", 0x000001, 0x080000, CRC(f3c021ad) SHA1(34792d861265b609d5022955eb7d2f471c63dfb8) )
	ROM_LOAD16_BYTE( "p4jsm.u34", 0x100000, 0x080000, CRC(d0fd4b33) SHA1(c272404f09bdb6596740ab150eb158cc22cc9aa6) )
	ROM_LOAD16_BYTE( "p4jsm.u33", 0x100001, 0x080000, CRC(ec667875) SHA1(d235a1d8dfa90e1c638e1f079ce528f61450e1f0) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr18169.4", 0x000000, 0x080000, CRC(5c00ccfb) SHA1(d043ffa6528bb9b76774c96df4edf8222a1878a4) )
ROM_END


ROM_START( pclubjv5 ) /* Print Club vol.5 (c)1996 Atlus */
	ROM_REGION( 0x200000, REGION_CPU1, 0 )
	ROM_LOAD16_BYTE( "p5jat.u32", 0x000000, 0x080000, CRC(72220e69) SHA1(615de759d73469841987fb028eaf5d5598c32553) )
	ROM_LOAD16_BYTE( "p5jat.u31", 0x000001, 0x080000, CRC(06d83fde) SHA1(dc68375ccb16cde7900eb05f702bc15e7e702ea5) )
	ROM_LOAD16_BYTE( "p5jat.u34", 0x100000, 0x080000, CRC(b172ab58) SHA1(47a70bd678f6c4dafe70b83bd3db678cf44de48b) )
	ROM_LOAD16_BYTE( "p5jat.u33", 0x100001, 0x080000, CRC(ba38ec50) SHA1(666fdba56d8a4dab041015c5e8102305b491d293) )

	ROM_REGION( 0x080000, REGION_SOUND1, 0 )
	ROM_LOAD( "epr18169.4", 0x000000, 0x080000, CRC(5c00ccfb) SHA1(d043ffa6528bb9b76774c96df4edf8222a1878a4) )
ROM_END



/******************************************************************************
    Machine Init Functions
*******************************************************************************

All of the Sega C/C2 games apart from Bloxeed used a protection chip.
The games contain various checks which make sure this protection chip is
present and returning the expected values. It appears that different
tables are used for Japanese vs. English variants of some games
(Puzzle & Action 2) but not others (Columns).


The basic operation in the EPM5032 macrocell (that is, without using the expander
products) is:

out = x1 XOR (x2 OR x3 OR x4)

where x1, x2, x3, and x4 are the AND of any number of input bits. Each input can
be inverted. The inputs come either from the external pins or from feedback from
the output pins.

Expander products (64 in total) are the NOT of the AND of any number of input bits
(optionally inverted). It's not clear whether the I/O feedback is available to
the expander or not. Looking at bit 1 of prot_func_tfrceac(), it would seem that
it should be, otherwise I don't see how the formula could be computed.

******************************************************************************/

static void segac2_common_init(int (*func)(int in))
{
	prot_func = func;
	bloxeed_sound = 0;
}



/* 317-0149 */
static int prot_func_columns(int in)
{
	int const b0 = BIT( in,2) ^ ((BIT(~in,0) && BIT( in,7)) || (BIT( in,4) && BIT( in,6)));
	int const b1 = BIT(~in,0) ^ (BIT( in,2) || (BIT( in,5) && BIT(~in,6) && BIT( in,7)));
	int const b2 = BIT( in,3) ^ ((BIT( in,0) && BIT( in,1)) || (BIT( in,4) && BIT( in,6)));
	int const b3 = BIT( in,1) ^ ((BIT( in,0) && BIT( in,1)) || (BIT( in,4) && BIT( in,5)) || (BIT(~in,6) && BIT( in,7)));	// 1 repeated

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0160 */
static int prot_func_columns2(int in)
{
	int const b0 =  BIT( in,2) ^ (BIT( in,1) || (BIT( in,4) && BIT( in,5)));
	int const b1 = (BIT( in,0) && BIT( in,3) && BIT( in,4)) ^ (BIT( in,6) || (BIT( in,5) && BIT( in,7)));
	int const b2 = (BIT( in,3) && BIT(~in,2) && BIT( in,4)) ^ (BIT( in,5) || (BIT( in,0) && BIT( in,1)) || (BIT( in,4) && BIT( in,6)));	// 4 repeated
	int const b3 = (BIT( in,1) && BIT( in,0) && BIT( in,2)) ^ ((BIT( in,4) && BIT(~in,6)) || (BIT( in,6) && BIT( in,7)));	// 6 repeated

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0172 */
static int prot_func_tfrceac(int in)
{
	int const b0 = BIT(~in,2) ^ ((BIT( in,0) && BIT(~in,7)) || (BIT( in,3) && BIT( in,4)));
	int const b1 = (BIT( in,4) && BIT(~in,5) && BIT( in,7)) ^ ((BIT(~in,0) || BIT(~in,3)) && (BIT(~in,6) || BIT(~in,7)));	// not in the form x1 XOR (x2 OR x3 OR x4)
	int const b2 = BIT( in,2) ^ ((BIT( in,4) && BIT(~in,5) && BIT( in,7)) || (BIT(~in,1) && BIT( in,6)));
	int const b3 = BIT( in,0) ^ ((BIT( in,1) && BIT( in,4) && BIT( in,6)) || (BIT( in,1) && BIT( in,4) && BIT( in,7)));	// 1,4 repeated

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0173 */
static int prot_func_borench(int in)
{
	int const b0 = (BIT( in,1) && BIT( in,2) && BIT( in,3) && BIT( in,7))   ^ (BIT( in,5) || (BIT(~in,0) && BIT(~in,4)));
	int const b1 = (BIT(~in,2) && BIT( in,3) && BIT( in,5))                 ^ (BIT( in,1) || (BIT( in,0) && BIT(~in,4)));
	int const b2 = (BIT( in,1) && BIT(~in,4) && BIT(~in,6))                 ^ (BIT( in,2) || BIT( in,3) || (BIT( in,5) && BIT( in,7)));
	int const b3 = (BIT(~in,0) && BIT( in,5) && (BIT( in,6) || BIT( in,7))) ^ (BIT( in,1) || (BIT( in,3) && BIT( in,4)));	// not in the form x1 XOR (x2 OR x3 OR x4)

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0178 */
static int prot_func_ribbit(int in)
{
	int const b0 = (BIT( in,0) && BIT( in,4)) ^ ((BIT( in,1) && BIT( in,2)) || BIT( in,3) || BIT(~in,5));
	int const b1 = (BIT( in,1) && BIT( in,5)) ^ ((BIT( in,2) && BIT( in,3)) || BIT( in,0) || BIT(~in,6));
	int const b2 = (BIT( in,2) && BIT( in,7)) ^ ((BIT( in,3) && BIT( in,0)) || BIT(~in,1) || BIT( in,7));
	int const b3 = (BIT( in,3) && BIT( in,6)) ^ ((BIT( in,0) && BIT( in,1)) || BIT(~in,2) || BIT( in,4));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0193 */
static int prot_func_twinsqua(int in)
{
	int const b0 = (BIT( in,2) && BIT(~in,5)) ^ (BIT( in,3) || BIT(~in,4));
	int const b1 = (BIT( in,0) && BIT(~in,2) && BIT( in,4)) ^ (BIT(~in,0) || BIT(~in,4) || BIT(~in,6));	// 0,4 repeated
	int const b2 = (BIT( in,3) && BIT(~in,5)) ^ (BIT( in,4) && BIT( in,7));
	int const b3 =  BIT( in,1) ^ ((BIT(~in,3) && BIT(~in,6)) || (BIT( in,4) && BIT(~in,6)) || (BIT(~in,1) && BIT( in,3) && BIT(~in,4)));	// 1,3,4,6 repeated

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0203 */
static int prot_func_puyo(int in)
{
	int const b0 = (BIT(~in,3) && BIT( in,7)) ^ ((BIT(~in,0) && BIT(~in,1)) || (BIT(~in,1) && BIT(~in,4)));	// 1 repeated
	int const b1 = (BIT( in,3) && BIT( in,5)) ^ (BIT(~in,2) || BIT( in,4) || BIT( in,6));
	int const b2 = (BIT(~in,2) && BIT(~in,5)) ^ (BIT( in,1) || BIT(~in,3) || BIT(~in,6));
	int const b3 =  BIT( in,1)                ^ ((BIT( in,0) && BIT( in,3) && BIT( in,7)) || BIT( in,4));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0211 */
static int prot_func_tantr(int in)
{
	int const b0 = (BIT( in,0) && BIT( in,4)) ^ ( BIT( in,5) || BIT(~in,6)  || (BIT(~in,3) && BIT( in,7)));
	int const b1 = (BIT( in,2) && BIT( in,6)) ^ ((BIT( in,1) && BIT( in,5)) || (BIT( in,3) && BIT( in,4)));
	int const b2 = (BIT(~in,0) && BIT( in,2)) ^ ( BIT( in,4) || BIT( in,7)  || (BIT( in,1) && BIT(~in,5)));
	int const b3 = (BIT(~in,2) && BIT( in,7)) ^ ( BIT(~in,0) || BIT( in,1)  || (BIT( in,3) && BIT( in,6)));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-???? */
static int prot_func_tantrkor(int in)
{
	int const b0 = (BIT(~in,1) && BIT(~in,7)) ^ (BIT(~in,2) && BIT(~in,4));
	int const b1 = (BIT( in,2) && BIT( in,6)) ^ (BIT( in,0) && BIT( in,1));
	int const b2 = (BIT(~in,3) && BIT(~in,6)) ^ (BIT( in,1) || BIT(~in,4));
	int const b3 = (BIT(~in,0) && BIT(~in,2)) ^ (BIT( in,5) && BIT(~in,6));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0218 */
static int prot_func_potopoto(int in)
{
	int const b0 = (BIT(~in,2) && BIT(~in,4)) ^ (BIT(~in,1) && BIT( in,3));
	int const b1 = (BIT( in,0) && BIT( in,5)) ^ (BIT( in,2) || BIT(~in,7));
	int const b2 = (BIT( in,0) && BIT( in,6)) ^ (BIT(~in,1) && BIT( in,7));
	int const b3 = (BIT( in,0) && BIT(~in,7)) ^ (BIT(~in,1) && BIT(~in,6));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0219 */
static int prot_func_stkclmnj(int in)
{
	int const b0 = (BIT( in,1) && BIT( in,4)) ^ (BIT( in,5) && BIT( in,2));
	int const b1 = (BIT(~in,2) && BIT( in,6)) ^ (BIT(~in,5) && BIT( in,7));
	int const b2 = (BIT(~in,3) && BIT( in,6)) ^ (BIT(~in,5) && BIT(~in,1));
	int const b3 = (BIT(~in,3) && BIT( in,5)) ^ (BIT(~in,6) || BIT(~in,7));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0223 */
static int prot_func_stkclmns(int in)
{
	int const b0 = (BIT( in,2) && BIT( in,4)) ^ (BIT( in,1) || BIT(~in,3));
	int const b1 = (BIT( in,0) && BIT( in,5)) ^ (BIT( in,2) && BIT( in,7));
	int const b2 = (BIT( in,0) && BIT(~in,6)) ^ (BIT( in,1) && BIT(~in,7));
	int const b3 = (BIT( in,0) && BIT(~in,7)) ^ (BIT(~in,1) && BIT( in,6));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0224 */
static int prot_func_ichirj(int in)
{
	int const b0 = (BIT( in,2) && BIT( in,4)) ^ (BIT(~in,5) && BIT(~in,2));
	int const b1 = (BIT( in,2) && BIT(~in,6)) ^ (BIT( in,5) && BIT( in,7));
	int const b2 = (BIT(~in,3) && BIT( in,6)) ^ (BIT(~in,5) && BIT(~in,1));
	int const b3 = (BIT(~in,1) && BIT( in,5)) ^ (BIT(~in,5) && BIT( in,7));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-???? */
static int prot_func_ichir(int in)
{
	int const b0 = (BIT(~in,2) && BIT( in,4)) ^ (BIT( in,5) && BIT(~in,2));
	int const b1 = (BIT( in,1) && BIT( in,6)) ^ (BIT( in,5) || BIT( in,7));
	int const b2 = (BIT(~in,3) && BIT( in,6)) ^ (BIT(~in,5) && BIT(~in,3));
	int const b3 = (BIT( in,0) && BIT(~in,5)) ^ (BIT( in,5) && BIT( in,7));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-???? */
static int prot_func_ichirk(int in)
{
	int const b0 = (BIT(~in,2) && BIT( in,4)) ^ (BIT( in,5) && BIT(~in,1));
	int const b1 = (BIT( in,0) && BIT( in,6)) ^ (BIT( in,5) && BIT( in,4));
	int const b2 = (BIT(~in,1) && BIT(~in,6)) ^ (BIT(~in,5) && BIT( in,3));
	int const b3 = (BIT( in,1) && BIT( in,5)) ^ (BIT( in,6) && BIT( in,7));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

/* 317-0228 */
static int prot_func_puyopuy2(int in)
{
	int const b0 = (BIT(~in,0) && BIT(~in,7)) ^ (BIT( in,1) || BIT(~in,4) || BIT(~in,6));
	int const b1 = (BIT( in,0) && BIT(~in,6)) ^ (BIT( in,3) && BIT( in,5));
	int const b2 = (BIT(~in,4) && BIT(~in,7)) ^ (BIT( in,0) || BIT(~in,6));
	int const b3 = (BIT(~in,1) && BIT( in,4)) ^ (BIT( in,2) && BIT(~in,3));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

static int prot_func_zunkyou(int in)
{
	int const b0 = (BIT(~in,1) && BIT( in,6)) ^ (BIT(~in,5) && BIT( in,7));
	int const b1 = (BIT( in,0) && BIT(~in,5)) ^ (BIT(~in,3) || BIT( in,4));
	int const b2 = (BIT( in,2) && BIT(~in,3)) ^ (BIT( in,4) && BIT(~in,5));
	int const b3 = (BIT( in,0) && BIT(~in,4)) ^ (BIT(~in,2) && BIT(~in,6));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

static int prot_func_pclub(int in)
{
	return 0xf;
}

static int prot_func_pclubjv2(int in)
{
	int const b0 = (BIT( in,3) && BIT(~in,4)) ^ ((BIT(~in,1) && BIT(~in,7)) || BIT( in,6));
	int const b1 = (BIT( in,0) && BIT( in,5)) ^  (BIT( in,2) && BIT(~in,6));
	int const b2 = (BIT(~in,1) && BIT( in,6)) ^  (BIT( in,3) || BIT(~in,5)  || BIT(~in,1));	// 1 repeated
	int const b3 = (BIT(~in,2) && BIT(~in,7)) ^  (BIT(~in,0) || BIT(~in,4));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

static int prot_func_pclubjv4(int in)
{
	int const b0 = (BIT(~in,2) && BIT( in,4)) ^ (BIT( in,1) && BIT(~in,6) && BIT(~in,3));
	int const b1 = (BIT(~in,3) && BIT(~in,4)) ^ (BIT( in,0) && BIT( in,5) && BIT(~in,6));
	int const b2 =  BIT(~in,0)                ^ (BIT( in,3) && BIT( in,4));
	int const b3 = (BIT(~in,1) && BIT( in,7)) ^ (BIT( in,5) && BIT(~in,7));	// 7 repeated

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}

static int prot_func_pclubjv5(int in)
{
	int const b0 = (BIT(~in,1) && BIT( in,5)) ^ (BIT(~in,2) && BIT(~in,6));
	int const b1 = (BIT(~in,0) && BIT( in,4)) ^ (BIT(~in,3) && BIT(~in,7));
	int const b2 = (BIT(~in,3) && BIT( in,7)) ^ (BIT(~in,0) || BIT(~in,4));
	int const b3 = (BIT(~in,2) && BIT( in,6)) ^ (BIT(~in,1) && BIT(~in,5));

	return (b3 << 3) | (b2 << 2) | (b1 << 1) | b0;
}



static DRIVER_INIT( c2boot )
{
	segac2_common_init(NULL);
}

static DRIVER_INIT( bloxeedc )
{
	segac2_common_init(NULL);
	bloxeed_sound = 1;
}

static DRIVER_INIT( columns )
{
	segac2_common_init(prot_func_columns);
}

static DRIVER_INIT( columns2 )
{
	segac2_common_init(prot_func_columns2);
}

static DRIVER_INIT( tfrceac )
{
	segac2_common_init(prot_func_tfrceac);
}

static DRIVER_INIT( tfrceacb )
{
	/* disable the palette bank switching from the protection chip */
	segac2_common_init(NULL);
	memory_install_write16_handler(0, ADDRESS_SPACE_PROGRAM, 0x800000, 0x800001, 0, 0, MWA16_NOP);
}

static DRIVER_INIT( borench )
{
	segac2_common_init(prot_func_borench);
}

static DRIVER_INIT( twinsqua )
{
	segac2_common_init(prot_func_twinsqua);
}

static DRIVER_INIT( ribbit )
{
	segac2_common_init(prot_func_ribbit);
}

static DRIVER_INIT( puyo )
{
	segac2_common_init(prot_func_puyo);
}

static DRIVER_INIT( tantr )
{
	segac2_common_init(prot_func_tantr);
}

static DRIVER_INIT( tantrkor )
{
	segac2_common_init(prot_func_tantrkor);
}

static DRIVER_INIT( potopoto )
{
	segac2_common_init(prot_func_potopoto);
}

static DRIVER_INIT( stkclmns )
{
	segac2_common_init(prot_func_stkclmns);
}

static DRIVER_INIT( stkclmnj )
{
	segac2_common_init(prot_func_stkclmnj);
}

static DRIVER_INIT( ichir )
{
	segac2_common_init(prot_func_ichir);
}

static DRIVER_INIT( ichirk )
{
	segac2_common_init(prot_func_ichirk);
}

static DRIVER_INIT( ichirj )
{
	segac2_common_init(prot_func_ichirj);
}

static DRIVER_INIT( ichirjbl )
{
	/* when did this actually work? - the protection is patched but the new check fails? */
	UINT16 *rom = (UINT16 *)memory_region(REGION_CPU1);
	rom[0x390/2] = 0x6600;
}

static DRIVER_INIT( puyopuy2 )
{
	segac2_common_init(prot_func_puyopuy2);
}

static DRIVER_INIT( zunkyou )
{
	segac2_common_init(prot_func_zunkyou);
}


static DRIVER_INIT( pclub )
{
	segac2_common_init(prot_func_pclub);

	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880120, 0x880121, 0, 0, printer_r );/*Print Club Vol.1*/
	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880124, 0x880125, 0, 0, printer_r );/*Print Club Vol.2*/
	memory_install_write16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880124, 0x880125, 0, 0, print_club_camera_w);
}

static DRIVER_INIT( pclubjv2 )
{
	segac2_common_init(prot_func_pclubjv2);

	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880120, 0x880121, 0, 0, printer_r );/*Print Club Vol.1*/
	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880124, 0x880125, 0, 0, printer_r );/*Print Club Vol.2*/
	memory_install_write16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880124, 0x880125, 0, 0, print_club_camera_w);
}

static DRIVER_INIT( pclubjv4 )
{
	segac2_common_init(prot_func_pclubjv4);

	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880120, 0x880121, 0, 0, printer_r );/*Print Club Vol.1*/
	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880124, 0x880125, 0, 0, printer_r );/*Print Club Vol.2*/
	memory_install_write16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880124, 0x880125, 0, 0, print_club_camera_w);
}

static DRIVER_INIT( pclubjv5 )
{
	segac2_common_init(prot_func_pclubjv5);

	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880120, 0x880121, 0, 0, printer_r );/*Print Club Vol.1*/
	memory_install_read16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880124, 0x880125, 0, 0, printer_r );/*Print Club Vol.2*/
	memory_install_write16_handler(0, ADDRESS_SPACE_PROGRAM, 0x880124, 0x880125, 0, 0, print_club_camera_w);
}



/******************************************************************************
    Game Drivers
*******************************************************************************

    These cover all the above games.

    Dates are all verified correct from Ingame display, some of the Titles
    such as Ichidant-R, Tant-R might be slightly incorrect as I've seen the
    games refered to by other names such as Ichident-R, Tanto-R, Tanto Arle
    etc.

    bloxeedc is set as as clone of bloxeed as it is the same game but running
    on a different piece of hardware.  The parent 'bloxeed' is a system18 game
    and does not currently work due to it being encrypted.

******************************************************************************/

/* System C Games */
GAME( 1989, bloxeedc, bloxeed,  segac,    bloxeedc, bloxeedc, ROT0, "Sega / Elorg",           "Bloxeed (World, C System)", 0 )
GAME( 1989, bloxeedu, bloxeed,  segac,    bloxeedc, bloxeedc, ROT0, "Sega / Elorg",           "Bloxeed (US, C System)", 0 )
GAME( 1990, columns,  0,        segac,    columns,  columns,  ROT0, "Sega",                   "Columns (World)", 0 )
GAME( 1990, columnsu, columns,  segac,    columnsu, columns,  ROT0, "Sega",                   "Columns (US, cocktail)", 0 ) // has cocktail mode dsw
GAME( 1990, columnsj, columns,  segac,    columns,  columns,  ROT0, "Sega",                   "Columns (Japan)", 0 )
GAME( 1990, columns2, 0,        segac,    columns2, columns2, ROT0, "Sega",                   "Columns II: The Voyage Through Time (World)", 0 )
GAME( 1990, column2j, columns2, segac,    columns2, columns2, ROT0, "Sega",                   "Columns II: The Voyage Through Time (Japan)", 0 )

/* System C-2 Games */
GAME( 1990, tfrceac,  0,        segac2,   tfrceac,  tfrceac,  ROT0, "Sega / Technosoft",      "ThunderForce AC", 0 )
GAME( 1990, tfrceacj, tfrceac,  segac2,   tfrceac,  tfrceac,  ROT0, "Sega / Technosoft",      "ThunderForce AC (Japan)", 0 )
GAME( 1990, tfrceacb, tfrceac,  segac2,   tfrceac,  tfrceacb, ROT0, "bootleg",                "ThunderForce AC (bootleg)", 0 )
GAME( 1990, borench,  0,        segac2,   borench,  borench,  ROT0, "Sega",                   "Borench", 0 )
GAME( 1991, twinsqua, 0,        segac2,   twinsqua, twinsqua, ROT0, "Sega",                   "Twin Squash", 0 )
GAME( 1991, ribbit,   0,        segac2,   ribbit,   ribbit,   ROT0, "Sega",                   "Ribbit!", 0 )
GAME( 1992, puyo,     0,        segac2,   puyo,     puyo,     ROT0, "Sega / Compile",         "Puyo Puyo (World)", 0 )
GAME( 1992, puyobl,   puyo,     segac2,   puyo,     puyo,     ROT0, "bootleg",                "Puyo Puyo (World, bootleg)", 0 )
GAME( 1992, puyoj,    puyo,     segac2,   puyo,     puyo,     ROT0, "Sega / Compile",         "Puyo Puyo (Japan)", 0 )
GAME( 1992, puyoja,   puyo,     segac2,   puyo,     puyo,     ROT0, "Sega / Compile",         "Puyo Puyo (Japan, Rev A)", 0 )
GAME( 1992, tantr,    0,        segac2,   ichir,    tantr,    ROT0, "Sega",                   "Puzzle & Action: Tant-R (Japan)", 0 )
GAME( 1993, tantrkor, tantr,    segac2,   ichir,    tantrkor, ROT0, "Sega",                   "Puzzle & Action: Tant-R (Korea)", 0 )
GAME( 1992, tantrbl,  tantr,    segac2,   ichir,    c2boot,   ROT0, "bootleg",                "Puzzle & Action: Tant-R (Japan) (bootleg set 1)", 0 )
GAME( 1994, tantrbl2, tantr,    segac,    ichir,    tantr,    ROT0, "bootleg",                "Puzzle & Action: Tant-R (Japan) (bootleg set 2)", 0 ) // Common bootleg in Europe, C board, no samples
GAME( 1994, tantrbl3, tantr,    segac,    ichir,    tantr,    ROT0, "bootleg",                "Puzzle & Action: Tant-R (Japan) (bootleg set 3)", 0 ) // Common bootleg in Europe, C board, no samples
GAME( 1994, potopoto, 0,        segac2,   potopoto, potopoto, ROT0, "Sega",                   "Poto Poto (Japan)", 0 )
GAME( 1994, stkclmns, 0,        segac2,   stkclmns, stkclmns, ROT0, "Sega",                   "Stack Columns (World)", 0 )
GAME( 1994, stkclmnj, stkclmns, segac2,   stkclmns, stkclmnj, ROT0, "Sega",                   "Stack Columns (Japan)", 0 )
GAME( 1994, ichir,    0,        segac2,   ichir,    ichir,    ROT0, "Sega",                   "Puzzle & Action: Ichidant-R (World)", 0 )
GAME( 1994, ichirk,   ichir,    segac2,   ichir,    ichirk,   ROT0, "Sega",                   "Puzzle & Action: Ichidant-R (Korea)", 0 )
GAME( 1994, ichirj,   ichir,    segac2,   ichir,    ichirj,   ROT0, "Sega",                   "Puzzle & Action: Ichidant-R (Japan)", 0 )
GAME( 1994, ichirjbl, ichir,    segac,    ichir,    ichirjbl, ROT0, "bootleg",                "Puzzle & Action: Ichidant-R (Japan) (bootleg)", 0 ) // C board, no samples
GAME( 1994, puyopuy2, 0,        segac2,   puyopuy2, puyopuy2, ROT0, "Compile (Sega license)", "Puyo Puyo 2 (Japan)", 0 )
GAME( 1994, zunkyou,  0,        segac2,   zunkyou,  zunkyou,  ROT0, "Sega",                   "Zunzunkyou No Yabou (Japan)", 0 )

/* Atlus Print Club 'Games' (C-2 Hardware, might not be possible to support them because they use camera + printer, really just put here for reference) */
GAME( 1995, pclubj,   0,        segac2,   pclub,    pclub,    ROT0, "Atlus",                  "Print Club (Japan Vol.1)", GAME_NOT_WORKING )
GAME( 1995, pclubjv2, pclubj,   segac2,   pclubjv2, pclubjv2, ROT0, "Atlus",                  "Print Club (Japan Vol.2)", GAME_NOT_WORKING )
GAME( 1996, pclubjv4, pclubj,   segac2,   pclubjv2, pclubjv4, ROT0, "Atlus",                  "Print Club (Japan Vol.4)", GAME_NOT_WORKING )
GAME( 1996, pclubjv5, pclubj,   segac2,   pclubjv2, pclubjv5, ROT0, "Atlus",                  "Print Club (Japan Vol.5)", GAME_NOT_WORKING )
