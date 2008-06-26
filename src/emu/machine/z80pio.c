/***************************************************************************

    Z80 PIO implementation

    based on original version (c) 1997, Tatsuyuki Satoh

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#include "driver.h"
#include "deprecat.h"
#include "z80pio.h"
#include "cpu/z80/z80.h"
#include "cpu/z80/z80daisy.h"



/***************************************************************************
    DEBUGGING
***************************************************************************/

#define VERBOSE		0

#define VPRINTF(x) do { if (VERBOSE) logerror x; } while (0)



/***************************************************************************
    CONSTANTS
***************************************************************************/

#define PIO_MODE0		0x00		/* output mode */
#define PIO_MODE1		0x01		/* input  mode */
#define PIO_MODE2		0x02		/* i/o    mode */
#define PIO_MODE3		0x03		/* bit    mode */

/* pio controll port operation (bit 0-3) */
#define PIO_OP_MODE		0x0f		/* mode select        */
#define PIO_OP_INTC		0x07		/* interrupt controll */
#define PIO_OP_INTE		0x03		/* interrupt enable   */

/* pio interrupt controll nit */
#define PIO_INT_ENABLE	0x80		/* ENABLE : 0=disable , 1=enable */
#define PIO_INT_AND		0x40		/* LOGIC  : 0=OR      , 1=AND    */
#define PIO_INT_HIGH	0x20		/* LEVEL  : 0=low     , 1=high   */
#define PIO_INT_MASK	0x10		/* MASK   : 0=off     , 1=on     */



/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

struct _z80pio
{
	UINT8 vector[2];                      /* interrupt vector               */
	void (*intr)(running_machine *, int which);            /* interrupt callbacks            */
	void (*rdyr[2])(int data);          /* RDY active callback            */
	read8_machine_func  port_read[2];     /* port read callbacks            */
	write8_machine_func port_write[2];    /* port write callbacks           */
	UINT8 mode[2];                        /* mode 00=in,01=out,02=i/o,03=bit*/
	UINT8 enable[2];                      /* interrupt enable               */
	UINT8 mask[2];                        /* mask folowers                  */
	UINT8 dir[2];                         /* direction (bit mode)           */
	UINT8 rdy[2];                         /* ready pin level                */
	UINT8 in[2];                          /* input port data                */
	UINT8 out[2];                         /* output port                    */
	UINT8 strobe[2];						/* strobe inputs */
	UINT8 int_state[2];                   /* interrupt status (daisy chain) */
};
typedef struct _z80pio z80pio;



/***************************************************************************
    GLOBAL VARIABLES
***************************************************************************/

static z80pio pios[MAX_PIO];



/***************************************************************************
    INTERNAL STATE MANAGEMENT
***************************************************************************/

static void	set_rdy(z80pio *pio, int ch, int state)
{
	/* set state */
	pio->rdy[ch] = state;

	/* call callback with state */
	if (pio->rdyr[ch])
		(*pio->rdyr[ch])(pio->rdy[ch]);
}


static void interrupt_check(running_machine *machine, int which)
{
	z80pio *pio = pios + which;

	/* if we have a callback, update it with the current state */
	if (pio->intr)
		(*pio->intr)(machine, (z80pio_irq_state(which) & Z80_DAISY_INT) ? ASSERT_LINE : CLEAR_LINE);
}


static void update_irq_state(running_machine *machine, z80pio *pio, int ch)
{
	int old_state = pio->int_state[ch];
	int irq = 0;
	int data;
	if (pio->mode[ch] == 0x13 || (pio->enable[ch] & PIO_INT_MASK)) return;

	/* only check if interrupts are enabled */
	if (pio->enable[ch] & PIO_INT_ENABLE)
	{
		/* in mode 3, interrupts are tricky */
		if (pio->mode[ch] == PIO_MODE3)
		{
			/* fetch input data (ignore output lines) */
			data = pio->in[ch] & pio->dir[ch];

			/* keep only relevant bits */
			data &= ~pio->mask[ch];

			/* if active low, invert the bits */
			if (!(pio->enable[ch] & PIO_INT_HIGH))
				data ^= pio->mask[ch];

			/* if AND logic, interrupt if all bits are set */
			if (pio->enable[ch] & PIO_INT_AND)
				irq = (data == pio->mask[ch]);

			/* otherwise, interrupt if at least one bit is set */
			else
				irq = (data != 0);

			/* if portB, portA mode 2 check */
			if (ch && (pio->mode[0] == PIO_MODE2))
			{
				if (pio->rdy[ch] == 0)
					irq = 1;
			}
		}

		/* otherwise, just interrupt when ready is cleared */
		else
			irq = (pio->rdy[ch] == 0);
	}

	if (irq)
		pio->int_state[ch] |=  Z80_DAISY_INT;
	else
		pio->int_state[ch] &= ~Z80_DAISY_INT;

	if (old_state != pio->int_state[ch])
		interrupt_check(machine, pio - pios);
}



