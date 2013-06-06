/***************************************************************************

    Generic (PC-style) IDE controller implementation

***************************************************************************/

#include "emu.h"
#include "idectrl.h"
#include "debugger.h"

/***************************************************************************
    DEBUGGING
***************************************************************************/

#define VERBOSE                     0
#define PRINTF_IDE_COMMANDS         0
#define PRINTF_IDE_PASSWORD         0

#define LOG(x)  do { if (VERBOSE) logerror x; } while (0)

#define LOGPRINT(x) do { if (VERBOSE) logerror x; if (PRINTF_IDE_COMMANDS) mame_printf_debug x; } while (0)



/***************************************************************************
    CONSTANTS
***************************************************************************/

#define MINIMUM_COMMAND_TIME                (attotime::from_usec(10))

#define TIME_PER_SECTOR                     (attotime::from_usec(100))
#define TIME_PER_ROTATION                   (attotime::from_hz(5400/60))
#define TIME_SECURITY_ERROR                 (attotime::from_msec(1000))

#define TIME_SEEK_MULTISECTOR               (attotime::from_msec(13))
#define TIME_NO_SEEK_MULTISECTOR            (attotime::from_nsec(16300))

#define IDE_STATUS_ERROR                    0x01
#define IDE_STATUS_HIT_INDEX                0x02
#define IDE_STATUS_BUFFER_READY             0x08
#define IDE_STATUS_SEEK_COMPLETE            0x10
#define IDE_STATUS_DRIVE_READY              0x40
#define IDE_STATUS_BUSY                     0x80

#define BANK(b, v) (((v) << 4) | (b))

#define IDE_BANK0_DATA                      BANK(0, 0)
#define IDE_BANK0_ERROR                     BANK(0, 1)
#define IDE_BANK0_SECTOR_COUNT              BANK(0, 2)
#define IDE_BANK0_SECTOR_NUMBER             BANK(0, 3)
#define IDE_BANK0_CYLINDER_LSB              BANK(0, 4)
#define IDE_BANK0_CYLINDER_MSB              BANK(0, 5)
#define IDE_BANK0_HEAD_NUMBER               BANK(0, 6)
#define IDE_BANK0_STATUS_COMMAND            BANK(0, 7)

#define IDE_BANK1_STATUS_CONTROL            BANK(1, 6)

#define IDE_BANK2_CONFIG_UNK                BANK(2, 4)
#define IDE_BANK2_CONFIG_REGISTER           BANK(2, 8)
#define IDE_BANK2_CONFIG_DATA               BANK(2, 0xc)

#define IDE_COMMAND_READ_MULTIPLE           0x20
#define IDE_COMMAND_READ_MULTIPLE_NORETRY   0x21
#define IDE_COMMAND_WRITE_MULTIPLE          0x30
#define IDE_COMMAND_WRITE_MULTIPLE_NORETRY  0x31
#define IDE_COMMAND_DIAGNOSTIC              0x90
#define IDE_COMMAND_SET_CONFIG              0x91
#define IDE_COMMAND_READ_MULTIPLE_BLOCK     0xc4
#define IDE_COMMAND_WRITE_MULTIPLE_BLOCK    0xc5
#define IDE_COMMAND_SET_BLOCK_COUNT         0xc6
#define IDE_COMMAND_READ_DMA                0xc8
#define IDE_COMMAND_WRITE_DMA               0xca
#define IDE_COMMAND_GET_INFO                0xec
#define IDE_COMMAND_SET_FEATURES            0xef
#define IDE_COMMAND_SECURITY_UNLOCK         0xf2
#define IDE_COMMAND_UNKNOWN_F9              0xf9
#define IDE_COMMAND_VERIFY_MULTIPLE         0x40
#define IDE_COMMAND_VERIFY_NORETRY          0x41
#define IDE_COMMAND_ATAPI_IDENTIFY          0xa1
#define IDE_COMMAND_RECALIBRATE             0x10
#define IDE_COMMAND_SEEK                    0x70
#define IDE_COMMAND_IDLE_IMMEDIATE          0xe1
#define IDE_COMMAND_IDLE                    0xe3
#define IDE_COMMAND_TAITO_GNET_UNLOCK_1     0xfe
#define IDE_COMMAND_TAITO_GNET_UNLOCK_2     0xfc
#define IDE_COMMAND_TAITO_GNET_UNLOCK_3     0x0f

#define IDE_ERROR_NONE                      0x00
#define IDE_ERROR_DEFAULT                   0x01
#define IDE_ERROR_TRACK0_NOT_FOUND          0x02
#define IDE_ERROR_UNKNOWN_COMMAND           0x04
#define IDE_ERROR_BAD_LOCATION              0x10
#define IDE_ERROR_BAD_SECTOR                0x80

#define IDE_BUSMASTER_STATUS_ACTIVE         0x01
#define IDE_BUSMASTER_STATUS_ERROR          0x02
#define IDE_BUSMASTER_STATUS_IRQ            0x04


void ide_controller_device::signal_interrupt()
{
	LOG(("IDE interrupt assert\n"));

	/* signal an interrupt */
	m_irq_handler(ASSERT_LINE);
	interrupt_pending = 1;
	bus_master_status |= IDE_BUSMASTER_STATUS_IRQ;
}


void ide_controller_device::clear_interrupt()
{
	LOG(("IDE interrupt clear\n"));

	/* clear an interrupt */
	m_irq_handler(CLEAR_LINE);
	interrupt_pending = 0;
}



/***************************************************************************
    DELAYED INTERRUPT HANDLING
***************************************************************************/

static TIMER_CALLBACK( delayed_interrupt )
{
	ide_controller_device *ide = (ide_controller_device *)ptr;
	ide->status &= ~IDE_STATUS_BUSY;
	ide->signal_interrupt();
}


static TIMER_CALLBACK( delayed_interrupt_buffer_ready )
{
	ide_controller_device *ide = (ide_controller_device *)ptr;
	ide->status &= ~IDE_STATUS_BUSY;
	ide->status |= IDE_STATUS_BUFFER_READY;
	ide->signal_interrupt();
}


void ide_controller_device::signal_delayed_interrupt(attotime time, int buffer_ready)
{
	/* clear buffer ready and set the busy flag */
	status &= ~IDE_STATUS_BUFFER_READY;
	status |= IDE_STATUS_BUSY;

	/* set a timer */
	if (buffer_ready)
		machine().scheduler().timer_set(time, FUNC(delayed_interrupt_buffer_ready), 0, this);
	else
		machine().scheduler().timer_set(time, FUNC(delayed_interrupt), 0, this);
}



