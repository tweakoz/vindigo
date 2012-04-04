/* 68307 TIMER module */
// 2x timers

#include "emu.h"
#include "m68kcpu.h"

READ16_HANDLER( m68307_internal_timer_r )
{
	m68ki_cpu_core *m68k = m68k_get_safe_token(&space->device());
	m68307_timer* timer = m68k->m68307TIMER;
	assert(timer != NULL);

	if (timer)
	{
		int pc = cpu_get_pc(&space->device());
		int which = offset & 0x8;

		switch (offset&0x7)
		{
			case m68307TIMER_TCN: /* 0x3 (0x126 / 0x136) */
				//if (pc!=0x2182e) logerror("%08x m68307_internal_timer_r %08x (%04x) (TCN - Timer Counter for timer %d)\n", pc, offset*2,mem_mask, which);
				return timer->read_tcn(mem_mask, which);
				break;

			
			default:
				logerror("%08x m68307_internal_timer_r %08x, (%04x)\n", pc, offset*2,mem_mask);
				break;
		}
	}
		
	return 0x0000;
}

WRITE16_HANDLER( m68307_internal_timer_w )
{
	m68ki_cpu_core *m68k = m68k_get_safe_token(&space->device());
	m68307_timer* timer = m68k->m68307TIMER;
	assert(timer != NULL);

	if (timer)
	{
		int pc = cpu_get_pc(&space->device());
		int which = offset & 0x8;

		switch (offset&0x7)
		{
			case m68307TIMER_TMR: /* 0x0 (0x120 / 0x130) */
				logerror("%08x m68307_internal_timer_w %08x, %04x (%04x) (TMR - Timer Mode Register for timer %d)\n", pc, offset*2,data,mem_mask, which);
				timer->write_tmr(data, mem_mask, which);
				break;

			case m68307TIMER_TRR: /* 0x1 (0x122 / 0x132) */
				logerror("%08x m68307_internal_timer_w %08x, %04x (%04x) (TRR - Timer Reference Register for timer %d)\n", pc, offset*2,data,mem_mask, which);
				timer->write_trr(data, mem_mask, which);
				break;

			case m68307TIMER_TCR: /* 0x2 (0x124 / 0x134) */
				logerror("%08x m68307_internal_timer_w %08x, %04x (%04x) (TCR - Timer Capture Register for timer %d) (illegal, read-only)\n", pc, offset*2,data,mem_mask, which);
				break;

			case m68307TIMER_TCN: /* 0x3 (0x126 / 0x136) */
				logerror("%08x m68307_internal_timer_w %08x, %04x (%04x) (TCN - Timer Counter for timer %d)\n", pc, offset*2,data,mem_mask, which);
				break;

			case m68307TIMER_TER: /* 0x4 (0x128 / 0x138) */
				/* 8-bit only!! */
				//logerror("%08x m68307_internal_timer_w %08x, %04x (%04x) (TER - Timer Event Register for timer %d)\n", pc, offset*2,data,mem_mask, which);
				timer->write_ter(data, mem_mask, which);
				break;

			case m68307TIMER_WRR: /* 0x5 (0x12a / 0x13a) */
				if (which==0)
				{
					logerror("%08x m68307_internal_timer_w %08x, %04x (%04x) (WRR - Watchdog Reference Register)\n", pc, offset*2,data,mem_mask);
				}
				else
				{
					logerror("%08x m68307_internal_timer_w %08x, %04x (%04x) (illegal)\n", pc, offset*2,data,mem_mask);
				}
				break;

			case m68307TIMER_WCR: /* 0x6 (0x12c / 0x13c) */
				if (which==0)
				{
					logerror("%08x m68307_internal_timer_w %08x, %04x (%04x) (WRR - Watchdog Counter Register)\n", pc, offset*2,data,mem_mask);
				}
				else
				{
					logerror("%08x m68307_internal_timer_w %08x, %04x (%04x) (illegal)\n", pc, offset*2,data,mem_mask);
				}
				break;

			case m68307TIMER_XXX: /* 0x7 (0x12e / 0x13e) */
				logerror("%08x m68307_internal_timer_w %08x, %04x (%04x) (illegal)\n", pc, offset*2,data,mem_mask);
				break;

		}
	}

//	printf("%d\n", (UINT32)m68k->device->total_cycles());

}

static TIMER_CALLBACK( m68307_timer0_callback )
{
//	printf("timer0\n");


	legacy_cpu_device *dev = (legacy_cpu_device *)ptr;
	m68ki_cpu_core* m68k = m68k_get_safe_token(dev);



	int prioritylevel = (m68k->m68307SIM->m_picr & 0x7000)>>12;
	int vector        = (m68k->m68307SIM->m_pivr & 0x00f0) | 0xa;



	m68307_single_timer* tptr = &m68k->m68307TIMER->singletimer[0];
	tptr->regs[m68307TIMER_TMR] |= 0x2;
	m68307_set_interrupt(dev, prioritylevel, vector);


	tptr->mametimer->adjust(m68k->device->cycles_to_attotime(100000));
}

