#include "link.h"
#include "assert_impl.h"
#include <string.h>


using namespace myvi;

void serializer_t::ChannelDMEChanged(u8 number, msg::tSuffix::tSuffix suffix) {
}
void serializer_t::RequestFrequency(u32 frequency) {
}
void serializer_t::RequestFrequencyZapr(u32 frequency) {
}
void serializer_t::RequestFrequencyOtv(u32 frequency) {
}
void serializer_t::RequestSpanChanged(u32 requestSpan) {
}
void serializer_t::ReplySpanChanged(u32 replySpan) {
}
void serializer_t::RFLevelChanged(s32 RFLevel) {
}
void serializer_t::RequestFormChanged(msg::tRequestForm::tRequestForm requestForm) {
}
void serializer_t::UserModeChanged(msg::UserMode::UserMode uMode) {
}
void serializer_t::OutputModeChanged(msg::OutputMode::OutputMode uMode) {
}
void serializer_t::resolution_x_changed(u32 v) {
}
void serializer_t::resolution_y_changed(u32 v) {
}
void serializer_t::offset_x_changed(u32 v) {
}
void serializer_t::download_response(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) {
}
void serializer_t::file_info_response(u32 file_id, u32 cur_len, u32 max_len, u32 crc)  {
}
void serializer_t::error(u32 code) {
}

// ------------------- public msg::exported_interface_t --------------------

void serializer_t::receive(u8 *data, u32 len) {
	_MY_ASSERT(exported, return);
}

// ---------------------------------------
//сериализер на стороне хоста
// ---------------------------------------


void host_serializer_t::set_dme_channel(u8 number, msg::tSuffix::tSuffix suffix) {
}
void host_serializer_t::set_dme_channel_range(u8 lo, u8 hi) {
}

void host_serializer_t::set_request_frequency(u32 value) {
}
void host_serializer_t::set_request_frequency_range(u32 lo, u32 hi) {
}
void host_serializer_t::set_response_frequency(u32 value) {
}

void host_serializer_t::set_request_span(u32 value) {
}
void host_serializer_t::set_request_span_range(u32 lo, u32 hi) {
}
void host_serializer_t::set_response_span(u32 value) {
}
void host_serializer_t::set_response_span_range(u32 lo, u32 hi) {
}

void host_serializer_t::set_request_frequency_zapr(u32 value) {
}
void host_serializer_t::set_request_frequency_zapr_range(u32 lo, u32 hi) {
}

void host_serializer_t::set_request_frequency_otv(u32 value) {
}
void host_serializer_t::set_request_frequency_otv_range(u32 lo, u32 hi) {
}

void host_serializer_t::set_request_level(s32 value) {
}
void host_serializer_t::set_request_level_range(s32 lo, s32 hi) {
}
void host_serializer_t::set_response_level(s32 value) {
}

void host_serializer_t::set_efficiency(u8 percent) {
}
//	void set_code_reception_interval(u32 value);
void host_serializer_t::set_response_delay(u32 value) {
}
void host_serializer_t::set_range(u32 value) {
}
void host_serializer_t::set_battery_status(u8 charge) {
}
void host_serializer_t::set_device_status(u8 value) {
}
void host_serializer_t::set_ksvn(u32 value) {
}

void host_serializer_t::set_co_code(const char * code, const char * aeroport) {
}
void host_serializer_t::set_statistics(u32 ev_range, u32 ev_delay, u32 sigma, u32 sko) {
}

void host_serializer_t::update_chart(s32 maxy, s32 *yvalues, s32 length, s32 remain) {
}
// n
void host_serializer_t::set_user_mode(msg::UserMode::UserMode uMode) {
}
void host_serializer_t::set_request_form(msg::tRequestForm::tRequestForm requestForm) {
}
void host_serializer_t::set_output_mode(msg::OutputMode::OutputMode uMode) {
}

void host_serializer_t::set_offset_x(u32 v) {
}
//	void set_time_sweep(u32 timeSweep);
//	void set_screen_saver(u8 screenSaver);
void host_serializer_t::key_event(key_t::key_t key) {
}
void host_serializer_t::upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) {
}
void host_serializer_t::download_file(u32 file_id, u32 offset, u32 length) {
}
void host_serializer_t::update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) {
}
void host_serializer_t::read_file_info(u32 file_id) {
}


// ------------------------ public pkt_serdes_t::serdes_receiver_t ----------------------
void host_serializer_t::receive(u8 *data, u32 len) {
	_MY_ASSERT(host, return);
}
