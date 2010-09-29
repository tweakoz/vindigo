/*****************************************************************************
 *
 *   sh2common.c
 *
 *   SH-2 non-specific components
 *
 *****************************************************************************/

#include "emu.h"
#include "debugger.h"
#include "sh2.h"
#include "sh2comn.h"

#define VERBOSE 0

#define LOG(x)	do { if (VERBOSE) logerror x; } while (0)

#ifdef USE_SH2DRC
#define GET_SH2(dev) *(sh2_state **)downcast<legacy_cpu_device *>(dev)->token()
#else
#define GET_SH2(dev) (sh2_state *)downcast<legacy_cpu_device *>(dev)->token()
#endif


static const int div_tab[4] = { 3, 5, 7, 0 };

INLINE UINT32 RL(sh2_state *sh2, offs_t A)
{
	if (A >= 0xe0000000)
		return sh2_internal_r(sh2->internal, (A & 0x1fc)>>2, 0xffffffff);

	if (A >= 0xc0000000)
		return sh2->program->read_dword(A);

	if (A >= 0x40000000)
		return 0xa5a5a5a5;

  return sh2->program->read_dword(A & AM);
}

INLINE void WL(sh2_state *sh2, offs_t A, UINT32 V)
{
	if (A >= 0xe0000000)
	{
		sh2_internal_w(sh2->internal, (A & 0x1fc)>>2, V, 0xffffffff);
		return;
	}

	if (A >= 0xc0000000)
	{
		sh2->program->write_dword(A,V);
		return;
	}

	if (A >= 0x40000000)
		return;

	sh2->program->write_dword(A & AM,V);
}

static void sh2_timer_resync(sh2_state *sh2)
{
	int divider = div_tab[(sh2->m[5] >> 8) & 3];
	UINT64 cur_time = sh2->device->total_cycles();

	if(divider)
		sh2->frc += (cur_time - sh2->frc_base) >> divider;
	sh2->frc_base = cur_time;
}

static void sh2_timer_activate(sh2_state *sh2)
{
	int max_delta = 0xfffff;
	UINT16 frc;

	timer_adjust_oneshot(sh2->timer, attotime_never, 0);

	frc = sh2->frc;
	if(!(sh2->m[4] & OCFA)) {
		UINT16 delta = sh2->ocra - frc;
		if(delta < max_delta)
			max_delta = delta;
	}

	if(!(sh2->m[4] & OCFB) && (sh2->ocra <= sh2->ocrb || !(sh2->m[4] & 0x010000))) {
		UINT16 delta = sh2->ocrb - frc;
		if(delta < max_delta)
			max_delta = delta;
	}

	if(!(sh2->m[4] & OVF) && !(sh2->m[4] & 0x010000)) {
		int delta = 0x10000 - frc;
		if(delta < max_delta)
			max_delta = delta;
	}

	if(max_delta != 0xfffff) {
		int divider = div_tab[(sh2->m[5] >> 8) & 3];
		if(divider) {
			max_delta <<= divider;
			sh2->frc_base = sh2->device->total_cycles();
			timer_adjust_oneshot(sh2->timer, sh2->device->cycles_to_attotime(max_delta), 0);
		} else {
			logerror("SH2.%s: Timer event in %d cycles of external clock", sh2->device->tag(), max_delta);
		}
	}
}


static TIMER_CALLBACK( sh2_timer_callback )
{
	sh2_state *sh2 = (sh2_state *)ptr;
	UINT16 frc;

	sh2_timer_resync(sh2);

	frc = sh2->frc;

	if(frc == sh2->ocrb)
		sh2->m[4] |= OCFB;

	if(frc == 0x0000)
		sh2->m[4] |= OVF;

	if(frc == sh2->ocra)
	{
		sh2->m[4] |= OCFA;

		if(sh2->m[4] & 0x010000)
			sh2->frc = 0;
	}

	sh2_recalc_irq(sh2);
	sh2_timer_activate(sh2);
}


/* 
  We have to do DMA on a timer (or at least, in chunks) due to the way some systems use it.
  The 32x is a difficult case, they set the SOURCE of the DMA to a FIFO buffer, which at most
  can have 8 words in it.  Attempting to do an 'instant DMA' in this scenario is impossible
  because the game is expecting the 68k of the system to feed data into the FIFO at the same
  time as the SH2 is transfering it out via DMA

  There are two ways we can do this

  a) with a high frequency timer (more accurate, but a large performance hit)

  or
  
  b) in the CPU_EXECUTE loop


  we're currently doing a)
  
  b) causes problems with ST-V games

*/



