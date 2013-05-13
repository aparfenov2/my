#ifndef _MODEL_H
#define _MODEL_H

#include "exported.h"



// ------------------- parameter abstraction -------------------------------

namespace myvi {


namespace model_changed_what_t {
	enum model_changed_what_t {
		DME,			// канал DME , out, v
		REQ_FREQ,		// частота запроса, МГЦ , out, in, v
		REQ_FREQ_ZAPR,	// частота запросов, импп/c , out, in, v
		REQ_FREQ_OTV,	// частота ответов, импп/c , out, in, v
		REQ_SPAN,		// кодовый интервал запроса, мкс , out, in, v
		REQ_LEVEL,		// уровень запроса dbm, out, in, v

		RSP_FREQ,		// частота ответа, МГЦ , in
		RSP_SPAN,		// кодовый интервал ответа, мкс , in
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
		OFFSETX
	};
}

template<typename T>
class range_t {
public:
	T lo;
	T hi;
public:
	range_t():lo(),hi() {
	}
	range_t(T alo, T ahi):lo(alo),hi(ahi) {
	}
};



class suffix_t {
public:
	string_t name;
	s32 multiplier;
public:
	suffix_t() {
		multiplier = 1;
	}
	suffix_t(string_t aname,s32 amultiplier) {
		name = aname;
		multiplier = amultiplier;
	}

    bool operator == (const suffix_t& other) const {
		return name == other.name && multiplier == other.multiplier;
    }

	bool is_empty() {
		return name.is_empty();
	}

};


namespace units {
	enum time_t {
		_NS = 1,
		MKS = 1000
	};
	enum time2_t {
		MS  = 1,
		SEC = 1000,
		MIN = 100000
	};
	enum freq_t {
		_HZ = 1,
		_KHZ = 1000,
		MHZ = 1000000
	};
	enum real_t {
		FRAC = 1,
		INT = 1000
	};


	class time2_suffixes_t : public iterator_t<suffix_t> {
	public:
		static suffix_t ms;
		static suffix_t sec;
		static suffix_t min;
	public:
		virtual suffix_t* next(void* prev) OVERRIDE {
			if (!prev) return &ms;
			if (prev == &ms) return &sec;
			if (prev == &sec) return &min;
			return 0;
		}

