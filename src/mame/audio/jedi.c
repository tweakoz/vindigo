/***************************************************************************

    Atari Return of the Jedi hardware

    driver by Dan Boris

***************************************************************************/

#include "emu.h"
#include "cpu/m6502/m6502.h"
#include "sound/tms5220.h"
#include "sound/pokey.h"
#include "includes/jedi.h"



/*************************************
 *
 *  Start
 *
 *************************************/

static SOUND_START( jedi )
{
	jedi_state *state = machine.driver_data<jedi_state>();

	/* set up save state */
	state->save_item(NAME(state->m_audio_latch));
	state->save_item(NAME(state->m_audio_ack_latch));
	state->save_item(NAME(state->m_speech_strobe_state));
}



/*************************************
 *
 *  Reset
 *
 *************************************/

static SOUND_RESET( jedi )
{
	jedi_state *state = machine.driver_data<jedi_state>();

	/* init globals */
	state->m_audio_latch = 0;
	state->m_audio_ack_latch = 0;
	*state->m_audio_comm_stat = 0;
	*state->m_speech_data = 0;
	state->m_speech_strobe_state = 0;
}



/*************************************
 *
 *  Interrupt handling
 *
 *************************************/

WRITE8_MEMBER(jedi_state::irq_ack_w)
{
	machine().device("audiocpu")->execute().set_input_line(M6502_IRQ_LINE, CLEAR_LINE);
}



/*************************************
 *
 *  Main CPU -> Sound CPU communications
 *
 *************************************/

WRITE8_MEMBER(jedi_state::jedi_audio_reset_w)
{
	machine().device("audiocpu")->execute().set_input_line(INPUT_LINE_RESET, (data & 1) ? CLEAR_LINE : ASSERT_LINE);
}


static TIMER_CALLBACK( delayed_audio_latch_w )
{
	jedi_state *state = machine.driver_data<jedi_state>();

	state->m_audio_latch = param;
	*state->m_audio_comm_stat |= 0x80;
}


WRITE8_MEMBER(jedi_state::jedi_audio_latch_w)
{
	machine().scheduler().synchronize(FUNC(delayed_audio_latch_w), data);
}


READ8_MEMBER(jedi_state::audio_latch_r)
{

	*m_audio_comm_stat &= ~0x80;
	return m_audio_latch;
}


CUSTOM_INPUT_MEMBER(jedi_state::jedi_audio_comm_stat_r)
{
	return *m_audio_comm_stat >> 6;
}



/*************************************
 *
 *  Sound CPU -> Main CPU communications
 *
 *************************************/

READ8_MEMBER(jedi_state::jedi_audio_ack_latch_r)
{

	*m_audio_comm_stat &= ~0x40;
	return m_audio_ack_latch;
}


WRITE8_MEMBER(jedi_state::audio_ack_latch_w)
{

	m_audio_ack_latch = data;
	*m_audio_comm_stat |= 0x40;
}



/*************************************
 *
 *  Speech access
 *
 *************************************/

WRITE8_MEMBER(jedi_state::speech_strobe_w)
{
	int new_speech_strobe_state = (~offset >> 8) & 1;

	if ((new_speech_strobe_state != m_speech_strobe_state) && new_speech_strobe_state)
	{
		device_t *tms = machine().device("tms");
		tms5220_data_w(tms, space, 0, *m_speech_data);
	}
	m_speech_strobe_state = new_speech_strobe_state;
}


READ8_MEMBER(jedi_state::speech_ready_r)
{
	return (tms5220_readyq_r(machine().device("tms"))) << 7;
}


WRITE8_MEMBER(jedi_state::speech_reset_w)
{
	/* not supported by the TMS5220 emulator */
}



/*************************************
 *
 *  Audio CPU memory handlers
 *
 *************************************/

static ADDRESS_MAP_START( audio_map, AS_PROGRAM, 8, jedi_state )
	AM_RANGE(0x0000, 0x07ff) AM_RAM
	AM_RANGE(0x0800, 0x080f) AM_MIRROR(0x07c0) AM_DEVREADWRITE("pokey1", pokey_device, read, write)
	AM_RANGE(0x0810, 0x081f) AM_MIRROR(0x07c0) AM_DEVREADWRITE("pokey2", pokey_device, read, write)
	AM_RANGE(0x0820, 0x082f) AM_MIRROR(0x07c0) AM_DEVREADWRITE("pokey3", pokey_device, read, write)
	AM_RANGE(0x0830, 0x083f) AM_MIRROR(0x07c0) AM_DEVREADWRITE("pokey4", pokey_device, read, write)
	AM_RANGE(0x1000, 0x1000) AM_MIRROR(0x00ff) AM_READNOP AM_WRITE(irq_ack_w)
	AM_RANGE(0x1100, 0x1100) AM_MIRROR(0x00ff) AM_READNOP AM_WRITEONLY AM_SHARE("speech_data")
	AM_RANGE(0x1200, 0x13ff) AM_READNOP AM_WRITE(speech_strobe_w)
	AM_RANGE(0x1400, 0x1400) AM_MIRROR(0x00ff) AM_READNOP AM_WRITE(audio_ack_latch_w)
	AM_RANGE(0x1500, 0x1500) AM_MIRROR(0x00ff) AM_READNOP AM_WRITE(speech_reset_w)
	AM_RANGE(0x1600, 0x17ff) AM_NOP
	AM_RANGE(0x1800, 0x1800) AM_MIRROR(0x03ff) AM_READ(audio_latch_r) AM_WRITENOP
	AM_RANGE(0x1c00, 0x1c00) AM_MIRROR(0x03fe) AM_READ(speech_ready_r) AM_WRITENOP
	AM_RANGE(0x1c01, 0x1c01) AM_MIRROR(0x03fe) AM_READONLY AM_WRITENOP AM_SHARE("audio_comm_stat")
	AM_RANGE(0x2000, 0x7fff) AM_NOP
	AM_RANGE(0x8000, 0xffff) AM_ROM
ADDRESS_MAP_END



/*************************************
 *
 *  Machine driver
 *
 *************************************/

MACHINE_CONFIG_FRAGMENT( jedi_audio )

	MCFG_CPU_ADD("audiocpu", M6502, JEDI_AUDIO_CPU_CLOCK)
	MCFG_CPU_PROGRAM_MAP(audio_map)

	MCFG_SOUND_START(jedi)
	MCFG_SOUND_RESET(jedi)

	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_POKEY_ADD("pokey1", JEDI_POKEY_CLOCK)
	MCFG_POKEY_OUTPUT_OPAMP(RES_K(1), 0.0, 5.0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.30)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.30)

	MCFG_POKEY_ADD("pokey2", JEDI_POKEY_CLOCK)
	MCFG_POKEY_OUTPUT_OPAMP(RES_K(1), 0.0, 5.0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.30)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.30)

	MCFG_POKEY_ADD("pokey3", JEDI_POKEY_CLOCK)
	MCFG_POKEY_OUTPUT_OPAMP(RES_K(1), 0.0, 5.0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 0.30)

	MCFG_POKEY_ADD("pokey4", JEDI_POKEY_CLOCK)
	MCFG_POKEY_OUTPUT_OPAMP(RES_K(1), 0.0, 5.0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 0.30)

	MCFG_SOUND_ADD("tms", TMS5220, JEDI_TMS5220_CLOCK)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "rspeaker", 1.0)
MACHINE_CONFIG_END
