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


void serializer_t::init(msg::exported_interface_t *aexported,msg::exported_interface2_t *aexported2, serial_interface_t *asintf ) {
	exported = aexported;
	exported2 = aexported2;
	sintf = asintf;
	sintf->subscribe(this);

	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
}

void serializer_t::ChannelDMEChanged(u8 number, msg::tSuffix::tSuffix suffix) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_channelDMEChanged = true;
	h.channelDMEChanged.number = number;
	h.channelDMEChanged.sfx = (myvi_proto_tSuffix)suffix;
	send_message(h, sintf);
}

void serializer_t::RequestFrequency(u32 frequency) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_requestFrequency = true;
	h.requestFrequency = frequency;
	send_message(h, sintf);
}
void serializer_t::RequestFrequencyZapr(u32 frequency) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_requestFrequencyZapr = true;
	h.requestFrequencyZapr = frequency;
	send_message(h, sintf);
}
void serializer_t::RequestFrequencyOtv(u32 frequency) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_requestFrequencyOtv = true;
	h.requestFrequencyOtv = frequency;
	send_message(h, sintf);
}
void serializer_t::RequestSpanChanged(u32 requestSpan) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_requestSpanChanged = true;
	h.requestSpanChanged = requestSpan;
	send_message(h, sintf);
}
void serializer_t::ReplySpanChanged(u32 replySpan) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_replySpanChanged = true;
	h.replySpanChanged = replySpan;
	send_message(h, sintf);
}
void serializer_t::RFLevelChanged(s32 RFLevel) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_rfLevelChanged = true;
	h.rfLevelChanged = RFLevel;
	send_message(h, sintf);
}
void serializer_t::RequestFormChanged(msg::tRequestForm::tRequestForm requestForm) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_requestFormChanged = true;
	h.requestFormChanged = (myvi_proto_tRequestForm)requestForm;
	send_message(h, sintf);
}
void serializer_t::UserModeChanged(msg::UserMode::UserMode uMode) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_userModeChanged = true;
	h.userModeChanged = (myvi_proto_UserMode)uMode;
	send_message(h, sintf);
}
void serializer_t::OutputModeChanged(msg::OutputMode::OutputMode uMode) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_outputModeChanged = true;
	h.outputModeChanged = (myvi_proto_OutputMode)uMode;
	send_message(h, sintf);
}
void serializer_t::resolution_x_changed(u32 v) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_resolution_x_changed = true;
	h.resolution_x_changed = v;
	send_message(h, sintf);
}
void serializer_t::resolution_y_changed(u32 v) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_resolution_y_changed = true;
	h.resolution_y_changed = v;
	send_message(h, sintf);
}
void serializer_t::offset_x_changed(u32 v) {
	myvi_proto_host_interface_t h;
	memset(&h,0,sizeof(h));
	h.has_offset_x_changed = true;
	h.offset_x_changed = v;
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

void serializer_t::receive_frame(u8 *data, u32 len) {

	myvi_proto_exported_interface_t ei;
	memset(&ei,0, sizeof(ei));

    pb_istream_t istream = pb_istream_from_buffer((uint8_t*)data, len);
    _WEAK_ASSERT(pb_decode(&istream, myvi_proto_exported_interface_t_fields, &ei), return);

	if (ei.has_set_dme_channel) {
		if (exported)
		exported->set_dme_channel(ei.set_dme_channel.number, (msg::tSuffix::tSuffix)ei.set_dme_channel.sfx);
	}
	if (ei.has_set_dme_channel_range) {
		if (exported)
		exported->set_dme_channel_range(ei.set_dme_channel_range.lo,ei.set_dme_channel_range.hi);
	}
	if (ei.has_set_request_frequency) {
		if (exported)
		exported->set_request_frequency(ei.set_request_frequency);
	}
	if (ei.has_set_request_frequency_range) {
		if (exported)
		exported->set_request_frequency_range(ei.set_request_frequency_range.lo,ei.set_request_frequency_range.hi);
	}
	if (ei.has_set_response_frequency) {
		if (exported)
		exported->set_response_frequency(ei.set_response_frequency);
	}
	if (ei.has_set_request_span) {
		if (exported)
		exported->set_request_span(ei.set_request_span);
	}
	if (ei.has_set_request_span_range) {
		if (exported)
		exported->set_request_span_range(ei.set_request_span_range.lo,ei.set_request_span_range.hi);
	}
	if (ei.has_set_response_span) {
		if (exported)
		exported->set_response_span(ei.set_response_span);
	}
	if (ei.has_set_response_span_range) {
		if (exported)
		exported->set_response_span_range(ei.set_response_span_range.lo,ei.set_response_span_range.hi);
	}
	if (ei.has_set_request_frequency_zapr) {
		if (exported)
		exported->set_request_frequency_zapr(ei.set_request_frequency_zapr);
	}
	if (ei.has_set_request_frequency_zapr_range) {
		if (exported)
		exported->set_request_frequency_zapr_range(ei.set_request_frequency_zapr_range.lo,ei.set_request_frequency_zapr_range.hi);
	}
	if (ei.has_set_request_frequency_otv) {
		if (exported)
		exported->set_request_frequency_otv(ei.set_request_frequency_otv);
	}
	if (ei.has_set_request_frequency_otv_range) {
		if (exported)
		exported->set_request_frequency_otv_range(ei.set_request_frequency_otv_range.lo,ei.set_request_frequency_otv_range.hi);
	}
	if (ei.has_set_request_level) {
		if (exported)
		exported->set_request_level(ei.set_request_level);
	}
	if (ei.has_set_request_level_range) {
		if (exported)
		exported->set_request_level_range(ei.set_request_level_range.lo,ei.set_request_level_range.hi);
	}
	if (ei.has_set_response_level) {
		if (exported)
		exported->set_response_level(ei.set_response_level);
	}
	if (ei.has_set_efficiency) {
		if (exported)
		exported->set_efficiency(ei.set_efficiency);
	}
	if (ei.has_set_response_delay) {
		if (exported)
		exported->set_response_delay(ei.set_response_delay);
	}
	if (ei.has_set_range) {
		if (exported)
		exported->set_range(ei.set_range);
	}
	if (ei.has_set_battery_status) {
		if (exported)
		exported->set_battery_status(ei.set_battery_status);
	}
	if (ei.has_set_device_status) {
		if (exported)
		exported->set_device_status(ei.set_device_status);
	}
	if (ei.has_set_ksvn) {
		if (exported)
		exported->set_ksvn(ei.set_ksvn);
	}
	if (ei.has_set_co_code) {
		if (exported)
		exported->set_co_code(
			ei.set_co_code.code,
			ei.set_co_code.aeroport
			);
	}
	if (ei.has_set_statistics) {
		if (exported)
		exported->set_statistics(
			ei.set_statistics.ev_range,
			ei.set_statistics.ev_delay,
			ei.set_statistics.sigma,
			ei.set_statistics.sko
			);
	}
	if (ei.has_update_chart) {
		if (exported)
		exported->update_chart(
			ei.update_chart.maxy,
			(s32*)ei.update_chart.yvalues,
			ei.update_chart.yvalues_count,
			ei.update_chart.remain
			);
	}
	if (ei.has_set_user_mode) {
		if (exported)
		exported->set_user_mode((msg::UserMode::UserMode)ei.set_user_mode);
	}
	if (ei.has_set_request_form) {
		if (exported)
		exported->set_request_form((msg::tRequestForm::tRequestForm)ei.set_request_form);
	}
	if (ei.has_set_output_mode) {
		if (exported)
		exported->set_output_mode((msg::OutputMode::OutputMode)ei.set_output_mode);
	}
	if (ei.has_set_offset_x) {
		if (exported)
		exported->set_offset_x(ei.set_offset_x);
	}
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

void host_serializer_t::init(msg::host_interface_t *ahost,msg::host_interface2_t *ahost2, serial_interface_t *asintf ) {
	host = ahost;
	host2 = ahost2;
	sintf = asintf;
	sintf->subscribe(this);

	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
}


void host_serializer_t::set_dme_channel(u8 number, msg::tSuffix::tSuffix suffix) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_dme_channel = true;
	ei.set_dme_channel.number = number;
	ei.set_dme_channel.sfx = (myvi_proto_tSuffix)suffix;
	send_message(ei, sintf);
}
void host_serializer_t::set_dme_channel_range(u8 lo, u8 hi) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_dme_channel_range = true;
	ei.set_dme_channel_range.lo = lo;
	ei.set_dme_channel_range.hi = hi;
	send_message(ei, sintf);
}