/***************************************************************************
    INITIALIZATION/CONFIGURATION
***************************************************************************/

void z80pio_init(int which, const z80pio_interface *intf)
{
	z80pio *pio = pios + which;

	assert(which < MAX_PIO);

	memset(pio, 0, sizeof(*pio));

	pio->intr = intf->intr;
	pio->port_read[0] = intf->portAread;
	pio->port_read[1] = intf->portBread;
	pio->port_write[0] = intf->portAwrite;
	pio->port_write[1] = intf->portBwrite;
	pio->rdyr[0] = intf->rdyA;
	pio->rdyr[1] = intf->rdyB;
	z80pio_reset(which);

    state_save_register_item_array("z80pio", which, pio->vector);
    state_save_register_item_array("z80pio", which, pio->mode);
    state_save_register_item_array("z80pio", which, pio->enable);
    state_save_register_item_array("z80pio", which, pio->mask);
    state_save_register_item_array("z80pio", which, pio->dir);
    state_save_register_item_array("z80pio", which, pio->rdy);
    state_save_register_item_array("z80pio", which, pio->in);
    state_save_register_item_array("z80pio", which, pio->out);
    state_save_register_item_array("z80pio", which, pio->strobe);
    state_save_register_item_array("z80pio", which, pio->int_state);
}


void z80pio_reset(int which)
{
	z80pio *pio = pios + which;
	int i;

	for (i = 0; i < 2; i++)
	{
		pio->mask[i]   = 0xff;	/* mask all on */
		pio->enable[i] = 0x00;	/* disable     */
		pio->mode[i]   = 0x01;	/* mode input  */
		pio->dir[i]    = 0x01;	/* dir  input  */
		set_rdy(pio, i, 0);	/* RDY = low   */
		pio->out[i]    = 0x00;	/* outdata = 0 */
		pio->int_state[i] = 0;
		pio->strobe[i] = 0;
	}
	interrupt_check(Machine, which);
}



/***************************************************************************
    CONTROL REGISTER READ/WRITE
***************************************************************************/

void z80pio_c_w(int which, int ch, UINT8 data)
{
	z80pio *pio = pios + which;

	/* load direction phase ? */
	if (pio->mode[ch] == 0x13)
	{
		VPRINTF(("PIO-%c Bits %02x\n", 'A' + ch, data));
		pio->dir[ch] = data;
		pio->mode[ch] = 0x03;
		return;
	}

	/* load mask folows phase ? */
	if (pio->enable[ch] & PIO_INT_MASK)
	{
		/* load mask folows */
		pio->mask[ch] = data;
		pio->enable[ch] &= ~PIO_INT_MASK;
		VPRINTF(("PIO-%c interrupt mask %02x\n",'A'+ch,data ));
		return;
	}

	switch (data & 0x0f)
	{
		case PIO_OP_MODE:	/* mode select 0=out,1=in,2=i/o,3=bit */
			pio->mode[ch] = (data >> 6);
			VPRINTF(("PIO-%c Mode %x\n", 'A' + ch, pio->mode[ch]));
			if (pio->mode[ch] == 0x03)
				pio->mode[ch] = 0x13;
			return;

		case PIO_OP_INTC:		/* interrupt control */
			pio->enable[ch] = data & 0xf0;
			pio->mask[ch] = 0x00;
			/* when interrupt enable , set vector request flag */
			VPRINTF(("PIO-%c Controll %02x\n", 'A' + ch, data));
			break;

		case PIO_OP_INTE:		/* interrupt enable controll */
			pio->enable[ch] &= ~PIO_INT_ENABLE;
			pio->enable[ch] |= (data & PIO_INT_ENABLE);
			VPRINTF(("PIO-%c enable %02x\n", 'A' + ch, data & 0x80));
			break;

		default:
			if (!(data & 1))
			{
				pio->vector[ch] = data;
				VPRINTF(("PIO-%c vector %02x\n", 'A' + ch, data));
			}
			else
				VPRINTF(("PIO-%c illegal command %02x\n", 'A' + ch, data));
			break;
	}

	/* interrupt check */
	update_irq_state(Machine, pio, ch);
}


