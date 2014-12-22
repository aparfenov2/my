/*
* Реализаци интерфейса связи системного уровня для прямой и обратной связи с этой системой
* ЭТО АВТОКОПИЯ !!!
*/

#include "link_sys_impl_${proto.proto.options.@namespace}.h"
#include "assert_impl.h"
#include <string.h>
#include "myvi.pb.h"
extern "C" {
#include "hdlc.h"
#include "ring_buffer.h"
}
#include <pb_encode.h>
#include <pb_decode.h>

using namespace ${proto.proto.options.@namespace};



static u8 hdlc_buf[512];


static void send_message(serializer_t *ser, ${proto.proto.options.@namespace}_proto_host_interface_t *message) {

	pb_ostream_t ostream = pb_ostream_from_buffer(hdlc_buf, sizeof(hdlc_buf));
	_WEAK_ASSERT(pb_encode(&ostream, ${proto.proto.options.@namespace}_proto_host_interface_t_fields, message), return);

	ser->framer.send_frame(hdlc_buf, ostream.bytes_written);

}

static void send_message(host_serializer_t *ser, ${proto.proto.options.@namespace}_proto_exported_interface_t *message) {

	pb_ostream_t ostream = pb_ostream_from_buffer(hdlc_buf, sizeof(hdlc_buf));
	_WEAK_ASSERT(pb_encode(&ostream, ${proto.proto.options.@namespace}_proto_exported_interface_t_fields, message),return);

	ser->framer.send_frame(hdlc_buf, ostream.bytes_written);

}



void serializer_t::send_packet(void *packet, u32 sizeof_packet) {

	_MY_ASSERT(sizeof_packet == sizeof(${proto.proto.options.@namespace}_proto_host_interface_t), return);

	${proto.proto.options.@namespace}_proto_host_interface_t *host_interface = (${proto.proto.options.@namespace}_proto_host_interface_t *)packet;
	send_message(this, host_interface);
}




void serializer_t::receive_frame(u8 *data, u32 len) {

	${proto.proto.options.@namespace}_proto_exported_interface_t ei;
	memset(&ei,0, sizeof(ei));
    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)data, len);
    _WEAK_ASSERT(pb_decode(&istream, ${proto.proto.options.@namespace}_proto_exported_interface_t_fields, &ei), return);

	_receive_frame(&ei, sizeof(ei));

}

// ---------------------------------------
//сериализер на стороне хоста
// ---------------------------------------




// ------------------------ public pkt_serdes_t::serdes_receiver_t ----------------------


void host_serializer_t::send_packet(void *packet, u32 sizeof_packet) {

	_MY_ASSERT(sizeof_packet == sizeof(${proto.proto.options.@namespace}_proto_exported_interface_t), return);

	${proto.proto.options.@namespace}_proto_exported_interface_t *exported_interface = (${proto.proto.options.@namespace}_proto_exported_interface_t *)packet;
	send_message(this, exported_interface);
}

void host_serializer_t::receive_frame(u8 *data, u32 len) {

	${proto.proto.options.@namespace}_proto_host_interface_t h;
	memset(&h,0, sizeof(h));

    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)data, len);
    _WEAK_ASSERT(pb_decode(&istream, ${proto.proto.options.@namespace}_proto_host_interface_t_fields, &h), return);

	_receive_frame(&h, sizeof(h));

}
