/*
* Реализаци интерфейса связи системного уровня для прямой и обратной связи с этой системой
* ЭТО АВТОКОПИЯ !!!
*/
#include "link_sys_impl.h"
#include "assert_impl.h"
#include <string.h>
#include "myvi.pb.h"
extern "C" {
#include "hdlc.h"
#include "ring_buffer.h"
}
#include <pb_encode.h>
#include <pb_decode.h>

extern "C" u16_t crc16_ccitt_calc_data(u16_t crc, u8_t* data, u16_t data_length);

using namespace link;


static serial_interface_t *current_serial_interface = 0;
extern "C" void rs485_put_char(char data) {
	_MY_ASSERT(current_serial_interface,return);
	current_serial_interface->send((u8*)&data,1);
}

static _internal_frame_receiver_t *current_frame_receiver = 0;

extern "C" void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {
	_MY_ASSERT(current_frame_receiver,return);
	current_frame_receiver->receive_frame((u8*)buffer, bytes_received);
}


static u8 hdlc_buf[512];


static void send_message(serializer_t *ser, link_proto_host_interface_t *message) {

	pb_ostream_t ostream = pb_ostream_from_buffer(hdlc_buf, sizeof(hdlc_buf));
	_WEAK_ASSERT(pb_encode(&ostream, link_proto_host_interface_t_fields, message), return);

	ser->framer.send_frame(hdlc_buf, ostream.bytes_written);

}

static void send_message(host_serializer_t *ser, link_proto_exported_interface_t *message) {

	pb_ostream_t ostream = pb_ostream_from_buffer(hdlc_buf, sizeof(hdlc_buf));
	_WEAK_ASSERT(pb_encode(&ostream, link_proto_exported_interface_t_fields, message),return);

	ser->framer.send_frame(hdlc_buf, ostream.bytes_written);

}


void framer_t::sys_init() {

	static bool hdlc_initialized = false;

	if (!hdlc_initialized) {
		hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
		hdlc_initialized = true;
	}
}

void framer_t::send_frame(u8 *data, u32 bytes_to_send) {

	current_serial_interface = sintf;
	hdlc_tx_frame((const u8_t*)data, bytes_to_send);
}


void framer_t::receive(u8 *data, u32 len) {
	u32 ofs = 0;
	current_frame_receiver = this->frame_receiver;
	while (len--) {
		hdlc_on_rx_byte(data[ofs++]);
	}
}



void serializer_t::send_packet(void *packet, u32 sizeof_packet) {

	_MY_ASSERT(sizeof_packet == sizeof(link_proto_host_interface_t), return);

	link_proto_host_interface_t *host_interface = (link_proto_host_interface_t *)packet;
	send_message(this, host_interface);
}




void serializer_t::receive_frame(u8 *data, u32 len) {

	link_proto_exported_interface_t ei;
	memset(&ei,0, sizeof(ei));
    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)data, len);
    _WEAK_ASSERT(pb_decode(&istream, link_proto_exported_interface_t_fields, &ei), return);

	_receive_frame(&ei, sizeof(ei));

}

// ---------------------------------------
//сериализер на стороне хоста
// ---------------------------------------




// ------------------------ public pkt_serdes_t::serdes_receiver_t ----------------------


void host_serializer_t::send_packet(void *packet, u32 sizeof_packet) {

	_MY_ASSERT(sizeof_packet == sizeof(link_proto_exported_interface_t), return);

	link_proto_exported_interface_t *exported_interface = (link_proto_exported_interface_t *)packet;
	send_message(this, exported_interface);
}

void host_serializer_t::receive_frame(u8 *data, u32 len) {

	link_proto_host_interface_t h;
	memset(&h,0, sizeof(h));

    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)data, len);
    _WEAK_ASSERT(pb_decode(&istream, link_proto_host_interface_t_fields, &h), return);

	_receive_frame(&h, sizeof(h));

}
