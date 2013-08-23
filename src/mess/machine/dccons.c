/*

    dc.c - Sega Dreamcast hardware

    MESS (DC home console) hardware overrides (GD-ROM drive etc)

    c230048 - 5 is written, want 6
    c0d9d9e - where bad happens, from routine @ c0da260

    c0d9d8e - R0 on return is the value to put in

    cfffee0 - stack location when bad happens

    TODO:
    - gdrom_alt_status is identical to normal status except that "but it does not clear DMA status information when it is accessed"

*/

#include "emu.h"
#include "cdrom.h"
#include "debugger.h"
#include "includes/dc.h"
#include "cpu/sh4/sh4.h"
#include "sound/aica.h"
#include "includes/dccons.h"

#define ATAPI_CYCLES_PER_SECTOR (5000)  // TBD for Dreamcast

#define ATAPI_STAT_BSY     0x80
#define ATAPI_STAT_DRDY    0x40
#define ATAPI_STAT_DMARDDF 0x20
#define ATAPI_STAT_SERVDSC 0x10
#define ATAPI_STAT_DRQ     0x08
#define ATAPI_STAT_CORR    0x04
#define ATAPI_STAT_CHECK   0x01

#define ATAPI_ERR_ABORT 0x04

#define ATAPI_INTREASON_COMMAND 0x01
#define ATAPI_INTREASON_IO      0x02
#define ATAPI_INTREASON_RELEASE 0x04

#define ATAPI_REG_DATA      0
#define ATAPI_REG_FEATURES  1
#define ATAPI_REG_INTREASON 2
#define ATAPI_REG_SAMTAG    3
#define ATAPI_REG_COUNTLOW  4
#define ATAPI_REG_COUNTHIGH 5
#define ATAPI_REG_DRIVESEL  6
#define ATAPI_REG_CMDSTATUS 7
#define ATAPI_REG_ERROR     16  // read-only ERROR (write is FEATURES)

#define GDROM_BUSY_STATE 	0x00
#define GDROM_PAUSE_STATE   0x01
#define GDROM_STANDBY_STATE 0x02
#define GDROM_PLAY_STATE    0x03
#define GDROM_SEEK_STATE    0x04
#define GDROM_SCAN_STATE	0x05
#define GDROM_OPEN_STATE    0x06
#define GDROM_NODISC_STATE  0x07
#define GDROM_RETRY_STATE   0x08
#define GDROM_ERROR_STATE   0x09

#define ATAPI_REG_MAX 24

#define ATAPI_XFER_PIO          0x00
#define ATAPI_XFER_PIO_FLOW     0x08
#define ATAPI_XFER_SINGLE_DMA   0x10
#define ATAPI_XFER_MULTI_DMA    0x20
#define ATAPI_XFER_ULTRA_DMA    0x40

#define ATAPI_DATA_SIZE ( 64 * 1024 )

#define MAX_TRANSFER_SIZE ( 63488 )

void dc_cons_state::gdrom_set_status(UINT8 flag,bool state)
{
	if(flag & ATAPI_STAT_DRQ)
		printf("DRQ %02x\n",state);

	if(state)
		atapi_regs[ATAPI_REG_CMDSTATUS] |= flag;
	else
		atapi_regs[ATAPI_REG_CMDSTATUS] &= ~flag;
}

void dc_cons_state::gdrom_set_error(UINT8 flag,bool state)
{
	if(state)
		atapi_regs[ATAPI_REG_ERROR] |= flag;
	else
		atapi_regs[ATAPI_REG_ERROR] &= ~flag;
}


void dc_cons_state::gdrom_raise_irq()
{
	dc_sysctrl_regs[SB_ISTEXT] |= IST_EXT_GDROM;
	dc_update_interrupt_status();
}

