#include "emu.h"
#include "cpu/m6809/m6809.h"
#include "machine/6522via.h"
#include "includes/beezer.h"

extern const via6522_interface b_via_0_interface;
extern const via6522_interface b_via_1_interface;

static int pbus;
static int banklatch;

static READ8_DEVICE_HANDLER( b_via_0_pa_r );
static READ8_DEVICE_HANDLER( b_via_0_pb_r );
static WRITE8_DEVICE_HANDLER( b_via_0_pa_w );
static WRITE8_DEVICE_HANDLER( b_via_0_pb_w );
static READ_LINE_DEVICE_HANDLER( b_via_0_ca2_r );

static READ8_DEVICE_HANDLER( b_via_1_pa_r );
static READ8_DEVICE_HANDLER( b_via_1_pb_r );
static WRITE8_DEVICE_HANDLER( b_via_1_pa_w );
static WRITE8_DEVICE_HANDLER( b_via_1_pb_w );


/* VIA 0 (aka "PPCNP74", U6 @1C on schematics)
    enabled at CE00-CFFF of main m6809 cpu when bankswitch is set to 0
    port A:
        bit 7: input, X from banking latch (d3 of banking register)
        bit 6: input, Y from banking latch (d4 of banking register)
        bit 5: input, Z from banking latch (d5 of banking register)
        bit 4: N/C
        bit 3: output, /RESET for audio subcpu
        bit 2: output, /ENABLE for LS139@2H for reading control and dipswitch inputs to pbus
        bit 1: output, MSb of selector for inputs to pbus
        bit 0: output, LSb of "
    port B:
        bits 7-0: input/output: pbus
    port C:
        CA1: N/C
        CA2: input: "TDISP" (one of the higher bits in the video line counter, a mirror of the D5 bit from beezer_line_r), done in /video/beezer.c
        CB1: ASH1 to via 1
        CB2: ASH2 to via 1
    /IRQ: to main m6809 cpu
    /RES: from main reset generator/watchdog/button

    TODO: find a better way to attach ca2 read to beezer_line_r
    */
const via6522_interface b_via_0_interface =
{
	/*inputs : A/B         */ DEVCB_HANDLER(b_via_0_pa_r), DEVCB_HANDLER(b_via_0_pb_r),
	/*inputs : CA/B1,CA/B2 */ DEVCB_NULL, DEVCB_DEVICE_LINE_MEMBER("via6522_1", via6522_device, read_ca2), DEVCB_LINE(b_via_0_ca2_r), DEVCB_DEVICE_LINE_MEMBER("via6522_1", via6522_device, read_ca1),
	/*outputs: A/B         */ DEVCB_HANDLER(b_via_0_pa_w), DEVCB_HANDLER(b_via_0_pb_w),
	/*outputs: CA/B1,CA/B2 */ DEVCB_NULL, DEVCB_NULL, DEVCB_NULL, DEVCB_DEVICE_LINE_MEMBER("via6522_1", via6522_device, write_ca1),
	/*irq                  */ DEVCB_CPU_INPUT_LINE("maincpu", M6809_IRQ_LINE)
};

/* VIA 1 (U18 @3C on schematics)
    port A:
        bits 7-0: input/output: pbus
    port B:
        bit 7: output: TIMER1 OUT (used to gate NOISE (see below) to clock channel 1 of 6840, plus acts as channel 0 by itself)
        bit 6: input: NOISE (from mn5837 14-bit LFSR, which also connects to clock above)
        bit 5: output?: N/C
        bit 4: output?: FMSEL1 (does not appear elsewhere on schematics! what does this do? needs tracing) - always 0?
        bit 3: output?: FMSEL0 (does not appear elsewhere on schematics! what does this do? needs tracing) - always 0?
        bit 2: output?: AM (does not appear elsewhere on schematics! what does this do? needs tracing) - always 0?
        bit 1: output: FM or AM (appears to control some sort of suppression or filtering change of the post-DAC amplifier when enabled, only during the TIMER1 OUT time-slot of the multiplexer, see page 1B 3-3 of schematics) - always 0? why is there a special circuit for it?
        bit 0: output?: DMOD DISABLE (does not appear elsewhere on schematics! what does this do? needs tracing) - on startup is 0, turns to 1 and stays that way?
    port C:
        CA1: AHS2 from via 0 (are these two switched?)
        CA2: AHS1 from via 0 "
        CB1: ??put: DMOD CLR (does not appear elsewhere on schematics! what does this do? needs tracing)
        CB2: ??put: DMOD DATA (does not appear elsewhere on schematics! what does this do? needs tracing)
    /IRQ: to audio/sub m6809 cpu
    /RES: from audio reset bit of via 0

    TODO: the entirety of port B, much needs tracing
    TODO: ports CB1 and CB2, need tracing; ports CA1 and CA2 could use verify as well
    */
