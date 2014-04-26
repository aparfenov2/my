/*
 * file_map.cpp
 *
 *  Created on: 17.04.2014
 *      Author: Администратор
 */

#include "file_map.h"

file_rec_t file_table[FILE_TABLE_SIZE] = {
		{FILE_TABLE_MAGIC,file_source_t::FLASH,FILE_TABLE_ADDR,0,0x010000,0}, // file table
		{TTCACHE_FILE_ID,file_source_t::FLASH,0x010000,0,0x040000,0}, // font cache
		{LOGO_FILE_ID,file_source_t::FLASH,0x050000,0,0x060000,0}, // startup bitmap
		{0x00,file_source_t::FLASH,0,0,0,0}
};


file_rec_t * find_file(u32 id) {
	file_rec_t *p = &file_table[0];
	while (p->file_id) {
		if (p->file_id == id) {
			return p;
		}
		p++;
	}
	return 0;
}
