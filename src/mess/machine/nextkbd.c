
#include "nextkbd.h"

const device_type NEXTKBD = &device_creator<nextkbd_device>;

DEVICE_ADDRESS_MAP_START(amap, 32, nextkbd_device)
	AM_RANGE(0x0, 0x3) AM_READWRITE(ctrl_r,  ctrl_w)
	AM_RANGE(0x4, 0x7) AM_READWRITE(ctrl2_r, ctrl2_w)
	AM_RANGE(0x8, 0xb) AM_READWRITE(data_r,  data_w)
ADDRESS_MAP_END

nextkbd_device::nextkbd_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) : device_t(mconfig, NEXTKBD, "NEXTKBD", tag, owner, clock, "nextkbd", __FILE__)
{
}

void nextkbd_device::setup(line_cb_t _int_change_cb, line_cb_t _int_power_cb, line_cb_t _int_nmi_cb)
{
	int_change_cb = _int_change_cb;
	int_power_cb = _int_power_cb;
	int_nmi_cb = _int_nmi_cb;
}

void nextkbd_device::device_start()
{
	kbd_timer = timer_alloc(0);

	save_item(NAME(control));
	save_item(NAME(control2));
	save_item(NAME(data));
	save_item(NAME(fifo_ir));
	save_item(NAME(fifo_iw));
	save_item(NAME(fifo_size));
	save_item(NAME(fifo));
	save_item(NAME(modifiers_state));
	save_item(NAME(nmi_active));
}

void nextkbd_device::device_reset()
{
	control = 0;
	control2 = 0;
	data = 0;
	fifo_ir = 0;
	fifo_iw = 0;
	fifo_size = 0;
	memset(fifo, 0, sizeof(fifo));
	modifiers_state = 0;
	nmi_active = false;
}

void nextkbd_device::send()
{
	if(control & FLAG_DATA)
		return;

	data = fifo_pop();
	control |= FLAG_DATA;
	if(!(control & FLAG_INT)) {
		control |= FLAG_INT;
		int_change_cb(true);
	}
}

void nextkbd_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	if(fifo_empty())
		return;

	send();
}

void nextkbd_device::fifo_push(UINT32 val)
{
	if(fifo_size == FIFO_SIZE)
		return;

	fifo_size++;
	fifo[fifo_iw++] = val;
	if(fifo_iw == FIFO_SIZE)
		fifo_iw = 0;
	if(fifo_size == 1)
		kbd_timer->adjust(attotime::from_hz(1000));
}

UINT32 nextkbd_device::fifo_pop()
{
	if(fifo_size == 0)
		return 0;

	fifo_size--;
	UINT32 res = fifo[fifo_ir++];
	if(fifo_ir == FIFO_SIZE)
		fifo_ir = 0;
	if(fifo_size == 0)
		kbd_timer->adjust(attotime::never);

	return res;
}

bool nextkbd_device::fifo_empty() const
{
	return !fifo_size;
}

READ32_MEMBER( nextkbd_device::ctrl_r )
{
	//  logerror("nextkbd: ctrl_r %08x @ %08x\n", control, mem_mask);
	return control;
}

READ32_MEMBER( nextkbd_device::ctrl2_r )
{
	return control2;
}

READ32_MEMBER( nextkbd_device::data_r )
{
	UINT32 old = control;
	control &= ~(FLAG_DATA|FLAG_INT);
	if(old & FLAG_INT)
		int_change_cb(false);
	return data;
}

WRITE32_MEMBER( nextkbd_device::ctrl_w )
{
	UINT32 diff;
	if(data & FLAG_RESET) {
		diff = control;
		device_reset();
	} else {
		diff = control ^ data;
		control = data;
	}
	if(diff & FLAG_INT)
		int_change_cb(control & FLAG_INT);
	//  logerror("nextkbd: ctrl_w %08x @ %08x\n", data, mem_mask);
}

WRITE32_MEMBER( nextkbd_device::ctrl2_w )
{
	//  logerror("nextkbd: ctrl2_w %08x @ %08x\n", data, mem_mask);
}

WRITE32_MEMBER( nextkbd_device::data_w )
{
}

INPUT_CHANGED_MEMBER( nextkbd_device::update )
{
	int bank = (int)(FPTR)param;
	switch(bank) {
	case 0: case 1: case 2: {
		int index;
		for(index=0; index < 32; index++)
			if(field.mask() & (1 << index))
				break;
		assert(index != 32);
		index += bank*32;
		UINT16 val = index | modifiers_state | KEYVALID;
		if(!newval)
			val |= KEYDOWN;
		if(val == 0x8826 || val == 0x884a) {
			nmi_active = true;
			int_nmi_cb(true);
		} else if(nmi_active) {
			nmi_active = false;
			int_nmi_cb(false);
		}
		fifo_push(val | (1<<28));
		send();
		break;
	}

	case 3:
		if(newval)
			modifiers_state |= field.mask();
		else
			modifiers_state &= ~field.mask();
		fifo_push(modifiers_state | (1<<28));
		send();
		break;

	case 4:
		if(field.mask() & 1)
			int_power_cb(newval & 1);
		break;
	}
}