const via6522_interface b_via_1_interface =
{
	/*inputs : A/B         */ DEVCB_HANDLER(b_via_1_pa_r), DEVCB_HANDLER(b_via_1_pb_r),
	/*inputs : CA/B1,CA/B2 */ DEVCB_DEVICE_LINE_MEMBER("via6522_0", via6522_device, read_cb2), DEVCB_NULL, DEVCB_DEVICE_LINE_MEMBER("via6522_0", via6522_device, read_cb1), DEVCB_NULL,
	/*outputs: A/B         */ DEVCB_HANDLER(b_via_1_pa_w), DEVCB_HANDLER(b_via_1_pb_w),
	/*outputs: CA/B1,CA/B2 */ DEVCB_NULL, DEVCB_NULL, DEVCB_DEVICE_LINE_MEMBER("via6522_0", via6522_device, write_cb1), DEVCB_NULL,
	/*irq                  */ DEVCB_CPU_INPUT_LINE("audiocpu", M6809_IRQ_LINE)
};

static READ_LINE_DEVICE_HANDLER( b_via_0_ca2_r )
{
	return 0; // TODO: TDISP on schematic, same as D5 bit of scanline count from 74LS161 counter at 7A; attach properly

}

static READ8_DEVICE_HANDLER( b_via_0_pa_r )
{
	return (banklatch&0x38)<<2; // return X,Y,Z bits TODO: the Z bit connects somewhere else... where?
}

static READ8_DEVICE_HANDLER( b_via_0_pb_r )
{
	return pbus;
}

static WRITE8_DEVICE_HANDLER( b_via_0_pa_w )
{
	if ((data & 0x08) == 0)
		cputag_set_input_line(device->machine, "audiocpu", INPUT_LINE_RESET, ASSERT_LINE);
	else
		cputag_set_input_line(device->machine, "audiocpu", INPUT_LINE_RESET, CLEAR_LINE);

	if ((data & 0x04) == 0)
	{
		switch (data & 0x03)
		{
		case 0:
			pbus = input_port_read(device->machine, "IN0");
			break;
		case 1:
			pbus = input_port_read(device->machine, "IN1") | (input_port_read(device->machine, "IN2") << 4);
			break;
		case 2:
			pbus = input_port_read(device->machine, "DSWB");
			break;
		case 3:
			pbus = input_port_read(device->machine, "DSWA"); // Technically DSWA isn't populated on the board and is pulled to 0xFF with resistor pack, but there IS a DSWA port in the driver so we may as well use it.
			break;
		}
	}
}

static WRITE8_DEVICE_HANDLER( b_via_0_pb_w )
{
	pbus = data;
}

static READ8_DEVICE_HANDLER( b_via_1_pa_r )
{
	return pbus;
}

static READ8_DEVICE_HANDLER( b_via_1_pb_r )
{
	return 0x1F | (device->machine->rand()&0x40); // TODO: bit 6 is NOISE input from MM5837 17-bit LFSR; attach to audio device
}

static WRITE8_DEVICE_HANDLER( b_via_1_pa_w )
{
	pbus = data;
}

static WRITE8_DEVICE_HANDLER( b_via_1_pb_w )
{
	beezer_timer1_w(device->machine->device("custom"), 0, data&0x80);
	//if ((data&0x1f) != 0x01)
	//	popmessage("via1 pb low write of 0x%02x is not supported! contact mamedev!", data&0x1f);
}

DRIVER_INIT( beezer )
{
	pbus = 0;
	banklatch = 0;
}

WRITE8_HANDLER( beezer_bankswitch_w )
{
	banklatch = data&0x3f; // latched 'x,y,z' plus bank bits in ls174 @ 4H
	if ((data & 0x07) == 0)
	{
		via6522_device *via_0 = space->machine->device<via6522_device>("via6522_0");
		memory_install_write8_handler(space, 0xc600, 0xc7ff, 0, 0, watchdog_reset_w);
		memory_install_write8_handler(space, 0xc800, 0xc9ff, 0, 0, beezer_map_w);
		memory_install_read8_handler(space, 0xca00, 0xcbff, 0, 0, beezer_line_r);
		space->install_handler(0xce00, 0xcfff, 0, 0, read8_delegate_create(via6522_device, read, *via_0), write8_delegate_create(via6522_device, write, *via_0));
	}
	else
	{
		UINT8 *rom = space->machine->region("maincpu")->base() + 0x10000;
		memory_install_ram(space, 0xc000, 0xcfff, 0, 0, rom + (data & 0x07) * 0x2000 + ((data & 0x08) ? 0x1000: 0));
	}
}
