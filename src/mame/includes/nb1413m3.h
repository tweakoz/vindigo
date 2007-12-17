/******************************************************************************

    Machine Hardware for Nichibutsu Mahjong series.

    Driver by Takahiro Nogi <nogi@kt.rim.or.jp> 1999/11/05 -

******************************************************************************/

enum {
	NB1413M3_NONE = 0,
	// unknown
	NB1413M3_JOKERMJN,
	NB1413M3_JANGOU,
	NB1413M3_JNGOLADY,
	NB1413M3_JNGNIGHT,
	NB1413M3_MJKING,
	NB1413M3_NIGHTGAL,
	NB1413M3_NGALSUMR,
	NB1413M3_ROYALNGT,
	NB1413M3_RYLQUEEN,
	NB1413M3_SWEETGAL,
	NB1413M3_SEXYGAL,
	// NB1411M1
	NB1413M3_PASTELG,
	NB1413M3_THREEDS,
	// NB1413M3
	NB1413M3_CRYSTALG,
	NB1413M3_CRYSTAL2,
	NB1413M3_NIGHTLOV,
	NB1413M3_CITYLOVE,
	NB1413M3_MCITYLOV,
	NB1413M3_SECOLOVE,
	NB1413M3_HOUSEMNQ,
	NB1413M3_HOUSEMN2,
	NB1413M3_LIVEGAL,
	NB1413M3_BIJOKKOY,
	NB1413M3_IEMOTO,
	NB1413M3_IEMOTOM,
	NB1413M3_RYUUHA,
	NB1413M3_SEIHA,
	NB1413M3_SEIHAM,
	NB1413M3_HYHOO,
	NB1413M3_HYHOO2,
	NB1413M3_SWINGGAL,
	NB1413M3_BIJOKKOG,
	NB1413M3_OJOUSAN,
	NB1413M3_OJOUSANM,
	NB1413M3_KORINAI,
	NB1413M3_KORINAIM,
	NB1413M3_MJCAMERA,
	NB1413M3_MJCAMERB,
	NB1413M3_MMCAMERA,
	NB1413M3_TAIWANMJ,
	NB1413M3_TAIWANMB,
	NB1413M3_OTONANO,
	NB1413M3_ABUNAI,
	NB1413M3_MJSIKAKU,
	NB1413M3_MMSIKAKU,
	NB1413M3_MSJIKEN,
	NB1413M3_HANAMOMO,
	NB1413M3_TELMAHJN,
	NB1413M3_GIONBANA,
	NB1413M3_MGION,
	NB1413M3_OMOTESND,
	NB1413M3_SCANDAL,
	NB1413M3_SCANDALM,
	NB1413M3_MGMEN89,
	NB1413M3_OHPYEPEE,
	NB1413M3_TOUGENK,
	NB1413M3_MJFOCUS,
	NB1413M3_MJFOCUSM,
	NB1413M3_GALKOKU,
	NB1413M3_MJNANPAS,
	NB1413M3_BANANADR,
	NB1413M3_GALKAIKA,
	NB1413M3_MCONTEST,
	NB1413M3_UCHUUAI,
	NB1413M3_TOKIMBSJ,
	NB1413M3_TOKYOGAL,
	NB1413M3_TRIPLEW1,
	NB1413M3_NTOPSTAR,
	NB1413M3_MLADYHTR,
	NB1413M3_PSTADIUM,
	NB1413M3_TRIPLEW2,
	NB1413M3_CLUB90S,
	NB1413M3_LOVEHOUS,
	NB1413M3_CHINMOKU,
	NB1413M3_VANILLA,
	NB1413M3_MJLSTORY,
	NB1413M3_QMHAYAKU,
	NB1413M3_MJGOTTUB,
	NB1413M3_MAIKO,
	NB1413M3_MMAIKO,
	NB1413M3_HANAOJI,
	NB1413M3_KAGUYA,
	NB1413M3_KAGUYA2,
	NB1413M3_APPAREL,
	NB1413M3_AV2MJ1BB,
	NB1413M3_AV2MJ2RG,
	NB1413M3_FINALBNY,
	NB1413M3_HYOUBAN,
	NB1413M3_ORANGEC,
	NB1413M3_ORANGECI,
	NB1413M3_VIPCLUB,
	NB1413M3_IDHIMITU,
	NB1413M3_KANATUEN,
	NB1413M3_KYUHITO,
	NB1413M3_PAIRSNB,
	NB1413M3_PAIRSTEN
};

#define NB1413M3_VCR_NOP	0x00
#define NB1413M3_VCR_POWER	0x01
#define NB1413M3_VCR_STOP	0x02
#define NB1413M3_VCR_REWIND	0x04
#define NB1413M3_VCR_PLAY	0x08
#define NB1413M3_VCR_FFORWARD	0x10
#define NB1413M3_VCR_PAUSE	0x20

