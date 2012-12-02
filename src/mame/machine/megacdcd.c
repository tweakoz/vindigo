/* CD controller code from megacd.c, used by Sega Cd / Mega CD */

/* todo: cleanup(!!), make more generic, unifiy implementation with NeoCD, turn into a device and move to the proper lc89510.c file
  currently this is a bit of a mix of system specific bits, incomplete implementations etc. as well as a rather kludgy combination
  of the CDC and drive emulation. */


#include "emu.h"
#include "machine/megacdcd.h"

const device_type LC89510_TEMP = &device_creator<lc89510_temp_device>;

lc89510_temp_device::lc89510_temp_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, LC89510_TEMP, "lc89510_temp_device", tag, owner, clock)
{
	segacd_dma_callback =  segacd_dma_delegate(FUNC(lc89510_temp_device::Fake_CDC_Do_DMA), this);
	is_neoCD = false;

	nff0002 = 0;
	nIRQAcknowledge = ~0;
	for (int i=0;i<10;i++)
		CDD_TX[i] = 0;
	for (int i=0;i<10;i++)
		CDD_RX[i] = 0;
	NeoCDCommsWordCount = 0;
	NeoCDAssyStatus = 0;
	NeoCDTrack = 0;
	NeoCDSectorMin = 0;
	NeoCDSectorSec = 0;
	NeoCDSectorFrm = 0;
	SCD_CURLBA = 0;
	for (int i=0;i<2352;i++)
		NeoCDSectorData[i] = 0;
	bNeoCDLoadSector = false;
	NeoCDDMAAddress1 = 0;
	NeoCDDMAAddress2 = 0;
	NeoCDDMAValue1   = 0;
	NeoCDDMAValue2   = 0;
	NeoCDDMACount    = 0;
	NeoCDDMAMode = 0;
	CDC_REG0 = 0;
	nNeoCDIRQVectorAck = 0;
	nNeoCDIRQVector = 0;
}


void lc89510_temp_device::set_CDC_Do_DMA(device_t &device,segacd_dma_delegate new_segacd_dma_callback)
{
	lc89510_temp_device &dev = downcast<lc89510_temp_device &>(device);
	dev.segacd_dma_callback = new_segacd_dma_callback;
}

void lc89510_temp_device::set_is_neoCD(device_t &device, bool is_neoCD)
{
	lc89510_temp_device &dev = downcast<lc89510_temp_device &>(device);
	dev.is_neoCD = is_neoCD;
}

// HACK for DMA handling, this gets replaced
void lc89510_temp_device::Fake_CDC_Do_DMA(int &dmacount, UINT8 *CDC_BUFFER, UINT16 &CDC_DMA_ADDR, UINT16 &CDC_DMA_ADDRC, UINT16 &destination )
{
	fatalerror("Fake_CDC_Do_DMA\n");
}

void lc89510_temp_device::device_start()
{
	segacd_dma_callback.bind_relative_to(*owner());
	m_cdda = (cdda_device*)subdevice("cdda");
}

void lc89510_temp_device::device_reset()
{

}


inline int lc89510_temp_device::to_bcd(int val, bool byte)
{
	if (val > 99) val = 99;

	if (byte) return (((val) / 10) << 4) + ((val) % 10);
	else return (((val) / 10) << 8) + ((val) % 10);
}



void lc89510_temp_device::set_data_audio_mode(void)
{
	if (CURRENT_TRACK_IS_DATA)
	{
		SET_CDD_DATA_MODE
	}
	else
	{
		SET_CDD_AUDIO_MODE
		//fatalerror("CDDA unsupported\n");
	}
}

void lc89510_temp_device::CDD_DoChecksum(void)
{
	int checksum =
		CDD_RX[0] +
		CDD_RX[1] +
		CDD_RX[2] +
		CDD_RX[3] +
		CDD_RX[4] +
		CDD_RX[5] +
		CDD_RX[6] +
		CDD_RX[7] +
		CDD_RX[8];

	checksum &= 0xf;
	checksum ^= 0xf;

	CDD_RX[9] = checksum;
}

void lc89510_temp_device::CDD_Export(void)
{
	CDD_RX[0] = (CDD_STATUS  & 0xff00)>>8;
	CDD_RX[1] = (CDD_STATUS  & 0x00ff)>>0;
	CDD_RX[2] = (CDD_MIN  & 0xff00)>>8;
	CDD_RX[3] = (CDD_MIN  & 0x00ff)>>0;
	CDD_RX[4] = (CDD_SEC & 0xff00)>>8;
	CDD_RX[5] = (CDD_SEC & 0x00ff)>>0;
	CDD_RX[6] = (CDD_FRAME   & 0xff00)>>8;
	CDD_RX[7] = (CDD_FRAME   & 0x00ff)>>0;
	CDD_RX[8] = (CDD_EXT     & 0x00ff)>>0;
	/* 9 = checksum */

	CDD_DoChecksum();

	CDD_CONTROL &= ~4; // Clear HOCK bit

}





void lc89510_temp_device::CheckCommand(running_machine& machine)
{
	if (CDD_DONE)
	{
		CDD_DONE = 0;
		CDD_Export();
		CHECK_SCD_LV4_INTERRUPT
	}

	if (SCD_READ_ENABLED)
	{
		set_data_audio_mode();
		Read_LBA_To_Buffer(machine);
	}
}


void lc89510_temp_device::CDD_GetStatus(void)
{
	UINT16 s = (CDD_STATUS & 0x0f00);

	if ((s == 0x0200) || (s == 0x0700) || (s == 0x0e00))
		CDD_STATUS = (SCD_STATUS & 0xff00) | (CDD_STATUS & 0x00ff);
}


void lc89510_temp_device::CDD_Stop(running_machine &machine)
{
	CLEAR_CDD_RESULT
	STOP_CDC_READ
	SCD_STATUS = CDD_STOPPED;
	CDD_STATUS = 0x0000;
	SET_CDD_DATA_MODE
	cdda_stop_audio( m_cdda ); //stop any pending CD-DA
}


void lc89510_temp_device::CDD_GetPos(void)
{
	CLEAR_CDD_RESULT
	UINT32 msf;
	CDD_STATUS &= 0xFF;
	if(segacd.cd == NULL) // no cd is there, bail out
		return;
	CDD_STATUS |= SCD_STATUS;
	msf = lba_to_msf_alt(SCD_CURLBA+150);
	CDD_MIN = to_bcd(((msf & 0x00ff0000)>>16),false);
	CDD_SEC = to_bcd(((msf & 0x0000ff00)>>8),false);
	CDD_FRAME = to_bcd(((msf & 0x000000ff)>>0),false);
}

void lc89510_temp_device::CDD_GetTrackPos(void)
{
	CLEAR_CDD_RESULT
	int elapsedlba;
	UINT32 msf;
	CDD_STATUS &= 0xFF;
	//  UINT32 end_msf = ;
	if(segacd.cd == NULL) // no cd is there, bail out
		return;
	CDD_STATUS |= SCD_STATUS;
	elapsedlba = SCD_CURLBA - segacd.toc->tracks[ cdrom_get_track(segacd.cd, SCD_CURLBA) ].physframeofs;
	msf = lba_to_msf_alt (elapsedlba);
	//popmessage("%08x %08x",SCD_CURLBA,segacd.toc->tracks[ cdrom_get_track(segacd.cd, SCD_CURLBA) + 1 ].physframeofs);
	CDD_MIN = to_bcd(((msf & 0x00ff0000)>>16),false);
	CDD_SEC = to_bcd(((msf & 0x0000ff00)>>8),false);
	CDD_FRAME = to_bcd(((msf & 0x000000ff)>>0),false);
}

void lc89510_temp_device::CDD_GetTrack(void)
{
	CLEAR_CDD_RESULT
	CDD_STATUS &= 0xFF;
	if(segacd.cd == NULL) // no cd is there, bail out
		return;
	CDD_STATUS |= SCD_STATUS;
	SCD_CURTRK = cdrom_get_track(segacd.cd, SCD_CURLBA)+1;
	CDD_MIN = to_bcd(SCD_CURTRK, false);
}

void lc89510_temp_device::CDD_Length(void)
{
	CLEAR_CDD_RESULT
	CDD_STATUS &= 0xFF;
	if(segacd.cd == NULL) // no cd is there, bail out
		return;
	CDD_STATUS |= SCD_STATUS;

	UINT32 startlba = (segacd.toc->tracks[cdrom_get_last_track(segacd.cd)].physframeofs);
	UINT32 startmsf = lba_to_msf_alt( startlba );

	CDD_MIN = to_bcd((startmsf&0x00ff0000)>>16,false);
	CDD_SEC = to_bcd((startmsf&0x0000ff00)>>8,false);
	CDD_FRAME = to_bcd((startmsf&0x000000ff)>>0,false);
}


void lc89510_temp_device::CDD_FirstLast(void)
{
	CLEAR_CDD_RESULT
	CDD_STATUS &= 0xFF;
	if(segacd.cd == NULL) // no cd is there, bail out
		return;
	CDD_STATUS |= SCD_STATUS;
	CDD_MIN = 1; // first
	CDD_SEC = to_bcd(cdrom_get_last_track(segacd.cd),false); // last
}

