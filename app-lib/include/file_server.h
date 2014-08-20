/*
 * file_server.h
 *
 *  Created on: 15.08.2014
 *      Author: user
 */

#ifndef FILE_SERVER_H_
#define FILE_SERVER_H_

#include "exported_sys.h"
#include "file_system.h"

#define _FILE_SERVER_DATABUF_LENGTH 512

namespace app {

class file_server_t :
	public link::exported_system_interface_t,
	public link::exported_file_interface_t {
public:
	link::host_file_interface_t *host;
	file_system_t *fs;
	u8 data_buf[_FILE_SERVER_DATABUF_LENGTH];
public:
	file_server_t() {
		host = 0;
		fs = 0;
//		data_buf = 0;
	}

	void init(link::host_file_interface_t *_host, file_system_t *_fs) {
		host = _host;
		fs = _fs;
		//data_buf = new u8[_FILE_SERVER_DATABUF_LENGTH];
	}

	// записать данные файла
	// file_id - дескриптор
	// offset - смещение от начала файла, байт
	// crc - опционально, crc
	// first - признак первой записи из серии
	// data - указатель на массив передаваемых данных
	// len - длина массива
	virtual void upload_file(u32 file_id, u32 offset, bool first, u8* data, u32 len) OVERRIDE {
		u32 written;
		if (fs->write_file(file_id, offset, len, data, written )) {
			host->error(0);
		} else {
			host->error(1);
		}
	}
	// запросить данные файла
	// file_id - дескриптор
	// offset - смещение от начала файла, байт
	// length - длина, байт
	virtual void download_file(u32 file_id, u32 offset, u32 length) OVERRIDE {
		u32 read;

		if (length < _FILE_SERVER_DATABUF_LENGTH) {
			if (fs->read_file(file_id,offset,length,data_buf, read)) {
				host->download_response(file_id,offset,false,data_buf,read);
			} else {
				host->error(1);
			}
		} else {
			host->error(2);
		}
	}
	// обновить информацию в таблице файлов
	// file_id - дескриптор
	// cur_len - текущая длина файла, байт
	// max_len - максимально допустимая длина файла, байт
	// crc - если != 0
	virtual void update_file_info(u32 file_id, u32 cur_len) OVERRIDE {
		if (fs->set_info(file_id,cur_len)) {
			host->error(0);
		} else {
			host->error(1);
		}
	}
	// запросить информацию о файле
	// file_id - дескриптор
	virtual void read_file_info(u32 file_id) OVERRIDE {
		u32 len,lmax;
		if (fs->get_info(file_id,len,lmax)) {
			host->file_info_response(file_id,len,lmax);
		} else {
			host->error(1);
		}
	}

};
} // ns app

#endif /* FILE_SERVER_H_ */
