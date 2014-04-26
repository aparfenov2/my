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

#include "link.h"
#include "disp_def.h"

using namespace std;
using namespace myvi;

#define _MAX_INT 2147483647

u8 buf0[BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16)];
surface_16bpp_t s1(TFT_WIDTH,TFT_HEIGHT,BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16), buf0);

//u8 buf0[BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,24)];
//surface_24bpp_t s1(TFT_WIDTH,TFT_HEIGHT,BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,24), buf0);

screen_1_t screen1;
extern resources_t res;
//host_emu_t emu;


int ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Hello world"), MB_ICONSTOP|MB_OK);
	return 1;
}


void save_ttcache() {
	u32 sz = 1024 * 200; 
	u8 *buf = new u8[sz];
	u32 dsz = globals::ttcache.save(buf,sz);

	ofstream outfile ("bmp\\ttcache.dat",ofstream::binary);
	outfile.write((char *)buf, dsz);
	outfile.close();

	_LOG2("font cache saved, size: " ,dsz );

//	globals::ttcache.init(buf,dsz);
	delete[] buf;
}

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
		cout << "set size to " << w << "x" << h << endl;
	}

	virtual bool callback(key_t::key_t key, s32 mx, s32 my, mkey_t::mkey_t mkey) OVERRIDE {
		if (key == key_t::K_SAVE) {
			save_ttcache();
		}
		if (key) {
			globals::modal_overlay.key_event((key_t::key_t)key);
		}
		screen1.mouse_event((s32)(mx / kx),(s32) (my / ky), (mkey_t::mkey_t)mkey);
		return rasterizer_t::render(&globals::modal_overlay, s1);
	}

};

class exported_interface2_impl_t : public msg::exported_interface2_t {
public:
	void key_event(key_t::key_t key) OVERRIDE {
		globals::modal_overlay.key_event(key);
	}

	void upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) OVERRIDE {
	}
	void download_file(u32 file_id, u32 offset, u32 length) OVERRIDE {
	}
	void update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) OVERRIDE {
	}
	void read_file_info(u32 file_id) OVERRIDE {
	}
};

class serial_interface_impl_t : public serial_interface_t {
public:
	serial_data_receiver_t *receiver;
	CSerial *serial;
public:
	serial_interface_impl_t(CSerial *aserial) {
		receiver = 0;
		serial = aserial;
	}

	virtual void send(u8 *data, u32 len) OVERRIDE {
		LONG  lLastError = serial->Write(data,len);
		if (lLastError != ERROR_SUCCESS) {
			ShowError(serial->GetLastError(), _T("Unable to send data"));
		}
	}

	virtual void subscribe(serial_data_receiver_t *areceiver) OVERRIDE {
		receiver = areceiver;
	}

	void cycle() {
		u8 buf[1024];
		u32 read;
		LONG  lLastError = serial->Read(buf, 1024, &read);
		if (lLastError != ERROR_SUCCESS) {
			ShowError(serial->GetLastError(), _T("Unable to receive data"));
		} else if (read) {
			receiver->receive(buf, read);
		}
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

	if (argc < 2) {
		cout << "arg0 : COM port name, arg2: --nottf" << endl;
		return -1;
	}
	_LOG1("log started");

// init ttcache
	if (argc < 3) {
		ttcache_t::init_lib();
	} else {
		cout << "skipped ttcache_t::init_lib() " << endl;
	}

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
	print_chars(res.ttf,s1,"àáâãäå¸æçèéêëìíîïðñòóôõö÷ùøüûúýþÿ");
	print_chars(res.ttf,s1,"ÀÁÂÃÄÅ¨ÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ÙØÜÛÚÝÞß");
	print_chars(res.ttf,s1,"`1234567890-=\\~!@#$%^&*()_+|[]{};':"",./<>?");

	screen1.init(); // init whole tree
	app_model_t::instance.init();
//	emu.set_target(&app_model_t::instance);

	globals::modal_overlay.w = TFT_WIDTH;
	globals::modal_overlay.h = TFT_HEIGHT;
	globals::modal_overlay.push_modal(&screen1);



	CSerial serial;

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

	serial_interface_impl_t sintf(&serial);
	myvi::serializer_t ser;
	exported_interface2_impl_t exported2;
	ser.init(&app_model_t::instance, &exported2, &sintf);
	app_model_t::instance.subscribe_host(&ser);

	test_drawer.create_window(s1);

	bool exit = false;
	while (!exit) {
		exit = test_drawer.cycle(s1);
		sintf.cycle();
	}

	return 0;
}

