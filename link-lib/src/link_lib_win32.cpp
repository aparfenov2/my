#include "link.h"
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


void serializer_t::init(msg::exported_interface_t *aexported, serial_interface_t *asintf ) {
	exported = aexported;
	sintf = asintf;
	sintf->subscribe(this);

	ring_buffer_init(&hdlc_ring_buf,(u8_t*)_hdlc_ring_buf, HDLC_RING_BUF_LEN);
	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
}

void serializer_t::ChannelDMEChanged(u8 number, msg::tSuffix::tSuffix suffix) {
	proto::host_interface_t host_interface;
	proto::ChannelDMEChanged *dmech = host_interface.mutable_channeldmechanged();
	dmech->set_number(number);
	dmech->set_sfx((proto::tSuffix)suffix);
	send_message(host_interface, sintf);
}

void serializer_t::RequestFrequency(u32 frequency) {
	proto::host_interface_t host_interface;
	host_interface.set_requestfrequency(frequency);
	send_message(host_interface, sintf);
}
void serializer_t::RequestFrequencyZapr(u32 frequency) {
	proto::host_interface_t host_interface;
	host_interface.set_requestfrequencyzapr(frequency);
	send_message(host_interface, sintf);
}
void serializer_t::RequestFrequencyOtv(u32 frequency) {
	proto::host_interface_t host_interface;
	host_interface.set_requestfrequencyotv(frequency);
	send_message(host_interface, sintf);
}
void serializer_t::RequestSpanChanged(u32 requestSpan) {
	proto::host_interface_t host_interface;
	host_interface.set_requestspanchanged(requestSpan);
	send_message(host_interface, sintf);
}
void serializer_t::ReplySpanChanged(u32 replySpan) {
	proto::host_interface_t host_interface;
	host_interface.set_replyspanchanged(replySpan);
	send_message(host_interface, sintf);
}
void serializer_t::RFLevelChanged(s32 RFLevel) {
	proto::host_interface_t host_interface;
	host_interface.set_rflevelchanged(RFLevel);
	send_message(host_interface, sintf);
}
void serializer_t::RequestFormChanged(msg::tRequestForm::tRequestForm requestForm) {
	proto::host_interface_t host_interface;
	host_interface.set_requestformchanged((proto::tRequestForm)requestForm);
	send_message(host_interface, sintf);
}
void serializer_t::UserModeChanged(msg::UserMode::UserMode uMode) {
	proto::host_interface_t host_interface;
	host_interface.set_usermodechanged((proto::UserMode)uMode);
	send_message(host_interface, sintf);
}
void serializer_t::OutputModeChanged(msg::OutputMode::OutputMode uMode) {
	proto::host_interface_t host_interface;
	host_interface.set_outputmodechanged((proto::OutputMode)uMode);
	send_message(host_interface, sintf);
}
void serializer_t::resolution_x_changed(u32 v) {
	proto::host_interface_t host_interface;
	host_interface.set_resolution_x_changed(v);
	send_message(host_interface, sintf);
}
void serializer_t::resolution_y_changed(u32 v) {
	proto::host_interface_t host_interface;
	host_interface.set_resolution_y_changed(v);
	send_message(host_interface, sintf);
}
void serializer_t::offset_x_changed(u32 v) {
	proto::host_interface_t host_interface;
	host_interface.set_offset_x_changed(v);
	send_message(host_interface, sintf);
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
	_MY_ASSERT(exported, return);

	myvi::proto::exported_interface_t ei;
	_WEAK_ASSERT(ei.ParseFromArray(data,len), return);

	if (ei.has_set_dme_channel()) {
		exported->set_dme_channel(ei.set_dme_channel().number(), (msg::tSuffix::tSuffix)ei.set_dme_channel().sfx());
	}
	if (ei.has_set_dme_channel_range()) {
		exported->set_dme_channel_range(ei.set_dme_channel_range().lo(),ei.set_dme_channel_range().hi());
	}
	if (ei.has_set_request_frequency()) {
		exported->set_request_frequency(ei.set_request_frequency());
	}
	if (ei.has_set_request_frequency_range()) {
		exported->set_request_frequency_range(ei.set_request_frequency_range().lo(),ei.set_request_frequency_range().hi());
	}
	if (ei.has_set_response_frequency()) {
		exported->set_response_frequency(ei.set_response_frequency());
	}
	if (ei.has_set_request_span()) {
		exported->set_request_span(ei.set_request_span());
	}
	if (ei.has_set_request_span_range()) {
		exported->set_request_span_range(ei.set_request_span_range().lo(),ei.set_request_span_range().hi());
	}
	if (ei.has_set_response_span()) {
		exported->set_response_span(ei.set_response_span());
	}
	if (ei.has_set_response_span_range()) {
		exported->set_response_span_range(ei.set_response_span_range().lo(),ei.set_response_span_range().hi());
	}
	if (ei.has_set_request_frequency_zapr()) {
		exported->set_request_frequency_zapr(ei.set_request_frequency_zapr());
	}
	if (ei.has_set_request_frequency_zapr_range()) {
		exported->set_request_frequency_zapr_range(ei.set_request_frequency_zapr_range().lo(),ei.set_request_frequency_zapr_range().hi());
	}
	if (ei.has_set_request_frequency_otv()) {
		exported->set_request_frequency_otv(ei.set_request_frequency_otv());
	}
	if (ei.has_set_request_frequency_otv_range()) {
		exported->set_request_frequency_otv_range(ei.set_request_frequency_otv_range().lo(),ei.set_request_frequency_otv_range().hi());
	}
	if (ei.has_set_request_level()) {
		exported->set_request_level(ei.set_request_level());
	}
	if (ei.has_set_request_level_range()) {
		exported->set_request_level_range(ei.set_request_level_range().lo(),ei.set_request_level_range().hi());
	}
	if (ei.has_set_response_level()) {
		exported->set_response_level(ei.set_response_level());
	}
	if (ei.has_set_efficiency()) {
		exported->set_efficiency(ei.set_efficiency());
	}
	if (ei.has_set_response_delay()) {
		exported->set_response_delay(ei.set_response_delay());
	}
	if (ei.has_set_range()) {
		exported->set_range(ei.set_range());
	}
	if (ei.has_set_battery_status()) {
		exported->set_battery_status(ei.set_battery_status());
	}
	if (ei.has_set_device_status()) {
		exported->set_device_status(ei.set_device_status());
	}
	if (ei.has_set_ksvn()) {
		exported->set_ksvn(ei.set_ksvn());
	}
	if (ei.has_set_co_code()) {
		exported->set_co_code(
			ei.set_co_code().code().c_str(),
			ei.set_co_code().aeroport().c_str()
			);
	}
	if (ei.has_set_statistics()) {
		exported->set_statistics(
			ei.set_statistics().ev_range(),
			ei.set_statistics().ev_delay(),
			ei.set_statistics().sigma(),
			ei.set_statistics().sko()
			);
	}
	if (ei.has_update_chart()) {
		exported->update_chart(
			ei.update_chart().maxy(),
			(s32*)ei.update_chart().yvalues().data(),
			ei.update_chart().yvalues_size(),
			ei.update_chart().remain()
			);
	}
	if (ei.has_set_user_mode()) {
		exported->set_user_mode((msg::UserMode::UserMode)ei.set_user_mode());
	}
	if (ei.has_set_request_form()) {
		exported->set_request_form((msg::tRequestForm::tRequestForm)ei.set_request_form());
	}
	if (ei.has_set_output_mode()) {
		exported->set_output_mode((msg::OutputMode::OutputMode)ei.set_output_mode());
	}
	if (ei.has_set_offset_x()) {
		exported->set_offset_x(ei.set_offset_x());
	}
	if (ei.has_key_event()) {
		exported->key_event((myvi::key_t::key_t)ei.key_event());
	}
	if (ei.has_upload_file()) {
		exported->upload_file(
			ei.upload_file().file_id(),
			ei.upload_file().offset(),
			ei.upload_file().crc(),
			ei.upload_file().first(),
			(u8*)ei.upload_file().data().data(),
			ei.upload_file().data().size()
			);
	}
	if (ei.has_download_file()) {
		exported->download_file(
			ei.download_file().file_id(),
			ei.download_file().offset(),
			ei.download_file().length()
			);
	}
	if (ei.has_update_file_info()) {
		exported->update_file_info(
			ei.update_file_info().file_id(),
			ei.update_file_info().cur_len(),
			ei.update_file_info().max_len(),
			ei.update_file_info().crc()
			);
	}
	if (ei.has_read_file_info()) {
		exported->read_file_info(ei.read_file_info());
	}
}

