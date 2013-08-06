/***************************************************************************

  a7800.c

  Driver file to handle emulation of the Atari 7800.

  Dan Boris

    2002/05/13 kubecj   added more banks for bankswitching
                            added PAL machine description
                            changed clock to be precise

    2012/10/25 Robert Tuccitto  NTSC Color Generator utilized for
                color palette with hue shift/start
                based on observation of several
                systems across multiple displays

    2012/11/09 Robert Tuccitto  Fixed 3 degree hue begin point
                miscalculation of color palette

    2012/12/05 Robert Tuccitto  Implemented proper IRE and phase
               value to the palette

    2012/12/14 Robert Tuccitto  Adjusted colorburst/tint/hue of entire
               palette to closer reflect default hardware configuration
               setting of ~180 degrees.  Palette settings now correspond
               documented and calculated settings as follows:

               Contrast = 0.0526 --> 0.05
               Brightness = 0.0 --> 0.00
               Color = 0.2162 --> 0.22
               Phase = 25.714 --> 25.7
               Colorburst/Hue = 180 degrees

    2013/02/27 Robert Tuccitto  Palette rebuild due to misaligned
               data references.  Corrected PAL color sequence order.

    2013/03/19 Robert Tuccitto  Stripped palette to raw video output
               values removing YIQ/YUV infer data.

    2013/04/02 Robert Tuccitto  Corrected rotation values and errors
               including duplicate entries for palette.

    2013/04/07 Robert Tuccitto  Address map locations for the XBOARD
               added.

    2013/05/01 Robert Tuccitto  Red and Blue miscalculated proportions
               fixed.

    2013/08/04 Robert Tuccitto  Green miscalculated proportions fixed.

***************************************************************************/

#include "emu.h"
#include "cpu/m6502/m6502.h"
#include "sound/pokey.h"
#include "sound/tiaintf.h"
#include "imagedev/cartslot.h"
#include "machine/6532riot.h"
#include "includes/a7800.h"


#define A7800_NTSC_Y1   XTAL_14_31818MHz
#define CLK_PAL 1773447


/***************************************************************************
    ADDRESS MAPS
***************************************************************************/

static ADDRESS_MAP_START( a7800_mem, AS_PROGRAM, 8, a7800_state )
	AM_RANGE(0x0000, 0x001f) AM_MIRROR(0x300) AM_READWRITE(a7800_TIA_r, a7800_TIA_w)
	AM_RANGE(0x0020, 0x003f) AM_MIRROR(0x300) AM_READWRITE(a7800_MARIA_r, a7800_MARIA_w)
	AM_RANGE(0x0040, 0x00ff) AM_READ_BANK("bank5") AM_WRITE(a7800_RAM0_w)   /* RAM (6116 block 0) */
	AM_RANGE(0x0140, 0x01ff) AM_RAMBANK("bank6")    /* RAM (6116 block 1) */
	AM_RANGE(0x0280, 0x02ff) AM_DEVREADWRITE("riot", riot6532_device, read, write)
	AM_RANGE(0x0450, 0x045f) /*XBOARD POKEY1*/
	AM_RANGE(0x0460, 0x046f) /*XBOARD POKEY2*/
	AM_RANGE(0x0470, 0x047f) /*XBOARD CTRL*/
	AM_RANGE(0x0480, 0x04ff) AM_MIRROR(0x100) AM_RAM    /* RIOT RAM */
	AM_RANGE(0x1800, 0x27ff) AM_RAM
	AM_RANGE(0x2800, 0x2fff) AM_RAMBANK("bank7")    /* MAINRAM */
	AM_RANGE(0x3000, 0x37ff) AM_RAMBANK("bank7")    /* MAINRAM */
	AM_RANGE(0x3800, 0x3fff) AM_RAMBANK("bank7")    /* MAINRAM */
	AM_RANGE(0x4000, 0x7fff) AM_ROMBANK("bank1")                        /* f18 hornet */
	AM_RANGE(0x8000, 0x9fff) AM_ROMBANK("bank2")                        /* sc */
	AM_RANGE(0xa000, 0xbfff) AM_ROMBANK("bank3")                        /* sc + ac */
	AM_RANGE(0xc000, 0xdfff) AM_ROMBANK("bank4")                        /* ac */
	AM_RANGE(0xe000, 0xffff) AM_ROM
	AM_RANGE(0x4000, 0xffff) AM_WRITE(a7800_cart_w) /* XBOARD SRAM */
