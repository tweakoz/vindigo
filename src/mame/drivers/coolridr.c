/******************************************************************************************************

    System H1 (c) 1994 Sega

    preliminary driver by David Haywood, Angelo Salese and Tomasz Slanina
    special thanks to Guru for references and HW advices

    TODO:
    - decode compressed GFX ROMs for "sprite" blitter (6,141,122 is the patent number)
    - DMA is still a bit of a mystery;
    - video emulation is pratically non-existant;
    - SCSP;
    - Many SH-1 ports needs investigations;
    - i8237 purpose is unknown, might even not be at the right place ...
    - IRQ generation
    - Understand & remove the hacks at the bottom;
    - IC1/IC10 are currently unused, might contain sprite data / music data for the SCSP / chars for the
      text tilemap/blitter;

=======================================================================================================

Cool Riders
Sega 1994

This game runs on SYSTEM-H1 hardware. Only one known game exists on this
PCB and this is it. The hardware seems overly complex for a 2D bike
racing game? The design of the PCB is very similar to vanilla Model 2
(i.e. Daytona etc). However instead of fully custom-badged chips,
many of the custom chips are off-the-shelf Hitachi/Toshiba gate-arrays.


PCB Layouts
-----------

SYSTEM-H1 ROM BD
171-6516C
837-9623
834-11482 (sticker)
|--------------------------------------------------------------|
|  IC17            IC18              IC5               IC10    |
|                                                              |
|  IC15            IC16              IC4               IC9     |
|                                                              |
|  IC13            IC14              IC3               IC8     |
|                                                              |
|  IC11            IC12              IC2               IC7     |
|                                                              |
|  IC31            IC32              IC1               IC6     |
|                                                              |
|  IC29            IC30                                        |
|JP1-JP8                                                       |
|  LED    32.500MHz   CN1         JP9-JP12    CN2              |
|--------------------------------------------------------------|
Notes:
      CN1/2   - Connectors joining to CPU board
      JP1-8   - Jumpers to set ROM sizes
                JP1-4 set to 1-2
                JP5-8 set to 2-3
      JP9-12  - Jumpers to set ROM sizes
                JP9 set to 1-2
                JP10-12 open (no jumpers) but JP12 pin 2 tied to JP10 pin 1
      IC*     - IC29-IC32 are 27C4002 EPROM
                IC1-IC10 are DIP42 32M mask ROM
                IC11-IC18 are DIP42 16M mask ROM


SYSTEM-H1 COMMUNICATION-BD
171-6849B
837-10942
|----------------------------------|
|                 CN2              |
|       74F74   74F245  MB84256    |
|       74F373  74F245  MB84256    |
|MB89237A         CN1              |
|                                  |
|       74F138 74F04   74F125      |
|                                  |
|       74F157 74F161  74F02       |
|                                  |
|       74F04  74F74   74F86     TX|
|MB89374                           |
|       74F02  74F160            RX|
|                         SN75179  |
|                     JP1 JP2 JP3  |
|       LED               CN3      |
|----------------------------------|
Notes: (All IC's shown)
      CN1/2   - Connectors joining to CPU board
      CN3     - Connector joining to Filter board
      RX/TX   - Optical cable connections for network (not used)
      JP*     - 3x 2-pin jumpers. JP1 shorted, other jumpers open
      MB84256 - Fujitsu MB84256 32k x8 SRAM (NDIP28)
      MB89374 - Fujitsu MB89374 Data Link Controller (SDIP42)
      MB89237A- Fujitsu MB89237A 8-Bit Proprietary DMAC (?) (DIP40)
      SN75179 - Texas Instruments SN75179 Differential Driver and Receiver Pair (DIP8)


SYSTEM-H1 CPU BD
171-6651A
837-10389
837-11481 (sticker)
|--------------------------------------------------------------|
|                                                EPR-17662.IC12|
|                                                              |
|   |--------|    |--------|                                   |
|   |SEGA    |    |SEGA    |           FM1208S      SEC_CONN   |
|   |315-5758|    |315-5757|       CN2         CN1             |
|   |        |    |        |           |------|            CN10|
|   |--------|    |--------|           |SH7032|                |
|CN8                                   |      |                |
|                                      |------|                |
|                                                              |
|                   PAL1  JP1 JP4 JP6 JP2                      |
|                   PAL2   JP3 JP5     MB3771                  |
|     28MHz   32MHz              PC910                 A1603C  |
|                                               DSW1   DAN803  |
|HM5241605                       |--------| |--------| DAP803  |
|HM5241605      PAL3             |SEGA    | |SEGA    |         |
|                                |315-5687| |315-5687| 315-5649|
|                                |        | |        |         |
|   |-----|        TMP68HC000N-16|--------| |--------|         |
|   |SH2  |                       514270      514270           |
|   |     |                         CN12               A1603C  |
|CN7|-----|         MB84256            TDA1386   TL062     CN11|
|                           22.579MHz   CN14                   |
|                   MB84256                                    |
|                                                              |
|                                      TDA1386   TL062         |
|--------------------------------------------------------------|
Notes:
      22.579MHz   - This OSC is tied to pin 1 of a 74AC04 logic chip. The output from that (pin 2) is tied
                    directly to both 315-5687 chips on pin 14. Therefore the clock input of the YMF292's is 22.579MHz
      514270      - Hitachi HM514270AJ-7 256k x16 DRAM (SOJ40)
      68000       - Clock 16.00MHz [32/2]
      A1603C      - NEC uPA1603C Monolithic N-Channel Power MOS FET Array (DIP16)
      CN7/8       - Connectors joining to ROM board (above)
      CN10/11     - Connectors joining to Filter board
      CN12/14     - Connectors for (possible) extra sound board (not used)
      DAN803      - Diotec Semiconductor DAN803 Small Signal Diode Array with common anodes (SIL9)
      DAP803      - Diotec Semiconductor DAP803 Small Signal Diode Array with common cathodes (SIL9)
      DSW1        - 4-position DIP switch. All OFF
      EPR-17662   - Toshiba TC57H1025 1M EPROM (DIP40)
      FM1208S     - RAMTRON FM1208S 4k (512 bytes x8) Nonvolatile Ferroelectric RAM (SOIC24)
      HM5241605   - Hitachi HM5241605 4M (256k x 16 x 2 banks) SDRAM (SSOP50)
      JP1-6       - Jumpers. JP2 open. JP5 1-2. All others 2-3
      MB3771      - Fujitsu MB3771 Master Reset IC (SOIC8)
      MB84256     - Fujitsu MB84256 32k x8 SRAM (SOP28)
      PAL1        - GAL16V8B also marked '315-5800' (DIP20)
      PAL2        - GAL16V8B also marked '315-5802' (DIP20)
      PAL3        - GAL16V8B also marked '315-5801' (DIP20)
      PC910       - Sharp PC910 opto-isolator (DIP8)
      SEC_CONN    - Sega security-board connector (not used)
      SH7032      - Hitachi HD6417032F20 ROMless SH1 CPU (QFP112). Clock input 16.00MHz on pin 71
      SH2         - Hitachi HD6417095 SH2 CPU (QFP144). Clock input 28.00MHz on pin 118
      TDA1386     - Philips TDA1386T Noise Shaping Filter DAC (SOP24)
      TL062       - Texas Instruments TL062 Low Power JFET Input Operational Amplifier (SOIC8)
      Sega Custom - 315-5757 (QFP160)
                    315-5758 (QFP168) also marked 'HG62G035R26F'
                    315-5649 (QFP100) custom I/O chip (also used on Model 2A/2B/2C, but NOT vanilla Model 2)
                    315-5687 (QFP128 x2) also marked 'YMF292-F' (also used on Model 2A/2B/2C and ST-V)
      Syncs       - Horizontal 24.24506kHz
                    Vertical 57.0426Hz


SYSTEM-H1 VIDEO BD
171-6514F
837-9621
|--------------------------------------------------------------|
|         CN2                                                  |
|                                                           JP4|
|                                   |--------|  TC55328 D431008|
|      |--------|          HM514270 |SEGA    |  TC55328 D431008|
|      |SEGA    |          HM514270 |315-5697|              JP3|
|CN1   |315-5691|  TC55328 HM514270 |        |  D431008 D431008|
|      |        |  TC55328 HM514270 |--------|  D431008 D431008|
|      |--------|                                           JP2|
|                                                           JP1|
|                                     315-5698   315-5648      |
|        |------------|                                        |
|        |  SEGA      |      50MHz                             |
|        |  315-5692  |               315-5696   315-5648      |
|        |            |                                     CN9|
|LED     |------------|                                        |
|40MHz     |--------|                 315-5696   315-5648      |
|          |SEGA    |                                          |
|   PAL1   |315-5693|                                          |
|CN4       |        |       315-5695  315-5695   315-5648      |
||--------||--------|                                          |
||SEGA    |                                                    |
||315-5694||--------| M5M411860 M5M411860 M5M411860 M5M411860  |
||        ||SEGA    | M5M411860 M5M411860 M5M411860 M5M411860  |
||--------||315-5693| M5M411860 M5M411860 M5M411860 M5M411860  |
|   PAL2   |        | M5M411860 M5M411860 M5M411860 M5M411860  |
|          |--------|                                          |
|--------------------------------------------------------------|
Notes:
      CN9         - Connector joining to Filter board
      CN1/2/4     - Connectors joining to CPU board
      JP1/2/3/4   - 4x 3-pin jumpers. All set to 1-2
      D431008     - NEC D431008 128k x8 SRAM (SOJ32)
      HM514270    - Hitachi HM514270AJ7 256k x16 DRAM (SOJ40)
      M5M411860   - Mitsubishi M5M411860TP435SF00-7 DRAM with fast page mode, 64k-words x 18 bits per word (maybe?) (TSOP42)
      TC55328     - Toshiba TC55328AJ-15 32k x8 SRAM (SOJ24)
      PAL1        - GAL16V8B also marked '315-5803' (DIP20)
      PAL2        - GAL16V8B also marked '315-5864' (DIP20)
      Sega Custom - 315-5648 (QFP64, x4)
                    315-5691 also marked 'HG62S0791R17F' (QFP208)
                    315-5692 also marked 'HG51B152FD' (QFP256)
                    315-5693 also marked 'HG62G019R16F' (QFP168, x3)
                    315-5694 (QFP208)
                    315-5695 (QFP100, x2)
                    315-5696 (QFP120, x2)
                    315-5697 (QFP208)
                    315-5698 (QFP144)

*******************************************************************************************************

Note: This hardware appears to have been designed as a test-bed for a new RLE based compression system
      used by the zooming sprites.  It is possible that Sega planned on using this for ST-V, but
      decided against it. Video/CPU part numbers give an interesting insight, since video hardware #
      sits between Model 1 & Model 2.

                 Year on
      System      PCB     PCB #      PALs
      ---------------------------------------------------------
      System32    1990    837-7428   315-5441 315-5442
      SysMulti32  1992    837-8676   315-5596
      Model 1     1992    837-8886   315-5546 315-5483 315-5484
      Model 2     1994    837-10071  315-5737 315-5741
      Model 2A    1994    837-10848  315-5737 315-5815
      STV         1994    837-10934  315-5833

      H1 (CPU)    1994    837-10389  315-5800 315-5801 315-5802
      H1 (Video)  1994    837-9621   315-5803 315-5864


   NOTE:  While the hardware and title screen might list 1994 as a copyright, MAME uses 1995 due to the
   abudance of evidence in trade journals and even it's own service manuals showing the year as 1995.

   References:
   Arcade game magazine called 'Gamest' show released on 04.28.1995
   VGL (Ultimate Video Game List published by AMP group) - year is printed as '94(4.95)'
   Sega Arcade History (published by Enterbrain) is '1995/4'.


******************************************************************************************************/


