/***************************************************************************

    ppccom.h

    Common PowerPC definitions and functions

***************************************************************************/

#ifndef __PPCCOM_H__
#define __PPCCOM_H__

#include "cpuintrf.h"
#include "ppc.h"


/***************************************************************************
    WIDE-SCALE CONTROLS
***************************************************************************/

#define SIMULATE_603_TLB				(0)



/***************************************************************************
    CONSTANTS
***************************************************************************/

/* core parameters */
#define POWERPC_MIN_PAGE_SHIFT		12
#define POWERPC_MIN_PAGE_SIZE		(1 << POWERPC_MIN_PAGE_SHIFT)
#define POWERPC_MIN_PAGE_MASK		(POWERPC_MIN_PAGE_SIZE - 1)
#define POWERPC_TLB_ENTRIES			128
#define PPC603_FIXED_TLB_ENTRIES	128


/* internal capabilities flags */
#define PPCCAP_OEA					0x01		/* TRUE if we conform to the OEA */
#define PPCCAP_VEA					0x02		/* TRUE if we conform to the VEA */
#define PPCCAP_FPU					0x04		/* TRUE if we have an FPU */
#define PPCCAP_MISALIGNED			0x08		/* TRUE if misaligned accesses are supported */
#define PPCCAP_4XX					0x10		/* TRUE if we are a non-OEA 4XX class chip */
#define PPCCAP_603_MMU				0x20		/* TRUE if we have 603-class MMU features */


/* bits in our internal TLB */
#define TLB_READ					0x01		/* reads are allowed */
#define TLB_WRITE					0x02		/* writes are allowed */


/* PowerPC flavors */
enum _powerpc_flavor
{
	PPC_MODEL_403GA				= 0x00200000,
	PPC_MODEL_403GB				= 0x00200100,
	PPC_MODEL_403GC				= 0x00200200,
	PPC_MODEL_403GCX			= 0x00201400,
	PPC_MODEL_405GP				= 0x40110000,
	PPC_MODEL_601				= 0x00010000,
	PPC_MODEL_603				= 0x00030000,	/* "Wart" */
	PPC_MODEL_604				= 0x00040000,	/* "Zephyr" */
	PPC_MODEL_602				= 0x00050200,	/* "Galahad" */
	PPC_MODEL_603E				= 0x00060103,	/* "Stretch", version 1.3 */
	PPC_MODEL_603EV				= 0x00070000,	/* "Valiant" */
	PPC_MODEL_603R				= 0x00071202,	/* "Goldeneye", version 2.1 */
	PPC_MODEL_740				= 0x00080301,	/* "Arthur", version 3.1 */
	PPC_MODEL_750				= PPC_MODEL_740,
	PPC_MODEL_740P				= 0x00080202,	/* "Conan Doyle", version 1.2 */
	PPC_MODEL_750P				= PPC_MODEL_740P,
	PPC_MODEL_755				= 0x00083203,	/* "Goldfinger", version 2.3 */
	PPC_MODEL_7400				= 0x000c0209,	/* "Max", version 2.9 */
	PPC_MODEL_7410				= 0x800c1104,	/* "Nitro", version 3.4 */
	PPC_MODEL_7450				= 0x80000201,	/* "Vger", version 2.1 */
	PPC_MODEL_7451				= 0x80000203,	/* "Vger", version 2.3 */
	PPC_MODEL_7441				= PPC_MODEL_7451,
	PPC_MODEL_7455				= 0x80010303,	/* "Apollo 6", version 3.3 */
	PPC_MODEL_7445				= PPC_MODEL_7455,
	PPC_MODEL_7457				= 0x80020101,	/* "Apollo 7", version 1.1 */
	PPC_MODEL_MPC8240			= 0x00810101,	/* "Kahlua" */
	PPC_MODEL_MPC8241			= 0x80811014,	/* "Kahlua Lt" */
	PPC_MODEL_MPC8245			= 0x80811014,	/* "Kahlua II" */
};
typedef enum _powerpc_flavor powerpc_flavor;


