/******************************************************************************

   - NORAUT POKER SYSTEM -
  -------------------------

  Driver by Roberto Fresca & Angelo Salese.


  Games running on this hardware:

  -- Z80 based --

   * Noraut Poker,                        1988,  Noraut Ltd.
   * Noraut Deluxe Poker (console),       198?,  Noraut Ltd.
   * Noraut Deluxe Poker (bootleg),       198?,  Unknown.
   * Noraut Joker Poker (original),       198?,  Noraut Ltd.
   * Noraut Joker Poker (alt),            1988,  Noraut Ltd.
   * Noraut Red Hot Joker Poker,          1988,  Noraut Ltd.
   * Noraut Red Hot Joker Poker (alt HW), 198?,  Noraut Ltd.
   * Noraut Poker (NTX10A),               1988,  Noraut Ltd.
   * Noraut Joker Poker (V3.010a),        2002,  Noraut Ltd.
   * Noraut Joker Poker (V3.011a),        2003,  Noraut Ltd.
   * Noraut unknown set 1 (console),      198?,  Noraut Ltd.
   * Noraut unknown set 2 (console),      198?,  Noraut Ltd.
   * Mainline Double Joker Poker,         198?,  Mainline London.  
   * Double Joker Poker (45%-75% payout), 199?,  DellFern Ltd.
   * Royal on Ten (Noraut Deluxe hack),   2005,  Unknown.
   * Credit Poker (ver.30c, standard),    1999,  CGI.
   * PMA Poker,                           198?,  PMA.
   * Poker / Black Jack (Model 7521),     198?,  M. Kramer Manufacturing.
   * Kimble Joker Poker,                  198?,  Kimble Gaming.

  -- 8080A based --

   * Draw Poker Hi-Lo,                    1983,  M. Kramer Manufacturing.
   * Draw Poker Hi-Lo (alt),              1983,  Unknown.
   * Draw Poker Hi-Lo (Japanese),         198?,  Unknown.
   * GTI Poker,                           1983,  GTI Inc.
   * Hi-Lo Double Up Joker Poker,         1983,  SMS Manufacturing Corp.
   * DRHL Poker (v.2.89),                 1986,  Drews Inc.
   * Turbo Poker 2,                       1993,  Micro Manufacturing, Inc.


  This hardware emulation opened a big can of worms. :)

  Seems that the original hardware/game was created by M.Kramer Manufacturing,
  and then reprogrammed, copied, bootlegged, used & abused by other companies
  like Noraut Ltd, Kimble Gaming, GTI, DellFern, Merit Industries, Red Card,
  Blue Games, CGI, Micro Manufacturing, SMS Manufacturing, Drews Distributing,
  Drew Industries, Lynch Enterprises Inc, Hillside Gaming Corp, Electro Sport,
  Mainline London, Southern Systems, Americade Amusement Inc, Prologic Ireland,
  Mosfat, Unique, GEI, etc...

  You can see some legal issues in the following links:
  http://www.altlaw.org/v1/cases/533481
  http://cases.justia.com/us-court-of-appeals/F2/783/421/41759/


  Special thanks to Alan Griffin, that kindly helped providing good references
  that allowed me to improve the Noraut system emulation.


*******************************************************************************


  HARDWARE NOTES:
  ---------------

  Hardware Layout (norautp):

  1x Z80
  3x PPI 8255
  2x 6116 SRAM
  1x 3.6 Vcc Battery.
  1x 18.432 MHz. Xtal.

  1x 555 + unknown yellow resonator, near the edge connector.
  1x 555 + resnet, near the battery.

  1x 10 DIP switches bank.
  2x 3pins jumpers (between the Z80 and ROM)

     JP1 (ABC);  JP2 (DEF)

  PCB silksceened:  AB+DE=512  BC+DE=256
                    (CUT BC)   EF=64/128

  PCB silksceened:  SMART-BOARD 131191 ISS.E (Made in USA)

  -------------------------------------------------------------


  Hardware Layout (norautjp):

  - CPU:             1x TMPZ84C00AP-8
  - RAM:             2x HM6116LP-4 CMOS Static Ram
  - I/O:             3x D8255AC-2 Peripeheral Interface Adapter
  - Prg ROMs:        1x 2764 Eprom
  - Gfx ROMs:        1x 2732 Eprom
  - Sound:           Discrete
  - Battery:         1x 3.6v Ni-cd 65Mah
  - Crystal:         1x 18.432Mhz
  - Resistor Array:  4x 9 Pin SIP 472G


  PCB Layout (norautjp):                                                       Edge Connector 36x2
   ______________________________________________________________________________________________
  |  _____                  _________    _________    _____         .........    _________       |
  | |D5555|                |74LS174N |  |74LS153N |  |D5555|        .........   |ULN2003A |      |
  | |_____|                |_________|  |_________|  |_____|    Resistor Array  |_________|      |
  |                                                               ___                            |
  |                                                              |VR1|                           |
  |              DIP SW x4                                       |___|                           |
  |  ________     _______                                                                        |
  | |Battery |   |1|2|3|4|  _________    _________    _________    _________     _________       |
  | |  3.6v  |   |_|_|_|_| |74LS157N |  |74LS153N |  |74LS161AP|  |74LS86AN |   |ULN2003A |      |
  | |________|             |_________|  |_________|  |_________|  |_________|   |_________|      |
  |                                                                                              |
  |                                                                                              |
  |                                                                                              | 36
  |  _________              _________                 _________    _________     _________       |___
  | |HD74LS04P|            |74LS166AP|               |74LS161AN|  |74LS153N |   |ULN2003A |       ___|
  | |_________|            |_________|               |_________|  |_________|   |_________|       ___|
  |                                                                                               ___|
  |                                   DIP SW x 8                               ________________   ___|
  |               _____________    _______________    _________    _________  |                |  ___|
  |              |             |  |1|2|3|4|5|6|7|8|  |74LS161AN|  |74LS157N | |    D8255AC-2   |  ___|
  |              |    2732     |  |_|_|_|_|_|_|_|_|  |_________|  |_________| |________________|  ___|
  |              |_____________|                                                                  ___|
  |                                                                                               ___|
  |                                                                            ________________   ___|
  |                                                                           |                |  ___|
  |  _____________        _____________                                       |    D8255AC-2   |  ___|
  | |             |      |             |                                      |________________|  ___|
  | |    6116     |      |    6116     |                                                          ___|
  | |_____________|      |_____________|              _________    _________                      ___|
  |                                                  |74LS161AN|  |74LS157N |                     ___|
  |                                                  |_________|  |_________|                     ___|
  |                                                                                               ___|
  |  ______________       ________________                                                        ___|
  | |              |     |                |           _________    _________                      ___|
  | |     2764     |     |    D8255AC-2   |          |74LS161AN|  |74LS157N |     .........       ___|
  | |______________|     |________________|          |_________|  |_________|     .........       ___|
  |                                                                              Resistor Array   ___|
  |                                                                                              |
  |                                                                                              | 1
  |                      _________                    _________    _________    _________        |
  |                     | 74LS32N |                  |74LS161AN|  | 74LS86P |  | 74LS04N |       |
  |                     |_________|                  |_________|  |_________|  |_________|       |
  |                                                                                              |
  |                                             XTAL                                             |
  |                                            .----.                                            |
  |  ____________________     __________      _________    _________    _________    _________   |
  | |                    |   |PALce16v8H|    | 74LS04N |  |74LS157N |  | 74LS11N |  |74LS74AN |  |
  | |   TMPZ84C00AP-8    |   |__________|    |_________|  |_________|  |_________|  |_________|  |
  | |____________________|                                                                       |
  |                                                                                              |
  |______________________________________________________________________________________________|



*******************************************************************************


  Noraut Edge Connector (pinouts)
  --------------------------------
  Component     PN   Solder Side
  --------------------------------
  GND           01   GND
  5v DC         02   5v DC
                03
  12v DC        04   12v DC
                05
                06
                07
  0v            08   Readout Switch
  0v            09   Low level hopper
  0v            10   50p in
  0v            11   pound in
  0v            12   Bet switch
  0v            13   Deal switch
  0v            14   Hold 1 switch
  0v            15   Half Gamble switch
  0v            16   Change Card switch
  Refil         17   Coin count/sense from hopper
  Low Switch    18   High swicth
  Hold 3 Switch 19   Hold 2 switch
  Hold 5 Switch 20   Hold 4 switch
  10p coin      21   Deflect
                22   50p in meter
                23   Hopper Motor Drive (low volt switch line NOT 24v)
                24
                25   spk+
                26   Panel lamps clock
  Monitor sync  27   Hold 1 lamp
  Bet lamp      28   Deal lamp
  Change lamp   29   Hold 4 lamp
  Hold 5 lamp   30   Panel lights reset
  High lamp     31   Half Gamble lamp
  Hold 2 lamp   32   Low lamp
  10p Meter out 33   Meter refil
  Video Green   34   Hold 3 lamp
  Video Blue    35   10p in Meter
  Video Red     36   Spark Detect (Not on all boards)


*******************************************************************************

  Control Panel
  -------------

  There are 2 control panel schemes:

  * The default one (11 button-lamps) for systems without CANCEL button.

  .--------------------------------------------------------------------------.
  |                                              .-------. .------. .------. |
  | .------. .------. .------. .------. .------. |  BET  | | HALF | |  HI  | |
  | |      | |      | |      | |      | |      | |COLLECT| |GAMBLE| |      | |
  | | HOLD | | HOLD | | HOLD | | HOLD | | HOLD | '-------' '------' '------' |
  | |CANCEL| |CANCEL| |CANCEL| |CANCEL| |CANCEL| .-------. .------. .------. |
  | |      | |      | |      | |      | |      | | DEAL  | |CHANGE| |  LO  | |
  | '------' '------' '------' '------' '------' | DRAW  | | CARD | |      | |
  |                                              '-------' '------' '------' |
  '--------------------------------------------------------------------------'

   HOLD buttons              = red.
   BET, DEAL, HI & LO        = yellow.
   HALF GAMBLE & CHANGE CARD = orange.


  * The alternate one (12 button-lamps) for systems with CANCEL button.

  .-------------------------------------------------------------.
  | .------. .------. .------. .------. .------.   .----------. |
  | | HOLD | | HOLD | | HOLD | | HOLD | | HOLD |   |   HIGH   | |
  | '------' '------' '------' '------' '------'   '----------' |
  | .------. .------. .------. .------. .------.   .----------. |
  | |CANCEL| |STAND | | SAVE | | DEAL | | BET  |   |   LOW    | |
  | '------' '------' '------' '------' '------'   '----------' |
  '-------------------------------------------------------------'

   HOLD & CANCEL buttons     = yellow (1).
   STAND & DEAL buttons      = orange (1).
   SAVE (HALF GAMBLE) button = blued-green (1).
   BET button                = red (1).
   HIGH & LOW buttons        = yellow.

   (1) Circular-shaped buttons.

  Some lamps are wired in different way in this scheme.


*******************************************************************************


  Noraut Poker discrete audio circuitry
  -------------------------------------

  3x ULN2003A (Darlington transistor array)
  1x D5555C   (CMOS timer)
  1x KN2222A  (Epitaxial planar general purpose NPN transistor)
  1x VR/POT

  .------.                              .------------.               .-------.
  |      |                              |   D5555C   |               |KN2222A|
  |      |                             4|            |3     R3       |       |      -->
  |   PC7|------------------------------|RST      OUT|-----ZZZZ------|B     E|>----ZZZZZ-----> Audio Out.
  |   PC6|----------.                  6|            |8              |   C   |      VR1
  |   PC5|-----.    |2-in         .-----|THR      VCC|-----------.   '---+---'          .----> Audio Out.
  |   PC4|--.  |  .-+------.      |    5|            |7          |       |              |
  |      |  |  |  |ULN2003A|      |  .--|CVOLT   DISC|--.        |       |              |
  |      |  |  |  '-+------'      |  |  |            |  |        +-------'            --+--
  |      |  |  |    |2-out   C1   |  |  |    GND     |  |        |                     GND
  | 8255 |  |  |    '--------||---+  |  '-----+------'  |        |                      -
  |  AP  |  |  '--.               |  |        |1        |        |                      '
  |      |  |     |3-in           |  |        |         |        |
  '------'  |  .--+-----.         |  |   C5   |         |        |
            |  |ULN2003A|         |  '---||---+         |        |    +5V
            |  '--+-----'         |           |         |        |    -+-
            |     |3-out     C2   |      C4   |         |    C6  |     |
            |     '----------||---+------||---+-------. | .--||--+-----'
            |2-in                 |           |       | | |      |
          .-+------.              |         --+--      '-'       |
          |ULN2003A|              |          GND        |        |
          '-+------'              |           -         |        |
            |2-out           C3   |     R1    '         |   R2   |
            '----------------||---+----ZZZZ-------------+--ZZZZ--'

  VR1 = 100 ohms

  R1 = 120 K ; Tolerance +/- 5%
  R2 = 2.2 K ; Tolerance +/- 5%
  R3 = 1 K   ; Tolerance +/- 5%

  C1 = 103J = 10000 pF  =  10 nF = 0.01 uF  ; Tolerance +/- 5%
  C2 = 223J = 22000 pF  =  22 nF = 0.022 uF ; Tolerance +/- 5%
  C3 = 473J = 47000 pF  =  47 nF = 0.047 uF ; Tolerance +/- 5%
  C4 = 103J = 10000 pF  =  10 nF = 0.01 uF  ; Tolerance +/- 5%

  C5 = 103  = 10000 pF  =  10 nF = 0.01 uF
  C6 = 104  = 100000 pF = 100 nF = 0.1 uF

  - C1, C2, C3 & C4 are polyester film / mylar capacitors.
  - C5 & C6 are ceramic capacitors.
  - All Capacitors are non-polarised.


*******************************************************************************


  Narout System Ports Map
  -----------------------

  (*) Confirmed lines.


  PPI-0 (60h-63h); PortA IN.
  DIP Switches bank:

  7654 3210
  ---- ---x  * DIP switch 8
  ---- --x-  * DIP switch 7
  ---- -x--  * DIP switch 6
  ---- x---  * DIP switch 5
  ---x ----  * DIP switch 4
  --x- ----  * DIP switch 3
  -x-- ----  * DIP switch 2
  x--- ----  * DIP switch 1


  PPI-0 (60h-63h); PortB OUT.
  Lamps:

  7654 3210
  ---- ---x  * CHANGE CARD lamp.
  ---- --x-  * SAVE / HALF GAMBLE lamp.
  ---- -x--  * HOLD 1 lamp.
  ---- x---  * HOLD 2 lamp.
  ---x ----  * HOLD 3 lamp.
  --x- ----  * HOLD 4 lamp.
  -x-- ----  * HOLD 5 lamp.
  x--- ----  * CANCEL lamp.


  PPI-0 (60h-63h); PortC OUT.
  Lamps & Coin Counters:

  7654 3210
  ---- ---x  * HI lamp.
  ---- --x-  * LO lamp.
  ---- -x--  * HOPPER MOTOR DRIVE
  ---- x---  * Payout pulse.
  ---x ----  * Coin 2 counter.
  --x- ----  * Coin 1 counter.
  -x-- ----  + Coin counter related.
  x--- ----  + DEFLECT (always activated).


-----------------------------------------------------------

  PPI-1 (a0h-a3h); PortA IN.
  Regular Inputs:

  7654 3210
  ---- ---x  * DEAL / DRAW button.
  ---- --x-  * BET / CHANGE CARD button.
  ---- -x--  * COIN 1 mech.
  ---- x---  * COIN 2 mech.
  ---x ----  * READOUT button (noraut11).
  --x- ----  * HI button.
  -x-- ----  * LO button.
  x--- ----  * PAYOUT button.


  PPI-1 (a0h-a3h); PortB IN.
  Regular Inputs:

  7654 3210
  ---- ---x  * STAND / TAKE button.
  ---- --x-  * SAVE / HALF GAMBLE button.
  ---- -x--  * HOLD 1 button.
  ---- x---  * HOLD 2 button.
  ---x ----  * HOLD 3 button.
  --x- ----  * HOLD 4 button.
  -x-- ----  * HOLD 5 button.
  x--- ----  * CANCEL button.


  PPI-1 (a0h-a3h); PortC OUT.
  Sound & Lamps:

  7654 3210
  ---- ---x  * DEAL / DRAW Lamp.
  ---- --x-  * BET / COLLECT Lamp.
  ---- -x--  + PANEL LIGHTS RESET (always activated after initalize).
  ---- x---  + PANEL LAMPS CLOCK
  xxxx ----  * Discrete Sound Lines.


-----------------------------------------------------------

  PPI-2 (a0h-a3h); PortA IN/OUT
  VRAM Handlers:

  7654 3210
  xxxx xxxx  VRAM DATA.


  PPI-2 (a0h-a3h); PortB OUT
  VRAM Handlers:

  7654 3210
  xxxx xxxx  VRAM ADDRESSING.


  PPI-2 (a0h-a3h); PortC IN/OUT.
  PortA handshake lines & PC0-PC2 (noraut11 = OUT; noraut12 = IN):

  7654 3210
  ---- ---x  * N/C (noraut11).
  ---- --x-  * N/C (noraut11).
  ---- -x--  * N/C (noraut11).
  ---- ---x  * N/C (noraut12).
  ---- --x-  * READOUT SWITCH (noraut12).
  ---- -x--  * LOW LEVEL HOPPER (noraut12).
  xxxx x---  * PortA HANDSHAKE LINES (all systems).


*******************************************************************************
*******************************************************************************


  Draw Poker Hi-Lo (1983).
  "NYMF O II" hardware.
  M. Kramer Inc.

  PCB layout (Draw Poker Hi-Lo)
   ___________________________________________________________________________
  |  _________                       ______                                   |
  | |HCF4093BE|           SN74174N  | U51  |  NE555P  916C472X2PE  ULN2003A   |
  | |         |                     |______|                                  |
  | |         |                                                               |
  | |MC14040  |  74123N   SN74157N            74161N  SN7486N      ULN2003A   |
  | |         |                                                               |
  | |         |                                                               |
  | |MWS5101  |  SN7404N  SN74166N  898-1-R   74161N               ULN2003A   |__
  | |         |                                                                __|
  | |         |            ______    _______                     ___________   __|
  | |5101E-1  |           | U31  |  |DIP-SW | 74161N  SN74157N  |AM8255 APC |  __|
  | |_________|           |______|  |_______|                   |___________|  __|
  |   ______                                                     ___________   __|
  |  | U26  |             2111A-2   2111A-2   74161N  SN74157N  | U20       |  __|
  |  |______|                                                   |___________|  __|
  |   ______     ______    ___________                                         __|
  |  | U19  |   | U18  |  |AM8255 APC |       74161N  SN74157N     898-1-R     __|
  |  |______|   |______|  |___________|                                        __|
  |              ______    __________                                         |
  |  74LS541N   | U12  |  |i D8228   |   OSC  74161N  SN7486N      SN7404N    |
  |             |______|  |__________|                                        |
  |              __________                                                   |
  |  DM7405N    |i P8080A  |     SN74LS155AN  iP8224  SN74157N 7411N 7474PC   |
  |             |__________|                                                  |
  |___________________________________________________________________________|

  OSC = 18.14 MHz

  U12 = AM2732
  U18 = AM2732
  U31 = AM2732A
  U51 = N82S129N

  U26, U19, U20 = unpopulated

  Edge connector is not JAMMA


*******************************************************************************


  Hardware Layout (Draw Poker Hi-Lo (alt)):

  Board layout/pcb tracks almost Identical to NORAUT boards.

  - CPU:             1x INTEL P8080A : L1087022 : INTEL '79.
  - RAM:             4x 2111A-2 Static Random Access Memory 256 x 4 bit.
  - I/O:             3x 8255 Peripeheral Interface Adapter.
  - Prg ROMs:        2x 2716 U11,U16 Eprom.
  - Gfx ROMs:        1x 2716 U27 Eprom :EXACT MATCH WITH NORAUT V3010A CHAR ROM.
  - Sound:           Discrete.
  - Crystal:         1x 18.000 MHz.

  PCB silksceened: REV A.
  PCB MARKED: Solderside "81 16".
  Component side "J3 018".

  U11 2716 EPROM MARKED:"2B27".
  U16 2716 EPROM MARKED:"4D30".

  Frequency measured on CPU P8080A (pins 15 & 22) = 2.00056 MHz.

  No date information found on PCB or in Roms.
  Some dates found on some of the IC's
  U6: 1979 :SOLDERED TO BOARD
  U10:1975 :SOLDERED TO BOARD
  U15:1979 :SOLDERED TO BOARD
  U23:1981 :IN SOCKET
  U27:1977 :IN SOCKET



  PCB Layout (Draw Poker Hi-Lo (alt)):                                        Edge Connector 36x2
   ______________________________________________________________________________________________
  |                         _________    _________    _____        .........     _________       |
  |                        |74LS174N |  |74LS153N |  |NE555|       .........    |ULN2003A |      |
  |       NO IC            |_________|  |_________|  |_____|       16-2-472     |_________|      |
  |        U46                 U45          U44        U43            U42          U41           |
  |                                                                                              |
  |                                                                                              |
  |                         _________    _________    _________    _________     _________       |
  |                        |74LS157N |  | 74153N  |  | 74161N  |  |  7486N  |   |ULN2003A |      |
  |       NO IC            |_________|  |_________|  |_________|  |_________|   |_________|      |
  |        U40                 U39          U38          U37          U36           U35          |
  |                                                                                              |
  |                                                                                              | 36
  |  _________              _________   916C471X2PE   _________    _________     _________       |___
  | |  7404N  |            | 74166N  |   .........   | 74161N  |  | 74153N  |   |ULN2003A |       ___|
  | |_________|            |_________|   .........   |_________|  |_________|   |_________|       ___|
  |     U34                    U33          U32          U31          U30           U29           ___|
  |                                   DIP SW x 8                               ________________   ___|
  |               _____________    _______________    _________    _________  |                |  ___|
  |  _________   |             |  |1|2|3|4|5|6|7|8|  | 74161N  |  | 74157N  | |    P8255A-5    |  ___|
  | | 2111A-2 |  |    2716     |  |_|_|_|_|_|_|_|_|  |_________|  |_________| |________________|  ___|
  | |_________|  |_____________|         U26             U25          U24            U23          ___|
  |     U28            U27                                                                        ___|
  |                                                                            ________________   ___|
  |                                                                           |                |  ___|
  |  _________        _________       _________       _________    _________  |    D8255AC-5   |  ___|
  | | 2111A-2 |      | 2111A-2 |     | 2111A-2 |     | 74161N  |  | 74157N  | |________________|  ___|
  | |_________|      |_________|     |_________|     |_________|  |_________|        U17          ___|
  |     U22              U21             U20             U19          U18                         ___|
  |                                                                                               ___|
  |  ______________       ________________                                                        ___|
  | |              |     |                |           _________    _________     916C471X2PE      ___|
  | |     2716     |     |   AM8255A PC   |          | 74161N  |  | 74157N  |     .........       ___|
  | |______________|     |________________|          |_________|  |_________|     .........       ___|
  |       U16                   U15                      U14          U13            U12          ___|
  |                                                                                              |
  |  ______________         ____________       _________       _________        _________        | 01
  | |              |       |            |     | 74161N  |     | 7486N   |      |  7404N  |       |
  | |     2716     |       |  i P8228   |     |_________|     |_________|      |_________|       |
  | |______________|       |____________|         U9              U8               U7            |
  |       U11                   U10           XTAL                                               |
  |                                          .----. 18Mhz                                        |
  |  ____________________     __________      _________    _________    _________    _________   |
  | |                    |   |  74155N  |    | i P8224 |  | 74157N  |  |  7411N  |  |  7474N  |  |
  | |     i P8080A       |   |__________|    |_________|  |_________|  |_________|  |_________|  |
  | |____________________|        U5              U4           U3           U2           U1      |
  |           U6                                                                                 |
  |______________________________________________________________________________________________|



  Draw Poker Hi-Lo (alt) discrete audio circuitry
  -----------------------------------------------

  3x ULN2003A (Darlington transistor array)
  1x NE555P   (Timer)
  1x F 2N4401 (NPN General Purpose Amplifier)


  .------.                              .------------.              .-------.
  |  U17 |                              |   NE555P   |              |2N4401 |
  |      |                             4|            |3     R3      |       |
  |   PC7|------------------------------|RST      OUT|-----ZZZZ-----|B     E|-------> Audio Out.
  |   PC6|----------.                  6|            |8             |   C   |
  |   PC5|-----.    |3-in         .-----|THR      VCC|-----------.  '---+---'  .----> Audio Out.
  |   PC4|--.  |  .-+------.      |    5|            |7          |      |      |
  |      |  |  |  |ULN2003A|      |  .--|CVOLT   DISC|--.        |      |      |
  |      |  |  |  '-+------'      |  |  |            |  |        +------'    --+--
  |      |  |  |    |3-out   C1   |  |  |    GND     |  |        |            GND
  | 8255 |  |  |    '--------||---+  |  '-----+------'  |        |             -
  |      |  |  '--.               |  |        |1        |        |             '
  |      |  |     |2-in           |  |        |         |        |
  '------'  |  .--+-----.         |  |   C5   |         |        |
            |  |ULN2003A|         |  '---||---+         |        |   +5V
            |  '--+-----'         |           |         |        |   -+-
            |     |2-out     C2   |      C4   |         |    C6  |    |
            |     '----------||---+------||---+-------. | .--||--+----'
            |2-in                 |           |       | | |      |
          .-+------.              |         --+--      '-'       |
          |ULN2003A|              |          GND        |        |
          '-+------'              |           -         |        |
            |2-out           C3   |     R1    '         |   R2   |
            '----------------||---+----ZZZZ-------------+--ZZZZ--'



  R1 = 120 K ; Tolerance +/- 5%
  R2 = 1 K   ; Tolerance +/- 5%
  R3 = 1 K   ; Tolerance +/- 5%

  C1 = .01 Z
  C2 = .022 Z
  C3 = 503   ; 50.000 pf = 50 nf = 0.05 uf.
  C4 = .01 Z
  C5 = .01 Z
  C6 = .1 Z


  All Capacitors are Ceramic Disc.

  ----------------------------------------------------------------------------

  Ports Map:
  ----------

  U23:
  PPI-0 (); PortA IN.
  DIP Switches bank:

  7654 3210
  ---- ---x  * DIP switch 8
  ---- --x-  * DIP switch 7
  ---- -x--  * DIP switch 6
  ---- x---  * DIP switch 5
  ---x ----  * DIP switch 4
  --x- ----  * DIP switch 3
  -x-- ----  * DIP switch 2
  x--- ----  * DIP switch 1


*******************************************************************************


  Hardware Layout (SMS Hi-Lo Double Up Joker Poker):

  - CPU:             1x AMD P8080A
  - RAM:             2x 2111A-2: Static Random Access Memory 256 x 4 bit.
  - RAM:             2x NEC D5101LC-1: 256x4 static CMOS RAM.
  - I/O:             3x 8255: Peripeheral Interface Adapter.
  - Prg ROMs:        2x 2732: U12,U18: Eprom.
  - Gfx ROMs:        1x 2716: U31: Eprom.
  - Sound:           Discrete.
  - Crystal:         1x 18.000 MHz.
  - PROM             1x 82S129: Bipolar PROM: U51.
                     1x 3.6 Vcc Battery.

  Frequency on CPU P8080A (pins 15 & 22) = 2.00032 MHz.


  PCB MARKED:

  Solderside:
  PCB silksceened: SMS Manufacturing Corporation.

  Component side:
  PCB silksceened: REV 2.
  PCB Engraved: "1350" "10-83".


  PCB Layout (SMS Hi-Lo Double Up Joker Poker):                                             Edge Connector 36x2
   ____________________________________________________________________________________________________________
  |  _________                            _________    _________    _____        .........     _________       |
  | |HCF4093BE|         NO IC            | 74174PC |  | 82S129N |  |NE555|       .........    |ULN2003A |      |
  | |_________|                          |_________|  |_________|  |_____|      916C471X2PE   |_________|      |
  |    U54               U53                 U52          U51        U50            U49          U48           |
  | ____________________                                                                                       |
  || 3.6v NI-CD BATTERY |                                                                                      |
  ||____________________|                                                                                      |
  | _________          _________          _________                 _________    _________     _________       |
  ||CD4040BE |        | 74123PC |        | 74157PC |     NO IC     | 74161   |  |  7486   |   |ULN2003A |      |
  ||_________|        |_________|        |_________|               |_________|  |_________|   |_________|      |
  |    U47                U46                U45          U44          U43          U42           U41          |
  |                                                                                                            |
  |                                                                                                            | 36
  | _________       _________             _________   MDP1601 471G  _________                  _________       |___
  ||D5101LC-1|     |  7404   |           |SN74166J |   .........   | 74161N  |     NO IC      |ULN2003A |       ___|
  ||_________|     |_________|           |_________|   .........   |_________|                |_________|       ___|
  |    U40             U39                   U38          U37          U36          U35           U34           ___|
  |                                                                                          ________________   ___|
  |                             _____________    _______________    _________    _________  |                |  ___|
  | _________                  |             |  |1|2|3|4|5|6|7|8|  | 74161   |  | 74157   | |   D8255AC-5    |  ___|
  ||D5101LC-1|       NO IC     |    2716     |  |_|_|_|_|_|_|_|_|  |_________|  |_________| |________________|  ___|
  ||_________|                 |_____________|         U30             U29          U28            U27          ___|
  |    U33            U32            U31            DIP SW x 8                                                  ___|
  |                                                                                          ________________   ___|
  |                                                                                         |                |  ___|
  |                                 _________       _________       _________    _________  |    D8255AC-5   |  ___|
  |   NO IC          NO IC         | 2111A-2 |     | 2111A-2 |     | 74161   |  | 74157   | |________________|  ___|
  |                                |_________|     |_________|     |_________|  |_________|        U20          ___|
  |    U26            U25              U24             U23             U22          U21                         ___|
  |                                                                                                             ___|
  |                ______________       ________________                                                        ___|
  |               |              |     |                |           _________    _________    MDP1601 471G      ___|
  |   NO IC       |     2732     |     |   D8255AC-5    |          | 74161   |  | 74157   |     .........       ___|
  |               |______________|     |________________|          |_________|  |_________|     .........       ___|
  |    U19              U18                   U17                      U16          U15            U14          ___|
  |                                                                                                            |
  |                ______________         ____________       _________       _________        _________        | 01
  | _________     |              |       |            |     | 74161N  |     |  7486   |      |  7404   |       |
  ||74LS 541F|    |     2732     |       | NEC B8228  |     |_________|     |_________|      |_________|       |
  ||_________|    |______________|       |____________|         U10             U9               U8            |
  |    U13              U12                   U11           XTAL                                               |
  |                                                        .----. 18Mhz                                        |
  | _________      ____________________     __________      _________    _________    _________    _________   |
  ||  7405   |    |                    |   | SN74155N |    |UPB 8224 |  | 74157   |  |  7411   |  |  7474   |  |
  ||_________|    |    AMD   P8080A    |   |__________|    |_________|  |_________|  |_________|  |_________|  |
  |    U7         |____________________|        U5              U4           U3           U2           U1      |
  |                        U6                                                                                  |
  |____________________________________________________________________________________________________________|



  SMS Hi-Lo Double Up Joker Poker discrete audio circuitry:
  --------------------------------------------------------

  3x ULN2003A (Darlington transistor array)
  1x NE555P   (Timer)
  1x PN2222   (Transistor)


  .------.                              .------------.              .-------.
  |  U17 |                              |   NE555P   |              |PN2222 |
  |      |                             4|            |3     R3      |       |
  |   PC7|------------------------------|RST      OUT|-----ZZZZ-----|B     E|-------> Audio Out.
  |   PC6|----------.                  6|            |8             |   C   |
  |   PC5|-----.    |3-in         .-----|THR      VCC|-----------.  '---+---'  .----> Audio Out.
  |   PC4|--.  |  .-+------.      |    5|            |7          |      |      |
  |      |  |  |  |ULN2003A|      |  .--|CVOLT   DISC|--.        |      |      |
  |      |  |  |  '-+------'      |  |  |            |  |        +------'    --+--
  |      |  |  |    |3-out   C1   |  |  |    GND     |  |        |            GND
  | 8255 |  |  |    '--------||---+  |  '-----+------'  |        |             -
  |      |  |  '--.               |  |        |1        |        |             '
  |      |  |     |2-in           |  |        |         |        |
  '------'  |  .--+-----.         |  |   C5   |         |        |
            |  |ULN2003A|         |  '---||---+         |        |   +5V
            |  '--+-----'         |           |         |        |   -+-
            |     |2-out     C2   |      C4   |         |    C6  |    |
            |     '----------||---+------||---+-------. | .--||--+----'
            |2-in                 |           |       | | |      |
          .-+------.              |         --+--      '-'       |
          |ULN2003A|              |          GND        |        |
          '-+------'              |           -         |        |
            |2-out           C3   |     R1    '         |   R2   |
            '----------------||---+----ZZZZ-------------+--ZZZZ--'



  R1 = 120 K ; Tolerance +/- 5%
  R2 = 1 K   ; Tolerance +/- 5%
  R3 = 1 K   ; Tolerance +/- 5%

  C1 = 103K = 10000 pF  =  10 nF = 0.01 uF
  C2 = .022 Z
  C3 = .05M
  C4 = .01 Z
  C5 = .01 Z
  C6 = .1 Z

  Similar circuitry and component values than DPHL PCB.


*******************************************************************************


  Hardware Layout (Turbo Poker 2 by Micro MFG):


  - CPU:             1x NEC D8080AFC-1 (U42).
  - BUS:             1x 8224 (U43)
  - RAM:             2x 2111-1 Static Random Access Memory 256 x 4 bit (U33 & U34).
  - I/O:             3x Intel P8255A Peripeheral Interface Adapter (U31, U36 & U38).
  - Prg ROMs:        1x 27256 (U39).
  - Gfx ROMs:        1x 2732 (U30).
  - Sound:           Discrete.
  - Crystal:         1x 18.000 MHz.


  Etched in copper on board:    TP2

  .U30  2732a    ; stickered  (c) 1993 MICRO MFG TURBO POKER CHAR, ROM.

  .U35  unknown  ; stickered  (c) 1993 MICRO MFG TP2#01 U35\IC4 16228 022194.

   Continuity errors when trying to read as a standard eprom.
   Silkscreened below the chip 'CUSTOM I.C.'. Looks like a normal EPROM.
   * (from other board that match 100% the set, it's a custom 68705 MCU)

  .U39  27256    ; stickered  (c) 1993 MICRO MFG TURBO-2 U39-014 US UTBK 022190.

  .U38  8255     ; stickered  MICRO MANUFACTURING, INC.  DATE: 02-24-1994  SER# LKY-PCB-142728.

  .U37  MMI PAL12L6-2  ; Blue dot on it. Saved in Jedec format.

  .U44  DS1220AD-150   ; Dallas 2K x 8 CMOS nonvolatile SRAM.

  .U23  82S131         ; Bipolar PROM.



        27256 @U39                               Estimated U35 pinouts

       .----------.                                   .----------.
  VPP -|01      28|- VCC                         GND -|01      28|- Pin 10 of U14 (7404)
  A12 -|02      27|- A14                         VCC -|02      27|- A7
   A7 -|03      26|- A13                         VCC -|03      26|- A6
   A6 -|04      25|- A8                          N/C -|04      25|- A5
   A5 -|05      24|- A9            Pull-up to pin 02 -|05      24|- A4
   A4 -|06      23|- A11                         VCC -|06      23|- A3
   A3 -|07      22|- /OE                         N/C -|07      22|- A2
   A2 -|08      21|- A10            Pin 9 of U37 PAL -|08      21|- A1
   A1 -|09      20|- /CE            Pin 8 of U37 PAL -|09      20|- A0
   A0 -|10      19|- D7         Pin 24 of U42 (8080) -|10      19|- D7
   D0 -|11      18|- D6             Pin 7 of U37 PAL -|11      18|- D6
   D1 -|12      17|- D5                           D0 -|12      17|- D5
   D2 -|13      16|- D4                           D1 -|13      16|- D4
  GND -|14      15|- D3                           D2 -|14      15|- D3
       '----------'                                   '----------'


  PCB Layout (Turbo Poker 2 by Micro MFG):                                         Edge Connector 36x2
   ___________________________________________________________________________________________________
  |  _________    _________    _________    _________    _________       _____      ________     _    |
  | | 74LS161 |  | 74LS161 |  | 74LS161 |  | 74LS161 |  | 74LS161 |     | 555 |    | KA2657 |  /   \  |
  | |_________|  |_________|  |_________|  |_________|  |_________|     |_____|    |________! | VR1 | |
  |     U1           U2           U3           U4           U5            U6           U7      \ _ /  |
  |                                                                                                   |
  |  _________    _________    _________    _________    _________                  ________          |
  | | 74LS161 |  | 74LS157 |  | 74LS157 |  | 74LS157 |  | 74LS157 |                | KA2657 |         |
  | |_________|  |_________|  |_________|  |_________|  |_________|                |________!         |
  |     U8           U9           U10          U11          U12                       U13             |
  |                                                                                                   |
  |  _________    _________    _________    _________    _________     _________    ________          | 36
  | | 74LS04P |  | 74LS11N |  | 74LS04P |  | DV7486N |  | DV7486N |   | CTS8427 |  | KA2657 |         |___
  | |_________|  |_________|  |_________|  |_________|  |_________|   |_________|  |________!          ___|
  |     U14          U15          U16          U17          U18       U19 (resnet)    U20              ___|
  |                                                                                                    ___|
  |  _________    _________    _________       _________________       _________    _______________    ___|
  | |  74123  |  | 74LS174 |  | 82S131N |     | 74LS541 (R dot) |     | CTS8427 |  |1|2|3|4|5|6|7|8|   ___|
  | |_________|  |_________|  |_________|     |_________________|     |_________|  |_|_|_|_|_|_|_|_|   ___|
  |     U21          U22          U23                 U24             U25 (resnet)  U26 (DIP SW x 8)   ___|
  |                                                                                                    ___|
  |  _________    _________    _________       __________________        ________________________      ___|
  | |  7474N  |  | 74LS157 |  | 74LS166 |     |                  |      |                        |     ___|
  | |_________|  |_________|  |_________|     | 2732A (char ROM) |      |     Intel  P8255A      |     ___|
  |     U27          U28          U29         |__________________|      |________________________|     ___|
  |                                                   U30                          U31                 ___|
  |  _________    __________   __________      ____________________      ________________________      ___|
  | |  7474N  |  | SY2111-1 | | SY2111-1 |    | Unknown custom ROM |    |                        |     ___|
  | |_________|  |__________| |__________|    |    (68705 MCU?)    |    |     Intel  P8255A      |     ___|
  |     U32          U33          U34         |____________________|    |________________________|     ___|
  |                                                    U35                         U36                 ___|
  |  _______________   ____________________    ____________________                                    ___|
  | |PAL12L6 (B dot)| |                    |  |                    |                                   ___|
  | |_______________| |  8255 (stickered)  |  |     27256 ROM      |                                  |
  |       U37         |____________________|  |____________________|                  __________      | 01
  |                           U38                    U39                             | TRW 8022 |     |
  |  ____________     _____________________    ____________________                  |__________|     |
  | | Intel 8224 |   |                     |  |                    |                     U45          |
  | |____________|   |   NEC  D8080AFC-1   |  |   8224 Clock GEN   |     ___________________          |
  |      U41         |_____________________|  |____________________|    |  Dallas DS1220AD  |         |
  |  ______                   U42                    U43                | Non Volatile SRAM |         |
  | | Xtal |                                                            |___________________|         |
  | | 18MHz|                                                                     U44                  |
  | |______|                                                                                          |
  |___________________________________________________________________________________________________|


  Discrete sound circuitry:
  -------------------------

                           ___ ___
  GND --------------------|1  U  8|-----VCC
                    N/C---|2     7|------------------------------------|---|1K Ohm|--- VCC
  Volume Pot -------------|3 555 6|--------------------|--|100K Ohm|---|
  Pin 10 U36 (8255)-------|4     5|---|0.1uF|---GND    |
                          |_______|                    |-|0.01uF|---- GND
                                                       |-|0.01uF|---- pin 12 U13 (KA2667)
                                                       |-|0.022uF|--- pin 11 U13 (KA2667)
                                                       |-|0.05uF|---- pin 10 U13 (KA2667)

  DIP Switches:

  DIP #1: SETUP      ON/OFF         ;"setup menu to change all the settings"
  DIP #2: RAISE      ON/OFF
  DIP #3: XCARDS     ON/OFF
  DIP #4: REPLAYS    ON/OFF
  DIP #5: BONUS      ON/OFF
  DIP #6: COIN TYPE  QUARTER/NICKEL
  DIP #7: HISCORE    ON/OFF         ;game saves high scores
  DIP #8: NOT USED


*******************************************************************************


  *** Game Notes ***


  - norautjp:

    At the first start-up, the game will give you a very clever
    "FU" screen. Press the following buttons *together* on different times
    to get rid of it (and actually initialize the machine):

    * start + bet buttons (1+2);
    * Hold 3 + Hold 2 + Save (Half Gamble) + Change Card (C+X+F+D)

    Also notice that you actually need to map the last four buttons on the
    same button / on a joypad since MAME's steady key doesn't seem to work on
    my end...


*******************************************************************************

  --------------------
  ***  Memory Map  ***
  --------------------


  Noraut HW:

  0x0000 - 0x1FFF    ; ROM space.
  0x2000 - 0x27FF    ; NVRAM.

  0x60 - 0x63        ; PPI 8255 0 - DIP Switches, lamps & counters.
  0xA0 - 0xA3        ; PPI 8255 1 - Regular Inputs, sound lines & remaining lamps.
  0xC0 - 0xC3        ; PPI 8255 2 - Video RAM access & other stuff.


  DPHL HW:

  0x0000 - 0x1FFF    ; ROM space.
  0x5000 - 0x53FF    ; NVRAM.

  0x7C - 0x7F        ; PPI 8255 0 - DIP Switches, lamps & counters.
  0xBC - 0xBF        ; PPI 8255 1 - Regular Inputs, sound lines & remaining lamps.
  0xDC - 0xDF        ; PPI 8255 2 - Video RAM access & other stuff.


*******************************************************************************


  DRIVER UPDATES:


  [2009-01-27]

  - Initial release.
  - Defined ROM, RAM.
  - Added 2x PPI 8255 for regular I/O.
  - Added complete inputs and hooked DIP switches.
  - Added video RAM support.
  - Added NVRAM.
  - Added lamps support.
  - Added coin counters.
  - Identified the sound writes.
  - Added hardware description.
  - Added pinout scheme.
  - Added technical notes.


  [2009-01-28]

  - Merged GTI Poker (gtipoker.c) with this driver.
  - Added new memory map and machine driver for gtipoker.
  - Hooked 2x PPI 8255 to gtipoker.
  - Hooked the video RAM access ports to gtipoker.
  - Changed norautpn description from Noraut Poker (No Payout),
    to Noraut Poker (bootleg), since the game has payout system.
  - Some clean-ups.


  [2009-08-21]

  - Switched to pre-defined Xtal clock.
  - Changed the way how graphics are banked/accessed.
  - Fixed the graphics offset and number of tiles per bank.
  - Added new set: Noraut Red Hot Joker Poker.
  - Added new set: Noraut Poker (NTX10A).
  - Added new set: Noraut Joker Poker (V3.010a).
  - Fixed the tile size/decode for the first GFX bank.
  - Added proper norautrh inputs, including the readout button.
  - Added partial DIP switches to norautrh.
  - Added more technical notes.


  [2009-08-23/26]

  - Added a default NVRAM to Noraut Joker Poker to bypass the 'F U' screen.
    This is due to the phisical keyboard limitation when needs to enter
    4 simultaneous inputs.
  - Executed a trojan on 2 noraut systems to confirm the way 16x32 tiles are decoded.
  - Fixed the x-offset for 32x32 tiles lines.
  - Fixed the screen aspect and visible area.
  - Confirmed correct colors. No bipolar PROM involved.
  - Added Noraut Joker Poker hardware and PCB layouts.
  - Documented the discrete audio circuitry. Added a full diagram.


  [2009-08-29]

  - Fixed the coin counters.
  - Documented all the output ports.
  - Added a scheme with descriptions for every existent port.
  - Added full lamps support to naroutp, naroutjp, naroutrh and naroutpn.
  - Created lamps layouts for 11 and 12-lamps scheme.
  - Rerouted some inputs to mantain the inputs layout.
  - Renamed some inputs to match the text with the real cab buttons.
  - Removed the imperfect colors flag from the existent sets.
  - Added 2 different control panel layouts to the source.
  - Updated technical notes.


  [2009-08-30]

  - Corrected CPU clock to Xtal/8.
  - Discovered 3 new I/O lines coming from PPI-2, PC0-PC2. They are mixed with the handshake ones.
  - Added the READOUT button to noraut12 games.
  - Splitted the main machine driver to cover 2 different Noraut systems.
  - Added partial support for PPI-2, PC0-PC2 output lines on noraut11 games.
  - Figured out other remaining I/O lines.
  - Added new handlers to simulate the handshake lines. Still need real support through PPI-2.
  - Updated technical notes.


  [2009-09-03]

  - Routed the whole video RAM access through PPI-2.
    (bypassed the handshake lines for now).
  - Merged back the noraut machine drivers after the 3rd PPI connection.
  - Added Low Level Hopper manual input.
  - Added a new machine driver for extended hardware.
    It has 2 jumpers that cut the a14 and a15 addressing lines.


  [2009-09-09]

  - Added accurate discrete sound system emulation.
  - Fixed the discrete sound system diagram, based on real sound references.


  [2009-10-12]

  - Added Draw Poker Hi-Lo hardware support, based on 8080A CPU.
  - Mirrored the PPI's offsets to simplify/merge the hardware emulation.
  - Added hardware documentation and PCB layouts from both DPHL sets.
  - Added DPHL discrete sound circuitry scheme/documentation.
  - Added Turbo Poker 2 from Micro Manufacturing.
  - Added PMA poker.
  - Documented the Turbo Poker 2 hardware.
  - Added Turbo Poker 2 PCB layout from hi-res picture.
  - Switched to the new PPI core.
  - Commented out the 3rd PPI device till handshaked strobe lines can be
    properly emulated. For now, all VRAM access is through direct handlers.
    This allow to remove the hacks per set needed to boot the games.


  [2009-10-13]

  - Added Draw Poker Hi-Lo (japanese), based on 8080A CPU.
  - Merged the gtipoker memory map and machine driver with dphl.
  - Created a base machine driver and then derivatives by hardware.
  - Splitted the regular RAM and NVRAM systems.
  - Added 'Hi-Lo Double Up Joker Poker' from SMS Manufacturing.
  - Added smshilo hardware details and PCB layout.
  - Added smshilo discrete sound circuitry scheme/documentation.


  [2009-11-26]

  - A lot of work in memory maps and machine driver stuff.
  - Improved Inputs / DIP switches for some games.
  - Added lamps support / layouts to new working games.
  - Added preliminary support to Kimble hardware.
  - Added support to CGI's Credit Poker 30x games.
  - Added new (and fixed old) technical notes.
  - Added Noraut Deluxe Poker (console).
  - Added Noraut Joker Poker (original).
  - Added Noraut Red Hot Joker Poker (alt HW).
  - Added Noraut Joker Poker (V3.011a).
  - Added Noraut unknown set 1 (console).
  - Added Noraut unknown set 2 (console).
  - Added Mainline Double Joker Poker.
  - Added Double Joker Poker (45%-75% payout).
  - Added Royal on Ten (Noraut Deluxe hack).
  - Added Credit Poker (ver.30c, standard).
  - Added Poker / Black Jack (Model 7521).
  - Added Kimble Joker Poker.
  - Added DRHL Poker (v.2.89).
  - Renamed norautpn descripion to Noraut Deluxe Poker (bootleg).
  - Added a placeholder for tpoker2's undumped 68705 MCU.
  - Reorganized the driver, plus some clean-ups.


  TODO:

  - Analize and hook the 3rd PPI device at 0xc0-0xc3.
    /OBF handshake line (PC7) doesn't seems to work properly.
  - Interrupts in i8080 based games.
  - Find if wide chars are hardcoded or tied to a bit.
  - Save support.
  - Parent/clone relationship.


*******************************************************************************/


