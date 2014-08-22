
#include "ssd1963drv.h"

#include "surface.h"
#include "assert_impl.h"
#include "bmp_math.h"
#include "app_events.h"

#include "resources.h"
#include "widgets.h"

#include "ttcache.h"

#include "file_map.h"
#include "file_system_impl.h"

#include "link.h"

#include "uart_drv.h"
#include "devices.h"
#include "some_funcs.h"
#include "Spi.h"

#include <stdio.h>

#include "custom_common.h"

#include "link_sys_impl.h"
#include "link_model_updater.h"
#include "file_server.h"


using namespace myvi;
using namespace hw;




extern "C" char* my_itoa(int i, char b[]);

class logger_impl_t : public logger_t {
public:
	link::host_debug_interface_t *host;
public:
	logger_impl_t() {
		host = 0;
	}
	void init(link::host_debug_interface_t *_host) {
		host = _host;
	}

    virtual logger_t& operator << (s32 v) OVERRIDE {
    	char ls[32];
    	my_itoa(v,ls);
    	printf(ls);
    	if (host) {
    		host->log_event(ls);
    	}
        return *this;
    }

    virtual logger_t& operator << (const char *v) OVERRIDE {
		if (v == _endl) {
			printf("\n");
	    	if (host) {
	    		host->log_event("\n");
	    	}
		} else {
			printf(v);
	    	if (host) {
	    		host->log_event(v);
	    	}
		}
        return *this;
    }

};


// ------------------------------- весь экран ------------------------------------

typedef custom::dynamic_view_mixin_aware_impl_t<gobject_t> _test_screen_super_t;

class test_screen_t : public _test_screen_super_t, public focus_aware_t {
	typedef _test_screen_super_t super;

public:

	virtual void init() OVERRIDE {

		w = TFT_WIDTH;
		h = TFT_HEIGHT;

//		rasterizer_t::debug = true;


		gen::view_meta_t *root_view_meta = gen::meta_registry_t::instance().find_view_meta("root");
		gobject_t *root_view = custom::view_meta_ex_t(root_view_meta).build_view_no_ctx();

		add_child(root_view, "root");

		root_view->x = 0;
		root_view->y = 0;
		root_view->w = w;
		root_view->h = h;


		init_children();

		do_layout();
		dirty = true;

	}

};


void draw_scene(surface_t &s1, surface_t &drv1) {

	gobject_t *gobj = & modal_overlay_t::instance();
	focus_aware_t * focus_aware = dynamic_cast<focus_aware_t*>(gobj);
	_MY_ASSERT(focus_aware, return);

	static key_t::key_t last_key = (key_t::key_t)0;
	key_t::key_t key = kbd_get_key();
	if (key != last_key) {
		last_key = key;

		// сначала отдаем на перехват фильтрам клавиатуры
		if (!custom::keyboard_filter_chain_t::instance().process_key(key)) {
			focus_aware->key_event((key_t::key_t)key);
		}
	}
	s16 enc_cnt = enc_reset_counter();
	while (enc_cnt) {
		if (enc_cnt > 0) {
			enc_cnt--;
			focus_aware->key_event(key_t::K_RIGHT);
		} else if (enc_cnt < 0) {
			enc_cnt++;
			focus_aware->key_event(key_t::K_LEFT);
		}
	}
	if (rasterizer_t::render(gobj, s1))
		s1.copy_to(0,0,-1,-1,0,0,drv1);
}



class serial_interface_impl_t : public link::serial_interface_t {
public:
	link::serial_data_receiver_t *receiver;
	uart_drv_t *uart;
public:
	serial_interface_impl_t() {
		receiver = 0;
		uart = 0;
	}

	void init(uart_drv_t *_uart) {
		uart = _uart;
	}

	virtual void send(u8 *data, u32 len) OVERRIDE {
		while(len--) {
			uart->write(*data++);
		}
	}

	virtual void subscribe(link::serial_data_receiver_t *areceiver) OVERRIDE {
		receiver = areceiver;
	}

	void cycle() {
		if (!uart->is_empty()) {

			u8 byte = uart->read();
			receiver->receive(&byte, 1);
		}
	}
};

void draw_pallete(surface_t &drv1) {
	// draw pallete
#define W 20
#define H 20


	drv1.ctx.pen_color = 0;
	drv1.fill(0,0,TFT_WIDTH,TFT_HEIGHT);

	s32 x = 0, y = 0, s = 0;
	for (s32 i=0; i<256; i++) {
		drv1.ctx.pen_color = i << (s * 8);
		s++;
		if (s >= 3) {
			s = 0;
		}
		drv1.fill(x,y,W,H);

		x += W+1;
		if (x+W > TFT_WIDTH) {
			x = 0;
			y += H+1;
			if (y+H > TFT_HEIGHT)
				break;
		}
	}
}