void sh2_notify_dma_data_available(running_device *device)
{
	sh2_state *sh2 = GET_SH2(device);
	//printf("call notify\n");
	
	for (int dma=0;dma<2;dma++)
	{
		//printf("sh2->dma_timer_active[dma] %04x\n",sh2->dma_timer_active[dma]);
		
		if (sh2->dma_timer_active[dma]==2) // 2 = stalled
		{
		//	printf("resuming stalled dma\n");
			sh2->dma_timer_active[dma]=1;
			timer_adjust_oneshot(sh2->dma_current_active_timer[dma], attotime_zero, dma);
		}		
	}

}

void sh2_do_dma(sh2_state *sh2, int dma)
{
	UINT32 dmadata;

	UINT32 tempsrc, tempdst;

	if (sh2->active_dma_count[dma] > 0)
	{


		// process current DMA
		switch(sh2->active_dma_size[dma])
		{
		case 0:
			{
				// we need to know the src / dest ahead of time without changing them
				// to allow for the callback to check if we can process the DMA at this
				// time (we need to know where we're reading / writing to/from)

				if(sh2->active_dma_incs[dma] == 2)
					tempsrc = sh2->active_dma_src[dma] - 1;
				else
					tempsrc = sh2->active_dma_src[dma];

				if(sh2->active_dma_incd[dma] == 2)
					tempdst = sh2->active_dma_dst[dma] - 1;
				else
					tempdst = sh2->active_dma_dst[dma];

				if (sh2->dma_callback_fifo_data_available)
				{
					int available = sh2->dma_callback_fifo_data_available(tempsrc, tempdst, 0, sh2->active_dma_size[dma]);
	
					if (!available)
					{
						//printf("dma stalled\n");
						sh2->dma_timer_active[dma]=2;// mark as stalled
						return;
					}
				}
			
				#ifdef USE_TIMER_FOR_DMA
				 //schedule next DMA callback
				timer_adjust_oneshot(sh2->dma_current_active_timer[dma], sh2->device->cycles_to_attotime(2), dma);
				#endif
		

				dmadata = sh2->program->read_byte(tempsrc);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_byte(tempdst, dmadata);

				if(sh2->active_dma_incs[dma] == 2)
					sh2->active_dma_src[dma] --;
				if(sh2->active_dma_incd[dma] == 2)
					sh2->active_dma_dst[dma] --;


				if(sh2->active_dma_incs[dma] == 1)
					sh2->active_dma_src[dma] ++;
				if(sh2->active_dma_incd[dma] == 1)
					sh2->active_dma_dst[dma] ++;

				sh2->active_dma_count[dma] --;
			}
			break;
		case 1:
			{
				if(sh2->active_dma_incs[dma] == 2)
					tempsrc = sh2->active_dma_src[dma] - 2;
				else
					tempsrc = sh2->active_dma_src[dma];

				if(sh2->active_dma_incd[dma] == 2)
					tempdst = sh2->active_dma_dst[dma] - 2;
				else
					tempdst = sh2->active_dma_dst[dma];

				if (sh2->dma_callback_fifo_data_available)
				{
					int available = sh2->dma_callback_fifo_data_available(tempsrc, tempdst, 0, sh2->active_dma_size[dma]);
	
					if (!available)
					{
						//printf("dma stalled\n");
						sh2->dma_timer_active[dma]=2;// mark as stalled
						return;
					}
				}

				#ifdef USE_TIMER_FOR_DMA
				 //schedule next DMA callback
				timer_adjust_oneshot(sh2->dma_current_active_timer[dma], sh2->device->cycles_to_attotime(2), dma);
				#endif

				// check: should this really be using read_word_32 / write_word_32?
				dmadata	= sh2->program->read_word(tempsrc);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_word(tempdst, dmadata);

				if(sh2->active_dma_incs[dma] == 2)
					sh2->active_dma_src[dma] -= 2;
				if(sh2->active_dma_incd[dma] == 2)
					sh2->active_dma_dst[dma] -= 2;

				if(sh2->active_dma_incs[dma] == 1)
					sh2->active_dma_src[dma] += 2;
				if(sh2->active_dma_incd[dma] == 1)
					sh2->active_dma_dst[dma] += 2;
			
				sh2->active_dma_count[dma] --;
			}
			break;
		case 2:
			{
				if(sh2->active_dma_incs[dma] == 2)
					tempsrc = sh2->active_dma_src[dma] - 4;
				else
					tempsrc = sh2->active_dma_src[dma];

				if(sh2->active_dma_incd[dma] == 2)
					tempdst = sh2->active_dma_dst[dma] - 4;
				else
					tempdst = sh2->active_dma_dst[dma];

				if (sh2->dma_callback_fifo_data_available)
				{
					int available = sh2->dma_callback_fifo_data_available(tempsrc, tempdst, 0, sh2->active_dma_size[dma]);
	
					if (!available)
					{
						//printf("dma stalled\n");
						sh2->dma_timer_active[dma]=2;// mark as stalled
						return;
					}
				}

				#ifdef USE_TIMER_FOR_DMA
				 //schedule next DMA callback
				timer_adjust_oneshot(sh2->dma_current_active_timer[dma], sh2->device->cycles_to_attotime(2), dma);
				#endif

				dmadata	= sh2->program->read_dword(tempsrc);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_dword(tempdst, dmadata);

				if(sh2->active_dma_incs[dma] == 2)
					sh2->active_dma_src[dma] -= 4;
				if(sh2->active_dma_incd[dma] == 2)
					sh2->active_dma_dst[dma] -= 4;

				if(sh2->active_dma_incs[dma] == 1)
					sh2->active_dma_src[dma] += 4;
				if(sh2->active_dma_incd[dma] == 1)
					sh2->active_dma_dst[dma] += 4;

				sh2->active_dma_count[dma] --;
			}
			break;
		case 3:
			{
				// shouldn't this really be 4 calls here instead?

				tempsrc = sh2->active_dma_src[dma];

				if(sh2->active_dma_incd[dma] == 2)
					tempdst = sh2->active_dma_dst[dma] - 16;
				else
					tempdst = sh2->active_dma_dst[dma];

				if (sh2->dma_callback_fifo_data_available)
				{
					int available = sh2->dma_callback_fifo_data_available(tempsrc, tempdst, 0, sh2->active_dma_size[dma]);
	
					if (!available)
					{
						//printf("dma stalled\n");
						sh2->dma_timer_active[dma]=2;// mark as stalled
						fatalerror("SH2 dma_callback_fifo_data_available == 0 in unsupported mode");
					}
				}

				#ifdef USE_TIMER_FOR_DMA
				 //schedule next DMA callback
				timer_adjust_oneshot(sh2->dma_current_active_timer[dma], sh2->device->cycles_to_attotime(2), dma);
				#endif

				dmadata = sh2->program->read_dword(tempsrc);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_dword(tempdst, dmadata);

				dmadata = sh2->program->read_dword(tempsrc+4);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_dword(tempdst+4, dmadata);

				dmadata = sh2->program->read_dword(tempsrc+8);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_dword(tempdst+8, dmadata);

				dmadata = sh2->program->read_dword(tempsrc+12);
				if (sh2->dma_callback_kludge) dmadata = sh2->dma_callback_kludge(tempsrc, tempdst, dmadata, sh2->active_dma_size[dma]);
				sh2->program->write_dword(tempdst+12, dmadata);

				if(sh2->active_dma_incd[dma] == 2)
					sh2->active_dma_dst[dma] -= 16;

				sh2->active_dma_src[dma] += 16;
				if(sh2->active_dma_incd[dma] == 1)
					sh2->active_dma_dst[dma] += 16;
			
				sh2->active_dma_count[dma]-=4;
			}
			break;
		}
	}
	else // the dma is complete
	{
	//	int dma = param & 1;
	//	sh2_state *sh2 = (sh2_state *)ptr;

		// fever soccer uses cycle-stealing mode, resume the CPU now DMA has finished
		if (sh2->active_dma_steal[dma])
		{
			sh2->device->resume(SUSPEND_REASON_HALT );
		}
			

		LOG(("SH2.%s: DMA %d complete\n", sh2->device->tag(), dma));
		sh2->m[0x63+4*dma] |= 2;
		sh2->dma_timer_active[dma] = 0;
		sh2_recalc_irq(sh2);

	}
}

