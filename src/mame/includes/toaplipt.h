/*******************************************************************************

    Input port macros used by many games in multiple Toaplan drivers
    (similar to Taito but bits are ACTIVE_HIGH instead of ACTIVE_LOW)

*******************************************************************************/


/**************************** Machine Dip Switches ****************************/

/* with location */
#define TOAPLAN_DSWA_BITS_2_TO_3_LOC(DIPBANK) \
	PORT_SERVICE_DIPLOC( 0x04, IP_ACTIVE_HIGH, #DIPBANK":!3" ) \
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) ) PORT_DIPLOCATION(#DIPBANK":!4") \
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

#define TOAPLAN_DSWA_BITS_1_TO_3_LOC(DIPBANK) \
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Flip_Screen ) ) PORT_DIPLOCATION(#DIPBANK":!2") \
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x02, DEF_STR( On ) ) \
	TOAPLAN_DSWA_BITS_2_TO_3_LOC(DIPBANK)

#define TOAPLAN_MACHINE_COCKTAIL_LOC(DIPBANK) \
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Cabinet ) ) PORT_DIPLOCATION(#DIPBANK":!1") \
	PORT_DIPSETTING(    0x01, DEF_STR( Upright ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) ) \
	TOAPLAN_DSWA_BITS_1_TO_3_LOC(DIPBANK)

#define TOAPLAN_MACHINE_NO_COCKTAIL_LOC(DIPBANK) \
	PORT_DIPUNUSED_DIPLOC( 0x01, IP_ACTIVE_HIGH, #DIPBANK":!1" ) \
	TOAPLAN_DSWA_BITS_1_TO_3_LOC(DIPBANK)


/* without location */
#define TOAPLAN_DSWA_BITS_2_TO_3 \
	PORT_SERVICE( 0x04, IP_ACTIVE_HIGH ) \
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) ) \
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

#define TOAPLAN_DSWA_BITS_1_TO_3 \
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Flip_Screen ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    0x02, DEF_STR( On ) ) \
	TOAPLAN_DSWA_BITS_2_TO_3

#define TOAPLAN_MACHINE_COCKTAIL \
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Cabinet ) ) \
	PORT_DIPSETTING(    0x01, DEF_STR( Upright ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) ) \
	TOAPLAN_DSWA_BITS_1_TO_3

#define TOAPLAN_MACHINE_NO_COCKTAIL \
	PORT_DIPUNUSED( 0x01, IP_ACTIVE_HIGH ) \
	TOAPLAN_DSWA_BITS_1_TO_3


/**************************** Coinage Dip Swicthes ****************************/

/* with location */
#define TOAPLAN_COINAGE_JAPAN_OLD_COIN_A_LOC(DIPBANK) \
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) ) PORT_DIPLOCATION(#DIPBANK":!5,!6") \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_2C ) )

#define TOAPLAN_COINAGE_JAPAN_OLD_COIN_B_LOC(DIPBANK) \
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) ) PORT_DIPLOCATION(#DIPBANK":!7,!8") \
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_2C ) )

#define TOAPLAN_COINAGE_WORLD_COIN_A_LOC(DIPBANK) \
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) ) PORT_DIPLOCATION(#DIPBANK":!5,!6") \
	PORT_DIPSETTING(    0x30, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )

#define TOAPLAN_COINAGE_WORLD_COIN_B_LOC(DIPBANK) \
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) ) PORT_DIPLOCATION(#DIPBANK":!7,!8") \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_6C ) )

#define TOAPLAN_COINAGE_JAPAN_OLD_LOC(DIPBANK) \
	TOAPLAN_COINAGE_JAPAN_OLD_COIN_A_LOC(DIPBANK) \
	TOAPLAN_COINAGE_JAPAN_OLD_COIN_B_LOC(DIPBANK)

#define TOAPLAN_COINAGE_WORLD_LOC(DIPBANK) \
	TOAPLAN_COINAGE_WORLD_COIN_A_LOC(DIPBANK) \
	TOAPLAN_COINAGE_WORLD_COIN_B_LOC(DIPBANK)

#define TOAPLAN_COINAGE_DUAL_LOC(DIP, MASK, WORLD, DIPBANK) \
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) ) PORT_DIPLOCATION(#DIPBANK":!5,!6") \
	PORT_DIPSETTING(    0x00, DEF_STR( 4C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_B ) ) PORT_DIPLOCATION(#DIPBANK":!7,!8") \
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_2C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_4C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD)

/* without location */
#define TOAPLAN_COINAGE_JAPAN_OLD_COIN_A \
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_2C ) )

