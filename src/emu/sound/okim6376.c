/**********************************************************************************************
 *
 *   OKI MSM6376 ADPCM
 *   by Mirko Buffoni, J. Wallace
 *
 *   TODO:
 *     add BEEP tone generator
 *     add echo - done, but not tested due to lack of supporting software
 *     add proper NAR handling - this varies with clock rate, and is ideally done
 *      off a second timer triggered by ST
 *     modernise
 **********************************************************************************************/


#include "emu.h"
#include "okim6376.h"

#define MAX_SAMPLE_CHUNK	10000
//#define MAX_WORDS           111

#define OKIVERBOSE 0
#define MSM6376LOG(x) do { if (OKIVERBOSE) logerror x; } while (0)


/* struct describing a single playing ADPCM voice */
struct ADPCMVoice
{
	UINT8 playing;			/* 1 if we are actively playing */

	UINT32 base_offset;		/* pointer to the base memory location */
	UINT32 sample;			/* current sample number */
	UINT32 count;			/* total samples to play */

	UINT32 volume;			/* output volume */
	INT32 signal;
	INT32 step;
};

typedef struct _okim6376_state okim6376_state;
struct _okim6376_state
{
	#define OKIM6376_VOICES		2
	struct ADPCMVoice voice[OKIM6376_VOICES];
	INT32 command[OKIM6376_VOICES];
	INT32 latch;			/* Command data is held before transferring to either channel */
	UINT8 *region_base;		/* pointer to the base of the region */
	sound_stream *stream;	/* which stream are we playing on? */
	UINT32 master_clock;	/* master clock frequency */
	UINT8 divisor;			/* can be 8,10,16, and is read out of ROM data */
	UINT8 channel;
	UINT8 nar;				/* Next Address Ready */
	UINT8 busy;
	UINT8 ch2;				/* 2CH pin - enables Channel 2 operation */
	UINT8 st;				/* STart */
	UINT8 st_pulses;		/* Keep track of attenuation */
	UINT8 ch2_update;		/* Pulse shape */
	UINT8 st_update;
};

/* step size index shift table */
static const int index_shift[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };

/* lookup table for the precomputed difference */
static int diff_lookup[49*16];

/* volume lookup table. Upon configuration, the number of ST pulses determine how much
   attenuation to apply to the sound signal. However, this only applies to the second
   channel*/
static const int volume_table[3] =
{
	0x20,	//   0 dB
	0x10,	//  -6.0 dB
	0x08,	// -12.0 dB
};

/* divisor lookup table. When an individual word is selected, it can be assigned one of three different 'rates'.
   These are implemented as clock divisors, and are looked up in the ROM header. More often than not, this value is 0,
   relating to a division by 8, or nominally 8KHz sampling.
   channel*/
static const int divisor_table[3] =
{
	8,
	10,
	16,
};

/* tables computed? */
static int tables_computed = 0;


INLINE okim6376_state *get_safe_token(device_t *device)
{
	assert(device != NULL);
	assert(device->type() == OKIM6376);
	return (okim6376_state *)downcast<legacy_device_base *>(device)->token();
}



/**********************************************************************************************

     compute_tables -- compute the difference tables

***********************************************************************************************/

static void compute_tables(void)
{
	/* nibble to bit map */
	static const int nbl2bit[16][4] =
	{
		{ 1, 0, 0, 0}, { 1, 0, 0, 1}, { 1, 0, 1, 0}, { 1, 0, 1, 1},
		{ 1, 1, 0, 0}, { 1, 1, 0, 1}, { 1, 1, 1, 0}, { 1, 1, 1, 1},
		{-1, 0, 0, 0}, {-1, 0, 0, 1}, {-1, 0, 1, 0}, {-1, 0, 1, 1},
		{-1, 1, 0, 0}, {-1, 1, 0, 1}, {-1, 1, 1, 0}, {-1, 1, 1, 1}
	};

	int step, nib;

	/* loop over all possible steps */
	for (step = 0; step <= 48; step++)
	{
		/* compute the step value */
		int stepval = floor(16.0 * pow(11.0 / 10.0, (double)step));

		/* loop over all nibbles and compute the difference */
		for (nib = 0; nib < 16; nib++)
		{
			diff_lookup[step*16 + nib] = nbl2bit[nib][0] *
				(stepval   * nbl2bit[nib][1] +
				 stepval/2 * nbl2bit[nib][2] +
				 stepval/4 * nbl2bit[nib][3] +
				 stepval/8);
		}
	}

	tables_computed = 1;
}