void lc89510_temp_device::CDD_GetTrackAdr(void)
{
	CLEAR_CDD_RESULT

	int track = (CDD_TX[5] & 0xF) + (CDD_TX[4] & 0xF) * 10;
	int last_track = cdrom_get_last_track(segacd.cd);

	CDD_STATUS &= 0xFF;
	if(segacd.cd == NULL) // no cd is there, bail out
		return;
	CDD_STATUS |= SCD_STATUS;

	if (track > last_track)
		track = last_track;

	if (track < 1)
		track = 1;

	UINT32 startlba = (segacd.toc->tracks[track-1].physframeofs);
	UINT32 startmsf = lba_to_msf_alt( startlba+150 );

	CDD_MIN = to_bcd((startmsf&0x00ff0000)>>16,false);
	CDD_SEC = to_bcd((startmsf&0x0000ff00)>>8,false);
	CDD_FRAME = to_bcd((startmsf&0x000000ff)>>0,false);
	CDD_EXT = track % 10;

	if (segacd.toc->tracks[track - 1].trktype != CD_TRACK_AUDIO)
		CDD_FRAME |= 0x0800;
}

UINT32 lc89510_temp_device::getmsf_from_regs(void)
{
	UINT32 msf = 0;

	msf  = ((CDD_TX[3] & 0xF) + (CDD_TX[2] & 0xF) * 10) << 16;
	msf |= ((CDD_TX[5] & 0xF) + (CDD_TX[4] & 0xF) * 10) << 8;
	msf |= ((CDD_TX[7] & 0xF) + (CDD_TX[6] & 0xF) * 10) << 0;

	return msf;
}

void lc89510_temp_device::CDD_Play(running_machine &machine)
{
	CLEAR_CDD_RESULT
	UINT32 msf = getmsf_from_regs();
	SCD_CURLBA = msf_to_lba(msf)-150;
	UINT32 end_msf = segacd.toc->tracks[ cdrom_get_track(segacd.cd, SCD_CURLBA) + 1 ].physframeofs;
	SCD_CURTRK = cdrom_get_track(segacd.cd, SCD_CURLBA)+1;
	CDC_UpdateHEAD();
	SCD_STATUS = CDD_PLAYINGCDDA;
	CDD_STATUS = 0x0102;
	set_data_audio_mode();
	printf("%d Track played\n",SCD_CURTRK);
	CDD_MIN = to_bcd(SCD_CURTRK, false);
	if(!(CURRENT_TRACK_IS_DATA))
		cdda_start_audio( m_cdda, SCD_CURLBA, end_msf - SCD_CURLBA );
	SET_CDC_READ
}


void lc89510_temp_device::CDD_Seek(void)
{
	CLEAR_CDD_RESULT
	UINT32 msf = getmsf_from_regs();
	SCD_CURLBA = msf_to_lba(msf)-150;
	SCD_CURTRK = cdrom_get_track(segacd.cd, SCD_CURLBA)+1;
	CDC_UpdateHEAD();
	STOP_CDC_READ
	SCD_STATUS = CDD_READY;
	CDD_STATUS = 0x0200;
	set_data_audio_mode();
}


void lc89510_temp_device::CDD_Pause(running_machine &machine)
{
	CLEAR_CDD_RESULT
	STOP_CDC_READ
	SCD_STATUS = CDD_READY;
	CDD_STATUS = SCD_STATUS;
	SET_CDD_DATA_MODE

	//segacd.current_frame = cdda_get_audio_lba( machine.device( "cdda" ) );
	//if(!(CURRENT_TRACK_IS_DATA))
	cdda_pause_audio( m_cdda, 1 );
}

void lc89510_temp_device::CDD_Resume(running_machine &machine)
{
	CLEAR_CDD_RESULT
	STOP_CDC_READ
	SCD_CURTRK = cdrom_get_track(segacd.cd, SCD_CURLBA)+1;
	SCD_STATUS = CDD_PLAYINGCDDA;
	CDD_STATUS = 0x0102;
	set_data_audio_mode();
	CDD_MIN = to_bcd (SCD_CURTRK, false);
	SET_CDC_READ
	//if(!(CURRENT_TRACK_IS_DATA))
	cdda_pause_audio( m_cdda, 0 );
}


void lc89510_temp_device::CDD_FF(running_machine &machine)
{
	fatalerror("Fast Forward unsupported\n");
}


void lc89510_temp_device::CDD_RW(running_machine &machine)
{
	fatalerror("Fast Rewind unsupported\n");
}


void lc89510_temp_device::CDD_Open(void)
{
	fatalerror("Close Tray unsupported\n");
	/* TODO: re-read CD-ROM buffer here (Mega CD has multi disc games iirc?) */
}


void lc89510_temp_device::CDD_Close(void)
{
	fatalerror("Open Tray unsupported\n");
	/* TODO: clear CD-ROM buffer here */
}


void lc89510_temp_device::CDD_Init(void)
{
	CLEAR_CDD_RESULT
	STOP_CDC_READ
	SCD_STATUS = CDD_READY;
	CDD_STATUS = SCD_STATUS;
	CDD_SEC = 1;
	CDD_FRAME = 1;
}


void lc89510_temp_device::CDD_Default(void)
{
	CLEAR_CDD_RESULT
	CDD_STATUS = SCD_STATUS;
}


void lc89510_temp_device::CDD_Reset(void)
{
	CLEAR_CDD_RESULT
	CDD_CONTROL = CDD_STATUS = 0;

	for (int i = 0; i < 10; i++)
		CDD_RX[i] = CDD_TX[i] = 0;

	CDD_DoChecksum();

	SCD_CURTRK = SCD_CURLBA = 0;
	SCD_STATUS = CDD_READY;
}

void lc89510_temp_device::CDC_Reset(void)
{
	memset(CDC_BUFFER, 0x00, ((16 * 1024 * 2) + SECTOR_SIZE));
	CDC_UpdateHEAD();

	CDC_DMA_ADDRC = CDC_DMACNT = CDC_PT = CDC_SBOUT = CDC_IFCTRL = CDC_CTRLB0 = CDC_CTRLB1 =
		CDC_CTRLB2 = CDC_HEADB1 = CDC_HEADB2 = CDC_HEADB3 = CDC_STATB0 = CDC_STATB1 = CDC_STATB2 = CDC_DECODE = 0;

	CDC_IFSTAT = 0xFF;
	CDC_WA = SECTOR_SIZE * 2;
	CDC_HEADB0 = 0x01;
	CDC_STATB3 = 0x80;
}


void lc89510_temp_device::lc89510_Reset(void)
{
	CDD_Reset();
	CDC_Reset();

	CDC_REG0 = CDC_REG1 = CDC_DMA_ADDR = SCD_STATUS_CDC = CDD_DONE = 0;
}

void lc89510_temp_device::CDC_End_Transfer(running_machine& machine)
{
	STOP_CDC_DMA
	CDC_REG0 |= 0x8000;
	CDC_REG0 &= ~0x4000;
	CDC_IFSTAT |= 0x08;

	if (CDC_IFCTRL & 0x40)
	{
		CDC_IFSTAT &= ~0x40;
		CHECK_SCD_LV5_INTERRUPT
	}
}


void lc89510_temp_device::CDC_Do_DMA(running_machine& machine, int rate)
{
	UINT32 length;

	UINT16 destination = CDC_REG0 & 0x0700;

	if (!(SCD_DMA_ENABLED))
		return;

	if ((destination == READ_MAIN) || (destination==READ_SUB))
	{
		CDC_REG0 |= 0x4000;
		return;
	}

	if (CDC_DMACNT <= (rate * 2))
	{
		length = (CDC_DMACNT + 1) >> 1;
		CDC_End_Transfer(machine);
	}
	else
		length = rate;


	int dmacount = length;

	// HACK
	segacd_dma_callback(dmacount, CDC_BUFFER, CDC_DMA_ADDR, CDC_DMA_ADDRC, destination );
	

	CDC_DMA_ADDRC += length*2;

	if (SCD_DMA_ENABLED)
		CDC_DMACNT -= length*2;
	else
		CDC_DMACNT = 0;
}




UINT16 lc89510_temp_device::CDC_Host_r(running_machine& machine, UINT16 type)
{
	UINT16 destination = CDC_REG0 & 0x0700;

	if (SCD_DMA_ENABLED)
	{
		if (destination == type)
		{
			CDC_DMACNT -= 2;

			if (CDC_DMACNT <= 0)
			{
				if (type==READ_SUB) CDC_DMACNT = 0;

				CDC_End_Transfer(machine);
			}

			UINT16 data = (CDC_BUFFER[CDC_DMA_ADDRC]<<8) | CDC_BUFFER[CDC_DMA_ADDRC+1];
			CDC_DMA_ADDRC += 2;

			return data;
		}
	}

	return 0;
}


UINT8 lc89510_temp_device::CDC_Reg_r(void)
{
	int reg = CDC_REG0 & 0xF;
	UINT8 ret = 0;
	UINT16 decoderegs = 0x73F2;

	if ((decoderegs>>reg)&1)
		CDC_DECODE |= (1 << reg);

	//if (reg!=REG_R_STAT3)
		CDC_REG0 = (CDC_REG0 & 0xFFF0) | ((reg+1)&0xf);


	switch (reg)
	{
		case REG_R_COMIN:  ret = 0/*COMIN*/;            break;
		case REG_R_IFSTAT: ret = CDC_IFSTAT;           break;
		case REG_R_DBCL:   ret = CDC_DMACNT & 0xff;       break;
		case REG_R_DBCH:   ret = (CDC_DMACNT >>8) & 0xff; break;
		case REG_R_HEAD0:  ret = CDC_HEADB0;           break;
		case REG_R_HEAD1:  ret = CDC_HEADB1;           break;
		case REG_R_HEAD2:  ret = CDC_HEADB2;           break;
		case REG_R_HEAD3:  ret = CDC_HEADB3;           break;
		case REG_R_PTL:	   ret = CDC_PT & 0xff;        break;
		case REG_R_PTH:	   ret = (CDC_PT >>8) & 0xff;  break;
		case REG_R_WAL:    ret = CDC_WA & 0xff;        break;
		case REG_R_WAH:    ret = (CDC_WA >>8) & 0xff;  break;
		case REG_R_STAT0:  ret = CDC_STATB0;           break;
		case REG_R_STAT1:  ret = CDC_STATB1;           break;
		case REG_R_STAT2:  ret = CDC_STATB2;           break;
		case REG_R_STAT3:  ret = CDC_STATB3;

			CDC_IFSTAT |= 0x20;

			// ??
			if ((CDC_CTRLB0 & 0x80) && (CDC_IFCTRL & 0x20))
			{
				if ((CDC_DECODE & decoderegs) == decoderegs)
				CDC_STATB3 = 0x80;
			}
			break;
	}

	return ret;
}

