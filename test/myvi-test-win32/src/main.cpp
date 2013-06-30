#include <tchar.h>
#include "surface.h"

#include "bmp_math.h"
#include "host_emu.h"

#include "assert_impl.h"

#include "test_tools.h"
#include "screen_1.h"
#include "resources.h"
#include "widgets.h"

#include "ttcache.h"

#include <iostream>
#include <fstream>

#include <windows.h>
#include "Serial.h"

extern "C" {
#include "hdlc.h"
}
#include "packetizer.h"
#include "serializer.h"

using namespace std;
using namespace myvi;

#define _MAX_INT 2147483647

u8 buf0[BMP_GET_SIZE(480,272,24)];
surface_24bpp_t s1(480,272,BMP_GET_SIZE(480,272,24), buf0);

screen_1_t screen1;
extern resources_t res;
//host_emu_t emu;
CSerial serial;
myvi::packetizer_impl_t pkz;
myvi::serializer_t ser;


int ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Hello world"), MB_ICONSTOP|MB_OK);
	return 1;
}

void rs485_put_char(char data) {
	LONG    lLastError = ERROR_SUCCESS;

	lLastError = serial.Write(&data, 1);

	if (lLastError != ERROR_SUCCESS) {
		ShowError(serial.GetLastError(), _T("Unable to send data"));
	}
}

// Í Ì‡Ï ÔË¯ÂÎ Ô‡ÍÂÚ ÓÚ Û‰‡ÎÂÌÌÓÈ ÒËÒÚÂÏ˚
void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {
	pkz.receive((const u8*)buffer,bytes_received);
}

void my_hdlc_tx_frame(const u8 *buffer, u16 bytes_to_send) {
	hdlc_tx_frame((const u8_t*)buffer, bytes_to_send);
}



class my_test_drawer_t : public test_drawer_t {
public:
	s32 kx;
	s32 ky;
public:
	my_test_drawer_t() {
		w = 960;
		h = 544;
		kx = 2;
		ky = 2;
		cout << "set size to " << w << "x" << h << endl;
	}

	virtual bool callback(key_t::key_t key, s32 mx, s32 my, mkey_t::mkey_t mkey) OVERRIDE {
		if (key == key_t::K_SAVE) {
			u32 sz = 1024 * 200; 
			u8 *buf = new u8[sz];
			u32 dsz = globals::ttcache.save(buf,sz);

			ofstream outfile ("bmp\\ttcache.dat",ofstream::binary);
			outfile.write((char *)buf, dsz);
			outfile.close();

			_LOG2("font cache saved, size: " ,dsz );

//			globals::ttcache.init(buf,dsz);
			delete[] buf;
		}
		if (key) {
			globals::modal_overlay.key_event((key_t::key_t)key);
		}
		screen1.mouse_event((s32)(mx / kx),(s32) (my / ky), (mkey_t::mkey_t)mkey);
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
		LONG    lLastError = ERROR_SUCCESS;

		lLastError = serial.Read(&byte,1,&read);

		if (lLastError != ERROR_SUCCESS) {
			ShowError(serial.GetLastError(), _T("Unable to send data"));
		}

		while (read) {
			hdlc_on_rx_byte(byte);

			lLastError = serial.Read(&byte,1,&read);

			if (lLastError != ERROR_SUCCESS) {
				ShowError(serial.GetLastError(), _T("Unable to send data"));
			}
		}

	}

};

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
my_test_drawer_t test_drawer;

font_size_t::font_size_t sizes[] = 
		{font_size_t::FS_8, font_size_t::FS_15, font_size_t::FS_20, font_size_t::FS_30 ,(font_size_t::font_size_t)0};

void print_chars(ttype_font_t &fnt, surface_t &s1, const char * chars) {
	for (s32 s=0; s<100 && sizes[s]; s++ ) {
		fnt.set_char_size_px(0,sizes[s]);
		fnt.print_to(0,0,s1,string_t(chars));
	}
}

void print_chars_gly(ttype_font_t &fnt, surface_t &s1, u32 *str) {
	for (s32 s=0; s<100 && sizes[s]; s++ ) {
		fnt.set_char_size_px(0,sizes[s]);
		fnt.print_to(0,0,s1,string32_t(str));
	}
}

int _tmain(int argc, _TCHAR* argv[]) {
	//test_ftoa();
	//test_atof();
	//return 0;
	if (argc < 1) {
		cout << "arg0 : COM port name" << endl;
		return -1;
	}
	_LOG1("log started");

// init ttcache
	ttcache_t::init_lib();

	ifstream infile ("bmp\\ttcache.dat",ofstream::binary);
	if (!infile) {
		_LOG1("cant open font cache file");
	} else {
		infile.seekg( 0, std::ios::end );
		u32 sz = (u32)infile.tellg();
		infile.seekg( 0, std::ios::beg );
		u8 *buf = new u8[sz];
		infile.read((char *)buf, sz);
		infile.close();

		globals::ttcache.init(buf,sz);
		_LOG2("font cache loaded, size: ",sz);
	}

	res.init();
// standard values
	static u32 gly_str[] = {0x0026,0x0027,0x0028,0x0029,0x002a,0x002b,0x002c,0x002d,0x002e,0x002f,0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x00};
	print_chars_gly(res.gly,s1,gly_str);
	print_chars(res.ttf,s1,"abcdefghijklmnopqrstuvwxyz");
	print_chars(res.ttf,s1,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	print_chars(res.ttf,s1,"‡·‚„‰Â∏ÊÁËÈÍÎÏÌÓÔÒÚÛÙıˆ˜˘¯¸˚˙˝˛ˇ");
	print_chars(res.ttf,s1,"¿¡¬√ƒ≈®∆«»… ÀÃÕŒœ–—“”‘’÷◊Ÿÿ‹€⁄›ﬁﬂ");
	print_chars(res.ttf,s1,"`1234567890-=\\~!@#$%^&*()_+|[]{};':"",./<>?");

	screen1.init(); // init whole tree
	app_model_t::instance.init();
//	emu.set_target(&app_model_t::instance);

	globals::modal_overlay.w = 320;
	globals::modal_overlay.h = 240;
	globals::modal_overlay.push_modal(&screen1);


	LONG    lLastError = ERROR_SUCCESS;

    // Attempt to open the serial port (COM1)
    lLastError = serial.Open(argv[1],0,0,false);
	if (lLastError != ERROR_SUCCESS)
		return ::ShowError(serial.GetLastError(), _T("Unable to open COM-port"));

    // Setup the serial port (9600,N81) using hardware handshaking
	lLastError = serial.Setup(CSerial::EBaud115200,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
		return ::ShowError(serial.GetLastError(), _T("Unable to set COM-port setting"));

	// Setup handshaking
	lLastError = serial.SetupHandshaking(CSerial::EHandshakeOff);
	if (lLastError != ERROR_SUCCESS)
		return ::ShowError(serial.GetLastError(), _T("Unable to set COM-port handshaking"));

    hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
    pkz.init(&my_hdlc_tx_frame);
	ser.init(&pkz,&app_model_t::instance);
	app_model_t::instance.subscribe_host(&ser);

	test_drawer.plot_surface(s1);

	return 0;
}

