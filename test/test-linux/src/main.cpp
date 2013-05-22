#include "surface.h"

#include "bmp_math.h"
#include "host_emu.h"

#include "assert_impl.h"

#include "screen_1.h"
#include "resources.h"
#include "widgets.h"

#include "ttcache.h"

#include <iostream>
#include <fstream>

extern "C" {
#include "hdlc.h"
}
#include "packetizer.h"
#include "serializer.h"

#include <errno.h>
#include <X11/Xlib.h>

using namespace std;
using namespace myvi;

#define _MAX_INT 2147483647

u8 buf0[BMP_GET_SIZE(320,240,24)];
surface_24bpp_t s1(320, 240, BMP_GET_SIZE(320,240,24), buf0);

screen_1_t screen1;
extern resources_t res;
//host_emu_t emu;
//CSerial serial;
myvi::packetizer_impl_t pkz;
myvi::serializer_t ser;

void rs485_put_char(char data) {
}

void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {
	pkz.receive((const u8*) buffer, bytes_received);
}

void my_hdlc_tx_frame(const u8 *buffer, u16 bytes_to_send) {
	hdlc_tx_frame((const u8_t*) buffer, bytes_to_send);
}

class my_test_drawer_t {
public:
	s32 kx;
	s32 ky;
public:
	my_test_drawer_t() {
//		w = 640;
//		h = 480;
//		kx = 2;
//		ky = 2;
//		cout << "set size to " << w << "x" << h << endl;
	}

	virtual bool callback(key_t::key_t key, s32 mx, s32 my, mkey_t::mkey_t mkey)
			OVERRIDE {
		if (key) {
			globals::modal_overlay.key_event((key_t::key_t) key);
		}
		screen1.mouse_event((s32) (mx / kx), (s32) (my / ky),
				(mkey_t::mkey_t) mkey);
		return rasterizer_t::render(&globals::modal_overlay, s1);
//		screen1.render(0,s1);
//		return true;
	}

	virtual void tick() OVERRIDE {
//		emu.update();
	}

	virtual void cycle() OVERRIDE {
		char byte;
		u32 read = 0;

		while (read) {
			hdlc_on_rx_byte(byte);

//			lLastError = serial.Read(&byte,1,&read);

		}

	}

};

class logger_impl_t: public logger_t {
public:
public:

	virtual logger_t& operator <<(s32 v) OVERRIDE {
		cout << v;
		return *this;
	}

	virtual logger_t& operator <<(const char *v) OVERRIDE {
		if (v == _endl)
			cout << endl;
		else
			cout << v;
		return *this;
	}

};

logger_impl_t logger_impl;
logger_t *logger_t::instance = &logger_impl;
my_test_drawer_t test_drawer;

int main() {
	_LOG1("log started");

	ttcache_t::init_lib();
	res.init();
	screen1.init(); // init whole tree
	app_model_t::instance.init();
//	emu.set_target(&app_model_t::instance);

	globals::modal_overlay.w = 320;
	globals::modal_overlay.h = 240;
	globals::modal_overlay.push_modal(&screen1);

	hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
	pkz.init(&my_hdlc_tx_frame);
	ser.init(&pkz, &app_model_t::instance);
	app_model_t::instance.subscribe_host(&ser);

//	test_drawer.plot_surface(s1);

	Display *d;
	Window w;
	XEvent e;
	int s;

	if ((d = XOpenDisplay(getenv("DISPLAY"))) == NULL) { // Соединиться с X сервером,
		_LOG2("Can't connect X server: %s\n", strerror(errno));
		exit(1);
	}
	s = DefaultScreen( d );
	w = XCreateSimpleWindow(d, RootWindow( d, s ), // Создать окно
			10, 10, 200, 200, 1, BlackPixel( d, s ), WhitePixel( d, s ));
	XSelectInput(d, w, ExposureMask | KeyPressMask); // На какие события будем реагировать?
	XMapWindow(d, w); // Вывести окно на экран
	while (1) { // Бесконечный цикл обработки событий
		XNextEvent(d, &e);
		if (e.type == Expose) { // Перерисовать окно
//			XFillRectangle(d, w, DefaultGC( d, s ), 20, 20, 10, 10);
//			XDrawString(d, w, DefaultGC( d, s ), 50, 50, msg, strlen(msg));
		}
		if (e.type == KeyPress) // При нажатии кнопки - выход
			break;
	}
	XCloseDisplay(d); // Закрыть соединение с X сервером

	return 0;
}

