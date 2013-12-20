/***************************************************************************************************
    DEC Rainbow 100

    Driver-in-progress by R. Belmont and Miodrag Milanovic.
	Portions (2013) by Karl-Ludwig Deisenhofer (VT video, floppy, preliminary keyboard, DIP switches).

    STATE AS OF DECEMBER 2013
    --------------------------
	- FLOPPY TIMING: 'wd17xx_complete_command' * must * be hard wired to about 13 usecs.
	  Line 1063 in 'wd17xx.c' has to be changed (until legacy code here is removed):
	  -      w->timer_cmd->adjust(attotime::from_usec(usecs));
	  +      w->timer_cmd->adjust(attotime::from_usec(13)); 

    - WORKAROUND AVAILABLE: keyboard emulation incomplete (inhibits the system from booting with ERROR 50 on cold or ERROR 13 on warm boot).
	- NOT WORKING: serial (ERROR 60). 
    - NOT WORKING: printer interface (ERROR 40). Like error 60 not mission-critical.

    - NON-CRITICAL: watchdog logic (triggered after 108 ms without interrupts on original machine) still does not work as intended.

					Timer is reset by TWO sources: the VERT INT L from the DC012, or the MHFU ENB L from the enable flip-flop.
					The MHFU gets active if the 8088 has not acknowledged a video processor interrupt within approx. 108 milliseconds.
	
					BIOS assumes a power-up reset if MHFU detection is disabled - and assumes a MHFU reset if MHFU detection is ENABLED.
					
					As there is no reset switch, only a limited software reset exists on a real DEC-100 (CTRL-SETUP within SETUP).

    - TO BE IMPLEMENTED AS SLOT DEVICES (for now, DIP settings affect 'system_parameter_r' only and are disabled):
            * Color graphics option (uses NEC upd7220 GDC)
            * Extended communication option (same as BUNDLE_OPTION ?)

    - OTHER UPGRADES (NEC_V20 should be easy, the TURBOW is harder to come by)
			* Suitable Solutions TURBOW286: 12 Mhz, 68-pin, low power AMD N80L286-12 and WAYLAND/EDSUN EL286-88-10-B ( 80286 to 8088 Processor Signal Converter )
			  plus DC 7174 or DT 7174 (barely readable). Add-on card, replaces main 8088 cpu (via ribbon cable). Altered BOOT ROM labeled 'TBSS1.3 - 3ED4'.

			* NEC_V20 (requires modded BOOT ROM because of - at least 2 - hard coded timing loops): 
                 100A:         100B/100+:						100B+ ALTERNATE RECOMMENDATION (fixes RAM size auto-detection problems when V20 is in place.
	                                                            Tested on a 30+ year old live machine. Your mileage may vary)

                 Location Data	Location Data                   Loc.|Data								
	             ....     ..    ....     ..  ------------------ 00C6 46 [ increases 'wait for Z80' from approx. 27,5 ms (old value 40) to 30,5 ms ]
	             ....     ..    ....     ..  ------------------ 0303 00 [ disable CHECKSUM ]
                 043F     64    072F     64	<----------------->	072F 73 [ increases minimum cycle time from 2600 (64) to 3000 ms (73) ]
                 067D	  20	0B36     20	<-----------------> 0B36 20 [ USE A VALUE OF 20 FOR THE NEC - as in the initial patch! CHANGES CAUSE VFR-ERROR 10 ]
                 1FFE     2B	3FFE     1B  (BIOS CHECKSUM)    
                 1FFF     70	3FFF     88  (BIOS CHECKSUM)    

			 => the 'leaked' DOS 3.10 Beta -for Rainbow- 'should not be used' on rigs with NEC V20. It possibly wasn't tested, but boots and runs well.
			 => on the NEC, auto detection (of option RAM) fails with the original V20 patch (above, left)
			    Expect RAM related system crashes after swapping CPUs and altering physical RAM _afterwards_.
				Hard coded CPU loops are to blame. Try values from the alternate patch (right). 
			 => AAD/AAM - Intel 8088 honors the second byte (operand), NEC V20 ignores it and always uses base 0Ah (10). 
			 => UNDOCUMENTED: NEC V20 does not have "POP CS" (opcode 0F). There are more differences (opcode D6; the 2 byte POP: 8F Cx; FF Fx instructions)
			    Commercial programs had to be patched back then (as was the case with Loderunner for PC).
			 => NEW OPCODES: REPC, REPNC, CHKIND, PREPARE, DISPOSE; BCD string operations (ADD4S, CMP4S, SUB4S), bit-ops (NOT, SET, TEST, ROL4, ROR4)
			    WARNING: undoc'd opcodes, INS, EXT and 8080 behaviour are unemulated yet! MESS' CPU source has up-to-date info. 

    Meaning of Diagnostics LEDs (from PC100ESV1.PDF found, e.g.,
    on ftp://ftp.update.uu.se/pub/rainbow/doc/rainbow-docs/

    Internal Diagnostic Messages                               F
    Msg Message                               Lights Display   A
    No.                                       * = on o = off   T
                                              - = on or off    A
                                              1 2 3 4 5 6 7    L
    --------------------------------------------------------------
     1  Main Board (Video)                    o * * o * o *   Yes
     2  Main Board* (unsolicited interrupt)   * * * * o * o   Yes
     3  Drive A or B (index)                  o o * o o * *
     4  Drive A or B (motor)                  * * o o o * *
     5  Drive A or B (seek)                   o * o o o * *
     6  Drive A or B (read)                   * o o o o * *
     7  Drive A or B (restore)                o * * o o * *
     8  Drive A or B (step)                   * o * o o * *
     9  System Load incomplete+ (System Load) o o o o o o o
    10  Main Board (video, vfr)               * * * o * o *   Yes
    11  System Load incomplete+ (Boot Load)   o o o o o o o
    12  Drive A or B (not ready)              o o o o o * *
    13  Keyboard                              * * o * o * o   Yes
    14  Main Board (nvm data)                 * * * * o * *
    15  (no msg. 15 in that table)
    16  Interrupts off*                       * * * o o o o   Cond.
    17  Main Board (video RAM)                * * * o * * o   Yes
    18  Main Board (Z80 crc)                  * * * * o o *   Yes
    19  Main Board RAM (0-64K)                - - - * * o *   Yes
    20  Main Board (unsolicited int., Z80)    * * * o o o *   Yes
    21  Drive Not Ready+                      o o o o o o o
    22  Remove Card or Diskette               o * * o o o *
    23  Non-System Diskette+                  o o o o o o o
    24  new memory size = nnnK                o o o o o o o
    25  Set Up Defaults stored                o o o o o o o
    26  Main Board (RAM arbitration)          * * * o * o o   Yes
    27  Main Board (RAM option)               - - - * * o o
    28  RX50 controller board                 * * * o o * *
    29  Main Board* (Z80 response)            * * * * o o o
    30  Main Board (ROM crc, ROM 0)           * * * * * * *   Yes
    31  Main Board (ROM crc, ROM 1)           * * * * * * o   Yes
    -   Main Board (ROM crc, ROM 2)           * * * o * * *   Yes
    33  Main Board (contention)               o o o o o * o   Yes
    40  Main Board (printer port)             * o * * o * o
    50  Main Board (keyboard port)            o o * * o * o   Yes
    60  Main Board (comm port)                o * * * o * o

    --------------------------------------------------------------
    *   These errors can occur at any time because the circuits
        are monitored constantly
    +   These messages may occur during power-up if auto boot is
        selected

PCB layout
----------

DEC-100 model B
= part no.70-19974-02 according to document EK-RB100-TM_001

PCB # 5416206 / 5016205-01C1:

        7-6-5-4 |3-2-1
        DIAGNOSTIC-LEDs |J3   | |J2     | |J1    |
|------|----8088|Z80-|--|VIDEO|-|PRINTER|-|SERIAL|---|
|  2 x 64 K             |/KBD.|                 !!!!!|
|  R  A  M             NEC D7201C            |P|!W90!|
|                                            |O|!!!!!|
|   [W6]   ROM 1       INTEL 8088            |W|     |
|          (23-020e5-00)                     |E|     |
|                                            |R|     |
| ...J5..  BOOT ROM 0      ...J4...          =J8     |
| ...J6... (23-022e5-00)                             |
| [W5]                                               |
|                                                    |
|     INTEL 8251A   ZILOG Z 80A                      |
|                [W18]                               |
| A  4x                74 LS 244                     |
| M  S           [W15]                               |
| 9  -   DEC-DC011     74 LS 245                     |
| 1  R           [W14]                               |
| 2  A                  [W13]                        |
| 8  M   CHARGEN.-                                   |
|        ROM (4K)           ...J7...  | ...J9 = RX50 |
|------------PCB# 5416206 / 5016205-01C1-------------|
NOTES
W5 + W6 are out when 16K x 8 EPROMS are used  
/ W5 + W6 installed => 32 K x 8 EPROMs (pin 27 = A14)

W13, W14, W15, W18 = for manufacturing tests.
=> W13 - W15 affect diagnostic read register (port $0a)
=> W18 pulls DSR to ground and affects 8251A - port $11 (bit 7)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!! DO NOT SHORT JUMPER / CONNECTOR [W90] ON LIVE HARDWARE  !! 
!!                                                         !!   
!! WARNING:  CIRCUIT DAMAGE could occur if this jumper is  !!
!! set by end users.        See PDF document AA-V523A-TV.  !! 
!!                                                         !!   
!! W90 connects to pin 2 (Voltage Bias on PWR connector J8)!! 
!! and is designed FOR ===> FACTORY TESTS OF THE PSU <===  !!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

WIRE CONNECTORS - SEEN ON SCHEMATICS - NOT PRESENT ON DEC-100 B (-A only?):
W16 pulls J2 printer port pin 1 to GND when set (chassis to logical GND).
W17 pulls J1 serial  port pin 1 to GND when set (chassis to logical GND).
****************************************************************************/

