/***************************************************************************

TODO:
- It seems shadows can both affect underlying sprites and not. This is currently
  hardcoded in the drivers; there might be a control bit somewhere.
  Games requiring shadows to affect sprites behind them:
  - Surprise Attack (dark glass walls in level 3)
  - 88 Games (angle indicator in the long jump event)
  - Sunset Riders (bull's eye in the saloon cutscene)
  - TMNT 2 (lightbeam in level 4 cave)
  - Metamorphic Force (double! lightbeam just before the last boss)
  Games requiring shadows to NOT affect sprites behind them:
  - Asterix (Asterix's shadow would be over his feet otherwise)
  - X-Men is dubious, see enemies halfway through level 1 coming from above with
    boulders over their heads.

- scrollcontrol = 30 in Golfing Greats (leader board)

- detatwin: sprites are left on screen during attract mode


                      Emulated
                         |
                  board #|year    CPU      tiles        sprites  priority palette    other
                    -----|---- ------- ------------- ------------- ------ ------ ----------------
Hyper Crash         GX401 1985                   GX400
Twinbee             GX412*1985   68000           GX400
Yie Ar Kung Fu      GX407*1985    6809
Gradius / Nemesis   GX456*1985   68000           GX400
Shao-lins Road      GX477*1985    6809
Jail Break          GX507*1986 KONAMI-1          005849                   PROMs
Finalizer           GX523*1985 KONAMI-1          005885                   PROMs
Konami's Ping Pong  GX555*1985     Z80
Iron Horse          GX560*1986    6809           005885                   PROMs
Konami GT           GX561*1985   68000           GX400
Green Beret         GX577*1985     Z80           005849                   PROMs
Galactic Warriors   GX578*1985   68000           GX400
Salamander          GX587*1986   68000           GX400
WEC Le Mans 24      GX602*1986 2x68000
BAW / Black Panther GX604*1987   68000           GX400                    007593
Combat School /     GX611*1987    6309           007121(x2)               007327
  Boot Camp
Rock 'n Rage /      GX620*1986    6309 007342        007420               007327
  Koi no Hotrock
Mr Kabuki/Mr Goemon GX621*1986     Z80           005849
Jackal              GX631*1986    6809           005885(x2)               007327 007343 (address decoder)
Contra / Gryzor     GX633*1987    6309           007121(x2)               007593
Flak Attack         GX669*1987    6309           007121                   007327 007452
Devil World / Dark  GX687*1987 2x68000           TWIN16
  Adventure / Majuu no Oukoku
Double Dribble      GX690*1986  3x6809           005885(x2)               007327 007452
Kitten Kaboodle /   GX712*1988                   GX400                    007593 051550
  Nyan Nyan Panic
Chequered Flag      GX717*1988  052001               051960 051937(x2)           051316(x2) (roz) 051733 (protection)
Fast Lane           GX752*1987    6309           007121                          051733 (protection) 007801
Hot Chase           GX763*1988 2x68000                                           051316(x3) (roz) 007634 007635 007558 007557
Rack 'Em Up /       GX765*1987    6309 007342        007420               007327 007324
  The Hustler
Haunted Castle      GX768*1988  052001           007121(x2)               007327
Ajax / Typhoon      GX770*1987   6309+ 052109 051962 051960 051937  PROM  007327 051316 (roz)
                                052001
Labyrinth Runner /  GX771*1987    6309           007121                   007593 051733 (protection) 051550
  Trick Trap
Super Contra        GX775*1988  052001 052109 051962 051960 051937  PROM  007327
Battlantis          GX777*1987    6309 007342        007420               007327 007324
Vulcan Venture /    GX785*1988 2x68000           TWIN16
  Gradius 2
City Bomber         GX787*1987   68000           GX400                    007593 051550
Over Drive          GX789*1990 2x68000               053247 053246 053251 051316(x2) (roz) 053249 053250(x2) (road) 053252(*)
Hyper Crash         GX790 1987
Blades of Steel     GX797*1987    6309 007342        007420               007327 051733 (protection)
The Main Event      GX799*1988    6309 052109 051962 051960 051937  PROM
Missing in Action   GX808*1989   68000 052109 051962 051960 051937  PROM
Missing in Action J GX808*1989 2x68000           TWIN16
Crime Fighters      GX821*1989  052526 052109 051962 051960 051937  PROM
Special Project Y   GX857*1989    6309 052109 051962 051960 051937  PROM         052591 (protection)
'88 Games           GX861*1988  052001 052109 051962 051960 051937  PROM         051316 (roz)
Final Round /       GX870*1988 1x68000           TWIN16?
  Hard Puncher
Thunder Cross       GX873*1988  052001 052109 051962 051960 051937  PROM  007327 052591 (protection)
Aliens              GX875*1990  052526 052109 051962 051960 051937  PROM
Gang Busters        GX878*1988  052526 052109 051962 051960 051937  PROM
Devastators         GX890*1988    6309 052109 051962 051960 051937  PROM         007324 051733 (protection)
Bottom of the Ninth GX891*1989    6809 052109 051962 051960 051937  PROM         051316 (roz)
Cue Brick           GX903*1989   68000 052109 051962 051960 051937  PROM
Cue Brick           GX903*1989 2x68000           TWIN16
Punk Shot           GX907*1990   68000 052109 051962 051960 051937 053251
Ultraman            GX910*1991   68000 ------ ------ 051960 051937  PROM         051316(x3) (roz) 051550
Surprise Attack     GX911*1990  053248 052109 051962 053245 053244 053251
Lightning Fighters /GX939*1990   68000 052109 051962 053245 053244 053251
  Trigon
Gradius 3           GX945*1989 2x68000 052109 051962 051960 051937  PROM
Parodius            GX955*1990  053248 052109 051962 053245 053244 053251
TMNT                GX963*1989   68000 052109 051962 051960 051937  PROM
Block Hole          GX973*1989  052526 052109 051962 051960 051937  PROM
Escape Kids         GX975*1991  053248 052109 051962 053247 053246 053251        053252(*)
Rollergames         GX999*1991  053248 ------ ------ 053245 053244               051316 (roz) 053252(*)
Bells & Whistles /  GX060*1991   68000 052109 051962 053245 053244 053251        054000 (collision)
  Detana!! Twin Bee
Golfing Greats      GX061*1991   68000 052109 051962 053245 053244 053251        053936 (roz+)
TMNT 2              GX063*1991   68000 052109 051962 053245 053244 053251        053990 (protection) 051550
Sunset Riders       GX064*1991   68000 052109 051962 053245 053244 053251        054358
X-Men               GX065*1992   68000 052109 051962 053247 053246 053251        054539 (sound)
XEXEX               GX067*1991   68000 054157 054156 053247 053246 053251        053250?("road") 054338 (alpha blending) 054539 (sound)
Asterix             GX068*1992   68000 054157 054156 053245 053244 053251        054358
G.I. Joe            GX069*1992   68000 054157 054156 053247 053246 053251        054539 (sound)
The Simpsons        GX072*1991  053248 052109 051962 053247 053246 053251
Thunder Cross 2     GX073*1991   68000 052109 051962 051960 051937 053251        054000 (collision)
Vendetta /          GX081*1991  053248 052109 051962 053247 053246 053251        054000 (collision)
  Crime Fighters 2
Premier Soccer      GX101*1993   68000 052109 051962 053245 053244 053251        053936 (roz+) 054986
Hexion              GX122*1992     Z80                                           052591 (protection) 053252(*)
Entapous /          GX123*1993   68000 054157 054156 055673 053246 055555        053252(*) 054000 053936 (roz+)
  Gaiapolis
Mystic Warrior      GX128*1993   68000 054157 054156 055673 053246 055555        054338 (alpha blending) 053252(*) 054539(x2) (sound)
Cowboys of Moo Mesa GX151*1992   68000 054157 054156 053247 053246 053251        053252(*) 054338 (alpha blending) 053990 (protection)
Violent Storm       GX168*1993   68000 054157 054156 055673 053246 055555        054338 (alpha blending) 055550 054539(x2) (sound)
Monster Maulers /   GX170*1993   68000 054157 054156 055673 053246 055555        053252(*) 055550 054338 (alpha blending) 054539 (sound) 053936 (roz+)
  Ultimate Battler Dadandarn
Bucky 'O Hare       GX173*1992   68000 054157 054156 053247 053246 053251        054338 (alpha blending) 054539 (sound)
Potrio              GX174 1992
Lethal Enforcers    GX191*1992    6309 054157(x2) 054156 053245 053244(x2)       054000 054539 (sound)
Metamorphic Force   GX224*1993   68000 054157 054157 055673 053246 055555
Martial Champion    GX234*1993   68000 054157 054156 055673 053246 055555        053252(*) 054338 (alpha blending) 053990 054539 (sound)
Run and Gun         GX247*1993   68000 (TTL tilemap) 055673 053246               053253(x2) 053252(*) 053936 (roz+) 054539(x2) (sound)
Quiz Gakumon no     GX248*1993   68000 052109 051962 053245 053244 053251        053990 (protection) 051550 - same board as TMNT2
  Susume
Polygonet Commander GX305+1993   68020 (TTL tilemap)                             XC56156-40(3D DSP) 054009(x2) 054010(x2) 054539 (sound)
System GX (ver 1)   GX300*1993   68020 056832 054156 055673 053246 055555        054338 (alpha blending) 054539(x2) (sound) 053252(*) 053936 (optional on ROM board, roz+)
System GX (ver 2)   GX300*1995   68020 056832 058143 055673 058142 055555        058144 (alpha blending) 058141 (sound) 053252(*) 053936 (optional on ROM board, roz+)
Beatmania DJ Main   GX858+1996   68020 056832 058143 056766        055555        058144 (alpha blending) 058141 (sound) 053252(*)
Tail to Nose             *1989   68000          V-System                         051316 (roz)
F-1 Grand Prix           *1991 2x68000          V-System                         053936 (roz+)
F-1 Grand Prix Part II   *1992 2x68000          V-System                         053936 (roz+)
Lethal Crash Race        *1993   68000          V-System                         053936 (roz+)
Super Slams              *1995   68000          V-System                         053936 (roz+)
Blazing Tornado          *1991   68000            Metro                          053936 (roz+)
Dragonball Z 2           *1994   68000 054157 054156 053247 053246 053251(x2)    053936(x2) (roz+) 053252(*)


Notes:
* 053252 seems to be just a timing/interrupt controller (see Vendetta schematics).

- Old games use 051961 instead of 052109, it is an earlier version functionally
  equivalent (maybe 052109 had bugs fixed). The list always shows 052109 because
  the two are exchangeable and 052109's are found also on original boards whose
  schematics show a 051961.

- Starting with the version 2 System GX mainboard, the following chip substitutions took place.
  All "new" chips are equivalent to their older counterparts, but are in a smaller package (and
  presumably are made on a smaller process).  The exception is the 058141, which is equivalent
  to 2 54539s (and yet takes less board space than even 1).

  058141 = 054539 (x2) (2 sound chips in one)
  058142 = 053246 (sprites)
  058143 = 054156 (tiles)
  058144 = 054338 (alpha blending)



Status of the ROM tests in the emulated games:

Chequered Flag      pass
Ajax / Typhoon      pass
Super Contra        pass
Over Drive          pass
The Main Event      pass
Missing in Action   pass
Crime Fighters      pass
Special Project Y   pass
Konami 88           pass
Thunder Cross       pass
Aliens              pass
Gang Busters        pass
Devastators         pass
Bottom of the Ninth pass
Punk Shot           pass
Surprise Attack     fails D05-6 (052109) because it uses mirror addresses to
                    select banks, and supporting those addresses breaks the
                    normal game ;-(
Lightning Fighters  pass
Gradius 3           pass
Parodius            pass
TMNT                pass
Block Hole          pass
Escape Kids         pass
Rollergames         pass
Bells & Whistles    pass
Golfing Greats      pass
TMNT 2              pass
Sunset Riders       pass
X-Men               pass
The Simpsons        pass
Thunder Cross 2     pass
Xexex               pass
Asterix             pass
GiJoe               pass
Vendetta            pass
Premier Soccer      fails 16D 18D 18F (053936)
Hexion              pass
Run and Gun         fails 36M (053936) 2U 2Y 5U 5Y (sprites)
Quiz Gakumon no Susume  pass
Dragonball Z 2      fails


THE FOLLOWING INFORMATION IS PRELIMINARY AND INACCURATE. DON'T RELY ON IT.


005885
------
Some games use two of these in pair. Jackal even puts together the two 4bpp
tilemaps to form a single 8bpp one.
It manages sprites and 32x32 or 64x32 tilemap (only Double Dribble uses the
64x32 one).
The chip also generates clock and interrupt signals suitable for a 6809.
It uses 0x2000 bytes of RAM for the tilemaps and sprites, and an additional
0x100 bytes, maybe for scroll RAM and line buffers. The maximum addressable
ROM is 0x20000 bytes (addressed 16 bits at a time). Tile and sprite data both
come from the same ROM space. Double Dribble and Jackal have external circuitry
to extend the limits and use separated addressing spaces for sprites and tiles.
All games use external circuitry to reuse one or both the tile flip attributes
as an additional address bit.
Two 256x4 lookup PROMs are also used to increase the color combinations.
All tilemap / sprite priority handling is done internally and the chip exports
5 bits of color code, composed of 1 bit indicating tile or sprite, and 4 bits
of ROM data remapped through the PROM.

inputs:
- address lines (A0-A13)
- data lines (DB0-DB7)
- misc interface stuff
- data from the gfx ROMs (RDL0-RDL7, RDU0-RDU7)
- data from the tile lookup PROMs (VCD0-VCD3)
- data from the sprite lookup PROMs (OCD0-OCD3)

outputs:
- address lines for tilemap RAM (AX0-AX12)
- data lines for tilemap RAM (VO0-VO7)
- address lines for the small RAM (FA0-FA7)
- data lines for the small RAM (FD0-FD7)
- address lines for the gfx ROMs (R0-R15)
- address lines for the tile lookup PROMs (VCF0-VCF3, VCB0-VCB3)
- address lines for the sprite lookup PROMs (OCB0-OCB3, OCF0-OCF3)
- NNMI, NIRQ, NFIR, NCPE, NCPQ, NEQ for the main CPU
- misc interface stuff
- color code to be output on screen (COL0-COL4)


control registers
000:          scroll y
001:          scroll x (low 8 bits)
002: -------x scroll x (high bit)
     ----xxx- row/colscroll control
              000 = solid scroll (finalizr, ddribble bg)
              100 = solid scroll (jackal)
              001 = ? (ddribble fg)
              011 = colscroll (jackal high scores)
              101 = rowscroll (ironhors, jackal map)
003: ------xx high bits of the tile code
     -----x-- unknown (finalizr)
     ----x--- selects sprite buffer (and makes a copy to a private buffer?)
     --x----- unknown (ironhors)
     -x------ unknown (ironhors)
     x------- unknown (ironhors, jackal)
004: -------x nmi enable
     ------x- irq enable
     -----x-- firq enable
     ----x--- flip screen



007121
------
This is an interesting beast. It is an evolution of the 005885, with more
features. Many games use two of these in pair.
It manages sprites and two 32x32 tilemaps. The tilemaps can be joined to form
a single 64x32 one, or one of them can be moved to the side of screen, giving
a high score display suitable for vertical games.
The chip also generates clock and interrupt signals suitable for a 6809.
It uses 0x2000 bytes of RAM for the tilemaps and sprites, and an additional
0x100 bytes, maybe for scroll RAM and line buffers. The maximum addressable
ROM is 0x80000 bytes (addressed 16 bits at a time). Tile and sprite data both
come from the same ROM space.
Two 256x4 lookup PROMs are also used to increase the color combinations.
All tilemap / sprite priority handling is done internally and the chip exports
7 bits of color code, composed of 2 bits of palette bank, 1 bit indicating tile
or sprite, and 4 bits of ROM data remapped through the PROM.

inputs:
- address lines (A0-A13)
- data lines (DB0-DB7)
- misc interface stuff
- data from the gfx ROMs (RDL0-RDL7, RDU0-RDU7)
- data from the tile lookup PROMs (VCD0-VCD3)
- data from the sprite lookup PROMs (OCD0-OCD3)

outputs:
- address lines for tilemap RAM (AX0-AX12)
- data lines for tilemap RAM (VO0-VO7)
- address lines for the small RAM (FA0-FA7)
- data lines for the small RAM (FD0-FD7)
- address lines for the gfx ROMs (R0-R17)
- address lines for the tile lookup PROMs (VCF0-VCF3, VCB0-VCB3)
- address lines for the sprite lookup PROMs (OCB0-OCB3, OCF0-OCF3)
- NNMI, NIRQ, NFIR, NE, NQ for the main CPU
- misc interface stuff
- color code to be output on screen (COA0-COA6)


control registers
000:          scroll x (low 8 bits)
001: -------x scroll x (high bit)
     ------x- enable rowscroll? (combatsc)
     ----x--- this probably selects an alternate screen layout used in combat
              school where tilemap #2 is overlayed on front and doesn't scroll.
              The 32 lines of the front layer can be individually turned on or
              off using the second 32 bytes of scroll RAM.
002:          scroll y
003: -------x bit 13 of the tile code
     ------x- unknown (contra)
     -----x-- might be sprite / tilemap priority (0 = sprites have priority)
              (combat school, contra, haunted castle(0/1), labyrunr)
     ----x--- selects sprite buffer (and makes a copy to a private buffer?)
     ---x---- screen layout selector:
              when this is set, 5 columns are added on the left of the screen
              (that means 5 rows at the top for vertical games), and the
              rightmost 2 columns are chopped away.
              Tilemap #2 is used to display the 5 additional columns on the
              left. The rest of tilemap #2 is not used and can be used as work
              RAM by the program.
              The visible area becomes 280x224.
              Note that labyrunr changes this at runtime, setting it during
              gameplay and resetting it on the title screen and crosshatch.
     --x----- might be sprite / tilemap priority (0 = sprites have priority)
              (combat school, contra, haunted castle(0/1), labyrunr)
     -x------ Chops away the leftmost and rightmost columns, switching the
              visible area from 256 to 240 pixels. This is used by combatsc on
              the scrolling stages, and by labyrunr on the title screen.
              At first I thought that this enabled an extra bank of 0x40
              sprites, needed by combatsc, but labyrunr proves that this is not
              the case
     x------- unknown (contra)
004: ----xxxx bits 9-12 of the tile code. Only the bits enabled by the following
              mask are actually used, and replace the ones selected by register
              005.
     xxxx---- mask enabling the above bits
005: selects where in the attribute byte to pick bits 9-12 of the tile code,
     output to pins R12-R15. The bit of the attribute byte to use is the
     specified bit (0-3) + 3, that is one of bits 3-6. Bit 7 is hardcoded as
     bit 8 of the code. Bits 0-2 are used for the color, however note that
     some games use bit 3 as well (see below).
     ------xx attribute bit to use for tile code bit  9
     ----xx-- attribute bit to use for tile code bit 10
     --xx---- attribute bit to use for tile code bit 11
     xx------ attribute bit to use for tile code bit 12
006: ----xxxx select additional effect for bits 3-6 of the tile attribute (the
              same ones indexed by register 005). Note that an attribute bit
              can therefore be used at the same time to be BOTH a tile code bit
              and an additional effect.
     -------x bit 3 of attribute is bit 3 of color (combatsc, fastlane, flkatck)
     ------x- bit 4 of attribute is tile flip X (assumption - no game uses this)
     -----x-- bit 5 of attribute is tile flip Y (flkatck)
     ----x--- bit 6 of attribute is tile priority over sprites (combatsc, hcastle,
              labyrunr)
              Note that hcastle sets this bit for layer 0, and bit 6 of the
              attribute is also used as bit 12 of the tile code, however that
              bit is ALWAYS set throughout the game.
              combatsc uses the bit in the "graduation" scene during attract mode,
              to place soldiers behind the stand.
              Use in labyrunr has not been investigated yet.
     --xx---- palette bank (both tiles and sprites, see contra)
007: -------x nmi enable
     ------x- irq enable
     -----x-- firq enable
     ----x--- flip screen
     ---x---- unknown (contra, labyrunr)



007342
------
The 007342 manages 2 64x32 scrolling tilemaps with 8x8 characters, and
optionally generates timing clocks and interrupt signals. It uses 0x2000
bytes of RAM, plus 0x0200 bytes for scrolling, and a variable amount of ROM.
It cannot read the ROMs.

control registers
000: ------x- INT control
     ---x---- flip screen (TODO: doesn't work with thehustl)
001: Used for banking in Rock'n'Rage
002: -------x MSB of x scroll 1
     ------x- MSB of x scroll 2
     ---xxx-- layer 1 row/column scroll control
              000 = disabled
              010 = unknown (bladestl shootout between periods)
              011 = 32 columns (Blades of Steel)
              101 = 256 rows (Battlantis, Rock 'n Rage)
     x------- enable sprite wraparound from bottom to top (see Blades of Steel
              high score table)
003: x scroll 1
004: y scroll 1
005: x scroll 2
006: y scroll 2
007: not used


007420
------
Sprite generator. 8 bytes per sprite with zoom. It uses 0x200 bytes of RAM,
and a variable amount of ROM. Nothing is known about its external interface.



052109/051962
-------------
These work in pair.
The 052109 manages 3 64x32 scrolling tilemaps with 8x8 characters, and
optionally generates timing clocks and interrupt signals. It uses 0x4000
bytes of RAM, and a variable amount of ROM. It cannot read the ROMs:
instead, it exports 21 bits (16 from the tilemap RAM + 3 for the character
raster line + 2 additional ones for ROM banking) and these are externally
used to generate the address of the required data on the ROM; the output of
the ROMs is sent to the 051962, along with a color code. In theory you could
have any combination of bits in the tilemap RAM, as long as they add to 16.
In practice, all the games supported so far standardize on the same format
which uses 3 bits for the color code and 13 bits for the character code.
The 051962 multiplexes the data of the three layers and converts it into
palette indexes and transparency bits which will be mixed later in the video
chain.
Priority is handled externally: these chips only generate the tilemaps, they
don't mix them.
Both chips are interfaced with the main CPU. When the RMRD pin is asserted,
the CPU can read the gfx ROM data. This is done by telling the 052109 which
dword to read (this is a combination of some banking registers, and the CPU
address lines), and then reading it from the 051962.

052109 inputs:
- address lines (AB0-AB15, AB13-AB15 seem to have a different function)
- data lines (DB0-DB7)
- misc interface stuff

052109 outputs:
- address lines for the private RAM (RA0-RA12)
- data lines for the private RAM (VD0-VD15)
- NMI, IRQ, FIRQ for the main CPU
- misc interface stuff
- ROM bank selector (CAB1-CAB2)
- character "code" (VC0-VC10)
- character "color" (COL0-COL7); used foc color but also bank switching and tile
  flipping. Exact meaning depends on externl connections. All evidence indicates
  that COL2 and COL3 select the tile bank, and are replaced with the low 2 bits
  from the bank register. The top 2 bits of the register go to CAB1-CAB2.
  However, this DOES NOT WORK with Gradius III. "color" seems to pass through
  unaltered.
- layer A horizontal scroll (ZA1H-ZA4H)
- layer B horizontal scroll (ZB1H-ZB4H)
- ????? (BEN)

051962 inputs:
- gfx data from the ROMs (VC0-VC31)
- color code (COL0-COL7); only COL4-COL7 seem to really be used for color; COL0
  is tile flip X.
- layer A horizontal scroll (ZA1H-ZA4H)
- layer B horizontal scroll (ZB1H-ZB4H)
- let main CPU read the gfx ROMs (RMRD)
- address lines to be used with RMRD (AB0-AB1)
- data lines to be used with RMRD (DB0-DB7)
- ????? (BEN)
- misc interface stuff

051962 outputs:
- FIX layer palette index (DFI0-DFI7)
- FIX layer transparency (NFIC)
- A layer palette index (DSA0-DSAD); DSAA-DSAD seem to be unused
- A layer transparency (NSAC)
- B layer palette index (DSB0-DSBD); DSBA-DSBD seem to be unused
- B layer transparency (NSBC)
- misc interface stuff


052109 memory layout:
0000-07ff: layer FIX tilemap (attributes)
0800-0fff: layer A tilemap (attributes)
1000-1fff: layer B tilemap (attributes)
180c-1833: A y scroll
1a00-1bff: A x scroll
1c00     : ?
1c80     : row/column scroll control
           ------xx layer A row scroll
                    00 = disabled
                    01 = disabled? (gradius3, vendetta)
                    10 = 32 lines
                    11 = 256 lines
           -----x-- layer A column scroll
                    0 = disabled
                    1 = 64 (actually 40) columns
           ---xx--- layer B row scroll
           --x----- layer B column scroll
           surpratk sets this register to 70 during the second boss. There is
           nothing obviously wrong so it's not clear what should happen.
           glfgreat sets it to 30 when showing the leader board
1d00     : bits 0 & 1 might enable NMI and FIRQ, not sure
         : bit 2 = IRQ enable
1d80     : ROM bank selector bits 0-3 = bank 0 bits 4-7 = bank 1
1e00     : ROM membank selector for ROM testing
1e80     : bit 0 = flip screen (applies to tilemaps only, not sprites)
         : bit 1 = set by crimfght, mainevt, surpratk, xmen, mia, punkshot, thndrx2, spy
         :         it seems to enable tile flip X, however flip X is handled by the
         :         051962 and it is not hardwired to a specific tile attribute.
         :         Note that xmen, punkshot and thndrx2 set the bit but the current
         :         drivers don't use flip X and seem to work fine.
         : bit 2 = enables tile flip Y when bit 1 of the tile attribute is set
1f00     : ROM bank selector bits 0-3 = bank 2 bits 4-7 = bank 3
2000-27ff: layer FIX tilemap (code)
2800-2fff: layer A tilemap (code)
3000-37ff: layer B tilemap (code)
3800-3807: nothing here, so the chip can share address space with a 051937
380c-3833: B y scroll
3a00-3bff: B x scroll
3c00-3fff: nothing here, so the chip can share address space with a 051960
3d80     : mirror of 1d80, but ONLY during ROM test (surpratk)
3e00     : mirror of 1e00, but ONLY during ROM test (surpratk)
3f00     : mirror of 1f00, but ONLY during ROM test (surpratk)
EXTRA ADDRESSING SPACE USED BY X-MEN:
4000-47ff: layer FIX tilemap (code high bits)
4800-4fff: layer A tilemap (code high bits)
5000-57ff: layer B tilemap (code high bits)

The main CPU doesn't have direct acces to the RAM used by the 052109, it has
to through the chip.



054156/054157
054156/056832
-------------

[Except for tilemap sizes, all numbers are in hex]

These work in pairs.  Similar in principle to the 052109/051962, they
manage 4 64x32 or 64x64 tilemaps.  They also handle linescroll on each
layer, and optional tile banking.  They use 4000 to 10000 bytes of
RAM, organized in 1000 or 2000 bytes banks.

The 56832 is a complete superset of the 54157 and supports higher color
depths (the 156/157 combo only goes to 5 bpp, the 156/832 combo goes to 8bpp).

These chips work in a fairly unusual way.  There are 4, 8, or 16 pages of VRAM, arranged
conceptually in a 4x4 2 dimensional grid.  Each page is a complete 64x32 tile tilemap.

The 4 physical tilemaps A, B, C, and, D are made up of these pages "glued together".
Each physical tilemap has an X and Y position in the 4x4 page grid indicating where
the page making up it's upper left corner is, as well as a width and height in pages.
If two tilemaps try to use the same page, the higher-letter one wins and the lower-letter
one is disabled completely.  E.g. A > B > C > D, so if A and B both try to use the
same page only A will be displayed.  Some games rely on this behavior to implicitly
disable tilemaps which otherwise should be displayed.

Tile encoding 2 bytes/tile (banks of 1000 bytes):
        pppx bbcc cccc cccc
  p = color palette
  x = flip x
  b = tile bank (0..3)
  c = tile code (0..3ff)


Tile encoding 4 bytes/tile (banks of 2000 bytes):
        ---- ---- pppp --yx  cccc cccc cccc cccc
  p = color palette
  x = flip x
  y = flip y
  b = tile bank (0..3)
  c = tile code (0..3ff)


Communication with these ics go through 4 memory zones:
  1000/2000 bytes: access to the currently selected ram bank
       2000 bytes: readonly access the the currently select tile
                   rom bank for rom checksumming
         40 bytes: writeonly access to the first register bank
          8 bytes: writeonly access to the second register bank

One of the register banks is probably on the 054156, and the other on
the 054157.

First register bank map (offsets in bytes, '-' means unused):
00    ---- ---- ??yx ????
  flip control

02    ---- ---- ???? ????
  unknown

04    ---- ---- ???? ????
  unknown (bit 1 may be bank count selection, 0 in xexex, 1 everywhere
  else)

06    ---- ---- ???? ???e
  enable irq

08    ---- ---- ???? ????
  unknown

0a    ---- ---- 3322 1100
  linescroll control, each pair of bits indicates the mode for the
  corresponding layer:
    0: per-line linescroll
    1: unused/unknown
    2: per-8 lines linescroll
    3: no linescroll

0c    ---- ---- ???? ????
  unknown (bit 1 may be bank size selection, 1 in asterix, 0 everywhere
  else)

0e    ---- ---- ---- ----

10-13 ---- ---- ---y y-hh
   layer Y position in the VRAM grid and height in pages

14-17 ---- ---- ---x x-ww
   layer X position in the VRAM grid and width in pages
18-1f ---- ---- ???? ????

20-27 yyyy yyyy yyyy yyyy
  scroll y position for each layer

28-2f xxxx xxxx xxxx xxxx
  scroll x position for each layer

30    ---- ---- ---b b--b
  linescroll ram bank selection

32    ---- ---- ---b b--b
  cpu-accessible ram bank selection

34    bbbb bbbb bbbb bbbb
  rom bank selection for checksumming (each bank is 0x2000 bytes)

36    ---- ---- ---- bbbb
  secondary rom bank selection for checksumming when tile banking is
  used

38    3333 2222 1111 0000
  tile banking look up table.  4 bits are looked up here for the two
  bits in the tile data.

3a    ???? ???? ???? ????
  unknown

3c    ???? ???? ???? ????
  unknown

3e    ---- ---- ---- ----


Second register bank map:
00    ---- ---- ???? ????
  unknown

02-07 are copies of the 02-07 registers from the first bank.


  Linescroll:

The linescroll is controlled by the register 0b, and uses the data in
the ram bank pointed by register 31.  The data for tilemap <n> starts
at offset 400*n in the bank for 1000 bytes ram banks, and 800*n+2 for
2000 bytes ram banks.  The scrolling information is a vector of half
words separated by 1 word padding for 2000 bytes banks.

This is a source-oriented linescroll, i.e. the first word is
associated to the first one of the tilemap, not matter what the
current scrolly position is.

In per-line mode, each word indicates the horizontal scroll of the
associated line.  Global scrollx is ignored.

In per-8 lines mode, each word associated to a line multiple of 8
indicates the horizontal scroll for that line and the 7 following
ones.  The other 7 words are ignored.  Global scrollx is ignored.



051960/051937
-------------
Sprite generators. Designed to work in pair. The 051960 manages the sprite
list and produces and address that is fed to the gfx ROMs. The data from the
ROMs is sent to the 051937, along with color code and other stuff from the
051960. The 051937 outputs up to 12 bits of palette index, plus "shadow" and
transparency information.
Both chips are interfaced to the main CPU, through 8-bit data buses and 11
bits of address space. The 051937 sits in the range 000-007, while the 051960
in the range 400-7ff (all RAM). The main CPU can read the gfx ROM data though
the 051937 data bus, while the 051960 provides the address lines.
The 051960 is designed to directly address 1MB of ROM space, since it produces
18 address lines that go to two 16-bit wide ROMs (the 051937 has a 32-bit data
bus to the ROMs). However, the addressing space can be increased by using one
or more of the "color attribute" bits of the sprites as bank selectors.
Moreover a few games store the gfx data in the ROMs in a format different from
the one expected by the 051960, and use external logic to reorder the address
lines.
The 051960 can also genenrate IRQ, FIRQ and NMI signals.

memory map:
000-007 is for the 051937, but also seen by the 051960
400-7ff is 051960 only
000     R  bit 0 = unknown, looks like a status flag or something
                   aliens waits for it to be 0 before starting to copy sprite data
                   thndrx2 needs it to pulse for the startup checks to succeed
000     W  bit 0 = irq enable/acknowledge?
           bit 2 = nmi enable?
           bit 3 = flip screen (applies to sprites only, not tilemaps)
           bit 4 = unknown, used by Devastators, TMNT, Aliens, Chequered Flag, maybe others
                   aliens sets it just after checking bit 0, and before copying
                   the sprite data
           bit 5 = enable gfx ROM reading
001     W  Devastators sets bit 1, function unknown.
           Ultraman sets the register to 0x0f.
           None of the other games I tested seem to set this register to other than 0.
002-003 W  selects the portion of the gfx ROMs to be read.
004     W  Aliens uses this to select the ROM bank to be read, but Punk Shot
           and TMNT don't, they use another bit of the registers above. Many
           other games write to this register before testing.
           It is possible that bits 2-7 of 003 go to OC0-OC5, and bits 0-1 of
           004 go to OC6-OC7.
004-007 R  reads data from the gfx ROMs (32 bits in total). The address of the
           data is determined by the register above and by the last address
           accessed on the 051960; plus bank switch bits for larger ROMs.
           It seems that the data can also be read directly from the 051960
           address space: 88 Games does this. First it reads 004 and discards
           the result, then it reads from the 051960 the data at the address
           it wants. The normal order is the opposite, read from the 051960 at
           the address you want, discard the result, and fetch the data from
           004-007.
400-7ff RW sprite RAM, 8 bytes per sprite



053245/053244
-------------
Sprite generators. The 053245 has a 16-bit data bus to the main CPU.
The sprites are buffered, a write to 006 activates to copy between the
main ram and the buffer.

053244 memory map (but the 053245 sees and processes them too):
000-001  W global X offset
002-003  W global Y offset
004      W unknown
005      W bit 0 = flip screen X
           bit 1 = flip screen Y
           bit 2 = unknown, used by Parodius
           bit 4 = enable gfx ROM reading
           bit 5 = unknown, used by Rollergames
006     RW accessing this register copies the sprite ram to the internal buffer
007      W unknown
008-009  W low 16 bits of the ROM address to read
00a-00b  W high bits of the ROM address to read.  3 bits for most games, 1 for asterix
00c-00f R  reads data from the gfx ROMs (32 bits in total). The address of the
           data is determined by the registers above; plus bank switch bits for
           larger ROMs.



053247/053246
-------------
Sprite generators. Nothing is known about their external interface.
The sprite RAM format is very similar to the 053245.

053246 memory map (but the 053247 sees and processes them too):
000-001 W  global X offset
002-003 W  global Y offset
004     W  low 8 bits of the ROM address to read
005     W  bit 0 = flip screen X
           bit 1 = flip screen Y
           bit 2 = unknown
           bit 4 = interrupt enable
           bit 5 = unknown
006-007 W  high 16 bits of the ROM address to read

???-??? R  reads data from the gfx ROMs (16 bits in total). The address of the
           data is determined by the registers above



051316
------
Manages a 32x32 tilemap (16x16 tiles, 512x512 pixels) which can be zoomed,
distorted and rotated.
It uses two internal 24 bit counters which are incremented while scanning the
picture. The coordinates of the pixel in the tilemap that has to be drawn to
the current beam position are the counters / (2^11).
The chip doesn't directly generate the color information for the pixel, it
just generates a 24 bit address (whose top 16 bits are the contents of the
tilemap RAM), and a "visible" signal. It's up to external circuitry to convert
the address into a pixel color. Most games seem to use 4bpp graphics, but Ajax
uses 7bpp.
If the value in the internal counters is out of the visible range (0..511), it
is truncated and the corresponding address is still generated, but the "visible"
signal is not asserted. The external circuitry might ignore that signal and
still generate the pixel, therefore making the tilemap a continuous playfield
that wraps around instead of a large sprite.

control registers
000-001 X counter starting value / 256
002-003 amount to add to the X counter after each horizontal pixel
004-005 amount to add to the X counter after each line (0 = no rotation)
006-007 Y counter starting value / 256
008-009 amount to add to the Y counter after each horizontal pixel (0 = no rotation)
00a-00b amount to add to the Y counter after each line
00c-00d ROM bank to read, used during ROM testing
00e     bit 0 = enable ROM reading (active low). This only makes the chip output the
                requested address: the ROM is actually read externally, not through
                the chip's data bus.
        bit 1 = unknown
        bit 2 = unknown
00f     unused



053936
------
Evolution of the 051316. The data bus is 16-bit instead of 8-bit.
When used in "simple" mode it can generate the same effects of the 051316, but it
doesn't have internal tilemap RAM, so it just generates a couple of X/Y coordinates
indicating the pixel to display at each moment. Therefore, the tilemap and tile
sizes are not fixed.
The important addition over the 051316 is 512x4 words of internal RAM used to control
rotation and zoom scanline by scanline instead that on the whole screen, allowing for
effects like linescroll (Super Slams) or 3D rotation of the tilemap (Golfing Greats,
Premier Soccer).

control registers
000 X counter starting value / 256
001 Y counter starting value / 256
002 ["simple" mode only] amount to add to the X counter after each line (0 = no rotation)
003 ["simple" mode only] amount to add to the Y counter after each line
004 ["simple" mode only] amount to add to the X counter after each horizontal pixel
005 ["simple" mode only] amount to add to the Y counter after each horizontal pixel (0 = no rotation)
006 x------- -------- when set, register (line*4)+2 must be multiplied by 256
    -x------ -------- when set, registers 002 and 003 must be multiplied by 256
    --xxxxxx -------- clipping for the generated address? usually 3F, Premier Soccer
                      sets it to 07 before penalty kicks
    -------- x------- when set, register (line*4)+3 must be multiplied by 256
    -------- -x------ when set, registers 004 and 005 must be multiplied by 256
    -------- --xxxxxx clipping for the generated address? usually 3F, Premier Soccer
                      sets it to 0F before penalty kicks
007 -------- -x------ enable "super" mode
    -------- --x----- unknown (enable address clipping from register 006?)
    -------- ---x---- unknown
    -------- ------x- (not sure) enable clipping with registers 008-00b
008 min x screen coordinate to draw to (only when enabled by register 7)
009 max x screen coordinate to draw to (only when enabled by register 7)
00a min y screen coordinate to draw to (only when enabled by register 7)
00b max y screen coordinate to draw to (only when enabled by register 7)
00c unknown
00d unknown
00e unknown
00f unknown

additional control from extra RAM:
(line*4)+0 X counter starting value / 256 (add to register 000)
(line*4)+1 Y counter starting value / 256 (add to register 001)
(line*4)+2 amount to add to the X counter after each horizontal pixel
(line*4)+3 amount to add to the Y counter after each horizontal pixel



053251
------
Priority encoder.

The chip has inputs for 5 layers (CI0-CI4); only 4 are used (CI1-CI4)
CI0-CI2 are 9(=5+4) bits inputs, CI3-CI4 8(=4+4) bits

The input connctions change from game to game. E.g. in Simpsons,
CI0 = grounded (background color)
CI1 = sprites
CI2 = FIX
CI3 = A
CI4 = B

in lgtnfght:
CI0 = grounded
CI1 = sprites
CI2 = FIX
CI3 = B
CI4 = A

there are three 6 bit priority inputs, PR0-PR2

simpsons:
PR0 = 111111
PR1 = xxxxx0 x bits coming from the sprite attributes
PR2 = 111111

lgtnfght:
PR0 = 111111
PR1 = 1xx000 x bits coming from the sprite attributes
PR2 = 111111

also two shadow inputs, SDI0 and SDI1 (from the sprite attributes)

the chip outputs the 11 bit palette index, CO0-CO10, and two shadow bits.

16 internal registers; registers are 6 bits wide (input is D0-D5)
For the most part, their meaning is unknown
All registers are write only.
There must be a way to enable/disable the three external PR inputs.
Some games initialize the priorities of the sprite & background layers,
others don't. It isn't clear whether the data written to those registers is
actually used, since the priority is taken from the external ports.

 0  priority of CI0 (higher = lower priority)
    punkshot: unused?
    lgtnfght: unused?
    simpsons: 3f = 111111
    xmen:     05 = 000101  default value
    xmen:     09 = 001001  used to swap CI0 and CI2
 1  priority of CI1 (higher = lower priority)
    punkshot: 28 = 101000
    lgtnfght: unused?
    simpsons: unused?
    xmen:     02 = 000010
 2  priority of CI2 (higher = lower priority)
    punkshot: 24 = 100100
    lgtnfght: 24 = 100100
    simpsons: 04 = 000100
    xmen:     09 = 001001  default value
    xmen:     05 = 000101  used to swap CI0 and CI2
 3  priority of CI3 (higher = lower priority)
    punkshot: 34 = 110100
    lgtnfght: 34 = 110100
    simpsons: 28 = 101000
    xmen:     00 = 000000
 4  priority of CI4 (higher = lower priority)
    punkshot: 2c = 101100  default value
    punkshot: 3c = 111100  used to swap CI3 and CI4
    punkshot: 26 = 100110  used to swap CI1 and CI4
    lgtnfght: 2c = 101100
    simpsons: 18 = 011000
    xmen:     fe = 111110
 5  unknown
    punkshot: unused?
    lgtnfght: 2a = 101010
    simpsons: unused?
    xmen: unused?
 6  unknown
    punkshot: 26 = 100110
    lgtnfght: 30 = 110000
    simpsons: 17 = 010111
    xmen:     03 = 000011 (written after initial tests)
 7  unknown
    punkshot: unused?
    lgtnfght: unused?
    simpsons: 27 = 100111
    xmen:     07 = 000111 (written after initial tests)
 8  unknown
    punkshot: unused?
    lgtnfght: unused?
    simpsons: 37 = 110111
    xmen:     ff = 111111 (written after initial tests)
 9  ----xx CI0 palette index base (CO9-CO10)
    --xx-- CI1 palette index base (CO9-CO10)
    xx---- CI2 palette index base (CO9-CO10)
10  ---xxx CI3 palette index base (CO8-CO10)
    xxx--- CI4 palette index base (CO8-CO10)
11  unknown
    punkshot: 00 = 000000
    lgtnfght: 00 = 000000
    simpsons: 00 = 000000
    xmen:     00 = 000000 (written after initial tests)
12  unknown
    punkshot: 04 = 000100
    lgtnfght: 04 = 000100
    simpsons: 05 = 000101
    xmen:     05 = 000101
13  unused
14  unused
15  unused


054000
------
Sort of a protection device, used for collision detection.
It is passed a few parameters, and returns a boolean telling if collision
happened. It has no access to gfx data, it only does arithmetical operations
on the parameters.

Memory map:
00      unused
01-03 W A center X
04    W unknown, needed by thndrx2 to pass the startup check, we use a hack
05      unused
06    W A semiaxis X
07    W A semiaxis Y
08      unused
09-0b W A center Y
0c    W unknown, needed by thndrx2 to pass the startup check, we use a hack
0d      unused
0e    W B semiaxis X
0f    W B semiaxis Y
10      unused
11-13 W B center Y
14      unused
15-17 W B center X
18    R 0 = collision, 1 = no collision


051733
------
Sort of a protection device, used for collision detection, and for
arithmetical operations.
It is passed a few parameters, and returns the result.

Memory map(preliminary):
------------------------
00-01 W operand 1
02-03 W operand 2
04-05 W operand 3

00-01 R operand 1 / operand 2
02-03 R operand 1 % operand 2?
04-05 R sqrt(operand 3<<16)
06    R unknown - return value written to 13?

06-07 W distance for collision check
08-09 W Y pos of obj1
0a-0b W X pos of obj1
0c-0d W Y pos of obj2
0e-0f W X pos of obj2
13    W unknown

07    R collision (0x80 = no, 0x00 = yes)
0a-0b R unknown (chequered flag), might just read back X pos
0e-0f R unknown (chequered flag), might just read back X pos

Other addresses are unknown or unused.

Fast Lane:
----------
$9def:
This routine is called only after a collision.
(R) 0x0006: unknown. Only bits 0-3 are used.

Blades of Steel:
----------------
$ac2f:
(R) 0x2f86: unknown. Only uses bit 0.

$a5de:
writes to 0x2f84-0x2f85, waits a little, and then reads from 0x2f84.

$7af3:
(R) 0x2f86: unknown. Only uses bit 0.


Devastators:
------------
$6ce8:
reads from 0x0006, and only uses bit 1.


K055550
-------

Protection chip which performs a memset() operation.

Used in Violent Storm and Ultimate Battler to clear VRAM between scenes, among
other things.  May also perform other functions since Violent Storm still isn't
happy...

Has word-wide registers as follows:

0: Count of units to transfer.  The write here triggers the transfer.
1-6: Unknown
7: Destination address, MSW
8: Destination address, LSW
9: Unknown
10: Size of transfer units, MSW
11: Size of transfer units, LSW
12: Unknown
13: Value to fill destination region with
14-15: Unknown


K055555
-------

Priority encoder.  Always found in conjunction with k054338, but the reverse
isn't true.  The 55555 has 8 inputs: "A", "B", "C", and "D" intended for a 156/157
type tilemap chip, "OBJ" intended for a '246 type sprite chip, and "SUB1-SUB3"
which can be used for 3 additional layers.

When used in combintion with a k054338, each input can be chosen to participate
in shadow/highlight operations, R/G/B alpha blending, and R/G/B brightness control.
Per-tile priority is supported for tilemap planes A and B.

There are also 3 shadow priority registers.  When these are enabled, layers and
sprites with a priority greater than or equal to them become a shadow, and either
then gets drawn as a shadow/highlight or not at all (I'm not sure what selects
this yet.  Dadandarn relies on this mechanism to hide the 53936 plane when
it doesn't want it visible).

It also appears that brightness control and alpha blend can be decided per-tile
and per-sprite, although this is not certain.

Additionally the 55555 can provide a gradient background with one palette entry
per scanline.  This is fairly rarely used, but does turn up in Gokujou Parodius as
well as the Sexy Parodius title screen.

Lots of byte-wise registers.  A partial map:

0: Palette index(?) for the gradient background
1: related to tilemap brightness control
2-5: COLSEL for various inputs (?)
6: COLCHG ON
7-18: priority levels (VA1/VA2/VAC/VB1/VB2/VBC/VC/VD/OBJ/S1/S2/S3)
19-22: INPRI for OBJ/S1/S2/S3
23-32: palette bases (VA/VB/VC/VD/OBJ/S1/S2/S3)
37: shadow 1 priority
38: shadow 2 priority
39: shadow 3 priority
40: shadow/highlight master enable
41: master shadow/highlight priority
42: VBRI: enables brightness control for each VRAM layer (bits: x x x x D B C A)
43: OSBRI: enables brightness control for OBJ and SUB layers, depending for OBJ on attr bits
44: OSBRI_ON: not quite sure
45: input enables.  bits as follows: (MSB) S3 S2 S1 OB VD VC VB VA (LSB)


k054338
-------
Color combiner engine.  Designed for use with the 55555, but also found in games
without one.

Registers (word-wise):

0: first 8 bits unknown, second 8 bits are the R component of the background color
1: G and B components (8 bits each) of the background color
2-4: shadow 1 R/G/B (16 bits per component.  In shadow mode, determines a blend
     value between total blackness and the original color.  In highlight mode,
     determines a blend value between total whiteness and the original color.
     The hardware clamps at black or white as necessary: see the Graphics Test
     in many System GX games).
5-7: shadow 2 R/G/B
8-10: shadow 3 R/G/B
11-12: brightness R/G/B (external circuit such as the 55555 decides which layers
       this applies to)
13-14: alpha blend R/G/B (external circuit such as the 55555 decides which layers
       this applies to)

***************************************************************************/

#include "emu.h"
#include "video/konicdev.h"

#define VERBOSE 0
#define LOG(x) do { if (VERBOSE) logerror x; } while (0)


#define XOR(a) WORD_XOR_BE(a)

/*
    This recursive function doesn't use additional memory
    (it could be easily converted into an iterative one).
    It's called shuffle because it mimics the shuffling of a deck of cards.
*/
static void konami_shuffle_16(UINT16 *buf,int len)
{
	int i;
	UINT16 t;

	if (len == 2) return;

	if (len % 4) fatalerror("shuffle() - not modulo 4\n");   /* must not happen */

	len /= 2;

	for (i = 0; i < len / 2; i++)
	{
		t = buf[len / 2 + i];
		buf[len / 2 + i] = buf[len + i];
		buf[len + i] = t;
	}

	konami_shuffle_16(buf,len);
	konami_shuffle_16(buf + len,len);
}

static void konami_shuffle_8(UINT8 *buf,int len)
{
	int i;
	UINT8 t;

	if (len == 2) return;

	if (len % 4) fatalerror("shuffle() - not modulo 4\n");  /* must not happen */

	len /= 2;

	for (i = 0; i < len / 2; i++)
	{
		t = buf[len / 2 + i];
		buf[len / 2 + i] = buf[len + i];
		buf[len + i] = t;
	}

	konami_shuffle_8(buf,len);
	konami_shuffle_8(buf + len,len);
}


/* helper function to join two 16-bit ROMs and form a 32-bit data stream */
void konamid_rom_deinterleave_2(running_machine &machine, const char *mem_region)
{
	konami_shuffle_16((UINT16 *)machine.root_device().memregion(mem_region)->base(),machine.root_device().memregion(mem_region)->bytes()/2);
}

/* hacked version of rom_deinterleave_2_half for Lethal Enforcers */
void konamid_rom_deinterleave_2_half(running_machine &machine, const char *mem_region)
{
	UINT8 *rgn = machine.root_device().memregion(mem_region)->base();

	konami_shuffle_16((UINT16 *)rgn,machine.root_device().memregion(mem_region)->bytes()/4);
	konami_shuffle_16((UINT16 *)(rgn+machine.root_device().memregion(mem_region)->bytes()/2),machine.root_device().memregion(mem_region)->bytes()/4);
}

/* helper function to join four 16-bit ROMs and form a 64-bit data stream */
void konamid_rom_deinterleave_4(running_machine &machine, const char *mem_region)
{
	konamid_rom_deinterleave_2(machine, mem_region);
	konamid_rom_deinterleave_2(machine, mem_region);
}


static void decode_gfx(running_machine &machine, int gfx_index, UINT8 *data, UINT32 total, const gfx_layout *layout, int bpp)
{
	gfx_layout gl;

	memcpy(&gl, layout, sizeof(gl));
	gl.total = total;
	machine.gfx[gfx_index] = auto_alloc(machine, gfx_element(machine, gl, data, machine.total_colors() >> bpp, 0));
}


static void deinterleave_gfx(running_machine &machine, const char *gfx_memory_region, int deinterleave)
{
	switch (deinterleave)
	{
	case KONAMI_ROM_DEINTERLEAVE_NONE:
		break;
	case KONAMI_ROM_DEINTERLEAVE_2:
		konamid_rom_deinterleave_2(machine, gfx_memory_region);
		break;
	case KONAMI_ROM_DEINTERLEAVE_2_HALF:
		konamid_rom_deinterleave_2_half(machine, gfx_memory_region);
		break;
	case KONAMI_ROM_DEINTERLEAVE_4:
		konamid_rom_deinterleave_4(machine, gfx_memory_region);
		break;
	case KONAMI_ROM_SHUFFLE8:
		konami_shuffle_8(machine.root_device().memregion(gfx_memory_region)->base(), machine.root_device().memregion(gfx_memory_region)->bytes());
		break;
	}
}

/* useful function to sort three tile layers by priority order */
void konami_sortlayers3( int *layer, int *pri )
{
#define SWAP(a,b) \
	if (pri[a] < pri[b]) \
	{ \
		int t; \
		t = pri[a]; pri[a] = pri[b]; pri[b] = t; \
		t = layer[a]; layer[a] = layer[b]; layer[b] = t; \
	}

	SWAP(0,1)
	SWAP(0,2)
	SWAP(1,2)
#undef  SWAP
}

/* useful function to sort four tile layers by priority order */
void konami_sortlayers4( int *layer, int *pri )
{
#define SWAP(a,b) \
	if (pri[a] <= pri[b]) \
	{ \
		int t; \
		t = pri[a]; pri[a] = pri[b]; pri[b] = t; \
		t = layer[a]; layer[a] = layer[b]; layer[b] = t; \
	}

	SWAP(0, 1)
	SWAP(0, 2)
	SWAP(0, 3)
	SWAP(1, 2)
	SWAP(1, 3)
	SWAP(2, 3)
#undef  SWAP
}

/* useful function to sort five tile layers by priority order */
void konami_sortlayers5( int *layer, int *pri )
{
#define SWAP(a,b) \
	if (pri[a] <= pri[b]) \
	{ \
		int t; \
		t = pri[a]; pri[a] = pri[b]; pri[b] = t; \
		t = layer[a]; layer[a] = layer[b]; layer[b] = t; \
	}

	SWAP(0, 1)
	SWAP(0, 2)
	SWAP(0, 3)
	SWAP(0, 4)
	SWAP(1, 2)
	SWAP(1, 3)
	SWAP(1, 4)
	SWAP(2, 3)
	SWAP(2, 4)
	SWAP(3, 4)
#undef  SWAP
}


/***************************************************************************/
/*                                                                         */
/*                                 007121                                  */
/*                                                                         */
/***************************************************************************/


const device_type K007121 = &device_creator<k007121_device>;

k007121_device::k007121_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K007121, "Konami 007121", tag, owner, clock, "k007121", __FILE__),
	m_flipscreen(0)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k007121_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k007121_device::device_start()
{
	save_item(NAME(m_ctrlram));
	save_item(NAME(m_flipscreen));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k007121_device::device_reset()
{
	int i;

	m_flipscreen = 0;

	for (i = 0; i < 8; i++)
		m_ctrlram[i] = 0;
}


/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

READ8_MEMBER( k007121_device::ctrlram_r )
{
	assert(offset < 8);

	return m_ctrlram[offset];
}


WRITE8_MEMBER( k007121_device::ctrl_w )
{
	assert(offset < 8);

	switch (offset)
	{
	case 6:
		/* palette bank change */
		if ((m_ctrlram[offset] & 0x30) != (data & 0x30))
			space.machine().tilemap().mark_all_dirty();
		break;
	case 7:
		m_flipscreen = data & 0x08;
		break;
	}

	m_ctrlram[offset] = data;
}

/*
 * Sprite Format
 * ------------------
 *
 * There are 0x40 sprites, each one using 5 bytes. However the number of
 * sprites can be increased to 0x80 with a control register (Combat School
 * sets it on and off during the game).
 *
 * Byte | Bit(s)   | Use
 * -----+-76543210-+----------------
 *   0  | xxxxxxxx | sprite code
 *   1  | xxxx---- | color
 *   1  | ----xx-- | sprite code low 2 bits for 16x8/8x8 sprites
 *   1  | ------xx | sprite code bank bits 1/0
 *   2  | xxxxxxxx | y position
 *   3  | xxxxxxxx | x position (low 8 bits)
 *   4  | xx------ | sprite code bank bits 3/2
 *   4  | --x----- | flip y
 *   4  | ---x---- | flip x
 *   4  | ----xxx- | sprite size 000=16x16 001=16x8 010=8x16 011=8x8 100=32x32
 *   4  | -------x | x position (high bit)
 *
 * Flack Attack uses a different, "wider" layout with 32 bytes per sprite,
 * mapped as follows, and the priority order is reversed. Maybe it is a
 * compatibility mode with an older custom IC. It is not known how this
 * alternate layout is selected.
 *
 * 0 -> e
 * 1 -> f
 * 2 -> 6
 * 3 -> 4
 * 4 -> 8
 *
 */

void k007121_device::sprites_draw( bitmap_ind16 &bitmap, const rectangle &cliprect, gfx_element *gfx, colortable_t *ctable,
							const UINT8 *source, int base_color, int global_x_offset, int bank_base, UINT32 pri_mask )
{
	//  gfx_element *gfx = gfxs[chip];
	bitmap_ind8 &priority_bitmap = gfx->machine().priority_bitmap;
	int flipscreen = m_flipscreen;
	int i, num, inc, offs[5];
	int is_flakatck = (ctable == NULL);

	if (is_flakatck)
	{
		num = 0x40;
		inc = -0x20;
		source += 0x3f * 0x20;
		offs[0] = 0x0e;
		offs[1] = 0x0f;
		offs[2] = 0x06;
		offs[3] = 0x04;
		offs[4] = 0x08;
	}
	else    /* all others */
	{
		/* TODO: sprite limit is supposed to be per-line! (check MT #00185) */
		num = 0x40;
		//num = (k007121->ctrlram[0x03] & 0x40) ? 0x80 : 0x40; /* WRONG!!! (needed by combatsc)  */

		inc = 5;
		offs[0] = 0x00;
		offs[1] = 0x01;
		offs[2] = 0x02;
		offs[3] = 0x03;
		offs[4] = 0x04;
		/* when using priority buffer, draw front to back */
		if (pri_mask != -1)
		{
			source += (num - 1)*inc;
			inc = -inc;
		}
	}

	for (i = 0; i < num; i++)
	{
		int number = source[offs[0]];               /* sprite number */
		int sprite_bank = source[offs[1]] & 0x0f;   /* sprite bank */
		int sx = source[offs[3]];                   /* vertical position */
		int sy = source[offs[2]];                   /* horizontal position */
		int attr = source[offs[4]];             /* attributes */
		int xflip = source[offs[4]] & 0x10;     /* flip x */
		int yflip = source[offs[4]] & 0x20;     /* flip y */
		int color = base_color + ((source[offs[1]] & 0xf0) >> 4);
		int width, height;
		int transparent_mask;
		static const int x_offset[4] = {0x0,0x1,0x4,0x5};
		static const int y_offset[4] = {0x0,0x2,0x8,0xa};
		int x,y, ex, ey, flipx, flipy, destx, desty;

		if (attr & 0x01) sx -= 256;
		if (sy >= 240) sy -= 256;

		number += ((sprite_bank & 0x3) << 8) + ((attr & 0xc0) << 4);
		number = number << 2;
		number += (sprite_bank >> 2) & 3;

		/* Flak Attack doesn't use a lookup PROM, it maps the color code directly */
		/* to a palette entry */
		if (is_flakatck)
			transparent_mask = 1 << 0;
		else
			transparent_mask = colortable_get_transpen_mask(ctable, gfx, color, 0);

		if (!is_flakatck || source[0x00])   /* Flak Attack needs this */
		{
			number += bank_base;

			switch (attr & 0xe)
			{
				case 0x06: width = height = 1; break;
				case 0x04: width = 1; height = 2; number &= (~2); break;
				case 0x02: width = 2; height = 1; number &= (~1); break;
				case 0x00: width = height = 2; number &= (~3); break;
				case 0x08: width = height = 4; number &= (~3); break;
				default: width = 1; height = 1;
//                  logerror("Unknown sprite size %02x\n", attr & 0xe);
//                  popmessage("Unknown sprite size %02x\n", attr & 0xe);
			}

			for (y = 0; y < height; y++)
			{
				for (x = 0; x < width; x++)
				{
					ex = xflip ? (width - 1 - x) : x;
					ey = yflip ? (height - 1 - y) : y;

					if (flipscreen)
					{
						flipx = !xflip;
						flipy = !yflip;
						destx = 248 - (sx + x * 8);
						desty = 248 - (sy + y * 8);
					}
					else
					{
						flipx = xflip;
						flipy = yflip;
						destx = global_x_offset + sx + x * 8;
						desty = sy + y * 8;
					}

					if (pri_mask != -1)
						pdrawgfx_transmask(bitmap,cliprect,gfx,
							number + x_offset[ex] + y_offset[ey],
							color,
							flipx,flipy,
							destx,desty,
							priority_bitmap,pri_mask,
							transparent_mask);
					else
						drawgfx_transmask(bitmap,cliprect,gfx,
							number + x_offset[ex] + y_offset[ey],
							color,
							flipx,flipy,
							destx,desty,
							transparent_mask);
				}
			}
		}

		source += inc;
	}
}

/***************************************************************************/
/*                                                                         */
/*                                 007342                                  */
/*                                                                         */
/***************************************************************************/

const device_type K007342 = &device_creator<k007342_device>;

k007342_device::k007342_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K007342, "Konami 007342", tag, owner, clock, "k007342", __FILE__),
	m_ram(NULL),
	m_scroll_ram(NULL),
	m_videoram_0(NULL),
	m_videoram_1(NULL),
	m_colorram_0(NULL),
	m_colorram_1(NULL),
	//m_tilemap[2];
	m_flipscreen(0),
	m_int_enabled(0)
	//m_regs[8],
	//m_scrollx[2],
	//m_scrolly[2]
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k007342_device::device_config_complete()
{
	// inherit a copy of the static data
	const k007342_interface *intf = reinterpret_cast<const k007342_interface *>(static_config());
	if (intf != NULL)
	*static_cast<k007342_interface *>(this) = *intf;
	
	// or initialize to defaults if none provided
	else
	{
		m_gfxnum = 0;
		m_callback = NULL;
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k007342_device::device_start()
{
	m_tilemap[0] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k007342_device::get_tile_info0),this), tilemap_mapper_delegate(FUNC(k007342_device::scan),this), 8, 8, 64, 32);
	m_tilemap[1] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k007342_device::get_tile_info1),this), tilemap_mapper_delegate(FUNC(k007342_device::scan),this), 8, 8, 64, 32);

	m_ram = auto_alloc_array_clear(machine(), UINT8, 0x2000);
	m_scroll_ram = auto_alloc_array_clear(machine(), UINT8, 0x0200);

	m_colorram_0 = &m_ram[0x0000];
	m_colorram_1 = &m_ram[0x1000];
	m_videoram_0 = &m_ram[0x0800];
	m_videoram_1 = &m_ram[0x1800];

	m_tilemap[0]->set_transparent_pen(0);
	m_tilemap[1]->set_transparent_pen(0);

	save_pointer(NAME(m_ram), 0x2000);
	save_pointer(NAME(m_scroll_ram), 0x0200);
	save_item(NAME(m_int_enabled));
	save_item(NAME(m_flipscreen));
	save_item(NAME(m_scrollx));
	save_item(NAME(m_scrolly));
	save_item(NAME(m_regs));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k007342_device::device_reset()
{
	int i;

	m_int_enabled = 0;
	m_flipscreen = 0;
	m_scrollx[0] = 0;
	m_scrollx[1] = 0;
	m_scrolly[0] = 0;
	m_scrolly[1] = 0;

	for (i = 0; i < 8; i++)
		m_regs[i] = 0;
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

READ8_MEMBER( k007342_device::read )
{
	return m_ram[offset];
}

WRITE8_MEMBER( k007342_device::write )
{
	m_ram[offset] = data;

	if (offset < 0x1000)    /* layer 0 */
		m_tilemap[0]->mark_tile_dirty(offset & 0x7ff);
	else                /* layer 1 */
		m_tilemap[1]->mark_tile_dirty(offset & 0x7ff);
}

READ8_MEMBER( k007342_device::scroll_r )
{
	return m_scroll_ram[offset];
}

WRITE8_MEMBER( k007342_device::scroll_w )
{
	m_scroll_ram[offset] = data;
}

WRITE8_MEMBER( k007342_device::vreg_w )
{
	switch(offset)
	{
		case 0x00:
			/* bit 1: INT control */
			m_int_enabled = data & 0x02;
			m_flipscreen = data & 0x10;
			m_tilemap[0]->set_flip(m_flipscreen ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
			m_tilemap[1]->set_flip(m_flipscreen ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
			break;
		case 0x01:  /* used for banking in Rock'n'Rage */
			if (data != m_regs[1])
				space.machine().tilemap().mark_all_dirty();
		case 0x02:
			m_scrollx[0] = (m_scrollx[0] & 0xff) | ((data & 0x01) << 8);
			m_scrollx[1] = (m_scrollx[1] & 0xff) | ((data & 0x02) << 7);
			break;
		case 0x03:  /* scroll x (register 0) */
			m_scrollx[0] = (m_scrollx[0] & 0x100) | data;
			break;
		case 0x04:  /* scroll y (register 0) */
			m_scrolly[0] = data;
			break;
		case 0x05:  /* scroll x (register 1) */
			m_scrollx[1] = (m_scrollx[1] & 0x100) | data;
			break;
		case 0x06:  /* scroll y (register 1) */
			m_scrolly[1] = data;
		case 0x07:  /* unused */
			break;
	}
	m_regs[offset] = data;
}

void k007342_device::tilemap_update( )
{
	int offs;

	/* update scroll */
	switch (m_regs[2] & 0x1c)
	{
		case 0x00:
		case 0x08:  /* unknown, blades of steel shootout between periods */
			m_tilemap[0]->set_scroll_rows(1);
			m_tilemap[0]->set_scroll_cols(1);
			m_tilemap[0]->set_scrollx(0, m_scrollx[0]);
			m_tilemap[0]->set_scrolly(0, m_scrolly[0]);
			break;

		case 0x0c:  /* 32 columns */
			m_tilemap[0]->set_scroll_rows(1);
			m_tilemap[0]->set_scroll_cols(512);
			m_tilemap[0]->set_scrollx(0, m_scrollx[0]);
			for (offs = 0; offs < 256; offs++)
				m_tilemap[0]->set_scrolly((offs + m_scrollx[0]) & 0x1ff,
						m_scroll_ram[2 * (offs / 8)] + 256 * m_scroll_ram[2 * (offs / 8) + 1]);
			break;

		case 0x14:  /* 256 rows */
			m_tilemap[0]->set_scroll_rows(256);
			m_tilemap[0]->set_scroll_cols(1);
			m_tilemap[0]->set_scrolly(0, m_scrolly[0]);
			for (offs = 0; offs < 256; offs++)
				m_tilemap[0]->set_scrollx((offs + m_scrolly[0]) & 0xff,
						m_scroll_ram[2 * offs] + 256 * m_scroll_ram[2 * offs + 1]);
			break;

		default:
//          popmessage("unknown scroll ctrl %02x", m_regs[2] & 0x1c);
			break;
	}

	m_tilemap[1]->set_scrollx(0, m_scrollx[1]);
	m_tilemap[1]->set_scrolly(0, m_scrolly[1]);

#if 0
	{
		static int current_layer = 0;

		if (machine.input().code_pressed_once(KEYCODE_Z)) current_layer = !current_layer;
		m_tilemap[current_layer]->enable(1);
		m_tilemap[!current_layer]->enable(0);

		popmessage("regs:%02x %02x %02x %02x-%02x %02x %02x %02x:%02x",
			m_regs[0], m_regs[1], m_regs[2], m_regs[3],
			m_regs[4], m_regs[5], m_regs[6], m_regs[7],
			current_layer);
	}
#endif
}

void k007342_device::tilemap_draw( bitmap_ind16 &bitmap, const rectangle &cliprect, int num, int flags, UINT32 priority )
{
	m_tilemap[num]->draw(bitmap, cliprect, flags, priority);
}

int k007342_device::is_int_enabled( )
{
	return m_int_enabled;
}


/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

/*
  data format:
  video RAM     xxxxxxxx    tile number (bits 0-7)
  color RAM     x-------    tiles with priority over the sprites
  color RAM     -x------    depends on external conections
  color RAM     --x-----    flip Y
  color RAM     ---x----    flip X
  color RAM     ----xxxx    depends on external connections (usually color and banking)
*/

TILEMAP_MAPPER_MEMBER(k007342_device::scan)
{
	/* logical (col,row) -> memory offset */
	return (col & 0x1f) + ((row & 0x1f) << 5) + ((col & 0x20) << 5);
}

void k007342_device::get_tile_info( tile_data &tileinfo, int tile_index, int layer, UINT8 *cram, UINT8 *vram )
{
	int color, code, flags;

	color = cram[tile_index];
	code = vram[tile_index];
	flags = TILE_FLIPYX((color & 0x30) >> 4);

	tileinfo.category = (color & 0x80) >> 7;

	m_callback(machine(), layer, m_regs[1], &code, &color, &flags);

	SET_TILE_INFO_MEMBER(
			m_gfxnum,
			code,
			color,
			flags);
}

TILE_GET_INFO_MEMBER(k007342_device::get_tile_info0)
{
	get_tile_info(tileinfo, tile_index, 0, m_colorram_0, m_videoram_0);
}

TILE_GET_INFO_MEMBER(k007342_device::get_tile_info1)
{
	get_tile_info(tileinfo, tile_index, 1, m_colorram_1, m_videoram_1);
}

/***************************************************************************/
/*                                                                         */
/*                                 007420                                  */
/*                                                                         */
/***************************************************************************/

#define K007420_SPRITERAM_SIZE 0x200

const device_type K007420 = &device_creator<k007420_device>;

k007420_device::k007420_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K007420, "Konami 007420", tag, owner, clock, "k007420", __FILE__),
	m_ram(NULL),
    m_flipscreen(0)
	//m_regs[8],
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k007420_device::device_config_complete()
{
	// inherit a copy of the static data
	const k007420_interface *intf = reinterpret_cast<const k007420_interface *>(static_config());
	if (intf != NULL)
	*static_cast<k007420_interface *>(this) = *intf;
	
	// or initialize to defaults if none provided
	else
	{
		m_banklimit = 0;
		m_callback = NULL;
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k007420_device::device_start()
{
	m_ram = auto_alloc_array_clear(machine(), UINT8, 0x200);

	save_pointer(NAME(m_ram), 0x200);
	save_item(NAME(m_flipscreen));   // current one uses 7342 one
	save_item(NAME(m_regs)); // current one uses 7342 ones
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k007420_device::device_reset()
{
	int i;

	m_flipscreen = 0;
	for (i = 0; i < 8; i++)
		m_regs[i] = 0;
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

READ8_MEMBER( k007420_device::read )
{
	return m_ram[offset];
}

WRITE8_MEMBER( k007420_device::write )
{
	m_ram[offset] = data;
}

/*
 * Sprite Format
 * ------------------
 *
 * Byte | Bit(s)   | Use
 * -----+-76543210-+----------------
 *   0  | xxxxxxxx | y position
 *   1  | xxxxxxxx | sprite code (low 8 bits)
 *   2  | xxxxxxxx | depends on external conections. Usually banking
 *   3  | xxxxxxxx | x position (low 8 bits)
 *   4  | x------- | x position (high bit)
 *   4  | -xxx---- | sprite size 000=16x16 001=8x16 010=16x8 011=8x8 100=32x32
 *   4  | ----x--- | flip y
 *   4  | -----x-- | flip x
 *   4  | ------xx | zoom (bits 8 & 9)
 *   5  | xxxxxxxx | zoom (low 8 bits)  0x080 = normal, < 0x80 enlarge, > 0x80 reduce
 *   6  | xxxxxxxx | unused
 *   7  | xxxxxxxx | unused
 */

void k007420_device::sprites_draw( bitmap_ind16 &bitmap, const rectangle &cliprect, gfx_element *gfx )
{
	int offs;
	int codemask = m_banklimit;
	int bankmask = ~m_banklimit;

	for (offs = K007420_SPRITERAM_SIZE - 8; offs >= 0; offs -= 8)
	{
		int ox, oy, code, color, flipx, flipy, zoom, w, h, x, y, bank;
		static const int xoffset[4] = { 0, 1, 4, 5 };
		static const int yoffset[4] = { 0, 2, 8, 10 };

		code = m_ram[offs + 1];
		color = m_ram[offs + 2];
		ox = m_ram[offs + 3] - ((m_ram[offs + 4] & 0x80) << 1);
		oy = 256 - m_ram[offs + 0];
		flipx = m_ram[offs + 4] & 0x04;
		flipy = m_ram[offs + 4] & 0x08;

		m_callback(machine(), &code, &color);

		bank = code & bankmask;
		code &= codemask;

		/* 0x080 = normal scale, 0x040 = double size, 0x100 half size */
		zoom = m_ram[offs + 5] | ((m_ram[offs + 4] & 0x03) << 8);
		if (!zoom)
			continue;
		zoom = 0x10000 * 128 / zoom;

		switch (m_ram[offs + 4] & 0x70)
		{
			case 0x30: w = h = 1; break;
			case 0x20: w = 2; h = 1; code &= (~1); break;
			case 0x10: w = 1; h = 2; code &= (~2); break;
			case 0x00: w = h = 2; code &= (~3); break;
			case 0x40: w = h = 4; code &= (~3); break;
			default: w = 1; h = 1;
//logerror("Unknown sprite size %02x\n",(m_ram[offs + 4] & 0x70) >> 4);
		}

		if (m_flipscreen)
		{
			ox = 256 - ox - ((zoom * w + (1 << 12)) >> 13);
			oy = 256 - oy - ((zoom * h + (1 << 12)) >> 13);
			flipx = !flipx;
			flipy = !flipy;
		}

		if (zoom == 0x10000)
		{
			int sx, sy;

			for (y = 0; y < h; y++)
			{
				sy = oy + 8 * y;

				for (x = 0; x < w; x++)
				{
					int c = code;

					sx = ox + 8 * x;
					if (flipx)
						c += xoffset[(w - 1 - x)];
					else
						c += xoffset[x];

					if (flipy)
						c += yoffset[(h - 1 - y)];
					else
						c += yoffset[y];

					if (c & bankmask)
						continue;
					else
						c += bank;

					drawgfx_transpen(bitmap,cliprect,gfx,
						c,
						color,
						flipx,flipy,
						sx,sy,0);

					if (m_regs[2] & 0x80)
						drawgfx_transpen(bitmap,cliprect,gfx,
							c,
							color,
							flipx,flipy,
							sx,sy-256,0);
				}
			}
		}
		else
		{
			int sx, sy, zw, zh;
			for (y = 0; y < h; y++)
			{
				sy = oy + ((zoom * y + (1 << 12)) >> 13);
				zh = (oy + ((zoom * (y + 1) + (1 << 12)) >> 13)) - sy;

				for (x = 0; x < w; x++)
				{
					int c = code;

					sx = ox + ((zoom * x + (1<<12)) >> 13);
					zw = (ox + ((zoom * (x + 1) + (1 << 12)) >> 13)) - sx;
					if (flipx)
						c += xoffset[(w - 1 - x)];
					else
						c += xoffset[x];

					if (flipy)
						c += yoffset[(h - 1 - y)];
					else
						c += yoffset[y];

					if (c & bankmask)
						continue;
					else
						c += bank;

					drawgfxzoom_transpen(bitmap,cliprect,gfx,
						c,
						color,
						flipx,flipy,
						sx,sy,
						(zw << 16) / 8,(zh << 16) / 8,0);

					if (m_regs[2] & 0x80)
						drawgfxzoom_transpen(bitmap,cliprect,gfx,
							c,
							color,
							flipx,flipy,
							sx,sy-256,
							(zw << 16) / 8,(zh << 16) / 8,0);
				}
			}
		}
	}
#if 0
	{
		static int current_sprite = 0;

		if (machine().input().code_pressed_once(KEYCODE_Z)) current_sprite = (current_sprite+1) & ((K007420_SPRITERAM_SIZE/8)-1);
		if (machine().input().code_pressed_once(KEYCODE_X)) current_sprite = (current_sprite-1) & ((K007420_SPRITERAM_SIZE/8)-1);

		popmessage("%02x:%02x %02x %02x %02x %02x %02x %02x %02x", current_sprite,
			m_ram[(current_sprite*8)+0], m_ram[(current_sprite*8)+1],
			m_ram[(current_sprite*8)+2], m_ram[(current_sprite*8)+3],
			m_ram[(current_sprite*8)+4], m_ram[(current_sprite*8)+5],
			m_ram[(current_sprite*8)+6], m_ram[(current_sprite*8)+7]);
	}
#endif
}


/***************************************************************************/
/*                                                                         */
/*                                 052109                                  */
/*                                                                         */
/***************************************************************************/

const device_type K052109 = &device_creator<k052109_device>;

k052109_device::k052109_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K052109, "Konami 052109", tag, owner, clock, "k052109", __FILE__),
	m_ram(NULL),
	m_videoram_F(NULL),
	m_videoram_A(NULL),
	m_videoram_B(NULL),
	m_videoram2_F(NULL),
	m_videoram2_A(NULL),
	m_videoram2_B(NULL),
	m_colorram_F(NULL),
	m_colorram_A(NULL),
	m_colorram_B(NULL),

	//m_tilemap[3],
	m_tileflip_enable(0),
	//m_charrombank[4],
	//m_charrombank_2[4],
	m_has_extra_video_ram(0),
	m_rmrd_line(0),
	m_irq_enabled(0),
	//m_dx[3], m_dy[3],
	m_romsubbank(0),
	m_scrollctrl(0)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k052109_device::device_config_complete()
{
	// inherit a copy of the static data
	const k052109_interface *intf = reinterpret_cast<const k052109_interface *>(static_config());
	if (intf != NULL)
	*static_cast<k052109_interface *>(this) = *intf;
	
	// or initialize to defaults if none provided
	else
	{
		m_gfx_memory_region = "";
		m_gfx_num = 0;
		m_plane_order = 0;
		m_deinterleave = 0;
		m_callback = NULL;
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k052109_device::device_start()
{
	UINT32 total;
	static const gfx_layout charlayout =
	{
		8,8,
		0,
		4,
		{ 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7 },
		{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
		32*8
	};
	static const gfx_layout charlayout_gradius3 =
	{
		8,8,
		0,
		4,
		{ 0, 1, 2, 3 },
		{ XOR(0)*4, XOR(1)*4, XOR(2)*4, XOR(3)*4, XOR(4)*4, XOR(5)*4, XOR(6)*4, XOR(7)*4 },
		{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
		32*8
	};


	/* decode the graphics */
	switch (m_plane_order)
	{
	case NORMAL_PLANE_ORDER:
		total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / 32;
		decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &charlayout, 4);
		break;

	case GRADIUS3_PLANE_ORDER:
		total = 0x1000;
		decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &charlayout_gradius3, 4);
		break;

	default:
		fatalerror("Unsupported plane_order\n");
	}

	/* deinterleave the graphics, if needed */
	deinterleave_gfx(machine(), m_gfx_memory_region, m_deinterleave);

	m_tilemap[0] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k052109_device::get_tile_info0),this), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);
	m_tilemap[1] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k052109_device::get_tile_info1),this), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);
	m_tilemap[2] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k052109_device::get_tile_info2),this), TILEMAP_SCAN_ROWS, 8, 8, 64, 32);

	m_ram = auto_alloc_array_clear(machine(), UINT8, 0x6000);

	m_colorram_F = &m_ram[0x0000];
	m_colorram_A = &m_ram[0x0800];
	m_colorram_B = &m_ram[0x1000];
	m_videoram_F = &m_ram[0x2000];
	m_videoram_A = &m_ram[0x2800];
	m_videoram_B = &m_ram[0x3000];
	m_videoram2_F = &m_ram[0x4000];
	m_videoram2_A = &m_ram[0x4800];
	m_videoram2_B = &m_ram[0x5000];

	m_tilemap[0]->set_transparent_pen(0);
	m_tilemap[1]->set_transparent_pen(0);
	m_tilemap[2]->set_transparent_pen(0);

	save_pointer(NAME(m_ram), 0x6000);
	save_item(NAME(m_rmrd_line));
	save_item(NAME(m_romsubbank));
	save_item(NAME(m_scrollctrl));
	save_item(NAME(m_irq_enabled));
	save_item(NAME(m_charrombank));
	save_item(NAME(m_charrombank_2));
	save_item(NAME(m_dx));
	save_item(NAME(m_dy));
	save_item(NAME(m_has_extra_video_ram));
	machine().save().register_postload(save_prepost_delegate(FUNC(k052109_device::tileflip_reset), this));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k052109_device::device_reset()
{
	int i;

	m_rmrd_line = CLEAR_LINE;
	m_irq_enabled = 0;
	m_romsubbank = 0;
	m_scrollctrl = 0;

	m_has_extra_video_ram = 0;

	for (i = 0; i < 3; i++)
		m_dx[i] = m_dy[i] = 0;

	for (i = 0; i < 4; i++)
	{
		m_charrombank[i] = 0;
		m_charrombank_2[i] = 0;
	}
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

READ8_MEMBER( k052109_device::read )
{
	if (m_rmrd_line == CLEAR_LINE)
	{
		if ((offset & 0x1fff) >= 0x1800)
		{
			if (offset >= 0x180c && offset < 0x1834)
			{   /* A y scroll */    }
			else if (offset >= 0x1a00 && offset < 0x1c00)
			{   /* A x scroll */    }
			else if (offset == 0x1d00)
			{   /* read for bitwise operations before writing */    }
			else if (offset >= 0x380c && offset < 0x3834)
			{   /* B y scroll */    }
			else if (offset >= 0x3a00 && offset < 0x3c00)
			{   /* B x scroll */    }
//          else
//logerror("%04x: read from unknown 052109 address %04x\n",space.device().safe_pc(),offset);
		}

		return m_ram[offset];
	}
	else    /* Punk Shot and TMNT read from 0000-1fff, Aliens from 2000-3fff */
	{
		int code = (offset & 0x1fff) >> 5;
		int color = m_romsubbank;
		int flags = 0;
		int priority = 0;
		int bank = m_charrombank[(color & 0x0c) >> 2] >> 2;   /* discard low bits (TMNT) */
		int addr;

		bank |= (m_charrombank_2[(color & 0x0c) >> 2] >> 2); // Surprise Attack uses this 2nd bank in the rom test

	if (m_has_extra_video_ram)
		code |= color << 8; /* kludge for X-Men */
	else
		m_callback(space.machine(), 0, bank, &code, &color, &flags, &priority);

		addr = (code << 5) + (offset & 0x1f);
		addr &= space.machine().root_device().memregion(m_gfx_memory_region)->bytes() - 1;

//      logerror("%04x: off = %04x sub = %02x (bnk = %x) adr = %06x\n", space.device().safe_pc(), offset, m_romsubbank, bank, addr);

		return space.machine().root_device().memregion(m_gfx_memory_region)->base()[addr];
	}
}

WRITE8_MEMBER( k052109_device::write )
{
	if ((offset & 0x1fff) < 0x1800) /* tilemap RAM */
	{
		if (offset >= 0x4000)
			m_has_extra_video_ram = 1;  /* kludge for X-Men */

		m_ram[offset] = data;
		m_tilemap[(offset & 0x1800) >> 11]->mark_tile_dirty(offset & 0x7ff);
	}
	else    /* control registers */
	{
		m_ram[offset] = data;

		if (offset >= 0x180c && offset < 0x1834)
		{   /* A y scroll */    }
		else if (offset >= 0x1a00 && offset < 0x1c00)
		{   /* A x scroll */    }
		else if (offset == 0x1c80)
		{
			if (m_scrollctrl != data)
			{
//popmessage("scrollcontrol = %02x", data);
//logerror("%04x: rowscrollcontrol = %02x\n", space.device().safe_pc(), data);
				m_scrollctrl = data;
			}
		}
		else if (offset == 0x1d00)
		{
//logerror("%04x: 052109 register 1d00 = %02x\n", space.device().safe_pc(), data);
			/* bit 2 = irq enable */
			/* the custom chip can also generate NMI and FIRQ, for use with a 6809 */
			m_irq_enabled = data & 0x04;
		}
		else if (offset == 0x1d80)
		{
			int dirty = 0;

			if (m_charrombank[0] != (data & 0x0f))
				dirty |= 1;
			if (m_charrombank[1] != ((data >> 4) & 0x0f))
				dirty |= 2;

			if (dirty)
			{
				int i;

				m_charrombank[0] = data & 0x0f;
				m_charrombank[1] = (data >> 4) & 0x0f;

				for (i = 0; i < 0x1800; i++)
				{
					int bank = (m_ram[i]&0x0c) >> 2;
					if ((bank == 0 && (dirty & 1)) || (bank == 1 && (dirty & 2)))
					{
						m_tilemap[(i & 0x1800) >> 11]->mark_tile_dirty(i & 0x7ff);
					}
				}
			}
		}
		else if (offset == 0x1e00 || offset == 0x3e00) // Surprise Attack uses offset 0x3e00
		{
//logerror("%04x: 052109 register 1e00 = %02x\n",space.device().safe_pc(),data);
			m_romsubbank = data;
		}
		else if (offset == 0x1e80)
		{
//if ((data & 0xfe)) logerror("%04x: 052109 register 1e80 = %02x\n",space.device().safe_pc(),data);
			m_tilemap[0]->set_flip((data & 1) ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
			m_tilemap[1]->set_flip((data & 1) ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
			m_tilemap[2]->set_flip((data & 1) ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
			if (m_tileflip_enable != ((data & 0x06) >> 1))
			{
				m_tileflip_enable = ((data & 0x06) >> 1);

				m_tilemap[0]->mark_all_dirty();
				m_tilemap[1]->mark_all_dirty();
				m_tilemap[2]->mark_all_dirty();
			}
		}
		else if (offset == 0x1f00)
		{
			int dirty = 0;

			if (m_charrombank[2] != (data & 0x0f))
				dirty |= 1;

			if (m_charrombank[3] != ((data >> 4) & 0x0f))
				dirty |= 2;

			if (dirty)
			{
				int i;

				m_charrombank[2] = data & 0x0f;
				m_charrombank[3] = (data >> 4) & 0x0f;

				for (i = 0; i < 0x1800; i++)
				{
					int bank = (m_ram[i] & 0x0c) >> 2;
					if ((bank == 2 && (dirty & 1)) || (bank == 3 && (dirty & 2)))
						m_tilemap[(i & 0x1800) >> 11]->mark_tile_dirty(i & 0x7ff);
				}
			}
		}
		else if (offset >= 0x380c && offset < 0x3834)
		{   /* B y scroll */    }
		else if (offset >= 0x3a00 && offset < 0x3c00)
		{   /* B x scroll */    }
		else if (offset == 0x3d80) // Surprise Attack uses offset 0x3d80 in rom test
		{
			// mirroring this write, breaks Surprise Attack in game tilemaps
			m_charrombank_2[0] = data & 0x0f;
			m_charrombank_2[1] = (data >> 4) & 0x0f;
		}
		else if (offset == 0x3f00) // Surprise Attack uses offset 0x3f00 in rom test
		{
			// mirroring this write, breaks Surprise Attack in game tilemaps
			m_charrombank_2[2] = data & 0x0f;
			m_charrombank_2[3] = (data >> 4) & 0x0f;
		}
//      else
//          logerror("%04x: write %02x to unknown 052109 address %04x\n",space.device().safe_pc(),data,offset);
	}
}

READ16_MEMBER( k052109_device::word_r )
{
	return read(space, offset + 0x2000) | (read(space, offset) << 8);
}

WRITE16_MEMBER( k052109_device::word_w )
{
	if (ACCESSING_BITS_8_15)
		write(space, offset, (data >> 8) & 0xff);
	if (ACCESSING_BITS_0_7)
		write(space, offset + 0x2000, data & 0xff);
}

READ16_MEMBER( k052109_device::lsb_r )
{
	return read(space, offset);
}

WRITE16_MEMBER( k052109_device::lsb_w )
{
	if(ACCESSING_BITS_0_7)
		write(space, offset, data & 0xff);
}

void k052109_device::set_rmrd_line( int state )
{
	m_rmrd_line = state;
}

int k052109_device::get_rmrd_line( )
{
	return m_rmrd_line;
}


void k052109_device::tilemap_mark_dirty( int tmap_num )
{
	m_tilemap[tmap_num]->mark_all_dirty();
}


void k052109_device::tilemap_update( )
{
	int xscroll, yscroll, offs;

#if 0
{
popmessage("%x %x %x %x",
	m_charrombank[0],
	m_charrombank[1],
	m_charrombank[2],
	m_charrombank[3]);
}
#endif

	if ((m_scrollctrl & 0x03) == 0x02)
	{
		UINT8 *scrollram = &m_ram[0x1a00];

		m_tilemap[1]->set_scroll_rows(256);
		m_tilemap[1]->set_scroll_cols(1);
		yscroll = m_ram[0x180c];
		m_tilemap[1]->set_scrolly(0, yscroll + m_dy[1]);
		for (offs = 0; offs < 256; offs++)
		{
			xscroll = scrollram[2 * (offs & 0xfff8) + 0] + 256 * scrollram[2 * (offs & 0xfff8) + 1];
			xscroll -= 6;
			m_tilemap[1]->set_scrollx((offs + yscroll) & 0xff, xscroll + m_dx[1]);
		}
	}
	else if ((m_scrollctrl & 0x03) == 0x03)
	{
		UINT8 *scrollram = &m_ram[0x1a00];

		m_tilemap[1]->set_scroll_rows(256);
		m_tilemap[1]->set_scroll_cols(1);
		yscroll = m_ram[0x180c];
		m_tilemap[1]->set_scrolly(0, yscroll + m_dy[1]);
		for (offs = 0; offs < 256; offs++)
		{
			xscroll = scrollram[2 * offs + 0] + 256 * scrollram[2 * offs + 1];
			xscroll -= 6;
			m_tilemap[1]->set_scrollx((offs + yscroll) & 0xff, xscroll + m_dx[1]);
		}
	}
	else if ((m_scrollctrl & 0x04) == 0x04)
	{
		UINT8 *scrollram = &m_ram[0x1800];

		m_tilemap[1]->set_scroll_rows(1);
		m_tilemap[1]->set_scroll_cols(512);
		xscroll = m_ram[0x1a00] + 256 * m_ram[0x1a01];
		xscroll -= 6;
		m_tilemap[1]->set_scrollx(0, xscroll + m_dx[1]);
		for (offs = 0; offs < 512; offs++)
		{
			yscroll = scrollram[offs / 8];
			m_tilemap[1]->set_scrolly((offs + xscroll) & 0x1ff, yscroll + m_dy[1]);
		}
	}
	else
	{
		UINT8 *scrollram = &m_ram[0x1a00];

		m_tilemap[1]->set_scroll_rows(1);
		m_tilemap[1]->set_scroll_cols(1);
		xscroll = scrollram[0] + 256 * scrollram[1];
		xscroll -= 6;
		yscroll = m_ram[0x180c];
		m_tilemap[1]->set_scrollx(0, xscroll + m_dx[1]);
		m_tilemap[1]->set_scrolly(0, yscroll + m_dy[1]);
	}

	if ((m_scrollctrl & 0x18) == 0x10)
	{
		UINT8 *scrollram = &m_ram[0x3a00];

		m_tilemap[2]->set_scroll_rows(256);
		m_tilemap[2]->set_scroll_cols(1);
		yscroll = m_ram[0x380c];
		m_tilemap[2]->set_scrolly(0, yscroll + m_dy[2]);
		for (offs = 0; offs < 256; offs++)
		{
			xscroll = scrollram[2 * (offs & 0xfff8) + 0] + 256 * scrollram[2 * (offs & 0xfff8) + 1];
			xscroll -= 6;
			m_tilemap[2]->set_scrollx((offs + yscroll) & 0xff, xscroll + m_dx[2]);
		}
	}
	else if ((m_scrollctrl & 0x18) == 0x18)
	{
		UINT8 *scrollram = &m_ram[0x3a00];

		m_tilemap[2]->set_scroll_rows(256);
		m_tilemap[2]->set_scroll_cols(1);
		yscroll = m_ram[0x380c];
		m_tilemap[2]->set_scrolly(0, yscroll + m_dy[2]);
		for (offs = 0; offs < 256; offs++)
		{
			xscroll = scrollram[2 * offs + 0] + 256 * scrollram[2 * offs + 1];
			xscroll -= 6;
			m_tilemap[2]->set_scrollx((offs + yscroll) & 0xff, xscroll + m_dx[2]);
		}
	}
	else if ((m_scrollctrl & 0x20) == 0x20)
	{
		UINT8 *scrollram = &m_ram[0x3800];

		m_tilemap[2]->set_scroll_rows(1);
		m_tilemap[2]->set_scroll_cols(512);
		xscroll = m_ram[0x3a00] + 256 * m_ram[0x3a01];
		xscroll -= 6;
		m_tilemap[2]->set_scrollx(0, xscroll + m_dx[2]);
		for (offs = 0; offs < 512; offs++)
		{
			yscroll = scrollram[offs / 8];
			m_tilemap[2]->set_scrolly((offs + xscroll) & 0x1ff, yscroll + m_dy[2]);
		}
	}
	else
	{
		UINT8 *scrollram = &m_ram[0x3a00];

		m_tilemap[2]->set_scroll_rows(1);
		m_tilemap[2]->set_scroll_cols(1);
		xscroll = scrollram[0] + 256 * scrollram[1];
		xscroll -= 6;
		yscroll = m_ram[0x380c];
		m_tilemap[2]->set_scrollx(0, xscroll + m_dx[2]);
		m_tilemap[2]->set_scrolly(0, yscroll + m_dy[2]);
	}

#if 0
if ((m_scrollctrl & 0x03) == 0x01 ||
		(m_scrollctrl & 0x18) == 0x08 ||
		((m_scrollctrl & 0x04) && (m_scrollctrl & 0x03)) ||
		((m_scrollctrl & 0x20) && (m_scrollctrl & 0x18)) ||
		(m_scrollctrl & 0xc0) != 0)
	popmessage("scrollcontrol = %02x", m_scrollctrl);

if (machine().input().code_pressed(KEYCODE_F))
{
	FILE *fp;
	fp=fopen("TILE.DMP", "w+b");
	if (fp)
	{
		fwrite(m_ram, 0x6000, 1, fp);
		popmessage("saved");
		fclose(fp);
	}
}
#endif
}

void k052109_device::tilemap_draw( bitmap_ind16 &bitmap, const rectangle &cliprect, int tmap_num, UINT32 flags, UINT8 priority )
{
	m_tilemap[tmap_num]->draw(bitmap, cliprect, flags, priority);
}

int k052109_device::is_irq_enabled( )
{
	return m_irq_enabled;
}

void k052109_device::set_layer_offsets( int layer, int dx, int dy )
{
	m_dx[layer] = dx;
	m_dy[layer] = dy;
}


/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

/*
  data format:
  video RAM    xxxxxxxx  tile number (low 8 bits)
  color RAM    xxxx----  depends on external connections (usually color and banking)
  color RAM    ----xx--  bank select (0-3): these bits are replaced with the 2
                         bottom bits of the bank register before being placed on
                         the output pins. The other two bits of the bank register are
                         placed on the CAB1 and CAB2 output pins.
  color RAM    ------xx  depends on external connections (usually banking, flip)
*/

void k052109_device::get_tile_info( tile_data &tileinfo, int tile_index, int layer, UINT8 *cram, UINT8 *vram1, UINT8 *vram2 )
{
	int flipy = 0;
	int code = vram1[tile_index] + 256 * vram2[tile_index];
	int color = cram[tile_index];
	int flags = 0;
	int priority = 0;
	int bank = m_charrombank[(color & 0x0c) >> 2];
	if (m_has_extra_video_ram)
		bank = (color & 0x0c) >> 2; /* kludge for X-Men */

	color = (color & 0xf3) | ((bank & 0x03) << 2);
	bank >>= 2;

	flipy = color & 0x02;

	m_callback(machine(), layer, bank, &code, &color, &flags, &priority);

	/* if the callback set flip X but it is not enabled, turn it off */
	if (!(m_tileflip_enable & 1))
		flags &= ~TILE_FLIPX;

	/* if flip Y is enabled and the attribute but is set, turn it on */
	if (flipy && (m_tileflip_enable & 2))
		flags |= TILE_FLIPY;

	SET_TILE_INFO_MEMBER(
			m_gfx_num,
			code,
			color,
			flags);

	tileinfo.category = priority;
}

TILE_GET_INFO_MEMBER(k052109_device::get_tile_info0)
{
	get_tile_info(tileinfo, tile_index, 0, m_colorram_F, m_videoram_F, m_videoram2_F);
}

TILE_GET_INFO_MEMBER(k052109_device::get_tile_info1)
{
	get_tile_info(tileinfo, tile_index, 1, m_colorram_A, m_videoram_A, m_videoram2_A);
}

TILE_GET_INFO_MEMBER(k052109_device::get_tile_info2)
{
	get_tile_info(tileinfo, tile_index, 2, m_colorram_B, m_videoram_B, m_videoram2_B);
}


void k052109_device::tileflip_reset()
{
	int data = m_ram[0x1e80];
	m_tilemap[0]->set_flip((data & 1) ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
	m_tilemap[1]->set_flip((data & 1) ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
	m_tilemap[2]->set_flip((data & 1) ? (TILEMAP_FLIPY | TILEMAP_FLIPX) : 0);
	m_tileflip_enable = ((data & 0x06) >> 1);
}


/***************************************************************************/
/*                                                                         */
/*                                 051960                                  */
/*                                                                         */
/***************************************************************************/

const device_type K051960 = &device_creator<k051960_device>;

k051960_device::k051960_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K051960, "Konami 051960", tag, owner, clock, "k051960", __FILE__),
	m_ram(NULL),
	m_gfx(NULL),
	//m_spriterombank[3],
	m_dx(0),
	m_dy(0),
	m_romoffset(0),
	m_spriteflip(0),
	m_readroms(0),
	m_irq_enabled(0),
	m_nmi_enabled(0),
	m_k051937_counter(0)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k051960_device::device_config_complete()
{
	// inherit a copy of the static data
	const k051960_interface *intf = reinterpret_cast<const k051960_interface *>(static_config());
	if (intf != NULL)
	*static_cast<k051960_interface *>(this) = *intf;
	
	// or initialize to defaults if none provided
	else
	{
		m_gfx_memory_region = "";
		m_gfx_num = 0;
		m_plane_order = 0;
		m_deinterleave = 0;
		m_callback = NULL;
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k051960_device::device_start()
{
	UINT32 total;
	static const gfx_layout spritelayout =
	{
		16,16,
		0,
		4,
		{ 0, 8, 16, 24 },
		{ 0, 1, 2, 3, 4, 5, 6, 7,
				8*32+0, 8*32+1, 8*32+2, 8*32+3, 8*32+4, 8*32+5, 8*32+6, 8*32+7 },
		{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
				16*32, 17*32, 18*32, 19*32, 20*32, 21*32, 22*32, 23*32 },
		128*8
	};
	static const gfx_layout spritelayout_reverse =
	{
		16,16,
		0,
		4,
		{ 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7,
				8*32+0, 8*32+1, 8*32+2, 8*32+3, 8*32+4, 8*32+5, 8*32+6, 8*32+7 },
		{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
				16*32, 17*32, 18*32, 19*32, 20*32, 21*32, 22*32, 23*32 },
		128*8
	};
	static const gfx_layout spritelayout_gradius3 =
	{
		16,16,
		0,
		4,
		{ 0, 1, 2, 3 },
		{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4,
			32*8+2*4, 32*8+3*4, 32*8+0*4, 32*8+1*4, 32*8+6*4, 32*8+7*4, 32*8+4*4, 32*8+5*4 },
		{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			64*8+0*32, 64*8+1*32, 64*8+2*32, 64*8+3*32, 64*8+4*32, 64*8+5*32, 64*8+6*32, 64*8+7*32 },
		128*8
	};

	/* decode the graphics */
	switch (m_plane_order)
	{
	case NORMAL_PLANE_ORDER:
		total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / 128;
		decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &spritelayout, 4);
		break;

	case REVERSE_PLANE_ORDER:
		total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / 128;
		decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &spritelayout_reverse, 4);
		break;

	case GRADIUS3_PLANE_ORDER:
		total = 0x4000;
		decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &spritelayout_gradius3, 4);
		break;

	default:
		fatalerror("Unknown plane_order\n");
	}

	if (VERBOSE && !(machine().config().m_video_attributes & VIDEO_HAS_SHADOWS))
		popmessage("driver should use VIDEO_HAS_SHADOWS");

	/* deinterleave the graphics, if needed */
	deinterleave_gfx(machine(), m_gfx_memory_region, m_deinterleave);

	m_gfx = machine().gfx[m_gfx_num];
	m_ram = auto_alloc_array_clear(machine(), UINT8, 0x400);

	save_item(NAME(m_romoffset));
	save_item(NAME(m_spriteflip));
	save_item(NAME(m_readroms));
	save_item(NAME(m_spriterombank));
	save_pointer(NAME(m_ram), 0x400);
	save_item(NAME(m_irq_enabled));
	save_item(NAME(m_nmi_enabled));
	save_item(NAME(m_dx));
	save_item(NAME(m_dy));

	save_item(NAME(m_k051937_counter));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k051960_device::device_reset()
{
	
	m_dx = m_dy = 0;
	m_k051937_counter = 0;

	m_romoffset = 0;
	m_spriteflip = 0;
	m_readroms = 0;
	m_irq_enabled = 0;
	m_nmi_enabled = 0;

	m_spriterombank[0] = 0;
	m_spriterombank[1] = 0;
	m_spriterombank[2] = 0;
}


/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

int k051960_device::k051960_fetchromdata( int byte )
{
	int code, color, pri, shadow, off1, addr;

	addr = m_romoffset + (m_spriterombank[0] << 8) + ((m_spriterombank[1] & 0x03) << 16);
	code = (addr & 0x3ffe0) >> 5;
	off1 = addr & 0x1f;
	color = ((m_spriterombank[1] & 0xfc) >> 2) + ((m_spriterombank[2] & 0x03) << 6);
	pri = 0;
	shadow = color & 0x80;
	m_callback(machine(), &code, &color, &pri, &shadow);

	addr = (code << 7) | (off1 << 2) | byte;
	addr &= machine().root_device().memregion(m_gfx_memory_region)->bytes() - 1;

//  popmessage("%s: addr %06x", machine().describe_context(), addr);

	return machine().root_device().memregion(m_gfx_memory_region)->base()[addr];
}

READ8_MEMBER( k051960_device::k051960_r )
{
	if (m_readroms)
	{
		/* the 051960 remembers the last address read and uses it when reading the sprite ROMs */
		m_romoffset = (offset & 0x3fc) >> 2;
		return k051960_fetchromdata(offset & 3);    /* only 88 Games reads the ROMs from here */
	}
	else
		return m_ram[offset];
}

WRITE8_MEMBER( k051960_device::k051960_w )
{
	m_ram[offset] = data;
}

READ16_MEMBER( k051960_device::k051960_word_r )
{
	return k051960_r(space, offset * 2 + 1) | (k051960_r(space, offset * 2) << 8);
}

WRITE16_MEMBER( k051960_device::k051960_word_w )
{
	if (ACCESSING_BITS_8_15)
		k051960_w(space, offset * 2, (data >> 8) & 0xff);
	if (ACCESSING_BITS_0_7)
		k051960_w(space, offset * 2 + 1, data & 0xff);
}


/* should this be split by k051960? */
READ8_MEMBER( k051960_device::k051937_r )
{
	if (m_readroms && offset >= 4 && offset < 8)
		return k051960_fetchromdata(offset & 3);
	else
	{
		if (offset == 0)
		{
			/* some games need bit 0 to pulse */
			return (m_k051937_counter++) & 1;
		}
		//logerror("%04x: read unknown 051937 address %x\n", device->cpu->safe_pc(), offset);
		return 0;
	}

	return 0;
}

WRITE8_MEMBER( k051960_device::k051937_w )
{
	if (offset == 0)
	{
		//if (data & 0xc2) popmessage("051937 reg 00 = %02x",data);

		/* bit 0 is IRQ enable */
		m_irq_enabled = data & 0x01;

		/* bit 1: probably FIRQ enable */

		/* bit 2 is NMI enable */
		m_nmi_enabled = data & 0x04;

		/* bit 3 = flip screen */
		m_spriteflip = data & 0x08;

		/* bit 4 used by Devastators and TMNT, unknown */

		/* bit 5 = enable gfx ROM reading */
		m_readroms = data & 0x20;
		//logerror("%04x: write %02x to 051937 address %x\n", machine().cpu->safe_pc(), data, offset);
	}
	else if (offset == 1)
	{
//  popmessage("%04x: write %02x to 051937 address %x", machine().cpu->safe_pc(), data, offset);
//logerror("%04x: write %02x to unknown 051937 address %x\n", machine().cpu->safe_pc(), data, offset);
	}
	else if (offset >= 2 && offset < 5)
	{
		m_spriterombank[offset - 2] = data;
	}
	else
	{
	//  popmessage("%04x: write %02x to 051937 address %x", machine().cpu->safe_pc(), data, offset);
	//logerror("%04x: write %02x to unknown 051937 address %x\n", machine().cpu->safe_pc(), data, offset);
	}
}

int k051960_device::k051960_is_irq_enabled( )
{
	return m_irq_enabled;
}

int k051960_device::k051960_is_nmi_enabled( )
{
	return m_nmi_enabled;
}

void k051960_device::k051960_set_sprite_offsets( int dx, int dy )
{
	m_dx = dx;
	m_dy = dy;
}


READ16_MEMBER( k051960_device::k051937_word_r )
{
	return k051937_r(space, offset * 2 + 1) | (k051937_r(space, offset * 2) << 8);
}

WRITE16_MEMBER( k051960_device::k051937_word_w )
{
	if (ACCESSING_BITS_8_15)
		k051937_w(space, offset * 2,(data >> 8) & 0xff);
	if (ACCESSING_BITS_0_7)
		k051937_w(space, offset * 2 + 1,data & 0xff);
}

/*
 * Sprite Format
 * ------------------
 *
 * Byte | Bit(s)   | Use
 * -----+-76543210-+----------------
 *   0  | x------- | active (show this sprite)
 *   0  | -xxxxxxx | priority order
 *   1  | xxx----- | sprite size (see below)
 *   1  | ---xxxxx | sprite code (high 5 bits)
 *   2  | xxxxxxxx | sprite code (low 8 bits)
 *   3  | xxxxxxxx | "color", but depends on external connections (see below)
 *   4  | xxxxxx-- | zoom y (0 = normal, >0 = shrink)
 *   4  | ------x- | flip y
 *   4  | -------x | y position (high bit)
 *   5  | xxxxxxxx | y position (low 8 bits)
 *   6  | xxxxxx-- | zoom x (0 = normal, >0 = shrink)
 *   6  | ------x- | flip x
 *   6  | -------x | x position (high bit)
 *   7  | xxxxxxxx | x position (low 8 bits)
 *
 * Example of "color" field for Punk Shot:
 *   3  | x------- | shadow
 *   3  | -xx----- | priority
 *   3  | ---x---- | use second gfx ROM bank
 *   3  | ----xxxx | color code
 *
 * shadow enables transparent shadows. Note that it applies to pen 0x0f ONLY.
 * The rest of the sprite remains normal.
 * Note that Aliens also uses the shadow bit to select the second sprite bank.
 */

void k051960_device::k051960_sprites_draw( bitmap_ind16 &bitmap, const rectangle &cliprect, int min_priority, int max_priority )
{
#define NUM_SPRITES 128
	int offs, pri_code;
	int sortedlist[NUM_SPRITES];
	UINT8 drawmode_table[256];

	memset(drawmode_table, DRAWMODE_SOURCE, sizeof(drawmode_table));
	drawmode_table[0] = DRAWMODE_NONE;

	for (offs = 0; offs < NUM_SPRITES; offs++)
		sortedlist[offs] = -1;

	/* prebuild a sorted table */
	for (offs = 0; offs < 0x400; offs += 8)
	{
		if (m_ram[offs] & 0x80)
		{
			if (max_priority == -1) /* draw front to back when using priority buffer */
				sortedlist[(m_ram[offs] & 0x7f) ^ 0x7f] = offs;
			else
				sortedlist[m_ram[offs] & 0x7f] = offs;
		}
	}

	for (pri_code = 0; pri_code < NUM_SPRITES; pri_code++)
	{
		int ox, oy, code, color, pri, shadow, size, w, h, x, y, flipx, flipy, zoomx, zoomy;
		/* sprites can be grouped up to 8x8. The draw order is
		     0  1  4  5 16 17 20 21
		     2  3  6  7 18 19 22 23
		     8  9 12 13 24 25 28 29
		    10 11 14 15 26 27 30 31
		    32 33 36 37 48 49 52 53
		    34 35 38 39 50 51 54 55
		    40 41 44 45 56 57 60 61
		    42 43 46 47 58 59 62 63
		*/
		static const int xoffset[8] = { 0, 1, 4, 5, 16, 17, 20, 21 };
		static const int yoffset[8] = { 0, 2, 8, 10, 32, 34, 40, 42 };
		static const int width[8] =  { 1, 2, 1, 2, 4, 2, 4, 8 };
		static const int height[8] = { 1, 1, 2, 2, 2, 4, 4, 8 };

		offs = sortedlist[pri_code];
		if (offs == -1)
			continue;

		code = m_ram[offs + 2] + ((m_ram[offs + 1] & 0x1f) << 8);
		color = m_ram[offs + 3] & 0xff;
		pri = 0;
		shadow = color & 0x80;
		m_callback(machine(), &code, &color, &pri, &shadow);

		if (max_priority != -1)
			if (pri < min_priority || pri > max_priority)
				continue;

		size = (m_ram[offs + 1] & 0xe0) >> 5;
		w = width[size];
		h = height[size];

		if (w >= 2) code &= ~0x01;
		if (h >= 2) code &= ~0x02;
		if (w >= 4) code &= ~0x04;
		if (h >= 4) code &= ~0x08;
		if (w >= 8) code &= ~0x10;
		if (h >= 8) code &= ~0x20;

		ox = (256 * m_ram[offs + 6] + m_ram[offs + 7]) & 0x01ff;
		oy = 256 - ((256 * m_ram[offs + 4] + m_ram[offs + 5]) & 0x01ff);
		ox += m_dx;
		oy += m_dy;
		flipx = m_ram[offs + 6] & 0x02;
		flipy = m_ram[offs + 4] & 0x02;
		zoomx = (m_ram[offs + 6] & 0xfc) >> 2;
		zoomy = (m_ram[offs + 4] & 0xfc) >> 2;
		zoomx = 0x10000 / 128 * (128 - zoomx);
		zoomy = 0x10000 / 128 * (128 - zoomy);

		if (m_spriteflip)
		{
			ox = 512 - (zoomx * w >> 12) - ox;
			oy = 256 - (zoomy * h >> 12) - oy;
			flipx = !flipx;
			flipy = !flipy;
		}

		drawmode_table[m_gfx->granularity() - 1] = shadow ? DRAWMODE_SHADOW : DRAWMODE_SOURCE;

		if (zoomx == 0x10000 && zoomy == 0x10000)
		{
			int sx, sy;

			for (y = 0; y < h; y++)
			{
				sy = oy + 16 * y;

				for (x = 0; x < w; x++)
				{
					int c = code;

					sx = ox + 16 * x;
					if (flipx)
						c += xoffset[(w - 1 - x)];
					else
						c += xoffset[x];

					if (flipy)
						c += yoffset[(h - 1 - y)];
					else
						c += yoffset[y];

					if (max_priority == -1)
						pdrawgfx_transtable(bitmap,cliprect,m_gfx,
								c,
								color,
								flipx,flipy,
								sx & 0x1ff,sy,
								machine().priority_bitmap,pri,
								drawmode_table,machine().shadow_table);
					else
						drawgfx_transtable(bitmap,cliprect,m_gfx,
								c,
								color,
								flipx,flipy,
								sx & 0x1ff,sy,
								drawmode_table,machine().shadow_table);
				}
			}
		}
		else
		{
			int sx, sy, zw, zh;

			for (y = 0; y < h; y++)
			{
				sy = oy + ((zoomy * y + (1 << 11)) >> 12);
				zh = (oy + ((zoomy * (y + 1) + (1 << 11)) >> 12)) - sy;

				for (x = 0; x < w; x++)
				{
					int c = code;

					sx = ox + ((zoomx * x + (1 << 11)) >> 12);
					zw = (ox + ((zoomx * (x+1) + (1 << 11)) >> 12)) - sx;
					if (flipx)
						c += xoffset[(w - 1 - x)];
					else
						c += xoffset[x];

					if (flipy)
						c += yoffset[(h - 1 - y)];
					else
						c += yoffset[y];

					if (max_priority == -1)
						pdrawgfxzoom_transtable(bitmap,cliprect,m_gfx,
								c,
								color,
								flipx,flipy,
								sx & 0x1ff,sy,
								(zw << 16) / 16,(zh << 16) / 16,
								machine().priority_bitmap,pri,
								drawmode_table,machine().shadow_table);
					else
						drawgfxzoom_transtable(bitmap,cliprect,m_gfx,
								c,
								color,
								flipx,flipy,
								sx & 0x1ff,sy,
								(zw << 16) / 16,(zh << 16) / 16,
								drawmode_table,machine().shadow_table);
				}
			}
		}
	}
#if 0
if (machine().input().code_pressed(KEYCODE_D))
{
	FILE *fp;
	fp=fopen("SPRITE.DMP", "w+b");
	if (fp)
	{
		fwrite(k051960_ram, 0x400, 1, fp);
		popmessage("saved");
		fclose(fp);
	}
}
#endif
#undef NUM_SPRITES
}


/***************************************************************************/
/*                                                                         */
/*                      05324x Family Sprite Generators                    */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/*                         053244 / 053245                                 */
/*                                                                         */
/***************************************************************************/


/*****************************************************************************
    DEVICE INTERFACE
*****************************************************************************/

const device_type K053244 = &device_creator<k05324x_device>;

k05324x_device::k05324x_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K053244, "Konami 053244 & 053245", tag, owner, clock, "k05324x", __FILE__),
	m_ram(NULL),
	m_buffer(NULL),
	m_gfx(NULL),
	//m_regs[0x10],
	m_rombank(0),
	m_ramsize(0),
	m_z_rejection(0)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k05324x_device::device_config_complete()
{
	// inherit a copy of the static data
	const k05324x_interface *intf = reinterpret_cast<const k05324x_interface *>(static_config());
	if (intf != NULL)
	*static_cast<k05324x_interface *>(this) = *intf;
	
	// or initialize to defaults if none provided
	else
	{
		m_gfx_memory_region = "";
		m_gfx_num = 0;
		m_plane_order = 0;
		m_dx = 0;
		m_dy = 0;
		m_deinterleave = 0;
		m_callback = NULL;
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k05324x_device::device_start()
{
	UINT32 total;
	static const gfx_layout spritelayout =
	{
		16,16,
		0,
		4,
		{ 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7,
				8*32+0, 8*32+1, 8*32+2, 8*32+3, 8*32+4, 8*32+5, 8*32+6, 8*32+7 },
		{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
				16*32, 17*32, 18*32, 19*32, 20*32, 21*32, 22*32, 23*32 },
		128*8
	};

	/* decode the graphics */
	switch (m_plane_order)
	{
	case NORMAL_PLANE_ORDER:
		total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / 128;
		decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &spritelayout, 4);
		break;

	default:
		fatalerror("Unsupported plane_order\n");
	}

	if (VERBOSE && !(machine().config().m_video_attributes & VIDEO_HAS_SHADOWS))
		popmessage("driver should use VIDEO_HAS_SHADOWS");

	/* deinterleave the graphics, if needed */
	deinterleave_gfx(machine(), m_gfx_memory_region, m_deinterleave);

	m_ramsize = 0x800;

	m_z_rejection = -1;
	m_gfx = machine().gfx[m_gfx_num];
	m_ram = auto_alloc_array_clear(machine(), UINT16, m_ramsize / 2);

	m_buffer = auto_alloc_array_clear(machine(), UINT16, m_ramsize / 2);

	save_pointer(NAME(m_ram), m_ramsize / 2);
	save_pointer(NAME(m_buffer), m_ramsize / 2);
	save_item(NAME(m_rombank));
	save_item(NAME(m_z_rejection));
	save_item(NAME(m_regs));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k05324x_device::device_reset()
{
	int i;

	m_rombank = 0;

	for (i = 0; i < 0x10; i++)
		m_regs[i] = 0;
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

void k05324x_device::k053245_set_sprite_offs( int offsx, int offsy )
{
	m_dx = offsx;
	m_dy = offsy;
}

READ16_MEMBER( k05324x_device::k053245_word_r )
{
	return m_ram[offset];
}

WRITE16_MEMBER( k05324x_device::k053245_word_w )
{
	COMBINE_DATA(m_ram + offset);
}

READ8_MEMBER( k05324x_device::k053245_r )
{
	if(offset & 1)
		return m_ram[offset >> 1] & 0xff;
	else
		return (m_ram[offset >> 1] >> 8) & 0xff;
}


WRITE8_MEMBER( k05324x_device::k053245_w )
{
	if(offset & 1)
		m_ram[offset >> 1] = (m_ram[offset >> 1] & 0xff00) | data;
	else
		m_ram[offset >> 1] = (m_ram[offset >> 1] & 0x00ff) | (data << 8);
}

void k05324x_device::k053245_clear_buffer( )
{
	int i, e;

	for (e = m_ramsize / 2, i = 0; i < e; i += 8)
		m_buffer[i] = 0;
}

void k05324x_device::k053245_update_buffer( )
{
	memcpy(m_buffer, m_ram, m_ramsize);
}

READ8_MEMBER( k05324x_device::k053244_r )
{
	if ((m_regs[5] & 0x10) && offset >= 0x0c && offset < 0x10)
	{
		int addr;

		addr = (m_rombank << 19) | ((m_regs[11] & 0x7) << 18)
			| (m_regs[8] << 10) | (m_regs[9] << 2)
			| ((offset & 3) ^ 1);
		addr &= machine().root_device().memregion(m_gfx_memory_region)->bytes() - 1;

		//  popmessage("%s: offset %02x addr %06x", machine().describe_context(), offset & 3, addr);

		return machine().root_device().memregion(m_gfx_memory_region)->base()[addr];
	}
	else if (offset == 0x06)
	{
		k053245_update_buffer();
		return 0;
	}
	else
	{
		//logerror("%s: read from unknown 053244 address %x\n", machine().describe_context(), offset);
		return 0;
	}
}

WRITE8_MEMBER( k05324x_device::k053244_w )
{
	m_regs[offset] = data;

	switch(offset)
	{
	case 0x05:
//      if (data & 0xc8)
//          popmessage("053244 reg 05 = %02x",data);
		/* bit 2 = unknown, Parodius uses it */
		/* bit 5 = unknown, Rollergames uses it */
//      logerror("%s: write %02x to 053244 address 5\n", space.machine().describe_context(), data);
		break;

	case 0x06:
		k053245_update_buffer();
		break;
	}
}


READ16_MEMBER( k05324x_device::k053244_lsb_r )
{
	return k053244_r(space, offset);
}

WRITE16_MEMBER( k05324x_device::k053244_lsb_w )
{
	if (ACCESSING_BITS_0_7)
		k053244_w(space, offset, data & 0xff);
}

READ16_MEMBER( k05324x_device::k053244_word_r )
{
	return (k053244_r(space, offset * 2) << 8) | k053244_r(space, offset * 2 + 1);
}

WRITE16_MEMBER( k05324x_device::k053244_word_w )
{
	if (ACCESSING_BITS_8_15)
		k053244_w(space, offset * 2, (data >> 8) & 0xff);
	if (ACCESSING_BITS_0_7)
		k053244_w(space, offset * 2 + 1, data & 0xff);
}

void k05324x_device::k053244_bankselect( int bank )
{
	m_rombank = bank;
}

void k05324x_device::k05324x_set_z_rejection( int zcode )
{
	m_z_rejection = zcode;
}

/*
 * Sprite Format
 * ------------------
 *
 * Word | Bit(s)           | Use
 * -----+-fedcba9876543210-+----------------
 *   0  | x--------------- | active (show this sprite)
 *   0  | -x-------------- | maintain aspect ratio (when set, zoom y acts on both axis)
 *   0  | --x------------- | flip y
 *   0  | ---x------------ | flip x
 *   0  | ----xxxx-------- | sprite size (see below)
 *   0  | ---------xxxxxxx | priority order
 *   1  | --xxxxxxxxxxxxxx | sprite code. We use an additional bit in TMNT2, but this is
 *                           probably not accurate (protection related so we can't verify)
 *   2  | ------xxxxxxxxxx | y position
 *   3  | ------xxxxxxxxxx | x position
 *   4  | xxxxxxxxxxxxxxxx | zoom y (0x40 = normal, <0x40 = enlarge, >0x40 = reduce)
 *   5  | xxxxxxxxxxxxxxxx | zoom x (0x40 = normal, <0x40 = enlarge, >0x40 = reduce)
 *   6  | ------x--------- | mirror y (top half is drawn as mirror image of the bottom)
 *   6  | -------x-------- | mirror x (right half is drawn as mirror image of the left)
 *   6  | --------x------- | shadow
 *   6  | ---------xxxxxxx | "color", but depends on external connections
 *   7  | ---------------- |
 *
 * shadow enables transparent shadows. Note that it applies to pen 0x0f ONLY.
 * The rest of the sprite remains normal.
 */

void k05324x_device::k053245_sprites_draw( bitmap_ind16 &bitmap, const rectangle &cliprect )
{
#define NUM_SPRITES 128
	int offs, pri_code, i;
	int sortedlist[NUM_SPRITES];
	int flipscreenX, flipscreenY, spriteoffsX, spriteoffsY;
	UINT8 drawmode_table[256];

	memset(drawmode_table, DRAWMODE_SOURCE, sizeof(drawmode_table));
	drawmode_table[0] = DRAWMODE_NONE;

	flipscreenX = m_regs[5] & 0x01;
	flipscreenY = m_regs[5] & 0x02;
	spriteoffsX = (m_regs[0] << 8) | m_regs[1];
	spriteoffsY = (m_regs[2] << 8) | m_regs[3];

	for (offs = 0; offs < NUM_SPRITES; offs++)
		sortedlist[offs] = -1;

	/* prebuild a sorted table */
	for (i = m_ramsize / 2, offs = 0; offs < i; offs += 8)
	{
		pri_code = m_buffer[offs];
		if (pri_code & 0x8000)
		{
			pri_code &= 0x007f;

			if (offs && pri_code == m_z_rejection)
				continue;

			if (sortedlist[pri_code] == -1)
				sortedlist[pri_code] = offs;
		}
	}

	for (pri_code = NUM_SPRITES - 1; pri_code >= 0; pri_code--)
	{
		int ox, oy, color, code, size, w, h, x, y, flipx, flipy, mirrorx, mirrory, shadow, zoomx, zoomy, pri;

		offs = sortedlist[pri_code];
		if (offs == -1)
			continue;

		/* the following changes the sprite draw order from
		     0  1  4  5 16 17 20 21
		     2  3  6  7 18 19 22 23
		     8  9 12 13 24 25 28 29
		    10 11 14 15 26 27 30 31
		    32 33 36 37 48 49 52 53
		    34 35 38 39 50 51 54 55
		    40 41 44 45 56 57 60 61
		    42 43 46 47 58 59 62 63

		    to

		     0  1  2  3  4  5  6  7
		     8  9 10 11 12 13 14 15
		    16 17 18 19 20 21 22 23
		    24 25 26 27 28 29 30 31
		    32 33 34 35 36 37 38 39
		    40 41 42 43 44 45 46 47
		    48 49 50 51 52 53 54 55
		    56 57 58 59 60 61 62 63
		*/

		/* NOTE: from the schematics, it looks like the top 2 bits should be ignored */
		/* (there are not output pins for them), and probably taken from the "color" */
		/* field to do bank switching. However this applies only to TMNT2, with its */
		/* protection mcu creating the sprite table, so we don't know where to fetch */
		/* the bits from. */
		code = m_buffer[offs + 1];
		code = ((code & 0xffe1) + ((code & 0x0010) >> 2) + ((code & 0x0008) << 1)
					+ ((code & 0x0004) >> 1) + ((code & 0x0002) << 2));
		color = m_buffer[offs + 6] & 0x00ff;
		pri = 0;

		m_callback(machine(), &code, &color, &pri);

		size = (m_buffer[offs] & 0x0f00) >> 8;

		w = 1 << (size & 0x03);
		h = 1 << ((size >> 2) & 0x03);

		/* zoom control:
		   0x40 = normal scale
		  <0x40 enlarge (0x20 = double size)
		  >0x40 reduce (0x80 = half size)
		*/
		zoomy = m_buffer[offs + 4];
		if (zoomy > 0x2000)
			continue;

		if (zoomy)
			zoomy = (0x400000 + zoomy / 2) / zoomy;
		else
			zoomy = 2 * 0x400000;
		if ((m_buffer[offs] & 0x4000) == 0)
		{
			zoomx = m_buffer[offs + 5];
			if (zoomx > 0x2000)
				continue;
			if (zoomx)
				zoomx = (0x400000 + zoomx / 2) / zoomx;
			else
				zoomx = 2 * 0x400000;
//          else zoomx = zoomy; /* workaround for TMNT2 */
		}
		else
			zoomx = zoomy;

		ox = m_buffer[offs+3] + spriteoffsX;
		oy = m_buffer[offs+2];

		ox += m_dx;
		oy += m_dy;

		flipx = m_buffer[offs] & 0x1000;
		flipy = m_buffer[offs] & 0x2000;
		mirrorx = m_buffer[offs + 6] & 0x0100;
		if (mirrorx)
			flipx = 0; // documented and confirmed

		mirrory = m_buffer[offs + 6] & 0x0200;
		shadow = m_buffer[offs + 6] & 0x0080;

		if (flipscreenX)
		{
			ox = 512 - ox;
			if (!mirrorx)
				flipx = !flipx;
		}
		if (flipscreenY)
		{
			oy = -oy;
			if (!mirrory)
				flipy = !flipy;
		}

		ox = (ox + 0x5d) & 0x3ff;
		if (ox >= 768) ox -= 1024;
		oy = (-(oy + spriteoffsY + 0x07)) & 0x3ff;
		if (oy >= 640) oy -= 1024;

		/* the coordinates given are for the *center* of the sprite */
		ox -= (zoomx * w) >> 13;
		oy -= (zoomy * h) >> 13;

		drawmode_table[m_gfx->granularity() - 1] = shadow ? DRAWMODE_SHADOW : DRAWMODE_SOURCE;

		for (y = 0; y < h; y++)
		{
			int sx, sy, zw, zh;

			sy = oy + ((zoomy * y + (1 << 11)) >> 12);
			zh = (oy + ((zoomy * (y + 1) + (1 << 11)) >> 12)) - sy;

			for (x = 0; x < w; x++)
			{
				int c, fx, fy;

				sx = ox + ((zoomx * x + (1 << 11)) >> 12);
				zw = (ox + ((zoomx * (x+1) + (1 << 11)) >> 12)) - sx;
				c = code;
				if (mirrorx)
				{
					if ((flipx == 0) ^ (2*x < w))
					{
						/* mirror left/right */
						c += (w - x - 1);
						fx = 1;
					}
					else
					{
						c += x;
						fx = 0;
					}
				}
				else
				{
					if (flipx) c += w-1-x;
					else c += x;
					fx = flipx;
				}
				if (mirrory)
				{
					if ((flipy == 0) ^ (2*y >= h))
					{
						/* mirror top/bottom */
						c += 8 * (h - y - 1);
						fy = 1;
					}
					else
					{
						c += 8 * y;
						fy = 0;
					}
				}
				else
				{
					if (flipy) c += 8 * (h - 1 - y);
					else c += 8 * y;
					fy = flipy;
				}

				/* the sprite can start at any point in the 8x8 grid, but it must stay */
				/* in a 64 entries window, wrapping around at the edges. The animation */
				/* at the end of the saloon level in Sunset Riders breaks otherwise. */
				c = (c & 0x3f) | (code & ~0x3f);

				if (zoomx == 0x10000 && zoomy == 0x10000)
				{
					pdrawgfx_transtable(bitmap,cliprect,m_gfx,
							c,
							color,
							fx,fy,
							sx,sy,
							machine().priority_bitmap,pri,
							drawmode_table,machine().shadow_table);
				}
				else
				{
					pdrawgfxzoom_transtable(bitmap,cliprect,m_gfx,
							c,
							color,
							fx,fy,
							sx,sy,
							(zw << 16) / 16,(zh << 16) / 16,
							machine().priority_bitmap,pri,
							drawmode_table,machine().shadow_table);

				}
			}
		}
	}
#if 0
if (machine().input().code_pressed(KEYCODE_D))
{
	FILE *fp;
	fp=fopen("SPRITE.DMP", "w+b");
	if (fp)
	{
		fwrite(m_buffer, 0x800, 1, fp);
		popmessage("saved");
		fclose(fp);
	}
}
#endif
#undef NUM_SPRITES
}

/* Lethal Enforcers has 2 of these chips hooked up in parallel to give 6bpp gfx.. let's cheat a
  bit and make emulating it a little less messy by using a custom function instead */
void k05324x_device::k053245_sprites_draw_lethal( bitmap_ind16 &bitmap, const rectangle &cliprect )
{
#define NUM_SPRITES 128
	int offs, pri_code, i;
	int sortedlist[NUM_SPRITES];
	int flipscreenX, flipscreenY, spriteoffsX, spriteoffsY;
	UINT8 drawmode_table[256];
	
	memset(drawmode_table, DRAWMODE_SOURCE, sizeof(drawmode_table));
	drawmode_table[0] = DRAWMODE_NONE;

	flipscreenX = m_regs[5] & 0x01;
	flipscreenY = m_regs[5] & 0x02;
	spriteoffsX = (m_regs[0] << 8) | m_regs[1];
	spriteoffsY = (m_regs[2] << 8) | m_regs[3];

	for (offs = 0; offs < NUM_SPRITES; offs++)
		sortedlist[offs] = -1;

	/* prebuild a sorted table */
	for (i = m_ramsize / 2, offs = 0; offs < i; offs += 8)
	{
		pri_code = m_buffer[offs];
		if (pri_code & 0x8000)
		{
			pri_code &= 0x007f;

			if (offs && pri_code == m_z_rejection)
				continue;

			if (sortedlist[pri_code] == -1)
				sortedlist[pri_code] = offs;
		}
	}

	for (pri_code = NUM_SPRITES - 1; pri_code >= 0; pri_code--)
	{
		int ox, oy, color, code, size, w, h, x, y, flipx, flipy, mirrorx, mirrory, shadow, zoomx, zoomy, pri;

		offs = sortedlist[pri_code];
		if (offs == -1)
			continue;

		/* the following changes the sprite draw order from
		     0  1  4  5 16 17 20 21
		     2  3  6  7 18 19 22 23
		     8  9 12 13 24 25 28 29
		    10 11 14 15 26 27 30 31
		    32 33 36 37 48 49 52 53
		    34 35 38 39 50 51 54 55
		    40 41 44 45 56 57 60 61
		    42 43 46 47 58 59 62 63

		    to

		     0  1  2  3  4  5  6  7
		     8  9 10 11 12 13 14 15
		    16 17 18 19 20 21 22 23
		    24 25 26 27 28 29 30 31
		    32 33 34 35 36 37 38 39
		    40 41 42 43 44 45 46 47
		    48 49 50 51 52 53 54 55
		    56 57 58 59 60 61 62 63
		*/

		/* NOTE: from the schematics, it looks like the top 2 bits should be ignored */
		/* (there are not output pins for them), and probably taken from the "color" */
		/* field to do bank switching. However this applies only to TMNT2, with its */
		/* protection mcu creating the sprite table, so we don't know where to fetch */
		/* the bits from. */
		code = m_buffer[offs + 1];
		code = ((code & 0xffe1) + ((code & 0x0010) >> 2) + ((code & 0x0008) << 1)
					+ ((code & 0x0004) >> 1) + ((code & 0x0002) << 2));
		color = m_buffer[offs + 6] & 0x00ff;
		pri = 0;

		m_callback(machine(), &code, &color, &pri);

		size = (m_buffer[offs] & 0x0f00) >> 8;

		w = 1 << (size & 0x03);
		h = 1 << ((size >> 2) & 0x03);

		/* zoom control:
		   0x40 = normal scale
		  <0x40 enlarge (0x20 = double size)
		  >0x40 reduce (0x80 = half size)
		*/
		zoomy = m_buffer[offs + 4];
		if (zoomy > 0x2000)
			continue;
		if (zoomy)
			zoomy = (0x400000 + zoomy / 2) / zoomy;
		else
			zoomy = 2 * 0x400000;
		if ((m_buffer[offs] & 0x4000) == 0)
		{
			zoomx = m_buffer[offs + 5];
			if (zoomx > 0x2000)
				continue;
			if (zoomx)
				zoomx = (0x400000 + zoomx / 2) / zoomx;
			else
				zoomx = 2 * 0x400000;
//          else zoomx = zoomy; /* workaround for TMNT2 */
		}
		else
			zoomx = zoomy;

		ox = m_buffer[offs + 3] + spriteoffsX;
		oy = m_buffer[offs + 2];

		ox += m_dx;
		oy += m_dy;

		flipx = m_buffer[offs] & 0x1000;
		flipy = m_buffer[offs] & 0x2000;
		mirrorx = m_buffer[offs + 6] & 0x0100;
		if (mirrorx)
			flipx = 0; // documented and confirmed
		mirrory = m_buffer[offs + 6] & 0x0200;
		shadow = m_buffer[offs + 6] & 0x0080;

		if (flipscreenX)
		{
			ox = 512 - ox;
			if (!mirrorx) flipx = !flipx;
		}
		if (flipscreenY)
		{
			oy = -oy;
			if (!mirrory) flipy = !flipy;
		}

		ox = (ox + 0x5d) & 0x3ff;
		if (ox >= 768) ox -= 1024;
		oy = (-(oy + spriteoffsY + 0x07)) & 0x3ff;
		if (oy >= 640) oy -= 1024;

		/* the coordinates given are for the *center* of the sprite */
		ox -= (zoomx * w) >> 13;
		oy -= (zoomy * h) >> 13;

		drawmode_table[machine().gfx[0]->granularity() - 1] = shadow ? DRAWMODE_SHADOW : DRAWMODE_SOURCE;

		for (y = 0; y < h; y++)
		{
			int sx, sy, zw, zh;

			sy = oy + ((zoomy * y + (1<<11)) >> 12);
			zh = (oy + ((zoomy * (y+1) + (1<<11)) >> 12)) - sy;

			for (x = 0; x < w; x++)
			{
				int c, fx, fy;

				sx = ox + ((zoomx * x + (1 << 11)) >> 12);
				zw = (ox + ((zoomx * (x+1) + (1 << 11)) >> 12)) - sx;
				c = code;
				if (mirrorx)
				{
					if ((flipx == 0) ^ (2 * x < w))
					{
						/* mirror left/right */
						c += (w - x - 1);
						fx = 1;
					}
					else
					{
						c += x;
						fx = 0;
					}
				}
				else
				{
					if (flipx) c += w-1-x;
					else c += x;
					fx = flipx;
				}
				if (mirrory)
				{
					if ((flipy == 0) ^ (2 * y >= h))
					{
						/* mirror top/bottom */
						c += 8 * (h - y - 1);
						fy = 1;
					}
					else
					{
						c += 8 * y;
						fy = 0;
					}
				}
				else
				{
					if (flipy) c += 8 * (h - 1 - y);
					else c += 8 * y;
					fy = flipy;
				}

				/* the sprite can start at any point in the 8x8 grid, but it must stay */
				/* in a 64 entries window, wrapping around at the edges. The animation */
				/* at the end of the saloon level in Sunset Riders breaks otherwise. */
				c = (c & 0x3f) | (code & ~0x3f);

				if (zoomx == 0x10000 && zoomy == 0x10000)
				{
					pdrawgfx_transtable(bitmap,cliprect,machine().gfx[0], /* hardcoded to 0 (decoded 6bpp gfx) for le */
							c,
							color,
							fx,fy,
							sx,sy,
							machine().priority_bitmap,pri,
							drawmode_table,machine().shadow_table);
				}
				else
				{
					pdrawgfxzoom_transtable(bitmap,cliprect,machine().gfx[0],  /* hardcoded to 0 (decoded 6bpp gfx) for le */
							c,
							color,
							fx,fy,
							sx,sy,
							(zw << 16) / 16,(zh << 16) / 16,
							machine().priority_bitmap,pri,
							drawmode_table,machine().shadow_table);

				}
			}
		}
	}
#if 0
if (machine().input().code_pressed(KEYCODE_D))
{
	FILE *fp;
	fp=fopen("SPRITE.DMP", "w+b");
	if (fp)
	{
		fwrite(m_buffer, 0x800, 1, fp);
		popmessage("saved");
		fclose(fp);
	}
}
#endif
#undef NUM_SPRITES
}

READ16_MEMBER( k05324x_device::k053244_reg_word_r )
{
	return(m_regs[offset * 2] << 8 | m_regs[offset * 2 + 1]);
}


/***************************************************************************/
/*                                                                         */
/*                                 053246/053247                           */
/*                                                                         */
/***************************************************************************/

struct k053247_state
{
	UINT16    *ram;

	gfx_element *gfx;

	UINT8    kx46_regs[8];
	UINT16   kx47_regs[16];
	int      dx, dy, wraparound;
	UINT8    objcha_line;
	int      z_rejection;

	k05324x_callback callback;

	const char *memory_region;
	screen_device *screen;
};



/*****************************************************************************
    INLINE FUNCTIONS
*****************************************************************************/

INLINE k053247_state *k053247_get_safe_token( device_t *device )
{
	assert(device != NULL);
	assert((device->type() == K053246 || device->type() == K053247 || device->type() == K055673));
	if (device->type() == K055673) {
		return (k053247_state *)downcast<k055673_device *>(device)->token();
	} else {
		return (k053247_state *)downcast<k053247_device *>(device)->token();
	}
}

INLINE const k053247_interface *k053247_get_interface( device_t *device )
{
	assert(device != NULL);
	assert((device->type() == K053246 || device->type() == K053247 || device->type() == K055673));
	return (const k053247_interface *) device->static_config();
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

#if 0
void k053247_get_gfx( device_t *device, gfx_element **gfx )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	*gfx = k053247->gfx;
}

void k053247_get_cb( device_t *device, void (**callback)(int *, int *, int *) )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	*callback = k053247->callback;
}

#endif

void k053247_get_ram( device_t *device, UINT16 **ram )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	*ram = k053247->ram;
}

int k053247_get_dx( device_t *device )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	return k053247->dx;
}

int k053247_get_dy( device_t *device )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	return k053247->dy;
}

int k053246_read_register( device_t *device, int regnum )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	return(k053247->kx46_regs[regnum]);
}

int k053247_read_register( device_t *device, int regnum )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	return(k053247->kx47_regs[regnum]);
}

void k053247_set_sprite_offs( device_t *device, int offsx, int offsy )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	k053247->dx = offsx;
	k053247->dy = offsy;
}

void k053247_wraparound_enable( device_t *device, int status )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	k053247->wraparound = status;
}

WRITE16_DEVICE_HANDLER( k053247_reg_word_w ) // write-only OBJSET2 registers (see p.43 table 6.1)
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	COMBINE_DATA(k053247->kx47_regs + offset);
}

WRITE32_DEVICE_HANDLER( k053247_reg_long_w )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	offset <<= 1;
	COMBINE_DATA(k053247->kx47_regs + offset + 1);
	mem_mask >>= 16;
	data >>= 16;
	COMBINE_DATA(k053247->kx47_regs + offset);
}

READ16_DEVICE_HANDLER( k053247_word_r )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	return k053247->ram[offset];
}

WRITE16_DEVICE_HANDLER( k053247_word_w )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	COMBINE_DATA(k053247->ram + offset);
}

READ32_DEVICE_HANDLER( k053247_long_r )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	return k053247->ram[offset * 2 + 1] | (k053247->ram[offset * 2] << 16);
}

WRITE32_DEVICE_HANDLER( k053247_long_w )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	offset <<= 1;
	COMBINE_DATA(k053247->ram + offset + 1);
	mem_mask >>= 16;
	data >>= 16;
	COMBINE_DATA(k053247->ram + offset);
}

READ8_DEVICE_HANDLER( k053247_r )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	int offs = offset >> 1;

	if (offset & 1)
		return(k053247->ram[offs] & 0xff);
	else
		return(k053247->ram[offs] >> 8);
}

WRITE8_DEVICE_HANDLER( k053247_w )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	int offs = offset >> 1;

	if (offset & 1)
		k053247->ram[offs] = (k053247->ram[offs] & 0xff00) | data;
	else
		k053247->ram[offs] = (k053247->ram[offs] & 0x00ff) | (data << 8);
}

// Mystic Warriors hardware games support a non-objcha based ROM readback
// write the address to the 246 as usual, but there's a completely separate ROM
// window that works without needing an objcha line.
// in this window, +0 = 32 bits from one set of ROMs, and +8 = 32 bits from another set
READ16_DEVICE_HANDLER( k055673_rom_word_r ) // 5bpp
{
	k053247_state *k053246 = k053247_get_safe_token(device);
	UINT8 *ROM8 = (UINT8 *)space.machine().root_device().memregion(k053246->memory_region)->base();
	UINT16 *ROM = (UINT16 *)space.machine().root_device().memregion(k053246->memory_region)->base();
	int size4 = (space.machine().root_device().memregion(k053246->memory_region)->bytes() / (1024 * 1024)) / 5;
	int romofs;

	size4 *= 4 * 1024 * 1024;   // get offset to 5th bit
	ROM8 += size4;

	romofs = k053246->kx46_regs[6] << 16 | k053246->kx46_regs[7] << 8 | k053246->kx46_regs[4];

	switch (offset)
	{
		case 0: // 20k / 36u
			return ROM[romofs + 2];
		case 1: // 17k / 36y
			return ROM[romofs + 3];
		case 2: // 10k / 32y
		case 3:
			romofs /= 2;
			return ROM8[romofs + 1];
		case 4: // 22k / 34u
			return ROM[romofs];
		case 5: // 19k / 34y
			return ROM[romofs + 1];
		case 6: // 12k / 29y
		case 7:
			romofs /= 2;
			return ROM8[romofs];
		default:
			LOG(("55673_rom_word_r: Unknown read offset %x\n", offset));
			break;
	}

	return 0;
}

READ16_DEVICE_HANDLER( k055673_GX6bpp_rom_word_r )
{
	k053247_state *k053246 = k053247_get_safe_token(device);
	UINT16 *ROM = (UINT16 *)space.machine().root_device().memregion(k053246->memory_region)->base();
	int romofs;

	romofs = k053246->kx46_regs[6] << 16 | k053246->kx46_regs[7] << 8 | k053246->kx46_regs[4];

	romofs /= 4;    // romofs increments 4 at a time
	romofs *= 12 / 2;   // each increment of romofs = 12 new bytes (6 new words)

	switch (offset)
	{
		case 0:
			return ROM[romofs + 3];
		case 1:
			return ROM[romofs + 4];
		case 2:
		case 3:
			return ROM[romofs + 5];
		case 4:
			return ROM[romofs];
		case 5:
			return ROM[romofs + 1];
		case 6:
		case 7:
			return ROM[romofs + 2];
		default:
//          LOG(("55673_rom_word_r: Unknown read offset %x (PC=%x)\n", offset, space.device().safe_pc()));
			break;
	}

	return 0;
}

READ8_DEVICE_HANDLER( k053246_r )
{
	k053247_state *k053246 = k053247_get_safe_token(device);
	if (k053246->objcha_line == ASSERT_LINE)
	{
		int addr;

		addr = (k053246->kx46_regs[6] << 17) | (k053246->kx46_regs[7] << 9) | (k053246->kx46_regs[4] << 1) | ((offset & 1) ^ 1);
		addr &= space.machine().root_device().memregion(k053246->memory_region)->bytes() - 1;
//      if (VERBOSE)
//          popmessage("%04x: offset %02x addr %06x", space.device().safe_pc(), offset, addr);
		return space.machine().root_device().memregion(k053246->memory_region)->base()[addr];
	}
	else
	{
//      LOG(("%04x: read from unknown 053246 address %x\n", space.device().safe_pc(), offset));
		return 0;
	}
}

WRITE8_DEVICE_HANDLER( k053246_w )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	k053247->kx46_regs[offset] = data;
}

READ16_DEVICE_HANDLER( k053246_word_r )
{
	offset <<= 1;
	return k053246_r(device, space, offset + 1) | (k053246_r(device, space, offset) << 8);
}

WRITE16_DEVICE_HANDLER( k053246_word_w )
{
	if (ACCESSING_BITS_8_15)
		k053246_w(device, space, offset << 1,(data >> 8) & 0xff);
	if (ACCESSING_BITS_0_7)
		k053246_w(device, space, (offset << 1) + 1,data & 0xff);
}

READ32_DEVICE_HANDLER( k053246_long_r )
{
	offset <<= 1;
	return (k053246_word_r(device, space, offset + 1, 0xffff) | k053246_word_r(device, space, offset, 0xffff) << 16);
}

WRITE32_DEVICE_HANDLER( k053246_long_w )
{
	offset <<= 1;
	k053246_word_w(device, space, offset, data >> 16, mem_mask >> 16);
	k053246_word_w(device, space, offset + 1, data, mem_mask);
}

void k053246_set_objcha_line( device_t *device, int state )
{
	k053247_state *k053246 = k053247_get_safe_token(device);
	k053246->objcha_line = state;
}

int k053246_is_irq_enabled( device_t *device )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	// This bit enables obj DMA rather than obj IRQ even though the two functions usually coincide.
	return k053247->kx46_regs[5] & 0x10;
}

/*
 * Sprite Format
 * ------------------
 *
 * Word | Bit(s)           | Use
 * -----+-fedcba9876543210-+----------------
 *   0  | x--------------- | active (show this sprite)
 *   0  | -x-------------- | maintain aspect ratio (when set, zoom y acts on both axis)
 *   0  | --x------------- | flip y
 *   0  | ---x------------ | flip x
 *   0  | ----xxxx-------- | sprite size (see below)
 *   0  | --------xxxxxxxx | zcode
 *   1  | xxxxxxxxxxxxxxxx | sprite code
 *   2  | ------xxxxxxxxxx | y position
 *   3  | ------xxxxxxxxxx | x position
 *   4  | xxxxxxxxxxxxxxxx | zoom y (0x40 = normal, <0x40 = enlarge, >0x40 = reduce)
 *   5  | xxxxxxxxxxxxxxxx | zoom x (0x40 = normal, <0x40 = enlarge, >0x40 = reduce)
 *   6  | x--------------- | mirror y (top half is drawn as mirror image of the bottom)
 *   6  | -x-------------- | mirror x (right half is drawn as mirror image of the left)
 *   6  | --xx------------ | reserved (sprites with these two bits set don't seem to be graphics data at all)
 *   6  | ----xx---------- | shadow code: 0=off, 0x400=preset1, 0x800=preset2, 0xc00=preset3
 *   6  | ------xx-------- | effect code: flicker, upper palette, full shadow...etc. (game dependent)
 *   6  | --------xxxxxxxx | "color", but depends on external connections (implies priority)
 *   7  | xxxxxxxxxxxxxxxx | game dependent
 *
 * shadow enables transparent shadows. Note that it applies to the last sprite pen ONLY.
 * The rest of the sprite remains normal.
 */

template<class _BitmapClass>
void k053247_sprites_draw_common( device_t *device, _BitmapClass &bitmap, const rectangle &cliprect )
{
#define NUM_SPRITES 256
	k053247_state *k053246 = k053247_get_safe_token(device);
	running_machine &machine = device->machine();

	/* sprites can be grouped up to 8x8. The draw order is
	     0  1  4  5 16 17 20 21
	     2  3  6  7 18 19 22 23
	     8  9 12 13 24 25 28 29
	    10 11 14 15 26 27 30 31
	    32 33 36 37 48 49 52 53
	    34 35 38 39 50 51 54 55
	    40 41 44 45 56 57 60 61
	    42 43 46 47 58 59 62 63
	*/
	static const int xoffset[8] = { 0, 1, 4, 5, 16, 17, 20, 21 };
	static const int yoffset[8] = { 0, 2, 8, 10, 32, 34, 40, 42 };

	int sortedlist[NUM_SPRITES];
	int offs,zcode;
	int ox, oy, color, code, size, w, h, x, y, xa, ya, flipx, flipy, mirrorx, mirrory, shadow, zoomx, zoomy, primask;
	int shdmask, nozoom, count, temp;

	int flipscreenx = k053246->kx46_regs[5] & 0x01;
	int flipscreeny = k053246->kx46_regs[5] & 0x02;
	int offx = (short)((k053246->kx46_regs[0] << 8) | k053246->kx46_regs[1]);
	int offy = (short)((k053246->kx46_regs[2] << 8) | k053246->kx46_regs[3]);

	int screen_width = k053246->screen->width();
	UINT8 drawmode_table[256];
	UINT8 shadowmode_table[256];
	UINT8 *whichtable;

	memset(drawmode_table, DRAWMODE_SOURCE, sizeof(drawmode_table));
	drawmode_table[0] = DRAWMODE_NONE;
	memset(shadowmode_table, DRAWMODE_SHADOW, sizeof(shadowmode_table));
	shadowmode_table[0] = DRAWMODE_NONE;

	/*
	    safeguard older drivers missing any of the following video attributes:

	    VIDEO_HAS_SHADOWS | VIDEO_HAS_HIGHLIGHTS
	*/
	if (machine.config().m_video_attributes & VIDEO_HAS_SHADOWS)
	{
		if (sizeof(typename _BitmapClass::pixel_t) == 4 && (machine.config().m_video_attributes & VIDEO_HAS_HIGHLIGHTS))
			shdmask = 3; // enable all shadows and highlights
		else
			shdmask = 0; // enable default shadows
	}
	else
		shdmask = -1; // disable everything

	/*
	    The k053247 does not draw pixels on top of those with equal or smaller Z-values
	    regardless of priority. Embedded shadows inherit Z-values from their host sprites
	    but do not assume host priorities unless explicitly told. In other words shadows
	    can have priorities different from that of normal pens in the same sprite,
	    in addition to the ability of masking themselves from specific layers or pixels
	    on the other sprites.

	    In front-to-back rendering, sprites cannot sandwich between alpha blended layers
	    or the draw code will have to figure out the percentage opacities of what is on
	    top and beneath each sprite pixel and blend the target accordingly. The process
	    is overly demanding for realtime software and is thus another shortcoming of
	    pdrawgfx and pixel based mixers. Even mahjong games with straight forward video
	    subsystems are feeling the impact by which the girls cannot appear under
	    translucent dialogue boxes.

	    These are a small part of the k053247's feature set but many games expect them
	    to be the minimum compliances. The specification will undoubtedly require
	    redesigning the priority system from the ground up. Drawgfx.c and tilemap.c must
	    also undergo heavy facelifts but in the end the changes could hurt simpler games
	    more than they help complex systems; therefore the new engine should remain
	    completely stand alone and self-contained. Implementation details are being
	    hammered down but too early to make propositions.
	*/

	// Prebuild a sorted table by descending Z-order.
	zcode = k053246->z_rejection;
	offs = count = 0;

	if (zcode == -1)
	{
		for (; offs < 0x800; offs += 8)
			if (k053246->ram[offs] & 0x8000)
				sortedlist[count++] = offs;
	}
	else
	{
		for (; offs < 0x800; offs += 8)
			if ((k053246->ram[offs] & 0x8000) && ((k053246->ram[offs] & 0xff) != zcode))
				sortedlist[count++] = offs;
	}

	w = count;
	count--;
	h = count;

	if (!(k053246->kx47_regs[0xc / 2] & 0x10))
	{
		// sort objects in decending order(smaller z closer) when OPSET PRI is clear
		for (y = 0; y < h; y++)
		{
			offs = sortedlist[y];
			zcode = k053246->ram[offs] & 0xff;
			for (x = y + 1; x < w; x++)
			{
				temp = sortedlist[x];
				code = k053246->ram[temp] & 0xff;
				if (zcode <= code)
				{
					zcode = code;
					sortedlist[x] = offs;
					sortedlist[y] = offs = temp;
				}
			}
		}
	}
	else
	{
		// sort objects in ascending order(bigger z closer) when OPSET PRI is set
		for (y = 0; y < h; y++)
		{
			offs = sortedlist[y];
			zcode = k053246->ram[offs] & 0xff;
			for (x = y + 1; x < w; x++)
			{
				temp = sortedlist[x];
				code = k053246->ram[temp] & 0xff;
				if (zcode >= code)
				{
					zcode = code;
					sortedlist[x] = offs;
					sortedlist[y] = offs = temp;
				}
			}
		}
	}

	for (; count >= 0; count--)
	{
		offs = sortedlist[count];

		code = k053246->ram[offs + 1];
		shadow = color = k053246->ram[offs + 6];
		primask = 0;

		k053246->callback(device->machine(), &code, &color, &primask);

		temp = k053246->ram[offs];

		size = (temp & 0x0f00) >> 8;
		w = 1 << (size & 0x03);
		h = 1 << ((size >> 2) & 0x03);

		/* the sprite can start at any point in the 8x8 grid. We have to */
		/* adjust the offsets to draw it correctly. Simpsons does this all the time. */
		xa = 0;
		ya = 0;
		if (code & 0x01) xa += 1;
		if (code & 0x02) ya += 1;
		if (code & 0x04) xa += 2;
		if (code & 0x08) ya += 2;
		if (code & 0x10) xa += 4;
		if (code & 0x20) ya += 4;
		code &= ~0x3f;

		oy = (short)k053246->ram[offs + 2];
		ox = (short)k053246->ram[offs + 3];

		if (k053246->wraparound)
		{
			offx &= 0x3ff;
			offy &= 0x3ff;
			oy &= 0x3ff;
			ox &= 0x3ff;
		}

		/* zoom control:
		   0x40 = normal scale
		  <0x40 enlarge (0x20 = double size)
		  >0x40 reduce (0x80 = half size)
		*/
		y = zoomy = k053246->ram[offs + 4] & 0x3ff;
		if (zoomy)
			zoomy = (0x400000 + (zoomy >> 1)) / zoomy;
		else
			zoomy = 0x800000;

		if (!(temp & 0x4000))
		{
			x = zoomx = k053246->ram[offs + 5] & 0x3ff;
			if (zoomx)
				zoomx = (0x400000 + (zoomx >> 1)) / zoomx;
			else
				zoomx = 0x800000;
		}
		else
		{
			zoomx = zoomy;
			x = y;
		}

// ************************************************************************************
//  for Escape Kids (GX975)
// ************************************************************************************
//    Escape Kids use 053246 #5 register's UNKNOWN Bit #5, #3 and #2.
//    Bit #5, #3, #2 always set "1".
//    Maybe, Bit #5 or #3 or #2 or combination means "FIX SPRITE WIDTH TO HALF" ?????
//    Below 7 lines supports this 053246's(???) function.
//    Don't rely on it, Please.  But, Escape Kids works correctly!
// ************************************************************************************
		if ( k053246->kx46_regs[5] & 0x08 ) // Check only "Bit #3 is '1'?" (NOTE: good guess)
		{
			zoomx >>= 1;        // Fix sprite width to HALF size
			ox = (ox >> 1) + 1; // Fix sprite draw position
			if (flipscreenx)
				ox += screen_width;
			nozoom = 0;
		}
		else
			nozoom = (x == 0x40 && y == 0x40);

		flipx = temp & 0x1000;
		flipy = temp & 0x2000;
		mirrorx = shadow & 0x4000;
		if (mirrorx)
			flipx = 0; // documented and confirmed
		mirrory = shadow & 0x8000;

		whichtable = drawmode_table;
		if (color == -1)
		{
			// drop the entire sprite to shadow unconditionally
			if (shdmask < 0) continue;
			color = 0;
			shadow = -1;
			whichtable = shadowmode_table;
			palette_set_shadow_mode(machine, 0);
		}
		else
		{
			if (shdmask >= 0)
			{
				shadow = (color & K053247_CUSTOMSHADOW) ? (color >> K053247_SHDSHIFT) : (shadow >> 10);
				if (shadow &= 3) palette_set_shadow_mode(machine, (shadow - 1) & shdmask);
			}
			else
				shadow = 0;
		}

		color &= 0xffff; // strip attribute flags

		if (flipscreenx)
		{
			ox = -ox;
			if (!mirrorx) flipx = !flipx;
		}
		if (flipscreeny)
		{
			oy = -oy;
			if (!mirrory) flipy = !flipy;
		}

		// apply wrapping and global offsets
		if (k053246->wraparound)
		{
			ox = ( ox - offx) & 0x3ff;
			oy = (-oy - offy) & 0x3ff;
			if (ox >= 0x300) ox -= 0x400;
			if (oy >= 0x280) oy -= 0x400;
		}
		else
		{
			ox =  ox - offx;
			oy = -oy - offy;
		}
		ox += k053246->dx;
		oy -= k053246->dy;

		// apply global and display window offsets

		/* the coordinates given are for the *center* of the sprite */
		ox -= (zoomx * w) >> 13;
		oy -= (zoomy * h) >> 13;

		drawmode_table[k053246->gfx->granularity() - 1] = shadow ? DRAWMODE_SHADOW : DRAWMODE_SOURCE;

		for (y = 0; y < h; y++)
		{
			int sx, sy, zw, zh;

			sy = oy + ((zoomy * y + (1 << 11)) >> 12);
			zh = (oy + ((zoomy * (y + 1) + (1 << 11)) >> 12)) - sy;

			for (x = 0; x < w; x++)
			{
				int c, fx, fy;

				sx = ox + ((zoomx * x + (1 << 11)) >> 12);
				zw = (ox + ((zoomx * (x+1) + (1 << 11)) >> 12)) - sx;
				c = code;
				if (mirrorx)
				{
					if ((flipx == 0) ^ ((x << 1) < w))
					{
						/* mirror left/right */
						c += xoffset[(w - 1 - x + xa) & 7];
						fx = 1;
					}
					else
					{
						c += xoffset[(x + xa) & 7];
						fx = 0;
					}
				}
				else
				{
					if (flipx) c += xoffset[(w - 1 - x + xa) & 7];
					else c += xoffset[(x + xa) & 7];
					fx = flipx;
				}
				if (mirrory)
				{
					if ((flipy == 0) ^ ((y<<1) >= h))
					{
						/* mirror top/bottom */
						c += yoffset[(h - 1 - y + ya) & 7];
						fy = 1;
					}
					else
					{
						c += yoffset[(y + ya) & 7];
						fy = 0;
					}
				}
				else
				{
					if (flipy) c += yoffset[(h - 1 - y + ya) & 7];
					else c += yoffset[(y + ya) & 7];
					fy = flipy;
				}

				if (nozoom)
				{
					pdrawgfx_transtable(bitmap,cliprect,k053246->gfx,
							c,
							color,
							fx,fy,
							sx,sy,
							machine.priority_bitmap,primask,
							whichtable,machine.shadow_table);
				}
				else
				{
					pdrawgfxzoom_transtable(bitmap,cliprect,k053246->gfx,
							c,
							color,
							fx,fy,
							sx,sy,
							(zw << 16) >> 4,(zh << 16) >> 4,
							machine.priority_bitmap,primask,
							whichtable,machine.shadow_table);
				}

				if (mirrory && h == 1)  /* Simpsons shadows */
				{
					if (nozoom)
					{
						pdrawgfx_transtable(bitmap,cliprect,k053246->gfx,
								c,
								color,
								fx,!fy,
								sx,sy,
								machine.priority_bitmap,primask,
								whichtable,machine.shadow_table);
					}
					else
					{
						pdrawgfxzoom_transtable(bitmap,cliprect,k053246->gfx,
								c,
								color,
								fx,!fy,
								sx,sy,
								(zw << 16) >> 4,(zh << 16) >> 4,
								machine.priority_bitmap,primask,
								whichtable,machine.shadow_table);
					}
				}
			} // end of X loop
		} // end of Y loop

	} // end of sprite-list loop
#undef NUM_SPRITES
}

void k053247_sprites_draw( device_t *device, bitmap_ind16 &bitmap, const rectangle &cliprect )
{ k053247_sprites_draw_common(device, bitmap, cliprect); }

void k053247_sprites_draw( device_t *device, bitmap_rgb32 &bitmap, const rectangle &cliprect )
{ k053247_sprites_draw_common(device, bitmap, cliprect); }


/*****************************************************************************
    DEVICE INTERFACE
*****************************************************************************/

static DEVICE_START( k053247 )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	const k053247_interface *intf = k053247_get_interface(device);
	running_machine &machine = device->machine();
	UINT32 total;
	static const gfx_layout spritelayout =
	{
		16,16,
		0,
		4,
		{ 0, 1, 2, 3 },
		{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4,
				10*4, 11*4, 8*4, 9*4, 14*4, 15*4, 12*4, 13*4 },
		{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
				8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
		128*8
	};
	static const gfx_layout tasman_16x16_layout =
	{
		16,16,
		RGN_FRAC(1,2),
		8,
		{ 0,8,16,24, RGN_FRAC(1,2)+0,RGN_FRAC(1,2)+8,RGN_FRAC(1,2)+16,RGN_FRAC(1,2)+24 },
		{ 0,1,2,3,4,5,6,7, 32,33,34,35,36,37,38,39 },
		{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64, 8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
		16*64
	};

	k053247->screen = machine.device<screen_device>(intf->screen);

	/* decode the graphics */
	switch (intf->plane_order)
	{
	case NORMAL_PLANE_ORDER:
		total = machine.root_device().memregion(intf->gfx_memory_region)->bytes() / 128;
		decode_gfx(machine, intf->gfx_num, machine.root_device().memregion(intf->gfx_memory_region)->base(), total, &spritelayout, 4);
		break;

	case TASMAN_PLANE_ORDER:
		total = machine.root_device().memregion(intf->gfx_memory_region)->bytes() / 128;
		decode_gfx(machine, intf->gfx_num, machine.root_device().memregion(intf->gfx_memory_region)->base(), total, &tasman_16x16_layout, 4);
		break;

	default:
		fatalerror("Unsupported plane_order\n");
	}

	if (VERBOSE)
	{
		if (k053247->screen->format() == BITMAP_FORMAT_RGB32)
		{
			if ((machine.config().m_video_attributes & (VIDEO_HAS_SHADOWS|VIDEO_HAS_HIGHLIGHTS)) != VIDEO_HAS_SHADOWS+VIDEO_HAS_HIGHLIGHTS)
				popmessage("driver missing SHADOWS or HIGHLIGHTS flag");
		}
		else
		{
			if (!(machine.config().m_video_attributes & VIDEO_HAS_SHADOWS))
				popmessage("driver should use VIDEO_HAS_SHADOWS");
		}
	}

	/* deinterleave the graphics, if needed */
	deinterleave_gfx(machine, intf->gfx_memory_region, intf->deinterleave);

	k053247->dx = intf->dx;
	k053247->dy = intf->dy;
	k053247->memory_region = intf->gfx_memory_region;
	k053247->gfx = machine.gfx[intf->gfx_num];
	k053247->callback = intf->callback;

	k053247->ram = auto_alloc_array_clear(machine, UINT16, 0x1000 / 2);

	device->save_pointer(NAME(k053247->ram), 0x1000 / 2);
	device->save_item(NAME(k053247->kx46_regs));
	device->save_item(NAME(k053247->kx47_regs));
	device->save_item(NAME(k053247->objcha_line));
	device->save_item(NAME(k053247->wraparound));
	device->save_item(NAME(k053247->z_rejection));
}

/* K055673 used with the 54246 in PreGX/Run and Gun/System GX games */
static DEVICE_START( k055673 )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	const k053247_interface *intf = k053247_get_interface(device);
	running_machine &machine = device->machine();
	UINT32 total;
	UINT8 *s1, *s2, *d;
	long i;
	UINT16 *K055673_rom;
	int size4;

	static const gfx_layout spritelayout =  /* System GX sprite layout */
	{
		16,16,
		0,
		5,
		{ 32, 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 40, 41, 42, 43, 44, 45, 46, 47 },
		{ 0, 10*8, 10*8*2, 10*8*3, 10*8*4, 10*8*5, 10*8*6, 10*8*7, 10*8*8,
			10*8*9, 10*8*10, 10*8*11, 10*8*12, 10*8*13, 10*8*14, 10*8*15 },
		16*16*5
	};
	static const gfx_layout spritelayout2 = /* Run and Gun sprite layout */
	{
		16,16,
		0,
		4,
		{ 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 32, 33, 34, 35, 36, 37, 38, 39 },
		{ 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960 },
		16*16*4
	};
	static const gfx_layout spritelayout3 = /* Lethal Enforcers II sprite layout */
	{
		16,16,
		0,
		8,
		{ 8*1,8*0,8*3,8*2,8*5,8*4,8*7,8*6 },
		{  0,1,2,3,4,5,6,7,64+0,64+1,64+2,64+3,64+4,64+5,64+6,64+7 },
		{ 128*0, 128*1, 128*2,  128*3,  128*4,  128*5,  128*6,  128*7,
			128*8, 128*9, 128*10, 128*11, 128*12, 128*13, 128*14, 128*15 },
		128*16
	};
	static const gfx_layout spritelayout4 = /* System GX 6bpp sprite layout */
	{
		16,16,
		0,
		6,
		{ 40, 32, 24, 16, 8, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 48, 49, 50, 51, 52, 53, 54, 55 },
		{ 0, 12*8, 12*8*2, 12*8*3, 12*8*4, 12*8*5, 12*8*6, 12*8*7, 12*8*8,
			12*8*9, 12*8*10, 12*8*11, 12*8*12, 12*8*13, 12*8*14, 12*8*15 },
		16*16*6
	};

	k053247->screen = machine.device<screen_device>(intf->screen);

	K055673_rom = (UINT16 *)machine.root_device().memregion(intf->gfx_memory_region)->base();

	/* decode the graphics */
	switch (intf->plane_order)  /* layout would be more correct than plane_order, but we use k053247_interface */
	{
	case K055673_LAYOUT_GX:
		size4 = (machine.root_device().memregion(intf->gfx_memory_region)->bytes() / (1024 * 1024)) / 5;
		size4 *= 4 * 1024 * 1024;
		/* set the # of tiles based on the 4bpp section */
		K055673_rom = auto_alloc_array(machine, UINT16, size4 * 5 / 2);
		d = (UINT8 *)K055673_rom;
		// now combine the graphics together to form 5bpp
		s1 = machine.root_device().memregion(intf->gfx_memory_region)->base(); // 4bpp area
		s2 = s1 + (size4);   // 1bpp area
		for (i = 0; i < size4; i+= 4)
		{
			*d++ = *s1++;
			*d++ = *s1++;
			*d++ = *s1++;
			*d++ = *s1++;
			*d++ = *s2++;
		}

		total = size4 / 128;
		decode_gfx(machine, intf->gfx_num, (UINT8 *)K055673_rom, total, &spritelayout, 4);
		break;

	case K055673_LAYOUT_RNG:
		total = machine.root_device().memregion(intf->gfx_memory_region)->bytes() / (16 * 16 / 2);
		decode_gfx(machine, intf->gfx_num, (UINT8 *)K055673_rom, total, &spritelayout2, 4);
		break;

	case K055673_LAYOUT_LE2:
		total = machine.root_device().memregion(intf->gfx_memory_region)->bytes() / (16 * 16);
		decode_gfx(machine, intf->gfx_num, (UINT8 *)K055673_rom, total, &spritelayout3, 4);
		break;

	case K055673_LAYOUT_GX6:
		total = machine.root_device().memregion(intf->gfx_memory_region)->bytes() / (16 * 16 * 6 / 8);
		decode_gfx(machine, intf->gfx_num, (UINT8 *)K055673_rom, total, &spritelayout4, 4);
		break;

	default:
		fatalerror("Unsupported layout\n");
	}

	if (VERBOSE && !(machine.config().m_video_attributes & VIDEO_HAS_SHADOWS))
		popmessage("driver should use VIDEO_HAS_SHADOWS");

	k053247->dx = intf->dx;
	k053247->dy = intf->dy;
	k053247->memory_region = intf->gfx_memory_region;
	k053247->gfx = machine.gfx[intf->gfx_num];
	k053247->callback = intf->callback;

	k053247->ram = auto_alloc_array(machine, UINT16, 0x1000 / 2);

	device->save_pointer(NAME(k053247->ram), 0x800);
	device->save_item(NAME(k053247->kx46_regs));
	device->save_item(NAME(k053247->kx47_regs));
	device->save_item(NAME(k053247->objcha_line));
	device->save_item(NAME(k053247->wraparound));
	device->save_item(NAME(k053247->z_rejection));
}

static DEVICE_RESET( k053247 )
{
	k053247_state *k053247 = k053247_get_safe_token(device);

	k053247->wraparound = 1;
	k053247->z_rejection = -1;
	k053247->objcha_line = CLEAR_LINE;

	memset(k053247->kx46_regs, 0, 8);
	memset(k053247->kx47_regs, 0, 32);
}


const device_type K055673 = &device_creator<k055673_device>;

k055673_device::k055673_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K055673, "Konami 055673", tag, owner, clock, "k055673", __FILE__)
{
	m_token = global_alloc_clear(k053247_state);
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k055673_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k055673_device::device_start()
{
	DEVICE_START_NAME( k055673 )(this);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k055673_device::device_reset()
{
	DEVICE_RESET_NAME( k053247 )(this);
}

const device_type K053246 = &device_creator<k053247_device>;

k053247_device::k053247_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K053246, "Konami 053246 & 053247", tag, owner, clock, "k053247", __FILE__)
{
	m_token = global_alloc_clear(k053247_state);
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k053247_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k053247_device::device_start()
{
	DEVICE_START_NAME( k053247 )(this);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k053247_device::device_reset()
{
	DEVICE_RESET_NAME( k053247 )(this);
}


/*
    In a K053247+K055555 setup objects with Z-code 0x00 should be ignored
    when PRFLIP is cleared, while objects with Z-code 0xff should be
    ignored when PRFLIP is set.

    These behaviors can also be seen in older K053245(6)+K053251 setups.
    Bucky'O Hare, The Simpsons and Sunset Riders rely on their implications
    to prepare and retire sprites. They probably apply to many other Konami
    games but it's hard to tell because most artifacts have been filtered
    by exclusion sort.

    A driver may call K05324x_set_z_rejection() to set which zcode to ignore.
    Parameter:
               -1 = accept all(default)
        0x00-0xff = zcode to ignore
*/

void k053247_set_z_rejection( device_t *device, int zcode )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	k053247->z_rejection = zcode;
}

/***************************************************************************/
/*                                                                         */
/*                                 051316                                  */
/*                                                                         */
/***************************************************************************/

const device_type K051316 = &device_creator<k051316_device>;

k051316_device::k051316_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K051316, "Konami 051316", tag, owner, clock, "k051316", __FILE__),
	m_ram(NULL),
	//m_tmap,
	m_bpp(0)
	//m_ctrlram[16]
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k051316_device::device_config_complete()
{
	// inherit a copy of the static data
	const k051316_interface *intf = reinterpret_cast<const k051316_interface *>(static_config());
	if (intf != NULL)
	*static_cast<k051316_interface *>(this) = *intf;
	
	// or initialize to defaults if none provided
	else
	{
		m_gfx_memory_region_tag = "";
		m_gfx_num = 0;
		m_bpp = 0;
		m_pen_is_mask = 0;
		m_transparent_pen = 0;
		m_wrap = 0;
		m_xoffs = 0;
		m_yoffs = 0;
		m_callback = NULL;
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k051316_device::device_start()
{
	int is_tail2nos = 0;
	UINT32 total;

	static const gfx_layout charlayout4 =
	{
		16,16,
		0,
		4,
		{ 0, 1, 2, 3 },
		{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4,
				8*4, 9*4, 10*4, 11*4, 12*4, 13*4, 14*4, 15*4 },
		{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
				8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
		128*8
	};

	static const gfx_layout charlayout7 =
	{
		16,16,
		0,
		7,
		{ 1,2,3,4,5,6,7 },
		{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
				8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
		{ 0*128, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128,
				8*128, 9*128, 10*128, 11*128, 12*128, 13*128, 14*128, 15*128 },
		256*8
	};

	static const gfx_layout charlayout8 =
	{
		16,16,
		0,
		8,
		{ 0,1,2,3,4,5,6,7 },
		{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
				8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },
		{ 0*128, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128,
				8*128, 9*128, 10*128, 11*128, 12*128, 13*128, 14*128, 15*128 },
		256*8
	};

	static const gfx_layout charlayout_tail2nos =
	{
		16,16,
		0,
		4,
		{ 0, 1, 2, 3 },
		{ XOR(0)*4, XOR(1)*4, XOR(2)*4, XOR(3)*4, XOR(4)*4, XOR(5)*4, XOR(6)*4, XOR(7)*4,
				XOR(8)*4, XOR(9)*4, XOR(10)*4, XOR(11)*4, XOR(12)*4, XOR(13)*4, XOR(14)*4, XOR(15)*4 },
		{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
				8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
		128*8
	};

	/* decode the graphics */
	switch (m_bpp)
	{
	case -4:
		total = 0x400;
		is_tail2nos = 1;
		decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region_tag)->base(), total, &charlayout_tail2nos, 4);
		break;

	case 4:
		total = machine().root_device().memregion(m_gfx_memory_region_tag)->bytes() / 128;
		decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region_tag)->base(), total, &charlayout4, 4);
		break;

	case 7:
		total = machine().root_device().memregion(m_gfx_memory_region_tag)->bytes() / 256;
		decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region_tag)->base(), total, &charlayout7, 7);
		break;

	case 8:
		total = machine().root_device().memregion(m_gfx_memory_region_tag)->bytes() / 256;
		decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region_tag)->base(), total, &charlayout8, 8);
		break;

	default:
		fatalerror("Unsupported bpp\n");
	}

	m_bpp = is_tail2nos ? 4 : m_bpp; // tail2nos is passed with bpp = -4 to setup the custom charlayout!
	
	m_tmap = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k051316_device::get_tile_info0),this), TILEMAP_SCAN_ROWS, 16, 16, 32, 32);

	m_ram = auto_alloc_array_clear(machine(), UINT8, 0x800);

	if (!m_pen_is_mask)
		m_tmap->set_transparent_pen(m_transparent_pen);
	else
	{
		m_tmap->map_pens_to_layer(0, 0, 0, TILEMAP_PIXEL_LAYER1);
		m_tmap->map_pens_to_layer(0, m_transparent_pen, m_transparent_pen, TILEMAP_PIXEL_LAYER0);
	}

	save_pointer(NAME(m_ram), 0x800);
	save_item(NAME(m_ctrlram));
	save_item(NAME(m_wrap));

}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k051316_device::device_reset()
{
	memset(m_ctrlram,  0, 0x10);
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

READ8_MEMBER( k051316_device::read )
{
	return m_ram[offset];
}

WRITE8_MEMBER( k051316_device::write )
{
	m_ram[offset] = data;
	m_tmap->mark_tile_dirty(offset & 0x3ff);
}


READ8_MEMBER( k051316_device::rom_r )
{
	if ((m_ctrlram[0x0e] & 0x01) == 0)
	{
		int addr = offset + (m_ctrlram[0x0c] << 11) + (m_ctrlram[0x0d] << 19);
		if (m_bpp <= 4)
			addr /= 2;
		addr &= space.machine().root_device().memregion(m_gfx_memory_region_tag)->bytes() - 1;

		//  popmessage("%s: offset %04x addr %04x", space.machine().describe_context(), offset, addr);

		return space.machine().root_device().memregion(m_gfx_memory_region_tag)->base()[addr];
	}
	else
	{
		//logerror("%s: read 051316 ROM offset %04x but reg 0x0c bit 0 not clear\n", space.machine().describe_context(), offset);
		return 0;
	}
}

WRITE8_MEMBER( k051316_device::ctrl_w )
{
	m_ctrlram[offset] = data;
	//if (offset >= 0x0c) logerror("%s: write %02x to 051316 reg %x\n", space.machine().describe_context(), data, offset);
}

// a few games (ajax, rollerg, ultraman, etc.) can enable and disable wraparound after start
void k051316_device::wraparound_enable( int status )
{
	m_wrap = status;
}

/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

void k051316_device::get_tile_info( tile_data &tileinfo, int tile_index )
{
	int code = m_ram[tile_index];
	int color = m_ram[tile_index + 0x400];
	int flags = 0;

	m_callback(machine(), &code, &color, &flags);

	SET_TILE_INFO_MEMBER(
			m_gfx_num,
			code,
			color,
			flags);
}


TILE_GET_INFO_MEMBER(k051316_device::get_tile_info0) { get_tile_info(tileinfo, tile_index); }


void k051316_device::zoom_draw( bitmap_ind16 &bitmap, const rectangle &cliprect, int flags, UINT32 priority )
{
	UINT32 startx, starty;
	int incxx, incxy, incyx, incyy;

	startx = 256 * ((INT16)(256 * m_ctrlram[0x00] + m_ctrlram[0x01]));
	incxx  =        (INT16)(256 * m_ctrlram[0x02] + m_ctrlram[0x03]);
	incyx  =        (INT16)(256 * m_ctrlram[0x04] + m_ctrlram[0x05]);
	starty = 256 * ((INT16)(256 * m_ctrlram[0x06] + m_ctrlram[0x07]));
	incxy  =        (INT16)(256 * m_ctrlram[0x08] + m_ctrlram[0x09]);
	incyy  =        (INT16)(256 * m_ctrlram[0x0a] + m_ctrlram[0x0b]);

	startx -= (16 + m_yoffs) * incyx;
	starty -= (16 + m_yoffs) * incyy;

	startx -= (89 + m_xoffs) * incxx;
	starty -= (89 + m_xoffs) * incxy;

	m_tmap->draw_roz(bitmap, cliprect, startx << 5,starty << 5,
			incxx << 5,incxy << 5,incyx << 5,incyy << 5,
			m_wrap,
			flags,priority);

#if 0
	popmessage("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
			m_ctrlram[0x00],
			m_ctrlram[0x01],
			m_ctrlram[0x02],
			m_ctrlram[0x03],
			m_ctrlram[0x04],
			m_ctrlram[0x05],
			m_ctrlram[0x06],
			m_ctrlram[0x07],
			m_ctrlram[0x08],
			m_ctrlram[0x09],
			m_ctrlram[0x0a],
			m_ctrlram[0x0b],
			m_ctrlram[0x0c], /* bank for ROM testing */
			m_ctrlram[0x0d],
			m_ctrlram[0x0e], /* 0 = test ROMs */
			m_ctrlram[0x0f]);
#endif
}


/***************************************************************************/
/*                                                                         */
/*                                 053936                                  */
/*                                                                         */
/***************************************************************************/

const device_type K053936 = &device_creator<k053936_device>;

k053936_device::k053936_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K053936, "Konami 053936", tag, owner, clock, "k053936", __FILE__),
	m_ctrl(NULL),
	m_linectrl(NULL)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k053936_device::device_config_complete()
{
	// inherit a copy of the static data
	const k053936_interface *intf = reinterpret_cast<const k053936_interface *>(static_config());
	if (intf != NULL)
	*static_cast<k053936_interface *>(this) = *intf;
	
	// or initialize to defaults if none provided
	else
	{
		m_wrap = 0;
		m_xoff = 0;
		m_yoff = 0;
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k053936_device::device_start()
{
	m_ctrl = auto_alloc_array_clear(machine(), UINT16, 0x20);
	m_linectrl = auto_alloc_array_clear(machine(), UINT16, 0x4000);

	save_pointer(NAME(m_ctrl), 0x20);
	save_pointer(NAME(m_linectrl), 0x4000);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k053936_device::device_reset()
{
	memset(m_ctrl, 0, 0x20);
}


/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

WRITE16_MEMBER( k053936_device::ctrl_w )
{
	COMBINE_DATA(&m_ctrl[offset]);
}

READ16_MEMBER( k053936_device::ctrl_r )
{
	return m_ctrl[offset];
}

WRITE16_MEMBER( k053936_device::linectrl_w )
{
	COMBINE_DATA(&m_linectrl[offset]);
}

READ16_MEMBER( k053936_device::linectrl_r )
{
	return m_linectrl[offset];
}

// there is another implementation of this in  video/konamigx.c (!)
//  why? shall they be merged?
void k053936_device::zoom_draw( bitmap_ind16 &bitmap, const rectangle &cliprect, tilemap_t *tmap, int flags, UINT32 priority, int glfgreat_hack )
{
	if (!tmap)
		return;

	if (m_ctrl[0x07] & 0x0040)
	{
		UINT32 startx, starty;
		int incxx, incxy;
		rectangle my_clip;
		int y, maxy;

		// Racin' Force will get to here if glfgreat_hack is enabled, and it ends
		// up setting a maximum y value of '13', thus causing nothing to be drawn.
		// It looks like the roz output should be flipped somehow as it seems to be
		// displaying the wrong areas of the tilemap and is rendered upside down,
		// although due to the additional post-processing the voxel renderer performs
		// it's difficult to know what the output SHOULD be.  (hold W in Racin' Force
		// to see the chip output)

		if (((m_ctrl[0x07] & 0x0002) && m_ctrl[0x09]) && (glfgreat_hack)) /* wrong, but fixes glfgreat */
		{
			my_clip.min_x = m_ctrl[0x08] + m_xoff + 2;
			my_clip.max_x = m_ctrl[0x09] + m_xoff + 2 - 1;
			if (my_clip.min_x < cliprect.min_x)
				my_clip.min_x = cliprect.min_x;
			if (my_clip.max_x > cliprect.max_x)
				my_clip.max_x = cliprect.max_x;

			y = m_ctrl[0x0a] + m_yoff - 2;
			if (y < cliprect.min_y)
				y = cliprect.min_y;
			maxy = m_ctrl[0x0b] + m_yoff - 2 - 1;
			if (maxy > cliprect.max_y)
				maxy = cliprect.max_y;
		}
		else
		{
			my_clip.min_x = cliprect.min_x;
			my_clip.max_x = cliprect.max_x;

			y = cliprect.min_y;
			maxy = cliprect.max_y;
		}

		while (y <= maxy)
		{
			UINT16 *lineaddr = m_linectrl + 4 * ((y - m_yoff) & 0x1ff);

			my_clip.min_y = my_clip.max_y = y;

			startx = 256 * (INT16)(lineaddr[0] + m_ctrl[0x00]);
			starty = 256 * (INT16)(lineaddr[1] + m_ctrl[0x01]);
			incxx  =       (INT16)(lineaddr[2]);
			incxy  =       (INT16)(lineaddr[3]);

			if (m_ctrl[0x06] & 0x8000)
				incxx *= 256;

			if (m_ctrl[0x06] & 0x0080)
				incxy *= 256;

			startx -= m_xoff * incxx;
			starty -= m_xoff * incxy;

			tmap->draw_roz(bitmap, my_clip, startx << 5,starty << 5,
					incxx << 5,incxy << 5,0,0,
					m_wrap,
					flags,priority);

			y++;
		}
	}
	else    /* "simple" mode */
	{
		UINT32 startx, starty;
		int incxx, incxy, incyx, incyy;

		startx = 256 * (INT16)(m_ctrl[0x00]);
		starty = 256 * (INT16)(m_ctrl[0x01]);
		incyx  =       (INT16)(m_ctrl[0x02]);
		incyy  =       (INT16)(m_ctrl[0x03]);
		incxx  =       (INT16)(m_ctrl[0x04]);
		incxy  =       (INT16)(m_ctrl[0x05]);

		if (m_ctrl[0x06] & 0x4000)
		{
			incyx *= 256;
			incyy *= 256;
		}

		if (m_ctrl[0x06] & 0x0040)
		{
			incxx *= 256;
			incxy *= 256;
		}

		startx -= m_yoff * incyx;
		starty -= m_yoff * incyy;

		startx -= m_xoff * incxx;
		starty -= m_xoff * incxy;

		tmap->draw_roz(bitmap, cliprect, startx << 5,starty << 5,
				incxx << 5,incxy << 5,incyx << 5,incyy << 5,
				m_wrap,
				flags,priority);
	}

#if 0
if (machine.input().code_pressed(KEYCODE_D))
	popmessage("%04x %04x %04x %04x\n%04x %04x %04x %04x\n%04x %04x %04x %04x\n%04x %04x %04x %04x",
			m_ctrl[0x00],
			m_ctrl[0x01],
			m_ctrl[0x02],
			m_ctrl[0x03],
			m_ctrl[0x04],
			m_ctrl[0x05],
			m_ctrl[0x06],
			m_ctrl[0x07],
			m_ctrl[0x08],
			m_ctrl[0x09],
			m_ctrl[0x0a],
			m_ctrl[0x0b],
			m_ctrl[0x0c],
			m_ctrl[0x0d],
			m_ctrl[0x0e],
			m_ctrl[0x0f]);
#endif
}

/***************************************************************************/
/*                                                                         */
/*                                 053251                                  */
/*                                                                         */
/***************************************************************************/

struct k053251_state
{
	int      dirty_tmap[5];

	UINT8    ram[16];
	int      tilemaps_set;
	int      palette_index[5];
};

/*****************************************************************************
    INLINE FUNCTIONS
*****************************************************************************/

INLINE k053251_state *k053251_get_safe_token( device_t *device )
{
	assert(device != NULL);
	assert(device->type() == K053251);

	return (k053251_state *)downcast<k053251_device *>(device)->token();
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

WRITE8_DEVICE_HANDLER( k053251_w )
{
	k053251_state *k053251 = k053251_get_safe_token(device);
	int i, newind;

	data &= 0x3f;

	if (k053251->ram[offset] != data)
	{
		k053251->ram[offset] = data;
		if (offset == 9)
		{
			/* palette base index */
			for (i = 0; i < 3; i++)
			{
				newind = 32 * ((data >> 2 * i) & 0x03);
				if (k053251->palette_index[i] != newind)
				{
					k053251->palette_index[i] = newind;
					k053251->dirty_tmap[i] = 1;
				}
			}

			if (!k053251->tilemaps_set)
				space.machine().tilemap().mark_all_dirty();
		}
		else if (offset == 10)
		{
			/* palette base index */
			for (i = 0; i < 2; i++)
			{
				newind = 16 * ((data >> 3 * i) & 0x07);
				if (k053251->palette_index[3 + i] != newind)
				{
					k053251->palette_index[3 + i] = newind;
					k053251->dirty_tmap[3 + i] = 1;
				}
			}

			if (!k053251->tilemaps_set)
				space.machine().tilemap().mark_all_dirty();
		}
	}
}

WRITE16_DEVICE_HANDLER( k053251_lsb_w )
{
	if (ACCESSING_BITS_0_7)
		k053251_w(device, space, offset, data & 0xff);
}

WRITE16_DEVICE_HANDLER( k053251_msb_w )
{
	if (ACCESSING_BITS_8_15)
		k053251_w(device, space, offset, (data >> 8) & 0xff);
}

int k053251_get_priority( device_t *device, int ci )
{
	k053251_state *k053251 = k053251_get_safe_token(device);
	return k053251->ram[ci];
}

int k053251_get_palette_index( device_t *device, int ci )
{
	k053251_state *k053251 = k053251_get_safe_token(device);
	return k053251->palette_index[ci];
}

int k053251_get_tmap_dirty( device_t *device, int tmap_num )
{
	k053251_state *k053251 = k053251_get_safe_token(device);
	assert(tmap_num < 5);
	return k053251->dirty_tmap[tmap_num];
}

void k053251_set_tmap_dirty( device_t *device, int tmap_num, int data )
{
	k053251_state *k053251 = k053251_get_safe_token(device);
	assert(tmap_num < 5);
	k053251->dirty_tmap[tmap_num] = data ? 1 : 0;
}

static void k053251_reset_indexes(k053251_state *k053251)
{
	k053251->palette_index[0] = 32 * ((k053251->ram[9] >> 0) & 0x03);
	k053251->palette_index[1] = 32 * ((k053251->ram[9] >> 2) & 0x03);
	k053251->palette_index[2] = 32 * ((k053251->ram[9] >> 4) & 0x03);
	k053251->palette_index[3] = 16 * ((k053251->ram[10] >> 0) & 0x07);
	k053251->palette_index[4] = 16 * ((k053251->ram[10] >> 3) & 0x07);
}


/*****************************************************************************
    DEVICE INTERFACE
*****************************************************************************/

static DEVICE_START( k053251 )
{
	k053251_state *k053251 = k053251_get_safe_token(device);

	device->save_item(NAME(k053251->ram));
	device->save_item(NAME(k053251->tilemaps_set));
	device->save_item(NAME(k053251->dirty_tmap));

	device->machine().save().register_postload(save_prepost_delegate(FUNC(k053251_reset_indexes), k053251));
}

static DEVICE_RESET( k053251 )
{
	k053251_state *k053251 = k053251_get_safe_token(device);
	int i;

	k053251->tilemaps_set = 0;

	for (i = 0; i < 0x10; i++)
		k053251->ram[i] = 0;

	for (i = 0; i < 5; i++)
		k053251->dirty_tmap[i] = 0;
}

const device_type K053251 = &device_creator<k053251_device>;

k053251_device::k053251_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K053251, "Konami 053251", tag, owner, clock, "k053251", __FILE__)
{
	m_token = global_alloc_clear(k053251_state);
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k053251_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k053251_device::device_start()
{
	DEVICE_START_NAME( k053251 )(this);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k053251_device::device_reset()
{
	DEVICE_RESET_NAME( k053251 )(this);
}

/***************************************************************************/
/*                                                                         */
/*                                 054000                                  */
/*                                                                         */
/***************************************************************************/

const device_type K054000 = &device_creator<k054000_device>;

k054000_device::k054000_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K054000, "Konami 054000", tag, owner, clock, "k054000", __FILE__)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k054000_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k054000_device::device_start()
{
	save_item(NAME(m_regs));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k054000_device::device_reset()
{
	int i;

	for (i = 0; i < 0x20; i++)
		m_regs[i] = 0;
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

WRITE8_MEMBER( k054000_device::write )
{
	//logerror("%04x: write %02x to 054000 address %02x\n",space.device().safe_pc(),data,offset);
	m_regs[offset] = data;
}

READ8_MEMBER( k054000_device::read )
{
	int Acx, Acy, Aax, Aay;
	int Bcx, Bcy, Bax, Bay;

	//logerror("%04x: read 054000 address %02x\n", space.device().safe_pc(), offset);

	if (offset != 0x18)
		return 0;

	Acx = (m_regs[0x01] << 16) | (m_regs[0x02] << 8) | m_regs[0x03];
	Acy = (m_regs[0x09] << 16) | (m_regs[0x0a] << 8) | m_regs[0x0b];

	/* TODO: this is a hack to make thndrx2 pass the startup check. It is certainly wrong. */
	if (m_regs[0x04] == 0xff)
		Acx+=3;
	if (m_regs[0x0c] == 0xff)
		Acy+=3;

	Aax = m_regs[0x06] + 1;
	Aay = m_regs[0x07] + 1;

	Bcx = (m_regs[0x15] << 16) | (m_regs[0x16] << 8) | m_regs[0x17];
	Bcy = (m_regs[0x11] << 16) | (m_regs[0x12] << 8) | m_regs[0x13];
	Bax = m_regs[0x0e] + 1;
	Bay = m_regs[0x0f] + 1;

	if (Acx + Aax < Bcx - Bax)
		return 1;

	if (Bcx + Bax < Acx - Aax)
		return 1;

	if (Acy + Aay < Bcy - Bay)
		return 1;

	if (Bcy + Bay < Acy - Aay)
		return 1;

	return 0;
}

READ16_MEMBER( k054000_device::lsb_r )
{
	return read(space, offset);
}

WRITE16_MEMBER( k054000_device::lsb_w )
{
	if (ACCESSING_BITS_0_7)
		write(space, offset, data & 0xff);
}

/***************************************************************************/
/*                                                                         */
/*                                 051733                                  */
/*                                                                         */
/***************************************************************************/

const device_type K051733 = &device_creator<k051733_device>;

k051733_device::k051733_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K051733, "Konami 051733", tag, owner, clock, "k051733", __FILE__),
	//m_ram[0x20],
	m_rng(0)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k051733_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k051733_device::device_start()
{
	save_item(NAME(m_ram));
	save_item(NAME(m_rng));	
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k051733_device::device_reset()
{
	int i;

	for (i = 0; i < 0x20; i++)
		m_ram[i] = 0;

	m_rng = 0;
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

WRITE8_MEMBER( k051733_device::write )
{
	//logerror("%04x: write %02x to 051733 address %02x\n", space.device().safe_pc(), data, offset);

	m_ram[offset] = data;
}


static int k051733_int_sqrt( UINT32 op )
{
	UINT32 i = 0x8000;
	UINT32 step = 0x4000;

	while (step)
	{
		if (i * i == op)
			return i;
		else if (i * i > op)
			i -= step;
		else
			i += step;
		step >>= 1;
	}
	return i;
}

READ8_MEMBER( k051733_device::read )
{
	int op1 = (m_ram[0x00] << 8) | m_ram[0x01];
	int op2 = (m_ram[0x02] << 8) | m_ram[0x03];
	int op3 = (m_ram[0x04] << 8) | m_ram[0x05];

	int rad = (m_ram[0x06] << 8) | m_ram[0x07];
	int yobj1c = (m_ram[0x08] << 8) | m_ram[0x09];
	int xobj1c = (m_ram[0x0a] << 8) | m_ram[0x0b];
	int yobj2c = (m_ram[0x0c] << 8) | m_ram[0x0d];
	int xobj2c = (m_ram[0x0e] << 8) | m_ram[0x0f];

	switch (offset)
	{
		case 0x00:
			if (op2)
				return (op1 / op2) >> 8;
			else
				return 0xff;
		case 0x01:
			if (op2)
				return (op1 / op2) & 0xff;
			else
				return 0xff;

		/* this is completely unverified */
		case 0x02:
			if (op2)
				return (op1 % op2) >> 8;
			else
				return 0xff;
		case 0x03:
			if (op2)
				return (op1 % op2) & 0xff;
			else
				return 0xff;

		case 0x04:
			return k051733_int_sqrt(op3 << 16) >> 8;

		case 0x05:
			return k051733_int_sqrt(op3 << 16) & 0xff;

		case 0x06:
			m_rng += m_ram[0x13];
			return m_rng; //RNG read, used by Chequered Flag for differentiate cars, implementation is a raw guess

		case 0x07:{ /* note: Chequered Flag definitely wants all these bits to be enabled */
			if (xobj1c + rad < xobj2c)
				return 0xff;

			if (xobj2c + rad < xobj1c)
				return 0xff;

			if (yobj1c + rad < yobj2c)
				return 0xff;

			if (yobj2c + rad < yobj1c)
				return 0xff;

			return 0;
		}
		case 0x0e: /* best guess */
			return (xobj2c - xobj1c) >> 8;
		case 0x0f:
			return (xobj2c - xobj1c) & 0xff;
		default:
			return m_ram[offset];
	}
}


/***************************************************************************/
/*                                                                         */
/*                                 054157 / 056832                         */
/*                                                                         */
/***************************************************************************/

#define K056832_PAGE_COLS 64
#define K056832_PAGE_ROWS 32
#define K056832_PAGE_HEIGHT (K056832_PAGE_ROWS*8)
#define K056832_PAGE_WIDTH  (K056832_PAGE_COLS*8)




const device_type K056832 = &device_creator<k056832_device>;

k056832_device::k056832_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K056832, "Konami 056832", tag, owner, clock, "k056832", __FILE__),
	//m_tilemap[K056832_PAGE_COUNT],
	//*m_pixmap[K056832_PAGE_COUNT],
    //m_regs[0x20],
	//m_regsb[4],
	m_rombase(NULL),
	m_videoram(NULL),
	m_num_gfx_banks(0),
	m_cur_gfx_banks(0),
	m_rom_half(0),
    //m_layer_assoc_with_page[K056832_PAGE_COUNT],
	//m_layer_offs[8][2],
	//m_lsram_page[8][2],
	//m_x[8],
	//m_y[8],
	//m_w[8],
	//m_h[8],
	//m_dx[8],
	//m_dy[8],
	//m_line_dirty[K056832_PAGE_COUNT][8],
	//m_all_lines_dirty[K056832_PAGE_COUNT],
	//m_page_tile_mode[K056832_PAGE_COUNT],
	//m_last_colorbase[K056832_PAGE_COUNT],
	//m_layer_tile_mode[8],
	m_default_layer_association(0),
	m_layer_association(0),
	m_active_layer(0),
	m_selected_page(0),
	m_selected_page_x4096(0),
	m_linemap_enabled(0),
	m_use_ext_linescroll(0),
	m_uses_tile_banks(0),
	m_cur_tile_bank(0)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k056832_device::device_config_complete()
{
	// inherit a copy of the static data
	const k056832_interface *intf = reinterpret_cast<const k056832_interface *>(static_config());
	if (intf != NULL)
	*static_cast<k056832_interface *>(this) = *intf;
	
	// or initialize to defaults if none provided
	else
	{
	m_gfx_memory_region = "";
	m_gfx_num = 0;
	m_bpp = 0;
	m_big = 0;
	m_djmain_hack = 0;
	m_deinterleave = 0;
	m_callback = NULL;
	m_k055555_tag = "";
	};
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k056832_device::device_start()
{
/* TODO: understand which elements MUST be init here (to keep correct layer
   associations) and which ones can can be init at RESET, if any */
	tilemap_t *tmap;
	int i;
	UINT32 total;
	static const gfx_layout charlayout8 =
	{
		8, 8,
		0,
		8,
		{ 8*7,8*3,8*5,8*1,8*6,8*2,8*4,8*0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7 },
		{ 0, 8*8, 8*8*2, 8*8*3, 8*8*4, 8*8*5, 8*8*6, 8*8*7 },
		8*8*8
	};
	static const gfx_layout charlayout8le =
	{
		8, 8,
		0,
		8,
//      { 0, 1, 2, 3, 0+(0x200000*8), 1+(0x200000*8), 2+(0x200000*8), 3+(0x200000*8) },
		{ 0+(0x200000*8), 1+(0x200000*8), 2+(0x200000*8), 3+(0x200000*8), 0, 1, 2, 3 },
		{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4 },
		{ 0*8*4, 1*8*4, 2*8*4, 3*8*4, 4*8*4, 5*8*4, 6*8*4, 7*8*4 },
		8*8*4
	};
	static const gfx_layout charlayout6 =
	{
		8, 8,
		0,
		6,
		{ 40, 32, 24, 8, 16, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7 },
		{ 0, 6*8, 6*8*2, 6*8*3, 6*8*4, 6*8*5, 6*8*6, 6*8*7 },
		8*8*6
	};
	static const gfx_layout charlayout5 =
	{
		8, 8,
		0,
		5,
		{ 32, 24, 8, 16, 0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7 },
		{ 0, 5*8, 5*8*2, 5*8*3, 5*8*4, 5*8*5, 5*8*6, 5*8*7 },
		8*8*5
	};
	static const gfx_layout charlayout4 =
	{
		8, 8,
		0,
		4,
		{ 0, 1, 2, 3 },
		{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4 },
		{ 0*8*4, 1*8*4, 2*8*4, 3*8*4, 4*8*4, 5*8*4, 6*8*4, 7*8*4 },
		8*8*4
	};
	static const gfx_layout charlayout4dj =
	{
		8, 8,
		0,
		4,
		{ 8*3,8*1,8*2,8*0 },
		{ 0, 1, 2, 3, 4, 5, 6, 7 },
		{ 0, 8*4, 8*4*2, 8*4*3, 8*4*4, 8*4*5, 8*4*6, 8*4*7 },
		8*8*4
	};
	static const gfx_layout charlayout8_tasman =
	{
		8,8,
		RGN_FRAC(1,1),
		8,
		{ 0,8,16,24,32,40,48,56 },
		{ 0,1,2,3,4,5,6,7 },    // bit order probably not exact - note ramp in first 16 tiles
		{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64},
		8*64
	};


	/* handle the various graphics formats */
	i = (m_big) ? 8 : 16;


	/* decode the graphics */
	switch (m_bpp)
	{
		case K056832_BPP_4:
			total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / (i * 4);
			decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &charlayout4, 4);
			break;

		case K056832_BPP_5:
			total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / (i * 5);
			decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &charlayout5, 4);
			break;

		case K056832_BPP_6:
			total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / (i * 6);
			decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &charlayout6, 4);
			break;

		case K056832_BPP_8:
			total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / (i * 8);
			decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &charlayout8, 4);
			break;

		case K056832_BPP_8LE:
			total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / (i * 8);
			decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &charlayout8le, 4);
			break;

		case K056832_BPP_8TASMAN:
			total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / (i * 8);
			decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &charlayout8_tasman, 4);
			break;

		case K056832_BPP_4dj:
			total = machine().root_device().memregion(m_gfx_memory_region)->bytes() / (i * 4);
			decode_gfx(machine(), m_gfx_num, machine().root_device().memregion(m_gfx_memory_region)->base(), total, &charlayout4dj, 4);
			break;

		default:
			fatalerror("Unsupported bpp\n");
	}

	machine().gfx[m_gfx_num]->set_granularity(16); /* override */

	/* deinterleave the graphics, if needed */
	deinterleave_gfx(machine(), m_gfx_memory_region, m_deinterleave);

	m_rombase = machine().root_device().memregion(m_gfx_memory_region)->base();
	m_num_gfx_banks = machine().root_device().memregion(m_gfx_memory_region)->bytes() / 0x2000;
	
	m_cur_gfx_banks = 0;
	m_use_ext_linescroll = 0;
	m_uses_tile_banks = 0;

	for (i = 0; i < 4; i++)
	{
		m_layer_offs[i][0] = 0;
		m_layer_offs[i][1] = 0;
		m_lsram_page[i][0] = i;
		m_lsram_page[i][1] = i << 11;
		m_x[i] = 0;
		m_y[i] = 0;
		m_w[i] = 0;
		m_h[i] = 0;
		m_dx[i] = 0;
		m_dy[i] = 0;
		m_layer_tile_mode[i] = 1;
	}

	m_default_layer_association = 1;
	m_active_layer = 0;
	m_linemap_enabled = 0;

	m_k055555 = machine().device(m_k055555_tag);

	memset(m_line_dirty, 0, sizeof(UINT32) * K056832_PAGE_COUNT * 8);

	for (i = 0; i < K056832_PAGE_COUNT; i++)
	{
		m_all_lines_dirty[i] = 0;
		m_page_tile_mode[i] = 1;
	}

	m_videoram = auto_alloc_array_clear(machine(), UINT16, 0x2000 * (K056832_PAGE_COUNT + 1) / 2);

	m_tilemap[0x0] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_info0),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0x1] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_info1),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0x2] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_info2),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0x3] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_info3),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0x4] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_info4),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0x5] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_info5),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0x6] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_info6),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0x7] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_info7),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0x8] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_info8),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0x9] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_info9),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0xa] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_infoa),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0xb] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_infob),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0xc] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_infoc),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0xd] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_infod),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0xe] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_infoe),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);
	m_tilemap[0xf] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k056832_device::get_tile_infof),this), TILEMAP_SCAN_ROWS,  8, 8, 64, 32);

	for (i = 0; i < K056832_PAGE_COUNT; i++)
	{
		tmap = m_tilemap[i];

		m_pixmap[i] = &tmap->pixmap();

		tmap->set_transparent_pen(0);
	}

	memset(m_videoram, 0x00, 0x20000);
	memset(m_regs,     0x00, sizeof(m_regs) );
	memset(m_regsb,    0x00, sizeof(m_regsb) );

	update_page_layout();

	change_rambank();
	change_rombank();

	save_pointer(NAME(m_videoram), 0x10000);
	save_item(NAME(m_regs));
	save_item(NAME(m_regsb));
	save_item(NAME(m_x));
	save_item(NAME(m_y));
	save_item(NAME(m_w));
	save_item(NAME(m_h));
	save_item(NAME(m_dx));
	save_item(NAME(m_dy));
	save_item(NAME(m_layer_tile_mode));

	save_item(NAME(m_default_layer_association));
	save_item(NAME(m_active_layer));
	save_item(NAME(m_linemap_enabled));
	save_item(NAME(m_use_ext_linescroll));
	save_item(NAME(m_uses_tile_banks));
	save_item(NAME(m_cur_tile_bank));
	save_item(NAME(m_rom_half));
	save_item(NAME(m_all_lines_dirty));
	save_item(NAME(m_page_tile_mode));

	for (i = 0; i < 8; i++)
	{
		save_item(NAME(m_layer_offs[i]), i);
		save_item(NAME(m_lsram_page[i]), i);
	}

	for (i = 0; i < K056832_PAGE_COUNT; i++)
	{
		save_item(NAME(m_line_dirty[i]), i);
		save_item(NAME(m_all_lines_dirty[i]), i);
		save_item(NAME(m_page_tile_mode[i]), i);
		save_item(NAME(m_last_colorbase[i]), i);
	}

	machine().save().register_postload(save_prepost_delegate(FUNC(k056832_device::postload), this));
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

#define mark_line_dirty(P, L) if (L < 0x100) m_line_dirty[P][L >> 5] |= 1 << (L & 0x1f)
#define mark_all_lines_dirty(P) m_all_lines_dirty[P] = 1

void k056832_device::mark_page_dirty( int page )
{
	if (m_page_tile_mode[page])
		m_tilemap[page]->mark_all_dirty();
	else
		mark_all_lines_dirty(page);
}

void k056832_device::mark_plane_dirty( int layer )
{
	int tilemode, i;

	tilemode = m_layer_tile_mode[layer];

	for (i = 0; i < K056832_PAGE_COUNT; i++)
	{
		if (m_layer_assoc_with_page[i] == layer)
		{
			m_page_tile_mode[i] = tilemode;
			mark_page_dirty(i);
		}
	}
}

void k056832_device::mark_all_tilemaps_dirty( )
{
	int i;

	for (i = 0; i < K056832_PAGE_COUNT; i++)
	{
		if (m_layer_assoc_with_page[i] != -1)
		{
			m_page_tile_mode[i] = m_layer_tile_mode[m_layer_assoc_with_page[i]];
			mark_page_dirty(i);
		}
	}
}

/* moo.c needs to call this in its VIDEO_UPDATE */
void k056832_device::mark_all_tmaps_dirty( )
{
	mark_all_tilemaps_dirty();
}

void k056832_device::update_page_layout( )
{
	int layer, rowstart, rowspan, colstart, colspan, r, c, page_idx, setlayer;

	// enable layer association by default
	m_layer_association = m_default_layer_association;

	// disable association if a layer grabs the entire 4x4 map (happens in Twinbee and Dadandarn)
	for (layer = 0; layer < 4; layer++)
	{
		if (!m_y[layer] && !m_x[layer] && m_h[layer] == 3 && m_w[layer] == 3)
		{
			m_layer_association = 0;
			break;
		}
	}

	// winning spike doesn't like layer association..
	if (m_djmain_hack == 2)
		m_layer_association = 0;

	// disable all tilemaps
	for (page_idx = 0; page_idx < K056832_PAGE_COUNT; page_idx++)
	{
		m_layer_assoc_with_page[page_idx] = -1;
	}


	// enable associated tilemaps
	for (layer = 0; layer < 4; layer++)
	{
		rowstart = m_y[layer];
		colstart = m_x[layer];
		rowspan  = m_h[layer] + 1;
		colspan  = m_w[layer] + 1;

		setlayer = (m_layer_association) ? layer : m_active_layer;

		for (r = 0; r < rowspan; r++)
		{
			for (c = 0; c < colspan; c++)
			{
				page_idx = (((rowstart + r) & 3) << 2) + ((colstart + c) & 3);
				if (!(m_djmain_hack==1) || m_layer_assoc_with_page[page_idx] == -1)
					m_layer_assoc_with_page[page_idx] = setlayer;
			}
		}
	}

	// refresh associated tilemaps
	mark_all_tilemaps_dirty();
}

int k056832_device::get_lookup( int bits )
{
	int res;

	res = (m_regs[0x1c] >> (bits << 2)) & 0x0f;

	if (m_uses_tile_banks)   /* Asterix */
		res |= m_cur_tile_bank << 4;

	return res;
}

void k056832_device::get_tile_info(  tile_data &tileinfo, int tile_index, int pageIndex )
{
	static const struct K056832_SHIFTMASKS
	{
		int flips, palm1, pals2, palm2;
	}
	k056832_shiftmasks[4] = {{6, 0x3f, 0, 0x00}, {4, 0x0f, 2, 0x30}, {2, 0x03, 2, 0x3c}, {0, 0x00, 2, 0x3f}};

	const struct K056832_SHIFTMASKS *smptr;
	int layer, flip, fbits, attr, code, color, flags;
	UINT16 *pMem;

	pMem  = &m_videoram[(pageIndex << 12) + (tile_index << 1)];

	if (m_layer_association)
	{
		layer = m_layer_assoc_with_page[pageIndex];
		if (layer == -1)
			layer = 0;  // use layer 0's palette info for unmapped pages
	}
	else
		layer = m_active_layer;

	fbits = (m_regs[3] >> 6) & 3;
	flip  = (m_regs[1] >> (layer << 1)) & 0x3; // tile-flip override (see p.20 3.2.2 "REG2")
	smptr = &k056832_shiftmasks[fbits];
	attr  = pMem[0];
	code  = pMem[1];

	// normalize the flip/palette flags
	// see the tables on pages 4 and 10 of the Pt. 2-3 "VRAM" manual
	// for a description of these bits "FBIT0" and "FBIT1"
	flip &= attr >> smptr->flips & 3;
	color = (attr & smptr->palm1) | (attr >> smptr->pals2 & smptr->palm2);
	flags = TILE_FLIPYX(flip);

	m_callback(machine(), layer, &code, &color, &flags);

	SET_TILE_INFO_MEMBER(
			m_gfx_num,
			code,
			color,
			flags);
}

TILE_GET_INFO_MEMBER(k056832_device::get_tile_info0) { get_tile_info(tileinfo, tile_index, 0x0); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_info1) { get_tile_info(tileinfo, tile_index, 0x1); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_info2) { get_tile_info(tileinfo, tile_index, 0x2); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_info3) { get_tile_info(tileinfo, tile_index, 0x3); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_info4) { get_tile_info(tileinfo, tile_index, 0x4); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_info5) { get_tile_info(tileinfo, tile_index, 0x5); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_info6) { get_tile_info(tileinfo, tile_index, 0x6); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_info7) { get_tile_info(tileinfo, tile_index, 0x7); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_info8) { get_tile_info(tileinfo, tile_index, 0x8); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_info9) { get_tile_info(tileinfo, tile_index, 0x9); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_infoa) { get_tile_info(tileinfo, tile_index, 0xa); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_infob) { get_tile_info(tileinfo, tile_index, 0xb); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_infoc) { get_tile_info(tileinfo, tile_index, 0xc); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_infod) { get_tile_info(tileinfo, tile_index, 0xd); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_infoe) { get_tile_info(tileinfo, tile_index, 0xe); }
TILE_GET_INFO_MEMBER(k056832_device::get_tile_infof) { get_tile_info(tileinfo, tile_index, 0xf); }

void k056832_device::change_rambank( )
{
	/* ------xx page col
	 * ---xx--- page row
	 */
	int bank = m_regs[0x19];

	if (m_regs[0] & 0x02)    // external linescroll enable
		m_selected_page = K056832_PAGE_COUNT;
	else
		m_selected_page = ((bank >> 1) & 0xc) | (bank & 3);

	m_selected_page_x4096 = m_selected_page << 12;

	// refresh associated tilemaps
	mark_all_tilemaps_dirty();
}

int k056832_device::get_current_rambank( )
{
	int bank = m_regs[0x19];

	return ((bank >> 1) & 0xc) | (bank & 3);
}

void k056832_device::change_rombank( )
{
	int bank;

	if (m_uses_tile_banks)   /* Asterix */
		bank = (m_regs[0x1a] >> 8) | (m_regs[0x1b] << 4) | (m_cur_tile_bank << 6);
	else
		bank = m_regs[0x1a] | (m_regs[0x1b] << 16);

	m_cur_gfx_banks = bank % m_num_gfx_banks;
}

void k056832_device::set_tile_bank( int bank )
{
	m_uses_tile_banks = 1;

	if (m_cur_tile_bank != bank)
	{
		m_cur_tile_bank = bank;

		mark_plane_dirty(0);
		mark_plane_dirty(1);
		mark_plane_dirty(2);
		mark_plane_dirty(3);
	}

	change_rombank();
}

/* call if a game uses external linescroll */
void k056832_device::SetExtLinescroll( )
{
	m_use_ext_linescroll = 1;
}

/* generic helper routine for ROM checksumming */
int k056832_device::rom_read_b( int offset, int blksize, int blksize2, int zerosec )
{
	UINT8 *rombase;
	int base, ret;

	rombase = (UINT8 *)machine().root_device().memregion(m_gfx_memory_region)->base();

	if ((m_rom_half) && (zerosec))
	{
		return 0;
	}

	// add in the bank offset
	offset += (m_cur_gfx_banks * 0x2000);

	// figure out the base of the ROM block
	base = (offset / blksize) * blksize2;

	// get the starting offset of the proper word inside the block
	base += (offset % blksize) * 2;

	if (m_rom_half)
	{
		ret = rombase[base + 1];
	}
	else
	{
		ret = rombase[base];
		m_rom_half = 1;
	}

	return ret;
}

READ16_MEMBER( k056832_device::k_5bpp_rom_word_r )
{
	if (mem_mask == 0xff00)
		return rom_read_b(offset * 2, 4, 5, 0)<<8;
	else if (mem_mask == 0x00ff)
		return rom_read_b(offset * 2 + 1, 4, 5, 0)<<16;
	else
	{
		//LOG(("Non-byte read of tilemap ROM, PC=%x (mask=%x)\n", space.device().safe_pc(), mem_mask));
	}
	return 0;
}

READ32_MEMBER( k056832_device::k_5bpp_rom_long_r )
{
	if (mem_mask == 0xff000000)
		return rom_read_b(offset * 4, 4, 5, 0) << 24;
	else if (mem_mask == 0x00ff0000)
		return rom_read_b(offset * 4 + 1, 4, 5, 0) << 16;
	else if (mem_mask == 0x0000ff00)
		return rom_read_b(offset * 4 + 2, 4, 5, 0) << 8;
	else if (mem_mask == 0x000000ff)
		return rom_read_b(offset * 4 + 3, 4, 5, 1);
	else
	{
		//LOG(("Non-byte read of tilemap ROM, PC=%x (mask=%x)\n", space.device().safe_pc(), mem_mask));
	}
	return 0;
}

READ32_MEMBER( k056832_device::k_6bpp_rom_long_r )
{
	if (mem_mask == 0xff000000)
		return rom_read_b(offset * 4, 4, 6, 0) << 24;
	else if (mem_mask == 0x00ff0000)
		return rom_read_b(offset * 4 + 1, 4, 6, 0) << 16;
	else if (mem_mask == 0x0000ff00)
		return rom_read_b(offset * 4 + 2, 4, 6, 0) << 8;
	else if (mem_mask == 0x000000ff)
		return rom_read_b(offset * 4 + 3, 4, 6, 0);
	else
	{
		//LOG(("Non-byte read of tilemap ROM, PC=%x (mask=%x)\n", space.device().safe_pc(), mem_mask));
	}
	return 0;
}

READ16_MEMBER( k056832_device::rom_word_r )
{
	int addr = 0x2000 * m_cur_gfx_banks + 2 * offset;

	if (!m_rombase)
		m_rombase = space.machine().root_device().memregion(m_gfx_memory_region)->base();

	return m_rombase[addr + 1] | (m_rombase[addr] << 8);
}

// data is arranged like this:
// 0000 1111 22 0000 1111 22
READ16_MEMBER( k056832_device::mw_rom_word_r )
{
	int bank = 10240 * m_cur_gfx_banks;
	int addr;

	if (!m_rombase)
		m_rombase = space.machine().root_device().memregion(m_gfx_memory_region)->base();

	if (m_regsb[2] & 0x8)
	{
		// we want only the 2s
		int bit;
		int res, temp;

		bit = offset % 4;
		addr = (offset / 4) * 5;

		temp = m_rombase[addr + 4 + bank];

		switch (bit)
		{
			default:
			case 0:
				res = (temp & 0x80) << 5;
				res |= ((temp & 0x40) >> 2);
				break;

			case 1:
				res = (temp & 0x20) << 7;
				res |= (temp & 0x10);
				break;

			case 2:
				res = (temp & 0x08) << 9;
				res |= ((temp & 0x04) << 2);
				break;

			case 3:
				res = (temp & 0x02) << 11;
				res |= ((temp & 0x01) << 4);
				break;
		}

		return res;
	}
	else
	{
		// we want only the 0s and 1s.

		addr = (offset >> 1) * 5;

		if (offset & 1)
		{
			addr += 2;
		}

		addr += bank;

		return m_rombase[addr + 1] | (m_rombase[addr] << 8);
	}

}

READ16_MEMBER( k056832_device::bishi_rom_word_r )
{
	int addr = 0x4000 * m_cur_gfx_banks + offset;

	if (!m_rombase)
		m_rombase = space.machine().root_device().memregion(m_gfx_memory_region)->base();

	return m_rombase[addr + 2] | (m_rombase[addr] << 8);
}

READ16_MEMBER( k056832_device::rom_word_8000_r )
{
	int addr = 0x8000 * m_cur_gfx_banks + 2 * offset;

	if (!m_rombase)
		m_rombase = space.machine().root_device().memregion(m_gfx_memory_region)->base();

	return m_rombase[addr + 2] | (m_rombase[addr] << 8);
}

READ16_MEMBER( k056832_device::old_rom_word_r )
{
	int addr = 0x2000 * m_cur_gfx_banks + 2 * offset;

	if (!m_rombase)
		m_rombase = space.machine().root_device().memregion(m_gfx_memory_region)->base();

	return m_rombase[addr + 1] | (m_rombase[addr] << 8);
}

READ32_MEMBER( k056832_device::rom_long_r )
{
	offset <<= 1;
	return (rom_word_r(space, offset + 1, 0xffff) | (rom_word_r(space, offset, 0xffff) << 16));
}

/* only one page is mapped to videoram at a time through a window */
READ16_MEMBER( k056832_device::ram_word_r )
{
	// reading from tile RAM resets the ROM readback "half" offset
	m_rom_half = 0;

	return m_videoram[m_selected_page_x4096 + offset];
}

READ16_MEMBER( k056832_device::ram_half_word_r )
{
	return m_videoram[m_selected_page_x4096 + (((offset << 1) & 0xffe) | ((offset >> 11) ^ 1))];
}

READ32_MEMBER( k056832_device::ram_long_r )
{
	UINT16 *pMem = &m_videoram[m_selected_page_x4096 + offset * 2];

	// reading from tile RAM resets the ROM readback "half" offset
	m_rom_half = 0;

	return (pMem[0]<<16 | pMem[1]);
}

READ32_MEMBER( k056832_device::unpaged_ram_long_r )
{
	UINT16 *pMem = &m_videoram[offset * 2];

	// reading from tile RAM resets the ROM readback "half" offset
	m_rom_half = 0;

	return (pMem[0]<<16 | pMem[1]);
}

/* special 8-bit handlers for Lethal Enforcers */
READ8_MEMBER( k056832_device::ram_code_lo_r )
{
	UINT16 *adr = &m_videoram[m_selected_page_x4096 + (offset * 2) + 1];

	return *adr & 0xff;
}

READ8_MEMBER( k056832_device::ram_code_hi_r )
{
	UINT16 *adr = &m_videoram[m_selected_page_x4096 + (offset * 2) + 1];

	return *adr >> 8;
}

READ8_MEMBER( k056832_device::ram_attr_lo_r )
{
	UINT16 *adr = &m_videoram[m_selected_page_x4096 + (offset * 2)];

	return *adr & 0xff;
}

READ8_MEMBER( k056832_device::ram_attr_hi_r )
{
	UINT16 *adr = &m_videoram[m_selected_page_x4096 + (offset * 2)];

	return *adr >> 8;
}

WRITE8_MEMBER( k056832_device::ram_code_lo_w )
{
	UINT16 *adr = &m_videoram[m_selected_page_x4096 + (offset * 2) + 1];

	*adr &= 0xff00;
	*adr |= data;

	if (!(m_regs[0] & 0x02)) // external linescroll enable
	{
		if (m_page_tile_mode[m_selected_page])
			m_tilemap[m_selected_page]->mark_tile_dirty(offset);
		else
			mark_line_dirty(m_selected_page, offset);
	}
}

WRITE8_MEMBER( k056832_device::ram_code_hi_w )
{
	UINT16 *adr = &m_videoram[m_selected_page_x4096 + (offset * 2) + 1];

	*adr &= 0x00ff;
	*adr |= data << 8;

	if (!(m_regs[0] & 0x02)) // external linescroll enable
	{
		if (m_page_tile_mode[m_selected_page])
			m_tilemap[m_selected_page]->mark_tile_dirty(offset);
		else
			mark_line_dirty(m_selected_page, offset);
	}
}

WRITE8_MEMBER( k056832_device::ram_attr_lo_w )
{
	UINT16 *adr = &m_videoram[m_selected_page_x4096 + (offset * 2)];

	*adr &= 0xff00;
	*adr |= data;

	if (!(m_regs[0] & 0x02)) // external linescroll enable
	{
		if (m_page_tile_mode[m_selected_page])
			m_tilemap[m_selected_page]->mark_tile_dirty(offset);
		else
			mark_line_dirty(m_selected_page, offset);
	}
}

WRITE8_MEMBER( k056832_device::ram_attr_hi_w )
{
	UINT16 *adr = &m_videoram[m_selected_page_x4096 + (offset * 2)];

	*adr &= 0x00ff;
	*adr |= data << 8;

	if (!(m_regs[0] & 0x02)) // external linescroll enable
	{
		if (m_page_tile_mode[m_selected_page])
			m_tilemap[m_selected_page]->mark_tile_dirty(offset);
		else
			mark_line_dirty(m_selected_page, offset);
	}
}

WRITE16_MEMBER( k056832_device::ram_word_w )
{
	UINT16 *tile_ptr;
	UINT16 old_mask, old_data;

	tile_ptr = &m_videoram[m_selected_page_x4096 + offset];
	old_mask = ~mem_mask;
	old_data = *tile_ptr;
	data = (data & mem_mask) | (old_data & old_mask);

	if(data != old_data)
	{
		offset >>= 1;
		*tile_ptr = data;

		if (m_page_tile_mode[m_selected_page])
			m_tilemap[m_selected_page]->mark_tile_dirty(offset);
		else
			mark_line_dirty(m_selected_page, offset);
	}
}

WRITE16_MEMBER( k056832_device::ram_half_word_w )
{
	UINT16 *adr = &m_videoram[m_selected_page_x4096 + (((offset << 1) & 0xffe) | 1)];
	UINT16 old = *adr;

	COMBINE_DATA(adr);
	if(*adr != old)
	{
		int dofs = (((offset << 1) & 0xffe) | 1);

		dofs >>= 1;

		if (m_page_tile_mode[m_selected_page])
			m_tilemap[m_selected_page]->mark_tile_dirty(dofs);
		else
			mark_line_dirty(m_selected_page, dofs);
	}
}

WRITE32_MEMBER( k056832_device::ram_long_w )
{
	UINT16 *tile_ptr;
	UINT32 old_mask, old_data;

	tile_ptr = &m_videoram[m_selected_page_x4096 + offset * 2];
	old_mask = ~mem_mask;
	old_data = (UINT32)tile_ptr[0] << 16 | (UINT32)tile_ptr[1];
	data = (data & mem_mask) | (old_data & old_mask);

	if (data != old_data)
	{
		tile_ptr[0] = data >> 16;
		tile_ptr[1] = data;

		if (m_page_tile_mode[m_selected_page])
			m_tilemap[m_selected_page]->mark_tile_dirty(offset);
		else
			mark_line_dirty(m_selected_page, offset);
	}
}

WRITE32_MEMBER( k056832_device::unpaged_ram_long_w )
{
	UINT16 *tile_ptr;
	UINT32 old_mask, old_data;

	tile_ptr = &m_videoram[offset * 2];
	old_mask = ~mem_mask;
	old_data = (UINT32)tile_ptr[0] << 16 | (UINT32)tile_ptr[1];
	data = (data & mem_mask) | (old_data & old_mask);

	if (data != old_data)
	{
		tile_ptr[0] = data >> 16;
		tile_ptr[1] = data;

		if (m_page_tile_mode[offset/0x800])
			m_tilemap[offset/0x800]->mark_tile_dirty(offset&0x7ff);
		else
			mark_line_dirty(offset/0x800, (offset&0x7ff));
	}
}

WRITE16_MEMBER( k056832_device::word_w )
{
	int layer, flip, mask, i;
	UINT32 old_data, new_data;

	old_data = m_regs[offset];
	COMBINE_DATA(&m_regs[offset]);
	new_data = m_regs[offset];

	if (new_data != old_data)
	{
		switch(offset)
		{
			/* -x-- ---- dotclock select: 0=8Mhz, 1=6Mhz (not used by GX)
			 * --x- ---- screen flip y
			 * ---x ---- screen flip x
			 * ---- --x- external linescroll RAM page enable
			 */
			case 0x00/2:
				if ((new_data & 0x30) != (old_data & 0x30))
				{
					flip = 0;
					if (new_data & 0x20) flip |= TILEMAP_FLIPY;
					if (new_data & 0x10) flip |= TILEMAP_FLIPX;
					for (i = 0; i < K056832_PAGE_COUNT; i++)
					{
						m_tilemap[i]->set_flip(flip);
					}
				}

				if ((new_data & 0x02) != (old_data & 0x02))
				{
					change_rambank();
				}
			break;

			/* -------- -----xxx external irqlines enable (not used by GX)
			 * -------- xx------ tilemap attribute config (FBIT0 and FBIT1)
			 */
			//case 0x06/2: break;

			// -------- ----DCBA tile mode: 0=512x1, 1=8x8
			// -------- DCBA---- synchronous scroll: 0=off, 1=on
			case 0x08/2:
				for (layer = 0; layer < 4; layer++)
				{
					mask = 1 << layer;
					i = new_data & mask;
					if (i != (old_data & mask))
					{
						m_layer_tile_mode[layer] = i;
						mark_plane_dirty(layer);
					}
				}
			break;

			/* -------- ------xx layer A linescroll config
			 * -------- ----xx-- layer B linescroll config
			 * -------- --xx---- layer C linescroll config
			 * -------- xx------ layer D linescroll config
			 *
			 * 0: linescroll
			 * 2: rowscroll
			 * 3: xy scroll
			 */
			//case 0x0a/2: break;

			case 0x32/2:
				change_rambank();
			break;

			case 0x34/2: /* ROM bank select for checksum */
			case 0x36/2: /* secondary ROM bank select for use with tile banking */
				change_rombank();
			break;

			// extended tile address
			//case 0x38/2: break;

			// 12 bit (signed) horizontal offset if global HFLIP enabled
			//case 0x3a/2: break;

			// 11 bit (signed) vertical offset if global VFLIP enabled
			//case 0x3c/2: break;

			default:
				layer = offset & 3;

				if (offset >= 0x10/2 && offset <= 0x16/2)
				{
					m_y[layer] = (new_data & 0x18) >> 3;
					m_h[layer] = (new_data & 0x3);
					m_active_layer = layer;
					update_page_layout();
				} else

				if (offset >= 0x18/2 && offset <= 0x1e/2)
				{
					m_x[layer] = (new_data & 0x18) >> 3;
					m_w[layer] = (new_data & 0x03);
					m_active_layer = layer;
					update_page_layout();
				} else

				if (offset >= 0x20/2 && offset <= 0x26/2)
				{
					m_dy[layer] = (INT16)new_data;
				} else

				if (offset >= 0x28/2 && offset <= 0x2e/2)
				{
					m_dx[layer] = (INT16)new_data;
				}
			break;
		}
	}
}

WRITE32_MEMBER( k056832_device::long_w )
{
	// GX does access of all 3 widths (8/16/32) so we can't do the
	// if (ACCESSING_xxx) trick.  in particular, 8-bit writes
	// are used to the tilemap bank register.
	offset <<= 1;
	word_w(space, offset, data >> 16, mem_mask >> 16);
	word_w(space, offset + 1, data, mem_mask);
}

WRITE16_MEMBER( k056832_device::b_word_w )
{
	COMBINE_DATA(&m_regsb[offset]);
}

WRITE8_MEMBER( k056832_device::write )
{
	if (offset & 1)
	{
		word_w(space, (offset >> 1), data, 0x00ff);
	}
	else
	{
		word_w(space, (offset >> 1), data << 8, 0xff00);
	}
}

WRITE8_MEMBER( k056832_device::b_w )
{
	if (offset & 1)
	{
		b_word_w(space, (offset >> 1), data, 0x00ff);
	}
	else
	{
		b_word_w(space, (offset >> 1), data<<8, 0xff00);
	}
}

WRITE32_MEMBER( k056832_device::b_long_w )
{
	if (ACCESSING_BITS_16_31)
	{
		b_word_w(space, offset << 1, data >> 16, mem_mask >> 16);
	}
	if (ACCESSING_BITS_0_15)
	{
		b_word_w(space, (offset << 1) + 1, data, mem_mask);
	}
}

template<class _BitmapClass>
int k056832_device::update_linemap( _BitmapClass &bitmap, int page, int flags )
{
	if (m_page_tile_mode[page])
		return(0);
	if (!m_linemap_enabled)
		return(1);

	{
		rectangle zerorect;
		tilemap_t *tmap;
		UINT32 *dirty;
		int all_dirty;
		UINT8 *xprdata;

		tmap = m_tilemap[page];
		bitmap_ind8 &xprmap  = tmap->flagsmap();
		xprdata = tmap->tile_flags();

		dirty = m_line_dirty[page];
		all_dirty = m_all_lines_dirty[page];

		if (all_dirty)
		{
			dirty[7] = dirty[6] = dirty[5] = dirty[4] = dirty[3] = dirty[2] = dirty[1] = dirty[0] = 0;
			m_all_lines_dirty[page] = 0;

			// force tilemap into a clean, static state
			// *really ugly but it minimizes alteration to tilemap.c
			memset(&zerorect, 0, sizeof(rectangle));    // zero dimension
			tmap->draw(bitmap, zerorect, 0, 0); // dummy call to reset tile_dirty_map
			xprmap.fill(0);                     // reset pixel transparency_bitmap;
			memset(xprdata, TILEMAP_PIXEL_LAYER0, 0x800);   // reset tile transparency_data;
		}
		else
		{
			if (!(dirty[0] | dirty[1] | dirty[2] | dirty[3] | dirty[4] | dirty[5] | dirty[6] | dirty[7]))
				return 0;
		}

#if 0   /* this code is broken.. really broken .. gijoe uses it for some line/column scroll style effects (lift level of attract mode)
            we REALLY shouldn't be writing directly back into the pixmap, surely this should
            be done when rendering instead

        */
		{
			bitmap_ind16 *pixmap;
			running_machine &machine = device->machine();

			UINT8 code_transparent, code_opaque;
			const pen_t *pal_ptr;
			const UINT8  *src_ptr;
			UINT8  *xpr_ptr;
			UINT16 *dst_ptr;
			UINT16 pen, basepen;
			int count, src_pitch, src_modulo;
			int dst_pitch;
			int line;
			gfx_element *src_gfx;
			int offs, mask;

			#define LINE_WIDTH 512

			#define DRAW_PIX(N) \
				pen = src_ptr[N]; \
				if (pen) \
				{ pen += basepen; xpr_ptr[count+N] = TILEMAP_PIXEL_LAYER0; dst_ptr[count+N] = pen; } else \
				{ xpr_ptr[count+N] = 0; }

			pixmap  = m_pixmap[page];
			pal_ptr = machine.pens;
			src_gfx = machine.gfx[m_gfx_num];
			src_pitch  = src_gfx->rowbytes();
			src_modulo = src_gfx->char_modulo;
			dst_pitch  = pixmap->rowpixels;

			for (line = 0; line < 256; line++)
			{
				tile_data tileinfo = {0};

				dst_ptr = &pixmap->pix16(line);
				xpr_ptr = &xprmap.pix8(line);

				if (!all_dirty)
				{
					offs = line >> 5;
					mask = 1 << (line & 0x1f);
					if (!(dirty[offs] & mask)) continue;
					dirty[offs] ^= mask;
				}

				for (count = 0; count < LINE_WIDTH; count += 8)
				{
					get_tile_info(&tileinfo, line, page);
					basepen = tileinfo.palette_base;
					code_transparent = tileinfo.category;
					code_opaque = code_transparent | TILEMAP_PIXEL_LAYER0;

					src_ptr = tileinfo.pen_data + count * 8;//src_base + ((tileinfo.tile_number & ~7) << 6);

					DRAW_PIX(0)
					DRAW_PIX(1)
					DRAW_PIX(2)
					DRAW_PIX(3)
					DRAW_PIX(4)
					DRAW_PIX(5)
					DRAW_PIX(6)
					DRAW_PIX(7)
				}
			}

			#undef LINE_WIDTH
			#undef DRAW_PIX
		}
#endif

	}

	return(0);
}

template<class _BitmapClass>
void k056832_device::tilemap_draw_common( _BitmapClass &bitmap, const rectangle &cliprect, int layer, UINT32 flags, UINT32 priority )
{
	UINT32 last_dx, last_visible, new_colorbase, last_active;
	int sx, sy, ay, tx, ty, width, height;
	int clipw, clipx, cliph, clipy, clipmaxy;
	int line_height, line_endy, line_starty, line_y;
	int sdat_start, sdat_walk, sdat_adv, sdat_wrapmask, sdat_offs;
	int pageIndex, flipx, flipy, corr, r, c;
	int cminy, cmaxy, cminx, cmaxx;
	int dminy, dmaxy, dminx, dmaxx;
	rectangle drawrect;
	tilemap_t *tmap;
	UINT16 *p_scroll_data;
	UINT16 ram16[2];

	int rowstart = m_y[layer];
	int colstart = m_x[layer];
	int rowspan  = m_h[layer] + 1;
	int colspan  = m_w[layer] + 1;
	int dy = m_dy[layer];
	int dx = m_dx[layer];
	int scrollbank = ((m_regs[0x18] >> 1) & 0xc) | (m_regs[0x18] & 3);
	int scrollmode = m_regs[0x05] >> (m_lsram_page[layer][0] << 1) & 3;

	if (m_use_ext_linescroll)
	{
		scrollbank = K056832_PAGE_COUNT;
	}

	height = rowspan * K056832_PAGE_HEIGHT;
	width  = colspan * K056832_PAGE_WIDTH;

	cminx = cliprect.min_x;
	cmaxx = cliprect.max_x;
	cminy = cliprect.min_y;
	cmaxy = cliprect.max_y;

	// flip correction registers
	flipy = m_regs[0] & 0x20;
	if (flipy)
	{
		corr = m_regs[0x3c/2];
		if (corr & 0x400)
			corr |= 0xfffff800;
	}
	else
		corr = 0;

	dy += corr;
	ay = (unsigned)(dy - m_layer_offs[layer][1]) % height;

	flipx = m_regs[0] & 0x10;
	if (flipx)
	{
		corr = m_regs[0x3a/2];
		if (corr & 0x800)
			corr |= 0xfffff000;
	}
	else
		corr = 0;

	corr -= m_layer_offs[layer][0];

	switch( scrollmode )
	{
		case 0: // linescroll
			p_scroll_data = &m_videoram[scrollbank<<12] + (m_lsram_page[layer][1]>>1);
			line_height = 1;
			sdat_wrapmask = 0x3ff;
			sdat_adv = 2;
		break;
		case 2: // rowscroll
			p_scroll_data = &m_videoram[scrollbank << 12] + (m_lsram_page[layer][1] >> 1);
			line_height = 8;
			sdat_wrapmask = 0x3ff;
			sdat_adv = 16;
		break;
		default: // xyscroll
			p_scroll_data = ram16;
			line_height = K056832_PAGE_HEIGHT;
			sdat_wrapmask = 0;
			sdat_adv = 0;
			ram16[0] = 0;
			ram16[1] = dx;
	}
	if (flipy)
		sdat_adv = -sdat_adv;
/*
if (scrollmode==2)
{
printf("%08x    %08x    %08x\n",layer,scrollbank<<12,m_lsram_page[layer][1]>>1);
printf("\n000-100:\n");
for (int zz=0x000; zz<0x100; zz++)
    printf("%04x    ",m_videoram[(scrollbank<<12)+(m_lsram_page[layer][1]>>1)+zz]);
printf("\n100-200:\n");
for (int zz=0x100; zz<0x200; zz++)
    printf("%04x    ",m_videoram[(scrollbank<<12)+(m_lsram_page[layer][1]>>1)+zz]);
printf("\n200-300:\n");
for (int zz=0x200; zz<0x300; zz++)
    printf("%04x    ",m_videoram[(scrollbank<<12)+(m_lsram_page[layer][1]>>1)+zz]);
printf("\n300-400:\n");
for (int zz=0x300; zz<0x400; zz++)
    printf("%04x    ",m_videoram[(scrollbank<<12)+(m_lsram_page[layer][1]>>1)+zz]);
printf("\nend\n");
}
*/
	last_active = m_active_layer;
	new_colorbase = (m_k055555 != NULL) ? k055555_get_palette_index(m_k055555, layer) : 0;

	for (r = 0; r < rowspan; r++)
	{
		if (rowspan > 1)
		{
			sy = ay;
			ty = r * K056832_PAGE_HEIGHT;

			if (!flipy)
			{
				// handle bottom-edge wraparoundness and cull off-screen tilemaps
				if ((r == 0) && (sy > height - K056832_PAGE_HEIGHT)) sy -= height;
				if ((sy + K056832_PAGE_HEIGHT <= ty) || (sy - K056832_PAGE_HEIGHT >= ty)) continue;

				// switch frame of reference and clip y
				if ((ty -= sy) >= 0)
				{
					cliph = K056832_PAGE_HEIGHT - ty;
					clipy = line_starty = ty;
					line_endy = K056832_PAGE_HEIGHT;
					sdat_start = 0;
				}
				else
				{
					cliph = K056832_PAGE_HEIGHT + ty;
					ty = -ty;
					clipy = line_starty = 0;
					line_endy = cliph;
					sdat_start = ty;
					if (scrollmode == 2) { sdat_start &= ~7; line_starty -= ty & 7; }
				}
			}
			else
			{
				ty += K056832_PAGE_HEIGHT;

				// handle top-edge wraparoundness and cull off-screen tilemaps
				if ((r == rowspan - 1) && (sy < K056832_PAGE_HEIGHT)) sy += height;
				if ((sy + K056832_PAGE_HEIGHT <= ty) || (sy - K056832_PAGE_HEIGHT >= ty)) continue;

				// switch frame of reference and clip y
				if ((ty -= sy) <= 0)
				{
					cliph = K056832_PAGE_HEIGHT + ty;
					clipy = line_starty = -ty;
					line_endy = K056832_PAGE_HEIGHT;
					sdat_start = K056832_PAGE_HEIGHT - 1;
					if (scrollmode == 2) sdat_start &= ~7;
				}
				else
				{
					cliph = K056832_PAGE_HEIGHT - ty;
					clipy = line_starty = 0;
					line_endy = cliph;
					sdat_start = cliph - 1;
					if (scrollmode == 2)
					{
						sdat_start &= ~7;
						line_starty -= ty & 7;
					}
				}
			}
		}
		else
		{
			cliph = line_endy = K056832_PAGE_HEIGHT;
			clipy = line_starty = 0;

			if (!flipy)
				sdat_start = dy;
			else
				/*
				    doesn't work with Metamorphic Force and Martial Champion (software Y-flipped) but
				    LE2U (naturally Y-flipped) seems to expect this condition as an override.

				    sdat_start = K056832_PAGE_HEIGHT-1 -dy;
				*/
			sdat_start = K056832_PAGE_HEIGHT - 1;

			if (scrollmode == 2) { sdat_start &= ~7; line_starty -= dy & 7; }
		}

		sdat_start += r * K056832_PAGE_HEIGHT;
		sdat_start <<= 1;

		clipmaxy = clipy + cliph - 1;

		for (c = 0; c < colspan; c++)
		{
			pageIndex = (((rowstart + r) & 3) << 2) + ((colstart + c) & 3);

			if (m_layer_association)
			{
				if (m_layer_assoc_with_page[pageIndex] != layer)
					continue;
			}
			else
			{
				if (m_layer_assoc_with_page[pageIndex] == -1)
					continue;

				m_active_layer = layer;
			}

			if (m_k055555 != NULL)       // are we using k055555 palette?
			{
				if (m_last_colorbase[pageIndex] != new_colorbase)
				{
					m_last_colorbase[pageIndex] = new_colorbase;
					mark_page_dirty(pageIndex);
				}
			}
			else
			{
				if (!pageIndex)
					m_active_layer = 0;
			}

			if (update_linemap(bitmap, pageIndex, flags))
				continue;

			tmap = m_tilemap[pageIndex];

			tmap->set_scrolly(0, ay);

			last_dx = 0x100000;
			last_visible = 0;

			for (sdat_walk = sdat_start, line_y = line_starty; line_y < line_endy; sdat_walk += sdat_adv, line_y += line_height)
			{
				dminy = line_y;
				dmaxy = line_y + line_height - 1;

				if (dminy < clipy) dminy = clipy;
				if (dmaxy > clipmaxy) dmaxy = clipmaxy;
				if (dminy > cmaxy || dmaxy < cminy) continue;

				sdat_offs = sdat_walk & sdat_wrapmask;

				drawrect.min_y = (dminy < cminy ) ? cminy : dminy;
				drawrect.max_y = (dmaxy > cmaxy ) ? cmaxy : dmaxy;
// printf("%04x  %04x\n",layer,flipy);
				// in xexex: K056832_DRAW_FLAG_MIRROR != flipy
				if ((scrollmode == 2) && (flags & K056832_DRAW_FLAG_MIRROR) && (flipy))
					dx = ((int)p_scroll_data[sdat_offs + 0x1e0 + 14]<<16 | (int)p_scroll_data[sdat_offs + 0x1e0 + 15]) + corr;
				else
					dx = ((int)p_scroll_data[sdat_offs]<<16 | (int)p_scroll_data[sdat_offs + 1]) + corr;

				if (last_dx == dx) { if (last_visible) goto LINE_SHORTCIRCUIT; continue; }
				last_dx = dx;

				if (colspan > 1)
				{
					//sx = (unsigned)dx % width;
					sx = (unsigned)dx & (width-1);

					//tx = c * K056832_PAGE_WIDTH;
					tx = c << 9;

					if (!flipx)
					{
						// handle right-edge wraparoundness and cull off-screen tilemaps
						if ((c == 0) && (sx > width - K056832_PAGE_WIDTH)) sx -= width;
						if ((sx + K056832_PAGE_WIDTH <= tx) || (sx - K056832_PAGE_WIDTH >= tx))
							{ last_visible = 0; continue; }

						// switch frame of reference and clip x
						if ((tx -= sx) <= 0) { clipw = K056832_PAGE_WIDTH + tx; clipx = 0; }
						else { clipw = K056832_PAGE_WIDTH - tx; clipx = tx; }
					}
					else
					{
						tx += K056832_PAGE_WIDTH;

						// handle left-edge wraparoundness and cull off-screen tilemaps
						if ((c == colspan-1) && (sx < K056832_PAGE_WIDTH)) sx += width;
						if ((sx + K056832_PAGE_WIDTH <= tx) || (sx - K056832_PAGE_WIDTH >= tx))
							{ last_visible = 0; continue; }

						// switch frame of reference and clip y
						if ((tx -= sx) >= 0) { clipw = K056832_PAGE_WIDTH - tx; clipx = 0; }
						else { clipw = K056832_PAGE_WIDTH + tx; clipx = -tx; }
					}
				}
				else { clipw = K056832_PAGE_WIDTH; clipx = 0; }

				last_visible = 1;

				dminx = clipx;
				dmaxx = clipx + clipw - 1;

				drawrect.min_x = (dminx < cminx ) ? cminx : dminx;
				drawrect.max_x = (dmaxx > cmaxx ) ? cmaxx : dmaxx;

				// soccer superstars visible area is >512 pixels, this causes problems with the logic because
				// the tilemaps are 512 pixels across.  Assume that if the limits were set as below that we
				// want the tilemap to be drawn on the right hand side..  this is probably not the correct
				// logic, but it works.
				if ((drawrect.min_x>0) && (drawrect.max_x==511))
					drawrect.max_x=cliprect.max_x;

				tmap->set_scrollx(0, dx);

				LINE_SHORTCIRCUIT:
					tmap->draw(bitmap, drawrect, flags, priority);

			} // end of line loop
		} // end of column loop
	} // end of row loop

	m_active_layer = last_active;
} // end of function

void k056832_device::tilemap_draw( bitmap_ind16 &bitmap, const rectangle &cliprect, int layer, UINT32 flags, UINT32 priority )
{ tilemap_draw_common(bitmap, cliprect, layer, flags, priority); }

void k056832_device::tilemap_draw( bitmap_rgb32 &bitmap, const rectangle &cliprect, int layer, UINT32 flags, UINT32 priority )
{ tilemap_draw_common(bitmap, cliprect, layer, flags, priority); }


void k056832_device::tilemap_draw_dj( bitmap_rgb32 &bitmap, const rectangle &cliprect, int layer, UINT32 flags, UINT32 priority )
{
	UINT32 last_dx, last_visible, new_colorbase, last_active;
	int sx, sy, ay, tx, ty, width, height;
	int clipw, clipx, cliph, clipy, clipmaxy;
	int line_height, line_endy, line_starty, line_y;
	int sdat_start, sdat_walk, sdat_adv, sdat_wrapmask, sdat_offs;
	int pageIndex, flipx, flipy, corr, r, c;
	int cminy, cmaxy, cminx, cmaxx;
	int dminy, dmaxy, dminx, dmaxx;
	rectangle drawrect;
	tilemap_t *tmap;
	UINT16 *p_scroll_data;
	UINT16 ram16[2];

	int rowstart = m_y[layer];
	int colstart = m_x[layer];
	int rowspan  = m_h[layer] + 1;
	int colspan  = m_w[layer] + 1;
	int dy = m_dy[layer];
	int dx = m_dx[layer];
	int scrollbank = ((m_regs[0x18] >> 1) & 0xc) | (m_regs[0x18] & 3);
	int scrollmode = m_regs[0x05] >> (m_lsram_page[layer][0] << 1) & 3;
	int need_wrap = -1;

	height = rowspan * K056832_PAGE_HEIGHT;
	width  = colspan * K056832_PAGE_WIDTH;

	cminx = cliprect.min_x;
	cmaxx = cliprect.max_x;
	cminy = cliprect.min_y;
	cmaxy = cliprect.max_y;

	// flip correction registers
	flipy = m_regs[0] & 0x20;
	if (flipy)
	{
		corr = m_regs[0x3c/2];
		if (corr & 0x400)
			corr |= 0xfffff800;
	}
	else
		corr = 0;
	dy += corr;
	ay = (unsigned)(dy - m_layer_offs[layer][1]) % height;

	flipx = m_regs[0] & 0x10;
	if (flipx)
	{
		corr = m_regs[0x3a/2];
		if (corr & 0x800)
			corr |= 0xfffff000;
	}
	else
		corr = 0;

	corr -= m_layer_offs[layer][0];

	switch( scrollmode )
	{
		case 0: // linescroll
			p_scroll_data = &m_videoram[scrollbank << 12] + (m_lsram_page[layer][1] >> 1);
			line_height = 1;
			sdat_wrapmask = 0x3ff;
			sdat_adv = 2;
		break;
		case 2: // rowscroll
			p_scroll_data = &m_videoram[scrollbank << 12] + (m_lsram_page[layer][1] >> 1);
			line_height = 8;
			sdat_wrapmask = 0x3ff;
			sdat_adv = 16;
		break;
		default: // xyscroll
			p_scroll_data = ram16;
			line_height = K056832_PAGE_HEIGHT;
			sdat_wrapmask = 0;
			sdat_adv = 0;
			ram16[0] = 0;
			ram16[1] = dx;
	}
	if (flipy)
		sdat_adv = -sdat_adv;

	last_active = m_active_layer;
	new_colorbase = (m_k055555 != NULL) ? k055555_get_palette_index(m_k055555, layer) : 0;

	for (r = 0; r <= rowspan; r++)
	{
		sy = ay;
		if (r == rowspan)
		{
			if (need_wrap < 0)
				continue;

			ty = need_wrap * K056832_PAGE_HEIGHT;
		}
		else
		{
			ty = r * K056832_PAGE_HEIGHT;
		}

		// cull off-screen tilemaps
		if ((sy + height <= ty) || (sy - height >= ty))
			continue;

		// switch frame of reference
		ty -= sy;

			// handle top-edge wraparoundness
			if (r == rowspan)
			{
				cliph = K056832_PAGE_HEIGHT + ty;
				clipy = line_starty = 0;
				line_endy = cliph;
				ty = -ty;
				sdat_start = ty;
				if (scrollmode == 2) { sdat_start &= ~7; line_starty -= ty & 7; }
			}

			// clip y
			else
			{
				if (ty < 0)
					ty += height;

				clipy = ty;
				cliph = K056832_PAGE_HEIGHT;

				if (clipy + cliph > height)
				{
					cliph = height - clipy;
					need_wrap =r;
				}

				line_starty = ty;
				line_endy = line_starty + cliph;
				sdat_start = 0;
			}

		if (r == rowspan)
			sdat_start += need_wrap * K056832_PAGE_HEIGHT;
		else
			sdat_start += r * K056832_PAGE_HEIGHT;
		sdat_start <<= 1;

		clipmaxy = clipy + cliph - 1;

		for (c = 0; c < colspan; c++)
		{
			if (r == rowspan)
				pageIndex = (((rowstart + need_wrap) & 3) << 2) + ((colstart + c) & 3);
			else
				pageIndex = (((rowstart + r) & 3) << 2) + ((colstart + c) & 3);

			if (m_layer_association)
			{
				if (m_layer_assoc_with_page[pageIndex] != layer)
					continue;
			}
			else
			{
				if (m_layer_assoc_with_page[pageIndex] == -1) continue;
				m_active_layer = layer;
			}

			if (m_k055555 != NULL)       // are we using k055555 palette?
			{
				if (m_last_colorbase[pageIndex] != new_colorbase)
				{
					m_last_colorbase[pageIndex] = new_colorbase;
					mark_page_dirty(pageIndex);
				}
			}
			else
			{
				if (!pageIndex)
					m_active_layer = 0;
			}

			if (update_linemap(bitmap, pageIndex, flags))
				continue;

			tmap = m_tilemap[pageIndex];
			tmap->set_scrolly(0, ay);

			last_dx = 0x100000;
			last_visible = 0;

			for (sdat_walk = sdat_start, line_y = line_starty; line_y < line_endy; sdat_walk += sdat_adv, line_y += line_height)
			{
				dminy = line_y;
				dmaxy = line_y + line_height - 1;

				if (dminy < clipy) dminy = clipy;
				if (dmaxy > clipmaxy) dmaxy = clipmaxy;
				if (dminy > cmaxy || dmaxy < cminy) continue;

				sdat_offs = sdat_walk & sdat_wrapmask;

				drawrect.min_y = (dminy < cminy ) ? cminy : dminy;
				drawrect.max_y = (dmaxy > cmaxy ) ? cmaxy : dmaxy;

				dx = ((int)p_scroll_data[sdat_offs] << 16 | (int)p_scroll_data[sdat_offs + 1]) + corr;

				if (last_dx == dx) { if (last_visible) goto LINE_SHORTCIRCUIT; continue; }
				last_dx = dx;

				if (colspan > 1)
				{
					//sx = (unsigned)dx % width;
					sx = (unsigned)dx & (width-1);

					//tx = c * K056832_PAGE_WIDTH;
					tx = c << 9;

					if (!flipx)
					{
						// handle right-edge wraparoundness and cull off-screen tilemaps
						if ((c == 0) && (sx > width - K056832_PAGE_WIDTH)) sx -= width;
						if ((sx + K056832_PAGE_WIDTH <= tx) || (sx - K056832_PAGE_WIDTH >= tx))
							{ last_visible = 0; continue; }

						// switch frame of reference and clip x
						if ((tx -= sx) <= 0) { clipw = K056832_PAGE_WIDTH + tx; clipx = 0; }
						else { clipw = K056832_PAGE_WIDTH - tx; clipx = tx; }
					}
					else
					{
						tx += K056832_PAGE_WIDTH;

						// handle left-edge wraparoundness and cull off-screen tilemaps
						if ((c == colspan-1) && (sx < K056832_PAGE_WIDTH)) sx += width;
						if ((sx + K056832_PAGE_WIDTH <= tx) || (sx - K056832_PAGE_WIDTH >= tx))
							{ last_visible = 0; continue; }

						// switch frame of reference and clip y
						if ((tx -= sx) >= 0) { clipw = K056832_PAGE_WIDTH - tx; clipx = 0; }
						else { clipw = K056832_PAGE_WIDTH + tx; clipx = -tx; }
					}
				}
				else
				{
					clipw = K056832_PAGE_WIDTH;
					clipx = 0;
				}

				last_visible = 1;

				dminx = clipx;
				dmaxx = clipx + clipw - 1;

				drawrect.min_x = (dminx < cminx ) ? cminx : dminx;
				drawrect.max_x = (dmaxx > cmaxx ) ? cmaxx : dmaxx;

				tmap->set_scrollx(0, dx);

				LINE_SHORTCIRCUIT:
					tmap->draw(bitmap, drawrect, flags, priority);

			} // end of line loop
		} // end of column loop
	} // end of row loop

	m_active_layer = last_active;

} // end of function


void k056832_device::set_layer_association( int status )
{
	m_default_layer_association = status;
}

int k056832_device::get_layer_association( )
{
	return(m_layer_association);
}

void k056832_device::set_layer_offs( int layer, int offsx, int offsy )
{
	m_layer_offs[layer][0] = offsx;
	m_layer_offs[layer][1] = offsy;
}

void k056832_device::set_lsram_page( int logical_page, int physical_page, int physical_offset )
{
	m_lsram_page[logical_page][0] = physical_page;
	m_lsram_page[logical_page][1] = physical_offset;
}

void k056832_device::linemap_enable( int enable )
{
	m_linemap_enabled = enable;
}

int k056832_device::is_irq_enabled( int irqline )
{
	return(m_regs[0x06/2] & (1 << irqline & 7));
}

void k056832_device::read_avac( int *mode, int *data )
{
	*mode = m_regs[0x04/2] & 7;
	*data = m_regs[0x38/2];
}

int k056832_device::read_register( int regnum )
{
	return(m_regs[regnum]);
}

void k056832_device::postload()
{
	update_page_layout();
	change_rambank();
	change_rombank();
}

//misc debug handlers

READ32_MEMBER( k056832_device::long_r )
{
	offset <<= 1;
	return (word_r(space, offset + 1, 0xffff) | word_r(space, offset, 0xffff) << 16);
}

READ16_MEMBER( k056832_device::word_r )
{
	return (m_regs[offset]);
}       // VACSET

READ16_MEMBER( k056832_device::b_word_r )
{
	return (m_regsb[offset]);
}   // VSCCS (board dependent)


/***************************************************************************/
/*                                                                         */
/*                                 055555                                  */
/*                                                                         */
/***************************************************************************/

/* K055555 5-bit-per-pixel priority encoder */
/* This device has 48 8-bit-wide registers */

struct k055555_state
{
	UINT8    regs[128];
};

/*****************************************************************************
    INLINE FUNCTIONS
*****************************************************************************/

INLINE k055555_state *k055555_get_safe_token( device_t *device )
{
	assert(device != NULL);
	assert(device->type() == K055555);

	return (k055555_state *)downcast<k055555_device *>(device)->token();
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

void k055555_write_reg( device_t *device, UINT8 regnum, UINT8 regdat )
{
	k055555_state *k055555 = k055555_get_safe_token(device);

	static const char *const rnames[46] =
	{
		"BGC CBLK", "BGC SET", "COLSET0", "COLSET1", "COLSET2", "COLSET3", "COLCHG ON",
		"A PRI 0", "A PRI 1", "A COLPRI", "B PRI 0", "B PRI 1", "B COLPRI", "C PRI", "D PRI",
		"OBJ PRI", "SUB1 PRI", "SUB2 PRI", "SUB3 PRI", "OBJ INPRI ON", "S1 INPRI ON", "S2 INPRI ON",
		"S3 INPRI ON", "A PAL", "B PAL", "C PAL", "D PAL", "OBJ PAL", "SUB1 PAL", "SUB2 PAL", "SUB3 PAL",
		"SUB2 PAL ON", "SUB3 PAL ON", "V INMIX", "V INMIX ON", "OS INMIX", "OS INMIX ON", "SHD PRI 1",
		"SHD PRI 2", "SHD PRI 3", "SHD ON", "SHD PRI SEL", "V BRI", "OS INBRI", "OS INBRI ON", "ENABLE"
	};

	if (regdat != k055555->regs[regnum])
	{
		LOG(("5^5: %x to reg %x (%s)\n", regdat, regnum, rnames[regnum]));
	}

	k055555->regs[regnum] = regdat;
}

WRITE32_DEVICE_HANDLER( k055555_long_w )
{
	UINT8 regnum, regdat;

	if (ACCESSING_BITS_24_31)
	{
		regnum = offset << 1;
		regdat = data >> 24;
	}
	else
	{
		if (ACCESSING_BITS_8_15)
		{
			regnum = (offset << 1) + 1;
			regdat = data >> 8;
		}
		else
		{
			// logerror("5^5: unknown mem_mask %08x\n", mem_mask);
			return;
		}
	}

	k055555_write_reg(device, regnum, regdat);
}

WRITE16_DEVICE_HANDLER( k055555_word_w )
{
	if (mem_mask == 0x00ff)
	{
		k055555_write_reg(device, offset, data & 0xff);
	}
	else
	{
		k055555_write_reg(device, offset, data >> 8);
	}
}

int k055555_read_register( device_t *device, int regnum )
{
	k055555_state *k055555 = k055555_get_safe_token(device);
	return k055555->regs[regnum];
}

int k055555_get_palette_index( device_t *device, int idx )
{
	k055555_state *k055555 = k055555_get_safe_token(device);
	return k055555->regs[K55_PALBASE_A + idx];
}

/*****************************************************************************
    DEVICE INTERFACE
*****************************************************************************/

static DEVICE_START( k055555 )
{
	k055555_state *k055555 = k055555_get_safe_token(device);
	device->save_item(NAME(k055555->regs));
}

static DEVICE_RESET( k055555 )
{
	k055555_state *k055555 = k055555_get_safe_token(device);
	memset(k055555->regs, 0, 64 * sizeof(UINT8));
}

const device_type K055555 = &device_creator<k055555_device>;

k055555_device::k055555_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K055555, "Konami 055555", tag, owner, clock, "k055555", __FILE__)
{
	m_token = global_alloc_clear(k055555_state);
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k055555_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k055555_device::device_start()
{
	DEVICE_START_NAME( k055555 )(this);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k055555_device::device_reset()
{
	DEVICE_RESET_NAME( k055555 )(this);
}

/***************************************************************************/
/*                                                                         */
/*                                 054338                                  */
/*                                                                         */
/***************************************************************************/

// k054338 alpha blend / final mixer (normally used with the 55555)
// because the implementation is video dependant, this is just a
// register-handling shell.


struct k054338_state
{
	UINT16    regs[32];
	int       shd_rgb[9];
	int       alphainverted;

	screen_device *screen;
	device_t *k055555;  /* used to fill BG color */
};

/*****************************************************************************
    INLINE FUNCTIONS
*****************************************************************************/

INLINE k054338_state *k054338_get_safe_token( device_t *device )
{
	assert(device != NULL);
	assert(device->type() == K054338);

	return (k054338_state *)downcast<k054338_device *>(device)->token();
}

INLINE const k054338_interface *k054338_get_interface( device_t *device )
{
	assert(device != NULL);
	assert(device->type() == K054338);
	return (const k054338_interface *) device->static_config();
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

WRITE16_DEVICE_HANDLER( k054338_word_w )
{
	k054338_state *k054338 = k054338_get_safe_token(device);
	COMBINE_DATA(k054338->regs + offset);
}

WRITE32_DEVICE_HANDLER( k054338_long_w )
{
	offset <<= 1;
	k054338_word_w(device, space, offset, data >> 16, mem_mask >> 16);
	k054338_word_w(device, space, offset + 1, data, mem_mask);
}

// returns a 16-bit '338 register
int  k054338_register_r( device_t *device, int reg )
{
	k054338_state *k054338 = k054338_get_safe_token(device);
	return k054338->regs[reg];
}

void k054338_update_all_shadows( device_t *device, int rushingheroes_hack )
{
	k054338_state *k054338 = k054338_get_safe_token(device);
	running_machine &machine = device->machine();
	int i, d;
	int noclip = k054338->regs[K338_REG_CONTROL] & K338_CTL_CLIPSL;

	for (i = 0; i < 9; i++)
	{
		d = k054338->regs[K338_REG_SHAD1R + i] & 0x1ff;
		if (d >= 0x100)
			d -= 0x200;
		k054338->shd_rgb[i] = d;
	}

	if (!rushingheroes_hack)
	{
		palette_set_shadow_dRGB32(machine, 0, k054338->shd_rgb[0], k054338->shd_rgb[1], k054338->shd_rgb[2], noclip);
		palette_set_shadow_dRGB32(machine, 1, k054338->shd_rgb[3], k054338->shd_rgb[4], k054338->shd_rgb[5], noclip);
		palette_set_shadow_dRGB32(machine, 2, k054338->shd_rgb[6], k054338->shd_rgb[7], k054338->shd_rgb[8], noclip);
	}
	else // rushing heroes seems to specify shadows in another format, or it's not being interpreted properly.
	{
		palette_set_shadow_dRGB32(machine, 0, -80, -80, -80, 0);
		palette_set_shadow_dRGB32(machine, 1, -80, -80, -80, 0);
		palette_set_shadow_dRGB32(machine, 2, -80, -80, -80, 0);
	}
}

// k054338 BG color fill
void k054338_fill_solid_bg( device_t *device, bitmap_rgb32 &bitmap )
{
	UINT32 bgcolor;
	UINT32 *pLine;
	int x, y;

	bgcolor = (k054338_register_r(device, K338_REG_BGC_R) & 0xff) << 16;
	bgcolor |= k054338_register_r(device, K338_REG_BGC_GB);

	/* and fill the screen with it */
	for (y = 0; y < bitmap.height(); y++)
	{
		pLine = &bitmap.pix32(y);
		for (x = 0; x < bitmap.width(); x++)
			*pLine++ = bgcolor;
	}
}

// Unified k054338/K055555 BG color fill
void k054338_fill_backcolor( device_t *device, bitmap_rgb32 &bitmap, int mode ) // (see p.67)
{
	k054338_state *k054338 = k054338_get_safe_token(device);
	int clipx, clipy, clipw, cliph, i, dst_pitch;
	int BGC_CBLK, BGC_SET;
	UINT32 *dst_ptr, *pal_ptr;
	int bgcolor;
	const rectangle &visarea = k054338->screen->visible_area();

	clipx = visarea.min_x & ~3;
	clipy = visarea.min_y;
	clipw = (visarea.max_x - clipx + 4) & ~3;
	cliph = visarea.max_y - clipy + 1;

	dst_ptr = &bitmap.pix32(clipy);
	dst_pitch = bitmap.rowpixels();
	dst_ptr += clipx;

	BGC_SET = 0;
	pal_ptr = device->machine().driver_data()->m_generic_paletteram_32;

	if (!mode || k054338->k055555 == NULL)
	{
		// single color output from CLTC
		bgcolor = (int)(k054338->regs[K338_REG_BGC_R] & 0xff) << 16 | (int)k054338->regs[K338_REG_BGC_GB];
	}
	else
	{
		BGC_CBLK = k055555_read_register(k054338->k055555, 0);
		BGC_SET  = k055555_read_register(k054338->k055555, 1);

		pal_ptr += BGC_CBLK << 9;

		// single color output from PCU2
		if (!(BGC_SET & 2))
		{
			bgcolor = *pal_ptr;
			mode = 0;
		}
		else bgcolor = 0;
	}

	if (!mode)
	{
		// single color fill
		dst_ptr += clipw;
		i = clipw = -clipw;
		do
		{
			do
			{
				dst_ptr[i] = dst_ptr[i+1] = dst_ptr[i+2] = dst_ptr[i+3] = bgcolor;
			}
			while (i += 4);

			dst_ptr += dst_pitch;
			i = clipw;
		}
		while (--cliph);
	}
	else
	{
		if (!(BGC_SET & 1))
		{
			// vertical gradient fill
			pal_ptr += clipy;
			dst_ptr += clipw;
			bgcolor = *pal_ptr++;
			i = clipw = -clipw;
			do
			{
				do
				{
					dst_ptr[i] = dst_ptr[i+1] = dst_ptr[i+2] = dst_ptr[i+3] = bgcolor;
				}
				while (i += 4);

				dst_ptr += dst_pitch;
				bgcolor = *pal_ptr++;
				i = clipw;
			}
			while (--cliph);
		}
		else
		{
			// horizontal gradient fill
			pal_ptr += clipx;
			clipw <<= 2;
			do
			{
				memcpy(dst_ptr, pal_ptr, clipw);
				dst_ptr += dst_pitch;
			}
			while (--cliph);
		}
	}
}

// addition blending unimplemented (requires major changes to drawgfx and tilemap.c)
int k054338_set_alpha_level( device_t *device, int pblend )
{
	k054338_state *k054338 = k054338_get_safe_token(device);
	UINT16 *regs;
	int ctrl, mixpri, mixset, mixlv;

	if (pblend <= 0 || pblend > 3)
	{
		return (255);
	}

	regs   = k054338->regs;
	ctrl   = k054338->regs[K338_REG_CONTROL];
	mixpri = ctrl & K338_CTL_MIXPRI;
	mixset = regs[K338_REG_PBLEND + (pblend >> 1 & 1)] >> (~pblend << 3 & 8);
	mixlv  = mixset & 0x1f;

	if (k054338->alphainverted)
		mixlv = 0x1f - mixlv;

	if (!(mixset & 0x20))
	{
		mixlv = (mixlv << 3) | (mixlv >> 2);
	}
	else
	{
		if (!mixpri)
		{
			// source x alpha  +  target (clipped at 255)
		}
		else
		{
			// source  +  target x alpha (clipped at 255)
		}

		// DUMMY
		if (mixlv && mixlv < 0x1f)
			mixlv = 0x10;

		mixlv = (mixlv << 3) | (mixlv >> 2);

		if (VERBOSE)
			popmessage("MIXSET%1d %s addition mode: %02x", pblend, (mixpri) ? "dst" : "src", mixset & 0x1f);
	}

	return mixlv;
}

void k054338_invert_alpha( device_t *device, int invert )
{
	k054338_state *k054338 = k054338_get_safe_token(device);
	k054338->alphainverted = invert;
}


#if 0
// FIXME
void k054338->export_config( device_t *device, int **shd_rgb )
{
	k054338_state *k054338 = k054338_get_safe_token(device);
	*shd_rgb = k054338->shd_rgb;
}
#endif

/*****************************************************************************
    DEVICE INTERFACE
*****************************************************************************/

static DEVICE_START( k054338 )
{
	k054338_state *k054338 = k054338_get_safe_token(device);
	const k054338_interface *intf = k054338_get_interface(device);

	k054338->screen = device->machine().device<screen_device>(intf->screen);
	k054338->k055555 = device->machine().device(intf->k055555);

	k054338->alphainverted = intf->alpha_inv;

	device->save_item(NAME(k054338->regs));
	device->save_item(NAME(k054338->shd_rgb));
}

static DEVICE_RESET( k054338 )
{
	k054338_state *k054338 = k054338_get_safe_token(device);

	memset(k054338->regs, 0, sizeof(UINT16)*32);
	memset(k054338->shd_rgb, 0, sizeof(int)*9);
}

const device_type K054338 = &device_creator<k054338_device>;

k054338_device::k054338_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K054338, "Konami 054338", tag, owner, clock, "k054338", __FILE__)
{
	m_token = global_alloc_clear(k054338_state);
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k054338_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k054338_device::device_start()
{
	DEVICE_START_NAME( k054338 )(this);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k054338_device::device_reset()
{
	DEVICE_RESET_NAME( k054338 )(this);
}


// Newer Konami devices

// from video/gticlub.c



/*****************************************************************************/
/* Konami K001006 Custom 3D Texel Renderer chip (KS10081) */

/***************************************************************************/
/*                                                                         */
/*                                  001006                                 */
/*                                                                         */
/***************************************************************************/

struct k001006_state
{
	screen_device *screen;

	UINT16 *     pal_ram;
	UINT16 *     unknown_ram;
	UINT32       addr;
	int          device_sel;

	UINT32 *     palette;

	const char     *gfx_region;
};

/*****************************************************************************
    INLINE FUNCTIONS
*****************************************************************************/

INLINE k001006_state *k001006_get_safe_token( device_t *device )
{
	assert(device != NULL);
	assert(device->type() == K001006);

	return (k001006_state *)downcast<k001006_device *>(device)->token();
}

INLINE const k001006_interface *k001006_get_interface( device_t *device )
{
	assert(device != NULL);
	assert((device->type() == K001006));
	return (const k001006_interface *) device->static_config();
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

READ32_DEVICE_HANDLER( k001006_r )
{
	k001006_state *k001006 = k001006_get_safe_token(device);

	if (offset == 1)
	{
		switch (k001006->device_sel)
		{
			case 0x0b:      // CG Board ROM read
			{
				UINT16 *rom = (UINT16*)space.machine().root_device().memregion(k001006->gfx_region)->base();
				return rom[k001006->addr / 2] << 16;
			}
			case 0x0d:      // Palette RAM read
			{
				UINT32 addr = k001006->addr;

				k001006->addr += 2;
				return k001006->pal_ram[addr >> 1];
			}
			case 0x0f:      // Unknown RAM read
			{
				return k001006->unknown_ram[k001006->addr++];
			}
			default:
			{
				fatalerror("k001006_r, unknown device %02X\n", k001006->device_sel);
			}
		}
	}
	return 0;
}

WRITE32_DEVICE_HANDLER( k001006_w )
{
	k001006_state *k001006 = k001006_get_safe_token(device);

	if (offset == 0)
	{
		COMBINE_DATA(&k001006->addr);
	}
	else if (offset == 1)
	{
		switch (k001006->device_sel)
		{
			case 0xd:   // Palette RAM write
			{
				int r, g, b, a;
				UINT32 index = k001006->addr;

				k001006->pal_ram[index >> 1] = data & 0xffff;

				a = (data & 0x8000) ? 0x00 : 0xff;
				b = ((data >> 10) & 0x1f) << 3;
				g = ((data >>  5) & 0x1f) << 3;
				r = ((data >>  0) & 0x1f) << 3;
				b |= (b >> 5);
				g |= (g >> 5);
				r |= (r >> 5);
				k001006->palette[index >> 1] = MAKE_ARGB(a, r, g, b);

				k001006->addr += 2;
				break;
			}
			case 0xf:   // Unknown RAM write
			{
			//  mame_printf_debug("Unknown RAM %08X = %04X\n", k001006->addr, data & 0xffff);
				k001006->unknown_ram[k001006->addr++] = data & 0xffff;
				break;
			}
			default:
			{
				mame_printf_debug("k001006_w: device %02X, write %04X to %08X\n", k001006->device_sel, data & 0xffff, k001006->addr++);
			}
		}
	}
	else if (offset == 2)
	{
		if (ACCESSING_BITS_16_31)
		{
			k001006->device_sel = (data >> 16) & 0xf;
		}
	}
}

UINT32 k001006_get_palette( device_t *device, int index )
{
	k001006_state *k001006 = k001006_get_safe_token(device);
	return k001006->palette[index];
}

/*****************************************************************************
    DEVICE INTERFACE
*****************************************************************************/

static DEVICE_START( k001006 )
{
	k001006_state *k001006 = k001006_get_safe_token(device);
	const k001006_interface *intf = k001006_get_interface(device);

	k001006->pal_ram = auto_alloc_array_clear(device->machine(), UINT16, 0x800);
	k001006->unknown_ram = auto_alloc_array_clear(device->machine(), UINT16, 0x1000);
	k001006->palette = auto_alloc_array(device->machine(), UINT32, 0x800);

	k001006->gfx_region = intf->gfx_region;

	device->save_pointer(NAME(k001006->pal_ram), 0x800*sizeof(UINT16));
	device->save_pointer(NAME(k001006->unknown_ram), 0x1000*sizeof(UINT16));
	device->save_pointer(NAME(k001006->palette), 0x800*sizeof(UINT32));
	device->save_item(NAME(k001006->device_sel));
	device->save_item(NAME(k001006->addr));
}

static DEVICE_RESET( k001006 )
{
	k001006_state *k001006 = k001006_get_safe_token(device);

	k001006->addr = 0;
	k001006->device_sel = 0;
	memset(k001006->palette, 0, 0x800*sizeof(UINT32));
}

const device_type K001006 = &device_creator<k001006_device>;

k001006_device::k001006_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K001006, "Konami 001006", tag, owner, clock, "k001006", __FILE__)
{
	m_token = global_alloc_clear(k001006_state);
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k001006_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k001006_device::device_start()
{
	DEVICE_START_NAME( k001006 )(this);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k001006_device::device_reset()
{
	DEVICE_RESET_NAME( k001006 )(this);
}

/*****************************************************************************/
/* Konami K001005 Custom 3D Pixel Renderer chip (KS10071) */

/***************************************************************************/
/*                                                                         */
/*                                  001005                                 */
/*                                                                         */
/***************************************************************************/

#include "video/poly.h"
#include "cpu/sharc/sharc.h"

struct poly_extra_data
{
	UINT32 color;
	int texture_x, texture_y;
	int texture_page;
	int texture_palette;
	int texture_mirror_x;
	int texture_mirror_y;
};

struct k001005_state
{
	screen_device *screen;
	device_t *cpu;
	device_t *dsp;
	device_t *k001006_1;
	device_t *k001006_2;

	UINT8  *     texture;
	UINT16 *     ram[2];
	UINT32 *     fifo;
	UINT32 *     _3d_fifo;

	UINT32    status;
	bitmap_rgb32 *bitmap[2];
	bitmap_ind32 *zbuffer;
	rectangle cliprect;
	int    ram_ptr;
	int    fifo_read_ptr;
	int    fifo_write_ptr;
	int    _3d_fifo_ptr;

	int tex_mirror_table[4][128];

	int bitmap_page;

	poly_manager *poly;
	poly_vertex prev_v[4];
	int prev_poly_type;

	UINT8 *gfxrom;
};

static const int decode_x_gti[8] = {  0, 16, 2, 18, 4, 20, 6, 22 };
static const int decode_y_gti[16] = {  0, 8, 32, 40, 1, 9, 33, 41, 64, 72, 96, 104, 65, 73, 97, 105 };

static const int decode_x_zr107[8] = {  0, 16, 1, 17, 2, 18, 3, 19 };
static const int decode_y_zr107[16] = {  0, 8, 32, 40, 4, 12, 36, 44, 64, 72, 96, 104, 68, 76, 100, 108 };


/*****************************************************************************
    INLINE FUNCTIONS
*****************************************************************************/

INLINE k001005_state *k001005_get_safe_token( device_t *device )
{
	assert(device != NULL);
	assert(device->type() == K001005);

	return (k001005_state *)downcast<k001005_device *>(device)->token();
}

INLINE const k001005_interface *k001005_get_interface( device_t *device )
{
	assert(device != NULL);
	assert((device->type() == K001005));
	return (const k001005_interface *) device->static_config();
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

static void k001005_render_polygons( device_t *device );

// rearranges the texture data to a more practical order
void k001005_preprocess_texture_data( UINT8 *rom, int length, int gticlub )
{
	int index;
	int i, x, y;
	UINT8 temp[0x40000];

	const int *decode_x;
	const int *decode_y;

	if (gticlub)
	{
		decode_x = decode_x_gti;
		decode_y = decode_y_gti;
	}
	else
	{
		decode_x = decode_x_zr107;
		decode_y = decode_y_zr107;
	}

	for (index = 0; index < length; index += 0x40000)
	{
		int offset = index;

		memset(temp, 0, 0x40000);

		for (i = 0; i < 0x800; i++)
		{
			int tx = ((i & 0x400) >> 5) | ((i & 0x100) >> 4) | ((i & 0x40) >> 3) | ((i & 0x10) >> 2) | ((i & 0x4) >> 1) | (i & 0x1);
			int ty = ((i & 0x200) >> 5) | ((i & 0x80) >> 4) | ((i & 0x20) >> 3) | ((i & 0x8) >> 2) | ((i & 0x2) >> 1);

			tx <<= 3;
			ty <<= 4;

			for (y = 0; y < 16; y++)
			{
				for (x = 0; x < 8; x++)
				{
					UINT8 pixel = rom[offset + decode_y[y] + decode_x[x]];

					temp[((ty + y) * 512) + (tx + x)] = pixel;
				}
			}

			offset += 128;
		}

		memcpy(&rom[index], temp, 0x40000);
	}
}

void k001005_swap_buffers( device_t *device )
{
	k001005_state *k001005 = k001005_get_safe_token(device);

	k001005->bitmap_page ^= 1;

	//if (k001005->status == 2)
	{
		k001005->bitmap[k001005->bitmap_page]->fill(device->machine().pens[0] & 0x00ffffff, k001005->cliprect);
		k001005->zbuffer->fill(0xffffffff, k001005->cliprect);
	}
}

READ32_DEVICE_HANDLER( k001005_r )
{
	k001005_state *k001005 = k001005_get_safe_token(device);

	switch(offset)
	{
		case 0x000:         // FIFO read, high 16 bits
		{
			UINT16 value = k001005->fifo[k001005->fifo_read_ptr] >> 16;
		//  mame_printf_debug("FIFO_r0: %08X\n", k001005->fifo_ptr);
			return value;
		}

		case 0x001:         // FIFO read, low 16 bits
		{
			UINT16 value = k001005->fifo[k001005->fifo_read_ptr] & 0xffff;
		//  mame_printf_debug("FIFO_r1: %08X\n", k001005->fifo_ptr);

			if (k001005->status != 1 && k001005->status != 2)
			{
				if (k001005->fifo_read_ptr < 0x3ff)
				{
					//k001005->dsp->execute().set_input_line(SHARC_INPUT_FLAG1, CLEAR_LINE);
					sharc_set_flag_input(k001005->dsp, 1, CLEAR_LINE);
				}
				else
				{
					//k001005->dsp->execute().set_input_line(SHARC_INPUT_FLAG1, ASSERT_LINE);
					sharc_set_flag_input(k001005->dsp, 1, ASSERT_LINE);
				}
			}
			else
			{
				//k001005->dsp->execute().set_input_line(SHARC_INPUT_FLAG1, ASSERT_LINE);
				sharc_set_flag_input(k001005->dsp, 1, ASSERT_LINE);
			}

			k001005->fifo_read_ptr++;
			k001005->fifo_read_ptr &= 0x7ff;
			return value;
		}

		case 0x11b:         // status ?
			return 0x8002;

		case 0x11c:         // slave status ?
			return 0x8000;

		case 0x11f:
			if (k001005->ram_ptr >= 0x400000)
			{
				return k001005->ram[1][(k001005->ram_ptr++) & 0x3fffff];
			}
			else
			{
				return k001005->ram[0][(k001005->ram_ptr++) & 0x3fffff];
			}

		default:
			//mame_printf_debug("k001005->r: %08X, %08X at %08X\n", offset, mem_mask, space.device().safe_pc());
			break;
	}
	return 0;
}

WRITE32_DEVICE_HANDLER( k001005_w )
{
	k001005_state *k001005 = k001005_get_safe_token(device);

	switch (offset)
	{
		case 0x000:         // FIFO write
		{
			if (k001005->status != 1 && k001005->status != 2)
			{
				if (k001005->fifo_write_ptr < 0x400)
				{
					//k001005->dsp->execute().set_input_line(SHARC_INPUT_FLAG1, ASSERT_LINE);
					sharc_set_flag_input(k001005->dsp, 1, ASSERT_LINE);
				}
				else
				{
					//k001005->dsp->execute().set_input_line(SHARC_INPUT_FLAG1, CLEAR_LINE);
					sharc_set_flag_input(k001005->dsp, 1, CLEAR_LINE);
				}
			}
			else
			{
				//k001005->dsp->execute().set_input_line(SHARC_INPUT_FLAG1, ASSERT_LINE);
				sharc_set_flag_input(k001005->dsp, 1, ASSERT_LINE);
			}

		//  mame_printf_debug("K001005 FIFO write: %08X at %08X\n", data, space.device().safe_pc());
			k001005->fifo[k001005->fifo_write_ptr] = data;
			k001005->fifo_write_ptr++;
			k001005->fifo_write_ptr &= 0x7ff;

			k001005->_3d_fifo[k001005->_3d_fifo_ptr++] = data;

			// !!! HACK to get past the FIFO B test (GTI Club & Thunder Hurricane) !!!
			if (k001005->cpu->safe_pc() == 0x201ee)
			{
				// This is used to make the SHARC timeout
				k001005->cpu->execute().spin_until_trigger(10000);
			}
			// !!! HACK to get past the FIFO B test (Winding Heat & Midnight Run) !!!
			if (k001005->cpu->safe_pc() == 0x201e6)
			{
				// This is used to make the SHARC timeout
				k001005->cpu->execute().spin_until_trigger(10000);
			}

			break;
		}

		case 0x100: break;

	//  case 0x10a:     poly_r = data & 0xff; break;
	//  case 0x10b:     poly_g = data & 0xff; break;
	//  case 0x10c:     poly_b = data & 0xff; break;

		case 0x11a:
			k001005->status = data;
			k001005->fifo_write_ptr = 0;
			k001005->fifo_read_ptr = 0;

			if (data == 2 && k001005->_3d_fifo_ptr > 0)
			{
				k001005_swap_buffers(device);
				k001005_render_polygons(device);
				poly_wait(k001005->poly, "render_polygons");
				k001005->_3d_fifo_ptr = 0;
			}
			break;

		case 0x11d:
			k001005->fifo_write_ptr = 0;
			k001005->fifo_read_ptr = 0;
			break;

		case 0x11e:
			k001005->ram_ptr = data;
			break;

		case 0x11f:
			if (k001005->ram_ptr >= 0x400000)
			{
				k001005->ram[1][(k001005->ram_ptr++) & 0x3fffff] = data & 0xffff;
			}
			else
			{
				k001005->ram[0][(k001005->ram_ptr++) & 0x3fffff] = data & 0xffff;
			}
			break;

		default:
			//mame_printf_debug("k001005->w: %08X, %08X, %08X at %08X\n", data, offset, mem_mask, space.device().safe_pc());
			break;
	}

}

/* emu/video/poly.c cannot handle atm callbacks passing a device parameter */
#define POLY_DEVICE 0

#if POLY_DEVICE
static void draw_scanline( device_t *device, void *dest, INT32 scanline, const poly_extent *extent, const void *extradata, int threadid )
{
	k001005_state *k001005 = k001005_get_safe_token(device);
	const poly_extra_data *extra = (const poly_extra_data *)extradata;
	bitmap_ind16 *destmap = (bitmap_ind16 *)dest;
	float z = extent->param[0].start;
	float dz = extent->param[0].dpdx;
	UINT32 *fb = &destmap->pix32(scanline);
	UINT32 *zb = &k001005->zbuffer->pix32(scanline);
	UINT32 color = extra->color;
	int x;

	for (x = extent->startx; x < extent->stopx; x++)
	{
		UINT32 iz = (UINT32)z >> 16;

		if (iz <= zb[x])
		{
			if (color & 0xff000000)
			{
				fb[x] = color;
				zb[x] = iz;
			}
		}

		z += dz;
	}
}
#endif

#if POLY_DEVICE
static void draw_scanline_tex( device_t *device, void *dest, INT32 scanline, const poly_extent *extent, const void *extradata, int threadid )
{
	k001005_state *k001005 = k001005_get_safe_token(device);
	const poly_extra_data *extra = (const poly_extra_data *)extradata;
	bitmap_ind16 *destmap = (bitmap_ind16 *)dest;
	UINT8 *texrom = k001005->gfxrom + (extra->texture_page * 0x40000);
	device_t *pal_device = (extra->texture_palette & 0x8) ? k001005->k001006_2 : k001005->k001006_1;
	int palette_index = (extra->texture_palette & 0x7) * 256;
	float z = extent->param[0].start;
	float u = extent->param[1].start;
	float v = extent->param[2].start;
	float w = extent->param[3].start;
	float dz = extent->param[0].dpdx;
	float du = extent->param[1].dpdx;
	float dv = extent->param[2].dpdx;
	float dw = extent->param[3].dpdx;
	int texture_mirror_x = extra->texture_mirror_x;
	int texture_mirror_y = extra->texture_mirror_y;
	int texture_x = extra->texture_x;
	int texture_y = extra->texture_y;
	int x;

	UINT32 *fb = &destmap->pix32(scanline);
	UINT32 *zb = &k001005->zbuffer->pix32(scanline);

	for (x = extent->startx; x < extent->stopx; x++)
	{
		UINT32 iz = (UINT32)z >> 16;
		//int iu = u >> 16;
		//int iv = v >> 16;

		if (iz < zb[x])
		{
			float oow = 1.0f / w;
			UINT32 color;
			int iu, iv;
			int iiv, iiu, texel;

			iu = u * oow;
			iv = v * oow;

			iiu = texture_x + k001005->tex_mirror_table[texture_mirror_x][(iu >> 4) & 0x7f];
			iiv = texture_y + k001005->tex_mirror_table[texture_mirror_y][(iv >> 4) & 0x7f];
			texel = texrom[((iiv & 0x1ff) * 512) + (iiu & 0x1ff)];
			color = k001006_get_palette(pal_device, palette_index + texel);

			if (color & 0xff000000)
			{
				fb[x] = color;
				zb[x] = iz;
			}
		}

		u += du;
		v += dv;
		z += dz;
		w += dw;
	}
}
#endif


static void k001005_render_polygons( device_t *device )
{
	k001005_state *k001005 = k001005_get_safe_token(device);
	int i, j;
#if POLY_DEVICE
	const rectangle &visarea = k001005->screen->visible_area();
#endif

//  mame_printf_debug("k001005->fifo_ptr = %08X\n", k001005->_3d_fifo_ptr);

	for (i = 0; i < k001005->_3d_fifo_ptr; i++)
	{
		if (k001005->_3d_fifo[i] == 0x80000003)
		{
			poly_extra_data *extra = (poly_extra_data *)poly_get_extra_data(k001005->poly);
//          poly_vertex v[4];
			int r, g, b, a;
			UINT32 color;
			int index = i;

			++index;

			for (j = 0; j < 4; j++)
			{
				int x, y;

				x = ((k001005->_3d_fifo[index] >>  0) & 0x3fff);
				y = ((k001005->_3d_fifo[index] >> 16) & 0x1fff);
				x |= ((x & 0x2000) ? 0xffffc000 : 0);
				y |= ((y & 0x1000) ? 0xffffe000 : 0);
				++index;
#if POLY_DEVICE
				v[j].x = ((float)(x) / 16.0f) + 256.0f;
				v[j].y = ((float)(-y) / 16.0f) + 192.0f;
				v[j].p[0] = 0;  /* ??? */
#endif
			}

			++index;

			r = (k001005->_3d_fifo[index] >>  0) & 0xff;
			g = (k001005->_3d_fifo[index] >>  8) & 0xff;
			b = (k001005->_3d_fifo[index] >> 16) & 0xff;
			a = (k001005->_3d_fifo[index] >> 24) & 0xff;
			color = (a << 24) | (r << 16) | (g << 8) | (b);
			++index;

			extra->color = color;
#if POLY_DEVICE
			poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline, 1, &v[0], &v[1], &v[2]);
			poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline, 1, &v[0], &v[2], &v[3]);
//          poly_render_polygon(k001005->poly, k001005->bitmap[k001005->bitmap_page],  &visarea, draw_scanline, 1, 4, v);
#endif
			i = index - 1;
		}
		else if (k001005->_3d_fifo[i] == 0x800000ae || k001005->_3d_fifo[i] == 0x8000008e ||
					k001005->_3d_fifo[i] == 0x80000096 || k001005->_3d_fifo[i] == 0x800000b6 ||
					k001005->_3d_fifo[i] == 0x8000002e || k001005->_3d_fifo[i] == 0x8000000e ||
					k001005->_3d_fifo[i] == 0x80000016 || k001005->_3d_fifo[i] == 0x80000036 ||
					k001005->_3d_fifo[i] == 0x800000aa || k001005->_3d_fifo[i] == 0x800000a8 ||
					k001005->_3d_fifo[i] == 0x800000b2)
		{
			// 0x00: xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx    Command
			//
			// 0x01: xxxx---- -------- -------- --------    Texture palette
			// 0x01: -------- -------x xxxx---- --------    Texture page
			// 0x01: -------- -------- ----x-x- x-x-x-x-    Texture X / 8
			// 0x01: -------- -------- -----x-x -x-x-x-x    Texture Y / 8

			poly_extra_data *extra = (poly_extra_data *)poly_get_extra_data(k001005->poly);
			poly_vertex v[4];
			int tx, ty;
			UINT32 color = 0;
			UINT32 header;
			UINT32 command;
			int num_verts = 0;
			int index = i;
			int poly_type = 0;

			command = k001005->_3d_fifo[index++];
			header = k001005->_3d_fifo[index++];

			for (j = 0; j < 4; j++)
			{
				INT16 u2, v2;
				int x, y, z;
				int end = 0;

				x = ((k001005->_3d_fifo[index] >>  0) & 0x3fff);
				y = ((k001005->_3d_fifo[index] >> 16) & 0x1fff);
				x |= ((x & 0x2000) ? 0xffffc000 : 0);
				y |= ((y & 0x1000) ? 0xffffe000 : 0);

				poly_type = k001005->_3d_fifo[index] & 0x4000;
				end = k001005->_3d_fifo[index] & 0x8000;
				++index;

				z = k001005->_3d_fifo[index];
				++index;

				if (end)
				{
					color = k001005->_3d_fifo[index];
					++index;

					u2 = (k001005->_3d_fifo[index] >> 16) & 0xffff;
					v2 = (k001005->_3d_fifo[index] >>  0) & 0xffff;
					++index;
				}
				else
				{
					u2 = (k001005->_3d_fifo[index] >> 16) & 0xffff;
					v2 = (k001005->_3d_fifo[index] >>  0) & 0xffff;
					++index;
				}

				v[j].x = ((float)(x) / 16.0f) + 256.0f;
				v[j].y = ((float)(-y) / 16.0f) + 192.0f;
				v[j].p[0] = *(float*)(&z);
				v[j].p[3] = 1.0f / v[j].p[0];
				v[j].p[1] = u2 * v[j].p[3];
				v[j].p[2] = v2 * v[j].p[3];

				++num_verts;

				if (end)
					break;
			}

			ty = ((header & 0x400) >> 5) |
					((header & 0x100) >> 4) |
					((header & 0x040) >> 3) |
					((header & 0x010) >> 2) |
					((header & 0x004) >> 1) |
					((header & 0x001) >> 0);

			tx = ((header & 0x800) >> 6) |
					((header & 0x200) >> 5) |
					((header & 0x080) >> 4) |
					((header & 0x020) >> 3) |
					((header & 0x008) >> 2) |
					((header & 0x002) >> 1);

			extra->texture_x = tx * 8;
			extra->texture_y = ty * 8;

			extra->texture_page = (header >> 12) & 0x1f;
			extra->texture_palette = (header >> 28) & 0xf;

			extra->texture_mirror_x = ((command & 0x10) ? 0x2 : 0) | ((header & 0x00400000) ? 0x1 : 0);
			extra->texture_mirror_y = ((command & 0x10) ? 0x2 : 0) | ((header & 0x00400000) ? 0x1 : 0);

			extra->color = color;

			if (num_verts < 3)
			{
#if POLY_DEVICE
				poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline_tex, 4, &k001005->prev_v[2], &v[0], &v[1]);
				if (k001005->prev_poly_type)
					poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline_tex, 4, &k001005->prev_v[2], &k001005->prev_v[3], &v[0]);
//              if (k001005->prev_poly_type)
//                  poly_render_quad(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline_tex, 4, &k001005->prev_v[2], &k001005->prev_v[3], &v[0], &v[1]);
//              else
//                  poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline_tex, 4, &k001005->prev_v[2], &v[0], &v[1]);
#endif
				memcpy(&k001005->prev_v[0], &k001005->prev_v[2], sizeof(poly_vertex));
				memcpy(&k001005->prev_v[1], &k001005->prev_v[3], sizeof(poly_vertex));
				memcpy(&k001005->prev_v[2], &v[0], sizeof(poly_vertex));
				memcpy(&k001005->prev_v[3], &v[1], sizeof(poly_vertex));
			}
			else
			{
#if POLY_DEVICE
				poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline_tex, 4, &v[0], &v[1], &v[2]);
				if (num_verts > 3)
					poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline_tex, 4, &v[2], &v[3], &v[0]);
//              poly_render_polygon(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline_tex, 4, num_verts, v);
#endif
				memcpy(k001005->prev_v, v, sizeof(poly_vertex) * 4);
			}

			k001005->prev_poly_type = poly_type;

			while ((k001005->_3d_fifo[index] & 0xffffff00) != 0x80000000 && index < k001005->_3d_fifo_ptr)
			{
				poly_extra_data *extra = (poly_extra_data *)poly_get_extra_data(k001005->poly);
#if POLY_DEVICE
				int new_verts = 0;
#endif
				if (poly_type)
				{
					memcpy(&v[0], &k001005->prev_v[2], sizeof(poly_vertex));
					memcpy(&v[1], &k001005->prev_v[3], sizeof(poly_vertex));
				}
				else
				{
					memcpy(&v[0], &k001005->prev_v[1], sizeof(poly_vertex));
					memcpy(&v[1], &k001005->prev_v[2], sizeof(poly_vertex));
				}

				for (j = 2; j < 4; j++)
				{
					INT16 u2, v2;
					int x, y, z;
					int end = 0;

					x = ((k001005->_3d_fifo[index] >>  0) & 0x3fff);
					y = ((k001005->_3d_fifo[index] >> 16) & 0x1fff);
					x |= ((x & 0x2000) ? 0xffffc000 : 0);
					y |= ((y & 0x1000) ? 0xffffe000 : 0);

					poly_type = k001005->_3d_fifo[index] & 0x4000;
					end = k001005->_3d_fifo[index] & 0x8000;
					++index;

					z = k001005->_3d_fifo[index];
					++index;

					if (end)
					{
						color = k001005->_3d_fifo[index];
						++index;

						u2 = (k001005->_3d_fifo[index] >> 16) & 0xffff;
						v2 = (k001005->_3d_fifo[index] >>  0) & 0xffff;
						++index;
					}
					else
					{
						u2 = (k001005->_3d_fifo[index] >> 16) & 0xffff;
						v2 = (k001005->_3d_fifo[index] >>  0) & 0xffff;
						++index;
					}

					v[j].x = ((float)(x) / 16.0f) + 256.0f;
					v[j].y = ((float)(-y) / 16.0f) + 192.0f;
					v[j].p[0] = *(float*)(&z);
					v[j].p[3] = 1.0f / v[j].p[0];
					v[j].p[1] = u2 * v[j].p[3];
					v[j].p[2] = v2 * v[j].p[3];

#if POLY_DEVICE
					++new_verts;
#endif

					if (end)
						break;
				}

				extra->texture_x = tx * 8;
				extra->texture_y = ty * 8;

				extra->texture_page = (header >> 12) & 0x1f;
				extra->texture_palette = (header >> 28) & 0xf;

				extra->texture_mirror_x = ((command & 0x10) ? 0x2 : 0) | ((header & 0x00400000) ? 0x1 : 0);
				extra->texture_mirror_y = ((command & 0x10) ? 0x2 : 0) | ((header & 0x00400000) ? 0x1 : 0);

				extra->color = color;

#if POLY_DEVICE
				poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline_tex, 4, &v[0], &v[1], &v[2]);
				if (new_verts > 1)
					poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline_tex, 4, &v[2], &v[3], &v[0]);
//              poly_render_polygon(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline_tex, 4, new_verts + 2, v);
#endif
				memcpy(k001005->prev_v, v, sizeof(poly_vertex) * 4);
			};

			i = index - 1;
		}
		else if (k001005->_3d_fifo[i] == 0x80000006 || k001005->_3d_fifo[i] == 0x80000026 ||
					k001005->_3d_fifo[i] == 0x80000020 || k001005->_3d_fifo[i] == 0x80000022)
		{
			poly_extra_data *extra = (poly_extra_data *)poly_get_extra_data(k001005->poly);
			poly_vertex v[4];
			int r, g, b, a;
			UINT32 color;
			int num_verts = 0;
			int index = i;
			int poly_type = 0;

			++index;

			for (j = 0; j < 4; j++)
			{
				int x, y, z;
				int end = 0;

				x = ((k001005->_3d_fifo[index] >>  0) & 0x3fff);
				y = ((k001005->_3d_fifo[index] >> 16) & 0x1fff);
				x |= ((x & 0x2000) ? 0xffffc000 : 0);
				y |= ((y & 0x1000) ? 0xffffe000 : 0);

				poly_type = k001005->_3d_fifo[index] & 0x4000;
				end = k001005->_3d_fifo[index] & 0x8000;
				++index;

				z = k001005->_3d_fifo[index];
				++index;

				v[j].x = ((float)(x) / 16.0f) + 256.0f;
				v[j].y = ((float)(-y) / 16.0f) + 192.0f;
				v[j].p[0] = *(float*)(&z);

				++num_verts;

				if (end)
					break;
			}

			r = (k001005->_3d_fifo[index] >>  0) & 0xff;
			g = (k001005->_3d_fifo[index] >>  8) & 0xff;
			b = (k001005->_3d_fifo[index] >> 16) & 0xff;
			a = (k001005->_3d_fifo[index] >> 24) & 0xff;
			color = (a << 24) | (r << 16) | (g << 8) | (b);
			index++;

			extra->color = color;

#if POLY_DEVICE
			poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline, 1, &v[0], &v[1], &v[2]);
			if (num_verts > 3)
				poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline, 1, &v[2], &v[3], &v[0]);
//          poly_render_polygon(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline, 1, num_verts, v);
#endif
			memcpy(k001005->prev_v, v, sizeof(poly_vertex) * 4);

			while ((k001005->_3d_fifo[index] & 0xffffff00) != 0x80000000 && index < k001005->_3d_fifo_ptr)
			{
				poly_extra_data *extra = (poly_extra_data *)poly_get_extra_data(k001005->poly);
				int new_verts = 0;

				if (poly_type)
				{
					memcpy(&v[0], &k001005->prev_v[2], sizeof(poly_vertex));
					memcpy(&v[1], &k001005->prev_v[3], sizeof(poly_vertex));
				}
				else
				{
					memcpy(&v[0], &k001005->prev_v[1], sizeof(poly_vertex));
					memcpy(&v[1], &k001005->prev_v[2], sizeof(poly_vertex));
				}

				for (j = 2; j < 4; j++)
				{
					int x, y, z;
					int end = 0;

					x = ((k001005->_3d_fifo[index] >>  0) & 0x3fff);
					y = ((k001005->_3d_fifo[index] >> 16) & 0x1fff);
					x |= ((x & 0x2000) ? 0xffffc000 : 0);
					y |= ((y & 0x1000) ? 0xffffe000 : 0);

					poly_type = k001005->_3d_fifo[index] & 0x4000;
					end = k001005->_3d_fifo[index] & 0x8000;
					++index;

					z = k001005->_3d_fifo[index];
					++index;

					v[j].x = ((float)(x) / 16.0f) + 256.0f;
					v[j].y = ((float)(-y) / 16.0f) + 192.0f;
					v[j].p[0] = *(float*)(&z);

					++new_verts;

					if (end)
						break;
				}

				r = (k001005->_3d_fifo[index] >>  0) & 0xff;
				g = (k001005->_3d_fifo[index] >>  8) & 0xff;
				b = (k001005->_3d_fifo[index] >> 16) & 0xff;
				a = (k001005->_3d_fifo[index] >> 24) & 0xff;
				color = (a << 24) | (r << 16) | (g << 8) | (b);
				index++;

				extra->color = color;

#if POLY_DEVICE
				poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline, 1, &v[0], &v[1], &v[2]);
				if (new_verts > 1)
					poly_render_triangle(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline, 1, &v[0], &v[2], &v[3]);
//              poly_render_polygon(k001005->poly, k001005->bitmap[k001005->bitmap_page], &visarea, draw_scanline, 1, new_verts + 2, v);
#endif
				memcpy(k001005->prev_v, v, sizeof(poly_vertex) * 4);
			};

			i = index - 1;
		}
		else if (k001005->_3d_fifo[i] == 0x80000000)
		{
		}
		else if ((k001005->_3d_fifo[i] & 0xffffff00) == 0x80000000)
		{
			/*
			mame_printf_debug("Unknown polygon type %08X:\n", k001005->_3d_fifo[i]);
			for (j = 0; j < 0x20; j++)
			{
			    mame_printf_debug("  %02X: %08X\n", j, k001005->_3d_fifo[i + 1 + j]);
			}
			mame_printf_debug("\n");
			*/
		}
	}
}

void k001005_draw( device_t *device, bitmap_rgb32 &bitmap, const rectangle &cliprect )
{
	k001005_state *k001005 = k001005_get_safe_token(device);
	int i, j;

	memcpy(&k001005->cliprect, &cliprect, sizeof(rectangle));

	for (j = cliprect.min_y; j <= cliprect.max_y; j++)
	{
		UINT32 *bmp = &bitmap.pix32(j);
		UINT32 *src = &k001005->bitmap[k001005->bitmap_page ^ 1]->pix32(j);

		for (i = cliprect.min_x; i <= cliprect.max_x; i++)
		{
			if (src[i] & 0xff000000)
			{
				bmp[i] = src[i];
			}
		}
	}
}

/*****************************************************************************
    DEVICE INTERFACE
*****************************************************************************/

static DEVICE_START( k001005 )
{
	k001005_state *k001005 = k001005_get_safe_token(device);
	const k001005_interface *intf = k001005_get_interface(device);
	int i, width, height;

	k001005->cpu = device->machine().device(intf->cpu);
	k001005->dsp = device->machine().device(intf->dsp);
	k001005->k001006_1 = device->machine().device(intf->k001006_1);
	k001005->k001006_2 = device->machine().device(intf->k001006_2);

	k001005->screen = device->machine().device<screen_device>(intf->screen);
	width = k001005->screen->width();
	height = k001005->screen->height();
	k001005->zbuffer = auto_bitmap_ind32_alloc(device->machine(), width, height);

	k001005->gfxrom = device->machine().root_device().memregion(intf->gfx_memory_region)->base();

	k001005->bitmap[0] = auto_bitmap_rgb32_alloc(device->machine(), k001005->screen->width(), k001005->screen->height());
	k001005->bitmap[1] = auto_bitmap_rgb32_alloc(device->machine(), k001005->screen->width(), k001005->screen->height());

	k001005->texture = auto_alloc_array(device->machine(), UINT8, 0x800000);

	k001005->ram[0] = auto_alloc_array(device->machine(), UINT16, 0x140000);
	k001005->ram[1] = auto_alloc_array(device->machine(), UINT16, 0x140000);

	k001005->fifo = auto_alloc_array(device->machine(), UINT32, 0x800);

	k001005->_3d_fifo = auto_alloc_array(device->machine(), UINT32, 0x10000);

	k001005->poly = poly_alloc(device->machine(), 4000, sizeof(poly_extra_data), POLYFLAG_ALLOW_QUADS);

	for (i = 0; i < 128; i++)
	{
		k001005->tex_mirror_table[0][i] = i & 0x3f;
		k001005->tex_mirror_table[1][i] = i & 0x3f;
		k001005->tex_mirror_table[2][i] = ((i & 0x3f) >= 0x20) ? (0x1f - (i & 0x1f)) : i & 0x1f;
		k001005->tex_mirror_table[3][i] = ((i & 0x7f) >= 0x40) ? (0x3f - (i & 0x3f)) : i & 0x3f;
	}


	device->save_pointer(NAME(k001005->texture), 0x800000);
	device->save_pointer(NAME(k001005->ram[0]), 0x140000);
	device->save_pointer(NAME(k001005->ram[1]), 0x140000);
	device->save_pointer(NAME(k001005->fifo), 0x800);
	device->save_pointer(NAME(k001005->_3d_fifo), 0x10000);
	device->save_item(NAME(k001005->status));
	device->save_item(NAME(k001005->ram_ptr));
	device->save_item(NAME(k001005->fifo_read_ptr));
	device->save_item(NAME(k001005->fifo_write_ptr));
	device->save_item(NAME(k001005->_3d_fifo_ptr));
	device->save_item(NAME(k001005->bitmap_page));
	device->save_item(NAME(k001005->prev_poly_type));
	device->save_item(NAME(*k001005->bitmap[0]));
	device->save_item(NAME(*k001005->bitmap[1]));
	device->save_item(NAME(*k001005->zbuffer));

	// FIXME: shall we save poly as well?
}

static DEVICE_RESET( k001005 )
{
	k001005_state *k001005 = k001005_get_safe_token(device);

	k001005->status = 0;
	k001005->ram_ptr = 0;
	k001005->fifo_read_ptr = 0;
	k001005->fifo_write_ptr = 0;
	k001005->_3d_fifo_ptr = 0;
	k001005->bitmap_page = 0;

	memset(k001005->prev_v, 0, sizeof(k001005->prev_v));
	k001005->prev_poly_type = 0;
}

static DEVICE_STOP( k001005 )
{
	k001005_state *k001005 = k001005_get_safe_token(device);
	poly_free(k001005->poly);
}

const device_type K001005 = &device_creator<k001005_device>;

k001005_device::k001005_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K001005, "Konami 001005", tag, owner, clock, "k001005", __FILE__)
{
	m_token = global_alloc_clear(k001005_state);
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k001005_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k001005_device::device_start()
{
	DEVICE_START_NAME( k001005 )(this);
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k001005_device::device_reset()
{
	DEVICE_RESET_NAME( k001005 )(this);
}

//-------------------------------------------------
//  device_stop - device-specific stop
//-------------------------------------------------

void k001005_device::device_stop()
{
	DEVICE_STOP_NAME( k001005 )(this);
}

/***************************************************************************/
/*                                                                         */
/*                                  001604                                 */
/*                                                                         */
/***************************************************************************/


struct k001604_state
{
	screen_device *screen;
	tilemap_t        *layer_8x8[2];
	tilemap_t        *layer_roz;
	int            gfx_index[2];

	UINT32 *       tile_ram;
	UINT32 *       char_ram;
	UINT32 *       reg;

	int            layer_size;
	int            roz_size;
	int            txt_mem_offset;
	int            roz_mem_offset;
};


#define K001604_NUM_TILES_LAYER0        16384
#define K001604_NUM_TILES_LAYER1        4096

/*****************************************************************************
    INLINE FUNCTIONS
*****************************************************************************/

INLINE k001604_state *k001604_get_safe_token( device_t *device )
{
	assert(device != NULL);
	assert(device->type() == K001604);

	return (k001604_state *)downcast<k001604_device *>(device)->token();
}

INLINE const k001604_interface *k001604_get_interface( device_t *device )
{
	assert(device != NULL);
	assert((device->type() == K001604));
	return (const k001604_interface *) device->static_config();
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

static const gfx_layout k001604_char_layout_layer_8x8 =
{
	8, 8,
	K001604_NUM_TILES_LAYER0,
	8,
	{ 8,9,10,11,12,13,14,15 },
	{ 1*16, 0*16, 3*16, 2*16, 5*16, 4*16, 7*16, 6*16 },
	{ 0*128, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128 },
	8*128
};

static const gfx_layout k001604_char_layout_layer_16x16 =
{
	16, 16,
	K001604_NUM_TILES_LAYER1,
	8,
	{ 8,9,10,11,12,13,14,15 },
	{ 1*16, 0*16, 3*16, 2*16, 5*16, 4*16, 7*16, 6*16, 9*16, 8*16, 11*16, 10*16, 13*16, 12*16, 15*16, 14*16 },
	{ 0*256, 1*256, 2*256, 3*256, 4*256, 5*256, 6*256, 7*256, 8*256, 9*256, 10*256, 11*256, 12*256, 13*256, 14*256, 15*256 },
	16*256
};


/* FIXME: The TILEMAP_MAPPER below depends on parameters passed by the device interface (being game dependent).
we might simplify the code, by passing the whole TILEMAP_MAPPER as a callback in the interface, but is it really worth? */

TILEMAP_MAPPER_MEMBER(k001604_device::k001604_scan_layer_8x8_0_size0)
{
	k001604_state *k001604 = k001604_get_safe_token(this);

	/* logical (col,row) -> memory offset */
	return (row * 128) + col + k001604->txt_mem_offset;
}

TILEMAP_MAPPER_MEMBER(k001604_device::k001604_scan_layer_8x8_0_size1)
{
	k001604_state *k001604 = k001604_get_safe_token(this);

	/* logical (col,row) -> memory offset */
	return (row * 256) + col + k001604->txt_mem_offset;
}

TILEMAP_MAPPER_MEMBER(k001604_device::k001604_scan_layer_8x8_1_size0)
{
	k001604_state *k001604 = k001604_get_safe_token(this);

	/* logical (col,row) -> memory offset */
	return (row * 128) + col + 64 + k001604->txt_mem_offset;
}

TILEMAP_MAPPER_MEMBER(k001604_device::k001604_scan_layer_8x8_1_size1)
{
	k001604_state *k001604 = k001604_get_safe_token(this);

	/* logical (col,row) -> memory offset */
	return (row * 256) + col + 64 + k001604->txt_mem_offset;
}

TILEMAP_MAPPER_MEMBER(k001604_device::k001604_scan_layer_roz_128)
{
	k001604_state *k001604 = k001604_get_safe_token(this);

	/* logical (col,row) -> memory offset */
	return (row * 128) + col + k001604->roz_mem_offset;
}

TILEMAP_MAPPER_MEMBER(k001604_device::k001604_scan_layer_roz_256)
{
	k001604_state *k001604 = k001604_get_safe_token(this);

	/* logical (col,row) -> memory offset */
	return (row * 256) + col + 128 + k001604->roz_mem_offset;
}

TILE_GET_INFO_MEMBER(k001604_device::k001604_tile_info_layer_8x8)
{
	k001604_state *k001604 = k001604_get_safe_token(this);
	UINT32 val = k001604->tile_ram[tile_index];
	int color = (val >> 17) & 0x1f;
	int tile = (val & 0x7fff);
	int flags = 0;

	if (val & 0x400000)
		flags |= TILE_FLIPX;
	if (val & 0x800000)
		flags |= TILE_FLIPY;

	SET_TILE_INFO_MEMBER(k001604->gfx_index[0], tile, color, flags);
}

TILE_GET_INFO_MEMBER(k001604_device::k001604_tile_info_layer_roz)
{
	k001604_state *k001604 = k001604_get_safe_token(this);
	UINT32 val = k001604->tile_ram[tile_index];
	int flags = 0;
	int color = (val >> 17) & 0x1f;
	int tile = k001604->roz_size ? (val & 0x7ff) : (val & 0x1fff);

	if (val & 0x400000)
		flags |= TILE_FLIPX;
	if (val & 0x800000)
		flags |= TILE_FLIPY;

	tile += k001604->roz_size ? 0x800 : 0x2000;

	SET_TILE_INFO_MEMBER(k001604->gfx_index[k001604->roz_size], tile, color, flags);
}


void k001604_draw_back_layer( device_t *device, bitmap_rgb32 &bitmap, const rectangle &cliprect )
{
	k001604_state *k001604 = k001604_get_safe_token(device);
	bitmap.fill(0, cliprect);

	if ((k001604->reg[0x60 / 4] & 0x40000000) == 0)
		return;

	int tile_size = k001604->roz_size ? 16 : 8;

	INT32 x  = (INT16)((k001604->reg[0x08] >> 16) & 0xffff);
	INT32 y  = (INT16)((k001604->reg[0x08] >>  0) & 0xffff);
	INT32 xx = (INT16)((k001604->reg[0x09] >>  0) & 0xffff);
	INT32 xy = (INT16)((k001604->reg[0x09] >> 16) & 0xffff);
	INT32 yx = (INT16)((k001604->reg[0x0a] >>  0) & 0xffff);
	INT32 yy = (INT16)((k001604->reg[0x0a] >> 16) & 0xffff);

	int pivotx = (INT16)((k001604->reg[0x00] >> 16) & 0xffff);
	int pivoty = (INT16)((k001604->reg[0x00] >>  0) & 0xffff);

	int startx  = ((x - pivotx) * 256) * 32;
	int starty  = ((y - pivoty) * 256) * 32;
	int incxx = (xx) * 32;
	int incxy = (-xy) * 32;
	int incyx = (-yx) * 32;
	int incyy = (yy) * 32;

	bitmap_ind16& pixmap = k001604->layer_roz->pixmap();

	// extract start/end points
	int sx = cliprect.min_x;
	int sy = cliprect.min_y;
	int ex = cliprect.max_x;
	int ey = cliprect.max_y;

	const rgb_t *clut = palette_entry_list_raw(bitmap.palette());

	int window_x, window_y, window_xmask, window_ymask;

	int layer_size = (k001604->reg[0x1b] >> 9) & 3;

	if (k001604->roz_size)
		window_x = ((k001604->reg[0x1b] >> 1) & 3) * 512;
	else
		window_x = ((k001604->reg[0x1b] >> 1) & 1) * 512;

	window_y = 0;

	switch (layer_size)
	{
		case 0: window_xmask = (128 * tile_size) - 1; break;
		case 2: window_xmask = (64 * tile_size) - 1; break;
		case 3: window_xmask = (32 * tile_size) - 1; break;
		default: fatalerror("k001604_draw_back_layer(): layer_size %d\n", layer_size); break;
	}

	window_ymask = pixmap.height() - 1;


	// loop over rows
	while (sy <= ey)
	{
		// initialize X counters
		int x = sx;
		UINT32 cx = startx;
		UINT32 cy = starty;

		UINT32 *dest = &bitmap.pix(sy, sx);

		// loop over columns
		while (x <= ex)
		{
			*dest = clut[pixmap.pix16(((cy >> 16) & window_ymask) + window_y, ((cx >> 16) & window_xmask) + window_x)];

			// advance in X
			cx += incxx;
			cy += incxy;
			x++;
			dest++;
		}

		// advance in Y
		startx += incyx;
		starty += incyy;
		sy++;
	}
}

void k001604_draw_front_layer( device_t *device, bitmap_rgb32 &bitmap, const rectangle &cliprect )
{
	k001604_state *k001604 = k001604_get_safe_token(device);

	k001604->layer_8x8[0]->set_scrollx(-cliprect.min_x);
	k001604->layer_8x8[0]->set_scrolly(-cliprect.min_y);

	k001604->layer_8x8[1]->set_scrollx(-cliprect.min_x);
	k001604->layer_8x8[1]->set_scrolly(-cliprect.min_y);

	//k001604->layer_8x8[1]->draw(bitmap, cliprect, 0,0);
	k001604->layer_8x8[0]->draw(bitmap, cliprect, 0,0);
}

READ32_DEVICE_HANDLER( k001604_tile_r )
{
	k001604_state *k001604 = k001604_get_safe_token(device);

	return k001604->tile_ram[offset];
}

READ32_DEVICE_HANDLER( k001604_char_r )
{
	k001604_state *k001604 = k001604_get_safe_token(device);

	int set, bank;
	UINT32 addr;

	set = (k001604->reg[0x60 / 4] & 0x1000000) ? 0x100000 : 0;

	if (set)
		bank = (k001604->reg[0x60 / 4] >> 8) & 0x3;
	else
		bank = (k001604->reg[0x60 / 4] & 0x3);

	addr = offset + ((set + (bank * 0x40000)) / 4);

	return k001604->char_ram[addr];
}

READ32_DEVICE_HANDLER( k001604_reg_r )
{
	k001604_state *k001604 = k001604_get_safe_token(device);

	switch (offset)
	{
		case 0x54/4:    return space.machine().rand() << 16;
		case 0x5c/4:    return space.machine().rand() << 16 | space.machine().rand();
	}

	return k001604->reg[offset];
}

WRITE32_DEVICE_HANDLER( k001604_tile_w )
{
	k001604_state *k001604 = k001604_get_safe_token(device);

	int x/*, y*/;
	COMBINE_DATA(k001604->tile_ram + offset);

	if (k001604->layer_size)
	{
		x = offset & 0xff;
		/*y = offset / 256;*/
	}
	else
	{
		x = offset & 0x7f;
		/*y = offset / 128;*/
	}

	if (k001604->layer_size)
	{
		if (x < 64)
		{
			k001604->layer_8x8[0]->mark_tile_dirty(offset);
		}
		else if (x < 128)
		{
			k001604->layer_8x8[1]->mark_tile_dirty(offset);
		}
		else
		{
			k001604->layer_roz->mark_tile_dirty(offset);
		}
	}
	else
	{
		if (x < 64)
		{
			k001604->layer_8x8[0]->mark_tile_dirty(offset);
		}
		else
		{
			k001604->layer_8x8[1]->mark_tile_dirty(offset);
		}

		k001604->layer_roz->mark_tile_dirty(offset);
	}
}

WRITE32_DEVICE_HANDLER( k001604_char_w )
{
	k001604_state *k001604 = k001604_get_safe_token(device);

	int set, bank;
	UINT32 addr;

	set = (k001604->reg[0x60/4] & 0x1000000) ? 0x100000 : 0;

	if (set)
		bank = (k001604->reg[0x60 / 4] >> 8) & 0x3;
	else
		bank = (k001604->reg[0x60 / 4] & 0x3);

	addr = offset + ((set + (bank * 0x40000)) / 4);

	COMBINE_DATA(k001604->char_ram + addr);

	space.machine().gfx[k001604->gfx_index[0]]->mark_dirty(addr / 32);
	space.machine().gfx[k001604->gfx_index[1]]->mark_dirty(addr / 128);
}

WRITE32_DEVICE_HANDLER( k001604_reg_w )
{
	k001604_state *k001604 = k001604_get_safe_token(device);

	COMBINE_DATA(k001604->reg + offset);

	switch (offset)
	{
		case 0x8:
		case 0x9:
		case 0xa:
			//printf("K001604_reg_w %02X, %08X, %08X\n", offset, data, mem_mask);
			break;
	}

	if (offset != 0x08 && offset != 0x09 && offset != 0x0a /*&& offset != 0x17 && offset != 0x18*/)
	{
		//printf("K001604_reg_w (%d), %02X, %08X, %08X at %08X\n", chip, offset, data, mem_mask, space.device().safe_pc());
	}
}

const device_type K001604 = &device_creator<k001604_device>;

k001604_device::k001604_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K001604, "Konami 001604", tag, owner, clock, "k001604", __FILE__)
{
	m_token = global_alloc_clear(k001604_state);
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k001604_device::device_config_complete()
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k001604_device::device_start()
{
	k001604_state *k001604 = k001604_get_safe_token(this);
	const k001604_interface *intf = k001604_get_interface(this);
	int roz_tile_size;

	k001604->layer_size = intf->layer_size;     // 0 -> width = 128 tiles, 1 -> width = 256 tiles
	k001604->roz_size = intf->roz_size;     // 0 -> 8x8, 1 -> 16x16

	k001604->txt_mem_offset = intf->txt_mem_offset;
	k001604->roz_mem_offset = intf->roz_mem_offset;

	k001604->gfx_index[0] = intf->gfx_index_1;
	k001604->gfx_index[1] = intf->gfx_index_2;

	k001604->char_ram = auto_alloc_array(machine(), UINT32, 0x200000 / 4);
	k001604->tile_ram = auto_alloc_array(machine(), UINT32, 0x20000 / 4);
	k001604->reg = auto_alloc_array(machine(), UINT32, 0x400 / 4);

	/* create tilemaps */
	roz_tile_size = k001604->roz_size ? 16 : 8;

	if (k001604->layer_size)
	{
		k001604->layer_8x8[0] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k001604_device::k001604_tile_info_layer_8x8),this), tilemap_mapper_delegate(FUNC(k001604_device::k001604_scan_layer_8x8_0_size1),this), 8, 8, 64, 64);
		k001604->layer_8x8[1] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k001604_device::k001604_tile_info_layer_8x8),this), tilemap_mapper_delegate(FUNC(k001604_device::k001604_scan_layer_8x8_1_size1),this), 8, 8, 64, 64);

		k001604->layer_roz = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k001604_device::k001604_tile_info_layer_roz),this), tilemap_mapper_delegate(FUNC(k001604_device::k001604_scan_layer_roz_256),this), roz_tile_size, roz_tile_size, 128, 64);
	}
	else
	{
		k001604->layer_8x8[0] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k001604_device::k001604_tile_info_layer_8x8),this), tilemap_mapper_delegate(FUNC(k001604_device::k001604_scan_layer_8x8_0_size0),this), 8, 8, 64, 64);
		k001604->layer_8x8[1] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k001604_device::k001604_tile_info_layer_8x8),this), tilemap_mapper_delegate(FUNC(k001604_device::k001604_scan_layer_8x8_1_size0),this), 8, 8, 64, 64);

		k001604->layer_roz = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k001604_device::k001604_tile_info_layer_roz),this), tilemap_mapper_delegate(FUNC(k001604_device::k001604_scan_layer_roz_128),this), roz_tile_size, roz_tile_size, 128, 64);
	}

	k001604->layer_8x8[0]->set_transparent_pen(0);
	k001604->layer_8x8[1]->set_transparent_pen(0);

	machine().gfx[k001604->gfx_index[0]] = auto_alloc(machine(), gfx_element(machine(), k001604_char_layout_layer_8x8, (UINT8*)&k001604->char_ram[0], machine().total_colors() / 16, 0));
	machine().gfx[k001604->gfx_index[1]] = auto_alloc(machine(), gfx_element(machine(), k001604_char_layout_layer_16x16, (UINT8*)&k001604->char_ram[0], machine().total_colors() / 16, 0));

	save_pointer(NAME(k001604->reg), 0x400 / 4);
	save_pointer(NAME(k001604->char_ram), 0x200000 / 4);
	save_pointer(NAME(k001604->tile_ram), 0x20000 / 4);

}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k001604_device::device_reset()
{
	k001604_state *k001604 = k001604_get_safe_token(this);

	memset(k001604->char_ram, 0, 0x200000);
	memset(k001604->tile_ram, 0, 0x10000);
	memset(k001604->reg, 0, 0x400);
}


// from drivers/hornet.c

/***************************************************************************/
/*                                                                         */
/*                                  037122                                 */
/*                                                                         */
/***************************************************************************/

#define K037122_NUM_TILES       16384

const device_type K037122 = &device_creator<k037122_device>;

k037122_device::k037122_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, K037122, "Konami 0371222", tag, owner, clock, "k037122", __FILE__),
	m_screen(NULL),
	m_tile_ram(NULL),
	m_char_ram(NULL),
	m_reg(NULL)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void k037122_device::device_config_complete()
{
	// inherit a copy of the static data
	const k037122_interface *intf = reinterpret_cast<const k037122_interface *>(static_config());
	if (intf != NULL)
		*static_cast<k037122_interface *>(this) = *intf;

	// or initialize to defaults if none provided
	else
	{
		m_screen_tag = "";
		m_gfx_index = 0;
	}
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void k037122_device::device_start()
{
	static const gfx_layout k037122_char_layout =
	{
	8, 8,
	K037122_NUM_TILES,
	8,
	{ 0,1,2,3,4,5,6,7 },
	{ 1*16, 0*16, 3*16, 2*16, 5*16, 4*16, 7*16, 6*16 },
	{ 0*128, 1*128, 2*128, 3*128, 4*128, 5*128, 6*128, 7*128 },
	8*128
	};	
	
	m_screen = machine().device<screen_device>(m_screen_tag);
	
	m_char_ram = auto_alloc_array_clear(machine(), UINT32, 0x200000 / 4);
	m_tile_ram = auto_alloc_array_clear(machine(), UINT32, 0x20000 / 4);
	m_reg = auto_alloc_array_clear(machine(), UINT32, 0x400 / 4);

	m_layer[0] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k037122_device::tile_info_layer0),this), TILEMAP_SCAN_ROWS, 8, 8, 256, 64);
	m_layer[1] = &machine().tilemap().create(tilemap_get_info_delegate(FUNC(k037122_device::tile_info_layer1),this), TILEMAP_SCAN_ROWS, 8, 8, 128, 64);

	m_layer[0]->set_transparent_pen(0);
	m_layer[1]->set_transparent_pen(0);

	machine().gfx[m_gfx_index] = auto_alloc_clear(machine(), gfx_element(machine(), k037122_char_layout, (UINT8*)m_char_ram, machine().total_colors() / 16, 0));

	save_pointer(NAME(m_reg), 0x400 / 4);
	save_pointer(NAME(m_char_ram), 0x200000 / 4);
	save_pointer(NAME(m_tile_ram), 0x20000 / 4);

}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void k037122_device::device_reset()
{
	memset(m_char_ram, 0, 0x200000);
	memset(m_tile_ram, 0, 0x20000);
	memset(m_reg, 0, 0x400);
}

/*****************************************************************************
    DEVICE HANDLERS
*****************************************************************************/

TILE_GET_INFO_MEMBER(k037122_device::tile_info_layer0)
{
	UINT32 val = m_tile_ram[tile_index + (0x8000/4)];
	int color = (val >> 17) & 0x1f;
	int tile = val & 0x3fff;
	int flags = 0;

	if (val & 0x400000)
		flags |= TILE_FLIPX;
	if (val & 0x800000)
		flags |= TILE_FLIPY;

	SET_TILE_INFO_MEMBER(m_gfx_index, tile, color, flags);
}

TILE_GET_INFO_MEMBER(k037122_device::tile_info_layer1)
{
	UINT32 val = m_tile_ram[tile_index];
	int color = (val >> 17) & 0x1f;
	int tile = val & 0x3fff;
	int flags = 0;

	if (val & 0x400000)
		flags |= TILE_FLIPX;
	if (val & 0x800000)
		flags |= TILE_FLIPY;

	SET_TILE_INFO_MEMBER(m_gfx_index, tile, color, flags);
}


void k037122_device::tile_draw( bitmap_rgb32 &bitmap, const rectangle &cliprect )
{
	const rectangle &visarea = m_screen->visible_area();

	if (m_reg[0xc] & 0x10000)
	{
		m_layer[1]->set_scrolldx(visarea.min_x, visarea.min_x);
		m_layer[1]->set_scrolldy(visarea.min_y, visarea.min_y);
		m_layer[1]->draw(bitmap, cliprect, 0, 0);
	}
	else
	{
		m_layer[0]->set_scrolldx(visarea.min_x, visarea.min_x);
		m_layer[0]->set_scrolldy(visarea.min_y, visarea.min_y);
		m_layer[0]->draw(bitmap, cliprect, 0, 0);
	}
}

void k037122_device::update_palette_color( UINT32 palette_base, int color )
{
	UINT32 data = m_tile_ram[(palette_base / 4) + color];

	palette_set_color_rgb(machine(), color, pal5bit(data >> 6), pal6bit(data >> 0), pal5bit(data >> 11));
}

READ32_MEMBER( k037122_device::sram_r )
{
	return m_tile_ram[offset];
}

WRITE32_MEMBER( k037122_device::sram_w )
{
	COMBINE_DATA(m_tile_ram + offset);

	if (m_reg[0xc] & 0x10000)
	{
		if (offset < 0x8000 / 4)
		{
			m_layer[1]->mark_tile_dirty(offset);
		}
		else if (offset >= 0x8000 / 4 && offset < 0x18000 / 4)
		{
			m_layer[0]->mark_tile_dirty(offset - (0x8000 / 4));
		}
		else if (offset >= 0x18000 / 4)
		{
			update_palette_color(0x18000, offset - (0x18000 / 4));
		}
	}
	else
	{
		if (offset < 0x8000 / 4)
		{
			update_palette_color(0, offset);
		}
		else if (offset >= 0x8000 / 4 && offset < 0x18000 / 4)
		{
			m_layer[0]->mark_tile_dirty(offset - (0x8000 / 4));
		}
		else if (offset >= 0x18000 / 4)
		{
			m_layer[1]->mark_tile_dirty(offset - (0x18000 / 4));
		}
	}
}


READ32_MEMBER( k037122_device::char_r )
{
	int bank = m_reg[0x30 / 4] & 0x7;

	return m_char_ram[offset + (bank * (0x40000 / 4))];
}

WRITE32_MEMBER( k037122_device::char_w )
{
	int bank = m_reg[0x30 / 4] & 0x7;
	UINT32 addr = offset + (bank * (0x40000/4));

	COMBINE_DATA(m_char_ram + addr);
	space.machine().gfx[m_gfx_index]->mark_dirty(addr / 32);
}

READ32_MEMBER( k037122_device::reg_r )
{
	switch (offset)
	{
		case 0x14/4:
		{
			return 0x000003fa;
		}
	}
	return m_reg[offset];
}

WRITE32_MEMBER( k037122_device::reg_w )
{
	COMBINE_DATA(m_reg + offset);
}


/***************************************************************************/
/*                                                                         */
/*                         misc debug handlers                             */
/*                                                                         */
/***************************************************************************/

READ16_DEVICE_HANDLER( k053246_reg_word_r )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	return(k053247->kx46_regs[offset * 2] << 8 | k053247->kx46_regs[offset * 2 + 1]);
}   // OBJSET1

READ16_DEVICE_HANDLER( k053247_reg_word_r )
{
	k053247_state *k053247 = k053247_get_safe_token(device);
	return(k053247->kx47_regs[offset]);
}   // OBJSET2

READ16_DEVICE_HANDLER( k054338_word_r )
{
	k054338_state *k054338 = k054338_get_safe_token(device);
	return(k054338->regs[offset]);
}       // CLTC

READ16_DEVICE_HANDLER( k053251_lsb_r )
{
	k053251_state *k053251 = k053251_get_safe_token(device);
	return(k053251->ram[offset]);
}       // PCU1

READ16_DEVICE_HANDLER( k053251_msb_r )
{
	k053251_state *k053251 = k053251_get_safe_token(device);
	return(k053251->ram[offset] << 8);
}       // PCU1

READ16_DEVICE_HANDLER( k055555_word_r )
{
	k055555_state *k055555 = k055555_get_safe_token(device);
	return(k055555->regs[offset] << 8);
}   // PCU2


READ32_DEVICE_HANDLER( k053247_reg_long_r )
{
	offset <<= 1;
	return (k053247_reg_word_r(device, space, offset + 1, 0xffff) | k053247_reg_word_r(device, space, offset, 0xffff) << 16);
}

READ32_DEVICE_HANDLER( k055555_long_r )
{
	offset <<= 1;
	return (k055555_word_r(device, space, offset + 1, 0xffff) | k055555_word_r(device, space, offset, 0xffff) << 16);
}
