/***************************************************************************

    ATA Device implementation.

***************************************************************************/

#include "atadev.h"

//-------------------------------------------------
//  ata_device_interface - constructor
//-------------------------------------------------

ata_device_interface::ata_device_interface(const machine_config &mconfig, device_t &device) :
	m_master_password(NULL),
	m_user_password(NULL),
	m_irq_handler(device),
	m_dmarq_handler(device)
{
}
