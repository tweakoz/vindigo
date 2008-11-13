#include "driver.h"
#include "video/konamiic.h"
#include "cpu/konami/konami.h"
#include "machine/eeprom.h"
#include "sound/k053260.h"
#include "includes/simpsons.h"

int simpsons_firq_enabled;

/***************************************************************************

  EEPROM

***************************************************************************/

static int init_eeprom_count;


static const eeprom_interface eeprom_intf =
{
	7,				/* address bits */
	8,				/* data bits */
	"011000",		/*  read command */
	"011100",		/* write command */
	0,				/* erase command */
	"0100000000000",/* lock command */
	"0100110000000" /* unlock command */
};

NVRAM_HANDLER( simpsons )
{
	if (read_or_write)
		eeprom_save(file);
	else
	{
		eeprom_init(&eeprom_intf);

		if (file)
		{
			init_eeprom_count = 0;
			eeprom_load(file);
		}
		else
			init_eeprom_count = 10;
	}
}

READ8_HANDLER( simpsons_eeprom_r )
{
	int res;

	res = (eeprom_read_bit() << 4);

	res |= 0x20;//konami_eeprom_ack() << 5; /* add the ack */

	res |= input_port_read(machine, "TEST") & 1; /* test switch */

	if (init_eeprom_count)
	{
		init_eeprom_count--;
		res &= 0xfe;
	}
	return res;
}

WRITE8_HANDLER( simpsons_eeprom_w )
{
	if ( data == 0xff )
		return;

	eeprom_write_bit(data & 0x80);
	eeprom_set_cs_line((data & 0x08) ? CLEAR_LINE : ASSERT_LINE);
	eeprom_set_clock_line((data & 0x10) ? ASSERT_LINE : CLEAR_LINE);

	simpsons_video_banking( machine, data & 3 );

	simpsons_firq_enabled = data & 0x04;
}

/***************************************************************************

  Coin Counters, Sound Interface

***************************************************************************/

WRITE8_HANDLER( simpsons_coin_counter_w )
{
	/* bit 0,1 coin counters */
	coin_counter_w(0,data & 0x01);
	coin_counter_w(1,data & 0x02);
	/* bit 2 selects mono or stereo sound */
	/* bit 3 = enable char ROM reading through the video RAM */
	K052109_set_RMRD_line((data & 0x08) ? ASSERT_LINE : CLEAR_LINE);
	/* bit 4 = INIT (unknown) */
	/* bit 5 = enable sprite ROM reading */
	K053246_set_OBJCHA_line((~data & 0x20) ? ASSERT_LINE : CLEAR_LINE);
}

READ8_HANDLER( simpsons_sound_interrupt_r )
{
	cpu_set_input_line_and_vector(machine->cpu[1], 0, HOLD_LINE, 0xff );
	return 0x00;
}

READ8_HANDLER( simpsons_sound_r )
{
	return k053260_0_r(machine, 2 + offset);
}


/***************************************************************************

  Banking, initialization

***************************************************************************/

static void simpsons_banking( int lines )
{
	memory_set_bank(1, lines & 0x3f);
}

MACHINE_RESET( simpsons )
{
	UINT8 *RAM = memory_region(machine, "main");

	cpu_set_info_fct(machine->cpu[0], CPUINFO_PTR_KONAMI_SETLINES_CALLBACK, (genf *)simpsons_banking);

	paletteram = &RAM[0x88000];
	simpsons_xtraram = &RAM[0x89000];
	spriteram16 = (UINT16 *)&RAM[0x8a000];

	simpsons_firq_enabled = 0;

	/* init the default banks */
	memory_configure_bank(1, 0, 64, memory_region(machine, "main") + 0x10000, 0x2000);
	memory_set_bank(1, 0);

	memory_configure_bank(2, 0, 2, memory_region(machine, "audio") + 0x10000, 0);
	memory_configure_bank(2, 2, 6, memory_region(machine, "audio") + 0x10000, 0x4000);
	memory_set_bank(2, 0);

	simpsons_video_banking( machine, 0 );
}