// ---------------------------------------
//сериализер на стороне хоста
// ---------------------------------------

void host_serializer_t::init(msg::host_interface_t *ahost, serial_interface_t *asintf ) {
	host = ahost;
	sintf = asintf;
	sintf->subscribe(this);

	ring_buffer_init(&hdlc_ring_buf,(u8_t*)_hdlc_ring_buf, HDLC_RING_BUF_LEN);
	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
}


void host_serializer_t::set_dme_channel(u8 number, msg::tSuffix::tSuffix suffix) {
	proto::exported_interface_t ei;
	proto::ChannelDMEChanged *dmech = ei.mutable_set_dme_channel();
	dmech->set_number(number);
	dmech->set_sfx((proto::tSuffix)suffix);
	send_message(ei, sintf);
}
void host_serializer_t::set_dme_channel_range(u8 lo, u8 hi) {
	proto::exported_interface_t ei;
	proto::exported_interface_t_range_u32_t *range = ei.mutable_set_dme_channel_range();
	range->set_lo(lo);
	range->set_hi(hi);
	send_message(ei, sintf);
}

void host_serializer_t::set_request_frequency(u32 value) {
	proto::exported_interface_t ei;
	ei.set_set_request_frequency(value);
	send_message(ei, sintf);
}
void host_serializer_t::set_request_frequency_range(u32 lo, u32 hi) {
	proto::exported_interface_t ei;
	proto::exported_interface_t_range_u32_t *range = ei.mutable_set_request_frequency_range();
	range->set_lo(lo);
	range->set_hi(hi);
	send_message(ei, sintf);
}
void host_serializer_t::set_response_frequency(u32 value) {
	proto::exported_interface_t ei;
	ei.set_set_response_frequency(value);
	send_message(ei, sintf);
}

