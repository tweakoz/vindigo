#ifndef NAMCO53_H
#define NAMCO53_H

#include "devcb.h"


typedef struct _namco_53xx_interface namco_53xx_interface;
struct _namco_53xx_interface
{
	devcb_read8 	in[4];		/* read handlers for ports A-D */
	devcb_write8 	out[2];		/* write handlers for ports A-B */
};


#define MDRV_NAMCO_53XX_ADD(_tag, _clock, _interface) \
	MDRV_DEVICE_ADD(_tag, NAMCO_53XX, _clock) \
	MDRV_DEVICE_CONFIG(_interface)

#define MDRV_NAMCO_53XX_REMOVE(_tag) \
	MDRV_DEVICE_REMOVE(_tag)


READ8_DEVICE_HANDLER( namco_53xx_read );


/* device get info callback */
#define NAMCO_53XX DEVICE_GET_INFO_NAME(namco_53xx)
DEVICE_GET_INFO( namco_53xx );


#endif	/* NAMCO53_H */