void host_serializer_t::set_request_frequency(u32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_frequency = true;
	ei.set_request_frequency = value;
	send_message(ei, sintf);
}
void host_serializer_t::set_request_frequency_range(u32 lo, u32 hi) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_frequency_range = true;
	ei.set_request_frequency_range.lo = lo;
	ei.set_request_frequency_range.hi = hi;
	send_message(ei, sintf);
}
void host_serializer_t::set_response_frequency(u32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_response_frequency = true;
	ei.set_response_frequency = value;
	send_message(ei, sintf);
}

void host_serializer_t::set_request_span(u32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_span = true;
	ei.set_request_span = value;
	send_message(ei, sintf);
}
void host_serializer_t::set_request_span_range(u32 lo, u32 hi) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_span_range = true;
	ei.set_request_span_range.lo = lo;
	ei.set_request_span_range.hi = hi;
	send_message(ei, sintf);
}
void host_serializer_t::set_response_span(u32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_response_span = true;
	ei.set_response_span = value;
	send_message(ei, sintf);
}
void host_serializer_t::set_response_span_range(u32 lo, u32 hi) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_response_span_range = true;
	ei.set_response_span_range.lo = lo;
	ei.set_response_span_range.hi = hi;
	send_message(ei, sintf);
}

void host_serializer_t::set_request_frequency_zapr(u32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_frequency_zapr = true;
	ei.set_request_frequency_zapr = value;
	send_message(ei, sintf);
}
void host_serializer_t::set_request_frequency_zapr_range(u32 lo, u32 hi) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_frequency_zapr_range = true;
	ei.set_request_frequency_zapr_range.lo = lo;
	ei.set_request_frequency_zapr_range.hi = hi;
	send_message(ei, sintf);
}