		static suffix_t match(units::time2_t v) {
			switch(v) {
			case units::MS: return ms;
			case units::SEC: return sec;
			case units::MIN: return min;
			default: break;
			}
			_MY_ASSERT(0,return suffix_t("badsfx",1));
			return suffix_t("badsfx",1);
		}
	};

} // ns units

template<typename T, typename Tunit>
class value_t {
public:
	T value;
public:
	value_t() {
		value = T();
	}
	value_t(T avalue, Tunit aunit) {
		value = avalue * (T)aunit;
	}
	bool atof(string_t src, Tunit multiplier) {
		return conv_utils::atof<T>(src,value,(T)multiplier);
	}
	bool ftoa(volatile_string_t &dst, Tunit divider) {
		return conv_utils::ftoa<T>(value,dst,(T)divider);
	}
};



class app_model_t;
class model_changed_msg_t {
public:
	app_model_t *model;
	model_changed_what_t::model_changed_what_t what;
public:
	model_changed_msg_t(app_model_t *_app_model, model_changed_what_t::model_changed_what_t _what) {
		model = _app_model;
		what = _what;
	}
};


// глобальная модель 
// отражает состояние кнопок
#define MAX_APP_SUBSCRIBERS 32
class app_model_t : 
	public msg::exported_interface_t, 
	public publisher_t<model_changed_msg_t,MAX_APP_SUBSCRIBERS> {

public:
	typedef value_t<u32,units::freq_t> freq_t;
	typedef value_t<u32,units::time_t> time_t;
	typedef value_t<u32,units::time2_t> time2_t;
	typedef value_t<s32,units::real_t> real_t;

	msg::host_interface_t *host;
	struct dme_t {
		u8 ch;
		msg::tSuffix::tSuffix suffix;
		bool enabled;
		range_t<u8> range;
	} dme;
	freq_t req_f;
	range_t<freq_t> req_f_range;
	freq_t rsp_f;
	time_t req_span; 
	range_t<time_t> req_span_range;
	time_t rsp_span;
	range_t<time_t> rsp_span_range;
	u32 req_zapr;
	range_t<u32> req_zapr_range;
	u32 req_otv;
	range_t<u32> req_otv_range;
	s32 req_lev;
	range_t<s32> req_lev_range;

	s32 efficiency;
	time_t rsp_delay;
	u32 range;
	s32 rsp_lev;
	u32 bat_stat;
	u32 dev_status;
	real_t ksvn;

	msg::UserMode::UserMode user_mode;
	msg::OutputMode::OutputMode out_mode;
	msg::tRequestForm::tRequestForm req_form;

	struct {
		u32 ev_range;
		time_t ev_delay;
		real_t sigma;
		real_t sko;
	} staticstics;

#define MAX_CHART_LEN 128
#define MAX_INPUT_LEN 32

	struct {
		time2_t dpx; // ms, sec, min - единиц на шаг X
		units::time2_t dpx_sfx; 
		real_t dpy; // - единиц на шаг Y

		typedef struct {
			s32 maxy;
			bool filled;
			stack_t<s32, MAX_CHART_LEN> y_data;
		} buf_t;

		buf_t buf1;
		buf_t buf2;
		buf_t *disp_buf; // массив для отображения принятых данных
		buf_t *store_buf; // массив для сохранения поступающих данных

		struct {
			time2_t x;
			units::time2_t x_sfx;
			real_t y;
			bool enabled;
		} cursor;

		struct {
			time2_t x;
			units::time2_t x_sfx;
			real_t y;
		} offset;

		void swap_buf() {
			disp_buf = (disp_buf == &buf1) ? &buf2 : &buf1;
			store_buf = (store_buf == &buf1) ? &buf2 : &buf1;
		}
	} osc;

	struct {
		string_impl_t<MAX_INPUT_LEN> code;
		string_impl_t<MAX_INPUT_LEN> airport;
	} co_code;


private:
	app_model_t() {
		host = 0;
		dme.ch = 50;
		dme.enabled = true;
		dme.suffix = msg::tSuffix::X;
		dme.range = range_t<u8>(1,126);

		req_f = freq_t(1024, units::MHZ);
		req_f_range = range_t<freq_t>(freq_t(1024, units::MHZ), freq_t(1151, units::MHZ));
		
		req_span = time_t(12, units::MKS);
		req_span_range = range_t<time_t>(time_t(9, units::MKS), time_t(40, units::MKS));

		rsp_span = req_span;
		rsp_span_range = req_span_range;

		req_form = msg::tRequestForm::fRandom;
		req_zapr = 1200;
		req_zapr_range = range_t<u32>(0, 5000);
		req_otv = 1100;
		req_otv_range = range_t<u32>(0, 5000);

		req_lev = 10;
		req_lev_range = range_t<s32>(-40, +40);

		rsp_f = freq_t(1011, units::MHZ);
		efficiency = 100;
		rsp_delay = time_t(10, units::MKS);
		range = 1500;
		rsp_lev = -76;
		bat_stat = 70;
		dev_status = 0;
		ksvn = real_t(1630, units::FRAC);

		staticstics.ev_range = 1500;
		staticstics.ev_delay = time_t(10,units::MKS);
		staticstics.sigma = real_t(0, units::FRAC);
		staticstics.sko = real_t(0, units::FRAC);

		osc.dpx = time2_t(1, units::MS);
		osc.dpx_sfx = units::MS;
		osc.dpy = real_t(100,units::INT); // %
		osc.buf1.maxy = 1000;
		osc.buf2.maxy = 1000;

		osc.cursor.x = time2_t(1,units::MS);
		osc.cursor.x_sfx = units::MS;
		osc.cursor.y = real_t(25,units::INT);
		osc.cursor.enabled = true;
		osc.buf1.filled = false;
		osc.buf2.filled = false;
		osc.disp_buf = &osc.buf1;
		osc.store_buf = &osc.buf2;

		user_mode = msg::UserMode::umInterrogator;
		out_mode = msg::OutputMode::omAntenna;
	}
public:
	static app_model_t instance;

	void init() {
		dme_ch_changed(dme.ch,dme.suffix);
		//req_freq_changed
		//req_span_changed
		//rsp_span_changed
		req_freq_zapr_changed(req_zapr);
		req_freq_otv_changed(req_otv);
		req_level_changed(req_lev);
		req_form_changed(req_form);
		user_mode_changed(user_mode);
		//output_mode_changed(out_mode);
		resolution_x_changed(osc.dpx,osc.dpx_sfx);
		resolution_y_changed(osc.dpy);
		screen_saver_changed(0);
		this->cursor_enabled_changed(osc.cursor.enabled);
		this->cursor_x_changed(osc.cursor.x,osc.cursor.x_sfx);
		this->cursor_y_changed(osc.cursor.y);
	}

// ------------------- public msg::host_interface_t,  ---------------------------------
	void dme_ch_changed(u8 number, msg::tSuffix::tSuffix suffix) {
		dme.ch = number;
		dme.suffix = suffix;
		dme.enabled = true;
		model_changed_msg_t msg(this,model_changed_what_t::DME);
		notify(msg);
		if (host) host->ChannelDMEChanged(number,suffix);
	}

	void req_freq_changed(freq_t frequency) {
		req_f = frequency;
		dme.enabled = false;
		model_changed_msg_t msg(this, model_changed_what_t::DME);
		notify(msg);
		model_changed_msg_t msg2(this,model_changed_what_t::REQ_FREQ);
		notify(msg2);

		if (host) host->RequestFrequency(frequency.value);
	}

	void req_freq_zapr_changed(u32 frequency) {
		req_zapr = frequency;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_FREQ_ZAPR);
		notify(msg);
		if (host) host->RequestFrequencyZapr(frequency);
	}