void host_serializer_t::set_request_span(u32 value) {
	proto::exported_interface_t ei;
	ei.set_set_request_span(value);
	send_message(ei, sintf);
}
void host_serializer_t::set_request_span_range(u32 lo, u32 hi) {
	proto::exported_interface_t ei;
	proto::exported_interface_t_range_u32_t *range = ei.mutable_set_request_span_range();
	range->set_lo(lo);
	range->set_hi(hi);
	send_message(ei, sintf);
}
void host_serializer_t::set_response_span(u32 value) {
	proto::exported_interface_t ei;
	ei.set_set_response_span(value);
	send_message(ei, sintf);
}
void host_serializer_t::set_response_span_range(u32 lo, u32 hi) {
	proto::exported_interface_t ei;
	proto::exported_interface_t_range_u32_t *range = ei.mutable_set_response_span_range();
	range->set_lo(lo);
	range->set_hi(hi);
	send_message(ei, sintf);
}

void host_serializer_t::set_request_frequency_zapr(u32 value) {
	proto::exported_interface_t ei;
	ei.set_set_request_frequency_zapr(value);
	send_message(ei, sintf);
}
void host_serializer_t::set_request_frequency_zapr_range(u32 lo, u32 hi) {
	proto::exported_interface_t ei;
	proto::exported_interface_t_range_u32_t *range = ei.mutable_set_request_frequency_zapr_range();
	range->set_lo(lo);
	range->set_hi(hi);
	send_message(ei, sintf);
}

void host_serializer_t::set_request_frequency_otv(u32 value) {
	proto::exported_interface_t ei;
	ei.set_set_request_frequency_otv(value);
	send_message(ei, sintf);
}
void host_serializer_t::set_request_frequency_otv_range(u32 lo, u32 hi) {
	proto::exported_interface_t ei;
	proto::exported_interface_t_range_u32_t *range = ei.mutable_set_request_frequency_otv_range();
	range->set_lo(lo);
	range->set_hi(hi);
	send_message(ei, sintf);
}

void host_serializer_t::set_request_level(s32 value) {
	proto::exported_interface_t ei;
	ei.set_set_request_level(value);
	send_message(ei, sintf);
}
void host_serializer_t::set_request_level_range(s32 lo, s32 hi) {
	proto::exported_interface_t ei;
	proto::exported_interface_t_range_s32_t *range = ei.mutable_set_request_level_range();
	range->set_lo(lo);
	range->set_hi(hi);
	send_message(ei, sintf);
}
void host_serializer_t::set_response_level(s32 value) {
	proto::exported_interface_t ei;
	ei.set_set_response_level(value);
	send_message(ei, sintf);
}

void host_serializer_t::set_efficiency(u8 percent) {
	proto::exported_interface_t ei;
	ei.set_set_efficiency(percent);
	send_message(ei, sintf);
}
//	void set_code_reception_interval(u32 value);
void host_serializer_t::set_response_delay(u32 value) {
	proto::exported_interface_t ei;
	ei.set_set_response_delay(value);
	send_message(ei, sintf);
}
void host_serializer_t::set_range(u32 value) {
	proto::exported_interface_t ei;
	ei.set_set_range(value);
	send_message(ei, sintf);
}
void host_serializer_t::set_battery_status(u8 charge) {
	proto::exported_interface_t ei;
	ei.set_set_battery_status(charge);
	send_message(ei, sintf);
}
void host_serializer_t::set_device_status(u8 value) {
	proto::exported_interface_t ei;
	ei.set_set_device_status(value);
	send_message(ei, sintf);
}
void host_serializer_t::set_ksvn(u32 value) {
	proto::exported_interface_t ei;
	ei.set_set_ksvn(value);
	send_message(ei, sintf);
}