/* exception types */
enum
{
	EXCEPTION_RESET		= 1,
	EXCEPTION_MACHCHECK	= 2,
	EXCEPTION_DSI		= 3,
	EXCEPTION_ISI		= 4,
	EXCEPTION_EI		= 5,
	EXCEPTION_ALIGN		= 6,
	EXCEPTION_PROGRAM	= 7,
	EXCEPTION_NOFPU		= 8,
	EXCEPTION_DECREMENT	= 9,
	EXCEPTION_SYSCALL	= 12,
	EXCEPTION_TRACE		= 13,
	EXCEPTION_FPASSIST	= 14,
	EXCEPTION_ITLBMISS	= 16,		/* PPCCAP_603_MMU */
	EXCEPTION_DTLBMISSL	= 17,		/* PPCCAP_603_MMU */
	EXCEPTION_DTLBMISSS	= 18,		/* PPCCAP_603_MMU */
	EXCEPTION_COUNT
};


/* SPRs */
enum
{
	/* UISA SPR register indexes */
	SPR_XER				= 0x001,	/* R/W  Fixed Point Exception Register */
	SPR_LR				= 0x008,	/* R/W  Link Register */
	SPR_CTR				= 0x009,	/* R/W  Count Register */

	/* VEA SPR register indexes */
	SPRVEA_TBL_R		= 0x10c,	/* R    Time Base Low */
	SPRVEA_TBU_R		= 0x10d,	/* R    Time Base High */

	/* OEA SPR register indexes */
	SPROEA_DSISR		= 0x012,	/* R/W  DSI Status Register */
	SPROEA_DAR			= 0x013,	/* R/W  Data Address Register */
	SPROEA_DEC			= 0x016,	/* R/W  Decrementer Register */
	SPROEA_SDR1			= 0x019,	/* R/W  Page Table Configuration */
	SPROEA_SRR0			= 0x01a,	/* R/W  Machine Status Save/Restore Register 0 */
	SPROEA_SRR1			= 0x01b,	/* R/W  Machine Status Save/Restore Register 1 */
	SPROEA_SPRG0		= 0x110,	/* R/W  SPR General 0 */
	SPROEA_SPRG1		= 0x111,	/* R/W  SPR General 1 */
	SPROEA_SPRG2		= 0x112,	/* R/W  SPR General 2 */
	SPROEA_SPRG3		= 0x113,	/* R/W  SPR General 3 */
	SPROEA_ASR			= 0x118,	/* R/W  Address Space Register (64-bit only) */
	SPROEA_EAR			= 0x11a,	/* R/W  External Access Register */
	SPROEA_PVR			= 0x11f,	/* R    Processor Version Number */
	SPROEA_IBAT0U		= 0x210,	/* R/W  Instruction BAT 0 Upper */
	SPROEA_IBAT0L		= 0x211,	/* R/W  Instruction BAT 0 Lower */
	SPROEA_IBAT1U		= 0x212,	/* R/W  Instruction BAT 1 Upper */
	SPROEA_IBAT1L		= 0x213,	/* R/W  Instruction BAT 1 Lower */
	SPROEA_IBAT2U		= 0x214,	/* R/W  Instruction BAT 2 Upper */
	SPROEA_IBAT2L		= 0x215,	/* R/W  Instruction BAT 2 Lower */
	SPROEA_IBAT3U		= 0x216,	/* R/W  Instruction BAT 3 Upper */
	SPROEA_IBAT3L		= 0x217,	/* R/W  Instruction BAT 3 Lower */
	SPROEA_DBAT0U		= 0x218,	/* R/W  Data BAT 0 Upper */
	SPROEA_DBAT0L		= 0x219,	/* R/W  Data BAT 0 Lower */
	SPROEA_DBAT1U		= 0x21a,	/* R/W  Data BAT 1 Upper */
	SPROEA_DBAT1L		= 0x21b,	/* R/W  Data BAT 1 Lower */
	SPROEA_DBAT2U		= 0x21c,	/* R/W  Data BAT 2 Upper */
	SPROEA_DBAT2L		= 0x21d,	/* R/W  Data BAT 2 Lower */
	SPROEA_DBAT3U		= 0x21e,	/* R/W  Data BAT 3 Upper */
	SPROEA_DBAT3L		= 0x21f,	/* R/W  Data BAT 3 Lower */
	SPROEA_DABR			= 0x3f5,	/* R/W  Data Address Breakpoint Register */

