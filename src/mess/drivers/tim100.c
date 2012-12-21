/***************************************************************************

        TIM-100 Terminal
		Mihajlo Pupin Institute

        21/12/2012 Skeleton driver.

****************************************************************************/

#include "emu.h"
#include "cpu/i8085/i8085.h"
#include "machine/i8251.h"
#include "video/i8275.h"

class tim100_state : public driver_device
{
public:
	tim100_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		  m_maincpu(*this, "maincpu") 
		{ }

	required_device<cpu_device> m_maincpu;
	UINT32 screen_update_tim100(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	bitmap_rgb32 m_bitmap;
	virtual void machine_reset();
};

static ADDRESS_MAP_START(tim100_mem, AS_PROGRAM, 8, tim100_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0x1fff) AM_ROM // 2764 at U16	
	AM_RANGE(0x2000, 0x27ff) AM_RAM // 2KB static ram CDM6116A at U15

	AM_RANGE(0xc000, 0xc001) AM_DEVREADWRITE_LEGACY("i8276", i8275_r, i8275_w)

	AM_RANGE(0x6000, 0x6000) AM_DEVREADWRITE("uart_u17", i8251_device, status_r, control_w)
	AM_RANGE(0x6001, 0x6001) AM_DEVREADWRITE("uart_u17", i8251_device, data_r, data_w)

	AM_RANGE(0x8000, 0x8000) AM_DEVREADWRITE("uart_u18", i8251_device, status_r, control_w)
	AM_RANGE(0x8001, 0x8001) AM_DEVREADWRITE("uart_u18", i8251_device, data_r, data_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START(tim100_io, AS_IO, 8, tim100_state)
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
ADDRESS_MAP_END


/* Input ports */
static INPUT_PORTS_START( tim100 )
INPUT_PORTS_END

static const rgb_t tim100_palette[3] = {
	MAKE_RGB(0x00, 0x00, 0x00), // black
	MAKE_RGB(0xa0, 0xa0, 0xa0), // white
	MAKE_RGB(0xff, 0xff, 0xff)	// highlight
};

void tim100_state::machine_reset()
{
	palette_set_colors(machine(), 0, tim100_palette, ARRAY_LENGTH(tim100_palette));
}

const gfx_layout tim100_charlayout =
{
	12, 16,				/* 8x16 characters */
	128,				/* 128 characters */
	1,				/* 1 bits per pixel */
	{0},				/* no bitplanes; 1 bit per pixel */
	{0,1,2,3,4,5,0+0x4000,1+0x4000,2+0x4000,3+0x4000,4+0x4000,5+0x4000},
	{0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8,
	 8 * 8, 9 * 8, 10 * 8, 11 * 8, 12 * 8, 13 * 8, 14 * 8, 15 * 8},
	8*16				/* space between characters */
};

static GFXDECODE_START( tim100 )
	GFXDECODE_ENTRY( "chargen", 0x0000, tim100_charlayout, 0, 1 )
GFXDECODE_END



UINT32 tim100_state::screen_update_tim100(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	device_t *devconf = machine().device("i8276");
	i8275_update( devconf, bitmap, cliprect);
	copybitmap(bitmap, m_bitmap, 0, 0, 0, 0, cliprect);
	return 0;
}


// this gets called via a (i8275_dack_w), so does nothing currently
// once fixed, pixel count needs adjusting
static I8275_DISPLAY_PIXELS(tim100_display_pixels)
{
	tim100_state *state = device->machine().driver_data<tim100_state>();
	int i;
	bitmap_rgb32 &bitmap = state->m_bitmap;
	const rgb_t *palette = palette_entry_list_raw(bitmap.palette());
	UINT8 *charmap = state->memregion("chargen")->base();
	UINT8 pixels = charmap[(linecount & 15) + (charcode << 4)];
	if (vsp)
	{
		pixels = 0;
	}
	
	if (lten)
	{
		pixels = 0xff;
	}

	if (rvv)
	{
		pixels ^= 0xff;
	}

	for(i=0;i<8;i++) // 6 pixels
	{
		bitmap.pix32(y, x + i) = palette[(pixels >> (7-i)) & 1 ? (hlgt ? 2 : 1) : 0];
	}
}

static const i8275_interface tim100_i8276_interface = {
	"screen",
	16, //12
	0,
	DEVCB_CPU_INPUT_LINE("maincpu", I8085_RST65_LINE),
	DEVCB_NULL,	
	tim100_display_pixels
};

static MACHINE_CONFIG_START( tim100, tim100_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",I8085A, XTAL_4_9152MHz) // divider unknown
	MCFG_CPU_PROGRAM_MAP(tim100_mem)
	MCFG_CPU_IO_MAP(tim100_io)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(40*16, 16*16)
	MCFG_SCREEN_VISIBLE_AREA(0, 40*16-1, 0, 16*16-1)
	MCFG_SCREEN_UPDATE_DRIVER(tim100_state, screen_update_tim100)

	MCFG_GFXDECODE( tim100 )

	MCFG_I8275_ADD	( "i8276", tim100_i8276_interface)
	
	MCFG_PALETTE_LENGTH(3)
	
	MCFG_I8251_ADD("uart_u17", default_i8251_interface)
	MCFG_I8251_ADD("uart_u18", default_i8251_interface)
MACHINE_CONFIG_END

/* ROM definition */
ROM_START( tim100 )
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASEFF )
	ROM_LOAD( "tim 100 v.3.2.0.u16",   0x0000, 0x2000, CRC(4de9c8ad) SHA1(b0914d6e8d618e92a87b4b39c35391541251e8cc))  
	ROM_REGION( 0x2000, "chargen", ROMREGION_INVERT )
	ROM_SYSTEM_BIOS( 0, "212", "v 2.1.2" )
	ROMX_LOAD( "tim 100kg v.2.1.2.u12", 0x0000, 0x2000, CRC(faf5743c) SHA1(310b662e9535878210f8aaab3e2b846fade60642),ROM_BIOS(1))	
	ROM_SYSTEM_BIOS( 1, "220", "v 2.2.0" )
	ROMX_LOAD( "tim 100kg v.2.2.0.u12", 0x0000, 0x2000, CRC(358dbbd3) SHA1(14b7d6ee41b19bedf2f070f5b28b03aaff2cac4f),ROM_BIOS(2))
ROM_END

/* Driver */
COMP( 1985, tim100, 0, 0, tim100, tim100, driver_device, 0, "Mihajlo Pupin Institute", "TIM-100", GAME_IS_SKELETON)
