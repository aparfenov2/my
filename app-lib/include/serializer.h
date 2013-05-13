/*
 * serializer.h
 *
 *  Created on: 12.03.2013
 *      Author: gordeev
 */

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include "exported.h"
#include "assert_impl.h"

#include "packetizer.h"
#include <string.h>

namespace myvi {

namespace pkt_what_t {
	enum pkt_what_t {
		DME,			// канал DME , out, v
		DME_RANGE,
		REQ_FREQ,		// частота запроса, МГЦ , out, in, v
		REQ_FREQ_RANGE,
		REQ_FREQ_ZAPR,	// частота запросов, импп/c , out, in, v
		REQ_FREQ_ZAPR_RANGE,
		REQ_FREQ_OTV,	// частота ответов, импп/c , out, in, v
		REQ_FREQ_OTV_RANGE,
		REQ_SPAN,		// кодовый интервал запроса, мкс , out, in, v
		REQ_SPAN_RANGE,
		REQ_LEVEL,		// уровень запроса dbm, out, in, v
		REQ_LEVEL_RANGE,

		RSP_FREQ,		// частота ответа, МГЦ , in
		RSP_SPAN,		// кодовый интервал ответа, мкс , in
		RSP_SPAN_RANGE,
		RSP_LEVEL,		// уровень ответа ,дБм, in
		RSP_DELAY,		// задержка ответа, мкс, in

		REQ_FORM,		// формат запроса, out, in
		USER_MODE,		// режим работы, out, in
		OUTPUT_MODE,	// конфигурация выхода, out, in

		EFFICIENCY,		// эффективность, %, in
		RANGE,			// дальность ,М, in
		BAT_STAT,		// заряд батареи , %, in
		DEV_STAT,		// статус устройтсва , in
		KSVN,			// ксвн , in
		CO_CODE,		// Код СО , in
		STATISTICS,		// статистика , in
		CHART,			// данные графика , in

		RESOLUTION_X,		// развертка по времени, мкс/дел , internal
		RESOLUTION_Y,		// развертка по времени, мкс/дел , internal
		CURSOR,
		SCREEN_SAVER,	// задержка отключения дисплея, с, internal
		OFFSETX,
		KEY_PRESSED
	};
}

namespace msg {

struct packet_t {
	pkt_what_t::pkt_what_t what;
	union {
		struct {
			u8 number;
			u8 lo;
			u8 hi;
			msg::tSuffix::tSuffix suffix;
		} ch_dme_changed;

		struct {
			u8 param_u8;
			u8 lo_u8;
			u8 hi_u8;
		} su8;

		struct {
			u32 param_u32;
			u32 lo_u32;
			u32 hi_u32;
		} su32;

		struct {
			s32 param_s32;
			s32 lo_s32;
			s32 hi_s32;
		} ss32;

		msg::tRequestForm::tRequestForm requestForm;
		msg::UserMode::UserMode uMode;
		msg::OutputMode::OutputMode oMode;

		struct {
			char code[62];
			char aeroport[62];
		} co_code;

		struct {
			u32 ev_range;
			u32 ev_delay;
			u32 sigma;
			u32 sko;
		} statistics;

		struct {
			s32 maxy;
			s32 length;
			s32 remain;
			s32 yvalues[59];
		} chart_data_part;
	};
};

#define _SER_MAX_CHART_LEN 59

#ifdef PLATFORM_C28
STATIC_ASSERT(sizeof(packet_t) == 126 , sizeof_pkt_what_t);
#else
STATIC_ASSERT(sizeof(packet_t) == 252 , sizeof_pkt_what_t);
#endif

} // ns msg

class pkt_serdes_t : public packetizer_t::receiver_t  {
public:
	class serdes_receiver_t {
	public:
		virtual void receive(msg::packet_t* data) = 0;
	};
public:
	packetizer_t *spi;
	serdes_receiver_t *parent;
public:
	pkt_serdes_t() {
		spi = 0;
		parent = 0;
	}

