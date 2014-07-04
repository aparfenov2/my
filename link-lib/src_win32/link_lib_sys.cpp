/*
* Реализаци интерфейса связи системного уровня для прямой и обратной связи с этой системой
*/

#include "link_sys.h"
#include "assert_impl.h"
#include <string.h>
#include "myvi.pb.h"
extern "C" {
#include "hdlc.h"
#include "ring_buffer.h"
}
extern "C" u16_t crc16_ccitt_calc_data(u16_t crc, u8_t* data, u16_t data_length);

using namespace myvi;


#define HDLC_RING_BUF_LEN 512
u8 _hdlc_ring_buf[HDLC_RING_BUF_LEN];
ring_buffer_t hdlc_ring_buf;

void rs485_put_char(char data) {
	ring_buffer_write_byte(&hdlc_ring_buf,data);
}

static _internal_frame_receiver_t *current_frame_receiver = 0;

void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {
	// call receive_frame someway
	_MY_ASSERT(current_frame_receiver,);
	current_frame_receiver->receive_frame((u8*)buffer, bytes_received);
}

template<typename T> 
void send_message(T &message, serial_interface_t *sintf) {

	u32 bytes_to_send = message.ByteSize();
	u8 * hdlc_buf = new u8[bytes_to_send];

	_MY_ASSERT(message.SerializeToArray(hdlc_buf, bytes_to_send),);


	hdlc_ring_buf.tail = hdlc_ring_buf.head;
	hdlc_tx_frame((const u8_t*)hdlc_buf, bytes_to_send);

	bytes_to_send = ring_buffer_read_data(&hdlc_ring_buf,(u8_t*)hdlc_buf,bytes_to_send);
	sintf->send(hdlc_buf,bytes_to_send);
	delete hdlc_buf;

}

template void send_message<>(proto::host_interface_t &, serial_interface_t * );
template void send_message<>(proto::exported_interface_t &, serial_interface_t * );


void serializer_t::init(exported_interface2_t *aexported2, serial_interface_t *asintf ) {
	exported2 = aexported2;
	sintf = asintf;
	sintf->subscribe(this);

	ring_buffer_init(&hdlc_ring_buf,(u8_t*)_hdlc_ring_buf, HDLC_RING_BUF_LEN);
	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
}

void serializer_t::download_response(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) {
	proto::host_interface_t host_interface;
	proto::file_data_t *fd  = host_interface.mutable_download_response();
	fd->set_file_id(file_id);
	fd->set_offset(offset);
	fd->set_crc(crc);
	fd->set_first(first);
	fd->set_data(data,len);
	send_message(host_interface, sintf);
}
void serializer_t::file_info_response(u32 file_id, u32 cur_len, u32 max_len, u32 crc)  {
	proto::host_interface_t host_interface;
	proto::file_info_t *fi = host_interface.mutable_file_info_response();
	fi->set_file_id(file_id);
	fi->set_cur_len(cur_len);
	fi->set_max_len(max_len);
	fi->set_crc(crc);
	send_message(host_interface, sintf);
}
void serializer_t::error(u32 code) {
	proto::host_interface_t host_interface;
	host_interface.set_error(code);
	send_message(host_interface, sintf);
}

// ------------------- public msg::exported_interface_t --------------------

void serializer_t::receive(u8 *data, u32 len) {
	u32 ofs = 0;
	current_frame_receiver = this;
	while (len--) {
		hdlc_on_rx_byte(data[ofs++]);
	}
}

void serializer_t::receive_frame(u8 *data, u32 len) {
	_MY_ASSERT(exported2, return);

	myvi::proto::exported_interface_t ei;
	_WEAK_ASSERT(ei.ParseFromArray(data,len), return);

	if (ei.has_key_event()) {
		exported2->key_event((myvi::key_t::key_t)ei.key_event());
	}
	if (ei.has_upload_file()) {
		exported2->upload_file(
			ei.upload_file().file_id(),
			ei.upload_file().offset(),
			ei.upload_file().crc(),
			ei.upload_file().first(),
			(u8*)ei.upload_file().data().data(),
			ei.upload_file().data().size()
			);
	}
	if (ei.has_download_file()) {
		exported2->download_file(
			ei.download_file().file_id(),
			ei.download_file().offset(),
			ei.download_file().length()
			);
	}
	if (ei.has_update_file_info()) {
		exported2->update_file_info(
			ei.update_file_info().file_id(),
			ei.update_file_info().cur_len(),
			ei.update_file_info().max_len(),
			ei.update_file_info().crc()
			);
	}
	if (ei.has_read_file_info()) {
		exported2->read_file_info(ei.read_file_info());
	}
}

// ---------------------------------------
//сериализер на стороне хоста
// ---------------------------------------

void host_serializer_t::init(host_interface2_t *ahost2, serial_interface_t *asintf ) {
	host2 = ahost2;
	sintf = asintf;
	sintf->subscribe(this);

	ring_buffer_init(&hdlc_ring_buf,(u8_t*)_hdlc_ring_buf, HDLC_RING_BUF_LEN);
	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
}


void host_serializer_t::key_event(key_t::key_t key) {
	proto::exported_interface_t ei;
	ei.set_key_event((proto::key_t)key);
	send_message(ei, sintf);
}
void host_serializer_t::upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) {
	proto::exported_interface_t ei;
	proto::file_data_t *fd = ei.mutable_upload_file();
	fd->set_file_id(file_id);
	fd->set_offset(offset);
	fd->set_crc(crc);
	fd->set_first(first);
	fd->set_data(data,len);
	send_message(ei, sintf);
}
void host_serializer_t::download_file(u32 file_id, u32 offset, u32 length) {
	proto::exported_interface_t ei;
	proto::download_request_t *dr = ei.mutable_download_file();
	dr->set_file_id(file_id);
	dr->set_offset(offset);
	dr->set_length(length);
	send_message(ei, sintf);
}
void host_serializer_t::update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) {
	proto::exported_interface_t ei;
	proto::file_info_t *fi = ei.mutable_update_file_info();
	fi->set_file_id(file_id);
	fi->set_cur_len(cur_len);
	fi->set_max_len(max_len);
	fi->set_crc(crc);
	send_message(ei, sintf);
}
void host_serializer_t::read_file_info(u32 file_id) {
	proto::exported_interface_t ei;
	ei.set_read_file_info(file_id);
	send_message(ei, sintf);
}


// ------------------------ public pkt_serdes_t::serdes_receiver_t ----------------------
void host_serializer_t::receive(u8 *data, u32 len) {
	u32 ofs = 0;
	current_frame_receiver = this;
	while (len--) {
		hdlc_on_rx_byte(data[ofs++]);
	}
}

void host_serializer_t::receive_frame(u8 *data, u32 len) {
	_MY_ASSERT(host2, return);

	myvi::proto::host_interface_t h;
	_WEAK_ASSERT(h.ParseFromArray(data,len), return);

	if (h.has_download_response()) {
		host2->download_response(
			h.download_response().file_id(),
			h.download_response().offset(),

			h.download_response().crc(),
			h.download_response().first(),

			(u8*)h.download_response().data().data(),
			h.download_response().data().size()
			);
	}
	if (h.has_file_info_response()) {
		host2->file_info_response(
			h.file_info_response().file_id(),
			h.file_info_response().cur_len(),
			h.file_info_response().max_len(),
			h.file_info_response().crc()
			);
	}
	if (h.has_error()) {
		host2->error(h.error());
	}
}