void host_serializer_t::set_request_frequency_otv(u32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_frequency_otv = true;
	ei.set_request_frequency_otv = value;
	send_message(ei, sintf);
}
void host_serializer_t::set_request_frequency_otv_range(u32 lo, u32 hi) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_frequency_otv_range = true;
	ei.set_request_frequency_otv_range.lo = lo;
	ei.set_request_frequency_otv_range.hi = hi;
	send_message(ei, sintf);
}

void host_serializer_t::set_request_level(s32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_level = true;
	ei.set_request_level = value;
	send_message(ei, sintf);
}
void host_serializer_t::set_request_level_range(s32 lo, s32 hi) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_level_range = true;
	ei.set_request_level_range.lo = lo;
	ei.set_request_level_range.hi = hi;
	send_message(ei, sintf);
}
void host_serializer_t::set_response_level(s32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_response_level = true;
	ei.set_response_level = value;
	send_message(ei, sintf);
}

void host_serializer_t::set_efficiency(u8 percent) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_efficiency = true;
	ei.set_efficiency = percent;
	send_message(ei, sintf);
}
//	void set_code_reception_interval(u32 value);
void host_serializer_t::set_response_delay(u32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_response_delay = true;
	ei.set_response_delay = value;
	send_message(ei, sintf);
}
void host_serializer_t::set_range(u32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_range = true;
	ei.set_range = value;
	send_message(ei, sintf);
}
void host_serializer_t::set_battery_status(u8 charge) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_battery_status = true;
	ei.set_battery_status = charge;
	send_message(ei, sintf);
}
void host_serializer_t::set_device_status(u8 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_device_status = true;
	ei.set_device_status = value;
	send_message(ei, sintf);
}
void host_serializer_t::set_ksvn(u32 value) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_ksvn = true;
	ei.set_ksvn = value;
	send_message(ei, sintf);
}