static TIMER_CALLBACK( sh2_dma_current_active_callback )
{
	int dma = param & 1;
	sh2_state *sh2 = (sh2_state *)ptr;

	sh2_do_dma(sh2, dma);
}


static void sh2_dmac_check(sh2_state *sh2, int dma)
{
	if(sh2->m[0x63+4*dma] & sh2->m[0x6c] & 1)
	{
		if(!sh2->dma_timer_active[dma] && !(sh2->m[0x63+4*dma] & 2))
		{

			sh2->active_dma_incd[dma] = (sh2->m[0x63+4*dma] >> 14) & 3;
			sh2->active_dma_incs[dma] = (sh2->m[0x63+4*dma] >> 12) & 3;
			sh2->active_dma_size[dma] = (sh2->m[0x63+4*dma] >> 10) & 3;
			sh2->active_dma_steal[dma] = (sh2->m[0x63+4*dma] &0x10);
			
			if(sh2->active_dma_incd[dma] == 3 || sh2->active_dma_incs[dma] == 3)
			{
				logerror("SH2: DMA: bad increment values (%d, %d, %d, %04x)\n", sh2->active_dma_incd[dma], sh2->active_dma_incs[dma], sh2->active_dma_size[dma], sh2->m[0x63+4*dma]);
				return;
			}
			sh2->active_dma_src[dma]   = sh2->m[0x60+4*dma];
			sh2->active_dma_dst[dma]   = sh2->m[0x61+4*dma];
			sh2->active_dma_count[dma] = sh2->m[0x62+4*dma];
			if(!sh2->active_dma_count[dma])
				sh2->active_dma_count[dma] = 0x1000000;

			LOG(("SH2: DMA %d start %x, %x, %x, %04x, %d, %d, %d\n", dma, sh2->active_dma_src[dma], sh2->active_dma_dst[dma], sh2->active_dma_count[dma], sh2->m[0x63+4*dma], sh2->active_dma_incs[dma], sh2->active_dma_incd[dma], sh2->active_dma_size[dma]));

			sh2->dma_timer_active[dma] = 1;

			sh2->active_dma_src[dma] &= AM;
			sh2->active_dma_dst[dma] &= AM;

			switch(sh2->active_dma_size[dma])
			{
			case 0:
				break;
			case 1:
				sh2->active_dma_src[dma] &= ~1;
				sh2->active_dma_dst[dma] &= ~1;	
				break;
			case 2:
				sh2->active_dma_src[dma] &= ~3;
				sh2->active_dma_dst[dma] &= ~3;
				break;
			case 3:
				sh2->active_dma_src[dma] &= ~3;
				sh2->active_dma_dst[dma] &= ~3;
				sh2->active_dma_count[dma] &= ~3;
				break;
			}
			
			

			
#ifdef USE_TIMER_FOR_DMA
			// start DMA timer
			
			// fever soccer uses cycle-stealing mode, requiring the CPU to be halted
			if (sh2->active_dma_steal[dma])
			{
				//printf("cycle stealing DMA\n");
				sh2->device->suspend(SUSPEND_REASON_HALT, 1 );
			}
			
			timer_adjust_oneshot(sh2->dma_current_active_timer[dma], sh2->device->cycles_to_attotime(2), dma);
#endif

		}
	}
	else
	{
		if(sh2->dma_timer_active[dma])
		{
			logerror("SH2: DMA %d cancelled in-flight\n", dma);
			//timer_adjust_oneshot(sh2->dma_complete_timer[dma], attotime_never, 0);
			timer_adjust_oneshot(sh2->dma_current_active_timer[dma], attotime_never, 0);
		
			sh2->dma_timer_active[dma] = 0;
		}
	}
}

