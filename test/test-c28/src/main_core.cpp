
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

#include "uart_drv.h"
#include "devices.h"
#include "some_funcs.h"
#include "Spi.h"

#include <stdio.h>




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
screen_1_t screen1;
//host_emu_t emu;
serializer_t ser;
uart_drv_t	uart;
Spi spi;
FRAM fram;
FlashDev flash;


void draw_scene(surface_t &s1) {
	static key_t::key_t last_key = (key_t::key_t)0;
	key_t::key_t key = kbd_get_key();
	if (key != last_key) {
		last_key = key;
		globals::modal_overlay.key_event(key);
	}
	s16 enc_cnt = enc_reset_counter();
	while (enc_cnt) {
		if (enc_cnt > 0) {
			enc_cnt--;
			globals::modal_overlay.key_event(key_t::K_RIGHT);
		} else if (enc_cnt < 0) {
			enc_cnt++;
			globals::modal_overlay.key_event(key_t::K_LEFT);
		}
	}
	if (rasterizer_t::render(&globals::modal_overlay, s1))
		s1.copy_to(0,0,-1,-1,0,0,drv1);
}

class serial_interface_impl_t : public serial_interface_t {
public:
	serial_data_receiver_t *receiver;
public:
	serial_interface_impl_t() {
		receiver = 0;
	}

	virtual void send(u8 *data, u32 len) OVERRIDE {
		while(len--) {
			uart.write(*data++);
		}
	}

	virtual void subscribe(serial_data_receiver_t *areceiver) OVERRIDE {
		receiver = areceiver;
	}

	void cycle() {
		if (!uart.is_empty()) {

			u8 byte = uart.read();
			receiver->receive(&byte, 1);
		}
	}
};

void draw_pallete(surface_t *drv1) {
	// draw pallete
#define W 20
#define H 20


	drv1->ctx.pen_color = 0;
	drv1->fill(0,0,TFT_WIDTH,TFT_HEIGHT);

	s32 x = 0, y = 0, s = 0;
	for (s32 i=0; i<256; i++) {
		drv1->ctx.pen_color = i << (s * 8);
		s++;
		if (s >= 3) {
			s = 0;
		}
		drv1->fill(x,y,W,H);

		x += W+1;
		if (x+W > TFT_WIDTH) {
			x = 0;
			y += H+1;
			if (y+H > TFT_HEIGHT)
				break;
		}
	}
}

void my_main() {



    spi.Init();
	kbd_init();
	enc_init();

	drv1.init();

    _WEAK_ASSERT(read_file_table(),);
// show logo
	file_rec_t *fr = find_file(LOGO_FILE_ID);
	if (fr) {
	    u32 picbuf_sz = BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,24);
		u32 pic_sz = fr->cur_len;

		if (pic_sz && pic_sz <= picbuf_sz) {

		    u8 *picbuf = new u8[picbuf_sz];
		    surface_24bpp_t spic(TFT_WIDTH,TFT_HEIGHT,picbuf_sz, picbuf);

			flash.ReadData2(fr->offset,(u16 *)picbuf, picbuf_sz);

		    drv1.set_allowed_area(0,0,TFT_WIDTH,TFT_HEIGHT);
		    spic.set_allowed_area(0,0,TFT_WIDTH,TFT_HEIGHT);

			spic.copy_to(0,0,-1,-1,0,0,drv1);
			delete picbuf;
		} else {
			draw_pallete(&drv1);
		}
	} else {
		draw_pallete(&drv1);
	}

	s32 buf_sz = BMP_GET_SIZE_16(TFT_WIDTH,TFT_HEIGHT);
	u8 *buf0 = new u8[buf_sz];
	surface_16bpp_t s1(TFT_WIDTH,TFT_HEIGHT,buf_sz, buf0);


    u8 *ttcache_dat;
    u32 ttcache_sz;

    _WEAK_ASSERT(allocate_and_read_font_cache(ttcache_dat,ttcache_sz),);

    if (ttcache_dat && ttcache_sz) {
    	globals::ttcache.init((u8 *)ttcache_dat,ttcache_sz);

    	res.init();

		screen1.init();
		screen1.dirty = true;

		globals::modal_overlay.w = TFT_WIDTH;
		globals::modal_overlay.h = TFT_HEIGHT;
		globals::modal_overlay.push_modal(&screen1);
		globals::modal_overlay.dirty = true;

		app_model_t::instance.init();
    }

    uart.init(&ScibRegs);

	serial_interface_impl_t sintf;
	serializer_t ser;
	exported_interface2_impl_t ex2(&ser);
	ser.init(&app_model_t::instance,&ex2,&sintf);
	app_model_t::instance.subscribe_host(&ser);

	init_pie_table();

	while (1) {
		if (ttcache_dat && ttcache_sz) {
			draw_scene(s1);
		}
		sintf.cycle();
	}

//	_ASSERT(0, return 0);

//	return 0;
}