void dc_cons_state::atapi_cmd_nop()
{
	gdrom_set_status(ATAPI_STAT_BSY,false);
	gdrom_set_status(ATAPI_STAT_DRDY,true);

	gdrom_set_status(ATAPI_STAT_CHECK,true);
	gdrom_set_error(ATAPI_ERR_ABORT,true);

	atapi_regs[ATAPI_REG_INTREASON] = ATAPI_INTREASON_COMMAND;
	gdrom_raise_irq();
}

void dc_cons_state::atapi_cmd_packet()
{
	atapi_data_ptr = 0;
	atapi_data_len = 0;

	/* we have no data */
	atapi_xferlen = 0;
	atapi_xfermod = 0;
	atapi_cdata_wait = 0;

	gdrom_set_status(ATAPI_STAT_BSY,false);
	gdrom_set_status(ATAPI_STAT_DRQ,true);
	atapi_regs[ATAPI_REG_INTREASON] = ATAPI_INTREASON_COMMAND;
	/* TODO: raise irq? */
}

void dc_cons_state::atapi_cmd_identify_packet()
{
	atapi_data_ptr = 0;
	atapi_data_len = 512;
	/* we have no data */
	atapi_xferlen = 0;
	atapi_xfermod = 0;

	memset( atapi_data, 0, atapi_data_len );

	atapi_data[ 0 ^ 1 ] = 0x86; // ATAPI device, cmd set 6 compliant, DRQ within 3 ms of PACKET command
	atapi_data[ 1 ^ 1 ] = 0x00;

	memset( &atapi_data[ 46 ], ' ', 8 );
	atapi_data[ 46 ^ 1 ] = 'S';
	atapi_data[ 47 ^ 1 ] = 'E';

	memset( &atapi_data[ 54 ], ' ', 40 );
	atapi_data[ 54 ^ 1 ] = 'C';
	atapi_data[ 55 ^ 1 ] = 'D';
	atapi_data[ 56 ^ 1 ] = '-';
	atapi_data[ 57 ^ 1 ] = 'R';
	atapi_data[ 58 ^ 1 ] = 'O';
	atapi_data[ 59 ^ 1 ] = 'M';
	atapi_data[ 60 ^ 1 ] = ' ';
	atapi_data[ 61 ^ 1 ] = 'D';
	atapi_data[ 62 ^ 1 ] = 'R';
	atapi_data[ 63 ^ 1 ] = 'I';
	atapi_data[ 64 ^ 1 ] = 'V';
	atapi_data[ 65 ^ 1 ] = 'E';
	atapi_data[ 66 ^ 1 ] = ' ';
	atapi_data[ 67 ^ 1 ] = ' ';
	atapi_data[ 68 ^ 1 ] = ' ';
	atapi_data[ 69 ^ 1 ] = ' ';
	atapi_data[ 70 ^ 1 ] = '6';
	atapi_data[ 71 ^ 1 ] = '.';
	atapi_data[ 72 ^ 1 ] = '4';
	atapi_data[ 73 ^ 1 ] = '2';

	atapi_data[ 98 ^ 1 ] = 0x04; // IORDY may be disabled
	atapi_data[ 99 ^ 1 ] = 0x00;

	atapi_regs[ATAPI_REG_COUNTLOW] = 0;
	atapi_regs[ATAPI_REG_COUNTHIGH] = 2;

	gdrom_set_status(ATAPI_STAT_BSY,false);
	gdrom_set_status(ATAPI_STAT_DRQ,true);
	atapi_regs[ATAPI_REG_INTREASON] = ATAPI_INTREASON_IO; /* ok? */
	gdrom_raise_irq();
}