#define NORAUT_MASTER_CLOCK		XTAL_18_432MHz
#define DPHL_MASTER_CLOCK		XTAL_18MHz
#define NORAUT_CPU_CLOCK		NORAUT_MASTER_CLOCK / 8		/* 2.30275 MHz - Measured: 2.305 MHz */
#define DPHL_CPU_CLOCK			DPHL_MASTER_CLOCK / 9		/* 2 MHz (from 8224) */

#include "driver.h"
#include "cpu/z80/z80.h"
#include "cpu/i8085/i8085.h"
#include "machine/i8255a.h"
#include "norautp.h"

#include "noraut11.lh"
#include "noraut12.lh"

static UINT16 *np_vram;
static UINT16 np_addr;


/*************************
*     Video Hardware     *
*************************/

static VIDEO_START( norautp )
{
	np_vram = auto_alloc_array(machine, UINT16, 0x1000/2);
}


static VIDEO_UPDATE( norautp )
{
	int x, y, count;

	count = 0;

	bitmap_fill(bitmap, cliprect, screen->machine->pens[0]); //black pen

	for(y = 0; y < 8; y++)
	{
		/* Double width, displaced 8 pixels in X */
		if(y == 2 || (y >= 4 && y < 6))
		{
			for(x = 0; x < 16; x++)
			{
				int tile = np_vram[count] & 0x3f;
				int colour = (np_vram[count] & 0xc0) >> 6;

				drawgfx_opaque(bitmap,cliprect, screen->machine->gfx[1], tile, colour, 0, 0, (x * 32) + 8, y * 32);

				count+=2;
			}
		}
		else
		{
			for(x = 0; x < 32; x++)
			{
				int tile = np_vram[count] & 0x3f;
				int colour = (np_vram[count] & 0xc0) >> 6;

				drawgfx_opaque(bitmap,cliprect, screen->machine->gfx[0], tile, colour, 0, 0, x * 16, y * 32);

				count++;
			}
		}
	}

	return 0;
}


