/***************************************************************************

Pong (c) 1972 Atari

driver by Couriersud

Notes:

TODO:

***************************************************************************/


#include "emu.h"

#include "machine/rescap.h"
#include "machine/netlist.h"
#include "netlist/devices/net_lib.h"
#include "sound/dac.h"
#include "video/fixfreq.h"
#include "astring.h"

/*
 * H count width to 512
 * Reset at 1C6 = 454
 * V count width to 512, counts on HReset
 * Reset at 105 = 261

 * Clock = 7.159 MHz

 * ==> 15.768 Khz Horz Freq
 * ==> 60.41 Refresh

 * HBlank 0 to 79
 * HSync 32 to 63
 * VBlank 0 to 15
 * VSync 4 to 7

 * Video = (HVID & VVID ) & (NET & PAD1 & PAD2)

 * Net at 256H alternating at 4V
 *
 *
 * http://www.youtube.com/watch?v=pDrRnJOCKZc
 */

#define MASTER_CLOCK    7159000
#define V_TOTAL         (0x105+1)       // 262
#define H_TOTAL         (0x1C6+1)       // 454

#define HBSTART                 (H_TOTAL)
#define HBEND                   (80)
#define VBSTART                 (V_TOTAL)
#define VBEND                   (16)

#define HRES_MULT                   (1)

fixedfreq_interface fixedfreq_mode_pong = {
	MASTER_CLOCK,
	H_TOTAL-67,H_TOTAL-40,H_TOTAL-8,H_TOTAL,
	V_TOTAL-22,V_TOTAL-19,V_TOTAL-16,V_TOTAL,
	1,  /* interlaced */
	0.32
};

fixedfreq_interface fixedfreq_mode_pongX2 = {
	MASTER_CLOCK * 2,
	(H_TOTAL-67) * 2, (H_TOTAL-40) * 2, (H_TOTAL-8) * 2, (H_TOTAL) * 2,
	V_TOTAL-22,V_TOTAL-19,V_TOTAL-16,V_TOTAL,
	1,  /* interlaced */
	0.32
};

enum input_changed_enum
{
	IC_PADDLE1,
	IC_PADDLE2,
	IC_COIN,
	IC_SWITCH,
	IC_VR1,
	IC_VR2
};

static NETLIST_START(pong_schematics)
	NETDEV_TTL_CONST(high, 1)
	NETDEV_TTL_CONST(low, 0)
#if 1
#if 0
	/* this is the clock circuit in schematics. */
	NETDEV_MAINCLOCK(xclk)
	//NETDEV_CLOCK(clk)
	NETDEV_PARAM(xclk.FREQ, 7159000.0*2)
	TTL_74107(ic_f6a, xclk, high, high, high)
	NET_ALIAS(clk, ic_f6a.Q)
#else
	/* abstracting this, performance increases by 40%
	 * No surprise, the clock is extremely expensive */
	NETDEV_MAINCLOCK(clk)
	//NETDEV_CLOCK(clk)
	NETDEV_PARAM(clk.FREQ, 7159000.0)
#endif
#else
	// benchmarking ...
	NETDEV_TTL_CONST(clk, 0)
	NETDEV_MAINCLOCK(xclk)
	NETDEV_PARAM(xclk.FREQ, 7159000.0*2)