void lc89510_temp_device::CDC_Reg_w(UINT8 data)
{
	int reg = CDC_REG0 & 0xF;

	int changers0[0x10] = { 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0};

	if (changers0[reg])
		CDC_REG0 = (CDC_REG0 & 0xFFF0) | (reg+1);

	switch (reg)
	{
	case REG_W_SBOUT:
			CDC_SBOUT = data;
			break;

	case REG_W_IFCTRL:
			CDC_IFCTRL = data;

			if (!(CDC_IFCTRL & 0x02))
			{
				CDC_DMACNT = 0;
				STOP_CDC_DMA;
				CDC_IFSTAT |= 0x08;
			}
			break;

	case REG_W_DBCL: CDC_DMACNT = (CDC_DMACNT &~ 0x00ff) | (data & 0x00ff) << 0; break;
	case REG_W_DBCH: CDC_DMACNT = (CDC_DMACNT &~ 0xff00) | (data & 0x00ff) << 8; break;
	case REG_W_DACL: CDC_DMA_ADDRC = (CDC_DMA_ADDRC &~ 0x00ff) | (data & 0x00ff) << 0; break;
	case REG_W_DACH: CDC_DMA_ADDRC = (CDC_DMA_ADDRC &~ 0xff00) | (data & 0x00ff) << 8; break;

	case REG_W_DTTRG:
			if (CDC_IFCTRL & 0x02)
			{
				CDC_IFSTAT &= ~0x08;
				SET_CDC_DMA;
				CDC_REG0 &= ~0x8000;
			}
			break;

	case REG_W_DTACK: CDC_IFSTAT |= 0x40; break;
	case REG_W_WAL: CDC_WA = (CDC_WA &~ 0x00ff) | (data & 0x00ff) << 0; break;
	case REG_W_WAH:	CDC_WA = (CDC_WA &~ 0xff00) | (data & 0x00ff) << 8;	break;
	case REG_W_CTRL0: CDC_CTRLB0 = data; break;
	case REG_W_CTRL1: CDC_CTRLB1 = data; break;
	case REG_W_PTL: CDC_PT = (CDC_PT &~ 0x00ff) | (data & 0x00ff) << 0; break;
	case REG_W_PTH: CDC_PT = (CDC_PT &~ 0xff00) | (data & 0x00ff) << 8;	break;
	case REG_W_CTRL2: CDC_CTRLB2 = data; break;
	case REG_W_RESET: CDC_Reset();       break;
	}
}

void lc89510_temp_device::CDD_Process(running_machine& machine, int reason)
{
	CDD_Export();
	CHECK_SCD_LV4_INTERRUPT
}

void lc89510_temp_device::CDD_Handle_TOC_Commands(void)
{
	int subcmd = CDD_TX[3];
	CDD_STATUS = (CDD_STATUS & 0xFF00) | subcmd;

	switch (subcmd)
	{
		case TOCCMD_CURPOS:	   CDD_GetPos();	  break;
		case TOCCMD_TRKPOS:	   CDD_GetTrackPos(); break;
		case TOCCMD_CURTRK:    CDD_GetTrack();   break;
		case TOCCMD_LENGTH:    CDD_Length();      break;
		case TOCCMD_FIRSTLAST: CDD_FirstLast();   break;
		case TOCCMD_TRACKADDR: CDD_GetTrackAdr(); break;
		default:               CDD_GetStatus();   break;
	}
}

static const char *const CDD_import_cmdnames[] =
{
	"Get Status",			// 0
	"Stop ALL",				// 1
	"Handle TOC",			// 2
	"Play",					// 3
	"Seek",					// 4
	"<undefined>",			// 5
	"Pause",				// 6
	"Resume",				// 7
	"FF",					// 8
	"RWD",					// 9
	"INIT",					// A
	"<undefined>",			// B
	"Close Tray",			// C
	"Open Tray",			// D
	"<undefined>",			// E
	"<undefined>"			// F
};

void lc89510_temp_device::CDD_Import(running_machine& machine)
{
	if(CDD_TX[0] != 2 && CDD_TX[0] != 0)
		printf("%s\n",CDD_import_cmdnames[CDD_TX[0]]);

	switch (CDD_TX[0])
	{
		case CMD_STATUS:	CDD_GetStatus();	       break;
		case CMD_STOPALL:	CDD_Stop(machine);		   break;
		case CMD_GETTOC:	CDD_Handle_TOC_Commands(); break;
		case CMD_READ:		CDD_Play(machine);         break;
		case CMD_SEEK:		CDD_Seek();	               break;
		case CMD_STOP:		CDD_Pause(machine);	       break;
		case CMD_RESUME:	CDD_Resume(machine);       break;
		case CMD_FF:		CDD_FF(machine);           break;
		case CMD_RW:		CDD_RW(machine);           break;
		case CMD_INIT:		CDD_Init();	               break;
		case CMD_CLOSE:		CDD_Open();                break;
		case CMD_OPEN:		CDD_Close();	           break;
		default:			CDD_Default();	           break;
	}

	CDD_DONE = 1;
}


/**************************************************************
 CDC Stuff ********
**************************************************************/



WRITE16_MEMBER( lc89510_temp_device::segacd_cdc_mode_address_w )
{
	COMBINE_DATA(&CDC_REG0);
}

READ16_MEMBER( lc89510_temp_device::segacd_cdc_mode_address_r )
{
	return CDC_REG0;
}

WRITE16_MEMBER( lc89510_temp_device::segacd_cdc_data_w )
{
	COMBINE_DATA(&CDC_REG1);

	if (ACCESSING_BITS_0_7)
		CDC_Reg_w(data);
}

READ16_MEMBER( lc89510_temp_device::segacd_cdc_data_r )
{
	UINT16 retdat = 0x0000;

	if (ACCESSING_BITS_0_7)
		retdat |= CDC_Reg_r();

	return retdat;
}


READ16_MEMBER( lc89510_temp_device::cdc_data_sub_r )
{
	return CDC_Host_r(space.machine(), READ_SUB);
}

READ16_MEMBER( lc89510_temp_device::cdc_data_main_r )
{
	return CDC_Host_r(space.machine(), READ_MAIN);
}




READ16_MEMBER( lc89510_temp_device::segacd_irq_mask_r )
{

	return segacd_irq_mask;
}

WRITE16_MEMBER( lc89510_temp_device::segacd_irq_mask_w )
{
	if (ACCESSING_BITS_0_7)
	{
		UINT16 control = CDD_CONTROL;

	//  printf("segacd_irq_mask_w %04x %04x (CDD control is %04x)\n",data, mem_mask, control);

		if (data & 0x10)
		{
			if (control & 0x04)
			{
				if (!(segacd_irq_mask & 0x10))
				{
					segacd_irq_mask = data & 0x7e;
					CDD_Process(space.machine(), 0);
					return;
				}
			}
		}

		segacd_irq_mask = data & 0x7e;
	}
	else
	{

		printf("segacd_irq_mask_w only MSB written\n");

	}
}

READ16_MEMBER( lc89510_temp_device::segacd_cdd_ctrl_r )
{

	return CDD_CONTROL;
}


WRITE16_MEMBER( lc89510_temp_device::segacd_cdd_ctrl_w )
{
	if (ACCESSING_BITS_0_7)
	{
		UINT16 control = CDD_CONTROL;


		//printf("segacd_cdd_ctrl_w %04x %04x (control %04x irq %04x\n", data, mem_mask, control, segacd_irq_mask);

		data &=0x4; // only HOCK bit is writable

		if (data & 0x4)
		{
			if (!(control & 0x4))
			{
				if (segacd_irq_mask&0x10)
				{
					CDD_Process(space.machine(), 1);
				}
			}
		}

		CDD_CONTROL |= data;
	}
	else
	{
		printf("segacd_cdd_ctrl_w only MSB written\n");
	}
}


// mapped as serial
UINT8 lc89510_temp_device::neocd_cdd_rx_r()
{
	UINT8 ret = 0;

	if (NeoCDCommsWordCount >= 0 && NeoCDCommsWordCount < 10) {
		ret = CDD_RX[NeoCDCommsWordCount] & 0x0F;
	}

	if (bNeoCDCommsClock) {
		ret |= 0x10;
	}
	
	return ret;
}

// mapped like 'ram'
READ8_MEMBER( lc89510_temp_device::segacd_cdd_rx_r )
{
	return CDD_RX[offset];
}

// mapped as serial
void lc89510_temp_device::neocd_cdd_tx_w(UINT8 data)
{
	//printf("neocd_cdd_tx_w %d, %02x\n", NeoCDCommsWordCount, data);

	if (NeoCDCommsWordCount >= 0 && NeoCDCommsWordCount < 10) {
		CDD_TX[NeoCDCommsWordCount] = data & 0x0F;
	}
}

WRITE8_MEMBER( lc89510_temp_device::segacd_cdd_tx_w )
{
	CDD_TX[offset] = data;

	if(offset == 9)
	{
		CDD_Import(space.machine());
	}
}




