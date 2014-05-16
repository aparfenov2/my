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
#include <fstream>

#include "disp_def.h"


using namespace std;
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
		cout << "set size to " << w << "x" << h << endl;
	}

	virtual bool callback(key_t::key_t key, s32 mx, s32 my, mkey_t::mkey_t mkey) OVERRIDE {
		if (key == key_t::K_SAVE) {
		}
		if (key) {
			gobj->key_event((key_t::key_t)key);
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
		cout << v;
		log << v;
//		log.flush();
        return *this;
    }

    virtual logger_t& operator << (const char *v) OVERRIDE {
		if (v == _endl) {
			cout << endl;
			log << endl;
		} else {
			cout << v;
			log << v;
		}
//		log.flush();
        return *this;
    }

};

logger_impl_t logger_impl("log.log"); 
logger_t *logger_t::instance = &logger_impl;


// Строка из текстбокса и комбобокса с суффиксами

class tedit_t : public inp_dlg_row_t {
public:

	virtual void do_layout() OVERRIDE {
		_MY_ASSERT(w && h, return);

		lval.w = w * 0.7;
		lval.h = h;
		lsfx.h = h;
		lsfx.x = lval.w + 5;
		lsfx.w = w - lsfx.x;

		layout_children();
	}
};

// строка из названия и tedit-a справа
class menu_row_t :  public gobject_t {
public:
	label_t lname;
	tedit_t valbox;
public:

	virtual void init() {
		gobject_t::init();

		menu_context_t &ctx = menu_context_t::instance;

		lname.visible = true;
		lname.ctx = ctx.lctx1;
		lname.ctx.font_size = font_size_t::FS_20;

		//_MY_ASSERT(parent,return);
		//parent->capture_key_events(this);
	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		_MY_ASSERT(parent,return);
		parent->dirty = dirty;
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lname;
		else if (prev == &lname) return &valbox;
		return 0;
	}

	virtual void do_layout() OVERRIDE {
		_MY_ASSERT(w && h, return);
		lname.w = w * 0.7;
		lname.h = h;
		valbox.h = h;
		valbox.x = lname.w + 5;
		valbox.w = w - valbox.x;

		layout_children();
	}


};


// весь экран
class test_screen_t : public gobject_t {

public:
	class suffixes_t : public iterator_t<string_t> {
	public:
		string_t str1;
		string_t str2;
	public:
		suffixes_t() {
			str1 = string_t("A");
			str2 = string_t("B");
		}

		virtual string_t* next(void* prev) OVERRIDE {
			if (!prev) return &str1;
			if (prev == &str1) return &str2;
			return 0;
		}

	};

public:
	tedit_t hdr_box;
	menu_row_t dlg_row;

	suffixes_t suffixes;

public:
	test_screen_t() {
	}

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

		menu_context_t::instance.bctx1 = bctx1;
		menu_context_t::instance.lctx1 = lctx1;
		menu_context_t::instance.lctxg = lctxg;

		hdr_box.x = hdr_box.y = 0;
		hdr_box.w = w;
		hdr_box.h = 20;
		hdr_box.lval.value = "Helo!";
		hdr_box.lsfx.values = &suffixes;
		hdr_box.lsfx.value = *suffixes.next(0);


		dlg_row.x = 0;
		dlg_row.y = 40;
		dlg_row.w = w;
		dlg_row.h = 20;
		dlg_row.valbox.lval.value = "Helo!";
		dlg_row.valbox.lsfx.values = &suffixes;
		dlg_row.valbox.lsfx.value = *suffixes.next(0);
		dlg_row.lname.text = "Menu_name";



		init_children();

		do_layout();
		dirty = true;

	}

	//virtual void key_event(key_t::key_t key) OVERRIDE {
	//}

	void mouse_event(s32 mx, s32 my, mkey_t::mkey_t mkey);

	virtual void render(surface_t &dst) OVERRIDE {
		// синий фон
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0xf9f9f9;//0x203E95;
		dst.fill(0,0,w,h);

	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &hdr_box;
		if (prev == &hdr_box) return &dlg_row;
		return 0;
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