#endif

	NETDEV_LOGIC_INPUT(SRST)
	NETDEV_ANALOG_INPUT(P2)

	TTL_7404_INVERT(hitQ, hit)
	TTL_7400_NAND(hit, hit1Q, hit2Q)

	TTL_7402_NOR(attractQ, StopG, runQ)
	TTL_7404_INVERT(attract, attractQ)

	TTL_7420_NAND(ic_h6a, hvidQ, hvidQ, hvidQ, hvidQ)
	NET_ALIAS(hvid, ic_h6a.Q)

	TTL_7400_NAND(ic_e6c, hvid, hblank)
	NET_ALIAS(MissQ, ic_e6c.Q)

	TTL_7404_INVERT(ic_d1e, MissQ)
	TTL_7400_NAND(ic_e1a, ic_d1e.Q, attractQ)
	NET_ALIAS(Missed, ic_e1a.Q)

	// runQ is basically the output of a RS flipflop
	// This is realized with discrete components in the real thing
	NETDEV_RSFF(runQ_ff, SRST, StopG)
	NET_ALIAS(runQ, runQ_ff.QQ)

	TTL_7400_NAND(rstspeed, SRSTQ, MissQ)
	TTL_7400_NAND(StopG, StopG1Q, StopG2Q)
	NET_ALIAS(L, ic_h3b.Q)
	NET_ALIAS(R, ic_h3b.QQ)

	TTL_7400_NAND(hit1Q, pad1, ic_g1b.Q)
	TTL_7400_NAND(hit2Q, pad2, ic_g1b.Q)

	TTL_7404_INVERT(SRSTQ, SRST)

	TTL_7400_NAND(ic_g3c, 128H, ic_h3a.QQ)
	TTL_7427_NOR(ic_g2c, ic_g3c.Q, 256H, vpad1Q)
	NET_ALIAS(pad1, ic_g2c.Q)
	TTL_7427_NOR(ic_g2a, ic_g3c.Q, 256HQ, vpad2Q)
	NET_ALIAS(pad2, ic_g2a.Q)

	// ----------------------------------------------------------------------------------------
	// horizontal counter
    // ----------------------------------------------------------------------------------------
	TTL_7493(ic_f8, clk, ic_f8.QA, ic_e7b.QQ, ic_e7b.QQ)    // f8, f9, f6b
	TTL_7493(ic_f9, ic_f8.QD, ic_f9.QA, ic_e7b.QQ, ic_e7b.QQ)   // f8, f9, f6b
	TTL_74107(ic_f6b, ic_f9.QD, high, high, ic_e7b.Q)
	TTL_7430_NAND(ic_f7, ic_f8.QB, ic_f8.QC, ic_f9.QC, ic_f9.QD, ic_f6b.Q, high, high, high)
	TTL_7474(ic_e7b, clk, ic_f7, high, high)

	NET_ALIAS(hreset, ic_e7b.QQ)
	NET_ALIAS(hresetQ, ic_e7b.Q)
	NET_ALIAS(  4H, ic_f8.QC)
	NET_ALIAS(  8H, ic_f8.QD)
	NET_ALIAS( 16H, ic_f9.QA)
	NET_ALIAS( 32H, ic_f9.QB)
	NET_ALIAS( 64H, ic_f9.QC)
	NET_ALIAS(128H, ic_f9.QD)
	NET_ALIAS(256H, ic_f6b.Q)
	NET_ALIAS(256HQ, ic_f6b.QQ)

    // ----------------------------------------------------------------------------------------
	// vertical counter
    // ----------------------------------------------------------------------------------------
	TTL_7493(ic_e8, hreset, ic_e8.QA, ic_e7a.QQ, ic_e7a.QQ) // e8, e9, d9b
	TTL_7493(ic_e9, ic_e8.QD,ic_e9.QA,  ic_e7a.QQ, ic_e7a.QQ)   // e8, e9, d9b
	TTL_74107(ic_d9b, ic_e9.QD, high, high, ic_e7a.Q)
	TTL_7474(ic_e7a, hreset, e7a_data, high, high)
	TTL_7410_NAND(e7a_data, ic_e8.QA, ic_e8.QC, ic_d9b.Q)

	NET_ALIAS(vreset, ic_e7a.QQ)
	NET_ALIAS(  4V, ic_e8.QC)
	NET_ALIAS(  8V, ic_e8.QD)
	NET_ALIAS( 16V, ic_e9.QA)
	NET_ALIAS( 32V, ic_e9.QB)
	NET_ALIAS( 64V, ic_e9.QC)
	NET_ALIAS(128V, ic_e9.QD)
	NET_ALIAS(256V,  ic_d9b.Q)
	NET_ALIAS(256VQ, ic_d9b.QQ)


    // ----------------------------------------------------------------------------------------
	// hblank flip flop
    // ----------------------------------------------------------------------------------------

	TTL_7400_NAND(ic_g5b, 16H, 64H)

	// the time critical one
	TTL_7400_NAND(ic_h5c, ic_h5b.Q, hresetQ)
	TTL_7400_NAND(ic_h5b, ic_h5c.Q, ic_g5b.Q)

	NET_ALIAS(hblank,  ic_h5c.Q)
	NET_ALIAS(hblankQ,  ic_h5b.Q)
	TTL_7400_NAND(hsyncQ, hblank, 32H)

    // ----------------------------------------------------------------------------------------
	// vblank flip flop
    // ----------------------------------------------------------------------------------------
	TTL_7402_NOR(ic_f5c, ic_f5d.Q, vreset)
	TTL_7402_NOR(ic_f5d, ic_f5c.Q, 16V)

	NET_ALIAS(vblank,  ic_f5d.Q)
	NET_ALIAS(vblankQ, ic_f5c.Q)

	TTL_7400_NAND(ic_h5a, 8V, 8V)
	TTL_7410_NAND(ic_g5a, vblank, 4V, ic_h5a.Q)
	NET_ALIAS(vsyncQ, ic_g5a.Q)

    // ----------------------------------------------------------------------------------------
	// move logic
    // ----------------------------------------------------------------------------------------

	TTL_7400_NAND(ic_e1d, hit_sound, ic_e1c.Q)
	TTL_7400_NAND(ic_e1c, ic_f1.QC, ic_f1.QD)
	TTL_7493(ic_f1, ic_e1d.Q, ic_f1.QA, rstspeed, rstspeed)

	TTL_7402_NOR(ic_g1d, ic_f1.QC, ic_f1.QD)
	TTL_7400_NAND(ic_h1a, ic_g1d.Q, ic_g1d.Q)
	TTL_7400_NAND(ic_h1d, ic_e1c.Q, ic_h1a.Q)

	TTL_7400_NAND(ic_h1c, ic_h1d.Q, vreset)
	TTL_7400_NAND(ic_h1b, ic_h1a.Q, vreset)
	TTL_7402_NOR(ic_g1c, 256HQ, vreset)

	TTL_74107(ic_h2a, ic_g1c.Q, ic_h2b.Q, low, ic_h1b.Q)
	TTL_74107(ic_h2b, ic_g1c.Q, high, move, ic_h1c.Q)

	TTL_7400_NAND(ic_h4a, ic_h2b.Q, ic_h2a.Q)
	NET_ALIAS(move, ic_h4a.Q)

	TTL_7400_NAND(ic_c1d, SC, attract)
	TTL_7404_INVERT(ic_d1a, ic_c1d.Q)
	TTL_7474(ic_h3b, ic_d1a.Q, ic_h3b.QQ, hit1Q, hit2Q)

	TTL_7400_NAND(ic_h4d, ic_h3b.Q, move)
	TTL_7400_NAND(ic_h4b, ic_h3b.QQ, move)
	TTL_7400_NAND(ic_h4c, ic_h4d.Q, ic_h4b.Q)
	NET_ALIAS(Aa, ic_h4c.Q)
	NET_ALIAS(Ba, ic_h4b.Q)

    // ----------------------------------------------------------------------------------------
	// hvid circuit
    // ----------------------------------------------------------------------------------------

	TTL_7400_NAND(hball_resetQ, Serve, attractQ)

	TTL_9316(ic_g7, clk, high, hblankQ, hball_resetQ, ic_g5c.Q, Aa, Ba, low, high)
	TTL_9316(ic_h7, clk, ic_g7.RC, high, hball_resetQ, ic_g5c.Q, low, low, low, high)
	TTL_74107(ic_g6b, ic_h7.RC, high, high, hball_resetQ)
	TTL_7410_NAND(ic_g5c, ic_g6b.Q, ic_h7.RC, ic_g7.RC)
	TTL_7420_NAND(ic_h6b, ic_g6b.Q, ic_h7.RC, ic_g7.QC, ic_g7.QD)
	NET_ALIAS(hvidQ, ic_h6b.Q)

    // ----------------------------------------------------------------------------------------
	// vvid circuit
    // ----------------------------------------------------------------------------------------

	TTL_9316(ic_b3, hsyncQ, high, vblankQ, high, ic_b2b.Q, a6, b6, c6, d6)
	TTL_9316(ic_a3, hsyncQ, ic_b3.RC, high, high, ic_b2b.Q, low, low, low, low)
	TTL_7400_NAND(ic_b2b, ic_a3.RC, ic_b3.RC)
	TTL_7410_NAND(ic_e2b, ic_a3.RC, ic_b3.QC, ic_b3.QD)
	NET_ALIAS(vvidQ, ic_e2b.Q)
	TTL_7404_INVERT(vvid, vvidQ)    // D2D
	NET_ALIAS(vpos256, ic_a3.RC)
	NET_ALIAS(vpos32, ic_a3.QB)
	NET_ALIAS(vpos16, ic_a3.QA)

    // ----------------------------------------------------------------------------------------
	// vball ctrl circuit
    // ----------------------------------------------------------------------------------------

	TTL_7450_ANDORINVERT(ic_a6a, b1, 256HQ, b2, 256H)
	TTL_7450_ANDORINVERT(ic_a6b, c1, 256HQ, c2, 256H)
	TTL_7450_ANDORINVERT(ic_b6b, d1, 256HQ, d2, 256H)

	TTL_7474(ic_a5b, hit, ic_a6a, attractQ, high)
	TTL_7474(ic_a5a, hit, ic_a6b, attractQ, high)
	TTL_7474(ic_b5a, hit, ic_b6b, attractQ, high)
	TTL_74107(ic_h2x, vblank, vvid, vvid, hitQ) // two marked at position h2a ==> this h2x

	TTL_7486_XOR(ic_a4c, ic_a5b.Q, ic_h2x.Q)
	TTL_7486_XOR(ic_a4b, ic_a5a.Q, ic_h2x.Q)

	TTL_7450_ANDORINVERT(ic_b6a, ic_b5a.Q, ic_h2x.Q, ic_b5a.QQ, ic_h2x.QQ)

	TTL_7404_INVERT(ic_c4a, ic_b6a)

	TTL_7483(ic_b4, ic_a4c, ic_a4b, ic_b6a, low, ic_c4a, high, high, low, low)
	NET_ALIAS(a6, ic_b4.SA)
	NET_ALIAS(b6, ic_b4.SB)
	NET_ALIAS(c6, ic_b4.SC)
	NET_ALIAS(d6, ic_b4.SD)

    // ----------------------------------------------------------------------------------------
	// serve monoflop
    // ----------------------------------------------------------------------------------------

	TTL_7404_INVERT(f4_trig, rstspeed)

	NETDEV_R(ic_f4_serve_R, RES_K(330))
    NETDEV_C(ic_f4_serve_C, CAP_U(4.7))
    NETDEV_NE555(ic_f4_serve)

    NET_C(ic_f4_serve.VCC, V5)
    NET_C(ic_f4_serve.GND, GND)
    NET_C(ic_f4_serve.RESET, V5)
    NET_C(ic_f4_serve_R.1, V5)
    NET_C(ic_f4_serve_R.2, ic_f4_serve.THRESH)
    NET_C(ic_f4_serve_R.2, ic_f4_serve.DISCH)
    NET_C(f4_trig, ic_f4_serve.TRIG)
    NET_C(ic_f4_serve_R.2, ic_f4_serve_C.1)
    NET_C(GND, ic_f4_serve_C.2)

    TTL_7427_NOR(ic_e5a, ic_f4_serve.OUT, StopG, runQ)
	TTL_7474(ic_b5b_serve, pad1, ic_e5a, ic_e5a, high)

	NET_ALIAS(Serve, ic_b5b_serve.QQ)
	NET_ALIAS(ServeQ, ic_b5b_serve.Q)

    // ----------------------------------------------------------------------------------------
	// score logic
    // ----------------------------------------------------------------------------------------

	TTL_7474(ic_h3a, 4H, 128H, high, attractQ)

    // ----------------------------------------------------------------------------------------
	// sound logic
    // ----------------------------------------------------------------------------------------
	TTL_7474(ic_c2a, vpos256, high, hitQ, high)
	TTL_74107(ic_f3_topbot, vblank, vvid, vvidQ, ServeQ)

    // ----------------------------------------------------------------------------------------
	// monoflop with NE555 determines duration of score sound
    // ----------------------------------------------------------------------------------------

	NETDEV_R(ic_g4_R, RES_K(220))
    NETDEV_C(ic_g4_C, CAP_U(1))
    NETDEV_NE555(ic_g4_sc)
    NET_ALIAS(SC, ic_g4_sc.OUT)

    NET_C(ic_g4_sc.VCC, V5)
    NET_C(ic_g4_sc.GND, GND)
    NET_C(ic_g4_sc.RESET, V5)
    NET_C(ic_g4_R.1, V5)
    NET_C(ic_g4_R.2, ic_g4_sc.THRESH)
    NET_C(ic_g4_R.2, ic_g4_sc.DISCH)
    NET_C(MissQ, ic_g4_sc.TRIG)
    NET_C(ic_g4_R.2, ic_g4_C.1)
    NET_C(GND, ic_g4_C.2)

    NET_ALIAS(hit_sound_en, ic_c2a.QQ)
	TTL_7400_NAND(hit_sound, hit_sound_en, vpos16)
	TTL_7400_NAND(score_sound, SC, vpos32)
	TTL_7400_NAND(topbothitsound, ic_f3_topbot.Q, vpos32)

	TTL_7410_NAND(ic_c4b, topbothitsound, hit_sound, score_sound)
	TTL_7400_NAND(ic_c1b, ic_c4b.Q, attractQ)
	NET_ALIAS(sound, ic_c1b.Q)


    // ----------------------------------------------------------------------------------------
	// paddle1 logic 1
    // ----------------------------------------------------------------------------------------

    NETDEV_POT(ic_b9_POT, RES_K(1))     // This is a guess!!
    NETDEV_R(ic_b9_RPRE, 470)

    NET_C(ic_b9_POT.1, V5)
    NET_C(ic_b9_POT.3, GND)
    NET_C(ic_b9_POT.2, ic_b9_RPRE.1)
    NET_C(ic_b9_RPRE.2, ic_b9.CONT)

    NETDEV_R(ic_b9_R, RES_K(71))
    NETDEV_C(ic_b9_C, CAP_U(.1))
    NETDEV_D(ic_b9_D, 1N914)
    NETDEV_NE555(ic_b9)

    NET_C(ic_b9.VCC, V5)
    NET_C(ic_b9.GND, GND)
    NET_C(ic_b9.RESET, V5)
    NET_C(ic_b9_R.1, V5)
    NET_C(ic_b9_R.2, ic_b9.THRESH)
    NET_C(ic_b9_R.2, ic_b9_D.A)
    NET_C(ic_b9_D.K, ic_b9.DISCH)
    NET_C(256VQ, ic_b9.TRIG)
    NET_C(ic_b9_R.2, ic_b9_C.1)
    NET_C(GND, ic_b9_C.2)

	TTL_7404_INVERT(ic_c9b, ic_b9.OUT)
	TTL_7400_NAND(ic_b7b, ic_a7b.Q, hsyncQ)
	TTL_7493(ic_b8, ic_b7b.Q, ic_b8.QA, ic_b9.OUT, ic_b9.OUT)
	TTL_7400_NAND(ic_b7a, ic_c9b.Q, ic_a7b.Q)
	TTL_7420_NAND(ic_a7b, ic_b8.QA, ic_b8.QB, ic_b8.QC, ic_b8.QD)
	NET_ALIAS(vpad1Q, ic_b7a.Q)

	NET_ALIAS(b1, ic_b8.QB)
	NET_ALIAS(c1, ic_b8.QC)
	NET_ALIAS(d1, ic_b8.QD)

    // ----------------------------------------------------------------------------------------
	// paddle1 logic 2
    // ----------------------------------------------------------------------------------------

	NETDEV_POT(ic_a9_POT, RES_K(1))     // This is a guess!!
    NETDEV_R(ic_a9_RPRE, 470)

    NET_C(ic_a9_POT.1, V5)
    NET_C(ic_a9_POT.3, GND)
    NET_C(ic_a9_POT.2, ic_a9_RPRE.1)
    NET_C(ic_a9_RPRE.2, ic_a9.CONT)

    NETDEV_R(ic_a9_R, RES_K(71))
    NETDEV_C(ic_a9_C, CAP_U(.1))
    NETDEV_D(ic_a9_D, 1N914)
    NETDEV_NE555(ic_a9)

    NET_C(ic_a9.VCC, V5)
    NET_C(ic_a9.GND, GND)
    NET_C(ic_a9.RESET, V5)
    NET_C(ic_a9_R.1, V5)
    NET_C(ic_a9_R.2, ic_a9.THRESH)
    NET_C(ic_a9_R.2, ic_a9_D.A)
    NET_C(ic_a9_D.K, ic_a9.DISCH)
    NET_C(256VQ, ic_a9.TRIG)
    NET_C(ic_a9_R.2, ic_a9_C.1)
    NET_C(GND, ic_a9_C.2)

	TTL_7404_INVERT(ic_c9a, ic_a9.OUT)
	TTL_7400_NAND(ic_b7c, ic_a7a.Q, hsyncQ)
	TTL_7493(ic_a8, ic_b7c.Q, ic_a8.QA, ic_a9.OUT, ic_a9.OUT)
	TTL_7400_NAND(ic_b7d, ic_c9a.Q, ic_a7a.Q)
	TTL_7420_NAND(ic_a7a, ic_a8.QA, ic_a8.QB, ic_a8.QC, ic_a8.QD)
	NET_ALIAS(vpad2Q, ic_b7d.Q)

	NET_ALIAS(b2, ic_a8.QB)
	NET_ALIAS(c2, ic_a8.QC)
	NET_ALIAS(d2, ic_a8.QD)

    // ----------------------------------------------------------------------------------------
	// C5-EN Logic
    // ----------------------------------------------------------------------------------------

	TTL_7404_INVERT(ic_e3a, 128H)
	TTL_7427_NOR( ic_e3b, 256H, 64H, ic_e3a.Q)
	TTL_7410_NAND(ic_e2c, 256H, 64H, ic_e3a.Q)
	TTL_7404_INVERT(ic_e3c, ic_e2c.Q)
	TTL_7402_NOR(ic_d2c, ic_e3c.Q, ic_e3b.Q)
	TTL_7404_INVERT(ic_g1a, 32V)
	TTL_7425_NOR(ic_f2a, ic_g1a.Q, 64V, 128V, ic_d2c.Q)
	NET_ALIAS(c5-en, ic_f2a.Q)

    // ----------------------------------------------------------------------------------------
	// Score logic ...
    // ----------------------------------------------------------------------------------------

	TTL_7402_NOR(ic_f5b, L, Missed)
	TTL_7490(ic_c7, ic_f5b, SRST, SRST, low, low)
	TTL_74107(ic_c8a, ic_c7.QD, high, high, SRSTQ)
	NETDEV_SWITCH2(sw1a, high, ic_c7.QC)
	NETDEV_PARAM(sw1a.POS, 0)
	TTL_7410_NAND(ic_d8a, ic_c7.QA, sw1a.Q, ic_c8a.Q)       // would be nand2 for 11 instead of 15 points, need a switch dev!

	NET_ALIAS(StopG1Q, ic_d8a.Q)
	NET_ALIAS(score1_1, ic_c7.QA)
	NET_ALIAS(score1_2, ic_c7.QB)
	NET_ALIAS(score1_4, ic_c7.QC)
	NET_ALIAS(score1_8, ic_c7.QD)
	NET_ALIAS(score1_10, ic_c8a.Q)
	NET_ALIAS(score1_10Q, ic_c8a.QQ)

	TTL_7402_NOR(ic_f5a, R, Missed)
	TTL_7490(ic_d7, ic_f5a, SRST, SRST, low, low)
	TTL_74107(ic_c8b, ic_d7.QD, high, high, SRSTQ)
	NETDEV_SWITCH2(sw1b, high, ic_d7.QC)
	NETDEV_PARAM(sw1b.POS, 0)
	TTL_7410_NAND(ic_d8b, ic_d7.QA, sw1b.Q, ic_c8b.Q)       // would be nand2 for 11 instead of 15 points, need a switch dev!

	NET_ALIAS(StopG2Q, ic_d8b.Q)
	NET_ALIAS(score2_1, ic_d7.QA)
	NET_ALIAS(score2_2, ic_d7.QB)
	NET_ALIAS(score2_4, ic_d7.QC)
	NET_ALIAS(score2_8, ic_d7.QD)
	NET_ALIAS(score2_10, ic_c8b.Q)
	NET_ALIAS(score2_10Q, ic_c8b.QQ)

    // ----------------------------------------------------------------------------------------
	// Score display
    // ----------------------------------------------------------------------------------------

	TTL_74153(ic_d6a, score1_10Q, score1_4, score2_10Q, score2_4, 32H, 64H, low)
	TTL_74153(ic_d6b, score1_10Q, score1_8, score2_10Q, score2_8, 32H, 64H, low)

	TTL_74153(ic_c6a, high, score1_1, high, score2_1, 32H, 64H, low)
	TTL_74153(ic_c6b, score1_10Q, score1_2, score2_10Q, score2_2, 32H, 64H, low)

	TTL_7448(ic_c5, ic_c6a.AY, ic_c6b.AY, ic_d6a.AY, ic_d6b.AY, high, c5-en, high)

	TTL_7404_INVERT(ic_e4b, 16H)
	TTL_7427_NOR(ic_e5c, ic_e4b.Q, 8H, 4H)
	NET_ALIAS(scoreFE, ic_e5c.Q)

	TTL_7400_NAND(ic_c3d, 8H, 4H)
	//TTL_7400_NAND(ic_c3d, 4H, 8H)
	TTL_7402_NOR(ic_d2b, ic_e4b.Q, ic_c3d.Q)
	NET_ALIAS(scoreBC, ic_d2b.Q)

	TTL_7427_NOR(ic_e5b, ic_e4b.Q, 8V, 4V)
	NET_ALIAS(scoreA, ic_e5b.Q)

	TTL_7410_NAND(ic_e2a, 16H, 8V, 4V)
	TTL_7404_INVERT(ic_e4a, ic_e2a.Q)
	NET_ALIAS(scoreGD, ic_e4a.Q)

	TTL_7404_INVERT(ic_e4c, 16V)

	TTL_7410_NAND(ic_d4a, ic_e4c.Q, ic_c5.f, scoreFE)
	TTL_7410_NAND(ic_d5c,      16V, ic_c5.e, scoreFE)
	TTL_7410_NAND(ic_c4c, ic_e4c.Q, ic_c5.b, scoreBC)
	TTL_7410_NAND(ic_d5a,      16V, ic_c5.c, scoreBC)
	TTL_7410_NAND(ic_d4c, ic_e4c.Q, ic_c5.a, scoreA)
	TTL_7410_NAND(ic_d4b, ic_e4c.Q, ic_c5.g, scoreGD)
	TTL_7410_NAND(ic_d5b,      16V, ic_c5.d, scoreGD)

	TTL_7430_NAND(ic_d3, ic_d4a, ic_d5c, ic_c4c, ic_d5a, ic_d4c, ic_d4b, ic_d5b, high)
	NET_ALIAS(score, ic_d3.Q)       //FIXME

	// net
	TTL_74107(ic_f3b, clk, 256H, 256HQ, high)
	TTL_7400_NAND(ic_g3b, ic_f3b.QQ, 256H)
	TTL_7427_NOR(ic_g2b, ic_g3b.Q, vblank, 4V)
	NET_ALIAS(net, ic_g2b.Q)

    // ----------------------------------------------------------------------------------------
	// video
    // ----------------------------------------------------------------------------------------

	TTL_7402_NOR(ic_g1b, hvidQ, vvidQ)
	TTL_7425_NOR(ic_f2b, ic_g1b.Q, pad1, pad2, net)
	TTL_7404_INVERT(ic_e4e, ic_f2b.Q)
	NET_ALIAS(video, ic_e4e.Q)

	TTL_7486_XOR(ic_a4d, hsyncQ, vsyncQ)
	TTL_7404_INVERT(ic_e4f, ic_a4d.Q)

	NETDEV_R(RV1, RES_K(1))
    NETDEV_R(RV2, RES_K(1.2))
    NETDEV_R(RV3, RES_K(22))
    NET_C(video, RV1.1)
    NET_C(score, RV2.1)
    NET_C(ic_e4f.Q, RV3.1)
    NET_C(RV1.2, RV2.2)
    NET_C(RV2.2, RV3.2)

    NET_ALIAS(videomix, RV3.2)

    NETDEV_SOLVER(Solver)
    NETDEV_PARAM(Solver.FREQ, 48000)
    NETDEV_ANALOG_CONST(V5, 5)
    NETDEV_ANALOG_CONST(V1, 1)
    NETDEV_ANALOG_CONST(GND, 0)

