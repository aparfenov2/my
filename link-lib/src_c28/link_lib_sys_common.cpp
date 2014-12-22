/*
* Реализаци интерфейса связи системного уровня для прямой и обратной связи с этой системой
* ЭТО АВТОКОПИЯ !!!
*/

#include "link_sys_impl_common.h"
#include "assert_impl.h"
#include <string.h>
extern "C" {
#include "hdlc.h"
#include "ring_buffer.h"
}


static link::serial_interface_t *current_serial_interface = 0;
extern "C" void rs485_put_char(char data) {
	_MY_ASSERT(current_serial_interface,return);
	current_serial_interface->send((u8*)&data,1);
}

static link::_internal_frame_receiver_t *current_frame_receiver = 0;

extern "C" void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {
	_MY_ASSERT(current_frame_receiver,return);
	current_frame_receiver->receive_frame((u8*)buffer, bytes_received);
}



void link::framer_t::sys_init() {

	static bool hdlc_initialized = false;

	if (!hdlc_initialized) {
		hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
		hdlc_initialized = true;
	}
}

void link::framer_t::send_frame(u8 *data, u32 bytes_to_send) {

	current_serial_interface = sintf;
	hdlc_tx_frame((const u8_t*)data, bytes_to_send);
}


void link::framer_t::receive(u8 *data, u32 len) {
	u32 ofs = 0;
	current_frame_receiver = this->frame_receiver;
	while (len--) {
		hdlc_on_rx_byte(data[ofs++]);
	}
}

