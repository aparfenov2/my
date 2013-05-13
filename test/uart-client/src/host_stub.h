#ifndef _HOST_STUB_H
#define _HOST_STUB_H

#include "exported.h"
#include "assert_impl.h"

class host_stub_t : public myvi::msg::host_interface_t {
public:

	virtual void ChannelDMEChanged(u8 number, myvi::msg::tSuffix::tSuffix suffix) OVERRIDE {
		_LOG3("ChannelDMEChanged: ", number, suffix);
	}
	virtual void RequestFrequency(u32 frequency) OVERRIDE {
		_LOG2("RequestFrequency: ", frequency);
	}
	virtual void RequestFrequencyZapr(u32 frequency) OVERRIDE {
		_LOG2("RequestFrequencyZapr: ", frequency);
	}
	virtual void RequestFrequencyOtv(u32 frequency) OVERRIDE {
		_LOG2("RequestFrequencyOtv: ", frequency);
	}
	virtual void RequestSpanChanged(u32 requestSpan) OVERRIDE {
		_LOG2("RequestSpanChanged: ", requestSpan);
	}
	virtual void ReplySpanChanged(u32 replySpan) OVERRIDE {
		_LOG2("ReplySpanChanged: ", replySpan);
	}
	virtual void RFLevelChanged(s32 RFLevel) OVERRIDE {
		_LOG2("RFLevelChanged: ", RFLevel);
	}
	virtual void RequestFormChanged(myvi::msg::tRequestForm::tRequestForm requestForm) OVERRIDE {
		_LOG2("RequestFormChanged: ", requestForm);
	}
	virtual void UserModeChanged(myvi::msg::UserMode::UserMode uMode) OVERRIDE {
		_LOG2("UserModeChanged: ", uMode);
	}
	virtual void OutputModeChanged(myvi::msg::OutputMode::OutputMode uMode) OVERRIDE {
		_LOG2("OutputModeChanged: ", uMode);
	}
	virtual void resolution_x_changed(u32 v) OVERRIDE {
		_LOG2("resolution_x_changed: ", v);
	}
	virtual void resolution_y_changed(u32 v) OVERRIDE {
		_LOG2("resolution_y_changed: ", v);
	}
	virtual void offset_x_changed(u32 v) OVERRIDE {
		_LOG2("offset_x_changed: ", v);
	}
};

#endif