// Workarounds DO NOT APPLY to the 190-B ROM. Only enable when compiling the 'rainbow' driver -
//#define FORCE_RAINBOW_100_LOGO 
#define KBD_DELAY 875 // (debounce delay). Recommended: 875. 

#include "emu.h"
#include "cpu/i86/i86.h"
#include "cpu/z80/z80.h"
#include "video/vtvideo.h"

#include "machine/wd17xx.h"
#include "imagedev/flopdrv.h"
#include "formats/basicdsk.h"

#include "machine/i8251.h"
#include "machine/dec_lk201.h"
#include "sound/beep.h"
#include "machine/nvram.h"

#include "rainbow.lh" // BEZEL - LAYOUT with LEDs for diag 1-7, keyboard 8-11 and floppy 20-23

class rainbow_state : public driver_device
{
public:
	rainbow_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_inp1(*this, "W13"),
			m_inp2(*this, "W14"),
			m_inp3(*this, "W15"),
			m_inp4(*this, "W18"),
			m_inp5(*this, "BUNDLE OPTION"),
			m_inp6(*this, "FLOPPY CONTROLLER"),
			m_inp7(*this, "GRAPHICS OPTION"),
			m_inp8(*this, "MEMORY PRESENT"),
			m_inp9(*this, "MONITOR TYPE"),

		m_beep(*this, "beeper"),
		m_crtc(*this, "vt100_video"),
		m_i8088(*this, "maincpu"),
		m_z80(*this, "subcpu"),
		m_fdc(*this, "wd1793"),
		m_kbd8251(*this, "kbdser"),
		m_lk201(*this, LK201_TAG),
		m_p_ram(*this, "p_ram"),

		m_p_vol_ram(*this, "vol_ram"),
		m_p_nvram(*this, "nvram"),

		m_shared(*this, "sh_ram"),
		m_maincpu(*this, "maincpu") { }

	required_ioport m_inp1;
	required_ioport m_inp2;
	required_ioport m_inp3;
	required_ioport m_inp4;
	required_ioport m_inp5;
	required_ioport m_inp6;
	required_ioport m_inp7;
	required_ioport m_inp8;
	required_ioport m_inp9;

	required_device<beep_device> m_beep;

	required_device<rainbow_video_device> m_crtc;
	required_device<cpu_device> m_i8088;
	required_device<cpu_device> m_z80;
	required_device<fd1793_device> m_fdc;
	required_device<i8251_device> m_kbd8251;
	required_device<lk201_device> m_lk201;
	required_shared_ptr<UINT8> m_p_ram;
	required_shared_ptr<UINT8> m_p_vol_ram;
	required_shared_ptr<UINT8> m_p_nvram;
	required_shared_ptr<UINT8> m_shared;
	UINT8 m_diagnostic;

	virtual void machine_start();

	DECLARE_READ8_MEMBER(read_video_ram_r);
	DECLARE_WRITE8_MEMBER(clear_video_interrupt);

	DECLARE_READ8_MEMBER(diagnostic_r);
	DECLARE_WRITE8_MEMBER(diagnostic_w);

	DECLARE_READ8_MEMBER(comm_control_r);
	DECLARE_WRITE8_MEMBER(comm_control_w);

	DECLARE_READ8_MEMBER(share_z80_r);
	DECLARE_WRITE8_MEMBER(share_z80_w);

	DECLARE_READ8_MEMBER(floating_bus_r);
	DECLARE_WRITE8_MEMBER(floating_bus_w);

	DECLARE_READ8_MEMBER(hd_status_68_r);
		// EMULATOR TRAP TO INTERCEPT KEYBOARD cmd in AH and PARAMETER in AL (port 90 = AL / port 91 = AH)
		// TODO: beeper and led handling should better be handled by LK201 code.
	DECLARE_WRITE8_MEMBER(PORT90_W);
	DECLARE_WRITE8_MEMBER(PORT91_W);

	DECLARE_READ8_MEMBER(i8088_latch_r);
	DECLARE_WRITE8_MEMBER(i8088_latch_w);
	DECLARE_READ8_MEMBER(z80_latch_r);
	DECLARE_WRITE8_MEMBER(z80_latch_w);

	DECLARE_WRITE8_MEMBER(z80_diskdiag_read_w);
	DECLARE_WRITE8_MEMBER(z80_diskdiag_write_w);

	DECLARE_READ8_MEMBER(z80_generalstat_r);

	DECLARE_READ8_MEMBER(z80_diskstatus_r);
	DECLARE_WRITE8_MEMBER(z80_diskcontrol_w);

	DECLARE_READ8_MEMBER(system_parameter_r);

	DECLARE_READ_LINE_MEMBER(dsr_r);

	DECLARE_READ_LINE_MEMBER(kbd_rx);
	DECLARE_WRITE_LINE_MEMBER(kbd_tx);
	DECLARE_WRITE_LINE_MEMBER(kbd_rxready_w);
	DECLARE_WRITE_LINE_MEMBER(kbd_txready_w);

	bool m_SCREEN_BLANK;

	int INT88, INTZ80;

	bool m_zflip;                   // Z80 alternate memory map with A15 inverted
	bool m_z80_halted;
	int  m_z80_diskcontrol;         // retains values needed for status register

