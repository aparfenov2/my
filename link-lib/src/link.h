/*
 * serializer.h
 *
 *  Created on: 12.03.2013
 *      Author: gordeev
 */

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include "exported.h"

namespace myvi {

// интерфейс канала передачи данных
class serial_data_receiver_t {
public:
	virtual void receive(u8 *data, u32 len) = 0;
};

class serial_interface_t {
public:
	virtual void send(u8 *data, u32 len) = 0;
	virtual void subscribe(serial_data_receiver_t *receiver) = 0;
};

class _internal_frame_receiver_t {
public:
	virtual void receive_frame(u8 *data, u32 len) = 0;
};

// 
class serializer_t : public msg::host_interface_t, public serial_data_receiver_t, public _internal_frame_receiver_t {

private:
	msg::exported_interface_t *exported;
	serial_interface_t *sintf;
public:
	serializer_t() {
		exported = 0;
		sintf = 0;
	}

	void init(msg::exported_interface_t *aexported, serial_interface_t *asintf );

// ----------------------- public msg::host_interface_t (туда)------------------------------------

	void ChannelDMEChanged(u8 number, msg::tSuffix::tSuffix suffix) OVERRIDE;
	void RequestFrequency(u32 frequency) OVERRIDE;
	void RequestFrequencyZapr(u32 frequency) OVERRIDE;
	void RequestFrequencyOtv(u32 frequency) OVERRIDE;
	void RequestSpanChanged(u32 requestSpan) OVERRIDE;
	void ReplySpanChanged(u32 replySpan) OVERRIDE;
	void RFLevelChanged(s32 RFLevel) OVERRIDE;
	void RequestFormChanged(msg::tRequestForm::tRequestForm requestForm) OVERRIDE;
	void UserModeChanged(msg::UserMode::UserMode uMode) OVERRIDE;
	void OutputModeChanged(msg::OutputMode::OutputMode uMode) OVERRIDE;
	void resolution_x_changed(u32 v) OVERRIDE;
	void resolution_y_changed(u32 v) OVERRIDE;
	void offset_x_changed(u32 v) OVERRIDE;

	void download_response(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) OVERRIDE;
	void file_info_response(u32 file_id, u32 cur_len, u32 max_len, u32 crc)  OVERRIDE;
	void error(u32 code) OVERRIDE;

// ------------------- public msg::serial_data_receiver_t --------------------
	// прием блока данных из интерфеса UART
	void receive(u8 *data, u32 len) OVERRIDE;
	// прием фрейма данных
	void receive_frame(u8 *data, u32 len) OVERRIDE;

}; // class


//сериализер на стороне хоста
class host_serializer_t : public msg::exported_interface_t, public serial_data_receiver_t, public _internal_frame_receiver_t {

private:
	msg::host_interface_t *host;
	serial_interface_t *sintf;
public:
	host_serializer_t() {
		host = 0;
		sintf = 0;
	}

	void init(msg::host_interface_t *ahost, serial_interface_t *asintf );
// ------------------------ public msg::exported_interface_t ----------------------------

	void set_dme_channel(u8 number, msg::tSuffix::tSuffix suffix) OVERRIDE;
	void set_dme_channel_range(u8 lo, u8 hi) OVERRIDE;

	void set_request_frequency(u32 value) OVERRIDE;
	void set_request_frequency_range(u32 lo, u32 hi) OVERRIDE;
	void set_response_frequency(u32 value) OVERRIDE;

	void set_request_span(u32 value) OVERRIDE;
	void set_request_span_range(u32 lo, u32 hi) OVERRIDE;
	void set_response_span(u32 value) OVERRIDE;
	void set_response_span_range(u32 lo, u32 hi) OVERRIDE;

	void set_request_frequency_zapr(u32 value) OVERRIDE;
	void set_request_frequency_zapr_range(u32 lo, u32 hi) OVERRIDE;

	void set_request_frequency_otv(u32 value) OVERRIDE;
	void set_request_frequency_otv_range(u32 lo, u32 hi) OVERRIDE;

	void set_request_level(s32 value) OVERRIDE;
	void set_request_level_range(s32 lo, s32 hi) OVERRIDE;
	void set_response_level(s32 value) OVERRIDE;

	void set_efficiency(u8 percent) OVERRIDE;
//	void set_code_reception_interval(u32 value);
	void set_response_delay(u32 value) OVERRIDE;
	void set_range(u32 value) OVERRIDE;
	void set_battery_status(u8 charge) OVERRIDE;
	void set_device_status(u8 value) OVERRIDE;
	void set_ksvn(u32 value) OVERRIDE;

	void set_co_code(const char * code, const char * aeroport) OVERRIDE;
	void set_statistics(u32 ev_range, u32 ev_delay, u32 sigma, u32 sko) OVERRIDE;

	void update_chart(s32 maxy, s32 *yvalues, s32 length, s32 remain) OVERRIDE;
// n
	void set_user_mode(msg::UserMode::UserMode uMode) OVERRIDE;
	void set_request_form(msg::tRequestForm::tRequestForm requestForm) OVERRIDE;
	void set_output_mode(msg::OutputMode::OutputMode uMode) OVERRIDE;

	void set_offset_x(u32 v) OVERRIDE;
//	void set_time_sweep(u32 timeSweep);
//	void set_screen_saver(u8 screenSaver);
	void key_event(key_t::key_t key) OVERRIDE;

	void upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) OVERRIDE;
	void download_file(u32 file_id, u32 offset, u32 length) OVERRIDE;
	void update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) OVERRIDE;
	void read_file_info(u32 file_id) OVERRIDE;


// ------------------------ public serial_data_receiver_t ----------------------
	// прием блока данных из интерфеса UART
	void receive(u8 *data, u32 len) OVERRIDE;
	// прием фрейма данных
	void receive_frame(u8 *data, u32 len) OVERRIDE;
}; // host_serializer_t

} // ns myvi
#endif /* SERIALIZER_H_ */