#define NBMJCTRL_PORT1 \
	PORT_START	/* (3) PORT 1-1 */ \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_KAN ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_M ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_I ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_E ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_A ) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#define NBMJCTRL_PORT2 \
	PORT_START	/* (4) PORT 1-2 */ \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_MAHJONG_BET ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_REACH ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_N ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_J ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_F ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_B ) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#define NBMJCTRL_PORT3 \
	PORT_START	/* (5) PORT 1-3 */ \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_RON ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_CHI ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_K ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_G ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_C ) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#define NBMJCTRL_PORT4 \
	PORT_START	/* (6) PORT 1-4 */ \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_PON ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_L ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_H ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_D ) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#define NBMJCTRL_PORT5 \
	PORT_START	/* (7) PORT 1-5 */ \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("P1 Mahjong Small") PORT_CODE(KEYCODE_BACKSPACE) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("P1 Mahjong Big") PORT_CODE(KEYCODE_ENTER) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_FLIP_FLOP ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_DOUBLE_UP ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_SCORE ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_LAST_CHANCE ) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#define NBMJCTRL_PORT6 \
	PORT_START	/* (6) PORT 2-1 */ \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START2 ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_KAN ) PORT_PLAYER(2)\
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_M ) PORT_PLAYER(2)\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_I ) PORT_PLAYER(2)\
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_E ) PORT_PLAYER(2)\
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_A ) PORT_PLAYER(2)\
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#define NBMJCTRL_PORT7 \
	PORT_START	/* (7) PORT 2-2 */ \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_MAHJONG_BET ) PORT_PLAYER(2)\
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_REACH ) PORT_PLAYER(2)\
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_N ) PORT_PLAYER(2)\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_J ) PORT_PLAYER(2)\
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_F ) PORT_PLAYER(2)\
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_B ) PORT_PLAYER(2)\
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#define NBMJCTRL_PORT8 \
	PORT_START	/* (8) PORT 2-3 */ \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_MAHJONG_RON ) PORT_PLAYER(2)\
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_CHI ) PORT_PLAYER(2)\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_K ) PORT_PLAYER(2)\
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_G ) PORT_PLAYER(2)\
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_C ) PORT_PLAYER(2)\
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#define NBMJCTRL_PORT9 \
	PORT_START	/* (9) PORT 2-4 */ \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_PON ) PORT_PLAYER(2)\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_L ) PORT_PLAYER(2)\
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_H ) PORT_PLAYER(2)\
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_D ) PORT_PLAYER(2)\
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#define NBMJCTRL_PORT10 \
	PORT_START	/* (10) PORT 2-5 */ \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("P2 Mahjong Small") \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_OTHER ) PORT_NAME("P2 Mahjong Big") \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_MAHJONG_FLIP_FLOP ) PORT_PLAYER(2)\
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_MAHJONG_DOUBLE_UP ) PORT_PLAYER(2)\
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_MAHJONG_SCORE ) PORT_PLAYER(2)\
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_MAHJONG_LAST_CHANCE ) PORT_PLAYER(2)\
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

/*----------- defined in machine/nb1413m3.c -----------*/

MACHINE_RESET( nb1413m3 );
WRITE8_HANDLER( nb1413m3_nmi_clock_w );
INTERRUPT_GEN( nb1413m3_interrupt );
NVRAM_HANDLER( nb1413m3 );
READ8_HANDLER( nb1413m3_sndrom_r );
WRITE8_HANDLER( nb1413m3_sndrombank1_w );
WRITE8_HANDLER( nb1413m3_sndrombank2_w );
READ8_HANDLER( nb1413m3_gfxrom_r );
WRITE8_HANDLER( nb1413m3_gfxrombank_w );
WRITE8_HANDLER( nb1413m3_gfxradr_l_w );
WRITE8_HANDLER( nb1413m3_gfxradr_h_w );
WRITE8_HANDLER( nb1413m3_inputportsel_w );
READ8_HANDLER( nb1413m3_inputport0_r );
READ8_HANDLER( nb1413m3_inputport1_r );
READ8_HANDLER( nb1413m3_inputport2_r );
READ8_HANDLER( nb1413m3_inputport3_r );
READ8_HANDLER( nb1413m3_dipsw1_r );
READ8_HANDLER( nb1413m3_dipsw2_r );
READ8_HANDLER( nb1413m3_dipsw3_l_r );
READ8_HANDLER( nb1413m3_dipsw3_h_r );
WRITE8_HANDLER( nb1413m3_outcoin_w );
WRITE8_HANDLER( nb1413m3_vcrctrl_w );

extern int nb1413m3_type;
extern int nb1413m3_sndromregion;
extern int nb1413m3_sndrombank1;
extern int nb1413m3_sndrombank2;
extern int nb1413m3_busyctr;
extern int nb1413m3_busyflag;
extern int nb1413m3_inputport;
extern UINT8 *nb1413m3_nvram;
extern size_t nb1413m3_nvram_size;