static PALETTE_INIT( norautp )
{
	/* 1st gfx bank */
	palette_set_color(machine, 0, MAKE_RGB(0x00, 0x00, 0xff));	/* blue */
	palette_set_color(machine, 1, MAKE_RGB(0xff, 0xff, 0x00)); 	/* yellow */
	palette_set_color(machine, 2, MAKE_RGB(0x00, 0x00, 0xff));	/* blue */
	palette_set_color(machine, 3, MAKE_RGB(0xff, 0xff, 0xff));	/* white */
	palette_set_color(machine, 4, MAKE_RGB(0xff, 0xff, 0xff));	/* white */
	palette_set_color(machine, 5, MAKE_RGB(0xff, 0x00, 0x00));	/* red */
	palette_set_color(machine, 6, MAKE_RGB(0xff, 0xff, 0xff));	/* white */
	palette_set_color(machine, 7, MAKE_RGB(0x00, 0x00, 0x00));	/* black */
}


/*************************
*      R/W Handlers      *
*************************/

static WRITE8_DEVICE_HANDLER( mainlamps_w )
{
/*  PPI-0 (60h-63h); PortB OUT.
    Lamps:

    7654 3210
    ---- ---x  * CHANGE CARD lamp.
    ---- --x-  * SAVE / HALF GAMBLE lamp.
    ---- -x--  * HOLD 1 lamp.
    ---- x---  * HOLD 2 lamp.
    ---x ----  * HOLD 3 lamp.
    --x- ----  * HOLD 4 lamp.
    -x-- ----  * HOLD 5 lamp.
    x--- ----  * CANCEL lamp.
*/
	output_set_lamp_value(0, (data >> 0) & 1);	/* CHANGE CARD lamp */
	output_set_lamp_value(1, (data >> 1) & 1);	/* SAVE / HALF GAMBLE lamp */
	output_set_lamp_value(2, (data >> 2) & 1);	/* HOLD 1 lamp */
	output_set_lamp_value(3, (data >> 3) & 1);	/* HOLD 2 lamp */
	output_set_lamp_value(4, (data >> 4) & 1);	/* HOLD 3 lamp */
	output_set_lamp_value(5, (data >> 5) & 1);	/* HOLD 4 lamp */
	output_set_lamp_value(6, (data >> 6) & 1);	/* HOLD 5 lamp */
	output_set_lamp_value(7, (data >> 7) & 1);	/* CANCEL lamp */
}