ADDRESS_MAP_END


/***************************************************************************
    INPUT PORTS
***************************************************************************/

static INPUT_PORTS_START( a7800 )
	PORT_START("joysticks")            /* IN0 */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP)    PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN)  PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT)  PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_PLAYER(2) PORT_8WAY
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP)    PORT_PLAYER(1) PORT_8WAY
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN)  PORT_PLAYER(1) PORT_8WAY
	PORT_BIT(0x40, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT)  PORT_PLAYER(1) PORT_8WAY
	PORT_BIT(0x80, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT) PORT_PLAYER(1) PORT_8WAY

	PORT_START("buttons")              /* IN1 */
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_BUTTON1)       PORT_PLAYER(2)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_BUTTON1)       PORT_PLAYER(1)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_BUTTON2)       PORT_PLAYER(2)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_BUTTON2)       PORT_PLAYER(1)
	PORT_BIT(0xF0, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("vblank")               /* IN2 */
	PORT_BIT(0x7F, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_CUSTOM) PORT_VBLANK("screen")

	PORT_START("console_buttons")      /* IN3 */
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER)  PORT_NAME("Reset")         PORT_CODE(KEYCODE_R)
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER)  PORT_NAME("Select")        PORT_CODE(KEYCODE_S)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER)  PORT_NAME(DEF_STR(Pause))  PORT_CODE(KEYCODE_O)
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_DIPNAME(0x40, 0x40, "Left Difficulty Switch")
	PORT_DIPSETTING(0x40, "A" )
	PORT_DIPSETTING(0x00, "B" )
	PORT_DIPNAME(0x80, 0x80, "Right Difficulty Switch")
	PORT_DIPSETTING(0x80, "A" )
	PORT_DIPSETTING(0x00, "B" )
INPUT_PORTS_END


/***************************************************************************
    PALETTE
***************************************************************************/

#define NTSC_GREY \
	MAKE_RGB(0x00,0x00,0x00), MAKE_RGB(0x12,0x12,0x12), MAKE_RGB(0x24,0x24,0x24), MAKE_RGB(0x36,0x36,0x36), \
	MAKE_RGB(0x47,0x47,0x47), MAKE_RGB(0x59,0x59,0x59), MAKE_RGB(0x6B,0x6B,0x6B), MAKE_RGB(0x7D,0x7D,0x7D), \
	MAKE_RGB(0x8F,0x8F,0x8F), MAKE_RGB(0xA1,0xA1,0xA1), MAKE_RGB(0xB2,0xB2,0xB2), MAKE_RGB(0xC4,0xC4,0xC4), \
	MAKE_RGB(0xD6,0xD6,0xD6), MAKE_RGB(0xE8,0xE8,0xE8), MAKE_RGB(0xFA,0xFA,0xFA), MAKE_RGB(0xFF,0xFF,0xFF   )

#define NTSC_GOLD \
	MAKE_RGB(0x1A,0x05,0x00), MAKE_RGB(0x2E,0x15,0x00), MAKE_RGB(0x41,0x26,0x00), MAKE_RGB(0x54,0x36,0x00), \
	MAKE_RGB(0x67,0x46,0x00), MAKE_RGB(0x7A,0x56,0x00), MAKE_RGB(0x8D,0x67,0x00), MAKE_RGB(0x9F,0x78,0x0C), \
	MAKE_RGB(0xB1,0x8A,0x1E), MAKE_RGB(0xC3,0x9D,0x30), MAKE_RGB(0xD5,0xAD,0x41), MAKE_RGB(0xE6,0xBF,0x53), \
	MAKE_RGB(0xF6,0xD2,0x65), MAKE_RGB(0xF6,0xE5,0x7A), MAKE_RGB(0xF7,0xF7,0x92), MAKE_RGB(0xF8,0xF8,0xA4   )