READ16_MEMBER( lc89510_temp_device::cdc_dmaaddr_r )
{
	return CDC_DMA_ADDR;
}

WRITE16_MEMBER( lc89510_temp_device::cdc_dmaaddr_w )
{
	COMBINE_DATA(&CDC_DMA_ADDR);
}

READ16_MEMBER( lc89510_temp_device::segacd_cdfader_r )
{
	return 0;
}

WRITE16_MEMBER( lc89510_temp_device::segacd_cdfader_w )
{
	static double cdfader_vol;
	if(data & 0x800f)
		printf("CD Fader register write %04x\n",data);

	cdfader_vol = (double)((data & 0x3ff0) >> 4);

	if(data & 0x4000)
		cdfader_vol = 100.0;
	else
		cdfader_vol = (cdfader_vol / 1024.0) * 100.0;

	//printf("%f\n",cdfader_vol);

	cdda_set_volume( m_cdda, cdfader_vol);
}

void lc89510_temp_device::reset_cd(void)
{
	/* init cd-rom device */

	lc89510_Reset();

	{
		cdrom_image_device *cddevice = machine().device<cdrom_image_device>("cdrom");
		if ( cddevice )
		{
			segacd.cd = cddevice->get_cdrom_file();
			if ( segacd.cd )
			{
				segacd.toc = cdrom_get_toc( segacd.cd );
				cdda_set_cdrom( m_cdda, segacd.cd );
				cdda_stop_audio(m_cdda ); //stop any pending CD-DA
			}
		}
	}



	if (segacd.cd)
		printf("cd found\n");
}




TIMER_DEVICE_CALLBACK_MEMBER( lc89510_temp_device::segacd_access_timer_callback )
{
	if (!is_neoCD)
	{
		CheckCommand(machine());
	}
	else
	{
		if (nff0002 & 0x0050) {
			nIRQAcknowledge &= ~0x10;
			NeoCDIRQUpdate(0);

			if (nff0002 & 0x0500) {
				Read_LBA_To_Buffer_NeoCD();
			}
		}
	}
}




static MACHINE_CONFIG_FRAGMENT( lc89510_temp_fragment )
	MCFG_TIMER_DRIVER_ADD_PERIODIC("hock_timer", lc89510_temp_device, segacd_access_timer_callback, attotime::from_hz(75))

	MCFG_SOUND_ADD( "cdda", CDDA, 0 )
	MCFG_SOUND_ROUTE( 0, ":lspeaker", 0.50 ) // TODO: accurate volume balance
	MCFG_SOUND_ROUTE( 1, ":rspeaker", 0.50 )
MACHINE_CONFIG_END

machine_config_constructor lc89510_temp_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( lc89510_temp_fragment );
}

/* Neo CD */


enum CDEmuStatusValue { idle = 0, reading, playing, paused, seeking, fastforward, fastreverse };
CDEmuStatusValue CDEmuStatus;

static inline CDEmuStatusValue CDEmuGetStatus()
{
//	printf("CDEmuGetStatus\n");
	return CDEmuStatus;
}

UINT8* lc89510_temp_device::CDEmuReadQChannel(int SCD_CURLBA)
{
//	printf("CDEmuReadQChannel\n");
	static unsigned char QChannelData[8];

	if(segacd.cd == NULL) // no cd is there, bail out
		return QChannelData;

//  SCD_CURLBA
	switch (CDEmuStatus) {
		case reading:
		case playing: {

			UINT32 msf;
			msf = lba_to_msf_alt(SCD_CURLBA+150);



			QChannelData[0] = cdrom_get_track(segacd.cd, SCD_CURLBA);

			QChannelData[1] = (msf >> 16)&0xff;
			QChannelData[2] = (msf >> 8)&0xff;
			QChannelData[3] = (msf >> 0)&0xff;

			int elapsedlba;
			elapsedlba = SCD_CURLBA - segacd.toc->tracks[ cdrom_get_track(segacd.cd, SCD_CURLBA) ].physframeofs;
			msf = lba_to_msf_alt (elapsedlba);

			QChannelData[4] = (msf >> 16)&0xff;
			QChannelData[5] = (msf >> 8)&0xff;
			QChannelData[6] = (msf >> 0)&0xff;

			if (QChannelData[0]==1)
				QChannelData[7] = 0x4;
			else
				QChannelData[7] = 0x0;

			break;
		}
		case paused: {
			break;
		}
		default: {
			memset(QChannelData, 0, sizeof(QChannelData));
		}
	}

	return QChannelData;
}

UINT8* lc89510_temp_device::CDEmuReadTOC(INT32 track)
{
//	printf("CDEmuReadTOC\n");

	static unsigned char TOCEntry[4];

	if(segacd.cd == NULL)
		return TOCEntry;


	if (track == -1) {
		printf("get first/last track nums\n");
		TOCEntry[0] = 1;
		TOCEntry[1] = cdrom_get_last_track(segacd.cd);
		TOCEntry[2] = 0;
		TOCEntry[3] = 0;

		return TOCEntry;
	}
	else if (track == -2) {
		printf("get disc length\n");

		UINT32 startlba = (segacd.toc->tracks[cdrom_get_last_track(segacd.cd)].physframeofs);
		UINT32 startmsf = lba_to_msf_alt( startlba );


		TOCEntry[0] = (startmsf >> 16)&0xff;
		TOCEntry[1] = (startmsf >> 8)&0xff;
		TOCEntry[2] = (startmsf >> 0)&0xff;

		TOCEntry[3] = 0;

		return TOCEntry;
	}
	else
	{
		printf("get track address\n");

		int last_track = cdrom_get_last_track(segacd.cd);

		if (track > last_track)
			track = last_track;

		if (track < 1)
			track = 1;

		UINT32 startlba = (segacd.toc->tracks[track-1].physframeofs);
		UINT32 startmsf = lba_to_msf_alt( startlba+150 );

		TOCEntry[0] = (startmsf >> 16)&0xff;
		TOCEntry[1] = (startmsf >> 8)&0xff;
		TOCEntry[2] = (startmsf >> 0)&0xff;
		TOCEntry[3] =  track % 10;;
	}

	return TOCEntry;

}

static void CDEmuStartRead()
{
	printf("CDEmuStartRead\n");
	CDEmuStatus = seeking;
}

static void CDEmuPause()
{
	printf("CDEmuPause\n");
	CDEmuStatus = paused;
}

static INT32 CDEmuStop()
{
	printf("CDEmuStop\n");
	return 1;
}

static INT32 CDEmuPlay(UINT8 M, UINT8 S, UINT8 F)
{
	printf("CDEmuPlay\n");
	return 1;
}

INT32 lc89510_temp_device::CDEmuLoadSector(INT32 LBA, char* pBuffer)
{
//	printf("CDEmuLoadSector %d\n", LBA);
	//LBA += 150;

	cdrom_read_data(segacd.cd, LBA, pBuffer, CD_TRACK_MODE1);

	return LBA+1;// - 150;
}



void lc89510_temp_device::NeoCDLBAToMSF(const INT32 LBA)
{
	NeoCDSectorMin = (LBA + CD_FRAMES_PREGAP)                    / CD_FRAMES_MINUTE;
	NeoCDSectorSec = (LBA + CD_FRAMES_PREGAP) % CD_FRAMES_MINUTE / CD_FRAMES_SECOND;
	NeoCDSectorFrm = (LBA + CD_FRAMES_PREGAP) % CD_FRAMES_SECOND;
}


void lc89510_temp_device::NeoCDCommsReset()
{
	bNeoCDCommsSend  = false;
	bNeoCDCommsClock = true;

	memset(CDD_TX, 0, sizeof(CDD_TX));
	memset(CDD_RX,  0, sizeof(CDD_RX));

	NeoCDCommsWordCount = 0;

	NeoCDAssyStatus = 9;

	memset(NeoCDSectorData, 0, sizeof(NeoCDSectorData));

	bNeoCDLoadSector = false;

	nff0016 = 0;
}

static INT32 SekIdle(INT32 nCycles)
{
	return nCycles;
}

