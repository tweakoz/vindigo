/* tecmosys protection related functions */

/*
    The device validates a password,
    then uploads the size of a code upload followed by the code upload itself.
    After that, it uploads 4 ranges of code to checksum, followed by the 4 checksums.
    The 68K does the checksumming, and returns the results to the protection device.

    Apart from inital protection calls and code upload, the vblank in both games writes
    info to the protection but they seem to ignore the returned data.
    Maybe the protection is tied to something else, or maybe it was preliminary work on
    further security.
    This is what happens in the vblank:
    - prot_w( 0xff )
    - val = prot_r()
    - prot_w( checksum1[val] )
    (The area following checksum1 is the code upload in deroon, and active RAM in tkdensho,
    so the value sent may be meaningless.)

    There is provision for calling the protection read/write functions from two of the trap 0xf switch
    statements in the 68K, but I don't see it being used anywhere.

    It looks like the code upload is very plain, it can only be 0xff bytes long, and not contain the byte 0xff.
    The checksum ranges can't contain 0xff either, although the checksum values can.
    I'd be very interested in putting some trojan ROMs together if anyone has a board to run them on.
    It might be possible to use one set of ROMs to get the checksum ranges,
    and another set with dump code places outside those ranges.
    You can get me at nuapete@hotmail.com
*/

#include "emu.h"
#include "includes/tecmosys.h"

enum DEV_STATUS
{
	DS_IDLE,
	DS_LOGIN,
	DS_SEND_CODE,
	DS_SEND_ADRS,
	DS_SEND_CHKSUMS,
	DS_DONE
};

struct prot_data
{
	UINT8 passwd_len;
	const UINT8* passwd;
	const UINT8* code;
	UINT8 checksum_ranges[17];
	UINT8 checksums[4];
};


// deroon prot data
static const UINT8 deroon_passwd[] = {'L','U','N','A',0};
static const UINT8 deroon_upload[] = {0x02, 0x4e, 0x75, 0x00 }; // code length, code, 0x00 trailer
static const struct prot_data deroon_data =
{
	5,
	deroon_passwd,
	deroon_upload,
	{
		0x10,0x11,0x12,0x13,	// range 1 using static ranges from the ROM to avoid calculating sums.
		0x24,0x25,0x26,0x27,	// range 2
		0x38,0x39,0x3a,0x3b,	// range 3
		0x4c,0x4d,0x4e,0x4f,	// range 4
		0x00,					// trailer
	},
	{ 0xa6, 0x29, 0x4b, 0x3f }
};

// tkdensho prot data
static const UINT8 tkdensho_passwd[] = {'A','G','E','P','R','O','T','E','C','T',' ','S','T','A','R','T',0};
static const UINT8 tkdensho_upload[] = {0x06, 0x4e, 0xf9, 0x00, 0x00, 0x22, 0xc4,0x00};
static const struct prot_data tkdensho_data =
{
	0x11,
	tkdensho_passwd,
	tkdensho_upload,
	{
		0x10,0x11,0x12,0x13,	// range 1
		0x24,0x25,0x26,0x27,	// range 2
		0x38,0x39,0x3a,0x3b,	// range 3
		0x4c,0x4d,0x4e,0x4f,	// range 4
		0x00,					// trailer
	},
	{ 0xbf, 0xfa, 0xda, 0xda }
};

static const struct prot_data tkdensha_data =
{
	0x11,
	tkdensho_passwd,
	tkdensho_upload,
	{
		0x10,0x11,0x12,0x13,	// range 1
		0x24,0x25,0x26,0x27,	// range 2
		0x38,0x39,0x3a,0x3b,	// range 3
		0x4c,0x4d,0x4e,0x4f,	// range 4
		0x00,					// trailer
	},
	{ 0xbf, 0xfa, 0x21, 0x5d }
};