	void send(msg::packet_t &pkt) {
		if (spi) {
			spi->send(&pkt, sizeof(pkt));
		}
	}
	virtual void receive(const void *data, s32 size) OVERRIDE {
		if (parent) {
			_WEAK_ASSERT(sizeof(msg::packet_t) == size, return);
			parent->receive((msg::packet_t*)data);
		}
	}
};

class serializer_t : public msg::host_interface_t, public pkt_serdes_t::serdes_receiver_t {


private:
	pkt_serdes_t ser;
	msg::exported_interface_t *exported;
public:
	serializer_t() {
		ser.parent = this;
	}

	void init(packetizer_t *spi, msg::exported_interface_t *aexported) {
		exported = aexported;
		ser.spi = spi;
		spi->subscribe(&ser);
	}

// ----------------------- public msg::host_interface_t (туда)------------------------------------
	virtual void ChannelDMEChanged(u8 number, msg::tSuffix::tSuffix suffix) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::DME;
		pkt.ch_dme_changed.number = number;
		pkt.ch_dme_changed.suffix = suffix;
		ser.send(pkt);
	}
	virtual void RequestFrequency(u32 frequency) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FREQ;
		pkt.su32.param_u32 = frequency;
		ser.send(pkt);
	}
	virtual void RequestFrequencyZapr(u32 frequency) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FREQ_ZAPR;
		pkt.su32.param_u32 = frequency;
		ser.send(pkt);
	}
	virtual void RequestFrequencyOtv(u32 frequency) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FREQ_OTV;
		pkt.su32.param_u32 = frequency;
		ser.send(pkt);
	}
	virtual void RequestSpanChanged(u32 requestSpan) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_SPAN;
		pkt.su32.param_u32 = requestSpan;
		ser.send(pkt);
	}
	virtual void ReplySpanChanged(u32 replySpan) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::RSP_SPAN;
		pkt.su32.param_u32 = replySpan;
		ser.send(pkt);
	}
	virtual void RFLevelChanged(s32 RFLevel) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_LEVEL;
		pkt.ss32.param_s32 = RFLevel;
		ser.send(pkt);
	}
	virtual void RequestFormChanged(msg::tRequestForm::tRequestForm requestForm) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FORM;
		pkt.requestForm = requestForm;
		ser.send(pkt);
	}
	virtual void UserModeChanged(msg::UserMode::UserMode uMode) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::USER_MODE;
		pkt.uMode = uMode;
		ser.send(pkt);
	}
	virtual void OutputModeChanged(msg::OutputMode::OutputMode oMode) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::OUTPUT_MODE;
		pkt.oMode = oMode;
		ser.send(pkt);
	}
	virtual void resolution_x_changed(u32 v) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::RESOLUTION_X;
		pkt.su32.param_u32 = v;
		ser.send(pkt);
	}
	virtual void resolution_y_changed(u32 v) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::RESOLUTION_Y;
		pkt.su32.param_u32 = v;
		ser.send(pkt);
	}
	virtual void offset_x_changed(u32 v) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::OFFSETX;
		pkt.su32.param_u32 = v;
		ser.send(pkt);
	}