void lc89510_temp_device::NeoCDDoDMA()
{

	// The LC8953 chip has a programmable DMA controller, which is not properly emulated.
	// Since the software only uses it in a limited way, we can apply a simple heuristic
	// to determnine the requested operation.

	// Additionally, we don't know how many cycles DMA operations take.
	// Here, only bus access is used to get a rough approximation --
	// each read/write takes a single cycle, setup and everything else is ignored.

//  bprintf(PRINT_IMPORTANT, _T("  - DMA controller transfer started (PC: 0x%06X)\n"), SekGetPC(-1));

	switch (NeoCDDMAMode) {

		case 0xCFFD: {
//          bprintf(PRINT_NORMAL, _T("    adr : 0x%08X - 0x%08X <- address, skip odd bytes\n"), NeoCDDMAAddress1, NeoCDDMAAddress1 + NeoCDDMACount * 8);

			//  - DMA controller 0x7E -> 0xCFFD (PC: 0xC07CE2)
			//  - DMA controller program[00] -> 0xFCF5 (PC: 0xC07CE8)
			//  - DMA controller program[02] -> 0xE8DA (PC: 0xC07CEE)
			//  - DMA controller program[04] -> 0x92DA (PC: 0xC07CF4)
			//  - DMA controller program[06] -> 0x92DB (PC: 0xC07CFA)
			//  - DMA controller program[08] -> 0x96DB (PC: 0xC07D00)
			//  - DMA controller program[10] -> 0x96F6 (PC: 0xC07D06)
			//  - DMA controller program[12] -> 0x2E02 (PC: 0xC07D0C)
			//  - DMA controller program[14] -> 0xFDFF (PC: 0xC07D12)

			SekIdle(NeoCDDMACount * 4);

			while (NeoCDDMACount--) {
				SekWriteWord(NeoCDDMAAddress1 + 0, NeoCDDMAAddress1 >> 24);
				SekWriteWord(NeoCDDMAAddress1 + 2, NeoCDDMAAddress1 >> 16);
				SekWriteWord(NeoCDDMAAddress1 + 4, NeoCDDMAAddress1 >>  8);
				SekWriteWord(NeoCDDMAAddress1 + 6, NeoCDDMAAddress1 >>  0);
				NeoCDDMAAddress1 += 8;
			}

			break;
		}

		case 0xE2DD: {
//          bprintf(PRINT_NORMAL, _T("    copy: 0x%08X - 0x%08X <- 0x%08X - 0x%08X, skip odd bytes\n"), NeoCDDMAAddress2, NeoCDDMAAddress2 + NeoCDDMACount * 2, NeoCDDMAAddress1, NeoCDDMAAddress1 + NeoCDDMACount * 4);

			//  - DMA controller 0x7E -> 0xE2DD (PC: 0xC0A190)
			//  - DMA controller program[00] -> 0xFCF5 (PC: 0xC0A192)
			//  - DMA controller program[02] -> 0x82BE (PC: 0xC0A194)
			//  - DMA controller program[04] -> 0x93DA (PC: 0xC0A196)
			//  - DMA controller program[06] -> 0xBE93 (PC: 0xC0A198)
			//  - DMA controller program[08] -> 0xDABE (PC: 0xC0A19A)
			//  - DMA controller program[10] -> 0xF62D (PC: 0xC0A19C)
			//  - DMA controller program[12] -> 0x02FD (PC: 0xC0A19E)
			//  - DMA controller program[14] -> 0xFFFF (PC: 0xC0A1A0)

			SekIdle(NeoCDDMACount * 1);

			while (NeoCDDMACount--) {
				SekWriteWord(NeoCDDMAAddress2 + 0, SekReadByte(NeoCDDMAAddress1 + 0));
				SekWriteWord(NeoCDDMAAddress2 + 2, SekReadByte(NeoCDDMAAddress1 + 1));
				NeoCDDMAAddress1 += 2;
				NeoCDDMAAddress2 += 4;
			}

			break;
		}

		case 0xFC2D: {
//          bprintf(PRINT_NORMAL, _T("    copy: 0x%08X - 0x%08X <- LC8951 external buffer, skip odd bytes\n"), NeoCDDMAAddress1, NeoCDDMAAddress1 + NeoCDDMACount * 4);

			//  - DMA controller 0x7E -> 0xFC2D (PC: 0xC0A190)
			//  - DMA controller program[00] -> 0xFCF5 (PC: 0xC0A192)
			//  - DMA controller program[02] -> 0x8492 (PC: 0xC0A194)
			//  - DMA controller program[04] -> 0xDA92 (PC: 0xC0A196)
			//  - DMA controller program[06] -> 0xDAF6 (PC: 0xC0A198)
			//  - DMA controller program[08] -> 0x2A02 (PC: 0xC0A19A)
			//  - DMA controller program[10] -> 0xFDFF (PC: 0xC0A19C)
			//  - DMA controller program[12] -> 0x48E7 (PC: 0xC0A19E)
			//  - DMA controller program[14] -> 0xFFFE (PC: 0xC0A1A0)

			char* data = LC8915InitTransfer();
			if (data == NULL) {
				break;
			}

			SekIdle(NeoCDDMACount * 4);

			while (NeoCDDMACount--) {
				SekWriteByte(NeoCDDMAAddress1 + 0, data[0]);
				SekWriteByte(NeoCDDMAAddress1 + 2, data[1]);
				NeoCDDMAAddress1 += 4;
				data += 2;
			}

			LC8915EndTransfer();

			break;
		}

		case 0xFE3D:

			//  - DMA controller 0x7E -> 0xFE3D (PC: 0xC0A190)
			//  - DMA controller program[00] -> 0xFCF5 (PC: 0xC0A192)
			//  - DMA controller program[02] -> 0x82BF (PC: 0xC0A194)
			//  - DMA controller program[04] -> 0x93BF (PC: 0xC0A196)
			//  - DMA controller program[06] -> 0xF629 (PC: 0xC0A198)
			//  - DMA controller program[08] -> 0x02FD (PC: 0xC0A19A)
			//  - DMA controller program[10] -> 0xFFFF (PC: 0xC0A19C)
			//  - DMA controller program[12] -> 0xF17D (PC: 0xC0A19E)
			//  - DMA controller program[14] -> 0xFCF5 (PC: 0xC0A1A0)

		case 0xFE6D: {
//          bprintf(PRINT_NORMAL, _T("    copy: 0x%08X - 0x%08X <- 0x%08X - 0x%08X\n"), NeoCDDMAAddress2, NeoCDDMAAddress2 + NeoCDDMACount * 2, NeoCDDMAAddress1, NeoCDDMAAddress1 + NeoCDDMACount * 2);

			//  - DMA controller 0x7E -> 0xFE6D (PC: 0xC0FD7A)
			//  - DMA controller program[00] -> 0xFCF5 (PC: 0xC0FD7C)
			//  - DMA controller program[02] -> 0x82BF (PC: 0xC0FD7E)
			//  - DMA controller program[04] -> 0xF693 (PC: 0xC0FD80)
			//  - DMA controller program[06] -> 0xBF29 (PC: 0xC0FD82)
			//  - DMA controller program[08] -> 0x02FD (PC: 0xC0FD84)
			//  - DMA controller program[10] -> 0xFFFF (PC: 0xC0FD86)
			//  - DMA controller program[12] -> 0xC515 (PC: 0xC0FD88)
			//  - DMA controller program[14] -> 0xFCF5 (PC: 0xC0FD8A)

			SekIdle(NeoCDDMACount * 1);

			while (NeoCDDMACount--) {
				SekWriteWord(NeoCDDMAAddress2, SekReadWord(NeoCDDMAAddress1));
				NeoCDDMAAddress1 += 2;
				NeoCDDMAAddress2 += 2;
			}

if (NeoCDDMAAddress2 == 0x0800)  {
// MapVectorTable(false);
//  bprintf(PRINT_ERROR, _T("    RAM vectors mapped (PC = 0x%08X\n"), SekGetPC(0));
//  extern INT32 bRunPause;
//  bRunPause = 1;
}
			break;
		}

		case 0xFEF5: {
//          bprintf(PRINT_NORMAL, _T("    adr : 0x%08X - 0x%08X <- address\n"), NeoCDDMAAddress1, NeoCDDMAAddress1 + NeoCDDMACount * 4);

			//  - DMA controller 0x7E -> 0xFEF5 (PC: 0xC07CE2)
			//  - DMA controller program[00] -> 0xFCF5 (PC: 0xC07CE8)
			//  - DMA controller program[02] -> 0x92E8 (PC: 0xC07CEE)
			//  - DMA controller program[04] -> 0xBE96 (PC: 0xC07CF4)
			//  - DMA controller program[06] -> 0xF629 (PC: 0xC07CFA)
			//  - DMA controller program[08] -> 0x02FD (PC: 0xC07D00)
			//  - DMA controller program[10] -> 0xFFFF (PC: 0xC07D06)
			//  - DMA controller program[12] -> 0xFC3D (PC: 0xC07D0C)
			//  - DMA controller program[14] -> 0xFCF5 (PC: 0xC07D12)

			SekIdle(NeoCDDMACount * 2);

			while (NeoCDDMACount--) {
				SekWriteWord(NeoCDDMAAddress1 + 0, NeoCDDMAAddress1 >> 16);
				SekWriteWord(NeoCDDMAAddress1 + 2, NeoCDDMAAddress1 >>  0);
				NeoCDDMAAddress1 += 4;
			}

			break;
		}

		case 0xFFC5: {
//          bprintf(PRINT_NORMAL, _T("    copy: 0x%08X - 0x%08X <- LC8951 external buffer\n"), NeoCDDMAAddress1, NeoCDDMAAddress1 + NeoCDDMACount * 2);

			//  - DMA controller 0x7E -> 0xFFC5 (PC: 0xC0A190)
			//  - DMA controller program[00] -> 0xFCF5 (PC: 0xC0A192)
			//  - DMA controller program[02] -> 0xA6F6 (PC: 0xC0A194)
			//  - DMA controller program[04] -> 0x2602 (PC: 0xC0A196)
			//  - DMA controller program[06] -> 0xFDFF (PC: 0xC0A198)
			//  - DMA controller program[08] -> 0xFC2D (PC: 0xC0A19A)
			//  - DMA controller program[10] -> 0xFCF5 (PC: 0xC0A19C)
			//  - DMA controller program[12] -> 0x8492 (PC: 0xC0A19E)
			//  - DMA controller program[14] -> 0xDA92 (PC: 0xC0A1A0)

			char* data = LC8915InitTransfer();
			if (data == NULL) {
				break;
			}

			SekIdle(NeoCDDMACount * 4);

			while (NeoCDDMACount--) {
				SekWriteByte(NeoCDDMAAddress1 + 0, data[0]);
				SekWriteByte(NeoCDDMAAddress1 + 1, data[1]);
				NeoCDDMAAddress1 += 2;
				data += 2;
			}

			LC8915EndTransfer();

			break;
		}

		case 0xFFCD:

			//  - DMA controller 0x7E -> 0xFFCD (PC: 0xC0A190)
			//  - DMA controller program[00] -> 0xFCF5 (PC: 0xC0A192)
			//  - DMA controller program[02] -> 0x92F6 (PC: 0xC0A194)
			//  - DMA controller program[04] -> 0x2602 (PC: 0xC0A196)
			//  - DMA controller program[06] -> 0xFDFF (PC: 0xC0A198)
			//  - DMA controller program[08] -> 0x7006 (PC: 0xC0A19A)
			//  - DMA controller program[10] -> 0x6100 (PC: 0xC0A19C)
			//  - DMA controller program[12] -> 0x2412 (PC: 0xC0A19E)
			//  - DMA controller program[14] -> 0x13FC (PC: 0xC0A1A0)

		case 0xFFDD: {
//          bprintf(PRINT_NORMAL, _T("    Fill: 0x%08X - 0x%08X <- 0x%04X\n"), NeoCDDMAAddress1, NeoCDDMAAddress1 + NeoCDDMACount * 2, NeoCDDMAValue1);

			//  - DMA controller 0x7E -> 0xFFDD (PC: 0xC07CE2)
			//  - DMA controller program[00] -> 0xFCF5 (PC: 0xC07CE8)
			//  - DMA controller program[02] -> 0x92F6 (PC: 0xC07CEE)
			//  - DMA controller program[04] -> 0x2602 (PC: 0xC07CF4)
			//  - DMA controller program[06] -> 0xFDFF (PC: 0xC07CFA)
			//  - DMA controller program[08] -> 0xFFFF (PC: 0xC07D00)
			//  - DMA controller program[10] -> 0xFCF5 (PC: 0xC07D06)
			//  - DMA controller program[12] -> 0x8AF0 (PC: 0xC07D0C)
			//  - DMA controller program[14] -> 0x1609 (PC: 0xC07D12)

			SekIdle(NeoCDDMACount * 1);

			while (NeoCDDMACount--) {
				SekWriteWord(NeoCDDMAAddress1, NeoCDDMAValue1);
				NeoCDDMAAddress1 += 2;
			}

			break;
		}
		default: {
			//bprintf(PRINT_ERROR, _T("    Unknown transfer type 0x%04X (PC: 0x%06X)\n"), NeoCDDMAMode, SekGetPC(-1));
			//bprintf(PRINT_NORMAL, _T("    ??? : 0x%08X  0x%08X 0x%04X 0x%04X 0x%08X\n"), NeoCDDMAAddress1, NeoCDDMAAddress2, NeoCDDMAValue1, NeoCDDMAValue2, NeoCDDMACount);

//extern INT32 bRunPause;
//bRunPause = 1;

		}
	}
}