/**********************************************************************************************

     reset_adpcm -- reset the ADPCM stream

***********************************************************************************************/

static void reset_adpcm(struct ADPCMVoice *voice)
{
	/* make sure we have our tables */
	if (!tables_computed)
		compute_tables();

	/* reset the signal/step */
	voice->signal = -2;
	voice->step = 0;
}



/**********************************************************************************************

     clock_adpcm -- clock the next ADPCM byte

***********************************************************************************************/

static INT16 clock_adpcm(struct ADPCMVoice *voice, UINT8 nibble)
{
	voice->signal += diff_lookup[voice->step * 16 + (nibble & 15)];

	/* clamp to the maximum 12bit */
	if (voice->signal > 2047)
		voice->signal = 2047;
	else if (voice->signal < -2048)
		voice->signal = -2048;

	/* adjust the step size and clamp */
	voice->step += index_shift[nibble & 7];
	if (voice->step > 48)
		voice->step = 48;
	else if (voice->step < 0)
		voice->step = 0;

	/* return the signal */
	return voice->signal;
}



/**********************************************************************************************

     generate_adpcm -- general ADPCM decoding routine

***********************************************************************************************/

static void generate_adpcm(okim6376_state *chip, struct ADPCMVoice *voice, INT16 *buffer, int samples)
{
	/* if this voice is active */
	if (voice->playing)
	{
		UINT8 *base = chip->region_base + voice->base_offset;
		int sample = voice->sample;
		int count = voice->count;

		/* loop while we still have samples to generate */
		while (samples)
		{
			int nibble;

			if (count == 0)
			{
				/* get the number of samples to play */
				count = (base[sample / 2] & 0x7f) << 1;

				/* end of voice marker */
				if (count == 0)
				{
					voice->playing = 0;
					break;
				}
				else
				{
					/* step past the count byte */
					sample += 2;
				}
			}

			/* compute the new amplitude and update the current step */
			nibble = base[sample / 2] >> (((sample & 1) << 2) ^ 4);

			/* output to the buffer, scaling by the volume */
			/* signal in range -4096..4095, volume in range 2..16 => signal * volume / 2 in range -32768..32767 */
			*buffer++ = clock_adpcm(voice, nibble) * voice->volume / 2;

			++sample;
			--count;
			--samples;
		}

		/* update the parameters */
		voice->sample = sample;
		voice->count = count;
	}

	/* fill the rest with silence */
	while (samples--)
		*buffer++ = 0;
}



/**********************************************************************************************

     okim6376_update -- update the sound chip so that it is in sync with CPU execution

***********************************************************************************************/

static STREAM_UPDATE( okim6376_update )
{
	okim6376_state *chip = (okim6376_state *)param;
	int i;

	memset(outputs[0], 0, samples * sizeof(*outputs[0]));

	for (i = 0; i < OKIM6376_VOICES; i++)
	{
		struct ADPCMVoice *voice = &chip->voice[i];
		stream_sample_t *buffer = outputs[0];
		INT16 sample_data[MAX_SAMPLE_CHUNK];
		int remaining = samples;

		/* loop while we have samples remaining */
		while (remaining)
		{
			int samples = (remaining > MAX_SAMPLE_CHUNK) ? MAX_SAMPLE_CHUNK : remaining;
			int samp;

			generate_adpcm(chip, voice, sample_data, samples);
			for (samp = 0; samp < samples; samp++)
				*buffer++ += sample_data[samp];

			remaining -= samples;
		}
	}
}



/**********************************************************************************************

     state save support for MAME

***********************************************************************************************/

static void adpcm_state_save_register(struct ADPCMVoice *voice, device_t *device, int index)
{
	device->save_item(NAME(voice->playing), index);
	device->save_item(NAME(voice->sample), index);
	device->save_item(NAME(voice->count), index);
	device->save_item(NAME(voice->signal), index);
	device->save_item(NAME(voice->step), index);
	device->save_item(NAME(voice->volume), index);
	device->save_item(NAME(voice->base_offset), index);
}

static void okim6376_state_save_register(okim6376_state *info, device_t *device)
{
	int j;
	for (j = 0; j < OKIM6376_VOICES; j++)
	{
		adpcm_state_save_register(&info->voice[j], device, j);
	}
		device->save_item(NAME(info->command[0]));
		device->save_item(NAME(info->command[1]));
}

