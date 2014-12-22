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


#define HDLC_RING_BUF_LEN	512
// ring buffer
static u8 _hdlc_ring_buf[HDLC_RING_BUF_LEN];
static ring_buffer_t hdlc_ring_buf;
// temporal buffer
static u8 hdlc_buf[HDLC_RING_BUF_LEN];

void rs485_put_char(char data) {
	ring_buffer_write_byte(&hdlc_ring_buf,data);
}

static link::_internal_frame_receiver_t *current_frame_receiver = 0;

void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {
	// call receive_frame someway
	_MY_ASSERT(current_frame_receiver, return);
	current_frame_receiver->receive_frame((u8*)buffer, bytes_received);
}




void link::framer_t::sys_init() {

	static bool hdlc_initialized = false;

	if (!hdlc_initialized) {
		ring_buffer_init(&hdlc_ring_buf,(u8_t*)_hdlc_ring_buf, HDLC_RING_BUF_LEN);
		hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
		hdlc_initialized = true;
	}
}

void link::framer_t::send_frame(u8 *data, u32 bytes_to_send) {

	hdlc_ring_buf.tail = hdlc_ring_buf.head;
	hdlc_tx_frame((const u8_t*)data, bytes_to_send);

	bytes_to_send = ring_buffer_read_data(&hdlc_ring_buf,(u8_t*)hdlc_buf, HDLC_RING_BUF_LEN);
	sintf->send(hdlc_buf,bytes_to_send);
}


void link::framer_t::receive(u8 *data, u32 len) {
	u32 ofs = 0;
	current_frame_receiver = this->frame_receiver;
	while (len--) {
		hdlc_on_rx_byte(data[ofs++]);
	}
}
