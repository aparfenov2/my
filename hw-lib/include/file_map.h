/*
 * file_map.h
 *
 *  Created on: 16.04.2014
 *      Author: Администратор
 */

#ifndef FILE_MAP_H_
#define FILE_MAP_H_

#include "types.h"

namespace file_source_t {
	typedef enum {
		FLASH,
		FRAM
	} file_source_t;
}

typedef struct {
	u32 file_id;
	file_source_t::file_source_t file_source;
	u32 offset;
	u32 cur_len;
	u32 max_len;
	u32 crc;
} file_rec_t;

#define FILE_TABLE_MAGIC 0xabba
#define FILE_TABLE_ADDR 0x000000

#define TTCACHE_FILE_ID 0x02
#define LOGO_FILE_ID 0x03
#define FILE_TABLE_SIZE 4
extern file_rec_t file_table[FILE_TABLE_SIZE];

file_rec_t * find_file(u32 id);


#endif /* FILE_MAP_H_ */