	/* PowerPC 4XX SPR register indexes */
	SPR4XX_SRR0			= 0x01a,	/* R/W  403GA Machine Status Save/Restore Register 0 */
	SPR4XX_SRR1			= 0x01b,	/* R/W  403GA Machine Status Save/Restore Register 1 */
	SPR4XX_SPRG0		= 0x110,	/* R/W  403GA SPR General 0 */
	SPR4XX_SPRG1		= 0x111,	/* R/W  403GA SPR General 1 */
	SPR4XX_SPRG2		= 0x112,	/* R/W  403GA SPR General 2 */
	SPR4XX_SPRG3		= 0x113,	/* R/W  403GA SPR General 3 */
	SPR4XX_PVR			= 0x11f,	/* R    403GA Processor Version Number */
	SPR4XX_SGR			= 0x3b9,	/* R/W  403GCX Storage Guarded Register */
	SPR4XX_DCWR			= 0x3ba,	/* R/W  403GCX Data Cache Write Through */
	SPR4XX_PID			= 0x3b1,	/* R/W  403GCX Process ID */
	SPR4XX_TBHU			= 0x3cc,	/* R/W  403GCX Time Base High User-mode */
	SPR4XX_TBLU			= 0x3cd,	/* R/W  403GCX Time Base Low User-mode */
	SPR4XX_ICDBDR		= 0x3d3,	/* R    403GA 406GA Instruction Cache Debug Data Register */
	SPR4XX_ESR			= 0x3d4,	/* R/W  403GA 406GA Exception Syndrome Register */
	SPR4XX_DEAR			= 0x3d5,	/* R    403GA 406GA Data Exception Address Register */
	SPR4XX_EVPR			= 0x3d6,	/* R/W  403GA 406GA Exception Vector Prefix Register */
	SPR4XX_CDBCR		= 0x3d7,	/* R/W  403GA 406GA Cache Debug Control Register */
	SPR4XX_TSR			= 0x3d8,	/* R/C  403GA 406GA Timer Status Register */
	SPR4XX_TCR			= 0x3da,	/* R/W  403GA 406GA Timer Control Register */
	SPR4XX_PIT			= 0x3db,	/* R/W  403GA 406GA Programmable Interval Timer */
	SPR4XX_TBHI			= 0x3dc,	/* R/W  403GA 406GA Time Base High */
	SPR4XX_TBLO			= 0x3dd,	/* R/W  403GA 406GA Time Base Low */
	SPR4XX_SRR2			= 0x3de,	/* R/W  403GA 406GA Machine Status Save/Restore Register 2 */
	SPR4XX_SRR3			= 0x3df,	/* R/W  403GA 406GA Machine Status Save/Restore Register 3 */
	SPR4XX_DBSR			= 0x3f0,	/* R/C  403GA 406GA Debug Status Register */
	SPR4XX_DBCR			= 0x3f2,	/* R/W  403GA 406GA Debug Control Register */
	SPR4XX_IAC1			= 0x3f4,	/* R/W  403GA 406GA Instruction Address Compare 1 */
	SPR4XX_IAC2			= 0x3f5,	/* R/W  403GA 406GA Instruction Address Compare 2 */
	SPR4XX_DAC1			= 0x3f6,	/* R/W  403GA 406GA Data Address Compare 1 */
	SPR4XX_DAC2			= 0x3f7,	/* R/W  403GA 406GA Data Address Compare 2 */
	SPR4XX_DCCR			= 0x3fa,	/* R/W  403GA 406GA Data Cache Cacheability Register */
	SPR4XX_ICCR			= 0x3fb,	/* R/W  403GA 406GA Instruction Cache Cacheability Registe */
	SPR4XX_PBL1			= 0x3fc,	/* R/W  403GA 406GA Protection Bound Lower 1 */
	SPR4XX_PBU1			= 0x3fd,	/* R/W  403GA 406GA Protection Bound Upper 1 */
	SPR4XX_PBL2			= 0x3fe,	/* R/W  403GA 406GA Protection Bound Lower 2 */
	SPR4XX_PBU2			= 0x3ff,	/* R/W  403GA 406GA Protection Bound Upper 2 */