void host_serializer_t::set_co_code(const char * code, const char * aeroport) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_co_code = true;
	memcpy(ei.set_co_code.code,code,sizeof(ei.set_co_code.code));
	memcpy(ei.set_co_code.aeroport,aeroport,sizeof(ei.set_co_code.aeroport));
	send_message(ei, sintf);
}
void host_serializer_t::set_statistics(u32 ev_range, u32 ev_delay, u32 sigma, u32 sko) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_statistics = true;
	ei.set_statistics.ev_range = ev_range;
	ei.set_statistics.ev_delay = ev_delay;
	ei.set_statistics.sigma = sigma;
	ei.set_statistics.sko = sko;
	send_message(ei, sintf);
}

void host_serializer_t::update_chart(s32 maxy, s32 *yvalues, s32 length, s32 remain) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_update_chart = true;
	ei.update_chart.maxy = maxy;
	_MY_ASSERT(length * sizeof(s32) <= sizeof(ei.update_chart.yvalues), return);
	memcpy(ei.update_chart.yvalues,yvalues,length * sizeof(s32));
	ei.update_chart.length = length;
	ei.update_chart.yvalues_count = length;
	ei.update_chart.remain = remain;
	send_message(ei, sintf);
}
// n
void host_serializer_t::set_user_mode(msg::UserMode::UserMode uMode) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_user_mode = true;
	ei.set_user_mode = (myvi_proto_UserMode)uMode;
	send_message(ei, sintf);
}
void host_serializer_t::set_request_form(msg::tRequestForm::tRequestForm requestForm) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_request_form = true;
	ei.set_request_form = (myvi_proto_tRequestForm)requestForm;
	send_message(ei, sintf);
}
void host_serializer_t::set_output_mode(msg::OutputMode::OutputMode uMode) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_output_mode = true;
	ei.set_output_mode = (myvi_proto_OutputMode)uMode;
	send_message(ei, sintf);
}

void host_serializer_t::set_offset_x(u32 v) {
	myvi_proto_exported_interface_t ei;
	memset(&ei,0,sizeof(ei));
	ei.has_set_offset_x = true;
	ei.set_offset_x = v;
	send_message(ei, sintf);
}
//	void set_time_sweep(u32 timeSweep);
//	void set_screen_saver(u8 screenSaver);
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

	if (h.has_channelDMEChanged) {
		if (host)
		host->ChannelDMEChanged(h.channelDMEChanged.number,(msg::tSuffix::tSuffix)h.channelDMEChanged.sfx);
	}
	if (h.has_requestFrequency) {
		if (host)
		host->RequestFrequency(h.requestFrequency);
	}
	if (h.has_requestFrequencyZapr) {
		if (host)
		host->RequestFrequencyZapr(h.requestFrequencyZapr);
	}
	if (h.has_requestFrequencyOtv) {
		if (host)
		host->RequestFrequencyOtv(h.requestFrequencyOtv);
	}
	if (h.has_requestSpanChanged) {
		if (host)
		host->RequestSpanChanged(h.requestSpanChanged);
	}
	if (h.has_replySpanChanged) {
		if (host)
		host->ReplySpanChanged(h.replySpanChanged);
	}
	if (h.has_rfLevelChanged) {
		if (host)
		host->RFLevelChanged(h.rfLevelChanged);
	}
	if (h.has_requestFormChanged) {
		if (host)
		host->RequestFormChanged((msg::tRequestForm::tRequestForm)h.requestFormChanged);
	}
	if (h.has_userModeChanged) {
		if (host)
		host->UserModeChanged((msg::UserMode::UserMode)h.userModeChanged);
	}
	if (h.has_outputModeChanged) {
		if (host)
		host->OutputModeChanged((msg::OutputMode::OutputMode)h.outputModeChanged);
	}
	if (h.has_resolution_x_changed) {
		if (host)
		host->resolution_x_changed(h.resolution_x_changed);
	}
	if (h.has_resolution_y_changed) {
		if (host)
		host->resolution_y_changed(h.resolution_y_changed);
	}
	if (h.has_offset_x_changed) {
		if (host)
		host->offset_x_changed(h.offset_x_changed);
	}
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
