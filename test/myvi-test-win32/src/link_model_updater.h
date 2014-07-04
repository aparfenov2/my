/*
 *  Реализация ответной части интерфейса связи системного уровня (обновление модели)
*   Работает в цепочке с реализацией чтения\записи файлов
 *
 *  Created on: 26.04.2014
 *      Author: Администратор
 */


#ifndef _EXPORTED_SYS_MODEL_IMPL_H
#define _EXPORTED_SYS_MODEL_IMPL_H

#include "exported_sys.h"
#include "custom_views.h"

namespace custom {

// 1. обновляет модель по приходу сообщений по линии связи
// 2. подпиcывается на обновления модели и отправляет их в линию связи
	
class link_model_updater_t : public myvi::exported_interface2_t, public myvi::subscriber_t<custom::model_message_t>  {
public:
	myvi::host_interface2_t *host2;
	myvi::exported_interface2_t *chained; // optional
public:
	link_model_updater_t(myvi::host_interface2_t *_host2, myvi::exported_interface2_t *_chained) {
		host2 = _host2;
		chained = _chained;
		custom::dynamic_model_t::instance().subscribe(this);
	}

	// собоытия обновления модели
	virtual void accept (custom::model_message_t &msg) OVERRIDE {
	}

	// запрос на чтение данных из модели
	virtual void read_model_data(char * path) OVERRIDE {
	}

	// запрос на запись данных в модель
	virtual void write_model_data(char * path, char * string_value) OVERRIDE {
	}

	virtual void write_model_data(char * path, s32 int_value) OVERRIDE {
	}

	virtual void write_model_data(char * path,  double float_value) OVERRIDE {
	}


	// остальные методы интерфейса связи передадим по цепочке
	virtual void key_event(myvi::key_t::key_t key) OVERRIDE {
		if (chained) chained->key_event(key);
	}

	virtual void upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) OVERRIDE {
		if (chained) chained->upload_file(file_id,offset,crc,first,data,len );
	}

	virtual void download_file(u32 file_id, u32 offset, u32 length) OVERRIDE {
		if (chained) chained->download_file(file_id,offset,length);
	}

	virtual void update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) OVERRIDE {
		if (chained) chained->update_file_info(file_id,cur_len,max_len, crc);
	}

	virtual void read_file_info(u32 file_id) OVERRIDE {
		if (chained) chained->read_file_info(file_id);
	}



};

}

#endif