	/* PowerPC 602 SPR register indexes */
	SPR602_TCR			= 0x3d8,	/* 602 */
	SPR602_IBR			= 0x3da,	/* 602 */
	SPR602_ESASRR		= 0x3db,	/* 602 */
	SPR602_SEBR			= 0x3de,	/* 602 */
	SPR602_SER			= 0x3df,	/* 602 */
	SPR602_SP			= 0x3fd,	/* 602 */
	SPR602_LT			= 0x3fe,	/* 602 */

	/* PowerPC 603 SPR register indexes */
	SPR603_TBL_R		= 0x10c,	/* R   603 Time Base Low (Read-only) */
	SPR603_TBU_R		= 0x10d,	/* R   603 Time Base High (Read-only) */
	SPR603_TBL_W		= 0x11c,	/* W   603 Time Base Low (Write-only) */
	SPR603_TBU_W		= 0x11d,	/* W   603 Time Base Hight (Write-only) */
	SPR603_DMISS		= 0x3d0,	/* R   603 Data TLB Miss Address Register */
	SPR603_DCMP			= 0x3d1,	/* R   603 Data TLB Compare Register */
	SPR603_HASH1		= 0x3d2,	/* R   603 Primary Hash Address Register */
	SPR603_HASH2		= 0x3d3,	/* R   603 Secondary Hash Address Register */
	SPR603_IMISS		= 0x3d4,	/* R   603 Instruction TLB Miss Address Register */
	SPR603_ICMP			= 0x3d5,	/* R   603 Instruction TLB Compare Register */
	SPR603_RPA			= 0x3d6,	/* R/W 603 Required Physical Address Register */
	SPR603_HID0			= 0x3f0,	/* R/W 603 Hardware Implementation Register 0 */
	SPR603_HID1			= 0x3f1,	/* R/W 603 Hardware Implementation Register 1 */
	SPR603_IABR			= 0x3f2,	/* R/W 603 Instruction Address Breakpoint Register */
	SPR603_HID2			= 0x3f3		/* R/W 603 */
};


/* PowerPC 4XX DCR register indexes */
enum
{
	DCR4XX_EXISR		= 0x040,	/* external interrupt status */
	DCR4XX_EXIER		= 0x042,	/* external interrupt enable */
	DCR4XX_BR0			= 0x080,	/* bank */
	DCR4XX_BR1			= 0x081,	/* bank */
	DCR4XX_BR2			= 0x082,	/* bank */
	DCR4XX_BR3			= 0x083,	/* bank */
	DCR4XX_BR4			= 0x084,	/* bank */
	DCR4XX_BR5			= 0x085,	/* bank */
	DCR4XX_BR6			= 0x086,	/* bank */
	DCR4XX_BR7			= 0x087,	/* bank */
	DCR4XX_BEAR			= 0x090,	/* bus error address */
	DCR4XX_BESR			= 0x091,	/* bus error syndrome */
	DCR4XX_IOCR			= 0x0a0,	/* io configuration */
	DCR4XX_DMACR0		= 0x0c0,	/* dma channel control */
	DCR4XX_DMACT0		= 0x0c1,	/* dma destination address */
	DCR4XX_DMADA0		= 0x0c2,	/* dma destination address */
	DCR4XX_DMASA0		= 0x0c3,	/* dma source address */
	DCR4XX_DMACC0		= 0x0c4,	/* dma chained count */
	DCR4XX_DMACR1		= 0x0c8,	/* dma channel control */
	DCR4XX_DMACT1		= 0x0c9,	/* dma destination address */
	DCR4XX_DMADA1		= 0x0ca,	/* dma destination address */
	DCR4XX_DMASA1		= 0x0cb,	/* dma source address */
	DCR4XX_DMACC1		= 0x0cc,	/* dma chained count */
	DCR4XX_DMACR2		= 0x0d0,	/* dma channel control */
	DCR4XX_DMACT2		= 0x0d1,	/* dma destination address */
	DCR4XX_DMADA2		= 0x0d2,	/* dma source address */
	DCR4XX_DMASA2		= 0x0d3,	/* dma source address */
	DCR4XX_DMACC2		= 0x0d4,	/* dma chained count */
	DCR4XX_DMACR3		= 0x0d8,	/* dma channel control */
	DCR4XX_DMACT3		= 0x0d9,	/* dma destination address */
	DCR4XX_DMADA3		= 0x0da,	/* dma source address */
	DCR4XX_DMASA3		= 0x0db,	/* dma source address */
	DCR4XX_DMACC3		= 0x0dc,	/* dma chained count */
	DCR4XX_DMASR		= 0x0e0		/* dma status */
};