void dc_cons_state::atapi_cmd_set_features()
{
	//TODO: error ABORT flag clear

	// set xfer mode?
	if (atapi_regs[ATAPI_REG_FEATURES] == 0x03)
	{
		printf("Set transfer mode to %x\n", atapi_regs[ATAPI_REG_COUNTLOW] & 0xf8);
		xfer_mode = atapi_regs[ATAPI_REG_COUNTLOW] & 0xf8;
	}
	else
	{
		printf("ATAPI: Unknown set features %x\n", atapi_regs[ATAPI_REG_FEATURES]);
	}

	gdrom_set_status(ATAPI_STAT_BSY,false);
	gdrom_set_error(ATAPI_ERR_ABORT,false);
	gdrom_set_status(ATAPI_STAT_SERVDSC,false);
	gdrom_set_status(ATAPI_STAT_DMARDDF,false);
	gdrom_set_status(ATAPI_STAT_CHECK,false);
	gdrom_set_status(ATAPI_STAT_DRDY,true);

	atapi_data_ptr = 0;
	atapi_data_len = 0;

	gdrom_raise_irq();
}

TIMER_CALLBACK_MEMBER(dc_cons_state::atapi_cmd_exec )
{
	atapi_cmd_timer->adjust(attotime::never);

	gdrom_set_status(ATAPI_STAT_CHECK,false);
	gdrom_set_error(ATAPI_ERR_ABORT,false);

	switch (cur_atapi_cmd)
	{
		case 0x00:
			atapi_cmd_nop();
			break;

		case 0xa0:  // PACKET
			atapi_cmd_packet();
			break;

		case 0xa1:  // IDENTIFY PACKET DEVICE
			atapi_cmd_identify_packet();
			break;

		case 0xef:  // SET FEATURES
			atapi_cmd_set_features();
			break;

		default:
			mame_printf_debug("ATAPI: Unknown IDE command %x\n", cur_atapi_cmd);
			break;
	}
}

TIMER_CALLBACK_MEMBER(dc_cons_state::atapi_xfer_end )
{
	UINT8 sector_buffer[ 4096 ];

	atapi_timer->adjust(attotime::never);

	printf("atapi_xfer_end atapi_xferlen = %d, atapi_xfermod=%d\n", atapi_xfermod, atapi_xferlen );

	mame_printf_debug("ATAPI: xfer_end.  xferlen = %d, atapi_xfermod = %d\n", atapi_xferlen, atapi_xfermod);

	while (atapi_xferlen > 0 )
	{
		struct sh4_ddt_dma ddtdata;

		// get a sector from the SCSI device
		gdrom->ReadData( sector_buffer, 2048 );

		atapi_xferlen -= 2048;

		// perform the DMA
		ddtdata.destination = atapi_xferbase;   // destination address
		ddtdata.length = 2048/4;
		ddtdata.size = 4;
		ddtdata.buffer = sector_buffer;
		ddtdata.direction=1;    // 0 source to buffer, 1 buffer to destination
		ddtdata.channel= -1;    // not used
		ddtdata.mode= -1;       // copy from/to buffer
		printf("ATAPI: DMA one sector to %x, %x remaining\n", atapi_xferbase, atapi_xferlen);
		sh4_dma_ddt(m_maincpu, &ddtdata);

		atapi_xferbase += 2048;
	}

	if (atapi_xfermod > MAX_TRANSFER_SIZE)
	{
		atapi_xferlen = MAX_TRANSFER_SIZE;
		atapi_xfermod = atapi_xfermod - MAX_TRANSFER_SIZE;
	}
	else
	{
		atapi_xferlen = atapi_xfermod;
		atapi_xfermod = 0;
	}

	if (atapi_xferlen > 0)
	{
		printf("ATAPI: starting next piece of multi-part transfer\n");
		atapi_regs[ATAPI_REG_COUNTLOW] = atapi_xferlen & 0xff;
		atapi_regs[ATAPI_REG_COUNTHIGH] = (atapi_xferlen>>8)&0xff;

		atapi_timer->adjust(m_maincpu->cycles_to_attotime((ATAPI_CYCLES_PER_SECTOR * (atapi_xferlen/2048))));
	}
	else
	{
		printf("ATAPI: Transfer completed, dropping DRQ\n");
		gdrom_set_status(ATAPI_STAT_DRDY,true);
		gdrom_set_status(ATAPI_STAT_DRQ,false);
		gdrom_set_status(ATAPI_STAT_BSY,false);
		atapi_regs[ATAPI_REG_INTREASON] = ATAPI_INTREASON_IO | ATAPI_INTREASON_COMMAND;
		atapi_regs[ATAPI_REG_SAMTAG] = GDROM_PAUSE_STATE | 0x80;

		g1bus_regs[SB_GDST]=0;
		dc_sysctrl_regs[SB_ISTNRM] |= IST_DMA_GDROM;
		dc_update_interrupt_status();
	}

	gdrom_raise_irq();

	printf( "atapi_xfer_end: %d %d\n", atapi_xferlen, atapi_xfermod );
}