#define NTSC_ORANGE \
	MAKE_RGB(0x31,0x00,0x00), MAKE_RGB(0x43,0x02,0x00), MAKE_RGB(0x56,0x12,0x00), MAKE_RGB(0x69,0x23,0x00), \
	MAKE_RGB(0x7C,0x33,0x00), MAKE_RGB(0x8E,0x44,0x00), MAKE_RGB(0x9F,0x56,0x0A), MAKE_RGB(0xB1,0x67,0x1C), \
	MAKE_RGB(0xC3,0x79,0x2E), MAKE_RGB(0xD4,0x8B,0x40), MAKE_RGB(0xE6,0x9D,0x52), MAKE_RGB(0xF8,0xAF,0x64), \
	MAKE_RGB(0xF9,0xC2,0x79), MAKE_RGB(0xF9,0xD6,0x91), MAKE_RGB(0xFA,0xE9,0xAA), MAKE_RGB(0xFA,0xFA,0xC2   )

#define NTSC_RED_ORANGE \
	MAKE_RGB(0x3E,0x00,0x00), MAKE_RGB(0x50,0x00,0x00), MAKE_RGB(0x61,0x03,0x00), MAKE_RGB(0x71,0x13,0x00), \
	MAKE_RGB(0x83,0x24,0x0A), MAKE_RGB(0x95,0x36,0x1B), MAKE_RGB(0xA7,0x48,0x2D), MAKE_RGB(0xB9,0x5A,0x3F), \
	MAKE_RGB(0xCB,0x6C,0x51), MAKE_RGB(0xDD,0x7E,0x63), MAKE_RGB(0xEE,0x90,0x75), MAKE_RGB(0xFD,0xA1,0x87), \
	MAKE_RGB(0xFD,0xB5,0xA0), MAKE_RGB(0xFE,0xC9,0xB8), MAKE_RGB(0xFE,0xDD,0xD2), MAKE_RGB(0xFE,0xF0,0xEA   )

#define NTSC_PINK \
	MAKE_RGB(0x3C,0x00,0x00), MAKE_RGB(0x4C,0x00,0x0A), MAKE_RGB(0x5B,0x00,0x15), MAKE_RGB(0x6D,0x0C,0x26), \
	MAKE_RGB(0x7F,0x1E,0x38), MAKE_RGB(0x91,0x30,0x4A), MAKE_RGB(0xA3,0x42,0x5C), MAKE_RGB(0xB5,0x54,0x6E), \
	MAKE_RGB(0xC7,0x66,0x80), MAKE_RGB(0xD8,0x77,0x91), MAKE_RGB(0xEA,0x89,0xA3), MAKE_RGB(0xF8,0x9C,0xB7), \
	MAKE_RGB(0xF9,0xAF,0xD0), MAKE_RGB(0xF9,0xC3,0xE8), MAKE_RGB(0xFB,0xD6,0xF0), MAKE_RGB(0xFD,0xE8,0xF6   )

#define NTSC_PURPLE \
	MAKE_RGB(0x2D,0x01,0x32), MAKE_RGB(0x3D,0x01,0x3D), MAKE_RGB(0x4D,0x01,0x49), MAKE_RGB(0x5F,0x0E,0x59), \
	MAKE_RGB(0x71,0x20,0x6A), MAKE_RGB(0x83,0x32,0x7C), MAKE_RGB(0x94,0x44,0x8E), MAKE_RGB(0xA6,0x56,0xA0), \
	MAKE_RGB(0xB8,0x67,0xB2), MAKE_RGB(0xCA,0x79,0xC4), MAKE_RGB(0xDC,0x8B,0xD6), MAKE_RGB(0xEE,0x9D,0xDE), \
	MAKE_RGB(0xF7,0xAE,0xE2), MAKE_RGB(0xF9,0xC0,0xE8), MAKE_RGB(0xFA,0xD3,0xEF), MAKE_RGB(0xFC,0xE4,0xF5   )

