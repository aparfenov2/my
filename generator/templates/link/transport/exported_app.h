/*
* ����������� ����������� ����� ����������� ������ ��� ������ � �������� ����� � ���� ��������
*
* WARNING: ������������� ��������������� ����
*/

#ifndef _MESSAGES_H
#define _MESSAGES_H


#include "types.h"
#include "app_events.h"


namespace myvi {


namespace msg {

// ������� ������ DME
namespace tSuffix {
	enum tSuffix {
		X = 0,
		Y = 1
	};
}

// ������ �������
namespace tRequestForm {
	enum tRequestForm {
		fRandom = 0, // ���
		fDetermine = 1, // �����
		fExternal = 2, // ����
		fReqOff = 3 // ����
	};
}

// ����� ������
namespace UserMode {
	enum UserMode {
		umInterrogator = 0, // ���������
		umTransmitter = 1 // ��������������
	};
}

// ������������ ������
namespace OutputMode {
	enum OutputMode {
		omAntenna  = 0, // ���
		omBeacon  = 1, // ����
		omCalibration   = 2 // ����������� ���������
	};
}


// ��������� ��������� ������� ��� ��������� ���������
class host_interface_t {
public:
public:
	// ����� DME
	virtual void ChannelDMEChanged(u8 number, tSuffix::tSuffix suffix) = 0;
	// ������� �������, ��
	virtual void RequestFrequency(u32 frequency) = 0;
	// ����� ���. ��� � ��� (������)
	virtual void RequestFrequencyZapr(u32 frequency) = 0;
	// ����� ���. ��� � ��� (�����)
	virtual void RequestFrequencyOtv(u32 frequency) = 0;
	// ������� �������� �������, ��
	virtual void RequestSpanChanged(u32 requestSpan) = 0;
	// ������� �������� ������, ��
	virtual void ReplySpanChanged(u32 replySpan) = 0;
	// ������� �������� ��������, dbm
	virtual void RFLevelChanged(s32 RFLevel) = 0;
	// ������ �������
	virtual void RequestFormChanged(tRequestForm::tRequestForm requestForm) = 0;
	// ����� ������
	virtual void UserModeChanged(UserMode::UserMode uMode) = 0;
	// ������������ �����
	virtual void OutputModeChanged(OutputMode::OutputMode uMode) = 0;
	// ��������� �� �������, ��
	virtual void resolution_x_changed(u32 v) = 0;
	// ����. ������� ��������, ��������
	virtual void resolution_y_changed(u32 v) = 0;
	// �������� �� ��� x, ��
	virtual void offset_x_changed(u32 v) = 0;
};


// ��������� "����" ��� ��������� �������
class exported_interface_t {
public:

//	virtual void subscribe_host(host_interface_t *host) = 0;

	// ����� DME
	virtual void set_dme_channel(u8 number, tSuffix::tSuffix suffix) = 0;
	// �������� ������� DME
	virtual void set_dme_channel_range(u8 lo, u8 hi) = 0;
	// ������� �������, ��
	virtual void set_request_frequency(u32 value) = 0;
	// �������� ������� �������, ��
	virtual void set_request_frequency_range(u32 lo, u32 hi) = 0;
	// ������� ������, ��
	virtual void set_response_frequency(u32 value) = 0;
	// ������� �������� �������, ��
	virtual void set_request_span(u32 value) = 0;
	// �������� ������� �������� �������, ��
	virtual void set_request_span_range(u32 lo, u32 hi) = 0;
	// ������� �������� ������, ��
	virtual void set_response_span(u32 value) = 0;
	// �������� ������� �������� ������, ��
	virtual void set_response_span_range(u32 lo, u32 hi) = 0;
	// ����� ���. ��� � ��� (������)
	virtual void set_request_frequency_zapr(u32 value) = 0;
	// �������� ����� ���. ��� � ��� (������)
	virtual void set_request_frequency_zapr_range(u32 lo, u32 hi) = 0;
	// ����� ���. ��� � ��� (�����)
	virtual void set_request_frequency_otv(u32 value) = 0;
	// �������� ����� ���. ��� � ��� (�����)
	virtual void set_request_frequency_otv_range(u32 lo, u32 hi) = 0;
	// ������� �������� ��������, dbm
	virtual void set_request_level(s32 value) = 0;
	// �������� �������  �������� �������, dbm
	virtual void set_request_level_range(s32 lo, s32 hi) = 0;
	// ������� �������� ������, dbm
	virtual void set_response_level(s32 value) = 0;
	// �������������, %
	virtual void set_efficiency(u8 percent) = 0;
//	virtual void set_code_reception_interval(u32 value) = 0;
	// ����� ���. ��� � ��� (�����)
	virtual void set_response_delay(u32 value) = 0;
	// ����������, �
	virtual void set_range(u32 value) = 0;
	// ����� �������, %
	virtual void set_battery_status(u8 charge) = 0;
	// ��������� ����������, 1 - ��������, 0 - �����
	virtual void set_device_status(u8 value) = 0;
	// ����, ��������
	virtual void set_ksvn(u32 value) = 0;
	// ��� CO
	virtual void set_co_code(const char * code, const char * aeroport) = 0;
	// ����������
	// ev_range, �
	// ev_delay, ���
	// sigma, sko, 1/1000
	virtual void set_statistics(u32 ev_range, u32 ev_delay, u32 sigma, u32 sko) = 0;
	// �������� ������
	// maxy - ������� ����. �������� � �������, 1/1000
	// yvalues - ������ �������� , 1/1000
	// length - ����� ������, s32 ����
	// remain == 0
	virtual void update_chart(s32 maxy, s32 *yvalues, s32 length, s32 remain) = 0;
	// ����� ������
	virtual void set_user_mode(UserMode::UserMode uMode) = 0;
	// ������ �������
	virtual void set_request_form(tRequestForm::tRequestForm requestForm) = 0;
	// ������������ �����
	virtual void set_output_mode(OutputMode::OutputMode uMode) = 0;
	// �������� �� ��� x, ��
	virtual void set_offset_x(u32 v) = 0;
//	virtual void set_time_sweep(u32 timeSweep) = 0;
//	virtual void set_screen_saver(u8 screenSaver) = 0;
};

} // ns msg
} // ns

#endif