static WRITE8_DEVICE_HANDLER( soundlamps_w )
{
/*  PPI-1 (a0h-a3h); PortC OUT.
    Sound & Lamps:

  7654 3210
  ---- ---x  * DEAL / DRAW Lamp.
  ---- --x-  * BET / COLLECT Lamp.
  ---- -x--  + PANEL LIGHTS RESET (always activated after initalize).
  ---- x---  + PANEL LAMPS CLOCK
  xxxx ----  * Discrete Sound Lines.
*/

	const device_config *discrete = devtag_get_device(device->machine, "discrete");

	output_set_lamp_value(8, (data >> 0) & 1);	/* DEAL / DRAW lamp */
	output_set_lamp_value(9, (data >> 1) & 1);	/* BET / COLLECT lamp */

	/* the 4 MSB are for discrete sound */
	discrete_sound_w(discrete, NORAUTP_SND_EN, (data >> 7) & 0x01);
	discrete_sound_w(discrete, NORAUTP_FREQ_DATA, (data >> 4) & 0x07);

//  popmessage("sound bits 4-5-6-7: %02x, %02x, %02x, %02x", ((data >> 4) & 0x01), ((data >> 5) & 0x01), ((data >> 6) & 0x01), ((data >> 7) & 0x01));
}

static WRITE8_DEVICE_HANDLER( counterlamps_w )
{
/*  PPI-0 (60h-63h); PortC OUT.
    Lamps & Coin Counters:

    7654 3210
    ---- ---x  * HI lamp.
    ---- --x-  * LO lamp.
    ---- -x--  * HOPPER MOTOR DRIVE
    ---- x---  * Payout pulse.
    ---x ----  * Coin 2 counter.
    --x- ----  * Coin 1 counter.
    -x-- ----  + Coin counter related.
    x--- ----  + DEFLECT (always activated).
*/
	output_set_lamp_value(10, (data >> 0) & 1);	/* HI lamp */
	output_set_lamp_value(11, (data >> 1) & 1);	/* LO lamp */

	coin_counter_w(0, data & 0x10);	/* Coin1/3 counter */
	coin_counter_w(1, data & 0x20);	/* Coin2 counter */
	coin_counter_w(2, data & 0x08);	/* Payout pulse */
}


/* Game waits for bit 7 (0x80) to be set.
   This should be the /OBF line (PC7) from PPI-2 (handshake).
   PC0-PC2 could be set as input or output.
*/

//static READ8_DEVICE_HANDLER( ppi2_portc_r )
//{
//  return;
//}

//static WRITE8_DEVICE_HANDLER( ppi2_portc_w )
//{
//  /* PC0-PC2 don't seems to be connected to any output */
//}


/*game waits for /OBF signal (bit 7) to be set.*/
static READ8_HANDLER( test_r )
{
	return 0xff;
}

static READ8_HANDLER( vram_data_r )
//static READ8_DEVICE_HANDLER( vram_data_r )
{
	return np_vram[np_addr];
}

static WRITE8_HANDLER( vram_data_w )
//static WRITE8_DEVICE_HANDLER( vram_data_w )
{
	np_vram[np_addr] = data & 0xff;

	/* trigger 8255-2 port C bit 7 (/OBF) */
//  i8255a_pc7_w(devtag_get_device(device->machine, "ppi8255_2"), 0);
//  i8255a_pc7_w(devtag_get_device(device->machine, "ppi8255_2"), 1);

}

static WRITE8_HANDLER( vram_addr_w )
//static WRITE8_DEVICE_HANDLER( vram_addr_w )
{
	np_addr = data;
}

/* game waits for bit 4 (0x10) to be reset.*/
static READ8_HANDLER( test2_r )
{
	return 0x00;
}