/***************************************************************************
    INITIALIZATION AND RESET
***************************************************************************/

UINT8 *ide_controller_device::ide_get_features(int _drive)
{
	return slot[_drive]->dev()->get_features();
}

void ide_controller_device::ide_set_gnet_readlock(const UINT8 onoff)
{
	gnetreadlock = onoff;
}

void ide_controller_device::ide_set_master_password(const UINT8 *password)
{
	master_password = password;
	master_password_enable = (master_password != NULL);
}


void ide_controller_device::ide_set_user_password(const UINT8 *password)
{
	user_password = password;
	user_password_enable = (user_password != NULL);
}


static TIMER_CALLBACK( reset_callback )
{
	reinterpret_cast<device_t *>(ptr)->reset();
}



/*************************************
 *
 *  Convert offset/mem_mask to offset
 *  and size
 *
 *************************************/

INLINE int convert_to_offset_and_size32(offs_t *offset, UINT32 mem_mask)
{
	int size = 4;

	/* determine which real offset */
	if (!ACCESSING_BITS_0_7)
	{
		(*offset)++, size = 3;
		if (!ACCESSING_BITS_8_15)
		{
			(*offset)++, size = 2;
			if (!ACCESSING_BITS_16_23)
				(*offset)++, size = 1;
		}
	}

	/* determine the real size */
	if (ACCESSING_BITS_24_31)
		return size;
	size--;
	if (ACCESSING_BITS_16_23)
		return size;
	size--;
	if (ACCESSING_BITS_8_15)
		return size;
	size--;
	return size;
}

INLINE int convert_to_offset_and_size16(offs_t *offset, UINT32 mem_mask)
{
	int size = 2;

	/* determine which real offset */
	if (!ACCESSING_BITS_0_7)
		(*offset)++, size = 1;

	if (ACCESSING_BITS_8_15)
		return size;
	size--;
	return size;
}



/*************************************
 *
 *  Advance to the next sector
 *
 *************************************/

void ide_controller_device::next_sector()
{
	/* LBA direct? */
	ide_device_interface *dev = slot[cur_drive]->dev();
	if (dev->cur_head_reg & 0x40)
	{
		dev->cur_sector++;
		if (dev->cur_sector == 0)
		{
			dev->cur_cylinder++;
			if (dev->cur_cylinder == 0)
				dev->cur_head++;
		}
	}

	/* standard CHS */
	else
	{
		/* sectors are 1-based */
		dev->cur_sector++;
		if (dev->cur_sector > dev->get_sectors())
		{
			/* heads are 0 based */
			dev->cur_sector = 1;
			dev->cur_head++;
			if (dev->cur_head >= dev->get_heads())
			{
				dev->cur_head = 0;
				dev->cur_cylinder++;
			}
		}
	}

	dev->cur_lba = dev->lba_address();
}



/*************************************
 *
 *  security error handling
 *
 *************************************/

static TIMER_CALLBACK( security_error_done )
{
	ide_controller_device *ide = (ide_controller_device *)ptr;

	/* clear error state */
	ide->status &= ~IDE_STATUS_ERROR;
	ide->status |= IDE_STATUS_DRIVE_READY;
}

void ide_controller_device::security_error()
{
	/* set error state */
	status |= IDE_STATUS_ERROR;
	status &= ~IDE_STATUS_DRIVE_READY;

	/* just set a timer and mark ourselves error */
	machine().scheduler().timer_set(TIME_SECURITY_ERROR, FUNC(security_error_done), 0, this);
}



/*************************************
 *
 *  Sector reading
 *
 *************************************/

void ide_controller_device::continue_read()
{
	/* reset the totals */
	buffer_offset = 0;

	/* clear the buffer ready and busy flag */
	status &= ~IDE_STATUS_BUFFER_READY;
	status &= ~IDE_STATUS_BUSY;

	if (master_password_enable || user_password_enable)
	{
		security_error();

		sector_count = 0;
		bus_master_status &= ~IDE_BUSMASTER_STATUS_ACTIVE;
		dma_active = 0;

		return;
	}

	/* if there is more data to read, keep going */
	if (sector_count > 0)
		sector_count--;
	if (sector_count > 0)
		read_next_sector();
	else
	{
		bus_master_status &= ~IDE_BUSMASTER_STATUS_ACTIVE;
		dma_active = 0;
	}
}


void ide_controller_device::write_buffer_to_dma()
{
	int bytesleft = IDE_DISK_SECTOR_SIZE;
	UINT8 *data = buffer;

//  LOG(("Writing sector to %08X\n", dma_address));

	/* loop until we've consumed all bytes */
	while (bytesleft--)
	{
		/* if we're out of space, grab the next descriptor */
		if (dma_bytes_left == 0)
		{
			/* if we're out of buffer space, that's bad */
			if (dma_last_buffer)
			{
				LOG(("DMA Out of buffer space!\n"));
				return;
			}

			/* fetch the address */
			dma_address = dma_space->read_byte(dma_descriptor++ ^ dma_address_xor);
			dma_address |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 8;
			dma_address |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 16;
			dma_address |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 24;
			dma_address &= 0xfffffffe;

			/* fetch the length */
			dma_bytes_left = dma_space->read_byte(dma_descriptor++ ^ dma_address_xor);
			dma_bytes_left |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 8;
			dma_bytes_left |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 16;
			dma_bytes_left |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 24;
			dma_last_buffer = (dma_bytes_left >> 31) & 1;
			dma_bytes_left &= 0xfffe;
			if (dma_bytes_left == 0)
				dma_bytes_left = 0x10000;

//          LOG(("New DMA descriptor: address = %08X  bytes = %04X  last = %d\n", dma_address, dma_bytes_left, dma_last_buffer));
		}

		/* write the next byte */
		dma_space->write_byte(dma_address++, *data++);
		dma_bytes_left--;
	}
}