WRITE32_HANDLER( sh2_internal_w )
{
	sh2_state *sh2 = GET_SH2(space->cpu);
	UINT32 old;

#ifdef USE_SH2DRC
	offset &= 0x7f;
#endif

	old = sh2->m[offset];
	COMBINE_DATA(sh2->m+offset);

	//  if(offset != 0x20)
	//      logerror("sh2_internal_w:  Write %08x (%x), %08x @ %08x\n", 0xfffffe00+offset*4, offset, data, mem_mask);

//    if(offset != 0x20)
//        printf("sh2_internal_w:  Write %08x (%x), %08x @ %08x (PC %x)\n", 0xfffffe00+offset*4, offset, data, mem_mask, cpu_get_pc(space->cpu));

	switch( offset )
	{
		// Timers
	case 0x04: // TIER, FTCSR, FRC
		if((mem_mask & 0x00ffffff) != 0)
			sh2_timer_resync(sh2);
//      printf("SH2.%s: TIER write %04x @ %04x\n", sh2->device->tag(), data >> 16, mem_mask>>16);
		sh2->m[4] = (sh2->m[4] & ~(ICF|OCFA|OCFB|OVF)) | (old & sh2->m[4] & (ICF|OCFA|OCFB|OVF));
		COMBINE_DATA(&sh2->frc);
		if((mem_mask & 0x00ffffff) != 0)
			sh2_timer_activate(sh2);
		sh2_recalc_irq(sh2);
		break;
	case 0x05: // OCRx, TCR, TOCR
//      printf("SH2.%s: TCR write %08x @ %08x\n", sh2->device->tag(), data, mem_mask);
		sh2_timer_resync(sh2);
		if(sh2->m[5] & 0x10)
			sh2->ocrb = (sh2->ocrb & (~mem_mask >> 16)) | ((data & mem_mask) >> 16);
		else
			sh2->ocra = (sh2->ocra & (~mem_mask >> 16)) | ((data & mem_mask) >> 16);
		sh2_timer_activate(sh2);
		break;

	case 0x06: // ICR
		break;

		// Interrupt vectors
	case 0x18: // IPRB, VCRA
	case 0x19: // VCRB, VCRC
	case 0x1a: // VCRD
		sh2_recalc_irq(sh2);
		break;

		// DMA
	case 0x1c: // DRCR0, DRCR1
		break;

		// Watchdog
	case 0x20: // WTCNT, RSTCSR
		break;

		// Standby and cache
	case 0x24: // SBYCR, CCR
		break;

		// Interrupt vectors cont.
	case 0x38: // ICR, IRPA
		break;
	case 0x39: // VCRWDT
		break;

		// Division box
	case 0x40: // DVSR
		break;
	case 0x41: // DVDNT
		{
			INT32 a = sh2->m[0x41];
			INT32 b = sh2->m[0x40];
			LOG(("SH2 '%s' div+mod %d/%d\n", sh2->device->tag(), a, b));
			if (b)
			{
				sh2->m[0x45] = a / b;
				sh2->m[0x44] = a % b;
			}
			else
			{
				sh2->m[0x42] |= 0x00010000;
				sh2->m[0x45] = 0x7fffffff;
				sh2->m[0x44] = 0x7fffffff;
				sh2_recalc_irq(sh2);
			}
			break;
		}
	case 0x42: // DVCR
		sh2->m[0x42] = (sh2->m[0x42] & ~0x00001000) | (old & sh2->m[0x42] & 0x00010000);
		sh2_recalc_irq(sh2);
		break;
	case 0x43: // VCRDIV
		sh2_recalc_irq(sh2);
		break;
	case 0x44: // DVDNTH
		break;
	case 0x45: // DVDNTL
		{
			INT64 a = sh2->m[0x45] | ((UINT64)(sh2->m[0x44]) << 32);
			INT64 b = (INT32)sh2->m[0x40];
			LOG(("SH2 '%s' div+mod %" I64FMT "d/%" I64FMT "d\n", sh2->device->tag(), a, b));
			if (b)
			{
				INT64 q = a / b;
				if (q != (INT32)q)
				{
					sh2->m[0x42] |= 0x00010000;
					sh2->m[0x45] = 0x7fffffff;
					sh2->m[0x44] = 0x7fffffff;
					sh2_recalc_irq(sh2);
				}
				else
				{
					sh2->m[0x45] = q;
					sh2->m[0x44] = a % b;
				}
			}
			else
			{
				sh2->m[0x42] |= 0x00010000;
				sh2->m[0x45] = 0x7fffffff;
				sh2->m[0x44] = 0x7fffffff;
				sh2_recalc_irq(sh2);
			}
			break;
		}

		// DMA controller
	case 0x60: // SAR0
	case 0x61: // DAR0
		break;
	case 0x62: // DTCR0
		sh2->m[0x62] &= 0xffffff;
		break;
	case 0x63: // CHCR0
		sh2->m[0x63] = (sh2->m[0x63] & ~2) | (old & sh2->m[0x63] & 2);
		sh2_dmac_check(sh2, 0);
		break;
	case 0x64: // SAR1
	case 0x65: // DAR1
		break;
	case 0x66: // DTCR1
		sh2->m[0x66] &= 0xffffff;
		break;
	case 0x67: // CHCR1
		sh2->m[0x67] = (sh2->m[0x67] & ~2) | (old & sh2->m[0x67] & 2);
		sh2_dmac_check(sh2, 1);
		break;
	case 0x68: // VCRDMA0
	case 0x6a: // VCRDMA1
		sh2_recalc_irq(sh2);
		break;
	case 0x6c: // DMAOR
		sh2->m[0x6c] = (sh2->m[0x6c] & ~6) | (old & sh2->m[0x6c] & 6);
		sh2_dmac_check(sh2, 0);
		sh2_dmac_check(sh2, 1);
		break;

		// Bus controller
	case 0x78: // BCR1
	case 0x79: // BCR2
	case 0x7a: // WCR
	case 0x7b: // MCR
	case 0x7c: // RTCSR
	case 0x7d: // RTCNT
	case 0x7e: // RTCOR
		break;

	default:
		logerror("sh2_internal_w:  Unmapped write %08x, %08x @ %08x\n", 0xfffffe00+offset*4, data, mem_mask);
		break;
	}
}