static void oki_process(device_t *device, int channel, int command)
{
	okim6376_state *info = get_safe_token(device);

	/* if a command is pending, process the second half */
	if ((command != -1) && (command != 0)) //process silence separately
	{
		int start;
		unsigned char *base/*, *base_end*/;
		info->nar = 0;//processing
		/* update the stream */
		info->stream->update();

		/* determine which voice(s) (voice is set by the state of 2CH) */
		{
			struct ADPCMVoice *voice = &info->voice[channel];

			/* determine the start position, max address space is 16Mbit */
			base = &info->region_base[info->command[channel] * 4];
			//base_end = &info->region_base[(MAX_WORDS+1) * 4];
			start = ((base[0] << 16) + (base[1] << 8) + base[2]) & 0x1fffff;

				if (start == 0)
				{
					voice->playing = 0;
					MSM6376LOG(("OKIM6376:'%s' Stopping %x\n",device->tag(),channel));
				}
				else
				{
					/* set up the voice to play this sample */
					if (!voice->playing)
					{
						voice->playing = 1;
						voice->base_offset = start;
						voice->sample = 0;
						voice->count = 0;

						/* also reset the ADPCM parameters */
						reset_adpcm(voice);
						/* Channel 1 cannot be attenuated, only Channel 2. We handle that separately */
						voice->volume = volume_table[0];
						MSM6376LOG(("OKIM6376:'%s' Playing %x\n",device->tag(),voice->base_offset));
					}
					else
					{
						MSM6376LOG(("OKIM6376:'%s' requested to play sample %02x on non-stopped voice\n",device->tag(),info->command[channel]));
					}
				}
				info->nar = 1;
			}
		}
		/* otherwise, see if this is a silence command */
		else
		{
			info->nar = 0;
			/* update the stream, then turn it off */
			info->stream->update();

			if (command ==0)
			{
				int i;
				for (i = 0; i < OKIM6376_VOICES; i++)
				{
					struct ADPCMVoice *voice = &info->voice[i];
					voice->playing = 0;
			}
			info->nar = 1;
		}
	}
}

/**********************************************************************************************

     DEVICE_START( okim6376 ) -- start emulation of an OKIM6376-compatible chip

***********************************************************************************************/

static DEVICE_START( okim6376 )
{
	okim6376_state *info = get_safe_token(device);
	int voice;

	compute_tables();

	info->command[0] = -1;
	info->command[1] = -1;
	info->latch = 0;
	info->divisor = divisor_table[0];
	info->region_base = *device->region();
	info->master_clock = device->clock();
	info->nar = 1;
	info->busy = 1;
	info->st = 1;
	info->st_update = 0;
	info->ch2_update = 0;
	info->st_pulses = 0;
	/* generate the name and create the stream */
	info->stream = device->machine().sound().stream_alloc(*device, 0, 1, info->master_clock / info->divisor, info, okim6376_update);

	/* initialize the voices */
	for (voice = 0; voice < OKIM6376_VOICES; voice++)
	{
		/* initialize the rest of the structure */
		info->voice[voice].volume = 0;
		reset_adpcm(&info->voice[voice]);
	}

	okim6376_state_save_register(info, device);
}

void okim6376_set_frequency(device_t *device, int frequency)
{
	okim6376_state *info = get_safe_token(device);

	info->master_clock = frequency;
	info->stream->set_sample_rate(info->master_clock / info->divisor);
}

/**********************************************************************************************

     DEVICE_RESET( okim6376 ) -- stop emulation of an OKIM6376-compatible chip

***********************************************************************************************/

static DEVICE_RESET( okim6376 )
{
	okim6376_state *info = get_safe_token(device);
	int i;

	info->stream->update();
	for (i = 0; i < OKIM6376_VOICES; i++)
		info->voice[i].playing = 0;
}




/**********************************************************************************************

     okim6376_status_r -- read the status port of an OKIM6376-compatible chip

***********************************************************************************************/

READ8_DEVICE_HANDLER( okim6376_r )
{
	okim6376_state *info = get_safe_token(device);
	int i, result;

	result = 0xff;

	/* set the bit to 1 if something is playing on a given channel */
	info->stream->update();
	for (i = 0; i < OKIM6376_VOICES; i++)
	{
		struct ADPCMVoice *voice = &info->voice[i];

		/* set the bit if it's playing */
		if (!voice->playing)
			result ^= 1 << i;
	}

	return result;
}


READ_LINE_DEVICE_HANDLER( okim6376_busy_r )
{
	okim6376_state *info = get_safe_token(device);
	int i,result=1;

	for (i = 0; i < OKIM6376_VOICES; i++)
	{
		struct ADPCMVoice *voice = &info->voice[i];

		/* set the bit low if it's playing */
		if (voice->playing)
			result = 0;
	}
	return result;
}