void ide_controller_device::read_sector_done()
{
	ide_device_interface *dev = slot[cur_drive]->dev();
	int lba = dev->lba_address(), count = 0;

	/* GNET readlock check */
	if (gnetreadlock) {
		status &= ~IDE_STATUS_ERROR;
		status &= ~IDE_STATUS_BUSY;
		return;
	}
	/* now do the read */
	count = dev->read_sector(lba, buffer);

	/* by default, mark the buffer ready and the seek complete */
	if (!verify_only)
		status |= IDE_STATUS_BUFFER_READY;
	status |= IDE_STATUS_SEEK_COMPLETE;

	/* and clear the busy and error flags */
	status &= ~IDE_STATUS_ERROR;
	status &= ~IDE_STATUS_BUSY;

	/* if we succeeded, advance to the next sector and set the nice bits */
	if (count == 1)
	{
		/* advance the pointers, unless this is the last sector */
		/* Gauntlet: Dark Legacy checks to make sure we stop on the last sector */
		if (sector_count != 1)
			next_sector();

		/* clear the error value */
		error = IDE_ERROR_NONE;

		/* signal an interrupt */
		if (!verify_only)
			sectors_until_int--;
		if (sectors_until_int == 0 || sector_count == 1)
		{
			sectors_until_int = ((command == IDE_COMMAND_READ_MULTIPLE_BLOCK) ? block_count : 1);
			signal_interrupt();
		}

		/* handle DMA */
		if (dma_active)
			write_buffer_to_dma();

		/* if we're just verifying or if we DMA'ed the data, we can read the next sector */
		if (verify_only || dma_active)
			continue_read();
	}

	/* if we got an error, we need to report it */
	else
	{
		/* set the error flag and the error */
		status |= IDE_STATUS_ERROR;
		error = IDE_ERROR_BAD_SECTOR;
		bus_master_status |= IDE_BUSMASTER_STATUS_ERROR;
		bus_master_status &= ~IDE_BUSMASTER_STATUS_ACTIVE;

		/* signal an interrupt */
		signal_interrupt();
	}
}


static TIMER_CALLBACK( read_sector_done_callback )
{
	ide_controller_device *ide = (ide_controller_device *)ptr;

	ide->read_sector_done();
}


void ide_controller_device::read_first_sector()
{
	ide_device_interface *dev = slot[cur_drive]->dev();
	/* mark ourselves busy */
	status |= IDE_STATUS_BUSY;

	/* just set a timer */
	if (command == IDE_COMMAND_READ_MULTIPLE_BLOCK)
	{
		int new_lba = dev->lba_address();
		attotime seek_time;

		if (new_lba == dev->cur_lba || new_lba == dev->cur_lba + 1)
			seek_time = TIME_NO_SEEK_MULTISECTOR;
		else
			seek_time = TIME_SEEK_MULTISECTOR;

		dev->cur_lba = new_lba;
		machine().scheduler().timer_set(seek_time, FUNC(read_sector_done_callback), 0, this);
	}
	else
		machine().scheduler().timer_set(TIME_PER_SECTOR, FUNC(read_sector_done_callback), 0, this);
}


void ide_controller_device::read_next_sector()
{
	/* mark ourselves busy */
	status |= IDE_STATUS_BUSY;

	if (command == IDE_COMMAND_READ_MULTIPLE_BLOCK)
	{
		if (sectors_until_int != 1)
			/* make ready now */
			read_sector_done();
		else
			/* just set a timer */
			machine().scheduler().timer_set(attotime::from_usec(1), FUNC(read_sector_done_callback), 0, this);
	}
	else
		/* just set a timer */
		machine().scheduler().timer_set(TIME_PER_SECTOR, FUNC(read_sector_done_callback), 0, this);
}



/*************************************
 *
 *  Sector writing
 *
 *************************************/

static TIMER_CALLBACK( write_sector_done_callback );

void ide_controller_device::continue_write()
{
	/* reset the totals */
	buffer_offset = 0;

	/* clear the buffer ready flag */
	status &= ~IDE_STATUS_BUFFER_READY;
	status |= IDE_STATUS_BUSY;

	if (command == IDE_COMMAND_WRITE_MULTIPLE_BLOCK)
	{
		if (sectors_until_int != 1)
		{
			/* ready to write now */
			write_sector_done();
		}
		else
		{
			/* set a timer to do the write */
			machine().scheduler().timer_set(TIME_PER_SECTOR, FUNC(write_sector_done_callback), 0, this);
		}
	}
	else
	{
		/* set a timer to do the write */
		machine().scheduler().timer_set(TIME_PER_SECTOR, FUNC(write_sector_done_callback), 0, this);
	}
}


void ide_controller_device::read_buffer_from_dma()
{
	int bytesleft = IDE_DISK_SECTOR_SIZE;
	UINT8 *data = buffer;

//  LOG(("Reading sector from %08X\n", dma_address));

	/* loop until we've consumed all bytes */
	while (bytesleft--)
	{
		/* if we're out of space, grab the next descriptor */
		if (dma_bytes_left == 0)
		{
			/* if we're out of buffer space, that's bad */
			if (dma_last_buffer)
			{
				LOG(("DMA Out of buffer space!\n"));
				return;
			}

			/* fetch the address */
			dma_address = dma_space->read_byte(dma_descriptor++ ^ dma_address_xor);
			dma_address |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 8;
			dma_address |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 16;
			dma_address |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 24;
			dma_address &= 0xfffffffe;

			/* fetch the length */
			dma_bytes_left = dma_space->read_byte(dma_descriptor++ ^ dma_address_xor);
			dma_bytes_left |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 8;
			dma_bytes_left |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 16;
			dma_bytes_left |= dma_space->read_byte(dma_descriptor++ ^ dma_address_xor) << 24;
			dma_last_buffer = (dma_bytes_left >> 31) & 1;
			dma_bytes_left &= 0xfffe;
			if (dma_bytes_left == 0)
				dma_bytes_left = 0x10000;

//          LOG(("New DMA descriptor: address = %08X  bytes = %04X  last = %d\n", dma_address, dma_bytes_left, dma_last_buffer));
		}

		/* read the next byte */
		*data++ = dma_space->read_byte(dma_address++);
		dma_bytes_left--;
	}
}