READ32_HANDLER( sh2_internal_r )
{
	sh2_state *sh2 = GET_SH2(space->cpu);

#ifdef USE_SH2DRC
	offset &= 0x7f;
#endif
	//  logerror("sh2_internal_r:  Read %08x (%x) @ %08x\n", 0xfffffe00+offset*4, offset, mem_mask);
	switch( offset )
	{
	case 0x04: // TIER, FTCSR, FRC
		if ( mem_mask == 0x00ff0000 )
			if ( sh2->ftcsr_read_callback != NULL )
				sh2->ftcsr_read_callback( (sh2->m[4] & 0xffff0000) | sh2->frc );
		sh2_timer_resync(sh2);
		return (sh2->m[4] & 0xffff0000) | sh2->frc;
	case 0x05: // OCRx, TCR, TOCR
		if(sh2->m[5] & 0x10)
			return (sh2->ocrb << 16) | (sh2->m[5] & 0xffff);
		else
			return (sh2->ocra << 16) | (sh2->m[5] & 0xffff);
	case 0x06: // ICR
		return sh2->icr << 16;

	case 0x38: // ICR, IPRA
		return (sh2->m[0x38] & 0x7fffffff) | (sh2->nmi_line_state == ASSERT_LINE ? 0 : 0x80000000);

	case 0x78: // BCR1
		return sh2->is_slave ? 0x00008000 : 0;

	case 0x41: // dvdntl mirrors
	case 0x47:
		return sh2->m[0x45];

	case 0x46: // dvdnth mirror
		return sh2->m[0x44];
	}
	return sh2->m[offset];
}