/* PowerPC 4XX SPU register indexes */
enum
{
	SPU4XX_LINE_STATUS		= 0x00,
	SPU4XX_HANDSHAKE_STATUS	= 0x02,
	SPU4XX_BAUD_DIVISOR_H	= 0x04,
	SPU4XX_BAUD_DIVISOR_L	= 0x05,
	SPU4XX_CONTROL			= 0x06,
	SPU4XX_RX_COMMAND		= 0x07,
	SPU4XX_TX_COMMAND		= 0x08,
	SPU4XX_BUFFER			= 0x09
};


/* FPSCR register bits */
#define FPSCR_FX			0x80000000
#define FPSCR_FEX			0x40000000
#define FPSCR_VX			0x20000000
#define FPSCR_OX			0x10000000
#define FPSCR_UX			0x08000000
#define FPSCR_ZX			0x04000000
#define FPSCR_XX			0x02000000


/* XER register bits */
#define XER_SO				0x80000000
#define XER_OV				0x40000000
#define XER_CA				0x20000000


/* Machine State Register bits - common */
#define MSR_ILE				0x00010000	/* Interrupt Little Endian Mode */
#define MSR_EE				0x00008000	/* External Interrupt Enable */
#define MSR_PR				0x00004000	/* Problem State */
#define MSR_ME				0x00001000	/* Machine Check Enable */
#define MSR_LE				0x00000001	/* Little Endian */

/* Machine State Register bits - OEA */
#define MSROEA_POW			0x00040000	/* Power Management Enable */
#define MSROEA_FP			0x00002000	/* Floating Point Available */
#define MSROEA_FE0			0x00000800	/* FP Exception Mode 0 */
#define MSROEA_SE			0x00000400	/* Single Step Trace Enable */
#define MSROEA_BE			0x00000200	/* Branch Trace Enable */
#define MSROEA_FE1			0x00000100	/* FP Exception Mode 1 */
#define MSROEA_IP			0x00000040	/* Interrupt Prefix */
#define MSROEA_IR			0x00000020	/* Instruction Relocate */
#define MSROEA_DR			0x00000010	/* Data Relocate */
#define MSROEA_RI			0x00000002	/* Recoverable Interrupt Enable */

/* Machine State Register bits - 4XX */
#define MSR4XX_WE			0x00040000	/* Wait State Enable */
#define MSR4XX_CE			0x00020000	/* Critical Interrupt Enable */
#define MSR4XX_DE			0x00000200	/* Debug Exception Enable */
#define MSR4XX_PE			0x00000008	/* Protection Enable (reserved for others) */
#define MSR4XX_PX			0x00000004	/* Protection Exclusive Mode (reserved for others) */

/* Machine State Register bits - 602 */
#define MSR602_AP			0x00800000	/* Access privilege state */
#define MSR602_SA			0x00400000	/* Supervisor access mode */

/* Machine State Register bits - 603 */
#define MSR603_TGPR			0x00020000	/* Temporary GPR Remapping */


/* DSISR bits for DSI/alignment exceptions */
#define DSISR_DIRECT		0x00000001		/* DSI: direct-store exception? */
#define DSISR_NOT_FOUND		0x00000002		/* DSI: not found in HTEG or DBAT */
#define DSISR_PROTECTED		0x00000010		/* DSI: exception due to protection */
#define DSISR_CACHE_ERROR	0x00000020		/* DSI: operation on incorrect cache type */
#define DSISR_STORE			0x00000040		/* DSI: store (1) or load (0) */
#define DSISR_DABR			0x00000200		/* DSI: DABR match occurred */
#define DSISR_NO_SEGMENT	0x00000400		/* DSI: no segment match found (64-bit only) */
#define DSISR_INVALID_ECWX	0x00000800		/* DSI: ECIWX or ECOWX used with EAR[E] = 0 */
#define DSISR_INSTRUCTION	0xfffff000		/* align: instruction decoding bits */