	void req_freq_otv_changed(u32 frequency) {
		req_otv = frequency;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_FREQ_OTV);
		notify(msg);
		if (host) host->RequestFrequencyOtv(frequency);
	}

	void req_span_changed(time_t requestSpan) {
		req_span = requestSpan;
		dme.enabled = false;
		model_changed_msg_t msg(this, model_changed_what_t::DME);
		notify(msg);
		model_changed_msg_t msg2(this,model_changed_what_t::REQ_SPAN);
		notify(msg2);

		if (host) host->RequestSpanChanged(requestSpan.value);
	}

	void rsp_span_changed(time_t replySpan) {
		rsp_span = replySpan;
		dme.enabled = false;
		model_changed_msg_t msg(this, model_changed_what_t::DME);
		notify(msg);
		model_changed_msg_t msg2(this,model_changed_what_t::RSP_SPAN);
		notify(msg2);

		if (host) host->ReplySpanChanged(replySpan.value);
	}

	void req_level_changed(s32 RFLevel) {
		req_lev = RFLevel;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_LEVEL);
		notify(msg);
		if (host) host->RFLevelChanged(RFLevel);
	}

	void req_form_changed(msg::tRequestForm::tRequestForm requestForm) {
		req_form = requestForm;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_FORM);
		notify(msg);
		if (host) host->RequestFormChanged(requestForm);
	}

	void user_mode_changed(msg::UserMode::UserMode uMode) {
		user_mode = uMode;
		model_changed_msg_t msg(this,model_changed_what_t::USER_MODE);
		notify(msg);
		model_changed_msg_t msg2(this, model_changed_what_t::REQ_FREQ);
		notify(msg2);
		if (host) host->UserModeChanged(uMode);
	}

	void output_mode_changed(msg::OutputMode::OutputMode uMode) {
		out_mode = uMode;
		model_changed_msg_t msg(this,model_changed_what_t::OUTPUT_MODE);
		notify(msg);
		if (host) host->OutputModeChanged(uMode);
	}

	void resolution_x_changed(time2_t timeSweep, units::time2_t sfx) {
		osc.dpx = timeSweep;
		osc.dpx_sfx = sfx;
		model_changed_msg_t msg(this,model_changed_what_t::RESOLUTION_X);
		notify(msg);
		if (host) host->resolution_x_changed(osc.dpx.value);
	}

	void resolution_y_changed(real_t timeSweep) {
		osc.dpy = timeSweep;
		model_changed_msg_t msg(this,model_changed_what_t::RESOLUTION_Y);
		notify(msg);
		if (host) host->resolution_y_changed(osc.dpy.value);
	}

	void screen_saver_changed(u8 screenSaver) {
		model_changed_msg_t msg(this,model_changed_what_t::SCREEN_SAVER);
		notify(msg);
	}

	void cursor_x_changed(time2_t v, units::time2_t sfx) {
		osc.cursor.x = v;
		osc.cursor.x_sfx = sfx;
		model_changed_msg_t msg(this,model_changed_what_t::CURSOR);
		notify(msg);
	}

	void offset_x_changed(time2_t v, units::time2_t sfx) {
		osc.offset.x = v;
		osc.offset.x_sfx = sfx;
		model_changed_msg_t msg(this,model_changed_what_t::OFFSETX);
		notify(msg);
		if (host) host->offset_x_changed(v.value);
	}

	void cursor_y_changed(real_t v) {
		osc.cursor.y = v;
		model_changed_msg_t msg(this,model_changed_what_t::CURSOR);
		notify(msg);
	}
	void cursor_enabled_changed(bool e) {
		osc.cursor.enabled = e;
		model_changed_msg_t msg(this,model_changed_what_t::CURSOR);
		notify(msg);
	}
	void start_stop_changed(bool started) {
		dev_status = started ? 1 : 0;
		model_changed_msg_t msg(this,model_changed_what_t::DEV_STAT);
		notify(msg);
	}