static INPUT_PORTS_START(nextkbd_keymap)
	PORT_START("0")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0)
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0)
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0)
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_BACKSLASH)  PORT_CHAR('\\') PORT_CHAR('|')
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_OPENBRACE)  PORT_CHAR('[')  PORT_CHAR('{')
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_CHAR(']')  PORT_CHAR('}')
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_I)          PORT_CHAR('i')  PORT_CHAR('I')
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_O)          PORT_CHAR('o')  PORT_CHAR('O')
	PORT_BIT(0x00000100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_P)          PORT_CHAR('p')  PORT_CHAR('P')
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_LEFT)       PORT_CHAR(UCHAR_MAMEKEY(LEFT))
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0)
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_0_PAD)      PORT_CHAR(UCHAR_MAMEKEY(0_PAD))
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_DEL_PAD)    PORT_CHAR(UCHAR_MAMEKEY(DEL_PAD))
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_ENTER_PAD)  PORT_CHAR(UCHAR_MAMEKEY(ENTER_PAD))
	PORT_BIT(0x00004000, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0)
	PORT_BIT(0x00008000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_DOWN)       PORT_CHAR(UCHAR_MAMEKEY(DOWN))
	PORT_BIT(0x00010000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_RIGHT)      PORT_CHAR(UCHAR_MAMEKEY(RIGHT))
	PORT_BIT(0x00020000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_1_PAD)      PORT_CHAR(UCHAR_MAMEKEY(1_PAD))
	PORT_BIT(0x00040000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_4_PAD)      PORT_CHAR(UCHAR_MAMEKEY(4_PAD))
	PORT_BIT(0x00080000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_6_PAD)      PORT_CHAR(UCHAR_MAMEKEY(6_PAD))
	PORT_BIT(0x00100000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_3_PAD)      PORT_CHAR(UCHAR_MAMEKEY(3_PAD))
	PORT_BIT(0x00200000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_PLUS_PAD)   PORT_CHAR(UCHAR_MAMEKEY(PLUS_PAD))
	PORT_BIT(0x00400000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_UP)         PORT_CHAR(UCHAR_MAMEKEY(UP))
	PORT_BIT(0x00800000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_2_PAD)      PORT_CHAR(UCHAR_MAMEKEY(2_PAD))
	PORT_BIT(0x01000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_5_PAD)      PORT_CHAR(UCHAR_MAMEKEY(5_PAD))
	PORT_BIT(0x02000000, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0)
	PORT_BIT(0x04000000, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0)
	PORT_BIT(0x08000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_BACKSPACE)  PORT_CHAR(8)
	PORT_BIT(0x10000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_EQUALS)     PORT_CHAR('=')  PORT_CHAR('+')
	PORT_BIT(0x20000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_MINUS)      PORT_CHAR('-')  PORT_CHAR('_')
	PORT_BIT(0x40000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_8)          PORT_CHAR('8')  PORT_CHAR('*')
	PORT_BIT(0x80000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 0) PORT_CODE(KEYCODE_9)          PORT_CHAR('9')  PORT_CHAR('(')

	PORT_START("1")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_0)          PORT_CHAR('0')  PORT_CHAR(')')
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_7_PAD)      PORT_CHAR(UCHAR_MAMEKEY(7_PAD))
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_8_PAD)      PORT_CHAR(UCHAR_MAMEKEY(8_PAD))
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_9_PAD)      PORT_CHAR(UCHAR_MAMEKEY(9_PAD))
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_MINUS_PAD)  PORT_CHAR(UCHAR_MAMEKEY(MINUS_PAD))
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_ASTERISK)   PORT_CHAR(UCHAR_MAMEKEY(ASTERISK))
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_TILDE)      PORT_CHAR('`')  PORT_CHAR('~')
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) // Keypad = ?
	PORT_BIT(0x00000100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_SLASH_PAD)  PORT_CHAR(UCHAR_MAMEKEY(SLASH_PAD))
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1)
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_ENTER)      PORT_CHAR(13)
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_QUOTE)      PORT_CHAR('\'') PORT_CHAR('"')
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_COLON)      PORT_CHAR(';')  PORT_CHAR(':')
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_L)          PORT_CHAR('l')  PORT_CHAR('L')
	PORT_BIT(0x00004000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_COMMA)      PORT_CHAR(',')  PORT_CHAR('<')
	PORT_BIT(0x00008000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_STOP)       PORT_CHAR('.')  PORT_CHAR('>')
	PORT_BIT(0x00010000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_SLASH)      PORT_CHAR('/')  PORT_CHAR('?')
	PORT_BIT(0x00020000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_Z)          PORT_CHAR('z')  PORT_CHAR('Z')
	PORT_BIT(0x00040000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_X)          PORT_CHAR('x')  PORT_CHAR('X')
	PORT_BIT(0x00080000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_C)          PORT_CHAR('c')  PORT_CHAR('C')
	PORT_BIT(0x00100000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_V)          PORT_CHAR('v')  PORT_CHAR('V')
	PORT_BIT(0x00200000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_B)          PORT_CHAR('b')  PORT_CHAR('B')
	PORT_BIT(0x00400000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_M)          PORT_CHAR('m')  PORT_CHAR('M')
	PORT_BIT(0x00800000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_N)          PORT_CHAR('n')  PORT_CHAR('N')
	PORT_BIT(0x01000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_SPACE)      PORT_CHAR(' ')
	PORT_BIT(0x02000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_A)          PORT_CHAR('a')  PORT_CHAR('A')
	PORT_BIT(0x04000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_S)          PORT_CHAR('s')  PORT_CHAR('S')
	PORT_BIT(0x08000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_D)          PORT_CHAR('d')  PORT_CHAR('D')
	PORT_BIT(0x10000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_F)          PORT_CHAR('f')  PORT_CHAR('F')
	PORT_BIT(0x20000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_G)          PORT_CHAR('g')  PORT_CHAR('G')
	PORT_BIT(0x40000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_K)          PORT_CHAR('k')  PORT_CHAR('K')
	PORT_BIT(0x80000000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 1) PORT_CODE(KEYCODE_J)          PORT_CHAR('j')  PORT_CHAR('J')

	PORT_START("2")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_H)          PORT_CHAR('h')  PORT_CHAR('H')
	PORT_BIT(0x00000002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_TAB)        PORT_CHAR('\t')
	PORT_BIT(0x00000004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_Q)          PORT_CHAR('q')  PORT_CHAR('Q')
	PORT_BIT(0x00000008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_W)          PORT_CHAR('w')  PORT_CHAR('W')
	PORT_BIT(0x00000010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_E)          PORT_CHAR('e')  PORT_CHAR('E')
	PORT_BIT(0x00000020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_R)          PORT_CHAR('r')  PORT_CHAR('R')
	PORT_BIT(0x00000040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_U)          PORT_CHAR('u')  PORT_CHAR('U')
	PORT_BIT(0x00000080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_Y)          PORT_CHAR('y')  PORT_CHAR('Y')
	PORT_BIT(0x00000100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_T)          PORT_CHAR('t')  PORT_CHAR('T')
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_ESC)        PORT_CHAR(UCHAR_MAMEKEY(ESC))
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_1)          PORT_CHAR('1')  PORT_CHAR('!')
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_2)          PORT_CHAR('2')  PORT_CHAR('@')
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_3)          PORT_CHAR('3')  PORT_CHAR('#')
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_4)          PORT_CHAR('4')  PORT_CHAR('$')
	PORT_BIT(0x00004000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_7)          PORT_CHAR('7')  PORT_CHAR('&')
	PORT_BIT(0x00008000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_6)          PORT_CHAR('6')  PORT_CHAR('^')
	PORT_BIT(0x00010000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2) PORT_CODE(KEYCODE_5)          PORT_CHAR('5')  PORT_CHAR('%')
	PORT_BIT(0xfffe0000, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 2)

	PORT_START("modifiers")
	PORT_BIT(0x00000100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 3) PORT_CODE(KEYCODE_LCONTROL)   PORT_CHAR(UCHAR_MAMEKEY(LCONTROL)) PORT_NAME("Control")
	PORT_BIT(0x00000200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 3) PORT_CODE(KEYCODE_LSHIFT)     PORT_CHAR(UCHAR_SHIFT_1) PORT_NAME("Shift (Left)")
	PORT_BIT(0x00000400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 3) PORT_CODE(KEYCODE_RSHIFT)     PORT_CHAR(UCHAR_SHIFT_1) PORT_NAME("Shift (Right)")
	PORT_BIT(0x00000800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 3) PORT_CODE(KEYCODE_LWIN)       PORT_NAME("Command (Left)")
	PORT_BIT(0x00001000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 3) PORT_CODE(KEYCODE_RWIN)       PORT_NAME("Command (Right)")
	PORT_BIT(0x00002000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 3) PORT_CODE(KEYCODE_LALT)       PORT_CHAR(UCHAR_MAMEKEY(LALT)) PORT_NAME("Alt (Left)")
	PORT_BIT(0x00004000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 3) PORT_CODE(KEYCODE_RALT)       PORT_CHAR(UCHAR_MAMEKEY(RALT)) PORT_NAME("Alt (Right)")
	PORT_BIT(0xffff80ff, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 3)

	PORT_START("special")
	PORT_BIT(0x00000001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 4) PORT_CODE(KEYCODE_HOME)       PORT_NAME("Power")
	PORT_BIT(0xfffffffe, IP_ACTIVE_HIGH, IPT_UNUSED)   PORT_CHANGED_MEMBER(DEVICE_SELF, nextkbd_device, update, 4)
INPUT_PORTS_END

ioport_constructor nextkbd_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(nextkbd_keymap);
}