#include "emu.h"
#include "debugger.h"
#include "cpu/sh2/sh2.h"
#include "cpu/m68000/m68000.h"
#include "sound/scsp.h"
#include "machine/am9517a.h"
#include "rendlay.h"

//#define FAKE_ASCII_ROM

class coolridr_state : public driver_device
{
public:
	coolridr_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_textBytesToWrite(0x00),
		m_blitterSerialCount(0x00),
		m_blitterMode(0x00),
		m_textOffset(0x0000),
		m_colorNumber(0x00000000),
		m_vCellCount(0x0000),
		m_hCellCount(0x0000),
		m_vPosition(0x0000),
		m_hPosition(0x0000),
		m_maincpu(*this, "maincpu"),
		m_subcpu(*this,"sub"),
		m_soundcpu(*this,"soundcpu"),
		m_dmac(*this, "i8237"),
		m_h1_vram(*this, "h1_vram"),
		m_h1_charram(*this, "h1_charram"),
		m_framebuffer_vram(*this, "fb_vram"),
		m_txt_vram(*this, "txt_vram"),
		m_sysh1_txt_blit(*this, "sysh1_txt_blit"),
		m_sysh1_workram_h(*this, "sysh1_workrah"),
		m_sound_dma(*this, "sound_dma"),
		m_soundram(*this, "soundram"),
		m_soundram2(*this, "soundram2")
		{ }

	// Blitter state
	UINT16 m_textBytesToWrite;
	INT16  m_blitterSerialCount;
	UINT8  m_blitterMode;
	UINT16 m_textOffset;
	UINT32 m_colorNumber;
	UINT16 m_vCellCount;
	UINT16 m_hCellCount;
	int m_vPosition;
	int m_hPosition;

	UINT16 m_vOrigin;
	UINT16 m_hOrigin;
	UINT16 m_vZoom;
	UINT16 m_hZoom;
	UINT32 m_blit0; // ?
	UINT32 m_blit1; // ?
	UINT32 m_blit2; // ?
	UINT32 m_blit3; // ?
	UINT32 m_blit4; // ?
	UINT32 m_blit5; // ?
	UINT32 m_blit10; // an address

	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_subcpu;
	required_device<cpu_device> m_soundcpu;
	required_device<am9517a_device> m_dmac;

	required_shared_ptr<UINT32> m_h1_vram;
	required_shared_ptr<UINT32> m_h1_charram;
	required_shared_ptr<UINT32> m_framebuffer_vram;
	required_shared_ptr<UINT32> m_txt_vram;
	required_shared_ptr<UINT32> m_sysh1_txt_blit;
	required_shared_ptr<UINT32> m_sysh1_workram_h;
	required_shared_ptr<UINT32> m_sound_dma;
	required_shared_ptr<UINT16> m_soundram;
	required_shared_ptr<UINT16> m_soundram2;
	bitmap_rgb32 m_temp_bitmap_sprites;
	bitmap_rgb32 m_temp_bitmap_sprites2;
	UINT32 m_test_offs;
	int m_color;
	UINT8 m_vblank;
	int m_scsp_last_line;
	UINT8 an_mux_data;
	UINT8 sound_data;