	bool m_kbd_tx_ready, m_kbd_rx_ready;
	int m_KBD;

	int m_beep_counter;
	int MOTOR_DISABLE_counter;

	int COLD_BOOT;
private:
	UINT8 m_z80_private[0x800];     // Z80 private 2K
	UINT8 m_z80_mailbox, m_8088_mailbox;

	void update_kbd_irq();
	virtual void machine_reset();

	int m_unit;
	device_t *m_image[4];

public:
	UINT32 screen_update_rainbow(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(vblank_irq);
	TIMER_DEVICE_CALLBACK_MEMBER(keyboard_tick);
	required_device<cpu_device> m_maincpu;
};


void rainbow_state::machine_start()
{  
	m_image[0] = subdevice(FLOPPY_0);
	m_image[1] = subdevice(FLOPPY_1);
	m_image[2] = subdevice(FLOPPY_2);
	m_image[3] = subdevice(FLOPPY_3);

	COLD_BOOT = 1;

	m_SCREEN_BLANK = false;

	save_item(NAME(m_z80_private));
	save_item(NAME(m_z80_mailbox));
	save_item(NAME(m_8088_mailbox));
	save_item(NAME(m_zflip));
	save_item(NAME(m_kbd_tx_ready));
	save_item(NAME(m_kbd_rx_ready));

	UINT8 *rom = memregion("maincpu")->base();

	
#ifdef FORCE_RAINBOW_100_LOGO
 rom[0xf4174]=0xeb; // jmps  RAINBOW100_LOGO__loc_33D
 rom[0xf4175]=0x08;  

 rom[0xf4000 + 0x364a]= 0x0a;  
 rom[0xf4384]=0xeb; // JMPS  =>  BOOT80 
#endif

	// Enables PORT90_W + PORT91_W via BIOS call (offset +$21 in HIGH ROM)
	// F8 / FC ROM REGION (CHECK + PATCH)
	if(rom[0xfc000 + 0x0022] == 0x22 && rom[0xfc000 + 0x0023] == 0x28)
	{
			rom[0xf4303]=0x00; // Disable CRC CHECK (F0 / F4 ROM)

			rom[0xfc000 + 0x0022] =0xfe;  // jmp to offset $1922
			rom[0xfc000 + 0x0023] =0x18;

			rom[0xfc000 + 0x1922] =0xe6;  // out 90,al
			rom[0xfc000 + 0x1923] =0x90;

			rom[0xfc000 + 0x1924] =0x86;  //  xchg al,ah
			rom[0xfc000 + 0x1925] =0xc4;

			rom[0xfc000 + 0x1926] =0xe6;  // out 91,al
			rom[0xfc000 + 0x1927] =0x91;

			rom[0xfc000 + 0x1928] =0x86;  // xchg al,ah
			rom[0xfc000 + 0x1929] =0xc4;

			rom[0xfc000 + 0x192a] =0xe9;  // jmp (original jump offset $2846) e9 + 19 0f
			rom[0xfc000 + 0x192b] =0x19;
			rom[0xfc000 + 0x192c] =0x0f;
	}

}

static ADDRESS_MAP_START( rainbow8088_map, AS_PROGRAM, 8, rainbow_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x00000, 0x0ffff) AM_RAM AM_SHARE("sh_ram")
	AM_RANGE(0x10000, 0x1ffff) AM_RAM

	// test at f4e00 - f4e1c
	AM_RANGE(0x20000, 0xdffff) AM_READWRITE(floating_bus_r,floating_bus_w)
	AM_RANGE(0x20000, 0xdffff) AM_RAM

	// Documentation claims there is a 256 x 4 bit NVRAM from 0xed000 to 0xed040 (*)
	//   shadowed at $ec000 - $ecfff and from $ed040 - $edfff.

	//  - PC-100 A might have had a smaller (NV-)RAM (*)
	//  - ED000 - ED0FF is the area the _DEC-100-B BIOS_ accesses - and checks.

	//  - Specs say that the CPU has direct access to volatile RAM only.
	//    So NVRAM is hidden now and loads & saves are triggered within the 
	//    'diagnostic_w' handler (similar to real hardware).

	//  - Address bits 8-12 are ignored (-> AM_MIRROR). 
	AM_RANGE(0xed000, 0xed0ff) AM_RAM AM_SHARE("vol_ram") AM_MIRROR(0x1f00) 
	AM_RANGE(0xed100, 0xed1ff) AM_RAM AM_SHARE("nvram") 

	AM_RANGE(0xee000, 0xeffff) AM_RAM AM_SHARE("p_ram")
	AM_RANGE(0xf0000, 0xfffff) AM_ROM
ADDRESS_MAP_END

static ADDRESS_MAP_START( rainbow8088_io , AS_IO, 8, rainbow_state)
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE (0x00, 0x00) AM_READWRITE(i8088_latch_r, i8088_latch_w)

	// 0x02 Communication status / control register (8088)
	AM_RANGE (0x02, 0x02) AM_READWRITE(comm_control_r, comm_control_w)

	// 0x04 Video processor DC011
	AM_RANGE (0x04, 0x04) AM_DEVWRITE("vt100_video", rainbow_video_device, dc011_w)

	// TODO: unmapped [06] : Communication bit rates (see page 21 of PC 100 SPEC)

	AM_RANGE (0x08, 0x08) AM_READ(system_parameter_r)

	AM_RANGE (0x0a, 0x0a) AM_READWRITE(diagnostic_r, diagnostic_w)

	// 0x0C Video processor DC012 
	AM_RANGE (0x0c, 0x0c) AM_DEVWRITE("vt100_video", rainbow_video_device, dc012_w) 

	// TODO: unmapped [0e] : PRINTER BIT RATE REGISTER (WO)

	AM_RANGE(0x10, 0x10) AM_DEVREADWRITE("kbdser", i8251_device, data_r, data_w)
	AM_RANGE(0x11, 0x11) AM_DEVREADWRITE("kbdser", i8251_device, status_r, control_w)

	// UNMAPPED:
	// 0x20 - 0x2f ***** EXTENDED COMM. OPTION (option select 1)- for example:
	// 0x27		(RESET EXTENDED COMM OPTION) - OUT 27 @ offset 1EA7 

	// 0x40  COMMUNICATIONS DATA REGISTER (MPSC)
	// 0x41  PRINTER DATA REGISTER (MPSC)
	// 0x42  COMMUNICATIONS CONTROL / STATUS REGISTER (MPSC)
	// 0x43  PRINTER CONTROL / STATUS REGISTER (MPSC)

	// 0x50 - 0xf  ***** OPTIONAL COLOR GRAPHICS - for example:
	// 0x50     (RESET_GRAPH. OPTION) - OUT 50 @ offsets F5EB5 + F5EB9 

	// ===========================================================
	// TODO: hard disc emulation!                                
	// ------ Rainbow uses 'WD 1010 AL' (Western Digital 1983)   
	//        Register compatible to WD2010 (present in MESS)    
	// R/W REGISTERS 60 - 68 (?)                                 
	// ===========================================================
	// HARD DISC SIZES AND LIMITS
	//   HARDWARE: 
	//      Controller has a built-in limit of 8 heads / 1024 cylinders (67 MB). Standard geometry is 4 surfaces. 
	//   SOFTWARE: the DEC boot loader (and FDISK from DOS 3.10) initially allowed a maximum hard disc size of 20 MB. 
    //   - DOS 3 has a 1024 cylinder limit (32 MB). 
    //   - the custom boot loader that comes with 'WUTIL 3.2' allows 117 MB and 8 surfaces.
	AM_RANGE (0x68, 0x68) AM_READ(hd_status_68_r)