UINT8 z80pio_c_r(int which, int ch)
{
	VPRINTF(("PIO-%c controll read\n", 'A' + ch ));
	return 0;
}



/***************************************************************************
    DATA REGISTER READ/WRITE
***************************************************************************/

void z80pio_d_w(int which, int ch, UINT8 data)
{
	z80pio *pio = pios + which;

	pio->out[ch] = data;	/* latch out data */
	if(pio->port_write[ch])
		pio->port_write[ch](Machine, 0, data);

	switch (pio->mode[ch])
	{
		case PIO_MODE0:			/* mode 0 output */
		case PIO_MODE2:			/* mode 2 i/o */
			set_rdy(pio, ch, 1); /* ready = H */
			update_irq_state(Machine, pio, ch);
			return;

		case PIO_MODE1:			/* mode 1 input */
		case PIO_MODE3:			/* mode 0 bit */
			return;

		default:
			VPRINTF(("PIO-%c data write,bad mode\n",'A'+ch ));
	}
}


UINT8 z80pio_d_r(int which, int ch)
{
	z80pio *pio = pios + which;

	switch (pio->mode[ch])
	{
		case PIO_MODE0:			/* mode 0 output */
			return pio->out[ch];

		case PIO_MODE1:			/* mode 1 input */
			set_rdy(pio, ch, 1);	/* ready = H */
			if(pio->port_read[ch])
				pio->in[ch] = pio->port_read[ch](Machine, 0);
			update_irq_state(Machine, pio, ch);
			return pio->in[ch];

		case PIO_MODE2:			/* mode 2 i/o */
			if (ch) VPRINTF(("PIO-B mode 2 \n"));
			set_rdy(pio, 1, 1); /* brdy = H */
			if(pio->port_read[ch])
				pio->in[ch] = pio->port_read[ch](Machine, 0);
			update_irq_state(Machine, pio, ch);
			return pio->in[ch];

		case PIO_MODE3:			/* mode 3 bit */
			if(pio->port_read[ch])
				pio->in[ch] = pio->port_read[ch](Machine, 0);
			return (pio->in[ch] & pio->dir[ch]) | (pio->out[ch] & ~pio->dir[ch]);
	}
	VPRINTF(("PIO-%c data read,bad mode\n",'A'+ch ));
	return 0;
}



/***************************************************************************
    PORT I/O
***************************************************************************/

void z80pio_p_w(int which, UINT8 ch, UINT8 data)
{
	z80pio *pio = pios + which;

	pio->in[ch]  = data;
	switch (pio->mode[ch])
	{
		case PIO_MODE0:
			VPRINTF(("PIO-%c OUTPUT mode and data write\n",'A'+ch ));
			break;

		case PIO_MODE2:	/* only port A */
			ch = 1;		/* handshake and IRQ is use portB */

		case PIO_MODE1:
			set_rdy(pio, ch, 0);
			update_irq_state(Machine, pio, ch);
			break;

		case PIO_MODE3:
			/* irq check */
			update_irq_state(Machine, pio, ch);
			break;
	}
}


int z80pio_p_r(int which, UINT8 ch)
{
	z80pio *pio = pios + which;

	switch (pio->mode[ch])
	{
		case PIO_MODE2:		/* port A only */
		case PIO_MODE0:
			set_rdy(pio, ch, 0);
			update_irq_state(Machine, pio, ch);
			break;

		case PIO_MODE1:
			VPRINTF(("PIO-%c INPUT mode and data read\n",'A'+ch ));
			break;

		case PIO_MODE3:
			return (pio->in[ch] & pio->dir[ch]) | (pio->out[ch] & ~pio->dir[ch]);
	}
	return pio->out[ch];
}


WRITE8_HANDLER( z80pioA_0_p_w ) { z80pio_p_w(0, 0, data);   }
WRITE8_HANDLER( z80pioB_0_p_w ) { z80pio_p_w(0, 1, data);   }
READ8_HANDLER( z80pioA_0_p_r )  { return z80pio_p_r(0, 0);  }
READ8_HANDLER( z80pioB_0_p_r )  { return z80pio_p_r(0, 1);  }