void ide_controller_device::write_sector_done()
{
	ide_device_interface *dev = slot[cur_drive]->dev();
	int lba = dev->lba_address(), count = 0;

	/* now do the write */
	count = dev->write_sector(lba, buffer);

	/* by default, mark the buffer ready and the seek complete */
	status |= IDE_STATUS_BUFFER_READY;
	status |= IDE_STATUS_SEEK_COMPLETE;

	/* and clear the busy adn error flags */
	status &= ~IDE_STATUS_ERROR;
	status &= ~IDE_STATUS_BUSY;

	/* if we succeeded, advance to the next sector and set the nice bits */
	if (count == 1)
	{
		/* advance the pointers, unless this is the last sector */
		/* Gauntlet: Dark Legacy checks to make sure we stop on the last sector */
		if (sector_count != 1)
			next_sector();

		/* clear the error value */
		error = IDE_ERROR_NONE;

		/* signal an interrupt */
		if (--sectors_until_int == 0 || sector_count == 1)
		{
			sectors_until_int = ((command == IDE_COMMAND_WRITE_MULTIPLE_BLOCK) ? block_count : 1);
			signal_interrupt();
		}

		/* signal an interrupt if there's more data needed */
		if (sector_count > 0)
			sector_count--;
		if (sector_count == 0)
			status &= ~IDE_STATUS_BUFFER_READY;

		/* keep going for DMA */
		if (dma_active && sector_count != 0)
		{
			read_buffer_from_dma();
			continue_write();
		}
		else
			dma_active = 0;
	}

	/* if we got an error, we need to report it */
	else
	{
		/* set the error flag and the error */
		status |= IDE_STATUS_ERROR;
		error = IDE_ERROR_BAD_SECTOR;
		bus_master_status |= IDE_BUSMASTER_STATUS_ERROR;
		bus_master_status &= ~IDE_BUSMASTER_STATUS_ACTIVE;

		/* signal an interrupt */
		signal_interrupt();
	}
}


static TIMER_CALLBACK( write_sector_done_callback )
{
	ide_controller_device *ide = (ide_controller_device *)ptr;
	ide->write_sector_done();
}



/*************************************
 *
 *  Handle IDE commands
 *
 *************************************/