class debug_intf_impl_t :
	public link::exported_system_interface_t,
	public link::exported_debug_interface_t {
public:
	link::host_debug_interface_t *host;
public:
	debug_intf_impl_t() {
		host = 0;
	}

	void init(link::host_debug_interface_t *_host) {
		host = _host;
	}

	virtual void key_event (u32 key_event) OVERRIDE {
		myvi::key_t::key_t key = (myvi::key_t::key_t)key_event;
		gobject_t *gobj = & modal_overlay_t::instance();
		focus_aware_t * focus_aware = dynamic_cast<focus_aware_t*>(gobj);
		_MY_ASSERT(focus_aware, return);

		if (!custom::keyboard_filter_chain_t::instance().process_key(key)) {
			focus_aware->key_event((key_t::key_t)key);
		}
	}

	virtual void test_request (u32 arg8, u32 arg16, u32 arg32, float argd) OVERRIDE {
		_MY_ASSERT(host, return);
		host->test_response(arg8,arg16,arg32,argd);
	}
};



logger_impl_t logger_impl;
logger_t *logger_t::instance = &logger_impl;
extern resources_t res;

serial_interface_impl_t sintf;
link::serializer_t serializer;
custom::link_model_updater_t link_model_updater;
app::file_server_t file_server;
file_system_impl_t file_system;
debug_intf_impl_t debug_intf_impl;
uart_drv_t	uart;
ssd1963drv_t drv1;
Spi spi;
FRAM fram;
FlashDev flash;


extern void init_singletones();

void my_main() {



    spi.Init();
	kbd_init();
	enc_init();

	drv1.init();

	fram.init(&spi);
	flash.init(&spi);


	init_singletones();


	_WEAK_ASSERT(file_system.init(&fram, &flash), 0);

// show logo
	bool can_show_logo = false;
	do {
		u32 logo_len, logo_max_len;
		_WEAK_ASSERT(file_system.get_info(LOGO_FILE_ID, logo_len, logo_max_len), break);

	    u32 picbuf_sz = BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,24);

		_WEAK_ASSERT(logo_len && logo_len <= picbuf_sz, break);

		u8 *picbuf = new u8[picbuf_sz];
		_WEAK_ASSERT(picbuf, break);

		surface_24bpp_t spic(TFT_WIDTH,TFT_HEIGHT,picbuf_sz, picbuf);

		do {
			_WEAK_ASSERT(file_system.read_file(LOGO_FILE_ID,0,logo_len, picbuf), break);

			drv1.set_allowed_area(0,0,TFT_WIDTH,TFT_HEIGHT);
			spic.set_allowed_area(0,0,TFT_WIDTH,TFT_HEIGHT);

			spic.copy_to(0,0,-1,-1,0,0,drv1);

			can_show_logo = true;
		} while (false);

		delete picbuf;

	} while(false);

	if (!can_show_logo) {
		draw_pallete(drv1);
	}

	s32 buf_sz = BMP_GET_SIZE_16(TFT_WIDTH,TFT_HEIGHT);
	u8 *buf0 = new u8[buf_sz];
	_MY_ASSERT(buf0, return);
	surface_16bpp_t s1(TFT_WIDTH,TFT_HEIGHT,buf_sz, buf0);


    uart.init(&ScibRegs);
    sintf.init(&uart);
    serializer.init(&sintf);

    logger_impl.init(serializer.get_host_debug_interface());

    debug_intf_impl.init(serializer.get_host_debug_interface());
    serializer.add_implementation(&debug_intf_impl);

    file_server.init(serializer.get_host_file_interface(), &file_system);
    serializer.add_implementation(&file_server);

    bool ttcache_loaded = false;
	do {
		u32 ttcache_file_id = TTCACHE_FILE_ID;
		u32 ttcache_len, ttcache_max_len;
		_WEAK_ASSERT(file_system.get_info(ttcache_file_id, ttcache_len, ttcache_max_len), break);
		if (!ttcache_len) break;

		u8 *ttcache_dat = new u8[ttcache_len + 0x0f];
		_WEAK_ASSERT(ttcache_dat, break);

		_WEAK_ASSERT(file_system.read_file(ttcache_file_id,0,ttcache_len, ttcache_dat), break);

		globals::ttcache.init((u8 *)ttcache_dat,ttcache_len);
		delete ttcache_dat;
		ttcache_loaded = true;

	} while(false);


    bool schema_loaded = false;
	do {
		u32 schema_file_id = SCHEMA_FILE_ID;
		u32 schema_len, schema_max_len;
		_WEAK_ASSERT(file_system.get_info(schema_file_id, schema_len, schema_max_len), break);
		if (!schema_len) break;

		u8 *xml = new u8[schema_len + 0x0f];
		_WEAK_ASSERT(xml, break);

		_WEAK_ASSERT(file_system.read_file(schema_file_id,0,schema_len, xml), break);

		xml[schema_len] = 0;
		gen::meta_registry_t::instance().init((char *)xml);
		schema_loaded = true;

	} while(false);

    if (ttcache_loaded && schema_loaded) {

    	res.init();



    	test_screen_t *screen1 = new test_screen_t();
		screen1->init();
		screen1->dirty = true;

    	link_model_updater.init(serializer.get_host_model_interface());
    	serializer.add_implementation(&link_model_updater);

		modal_overlay_t::instance().w = TFT_WIDTH;
		modal_overlay_t::instance().h = TFT_HEIGHT;
		modal_overlay_t::instance().push_modal(screen1);
		modal_overlay_t::instance().dirty = true;

    }


	init_pie_table();

	while (1) {
		if (ttcache_loaded && schema_loaded) {
			draw_scene(s1, drv1);
		}
		sintf.cycle();
	}

}
