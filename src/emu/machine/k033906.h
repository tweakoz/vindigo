/***************************************************************************

    Konami 033906

***************************************************************************/

#ifndef __K033906_H__
#define __K033906_H__

#include "devlegcy.h"


/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

typedef struct _k033906_interface k033906_interface;
struct _k033906_interface
{
	const char         *voodoo;
};

DECLARE_LEGACY_DEVICE(K033906, k033906);


/***************************************************************************
    MACROS / CONSTANTS
***************************************************************************/

#define MDRV_K033906_ADD(_tag, _config) \
	MDRV_DEVICE_ADD(_tag, K033906, 0) \
	MDRV_DEVICE_CONFIG(_config)


/***************************************************************************
    DEVICE I/O FUNCTIONS
***************************************************************************/

extern READ32_DEVICE_HANDLER( k033906_r );
extern WRITE32_DEVICE_HANDLER( k033906_w );
extern WRITE_LINE_DEVICE_HANDLER( k033906_set_reg );


#endif	/* __K033906_H__ */
