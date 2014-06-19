
#include "ssd1963drv.h"

#include "surface.h"
#include "assert_impl.h"
#include "bmp_math.h"
#include "app_events.h"
#include "resources.h"
#include "widgets.h"

#include "ttcache.h"

#include "file_map.h"
#include "link.h"
#include "exported2_impl.h"

#include "uart_drv.h"
#include "devices.h"
#include "some_funcs.h"
#include "Spi.h"

#include <stdio.h>

#include "test_playground.h"



using namespace myvi;


extern "C" char* my_itoa(int i, char b[]);

class logger_impl_t : public logger_t {
public:
public:


    virtual logger_t& operator << (s32 v) OVERRIDE {
    	char ls[32];
    	my_itoa(v,ls);
    	printf(ls);
        return *this;
    }

    virtual logger_t& operator << (const char *v) OVERRIDE {
		if (v == _endl)
			printf("\n");
		else
			printf(v);
        return *this;
    }

};

logger_impl_t logger_impl;

logger_t *logger_t::instance = &logger_impl;

extern resources_t res;

ssd1963drv_t drv1;
Spi spi;
FRAM fram;
FlashDev flash;
uart_drv_t	uart;


void drawScene(gobject_t *obj, surface_t &s1) {
	static key_t::key_t last_key = (key_t::key_t)0;
	key_t::key_t key = kbd_get_key();

	focus_aware_t * focus_aware = dynamic_cast<focus_aware_t*>(obj);
	_MY_ASSERT(focus_aware, return);

	if (key != last_key) {
		last_key = key;
		focus_aware->key_event((key_t::key_t)key);
	}
	s16 enc_cnt = enc_reset_counter();
	while (enc_cnt) {
		if (enc_cnt > 0) {
			enc_cnt--;
			focus_aware->key_event(key != key_t::K_SAVE ? key_t::K_RIGHT : key_t::K_DOWN);
		} else if (enc_cnt < 0) {
			enc_cnt++;
			focus_aware->key_event(key != key_t::K_SAVE ? key_t::K_LEFT : key_t::K_UP);
		}
	}
	if (rasterizer_t::render(obj, s1))
		s1.copy_to(0,0,-1,-1,0,0,drv1);
}


void my_main() {



    spi.Init();
	kbd_init();
	enc_init();

	drv1.init();

    _WEAK_ASSERT(read_file_table(),);

	s32 buf_sz = BMP_GET_SIZE_16(TFT_WIDTH,TFT_HEIGHT);
	u8 *buf0 = new u8[buf_sz];
	surface_16bpp_t s1(TFT_WIDTH,TFT_HEIGHT,buf_sz, buf0);


    u8 *ttcache_dat;
    u32 ttcache_sz;

    _WEAK_ASSERT(allocate_and_read_font_cache(ttcache_dat,ttcache_sz),);

    if (!ttcache_dat || !ttcache_sz) {
    	while(1) asm("ESTOP0");
    }
	globals::ttcache.init((u8 *)ttcache_dat,ttcache_sz);

	res.init();

	test_screen_t *screen1 = new test_screen_t();
	screen1->init();
	screen1->dirty = true;

	init_pie_table();

	while (1) {
		drawScene(screen1, s1);
	}

//	_ASSERT(0, return 0);

//	return 0;
}