/* PowerPC 4XX IRQ bits */
#define PPC4XX_IRQ_BIT_CRITICAL		(0x80000000 >> 0)
#define PPC4XX_IRQ_BIT_SPUR			(0x80000000 >> 4)
#define PPC4XX_IRQ_BIT_SPUT			(0x80000000 >> 5)
#define PPC4XX_IRQ_BIT_JTAGR		(0x80000000 >> 6)
#define PPC4XX_IRQ_BIT_JTAGT		(0x80000000 >> 7)
#define PPC4XX_IRQ_BIT_DMA0			(0x80000000 >> 8)
#define PPC4XX_IRQ_BIT_DMA1			(0x80000000 >> 9)
#define PPC4XX_IRQ_BIT_DMA2			(0x80000000 >> 10)
#define PPC4XX_IRQ_BIT_DMA3			(0x80000000 >> 11)
#define PPC4XX_IRQ_BIT_EXT0			(0x80000000 >> 27)
#define PPC4XX_IRQ_BIT_EXT1			(0x80000000 >> 28)
#define PPC4XX_IRQ_BIT_EXT2			(0x80000000 >> 29)
#define PPC4XX_IRQ_BIT_EXT3			(0x80000000 >> 30)
#define PPC4XX_IRQ_BIT_EXT4			(0x80000000 >> 31)

#define PPC4XX_IRQ_BIT_EXT(n)		(PPC4XX_IRQ_BIT_EXT0 >> (n))
#define PPC4XX_IRQ_BIT_DMA(n)		(PPC4XX_IRQ_BIT_DMA0 >> (n))


/* PowerPC 4XX DMA control bits */
#define PPC4XX_DMACR_CE				0x80000000		/* channel enable */
#define PPC4XX_DMACR_CIE			0x40000000		/* channel interrupt enable */
#define PPC4XX_DMACR_TD				0x20000000		/* transfer direction */
#define PPC4XX_DMACR_PL				0x10000000		/* peripheral location */
#define PPC4XX_DMACR_PW_MASK		0x0c000000		/* peripheral width */
#define PPC4XX_DMACR_DAI			0x02000000		/* destination address increment */
#define PPC4XX_DMACR_SAI			0x01000000		/* source address increment */
#define PPC4XX_DMACR_CP				0x00800000		/* channel priority */
#define PPC4XX_DMACR_TM_MASK		0x00600000		/* transfer mode */
#define PPC4XX_DMACR_PSC_MASK		0x00180000		/* peripheral setup cycles */
#define PPC4XX_DMACR_PWC_MASK		0x0007e000		/* peripheral wait cycles */
#define PPC4XX_DMACR_PHC_MASK		0x00001c00		/* peripheral hold cycles */
#define PPC4XX_DMACR_ETD			0x00000200		/* end-of-transfer pin direction */
#define PPC4XX_DMACR_TCE			0x00000100		/* terminal count enable */
#define PPC4XX_DMACR_CH				0x00000080		/* chaining enable */
#define PPC4XX_DMACR_BME			0x00000040		/* burst mode enable */
#define PPC4XX_DMACR_ECE			0x00000020		/* EOT chain mode enable */
#define PPC4XX_DMACR_TCD			0x00000010		/* TC chain mode disable */
#define PPC4XX_DMACR_PCE			0x00000008


/* PowerPC 4XX Timer control register bits */
#define PPC4XX_TCR_WP_MASK			0xc0000000		/* watchdog period */
#define PPC4XX_TCR_WRC_MASK			0x30000000		/* watchdog reset control */
#define PPC4XX_TCR_WIE				0x08000000		/* watchdog interrupt enable */
#define PPC4XX_TCR_PIE				0x04000000		/* PIT interrupt enable */
#define PPC4XX_TCR_FP_MASK			0x03000000		/* FIT period */
#define PPC4XX_TCR_FIE				0x00800000		/* FIT interrupt enable */
#define PPC4XX_TCR_ARE				0x00400000		/* auto reload enable */


/* PowerPC 4XX Timer status register bits */
#define PPC4XX_TSR_ENW				0x80000000		/* enable next watchdog */
#define PPC4XX_TSR_WIS				0x40000000		/* watchdog interrupt status */
#define PPC4XX_TSR_WRS_MASK			0x30000000		/* watchdog reset status */
#define PPC4XX_TSR_PIS				0x08000000		/* PIT interrupt status */
#define PPC4XX_TSR_FIS				0x04000000		/* FIT interrupt status */


