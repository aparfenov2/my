/*
 * options.cpp
 *
 *  Created on: 27.08.2014
 *      Author: user
 */


#include "options.h"
#include "assert_impl.h"

using namespace hw;

options_t options_t::_instance;

struct opt_rec_t {
	u32 key;
	u16 addr;
};

opt_rec_t options_map[] = {
		{OPT_LANG, 0x0000},
		{0, 0x0000}
};

static opt_rec_t * find_rec(u32 key) {
	for (opt_rec_t *p = options_map; p; p++) {
		if (p->key == key) {
			return p;
		}
	}
	return 0;
}

void options_t::get_int_value(u32 key, u32 &val) {
	_MY_ASSERT(fram, return);
	val = 0;
	opt_rec_t *rec = find_rec(key);
	_WEAK_ASSERT(rec, return);
	fram->Read(rec->addr, (s32 *)&val);
}

void options_t::set_int_value(u32 key, u32 val) {
	_MY_ASSERT(fram, return);
	opt_rec_t *rec = find_rec(key);
	_WEAK_ASSERT(rec, return);
	fram->Write(rec->addr,(s32) val);
}