	DECLARE_READ32_MEMBER(sysh1_sound_dma_r);
	DECLARE_WRITE32_MEMBER(sysh1_sound_dma_w);
	DECLARE_READ32_MEMBER(sysh1_ioga_r);
	DECLARE_WRITE32_MEMBER(sysh1_ioga_w);
	DECLARE_WRITE32_MEMBER(sysh1_txt_blit_w);
	DECLARE_WRITE32_MEMBER(sysh1_pal_w);
	DECLARE_WRITE32_MEMBER(sysh1_dma_w);
	DECLARE_WRITE32_MEMBER(sysh1_char_w);
	DECLARE_READ32_MEMBER(coolridr_hack1_r);
	DECLARE_READ32_MEMBER(coolridr_hack2_r);
	DECLARE_READ16_MEMBER(h1_soundram_r);
	DECLARE_READ16_MEMBER(h1_soundram2_r);
	DECLARE_WRITE16_MEMBER(h1_soundram_w);
	DECLARE_WRITE16_MEMBER(h1_soundram2_w);
	DECLARE_READ8_MEMBER(analog_mux_r);
	DECLARE_WRITE8_MEMBER(analog_mux_w);
	DECLARE_WRITE8_MEMBER(lamps_w);
	DECLARE_WRITE_LINE_MEMBER(scsp_to_main_irq);
	DECLARE_WRITE8_MEMBER(sound_to_sh1_w);
	DECLARE_DRIVER_INIT(coolridr);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	UINT32 screen_update_coolridr(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect, int which);
	UINT32 screen_update_coolridr1(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_coolridr2(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(system_h1);
	TIMER_DEVICE_CALLBACK_MEMBER(system_h1_main);
	TIMER_DEVICE_CALLBACK_MEMBER(system_h1_sub);
};



/* video */

void coolridr_state::video_start()
{
	machine().primary_screen->register_screen_bitmap(m_temp_bitmap_sprites);
	machine().primary_screen->register_screen_bitmap(m_temp_bitmap_sprites2);
	m_test_offs = 0x2000;
}

// might be a page 'map / base' setup somewhere, but it's just used for ingame backgrounds
/* 0x00000 - 0x1ffff = screen 1 */
/* 0x20000 - 0x3ffff = screen 2 */
/* 0x40000 - 0xfffff = ? */
UINT32 coolridr_state::screen_update_coolridr(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect, int which)
{
	/* planes seems to basically be at 0x8000 and 0x28000... */
	gfx_element *gfx = machine().gfx[2];
	UINT32 count;
	int y,x;
	int color;
	count = m_test_offs/4;
	color = m_color;

	if (which==1)
	{
		count += 0x20000/4;
//		color += 0x5e;
		color += 2;
	}
	else
	{
//		color += 0x4e;
//		color += 0x0;

	}

	for (y=0;y<64;y++)
	{
		for (x=0;x<128;x+=2)
		{
			int tile;

			tile = (m_h1_vram[count] & 0x0fff0000) >> 16;
			drawgfx_opaque(bitmap,cliprect,gfx,tile,color,0,0,(x+0)*16,y*16);

			tile = (m_h1_vram[count] & 0x00000fff) >> 0;
			drawgfx_opaque(bitmap,cliprect,gfx,tile,color,0,0,(x+1)*16,y*16);

			count++;
		}
	}

	if (which==0)
	{
		copybitmap_trans(bitmap, m_temp_bitmap_sprites, 0, 0, 0, 0, cliprect, 0);
		m_temp_bitmap_sprites.fill(0, cliprect);
	}
	else
	{
		copybitmap_trans(bitmap, m_temp_bitmap_sprites2, 0, 0, 0, 0, cliprect, 0);
		m_temp_bitmap_sprites2.fill(0, cliprect);
	}

	return 0;
}

UINT32 coolridr_state::screen_update_coolridr1(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
#if 0
	if(machine().input().code_pressed(KEYCODE_Z))
		m_test_offs+=4;

	if(machine().input().code_pressed(KEYCODE_X))
		m_test_offs-=4;

	if(machine().input().code_pressed(KEYCODE_C))
		m_test_offs+=0x40;

	if(machine().input().code_pressed(KEYCODE_V))
		m_test_offs-=0x40;

	if(machine().input().code_pressed(KEYCODE_B))
		m_test_offs+=0x400;

	if(machine().input().code_pressed(KEYCODE_N))
		m_test_offs-=0x400;

	if(machine().input().code_pressed_once(KEYCODE_A))
		m_color++;

	if(machine().input().code_pressed_once(KEYCODE_S))
		m_color--;

	if(m_test_offs > 0x100000*4)
		m_test_offs = 0;

#endif

//	popmessage("%08x %04x",m_test_offs,m_color);

	return screen_update_coolridr(screen,bitmap,cliprect,0);
}

UINT32 coolridr_state::screen_update_coolridr2(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	return screen_update_coolridr(screen,bitmap,cliprect,1);
}

/* end video */



/* According to Guru, this is actually the same I/O chip of Sega Model 2 HW */
#if 0
READ32_MEMBER(coolridr_state::sysh1_ioga_r)
{
	//return machine().rand();//h1_ioga[offset];
	return h1_ioga[offset];
}

WRITE32_MEMBER(coolridr_state::sysh1_ioga_w)
{
	COMBINE_DATA(&h1_ioga[offset]);
}
#endif


/* This is a RLE-based sprite blitter (US Patent #6,141,122), very unusual from Sega... */
WRITE32_MEMBER(coolridr_state::sysh1_txt_blit_w)
{
	COMBINE_DATA(&m_sysh1_txt_blit[offset]);


	switch(offset)
	{
		// The mode register
		case 0x04:
		{
			m_blitterMode = (data & 0x00ff0000) >> 16;

			if (m_blitterMode == 0xf4)
			{
				// Some sort of addressing state.
				// In the case of text, simply writes 4 characters per 32-bit word.
				// These values may be loaded into RAM somewhere as they are written.
				// The number of characters is determined by the upper-most 8 bits.
				m_textBytesToWrite = (data & 0xff000000) >> 24;
				m_textOffset = (data & 0x0000ffff);
				m_blitterSerialCount = 0;
				
				// this is ONLY used when there is text on the screen
				
				//printf("set mode %08x\n", data);

			
			}
			else if (m_blitterMode == 0x30 || m_blitterMode == 0x40 || m_blitterMode == 0x50 || m_blitterMode == 0x60
				  || m_blitterMode == 0x90 || m_blitterMode == 0xa0 || m_blitterMode == 0xb0 || m_blitterMode == 0xc0)
			{
				// The blitter function(s).
				// After this is set a fixed count of 11 32-bit words are sent to the data register.
				// The lower word always seems to be 0x0001 and the upper byte always 0xac.
				m_blitterSerialCount = 0;

				// form 0xacMM-xxx   ac = fixed value for this mode?  MM = modes above.  -xxx = some kind of offset? but it doesn't increment for each blit like the textOffset / paletteOffset stuff, investigate  

			}
			else if (m_blitterMode == 0x10)
			{
				// Could be a full clear of VRAM?
				for(UINT32 vramAddr = 0x3f40000; vramAddr < 0x3f4ffff; vramAddr+=4)
					space.write_dword(vramAddr, 0x00000000);

				m_blitterSerialCount = 0;
			}
			else if (m_blitterMode == 0xe0)
			{
				// uploads palettes...
				// does NOT upload the palette for the WDUD screen when set to US mode this way..
				m_blitterSerialCount = 0;
				m_textOffset = (data & 0x0000ffff)>>2; // it's a byte offset

			//	printf("set e0 %08x\n", data);

			}
			else
			{
				printf("set unknown blit mode %02x\n", m_blitterMode);
			}
			break;
		}

		// The data register
		case 0x05:
		{
			if (m_blitterMode == 0xf4)
			{
				// Uploads a series of bytes that index into the encoded sprite table
				const size_t memOffset = 0x03f40000 + m_textOffset + m_blitterSerialCount;
				space.write_dword(memOffset, data);
				m_blitterSerialCount += 0x04;

				// DEBUG: Uncomment to see the ASCII strings as they are being blitted
				//if (m_blitterSerialCount >= m_textBytesToWrite)
				//{
				//  for (int i = 0; i < m_textBytesToWrite+1; i++)
				//      printf("%c", read_byte(0x03f40000 + m_textOffset + i));
				//  printf("\n");
				//}
			}
			else if (m_blitterMode == 0x30 || m_blitterMode == 0x40 || m_blitterMode == 0x50 || m_blitterMode == 0x60
				  || m_blitterMode == 0x90 || m_blitterMode == 0xa0 || m_blitterMode == 0xb0 || m_blitterMode == 0xc0)
			{
				// Serialized 32-bit words in order of appearance:
				//  0: 00000000 - totally unknown : always seems to be zero
				//  1: xxxxxxxx - "Color Number" (all bits or just lower 16/8?)
				//  2: 00000000 - unknown : OT flag?  (transparency)
				//  3: 00000000 - unknown : RF flag?  (90 degree rotation)
				//  4: 07000000 - unknown : VF flag?  (vertically flipped)
				//  5: 00010000 - unknown : HF flag?  (horizontally flipped) (or mode... )
				//  6: vvvv---- - "Vertical Cell Count"
				//  6: ----hhhh - "Horizontal Cell Count"
				//  7: 00000000 - unknown : "Vertical|Horizontal Zoom Centers"?
				//  8: 00400040 - unknown : "Vertical|Horizontal Zoom Ratios"?
				//  9: xxxx---- - "Display Vertical Position"
				//  9: ----yyyy - "Display Horizontal Position"
				// 10: 00000000 - unknown : always seems to be zero - NO, for some things (not text) it's also a reference to 3f40000 region like #11
				// 11: ........ - complex - likely an address into bytes uploaded by mode 0xf4  (likely, it's only used when text is present otherwise it's always 0, some indirect tile mode I guess)
				//                (See ifdef'ed out code below for a closer examination)

				// Serialized counts
				if (m_blitterSerialCount == 0)
				{
					// set to 0x00000001 on some objects during the 'film strip' part of attract, otherwise 0
					// those objects don't seem visible anyway so might have some special meaning
					m_blit0 = data;
				//	if (data!=0) printf("blit %08x\n", data);
				}
				else if (m_blitterSerialCount == 1)
				{
					// 000u0ccc  - c = colour? u = 0/1
					m_blit1 = data;

					m_colorNumber = (data & 0x000000ff);    // Probably more bits
				//	if (data!=0) printf("blit %08x\n", data);
				}
				else if (m_blitterSerialCount == 2)
				{
					// seems to be more complex than just transparency
					m_blit2 = data;

					 // 00??0uuu  
					 // ?? seems to be 00 or 7f
					 // uuu, at least 11 bits used, maybe 12

				}
				else if (m_blitterSerialCount == 3)
				{
					m_blit3 = data;
					// 0000xxxx
					//  to
					// 001fxxxx
				}
				else if (m_blitterSerialCount == 4)
				{
					m_blit4 = data;

					//0x000y0z
					// x = 1, 2, 3 or 7
					// y = 0 or 1
					// z = 0 or 1
				}
				else if (m_blitterSerialCount == 5)
				{
					m_blit5 = data;
					// this might enable the text indirection thing?

				//	if (data!=0) printf("blit %08x\n", data);
					// 00010000 (text)
					// 00000001 (other)

				}
				else if (m_blitterSerialCount == 6)
				{
					m_vCellCount = (data & 0xffff0000) >> 16;
					m_hCellCount = (data & 0x0000ffff);
				}
				else if (m_blitterSerialCount == 7)
				{
					m_vOrigin = (data & 0xffff0000) >> 16;
					m_hOrigin = (data & 0x0000ffff);
					//printf("%04x %04x\n", m_vOrigin, m_hOrigin);
				}
				else if (m_blitterSerialCount == 8)
				{
					m_vZoom = (data & 0xffff0000) >> 16;
					m_hZoom = (data & 0x0000ffff);
				}
				else if (m_blitterSerialCount == 9)
				{
					m_vPosition = (data & 0xffff0000) >> 16;
					m_hPosition = (data & 0x0000ffff);

					if (m_hPosition & 0x8000) m_hPosition -= 0x10000;
					if (m_vPosition & 0x8000) m_vPosition -= 0x10000;
				}
				else if (m_blitterSerialCount == 10)
				{
					// this is an address on some objects..
					// to be specific, the center line of the road (actual road object? which currently gets shown as a single pixel column?)
					// and the horizontal road used in the background of the title screen (which currently looks normal)
					// I guess it's some kind of indirect way to do a line effect?
					m_blit10 = data;

				//	if (data!=0) printf("blit %08x\n", data);
				}
				else if (m_blitterSerialCount == 11)
				{
					// for text objects this is an address containing the 8-bit tile numbers to use for ASCII text
					// I guess the tiles are decoded by a DMA operation earlier, from the compressed ROM?

					// we also use this to trigger the actual draw operation

					//printf("blit %08x\n", data);
					
					// debug, hide objects without m_blit10 set
					//if (m_blit10==0) return;
					//if (m_blit0==0) return;

					const UINT32 memOffset = data;
					bitmap_rgb32* drawbitmap;

					// guess, you can see the different sizes of bike cross from the left screen to the right where the attract text is
					if (m_blitterMode == 0x30 || m_blitterMode == 0x40 || m_blitterMode == 0x50 || m_blitterMode == 0x60)
						drawbitmap = &m_temp_bitmap_sprites;
					else // 0x90, 0xa0, 0xb0, 0xc0
						drawbitmap = &m_temp_bitmap_sprites2;

					int sizex = m_hCellCount * 16 * m_hZoom;
					int sizey = m_vCellCount * 16 * m_vZoom;
					m_hPosition *= 0x40;
					m_vPosition *= 0x40;

					switch (m_vOrigin & 3)
					{
					case 0:
						// top
						break;
					case 1:
						m_vPosition -= sizey / 2 ;
						// middle?
						break;
					case 2:
						m_vPosition -= sizey;
						// bottom?
						break;
					case 3:
						// invalid?
						break;
					}

					switch (m_hOrigin & 3)
					{
					case 0:
						// left
						break;
					case 1:
						m_hPosition -= sizex / 2;
						// middle?
						break;
					case 2:
						m_hPosition -= sizex;
						// right?
						break;
					case 3:
						// invalid?
						break;
					}

					// Splat some sprites
					for (int h = 0; h < m_hCellCount; h++)
					{
						for (int v = 0; v < m_vCellCount; v++)
						{
							const int pixelOffsetX = ((m_hPosition) + (h* 16 * m_hZoom)) / 0x40;
							const int pixelOffsetY = ((m_vPosition) + (v* 16 * m_vZoom)) / 0x40;

							// It's unknown if it's row-major or column-major
							// TODO: Study the CRT test and "Cool Riders" logo for clues.
							UINT8 spriteNumber = space.read_byte(memOffset + h + (v*h));

							// DEBUG: For demo purposes, skip &spaces and NULL characters
							if (m_blitterMode == 0x30 || m_blitterMode == 0x90)
								if (spriteNumber == 0x20 || spriteNumber == 0x00)
									continue;

#ifdef FAKE_ASCII_ROM
							if (m_blitterMode == 0x30 || m_blitterMode == 0x90)
							{

								drawgfx_opaque(*drawbitmap,drawbitmap->cliprect(), machine().gfx[3],spriteNumber,0,0,0,pixelOffsetX,pixelOffsetY);
								continue;
							}
#endif


							int blockwide = ((16*m_hZoom)/0x40)-1;
							int blockhigh = ((16*m_vZoom)/0x40)-1;
							// hack
							if (blockwide<=0) blockwide = 1;
							if (blockhigh<=0) blockhigh = 1;

							// DEBUG: Draw 16x16 block
							for (int x = 0; x < blockwide; x++)
							{
								for (int y = 0; y < blockhigh; y++)
								{

									UINT32 color = 0xffffffff;
									// HACKS to draw coloured blocks in easy to distinguish colours
									if (m_blitterMode == 0x30 || m_blitterMode == 0x90)
									{
										if (m_colorNumber == 0x5b)
											color = 0xffff0000;
										else if (m_colorNumber == 0x5d)
											color = 0xff00ff00;
										else if (m_colorNumber == 0x5e)
											color = 0xff0000ff;
										else
											color = 0xff00ffff;
									}
									else if (m_blitterMode == 0x40 || m_blitterMode == 0xa0)
									{
										color = 0xff000000 | (((m_colorNumber & 0xff) | 0x80)-0x40);
									}
									else if (m_blitterMode == 0x50 || m_blitterMode == 0xb0)
									{
										color = 0xff000000 | ((((m_colorNumber & 0xff) | 0x80)-0x40) << 8);
									}
									else if (m_blitterMode == 0x60 || m_blitterMode == 0xc0)
									{
										color = 0xff000000 | ((((m_colorNumber & 0xff) | 0x80)-0x40) << 16);
									}


									if (drawbitmap->cliprect().contains(pixelOffsetX+x, pixelOffsetY+y))
										if (drawbitmap->pix32(pixelOffsetY+y, pixelOffsetX+x)==0) drawbitmap->pix32(pixelOffsetY+y, pixelOffsetX+x) = color;
								}
							}
						}
					}
				}
				else
				{
					printf("more than 11 dwords (%d) in blit?\n", m_blitterSerialCount);
				}


				m_blitterSerialCount++;
			}
			// ??
			else if (m_blitterMode == 0x10) // at startup
			{
				//printf("blit mode %02x %02x %08x\n", m_blitterMode, m_blitterSerialCount,  data);
				m_blitterSerialCount++;
			}
			else if (m_blitterMode == 0xe0) // when going into game (in units of 0x10 writes)
			{
				// it writes the palette for the bgs here, with fade effects?
				//  is this the only way for the tile colours to be actually used, or does this just go to memory somewhere too?
				//printf("blit mode %02x %02x %08x\n", m_blitterMode, m_blitterSerialCount,  data);

				sysh1_pal_w(space,m_textOffset,data,0xffffffff);
				m_textOffset++;

			}
			else
			{
				logerror("unk blit mode %02x\n", m_blitterMode);
			}
			break;
		}
	}
}


// NOTE, this gets called from the blitter code above AND the DMA code below.. addresses from each are probably wrong
WRITE32_MEMBER(coolridr_state::sysh1_pal_w)
{
	int r,g,b;
	COMBINE_DATA(&m_generic_paletteram_32[offset]);

	r = ((m_generic_paletteram_32[offset] & 0x00007c00) >> 10);
	g = ((m_generic_paletteram_32[offset] & 0x000003e0) >> 5);
	b = ((m_generic_paletteram_32[offset] & 0x0000001f) >> 0);
	palette_set_color_rgb(machine(),(offset*2)+1,pal5bit(r),pal5bit(g),pal5bit(b));
	r = ((m_generic_paletteram_32[offset] & 0x7c000000) >> 26);
	g = ((m_generic_paletteram_32[offset] & 0x03e00000) >> 21);
	b = ((m_generic_paletteram_32[offset] & 0x001f0000) >> 16);
	palette_set_color_rgb(machine(),offset*2,pal5bit(r),pal5bit(g),pal5bit(b));
}


/* FIXME: this seems to do a hell lot of stuff, it's not ST-V SCU but still somewhat complex :/ */
static void sysh1_dma_transfer( address_space &space, UINT16 dma_index )
{
	coolridr_state *state = space.machine().driver_data<coolridr_state>();
	UINT32 src,dst,size,type,s_i;
	UINT8 end_dma_mark;

	end_dma_mark = 0;

	do{
		src = (state->m_framebuffer_vram[(0+dma_index)/4] & 0x0fffffff);
		dst = (state->m_framebuffer_vram[(4+dma_index)/4]);
		size = state->m_framebuffer_vram[(8+dma_index)/4];
		type = (state->m_framebuffer_vram[(0+dma_index)/4] & 0xf0000000) >> 28;

		#if 0
		if(type == 0xc || type == 0xd || type == 0xe)
			printf("* %08x %08x %08x %08x\n",src,dst,size,type);
		else if(type != 0 && type != 0x4)
			printf("%08x %08x %08x %08x\n",src,dst,size,type);
		#endif

		if(type == 0x3 || type == 0x4)
		{
			//type 3 sets a DMA state->m_param, type 4 sets some kind of table? Skip it for now
			dma_index+=4;
			continue;
		}

		if(type == 0xc)
		{
			dst &= 0xfffff;

			dst |= 0x3000000; //to videoram, FIXME: unknown offset
			size*=2;
		}
		if(type == 0xd)
		{
			dst &= 0xfffff;

			dst |= 0x3d00000; //to charram, FIXME: unknown offset
			size*=2;
		}

		if(type == 0xe)
		{
			dst &= 0xfffff;

			dst |= 0x3c00000; //to paletteram FIXME: unknown offset
			//size/=2;

			// this is used when transfering palettes written by the blitter? maybe?
			//  it might be a better indication of where blitter command 0xe0 should REALLY write data (at 0x3e00000)...
			if((src & 0xff00000) == 0x3e00000)
			{
				src &= 0xfffff;
				src |= 0x3c00000;
			}
			//	return; //FIXME: kludge to avoid palette corruption
			//debugger_break(space.machine());
		}

		if(type == 0xc || type == 0xd || type == 0xe)
		{
			for(s_i=0;s_i<size;s_i+=4)
			{
				space.write_dword(dst,space.read_dword(src));
				dst+=4;
				src+=4;
			}
		}
		else
		{
			//printf("%08x %08x %08x %08x\n",src,dst,size,type);
		}

		if(type == 0x00)
			end_dma_mark = 1; //end of DMA list

		dma_index+=0xc;

	}while(!end_dma_mark );
}

WRITE32_MEMBER(coolridr_state::sysh1_dma_w)
{
	COMBINE_DATA(&m_framebuffer_vram[offset]);

	if(offset*4 == 0x000)
	{
		if((m_framebuffer_vram[offset] & 0xff00000) == 0xfe00000)
			sysh1_dma_transfer(space, m_framebuffer_vram[offset] & 0xffff);
	}
}

WRITE32_MEMBER(coolridr_state::sysh1_char_w)
{
	COMBINE_DATA(&m_h1_charram[offset]);

	{
		UINT8 *gfx = memregion("ram_gfx")->base();

		gfx[offset*4+0] = (m_h1_charram[offset] & 0xff000000) >> 24;
		gfx[offset*4+1] = (m_h1_charram[offset] & 0x00ff0000) >> 16;
		gfx[offset*4+2] = (m_h1_charram[offset] & 0x0000ff00) >> 8;
		gfx[offset*4+3] = (m_h1_charram[offset] & 0x000000ff) >> 0;

		machine().gfx[2]->mark_dirty(offset/64); //*4/256
	}
}

static ADDRESS_MAP_START( system_h1_map, AS_PROGRAM, 32, coolridr_state )
	AM_RANGE(0x00000000, 0x001fffff) AM_ROM AM_SHARE("share1") AM_WRITENOP
	AM_RANGE(0x01000000, 0x01ffffff) AM_ROM AM_REGION("gfx_data",0x0000000)

	AM_RANGE(0x03000000, 0x030fffff) AM_RAM AM_SHARE("h1_vram")//bg vram
	AM_RANGE(0x03c00000, 0x03c0ffff) AM_RAM_WRITE(sysh1_pal_w) AM_SHARE("paletteram") // palettes get written here, but the actual used ones seem to get sent via blitter??
	AM_RANGE(0x03d00000, 0x03dfffff) AM_RAM_WRITE(sysh1_char_w) AM_SHARE("h1_charram") //FIXME: half size
	AM_RANGE(0x03e00000, 0x03efffff) AM_RAM_WRITE(sysh1_dma_w) AM_SHARE("fb_vram") //FIXME: not all of it

	AM_RANGE(0x03f00000, 0x03f0ffff) AM_RAM AM_SHARE("share3") /*Communication area RAM*/
	AM_RANGE(0x03f40000, 0x03f4ffff) AM_RAM AM_SHARE("txt_vram")//text tilemap + "lineram"
	AM_RANGE(0x04000000, 0x0400003f) AM_RAM_WRITE(sysh1_txt_blit_w) AM_SHARE("sysh1_txt_blit")
	AM_RANGE(0x06000000, 0x060fffff) AM_RAM AM_SHARE("sysh1_workrah")
	AM_RANGE(0x20000000, 0x201fffff) AM_ROM AM_SHARE("share1")

	AM_RANGE(0x60000000, 0x600003ff) AM_WRITENOP
ADDRESS_MAP_END

READ16_MEMBER( coolridr_state::h1_soundram_r)
{
	return m_soundram[offset];
}

READ16_MEMBER( coolridr_state::h1_soundram2_r)
{
	return m_soundram2[offset];
}

WRITE16_MEMBER( coolridr_state::h1_soundram_w)
{
	COMBINE_DATA(&m_soundram[offset]);
}

WRITE16_MEMBER( coolridr_state::h1_soundram2_w)
{
	COMBINE_DATA(&m_soundram2[offset]);
}

READ8_MEMBER( coolridr_state::analog_mux_r )
{
	static const char *const adcnames[] = { "AN0", "AN1", "AN2", "AN3", "AN4", "AN5", "AN6", "AN7" };
	UINT8 adc_data = ioport(adcnames[an_mux_data])->read_safe(0);
	an_mux_data++;
	an_mux_data &= 0x7;
	return adc_data;
}

WRITE8_MEMBER( coolridr_state::analog_mux_w )
{
	an_mux_data = data;
}

WRITE8_MEMBER( coolridr_state::lamps_w )
{
	/*
	x--- ---- P2 Music select Lamp
	-x-- ---- P1 Music select Lamp
	--x- ---- P2 Race Leader Lamp
	---x ---- P1 Race Leader Lamp
	---- x--- P2 Start Lamp
	---- -x-- P1 Start Lamp
	---- ---x (used in game?)
	*/
}


READ32_MEMBER(coolridr_state::sysh1_sound_dma_r)
{
	if(offset == 8)
	{
		//popmessage("%02x",sound_data);
		/* TODO: this probably stalls the DMA transfers. */
		return sound_data;
	}

	if(offset == 2 || offset == 6) // DMA status
		return 0;

	printf("%08x\n",offset);

	return m_sound_dma[offset];
}

WRITE32_MEMBER(coolridr_state::sysh1_sound_dma_w)
{
	address_space &main_space = m_maincpu->space(AS_PROGRAM);
	address_space &sound_space = m_soundcpu->space(AS_PROGRAM);

	//printf("%08x %08x\n",offset*4,m_h1_unk[offset]);

	if(offset == 8)
	{
		//probably writing to upper word disables m68k, to lower word enables it
		machine().device("soundcpu")->execute().set_input_line(INPUT_LINE_RESET, (data) ? ASSERT_LINE : CLEAR_LINE);
		return;
	}

	if(offset == 2)
	{
		if(data & 1 && (!(m_sound_dma[2] & 1))) // 0 -> 1 transition enables DMA
		{
			UINT32 src = m_sound_dma[0];
			UINT32 dst = m_sound_dma[1];
			UINT32 size = (m_sound_dma[2]>>16)*0x40;

			for(int i = 0;i < size; i+=2)
			{
				sound_space.write_word(dst,main_space.read_word(src));
				src+=2;
				dst+=2;
			}
		}
	}

	if(offset == 6)
	{
		if(data & 1 && (!(m_sound_dma[6] & 1))) // 0 -> 1 transition enables DMA
		{
			UINT32 src = m_sound_dma[4];
			UINT32 dst = m_sound_dma[5];
			UINT32 size = (m_sound_dma[6]>>16)*0x40;

			//printf("%08x %08x %08x %02x\n",src,dst,size,sound_data);

			for(int i = 0;i < size; i+=2)
			{
				sound_space.write_word(dst,main_space.read_word(src));
				src+=2;
				dst+=2;
			}
		}
	}

	COMBINE_DATA(&m_sound_dma[offset]);
}


static ADDRESS_MAP_START( coolridr_submap, AS_PROGRAM, 32, coolridr_state )
	AM_RANGE(0x00000000, 0x0001ffff) AM_ROM AM_SHARE("share2")

	AM_RANGE(0x01000000, 0x0100ffff) AM_RAM //communication RAM

	AM_RANGE(0x03000000, 0x0307ffff) AM_READWRITE16(h1_soundram_r, h1_soundram_w,0xffffffff) //AM_SHARE("soundram")
	AM_RANGE(0x03100000, 0x03100fff) AM_DEVREADWRITE16_LEGACY("scsp1", scsp_r, scsp_w, 0xffffffff)
	AM_RANGE(0x03200000, 0x0327ffff) AM_READWRITE16(h1_soundram2_r, h1_soundram2_w,0xffffffff) //AM_SHARE("soundram2")
	AM_RANGE(0x03300000, 0x03300fff) AM_DEVREADWRITE16_LEGACY("scsp2", scsp_r, scsp_w, 0xffffffff)

//	AM_RANGE(0x04000000, 0x0400001f) AM_DEVREADWRITE8("i8237", am9517a_device, read, write, 0xffffffff)
	AM_RANGE(0x04000000, 0x0400003f) AM_READWRITE(sysh1_sound_dma_r,sysh1_sound_dma_w) AM_SHARE("sound_dma")
//	AM_RANGE(0x04200000, 0x0420003f) AM_RAM /* hi-word for DMA? */

	AM_RANGE(0x05000000, 0x05000fff) AM_RAM
	AM_RANGE(0x05200000, 0x052001ff) AM_RAM
	AM_RANGE(0x05300000, 0x0530ffff) AM_RAM AM_SHARE("share3") /*Communication area RAM*/
	AM_RANGE(0x05ff0000, 0x05ffffff) AM_RAM /*???*/
	AM_RANGE(0x06000000, 0x060001ff) AM_RAM // backup RAM
	AM_RANGE(0x06100000, 0x06100003) AM_READ_PORT("IN0") AM_WRITE8(lamps_w,0x000000ff)
	AM_RANGE(0x06100004, 0x06100007) AM_READ_PORT("IN1")
	AM_RANGE(0x06100008, 0x0610000b) AM_READ_PORT("IN5")
	AM_RANGE(0x0610000c, 0x0610000f) AM_READ_PORT("IN6")
	AM_RANGE(0x06100010, 0x06100013) AM_READ_PORT("IN2") AM_WRITENOP
	AM_RANGE(0x06100014, 0x06100017) AM_READ_PORT("IN3")
	AM_RANGE(0x0610001c, 0x0610001f) AM_READWRITE8(analog_mux_r,analog_mux_w,0x000000ff) //AM_WRITENOP
	AM_RANGE(0x06200000, 0x06200fff) AM_RAM //network related?
	AM_RANGE(0x07fff000, 0x07ffffff) AM_RAM
	AM_RANGE(0x20000000, 0x2001ffff) AM_ROM AM_SHARE("share2")

	AM_RANGE(0x60000000, 0x600003ff) AM_WRITENOP
ADDRESS_MAP_END

WRITE8_MEMBER(coolridr_state::sound_to_sh1_w)
{
	sound_data = data;
}

static ADDRESS_MAP_START( system_h1_sound_map, AS_PROGRAM, 16, coolridr_state )
	AM_RANGE(0x000000, 0x07ffff) AM_RAM AM_REGION("scsp1",0) AM_SHARE("soundram")
	AM_RANGE(0x100000, 0x100fff) AM_DEVREADWRITE_LEGACY("scsp1", scsp_r, scsp_w)
	AM_RANGE(0x200000, 0x27ffff) AM_RAM AM_REGION("scsp2",0) AM_SHARE("soundram2")
	AM_RANGE(0x300000, 0x300fff) AM_DEVREADWRITE_LEGACY("scsp2", scsp_r, scsp_w)
	AM_RANGE(0x800000, 0x80ffff) AM_RAM
	AM_RANGE(0x900000, 0x900001) AM_WRITE8(sound_to_sh1_w,0x00ff)
ADDRESS_MAP_END



static const gfx_layout tiles8x8_layout =
{
	16,16,
	RGN_FRAC(1,1),
	8,
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120 },
	{ 0*128, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128, 8*128, 9*128, 10*128, 11*128, 12*128, 13*128, 14*128, 15*128 },
	16*128
};

#if 0
static const gfx_layout test =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 0*4,1*4,2*4,3*4,4*4,5*4,6*4, 7*4 },
	{ 0*8*4, 1*8*4, 2*8*4, 3*8*4, 4*8*4, 5*8*4, 6*8*4, 7*8*4 },
	8*8*4
};
#endif