/* instruction decoding masks */
#define M_LI    			0x03fffffc
#define M_AA    			0x00000002
#define M_LK    			0x00000001
#define M_BO    			0x03e00000
#define M_BI    			0x001f0000
#define M_BD    			0x0000fffc
#define M_RT    			0x03e00000
#define M_RD    			0x03e00000
#define M_RS    			0x03e00000
#define M_RA    			0x001f0000
#define M_RB    			0x0000f800
#define M_CRFD  			0x03800000
#define M_L     			0x00200000
#define M_TO    			0x03e00000
#define M_D     			0x0000ffff
#define M_SIMM  			0x0000ffff
#define M_UIMM  			0x0000ffff
#define M_NB    			0x0000f800
#define M_SR    			0x000f0000
#define M_SH    			0x0000f800
#define M_CRFS  			0x001c0000
#define M_IMM   			0x0000f000
#define M_CRBD  			0x03e00000
#define M_RC    			0x00000001
#define M_CRBA  			0x001f0000
#define M_CRBB  			0x0000f800
#define M_SPR   			0x001FF800
#define M_TBR   			0x001FF800
#define M_CRM   			0x000FF000
#define M_DCR				0x001FF800
#define M_FM    			0x01FE0000
#define M_OE    			0x00000400
#define M_REGC  			0x000007c0
#define M_MB    			0x000007c0
#define M_ME    			0x0000003e
#define M_XO    			0x000007fe



/***************************************************************************
    HELPER MACROS
***************************************************************************/

/*
 * Field Defining Macros
 *
 * These macros generate instruction words with their associated fields filled
 * in with the passed value.
 */

#define D_OP(op)			((op & 0x3f) << 26)
#define D_XO(xo)			((xo & 0x3ff) << 1)
#define D_RT(r)				((r & 0x1f) << (31 - 10))
#define D_RA(r)				((r & 0x1f) << (31 - 15))
#define D_UIMM(u)			(u & 0xffff)

/*
 * Macros to Get Field Values
 *
 * These macros return the values of fields in an opcode. They all return
 * unsigned values and do not perform any sign extensions.
 */

#define G_RT(op)			((op & M_RT) >> (31 - 10))
#define G_RD(op)			((op & M_RD) >> (31 - 10))
#define G_RS(op)			((op & M_RS) >> (31 - 10))
#define G_RA(op)			((op & M_RA) >> (31 - 15))
#define G_RB(op)			((op & M_RB) >> (31 - 20))
#define G_SIMM(op)			(op & M_SIMM)
#define G_UIMM(op)			(op & M_UIMM)
#define G_LI(op)			((op & M_LI) >> 2)
#define G_BO(op)			((op & M_BO) >> (31 - 10))
#define G_BI(op)			((op & M_BI) >> (31 - 15))
#define G_BD(op)			((op & M_BD) >> 2)
#define G_CRFD(op)			((op & M_CRFD) >> (31 - 8))
#define G_L(op)				((op & M_L) >> (31 - 10))
#define G_CRBD(op)			((op & M_CRBD) >> (31 - 10))
#define G_CRBA(op)			((op & M_CRBA) >> (31 - 15))
#define G_CRBB(op)			((op & M_CRBB) >> (31 - 20))
#define G_REGC(op)			((op & M_REGC) >> (31 - 25))
#define G_D(op)				(op & M_D)
#define G_NB(op)			((op & M_NB) >> (31 - 20))
#define G_CRFS(op)			((op & M_CRFS) >> (31 - 13))
#define G_SPR(op)			((op & M_SPR) >> (31 - 20))
#define G_TBR(op)			((op & M_TBR) >> (31 - 20))
#define G_DCR(op)			((op & M_DCR) >> (31 - 20))
#define G_SR(op)			((op & M_SR) >> (31 - 15))
#define G_CRM(op)			((op & M_CRM) >> (31 - 19))
#define G_FM(op)			((op & M_FM) >> (31 - 14))
#define G_IMM(op)			((op & M_IMM) >> (31 - 19))
#define G_SH(op)			((op & M_SH) >> (31 - 20))
#define G_MB(op)			((op & M_MB) >> (31 - 25))
#define G_ME(op)			((op & M_ME) >> 1)
#define G_TO(op)			((op & M_TO) >> (31 - 10))
#define G_XO(op)			((op & M_XO) >> (31 - 30))



