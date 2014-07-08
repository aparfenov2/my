/*
 * Описание реализации интерфейса связи системного уровня
 *
 *  Created on: 12.03.2013
 *      Author: gordeev
 */

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include "exported_sys.h"

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
class serializer_t : public host_system_interface_t, public serial_data_receiver_t, public _internal_frame_receiver_t {

private:
	exported_system_interface_t *exported2;
	serial_interface_t *sintf;
public:
	serializer_t() {
		sintf = 0;
	}

	void init(exported_system_interface_t *aexported2, serial_interface_t *asintf );

// ----------------------- public host_system_interface_t (туда)------------------------------------

	// ответ на запрос на чтение данных из модели
	virtual void read_model_data_response(char * path, char * string_value, u32 code) OVERRIDE;
	virtual void read_model_data_response(char * path, s32 int_value, u32 code ) OVERRIDE;
	virtual void read_model_data_response(char * path, double float_value, u32 code ) OVERRIDE;

	// ответ на запись данных в модель
	virtual void write_model_data_ack(u32 code) OVERRIDE;

	void download_response(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) OVERRIDE;
	void file_info_response(u32 file_id, u32 cur_len, u32 max_len, u32 crc)  OVERRIDE;
	void error(u32 code) OVERRIDE;

// ------------------- public serial_data_receiver_t --------------------
	// прием блока данных из интерфеса UART
	void receive(u8 *data, u32 len) OVERRIDE;
	// прием фрейма данных
	void receive_frame(u8 *data, u32 len) OVERRIDE;

}; // class


//сериализер на стороне хоста
class host_serializer_t : public exported_system_interface_t, public serial_data_receiver_t, public _internal_frame_receiver_t {

private:
	host_system_interface_t *host2;
	serial_interface_t *sintf;
public:
	host_serializer_t() {
		sintf = 0;
	}

	// ahost2 - реализация ответной части системного интерфейса на стороне хоста
	// aintf - реализация интерфейса последовательной передачи данных 
	void init(host_system_interface_t *ahost2, serial_interface_t *asintf );
// ------------------------ public exported_system_interface_t ----------------------------

	// запрос на чтение данных из модели
	virtual void read_model_data(char * path) OVERRIDE;

	// запрос на запись данных в модель
	virtual void write_model_data(char * path, char * string_value) OVERRIDE;
	virtual void write_model_data(char * path, s32 int_value) OVERRIDE;
	virtual void write_model_data(char * path,  double float_value) OVERRIDE;

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