#define NTSC_PURPLE_BLUE \
	MAKE_RGB(0x16,0x06,0x5B), MAKE_RGB(0x26,0x06,0x67), MAKE_RGB(0x36,0x06,0x73), MAKE_RGB(0x48,0x18,0x85), \
	MAKE_RGB(0x5A,0x2A,0x96), MAKE_RGB(0x6C,0x3C,0xA8), MAKE_RGB(0x7E,0x4D,0xB9), MAKE_RGB(0x90,0x5F,0xCB), \
	MAKE_RGB(0xA2,0x71,0xDD), MAKE_RGB(0xB3,0x83,0xE9), MAKE_RGB(0xC6,0x93,0xE8), MAKE_RGB(0xDA,0xA4,0xE8), \
	MAKE_RGB(0xED,0xB3,0xE7), MAKE_RGB(0xF9,0xC4,0xEA), MAKE_RGB(0xFB,0xD7,0xF0), MAKE_RGB(0xFD,0xE9,0xF7   )

#define NTSC_BLUE1 \
	MAKE_RGB(0x08,0x08,0x71), MAKE_RGB(0x09,0x09,0x7E), MAKE_RGB(0x1B,0x15,0x8E), MAKE_RGB(0x2D,0x27,0x9F), \
	MAKE_RGB(0x3F,0x39,0xB1), MAKE_RGB(0x50,0x4B,0xC3), MAKE_RGB(0x62,0x5D,0xD5), MAKE_RGB(0x74,0x6F,0xE7), \
	MAKE_RGB(0x86,0x81,0xF7), MAKE_RGB(0x99,0x91,0xF6), MAKE_RGB(0xAD,0xA1,0xF6), MAKE_RGB(0xC0,0xB1,0xF6), \
	MAKE_RGB(0xD2,0xC2,0xF6), MAKE_RGB(0xE6,0xD2,0xF6), MAKE_RGB(0xF9,0xE3,0xF5), MAKE_RGB(0xFE,0xF4,0xFB   )

#define NTSC_BLUE2 \
	MAKE_RGB(0x08,0x08,0x68), MAKE_RGB(0x09,0x14,0x7E), MAKE_RGB(0x09,0x28,0x94), MAKE_RGB(0x14,0x39,0xA6), \
	MAKE_RGB(0x26,0x4B,0xB8), MAKE_RGB(0x38,0x5D,0xCA), MAKE_RGB(0x49,0x70,0xDD), MAKE_RGB(0x5B,0x81,0xEE), \
	MAKE_RGB(0x6D,0x93,0xFF), MAKE_RGB(0x80,0xA3,0xFF), MAKE_RGB(0x92,0xB4,0xFF), MAKE_RGB(0xA6,0xC4,0xFF), \
	MAKE_RGB(0xBA,0xD4,0xFF), MAKE_RGB(0xCB,0xE5,0xFF), MAKE_RGB(0xDF,0xF5,0xFF), MAKE_RGB(0xF2,0xFF,0xFF   )

#define NTSC_LIGHT_BLUE \
	MAKE_RGB(0x05,0x10,0x4E), MAKE_RGB(0x05,0x23,0x66), MAKE_RGB(0x06,0x37,0x7F), MAKE_RGB(0x06,0x4B,0x97), \
	MAKE_RGB(0x13,0x5D,0xA9), MAKE_RGB(0x25,0x6F,0xBB), MAKE_RGB(0x37,0x80,0xCB), MAKE_RGB(0x48,0x92,0xDE), \
	MAKE_RGB(0x5A,0xA4,0xF0), MAKE_RGB(0x6C,0xB6,0xFF), MAKE_RGB(0x7D,0xC7,0xFF), MAKE_RGB(0x91,0xD8,0xFF), \
	MAKE_RGB(0xA3,0xE7,0xFF), MAKE_RGB(0xB7,0xF7,0xFF), MAKE_RGB(0xC9,0xFF,0xFF), MAKE_RGB(0xDB,0xFF,0xFF   )