/*************************
* Memory Map Information *
*************************/
/*

  CPU & PPI settings by set...

  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  |   Set    |   CPU   | PPI-0 offset | config | PPI-1 offset | config | PPI-2 offset |         config         |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | norautp  |   Z80   |  0x60-0x63   |  0x90  |  0xA0-0xA3   |  0x92  |  0xC0-0xC3   |  0xC1 (PC0-2 as input) |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | norautdx |   Z80   |  0x60-0x63   |  0x90  |  0xA0-0xA3   |  0x92  |  0xC0-0xC3   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | norautpn |   Z80   |  0x60-0x63   |  0x90  |  0xA0-0xA3   |  0x92  |  0xC0-0xC3   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | norautjp |   Z80   |  0x60-0x63   |  0x90  |  0xA0-0xA3   |  0x92  |  0xC0-0xC3   |  0xC1 (PC0-2 as input) |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | norautrh |   Z80   |  0x60-0x63   |  0x90  |  0xA0-0xA3   |  0x92  |  0xC0-0xC3   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | norautu  |  BOXED  |  0x60-0x63   |  0x90  |  0xA0-0xA3   |  0x92  |  0xC0-0xC3   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | norautv3 |   Z80   |   unknown    |        |   unknown    |        |   unknown    |                        |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | pma      |   Z80   |  0x7C-0x7F   |  0x90  |  0xBC-0xBF   |  0x92  |  0xDC-0xDF   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | bjpoker  |   Z80   |  0x7C-0x7F   |  0x90  |  0xBC-0xBF   |  0x92  |  0xDC-0xDF   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | dphl     |  8080A  |  0x7C-0x7F   |  0x90  |  0xBC-0xBF   |  0x92  |  0xDC-0xDF   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | dphla    |  8080A  |  0x60-0x63   |  0x90  |  0xA0-0xA3   |  0x92  |  0xC0-0xC3   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | dphljp   |  8080A  |  0x7C-0x7F   |  0x90  |  0xBC-0xBF   |  0x92  |  0xDC-0xDF   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | gtipoker |  8080A  |  0x7C-0x7F   |  0x90  |  0xBC-0xBF   |  0x92  |  0xDC-0xDF   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | smshilo  |  8080A  |  0x7C-0x7F   |  0x90  |  0xBC-0xBF   |  0x92  |  0xDC-0xDF   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | tpoker2  |  8080A  |  0x7C-0x7F   |  0x90  |  0xBC-0xBF   |  0x92  |  0xDC-0xDF   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+
  | drhl     |  8080A? |  0x7C-0x7F   |  0x90  |  0xBC-0xBF   |  0x92  |  0xDC-0xDF   |          0xC0          |
  +----------+---------+--------------+--------+--------------+--------+--------------+------------------------+

*/
static ADDRESS_MAP_START( norautp_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0x3fff)
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0x2000, 0x27ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)	/* 6116 */
ADDRESS_MAP_END

static ADDRESS_MAP_START( norautp_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x60, 0x63) AM_MIRROR(0x1c) AM_DEVREADWRITE("ppi8255_0", i8255a_r, i8255a_w)
	AM_RANGE(0xa0, 0xa3) AM_MIRROR(0x1c) AM_DEVREADWRITE("ppi8255_1", i8255a_r, i8255a_w)
//  AM_RANGE(0xc0, 0xc3) AM_MIRROR(0x3c) AM_DEVREADWRITE("ppi8255_2", i8255a_r, i8255a_w)
	AM_RANGE(0xc0, 0xc0) AM_MIRROR(0x3c) AM_READWRITE(vram_data_r, vram_data_w)
	AM_RANGE(0xc1, 0xc1) AM_MIRROR(0x3c) AM_WRITE(vram_addr_w)
	AM_RANGE(0xc2, 0xc2) AM_MIRROR(0x3c) AM_READ(test_r)
	AM_RANGE(0xef, 0xef) AM_READ(test2_r)
ADDRESS_MAP_END

/*
  Video RAM R/W:

  c0 --> W  ; data (hanshaked)
  c1 --> W  ; addressing
  c2 --> R  ; status (handshaking lines) + input (PC0-2)
  c3 --> W  ; PPI control + alternate 00 & 01 (PC1 out?)

  PPI Mirror isn't accurate.
  There are writes to 0xF7 and reads + compare to 0xEF.
  Don't know what's supposed to be mirrored there.

*/

static ADDRESS_MAP_START( nortest1_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0x7fff)
	AM_RANGE(0x0000, 0x2fff) AM_ROM
	AM_RANGE(0x5000, 0x57ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
ADDRESS_MAP_END

static ADDRESS_MAP_START( norautxp_map, ADDRESS_SPACE_PROGRAM, 8 )
//	ADDRESS_MAP_GLOBAL_MASK(~0x4000)
	ADDRESS_MAP_GLOBAL_MASK(0x7fff)
	AM_RANGE(0x0000, 0x3fff) AM_ROM	/* need to be checked */
	AM_RANGE(0x6000, 0x67ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size) /* HM6116 */
ADDRESS_MAP_END

static ADDRESS_MAP_START( norautx4_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x3fff) AM_ROM	/* need to be checked */
	AM_RANGE(0x6000, 0x67ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size) /* 6116 */
ADDRESS_MAP_END

static ADDRESS_MAP_START( norautx8_map, ADDRESS_SPACE_PROGRAM, 8 )
	AM_RANGE(0x0000, 0x7fff) AM_ROM	/* need to be checked */
	AM_RANGE(0xc000, 0xc7ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size) /* 6116 */
ADDRESS_MAP_END

static ADDRESS_MAP_START( norautxp_portmap, ADDRESS_SPACE_IO, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
ADDRESS_MAP_END


/*********** 8080 based **********/

static ADDRESS_MAP_START( dphl_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0x7fff)	/* A15 not connected */
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x5000, 0x53ff) AM_RAM	AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)	/* should be 2x 0x100 segments (4x 2111) */
ADDRESS_MAP_END

static ADDRESS_MAP_START( dphla_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0x3fff)
	AM_RANGE(0x0000, 0x1fff) AM_ROM
	AM_RANGE(0x2000, 0x23ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
ADDRESS_MAP_END

static ADDRESS_MAP_START( dphltest_map, ADDRESS_SPACE_PROGRAM, 8 )
//  ADDRESS_MAP_GLOBAL_MASK(0x7fff) /* A15 not connected */
	AM_RANGE(0x0000, 0x6fff) AM_ROM
	AM_RANGE(0x7000, 0x7fff) AM_RAM
	AM_RANGE(0x8000, 0x87ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
ADDRESS_MAP_END

/*
DPHL

  7F -> 90
  BF -> 92
  DF -> C0 (hndshk)

DPHLA

  63 -> 90
  A3 -> 92
  C3 -> C0 (hndshk)

*/

static ADDRESS_MAP_START( drhl_map, ADDRESS_SPACE_PROGRAM, 8 )
	ADDRESS_MAP_GLOBAL_MASK(0x7fff)	/* A15 not connected */
	AM_RANGE(0x0000, 0x3fff) AM_ROM
	AM_RANGE(0x5000, 0x53ff) AM_RAM AM_BASE(&generic_nvram) AM_SIZE(&generic_nvram_size)
	AM_RANGE(0x5400, 0x57ff) AM_RAM
ADDRESS_MAP_END


/*************************
*      Input Ports       *
*************************/

static INPUT_PORTS_START( norautp )

	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_GAMBLE_DEAL ) PORT_NAME("Deal / Draw")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_POKER_BET )   PORT_NAME("Bet / Collect")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_IMPULSE(2)	/* Coin A */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_IMPULSE(2)	/* Coin B */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_OTHER )       PORT_CODE(KEYCODE_K) PORT_NAME("IN0-5")
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_GAMBLE_HIGH ) PORT_NAME("Hi")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_GAMBLE_LOW )  PORT_NAME("Lo")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_GAMBLE_PAYOUT )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )     PORT_CODE(KEYCODE_F) PORT_NAME("Change Card")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_GAMBLE_HALF ) PORT_NAME("Half Gamble / Save")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_POKER_HOLD1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_POKER_HOLD2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_POKER_HOLD3 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_POKER_HOLD4 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_POKER_HOLD5 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN3 ) PORT_IMPULSE(2)	/* Coin C */

	PORT_START("IN2")	/* Only 3 lines: PPI-2; PC0-PC2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER )       PORT_CODE(KEYCODE_J) PORT_NAME("IN2-1")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_SERVICE )     PORT_CODE(KEYCODE_9) PORT_NAME("Readout")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_OTHER )       PORT_CODE(KEYCODE_L) PORT_NAME("Low Level Hopper")


	PORT_START("DSW1")
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
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x20, "A=5; B=25; C=1" )
	PORT_DIPSETTING(    0x00, "A=50; B=25; C=5" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Set Value" )
	PORT_DIPSETTING(    0x80, "2 Pence" )
	PORT_DIPSETTING(    0x00, "10 Pence" )
INPUT_PORTS_END

static INPUT_PORTS_START( norautrh )

	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_GAMBLE_DEAL ) PORT_NAME("Deal / Draw")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_POKER_BET )   PORT_NAME("Bet / Change Card")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_IMPULSE(2)	/* Coin A */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_IMPULSE(2)	/* Coin B */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE )     PORT_NAME("Readout") PORT_CODE(KEYCODE_9)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_GAMBLE_HIGH ) PORT_NAME("Hi")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_GAMBLE_LOW )  PORT_NAME("Lo")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_GAMBLE_PAYOUT )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_GAMBLE_TAKE ) PORT_NAME("Stand / Take")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_GAMBLE_HALF ) PORT_NAME("Save / Half Gamble")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_POKER_HOLD1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_POKER_HOLD2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_POKER_HOLD3 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_POKER_HOLD4 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_POKER_HOLD5 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_POKER_CANCEL )	/* Coin C for other games */

	PORT_START("IN2")	/* Only 3 lines: PPI-2; PC0-PC2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )


	PORT_START("DSW1")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW1:8")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )	PORT_DIPLOCATION("DSW1:7")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "Bet Max" )			PORT_DIPLOCATION("DSW1:6")
	PORT_DIPSETTING(    0x04, "5" )
	PORT_DIPSETTING(    0x00, "25" )
	PORT_DIPNAME( 0x08, 0x08, "Raise Ante" )		PORT_DIPLOCATION("DSW1:5")
	PORT_DIPSETTING(    0x08, "Random" )
	PORT_DIPSETTING(    0x00, "Always" )
	PORT_DIPNAME( 0x10, 0x00, "Type of Game" )		PORT_DIPLOCATION("DSW1:4")
	PORT_DIPSETTING(    0x10, "Jacks Plus" )
	PORT_DIPSETTING(    0x00, "Joker Poker" )
	PORT_DIPNAME( 0xa0, 0x20, DEF_STR( Coinage ) )	PORT_DIPLOCATION("DSW1:3,1")
	PORT_DIPSETTING(    0x00, "A=1; B=5" )
	PORT_DIPSETTING(    0xa0, "A=5; B=25" )
	PORT_DIPSETTING(    0x20, "A=10; B=5" )
	PORT_DIPSETTING(    0x80, "A=50; B=25" )
	PORT_DIPNAME( 0x40, 0x00, "Show Bet")			PORT_DIPLOCATION("DSW1:2")
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( norautpn )

	PORT_START("IN0")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_GAMBLE_DEAL ) PORT_NAME("Deal / Start")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_POKER_BET )   PORT_NAME("Bet / Change Card")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 ) PORT_IMPULSE(2)	/* Coin A */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 ) PORT_IMPULSE(2)	/* Coin B */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE )     PORT_NAME("Readout") PORT_CODE(KEYCODE_9)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_GAMBLE_HIGH ) PORT_NAME("Hi")
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_GAMBLE_LOW )  PORT_NAME("Lo")
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_GAMBLE_PAYOUT )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_GAMBLE_TAKE ) PORT_NAME("Stand / Take")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_GAMBLE_HALF ) PORT_NAME("Save / Half Gamble")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_POKER_HOLD1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_POKER_HOLD2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_POKER_HOLD3 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_POKER_HOLD4 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_POKER_HOLD5 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_POKER_CANCEL )

	PORT_START("IN2")	/* Only 3 lines: PPI-2; PC0-PC2 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNUSED )


	PORT_START("DSW1")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "Bet Max" )
	PORT_DIPSETTING(    0x04, "1" )
	PORT_DIPSETTING(    0x00, "5" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x08, "A=1; B=5" )
	PORT_DIPSETTING(    0x00, "A=50; B=5" )
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
INPUT_PORTS_END

static INPUT_PORTS_START( mainline )

	PORT_INCLUDE( norautrh )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0xa0, 0x20, DEF_STR( Coinage ) )	PORT_DIPLOCATION("DSW1:3,1")
	PORT_DIPSETTING(    0x00, "A=1; B=10" )
	PORT_DIPSETTING(    0xa0, "A=1; B=25" )
	PORT_DIPSETTING(    0x20, "A=10; B=10" )
	PORT_DIPSETTING(    0x80, "A=50; B=25" )
INPUT_PORTS_END

static INPUT_PORTS_START( ndxron10 )

	PORT_INCLUDE( norautpn )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x08, "A=1; B=5" )
	PORT_DIPSETTING(    0x00, "A=1; B=25" )
INPUT_PORTS_END

static INPUT_PORTS_START( norautkl )

	PORT_INCLUDE( norautrh )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x01, 0x00, "Bet Max" )				PORT_DIPLOCATION("DSW1:8")
	PORT_DIPSETTING(    0x01, "25" )
	PORT_DIPSETTING(    0x00, "10" )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )		PORT_DIPLOCATION("DSW1:7")
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, "Raise Ante" )			PORT_DIPLOCATION("DSW1:6")
	PORT_DIPSETTING(    0x04, DEF_STR( None ) )
	PORT_DIPSETTING(    0x00, "Random" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown )  )		PORT_DIPLOCATION("DSW1:5")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Type of Game" )			PORT_DIPLOCATION("DSW1:4")
	PORT_DIPSETTING(    0x10, "Deluxe Poker" )
	PORT_DIPSETTING(    0x00, "Joker Poker" )
	PORT_DIPNAME( 0x20, 0x20, "Min Bet for Jokers" )	PORT_DIPLOCATION("DSW1:3")
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x20, "4" )
	PORT_DIPNAME( 0x40, 0x00, "Deal Speed")				PORT_DIPLOCATION("DSW1:2")
	PORT_DIPSETTING(    0x40, "Slow" )
	PORT_DIPSETTING(    0x00, "Fast" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Coinage ) )		PORT_DIPLOCATION("DSW1:1")
	PORT_DIPSETTING(    0x00, "A=5; B=2" )
	PORT_DIPSETTING(    0x80, "A=1; B=1" )
INPUT_PORTS_END


/*************************
*    Graphics Layouts    *
*************************/