void dc_cons_state::dreamcast_atapi_init()
{
	xfer_mode = ATAPI_XFER_PIO;

	atapi_regs = auto_alloc_array_clear(machine(), UINT8, ATAPI_REG_MAX);

	atapi_regs[ATAPI_REG_CMDSTATUS] = 0;
	atapi_regs[ATAPI_REG_ERROR] = 1;
	atapi_regs[ATAPI_REG_COUNTLOW] = 0x14;
	atapi_regs[ATAPI_REG_COUNTHIGH] = 0xeb;

	atapi_data_ptr = 0;
	atapi_data_len = 0;
	atapi_cdata_wait = 0;

	atapi_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(dc_cons_state::atapi_xfer_end),this));
	atapi_timer->adjust(attotime::never);
	atapi_cmd_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(dc_cons_state::atapi_cmd_exec),this));
	atapi_cmd_timer->adjust(attotime::never);

	gdrom = NULL;

	atapi_data = auto_alloc_array(machine(), UINT8,  ATAPI_DATA_SIZE );

	save_pointer(NAME(atapi_regs), ATAPI_REG_MAX );
	save_pointer(NAME(atapi_data), ATAPI_DATA_SIZE / 2 );
	save_item(NAME(atapi_data_ptr));
	save_item(NAME(atapi_data_len));
	save_item(NAME(atapi_xferlen));
	save_item(NAME(atapi_xferbase));
	save_item(NAME(atapi_cdata_wait));
	save_item(NAME(atapi_xfermod));

	gdrom = machine().device<gdrom_device>( "cdrom" );
}

void dc_cons_state::dreamcast_atapi_reset()
{
	atapi_regs[ATAPI_REG_CMDSTATUS] = 0;
	atapi_regs[ATAPI_REG_ERROR] = 1;
	atapi_regs[ATAPI_REG_COUNTLOW] = 0x14;
	atapi_regs[ATAPI_REG_COUNTHIGH] = 0xeb;

	atapi_data_ptr = 0;
	atapi_data_len = 0;
	atapi_cdata_wait = 0;

	atapi_xferlen = 0;
	atapi_xfermod = 0;
}

/*

 GDROM regsters:

 5f7018: alternate status/device control
 5f7080: data
 5f7084: error/features
 5f7088: interrupt reason/sector count
 5f708c: sector number
 5f7090: byte control low
 5f7094: byte control high
 5f7098: drive select
 5f709c: status/command

c002910 - ATAPI packet writes
c002796 - aux status read after that
c000776 - DMA triggered to c008000

*/

