extern "C" {
#include "sys/alt_stdio.h"
}

#include <io.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
//#include "ssd1963.h"
#include "ssd1963drv.h"

#include "surface.h"
//#include "truetype.h"
#include "assert_impl.h"
#include "bmp_math.h"
#include "app_events.h"
#include "screen_1.h"
#include "resources.h"
#include "widgets.h"

//#include "lena_320.h"
//#include "ARIALNI.h"

#include "ttcache.h"
#include "ttcache_dat.h"

#include "host_emu.h"

using namespace myvi;

extern "C" char* my_itoa(int i, char b[]);

class logger_impl_t : public logger_t {
public:
public:

    virtual logger_t& operator << (u32 &v) OVERRIDE {
    	char ls[32];
    	my_itoa(v,ls);
    	alt_putstr(ls);
        return *this;
    }

    virtual logger_t& operator << (s32 &v) OVERRIDE {
    	char ls[32];
    	my_itoa(v,ls);
    	alt_putstr(ls);
        return *this;
    }

    virtual logger_t& operator << (const char *v) OVERRIDE {
		if (v == _endl)
			alt_putstr("\n");
		else
	    	alt_putstr(v);
        return *this;
    }

    virtual logger_t& operator << (const string_t &v) OVERRIDE {
    	alt_putstr(v.c_str());
        return *this;
    }

};

logger_impl_t logger_impl;

logger_t *logger_t::instance = &logger_impl;


#define KEY_TOGGLE_HW 12 // KEY3

u32 getKey() {
	u32 key = ~IORD_ALTERA_AVALON_PIO_DATA(PIO_KEY_BASE);
	return key;
}

#define COL_a 0
#define COL_b 1
#define COL_c 2
#define COL_d 3
#define COL_e 4

key_t::key_t getAppKey() {
	key_t::key_t ret = (key_t::key_t)0;
	for (int row=0; row<4; row++) {
		// write row
		u32 led = IORD_ALTERA_AVALON_PIO_DATA(PIO_LED_BASE);
		u8 row_msk = (1 << row);
		// clear other rows
		u32 led_msk = (((row_msk) & 0x0f) << 8);
		led &= ~led_msk;
		// set selected row
		led_msk = (((~row_msk) & 0x0f) << 8);
		led |= led_msk;
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_LED_BASE, led);
		// read col
		u32 key = ~IORD_ALTERA_AVALON_PIO_DATA(PIO_KEY_BASE);
		u8 col_res = (key >> 4);
		int rowi = row+1;
		if (rowi == 1 && (col_res & (1 << COL_a))) ret = key_t::K_F1;
		if (rowi == 1 && (col_res & (1 << COL_b))) ret = key_t::K_F2;
		if (rowi == 1 && (col_res & (1 << COL_c))) ret = key_t::K_F3;
		if (rowi == 1 && (col_res & (1 << COL_d))) ret = key_t::K_F4;
		if (rowi == 3 && (col_res & (1 << COL_b))) ret = key_t::K_LEFT;
		if (rowi == 3 && (col_res & (1 << COL_c))) ret = key_t::K_DOWN;
		if (rowi == 3 && (col_res & (1 << COL_d))) ret = key_t::K_RIGHT;
		if (rowi == 4 && (col_res & (1 << COL_c))) ret = key_t::K_UP;
		if (rowi == 4 && (col_res & (1 << COL_e))) ret = key_t::K_ENTER;
		if (rowi == 4 && (col_res & (1 << COL_a))) ret = key_t::K_ESC;
	}
	return ret;
}

//#define BW 320
//#define BH 240
//surface_24bpp_t bk_surface(BW,BH,BMP_GET_SIZE(BW,BH,24), lena_320);


extern "C" char hw_mode;
extern resources_t res;

ssd1963drv_t drv1;
screen_1_t screen1;
host_emu_t emu;


void drawScene(surface_t &s1) {
	static key_t::key_t last_key = (key_t::key_t)0;
	key_t::key_t key = getAppKey();
	if (key != last_key) {
		last_key = key;
		globals::modal_overlay.key_event(key);
	}
	if (rasterizer_t::render(&globals::modal_overlay, s1))
		s1.copy_to(0,0,-1,-1,0,0,drv1);
}

#define BASE_ADDRESS  (volatile datum *) 0x00800000
#define NUM_BYTES     (8 * 1024 * 1024)

int main() {

//	_ASSERT(!memTest());

	u8 buf0[BMP_GET_SIZE(320,240,24)];
	surface_24bpp_t s1(320,240,BMP_GET_SIZE(320,240,24), buf0);

	IOWR_ALTERA_AVALON_PIO_DIRECTION(PIO_1_BASE,0xFF);

	alt_putstr("Hello from Nios II!\n");

	globals::ttcache.init(ttcache_dat,sizeof(ttcache_dat));

	res.init();
	screen1.init();
	screen1.dirty = true;

	globals::modal_overlay.w = 320;
	globals::modal_overlay.h = 240;
	globals::modal_overlay.push_modal(&screen1);
	globals::modal_overlay.dirty = true;

	drv1.init();
	app_model_t::instance.init();
	emu.init();

//	alt_putstr("drv init done\n");
//	drawScene(true, s1);

	char key_toggle_hw_pressed = 0;
	while (1) {
		int key = getKey();
		if ((key & (1 << KEY_TOGGLE_HW)) && !key_toggle_hw_pressed) {
			key_toggle_hw_pressed = 1;
			hw_mode = 1-hw_mode;

			drv1.init();
			screen1.dirty = true;
//			alt_putstr("drv init done\n");
//			drawScene(true, s1);
//			alt_busy_sleep(1000000);
		} else if (!(key & (1 << KEY_TOGGLE_HW))) {
			key_toggle_hw_pressed = 0;
		}
		emu.update();
		drawScene(s1);
	}

	_ASSERT(0, return 0);

	return 0;
}