#define TOAPLAN_COINAGE_JAPAN_OLD_COIN_B \
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_2C ) )

#define TOAPLAN_COINAGE_WORLD_COIN_A \
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )

#define TOAPLAN_COINAGE_WORLD_COIN_B \
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_6C ) )

#define TOAPLAN_COINAGE_JAPAN_OLD \
	TOAPLAN_COINAGE_JAPAN_OLD_COIN_A \
	TOAPLAN_COINAGE_JAPAN_OLD_COIN_B

#define TOAPLAN_COINAGE_WORLD \
	TOAPLAN_COINAGE_WORLD_COIN_A \
	TOAPLAN_COINAGE_WORLD_COIN_B

#define TOAPLAN_COINAGE_DUAL(DIP, MASK, WORLD) \
	PORT_DIPNAME( 0x30, 0x00, DEF_STR( Coin_A ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 4C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x20, DEF_STR( 3C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x30, DEF_STR( 2C_3C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_2C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPNAME( 0xc0, 0x00, DEF_STR( Coin_B ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 2C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_3C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_2C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_NOTEQUALS,WORLD) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_2C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_3C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_4C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_6C ) ) PORT_CONDITION(#DIP,MASK,PORTCOND_EQUALS,WORLD)


/************************** Difficulty Dip Switches ***************************/

/* with location */
#define TOAPLAN_DIFFICULTY_LOC(DIPBANK) \
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Difficulty ) ) PORT_DIPLOCATION(#DIPBANK":!1,!2") \
	PORT_DIPSETTING(    0x01, DEF_STR( Easy ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) ) \
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) ) \
	PORT_DIPSETTING(    0x03, DEF_STR( Very_Hard ) )

/* without location */
#define TOAPLAN_DIFFICULTY \
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Difficulty ) ) \
	PORT_DIPSETTING(    0x01, DEF_STR( Easy ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Normal ) ) \
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) ) \
	PORT_DIPSETTING(    0x03, DEF_STR( Very_Hard ) )


/************************** Joysticks and buttons *****************************/

