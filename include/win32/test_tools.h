#ifndef _TTOLS_H
#define _TTOLS_H

#include "surface.h"
#include "app_events.h"
namespace myvi {

class test_drawer_t {
public:
	u32 w;
	u32 h;
public:
	test_drawer_t() {
		w = 320, h = 240;
	}
	// return : if true - update
	virtual bool callback(key_t::key_t key, s32 mx, s32 my, mkey_t::mkey_t mkey) {
		return false;
	}
	virtual void tick() {
	}
	virtual void cycle() {
	}
	void plot_surface(surface_t &s);
};

} // ns
#endif