void ide_controller_device::handle_command(UINT8 _command)
{
	UINT8 key[5];
	ide_device_interface *dev = slot[cur_drive]->dev();

	/* implicitly clear interrupts here */
	clear_interrupt();
	command = _command;
	switch (command)
	{
		case IDE_COMMAND_READ_MULTIPLE:
		case IDE_COMMAND_READ_MULTIPLE_NORETRY:
			LOGPRINT(("IDE Read multiple: C=%d H=%d S=%d LBA=%d count=%d\n",
				dev->cur_cylinder, dev->cur_head, dev->cur_sector, dev->lba_address(), sector_count));

			/* reset the buffer */
			buffer_offset = 0;
			sectors_until_int = 1;
			dma_active = 0;
			verify_only = 0;

			/* start the read going */
			read_first_sector();
			break;

		case IDE_COMMAND_READ_MULTIPLE_BLOCK:
			LOGPRINT(("IDE Read multiple block: C=%d H=%d S=%d LBA=%d count=%d\n",
				dev->cur_cylinder, dev->cur_head, dev->cur_sector, dev->lba_address(), sector_count));

			/* reset the buffer */
			buffer_offset = 0;
			sectors_until_int = 1;
			dma_active = 0;
			verify_only = 0;

			/* start the read going */
			read_first_sector();
			break;

		case IDE_COMMAND_VERIFY_MULTIPLE:
		case IDE_COMMAND_VERIFY_NORETRY:
			LOGPRINT(("IDE Read verify multiple with/without retries: C=%d H=%d S=%d LBA=%d count=%d\n",
				dev->cur_cylinder, dev->cur_head, dev->cur_sector, dev->lba_address(), sector_count));

			/* reset the buffer */
			buffer_offset = 0;
			sectors_until_int = 1;
			dma_active = 0;
			verify_only = 1;

			/* start the read going */
			read_first_sector();
			break;

		case IDE_COMMAND_READ_DMA:
			LOGPRINT(("IDE Read multiple DMA: C=%d H=%d S=%d LBA=%d count=%d\n",
				dev->cur_cylinder, dev->cur_head, dev->cur_sector, dev->lba_address(), sector_count));

			/* reset the buffer */
			buffer_offset = 0;
			sectors_until_int = sector_count;
			dma_active = 1;
			verify_only = 0;

			/* start the read going */
			if (bus_master_command & 1)
				read_first_sector();
			break;

		case IDE_COMMAND_WRITE_MULTIPLE:
		case IDE_COMMAND_WRITE_MULTIPLE_NORETRY:
			LOGPRINT(("IDE Write multiple: C=%d H=%d S=%d LBA=%d count=%d\n",
				dev->cur_cylinder, dev->cur_head, dev->cur_sector, dev->lba_address(), sector_count));

			/* reset the buffer */
			buffer_offset = 0;
			sectors_until_int = 1;
			dma_active = 0;

			/* mark the buffer ready */
			status |= IDE_STATUS_BUFFER_READY;
			break;

		case IDE_COMMAND_WRITE_MULTIPLE_BLOCK:
			LOGPRINT(("IDE Write multiple block: C=%d H=%d S=%d LBA=%d count=%d\n",
				dev->cur_cylinder, dev->cur_head, dev->cur_sector, dev->lba_address(), sector_count));

			/* reset the buffer */
			buffer_offset = 0;
			sectors_until_int = 1;
			dma_active = 0;

			/* mark the buffer ready */
			status |= IDE_STATUS_BUFFER_READY;
			break;

		case IDE_COMMAND_WRITE_DMA:
			LOGPRINT(("IDE Write multiple DMA: C=%d H=%d S=%d LBA=%d count=%d\n",
				dev->cur_cylinder, dev->cur_head, dev->cur_sector, dev->lba_address(), sector_count));

			/* reset the buffer */
			buffer_offset = 0;
			sectors_until_int = sector_count;
			dma_active = 1;

			/* start the read going */
			if (bus_master_command & 1)
			{
				read_buffer_from_dma();
				continue_write();
			}
			break;

		case IDE_COMMAND_SECURITY_UNLOCK:
			LOGPRINT(("IDE Security Unlock\n"));

			/* reset the buffer */
			buffer_offset = 0;
			sectors_until_int = 0;
			dma_active = 0;

			/* mark the buffer ready */
			status |= IDE_STATUS_BUFFER_READY;
			signal_interrupt();
			break;

		case IDE_COMMAND_GET_INFO:
			LOGPRINT(("IDE Read features\n"));

			/* reset the buffer */
			buffer_offset = 0;
			sector_count = 1;

			/* build the features page */
			memcpy(buffer, slot[cur_drive]->dev()->get_features(), sizeof(buffer));

			/* indicate everything is ready */
			status |= IDE_STATUS_BUFFER_READY;
			status |= IDE_STATUS_SEEK_COMPLETE;
			status |= IDE_STATUS_DRIVE_READY;

			/* and clear the busy adn error flags */
			status &= ~IDE_STATUS_ERROR;
			status &= ~IDE_STATUS_BUSY;

			/* clear the error too */
			error = IDE_ERROR_NONE;

			/* signal an interrupt */
			signal_delayed_interrupt(MINIMUM_COMMAND_TIME, 1);
			break;

		case IDE_COMMAND_DIAGNOSTIC:
			error = IDE_ERROR_DEFAULT;

			/* signal an interrupt */
			signal_delayed_interrupt(MINIMUM_COMMAND_TIME, 0);
			break;

		case IDE_COMMAND_RECALIBRATE:
			/* clear the error too */
			error = IDE_ERROR_NONE;
			/* signal an interrupt */
			signal_delayed_interrupt(MINIMUM_COMMAND_TIME, 0);
			break;

		case IDE_COMMAND_IDLE:
			/* clear the error too */
			error = IDE_ERROR_NONE;

			/* for timeout disabled value is 0 */
			sector_count = 0;
			/* signal an interrupt */
			signal_interrupt();
			break;

		case IDE_COMMAND_SET_CONFIG:
			LOGPRINT(("IDE Set configuration (%d heads, %d sectors)\n", dev->cur_head + 1, sector_count));
			status &= ~IDE_STATUS_ERROR;
			error = IDE_ERROR_NONE;
			dev->set_geometry(sector_count,dev->cur_head + 1);

			/* signal an interrupt */
			signal_delayed_interrupt(MINIMUM_COMMAND_TIME, 0);
			break;

		case IDE_COMMAND_UNKNOWN_F9:
			/* only used by Killer Instinct AFAICT */
			LOGPRINT(("IDE unknown command (F9)\n"));

			/* signal an interrupt */
			signal_interrupt();
			break;

		case IDE_COMMAND_SET_FEATURES:
			LOGPRINT(("IDE Set features (%02X %02X %02X %02X %02X)\n", precomp_offset, sector_count & 0xff, dev->cur_sector, dev->cur_cylinder & 0xff, dev->cur_cylinder >> 8));

			/* signal an interrupt */
			signal_delayed_interrupt(MINIMUM_COMMAND_TIME, 0);
			break;

		case IDE_COMMAND_SET_BLOCK_COUNT:
			LOGPRINT(("IDE Set block count (%02X)\n", sector_count));

			block_count = sector_count;
			// judge dredd wants 'drive ready' on this command
			status |= IDE_STATUS_DRIVE_READY;

			/* signal an interrupt */
			signal_interrupt();
			break;

		case IDE_COMMAND_TAITO_GNET_UNLOCK_1:
			LOGPRINT(("IDE GNET Unlock 1\n"));

			sector_count = 1;
			status |= IDE_STATUS_DRIVE_READY;
			status &= ~IDE_STATUS_ERROR;
			signal_interrupt();
			break;

		case IDE_COMMAND_TAITO_GNET_UNLOCK_2:
			LOGPRINT(("IDE GNET Unlock 2\n"));

			/* reset the buffer */
			buffer_offset = 0;
			sectors_until_int = 0;
			dma_active = 0;

			/* mark the buffer ready */
			status |= IDE_STATUS_BUFFER_READY;
			signal_interrupt();
			break;

		case IDE_COMMAND_TAITO_GNET_UNLOCK_3:
			LOGPRINT(("IDE GNET Unlock 3\n"));

			/* key check */
			dev->read_key(key);
			if ((precomp_offset == key[0]) && (sector_count == key[1]) && (dev->cur_sector == key[2]) && (dev->cur_cylinder == (((UINT16)key[4]<<8)|key[3])))
			{
				gnetreadlock= 0;
			}

			/* update flags */
			status |= IDE_STATUS_DRIVE_READY;
			status &= ~IDE_STATUS_ERROR;
			signal_interrupt();
			break;

		case IDE_COMMAND_SEEK:
			/*
			    cur_cylinder, cur_sector and cur_head
			    are all already set in this case so no need
			    so that implements actual seek
			*/
			/* clear the error too */
			error = IDE_ERROR_NONE;

			/* for timeout disabled value is 0 */
			sector_count = 0;
			/* signal an interrupt */
			signal_interrupt();
			break;


		default:
			LOGPRINT(("IDE unknown command (%02X)\n", command));
			status |= IDE_STATUS_ERROR;
			error = IDE_ERROR_UNKNOWN_COMMAND;
			signal_interrupt();
			//debugger_break(device->machine());
			break;
	}
}



/*************************************
 *
 *  IDE controller read
 *
 *************************************/

