#ifndef _TTOLS_H
#define _TTOLS_H

#include "surface.h"
#include "app_events.h"
#include "disp_def.h"

namespace myvi {

class test_drawer_t {
public:
	u32 w;
	u32 h;

public:
	test_drawer_t() {
		w = TFT_WIDTH, h = TFT_HEIGHT;
	}
	// return : if true - update
	virtual bool callback(key_t::key_t key, s32 mx, s32 my, mkey_t::mkey_t mkey) {
		return false;
	}
	bool cycle(surface_t &s);
	void create_window(surface_t &s);
};

} // ns
#endif
