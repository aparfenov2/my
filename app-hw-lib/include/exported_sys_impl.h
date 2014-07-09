/*
 *  Реализация ответной части интерфейса связи системного уровня (чтение\запись файлов)
*   Работает в цепочке с реализацией обновления модели
 *
 *  Created on: 26.04.2014
 *      Author: Администратор
 */

#ifndef EXPORTED2_IMPL_H_
#define EXPORTED2_IMPL_H_

#include "exported_sys.h"

namespace myvi {
class exported_interface2_impl_t : public exported_interface2_t  {
public:
	host_interface2_t *host;
	exported_interface2_t *chained;
public:
	exported_interface2_impl_t(host_interface2_t *ahost, exported_interface2_t *_chained OPTIONAL) {
		host = ahost;
		chained = _chained;
	}

	// запрос на чтение данных из модели
	virtual void read_model_data(char * path) OVERRIDE {
		if (chained) chained->read_model_data(path);
	}

	// запрос на запись данных в модель
	virtual void write_model_data(char * path, char * string_value) OVERRIDE {
		if (chained) chained->write_model_data(path, string_value);
	}

	virtual void write_model_data(char * path, s32 int_value) OVERRIDE {
		if (chained) chained->write_model_data(path, int_value);
	}

	virtual void write_model_data(char * path,  double float_value) OVERRIDE {
		if (chained) chained->write_model_data(path, float_value);
	}


	void key_event(key_t::key_t key) OVERRIDE;
	void upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) OVERRIDE;
	void download_file(u32 file_id, u32 offset, u32 length) OVERRIDE;
	void update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) OVERRIDE;
	void read_file_info(u32 file_id) OVERRIDE;

}; // class

} // ns myvi

bool read_file_table();
bool allocate_and_read_font_cache(u8 *&buf, u32 &sz);


#endif /* EXPORTED2_IMPL_H_ */