	AM_RANGE (0x90, 0x90) AM_WRITE(PORT90_W)
	AM_RANGE (0x91, 0x91) AM_WRITE(PORT91_W)
ADDRESS_MAP_END

static ADDRESS_MAP_START(rainbowz80_mem, AS_PROGRAM, 8, rainbow_state)
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE( 0x0000, 0xffff ) AM_READWRITE(share_z80_r, share_z80_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( rainbowz80_io, AS_IO, 8, rainbow_state)
	ADDRESS_MAP_UNMAP_HIGH
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x00, 0x00) AM_READWRITE(z80_latch_r, z80_latch_w)
	AM_RANGE(0x20, 0x20) AM_READWRITE(z80_generalstat_r, z80_diskdiag_read_w) // read to port 0x20 used by MS-DOS 2.x diskette loader.
	AM_RANGE(0x21, 0x21) AM_READWRITE(z80_generalstat_r, z80_diskdiag_write_w)
	AM_RANGE(0x40, 0x40) AM_READWRITE(z80_diskstatus_r, z80_diskcontrol_w)
	AM_RANGE(0x60, 0x60) AM_DEVREADWRITE_LEGACY("wd1793", wd17xx_status_r, wd17xx_command_w)
	AM_RANGE(0x61, 0x61) AM_DEVREADWRITE_LEGACY("wd1793", wd17xx_track_r, wd17xx_track_w)
	AM_RANGE(0x62, 0x62) AM_DEVREADWRITE_LEGACY("wd1793", wd17xx_sector_r, wd17xx_sector_w)
	AM_RANGE(0x63, 0x63) AM_DEVREADWRITE_LEGACY("wd1793", wd17xx_data_r, wd17xx_data_w)
ADDRESS_MAP_END

/* Input ports */

/* KEYBOARD (preliminary - transfer to DEC_LK201.xx as soon as possible) */
static INPUT_PORTS_START( rainbow100b_in )
/* DIP switches */
		PORT_START("MONITOR TYPE")
		PORT_DIPNAME( 0x03, 0x03, "MONOCHROME MONITOR") 
		PORT_DIPSETTING(    0x01, "PAPER WHITE" )
		PORT_DIPSETTING(    0x02, "GREEN" )
		PORT_DIPSETTING(    0x03, "AMBER" )

		PORT_START("MEMORY PRESENT")
		PORT_DIPNAME( 0xF000, 0x2000, "MEMORY PRESENT")
		PORT_DIPSETTING(    0x2000, "128 K (BOARD DEFAULT)" ) // NOTE: 0x2000 hard coded in 'system_parameter_r'
		PORT_DIPSETTING(    0x3000, "192 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0x4000, "256 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0x5000, "320 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0x6000, "384 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0x7000, "448 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0x8000, "512 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0x9000, "576 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0xA000, "640 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0xB000, "704 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0xC000, "768 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0xD000, "832 K (MEMORY OPTION)" )
		PORT_DIPSETTING(    0xE000, "896 K (MEMORY OPTION)" )

		PORT_START("GRAPHICS OPTION")
		PORT_DIPNAME( 0x00, 0x00, "GRAPHICS OPTION") PORT_TOGGLE
		PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x04, DEF_STR( On ) )

		PORT_START("BUNDLE OPTION")
		PORT_DIPNAME( 0x00, 0x00, "BUNDLE OPTION") PORT_TOGGLE
		PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x01, DEF_STR( On ) )

	PORT_START("W13") // W13 - W18 affect 'system_parameter_r'
		PORT_DIPNAME( 0x02, 0x02, "W13 (FACTORY TEST A, LEAVE OFF)") PORT_TOGGLE
		PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("W14")
		PORT_DIPNAME( 0x04, 0x04, "W14 (FACTORY TEST B, LEAVE OFF)") PORT_TOGGLE
		PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_START("W15")
		PORT_DIPNAME( 0x08, 0x08, "W15 (FACTORY TEST C, LEAVE OFF)") PORT_TOGGLE
		PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x00, DEF_STR( On ) )
    // DSR = 1 when switch is OFF - see i8251.c (status_r) 
	PORT_START("W18")
		PORT_DIPNAME( 0x01, 0x00, "W18 (FACTORY TEST D, LEAVE OFF) (8251A: DSR)") PORT_TOGGLE 
		PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x01, DEF_STR( On ) )

    // J17 jumper on FDC controller board shifts drive select (experimental) -
	PORT_START("FLOPPY CONTROLLER") 
		PORT_DIPNAME( 0x02, 0x00, "J17 DRIVE SELECT (A => C and B => D)") PORT_TOGGLE
		PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
		PORT_DIPSETTING(    0x02, DEF_STR( On ) )
INPUT_PORTS_END

// 800K native format (80 * 10). Also reads VT-180 disks and PC-DOS 360 k disks 
// ( both: 512 byte sectors, single sided, 9 sectors per track, 40 tracks )
static LEGACY_FLOPPY_OPTIONS_START( dec100_floppy )
    LEGACY_FLOPPY_OPTION( dec100_floppy, "td0", "Teledisk floppy disk image", td0_dsk_identify, td0_dsk_construct, td0_dsk_destruct, NULL )
	LEGACY_FLOPPY_OPTION( dec100_floppy, "img", "DEC Rainbow 100", basicdsk_identify_default, basicdsk_construct_default,    NULL,             
		HEADS([1])
		TRACKS(40/[80])
		SECTORS(9/[10])
		SECTOR_LENGTH([512])
		INTERLEAVE([0])
		FIRST_SECTOR_ID([1])
						)
LEGACY_FLOPPY_OPTIONS_END
 
void rainbow_state::machine_reset()
{
	if (COLD_BOOT == 1)
	{
		COLD_BOOT = 2;
		m_crtc->MHFU(-100); // reset MHFU counter
	}

	m_z80->set_input_line(INPUT_LINE_HALT, ASSERT_LINE);

	INT88 = false;
	INTZ80 = false;

	m_zflip = true;
	m_z80_halted = true;
	m_kbd_tx_ready = m_kbd_rx_ready = false;

	m_kbd8251->input_callback(device_serial_interface::CTS); // raise clear to send

	m_KBD = 0;

	m_beep->set_frequency(2000);
		m_beep->set_state(0);

	// RESET ALL LEDs
	output_set_value("led1", 1);
	output_set_value("led2", 1);
	output_set_value("led3", 1);
	output_set_value("led4", 1);
	output_set_value("led5", 1);
	output_set_value("led6", 1);
	output_set_value("led7", 1);
	output_set_value("led8", 1);
	output_set_value("led9", 1);
	output_set_value("led10", 1);
	output_set_value("led11", 1);

	MOTOR_DISABLE_counter = 2; // soon resets drv.LEDs
	m_unit = 0;

}

UINT32 rainbow_state::screen_update_rainbow(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	// TEST-DEBUG: no screen updates during diskette operations!
	if (MOTOR_DISABLE_counter)
		return 0;

	m_crtc->palette_select( m_inp9->read() );

	if ( m_SCREEN_BLANK )
		 m_crtc->video_blanking(bitmap, cliprect);
	else 
		 m_crtc->video_update(bitmap, cliprect);
	return 0;
}

// It is no longer possible to key in the RAM size on the 100-B.
// The DEC-100-B boot ROM probes until a 'flaky' area is found (around F400:0E04).