#define NTSC_TURQUOISE \
	MAKE_RGB(0x01,0x1D,0x28), MAKE_RGB(0x01,0x30,0x3F), MAKE_RGB(0x02,0x43,0x58), MAKE_RGB(0x02,0x57,0x71), \
	MAKE_RGB(0x0B,0x6A,0x85), MAKE_RGB(0x1C,0x7C,0x98), MAKE_RGB(0x2E,0x8E,0xAA), MAKE_RGB(0x41,0xA0,0xBB), \
	MAKE_RGB(0x52,0xB2,0xCD), MAKE_RGB(0x64,0xC3,0xDF), MAKE_RGB(0x76,0xD5,0xF1), MAKE_RGB(0x88,0xE7,0xFF), \
	MAKE_RGB(0x98,0xF8,0xFF), MAKE_RGB(0xAA,0xFF,0xFF), MAKE_RGB(0xBC,0xFF,0xFF), MAKE_RGB(0xCE,0xFF,0xFF   )

#define NTSC_GREEN_BLUE \
	MAKE_RGB(0x03,0x24,0x0D), MAKE_RGB(0x05,0x37,0x13), MAKE_RGB(0x06,0x4A,0x2A), MAKE_RGB(0x07,0x5E,0x41), \
	MAKE_RGB(0x10,0x70,0x56), MAKE_RGB(0x21,0x82,0x69), MAKE_RGB(0x33,0x94,0x7B), MAKE_RGB(0x45,0xA6,0x8D), \
	MAKE_RGB(0x57,0xB8,0x9F), MAKE_RGB(0x69,0xCA,0xB1), MAKE_RGB(0x7B,0xDC,0xC2), MAKE_RGB(0x8C,0xED,0xD3), \
	MAKE_RGB(0x9D,0xFF,0xE6), MAKE_RGB(0xAF,0xFF,0xF3), MAKE_RGB(0xBF,0xFF,0xFE), MAKE_RGB(0xCE,0xFF,0xFF   )

#define NTSC_GREEN \
	MAKE_RGB(0x04,0x27,0x0E), MAKE_RGB(0x06,0x3A,0x15), MAKE_RGB(0x07,0x4C,0x1B), MAKE_RGB(0x0B,0x5E,0x21), \
	MAKE_RGB(0x1E,0x6F,0x25), MAKE_RGB(0x30,0x81,0x37), MAKE_RGB(0x42,0x92,0x49), MAKE_RGB(0x53,0xA4,0x5A), \
	MAKE_RGB(0x65,0xB6,0x6C), MAKE_RGB(0x77,0xC8,0x7E), MAKE_RGB(0x89,0xDA,0x90), MAKE_RGB(0x9B,0xEC,0xA2), \
	MAKE_RGB(0xAD,0xFE,0xB4), MAKE_RGB(0xBD,0xFE,0xBF), MAKE_RGB(0xCD,0xFE,0xCA), MAKE_RGB(0xDD,0xFE,0xD6   )

#define NTSC_YELLOW_GREEN \
	MAKE_RGB(0x03,0x23,0x0C), MAKE_RGB(0x05,0x36,0x13), MAKE_RGB(0x0D,0x47,0x17), MAKE_RGB(0x20,0x57,0x17), \
	MAKE_RGB(0x32,0x67,0x17), MAKE_RGB(0x46,0x78,0x16), MAKE_RGB(0x58,0x89,0x1C), MAKE_RGB(0x6A,0x9B,0x2E), \
	MAKE_RGB(0x7C,0xAD,0x40), MAKE_RGB(0x8E,0xBE,0x52), MAKE_RGB(0x9F,0xD0,0x64), MAKE_RGB(0xB1,0xE2,0x76), \
	MAKE_RGB(0xC3,0xF4,0x88), MAKE_RGB(0xD5,0xF9,0x96), MAKE_RGB(0xE5,0xF9,0xA0), MAKE_RGB(0xF5,0xF9,0xAC   )