UINT32 ide_controller_device::ide_controller_read(int bank, offs_t offset, int size)
{
	UINT32 result = 0;
	ide_device_interface *dev = slot[cur_drive]->dev();

	/* logit */
//  if (BANK(bank, offset) != IDE_BANK0_DATA && BANK(bank, offset) != IDE_BANK0_STATUS_COMMAND && BANK(bank, offset) != IDE_BANK1_STATUS_CONTROL)
		LOG(("%s:IDE read at %d:%X, size=%d\n", machine().describe_context(), bank, offset, size));

	if (dev != NULL)
	{
		if (dev->is_ready()) {
			status |= IDE_STATUS_DRIVE_READY;
		} else {
			status &= ~IDE_STATUS_DRIVE_READY;
		}
	}
	else
	{
		/* even a do-nothing operation should take a little time */

		status ^= IDE_STATUS_BUSY;
		return status;
	}

	switch (BANK(bank, offset))
	{
		/* unknown config register */
		case IDE_BANK2_CONFIG_UNK:
			return config_unknown;

		/* active config register */
		case IDE_BANK2_CONFIG_REGISTER:
			return config_register_num;

		/* data from active config register */
		case IDE_BANK2_CONFIG_DATA:
			if (config_register_num < IDE_CONFIG_REGISTERS)
				return config_register[config_register_num];
			return 0;

		/* read data if there's data to be read */
		case IDE_BANK0_DATA:
			if (status & IDE_STATUS_BUFFER_READY)
			{
				/* fetch the correct amount of data */
				result = buffer[buffer_offset++];
				if (size > 1)
					result |= buffer[buffer_offset++] << 8;
				if (size > 2)
				{
					result |= buffer[buffer_offset++] << 16;
					result |= buffer[buffer_offset++] << 24;
				}

				/* if we're at the end of the buffer, handle it */
				if (buffer_offset >= IDE_DISK_SECTOR_SIZE)
				{
					LOG(("%s:IDE completed PIO read\n", machine().describe_context()));
					continue_read();
					error = IDE_ERROR_DEFAULT;
				}
			}
			break;

		/* return the current error */
		case IDE_BANK0_ERROR:
			return error;

		/* return the current sector count */
		case IDE_BANK0_SECTOR_COUNT:
			return sector_count;

		/* return the current sector */
		case IDE_BANK0_SECTOR_NUMBER:
			return dev->cur_sector;

		/* return the current cylinder LSB */
		case IDE_BANK0_CYLINDER_LSB:
			return dev->cur_cylinder & 0xff;

		/* return the current cylinder MSB */
		case IDE_BANK0_CYLINDER_MSB:
			return dev->cur_cylinder >> 8;

		/* return the current head */
		case IDE_BANK0_HEAD_NUMBER:
			return dev->cur_head_reg;

		/* return the current status and clear any pending interrupts */
		case IDE_BANK0_STATUS_COMMAND:
		/* return the current status but don't clear interrupts */
		case IDE_BANK1_STATUS_CONTROL:
			result = status;
			if (last_status_timer->elapsed() > TIME_PER_ROTATION)
			{
				result |= IDE_STATUS_HIT_INDEX;
				last_status_timer->adjust(attotime::never);
			}

			/* clear interrutps only when reading the real status */
			if (BANK(bank, offset) == IDE_BANK0_STATUS_COMMAND)
			{
				if (interrupt_pending)
					clear_interrupt();
			}
			break;

		/* log anything else */
		default:
			logerror("%s:unknown IDE read at %03X, size=%d\n", machine().describe_context(), offset, size);
			break;
	}

	/* return the result */
	return result;
}



/*************************************
 *
 *  IDE controller write
 *
 *************************************/

void ide_controller_device::ide_controller_write(int bank, offs_t offset, int size, UINT32 data)
{
	switch (BANK(bank, offset))
	{
		case IDE_BANK0_HEAD_NUMBER:
			cur_drive = (data & 0x10) >> 4;
			break;
	}

	ide_device_interface *dev = slot[cur_drive]->dev();
	if (dev == NULL)
		return;

	/* logit */
	if (BANK(bank, offset) != IDE_BANK0_DATA)
		LOG(("%s:IDE write to %d:%X = %08X, size=%d\n", machine().describe_context(), bank, offset, data, size));
	//  fprintf(stderr, "ide write %03x %02x size=%d\n", offset, data, size);
	switch (BANK(bank, offset))
	{
		/* unknown config register */
		case IDE_BANK2_CONFIG_UNK:
			config_unknown = data;
			break;

		/* active config register */
		case IDE_BANK2_CONFIG_REGISTER:
			config_register_num = data;
			break;

		/* data from active config register */
		case IDE_BANK2_CONFIG_DATA:
			if (config_register_num < IDE_CONFIG_REGISTERS)
				config_register[config_register_num] = data;
			break;

		/* write data */
		case IDE_BANK0_DATA:
			if (status & IDE_STATUS_BUFFER_READY)
			{
				/* store the correct amount of data */
				buffer[buffer_offset++] = data;
				if (size > 1)
					buffer[buffer_offset++] = data >> 8;
				if (size > 2)
				{
					buffer[buffer_offset++] = data >> 16;
					buffer[buffer_offset++] = data >> 24;
				}

				/* if we're at the end of the buffer, handle it */
				if (buffer_offset >= IDE_DISK_SECTOR_SIZE)
				{
					LOG(("%s:IDE completed PIO write\n", machine().describe_context()));
					if (command == IDE_COMMAND_SECURITY_UNLOCK)
					{
						if (user_password_enable && memcmp(buffer, user_password, 2 + 32) == 0)
						{
							LOGPRINT(("IDE Unlocked user password\n"));
							user_password_enable = 0;
						}
						if (master_password_enable && memcmp(buffer, master_password, 2 + 32) == 0)
						{
							LOGPRINT(("IDE Unlocked master password\n"));
							master_password_enable = 0;
						}
						if (PRINTF_IDE_PASSWORD)
						{
							int i;

							for (i = 0; i < 34; i += 2)
							{
								if (i % 8 == 2)
									mame_printf_debug("\n");

								mame_printf_debug("0x%02x, 0x%02x, ", buffer[i], buffer[i + 1]);
								//mame_printf_debug("0x%02x%02x, ", buffer[i], buffer[i + 1]);
							}
							mame_printf_debug("\n");
						}

						/* clear the busy and error flags */
						status &= ~IDE_STATUS_ERROR;
						status &= ~IDE_STATUS_BUSY;
						status &= ~IDE_STATUS_BUFFER_READY;

						if (master_password_enable || user_password_enable)
							security_error();
						else
							status |= IDE_STATUS_DRIVE_READY;
					}
					else if (command == IDE_COMMAND_TAITO_GNET_UNLOCK_2)
					{
						UINT8 key[5] = { 0 };
						int i, bad = 0;
						dev->read_key(key);

						for (i=0; !bad && i<512; i++)
							bad = ((i < 2 || i >= 7) && buffer[i]) || ((i >= 2 && i < 7) && buffer[i] != key[i-2]);

						status &= ~IDE_STATUS_BUSY;
						status &= ~IDE_STATUS_BUFFER_READY;
						if (bad)
							status |= IDE_STATUS_ERROR;
						else {
							status &= ~IDE_STATUS_ERROR;
							gnetreadlock= 0;
						}
					}
					else
						continue_write();

				}
			}
			break;

		/* precompensation offset?? */
		case IDE_BANK0_ERROR:
			precomp_offset = data;
			break;

		/* sector count */
		case IDE_BANK0_SECTOR_COUNT:
			sector_count = data ? data : 256;
			break;

		/* current sector */
		case IDE_BANK0_SECTOR_NUMBER:
			dev->cur_sector = data;
			break;

		/* current cylinder LSB */
		case IDE_BANK0_CYLINDER_LSB:
			dev->cur_cylinder = (dev->cur_cylinder & 0xff00) | (data & 0xff);
			break;

		/* current cylinder MSB */
		case IDE_BANK0_CYLINDER_MSB:
			dev->cur_cylinder = (dev->cur_cylinder & 0x00ff) | ((data & 0xff) << 8);
			break;

		/* current head */
		case IDE_BANK0_HEAD_NUMBER:
			dev->cur_head = data & 0x0f;
			dev->cur_head_reg = data;
			// LBA mode = data & 0x40
			break;

		/* command */
		case IDE_BANK0_STATUS_COMMAND:
			handle_command(data);
			break;

		/* adapter control */
		case IDE_BANK1_STATUS_CONTROL:
			adapter_control = data;

			/* handle controller reset */
			//if (data == 0x04)
			if (data & 0x04)
			{
				status |= IDE_STATUS_BUSY;
				status &= ~IDE_STATUS_DRIVE_READY;
				reset_timer->adjust(attotime::from_msec(5));
			}
			break;
	}
}