void sh2_set_ftcsr_read_callback(running_device *device, void (*callback)(UINT32))
{
	sh2_state *sh2 = GET_SH2(device);
	sh2->ftcsr_read_callback = callback;
}

void sh2_set_frt_input(running_device *device, int state)
{
	sh2_state *sh2 = GET_SH2(device);

	if(state == PULSE_LINE)
	{
		sh2_set_frt_input(device, ASSERT_LINE);
		sh2_set_frt_input(device, CLEAR_LINE);
		return;
	}

	if(sh2->frt_input == state) {
		return;
	}

	sh2->frt_input = state;

	if(sh2->m[5] & 0x8000) {
		if(state == CLEAR_LINE) {
			return;
		}
	} else {
		if(state == ASSERT_LINE) {
			return;
		}
	}

	sh2_timer_resync(sh2);
	sh2->icr = sh2->frc;
	sh2->m[4] |= ICF;
	logerror("SH2.%s: ICF activated (%x)\n", sh2->device->tag(), sh2->pc & AM);
	sh2_recalc_irq(sh2);
}

void sh2_set_irq_line(sh2_state *sh2, int irqline, int state)
{
	if (irqline == INPUT_LINE_NMI)
	{
		if (sh2->nmi_line_state == state)
			return;
		sh2->nmi_line_state = state;

		if( state == CLEAR_LINE )
		{
			LOG(("SH-2 '%s' cleared nmi\n", sh2->device->tag()));
		}
		else
		{
			LOG(("SH-2 '%s' assert nmi\n", sh2->device->tag()));

			sh2_exception(sh2, "Set IRQ line", 16);

			#ifdef USE_SH2DRC
			sh2->pending_nmi = 1;
			#endif
		}
	}
	else
	{
		if (sh2->irq_line_state[irqline] == state)
			return;
		sh2->irq_line_state[irqline] = state;

		if( state == CLEAR_LINE )
		{
			LOG(("SH-2 '%s' cleared irq #%d\n", sh2->device->tag(), irqline));
			sh2->pending_irq &= ~(1 << irqline);
		}
		else
		{
			LOG(("SH-2 '%s' assert irq #%d\n", sh2->device->tag(), irqline));
			sh2->pending_irq |= 1 << irqline;
			#ifdef USE_SH2DRC
			sh2->test_irq = 1;
			#else
			if(sh2->delay)
				sh2->test_irq = 1;
			else
				CHECK_PENDING_IRQ("sh2_set_irq_line");
			#endif
		}
	}
}