#define NTSC_ORANGE_GREEN \
	MAKE_RGB(0x02,0x17,0x08), MAKE_RGB(0x13,0x28,0x09), MAKE_RGB(0x26,0x38,0x09), MAKE_RGB(0x3A,0x49,0x09), \
	MAKE_RGB(0x4C,0x59,0x09), MAKE_RGB(0x60,0x6A,0x09), MAKE_RGB(0x73,0x7A,0x09), MAKE_RGB(0x84,0x8B,0x13), \
	MAKE_RGB(0x96,0x9D,0x25), MAKE_RGB(0xA9,0xAF,0x37), MAKE_RGB(0xBA,0xC1,0x49), MAKE_RGB(0xCD,0xD3,0x5A), \
	MAKE_RGB(0xDF,0xE5,0x6C), MAKE_RGB(0xF0,0xF6,0x7E), MAKE_RGB(0xF6,0xF6,0x8A), MAKE_RGB(0xF7,0xF7,0x9A   )

#define NTSC_LIGHT_ORANGE \
	MAKE_RGB(0x19,0x05,0x00), MAKE_RGB(0x2D,0x15,0x00), MAKE_RGB(0x40,0x26,0x00), MAKE_RGB(0x53,0x36,0x00), \
	MAKE_RGB(0x67,0x47,0x00), MAKE_RGB(0x7A,0x57,0x00), MAKE_RGB(0x8C,0x68,0x00), MAKE_RGB(0x9E,0x79,0x0C), \
	MAKE_RGB(0xB0,0x8B,0x1E), MAKE_RGB(0xC2,0x9D,0x30), MAKE_RGB(0xD4,0xAF,0x42), MAKE_RGB(0xE6,0xC0,0x54), \
	MAKE_RGB(0xF6,0xD3,0x66), MAKE_RGB(0xF6,0xE5,0x79), MAKE_RGB(0xF7,0xF7,0x91), MAKE_RGB(0xF8,0xF8,0xA3   )

static const rgb_t a7800_palette[256*3] =
{
	NTSC_GREY,
	NTSC_GOLD,
	NTSC_ORANGE,
	NTSC_RED_ORANGE,
	NTSC_PINK,
	NTSC_PURPLE,
	NTSC_PURPLE_BLUE,
	NTSC_BLUE1,
	NTSC_BLUE2,
	NTSC_LIGHT_BLUE,
	NTSC_TURQUOISE,
	NTSC_GREEN_BLUE,
	NTSC_GREEN,
	NTSC_YELLOW_GREEN,
	NTSC_ORANGE_GREEN,
	NTSC_LIGHT_ORANGE
};

static const rgb_t a7800p_palette[256*3] =
{
	NTSC_GREY,
	NTSC_ORANGE_GREEN,
	NTSC_GOLD,
	NTSC_ORANGE,
	NTSC_RED_ORANGE,
	NTSC_PINK,
	NTSC_PURPLE,
	NTSC_PURPLE_BLUE,
	NTSC_BLUE1,
	NTSC_BLUE2,
	NTSC_LIGHT_BLUE,
	NTSC_TURQUOISE,
	NTSC_GREEN_BLUE,
	NTSC_GREEN,
	NTSC_YELLOW_GREEN,
	NTSC_ORANGE_GREEN
};


/* Initialise the palette */
void a7800_state::palette_init()
{
	palette_set_colors(machine(), 0, a7800_palette, ARRAY_LENGTH(a7800_palette));
}


PALETTE_INIT_MEMBER(a7800_state,a7800p)
{
	palette_set_colors(machine(), 0, a7800p_palette, ARRAY_LENGTH(a7800p_palette));
}


/***************************************************************************
    MACHINE DRIVERS
***************************************************************************/