void host_serializer_t::set_co_code(const char * code, const char * aeroport) {
	proto::exported_interface_t ei;
	proto::exported_interface_t_co_code_t *co = ei.mutable_set_co_code();
	co->set_code(code);
	co->set_aeroport(aeroport);
	send_message(ei, sintf);
}
void host_serializer_t::set_statistics(u32 ev_range, u32 ev_delay, u32 sigma, u32 sko) {
	proto::exported_interface_t ei;
	proto::exported_interface_t_statistics_t *stat = ei.mutable_set_statistics();
	stat->set_ev_range(ev_range);
	stat->set_ev_delay(ev_delay);
	stat->set_sigma(sigma);
	stat->set_sko(sko);
	send_message(ei, sintf);
}

void host_serializer_t::update_chart(s32 maxy, s32 *yvalues, s32 length, s32 remain) {
	proto::exported_interface_t ei;
	proto::exported_interface_t_chart_data_t *cdata = ei.mutable_update_chart();
	cdata->set_maxy(maxy);
	for (int i=0; i < length; i++) {
		cdata->set_yvalues(i, yvalues[i]);
	}
	cdata->set_length(length);
	cdata->set_remain(remain);
	send_message(ei, sintf);
}
// n
void host_serializer_t::set_user_mode(msg::UserMode::UserMode uMode) {
	proto::exported_interface_t ei;
	ei.set_set_user_mode((proto::UserMode)uMode);
	send_message(ei, sintf);
}
void host_serializer_t::set_request_form(msg::tRequestForm::tRequestForm requestForm) {
	proto::exported_interface_t ei;
	ei.set_set_request_form((proto::tRequestForm)requestForm);
	send_message(ei, sintf);
}
void host_serializer_t::set_output_mode(msg::OutputMode::OutputMode uMode) {
	proto::exported_interface_t ei;
	ei.set_set_output_mode((proto::OutputMode)uMode);
	send_message(ei, sintf);
}

void host_serializer_t::set_offset_x(u32 v) {
	proto::exported_interface_t ei;
	ei.set_set_offset_x(v);
	send_message(ei, sintf);
}
//	void set_time_sweep(u32 timeSweep);
//	void set_screen_saver(u8 screenSaver);
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
	_MY_ASSERT(host, return);

	myvi::proto::host_interface_t h;
	_WEAK_ASSERT(h.ParseFromArray(data,len), return);

	if (h.has_channeldmechanged()) {
		host->ChannelDMEChanged(h.channeldmechanged().number(),(msg::tSuffix::tSuffix)h.channeldmechanged().sfx());
	}
	if (h.has_requestfrequency()) {
		host->RequestFrequency(h.requestfrequency());
	}
	if (h.has_requestfrequencyzapr()) {
		host->RequestFrequencyZapr(h.requestfrequencyzapr());
	}
	if (h.has_requestfrequencyotv()) {
		host->RequestFrequencyOtv(h.requestfrequencyotv());
	}
	if (h.has_requestspanchanged()) {
		host->RequestSpanChanged(h.requestspanchanged());
	}
	if (h.has_replyspanchanged()) {
		host->ReplySpanChanged(h.replyspanchanged());
	}
	if (h.has_rflevelchanged()) {
		host->RFLevelChanged(h.rflevelchanged());
	}
	if (h.has_requestformchanged()) {
		host->RequestFormChanged((msg::tRequestForm::tRequestForm)h.requestformchanged());
	}
	if (h.has_usermodechanged()) {
		host->UserModeChanged((msg::UserMode::UserMode)h.usermodechanged());
	}
	if (h.has_outputmodechanged()) {
		host->OutputModeChanged((msg::OutputMode::OutputMode)h.outputmodechanged());
	}
	if (h.has_resolution_x_changed()) {
		host->resolution_x_changed(h.resolution_x_changed());
	}
	if (h.has_resolution_y_changed()) {
		host->resolution_y_changed(h.resolution_y_changed());
	}
	if (h.has_offset_x_changed()) {
		host->offset_x_changed(h.offset_x_changed());
	}
	if (h.has_download_response()) {
		host->download_response(
			h.download_response().file_id(),
			h.download_response().offset(),

			h.download_response().crc(),
			h.download_response().first(),

			(u8*)h.download_response().data().data(),
			h.download_response().data().size()
			);
	}
	if (h.has_file_info_response()) {
		host->file_info_response(
			h.file_info_response().file_id(),
			h.file_info_response().cur_len(),
			h.file_info_response().max_len(),
			h.file_info_response().crc()
			);
	}
	if (h.has_error()) {
		host->error(h.error());
	}
}