/*************************************
 *
 *  Bus master read
 *
 *************************************/

UINT32 ide_controller_device::ide_bus_master_read(offs_t offset, int size)
{
	LOG(("%s:ide_bus_master_read(%d, %d)\n", machine().describe_context(), offset, size));

	/* command register */
	if (offset == 0)
		return bus_master_command | (bus_master_status << 16);

	/* status register */
	if (offset == 2)
		return bus_master_status;

	/* descriptor table register */
	if (offset == 4)
		return bus_master_descriptor;

	return 0xffffffff;
}



/*************************************
 *
 *  Bus master write
 *
 *************************************/

void ide_controller_device::ide_bus_master_write(offs_t offset, int size, UINT32 data)
{
	LOG(("%s:ide_bus_master_write(%d, %d, %08X)\n", machine().describe_context(), offset, size, data));

	/* command register */
	if (offset == 0)
	{
		UINT8 old = bus_master_command;
		UINT8 val = data & 0xff;

		/* save the read/write bit and the start/stop bit */
		bus_master_command = (old & 0xf6) | (val & 0x09);
		bus_master_status = (bus_master_status & ~IDE_BUSMASTER_STATUS_ACTIVE) | (val & 0x01);

		/* handle starting a transfer */
		if (!(old & 1) && (val & 1))
		{
			/* reset all the DMA data */
			dma_bytes_left = 0;
			dma_last_buffer = 0;
			dma_descriptor = bus_master_descriptor;

			/* if we're going live, start the pending read/write */
			if (dma_active)
			{
				if (bus_master_command & 8)
					read_next_sector();
				else
				{
					read_buffer_from_dma();
					continue_write();
				}
			}
		}
	}

	/* status register */
	if (offset <= 2 && offset + size > 2)
	{
		UINT8 old = bus_master_status;
		UINT8 val = data >> (8 * (2 - offset));

		/* save the DMA capable bits */
		bus_master_status = (old & 0x9f) | (val & 0x60);

		/* clear interrupt and error bits */
		if (val & IDE_BUSMASTER_STATUS_IRQ)
			bus_master_status &= ~IDE_BUSMASTER_STATUS_IRQ;
		if (val & IDE_BUSMASTER_STATUS_ERROR)
			bus_master_status &= ~IDE_BUSMASTER_STATUS_ERROR;
	}

	/* descriptor table register */
	if (offset == 4)
		bus_master_descriptor = data & 0xfffffffc;
}



/*************************************
 *
 *  IDE direct handlers (16-bit)
 *
 *************************************/

/*
    ide_bus_r()

    Read a 16-bit word from the IDE bus directly.

    select: 0->CS1Fx active, 1->CS3Fx active
    offset: register offset (state of DA2-DA0)
*/
int ide_controller_device::ide_bus_r(int select, int offset)
{
	return ide_controller_read(select ? 1 : 0, offset, select == 0 && offset == 0 ? 2 : 1);
}

/*
    ide_bus_w()

    Write a 16-bit word to the IDE bus directly.

    select: 0->CS1Fx active, 1->CS3Fx active
    offset: register offset (state of DA2-DA0)
    data: data written (state of D0-D15 or D0-D7)
*/
void ide_controller_device::ide_bus_w(int select, int offset, int data)
{
	if (select == 0 && offset == 0)
		ide_controller_write(0, 0, 2, data);
	else
		ide_controller_write(select ? 1 : 0, offset, 1, data & 0xff);
}

UINT32 ide_controller_device::ide_controller_r(int reg, int size)
{
	if (reg >= 0x1f0 && reg < 0x1f8)
		return ide_controller_read(0, reg & 7, size);
	if (reg >= 0x3f0 && reg < 0x3f8)
		return ide_controller_read(1, reg & 7, size);
	if (reg >= 0x030 && reg < 0x040)
		return ide_controller_read(2, reg & 0xf, size);
	return 0xffffffff;
}

void ide_controller_device::ide_controller_w(int reg, int size, UINT32 data)
{
	if (reg >= 0x1f0 && reg < 0x1f8)
		ide_controller_write(0, reg & 7, size, data);
	if (reg >= 0x3f0 && reg < 0x3f8)
		ide_controller_write(1, reg & 7, size, data);
	if (reg >= 0x030 && reg < 0x040)
		ide_controller_write(2, reg & 0xf, size, data);
}


/*************************************
 *
 *  32-bit IDE handlers
 *
 *************************************/

READ32_MEMBER( ide_controller_device::ide_controller32_r )
{
	int size;

	offset *= 4;
	size = convert_to_offset_and_size32(&offset, mem_mask);

	return ide_controller_r(offset, size) << ((offset & 3) * 8);
}


WRITE32_MEMBER( ide_controller_device::ide_controller32_w )
{
	int size;

	offset *= 4;
	size = convert_to_offset_and_size32(&offset, mem_mask);
	data = data >> ((offset & 3) * 8);

	ide_controller_w(offset, size, data);
}


READ16_MEMBER( ide_controller_device::ide_controller16_pcmcia_r )
{
	int size;
	UINT32 res = 0xffff;

	offset *= 2;
	size = convert_to_offset_and_size16(&offset, mem_mask);

	if (offset < 0x008)
		res = ide_controller_read(0, offset & 7, size);
	if (offset >= 0x008 && offset < 0x010)
		res = ide_controller_read(1, offset & 7, size);

	return res << ((offset & 1) * 8);
}