READ32_MEMBER(dc_cons_state::dc_mess_gdrom_r)
{
//  printf("gdrom_r: @ %x (off %x), mask %llx (PC %x)\n", offset, off, mem_mask, space.device().safe_pc());

	switch(offset)
	{
		case 0x18/4:
			return atapi_regs[ATAPI_REG_CMDSTATUS] | 0x10;
		case 0x80/4:
			UINT32 data;
			if (atapi_data_ptr == 0 && atapi_data_len == 0)
			{
				// get the data from the device
				if( atapi_xferlen > 0 )
				{
					gdrom->ReadData( atapi_data, atapi_xferlen );
					atapi_data_len = atapi_xferlen;
				}

				if (atapi_xfermod > MAX_TRANSFER_SIZE)
				{
					atapi_xferlen = MAX_TRANSFER_SIZE;
					atapi_xfermod = atapi_xfermod - MAX_TRANSFER_SIZE;
				}
				else
				{
					atapi_xferlen = atapi_xfermod;
					atapi_xfermod = 0;
				}

//			    printf( "atapi_r: atapi_xferlen=%d\n", atapi_xferlen );
				if( atapi_xferlen != 0 )
				{
					//atapi_regs[ATAPI_REG_CMDSTATUS] = ATAPI_STAT_DRQ | ATAPI_STAT_SERVDSC;
					//atapi_regs[ATAPI_REG_INTREASON] = ATAPI_INTREASON_IO;
				}
				else
				{
					printf("ATAPI: dropping DRQ\n");
					//gdrom_set_status(ATAPI_STAT_DRQ,false);
					//atapi_regs[ATAPI_REG_CMDSTATUS] = 0;
					//atapi_regs[ATAPI_REG_INTREASON] = ATAPI_INTREASON_IO;
				}

				atapi_regs[ATAPI_REG_COUNTLOW] = atapi_xferlen & 0xff;
				atapi_regs[ATAPI_REG_COUNTHIGH] = (atapi_xferlen>>8)&0xff;

				gdrom_raise_irq();
			}

			if( atapi_cdata_wait )
			{
				data = atapi_data[atapi_data_ptr++];
				data |= ( atapi_data[atapi_data_ptr++] << 8 );
				atapi_cdata_wait-=2;
				if( atapi_cdata_wait == 0 )
				{
//					printf( "atapi_r: read all bytes\n" );
					atapi_data_ptr = 0;
					atapi_data_len = 0;

					if( atapi_xferlen == 0 )
					{
						printf("Read from SCSI\n");
						//debugger_break(machine());
						gdrom_set_status(ATAPI_STAT_DRQ,false);
						atapi_regs[ATAPI_REG_INTREASON] = ATAPI_INTREASON_IO;
						gdrom_raise_irq();
					}
				}
			}
			else
			{
				data = 0;
				printf("Read from empty SCSI queue\n");
			}

			return data;
		case 0x84/4:
			return atapi_regs[ATAPI_REG_ERROR];
		case 0x88/4:
			return atapi_regs[ATAPI_REG_INTREASON];
		case 0x8c/4:
			return atapi_regs[ATAPI_REG_SAMTAG];
		case 0x90/4:
			return atapi_regs[ATAPI_REG_COUNTLOW];
		case 0x94/4:
			return atapi_regs[ATAPI_REG_COUNTHIGH];
		case 0x98/4:
			return atapi_regs[ATAPI_REG_DRIVESEL];
		case 0x9c/4:
			dc_sysctrl_regs[SB_ISTEXT] &= ~IST_EXT_GDROM;
			dc_update_interrupt_status();
			return atapi_regs[ATAPI_REG_CMDSTATUS] | 0x10;
	}

	return 0;
}