WRITE8_HANDLER( z80pioA_1_p_w ) { z80pio_p_w(1, 0, data);   }
WRITE8_HANDLER( z80pioB_1_p_w ) { z80pio_p_w(1, 1, data);   }
READ8_HANDLER( z80pioA_1_p_r )  { return z80pio_p_r(1, 0);  }
READ8_HANDLER( z80pioB_1_p_r )  { return z80pio_p_r(1, 1);  }



/***************************************************************************
    STROBE STATE MANAGEMENT
***************************************************************************/

static void z80pio_update_strobe(int which, int ch, int state)
{
	z80pio *pio = pios + which;

	switch (pio->mode[ch])
	{
		/* output mode: a positive edge is used by peripheral to acknowledge
            the receipt of data */
		case PIO_MODE0:
		{
			/* ensure valid */
			state = state & 0x01;

			/* strobe changed state? */
			if ((pio->strobe[ch] ^ state) != 0)
			{
				/* yes */
				if (state != 0)
				{
					/* positive edge */
					VPRINTF(("PIO-%c positive strobe\n",'A' + ch));

					/* ready is now inactive */
					set_rdy(pio, ch, 0);

					/* int enabled? */
					if (pio->enable[ch] & PIO_INT_ENABLE)
					{
						/* trigger an int request */
						pio->int_state[ch] |= Z80_DAISY_INT;
					}
				}
			}

			/* store strobe state */
			pio->strobe[ch] = state;

			/* check interrupt */
			interrupt_check(Machine, which);
		}
		break;

		/* input mode: strobe is used by peripheral to load data from the peripheral
            into port a input register, data loaded into pio when signal is active */

		default:
			break;
	}
}


void z80pio_astb_w(int which, int state) { z80pio_update_strobe(which, 0, state); }
void z80pio_bstb_w(int which, int state) { z80pio_update_strobe(which, 1, state); }



/***************************************************************************
    DAISY CHAIN INTERFACE
***************************************************************************/

int z80pio_irq_state(int which)
{
	z80pio *pio = pios + which;
	int state = 0;
	int ch;

	/* loop over all channels */
	for (ch = 0; ch < 2; ch++)
	{
		/* if we're servicing a request, don't indicate more interrupts */
		if (pio->int_state[ch] & Z80_DAISY_IEO)
		{
			state |= Z80_DAISY_IEO;
			break;
		}
		state |= pio->int_state[ch];
	}
	return state;
}


int z80pio_irq_ack(int which)
{
	z80pio *pio = pios + which;
	int ch;

	/* loop over all channels */
	for (ch = 0; ch < 2; ch++)

		/* find the first channel with an interrupt requested */
		if (pio->int_state[ch] & Z80_DAISY_INT)
		{
			/* clear interrupt, switch to the IEO state, and update the IRQs */
			pio->int_state[ch] = Z80_DAISY_IEO;
			interrupt_check(Machine, which);
			return pio->vector[ch];
		}

	VPRINTF(("z80pio_irq_ack: failed to find an interrupt to ack!"));
	return pio->vector[0];
}


void z80pio_irq_reti(int which)
{
	z80pio *pio = pios + which;
	int ch;

	/* loop over all channels */
	for (ch = 0; ch < 2; ch++)

		/* find the first channel with an IEO pending */
		if (pio->int_state[ch] & Z80_DAISY_IEO)
		{
			/* clear the IEO state and update the IRQs */
			pio->int_state[ch] &= ~Z80_DAISY_IEO;
			interrupt_check(Machine, which);
			return;
		}

	VPRINTF(("z80pio_irq_reti: failed to find an interrupt to clear IEO on!"));
}

/***************************************************************************
    READ/WRITE HANDLERS
***************************************************************************/
READ8_HANDLER(z80pio_0_r)
{
	return (offset & 2) ? z80pio_c_r(0, offset & 1) : z80pio_d_r(0, offset & 1);
}

WRITE8_HANDLER(z80pio_0_w)
{
	if (offset & 2)
		z80pio_c_w(0, offset & 1, data);
	else
		z80pio_d_w(0, offset & 1, data);
}

READ8_HANDLER(z80pio_1_r)
{
	return (offset & 2) ? z80pio_c_r(1, offset & 1) : z80pio_d_r(1, offset & 1);
}

WRITE8_HANDLER(z80pio_1_w)
{
	if (offset & 2)
		z80pio_c_w(1, offset & 1, data);
	else
		z80pio_d_w(1, offset & 1, data);
}
