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
#include "custom_common.h"

namespace custom {

// 1. обновляет модель по приходу сообщений по линии связи
// 2. подпиcывается на обновления модели и отправляет их в линию связи
	
class link_model_updater_t : 
	public link::exported_system_interface_t, 
	public link::exported_model_interface_t, 
	public myvi::subscriber_t<custom::model_message_t>  {
public:
	link::host_model_interface_t *host2;
	bool allowed_respond;
public:
	link_model_updater_t() {
		host2 = 0;
		allowed_respond = true;
	}

	void init(link::host_model_interface_t *_host2) {
		host2 = _host2;
		model_t::instance()->subscribe(this);
	}

	void respond(myvi::string_t path, variant_t value) {
		if (!allowed_respond) return;

		switch (value.type) {
		case variant_type_t::STRING:
			host2->read_model_data_response(path.c_str(),0, value.get_string_value().c_str());
			break;
		case variant_type_t::INT:
			host2->read_model_data_response(path.c_str(),0, value.get_int_value());
			break;
		case variant_type_t::FLOAT:
			host2->read_model_data_response(path.c_str(),0, value.get_float_value());
			break;
		}
	}

	// собоытия обновления модели
	virtual void accept (custom::model_message_t &msg) OVERRIDE {
		respond(msg.path, msg.value);
	}

	// запрос на чтение данных из модели
	virtual void read_model_data(const char * path) OVERRIDE {
		variant_t value;
		model_t::instance()->read(path, value);
		respond(myvi::string_t(path), value);
	}

	// запрос на запись данных в модель
	virtual void write_model_data(const char * path, const char * string_value) OVERRIDE {
		variant_t value(string_value);
		allowed_respond = false;
		model_t::instance()->update(path, value);
		allowed_respond = true;
	}

	virtual void write_model_data(const char * path, s32 int_value) OVERRIDE {
		variant_t value(int_value);
		allowed_respond = false;
		model_t::instance()->update(path, value);
		allowed_respond = true;
	}

	virtual void write_model_data(const char * path,  double float_value) OVERRIDE {
		variant_t value(float_value);
		allowed_respond = false;
		model_t::instance()->update(path, value);
		allowed_respond = true;
	}


};


// оповещает по сети об изменениях модели
class link_model_repeater_t : 
	public link::host_system_interface_t,
	public link::host_model_interface_t, 
	public myvi::subscriber_t<custom::model_message_t> {
public:
	link::exported_model_interface_t *exported_sys;
	bool allowed_respond;
public:
	link_model_repeater_t() {
		exported_sys = 0;
		allowed_respond = true;
	}

	void init(link::exported_model_interface_t *_exported_sys) {
		exported_sys = _exported_sys;
		model_t::instance()->subscribe(this);
	}


	void broadcast(myvi::string_t path, variant_t value) {
		if (!allowed_respond) return;

		switch (value.type) {
		case variant_type_t::STRING:
			exported_sys->write_model_data(path.c_str(), value.get_string_value().c_str());
			break;
		case variant_type_t::INT:
			exported_sys->write_model_data(path.c_str(), value.get_int_value());
			break;
		case variant_type_t::FLOAT:
			exported_sys->write_model_data(path.c_str(), value.get_float_value());
			break;
		}
	}

	// собоытия обновления модели
	virtual void accept (custom::model_message_t &msg) OVERRIDE {
		broadcast(msg.path, msg.value);
	}

	// ответ на запрос на чтение данных из удалённой модели
	virtual void read_model_data_response(const char * path, u32 code, const char * string_value) OVERRIDE {
		variant_t value(string_value);
		allowed_respond = false;
		model_t::instance()->update(path, value);
		allowed_respond = true;
	}

	virtual void read_model_data_response(const char * path, u32 code, s32 int_value ) OVERRIDE {
		variant_t value(int_value);
		allowed_respond = false;
		model_t::instance()->update(path, value);
		allowed_respond = true;
	}

	virtual void read_model_data_response(const char * path, u32 code, double float_value ) OVERRIDE {
		variant_t value(float_value);
		allowed_respond = false;
		model_t::instance()->update(path, value);
		allowed_respond = true;
	}


	// ответ на запись данных в удаленную модель
	virtual void write_model_data_ack(u32 code) OVERRIDE {
	}


};

} // ns

#endif