static MACHINE_CONFIG_START( a7800_ntsc, a7800_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", M6502, A7800_NTSC_Y1/8) /* 1.79 MHz (switches to 1.19 MHz on TIA or RIOT access) */
	MCFG_CPU_PROGRAM_MAP(a7800_mem)
	MCFG_TIMER_DRIVER_ADD_SCANLINE("scantimer", a7800_state, a7800_interrupt, "screen", 0, 1)


	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(640,262)
	MCFG_SCREEN_VISIBLE_AREA(0,319,25,45+204)
	MCFG_SCREEN_UPDATE_DRIVER(a7800_state, screen_update_a7800)

	MCFG_PALETTE_LENGTH(ARRAY_LENGTH(a7800_palette))


	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_TIA_ADD("tia", 31400)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)
	MCFG_POKEY_ADD("pokey", A7800_NTSC_Y1/8)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)

	/* devices */
	MCFG_RIOT6532_ADD("riot", A7800_NTSC_Y1/12, a7800_r6532_interface)

	MCFG_CARTSLOT_ADD("cart")
	MCFG_CARTSLOT_EXTENSION_LIST("bin,a78")
	MCFG_CARTSLOT_NOT_MANDATORY
	MCFG_CARTSLOT_LOAD(a7800_state,a7800_cart)
	MCFG_CARTSLOT_PARTIALHASH(a7800_partialhash)
	MCFG_CARTSLOT_INTERFACE("a7800_cart")

	/* software lists */
	MCFG_SOFTWARE_LIST_ADD("cart_list","a7800")
	MCFG_SOFTWARE_LIST_FILTER("cart_list","NTSC")
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( a7800_pal, a7800_ntsc )

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK(CLK_PAL)
//  MCFG_TIMER_ADD_SCANLINE("scantimer", a7800_interrupt, "screen", 0, 1)

	MCFG_SCREEN_MODIFY( "screen" )
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_SIZE(640,312)
	MCFG_SCREEN_VISIBLE_AREA(0,319,50,50+225)
	MCFG_PALETTE_INIT_OVERRIDE(a7800_state, a7800p )

	/* sound hardware */
	MCFG_DEVICE_REMOVE("pokey")
	MCFG_POKEY_ADD("pokey", CLK_PAL)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.00)

	/* devices */
	MCFG_DEVICE_REMOVE("riot")
	MCFG_RIOT6532_ADD("riot", 3546894/3, a7800_r6532_interface)

	/* software lists */
	MCFG_DEVICE_REMOVE("cart_list")
	MCFG_SOFTWARE_LIST_ADD("cart_list","a7800")
	MCFG_SOFTWARE_LIST_FILTER("cart_list","PAL")
MACHINE_CONFIG_END


/***************************************************************************
    ROM DEFINITIONS
***************************************************************************/

ROM_START( a7800 )
	ROM_REGION(0x40000, "maincpu", 0)
	ROM_FILL(0x0000, 0x40000, 0xff)
	ROM_SYSTEM_BIOS( 0, "a7800", "Atari 7800" )
	ROMX_LOAD("7800.u7", 0xf000, 0x1000, CRC(5d13730c) SHA1(d9d134bb6b36907c615a594cc7688f7bfcef5b43), ROM_BIOS(1))
	ROM_SYSTEM_BIOS( 1, "a7800pr", "Atari 7800 (prototype with Asteroids)" )
	ROMX_LOAD("c300558-001a.u7", 0xc000, 0x4000, CRC(a0e10edf) SHA1(14584b1eafe9721804782d4b1ac3a4a7313e455f), ROM_BIOS(2))
ROM_END

ROM_START( a7800p )
	ROM_REGION(0x40000, "maincpu", 0)
	ROM_FILL(0x0000, 0x40000, 0xff)
	ROM_LOAD("7800pal.rom", 0xc000, 0x4000, CRC(d5b61170) SHA1(5a140136a16d1d83e4ff32a19409ca376a8df874))
ROM_END


/***************************************************************************
    GAME DRIVERS
***************************************************************************/

/*    YEAR  NAME      PARENT    COMPAT  MACHINE     INPUT     INIT          COMPANY   FULLNAME */
CONS( 1986, a7800,    0,        0,      a7800_ntsc, a7800, a7800_state,    a7800_ntsc,  "Atari",  "Atari 7800 (NTSC)" , 0)
CONS( 1986, a7800p,   a7800,    0,      a7800_pal,  a7800, a7800_state,    a7800_pal,   "Atari",  "Atari 7800 (PAL)" , 0)
