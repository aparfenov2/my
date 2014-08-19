/*
* Реализаци интерфейса связи системного уровня для прямой и обратной связи с этой системой
*/

#include "link_sys_impl.h"
#include "assert_impl.h"
#include <string.h>
#include "myvi.pb.h"
extern "C" {
#include "hdlc.h"
#include "ring_buffer.h"
}
extern "C" u16_t crc16_ccitt_calc_data(u16_t crc, u8_t* data, u16_t data_length);

using namespace link;



#define HDLC_RING_BUF_LEN	512
// ring buffer
u8 _hdlc_ring_buf[HDLC_RING_BUF_LEN];
ring_buffer_t hdlc_ring_buf;
// temporal buffer
u8 hdlc_buf[HDLC_RING_BUF_LEN];

void rs485_put_char(char data) {
	ring_buffer_write_byte(&hdlc_ring_buf,data);
}

static _internal_frame_receiver_t *current_frame_receiver = 0;

void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {
	// call receive_frame someway
	_MY_ASSERT(current_frame_receiver, return);
	current_frame_receiver->receive_frame((u8*)buffer, bytes_received);
}

template<typename TBase, typename T> 
static void send_message(TBase *serializer, T *message) {

	u32 bytes_to_send = message->ByteSize();
	_MY_ASSERT(message->SerializeToArray(hdlc_buf, bytes_to_send), return);

	serializer->framer.send_frame(hdlc_buf, bytes_to_send);


}

template void send_message<>(serializer_t *, proto::host_interface_t * );
template void send_message<>(host_serializer_t *, proto::exported_interface_t *);



void framer_t::sys_init() {

	static bool hdlc_initialized = false;

	if (!hdlc_initialized) {
		ring_buffer_init(&hdlc_ring_buf,(u8_t*)_hdlc_ring_buf, HDLC_RING_BUF_LEN);
		hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
		hdlc_initialized = true;
	}
}

void framer_t::send_frame(u8 *data, u32 bytes_to_send) {

	hdlc_ring_buf.tail = hdlc_ring_buf.head;
	hdlc_tx_frame((const u8_t*)data, bytes_to_send);

	bytes_to_send = ring_buffer_read_data(&hdlc_ring_buf,(u8_t*)hdlc_buf, HDLC_RING_BUF_LEN);
	sintf->send(hdlc_buf,bytes_to_send);
}


void framer_t::receive(u8 *data, u32 len) {
	u32 ofs = 0;
	current_frame_receiver = this->frame_receiver;
	while (len--) {
		hdlc_on_rx_byte(data[ofs++]);
	}
}







void serializer_t::send_packet(void *packet, u32 sizeof_packet) {

	_MY_ASSERT(sizeof_packet == sizeof(proto::host_interface_t), return);

	proto::host_interface_t *host_interface = (proto::host_interface_t *)packet;
	send_message(this, host_interface);
}




void serializer_t::receive_frame(u8 *data, u32 len) {

	proto::exported_interface_t ei;
	_WEAK_ASSERT(ei.ParseFromArray(data,len), return);

	//host_model_interface_t_serializer.receive_packet(&ei, sizeof(ei));
	_receive_frame(&ei, sizeof(ei));
	
}

// ---------------------------------------
//сериализер на стороне хоста
// ---------------------------------------




// ------------------------ public pkt_serdes_t::serdes_receiver_t ----------------------


void host_serializer_t::send_packet(void *packet, u32 sizeof_packet) {

	_MY_ASSERT(sizeof_packet == sizeof(proto::exported_interface_t), return);

	proto::exported_interface_t *exported_interface = (proto::exported_interface_t *)packet;
	send_message(this, exported_interface);
}

void host_serializer_t::receive_frame(u8 *data, u32 len) {

	proto::host_interface_t h;
	_WEAK_ASSERT(h.ParseFromArray(data,len), return);

	_receive_frame(&h, sizeof(h));

}
