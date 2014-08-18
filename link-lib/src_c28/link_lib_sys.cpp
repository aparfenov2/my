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


void serializer_t::init(serial_interface_t *asintf ) {
	sintf = asintf;
	sintf->subscribe(this);

	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
}



// ответ на запрос на чтение данных из модели
void serializer_t::read_model_data_response(char * path, char * string_value, u32 code) {

	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_read_model_data_response = true;
	memcpy(h.read_model_data_response.path, path, sizeof(h.read_model_data_response.path));
	memcpy(h.read_model_data_response.string_value, string_value, sizeof(h.read_model_data_response.string_value));
	h.read_model_data_response.code = code;
	send_message(h, sintf);
}

void serializer_t::read_model_data_response(char * path, s32 int_value, u32 code ) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_read_model_data_response = true;
	memcpy(h.read_model_data_response.path, path, sizeof(h.read_model_data_response.path));
	h.read_model_data_response.int_value = int_value;
	h.read_model_data_response.code = code;
	send_message(h, sintf);
}

void serializer_t::read_model_data_response(char * path, double float_value, u32 code ) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_read_model_data_response = true;
	memcpy(h.read_model_data_response.path, path, sizeof(h.read_model_data_response.path));
	h.read_model_data_response.float_value = float_value;
	h.read_model_data_response.code = code;
	send_message(h, sintf);
}


// ответ на запись данных в модель
void serializer_t::write_model_data_ack(u32 code) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_write_model_data_ack = true;
	h.write_model_data_ack.code = code;
	send_message(h, sintf);
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

template<typename T>
static T * find_implementation(serializer_t *ser) {
	for (std::vector<exported_system_interface_t *>::iterator it = ser->exported_intrfaces.begin();
			it != ser->exported_intrfaces.end(); it++) {
		T *casted = dynamic_cast<T*>(*it);
		if (casted) {
			return casted;
		}
	}
	return 0;
}


void serializer_t::receive_frame(u8 *data, u32 len) {

	myvi_proto_exported_interface_t ei;
	memset(&ei,0, sizeof(ei));

    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)data, len);
    _WEAK_ASSERT(pb_decode(&istream, myvi_proto_exported_interface_t_fields, &ei), return);

    exported_model_interface_t *exported_model = find_implementation<exported_model_interface_t>(this);
	if (exported_model) {
		if (ei.has_read_model_data_request) {
			exported_model->read_model_data((char *)ei.read_model_data_request.path);
		}
		if (ei.has_write_model_data_request) {
			if (ei.write_model_data_request.has_string_value) {
				exported_model->write_model_data(
					(char *)ei.write_model_data_request.path,
					(char *)ei.write_model_data_request.string_value
					);
			}
			if (ei.write_model_data_request.has_int_value) {
				exported_model->write_model_data(
					(char *)ei.write_model_data_request.path,
					(s32)ei.write_model_data_request.int_value
					);
			}
			if (ei.write_model_data_request.has_float_value) {
				exported_model->write_model_data(
					(char *)ei.write_model_data_request.path,
					ei.write_model_data_request.float_value
					);
			}
		}
	}

	exported_emulation_interface_t *exported_emulation	 = find_implementation<exported_emulation_interface_t>(this);
	if (exported_emulation) {
		if (ei.has_key_event) {
				exported_emulation->key_event((myvi::key_t::key_t)ei.key_event);
		}
	}

	exported_file_interface_t *exported_file = find_implementation<exported_file_interface_t>(this);
	if (exported_file) {

		if (ei.has_upload_file) {
			exported_file->upload_file(
				ei.upload_file.file_id,
				ei.upload_file.offset,
				ei.upload_file.crc,
				ei.upload_file.first,
				(u8*)ei.upload_file.data.bytes,
				ei.upload_file.data.size
				);
		}
		if (ei.has_download_file) {
			exported_file->download_file(
				ei.download_file.file_id,
				ei.download_file.offset,
				ei.download_file.length
				);
		}
		if (ei.has_update_file_info) {
			exported_file->update_file_info(
				ei.update_file_info.file_id,
				ei.update_file_info.cur_len,
				ei.update_file_info.max_len,
				ei.update_file_info.crc
				);
		}
		if (ei.has_read_file_info) {
			exported_file->read_file_info(ei.read_file_info);
		}
	}
}

