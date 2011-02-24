
class lsasquad_state : public driver_device
{
public:
	lsasquad_state(running_machine &machine, const driver_device_config_base &config)
		: driver_device(machine, config) { }

	/* memory pointers */
	UINT8 *      scrollram;
	UINT8 *      videoram;
	UINT8 *      spriteram;
	size_t       spriteram_size;
	size_t       videoram_size;

	/* misc */
	int sound_pending;
	int sound_nmi_enable, pending_nmi, sound_cmd, sound_result;

	/* mcu */
	UINT8 from_main, from_mcu;
	int mcu_sent, main_sent;
	UINT8 port_a_in, port_a_out, ddr_a;
	UINT8 port_b_in, port_b_out, ddr_b;

	/* devices */
	device_t *maincpu;
	device_t *audiocpu;
	device_t *mcu;
};

/*----------- defined in machine/lsasquad.c -----------*/

WRITE8_HANDLER( lsasquad_sh_nmi_disable_w );
WRITE8_HANDLER( lsasquad_sh_nmi_enable_w );
WRITE8_HANDLER( lsasquad_sound_command_w );
READ8_HANDLER( lsasquad_sh_sound_command_r );
WRITE8_HANDLER( lsasquad_sh_result_w );
READ8_HANDLER( lsasquad_sound_result_r );
READ8_HANDLER( lsasquad_sound_status_r );

READ8_HANDLER( lsasquad_68705_port_a_r );
WRITE8_HANDLER( lsasquad_68705_port_a_w );
WRITE8_HANDLER( lsasquad_68705_ddr_a_w );
READ8_HANDLER( lsasquad_68705_port_b_r );
WRITE8_HANDLER( lsasquad_68705_port_b_w );
WRITE8_HANDLER( lsasquad_68705_ddr_b_w );
WRITE8_HANDLER( lsasquad_mcu_w );
READ8_HANDLER( lsasquad_mcu_r );
READ8_HANDLER( lsasquad_mcu_status_r );

READ8_HANDLER( daikaiju_sound_status_r );
READ8_HANDLER( daikaiju_sh_sound_command_r );
READ8_HANDLER( daikaiju_mcu_status_r );


/*----------- defined in video/lsasquad.c -----------*/

SCREEN_UPDATE( lsasquad );
SCREEN_UPDATE( daikaiju );
