/****************************************************************************
 *
 * warpwarp.c
 *
 * sound driver
 * juergen buchmueller <pullmoll@t-online.de>, jan 2000
 *
 ****************************************************************************/

#include <math.h>
#include "driver.h"
#include "deprecat.h"
#include "streams.h"
#include "sound/custom.h"
#include "includes/warpwarp.h"

#define CLOCK_16H	(18432000/3/2/16)
#define CLOCK_1V    (18432000/3/2/384)

static INT16 *decay = NULL;
static sound_stream *channel;
static int sound_latch = 0;
static int music1_latch = 0;
static int music2_latch = 0;
static int sound_signal = 0;
static int sound_volume = 0;
static emu_timer *sound_volume_timer = NULL;
static int music_signal = 0;
static int music_volume = 0;
static emu_timer *music_volume_timer = NULL;
static int noise = 0;

static TIMER_CALLBACK( sound_volume_decay )
{
	if( --sound_volume < 0 )
		sound_volume = 0;
}

WRITE8_HANDLER( warpwarp_sound_w )
{
	stream_update(channel);
	sound_latch = data & 0x0f;
	sound_volume = 0x7fff; /* set sound_volume */
	noise = 0x0000;  /* reset noise shifter */

    /* faster decay enabled? */
	if( sound_latch & 8 )
	{
		/*
         * R85(?) is 10k, Rb is 0, C92 is 1uF
         * charge time t1 = 0.693 * (R24 + Rb) * C57 -> 0.22176s
         * discharge time t2 = 0.693 * (Rb) * C57 -> 0
         * C90(?) is only charged via D17 (1N914), no discharge!
         * Decay:
         * discharge C90(?) (1uF) through R13||R14 (22k||47k)
         * 0.639 * 15k * 1uF -> 0.9585s
         */
        attotime period = attotime_div(attotime_mul(ATTOTIME_IN_HZ(32768), 95850), 100000);
		timer_adjust_periodic(sound_volume_timer, period, 0, period);
	}
	else
	{
		/*
         * discharge only after R93 (100k) and through the 10k
         * potentiometerin the amplifier section.
         * 0.639 * 110k * 1uF -> 7.0290s
         * ...but this is not very realistic for the game sound :(
         * maybe there _is_ a discharge through the diode D17?
         */
//      attotime period = attotime_div(attotime_mul(ATTOTIME_IN_HZ(32768), 702900), 100000);
        attotime period = attotime_div(attotime_mul(ATTOTIME_IN_HZ(32768), 191700), 100000);
		timer_adjust_periodic(sound_volume_timer, period, 0, period);
    }
}

WRITE8_HANDLER( warpwarp_music1_w )
{
    stream_update(channel);
	music1_latch = data & 0x3f;
}

static TIMER_CALLBACK( music_volume_decay )
{
	if( --music_volume < 0 )
        music_volume = 0;
}

WRITE8_HANDLER( warpwarp_music2_w )
{
    stream_update(channel);
	music2_latch = data & 0x3f;
	music_volume = 0x7fff;
	/* fast decay enabled? */
	if( music2_latch & 0x10 )
	{
		/*
         * Ra (R83?) is 10k, Rb is 0, C92 is 1uF
         * charge time t1 = 0.693 * (Ra + Rb) * C -> 0.22176s
         * discharge time is (nearly) zero, because Rb is zero
         * C95(?) is only charged via D17, not discharged!
         * Decay:
         * discharge C95(?) (10uF) through R13||R14 (22k||47k)
         * 0.639 * 15k * 10uF -> 9.585s
         * ...I'm sure this is off by one number of magnitude :/
         */
        attotime period = attotime_div(attotime_mul(ATTOTIME_IN_HZ(32768), 95850), 100000);
		timer_adjust_periodic(music_volume_timer, period, 0, period);
	}
	else
	{
		/*
         * discharge through R14 (47k),
         * discharge C95(?) (10uF) through R14 (47k)
         * 0.639 * 47k * 10uF -> 30.033s
         */
//      attotime period = attotime_div(attotime_mul(ATTOTIME_IN_HZ(32768), 3003300), 100000);
        attotime period = attotime_div(attotime_mul(ATTOTIME_IN_HZ(32768),  300330), 100000);
		timer_adjust_periodic(music_volume_timer, period, 0, period);
	}

}