// Unexpected low RAM sizes are an indication of option RAM (at worst: 128 K on board) failure.
// While motherboard errors often render the system unbootable, bad option RAM (> 128 K)
// can be narrowed down with the Diagnostic Disk and codes from the 'Pocket Service Guide' 
// EK-PC100-PS-002 (APPENDIX B.2.2); pc100ps2.pdf
// ================================================================
// - Simulate floating bus for initial RAM detection -
// FIXME: code valid ONLY within ROM section F4Exxx.
// 
// NOTE: MS-DOS 2.x unfortunately probes RAM in a similar way.
// => SET OPTION RAM to 896 K for unknown applications (and DOS) <=
// ================================================================
READ8_MEMBER(rainbow_state::floating_bus_r)
{
	int pc = space.device().safe_pc();

	if ( ((pc & 0xFFF00) == 0xF4E00) && 
		 ( m_maincpu->state_int(I8086_DS) >= m_inp8->read() )
	   )
	{
		return (offset>>16) + 2;
	} 
	else
		return space.read_byte(offset);
}

WRITE8_MEMBER(rainbow_state::floating_bus_w)
{
		space.write_byte(offset,data);
}


READ8_MEMBER(rainbow_state::share_z80_r)
{
	if (m_zflip)
	{
		if (offset < 0x8000)
		{
			return m_shared[offset + 0x8000];
		}
		else if (offset < 0x8800)
		{
			return m_z80_private[offset & 0x7ff];
		}

		return m_shared[offset ^ 0x8000];
	}
	else
	{
		if (offset < 0x800)
		{
			return m_z80_private[offset];
		}

		return m_shared[offset];
	}

	return 0xff;
}

WRITE8_MEMBER(rainbow_state::share_z80_w)
{
	if (m_zflip)
	{
		if (offset < 0x8000)
		{
			m_shared[offset + 0x8000] = data;
		}
		else if (offset < 0x8800)
		{
			m_z80_private[offset & 0x7ff] = data;
		}

		m_shared[offset ^ 0x8000] = data;
	}
	else
	{
		if (offset < 0x800)
		{
			m_z80_private[offset] = data;
		}
		else
		{
			m_shared[offset] = data;
		}
	}
}

// Until a full-blown hard-disc emulation evolves, deliver an error message:
READ8_MEMBER(rainbow_state::hd_status_68_r)
{
	// Top 3 bits = status / error code 
	// SEE ->   W_INCHESTER__loc_80E

	// return 0xa0; // A0 : OK, DRIVE IS READY (!)
	
	return 0xe0; //  => 21 DRIVE NOT READY (BIOS; when W is pressed on boot screen)
}

READ8_MEMBER(rainbow_state::system_parameter_r)
{
/*  Info about option boards is in bits 0 - 3:
    Bundle card (1) | Floppy (2) | Graphics (4) | Memory option (8)

    0 1 2 3 4 5 6 7
    B F G M
   ( 1 means NOT present )
*/
	// Hard coded value 0x2000 - see DIP switch setup!
	return 0x0f - m_inp5->read() 
	            - 0                 // floppy is hard coded in emulator.
				- m_inp7->read() 
				- (m_inp8->read() > 0x2000) ? 8 : 0;
}

READ8_MEMBER(rainbow_state::comm_control_r)
{ 
/*	[02] COMMUNICATIONS STATUS REGISTER - PAGE 154 (**** READ **** )
	Used to read status of SERIAL port, IRQ line of each CPU, and MHFU logic enable signal.

//    What the specs says on how MHFU detection is disabled:
//	  1.  by first disabling interrupts with CLI
//	  2.  by writing 0x00 to port 0x10C (handled by 'dc012_w' in vtvideo) 
//	 (3.) MHFU is re-enabled by writing to 0x0c (or automatically after STI - when under BIOS control ?)
*/	
	// During boot phase 2, do not consider MHFU ENABLE. Prevents ERROR 16.
	int data;
	if (COLD_BOOT == 2)
		data = 0;
	else
		data = m_crtc->MHFU(1);

	return (  ( (data > 0) ? 0x00 : 0x20) |// (L): status of MHFU flag => bit pos.5
                  (	(INT88)    ? 0x00 : 0x40 ) |               // (L)
                  (	(INTZ80)   ? 0x00 : 0x80 )                 // (L)
           ); 
}

WRITE8_MEMBER(rainbow_state::comm_control_w)
{
/* Communication control register of -COMM- port (when written):

   8088 LEDs:
   5  7  6  4    <- BIT POSITION
   D6 -D5-D4-D3  <- INTERNAL LED NUMBER (DEC PDF)
   -4--5--6--7-  <- NUMBERS EMBOSSED ON BACK OF PLASTIC HOUSING (see error chart)
*/
	output_set_value("led4", BIT(data, 5)); // LED "D6"
	output_set_value("led5", BIT(data, 7)); // LED "D5"
	output_set_value("led6", BIT(data, 6)); // LED "D4"
	output_set_value("led7", BIT(data, 4)); // LED "D3"

//  printf("%02x to COMM.CONTROL REGISTER\n", data);
}

// EMULATOR TRAP (patched into ROM @ machine_start) via BIOS : call / offset +$21  (AL / AH)
WRITE8_MEMBER(rainbow_state::PORT90_W)
{
	//printf("KBD COMMAND : %02x to AL (90)\n", data);

	m_KBD = 0; // reset previous command.

  if(MOTOR_DISABLE_counter == 0) 
  {

	if (data == LK_CMD_POWER_UP) {      // Powerup (beep)
		//m_beep->set_state(1);
		//m_beep_counter=600;  // BELL = 125 ms
	}

	if (data == LK_CMD_BELL) {
		m_KBD = data;
		m_beep->set_state(1);
		m_beep_counter=600;  // BELL = 125 ms
	}

	if (data == LK_CMD_SOUND_CLK) {    // emit a keyclick (2ms)
		m_KBD = data;
		m_beep->set_state(1);
		m_beep_counter=25; // longer than calculated ( 9,6 )
	}

	if (data == LK_CMD_ENB_BELL) {   // enable the bell - PARAMETER: VOLUME!
		m_KBD = data;
	}

	if (data == LK_CMD_ENB_KEYCLK) {   // enable the keyclick- PARAMETER: VOLUME!
		m_KBD = data;
	}

	if (data == LK_CMD_LEDS_ON ) {  // light LEDs -
		m_KBD = data;
	}
	if (data == LK_CMD_LEDS_OFF) {  // switch off LEDs -
		m_KBD = data;
	}

  } // prevent beeps during disk load operations
}