// ------------------- public msg::exported_interface_t --------------------

	virtual void receive(msg::packet_t *pkt) OVERRIDE {
		_MY_ASSERT(exported, return);

		switch (pkt->what) {
		case pkt_what_t::DME:
			exported->set_dme_channel(pkt->ch_dme_changed.number, pkt->ch_dme_changed.suffix);
			break;
		case pkt_what_t::DME_RANGE:
			exported->set_dme_channel_range(pkt->ch_dme_changed.lo, pkt->ch_dme_changed.hi);
			break;

		case pkt_what_t::REQ_FREQ:
			exported->set_request_frequency(pkt->su32.param_u32);
			break;
		case pkt_what_t::REQ_FREQ_RANGE:
			exported->set_request_frequency_range(pkt->su32.lo_u32, pkt->su32.hi_u32);
			break;
		case pkt_what_t::RSP_FREQ:
			exported->set_response_frequency(pkt->su32.param_u32);
			break;

		case pkt_what_t::REQ_FREQ_ZAPR:
			exported->set_request_frequency_zapr(pkt->su32.param_u32);
			break;
		case pkt_what_t::REQ_FREQ_ZAPR_RANGE:
			exported->set_request_frequency_zapr_range(pkt->su32.lo_u32, pkt->su32.hi_u32);
			break;
		case pkt_what_t::REQ_FREQ_OTV:
			exported->set_request_frequency_otv(pkt->su32.param_u32);
			break;
		case pkt_what_t::REQ_FREQ_OTV_RANGE:
			exported->set_request_frequency_otv_range(pkt->su32.lo_u32, pkt->su32.hi_u32);
			break;

		case pkt_what_t::REQ_SPAN:
			exported->set_request_span(pkt->su32.param_u32);
			break;
		case pkt_what_t::REQ_SPAN_RANGE:
			exported->set_request_span_range(pkt->su32.lo_u32, pkt->su32.hi_u32);
			break;
		case pkt_what_t::RSP_SPAN:
			exported->set_response_span(pkt->su32.param_u32);
			break;
		case pkt_what_t::RSP_SPAN_RANGE:
			exported->set_response_span_range(pkt->su32.lo_u32, pkt->su32.hi_u32);
			break;

		case pkt_what_t::RANGE:
			exported->set_range(pkt->su32.param_u32);
			break;

		case pkt_what_t::REQ_LEVEL:
			exported->set_request_level(pkt->ss32.param_s32);
			break;
		case pkt_what_t::REQ_LEVEL_RANGE:
			exported->set_request_level_range(pkt->ss32.lo_s32, pkt->ss32.hi_s32);
			break;
		case pkt_what_t::RSP_LEVEL:
			exported->set_response_level(pkt->ss32.param_s32);
			break;

		case pkt_what_t::EFFICIENCY:
			exported->set_efficiency(pkt->su8.param_u8);
			break;
		case pkt_what_t::RSP_DELAY:
			exported->set_response_delay(pkt->su32.param_u32);
			break;
		case pkt_what_t::BAT_STAT:
			exported->set_battery_status(pkt->su8.param_u8);
			break;
		case pkt_what_t::DEV_STAT:
			exported->set_device_status(pkt->su8.param_u8);
			break;
		case pkt_what_t::KSVN:
			exported->set_ksvn(pkt->su32.param_u32);
			break;
		case pkt_what_t::CO_CODE:
			exported->set_co_code(pkt->co_code.code, pkt->co_code.aeroport);
			break;
		case pkt_what_t::STATISTICS:
			exported->set_statistics(pkt->statistics.ev_range, pkt->statistics.ev_delay, pkt->statistics.sigma, pkt->statistics.sko);
			break;
		case pkt_what_t::CHART:
			exported->update_chart(pkt->chart_data_part.maxy, pkt->chart_data_part.yvalues, pkt->chart_data_part.length, pkt->chart_data_part.remain);
			break;
		case pkt_what_t::USER_MODE:
			exported->set_user_mode(pkt->uMode);
			break;
		case pkt_what_t::REQ_FORM:
			exported->set_request_form(pkt->requestForm);
			break;
		case pkt_what_t::OUTPUT_MODE:
			exported->set_output_mode(pkt->oMode);
			break;
		case pkt_what_t::OFFSETX:
			exported->set_offset_x(pkt->su32.param_u32);
			break;
		case pkt_what_t::KEY_PRESSED:
			exported->key_event((key_t::key_t)pkt->su8.param_u8);
			break;
			
		default:
			_WEAK_ASSERT(0,return);
			break;
		}
	}


}; // class


//сериализер на стороне хоста
class host_serializer_t : public msg::exported_interface_t, public pkt_serdes_t::serdes_receiver_t {

private:
	pkt_serdes_t ser;
	msg::host_interface_t *host;
public:
	host_serializer_t() {
		ser.parent = this;
	}