void lc89510_temp_device::NeoCDProcessCommand()
{
	memset(CDD_RX,  0, sizeof(CDD_RX));

	if (CDD_TX[0]) {
		CDD_RX[1] = 15;
	}

	switch (CDD_TX[0]) {
		case CMD_STATUS: // CDD_GetStatus();
			break;
		case CMD_STOPALL: // CDD_Stop(machine);
//                              //bprintf(PRINT_ERROR, _T("    CD comms received command %i\n"), CDD_TX[0]);
			CDEmuStop();

			NeoCDAssyStatus = 0x0E;
			bNeoCDLoadSector = false;
			break;
		case CMD_GETTOC: // CDD_Handle_TOC_Commands();
//                              //bprintf(PRINT_ERROR, _T("    CD comms received command %i\n"), CDD_TX[0]);
			CDD_RX[1] = CDD_TX[3];
			 switch (CDD_TX[3]) {



				case TOCCMD_CURPOS: { //  CDD_GetPos();
					UINT8* ChannelData = CDEmuReadQChannel(SCD_CURLBA);

					CDD_RX[2] = ChannelData[1] / 10;
					CDD_RX[3] = ChannelData[1] % 10;

					CDD_RX[4] = ChannelData[2] / 10;
					CDD_RX[5] = ChannelData[2] % 10;

					CDD_RX[6] = ChannelData[3] / 10;
					CDD_RX[7] = ChannelData[3] % 10;

					CDD_RX[8] = ChannelData[7];

// //bprintf(PRINT_ERROR, _T("    %02i %02i:%02i:%02i %02i:%02i:%02i %02i\n"), ChannelData[0], ChannelData[1], ChannelData[2], ChannelData[3], ChannelData[4], ChannelData[5], ChannelData[6], ChannelData[7]);

					break;
				}
				case TOCCMD_TRKPOS: { //  CDD_GetTrackPos();
					UINT8* ChannelData = CDEmuReadQChannel(SCD_CURLBA);

					CDD_RX[2] = ChannelData[4] / 10;
					CDD_RX[3] = ChannelData[4] % 10;

					CDD_RX[4] = ChannelData[5] / 10;
					CDD_RX[5] = ChannelData[5] % 10;

					CDD_RX[6] = ChannelData[6] / 10;
					CDD_RX[7] = ChannelData[6] % 10;

					CDD_RX[8] = ChannelData[7];

					break;
				}
				case TOCCMD_CURTRK: { //  CDD_GetTrack();

					UINT8* ChannelData = CDEmuReadQChannel(SCD_CURLBA);

					CDD_RX[2] = ChannelData[0] / 10;
					CDD_RX[3] = ChannelData[0] % 10;


					CDD_RX[8] = ChannelData[7];

					break;
				}
				case TOCCMD_LENGTH: { //  CDD_Length();
					UINT8* TOCEntry = CDEmuReadTOC(-2);

					CDD_RX[2] = TOCEntry[0] / 10;
					CDD_RX[3] = TOCEntry[0] % 10;

					CDD_RX[4] = TOCEntry[1] / 10;
					CDD_RX[5] = TOCEntry[1] % 10;

					CDD_RX[6] = TOCEntry[2] / 10;
					CDD_RX[7] = TOCEntry[2] % 10;

					break;
				}
				case TOCCMD_FIRSTLAST: { //  CDD_FirstLast();
					UINT8* TOCEntry = CDEmuReadTOC(-1);

					CDD_RX[2] = TOCEntry[0] / 10;
					CDD_RX[3] = TOCEntry[0] % 10;

					CDD_RX[4] = TOCEntry[1] / 10;
					CDD_RX[5] = TOCEntry[1] % 10;

					break;
				}
				case TOCCMD_TRACKADDR:	{ //  CDD_GetTrackAdr(); 
					NeoCDTrack = CDD_TX[4] * 10 + CDD_TX[5];

					UINT8* TOCEntry = CDEmuReadTOC(NeoCDTrack);

					CDD_RX[2] = TOCEntry[0] / 10;
					CDD_RX[3] = TOCEntry[0] % 10;

					CDD_RX[4] = TOCEntry[1] / 10;
					CDD_RX[5] = TOCEntry[1] % 10;

					CDD_RX[6] = TOCEntry[2] / 10;
					CDD_RX[7] = TOCEntry[2] % 10;

					// bit 3 of the 1st minutes digit indicates a data track
					if (TOCEntry[3] & 4) {
						CDD_RX[6] |= 8;
					}

					CDD_RX[8] = NeoCDTrack % 10;

					break;
				}

				case 6: {

					UINT8* ChannelData = CDEmuReadQChannel(SCD_CURLBA);

					CDD_RX[8] = ChannelData[7];

					break;
				}

				case 7: {

					// must be 02, 0E, 0F, or 05
					CDD_RX[2] = 0;
					CDD_RX[3] = 5;

					CDD_RX[4] = 0;
					CDD_RX[5] = 0;

					CDD_RX[6] = 0;
					CDD_RX[7] = 0;
					break;
				}
			 }
			break;


		case CMD_READ: { // CDD_Play(machine); 

			if (LC8951RegistersW[REG_W_CTRL0] & 4) {

				if (CDEmuGetStatus() == playing) {
					//bprintf(PRINT_ERROR, _T("*** Switching CD mode to CD-ROM while in audio mode!(PC: 0x%06X)\n"), SekGetPC(-1));
				}

				SCD_CURLBA  = CDD_TX[2] * (10 * CD_FRAMES_MINUTE);
				SCD_CURLBA += CDD_TX[3] * ( 1 * CD_FRAMES_MINUTE);
				SCD_CURLBA += CDD_TX[4] * (10 * CD_FRAMES_SECOND);
				SCD_CURLBA += CDD_TX[5] * ( 1 * CD_FRAMES_SECOND);
				SCD_CURLBA += CDD_TX[6] * (10                   );
				SCD_CURLBA += CDD_TX[7] * ( 1                   );

				SCD_CURLBA -= CD_FRAMES_PREGAP;

				CDEmuStartRead();
//              LC8951RegistersR[REG_R_IFSTAT] |= 0x20;
			} else {

				if (CDEmuGetStatus() == reading) {
					//bprintf(PRINT_ERROR, _T("*** Switching CD mode to audio while in CD-ROM mode!(PC: 0x%06X)\n"), SekGetPC(-1));
				}

				CDEmuPlay((CDD_TX[2] * 10) + CDD_TX[3], (CDD_TX[4] * 10) + CDD_TX[5], (CDD_TX[6] * 10) + CDD_TX[7]);
			}

			NeoCDAssyStatus = 1;
			bNeoCDLoadSector = true;

			break;
		}
		case CMD_SEEK: // CDD_Seek();	
//          //bprintf(PRINT_ERROR, _T("    CD comms received command %i\n"), CDD_TX[0]);
			CDEmuPause();
			break;
		case 5:
//          //bprintf(PRINT_ERROR, _T("    CD comms received command %i\n"), CDD_TX[0]);
//          NeoCDAssyStatus = 9;
//          bNeoCDLoadSector = false;
			break;

		case CMD_STOP: // CDD_Pause(machine);
//          //bprintf(PRINT_ERROR, _T("    CD comms received command %i\n"), CDD_TX[0]);
			NeoCDAssyStatus = 4;
			bNeoCDLoadSector = false;
			break;
		case CMD_RESUME: // 	CDD_Resume(machine); 
//          //bprintf(PRINT_ERROR, _T("    CD comms received command %i\n"), CDD_TX[0]);
			NeoCDAssyStatus = 1;
			bNeoCDLoadSector = true;
			break;
			 
		case CMD_FF: // CDD_FF(machine);   
		case CMD_RW: // CDD_RW(machine);   
		case CMD_INIT: // CDD_Init();	   
		case 11:
		case CMD_CLOSE: // CDD_Close();	
		case CMD_OPEN: // CDD_Open();     
		case 14:
		case 15:
//          //bprintf(PRINT_ERROR, _T("    CD comms received command %i\n"), CDD_TX[0]);
			NeoCDAssyStatus = 9;
			bNeoCDLoadSector = false;
			break;
	}
}