WRITE8_MEMBER(rainbow_state::PORT91_W)
{
	// 4 leds, represented in the low 4 bits of a byte
	if (m_KBD == LK_CMD_LEDS_ON) {  // light LEDs -
		if (data & 1) { output_set_value("led8", 0); } //   KEYBOARD :  "Wait" LED
		if (data & 2) { output_set_value("led9", 0); } //   KEYBOARD :  "Compose" LED
		if (data & 4) { output_set_value("led10", 0); } //  KEYBOARD :  "Lock" LED
		if (data & 8) { output_set_value("led11", 0); } //  KEYBOARD :  "Hold" LED
		m_KBD = 0; // reset previous command.
	}
	if (m_KBD == LK_CMD_LEDS_OFF) {  // switch off LEDs -
		if (data & 1) { output_set_value("led8", 1); } //   KEYBOARD :  "Wait" LED
		if (data & 2) { output_set_value("led9", 1); } //   KEYBOARD :  "Compose" LED
		if (data & 4) { output_set_value("led10", 1); } //  KEYBOARD :  "Lock" LED
		if (data & 8) { output_set_value("led11", 1); } //  KEYBOARD :  "Hold" LED
		m_KBD = 0; // reset previous command.
	}

	// NVRAM offet $A8 : BELL VOLUME (=> ENABLE BELL 0x23)
	if ( (m_KBD == LK_CMD_BELL) || (m_KBD == LK_CMD_ENB_BELL) )    /* BOTH sound or enable bell have a parameter */
	{ 	/* max volume is 0, lowest is 0x7 */
//		printf("\n%02x BELL CMD has bell volume = %02x\n", m_KBD, 8 - (data & 7));
		m_KBD = 0; // reset previous command.
	}

	// NVRAM offet $A9 = KEYCLICK VOLUME (=> ENABLE CLK 0x1b)
	// NVRAM offset $8C = KEYCLICK ENABLE / DISABLE (0/1)
	if ( ( m_KBD == LK_CMD_ENB_KEYCLK ) || ( m_KBD == LK_CMD_SOUND_CLK ) )  /* BOTH keyclick cmds have a parameter */ 
	{	// max volume is 0, lowest is 0x7  - 87 (BELL VOL:1) and  80 (BELL VOL:8)
//		printf("\n%02x CLICK CMD - keyclick volume = %02x\n", m_KBD, 8 - (data & 7));
		m_KBD = 0; // reset previous command.
	}

if (m_KBD > 0)	
	printf("UNHANDLED PARAM FOR MODE: %02x / KBD PARAM %02x to AH (91) \n", m_KBD, data);

}
// 8088 reads port 0x00. See page 133 (4-34)
READ8_MEMBER(rainbow_state::i8088_latch_r)
{
//    printf("Read %02x from 8088 mailbox\n", m_8088_mailbox);
	m_i8088->set_input_line(INPUT_LINE_INT0, CLEAR_LINE);

	INT88 = false; // BISLANG:  INTZ80 = false; //   
	return m_8088_mailbox;
}

// 8088 writes port 0x00. See page 133 (4-34)
WRITE8_MEMBER(rainbow_state::i8088_latch_w)
{
//    printf("%02x to Z80 mailbox\n", data);
	m_z80->set_input_line_and_vector(0, ASSERT_LINE, 0xf7);
	m_z80_mailbox = data;

   INTZ80 = true; //
}

// Z80 reads port 0x00
// See page 134 (4-35)
READ8_MEMBER(rainbow_state::z80_latch_r)
{
//    printf("Read %02x from Z80 mailbox\n", m_z80_mailbox);
	m_z80->set_input_line(0, CLEAR_LINE);

	INTZ80 = false; // BISLANG: INT88 = false; 
	return m_z80_mailbox;
}

// Z80 writes to port 0x00
// See page 134 (4-35)
WRITE8_MEMBER(rainbow_state::z80_latch_w)
{
	//    printf("%02x to 8088 mailbox\n", data);
	m_i8088->set_input_line_and_vector(INPUT_LINE_INT0, ASSERT_LINE, 0x27);
	m_8088_mailbox = data;

	INT88 = true; 
}

// WRITE to 0x20
WRITE8_MEMBER(rainbow_state::z80_diskdiag_read_w)
{
	m_zflip = true;
}

// (Z80) : PORT 21H  _READ_
READ8_MEMBER(rainbow_state::z80_generalstat_r)
{
/*	
General / diag.status register Z80 / see page 157 (table 4-18).

D7 : STEP L : reflects status of STEP signal _FROM FDC_
      (when this 2us output pulse is low, the stepper will move into DIR)
D6 : WRITE GATE L :reflects status of WRITE GATE signal _FROM FDC_
      (asserted low before data can be written on the diskette)
D5 : TR00: reflects status of TRACK 0 signal (= 1) * from the disk drive *
D4 : DIR L: reflects status of DIRECTION signal * FROM FDC * to disk
      (when low, the head will step towards the center)
D3 : READY L: reflects status of READY L signal * from the disk drive *
     (low active, asserts when disk is inserted and door is closed)
D2 : INT88 L: (bit reads the INT88 bit sent by Z80 to interrupt 8088)
D1 : INTZ80 L: (bit reads the INTZ80 bit sent by 8088 to interrupt Z80)
D0 : ZFLIP L: (read from the diagnostic control register of Z80A)

NOTES: ALL LOW ACTIVE - EXCEPT TR00
*/
	// * TRACK 00 *  signal for current drive
	int tk00 = ( floppy_tk00_r( m_image[m_unit] ) == CLEAR_LINE ) ? 0x20 : 0x00; 

	int fdc_ready = floppy_drive_get_flag_state( m_image[m_unit] , FLOPPY_DRIVE_READY); 

	int data=(   0x80                    |   // (STEP L)
//		     (  (fdc_write_gate) )       |   
			 (  (tk00)           )                             | 
//			 (   fdc_direction)                                |     
		     (  (fdc_ready)? 0x00 : 0x08 )                     |
		     (	(INT88)    ? 0x00 : 0x04 )                     |    
             (	(INTZ80)   ? 0x00 : 0x02 )                     |     
			 (  (m_zflip)  ? 0x00 : 0x01 )
			);
	
	return data;
}

// (Z80) : PORT 21H * WRITE *
WRITE8_MEMBER(rainbow_state::z80_diskdiag_write_w)
{
/*   Z80 LEDs:
     4   5   6  <- bit #
    D11 D10 -D9 <- INTERNAL LED NUMBER (see PDF)
    -1 --2-- 3  <- NUMBERS EMBOSSED ON BACK OF PLASTIC HOUSING (see error chart)
*/
	output_set_value("led1", BIT(data, 4)); // LED "D11"
	output_set_value("led2", BIT(data, 5)); // LED "D10"
	output_set_value("led3", BIT(data, 6)); // LED "D9"

	m_zflip = false;
}

// (Z80) : PORT 40H _READ_

// **********************************************************************
//  POLARITY OF _DRQ_ AND _IRQ_ (depends on controller type!)  
// **********************************************************************
READ8_MEMBER(rainbow_state::z80_diskstatus_r)
{
static int last_track;
int track = wd17xx_track_r(m_fdc, space, 0);

if (track != last_track) 
	printf("\n%02d",track);
last_track = track;

// 40H diskette status Register **** READ ONLY *** ( 4-60 of TM100.pdf )

// AND 00111011 - return what was WRITTEN to D5-D3, D1, D0 previously
//                (except D7,D6,D2)
int data = m_z80_diskcontrol && 0x3b;       

// D7: DRQ: reflects status of DATA REQUEST signal from FDC. 
// '1' indicates that FDC has read data OR requires new write data.
data |= wd17xx_drq_r(m_fdc) ? 0x80 : 0x00;  

// D6: IRQ: indicates INTERRUPT REQUEST signal from FDC. Indicates that a 
//          status bit has changed. Set to 1 at the completion of any 
//          command (.. see page 207 or 5-25).
data |= wd17xx_intrq_r(m_fdc) ? 0x40 : 0x00;                       

// D5: SIDE 0H: status of side select signal at J2 + J3 of RX50 controller. 
//	            For 1 sided drives, this bit will always read low (0).

// D4: MOTOR 1 ON L: 0 = indicates MOTOR 1 ON bit is set in drive control reg.
// D3: MOTOR 0 ON L: 0 = indicates MOTOR 0 ON bit is set in drive  " 

// D2: TG43 L :  0 = INDICATES TRACK > 43 SIGNAL FROM FDC TO DISK DRIVE.
data |= ( track > 43) ? 0x00 : 0x04; 

// D1: DS1 H: reflect status of bits 0 and 1 form disk.control reg.
// D0: DS0 H: "  
  return data;
}

