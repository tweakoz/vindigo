/*

    Sega/Yamaha AICA emulation
*/

#ifndef __AICA_H__
#define __AICA_H__

typedef struct _aica_interface aica_interface;
struct _aica_interface
{
	int master;
	int roffset;				/* offset in the region */
	void (*irq_callback)(running_device *device, int state);	/* irq callback */
};

void aica_set_ram_base(running_device *device, void *base, int size);

// AICA register access
READ16_DEVICE_HANDLER( aica_r );
WRITE16_DEVICE_HANDLER( aica_w );

// MIDI I/O access
WRITE16_DEVICE_HANDLER( aica_midi_in );
READ16_DEVICE_HANDLER( aica_midi_out_r );

DEVICE_GET_INFO( aica );
#define SOUND_AICA DEVICE_GET_INFO_NAME( aica )

#endif /* __AICA_H__ */