static void tecmosys_prot_reset(running_machine &machine)
{
	tecmosys_state *state = machine.driver_data<tecmosys_state>();
	state->device_read_ptr = 0;
	state->device_status = DS_IDLE;
	state->device_value = 0xff;
}

void tecmosys_prot_init(running_machine &machine, int which)
{
	tecmosys_state *state = machine.driver_data<tecmosys_state>();
	switch (which)
	{
	case 0:	state->device_data = &deroon_data; break;
	case 1: state->device_data = &tkdensho_data; break;
	case 2: state->device_data = &tkdensha_data; break;
	}

	machine.add_notifier(MACHINE_NOTIFY_RESET, tecmosys_prot_reset);
}

READ16_HANDLER(tecmosys_prot_status_r)
{
	if (ACCESSING_BITS_8_15)
	{
		// Bit 7: 0 = ready to write
		// Bit 6: 0 = ready to read
		return 0;
	}

	return 0xc0; // simulation is always ready
}

WRITE16_HANDLER(tecmosys_prot_status_w)
{
	// deroon clears the status in one place.
}


READ16_HANDLER(tecmosys_prot_data_r)
{
	tecmosys_state *state = space->machine().driver_data<tecmosys_state>();
	// prot appears to be read-ready for two consecutive reads
	// but returns 0xff for subsequent reads.
	UINT8 ret = state->device_value;
	state->device_value = 0xff;
	//logerror("- prot_r = 0x%02x\n", ret );
	return ret << 8;
}


WRITE16_HANDLER(tecmosys_prot_data_w)
{
	tecmosys_state *state = space->machine().driver_data<tecmosys_state>();
	// Only LSB
	data >>= 8;

	//logerror("+ prot_w( 0x%02x )\n", data );

	switch( state->device_status )
	{
		case DS_IDLE:
			if( data == 0x13 )
			{
				state->device_status = DS_LOGIN;
				state->device_value = state->device_data->passwd_len;
				state->device_read_ptr = 0;
				break;
			}
			break;

		case DS_LOGIN:
			if( state->device_read_ptr >= state->device_data->passwd_len)
			{
				state->device_status = DS_SEND_CODE;
				state->device_value = state->device_data->code[0];
				state->device_read_ptr = 1;
			}
			else
				state->device_value = state->device_data->passwd[state->device_read_ptr++] == data ? 0 : 0xff;
			break;

		case DS_SEND_CODE:
			if( state->device_read_ptr >= state->device_data->code[0]+2 ) // + code_len + trailer
			{
				state->device_status = DS_SEND_ADRS;
				state->device_value = state->device_data->checksum_ranges[0];
				state->device_read_ptr = 1;
			}
			else
				state->device_value = data == state->device_data->code[state->device_read_ptr-1] ? state->device_data->code[state->device_read_ptr++] : 0xff;
			break;

		case DS_SEND_ADRS:
			if( state->device_read_ptr >= 16+1 ) //+ trailer
			{
				state->device_status = DS_SEND_CHKSUMS;
				state->device_value = 0;
				state->device_read_ptr = 0;
			}
			else
			{
				state->device_value = data == state->device_data->checksum_ranges[state->device_read_ptr-1] ? state->device_data->checksum_ranges[state->device_read_ptr++] : 0xff;
			}
			break;

		case DS_SEND_CHKSUMS:
			if( state->device_read_ptr >= 5 )
			{
				state->device_status = DS_DONE;
				state->device_value = 0;
			}
			else
				state->device_value = data == state->device_data->checksums[state->device_read_ptr] ? state->device_data->checksums[state->device_read_ptr++] : 0xff;
			break;

		case DS_DONE:
			switch( data )
			{
				case 0xff: // trigger
				case 0x00: // checksum1[val] tkdensho
				case 0x20: // checksum1[val] deroon \ This is active RAM, so there may be more cases
				case 0x01: // checksum1[val] deroon / that can be ignored
					break;

				default:
					logerror( "Protection still in use??? w=%02x\n", data );
					break;
			}
			break;
	}
}
