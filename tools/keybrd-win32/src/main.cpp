#include <tchar.h>

#include "assert_impl.h"

#include "test_tools.h"

#include <iostream>
#include <conio.h>

#include "link_sys_impl.h"


using namespace link;


class host_debug_intf_impl_t : 
	public link::host_system_interface_t, 
	public link::host_debug_interface_t {
public:
public:
	host_debug_intf_impl_t() {
	}

	virtual void test_response (u32 arg8, u32 arg16, u32 arg32, float argd) OVERRIDE {
	}

	virtual void log_event (const char * msg) OVERRIDE {
		_LOG2("remote: ",msg);
	}
};


static myvi::key_t::key_t translate_key(int k) {
	switch (k) {
	case 75 : return myvi::key_t::K_LEFT;
	case 77 : return myvi::key_t::K_RIGHT;
	case 72 : return myvi::key_t::K_UP;
	case 80 : return myvi::key_t::K_DOWN;

	case 27 : return myvi::key_t::K_ESC;
	case 13 : return myvi::key_t::K_ENTER;
	case 46 : return myvi::key_t::K_DOT;
	case 8 : return myvi::key_t::K_BKSP;
	case 83 : return myvi::key_t::K_DELETE;

	case 26 : return myvi::key_t::K_EXIT; // ctrl-z
	case 17 : return myvi::key_t::K_EXIT; // ctrl-q

	case 59 : return myvi::key_t::K_F1;
	case 60 : return myvi::key_t::K_F2;
	case 61 : return myvi::key_t::K_F3;
	case 62 : return myvi::key_t::K_F4;
	case 63 : return myvi::key_t::K_F5;

	case '0' : return myvi::key_t::K_0;
	case '1' : return myvi::key_t::K_1;
	case '2' : return myvi::key_t::K_2;
	case '3' : return myvi::key_t::K_3;
	case '4' : return myvi::key_t::K_4;
	case '5' : return myvi::key_t::K_5;
	case '6' : return myvi::key_t::K_6;
	case '7' : return myvi::key_t::K_7;
	case '8' : return myvi::key_t::K_8;
	case '9' : return myvi::key_t::K_9;
	}
	return (myvi::key_t::key_t)0;
}

int _tmain(int argc, _TCHAR* argv[]) {

	_LOG1("log started");

	//while (1) {
	//	if (_kbhit()) {
	//		int key = getch();
	//		std::cout << key << std::endl;
	//	}
	//}

	_TCHAR* com_port_name = 0;

// parse command line
	for (s32 i=0; i < argc; i++) {
		if (_tcscmp(argv[i], _T("--com")) == 0) {
			i++;
			if (i < argc) {
				com_port_name = argv[i];
				continue;
			} else break;
		}
	}

	if (!com_port_name ) {
		std::cout << "--com: COM port name" << std::endl;
		return 1;
	}



	test::serial_port_t port(com_port_name);
	test::serial_interface_impl_t sintf;

	if (!sintf.init(port)) {
		std::cout << "cant initialize port" << std::endl;
		return 1;
	}

	// роль хоста
	link::host_serializer_t *host_serializer = new link::host_serializer_t();
	host_serializer->init(&sintf);

	host_debug_intf_impl_t hfi_impl;
	host_serializer->add_implementation(&hfi_impl);

	link::exported_debug_interface_t *debug_intf = host_serializer->get_exported_debug_interface();


	while (1) {
		if (_kbhit()) {
			int key = getch();
			myvi::key_t::key_t tk = translate_key(key);
			if (tk) {
				if (tk == myvi::key_t::K_EXIT) break;
				debug_intf->key_event(tk);
			}
		}
		sintf.cycle();
	}


	return 0;
}