WRITE32_MEMBER(dc_cons_state::dc_mess_gdrom_w )
{
	switch(offset)
	{
		case 0x18/4:
			/* Device Control */
			//COMBINE_DATA(&atapi_regs[ATAPI_REG_CMDSTATUS]);
			return;
		/* TODO: move this behind a timer */
		case 0x80/4:
		{
//      	printf("atapi_w: data=%04x\n", data );

//      	printf("ATAPI: packet write %04x\n", data);
			atapi_data[atapi_data_ptr++] = data & 0xff;
			atapi_data[atapi_data_ptr++] = data >> 8;

		//printf("%02x %02x %d\n",data & 0xff, data >> 8,atapi_data_ptr);

			if (atapi_cdata_wait)
			{
//      	    printf("ATAPI: waiting, ptr %d wait %d\n", atapi_data_ptr, atapi_cdata_wait);
				if (atapi_data_ptr == atapi_cdata_wait)
				{
					// send it to the device
					gdrom->WriteData( atapi_data, atapi_cdata_wait );

					// assert IRQ
					gdrom_raise_irq();

					// not sure here, but clear DRQ at least?
					gdrom_set_status(ATAPI_STAT_DRQ,false);
					printf("cdata wait status\n");
					atapi_cdata_wait = 0;
				}
			}
			else if ( atapi_data_ptr == 12 )
			{
				int phase;

//      	    printf("atapi_w: command %02x\n", atapi_data[0]&0xff );

				// reset data pointer for reading SCSI results
				atapi_data_ptr = 0;
				atapi_data_len = 0;

				// send it to the SCSI device
				gdrom->SetCommand( atapi_data, 12 );
				gdrom->ExecCommand( &atapi_xferlen );
				gdrom->GetPhase( &phase );

				if (atapi_xferlen != -1)
				{
					printf("ATAPI: SCSI command %02x returned %d bytes from the device\n", atapi_data[0]&0xff, atapi_xferlen);

					// store the returned command length in the ATAPI regs, splitting into
					// multiple transfers if necessary
					atapi_xfermod = 0;
					if (atapi_xferlen > MAX_TRANSFER_SIZE)
					{
						atapi_xfermod = atapi_xferlen - MAX_TRANSFER_SIZE;
						atapi_xferlen = MAX_TRANSFER_SIZE;
					}

					atapi_regs[ATAPI_REG_COUNTLOW] = atapi_xferlen & 0xff;
					atapi_regs[ATAPI_REG_COUNTHIGH] = (atapi_xferlen>>8)&0xff;

					if (atapi_xferlen == 0)
					{
						// if no data to return, set the registers properly
						//atapi_regs[ATAPI_REG_CMDSTATUS] = ATAPI_STAT_DRDY;
						gdrom_set_status(ATAPI_STAT_DRDY,true);
						gdrom_set_status(ATAPI_STAT_DRQ,false);
						atapi_regs[ATAPI_REG_INTREASON] = ATAPI_INTREASON_IO|ATAPI_INTREASON_COMMAND;
					}
					else
					{
						printf("ATAPI features %02x\n",atapi_regs[ATAPI_REG_FEATURES]);
						// indicate data ready: set DRQ and DMA ready, and IO in INTREASON
						if (atapi_regs[ATAPI_REG_FEATURES] & 0x01)  // DMA feature
						{
							//gdrom_set_status(ATAPI_STAT_BSY | ATAPI_STAT_DRDY | ATAPI_STAT_SERVDSC,true);
							//atapi_regs[ATAPI_REG_CMDSTATUS] = ATAPI_STAT_BSY | ATAPI_STAT_DRDY | ATAPI_STAT_SERVDSC;
						}
						else
						{
							//gdrom_set_status(ATAPI_STAT_SERVDSC,true);
							/* Ok? */
							gdrom_set_status(ATAPI_STAT_DRQ,true);
							//atapi_regs[ATAPI_REG_CMDSTATUS] = ATAPI_STAT_DRQ | ATAPI_STAT_SERVDSC | ATAPI_STAT_DRQ;
						}
						atapi_regs[ATAPI_REG_INTREASON] = ATAPI_INTREASON_IO;
					}

					switch( phase )
					{
					case SCSI_PHASE_DATAOUT:
					case SCSI_PHASE_DATAIN:
						atapi_cdata_wait = atapi_xferlen;
						break;
					}

					// perform special ATAPI processing of certain commands
					switch (atapi_data[0]&0xff)
					{
						case 0x00: // BUS RESET / TEST UNIT READY
						case 0xbb: // SET CDROM SPEED
							//atapi_regs[ATAPI_REG_CMDSTATUS] = 0;
							break;

						case 0x45: // PLAY
							gdrom_set_status(ATAPI_STAT_BSY,true);
							atapi_timer->adjust( downcast<cpu_device *>(&space.device())->cycles_to_attotime(ATAPI_CYCLES_PER_SECTOR ) );
							break;
					}

					// assert IRQ
					gdrom_raise_irq();
				}
				else
				{
					printf("ATAPI: SCSI device returned error!\n");

					//atapi_regs[ATAPI_REG_CMDSTATUS] = ATAPI_STAT_DRQ | ATAPI_STAT_CHECK;
					//atapi_regs[ATAPI_REG_ERROR] = 0x50; // sense key = ILLEGAL REQUEST
					atapi_regs[ATAPI_REG_COUNTLOW] = 0;
					atapi_regs[ATAPI_REG_COUNTHIGH] = 0;
				}
			}
		}
		return;
		case 0x84/4:
			COMBINE_DATA(&atapi_regs[ATAPI_REG_FEATURES]);
			return;
		case 0x88/4:
			COMBINE_DATA(&atapi_regs[ATAPI_REG_INTREASON]);
			return;
		case 0x8c/4:
			COMBINE_DATA(&atapi_regs[ATAPI_REG_SAMTAG]);
			return;
		case 0x90/4:
			COMBINE_DATA(&atapi_regs[ATAPI_REG_COUNTLOW]);
			return;
		case 0x94/4:
			COMBINE_DATA(&atapi_regs[ATAPI_REG_COUNTHIGH]);
			return;
		case 0x98/4:
			COMBINE_DATA(&atapi_regs[ATAPI_REG_DRIVESEL]);
			return;
		case 0x9c/4:
			{
				printf("ATAPI command %x issued! (PC=%x)\n", data, space.device().safe_pc());

				gdrom_set_status(ATAPI_STAT_BSY,true);
				gdrom_set_status(ATAPI_STAT_DRQ,false);
				gdrom_set_status(ATAPI_STAT_DRDY,false);
				cur_atapi_cmd = data;
				/* TODO: timing of this */
				atapi_cmd_timer->adjust(m_maincpu->cycles_to_attotime(ATAPI_CYCLES_PER_SECTOR));
			}
			return;
	}

}

