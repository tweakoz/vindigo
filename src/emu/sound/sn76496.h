#pragma once

#ifndef __SN76496_H__
#define __SN76496_H__

#include "devlegcy.h"

READ_LINE_DEVICE_HANDLER( sn76496_ready_r );
WRITE8_DEVICE_HANDLER( sn76496_w );
WRITE8_DEVICE_HANDLER( sn76496_stereo_w );

DECLARE_LEGACY_SOUND_DEVICE(SN76496, sn76496);
DECLARE_LEGACY_SOUND_DEVICE(SN76489, sn76489);
DECLARE_LEGACY_SOUND_DEVICE(SN76489A, sn76489a);
DECLARE_LEGACY_SOUND_DEVICE(SN76494, sn76494);
DECLARE_LEGACY_SOUND_DEVICE(SN94624, sn94624);
DECLARE_LEGACY_SOUND_DEVICE(NCR7496, ncr7496);
DECLARE_LEGACY_SOUND_DEVICE(GAMEGEAR, gamegear);
DECLARE_LEGACY_SOUND_DEVICE(SEGAPSG, segapsg);

#endif /* __SN76496_H__ */