static const gfx_layout fakeascii =
{
	16,16,
	512,
	4,
	{ 0,1,2,3 },
	{ 0*4,0*4,1*4,1*4,2*4,2*4,3*4,3*4,4*4,4*4,5*4,5*4,6*4,6*4, 7*4,7*4 },
	{ 0*8*4,0*8*4, 1*8*4,1*8*4, 2*8*4,2*8*4, 3*8*4,3*8*4, 4*8*4,4*8*4, 5*8*4,5*8*4, 6*8*4,6*8*4, 7*8*4,7*8*4 },
	8*8*4
};


static GFXDECODE_START( coolridr )
//  GFXDECODE_ENTRY( "maincpu_data", 0, tiles8x8_layout, 0, 16 )
	GFXDECODE_ENTRY( "gfx_data", 0, tiles8x8_layout, 0, 0x100 )
	GFXDECODE_ENTRY( "gfx5", 0, tiles8x8_layout, 0, 0x100 )
	GFXDECODE_ENTRY( "ram_gfx", 0, tiles8x8_layout, 0, 0x100 )
	GFXDECODE_ENTRY( "fakeascii", 0x18000, fakeascii, 0x1000, 16 )
GFXDECODE_END

static INPUT_PORTS_START( coolridr )
	PORT_START("IN0")
	PORT_DIPNAME( 0x00000001, 0x00000001, "IN0-0" )
	PORT_DIPSETTING(    0x00000001, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000002, 0x00000002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000002, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000004, 0x00000004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000008, 0x00000008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000010, 0x00000010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000020, 0x00000020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000040, 0x00000040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000080, 0x00000080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00010000, 0x00010000, "IN0-1" )
	PORT_DIPSETTING(    0x00010000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00020000, 0x00020000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00020000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00040000, 0x00040000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00040000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00080000, 0x00080000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00080000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00100000, 0x00100000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00100000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00200000, 0x00200000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00200000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00400000, 0x00400000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00400000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00800000, 0x00800000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00800000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_BIT( 0xff00ff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x00000003, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x00000004, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) PORT_NAME("P1 Music <<")
	PORT_BIT( 0x00000008, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) PORT_NAME("P1 Music >>")
	PORT_BIT( 0x00000010, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(1) PORT_NAME("P1 Shift Up")
	PORT_BIT( 0x00000020, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(1) PORT_NAME("P1 Shift Down")
	PORT_BIT( 0x000000c0, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0000ff00, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x00010000, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_NAME("P1 Coin")
	PORT_BIT( 0x00020000, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_NAME("P2 Coin")
	PORT_SERVICE_NO_TOGGLE( 0x00040000, IP_ACTIVE_LOW )
	PORT_BIT( 0x00080000, IP_ACTIVE_LOW, IPT_SERVICE1 ) PORT_NAME("P1 Service Switch")
	PORT_BIT( 0x00100000, IP_ACTIVE_LOW, IPT_START1 ) PORT_NAME("P1 Start")
	PORT_BIT( 0x00200000, IP_ACTIVE_LOW, IPT_START2 ) PORT_NAME("P2 Start")
	PORT_BIT( 0x00400000, IP_ACTIVE_LOW, IPT_SERVICE2 ) PORT_NAME("P2 Service Switch")
	PORT_BIT( 0x00800000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0xff000000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN2")
	PORT_DIPNAME( 0x00000001, 0x00000001, "IN2-0" )
	PORT_DIPSETTING(    0x00000001, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000002, 0x00000002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000002, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000004, 0x00000004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000008, 0x00000008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000010, 0x00000010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000020, 0x00000020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000040, 0x00000040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000080, 0x00000080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00010000, 0x00010000, "IN2-1" )
	PORT_DIPSETTING(    0x00010000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00020000, 0x00020000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00020000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00040000, 0x00040000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00040000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00080000, 0x00080000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00080000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00100000, 0x00100000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00100000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00200000, 0x00200000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00200000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00400000, 0x00400000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00400000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00800000, 0x00800000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00800000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_BIT( 0xff00ff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN3")
	PORT_DIPNAME( 0x00000001, 0x00000001, "IN3-0" )
	PORT_DIPSETTING(    0x00000001, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000002, 0x00000002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000002, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000004, 0x00000004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000008, 0x00000008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000010, 0x00000010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000020, 0x00000020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000040, 0x00000040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000080, 0x00000080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00010000, 0x00010000, "IN3-1" )
	PORT_DIPSETTING(    0x00010000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00020000, 0x00020000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00020000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00040000, 0x00040000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00040000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00080000, 0x00080000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00080000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00100000, 0x00100000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00100000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00200000, 0x00200000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00200000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00400000, 0x00400000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00400000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00800000, 0x00800000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00800000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_BIT( 0xff00ff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN4")
	PORT_DIPNAME( 0x00000001, 0x00000001, "IN4-0" )
	PORT_DIPSETTING(    0x00000001, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000002, 0x00000002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000002, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000004, 0x00000004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000008, 0x00000008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000010, 0x00000010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000020, 0x00000020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000040, 0x00000040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000080, 0x00000080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00010000, 0x00010000, "IN4-1" )
	PORT_DIPSETTING(    0x00010000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00020000, 0x00020000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00020000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00040000, 0x00040000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00040000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00080000, 0x00080000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00080000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00100000, 0x00100000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00100000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00200000, 0x00200000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00200000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00400000, 0x00400000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00400000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00800000, 0x00800000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00800000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_BIT( 0xff00ff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN5")
	PORT_DIPNAME( 0x00000001, 0x00000001, "IN5-0" )
	PORT_DIPSETTING(    0x00000001, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000002, 0x00000002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000002, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000004, 0x00000004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000008, 0x00000008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000010, 0x00000010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000020, 0x00000020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000040, 0x00000040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000080, 0x00000080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_BIT( 0x00030000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x00040000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2) PORT_NAME("P2 Music <<")
	PORT_BIT( 0x00080000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) PORT_NAME("P2 Music >>")
	PORT_BIT( 0x00100000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_PLAYER(2) PORT_NAME("P2 Shift Up")
	PORT_BIT( 0x00200000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_PLAYER(2) PORT_NAME("P2 Shift Down")
	PORT_BIT( 0x00c00000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0xff00ff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN6")
	PORT_DIPNAME( 0x00000001, 0x00000001, "IN6-0" )
	PORT_DIPSETTING(    0x00000001, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000002, 0x00000002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000002, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000004, 0x00000004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000004, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000008, 0x00000008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000008, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000010, 0x00000010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000010, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000020, 0x00000020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000020, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000040, 0x00000040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000040, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00000080, 0x00000080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00000080, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00010000, 0x00010000, "IN6-1" )
	PORT_DIPSETTING(    0x00010000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00020000, 0x00020000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00020000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00040000, 0x00040000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00040000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00080000, 0x00080000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00080000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00100000, 0x00100000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00100000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00200000, 0x00200000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00200000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00400000, 0x00400000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00400000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_DIPNAME( 0x00800000, 0x00800000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00800000, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00000000, DEF_STR( On ) )
	PORT_BIT( 0xff00ff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("AN0")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(1) PORT_NAME("P1 Handle Bar")

	PORT_START("AN1")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(50) PORT_KEYDELTA(25) PORT_PLAYER(1) PORT_REVERSE PORT_NAME("P1 Throttle")

	PORT_START("AN2")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(50) PORT_KEYDELTA(25) PORT_PLAYER(1) PORT_REVERSE PORT_NAME("P1 Brake")

	PORT_START("AN3")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("AN4")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(30) PORT_KEYDELTA(10) PORT_PLAYER(2) PORT_NAME("P2 Handle Bar")

	PORT_START("AN5")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(50) PORT_KEYDELTA(25) PORT_PLAYER(2) PORT_REVERSE PORT_NAME("P2 Throttle")

	PORT_START("AN6")
	PORT_BIT( 0xff, 0x00, IPT_PEDAL2 ) PORT_MINMAX(0x00,0xff) PORT_SENSITIVITY(50) PORT_KEYDELTA(25) PORT_PLAYER(2) PORT_REVERSE PORT_NAME("P2 Brake")

	PORT_START("AN7")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


// IRQs 4 & 6 are valid on SH-2
TIMER_DEVICE_CALLBACK_MEMBER(coolridr_state::system_h1_main)
{
	int scanline = param;

	if(scanline == 384)
		m_maincpu->set_input_line(4, HOLD_LINE);

	if(scanline == 0)
		m_maincpu->set_input_line(6, HOLD_LINE);

}

TIMER_DEVICE_CALLBACK_MEMBER(coolridr_state::system_h1_sub)
{
	int scanline = param;

	/* 10: reads from 0x4000000 (sound irq) */
	/* 12: reads from inputs (so presumably V-Blank) */
	/* 14: tries to r/w to 0x62***** area (network irq?) */

	if(scanline == 384)
		m_subcpu->set_input_line(0xc, HOLD_LINE);
}


void coolridr_state::machine_start()
{
//  machine().device("maincpu")->execute().set_input_line(INPUT_LINE_HALT, ASSERT_LINE);
//	machine().device("soundcpu")->execute().set_input_line(INPUT_LINE_HALT, ASSERT_LINE);

//	memcpy(memregion("soundcpu")->base(), memregion("maincpu")->base()+0x100000, 0x80000);
//	m_soundcpu->reset();
}

void coolridr_state::machine_reset()
{
//  machine().device("maincpu")->execute().set_input_line(INPUT_LINE_HALT, ASSERT_LINE);
	machine().device("soundcpu")->execute().set_input_line(INPUT_LINE_RESET, ASSERT_LINE);

//	memcpy(m_soundram, memregion("soundcpu")->base()+0x80000, 0x80000);
//  m_soundcpu->reset();


}


static I8237_INTERFACE( dmac_intf )
{
	DEVCB_NULL, //DEVCB_DRIVER_LINE_MEMBER(coolridr_state, coolridr_dma_hrq_changed),
	DEVCB_NULL, //DEVCB_DRIVER_LINE_MEMBER(coolridr_state, coolridr_tc_w),
	DEVCB_NULL, //DEVCB_DRIVER_MEMBER(coolridr_state, coolridr_dma_read_byte),
	DEVCB_NULL,//DEVCB_DRIVER_MEMBER(coolridr_state, coolridr_dma_write_byte),
	{ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL },
	{ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_NULL },
	{ DEVCB_NULL /*DEVCB_DRIVER_LINE_MEMBER(coolridr_state, coolridr_dack0_w)*/,
		DEVCB_NULL/*DEVCB_DRIVER_LINE_MEMBER(coolridr_state, coolridr_dack1_w)*/,
		DEVCB_NULL/*DEVCB_DRIVER_LINE_MEMBER(coolridr_state, coolridr_dack2_w)*/,
		DEVCB_NULL/*DEVCB_DRIVER_LINE_MEMBER(coolridr_state, coolridr_dack3_w)*/ }
};

static void scsp_irq(device_t *device, int irq)
{
	coolridr_state *state = device->machine().driver_data<coolridr_state>();
	if (irq > 0)
	{
		state->m_scsp_last_line = irq;
		device->machine().device("soundcpu")->execute().set_input_line(irq, ASSERT_LINE);
	}
	else
		device->machine().device("soundcpu")->execute().set_input_line(-irq, CLEAR_LINE);
}

WRITE_LINE_MEMBER(coolridr_state::scsp_to_main_irq)
{
	m_subcpu->set_input_line(0xe, HOLD_LINE);
}

static const scsp_interface scsp_config =
{
	0,
	scsp_irq,
	DEVCB_DRIVER_LINE_MEMBER(coolridr_state, scsp_to_main_irq)
};

static const scsp_interface scsp2_interface =
{
	0,
	NULL,
	DEVCB_DRIVER_LINE_MEMBER(coolridr_state, scsp_to_main_irq)
};

#define MAIN_CLOCK XTAL_28_63636MHz

static MACHINE_CONFIG_START( coolridr, coolridr_state )
	MCFG_CPU_ADD("maincpu", SH2, MAIN_CLOCK)  // 28 mhz
	MCFG_CPU_PROGRAM_MAP(system_h1_map)
	MCFG_TIMER_DRIVER_ADD_SCANLINE("scantimer", coolridr_state, system_h1_main, "lscreen", 0, 1)

	MCFG_CPU_ADD("soundcpu", M68000, 11289600) //256 x 44100 Hz = 11.2896 MHz
	MCFG_CPU_PROGRAM_MAP(system_h1_sound_map)

	MCFG_CPU_ADD("sub", SH1, 16000000)  // SH7032 HD6417032F20!! 16 mhz
	MCFG_CPU_PROGRAM_MAP(coolridr_submap)
	MCFG_TIMER_DRIVER_ADD_SCANLINE("scantimer2", coolridr_state, system_h1_sub, "lscreen", 0, 1)

	MCFG_I8237_ADD("i8237", 16000000, dmac_intf)

	MCFG_GFXDECODE(coolridr)

	MCFG_SCREEN_ADD("lscreen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_SIZE(640, 512)
	MCFG_SCREEN_VISIBLE_AREA(0,495, 0, 383) // the game uses this resolution
	//MCFG_SCREEN_VISIBLE_AREA(0,639, 0, 479) // the 'for use in Japan screen uses this resolution' (Outrunners also uses the higher res for this screen on system 32..)
	MCFG_SCREEN_UPDATE_DRIVER(coolridr_state, screen_update_coolridr1)

	MCFG_SCREEN_ADD("rscreen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_SIZE(640, 512)
	MCFG_SCREEN_VISIBLE_AREA(0,495, 0, 383) // the game uses this resolution
	//MCFG_SCREEN_VISIBLE_AREA(0,639, 0, 479) // the 'for use in ... screen uses this resolution'
	MCFG_SCREEN_UPDATE_DRIVER(coolridr_state, screen_update_coolridr2)


	MCFG_PALETTE_LENGTH(0x10000)
	MCFG_DEFAULT_LAYOUT(layout_dualhsxs)

	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")
	MCFG_SOUND_ADD("scsp1", SCSP, 0)
	MCFG_SOUND_CONFIG(scsp_config)
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(0, "rspeaker", 1.0)

	MCFG_SOUND_ADD("scsp2", SCSP, 0)
	MCFG_SOUND_CONFIG(scsp2_interface)
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(0, "rspeaker", 1.0)
MACHINE_CONFIG_END

ROM_START( coolridr )
	ROM_REGION( 0x200000, "maincpu", 0 ) /* SH2 code */
	ROM_LOAD32_WORD_SWAP( "ep17659.30", 0x0000000, 0x080000, CRC(473027b0) SHA1(acaa212869dd79550235171b9f054e82750f74c3) )
	ROM_LOAD32_WORD_SWAP( "ep17658.29", 0x0000002, 0x080000, CRC(7ecfdfcc) SHA1(97cb3e6cf9764c8db06de12e4e958148818ef737) )
	ROM_LOAD32_WORD_SWAP( "ep17661.32", 0x0100000, 0x080000, CRC(81a7d90b) SHA1(99f8c3e75b94dd1b60455c26dc38ce08db82fe32) )
	ROM_LOAD32_WORD_SWAP( "ep17660.31", 0x0100002, 0x080000, CRC(27b7a507) SHA1(4c28b1d18d75630a73194b5d4fd166f3b647c595) )

	/* Page 12 of the service manual states that these 4 regions are tested, so I believe that they are read by the SH-2 */
	ROM_REGION32_BE( 0x1000000, "gfx_data", 0 ) /* SH2 code */
	ROM_LOAD32_WORD_SWAP( "mp17650.11", 0x0000002, 0x0200000, CRC(0ccc84a1) SHA1(65951685b0c8073f6bd1cf9959e1b4d0fc6031d8) )
	ROM_LOAD32_WORD_SWAP( "mp17651.12", 0x0000000, 0x0200000, CRC(25fd7dde) SHA1(a1c3f3d947ce20fbf61ea7ab235259be9b7d35a8) )
	ROM_LOAD32_WORD_SWAP( "mp17652.13", 0x0400002, 0x0200000, CRC(be9b4d05) SHA1(0252ba647434f69d6eacb4efc6f55e6af534c7c5) )
	ROM_LOAD32_WORD_SWAP( "mp17653.14", 0x0400000, 0x0200000, CRC(64d1406d) SHA1(779dbbf42a14a6be1de9afbae5bbb18f8f36ceb3) )
	ROM_LOAD32_WORD_SWAP( "mp17654.15", 0x0800002, 0x0200000, CRC(5dee5cba) SHA1(6e6ec8574bdd35cc27903fc45f0d4a36ce9df103) )
	ROM_LOAD32_WORD_SWAP( "mp17655.16", 0x0800000, 0x0200000, CRC(02903cf2) SHA1(16d555fda144e0f1b62b428e9158a0e8ebf7084e) )
	ROM_LOAD32_WORD_SWAP( "mp17656.17", 0x0c00002, 0x0200000, CRC(945c89e3) SHA1(8776d74f73898d948aae3c446d7c710ad0407603) )
	ROM_LOAD32_WORD_SWAP( "mp17657.18", 0x0c00000, 0x0200000, CRC(74676b1f) SHA1(b4a9003a052bde93bebfa4bef9e8dff65003c3b2) )

	ROM_REGION32_BE( 0x100000, "ram_gfx", ROMREGION_ERASE00 ) /* SH2 code */

	ROM_REGION( 0x100000, "soundcpu", ROMREGION_ERASE00 )   /* 68000 */
	ROM_COPY( "maincpu", 0x100000, 0x080000, 0x080000 ) //hardcoded from SH-2 roms? no, It doesn't seem so... (missing a DMA transfer for it?)

	ROM_REGION( 0x100000, "sub", 0 ) /* SH1 */
	ROM_LOAD16_WORD_SWAP( "ep17662.12", 0x000000, 0x020000,  CRC(50d66b1f) SHA1(f7b7f2f5b403a13b162f941c338a3e1207762a0b) )

	/* these are compressed sprite data */
	ROM_REGION( 0x2800000, "gfx5", ROMREGION_ERASEFF )
	/* logical interleaving according to the readme? */
	ROM_LOAD32_WORD_SWAP( "mpr-17640.ic1", 0x0000002, 0x0400000, CRC(981e3e69) SHA1(d242055e0359ec4b5fac4676b2f974fbc974cc68) )
	ROM_LOAD32_WORD_SWAP( "mpr-17645.ic6", 0x0000000, 0x0400000, CRC(56968d07) SHA1(e88c3d66ea05affb4681a25d155f097bd1b5a84b) )
	ROM_LOAD32_WORD_SWAP( "mpr-17641.ic2", 0x0800002, 0x0400000, CRC(fccc3dae) SHA1(0df7fd8b1110ba9063dc4dc40301267229cb9a35) )
	ROM_LOAD32_WORD_SWAP( "mpr-17646.ic7", 0x0800000, 0x0400000, CRC(b77eb2ad) SHA1(b832c0f1798aca39adba840d56ae96a75346670a) )
	ROM_LOAD32_WORD_SWAP( "mpr-17642.ic3", 0x1000002, 0x0400000, CRC(1a5bcc73) SHA1(a7df04c0a326323ea185db5f55b3e0449d76c535) )
	ROM_LOAD32_WORD_SWAP( "mpr-17647.ic8", 0x1000000, 0x0400000, CRC(9dd9330c) SHA1(c91a7f497c1f4bd283bd683b06dff88893724d51) )
	ROM_LOAD32_WORD_SWAP( "mpr-17643.ic4", 0x1800002, 0x0400000, CRC(5100f23b) SHA1(659c2300399ff1cbd24fb1eb18cfd6c26e06fd96) )
	ROM_LOAD32_WORD_SWAP( "mpr-17648.ic9", 0x1800000, 0x0400000, CRC(bf184cce) SHA1(62c004ea279f9a649d21426369336c2e1f9d24da) )
	ROM_LOAD32_WORD_SWAP( "mpr-17644.ic5", 0x2000002, 0x0400000, CRC(80199c79) SHA1(e525d8ee9f9176101629853e50cca73b02b16a38) )
	ROM_LOAD32_WORD_SWAP( "mpr-17649.ic10",0x2000000, 0x0400000, CRC(618c47ae) SHA1(5b69ad36fcf8e70d34c3b2fc71412ce953c5ceb3) )

	ROM_REGION( 0x80000, "scsp1", 0 )   /* first SCSP's RAM */
	ROM_FILL( 0x000000, 0x80000, 0 )

	ROM_REGION( 0x80000, "scsp2", 0 )   /* second SCSP's RAM */
	ROM_FILL( 0x000000, 0x80000, 0 )


	ROM_REGION( 0x2800000, "fakeascii", ROMREGION_ERASEFF )
#ifdef FAKE_ASCII_ROM
	ROM_LOAD( "video", 0x000000, 0x020000,  CRC(8857ec5a) SHA1(5bed14933af060cb4a1ce6a961c4ca1467a1cbc2) ) // dump of the orunners video ram so we can use the charset (its 8x8 not 16x16 tho, but who cares)
#endif
ROM_END


/*
TODO: both irq routines writes 1 to 0x60d8894, sets up the Watchdog timer then expect that this buffer goes low IN the irq routines.
	  The Watchdog Timer is setted up with these params:
	  0xee for wtcnt
	  0x39 for wtcsr (enable irq (bit 5), enable timer (bit 4), clock select divider / 64 (bits 2-0))
	  vector is 0x7f (so VBR+0x1fc)
	  level is 0xf
... and indeed the Watchdog irq routine effectively clears this RAM buffer. What the manual doesn't say is that the Watchdog timer irq
    presumably is an NMI if this is even possible ...
*/
READ32_MEMBER(coolridr_state::coolridr_hack2_r)
{
	offs_t pc = downcast<cpu_device *>(&space.device())->pc();
	if(pc == 0x6002cba || pc == 0x6002d42)
		return 0;

	return m_sysh1_workram_h[0xd8894/4];
}

DRIVER_INIT_MEMBER(coolridr_state,coolridr)
{
//  machine().device("maincpu")->memory().space(AS_PROGRAM).install_legacy_read_handler(0x60d88a4, 0x060d88a7, FUNC(coolridr_hack1_r) );
	machine().device("maincpu")->memory().space(AS_PROGRAM).install_read_handler(0x60d8894, 0x060d8897, read32_delegate(FUNC(coolridr_state::coolridr_hack2_r), this));
}

GAME( 1995, coolridr,    0, coolridr,    coolridr, coolridr_state,    coolridr, ROT0,  "Sega", "Cool Riders",GAME_NOT_WORKING|GAME_NO_SOUND ) // region is set in test mode