static const gfx_layout charlayout =
/*
  Trojanned 2 Narout Poker PCBs to see how the hardware decodes
  the 16x32 tiles. The following GFX layout is 100% accurate.
*/
{
	16, 32,
	RGN_FRAC(1,2),
	1,
	{ 0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
	{ 0*16, 0*16, 0*16, 0*16, 0*16, 0*16, 0*16, 0*16, 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
	  8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16, 0*16, 0*16, 0*16, 0*16, 0*16, 0*16, 0*16, 0*16, },
	16*16
};

static const gfx_layout charlayout32x32 =
{
	32,32,
	RGN_FRAC(1,2),
	1,
	{ 0 },
	{ 0,0, 1,1, 2,2, 3,3, 4,4, 5,5, 6,6, 7,7, 8,8, 9,9, 10,10, 11,11, 12,12, 13,13, 14,14, 15,15 },
	{ 0*16, 0*16, 1*16, 1*16, 2*16, 2*16, 3*16, 3*16, 4*16, 4*16, 5*16, 5*16, 6*16, 6*16, 7*16, 7*16,
	  8*16, 8*16, 9*16, 9*16, 10*16,10*16,11*16,11*16,12*16,12*16,13*16,13*16,14*16,14*16,15*16,15*16 },
	16*16
};


/******************************
* Graphics Decode Information *
******************************/

/* GFX are stored in the 2nd half... Maybe the HW could handle 2 bitplanes? */
static GFXDECODE_START( norautp )
	GFXDECODE_ENTRY( "gfx", 0x800, charlayout,      0, 4 )
	GFXDECODE_ENTRY( "gfx", 0x800, charlayout32x32, 0, 4 )
GFXDECODE_END


/************************************
*      PPI 8255 (x3) Interface      *
************************************/

static I8255A_INTERFACE (ppi8255_intf_0)
{
	/* (60-63) Mode 0 - Port A set as input */
	DEVCB_INPUT_PORT("DSW1"),		/* Port A read */
	DEVCB_NULL,						/* Port B read */
	DEVCB_NULL,						/* Port C read */
	DEVCB_NULL,						/* Port A write */
	DEVCB_HANDLER(mainlamps_w),		/* Port B write */
	DEVCB_HANDLER(counterlamps_w)	/* Port C write */
};

static I8255A_INTERFACE (ppi8255_intf_1)
{
	/* (a0-a3) Mode 0 - Ports A & B set as input */
	DEVCB_INPUT_PORT("IN0"),		/* Port A read */
	DEVCB_INPUT_PORT("IN1"),		/* Port B read */
	DEVCB_NULL,						/* Port C read */
	DEVCB_NULL,						/* Port A write */
	DEVCB_NULL,						/* Port B write */
	DEVCB_HANDLER(soundlamps_w)		/* Port C write */
};

//static I8255A_INTERFACE (ppi8255_intf_2)
//{
    /* (c0-c3) Group A Mode 2 (5-lines handshacked bidirectional port)
               Group B Mode 0, output;  (see below for lines PC0-PC2) */
//  DEVCB_HANDLER(vram_data_r),     /* Port A read (VRAM data read)*/
//  DEVCB_NULL,                     /* Port B read */
//  DEVCB_HANDLER(ppi2_portc_r),    /* Port C read */
//  DEVCB_HANDLER(vram_data_w),     /* Port A write (VRAM data write) */
//  DEVCB_HANDLER(vram_addr_w),     /* Port B write (VRAM address write) */
//  DEVCB_HANDLER(ppi2_portc_w)     /* Port C write */

	/*  PPI-2 is configured as mixed mode2 and mode0 output.
        It means that port A should be bidirectional and port B just as output.
        Port C as hshk regs, and P0-P2 as input (norautp, norautjp) or output (other sets).
    */
//};


/*************************
*    Machine Drivers     *
*************************/

static MACHINE_DRIVER_START( noraut_base )

	/* basic machine hardware */
	MDRV_CPU_ADD("maincpu", Z80, NORAUT_CPU_CLOCK)
	MDRV_CPU_PROGRAM_MAP(norautp_map)
	MDRV_CPU_IO_MAP(norautp_portmap)

	MDRV_NVRAM_HANDLER(generic_0fill)	/* doesn't work if placed at derivative drivers */

	/* 3x 8255 */
	MDRV_I8255A_ADD( "ppi8255_0", ppi8255_intf_0 )
	MDRV_I8255A_ADD( "ppi8255_1", ppi8255_intf_1 )
//  MDRV_I8255A_ADD( "ppi8255_2", ppi8255_intf_2 )

	/* video hardware */
	MDRV_SCREEN_ADD("screen", RASTER)
	MDRV_SCREEN_REFRESH_RATE(60)
	MDRV_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(0))
	MDRV_SCREEN_FORMAT(BITMAP_FORMAT_INDEXED16)
	MDRV_SCREEN_SIZE(32*16, 32*16)
	MDRV_SCREEN_VISIBLE_AREA(3*16, 31*16-1, (0*16) + 8, 16*16-1)	/* the hardware clips the top 8 pixels */

	MDRV_GFXDECODE(norautp)

	MDRV_PALETTE_INIT(norautp)
	MDRV_PALETTE_LENGTH(8)
	MDRV_VIDEO_START(norautp)
	MDRV_VIDEO_UPDATE(norautp)

	/* sound hardware */
	MDRV_SPEAKER_STANDARD_MONO("mono")
	MDRV_SOUND_ADD("discrete", DISCRETE, 0)
	MDRV_SOUND_CONFIG_DISCRETE(norautp)
	MDRV_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( norautp )
	MDRV_IMPORT_FROM(noraut_base)

	/* basic machine hardware */
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( norautxp )
	MDRV_IMPORT_FROM(noraut_base)

	/* basic machine hardware */
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(norautxp_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( nortest1 )
	MDRV_IMPORT_FROM(noraut_base)

	/* basic machine hardware */
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(nortest1_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( norautx4 )
	MDRV_IMPORT_FROM(noraut_base)

	/* basic machine hardware */
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(norautx4_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( norautx8 )
	MDRV_IMPORT_FROM(noraut_base)

	/* basic machine hardware */
	MDRV_CPU_MODIFY("maincpu")
	MDRV_CPU_PROGRAM_MAP(norautx8_map)
	MDRV_CPU_VBLANK_INT("screen", irq0_line_hold)
MACHINE_DRIVER_END


/**** 8080A based ****/


static MACHINE_DRIVER_START( dphl )
	MDRV_IMPORT_FROM(noraut_base)

	/* basic machine hardware */
	MDRV_CPU_REPLACE("maincpu", 8080, DPHL_CPU_CLOCK)
	MDRV_CPU_PROGRAM_MAP(dphl_map)

	/* sound hardware */
	MDRV_SOUND_MODIFY("discrete")
	MDRV_SOUND_CONFIG_DISCRETE(dphl)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( dphla )
	MDRV_IMPORT_FROM(noraut_base)

	/* basic machine hardware */
	MDRV_CPU_REPLACE("maincpu", 8080, DPHL_CPU_CLOCK)
	MDRV_CPU_PROGRAM_MAP(dphla_map)

	/* sound hardware */
	MDRV_SOUND_MODIFY("discrete")
	MDRV_SOUND_CONFIG_DISCRETE(dphl)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( dphltest )
	MDRV_IMPORT_FROM(noraut_base)

	/* basic machine hardware */
	MDRV_CPU_REPLACE("maincpu", 8080, DPHL_CPU_CLOCK)
	MDRV_CPU_PROGRAM_MAP(dphltest_map)

	/* sound hardware */
	MDRV_SOUND_MODIFY("discrete")
	MDRV_SOUND_CONFIG_DISCRETE(dphl)
MACHINE_DRIVER_END


static MACHINE_DRIVER_START( drhl )
	MDRV_IMPORT_FROM(noraut_base)

	/* basic machine hardware */
	MDRV_CPU_REPLACE("maincpu", 8080, DPHL_CPU_CLOCK)
	MDRV_CPU_PROGRAM_MAP(drhl_map)

	/* sound hardware */
	MDRV_SOUND_MODIFY("discrete")
	MDRV_SOUND_CONFIG_DISCRETE(dphl)
MACHINE_DRIVER_END


/*************************
*        Rom Load        *
*************************/

/************************************** Z80 sets **************************************/
/*  The following ones are 'Draw Poker Hi-Lo' type, running in a Z80 based hardware   */
/**************************************************************************************/

/*

  Noraut Poker.

  The First one released by Noraut.

*/

ROM_START( norautp )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "jpoker.bin",	    0x0000,  0x2000,  CRC(e22ed34d) SHA1(108f034335b5bed183ee316a61880f7b9485b34f) )

	ROM_REGION( 0x10000, "gfx", 0 )
	ROM_LOAD( "displayrom.bin",	0x00000, 0x10000, CRC(ed3605bd) SHA1(0174e880835815558328789226234e36b673b249) )
ROM_END

/*

  Noraut Deluxe Poker (console).
  From PCB with daghterboard.

  ----

  - Noraut deluxe 24 pin z80.bin

  Norauts original or first launch into the market.
  No Jokers, lose on same card in gamble.

  - Noraut deluxe 24pin console.bin

  Requires daughterboard console in z80 to run.
  Same as Noraut 24 pin chip with z80, but more stable.

*/

ROM_START( norautdx )
	ROM_REGION( 0x10000, "maincpu", 0 )	/* console version */
	ROM_LOAD( "noraut_deluxe_24pin_console.bin", 0x0000, 0x1000, CRC(d41bd404) SHA1(52e984ca28a15a1485ca672dd1fef973cf0c7617) )
	ROM_LOAD( "noraut_deluxe_24pin_z80.bin",     0x1000, 0x0800, CRC(c70bc8f9) SHA1(d947be4e6741f3a884ceca76d1a0fd13625a5f78) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(                     0x0000, 0x0800, 0xff )
	ROM_LOAD( "graphics2716.bin", 0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )
ROM_END

/*

  Poker game - manufacturer unknown.
  Seems a bootleg of Noraut Deluxe poker.

  Z80 CPU

  Program rom = 2764 (2nd Half blank)
  Character rom = 2732

  18.432 Mhz crystal

  sound discrete with ne555 timer chip (located near amp/volume control).

*/

ROM_START( norautpn )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "prog.bin",   0x0000, 0x2000, CRC(8b1cfd24) SHA1(d673baed1c1e5b54a34b7a5857b269a725737e92) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_LOAD( "char.bin",   0x0000, 0x1000, CRC(955eac6f) SHA1(470d8bad1a5d2a0a08dd129e6393c3c3a4ef2159) )
ROM_END

/*

  Noraut Joker Poker (original)

  Noraut 4bet joker chip U1 24pin z80.
  Noraut 4bet joker chip U16 24pin z80.

  ----

  Above two program chips used in old boards
  with 2x 32k program sockets.

*/

ROM_START( norautjo )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "noraut4betjoker.u1",  0x0000, 0x1000, CRC(082f28c6) SHA1(995b991dad50373c8ba9b63002f124b3f87e7889) )
	ROM_LOAD( "noraut4betjoker.u16", 0x1000, 0x1000, CRC(1452cac3) SHA1(ddc4b195c3bd8a3ff56f5c7da050d6136e442323) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(                     0x0000, 0x0800, 0xff )
	ROM_LOAD( "graphics2716.bin", 0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )
ROM_END

/*

  Noraut Joker Poker

  Program:27C64
  Marked:
  "MX10A JOKER G.L
   N.C.R  C.C  M.S"

  Char:2732
  Marked:
  "N1-057-5"

  CPU: TMPZ84C00AP-8

*/

ROM_START( norautjp )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "2764-1prog.bin",   0x0000, 0x2000, CRC(5f776ce1) SHA1(673b8c67ebd5c1334187a9407b86a43150cbe67b) )

	ROM_REGION( 0x1000,	"gfx", 0 )
	ROM_FILL(                     0x0000, 0x0800, 0xff )
	ROM_LOAD( "2732-1char.bin",   0x0800, 0x0800, CRC(d94be899) SHA1(b7212162324fa2d67383a475052e3b351bb1af5f) ) 	/* first half 0xff filled */
	ROM_CONTINUE(                 0x0800, 0x0800 )

	ROM_REGION( 0x400,	"nvram", 0 )
	ROM_LOAD( "norautjp_nv.bin",  0x0000, 0x0400, CRC(0a0614b2) SHA1(eb21b2723b41743daf787cfc379bc67cce2b8538) )	/* default NVRAM */

ROM_END

/*

  Noraut Red Hot Joker Poker.

  Red hot joker poker scrolls across screen.
  Eprom has Red Hot on sticker.
  Char: Handwritten sticker with "Club250 grapics" on it.

  Pressing the readout button brings you to a menu with RESET / READOUT.
  Pressing on Readout brings you to "coins in", "coins out" and "balance".

  No date info on board or found in rom.

*/

ROM_START( norautrh )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "norautrh.bin",  0x0000, 0x2000, CRC(f5447d1a) SHA1(75d6439481e469e82e5561146966c9c7b44f34fe) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_LOAD( "club250.bin",   0x0000, 0x1000, CRC(d94be899) SHA1(b7212162324fa2d67383a475052e3b351bb1af5f) )
ROM_END

/*

  Noraut Red Hot 1-bet Joker Poker.

  Last release of Noraut's Joker Poker. Always have joker with multi change card 
  and hint card in gamble. Quite popular in Norther Ireland but some operators 
  have the percentage to payout at less than 10%

*/

ROM_START( norautra )
	ROM_REGION( 0x10000, "maincpu", 0 )	/* Program ROM is 0000-1fff and identical to norautrh, the rest is filled with FF's */
	ROM_LOAD( "noraut_red_hot_1bet_joker_poker.bin", 0x0000, 0x8000, CRC(f284b574) SHA1(ff683731f3dbdaed5d0d25276ca90b68a422e403) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(                     0x0000, 0x0800, 0xff )
	ROM_LOAD( "graphics2716.bin", 0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )
ROM_END

/*

  Unknown Noraut: "NTX10A  V6"
  None working old noraut board with daughter card upgrade
  daughter card looks like an old upgrade PCB marked:
  "Noraut LTD Game Module"
  half of which is incased in epoxy resin.
  only thing not visble on this board compared to others i have is the cpu
  with is under the epoxy not sure what else is their.

  D Card contains:
  Backup Battery

  Program Eprom:27C256
  Marked: "NTX10A  V6"
           "C201"

  CPU:
  Unknown Incased in epoxy

  NVRAM: HY6264A

  PAL:
  PAL16L8ACN

  Charcter Eprom is mounted on main board
  CHAR Eprom:2732
  Marked: "GU27"

  daughter card is connected on to another card containing only pcb tracks no components
  This second board connects to main board with ribbon cable to the 40pin socket where
  the original cpu would of been.

  No date info on board or found in rom.

*/

ROM_START( norautu )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "2563.bin",	0x0000, 0x8000, CRC(6cbe68bd) SHA1(93201baaf03a9bba6c52c64cc26e8e445aa6454e) )
	ROM_RELOAD(				0x8000, 0x8000 )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_LOAD( "club250.bin", 0x0000, 0x1000, CRC(d94be899) SHA1(b7212162324fa2d67383a475052e3b351bb1af5f) )
ROM_END


/*

  NORAUT V3.010a.

  Board upgraded with daughter card.
  Daughter card looks modern and is marked
  "memory expansion module"
  "Unique Ireland"

  D Card contains:
  Backup Battery

  Program Eprom:27C512
  Marked:
  "G45P A V3.010a GU27
   Euro 27C512 20MAR02"


  PAL:PAL16l8ANC
  Marked VER.2

  CPU:
  Zilog
  Z8400APS
  Z80 CPU

  NVRAM: 6116

  Two jumpers on card , game will not boot if these are removed or placed on other pins
  cabinet beeps and shows grapics on screen. Removing while game is on cause game to freeze.
  Unknown what their for.

  Charcter Eprom is mounted on main board
  CHAR Eprom:2716
  Marked "GU27"

  No date info found in rom. Program eprom sticker: "Euro 27C512 20MAR02"

  This version contains a hidden menu with lots of differnt options
  to access this menu you must hold the HI and LOW button and press the readout/test switch
  the screen will go blank then you release the 3 buttons and the menu appears.

  Pressing the readout button brings you to a menu with RESET / READOUT
  Pressing on Readout brings you to "coins in", "coins out" and "balance".

  The daughter card connects direct to main pcb through 40 pins into original cpu socket
  and 12 pins to one side of original program eprom.

*/

ROM_START( noraut3a )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "g45pa.bin", 0x0000, 0x10000, CRC(f966f4d2) SHA1(99c21ceb59664f32fd1269351fa976370d486f2e) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(              0x0000, 0x0800, 0xff )
	ROM_LOAD( "gu27.bin",  0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )
ROM_END

/*

  V3011a 
  cpu :Z84C0006PEC

  Prog eprom marked
  G45P A V3.011A GU27
  EURO 27C512 10NOV03

  char marked "GU27" same asv3010a
  pal marked "VER 2" same as v3010a

  Everything else the same as v3010a

*/

ROM_START( noraut3b )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "3.011a.bin", 0x0000, 0x10000, CRC(82407395) SHA1(b36ef466d7fbc236f24f70f80bd9cd24ed75f51c) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(              0x0000, 0x0800, 0xff )
	ROM_LOAD( "gu27.bin",  0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )
ROM_END

/*

  Noraut 2 bet joker hard console.
  From PCB with daghterboard.

  ----

  Requires daughterboard console in z80 to run.
  Very old program.

*/

ROM_START( norautua )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "noraut_2_bet_joker_hard_console.bin", 0x0000, 0x2000, CRC(dd6b03a2) SHA1(221425534a255e5b8ef83fb567d578c127ba9a90) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(                     0x0000, 0x0800, 0xff )
	ROM_LOAD( "graphics2716.bin", 0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )
ROM_END

/*

  Noraut 4 bet joker console.
  From PCB with daghterboard.

  ----

  Requires daughterboard console in z80 to run.

*/

ROM_START( norautub )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "noraut_4_bet_joker_console.bin", 0x0000, 0x2000, CRC(abf7725c) SHA1(997c7dbb4c4e5f1cdab28fefd17ab3c88bbc2531) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(                     0x0000, 0x0800, 0xff )
	ROM_LOAD( "graphics2716.bin", 0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )
ROM_END

/*

  Mainline 4 bet joker z80 28pin 60%-75% payout.
  25% Payout Hard 27c 64K uses normal pal.

*/

ROM_START( mainline )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "25_payout_hard.bin", 0x0000, 0x2000, CRC(0d8a34f0) SHA1(b90c3b1949b7c2108d2202c8a5f1d54de4a572ce) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(                     0x0000, 0x0800, 0xff )
	ROM_LOAD( "graphics2716.bin", 0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )

	ROM_REGION( 0x0200,  "plds", 0 )
	ROM_LOAD( "pal16l8_25.bin",  0x0000, 0x0104, CRC(7d9ab06c) SHA1(0f1936e24f77e95e5c95b50a421473f13929809e) )
ROM_END

/*

  DellFern 4-bet Joker z80 28pin
  45%-75% payout.

  Board also made by DellFern.
  UK company that gone bust many years ago.

*/

ROM_START( df_djpkr )
	ROM_REGION( 0x10000, "maincpu", 0 )	/* Program ROM is 0000-1fff, copied along the 64K of the ROM */
	ROM_LOAD( "dellfern_4bet_joker_z80_28pin_45-75_payout.bin", 0x0000, 0x10000, CRC(9d150a47) SHA1(da9c0d6632faab685dd061f39b01d8e65793e1e6) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(                     0x0000, 0x0800, 0xff )
	ROM_LOAD( "graphics2716.bin", 0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )
ROM_END

/*

  Deluxe Poker...

  Stickered:
  AM 42
  RON 10

  Running in a standard 24-pin sockets
  Unmodified Noraut board.

  Version 18/04/2005
  New Gamble Limit=??
  Message changed to ROYAL ON 10
  CoinValue1 equ 020H ;DIP SW 2 - OFF
  CoinValue2 equ 050H ;DIP SW 2 - ON

*/

ROM_START( ndxron10 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "am42_ron10.bin",  0x0000, 0x2000, CRC(7afe5bb6) SHA1(cd3ad96c0de6b58be7507526ff605a9b82894f5c) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_LOAD( "club250.bin",   0x0000, 0x1000, CRC(d94be899) SHA1(b7212162324fa2d67383a475052e3b351bb1af5f) )
ROM_END

/*

   CGI - Credit Poker (v.30c, standard)

*/

ROM_START( cgip30cs )
	ROM_REGION( 0x10000, "maincpu", 0 )	/* Program ROM is 0000-3fff, duplicated to fit the ROM size */
	ROM_LOAD( "cgi_standard_no_bonus_30c_z80_28pin.bin", 0x0000, 0x8000, CRC(7c784964) SHA1(c3deeacc73493939a11dd4cdf0fe07fcd2a9ad8a) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(                     0x0000, 0x0800, 0xff )
	ROM_LOAD( "graphics2716.bin", 0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )
ROM_END

/*

  PCB silkscreened PMA-32-C.
  Someone had written "poker" on it.

  CPU:   LH0080 (Sharp Z80).
  I/O:   3x PPI 8255.
  Xtal:  18 MHz.
  NVRAM: Yes, battery attached.

  ROMs: 2x 2732 (E4 & E5).
        1x 2716 (J2).

  PROM: tb24s10n (D3) read as 82s129.

*/

ROM_START( pma )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "pma.e5", 0x0000, 0x1000, CRC(e05ab5b9) SHA1(8bd13e8ed723ac256545f19bef4fa3fe507ab9d5) )
	ROM_RELOAD(			0x1000, 0x1000 )
	ROM_LOAD( "pma.e4", 0x2000, 0x1000, CRC(0f8b11fc) SHA1(7292b0ac368c469ff2e1ede1765c08f1ccc1a36c) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(           0x0000, 0x0800, 0xff )
	ROM_LOAD( "pma.j2", 0x0800, 0x0800, CRC(412fc492) SHA1(094ea0ffd0c22274cfe164f07c009ffe022331fd) )

	ROM_REGION( 0x0200,  "proms", 0 )
	ROM_LOAD( "pma.d3",  0x0000, 0x0200, CRC(6e172c11) SHA1(b52439a5075cc68ae2792946a5ce973d9f8e4104) )
ROM_END

/*

  Poker / Black Jack (Model 7521)

  PCB: Etched: KRAMER MFG  PWB - 000 - 40065 REV B

  - 1x Z80 CPU
  - 3x 8255
  - 1x 6116 RAM.
  - 1x Dallas DS1220Y Nonvolatile SRAM.
  - 1x Xtal 18 MHz.
  - 3x 8 DIP switches banks.

*/

ROM_START( bjpoker )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "7521-ver.1.u12", 0x0000, 0x2000, CRC(c7b16be0) SHA1(6c875acd4e8468afa1184863a2c6bd7eb086f6e1) )
	ROM_LOAD( "7521-ver.1.u18", 0x2000, 0x2000, CRC(1eff06cc) SHA1(c2135883e14c156dbfd2f38594a896887acb5d2f) )
	ROM_LOAD( "7521-ver.1.u19", 0x4000, 0x2000, CRC(cadfe6f4) SHA1(5a0881231fd84e270441973c9d08beee2a176cbc) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_LOAD( "cf7b.u31",  0x0000, 0x1000, CRC(fcfc4d25) SHA1(31455903244ec8ef9005748f265f561b7a082a9c) )

	ROM_REGION( 0x0100,  "proms", 0 )
	ROM_LOAD( "077b.u51", 0x0000, 0x0100, NO_DUMP )
ROM_END

/*

  Kimble Poker
  Older than Noraut stuff...

  - z80 based, with daughterboard.
  - 8255 weren't initialized.
  - RAM seems at c000-c7ff

*/

ROM_START( kimblejp )
	ROM_REGION( 0x10000, "maincpu", 0 )	/* Program ROM is 0000-6e40 */
	ROM_LOAD( "jpc525611.bin", 0x0000, 0x8000, CRC(4a3f1aef) SHA1(570ef733989da6e89f0387f1e80b934cec7a7663) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(                     0x0000, 0x0800, 0xff )
	ROM_LOAD( "quizcharll.bin", 0x0800, 0x0800, CRC(4edb4717) SHA1(466556595abfbc11e31f2b0d9ca0213bd649253c) )
ROM_END


/************************************* i8080 sets *************************************/
/* The following ones are 'Draw Poker Hi-Lo' type, running in a i8080a based hardware */
/**************************************************************************************/

/*

  Draw Poker Hi-Lo (1983).
  "NYMF O II" hardware.
  M. Kramer Inc.

*/

ROM_START( dphl )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "dphl_6468.u12", 0x0000, 0x1000, CRC(d8c4fe5c) SHA1(6bc745fefb8a3a21ca281d519895828047526de7) )
	ROM_LOAD( "dphl_36e3.u18", 0x1000, 0x1000, CRC(06cf6789) SHA1(587d883c399348b518e3be4d1dc2581824055328) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_LOAD( "dphl_model_2_cgi_3939.u31",  0x0000, 0x1000, CRC(2028db2c) SHA1(0f81bb71e88c60df3817f58c28715ce2ea01ad4d) )

	ROM_REGION( 0x0100,  "proms", 0 )
	ROM_LOAD( "98ce.u51",  0x0000, 0x0100, CRC(812dc1f1) SHA1(b2af33ff36f2eca2f782bc2239bc9e54c2564f6a) )
ROM_END

/*

  Draw Poker Hi-Lo (alt).

  PCB silksceened: REV A.
  PCB MARKED: Solderside "81 16".
  Component side "J3 018".

  U11 2716 EPROM MARKED:"2B27".
  U16 2716 EPROM MARKED:"4D30".

  Layout similar to noraut PCB's.

*/

ROM_START( dphla )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "2b27.u11", 0x0000, 0x0800, CRC(3a7ece95) SHA1(bc7c89e3f490da0723b3a7617ab9a747f8db7ea7) )
	ROM_LOAD( "4d30.u16", 0x0800, 0x0800, CRC(32594684) SHA1(cda1ed09ec30082d23e690058261523e0d34938e) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(              0x0000, 0x0800, 0xff )
	ROM_LOAD( "char.u27",  0x0800, 0x0800, CRC(174a5eec) SHA1(44d84a0cf29a0bf99674d95084c905d3bb0445ad) )
ROM_END

/*

  Etched on top of board in copper: "MADE IN JAPAN".
  Stickered on top: "Serial No. 10147".

  Orange dot sticker dot near pin 1.
  White dot sticker at other end of connector.

  .u18    MB8516  read as 2716    stickered   13.
  .u19    MB8516  read as 2716    stickered   11.
  .u12    MB8516  read as 2716    stickered   12.
  .u31    MB8516  read as 2716    stickered   10.
  .u51    6301    read as 82s129

  1x 18.000 Crystal
  1x 8080
  3x 8255
  2x 5101
  1x 8228
  2x 2114
  1x 8 DIP Switches bank.

  Mini daughterboard attached.

*/

ROM_START( dphljp )	/* close to GTI Poker */
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "japan_12.u12", 0x0000, 0x0800, CRC(086a2303) SHA1(900c7241c33a38fb1a791b311e50f7d7f43bb955) )
	ROM_RELOAD(	              0x0800, 0x0800 )
	ROM_LOAD( "japan_13.u18", 0x1000, 0x0800, CRC(ccaad5cb) SHA1(5f6ca497ccb7c535714a6e24df00f2831a7840c1) )
	ROM_RELOAD(	              0x1800, 0x0800 )
	ROM_LOAD( "japan_11.u19", 0x2000, 0x0800, CRC(9f9c67d5) SHA1(cd11849b245406821af7ac3554805c9dd89645b2) )	// ???

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(                 0x0000, 0x0800, 0xff )
	ROM_LOAD( "japan_10.u31", 0x0800, 0x0800, CRC(412fc492) SHA1(094ea0ffd0c22274cfe164f07c009ffe022331fd) )

	ROM_REGION( 0x0100,  "proms", 0 )
	ROM_LOAD( "japan_6301.u51", 0x0000, 0x0100, CRC(88302127) SHA1(aed1273974917673405f1234ab64e6f8b3856c34) )
ROM_END

/*

  Has (c)1983 GTI in the roms, and was called 'Poker.zip'
  GFX roms contain 16x16 tiles of cards.
  Nothing else is known about this set/game.

*/

ROM_START( gtipoker )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "u12.rom", 0x0000, 0x1000, CRC(abaa257a) SHA1(f830213ae0aaad5a9a44ec77c5a186e9e02fa041) )
	ROM_LOAD( "u18.rom", 0x1000, 0x1000, CRC(1b7e2877) SHA1(717fb70889804baa468203f20b1e7f73b55cc21e) )

	ROM_REGION( 0x1000, "gfx",0 )
	ROM_LOAD( "u31.rom", 0x0000, 0x1000, CRC(2028db2c) SHA1(0f81bb71e88c60df3817f58c28715ce2ea01ad4d) )
ROM_END

/*

  Hi-Lo Double Up Joker Poker
  SMS Manufacturing Corp., 1983.

  almost identical to DPHL.
  Only one different program rom.
  Seems to be patched with 2 extra subroutines.

*/

ROM_START( smshilo )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "u12.bin", 0x0000, 0x1000, CRC(bd9acce8) SHA1(33e7e1805c03a704f9c8785b8e858310bfdc8b10) )
	ROM_LOAD( "u18.bin", 0x1000, 0x1000, CRC(06cf6789) SHA1(587d883c399348b518e3be4d1dc2581824055328) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_FILL(            0x0000, 0x0800, 0xff )
	ROM_LOAD( "u31.bin", 0x0800, 0x0800, CRC(412fc492) SHA1(094ea0ffd0c22274cfe164f07c009ffe022331fd) )

	ROM_REGION( 0x0100,  "proms", 0 )
	ROM_LOAD( "u51.bin", 0x0000, 0x0100, CRC(812dc1f1) SHA1(b2af33ff36f2eca2f782bc2239bc9e54c2564f6a) )
ROM_END

/*

  DRHL
  poker looking board

  .u26  2732  stickered DRHL V2.89 U-26
  .u19  2732  stickered DRHL V2.89 U-19
  .u12  2732  stickered DRHL V2.89 U-12
  .u18  2732  stickered DRHL V2.89 U-18
  .u31  2732  stickered DRHL V1.0  U 31
  .u51  ampal16l8pc  printed 147-pal

  ROM text showed poker text and
  COPYRIGHT 1986 DREWS INC.
  COPYRIGHT 1986 DREW'S DISTRIBUTING INC.
 
*/

ROM_START( drhl )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "drhl_v2.89.u12", 0x0000, 0x1000, CRC(a0f63638) SHA1(e8046b4042ca1e203d831de70da2bf940b2094a0) )
	ROM_LOAD( "drhl_v2.89.u18", 0x1000, 0x1000, CRC(f3590633) SHA1(bb4a186d13b24ffd22e291c8d6c67f5012aa8001) )
	ROM_LOAD( "drhl_v2.89.u19", 0x2000, 0x1000, CRC(8abd7f40) SHA1(e8e489c670a17d4c48491b94bad1976cbb4742eb) )
	ROM_LOAD( "drhl_v2.89.u26", 0x3000, 0x1000, CRC(44ae3cdc) SHA1(51090c737873b652ba95435d553fb88fac730892) )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_LOAD( "drhl_v1.0.u31",  0x0000, 0x1000, CRC(bbc7c970) SHA1(9268a430764a5ea8ba7cd18944ec254a44d9dff2) )

	ROM_REGION( 0x0200,  "plds", 0 )	/* possible bad dump. fusemap's 1st half is all 1's and 2nd half 0's */
	ROM_LOAD( "drhl_ampal16l8pc.u51",  0x0000, 0x0104, CRC(bd76fb53) SHA1(2d0634e8edb3289a103719466465e9777606086e) )
ROM_END

/*

  Turbo Poker 2 by Micro MFG.

  - CPU:             1x NEC D8080AFC-1 (U42).
  - BUS:             1x 8224 (U43)
  - RAM:             2x 2111-1 Static Random Access Memory 256 x 4 bit (U33 & U34).
  - I/O:             3x Intel P8255A Peripeheral Interface Adapter (U31, U36 & U38).
  - Prg ROMs:        1x 27256 (U39).
  - Gfx ROMs:        1x 2732 (U30).
  - Sound:           Discrete.
  - Crystal:         1x 18.000 MHz.

  - 1x Custom MCU based on 68705.

*/

ROM_START( tpoker2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "tp2.u39", 0x0000, 0x8000, CRC(543149fe) SHA1(beb61a27c2797341e23e020e754d63fde3b4fbb2) )

	ROM_REGION( 0x0800,  "mcu", 0 )	/* 68705 */
	ROM_LOAD( "tp2.u35", 0x0000, 0x0800, NO_DUMP )

	ROM_REGION( 0x1000,  "gfx", 0 )
	ROM_LOAD( "tp2.u30", 0x0000, 0x1000, CRC(6df86e08) SHA1(a451f71db7b59500b99207234ef95793afc11f03) )

	ROM_REGION( 0x0800,  "other", 0 )
	ROM_LOAD( "tp2.u44", 0x0000, 0x0800, CRC(6b5453b7) SHA1(6793952800de067fd76b889f4f7c62c8474b8c3a) )

	ROM_REGION( 0x0400,  "proms", 0 )
	ROM_LOAD( "tp2.u23", 0x0000, 0x0400, CRC(0222124f) SHA1(5cd8d24ee8e6525a5f9e6a93fa8854f36f4319ee) )

	ROM_REGION( 0x0034,  "plds", 0 )
	ROM_LOAD( "tp2_pld.u37",  0x0000, 0x0034, CRC(25651948) SHA1(62cd4d73c6ca8ea5d4beb9ae262d1383f8149462) )
ROM_END


/**************************
*       Driver Init       *
**************************/

/* These are to patch the check for /OBF handshake line,
   that seems to be wrong. Otherwise will enter in an infinite loop.

  110D: DB C2      in   a,($C2)  ; read from PPI-2, portC. (OBF should be set, but isn't)
  110F: 07         rlca          ; rotate left.
  1110: 30 FB      jr   nc,$110D

  This routine is to shift the handshaked lines, transferring the status
  from /ACK (bit 6) to /OBF (bit 7).

*/
//static DRIVER_INIT( norautrh )
//{
//	UINT8 *ROM = memory_region(machine, "maincpu");
//	ROM[0x1110] = 0x00;
//	ROM[0x1111] = 0x00;
//}

//static DRIVER_INIT( norautpn )
//{
//	UINT8 *ROM = memory_region(machine, "maincpu");
//	ROM[0x0827] = 0x00;
//	ROM[0x0828] = 0x00;
//}

//static DRIVER_INIT( norautu )
//{
//	UINT8 *ROM = memory_region(machine, "maincpu");
//	ROM[0x083c] = 0x00;
//	ROM[0x083d] = 0x00;
//	ROM[0x083e] = 0x00;
//}

//static DRIVER_INIT( gtipoker )
//{
//	UINT8 *ROM = memory_region(machine, "maincpu");
//	ROM[0x0cc6] = 0x00;
//	ROM[0x0cc7] = 0x00;
//	ROM[0x0cc8] = 0x00;
//	ROM[0x10a5] = 0x00;
//	ROM[0x10a6] = 0x00;
//	ROM[0x10a7] = 0x00;
//}

//static DRIVER_INIT( dphl )
//{
//	UINT8 *ROM = memory_region(machine, "maincpu");
//	ROM[0x1510] = 0x00;
//	ROM[0x1511] = 0x00;
//	ROM[0x1512] = 0x00;
//}

//static DRIVER_INIT( dphla )
//{
//	UINT8 *ROM = memory_region(machine, "maincpu");
//	ROM[0x0b09] = 0x00;
//	ROM[0x0b0a] = 0x00;
//	ROM[0x0b0b] = 0x00;
//}

static DRIVER_INIT( enc )
{
/* Attempt to decrypt the program ROM */

//	UINT8 *rom = memory_region(machine, "maincpu");
//	UINT8 *buffer;
//	int size = 0x2000; //memory_region_length(machine, "maincpu");
//	int start = 0;
//	int i;


//	for (i = start; i < size; i++)
//	{
//		rom[i] = rom[i] ^ 0x09 ^ 0xff;
//		rom[i+1] = rom[i+1] ^ 0xfb ^ 0xff;
//		rom[i+2] = rom[i+2] ^ 0xb2 ^ 0xff;
//		rom[i+3] = rom[i+3] ^ 0x60 ^ 0xff;
//		rom[i+4] = rom[i+4] ^ 0xce ^ 0xff;
//		rom[i+5] = rom[i+5] ^ 0x44 ^ 0xff;
//		rom[i+6] = rom[i+6] ^ 0x6e ^ 0xff;
//		rom[i+7] = rom[i+7] ^ 0x61 ^ 0xff;
//		rom[i+8] = rom[i+8] ^ 0x37 ^ 0xff;
//		rom[i+9] = rom[i+9] ^ 0x5e ^ 0xff;
//		rom[i+10] = rom[i+10] ^ 0xfb ^ 0xff;
//		rom[i+11] = rom[i+11] ^ 0xc6 ^ 0xff;
//		rom[i+12] = rom[i+12] ^ 0x1d ^ 0xff;
//		rom[i+13] = rom[i+13] ^ 0x33 ^ 0xff;
//		rom[i+14] = rom[i+14] ^ 0x00 ^ 0xff;
//		rom[i+15] = rom[i+15] ^ 0xff ^ 0xff;

//		i = i + 16;
//	}

//	buffer = alloc_array_or_die(UINT8, size);
//	memcpy(buffer, rom, size);

//	free(buffer);
}

static DRIVER_INIT( deb )
/* Just for debugging purposes */
/*   Should be removed soon    */
{
	UINT8 *ROM = memory_region(machine, "maincpu");
	ROM[0x02f7] = 0xca;
	ROM[0x02f8] = 0x18;
	ROM[0x206c] = 0xff;
}


/*************************
*      Game Drivers      *
*************************/

/************************************** Z80 sets **************************************/
/*  The following ones are 'Draw Poker Hi-Lo' type, running in a Z80 based hardware   */
/**************************************************************************************/

/*     YEAR  NAME      PARENT   MACHINE   INPUT     INIT ROT    COMPANY                      FULLNAME                              FLAGS             LAYOUT */

GAMEL( 1988, norautp,  0,       norautp,  norautp,  0,   ROT0, "Noraut Ltd.",               "Noraut Poker",                        0,                layout_noraut11 )
GAMEL( 198?, norautdx, 0,       norautp,  norautpn, 0,   ROT0, "Noraut Ltd.",               "Noraut Deluxe Poker (console)",       0,                layout_noraut12 )
GAMEL( 198?, norautpn, norautp, norautp,  norautpn, 0,   ROT0, "bootleg",                   "Noraut Deluxe Poker (bootleg)",       0,                layout_noraut12 )
GAMEL( 198?, norautjo, 0,       norautp,  mainline, 0,   ROT0, "Noraut Ltd.",               "Noraut Joker Poker (original)",       0,                layout_noraut12 )
GAMEL( 1988, norautjp, norautp, norautp,  norautp,  0,   ROT0, "Noraut Ltd.",               "Noraut Joker Poker (alt)",            0,                layout_noraut11 )
GAMEL( 1988, norautrh, 0,       norautp,  norautrh, 0,   ROT0, "Noraut Ltd.",               "Noraut Red Hot Joker Poker",          0,                layout_noraut12 )
GAMEL( 198?, norautra, 0,       norautp,  norautrh, 0,   ROT0, "Noraut Ltd.",               "Noraut Red Hot Joker Poker (alt HW)", 0,                layout_noraut12 ) // 1-bet?? where??...
GAME(  1988, norautu,  0,       norautxp, norautp,  0,   ROT0, "Noraut Ltd.",               "Noraut Poker (NTX10A)",               GAME_NOT_WORKING )
GAME(  2002, noraut3a, 0,       norautxp, norautp,  0,   ROT0, "Noraut Ltd.",               "Noraut Joker Poker (V3.010a)",        GAME_NOT_WORKING )
GAME(  2003, noraut3b, 0,       norautxp, norautp,  0,   ROT0, "Noraut Ltd.",               "Noraut Joker Poker (V3.011a)",        GAME_NOT_WORKING )
GAMEL( 198?, norautua, 0,       norautp,  norautp,  enc, ROT0, "Noraut Ltd.",               "Noraut unknown set 1 (console)",      GAME_NOT_WORKING, layout_noraut12 )
GAMEL( 198?, norautub, 0,       norautp,  norautp,  enc, ROT0, "Noraut Ltd.",               "Noraut unknown set 2 (console)",      GAME_NOT_WORKING, layout_noraut12 )
GAMEL( 198?, mainline, 0,       norautp,  mainline, 0,   ROT0, "Mainline London",           "Mainline Double Joker Poker",         0,                layout_noraut12 )
GAMEL( 199?, df_djpkr, 0,       norautp,  mainline, 0,   ROT0, "DellFern Ltd.",             "Double Joker Poker (45%-75% payout)", 0,                layout_noraut12 )
GAMEL( 2005, ndxron10, 0,       norautp,  ndxron10, 0,   ROT0, "<unknown>",                 "Royal on Ten (Noraut Deluxe hack)",   0,                layout_noraut12 )
GAMEL( 1999, cgip30cs, 0,       norautx4, norautkl, deb, ROT0, "CGI",                       "Credit Poker (ver.30c, standard)",    0,                layout_noraut12 )
GAME(  1983, pma,      0,       nortest1, norautp,  0,   ROT0, "PMA",                       "PMA Poker",                           GAME_NOT_WORKING )
GAMEL( 198?, bjpoker,  0,       norautxp, norautrh, 0,   ROT0, "M. Kramer Manufacturing.",  "Poker / Black Jack (Model 7521)",     GAME_NOT_WORKING, layout_noraut12 )
GAMEL( 198?, kimblejp, 0,       norautx8, norautkl, 0,   ROT0, "Kimble Gaming",             "Kimble Joker Poker",                  GAME_NOT_WORKING, layout_noraut12 )


/************************************* i8080 sets *************************************/
/* The following ones are 'Draw Poker Hi-Lo' type, running in a i8080a based hardware */
/**************************************************************************************/

/*     YEAR  NAME      PARENT   MACHINE   INPUT     INIT ROT    COMPANY                      FULLNAME                              FLAGS             LAYOUT */

GAME(  1983, dphl,     0,       dphl,     norautp,  0,   ROT0, "M. Kramer Manufacturing.",  "Draw Poker Hi-Lo (M.Kramer)",         GAME_NOT_WORKING )
GAME(  1983, dphla,    0,       dphla,    norautp,  0,   ROT0, "<unknown>",                 "Draw Poker Hi-Lo (Alt)",              GAME_NOT_WORKING )
GAME(  1983, dphljp,   0,       dphl,     norautp,  0,   ROT0, "<unknown>",                 "Draw Poker Hi-Lo (Japanese)",         GAME_NOT_WORKING )
GAME(  1983, gtipoker, 0,       dphl,     norautp,  0,   ROT0, "GTI Inc",                   "GTI Poker",                           GAME_NOT_WORKING )
GAME(  1983, smshilo,  0,       dphla,    norautp,  0,   ROT0, "SMS Manufacturing Corp.",   "Hi-Lo Double Up Joker Poker ",        GAME_NOT_WORKING )
GAME(  1986, drhl,     0,       drhl,     norautp,  0,   ROT0, "Drews Inc.",                "DRHL Poker (v.2.89)",                 GAME_NOT_WORKING )

/* The following one also has a custom 68705 MCU */
GAME(  1993, tpoker2,  0,       dphltest, norautp,  0,   ROT0, "Micro Manufacturing, Inc.", "Turbo Poker 2",                       GAME_NOT_WORKING )