static void warpwarp_sound_update(void *param, stream_sample_t **inputs, stream_sample_t **outputs, int length)
{
    static int vcarry = 0;
    static int vcount = 0;
    static int mcarry = 0;
	static int mcount = 0;
	stream_sample_t *buffer = outputs[0];

    while (length--)
    {
		*buffer++ = (sound_signal + music_signal) / 2;

		/*
         * The music signal is selected at a rate of 2H (1.536MHz) from the
         * four bits of a 4 bit binary counter which is clocked with 16H,
         * which is 192kHz, and is divided by 4 times (64 - music1_latch).
         *  0 = 256 steps -> 750 Hz
         *  1 = 252 steps -> 761.9 Hz
         * ...
         * 32 = 128 steps -> 1500 Hz
         * ...
         * 48 =  64 steps -> 3000 Hz
         * ...
         * 63 =   4 steps -> 48 kHz
         */
		mcarry -= CLOCK_16H / (4 * (64 - music1_latch));
		while( mcarry < 0 )
		{
			mcarry += CLOCK_16H;
			mcount++;
			music_signal = (mcount & ~music2_latch & 15) ? decay[music_volume] : 0;
			/* override by noise gate? */
			if( (music2_latch & 32) && (noise & 0x8000) )
				music_signal = decay[music_volume];
		}

		/* clock 1V = 8kHz */
		vcarry -= CLOCK_1V;
        while (vcarry < 0)
        {
            vcarry += CLOCK_16H;
            vcount++;

            /* noise is clocked with raising edge of 2V */
			if ((vcount & 3) == 2)
			{
				/* bit0 = bit0 ^ !bit10 */
				if ((noise & 1) == ((noise >> 10) & 1))
					noise = (noise << 1) | 1;
				else
					noise = noise << 1;
			}

            switch (sound_latch & 7)
            {
            case 0: /* 4V */
				sound_signal = (vcount & 0x04) ? decay[sound_volume] : 0;
                break;
            case 1: /* 8V */
				sound_signal = (vcount & 0x08) ? decay[sound_volume] : 0;
                break;
            case 2: /* 16V */
				sound_signal = (vcount & 0x10) ? decay[sound_volume] : 0;
                break;
            case 3: /* 32V */
				sound_signal = (vcount & 0x20) ? decay[sound_volume] : 0;
                break;
            case 4: /* TONE1 */
				sound_signal = !(vcount & 0x01) && !(vcount & 0x10) ? decay[sound_volume] : 0;
                break;
            case 5: /* TONE2 */
				sound_signal = !(vcount & 0x02) && !(vcount & 0x20) ? decay[sound_volume] : 0;
                break;
            case 6: /* TONE3 */
				sound_signal = !(vcount & 0x04) && !(vcount & 0x40) ? decay[sound_volume] : 0;
                break;
			default: /* NOISE */
				/* QH of 74164 #4V */
				sound_signal = (noise & 0x8000) ? decay[sound_volume] : 0;
            }

        }
    }
}

void *warpwarp_sh_start(int clock, const custom_sound_interface *config)
{
	int i;

	decay = (INT16 *) auto_malloc(32768 * sizeof(INT16));

    for( i = 0; i < 0x8000; i++ )
		decay[0x7fff-i] = (INT16) (0x7fff/exp(1.0*i/4096));

	channel = stream_create(0, 1, CLOCK_16H, NULL, warpwarp_sound_update);

	sound_volume_timer = timer_alloc(Machine, sound_volume_decay, NULL);
	music_volume_timer = timer_alloc(Machine, music_volume_decay, NULL);
    return auto_malloc(1);
}
