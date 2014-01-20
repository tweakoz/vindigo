/*****************************************************************************
 *
 * machine/i8271.h
 *
 ****************************************************************************/

#ifndef I8271_H_
#define I8271_H_

/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

typedef void (*i8271_dma_request)(device_t &device, int state, int read_);
#define I8271_DMA_REQUEST(name) void name(device_t &device, int state, int read_)

struct i8271_interface
{
	devcb_write_line m_interrupt_cb;
	i8271_dma_request m_dma_request;
	const char *m_floppy_drive_tags[2];
};

/***************************************************************************
    MACROS
***************************************************************************/

class i8271_device : public device_t,
							public i8271_interface
{
public:
	i8271_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~i8271_device() {}

	DECLARE_READ8_MEMBER(read);
	DECLARE_WRITE8_MEMBER(write);

	DECLARE_READ8_MEMBER(dack_r);
	DECLARE_WRITE8_MEMBER(dack_w);

	DECLARE_READ8_MEMBER(data_r);
	DECLARE_WRITE8_MEMBER(data_w);
	
protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
	
private:
	// internal state
	enum
	{
		TIMER_DATA_CALLBACK,
		TIMER_TIMED_COMMAND_COMPLETE
	};
	
	int m_flags;
	int m_state;
	unsigned char m_Command;
	unsigned char m_StatusRegister;
	unsigned char m_CommandRegister;
	unsigned char m_ResultRegister;
	unsigned char m_ParameterRegister;
	unsigned char m_ResetRegister;
	unsigned char m_data;

	/* number of parameters required after command is specified */
	unsigned long m_ParameterCount;
	/* number of parameters written so far */
	unsigned long m_ParameterCountWritten;

	unsigned char m_CommandParameters[8];

	/* current track for each drive */
	unsigned long   m_CurrentTrack[2];

	/* 2 bad tracks for drive 0, followed by 2 bad tracks for drive 1 */
	unsigned long   m_BadTracks[4];

	/* mode special register */
	unsigned long m_Mode;


	/* drive outputs */
	int m_drive;
	int m_side;

	/* drive control output special register */
	int m_drive_control_output;
	/* drive control input special register */
	int m_drive_control_input;

	unsigned long m_StepRate;
	unsigned long m_HeadSettlingTime;
	unsigned long m_IndexCountBeforeHeadUnload;
	unsigned long m_HeadLoadTime;

	/* id on disc to find */
	//int m_ID_C;
	//int m_ID_H;
	int m_ID_R;
	int m_ID_N;

	/* id of data for read/write */
	int m_data_id;

	int m_ExecutionPhaseTransferCount;
	char *m_pExecutionPhaseData;
	int m_ExecutionPhaseCount;

	/* sector counter and id counter */
	int m_Counter;

	/* ==0, to cpu, !=0 =from cpu */
	//int m_data_direction;

	emu_timer *m_data_timer;
	emu_timer *m_command_complete_timer;
	
	devcb_resolved_write_line m_interrupt_func;
	
	device_t *current_image();
	void seek_to_track(int track);
	void load_bad_tracks(int surface);
	void write_bad_track(int surface, int track, int data);
	void write_current_track(int surface, int track);
	int read_current_track(int surface);
	int read_bad_track(int surface, int track);
	void get_drive();
	void check_all_parameters_written();
	void update_state();
	void initialise_execution_phase_read(int transfer_size);
	void initialise_execution_phase_write(int transfer_size);
	void command_execute();
	void command_continue();
	void command_complete(int result, int int_rq);
	void timed_command_complete();
	void data_request();
	void clear_data_request();
	void timed_data_request();
	/* locate sector for read/write operation */
	int find_sector();
	/* do a read operation */
	void do_read();
	void do_write();
	void do_read_id();
	void set_irq_state(int);
	void set_dma_drq();
};

extern const device_type I8271;


/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/

#define MCFG_I8271_ADD(_tag, _intrf) \
	MCFG_DEVICE_ADD(_tag, I8271, 0) \
	MCFG_DEVICE_CONFIG(_intrf)

#endif /* I8271_H_ */
