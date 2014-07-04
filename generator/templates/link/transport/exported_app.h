/*
* Определение интерфейсов связи прикладного уровня для прямой и обратной связи с этой системой
*
* WARNING: АВТОМАТИЧЕСКИ СГЕНЕРИРОВАННЫЙ ФАЙЛ
*/

#ifndef _MESSAGES_H
#define _MESSAGES_H


#include "types.h"
#include "app_events.h"


namespace myvi {


namespace msg {

// суффикс канала DME
namespace tSuffix {
	enum tSuffix {
		X = 0,
		Y = 1
	};
}

// Формат запроса
namespace tRequestForm {
	enum tRequestForm {
		fRandom = 0, // ХИП
		fDetermine = 1, // Пачка
		fExternal = 2, // Внеш
		fReqOff = 3 // Выкл
	};
}

// Режим работы
namespace UserMode {
	enum UserMode {
		umInterrogator = 0, // Запросчик
		umTransmitter = 1 // Приемоответчик
	};
}

// Конфигурация выхода
namespace OutputMode {
	enum OutputMode {
		omAntenna  = 0, // Ант
		omBeacon  = 1, // Маяк
		omCalibration   = 2 // Контрольный генератор
	};
}


// интерфейс удаленной системы для получения сообщений
class host_interface_t {
public:
public:
	// канал DME
	virtual void ChannelDMEChanged(u8 number, tSuffix::tSuffix suffix) = 0;
	// частота запроса, Гц
	virtual void RequestFrequency(u32 frequency) = 0;
	// число имп. пар в сек (запрос)
	virtual void RequestFrequencyZapr(u32 frequency) = 0;
	// число имп. пар в сек (ответ)
	virtual void RequestFrequencyOtv(u32 frequency) = 0;
	// Кодовый интервал запроса, нс
	virtual void RequestSpanChanged(u32 requestSpan) = 0;
	// Кодовый интервал ответа, нс
	virtual void ReplySpanChanged(u32 replySpan) = 0;
	// Уровень выходной мощности, dbm
	virtual void RFLevelChanged(s32 RFLevel) = 0;
	// формат запроса
	virtual void RequestFormChanged(tRequestForm::tRequestForm requestForm) = 0;
	// режим работы
	virtual void UserModeChanged(UserMode::UserMode uMode) = 0;
	// конфигурация входа
	virtual void OutputModeChanged(OutputMode::OutputMode uMode) = 0;
	// развертка по времени, мс
	virtual void resolution_x_changed(u32 v) = 0;
	// макс. уровень мощности, тысячные
	virtual void resolution_y_changed(u32 v) = 0;
	// смещение по оси x, мс
	virtual void offset_x_changed(u32 v) = 0;
};


// интерфейс "себя" для удаленной системы
class exported_interface_t {
public:

//	virtual void subscribe_host(host_interface_t *host) = 0;

	// канал DME
	virtual void set_dme_channel(u8 number, tSuffix::tSuffix suffix) = 0;
	// диапазон каналов DME
	virtual void set_dme_channel_range(u8 lo, u8 hi) = 0;
	// частота запроса, Гц
	virtual void set_request_frequency(u32 value) = 0;
	// диапазон частоты запроса, Гц
	virtual void set_request_frequency_range(u32 lo, u32 hi) = 0;
	// частота ответа, Гц
	virtual void set_response_frequency(u32 value) = 0;
	// Кодовый интервал запроса, нс
	virtual void set_request_span(u32 value) = 0;
	// диапазон Кодовый интервал запроса, нс
	virtual void set_request_span_range(u32 lo, u32 hi) = 0;
	// Кодовый интервал ответа, нс
	virtual void set_response_span(u32 value) = 0;
	// диапазон Кодовый интервал ответа, нс
	virtual void set_response_span_range(u32 lo, u32 hi) = 0;
	// число имп. пар в сек (запрос)
	virtual void set_request_frequency_zapr(u32 value) = 0;
	// диапазон число имп. пар в сек (запрос)
	virtual void set_request_frequency_zapr_range(u32 lo, u32 hi) = 0;
	// число имп. пар в сек (ответ)
	virtual void set_request_frequency_otv(u32 value) = 0;
	// диапазон число имп. пар в сек (ответ)
	virtual void set_request_frequency_otv_range(u32 lo, u32 hi) = 0;
	// Уровень выходной мощности, dbm
	virtual void set_request_level(s32 value) = 0;
	// диапазон Уровень  мощности запроса, dbm
	virtual void set_request_level_range(s32 lo, s32 hi) = 0;
	// Уровень мощности ответа, dbm
	virtual void set_response_level(s32 value) = 0;
	// эффективность, %
	virtual void set_efficiency(u8 percent) = 0;
//	virtual void set_code_reception_interval(u32 value) = 0;
	// число имп. пар в сек (ответ)
	virtual void set_response_delay(u32 value) = 0;
	// расстояние, м
	virtual void set_range(u32 value) = 0;
	// заряд батареи, %
	virtual void set_battery_status(u8 charge) = 0;
	// состояние устройства, 1 - передача, 0 - готов
	virtual void set_device_status(u8 value) = 0;
	// ксвн, тысячные
	virtual void set_ksvn(u32 value) = 0;
	// код CO
	virtual void set_co_code(const char * code, const char * aeroport) = 0;
	// статистика
	// ev_range, м
	// ev_delay, мкс
	// sigma, sko, 1/1000
	virtual void set_statistics(u32 ev_range, u32 ev_delay, u32 sigma, u32 sko) = 0;
	// обновить график
	// maxy - уровень макс. значения в выборке, 1/1000
	// yvalues - массив значений , 1/1000
	// length - длина выбрки, s32 слов
	// remain == 0
	virtual void update_chart(s32 maxy, s32 *yvalues, s32 length, s32 remain) = 0;
	// режим работы
	virtual void set_user_mode(UserMode::UserMode uMode) = 0;
	// формат запроса
	virtual void set_request_form(tRequestForm::tRequestForm requestForm) = 0;
	// конфигурация входа
	virtual void set_output_mode(OutputMode::OutputMode uMode) = 0;
	// смещение по оси x, мс
	virtual void set_offset_x(u32 v) = 0;
//	virtual void set_time_sweep(u32 timeSweep) = 0;
//	virtual void set_screen_saver(u8 screenSaver) = 0;
};

} // ns msg
} // ns

#endif
