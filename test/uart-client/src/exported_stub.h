/*
 * exported_stub.h
 *
 *  Created on: 14.03.2013
 *      Author: gordeev
 */

#ifndef EXPORTED_STUB_H_
#define EXPORTED_STUB_H_

#include "exported.h"
#include "assert_impl.h"

class exported_stub_t : public myvi::msg::exported_interface_t {

	virtual void set_dme_channel(u8 number, myvi::msg::tSuffix::tSuffix suffix) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_dme_channel_range(u8 lo, u8 hi) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}

	virtual void set_request_frequency(u32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_request_frequency_range(u32 lo, u32 hi) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_response_frequency(u32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}

	virtual void set_request_span(u32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_request_span_range(u32 lo, u32 hi) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_response_span(u32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_response_span_range(u32 lo, u32 hi) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}

	virtual void set_request_frequency_zapr(u32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_request_frequency_zapr_range(u32 lo, u32 hi) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}

	virtual void set_request_frequency_otv(u32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_request_frequency_otv_range(u32 lo, u32 hi) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}

	virtual void set_request_level(s32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_request_level_range(s32 lo, s32 hi) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_response_level(s32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}

	virtual void set_efficiency(u8 percent) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
//	virtual void set_code_reception_interval(u32 value) = 0;
	virtual void set_response_delay(u32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_range(u32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_battery_status(u8 charge) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_device_status(u8 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_ksvn(u32 value) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}

	virtual void set_co_code(const char * code, const char * aeroport) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_statistics(u32 ev_range, u32 ev_delay, u32 sigma, u32 sko) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}

	virtual void update_chart(s32 maxy, s32 *yvalues, s32 length, s32 remain) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
// new
	virtual void set_user_mode(myvi::msg::UserMode::UserMode uMode) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_request_form(myvi::msg::tRequestForm::tRequestForm requestForm) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
	virtual void set_output_mode(myvi::msg::OutputMode::OutputMode uMode) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}

	virtual void set_offset_x(u32 v) OVERRIDE {
		_WEAK_ASSERT(0,return);
	}
//	virtual void set_time_sweep(u32 timeSweep) = 0;
//	virtual void set_screen_saver(u8 screenSaver) = 0;

};


#endif /* EXPORTED_STUB_H_ */
