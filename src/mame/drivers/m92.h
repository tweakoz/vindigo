/*******************************************************************************

    Irem M92 input port macros

    Can probably be used for other Irem games..

*******************************************************************************/

#define IREM_JOYSTICKS_1_2 \
	PORT_START_TAG("JOY12")	\
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1) \
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1) \
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1) \
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1) \
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_UNUSED ) \
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNUSED ) \
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) \
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) \
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2) \
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2) \
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2) \
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2) \
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_UNUSED ) \
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNUSED ) \
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(2) \
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(2)

#define IREM_JOYSTICKS_3_4 \
	PORT_START_TAG("JOY34")	\
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3) \
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3) \
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3) \
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3) \
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_START3 ) /* If common slots, Coin3 if separate */ \
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_COIN3 ) \
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(3) \
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(3) \
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4) \
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4) \
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4) \
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4) \
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_START4 ) /* If common slots, Coin3 if separate */ \
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_COIN4 ) \
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(4) \
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(4)

#define IREM_COINS \
	PORT_START	\
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START2 ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE1 ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE )  \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNUSED )  \
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SPECIAL ) /* Actually vblank, handled above */ \

	/* Coin Mode 1, the new one (m72 early games have different Coinage) */ \
#define IREM_COIN_MODE_1_NEW \
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW2:5,6,7,8") \
	PORT_DIPSETTING(    0xa0, DEF_STR( 6C_1C ) ) \
	PORT_DIPSETTING(    0xb0, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0xd0, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0xe0, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x10, "2 Coins to Start/1 to Continue") \
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_2C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_3C ) ) \
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x90, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x60, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_6C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )

#define IREM_COIN_MODE_1_NEW_HIGH \
	PORT_DIPNAME( 0xf000, 0xf000, DEF_STR( Coinage ) ) PORT_DIPLOCATION("SW2:5,6,7,8") \
	PORT_DIPSETTING(    0xa000, DEF_STR( 6C_1C ) ) \
	PORT_DIPSETTING(    0xb000, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0xc000, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(    0xd000, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0xe000, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x1000, "2 Coins to Start/1 to Continue") \
	PORT_DIPSETTING(    0x3000, DEF_STR( 3C_2C ) ) \
	PORT_DIPSETTING(    0x2000, DEF_STR( 4C_3C ) ) \
	PORT_DIPSETTING(    0xf000, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(    0x4000, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(    0x9000, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x8000, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x7000, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(    0x6000, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x5000, DEF_STR( 1C_6C ) ) \
	PORT_DIPSETTING(    0x0000, DEF_STR( Free_Play ) )

#define IREM_COIN_MODE_2 \
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) ) \
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) ) \
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_B ) ) \
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_5C ) ) \
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) )