	void init(packetizer_t *spi, msg::host_interface_t *ahost) {
		host = ahost;
		ser.spi = spi;
		spi->subscribe(&ser);
	}
// ------------------------ public msg::exported_interface_t ----------------------------
	virtual void set_dme_channel(u8 number, msg::tSuffix::tSuffix suffix) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::DME;
		pkt.ch_dme_changed.number = number;
		pkt.ch_dme_changed.suffix = suffix;
		ser.send(pkt);
	}
	virtual void set_dme_channel_range(u8 lo, u8 hi) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::DME_RANGE;
		pkt.ch_dme_changed.lo = lo;
		pkt.ch_dme_changed.hi = hi;
		ser.send(pkt);
	}

	virtual void set_request_frequency(u32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FREQ;
		pkt.su32.param_u32 = value;
		ser.send(pkt);
	}
	virtual void set_request_frequency_range(u32 lo, u32 hi) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FREQ_RANGE;
		pkt.su32.lo_u32 = lo;
		pkt.su32.hi_u32 = hi;
		ser.send(pkt);
	}
	virtual void set_response_frequency(u32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::RSP_FREQ;
		pkt.su32.param_u32 = value;
		ser.send(pkt);
	}

	virtual void set_request_span(u32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_SPAN;
		pkt.su32.param_u32 = value;
		ser.send(pkt);
	}
	virtual void set_request_span_range(u32 lo, u32 hi) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_SPAN_RANGE;
		pkt.su32.lo_u32 = lo;
		pkt.su32.hi_u32 = hi;
		ser.send(pkt);
	}
	virtual void set_response_span(u32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::RSP_SPAN;
		pkt.su32.param_u32 = value;
		ser.send(pkt);
	}
	virtual void set_response_span_range(u32 lo, u32 hi) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::RSP_SPAN_RANGE;
		pkt.su32.lo_u32 = lo;
		pkt.su32.hi_u32 = hi;
		ser.send(pkt);
	}

	virtual void set_request_frequency_zapr(u32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FREQ_ZAPR;
		pkt.su32.param_u32 = value;
		ser.send(pkt);
	}
	virtual void set_request_frequency_zapr_range(u32 lo, u32 hi) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FREQ_ZAPR_RANGE;
		pkt.su32.lo_u32 = lo;
		pkt.su32.hi_u32 = hi;
		ser.send(pkt);
	}

	virtual void set_request_frequency_otv(u32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FREQ_OTV;
		pkt.su32.param_u32 = value;
		ser.send(pkt);
	}
	virtual void set_request_frequency_otv_range(u32 lo, u32 hi) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FREQ_OTV_RANGE;
		pkt.su32.lo_u32 = lo;
		pkt.su32.hi_u32 = hi;
		ser.send(pkt);
	}

	virtual void set_request_level(s32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_LEVEL;
		pkt.ss32.param_s32 = value;
		ser.send(pkt);
	}
	virtual void set_request_level_range(s32 lo, s32 hi) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_LEVEL_RANGE;
		pkt.ss32.lo_s32 = lo;
		pkt.ss32.hi_s32 = hi;
		ser.send(pkt);
	}
	virtual void set_response_level(s32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::RSP_LEVEL;
		pkt.ss32.param_s32 = value;
		ser.send(pkt);
	}

	virtual void set_efficiency(u8 percent) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::EFFICIENCY;
		pkt.su8.param_u8 = percent;
		ser.send(pkt);
	}
//	virtual void set_code_reception_interval(u32 value) = 0;
	virtual void set_response_delay(u32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::RSP_DELAY;
		pkt.su32.param_u32 = value;
		ser.send(pkt);
	}
	virtual void set_range(u32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::RANGE;
		pkt.su32.param_u32 = value;
		ser.send(pkt);
	}
	virtual void set_battery_status(u8 charge) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::BAT_STAT;
		pkt.su8.param_u8 = charge;
		ser.send(pkt);
	}
	virtual void set_device_status(u8 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::DEV_STAT;
		pkt.su8.param_u8 = value;
		ser.send(pkt);
	}
	virtual void set_ksvn(u32 value) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::KSVN;
		pkt.su32.param_u32 = value;
		ser.send(pkt);
	}


	virtual void set_co_code(const char * code, const char * aeroport) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::CO_CODE;
		memcpy(pkt.co_code.code,code,_MY_MIN(strlen(code)+1,sizeof(pkt.co_code.code)));
		memcpy(pkt.co_code.aeroport,aeroport,_MY_MIN(strlen(aeroport)+1,sizeof(pkt.co_code.aeroport)));
		ser.send(pkt);
	}
	virtual void set_statistics(u32 ev_range, u32 ev_delay, u32 sigma, u32 sko) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::STATISTICS;
		pkt.statistics.ev_range = ev_range;
		pkt.statistics.ev_delay = ev_delay;
		pkt.statistics.sigma = sigma;
		pkt.statistics.sko = sko;
		ser.send(pkt);
	}

	virtual void update_chart(s32 maxy, s32 *yvalues, s32 length, s32 aremain) OVERRIDE {
		s32 remain = length;
		s32 offset = 0;

		do {
			s32 pkt_len = _MY_MIN(remain,_SER_MAX_CHART_LEN);
			remain -= pkt_len;

			_update_chart(
				maxy, 
				yvalues + offset, 
				pkt_len,
				remain + aremain
				);

			offset += _SER_MAX_CHART_LEN;

		} while (remain);
	}

	void _update_chart(s32 maxy, s32 *yvalues, s32 length, s32 remain) {

		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		_MY_ASSERT(length <= sizeof(pkt.chart_data_part.yvalues)/sizeof(s32), return);

		pkt.what = pkt_what_t::CHART;
		pkt.chart_data_part.maxy = maxy;
		pkt.chart_data_part.length = length;
		pkt.chart_data_part.remain = remain;
		memcpy(pkt.chart_data_part.yvalues,yvalues, length * sizeof(s32));
		ser.send(pkt);
	}