void lc89510_temp_device::NeoCDCommsControl(UINT8 clock, UINT8 send)
{
	if (clock && !bNeoCDCommsClock) {
		NeoCDCommsWordCount++;
		if (NeoCDCommsWordCount >= 10) {
			NeoCDCommsWordCount = 0;

			if (send) {

				// command receive complete

				if (CDD_TX[0]) {
					INT32  sum = 0;

						printf("has command %02x\n", CDD_TX[0]);

//                  bprintf(PRINT_NORMAL, _T("  - CD mechanism command receive completed : 0x"));
					for (INT32 i = 0; i < 9; i++) {
//                      bprintf(PRINT_NORMAL, _T("%X"), CDD_TX[i]);
						sum += CDD_TX[i];
					}
					sum = ~(sum + 5) & 0x0F;
//                  bprintf(PRINT_NORMAL, _T(" (CS 0x%X, %s)\n"), CDD_TX[9], (sum == CDD_TX[9]) ? _T("OK") : _T("NG"));
					if (sum == CDD_TX[9]) {

						printf("request to process command %02x\n", CDD_TX[0]);

						NeoCDProcessCommand();

						if (CDD_TX[0]) {

							if (NeoCDAssyStatus == 1) {
								if (CDEmuGetStatus() == idle) {
									NeoCDAssyStatus = 0x0E;
									bNeoCDLoadSector = false;
								}
							}

							CDD_RX[0] = NeoCDAssyStatus;

							// compute checksum

							sum = 0;

							for (INT32 i = 0; i < 9; i++) {
								sum += CDD_RX[i];
							}
							CDD_RX[9] = ~(sum + 5) & 0x0F;
						}
					}
				}
			} else {

				// status send complete

//              if (CDD_RX[0] || CDD_RX[1]) {
//                  INT32  sum = 0;
//
//                  bprintf(PRINT_NORMAL, _T("  - CD mechanism status send completed : 0x"));
//                  for (INT32 i = 0; i < 9; i++) {
//                      bprintf(PRINT_NORMAL, _T("%X"), CDD_RX[i]);
//                      sum += CDD_RX[i];
//                  }
//                  sum = ~(sum + 5) & 0x0F;
//                  bprintf(PRINT_NORMAL, _T(" (CS 0x%X, %s)\n"), CDD_RX[9], (sum == CDD_RX[9]) ? _T("OK") : _T("NG"));
//              }

//              if (NeoCDAssyStatus == 0xE) {
//                  NeoCDAssyStatus = 9;
//              }
			}

		}
		bNeoCDCommsSend = send;
	}
	bNeoCDCommsClock = clock;
}


void lc89510_temp_device::CDC_UpdateHEAD(void) // segacd
{
	if (CDC_CTRLB1 & 0x01)
	{
		CDC_HEADB0 = CDC_HEADB1 = CDC_HEADB2 = CDC_HEADB3 = 0x00;
	}
	else
	{
		UINT32 msf = lba_to_msf_alt(SCD_CURLBA+150);
		CDC_HEADB0 = to_bcd (((msf & 0x00ff0000)>>16), true);
		CDC_HEADB1 = to_bcd (((msf & 0x0000ff00)>>8), true);
		CDC_HEADB2 = to_bcd (((msf & 0x000000ff)>>0), true);
		CDC_HEADB3 = 0x01;
	}
}

void lc89510_temp_device::LC8951UpdateHeader() // neocd
{
	NeoCDLBAToMSF(SCD_CURLBA);

	if (LC8951RegistersW[REG_W_CTRL1] & 1) {

		// HEAD registers have sub-header

		LC8951RegistersR[REG_R_HEAD0] = 0;													// HEAD0
		LC8951RegistersR[REG_R_HEAD1] = 0;													// HEAD1
		LC8951RegistersR[REG_R_HEAD2] = 0;													// HEAD2
		LC8951RegistersR[REG_R_HEAD3] = 0;													// HEAD3

	} else {

		// HEAD registers have header

		LC8951RegistersR[REG_R_HEAD0] = ((NeoCDSectorMin / 10) << 4) | (NeoCDSectorMin % 10);	// HEAD0
		LC8951RegistersR[REG_R_HEAD1] = ((NeoCDSectorSec / 10) << 4) | (NeoCDSectorSec % 10);	// HEAD1
		LC8951RegistersR[REG_R_HEAD2] = ((NeoCDSectorFrm / 10) << 4) | (NeoCDSectorFrm % 10);	// HEAD2
		LC8951RegistersR[REG_R_HEAD3] = 1;													// HEAD3
	}
}

char* lc89510_temp_device::LC8915InitTransfer()
{
	if (!LC8951RegistersW[REG_W_DTTRG]) {
		//bprintf(PRINT_ERROR, _T("    LC8951 DTTRG status invalid\n"));
		return NULL;
	}
	if (!(LC8951RegistersW[REG_W_IFCTRL] & 0x02)) {
		//bprintf(PRINT_ERROR, _T("    LC8951 DOUTEN status invalid\n"));
		return NULL;
	}
	if (((LC8951RegistersW[REG_W_DACH] << 8) | LC8951RegistersW[REG_W_DACL]) + (NeoCDDMACount << 1) > 2352) {
		//bprintf(PRINT_ERROR, _T("    DMA transfer exceeds current sector in LC8951 external buffer\n"));
		return NULL;
	}

	return NeoCDSectorData + ((LC8951RegistersW[REG_W_DACH] << 8) | LC8951RegistersW[REG_W_DACL]);
}

void lc89510_temp_device::LC8915EndTransfer()
{
	LC8951RegistersW[REG_W_DTTRG]  = 0x00;												// reset DTTRG

	LC8951RegistersR[REG_R_IFSTAT] |= 0x48;												//   set DTEI & DTBSY
	if (LC8951RegistersW[REG_W_IFCTRL] & 0x40) {

		// trigger DTE interrupt

		// the Neo Geo CD doesn't use the DTE interrupt
		// nIRQAcknowledge &= ~0x20;
		// NeoCDIRQUpdate(0);

	}
}

void lc89510_temp_device::LC8951Reset()
{
	memset(LC8951RegistersR, 0, sizeof(LC8951RegistersR));
	memset(LC8951RegistersW, 0, sizeof(LC8951RegistersW));
	LC8951RegistersR[REG_R_IFSTAT] = 0xFF;
	LC8951RegistersR[REG_R_STAT3] = 0x80;

	LC8951UpdateHeader();
}

void lc89510_temp_device::set_DMA_regs(int offset, UINT16 wordValue)
{
	switch (offset)
	{
		case 0x0064:
			NeoCDDMAAddress1 &= 0x0000FFFF;
			NeoCDDMAAddress1 |= wordValue << 16;
			break;
		case 0x0066:
			NeoCDDMAAddress1 &= 0xFFFF0000;
			NeoCDDMAAddress1 |= wordValue;
			break;
		case 0x0068:
			NeoCDDMAAddress2 &= 0x0000FFFF;
			NeoCDDMAAddress2 |= wordValue << 16;
			break;
		case 0x006A:
			NeoCDDMAAddress2 &= 0xFFFF0000;
			NeoCDDMAAddress2 |= wordValue;
			break;
		case 0x006C:
			NeoCDDMAValue1 = wordValue;
			break;
		case 0x006E:
			NeoCDDMAValue2 = wordValue;
			break;
		case 0x0070:
			NeoCDDMACount &= 0x0000FFFF;
			NeoCDDMACount |= wordValue << 16;
			break;
		case 0x0072:
			NeoCDDMACount &= 0xFFFF0000;
			NeoCDDMACount |= wordValue;
			break;

		case 0x007E:
			NeoCDDMAMode = wordValue;
//          bprintf(PRINT_NORMAL, _T("  - DMA controller 0x%2X -> 0x%04X (PC: 0x%06X)\n"), sekAddress & 0xFF, wordValue, SekGetPC(-1));
			break;

	}
}

void lc89510_temp_device::reset_NeoCd(void)
{
	{
		cdrom_image_device *cddevice = machine().device<cdrom_image_device>("cdrom");
		if ( cddevice )
		{
			segacd.cd = cddevice->get_cdrom_file();
			if ( segacd.cd )
			{
				printf("found cd\n");
				segacd.toc = cdrom_get_toc( segacd.cd );
				cdda_set_cdrom( m_cdda, segacd.cd );
				cdda_stop_audio( m_cdda ); //stop any pending CD-DA
			}
		}
	}
}


