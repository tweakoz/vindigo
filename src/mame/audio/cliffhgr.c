#include "driver.h"
#include "sound/discrete.h"

#define CLIFF_ENABLE_SND_1	NODE_01
#define CLIFF_ENABLE_SND_2	NODE_02

static const discrete_555_desc desc_cliffhgr_555 =
{
	DISC_555_OUT_ENERGY | DISC_555_OUT_DC,
	5,	/* B+ voltage of 555 */
	DEFAULT_555_VALUES
};

static const discrete_mixer_desc desc_cliffhgr_mixer =
{
	DISC_MIXER_IS_RESISTOR,
	{ 1.0/(1.0/RES_K(4.7)+1.0/RES_K(10))+RES_K(19), 1.0/(1.0/RES_K(5.1)+1.0/RES_K(12))+RES_K(19) },
	{ 0, 0 },
	{ CAP_U(4.7), CAP_U(4.7) },
	0,
	RES_K(10),
	0,
	CAP_U(4.7),
	0,
	8
};

DISCRETE_SOUND_START(cliffhgr)
	DISCRETE_INPUT_LOGIC(CLIFF_ENABLE_SND_1)
	DISCRETE_INPUT_LOGIC(CLIFF_ENABLE_SND_2)

	DISCRETE_555_ASTABLE(NODE_03, 1, RES_K(24), RES_K(19), CAP_U(0.047), &desc_cliffhgr_555)
	DISCRETE_555_ASTABLE(NODE_04, 1, RES_K(24), RES_K(19), CAP_U(0.1), &desc_cliffhgr_555)

	DISCRETE_ONOFF(NODE_05,CLIFF_ENABLE_SND_1,NODE_03)
	DISCRETE_ONOFF(NODE_06,CLIFF_ENABLE_SND_2,NODE_04)

	DISCRETE_RCFILTER(NODE_07,1,NODE_05,1.0/(1.0/RES_K(4.7)+1.0/RES_K(10)), CAP_U(0.1))
	DISCRETE_RCFILTER(NODE_08,1,NODE_06,1.0/(1.0/RES_K(5.1)+1.0/RES_K(12)), CAP_U(0.1))

	DISCRETE_GAIN(NODE_09,NODE_07,RES_K(10)/(RES_K(4.7)+RES_K(10)))
	DISCRETE_GAIN(NODE_10,NODE_08,RES_K(12)/(RES_K(5.1)+RES_K(12)))

	DISCRETE_MIXER2(NODE_11,1,NODE_09,NODE_10,&desc_cliffhgr_mixer)

//  DISCRETE_WAVELOG1(NODE_11,32767/9)
//  DISCRETE_WAVELOG2(CLIFF_ENABLE_SND_1,1000,NODE_09,1000)
//  DISCRETE_WAVELOG2(CLIFF_ENABLE_SND_2,1000,NODE_10,1000)
	DISCRETE_OUTPUT(NODE_11,32767/9)
DISCRETE_SOUND_END
