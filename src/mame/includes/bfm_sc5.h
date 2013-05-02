#include "cpu/m68000/m68000.h"
#include "includes/bfm_sc45.h"

class bfm_sc5_state : public driver_device
{
public:
	bfm_sc5_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, "maincpu")
	{ }

protected:

	// devices
	required_device<cpu_device> m_maincpu;
public:
	DECLARE_DRIVER_INIT(sc5);
	DECLARE_WRITE_LINE_MEMBER(bfm_sc5_ym_irqhandler);
	DECLARE_READ8_MEMBER( sc5_10202F0_r );
	DECLARE_WRITE8_MEMBER( sc5_10202F0_w );

};
