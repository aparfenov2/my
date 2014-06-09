#include <tchar.h>
#include "surface.h"

#include "bmp_math.h"
#include "assert_impl.h"

#include "test_tools.h"
#include "resources.h"
#include "widgets.h"

#include "menu_common.h"

#include "ttcache.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include "disp_def.h"

#include "generator_common.h"
#include "generated_meta.h"
#include "custom_views.h"


using namespace myvi;

u8 buf0[BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16)];
surface_16bpp_t s1(TFT_WIDTH,TFT_HEIGHT,BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16), buf0);

extern resources_t res;



class my_test_drawer_t : public test_drawer_t {
public:
	s32 kx;
	s32 ky;
	gobject_t *gobj;
public:
	my_test_drawer_t(gobject_t *agobj) {
		gobj = agobj;
		w = TFT_WIDTH * 2;
		h = TFT_HEIGHT * 2;
		kx = 2;
		ky = 2;
		std::cout << "set size to " << w << "x" << h << std::endl;
	}

	virtual bool callback(key_t::key_t key, s32 mx, s32 my, mkey_t::mkey_t mkey) OVERRIDE {
		if (key == key_t::K_SAVE) {
		}
		if (key) {
			focus_aware_t * focus_aware = dynamic_cast<focus_aware_t*>(gobj);
			if (focus_aware) {
				focus_aware->key_event((key_t::key_t)key);
			}
		}
		return rasterizer_t::render(gobj, s1);
	}

};


class logger_impl_t : public logger_t {
public:
	std::ofstream log;
public:

	logger_impl_t(std::string path):log(path) {
	}

    virtual logger_t& operator << (s32 v) OVERRIDE {
		std::cout << v;
		log << v;
//		log.flush();
        return *this;
    }

    virtual logger_t& operator << (const char *v) OVERRIDE {
		if (v == _endl) {
			std::cout << std::endl;
			log << std::endl;
		} else {
			std::cout << v;
			log << v;
		}
//		log.flush();
        return *this;
    }

};

logger_impl_t logger_impl("log.log"); 
logger_t *logger_t::instance = &logger_impl;






// ------------------------------------------------------------
// ------------ GENERATOR PLAYGROUND -----------------
// ------------------------------------------------------------



// ------------------------------- весь экран ------------------------------------
class test_screen_t : public gobject_t, public focus_aware_t {

public:
	tedit_t hdr_box;
	scrollable_menu_t scrollable;
	gen::suffixes_t suffixes;


public:

	virtual void init() OVERRIDE {

		w = TFT_WIDTH;
		h = TFT_HEIGHT;

		button_context_t bctx1;
		bctx1.bk_sel_color = 0x292929; // gray
		bctx1.bk_color = 0x203E95; // blue

		label_context_t lctx1;
		lctx1.sctx.pen_color = 0x010101;
		lctx1.font = &res.ttf;
		lctx1.font_size = font_size_t::FS_8;

		label_context_t lctxg;
		lctxg.sctx.pen_color = 0x010101;
		lctxg.font = &res.gly;
		lctxg.font_size = font_size_t::FS_30;

		menu_context_t::instance().bctx1 = bctx1;
		menu_context_t::instance().lctx1 = lctx1;
		menu_context_t::instance().lctxg = lctxg;

		//gen::view_meta_t *root_view_meta = gen::meta_registry_t::instance().find_view_meta("root");
		//gobject_t *menu = view_factory_t::build_view(root_view_meta);

		//add_child(menu);

		//menu->x = 0;
		//menu->y = 0;
		//menu->w = w;
		//menu->h = h;


		hdr_box.x = 0;
		hdr_box.y = 0;
		hdr_box.w = w;
		hdr_box.h = 20;

		hdr_box.lval.value = "Helo!";
		hdr_box.lsfx.values = &suffixes;
		hdr_box.lsfx.value = *suffixes.next(0);

		add_child(&hdr_box);

		scrollable.x = 0;
		scrollable.y = 20;
		scrollable.w = w;
		scrollable.h = h/4;

		add_child(&scrollable);


		init_children();

		do_layout();
		dirty = true;

	}

	virtual void render(surface_t &dst) OVERRIDE {
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0xf9f9f9;//0x203E95;
		s32 ax,ay;
		translate(ax,ay);
		dst.fill(ax,ay,w,h);

	}


};



int _tmain(int argc, _TCHAR* argv[]) {

	_LOG1("log started");

// init ttcache
	ttcache_t::init_lib();
	res.init();

	test_screen_t test_screen;
	test_screen.init(); // init whole tree

	my_test_drawer_t test_drawer(&test_screen);
	test_drawer.create_window(s1);

	bool exit = false;
	while (!exit) {
		exit = test_drawer.cycle(s1);
	}

	return 0;
}