void sh2_recalc_irq(sh2_state *sh2)
{
	int irq = 0, vector = -1;
	int  level;

	// Timer irqs
	if((sh2->m[4]>>8) & sh2->m[4] & (ICF|OCFA|OCFB|OVF))
	{
		level = (sh2->m[0x18] >> 24) & 15;
		if(level > irq)
		{
			int mask = (sh2->m[4]>>8) & sh2->m[4];
			irq = level;
			if(mask & ICF)
				vector = (sh2->m[0x19] >> 8) & 0x7f;
			else if(mask & (OCFA|OCFB))
				vector = sh2->m[0x19] & 0x7f;
			else
				vector = (sh2->m[0x1a] >> 24) & 0x7f;
		}
	}

	// DMA irqs
	if((sh2->m[0x63] & 6) == 6) {
		level = (sh2->m[0x38] >> 8) & 15;
		if(level > irq) {
			irq = level;
			vector = (sh2->m[0x68] >> 24) & 0x7f;
		}
	}

	if((sh2->m[0x67] & 6) == 6) {
		level = (sh2->m[0x38] >> 8) & 15;
		if(level > irq) {
			irq = level;
			vector = (sh2->m[0x6a] >> 24) & 0x7f;
		}
	}

	sh2->internal_irq_level = irq;
	sh2->internal_irq_vector = vector;
	sh2->test_irq = 1;
}

void sh2_exception(sh2_state *sh2, const char *message, int irqline)
{
	int vector;

	if (irqline != 16)
	{
		if (irqline <= ((sh2->sr >> 4) & 15)) /* If the cpu forbids this interrupt */
			return;

		// if this is an sh2 internal irq, use its vector
		if (sh2->internal_irq_level == irqline)
		{
			vector = sh2->internal_irq_vector;
			LOG(("SH-2 '%s' exception #%d (internal vector: $%x) after [%s]\n", sh2->device->tag(), irqline, vector, message));
		}
		else
		{
			if(sh2->m[0x38] & 0x00010000)
			{
				vector = sh2->irq_callback(sh2->device, irqline);
				LOG(("SH-2 '%s' exception #%d (external vector: $%x) after [%s]\n", sh2->device->tag(), irqline, vector, message));
			}
			else
			{
				sh2->irq_callback(sh2->device, irqline);
				vector = 64 + irqline/2;
				LOG(("SH-2 '%s' exception #%d (autovector: $%x) after [%s]\n", sh2->device->tag(), irqline, vector, message));
			}
		}
	}
	else
	{
		vector = 11;
		LOG(("SH-2 '%s' nmi exception (autovector: $%x) after [%s]\n", sh2->device->tag(), vector, message));
	}

	#ifdef USE_SH2DRC
	sh2->evec = RL( sh2, sh2->vbr + vector * 4 );
	sh2->evec &= AM;
	sh2->irqsr = sh2->sr;

	/* set I flags in SR */
	if (irqline > SH2_INT_15)
		sh2->sr = sh2->sr | I;
	else
		sh2->sr = (sh2->sr & ~I) | (irqline << 4);

//  printf("sh2_exception [%s] irqline %x evec %x save SR %x new SR %x\n", message, irqline, sh2->evec, sh2->irqsr, sh2->sr);
	#else
	sh2->r[15] -= 4;
	WL( sh2, sh2->r[15], sh2->sr );		/* push SR onto stack */
	sh2->r[15] -= 4;
	WL( sh2, sh2->r[15], sh2->pc );		/* push PC onto stack */

	/* set I flags in SR */
	if (irqline > SH2_INT_15)
		sh2->sr = sh2->sr | I;
	else
		sh2->sr = (sh2->sr & ~I) | (irqline << 4);

	/* fetch PC */
	sh2->pc = RL( sh2, sh2->vbr + vector * 4 );
	#endif
}

