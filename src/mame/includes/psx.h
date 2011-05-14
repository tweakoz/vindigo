/***************************************************************************

  includes/psx.h

***************************************************************************/

#if !defined( PSX_H )

#include "cpu/psx/dma.h"
#include "cpu/psx/sio.h"

#define PSX_IRQ_VBLANK			0x0001
#define PSX_IRQ_CDROM			0x0004
#define PSX_IRQ_DMA				0x0008
#define PSX_IRQ_ROOTCOUNTER0	0x0010
#define PSX_IRQ_ROOTCOUNTER1	0x0020
#define PSX_IRQ_ROOTCOUNTER2	0x0040
#define PSX_IRQ_SIO0			0x0080
#define PSX_IRQ_SIO1			0x0100
#define PSX_IRQ_SPU				0x0200
#define PSX_IRQ_EXTCD			0x0400
#define PSX_IRQ_MASK			(PSX_IRQ_VBLANK | PSX_IRQ_CDROM | PSX_IRQ_DMA | PSX_IRQ_ROOTCOUNTER2 | PSX_IRQ_ROOTCOUNTER1 | PSX_IRQ_ROOTCOUNTER0 | PSX_IRQ_SIO0 | PSX_IRQ_SIO1 | PSX_IRQ_SPU | PSX_IRQ_EXTCD)

typedef struct _psx_machine psx_machine;
struct _psx_machine
{
	running_machine &machine() const { assert(m_machine != NULL); return *m_machine; }

	running_machine *m_machine;
	UINT32 *p_n_psxram;
	size_t n_psxramsize;

	UINT32 n_com_delay;
	UINT32 n_irqdata;
	UINT32 n_irqmask;
};

typedef struct _psx_gpu psx_gpu;

class psx_state : public driver_device
{
public:
	psx_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag) { }

	psx_machine *m_p_psx;
	psx_gpu *m_p_psxgpu;

	UINT32 *m_p_n_psxram;
	size_t m_n_psxramsize;
};


/*----------- defined in video/psx.c -----------*/

PALETTE_INIT( psx );
VIDEO_START( psx_type1 );
VIDEO_START( psx_type2 );
SCREEN_UPDATE( psx );
INTERRUPT_GEN( psx_vblank );
extern void psx_gpu_reset( running_machine &machine );
extern void psx_gpu_read( running_machine &, UINT32 *p_ram, INT32 n_size );
extern void psx_gpu_write( running_machine &, UINT32 *p_ram, INT32 n_size );
READ32_HANDLER( psx_gpu_r );
WRITE32_HANDLER( psx_gpu_w );
extern void psx_lightgun_set( running_machine &, int, int );

/*----------- defined in machine/psx.c -----------*/

WRITE32_HANDLER( psx_com_delay_w );
READ32_HANDLER( psx_com_delay_r );
WRITE32_HANDLER( psx_irq_w );
READ32_HANDLER( psx_irq_r );
extern void psx_irq_set( running_machine &, UINT32 );
extern void psx_dma_install_read_handler( running_machine &, int, psx_dma_read_delegate );
extern void psx_dma_install_write_handler( running_machine &, int, psx_dma_read_delegate );
WRITE32_HANDLER( psx_counter_w );
READ32_HANDLER( psx_counter_r );
extern void psx_sio_install_handler( running_machine &, int, psx_sio_handler );
extern void psx_sio_input( running_machine &, int, int, int );

extern void psx_machine_init( running_machine &machine );
extern void psx_driver_init( running_machine &machine );

#define PSX_H ( 1 )
#endif
