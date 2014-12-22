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

using namespace ${proto.proto.options.@namespace};



#define HDLC_RING_BUF_LEN	512
// temporal buffer
u8 hdlc_buf[HDLC_RING_BUF_LEN];


template<typename TBase, typename T> 
static void send_message(TBase *serializer, T *message) {

	u32 bytes_to_send = message->ByteSize();
	_MY_ASSERT(message->SerializeToArray(hdlc_buf, bytes_to_send), return);

	serializer->framer.send_frame(hdlc_buf, bytes_to_send);


}

template void send_message<>(serializer_t *, proto::host_interface_t * );
template void send_message<>(host_serializer_t *, proto::exported_interface_t *);




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
