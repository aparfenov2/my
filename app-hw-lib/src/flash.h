/*
 * flash.h
 *
 *  Created on: 29.08.2014
 *      Author: user
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "types.h"

typedef struct section_t {
	struct section_t *next;
	u32 key;
	u32 len;
	u32 flash_addr;
	u32 curr_offs;
	u32 crc;
	u8 *data;
} section_t;


#endif /* FLASH_H_ */
