#ifndef _HOST_EMU
#define _HOST_EMU

#include "exported.h"
#include "assert_impl.h"
//#include <iostream>
#include "basic.h"
#include <cstdlib>
//#include "serializer.h"

namespace myvi {

//	using namespace msg;
//	using namespace std;

#define _SER_MAX_CHART_LEN 100

class host_emu_t : public msg::host_interface_t {

struct dme_rec_t {
	u8 ch;
	msg::tSuffix::tSuffix sfx;
	u32 req_f;
	u32 req_span;
	u32 rsp_span;
};

public:
	static const dme_rec_t dme_table[];

#define MAX_CHART_LEN 128
	struct {
		u32 dpx; // ms, sec, min
		u32 maxy;
		stack_t<s32, MAX_CHART_LEN> y_values;
	} osc;
	msg::exported_interface_t *exported_interface;
public:
	host_emu_t() {
		osc.dpx = 1;
		osc.maxy = 1000; // %
		for (s32 i=0; i < MAX_CHART_LEN; i++) {
			s32 v = (std::rand() % osc.maxy) ;
			osc.y_values.push(v);
		}
	}

	void set_target(msg::exported_interface_t *aexported_interface) {
		exported_interface = aexported_interface;
//		exported_interface->subscribe_host(this);
	}

	void update() { // on_tick

		for (s32 i=0; i < osc.y_values.length(); i++) {
			s32 v = (std::rand() % osc.maxy) ;
			osc.y_values.data[i] = v;
		}

		s32 remain = osc.y_values.length();
		s32 offset = 0;

		do {
			s32 pkt_len = _MY_MIN(remain,_SER_MAX_CHART_LEN);
			remain -= pkt_len;

			exported_interface->update_chart(
				osc.maxy, 
				osc.y_values.data + offset, 
				pkt_len,
				remain
				);

			offset += _SER_MAX_CHART_LEN;

		} while (remain);
	}


	virtual void ChannelDMEChanged(u8 number, msg::tSuffix::tSuffix suffix) OVERRIDE {

		const dme_rec_t *rec = 0;
		for (s32 i=0; dme_table[i].ch; i++) {
			if (dme_table[i].ch == number && dme_table[i].sfx == suffix) {
				rec = &dme_table[i];
				break;
			}
		}
		_MY_ASSERT(rec,return);

		s32 req_f = rec->req_f * 1000000;
		s32 req_span = rec->req_span * 1000;
		s32 rsp_span = rec->rsp_span * 1000;

		exported_interface->set_dme_channel(number, suffix);
		exported_interface->set_request_frequency(req_f);
		exported_interface->set_request_span(req_span);
		exported_interface->set_response_span(rsp_span);
	}

	virtual void RequestFrequency(u32 frequency) OVERRIDE {
		exported_interface->set_request_frequency(frequency);

	}

	virtual void RequestFrequencyZapr(u32 frequency) OVERRIDE {
		exported_interface->set_request_frequency_zapr(frequency);
	}
	virtual void RequestFrequencyOtv(u32 frequency) OVERRIDE {
		exported_interface->set_request_frequency_otv(frequency);
	}
	virtual void RequestSpanChanged(u32 requestSpan) OVERRIDE {
		exported_interface->set_request_span(requestSpan);
	}
	virtual void ReplySpanChanged(u32 replySpan) OVERRIDE {
		exported_interface->set_response_span(replySpan);
	}
	virtual void RFLevelChanged(s32 RFLevel) OVERRIDE {
		exported_interface->set_request_level(RFLevel);
	}
	virtual void RequestFormChanged(msg::tRequestForm::tRequestForm requestForm) OVERRIDE {
		exported_interface->set_request_form(requestForm);
	}
	virtual void UserModeChanged(msg::UserMode::UserMode uMode) OVERRIDE {
		exported_interface->set_user_mode(uMode);

		if (uMode == msg::UserMode::umInterrogator) {
			exported_interface->set_request_frequency_range(1024*1000000, 1151*1000000);
		} else {
			exported_interface->set_request_frequency_range(960*1000000, 1215*1000000);
		}
	}
	virtual void OutputModeChanged(msg::OutputMode::OutputMode uMode) OVERRIDE {
		exported_interface->set_output_mode(uMode);
	}

	virtual void resolution_x_changed(u32 v) OVERRIDE {
		osc.dpx = v;
	}

	virtual void resolution_y_changed(u32 v) OVERRIDE {
	}

	virtual void offset_x_changed(u32 v) OVERRIDE {
		exported_interface->set_offset_x(v);
	}

	//virtual void TimeSweepChanged(u32 timeSweep) OVERRIDE {
	//	exported_interface->set_time_sweep(timeSweep);
	//}
	//virtual void ScreenSaverChanged(u8 screenSaver) OVERRIDE {
	//	exported_interface->set_screen_saver(screenSaver);
	//}

};

} // ns
#endif
