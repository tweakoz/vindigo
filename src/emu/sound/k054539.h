/*********************************************************

    Konami 054539 PCM Sound Chip

*********************************************************/

#pragma once

#ifndef __K054539_H__
#define __K054539_H__

typedef struct _k054539_interface k054539_interface;
struct _k054539_interface
{
	const char *rgnoverride;
	void (*apan)(double, double);	/* Callback for analog output mixing levels (0..1 for each channel) */
	void (*irq)(running_machine *);
};


WRITE8_HANDLER( k054539_0_w );
READ8_HANDLER( k054539_0_r );
WRITE8_HANDLER( k054539_1_w );
READ8_HANDLER( k054539_1_r );

//* control flags, may be set at DRIVER_INIT().
#define K054539_RESET_FLAGS     0
#define K054539_REVERSE_STEREO  1
#define K054539_DISABLE_REVERB  2
#define K054539_UPDATE_AT_KEYON 4

void k054539_init_flags(int chip, int flags);

/*
    Note that the eight PCM channels of a K054539 do not have seperate
    volume controls. Considering the global attenuation equation may not
    be entirely accurate, k054539_set_gain() provides means to control
    channel gain. It can be called anywhere but preferrably from
    DRIVER_INIT().

    Parameters:
        chip    : 0 / 1
        channel : 0 - 7
        gain    : 0.0=silent, 1.0=no gain, 2.0=twice as loud, etc.
*/
void k054539_set_gain(int chip, int channel, double gain);

SND_GET_INFO( k054539 );
#define SOUND_K054539 SND_GET_INFO_NAME( k054539 )

#endif /* __K054539_H__ */