WRITE16_MEMBER( ide_controller_device::ide_controller16_pcmcia_w )
{
	int size;

	offset *= 2;
	size = convert_to_offset_and_size16(&offset, mem_mask);
	data = data >> ((offset & 1) * 8);

	if (offset < 0x008)
		ide_controller_write(0, offset & 7, size, data);
	if (offset >= 0x008 && offset < 0x010)
		ide_controller_write(1, offset & 7, size, data);
}

READ32_MEMBER( ide_controller_device::ide_bus_master32_r )
{
	int size;

	offset *= 4;
	size = convert_to_offset_and_size32(&offset, mem_mask);

	return ide_bus_master_read(offset, size) << ((offset & 3) * 8);
}


WRITE32_MEMBER( ide_controller_device::ide_bus_master32_w )
{
	int size;

	offset *= 4;
	size = convert_to_offset_and_size32(&offset, mem_mask);

	ide_bus_master_write(offset, size, data >> ((offset & 3) * 8));
}



/*************************************
 *
 *  16-bit IDE handlers
 *
 *************************************/

READ16_MEMBER( ide_controller_device::ide_controller16_r )
{
	int size;

	offset *= 2;
	size = convert_to_offset_and_size16(&offset, mem_mask);

	return ide_controller_r(offset, size) << ((offset & 1) * 8);
}


WRITE16_MEMBER( ide_controller_device::ide_controller16_w )
{
	int size;

	offset *= 2;
	size = convert_to_offset_and_size16(&offset, mem_mask);

	ide_controller_w(offset, size, data >> ((offset & 1) * 8));
}

SLOT_INTERFACE_START(ide_devices)
	SLOT_INTERFACE("hdd", IDE_HARDDISK)
SLOT_INTERFACE_END

const device_type IDE_CONTROLLER = &device_creator<ide_controller_device>;

ide_controller_device::ide_controller_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
	device_t(mconfig, IDE_CONTROLLER, "IDE Controller", tag, owner, clock),
	status(0),
	adapter_control(0),
	error(0),
	command(0),
	interrupt_pending(0),
	precomp_offset(0),
	buffer_offset(0),
	sector_count(0),
	block_count(0),
	sectors_until_int(0),
	verify_only(0),
	dma_active(0),
	dma_space(NULL),
	dma_address_xor(0),
	dma_last_buffer(0),
	dma_address(0),
	dma_descriptor(0),
	dma_bytes_left(0),
	bus_master_command(0),
	bus_master_status(0),
	bus_master_descriptor(0),
	config_unknown(0),
	config_register_num(0),
	master_password_enable(0),
	user_password_enable(0),
	master_password(NULL),
	user_password(NULL),
	gnetreadlock(0),
	cur_drive(0),
	m_irq_handler(*this),
	bmcpu(NULL),
	bmspace(0)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void ide_controller_device::device_start()
{
	m_irq_handler.resolve_safe();

	/* set MAME harddisk handle */
	slot[0] = owner()->subdevice<ide_slot_device>("drive_0");
	slot[1] = owner()->subdevice<ide_slot_device>("drive_1");

	/* find the bus master space */
	if (bmcpu != NULL)
	{
		device_t *bmtarget = machine().device(bmcpu);
		if (bmtarget == NULL)
			throw emu_fatalerror("IDE controller '%s' bus master target '%s' doesn't exist!", tag(), bmcpu);
		device_memory_interface *memory;
		if (!bmtarget->interface(memory))
			throw emu_fatalerror("IDE controller '%s' bus master target '%s' has no memory!", tag(), bmcpu);
		dma_space = &memory->space(bmspace);
		dma_address_xor = (dma_space->endianness() == ENDIANNESS_LITTLE) ? 0 : 3;
	}

	/* create a timer for timing status */
	last_status_timer = machine().scheduler().timer_alloc(FUNC_NULL);
	reset_timer = machine().scheduler().timer_alloc(FUNC(reset_callback), this);

	/* register ide states */
	save_item(NAME(adapter_control));
	save_item(NAME(status));
	save_item(NAME(error));
	save_item(NAME(command));
	save_item(NAME(interrupt_pending));
	save_item(NAME(precomp_offset));

	save_item(NAME(buffer));
	save_item(NAME(buffer_offset));
	save_item(NAME(sector_count));

	save_item(NAME(block_count));
	save_item(NAME(sectors_until_int));

	save_item(NAME(dma_active));
	save_item(NAME(dma_last_buffer));
	save_item(NAME(dma_address));
	save_item(NAME(dma_descriptor));
	save_item(NAME(dma_bytes_left));

	save_item(NAME(bus_master_command));
	save_item(NAME(bus_master_status));
	save_item(NAME(bus_master_descriptor));

	save_item(NAME(config_unknown));
	save_item(NAME(config_register));
	save_item(NAME(config_register_num));

	save_item(NAME(master_password_enable));
	save_item(NAME(user_password_enable));

	save_item(NAME(gnetreadlock));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void ide_controller_device::device_reset()
{
	LOG(("IDE controller reset performed\n"));
	/* reset the drive state */
	cur_drive = 0;
	status = IDE_STATUS_DRIVE_READY | IDE_STATUS_SEEK_COMPLETE;
	error = IDE_ERROR_DEFAULT;
	buffer_offset = 0;
	gnetreadlock = 0;
	master_password_enable = (master_password != NULL);
	user_password_enable = (user_password != NULL);
	clear_interrupt();
}



//**************************************************************************
//  IDE SLOT DEVICE
//**************************************************************************

// device type definition
const device_type IDE_SLOT = &device_creator<ide_slot_device>;

//-------------------------------------------------
//  ide_slot_device - constructor
//-------------------------------------------------

ide_slot_device::ide_slot_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, IDE_SLOT, "IDE Connector", tag, owner, clock),
		device_slot_interface(mconfig, *this),
		m_dev(NULL)
{
}

//-------------------------------------------------
//  device_config_complete - perform any
//  operations now that the configuration is
//  complete
//-------------------------------------------------

void ide_slot_device::device_config_complete()
{
	m_dev = dynamic_cast<ide_device_interface *>(get_card_device());
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void ide_slot_device::device_start()
{
}