static TIMER_CALLBACK( m68307_timer1_callback )
{
//	int prioritylevel = (m68k->m68307SIM->m_picr & 0x0700)>>8;
//	int vector        = (m68k->m68307SIM->m_pivr & 0x00f0) | 0xb;

	printf("timer1\n");
}

static TIMER_CALLBACK( m68307_wd_timer_callback )
{
	printf("wd timer\n");
}

void m68307_timer::init(legacy_cpu_device *device)
{

	//m68ki_cpu_core *m68k;
	
	//m68k = m68k_get_safe_token(device);


	m68307_single_timer* tptr;
			
	tptr = &singletimer[0];
	tptr->mametimer = device->machine().scheduler().timer_alloc(FUNC(m68307_timer0_callback), device);

	tptr = &singletimer[1];
	tptr->mametimer = device->machine().scheduler().timer_alloc(FUNC(m68307_timer1_callback), device);

	parent = device;

	wd_mametimer = device->machine().scheduler().timer_alloc(FUNC(m68307_wd_timer_callback), device);


}

UINT16 m68307_timer::read_tcn(UINT16 mem_mask, int which)
{
//	return 0;
	return 0x411d;
//	return 0x3a98;
}

void m68307_timer::write_ter(UINT16 data, UINT16 mem_mask, int which)
{
//	m68ki_cpu_core* m68k = m68k_get_safe_token(parent);
	m68307_single_timer* tptr = &singletimer[which];

//	UINT16 ter = tptr->regs[m68307TIMER_TMR;
	if (data & 0x2) tptr->regs[m68307TIMER_TMR] &= ~0x2;

//	COMBINE_DATA(&tptr->regs[m68307TIMER_TMR]);

}

void m68307_timer::write_tmr(UINT16 data, UINT16 mem_mask, int which)
{
	m68ki_cpu_core* m68k = m68k_get_safe_token(parent);
	m68307_single_timer* tptr = &singletimer[which];

	COMBINE_DATA(&tptr->regs[m68307TIMER_TMR]);

	data = tptr->regs[m68307TIMER_TMR];

	int ps   = data & (0xff00)>>8;
	int ce   = data & (0x00c0)>>6;
	int om   = data & (0x0020)>>5;
	int ori  = data & (0x0010)>>4;
	int frr  = data & (0x0008)>>3;
	int iclk = data & (0x0006)>>1;
	int rst  = data & (0x0001)>>0;


	logerror("tmr value %04x : Details :\n", data);
	logerror("prescale %d\n", ps);
	logerror("(clock divided by %d)\n", ps+1);
	logerror("capture edge / enable interrupt %d\n", ce);
	if (ce==0x0) logerror("(disable interrupt on capture event)\n");
	if (ce==0x1) logerror("(capture on rising edge only + enable capture interrupt)\n");
	if (ce==0x2) logerror("(capture on falling edge only + enable capture interrupt)\n");
	if (ce==0x3) logerror("(capture on any edge + enable capture interrupt)\n");
	logerror("output mode %d\n", om);
	if (om==0x0) logerror("(active-low pulse for one cycle))\n");
	if (om==0x1) logerror("(toggle output)\n");
	logerror("output reference interrupt %d\n", ori);
	if (ori==0x0) logerror("(disable reference interrupt)\n");
	if (ori==0x1) logerror("(enable interrupt on reaching reference value))\n");
	logerror("free running %d\n", frr);
	if (frr==0x0) logerror("(free running mode, counter continues after value reached)\n");
	if (frr==0x1) logerror("(restart mode, counter resets after value reached)\n");
	logerror("interrupt clock source %d\n", iclk);
	if (iclk==0x0) logerror("(stop count)\n");
	if (iclk==0x1) logerror("(master system clock)\n");
	if (iclk==0x2) logerror("(master system clock divided by 16)\n");
	if (iclk==0x3) logerror("(TIN Pin)\n");
	logerror("reset %d\n", rst);
	if (rst==0x0) logerror("(timer is reset)\n");
	if (rst==0x1) logerror("(timer is running)\n");

	tptr->mametimer->adjust(m68k->device->cycles_to_attotime(100000));

	logerror("\n");

}

void m68307_timer::write_trr(UINT16 data, UINT16 mem_mask, int which)
{
	m68307_single_timer* tptr = &singletimer[which];

	COMBINE_DATA(&tptr->regs[m68307TIMER_TRR]);
}



void m68307_timer::reset(void)
{
	for (int i=0;i<2;i++)
	{
		m68307_single_timer* tptr = &singletimer[i];

		tptr->regs[m68307TIMER_TMR] = 0x0000;
		tptr->regs[m68307TIMER_TRR] = 0xffff;
		tptr->regs[m68307TIMER_TCR] = 0x0000;
		tptr->regs[m68307TIMER_TCN] = 0x0000;
		tptr->regs[m68307TIMER_TER] = 0x0000;
		tptr->regs[m68307TIMER_WRR] = 0xffff;
		tptr->regs[m68307TIMER_WCR] = 0xffff;
		tptr->regs[m68307TIMER_XXX] = 0;
		tptr->enabled = false;
		tptr->mametimer->adjust(attotime::never);
	}

	wd_mametimer->adjust(attotime::never);


}

