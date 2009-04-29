#pragma once

#ifndef __SN76496_H__
#define __SN76496_H__

READ8_DEVICE_HANDLER( sn76496_ready_r );
WRITE8_DEVICE_HANDLER( sn76496_w );

DEVICE_GET_INFO( sn76496 );
DEVICE_GET_INFO( sn76489 );
DEVICE_GET_INFO( sn76489a );
DEVICE_GET_INFO( sn76494 );
DEVICE_GET_INFO( gamegear );
DEVICE_GET_INFO( smsiii );

#define SOUND_SN76496 DEVICE_GET_INFO_NAME( sn76496 )
#define SOUND_SN76489 DEVICE_GET_INFO_NAME( sn76489 )
#define SOUND_SN76489A DEVICE_GET_INFO_NAME( sn76489a )
#define SOUND_SN76494 DEVICE_GET_INFO_NAME( sn76494 )
#define SOUND_GAMEGEAR DEVICE_GET_INFO_NAME( gamegear )
#define SOUND_SMSIII DEVICE_GET_INFO_NAME( smsiii )

#endif /* __SN76496_H__ */