READ_LINE_DEVICE_HANDLER( okim6376_nar_r )
{
	okim6376_state *info = get_safe_token(device);
	MSM6376LOG(("OKIM6376:'%s' NAR %x\n",device->tag(),info->nar));
	return info->nar;
}

WRITE_LINE_DEVICE_HANDLER( okim6376_ch2_w )
{
	okim6376_state *info = get_safe_token(device);
	info->ch2_update = 0;//Clear flag
	MSM6376LOG(("OKIM6376:'%s' CH2 %x\n",device->tag(),state));

	if (info->ch2 != state)
	{
		info->ch2 = state;
		info->ch2_update = 1;
	}

	if((!info->ch2)&&(info->ch2_update))
	{
		struct ADPCMVoice *voice0 = &info->voice[0];
		struct ADPCMVoice *voice1 = &info->voice[1];
		//Echo functions when Channel 1 is playing, and ST is still high
		if ((voice0->playing)&&(info->st))
		{
			info->command[1] = info->command[0];//copy sample over
			voice1->volume = volume_table[1]; //echo is 6dB attenuated
 		}
		else
		{// Process as a proper channel
			info->channel = 1;
			if (info->command[1] != info->latch)
			{
				info->command[1] = info->latch;
			}
		}
	}

	if((info->ch2)&&(info->ch2_update))
	{
		info->channel = 0;
		oki_process(device, 0, info->command[1]);
	}
}


WRITE_LINE_DEVICE_HANDLER( okim6376_st_w )
{
	//As in STart, presumably, this triggers everything

	okim6376_state *info = get_safe_token(device);

	info->st_update = 0;//Clear flag
	MSM6376LOG(("OKIM6376:'%s' ST %x\n",device->tag(),state));

	if (info->st != state)
	{

		info->st = state;
		info->st_update = 1;

		if ((info->channel == 1) & !info->st)//ST acts as attenuation for Channel 2 when low, and stays at that level until the channel is reset
		{
			struct ADPCMVoice *voice = &info->voice[info->channel];
			{

				info->st_pulses ++;
				if (info->st_pulses > 3)
				{
					info->st_pulses = 3; //undocumented behaviour beyond 3 pulses
				}

				MSM6376LOG(("OKIM6376:'%s' Attenuation %x\n",device->tag(),info->st_pulses));
				voice->volume = volume_table[info->st_pulses];
			}
		}
		if (info->st && info->st_update)
		{
			info->command[info->channel] = info->latch;
			MSM6376LOG(("OKIM6376:'%s' Latching %x into  %x\n",device->tag(),info->latch,info->channel));

			if (info->channel ==0)
			{
				oki_process(device, 0, info->command[0]);
			}
		}
	}
}

/**********************************************************************************************

     okim6376_data_w -- write to the data port of an OKIM6376-compatible chip

***********************************************************************************************/

WRITE8_DEVICE_HANDLER( okim6376_w )
{
	// The data port is purely used to set the latch, everything else is started by an ST pulse

	okim6376_state *info = get_safe_token(device);
	info->latch = data & 0x7f;
	// FIX: The maximum adpcm words supported are 111, there are another 8 commands to generate BEEP tone, like beep.c (0x70 to 0x77),
	// and others for internal testing, that the manual explicitly says not to use (0x78 to 0x7f).
}



/**************************************************************************
 * Generic get_info
 **************************************************************************/

DEVICE_GET_INFO( okim6376 )
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case DEVINFO_INT_TOKEN_BYTES:				info->i = sizeof(okim6376_state);					break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case DEVINFO_FCT_START:						info->start = DEVICE_START_NAME( okim6376 );		break;
		case DEVINFO_FCT_STOP:						/* nothing */										break;
		case DEVINFO_FCT_RESET:						info->reset = DEVICE_RESET_NAME( okim6376 );		break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case DEVINFO_STR_NAME:						strcpy(info->s, "OKI6376");							break;
		case DEVINFO_STR_FAMILY:					strcpy(info->s, "OKI ADPCM");						break;
		case DEVINFO_STR_VERSION:					strcpy(info->s, "1.0");								break;
		case DEVINFO_STR_SOURCE_FILE:				strcpy(info->s, __FILE__);							break;
		case DEVINFO_STR_CREDITS:					strcpy(info->s, "Copyright Nicola Salmoria and the MAME Team"); break;
	}
}


DEFINE_LEGACY_SOUND_DEVICE(OKIM6376, okim6376);