// register decode helpers

// this accepts only 32-bit accesses
int dc_cons_state::decode_reg32_64( UINT32 offset, UINT64 mem_mask, UINT64 *shift)
{
	int reg = offset * 2;

	*shift = 0;

	// non 32-bit accesses have not yet been seen here, we need to know when they are
	if ((mem_mask != U64(0xffffffff00000000)) && (mem_mask != U64(0x00000000ffffffff)))
	{
		mame_printf_verbose("%s:Wrong mask!\n", machine().describe_context());
//      debugger_break(machine);
	}

	if (mem_mask == U64(0xffffffff00000000))
	{
		reg++;
		*shift = 32;
	}

	return reg;
}

READ32_MEMBER(dc_cons_state::dc_mess_g1_ctrl_r )
{
	switch(offset)
	{
		case SB_GDST:
			break;
		case SB_GDLEND:
			//debugger_break(machine());
			return atapi_xferlen; // TODO: check me
		default:
			printf("G1CTRL:  Unmapped read %08x\n", 0x5f7400+offset*4);
			debugger_break(machine());
	}
	return g1bus_regs[offset];
}

WRITE32_MEMBER(dc_cons_state::dc_mess_g1_ctrl_w )
{
	g1bus_regs[offset] = data; // 5f7400+reg*4=dat
//  mame_printf_verbose("G1CTRL: [%08x=%x] write %" I64FMT "x to %x, mask %" I64FMT "x\n", 0x5f7400+reg*4, dat, data, offset, mem_mask);
	switch (offset)
	{
	case SB_GDST:
		if (data & 1 && g1bus_regs[SB_GDEN] == 1) // 0 -> 1
		{
			if (g1bus_regs[SB_GDDIR] == 0)
			{
				printf("G1CTRL: unsupported transfer\n");
				return;
			}

			atapi_xferbase = g1bus_regs[SB_GDSTAR];
			atapi_timer->adjust(m_maincpu->cycles_to_attotime((ATAPI_CYCLES_PER_SECTOR * (atapi_xferlen/2048))));
			atapi_regs[ATAPI_REG_SAMTAG] = GDROM_PAUSE_STATE | 0x80;
		}
		break;
	}
}