// new
	virtual void set_user_mode(msg::UserMode::UserMode uMode) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::USER_MODE;
		pkt.uMode = uMode;
		ser.send(pkt);
	}
	virtual void set_request_form(msg::tRequestForm::tRequestForm requestForm) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::REQ_FORM;
		pkt.requestForm = requestForm;
		ser.send(pkt);
	}
	virtual void set_output_mode(msg::OutputMode::OutputMode oMode) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::OUTPUT_MODE;
		pkt.oMode = oMode;
		ser.send(pkt);
	}

	virtual void set_offset_x(u32 v) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::OFFSETX;
		pkt.su32.param_u32 = v;
		ser.send(pkt);
	}

	virtual void key_event(key_t::key_t key) OVERRIDE {
		msg::packet_t pkt;
		memset(&pkt,0,sizeof(pkt));
		pkt.what = pkt_what_t::KEY_PRESSED;
		pkt.su8.param_u8 = key;
		ser.send(pkt);
	}


// ------------------------ public pkt_serdes_t::serdes_receiver_t ----------------------
	virtual void receive(msg::packet_t *pkt) OVERRIDE {
		_MY_ASSERT(host, return);
		switch (pkt->what) {
		case pkt_what_t::DME:
			host->ChannelDMEChanged(pkt->ch_dme_changed.number,pkt->ch_dme_changed.suffix);
			break;
		case pkt_what_t::REQ_FREQ:
			host->RequestFrequency(pkt->su32.param_u32);
			break;
		case pkt_what_t::REQ_FREQ_ZAPR:
			host->RequestFrequencyZapr(pkt->su32.param_u32);
			break;
		case pkt_what_t::REQ_FREQ_OTV:
			host->RequestFrequencyOtv(pkt->su32.param_u32);
			break;
		case pkt_what_t::REQ_SPAN:
			host->RequestSpanChanged(pkt->su32.param_u32);
			break;
		case pkt_what_t::RSP_SPAN:
			host->ReplySpanChanged(pkt->su32.param_u32);
			break;
		case pkt_what_t::REQ_LEVEL:
			host->RFLevelChanged(pkt->ss32.param_s32);
			break;
		case pkt_what_t::REQ_FORM:
			host->RequestFormChanged(pkt->requestForm);
			break;
		case pkt_what_t::USER_MODE:
			host->UserModeChanged(pkt->uMode);
			break;
		case pkt_what_t::OUTPUT_MODE:
			host->OutputModeChanged(pkt->oMode);
			break;
		case pkt_what_t::RESOLUTION_X:
			host->resolution_x_changed(pkt->su32.param_u32);
			break;
		case pkt_what_t::RESOLUTION_Y:
			host->resolution_y_changed(pkt->su32.param_u32);
			break;
		case pkt_what_t::OFFSETX:
			host->offset_x_changed(pkt->su32.param_u32);
			break;
		default:
			_WEAK_ASSERT(0,return);
			break;
		}
	}
}; // host_serializer_t

} // ns myvi
#endif /* SERIALIZER_H_ */