// (Z80) : PORT 40H  * WRITE *

// RX-50 has head A and head B (1 for each of the 2 disk slots in a RX-50).

// TODO: find out how head load and drive select really work.
WRITE8_MEMBER(rainbow_state::z80_diskcontrol_w)
{
	// FORCE_READY = 0 : assert DRIVE READY on FDC (diagnostic override; USED BY BIOS!)
	//               1 : set ready only if drive is present, disk is in the drive,
	//                   and disk motor is on - for Amstrad, Spectrum, PCW...
	int force_ready = ( (data & 4) != 0 ) ? 0 : 1;

	int drive;
	if ( m_inp6->read() && ((data & 3) < 2) )
		drive = (data & 1) + 2;
	else
		drive = data & 3;

	int selected_drive = 255;

	if (flopimg_get_image( floppy_get_device( machine(), drive ) ) != NULL)
	{	selected_drive = drive;
		wd17xx_set_drive(m_fdc, selected_drive);
	}

	// WD emulation (wd17xx.c) will ignore 'side select' if set to WD1793.
	// Is it safe to * always assume * single sided 400 K disks?
	wd17xx_set_side(m_fdc, (data & 20) ? 1 : 0);  

	wd17xx_dden_w(m_fdc, 0); /* SEE 'WRITE_TRACK' : 1 = SD; 0 = DD; enable double density */

    output_set_value("driveled0",  (selected_drive == 0) ? 1 : 0 ); 
	output_set_value("driveled1",  (selected_drive == 1) ? 1 : 0 ); 

	output_set_value("driveled2",  (selected_drive == 2) ? 1 : 0 ); 
	output_set_value("driveled3",  (selected_drive == 3) ? 1 : 0 ); 

	if (selected_drive < 4) 
	{	    
			m_unit = selected_drive; 

			// MOTOR ON flags 1+2 proved to be unreliable in this context.
			// So this timeout only disables LEDs.
			MOTOR_DISABLE_counter = 10000; // prolonged timeout. DEFAULT: 2400 = 500 ms

			for(int f_num=0; f_num <= 3; f_num++)
			{
				// Although 1773 does not feature 'motor on' this statement is required:
				// CLEAR_LINE = turn motor on -
				floppy_mon_w(m_image[f_num], (f_num == selected_drive) ? CLEAR_LINE : ASSERT_LINE);

				// Parameters: DRIVE, STATE, FLAG
				floppy_drive_set_ready_state( m_image[f_num], 
					                          (f_num == selected_drive) ? 1 : 0, 
											  (f_num == selected_drive) ? force_ready : 0
											);
			}	
	}

   m_z80_diskcontrol = data;
}

READ8_MEMBER( rainbow_state::read_video_ram_r )
{
	return m_p_ram[offset];
}

INTERRUPT_GEN_MEMBER(rainbow_state::vblank_irq)
{
	device.execute().set_input_line_and_vector(INPUT_LINE_INT0, ASSERT_LINE, 0x20);
}

WRITE8_MEMBER( rainbow_state::clear_video_interrupt )
{
	m_i8088->set_input_line(INPUT_LINE_INT0, CLEAR_LINE);
}

READ8_MEMBER( rainbow_state::diagnostic_r )
{
//    printf("%02x DIP value ORed to diagnostic\n", ( m_inp1->read() | m_inp2->read() | m_inp3->read()   )  );

	return ( (m_diagnostic & (0xf1)) | (    m_inp1->read() |
											m_inp2->read() |
											m_inp3->read()   
									   )
			);
}

WRITE8_MEMBER( rainbow_state::diagnostic_w )
{
//    printf("%02x to diag port (PC=%x)\n", data, space.device().safe_pc());
	m_SCREEN_BLANK = (data & 2) ? false : true;

	//  SAVE / PROGRAM NVM: transfer data from volatile memory to NVM 
	if ( !(data & 0x40)  && (m_diagnostic & 0x40) ) 
		memcpy( m_p_nvram, m_p_vol_ram, 256); 

	// READ / RECALL NVM: transfer data from NVM to volatile memory 
	if ( (data & 0x80)  && !(m_diagnostic & 0x80) ) 
		memcpy( m_p_vol_ram, m_p_nvram, 256);
		
	if (!(data & 1))
	{
		m_z80->set_input_line(INPUT_LINE_HALT, ASSERT_LINE);
		m_z80_halted = true;
	}

	if ((data & 1) && (m_z80_halted))
	{
		m_zflip = true;
		m_z80_halted = false;
		m_z80->set_input_line(INPUT_LINE_HALT, CLEAR_LINE);
		m_z80->reset();
	}

	/*	Page 197 or 5-13 of formatter description:
		ZRESET L : this low input from the 8088 diagnostic write register
		resets the formatter controller, loads 03H into the command register, 
		and resets the not ready (status bit 7).

		When ZRESET goes high (1), a restore command is executed regardless
		of the state of the ready signal from the diskette drive and 
		01H is loaded into the sector register.
	*/
	
	// reset device when going from high to low,
	// restore command when going from low to high :
	wd17xx_mr_w(m_fdc, (data & 1) ? 1 : 0);

	m_diagnostic = data;
}


READ_LINE_MEMBER(rainbow_state::dsr_r)
{
	return m_inp4->read(); // W18 (1/0)
}

// KEYBOARD
void rainbow_state::update_kbd_irq()
{
	if ((m_kbd_rx_ready) || (m_kbd_tx_ready))
	{
		m_i8088->set_input_line_and_vector(INPUT_LINE_INT0, ASSERT_LINE, 0x26);
	}
	else
	{
		m_i8088->set_input_line(INPUT_LINE_INT0, CLEAR_LINE);
	}
}


READ_LINE_MEMBER(rainbow_state::kbd_rx)
{
//    printf("read keyboard\n");
	return 0x00;
}

WRITE_LINE_MEMBER(rainbow_state::kbd_tx)
{
//    printf("%02x to keyboard\n", state);
}

WRITE_LINE_MEMBER(rainbow_state::kbd_rxready_w)
{
//    printf("rxready %d\n", state);
	m_kbd_rx_ready = (state == 1) ? true : false;
	update_kbd_irq();
}

WRITE_LINE_MEMBER(rainbow_state::kbd_txready_w)
{
//    printf("txready %d\n", state);
	m_kbd_tx_ready = (state == 1) ? true : false;
	update_kbd_irq();
}

TIMER_DEVICE_CALLBACK_MEMBER(rainbow_state::keyboard_tick)
{
	m_kbd8251->transmit_clock();
	m_kbd8251->receive_clock();

	if (MOTOR_DISABLE_counter) 
		MOTOR_DISABLE_counter--;

	if (MOTOR_DISABLE_counter == 1)
	{
			output_set_value("driveled0", 0); // DRIVE 0 (A)
			output_set_value("driveled1", 0); // DRIVE 1 (B)
			output_set_value("driveled2", 0); // DRIVE 2 (C)
			output_set_value("driveled3", 0); // DRIVE 3 (D)
	}

    if ( m_crtc->MHFU(1) ) // MHFU ENABLED ?
    {	         
/*				int data = m_crtc->MHFU(-1); // increment MHFU, return new value
                //  if (data >  480) ...
				//	   m_crtc->MHFU(-100); 
				//	   machine().schedule_hard_reset(); // not exactly a proper watchdog reset
*/				
	}

	if (m_beep_counter > 1)
			m_beep_counter--;
	else
		if ( m_beep_counter == 1 )
		{   m_beep->set_state(0);
			m_beep_counter = 0;
		}
}

