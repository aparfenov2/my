/*
* Реализаци интерфейса связи системного уровня для прямой и обратной связи с этой системой
*/
#include "link.h"
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

using namespace myvi;


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

static void send_message(myvi_proto_host_interface_t &message, serial_interface_t *sintf) {

	pb_ostream_t ostream = pb_ostream_from_buffer(hdlc_buf, sizeof(hdlc_buf));
	_WEAK_ASSERT(pb_encode(&ostream, myvi_proto_host_interface_t_fields, &message), return);

	current_serial_interface = sintf;
	hdlc_tx_frame((const u8_t*)hdlc_buf, ostream.bytes_written);

}

static void send_message(myvi_proto_exported_interface_t &message, serial_interface_t *sintf) {

	pb_ostream_t ostream = pb_ostream_from_buffer(hdlc_buf, sizeof(hdlc_buf));
	_WEAK_ASSERT(pb_encode(&ostream, myvi_proto_exported_interface_t_fields, &message),return);

	current_serial_interface = sintf;
	hdlc_tx_frame((const u8_t*)hdlc_buf, ostream.bytes_written);

}


void serializer_t::init(msg::exported_interface2_t *aexported2, serial_interface_t *asintf ) {
	exported2 = aexported2;
	sintf = asintf;
	sintf->subscribe(this);

	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
}

void serializer_t::download_response(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_download_response = true;
	h.download_response.file_id = file_id;
	h.download_response.offset = offset;
	h.download_response.crc = crc;
	h.download_response.first = first;
	h.download_response.data.size = len;
	_MY_ASSERT(len < sizeof(h.download_response.data.bytes), return);
	memcpy(h.download_response.data.bytes,data,len);

	send_message(h, sintf);
}
void serializer_t::file_info_response(u32 file_id, u32 cur_len, u32 max_len, u32 crc)  {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_file_info_response = true;
	h.file_info_response.file_id = file_id;
	h.file_info_response.cur_len = cur_len;
	h.file_info_response.has_max_len = true;
	h.file_info_response.max_len = max_len;
	h.file_info_response.has_crc = true;
	h.file_info_response.crc = crc;
	send_message(h, sintf);
}
void serializer_t::error(u32 code) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_error = true;
	h.error = code;
	send_message(h, sintf);
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

	myvi_proto_exported_interface_t ei;
	memset(&ei,0, sizeof(ei));

    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)data, len);
    _WEAK_ASSERT(pb_decode(&istream, myvi_proto_exported_interface_t_fields, &ei), return);

	if (ei.has_key_event) {
		if (exported2)
		exported2->key_event((myvi::key_t::key_t)ei.key_event);
	}
	if (ei.has_upload_file) {
		if (exported2)
		exported2->upload_file(
			ei.upload_file.file_id,
			ei.upload_file.offset,
			ei.upload_file.crc,
			ei.upload_file.first,
			(u8*)ei.upload_file.data.bytes,
			ei.upload_file.data.size
			);
	}
	if (ei.has_download_file) {
		if (exported2)
		exported2->download_file(
			ei.download_file.file_id,
			ei.download_file.offset,
			ei.download_file.length
			);
	}
	if (ei.has_update_file_info) {
		if (exported2)
		exported2->update_file_info(
			ei.update_file_info.file_id,
			ei.update_file_info.cur_len,
			ei.update_file_info.max_len,
			ei.update_file_info.crc
			);
	}
	if (ei.has_read_file_info) {
		if (exported2)
		exported2->read_file_info(ei.read_file_info);
	}
}

// ---------------------------------------
//сериализер на стороне хоста
// ---------------------------------------

void host_serializer_t::init(msg::host_interface2_t *ahost2, serial_interface_t *asintf ) {
	host2 = ahost2;
	sintf = asintf;
	sintf->subscribe(this);

	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
}


void host_serializer_t::key_event(key_t::key_t key) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_key_event = true;
	ei.key_event = (myvi_proto_key_t)key;
	send_message(ei, sintf);
}
void host_serializer_t::upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_upload_file = true;
	ei.upload_file.file_id = file_id;
	ei.upload_file.offset = offset;
	ei.upload_file.crc = crc;
	ei.upload_file.first = first;
	_MY_ASSERT(len <= sizeof(ei.upload_file.data.bytes),return);
	memcpy(ei.upload_file.data.bytes,data,len);
	ei.upload_file.data.size = len;
	send_message(ei, sintf);
}
void host_serializer_t::download_file(u32 file_id, u32 offset, u32 length) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_download_file = true;
	ei.download_file.file_id = file_id;
	ei.download_file.offset = offset;
	ei.download_file.length = length;
	send_message(ei, sintf);
}
void host_serializer_t::update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_update_file_info = true;
	ei.update_file_info.file_id = file_id;
	ei.update_file_info.cur_len = cur_len;
	ei.update_file_info.max_len = max_len;
	ei.update_file_info.crc = crc;
	send_message(ei, sintf);
}
void host_serializer_t::read_file_info(u32 file_id) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_read_file_info = true;
	ei.read_file_info = file_id;
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

	myvi_proto_host_interface_t h;
	memset(&h,0, sizeof(h));

    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)data, len);
    _WEAK_ASSERT(pb_decode(&istream, myvi_proto_host_interface_t_fields, &h), return);

	if (h.has_download_response) {
		if (host2)
		host2->download_response(
			h.download_response.file_id,
			h.download_response.offset,

			h.download_response.crc,
			h.download_response.first,

			(u8*)h.download_response.data.bytes,
			h.download_response.data.size
			);
	}
	if (h.has_file_info_response) {
		if (host2)
		host2->file_info_response(
			h.file_info_response.file_id,
			h.file_info_response.cur_len,
			h.file_info_response.max_len,
			h.file_info_response.crc
			);
	}
	if (h.has_error) {
		if (host2)
		host2->error(h.error);
	}
}
