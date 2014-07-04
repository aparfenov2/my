/*
* Определение интерфейса связи системного уровня для прямой и обратной связи с этой системой
*/

#ifndef _MESSAGES_SYS_H
#define _MESSAGES_SYS_H


#include "types.h"
#include "app_events.h"


namespace myvi {

// технологические расширения интрефейса связи

class host_interface2_t {
public:

	// ответ на запрос на чтение данных из модели
	virtual void read_model_data_response(char * path, char * string_value, u32 code) = 0;
	virtual void read_model_data_response(char * path, s32 int_value, u32 code ) = 0;
	virtual void read_model_data_response(char * path, double float_value, u32 code ) = 0;

	// ответ на запись данных в модель
	virtual void write_model_data_ack(u32 code) = 0;

	// принимаемые данные файла (ответ запроса на скачивание)
	// file_id - дескриптор
	// offset - смещение от начала файла, байт
	// crc - опционально, crc
	// first - признак первой записи из серии
	// data - указатель на массив принимаемых данных
	// len - длина массива
	virtual void download_response(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) = 0;
	// принимаемая запись из таблицы файлов
	// file_id - дескриптор
	// cur_len - текущая длина файла, байт
	// max_len - максимально допустимая длина файла, байт
	// crc - если != 0
	virtual void file_info_response(u32 file_id, u32 cur_len, u32 max_len, u32 crc) = 0;
	// возвращаемый код ошибки неудачной файловой операции
	virtual void error(u32 code) = 0;
};


class exported_interface2_t {
public:

	// запрос на чтение данных из модели
	virtual void read_model_data(char * path) = 0;

	// запрос на запись данных в модель
	virtual void write_model_data(char * path, char * string_value) = 0;
	virtual void write_model_data(char * path, s32 int_value) = 0;
	virtual void write_model_data(char * path,  double float_value) = 0;

	// событие клавиатуры
	virtual void key_event(key_t::key_t key) = 0;
	// записать данные файла
	// file_id - дескриптор
	// offset - смещение от начала файла, байт
	// crc - опционально, crc
	// first - признак первой записи из серии
	// data - указатель на массив передаваемых данных
	// len - длина массива
	virtual void upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) = 0;
	// запросить данные файла
	// file_id - дескриптор
	// offset - смещение от начала файла, байт
	// length - длина, байт
	virtual void download_file(u32 file_id, u32 offset, u32 length) = 0;
	// обновить информацию в таблице файлов
	// file_id - дескриптор
	// cur_len - текущая длина файла, байт
	// max_len - максимально допустимая длина файла, байт
	// crc - если != 0
	virtual void update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) = 0;
	// запросить информацию о файле
	// file_id - дескриптор
	virtual void read_file_info(u32 file_id) = 0;
};

} // ns

#endif