void sh2_common_init(sh2_state *sh2, legacy_cpu_device *device, device_irq_callback irqcallback)
{
	const sh2_cpu_core *conf = (const sh2_cpu_core *)device->baseconfig().static_config();
	int i;

	sh2->timer = timer_alloc(device->machine, sh2_timer_callback, sh2);
	timer_adjust_oneshot(sh2->timer, attotime_never, 0);

	sh2->dma_current_active_timer[0] = timer_alloc(device->machine, sh2_dma_current_active_callback, sh2);
	timer_adjust_oneshot(sh2->dma_current_active_timer[0], attotime_never, 0);

	sh2->dma_current_active_timer[1] = timer_alloc(device->machine, sh2_dma_current_active_callback, sh2);
	timer_adjust_oneshot(sh2->dma_current_active_timer[1], attotime_never, 0);


	sh2->m = auto_alloc_array(device->machine, UINT32, 0x200/4);

	if(conf)
	{
		sh2->is_slave = conf->is_slave;
		sh2->dma_callback_kludge = conf->dma_callback_kludge;
		sh2->dma_callback_fifo_data_available = conf->dma_callback_fifo_data_available;
	}
	else
	{
		sh2->is_slave = 0;
		sh2->dma_callback_kludge = NULL;
		sh2->dma_callback_fifo_data_available = NULL;

	}
	sh2->irq_callback = irqcallback;
	sh2->device = device;
	sh2->program = device->space(AS_PROGRAM);
	sh2->direct = &sh2->program->direct();
	sh2->internal = device->space(AS_PROGRAM);

	state_save_register_device_item(device, 0, sh2->pc);
	state_save_register_device_item(device, 0, sh2->sr);
	state_save_register_device_item(device, 0, sh2->pr);
	state_save_register_device_item(device, 0, sh2->gbr);
	state_save_register_device_item(device, 0, sh2->vbr);
	state_save_register_device_item(device, 0, sh2->mach);
	state_save_register_device_item(device, 0, sh2->macl);
	state_save_register_device_item_array(device, 0, sh2->r);
	state_save_register_device_item(device, 0, sh2->ea);
	state_save_register_device_item(device, 0, sh2->delay);
	state_save_register_device_item(device, 0, sh2->cpu_off);
	state_save_register_device_item(device, 0, sh2->dvsr);
	state_save_register_device_item(device, 0, sh2->dvdnth);
	state_save_register_device_item(device, 0, sh2->dvdntl);
	state_save_register_device_item(device, 0, sh2->dvcr);
	state_save_register_device_item(device, 0, sh2->pending_irq);
	state_save_register_device_item(device, 0, sh2->test_irq);
	state_save_register_device_item(device, 0, sh2->pending_nmi);
	state_save_register_device_item(device, 0, sh2->irqline);
	state_save_register_device_item(device, 0, sh2->evec);
	state_save_register_device_item(device, 0, sh2->irqsr);
	state_save_register_device_item(device, 0, sh2->target);
	for (i = 0; i < 16; ++i)
	{
		state_save_register_device_item(device, i, sh2->irq_queue[i].irq_vector);
		state_save_register_device_item(device, i, sh2->irq_queue[i].irq_priority);
	}
	state_save_register_device_item(device, 0, sh2->pcfsel);
	state_save_register_device_item(device, 0, sh2->maxpcfsel);
	state_save_register_device_item_array(device, 0, sh2->pcflushes);
	state_save_register_device_item_array(device, 0, sh2->irq_line_state);
	state_save_register_device_item_pointer(device, 0, sh2->m, 0x200/4);
	state_save_register_device_item(device, 0, sh2->nmi_line_state);
	state_save_register_device_item(device, 0, sh2->frc);
	state_save_register_device_item(device, 0, sh2->ocra);
	state_save_register_device_item(device, 0, sh2->ocrb);
	state_save_register_device_item(device, 0, sh2->icr);
	state_save_register_device_item(device, 0, sh2->frc_base);
	state_save_register_device_item(device, 0, sh2->frt_input);
	state_save_register_device_item(device, 0, sh2->internal_irq_level);
	state_save_register_device_item(device, 0, sh2->internal_irq_vector);
	state_save_register_device_item_array(device, 0, sh2->dma_timer_active);
}