#if 0
    NETDEV_R(R1, 10)
    NETDEV_R(R2, 10)
    NETDEV_R(R3, 10)
    NET_C(V5,R1.1)
    NET_C(R1.2, R2.1)
    NET_C(R2.2, R3.1)
    NET_C(R3.2, GND)
#endif
#if 0
    NETDEV_R(R4, 1000)
    NETDEV_C(C1, 1e-6)
    NET_C(V5,R4.1)
    NET_C(R4.2, C1.1)
    NET_C(C1.2, GND)
    //NETDEV_LOG(log1, C1.1)
#endif

#define tt(_n) \
    NETDEV_R(R ## _n, 1000) \
	NETDEV_D(D ## _n) \
    NET_C(V5, R ## _n.1) \
    NET_C(R ## _n.2, D ## _n.A) \
    NET_C(D ## _n.K, GND)

/*    tt(20)
    tt(21)
    tt(22)
    tt(23)
    tt(24)
    tt(25)
    tt(26)
    tt(27)
    tt(28)
    tt(29)
*/

#if 0
    NETDEV_R(R5, 1000)
    NETDEV_1N914(D1)
    NET_C(V5, R5.1)
    NET_C(R5.2, D1.A)
    NET_C(D1.K, GND)
    //NETDEV_LOG(log1, D1.A)
#endif

#if 0
    // astable NAND Multivibrator
    NETDEV_R(R1, 1000)
    NETDEV_C(C1, 1e-6)
    TTL_7400_NAND(n1,R1.1,R1.1)
    TTL_7400_NAND(n2,R1.2,R1.2)
    NET_C(n1.Q, R1.2)
    NET_C(n2.Q, C1.1)
    NET_C(C1.2, R1.1)
    //NETDEV_LOG(log2, C1.2)
    //NETDEV_LOG(log2, n1.Q)
    //NETDEV_LOG(log3, n2.Q)
#endif

#if 0
    // astable NE555, 1.13 ms period
    NETDEV_R(RA, 5000)
    NETDEV_R(RB, 3000)
    NETDEV_C(C, 0.15e-6)
    NETDEV_NE555(555)

    NET_C(GND, 555.GND)
    NET_C(V5, 555.VCC)

    NET_C(RA.1, 555.VCC)
    NET_C(RA.2, 555.DISCH)

    NET_C(RB.1, 555.DISCH)
    NET_C(RB.2, 555.TRIG)

    NET_C(RB.2, 555.THRESH)

    NET_C(555.TRIG, C.1)
    NET_C(C.2, GND)
    //NETDEV_LOG(log2, C.1)
    //NETDEV_LOG(log3, 555.OUT)
#endif

#if 0
    NETDEV_BC238B(Q)
    NETDEV_R(RB, 1000)
    NETDEV_R(RC, 1000)

    NET_C(RC.1, V5)
    NET_C(RC.2, Q.C)
    NET_C(RB.1, 128H)
    NET_C(RB.2, Q.B)
    NET_C(Q.E, GND)
    //NETDEV_LOG(logB, Q.B)
    //NETDEV_LOG(logC, Q.C)
#endif

#if 0
    NETDEV_VCVS(VV)
    NETDEV_R(R1, 1000)
    NETDEV_R(R2, 10000)

    NET_C(V5, R1.1)
    NET_C(R1.2, VV.IN)
    NET_C(R2.1, VV.OP)
    NET_C(R2.2, VV.IN)
    NET_C(VV.ON, GND)
    NET_C(VV.IP, GND)
    NETDEV_LOG(logX, VV.OP)

#endif

#if 0
    NETDEV_VCCS(VV)
    NETDEV_PARAM(VV.G, 100000)  // typical OP-AMP amplification
    NETDEV_R(R1, 1000)
    NETDEV_R(R2, 1)
    NETDEV_R(R3, 10000)

    NET_C(4V, R1.1)
    NET_C(R1.2, VV.IN)
    NET_C(R2.1, VV.OP)
    NET_C(R3.1, VV.IN)
    NET_C(R3.2, VV.OP)
    NET_C(R2.2, GND)
    NET_C(VV.ON, GND)
    NET_C(VV.IP, GND)
    //NETDEV_LOG(logX, VV.OP)
    //NETDEV_LOG(logY, 4V)

#endif

#if 0
    NETDEV_VCVS(VV)
    NETDEV_PARAM(VV.G, 100000)  // typical OP-AMP amplification
    NETDEV_PARAM(VV.RO, 50)  // typical OP-AMP amplification
    NETDEV_R(R1, 1000)
    NETDEV_R(R3, 10000) // ==> 10x amplification (inverting)

    NET_C(4V, R1.1)
    NET_C(R1.2, VV.IN)
    NET_C(R3.1, VV.IN)
    NET_C(R3.2, VV.OP)
    NET_C(VV.ON, GND)
    NET_C(VV.IP, GND)
    NETDEV_LOG(logX, VV.OP)
    NETDEV_LOG(logY, 4V)

#endif

#if 0
    // Impedance converter with resistor
    NETDEV_VCVS(VV)
    NETDEV_PARAM(VV.G, 100000)  // typical OP-AMP amplification
    NETDEV_PARAM(VV.RO, 50)  // typical OP-AMP amplification
    NETDEV_R(R3, 10000)

    NET_C(4V, VV.IP)
    NET_C(R3.1, VV.IN)
    NET_C(R3.2, VV.OP)
    NET_C(VV.ON, GND)
    NETDEV_LOG(logX, VV.OP)
    NETDEV_LOG(logY, 4V)

#endif

#if 0
    // Impedance converter without resistor
    NETDEV_VCVS(VV)
    NETDEV_PARAM(VV.G, 100000)  // typical OP-AMP amplification
    NETDEV_PARAM(VV.RO, 50)  // typical OP-AMP amplification

    NET_C(4V, VV.IP)
    NET_C(VV.IN, VV.OP)
    NET_C(VV.ON, GND)
    NETDEV_LOG(logX, VV.OP)
    NETDEV_LOG(logY, 4V)

#endif

#if 0
    /* Impedance converter current source opamp model from
     *
     * http://www.ecircuitcenter.com/Circuits/opmodel1/opmodel1.htm
     *
     * Bandwidth 10Mhz
     *
     */
    NETDEV_VCCS(G1)
    NETDEV_PARAM(G1.G, 100)  // typical OP-AMP amplification 100 * 1000 = 100000
    NETDEV_R(RP1, 1000)
    NETDEV_C(CP1, 1.59e-6)   // <== change to 1.59e-3 for 10Khz bandwidth
    NETDEV_VCVS(EBUF)
    NETDEV_PARAM(EBUF.RO, 50)
    NETDEV_PARAM(EBUF.G, 1)

    NET_C(G1.IP, 4V)
    NET_C(G1.IN, EBUF.OP)
    NET_C(EBUF.ON, GND)

    NET_C(G1.ON, GND)
    NET_C(RP1.2, GND)
    NET_C(CP1.2, GND)
    NET_C(EBUF.IN, GND)

    NET_C(RP1.1, G1.OP)
    NET_C(CP1.1, RP1.1)
    NET_C(EBUF.IP, RP1.1)

    //NETDEV_LOG(logX, EBUF.OP)
    //NETDEV_LOG(logY, 4V)

#endif

NETLIST_END

class pong_state : public driver_device
{
public:
	pong_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, "maincpu"),
			m_video(*this, "fixfreq"),

			m_dac(*this, "dac"),                /* just to have a sound device */
			m_srst(*this, "maincpu", "SRST"),
			m_p_P0(*this, "maincpu", "ic_b9_POT.DIAL"),
            m_p_P1(*this, "maincpu", "ic_a9_POT.DIAL"),
			m_p_V0(*this, "maincpu", "P1"),  // pong - legacy
			m_p_V1(*this, "maincpu", "P2"),  // pong - legacy
			m_sw1a(*this, "maincpu", "sw1a.POS"),
			m_sw1b(*this, "maincpu", "sw1b.POS"),
			m_p_R0(*this, "maincpu", "ic_a9_R.R"),
			m_p_R1(*this, "maincpu", "ic_b9_R.R"),
			m_p_R0x(*this, "maincpu", "ic_a9.R"),  // pong - legacy
			m_p_R1x(*this, "maincpu", "ic_b9.R"),  // pong - legacy
			isRomPong(false)
	{
	}

	// devices
	required_device<netlist_mame_device> m_maincpu;
	required_device<fixedfreq_device> m_video;
	required_device<dac_device> m_dac; /* just to have a sound device */

	// sub devices
	netlist_mame_device::required_output<netlist_logic_output_t> m_srst;
    netlist_mame_device::optional_param<netlist_param_double_t> m_p_P0;
    netlist_mame_device::optional_param<netlist_param_double_t> m_p_P1;
	netlist_mame_device::optional_output<netlist_analog_output_t> m_p_V0;
	netlist_mame_device::optional_output<netlist_analog_output_t> m_p_V1;
	netlist_mame_device::required_param<netlist_param_int_t> m_sw1a;
	netlist_mame_device::required_param<netlist_param_int_t> m_sw1b;
	netlist_mame_device::optional_param<netlist_param_double_t> m_p_R0;
	netlist_mame_device::optional_param<netlist_param_double_t> m_p_R1;
    netlist_mame_device::optional_param<netlist_param_double_t> m_p_R0x;
    netlist_mame_device::optional_param<netlist_param_double_t> m_p_R1x;

	bool isRomPong;

	UINT32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	DECLARE_INPUT_CHANGED_MEMBER(input_changed);

	NETDEV_ANALOG_CALLBACK_MEMBER(sound_cb)
	{
		//printf("snd %f\n", newval);
		//dac_w(m_dac, 0, newval*64);
		m_dac->write_unsigned8(64*data);
	}

	NETDEV_ANALOG_CALLBACK_MEMBER(video_cb)
	{
		m_video->update_vid(data, time);
		//printf("%20.15f\n", newval);
	}

protected:

	// driver_device overrides
	virtual void machine_start();
	virtual void machine_reset();

	virtual void video_start();

private:

};

static NETLIST_START(pong)

    //NETLIST_INCLUDE(pong_schematics)
    NETLIST_MEMREGION("maincpu")

    NETDEV_ANALOG_CALLBACK(sound_cb, sound, pong_state, sound_cb, "")
    NETDEV_ANALOG_CALLBACK(video_cb, videomix, pong_state, video_cb, "")
NETLIST_END

static NETLIST_START(pong_fast)

    NETLIST_INCLUDE(pong_schematics)

    NETDEV_ANALOG_CALLBACK(sound_cb, sound, pong_state, sound_cb, "")
    NETDEV_ANALOG_CALLBACK(video_cb, videomix, pong_state, video_cb, "")

NETLIST_END

void pong_state::machine_start()
{
    if (strcmp(this->shortname(),"pong")==0)
    {
        isRomPong = true;
    }
}

void pong_state::machine_reset()
{
}


void pong_state::video_start()
{
}


INPUT_CHANGED_MEMBER(pong_state::input_changed)
{
	static const double NE555_R = RES_K(5);
	static const double PRE_R = RES_R(470);
	static const double POT_R = RES_K(1);

	double pad;
	int numpad = (FPTR) (param);

	switch (numpad)
	{
	case IC_PADDLE1:
	case IC_PADDLE2:
	{
		// http://ion.chem.usu.edu/~sbialkow/Classes/564/Thevenin/Thevenin.html

		double fac = (double) newval / (double) 256;
		fac = (exp(fac) - 1.0) / (exp(1.0) -1.0) ;
		double R1 = fac * POT_R;
		double R3 = (1.0 - fac) * POT_R;
		double vA = 5.0 * R3 / (R3 + R1);
		double vB = 5.0 * 2 * NE555_R / (2 * NE555_R + NE555_R);
		double Req = RES_2_PARALLEL(R1, R3) + RES_2_PARALLEL(NE555_R, 2.0 * NE555_R);
		double pad = vA + (vB - vA)*PRE_R / (Req + PRE_R);
		if (!isRomPong)
		{
	        switch (numpad)
	        {
	        case IC_PADDLE1:    m_p_P0->setTo(fac); break;
	        case IC_PADDLE2:    m_p_P1->setTo(fac); break;
	        }
		}
		else
		{
	        switch (numpad)
	        {
	        case IC_PADDLE1:    m_p_V0->set_Q(pad, NLTIME_FROM_NS(0)); break;
	        case IC_PADDLE2:    m_p_V1->set_Q(pad, NLTIME_FROM_NS(0)); break;
	        }
		}
		//printf("%d %f\n", newval, (float) pad);
		break;
	}
	case IC_SWITCH:
		m_sw1a->setTo(newval ? 1 : 0);
		m_sw1b->setTo(newval ? 1 : 0);
		break;
	case IC_COIN:
		m_srst->set_Q(newval & 1, NLTIME_FROM_US(500));
		break;
	case IC_VR1:
	case IC_VR2:
		pad = (double) newval / (double) 100 * RES_K(50) + RES_K(56);
        if (!isRomPong)
        {
            switch (numpad)
            {
            case IC_VR1:    m_p_R0->setTo(pad); break;
            case IC_VR2:    m_p_R1->setTo(pad); break;
            }
        }
        else
        {
            switch (numpad)
            {
            case IC_VR1:    m_p_R0x->setTo(pad); break;
            case IC_VR2:    m_p_R1x->setTo(pad); break;
            }
        }
		break;
	}


}

static INPUT_PORTS_START( pong )
	PORT_START( "PADDLE0" ) /* fake input port for player 1 paddle */
	PORT_BIT( 0xff, 0x00, IPT_PADDLE ) PORT_SENSITIVITY(2) PORT_KEYDELTA(100) PORT_CENTERDELTA(0)   PORT_CHANGED_MEMBER(DEVICE_SELF, pong_state, input_changed,IC_PADDLE1)

	PORT_START( "PADDLE1" ) /* fake input port for player 2 paddle */
	PORT_BIT( 0xff, 0x00, IPT_PADDLE ) PORT_SENSITIVITY(2) PORT_KEYDELTA(100) PORT_CENTERDELTA(0) PORT_PLAYER(2) PORT_CHANGED_MEMBER(DEVICE_SELF, pong_state, input_changed, IC_PADDLE2)

	PORT_START("IN0") /* fake as well */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )     PORT_CHANGED_MEMBER(DEVICE_SELF, pong_state, input_changed, IC_COIN)
	PORT_DIPNAME( 0x06, 0x00, "Game Won" )          PORT_DIPLOCATION("SW1A:1,SW1B:1") PORT_CHANGED_MEMBER(DEVICE_SELF, pong_state, input_changed, IC_SWITCH)
	PORT_DIPSETTING(    0x00, "11" )
	PORT_DIPSETTING(    0x06, "15" )

	PORT_START("VR1")
	PORT_ADJUSTER( 63, "VR1 - 50k, Paddle 1 adjustment" )   PORT_CHANGED_MEMBER(DEVICE_SELF, pong_state, input_changed, IC_VR1)
	PORT_START("VR2")
	PORT_ADJUSTER( 63, "VR2 - 50k, Paddle 2 adjustment" )   PORT_CHANGED_MEMBER(DEVICE_SELF, pong_state, input_changed, IC_VR2)
	//PORT_START("GATESPEED")
	//PORT_ADJUSTER( 100, "Logic Gate Delay" ) PORT_MINMAX(10, 200) PORT_CHANGED_MEMBER(DEVICE_SELF, pong_state, input_changed, IC_GATEDELAY)

INPUT_PORTS_END

static MACHINE_CONFIG_START( pong, pong_state )

	/* basic machine hardware */
	MCFG_NETLIST_ADD("maincpu", pong)

	/* video hardware */

	//MCFG_FIXFREQ_ADD("fixfreq", "screen", fixedfreq_mode_ntsc720)
	//MCFG_FIXFREQ_ADD("fixfreq", "screen", fixedfreq_mode_pongX2)
	MCFG_FIXFREQ_ADD("fixfreq", "screen", fixedfreq_mode_pong)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("dac", DAC, 48000)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( pongf, pong )

	/* basic machine hardware */
	MCFG_NETLIST_REPLACE("maincpu", pong_fast)

MACHINE_CONFIG_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( pong ) /* dummy to satisfy game entry*/
	ROM_REGION( 0x10000, "maincpu", 0 ) /* enough for netlist */
	ROM_LOAD( "pong.netlist", 0x000000, 0x0029e4, CRC(e9c409a1) SHA1(1dc99437f49261c3cb3f46153c6258043bc720a0) )
ROM_END

ROM_START( pongf ) /* dummy to satisfy game entry*/
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASE00 )
ROM_END


GAME( 1972, pong,  0, pong, pong, driver_device,  0, ROT0, "Atari", "Pong (Rev E)", 0 )
GAME( 1972, pongf,  0, pongf, pong, driver_device,  0, ROT0, "Atari", "Pong (Rev E), no subcycles", 0 )