/***************************************************************************
    STRUCTURES & TYPEDEFS
***************************************************************************/

typedef int (*ppc4xx_spu_rx_handler)(UINT8 *data);
typedef void (*ppc4xx_spu_tx_handler)(UINT8 data);


/* PowerPC 4XX-specific serial port state */
typedef struct _ppc4xx_spu_state ppc4xx_spu_state;
struct _ppc4xx_spu_state
{
	UINT8			regs[9];
	UINT8			txbuf;
	UINT8			rxbuf;
	emu_timer *		timer;
	ppc4xx_spu_rx_handler rx_handler;
	ppc4xx_spu_tx_handler tx_handler;
};


/* forward declaration of implementation-specific state */
typedef struct _ppcimp_state ppcimp_state;


/* PowerPC state */
typedef struct _powerpc_state powerpc_state;
struct _powerpc_state
{
	/* core registers */
	UINT32			pc;
	int				icount;
	UINT32			r[32];
	double			f[32];
	UINT32			cr;
	UINT32			fpscr;
	UINT32			msr;
	UINT32			sr[16];
	UINT32			spr[1024];
	UINT32			dcr[256];
	
	/* parameters for calls */
	UINT32			param0;
	UINT32			param1;

	/* MMU */
	UINT32 *		tlb_table;
	UINT32 *		tlb_entries;
	UINT32			tlb_index;

	/* architectural distinctions */
	powerpc_flavor	flavor;
	UINT8			cap;
	UINT8			cache_line_size;
	UINT32			tb_divisor;
	int				(*spr_read)(powerpc_state *ppc);
	int				(*spr_write)(powerpc_state *ppc);
	int				(*tlb_fill)(powerpc_state *ppc);

	/* PowerPC 4xx-specific state */
	ppc4xx_spu_state spu;
	emu_timer *		fit_timer;
	emu_timer *		pit_timer;
	emu_timer *		wdog_timer;
	UINT32			pit_reload;
	UINT32			irqstate;
	
	/* PowerPC 603-specific state */
	UINT32			mmu603_cmp;
	UINT32			mmu603_hash[2];
	UINT32			mmu603_r[4];

	/* internal stuff */
	int				cpunum;
	int 			(*irq_callback)(int irqline);
	UINT32			irq_pending;
	UINT32			system_clock;
	UINT32			cpu_clock;
	UINT64			tb_zero_cycles;
	UINT64			dec_zero_cycles;
	emu_timer *		decrementer_int_timer;
	
	/* for use by specific implementations */
	ppcimp_state *	impstate;
};



/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

size_t ppccom_init(powerpc_state *ppc, powerpc_flavor flavor, UINT8 cap, int tb_divisor, int clock, const powerpc_config *config, int (*irqcallback)(int), void *memory);
void ppccom_reset(powerpc_state *ppc);
#ifdef ENABLE_DEBUGGER
offs_t ppccom_dasm(powerpc_state *ppc, char *buffer, offs_t pc, const UINT8 *oprom, const UINT8 *opram);
#endif
void ppccom_update_cycle_counting(powerpc_state *ppc);

int ppccom_translate_address(powerpc_state *ppc, int space, int intention, offs_t *address);

void ppccom_set_info(powerpc_state *ppc, UINT32 state, cpuinfo *info);
void ppccom_get_info(powerpc_state *ppc, UINT32 state, cpuinfo *info);

void ppccom_tlb_fill(powerpc_state *ppc);
void ppccom_tlb_flush(powerpc_state *ppc);

void ppccom_execute_tlbie(powerpc_state *ppc);
void ppccom_execute_tlbia(powerpc_state *ppc);
void ppccom_execute_tlbl(powerpc_state *ppc);
void ppccom_execute_mftb(powerpc_state *ppc);
void ppccom_execute_mfspr(powerpc_state *ppc);
void ppccom_execute_mtspr(powerpc_state *ppc);
void ppccom_execute_mfdcr(powerpc_state *ppc);
void ppccom_execute_mtdcr(powerpc_state *ppc);

void ppc4xx_set_info(powerpc_state *ppc, UINT32 state, cpuinfo *info);
void ppc4xx_get_info(powerpc_state *ppc, UINT32 state, cpuinfo *info);

#endif
