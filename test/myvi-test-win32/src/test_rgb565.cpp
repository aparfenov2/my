#include <tchar.h>
#include <windows.h>
#include <iostream>

#include "surface.h"
#include "bmp_math.h"
#include "assert_impl.h"
#include "test_tools.h"
#include "resources.h"
#include "ttcache.h"
#include "widgets.h"
#include "disp_def.h"


using namespace std;
using namespace myvi;

class logger_impl_t : public logger_t {
public:
public:

    virtual logger_t& operator << (s32 v) OVERRIDE {
		cout << v;
        return *this;
    }

    virtual logger_t& operator << (const char *v) OVERRIDE {
		if (v == _endl)
			cout << endl;
		else
			cout << v;
        return *this;
    }

};

logger_impl_t logger_impl; 
logger_t *logger_t::instance = &logger_impl;


u8 buf0[BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16)];
surface_16bpp_t s565(TFT_WIDTH,TFT_HEIGHT,BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16), buf0);

u8 buf1[BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,24)];
surface_24bpp_t s24(TFT_WIDTH,TFT_HEIGHT,BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,24), buf1);


class my_test_drawer_t : public test_drawer_t {
public:
	s32 kx;
	s32 ky;
public:
	my_test_drawer_t() {
		w = TFT_WIDTH * 2;
		h = TFT_HEIGHT * 2;
		kx = 2;
		ky = 2;
	}

};

my_test_drawer_t test_drawer;
extern resources_t res;

void print_chars(ttype_font_t &fnt, surface_t &s1, const char * chars) {
	fnt.set_char_size_px(0,font_size_t::FS_15);
	fnt.print_to(0,0,s1,string_t(chars));
}


int _tmain(int argc, _TCHAR* argv[]) {

	ttcache_t::init_lib();
	res.init();

	surface_t *s = &s565;
	s->ctx.pen_color = 0x203E95;
//	s->ctx.pen_color = 0x121415;
	s->set_allowed_area(0,0,320,240);

//	s16.line(0,0,100,false);
	s->fill(0,0,320,240);
	print_chars(res.ttf,*s,"abcdefghijklmnopqrstuvwxyz");

	test_drawer.plot_surface(*s);
	return 0;
}