/* generic (might be moved elsewhere) */
#define INPUT_GENERIC_JOY_LOW_NIBBLE(PL, STATE, WAY, B01, B02, B04, B08) \
	PORT_BIT( 0x01, STATE, IPT_JOYSTICK_##B01 ) WAY PORT_PLAYER(PL) \
	PORT_BIT( 0x02, STATE, IPT_JOYSTICK_##B02 ) WAY PORT_PLAYER(PL) \
	PORT_BIT( 0x04, STATE, IPT_JOYSTICK_##B04 ) WAY PORT_PLAYER(PL) \
	PORT_BIT( 0x08, STATE, IPT_JOYSTICK_##B08 ) WAY PORT_PLAYER(PL)


/* Toaplan generic (more might be needed) */
#define TOAPLAN_GENERIC_JOY_MONO_UDLR(PL) \
	INPUT_GENERIC_JOY_LOW_NIBBLE(PL, IP_ACTIVE_HIGH, PORT_8WAY, UP, DOWN, LEFT, RIGHT)


#define TOAPLAN_GENERIC_BUTTONS_HIGH_NIBBLE(PL, STATE, BUTTON_A, BUTTON_B) \
	PORT_BIT( 0x10, STATE, IPT_##BUTTON_A ) PORT_PLAYER(PL) \
	PORT_BIT( 0x20, STATE, IPT_##BUTTON_B ) PORT_PLAYER(PL) \
	/* DO NOT fill bit 6 as it may be defined as START button ! */

#define TOAPLAN_GENERIC_1_BUTTON(PL) \
	TOAPLAN_GENERIC_BUTTONS_HIGH_NIBBLE(PL, IP_ACTIVE_HIGH, BUTTON1, UNKNOWN) \
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

#define TOAPLAN_GENERIC_2_BUTTONS(PL) \
	TOAPLAN_GENERIC_BUTTONS_HIGH_NIBBLE(PL, IP_ACTIVE_HIGH, BUTTON1, BUTTON2) \
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

#define TOAPLAN_GENERIC_3_BUTTONS(PL) \
	TOAPLAN_GENERIC_BUTTONS_HIGH_NIBBLE(PL, IP_ACTIVE_HIGH, BUTTON1, BUTTON2) \
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_PLAYER(PL) \
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

#define TOAPLAN_GENERIC_1_BUTTON_START(PL) \
	TOAPLAN_GENERIC_BUTTONS_HIGH_NIBBLE(PL, IP_ACTIVE_HIGH, BUTTON1, UNKNOWN) \
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START##PL ) \
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

#define TOAPLAN_GENERIC_2_BUTTONS_START(PL) \
	TOAPLAN_GENERIC_BUTTONS_HIGH_NIBBLE(PL, IP_ACTIVE_HIGH, BUTTON1, BUTTON2) \
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_START##PL ) \
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )


/* Toaplan games (more might be needed) */
#define TOAPLAN_JOY_UDLR_1_BUTTON(PL) \
	TOAPLAN_GENERIC_JOY_MONO_UDLR(PL) \
	TOAPLAN_GENERIC_1_BUTTON(PL)

#define TOAPLAN_JOY_UDLR_2_BUTTONS(PL) \
	TOAPLAN_GENERIC_JOY_MONO_UDLR(PL) \
	TOAPLAN_GENERIC_2_BUTTONS(PL)

#define TOAPLAN_JOY_UDLR_3_BUTTONS(PL) \
	TOAPLAN_GENERIC_JOY_MONO_UDLR(PL) \
	TOAPLAN_GENERIC_3_BUTTONS(PL)


#define TOAPLAN_JOY_UDLR_1_BUTTON_START(PL) \
	TOAPLAN_GENERIC_JOY_MONO_UDLR(PL) \
	TOAPLAN_GENERIC_1_BUTTON_START(PL)

#define TOAPLAN_JOY_UDLR_2_BUTTONS_START(PL) \
	TOAPLAN_GENERIC_JOY_MONO_UDLR(PL) \
	TOAPLAN_GENERIC_2_BUTTONS_START(PL)


/******************************* Test switch **********************************/

/* Set to 0 if you want to see the "TEST" switch as a standard input
   Set to 1 if you want to see the "TEST" switch as a fake Dip Switch */

#define SHOW_TEST_AS_DIP	1

#if SHOW_TEST_AS_DIP
#define TOAPLAN_TEST_SWITCH(MASK, STATE) \
	PORT_DIPNAME( MASK, MASK & STATE, "Test Switch" ) PORT_CODE(KEYCODE_F1) PORT_TOGGLE \
	PORT_DIPSETTING(    MASK & STATE, DEF_STR( Off ) ) \
	PORT_DIPSETTING(    MASK & ~STATE, DEF_STR( On ) )
#else
#define TOAPLAN_TEST_SWITCH(MASK, STATE) \
	PORT_BIT( MASK, MASK & STATE, IPT_OTHER ) PORT_NAME("Test Switch") PORT_CODE(KEYCODE_F1)
#endif

