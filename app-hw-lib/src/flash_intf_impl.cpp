/*
 * flash_intf_impl.cpp
 *
 *  Created on: 29.08.2014
 *      Author: user
 */


#include "flash_intf_impl.h"
#include "assert_impl.h"
#include "flash.h"

#include <string.h>

extern "C" {
#include "hdlc.h"
}


using namespace link;
using namespace hw;

#define CRC_SEED 0xabba

extern "C" u16_t crc16_ccitt_calc_data(u16_t crc, u8_t* data, u16_t data_length);
extern "C" u32 pack(const u8 *data, u32 sz, void *dst);

static section_t *first = 0;
extern "C" void flash_it(section_t *first);

void flash_intf_impl_t::define_section (u32 key, u32 len, u32 flash_addr, u32 crc) {
	_MY_ASSERT(host_intf, return);
	section_t *prev = first;
	first = new section_t();
	first->key = key;
	first->next = prev;
	first->len = len;
	first->curr_offs = 0;
	first->crc = crc;
	first->flash_addr = flash_addr;
	first->data = new u8[len];
	host_intf->define_section_ack(first->data ? 0 : 1);
}

void flash_intf_impl_t::post_sec_data (u32 key, u32 len, u32 remain, u8 * data)  {
	_MY_ASSERT(host_intf, return);
	do {
		_WEAK_ASSERT(first && first->data && key == first->key, break);
		std::memmove(first->data + first->curr_offs, data, len);
		first->curr_offs += len;
		if (!remain) {
			u32 crc = crc16_ccitt_calc_data(CRC_SEED, first->data, first->len );
			_WEAK_ASSERT(crc == first->crc, break);
			u8 *oldp = first->data;
			first->data = new u8[first->len / 2];
			first->len = pack(oldp, first->len, first->data);
			delete oldp;
		}
		host_intf->post_sec_data_ack(0);
		return;
	} while (false);
	host_intf->post_sec_data_ack(1);
}

static section_t* reverse(section_t *root) {
	section_t* new_root = 0;
	while (root) {
		section_t* next = root->next;
		root->next = new_root;
		new_root = root;
		root = next;
	}
	return new_root;
}

void flash_intf_impl_t::transfer_complete (u32 transfer_complete) {
	host_intf->transfer_complete_accepted(0);
	first = reverse(first);
	flash_it(first);
}