// ----------------------- public msg::exported_interface_t, ------------------------------

	void subscribe_host(msg::host_interface_t *ahost)  {
		host = ahost;
	}

	virtual void set_dme_channel(u8 number, msg::tSuffix::tSuffix suffix) OVERRIDE {
		dme.ch = number;
		dme.suffix = suffix;
		dme.enabled = true;
		model_changed_msg_t msg(this,model_changed_what_t::DME);
		notify(msg);
	}

	virtual void set_dme_channel_range(u8 lo, u8 hi) OVERRIDE {
		dme.range.lo = lo;
		dme.range.hi = hi;
		model_changed_msg_t msg(this,model_changed_what_t::DME);
		notify(msg);
	}

	virtual void set_request_frequency(u32 value) OVERRIDE {
		req_f.value = value;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_FREQ);
		notify(msg);
	}

	virtual void set_request_frequency_range(u32 lo, u32 hi) OVERRIDE {
		req_f_range.lo.value = lo;
		req_f_range.hi.value = hi;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_FREQ);
		notify(msg);
	}


	virtual void set_response_frequency(u32 value) OVERRIDE {
		rsp_f.value = value;
		model_changed_msg_t msg(this,model_changed_what_t::RSP_FREQ);
		notify(msg);
	}

	virtual void set_request_span(u32 value) OVERRIDE {
		req_span.value = value;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_SPAN);
		notify(msg);
	}

	virtual void set_request_span_range(u32 lo, u32 hi) OVERRIDE {
		req_span_range.lo.value = lo;
		req_span_range.hi.value = hi;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_SPAN);
		notify(msg);
	}

	virtual void set_response_span(u32 value) OVERRIDE {
		rsp_span.value = value;
		model_changed_msg_t msg(this,model_changed_what_t::RSP_SPAN);
		notify(msg);
	}

	virtual void set_response_span_range(u32 lo, u32 hi) OVERRIDE {
		rsp_span_range.lo.value = lo;
		rsp_span_range.hi.value = hi;
		model_changed_msg_t msg(this,model_changed_what_t::RSP_SPAN);
		notify(msg);
	}

	virtual void set_request_frequency_zapr_range(u32 lo, u32 hi) OVERRIDE {
		req_zapr_range.lo = lo;
		req_zapr_range.hi = hi;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_FREQ_ZAPR);
		notify(msg);
	}

	virtual void set_request_frequency_otv_range(u32 lo, u32 hi) OVERRIDE {
		req_otv_range.lo = lo;
		req_otv_range.hi = hi;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_FREQ_OTV);
		notify(msg);
	}

	virtual void set_request_level_range(s32 lo, s32 hi) OVERRIDE {
		req_lev_range.lo = lo;
		req_lev_range.hi = hi;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_LEVEL);
		notify(msg);
	}


	virtual void set_efficiency(u8 percent) OVERRIDE {
		efficiency = percent;
		model_changed_msg_t msg(this,model_changed_what_t::EFFICIENCY);
		notify(msg);
	}


	//virtual void set_code_reception_interval(u32 value) OVERRIDE {
	//	notify(model_changed_msg_t(this,model_changed_what_t::RSP_SPAN));
	//}

	virtual void set_response_delay(u32 value) OVERRIDE {
		rsp_delay.value = value;
		model_changed_msg_t msg(this,model_changed_what_t::RSP_DELAY);
		notify(msg);
	}

	virtual void set_range(u32 value) OVERRIDE {
		range = value;
		model_changed_msg_t msg(this,model_changed_what_t::RANGE);
		notify(msg);
	}

	virtual void set_response_level(s32 value) OVERRIDE {
		rsp_lev = value;
		model_changed_msg_t msg(this,model_changed_what_t::RSP_LEVEL);
		notify(msg);
	}

	virtual void set_request_level(s32 value) OVERRIDE {
		req_lev = value;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_LEVEL);
		notify(msg);
	}

	virtual void set_battery_status(u8 charge) OVERRIDE {
		bat_stat = charge;
		model_changed_msg_t msg(this,model_changed_what_t::BAT_STAT);
		notify(msg);
	}

	virtual void set_device_status(u8 value) OVERRIDE {
		dev_status = value;
		model_changed_msg_t msg(this,model_changed_what_t::DEV_STAT);
		notify(msg);
	}

	virtual void set_ksvn(u32 value) OVERRIDE {
		ksvn.value = value;
		model_changed_msg_t msg(this,model_changed_what_t::KSVN);
		notify(msg);
	}

	virtual void set_co_code(const char * code, const char * aeroport) OVERRIDE {
		co_code.code = code;
		co_code.airport = aeroport;
		model_changed_msg_t msg(this,model_changed_what_t::CO_CODE);
		notify(msg);
	}

	virtual void set_statistics(u32 ev_range, u32 ev_delay, u32 sigma, u32 sko) OVERRIDE {
		staticstics.ev_range = ev_range;
		staticstics.ev_delay.value = ev_delay;
		staticstics.sigma.value = sigma;
		staticstics.sko.value = sko;
		model_changed_msg_t msg(this,model_changed_what_t::STATISTICS);
		notify(msg);
	}

	virtual void update_chart(s32 maxy, s32 *yvalues, s32 length, s32 remain) OVERRIDE {
//		_MY_ASSERT(dpx == osc.dpx.value,return); // разрешеие по горизонтали должно совпадать с текущим

		if (osc.store_buf->filled) {
			osc.store_buf->filled = false;
			osc.store_buf->y_data.clear();
		}

		if (osc.store_buf->y_data.length() + length <= osc.store_buf->y_data.capacity() ) {
			osc.store_buf->y_data.append(yvalues, length);
		}
		osc.store_buf->maxy = maxy;

		if (!remain) {
			osc.store_buf->filled = true;
			osc.swap_buf();
			model_changed_msg_t msg(this,model_changed_what_t::CHART);
			notify(msg);
		}
	}

	virtual void set_request_frequency_zapr(u32 value) OVERRIDE {
		req_zapr = value;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_FREQ_ZAPR);
		notify(msg);
	}
	virtual void set_request_frequency_otv(u32 value) OVERRIDE {
		req_otv = value;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_FREQ_OTV);
		notify(msg);
	}
	virtual void set_user_mode(msg::UserMode::UserMode uMode) OVERRIDE {
		user_mode = uMode;
		model_changed_msg_t msg(this,model_changed_what_t::USER_MODE);
		notify(msg);
	}
	virtual void set_request_form(msg::tRequestForm::tRequestForm requestForm) OVERRIDE {
		req_form = requestForm;
		model_changed_msg_t msg(this,model_changed_what_t::REQ_FORM);
		notify(msg);
	}
	virtual void set_output_mode(msg::OutputMode::OutputMode uMode) OVERRIDE {
		out_mode = uMode;
		model_changed_msg_t msg(this,model_changed_what_t::OUTPUT_MODE);
		notify(msg);
	}
	//virtual void set_time_sweep(u32 timeSweep) OVERRIDE {
	//	notify(model_changed_msg_t(this,model_changed_what_t::TIME_SWEEP));
	//}
	//virtual void set_screen_saver(u8 screenSaver) OVERRIDE {
	//	notify(model_changed_msg_t(this,model_changed_what_t::SCREEN_SAVER));
	//}

	virtual void set_offset_x(u32 v) OVERRIDE {
		osc.offset.x.value = v;
		model_changed_msg_t msg(this,model_changed_what_t::OFFSETX);
		notify(msg);
	}

	virtual void key_event(key_t::key_t key) OVERRIDE {
		globals::modal_overlay.key_event(key);
	}
};

} // ns myvi
#endif