/*
 *  CD-ROM / DMA control
 *
 *  DMA

    FF0061  Write 0x40 means start DMA transfer
    FF0064  Source address (in copy mode), Target address (in filll mode)
    FF0068  Target address (in copy mode)
    FF006C  Fill word
    FF0070  Words count
    FF007E  \
    ......   | DMA programming words?   NeoGeoCD uses Sanyo Puppet LC8359 chip to
    FF008E  /                           interface with CD, and do DMA transfers

    Memory access control

    FF011C  DIP SWITCH (Region code)
    FF0105  Area Selector (5 = FIX, 0 = SPR, 4 = Z80, 1 = PCM)
    FF01A1  Sprite bank selector
    FF01A3  PCM bank selector
    FF0120  Prepare sprite area for transfer
    FF0122  Prepare PCM area for transfer
    FF0126  Prepare Z80 area for transfer
    FF0128  Prepare Fix area for transfer
    FF0140  Terminate work on Spr Area  (Sprites must be decoded here)
    FF0142  Terminate work on Pcm Area
    FF0146  Terminate work on Z80 Area  (Z80 needs to be reset)
    FF0148  Terminate work on Fix Area

    CD-ROM:
    0xff0102 == 0xF0 start cd transfer
    int m=bcd(fast_r8(0x10f6c8));
    int s=bcd(fast_r8(0x10f6c9));
    int f=bcd(fast_r8(0x10f6ca));
    int seccount=fast_r16(0x10f688);

    inisec=((m*60)+s)*75+f;
    inisec-=150;
    dstaddr=0x111204; // this must come from somewhere

    the value @ 0x10f688 is decremented each time a sector is read until it's 0.

 *
 */

void lc89510_temp_device::Read_LBA_To_Buffer_NeoCD()
{
	if ((nff0002 & 0x0500)) {
		if (NeoCDAssyStatus == 1 && bNeoCDLoadSector) {

//          if (LC8951RegistersW[REG_W_CTRL0] & 0x80) {
				SCD_CURLBA++;
				SCD_CURLBA = CDEmuLoadSector(SCD_CURLBA, NeoCDSectorData + 4) -1;
//          }

			if (LC8951RegistersW[REG_W_CTRL0] & 0x80) {
				LC8951UpdateHeader();

				LC8951RegistersR[REG_R_STAT0] = 0x80;										// STAT0
				LC8951RegistersR[REG_R_STAT1] = 0;											// STAT1
				LC8951RegistersR[REG_R_STAT2] = 0x10;										// STAT2
				LC8951RegistersR[REG_R_STAT3] = 0;											// STAT3

//              bprintf(PRINT_IMPORTANT, _T("    Sector %08i (%02i:%02i:%02i) read\n"), SCD_CURLBA, NeoCDSectorMin, NeoCDSectorSec, NeoCDSectorFrm);

// CDZ protection hack? (error correction on the CDC should correct this?)
#if 1
				if (NeoCDSectorData[4 + 64] == 'g' && !strncmp(NeoCDSectorData + 4, "Copyright by SNK", 16)) {
//                  printf(PRINT_ERROR, _T("    simulated CDZ protection error\n"));
//                  bprintf(PRINT_ERROR, _T("    %.70hs\n"), NeoCDSectorData + 4);

					NeoCDSectorData[4 + 64] = 'f';

					// LC8951RegistersR[REG_R_STATB0] = 0x00;                                 // STAT0
				}
#endif

				nIRQAcknowledge &= ~0x20;
				NeoCDIRQUpdate(0);

				LC8951RegistersR[REG_R_IFSTAT] &= ~0x20;

//              bprintf(PRINT_IMPORTANT, _T("    DECI interrupt triggered\n"));
			}
		}

		bNeoCDLoadSector = true;
//      bNeoCDLoadSector = false;
	}
}



void lc89510_temp_device::scd_ctrl_checks(running_machine& machine)
{
	CDC_STATB0 = 0x80;

	(CDC_CTRLB0 & 0x10) ? (CDC_STATB2 = CDC_CTRLB1 & 0x08) : (CDC_STATB2 = CDC_CTRLB1 & 0x0C);
	(CDC_CTRLB0 & 0x02) ? (CDC_STATB3 = 0x20) : (CDC_STATB3 = 0x00);

	if (CDC_IFCTRL & 0x20)
	{
		CHECK_SCD_LV5_INTERRUPT
		CDC_IFSTAT &= ~0x20;
		CDC_DECODE = 0;
	}
}

void lc89510_temp_device::scd_advance_current_readpos(void)
{
	SCD_CURLBA++;

	CDC_WA += SECTOR_SIZE;
	CDC_PT += SECTOR_SIZE;

	CDC_WA &= 0x7fff;
	CDC_PT &= 0x7fff;
}

int lc89510_temp_device::Read_LBA_To_Buffer(running_machine& machine)
{
	bool data_track = false;
	if (CDD_CONTROL & 0x0100) data_track = true;

	if (data_track)
		cdrom_read_data(segacd.cd, SCD_CURLBA, SCD_BUFFER, CD_TRACK_MODE1);

	CDC_UpdateHEAD();

	if (!data_track)
	{
		scd_advance_current_readpos();
	}

	if (CDC_CTRLB0 & 0x80)
	{
		if (CDC_CTRLB0 & 0x04)
		{
			if (data_track)
			{
				scd_advance_current_readpos();

				memcpy(&CDC_BUFFER[CDC_PT + 4], SCD_BUFFER, 2048);
				CDC_BUFFER[CDC_PT+0] = CDC_HEADB0;
				CDC_BUFFER[CDC_PT+1] = CDC_HEADB1;
				CDC_BUFFER[CDC_PT+2] = CDC_HEADB2;
				CDC_BUFFER[CDC_PT+3] = CDC_HEADB3;
			}
			else
			{
				memcpy(&CDC_BUFFER[CDC_PT], SCD_BUFFER, SECTOR_SIZE);
			}
		}

		scd_ctrl_checks(machine);
	}


	return 0;
}




void lc89510_temp_device::SekWriteWord(UINT32 a, UINT16 d)
{
//  printf("write word %08x %04x\n", a, d);
	dma_space->write_word(a,d);
}

void lc89510_temp_device::SekWriteByte(UINT32 a, UINT8 d)
{
//  printf("write byte %08x %02x\n", a, d);
	dma_space->write_byte(a,d);
}

UINT32 lc89510_temp_device::SekReadByte(UINT32 a)
{
//  printf("read byte %08x\n", a);
	return dma_space->read_byte(a);
}


UINT32 lc89510_temp_device::SekReadWord(UINT32 a)
{
//  printf("read WORD %08x\n", a);
	return dma_space->read_word(a);
}


void lc89510_temp_device::NeoCDIRQUpdate(UINT8 byteValue)
{
	// do we also need to check the regular interrupts like FBA?

	nIRQAcknowledge |= (byteValue & 0x38);

	if ((nIRQAcknowledge & 0x08) == 0) {
		nNeoCDIRQVector = 0x17;
		nNeoCDIRQVectorAck = 1;
		machine().device("maincpu")->execute().set_input_line(4, HOLD_LINE);
		return;
	}
	if ((nIRQAcknowledge & 0x10) == 0) {
		nNeoCDIRQVector = 0x16;
		nNeoCDIRQVectorAck = 1;
		machine().device("maincpu")->execute().set_input_line(4, HOLD_LINE);
		return;
	}
	if ((nIRQAcknowledge & 0x20) == 0) {
		nNeoCDIRQVector = 0x15;
		nNeoCDIRQVectorAck = 1;
		machine().device("maincpu")->execute().set_input_line(4, HOLD_LINE);
		return;
	}
}

void lc89510_temp_device::nff0002_set(UINT16 wordValue)
{
	nff0002 = wordValue;
}

void lc89510_temp_device::nff0016_set(UINT16 wordValue)
{
	nff0016 = wordValue;
}

UINT16 lc89510_temp_device::nff0016_r(void) { return nff0016; }

UINT16 lc89510_temp_device::nLC8951_r(void)
{
	int regno = CDC_REG0 & 0xf;
	INT32 reg = LC8951RegistersR[regno];

	switch (regno) {
		case 0x03:														// DBCH
			LC8951RegistersR[REG_R_DBCH] &=  0x0F;
			LC8951RegistersR[REG_R_DBCH] |=  (LC8951RegistersR[REG_R_IFSTAT] & 0x40) ? 0x00 : 0xF0;
			break;
		case 0x0D:														// STAT3
			LC8951RegistersR[REG_R_IFSTAT] |= 0x20;								// reset DECI
			// bprintf(PRINT_ERROR, _T("  - DECI (PC: 0x%06X)\n"), SekGetPC(-1));
			break;
	}

	CDC_REG0 = (regno + 1) & 0x0F;
	return reg;
}

void lc89510_temp_device::nLC8951_w(UINT16 byteValue)
{
	int regno = CDC_REG0 & 0xf;

	switch (regno) {
		case 3:															// DBCH
			LC8951RegistersW[REG_W_DBCH]  = byteValue & 0x0F;
			break;
		case 6:															// DTTRG
			LC8951RegistersW[REG_W_DTTRG]  = ~0x00;
			LC8951RegistersR[REG_R_IFSTAT] &= ~0x08;
			break;
		case 7:															// DTACK
			LC8951RegistersW[REG_W_DTACK]  = ~0x00;
			LC8951RegistersR[REG_R_IFSTAT] &= ~0x40;
			break;
//          case 10:
//              LC8951RegistersW[regno] = byteValue;
//              bprintf(PRINT_NORMAL, _T("  - CTRL0 -> %02X (PC: 0x%06X)\n"), LC8951RegistersW[regno], byteValue, SekGetPC(-1));
//              break;
		case 11:
			LC8951RegistersW[REG_W_CTRL1]  = byteValue;							// CTRL1
			LC8951UpdateHeader();
			break;
		case 15:
			LC8951Reset();
			break;
		default:
			LC8951RegistersW[regno] = byteValue;
	}
	CDC_REG0 = (regno + 1) & 0x0F;
}