// ---------------------------------------
//сериализер на стороне хоста
// ---------------------------------------

void host_serializer_t::init(serial_interface_t *asintf ) {
	sintf = asintf;
	sintf->subscribe(this);

	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
}


void host_serializer_t::read_model_data(char * path) {

	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_read_model_data_request = true;
	memcpy(ei.read_model_data_request.path,path, sizeof(ei.read_model_data_request.path));
	send_message(ei, sintf);

}

// с на запись данных в модель
void host_serializer_t::write_model_data(char * path, char * string_value) {

	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_write_model_data_request = true;
	ei.write_model_data_request.has_string_value = true;
	memcpy(ei.write_model_data_request.path,path, sizeof(ei.write_model_data_request.path));
	memcpy(ei.write_model_data_request.string_value,string_value, sizeof(ei.write_model_data_request.string_value));
	send_message(ei, sintf);
}

void host_serializer_t::write_model_data(char * path, s32 int_value) {

	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_write_model_data_request = true;
	ei.write_model_data_request.has_int_value = true;
	memcpy(ei.write_model_data_request.path,path, sizeof(ei.write_model_data_request.path));
	ei.write_model_data_request.int_value = int_value;
	send_message(ei, sintf);
}

void host_serializer_t::write_model_data(char * path,  double float_value) {

	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_write_model_data_request = true;
	ei.write_model_data_request.has_float_value = true;
	memcpy(ei.write_model_data_request.path,path, sizeof(ei.write_model_data_request.path));
	ei.write_model_data_request.float_value = float_value;
	send_message(ei, sintf);

}


void host_serializer_t::key_event(myvi::key_t::key_t key) {
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


template<typename T>
static T * find_implementation(host_serializer_t *ser) {
	for (std::vector<host_system_interface_t *>::iterator it = ser->host_interfaces.begin();
			it != ser->host_interfaces.end(); it++) {
		T *casted = dynamic_cast<T*>(*it);
		if (casted) {
			return casted;
		}
	}
	return null;
}



void host_serializer_t::receive_frame(u8 *data, u32 len) {

	myvi_proto_host_interface_t h;
	memset(&h,0, sizeof(h));

    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)data, len);
    _WEAK_ASSERT(pb_decode(&istream, myvi_proto_host_interface_t_fields, &h), return);

    host_model_interface_t *host_model = find_implementation<host_model_interface_t>(this);

    if (host_model) {
		if (h.has_read_model_data_response) {
			if (h.read_model_data_response.has_string_value) {
				host_model->read_model_data_response(
						(char *)h.read_model_data_response.path,
						(char *)h.read_model_data_response.string_value,
						h.read_model_data_response.code
					);
			}
			if (h.read_model_data_response.has_int_value) {
				host_model->read_model_data_response(
						(char *)h.read_model_data_response.path,
						(s32)h.read_model_data_response.int_value,
						h.read_model_data_response.code
					);
			}
			if (h.read_model_data_response.has_float_value) {
				host_model->read_model_data_response(
						(char *)h.read_model_data_response.path,
						h.read_model_data_response.float_value,
						h.read_model_data_response.code
					);
			}
		}
    }

    host_file_interface_t *host_file = find_implementation<host_file_interface_t>(this);

    if (host_file) {
		if (h.has_download_response) {
			host_file->download_response(
				h.download_response.file_id,
				h.download_response.offset,

				h.download_response.crc,
				h.download_response.first,

				(u8*)h.download_response.data.bytes,
				h.download_response.data.size
				);
		}
		if (h.has_file_info_response) {
			host_file->file_info_response(
				h.file_info_response.file_id,
				h.file_info_response.cur_len,
				h.file_info_response.max_len,
				h.file_info_response.crc
				);
		}
		if (h.has_error) {
			host_file->error(h.error);
		}
    }
}
