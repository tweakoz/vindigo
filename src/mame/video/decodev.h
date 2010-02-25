/*************************************************************************

    decodev.h

    Implementation of various Data East custom video ICs

**************************************************************************/

#include "devcb.h"


/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

typedef int (*deco16_bank_cb)( const int bank );


typedef struct _deco16ic_interface deco16ic_interface;
struct _deco16ic_interface
{
	const char         *screen;
	int                pf12_only;
	int                split;
	int                full_width;

	int                trans_mask1, trans_mask2, trans_mask3, trans_mask4;
	int                col_base1, col_base2, col_base3, col_base4;
	int                col_mask1, col_mask2, col_mask3, col_mask4;
	deco16_bank_cb     bank_cb0, bank_cb1, bank_cb2, bank_cb3;
};

/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

DEVICE_GET_INFO( deco16ic );


/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/

#define DECO16IC DEVICE_GET_INFO_NAME( deco16ic )

#define MDRV_DECO16IC_ADD(_tag, _interface) \
	MDRV_DEVICE_ADD(_tag, DECO16IC, 0) \
	MDRV_DEVICE_CONFIG(_interface)

/***************************************************************************
    DEVICE I/O FUNCTIONS
***************************************************************************/

WRITE16_DEVICE_HANDLER( decodev_pf1_data_w );
WRITE16_DEVICE_HANDLER( decodev_pf2_data_w );
WRITE16_DEVICE_HANDLER( decodev_pf3_data_w );
WRITE16_DEVICE_HANDLER( decodev_pf4_data_w );

READ16_DEVICE_HANDLER( decodev_pf1_data_r );
READ16_DEVICE_HANDLER( decodev_pf2_data_r );
READ16_DEVICE_HANDLER( decodev_pf3_data_r );
READ16_DEVICE_HANDLER( decodev_pf4_data_r );

WRITE16_DEVICE_HANDLER( decodev_pf12_control_w );
WRITE16_DEVICE_HANDLER( decodev_pf34_control_w );

READ16_DEVICE_HANDLER( decodev_pf12_control_r );
READ16_DEVICE_HANDLER( decodev_pf34_control_r );


WRITE32_DEVICE_HANDLER( decodev_pf1_data_dword_w );
WRITE32_DEVICE_HANDLER( decodev_pf2_data_dword_w );
WRITE32_DEVICE_HANDLER( decodev_pf3_data_dword_w );
WRITE32_DEVICE_HANDLER( decodev_pf4_data_dword_w );

READ32_DEVICE_HANDLER( decodev_pf1_data_dword_r );
READ32_DEVICE_HANDLER( decodev_pf2_data_dword_r );
READ32_DEVICE_HANDLER( decodev_pf3_data_dword_r );
READ32_DEVICE_HANDLER( decodev_pf4_data_dword_r );

WRITE32_DEVICE_HANDLER( decodev_pf12_control_dword_w );
WRITE32_DEVICE_HANDLER( decodev_pf34_control_dword_w );

READ32_DEVICE_HANDLER( decodev_pf12_control_dword_r );
READ32_DEVICE_HANDLER( decodev_pf34_control_dword_r );


WRITE16_DEVICE_HANDLER( decodev_nonbuffered_palette_w );
WRITE16_DEVICE_HANDLER( decodev_buffered_palette_w );
WRITE16_DEVICE_HANDLER( decodev_palette_dma_w );

WRITE16_DEVICE_HANDLER( decodev_priority_w );
READ16_DEVICE_HANDLER( decodev_priority_r );

READ16_DEVICE_HANDLER( decodev_71_r );

void decodev_print_debug_info(running_device *device, bitmap_t *bitmap);

void decodev_pf12_update(running_device *device, const UINT16 *rowscroll_1_ptr, const UINT16 *rowscroll_2_ptr);
void decodev_pf34_update(running_device *device, const UINT16 *rowscroll_1_ptr, const UINT16 *rowscroll_2_ptr);

void decodev_tilemap_1_draw(running_device *device, bitmap_t *bitmap, const rectangle *cliprect, int flags, UINT32 priority);
void decodev_tilemap_2_draw(running_device *device, bitmap_t *bitmap, const rectangle *cliprect, int flags, UINT32 priority);
void decodev_tilemap_3_draw(running_device *device, bitmap_t *bitmap, const rectangle *cliprect, int flags, UINT32 priority);
void decodev_tilemap_4_draw(running_device *device, bitmap_t *bitmap, const rectangle *cliprect, int flags, UINT32 priority);

/* used by boogwing */
void decodev_tilemap_34_combine_draw(running_device *device, bitmap_t *bitmap, const rectangle *cliprect, int flags, UINT32 priority);


/* used by boogwing */
void decodev_clear_sprite_priority_bitmap(running_device *device);
void decodev_pdrawgfx(
		running_device *device,
		bitmap_t *dest,const rectangle *clip,const gfx_element *gfx,
		UINT32 code,UINT32 color,int flipx,int flipy,int sx,int sy,
		int transparent_color,UINT32 pri_mask,UINT32 sprite_mask,UINT8 write_pri,UINT8 alpha);