static const vt_video_interface video_interface =
{
	"chargen",
	DEVCB_DRIVER_MEMBER(rainbow_state, read_video_ram_r),
	DEVCB_DRIVER_MEMBER(rainbow_state, clear_video_interrupt)
};

/* F4 Character Displayer */
static const gfx_layout rainbow_charlayout =
{
	8, 10,                  /* 8 x 16 characters */
	256,                    /* 256 characters */
	1,                  /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	/* x offsets */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	/* y offsets */
	{ 15*8, 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8 },
	8*16                    /* every char takes 16 bytes */
};

static GFXDECODE_START( rainbow )
	GFXDECODE_ENTRY( "chargen", 0x0000, rainbow_charlayout, 0, 1 )
GFXDECODE_END

// Rainbow Z80 polls only, no IRQ/DRQ are connected
const wd17xx_interface rainbow_wd17xx_interface =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	{FLOPPY_0, FLOPPY_1, FLOPPY_2, FLOPPY_3}
};

static const floppy_interface floppy_intf =
{
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	DEVCB_NULL,
	FLOPPY_STANDARD_5_25_SSDD_80,
	LEGACY_FLOPPY_OPTIONS_NAME( dec100_floppy ),
	"floppy_5_25",
	NULL
};

static const i8251_interface i8251_intf =
{
	DEVCB_DRIVER_LINE_MEMBER(rainbow_state, kbd_rx),         // rxd in
	DEVCB_DRIVER_LINE_MEMBER(rainbow_state, kbd_tx),         // txd out
	DEVCB_DRIVER_LINE_MEMBER(rainbow_state, dsr_r),       // dsr
	DEVCB_NULL,         // dtr
	DEVCB_NULL,         // rts
	DEVCB_DRIVER_LINE_MEMBER(rainbow_state, kbd_rxready_w),
	DEVCB_DRIVER_LINE_MEMBER(rainbow_state, kbd_txready_w),
	DEVCB_NULL,         // tx empty
	DEVCB_NULL          // syndet
};

static MACHINE_CONFIG_START( rainbow, rainbow_state )
	MCFG_DEFAULT_LAYOUT(layout_rainbow)

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu",I8088, XTAL_24_0734MHz / 5)
	MCFG_CPU_PROGRAM_MAP(rainbow8088_map)
	MCFG_CPU_IO_MAP(rainbow8088_io)
	MCFG_CPU_VBLANK_INT_DRIVER("screen", rainbow_state,  vblank_irq)

	MCFG_CPU_ADD("subcpu",Z80, XTAL_24_0734MHz / 6)
	MCFG_CPU_PROGRAM_MAP(rainbowz80_mem)
	MCFG_CPU_IO_MAP(rainbowz80_io)

	/* video hardware */
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500)) /* not accurate */
	MCFG_SCREEN_SIZE(132*10, 49*10)
	MCFG_SCREEN_VISIBLE_AREA(0, 80 * 10-1, 0, 24 * 10-1)
	MCFG_SCREEN_UPDATE_DRIVER(rainbow_state, screen_update_rainbow)
	MCFG_GFXDECODE(rainbow)
	MCFG_PALETTE_LENGTH(4)

	MCFG_RAINBOW_VIDEO_ADD("vt100_video", video_interface)

		/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("beeper", BEEP, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS,"mono",0.50)

	MCFG_FD1793_ADD("wd1793", rainbow_wd17xx_interface )
	MCFG_LEGACY_FLOPPY_4_DRIVES_ADD(floppy_intf) 
	MCFG_SOFTWARE_LIST_ADD("flop_list","rainbow")

	MCFG_I8251_ADD("kbdser", i8251_intf)
	MCFG_TIMER_DRIVER_ADD_PERIODIC("keyboard", rainbow_state, keyboard_tick, attotime::from_hz(4800))

	MCFG_LK201_ADD()
	MCFG_NVRAM_ADD_0FILL("nvram")
MACHINE_CONFIG_END



// ROM definition for 100-B 
ROM_START( rainbow )
	ROM_REGION(0x100000,"maincpu", 0)
	ROM_LOAD( "23-022e5-00.bin",  0xf0000, 0x4000, CRC(9d1332b4) SHA1(736306d2a36bd44f95a39b36ebbab211cc8fea6e))
	ROM_RELOAD(0xf4000,0x4000)
	ROM_LOAD( "23-020e5-00.bin", 0xf8000, 0x4000, CRC(8638712f) SHA1(8269b0d95dc6efbe67d500dac3999df4838625d8)) // German, French, English
	//ROM_LOAD( "23-015e5-00.bin", 0xf8000, 0x4000, NO_DUMP) // Dutch, French, English
	//ROM_LOAD( "23-016e5-00.bin", 0xf8000, 0x4000, NO_DUMP) // Finish, Swedish, English
	//ROM_LOAD( "23-017e5-00.bin", 0xf8000, 0x4000, NO_DUMP) // Danish, Norwegian, English
	//ROM_LOAD( "23-018e5-00.bin", 0xf8000, 0x4000, NO_DUMP) // Spanish, Italian, English
	ROM_RELOAD(0xfc000,0x4000)
	ROM_REGION(0x1000, "chargen", 0)
	ROM_LOAD( "chargen.bin", 0x0000, 0x1000, CRC(1685e452) SHA1(bc299ff1cb74afcededf1a7beb9001188fdcf02f))
ROM_END

// 'Rainbow 190 B' (announced March 1985) is identical hardware with alternate ROM v5.05
// According to an article in Wall Street Journal, it came with a 10 MB HD and 640 K RAM.

// We have no version history. The BOOT 2.4 README reveals 'recent ROM changes for MASS 11' 
// in January 1985. These were not contained in the older version 04.03.11 (for PC-100-A) 
// and also not present in version 05.03 (from PC-100B / PC100B+).

// A first glance:
// => jump tables (F4000-F40083 and FC000-FC004D) were not extended.
// => absolute addresses of some internal routines have changed.
// => programs that do not rely on specific ROM versions should be compatible.
ROM_START( rainb190 )
	ROM_REGION(0x100000,"maincpu", 0)
	ROM_LOAD( "dec190rom0.bin",  0xf0000, 0x4000, CRC(FAC191D2) )
	ROM_RELOAD(0xf4000,0x4000)
	ROM_LOAD( "dec190rom1.bin", 0xf8000, 0x4000, CRC(5CE59632) ) 

	ROM_RELOAD(0xfc000,0x4000)
	ROM_REGION(0x1000, "chargen", 0)
	ROM_LOAD( "chargen.bin", 0x0000, 0x1000, CRC(1685e452) SHA1(bc299ff1cb74afcededf1a7beb9001188fdcf02f))
ROM_END

/* Driver */

/*    YEAR  NAME         PARENT   COMPAT  MACHINE       INPUT      STATE          INIT COMPANY                         FULLNAME       FLAGS */
COMP( 1983, rainbow   , 0      ,      0,  rainbow, rainbow100b_in, driver_device, 0,  "Digital Equipment Corporation", "Rainbow 100-B", GAME_NOT_WORKING | GAME_IMPERFECT_COLORS)
COMP( 1985, rainb190, rainbow,      0,  rainbow, rainbow100b_in, driver_device, 0,  "Digital Equipment Corporation", "Rainbow 190-B", GAME_NOT_WORKING | GAME_IMPERFECT_COLORS)