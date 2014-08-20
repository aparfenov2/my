
#include <stdio.h>
#include "assert_impl.h"

#include "link.h"
#include "exported_sys.h"
#include "link_sys_impl.h"

using namespace link;


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



class loopback_serial_interface_t : public link::serial_interface_t {
public:
	loopback_serial_interface_t *remote;
	link::serial_data_receiver_t *receiver;
public:
	loopback_serial_interface_t() {
		remote = 0;
		receiver = 0;
	}

	virtual void send(u8 *data, u32 len) OVERRIDE {
		_MY_ASSERT(remote , return);
		_MY_ASSERT(remote->receiver, return);
		remote->receiver->receive(data,len);
	}

	virtual void subscribe(link::serial_data_receiver_t *_receiver) OVERRIDE {
		receiver = _receiver;
	}

	void wire(loopback_serial_interface_t *_remote) {
		remote = _remote;
	}
};

u8 arg8 = 0xaf;
u16 arg16 = 0xafaf;
u32 arg32 = 0xafafafaf;
float argd = 10203040506070.0;

class debug_intf_impl_t :
	public link::exported_system_interface_t,
	public link::exported_debug_interface_t {
public:

	virtual void key_event (u32 key_event) OVERRIDE {
	}

	virtual void test_request (u32 _arg8, u32 _arg16, u32 _arg32, float _argd) OVERRIDE {
		_WEAK_ASSERT(arg8 == _arg8, return);
		_WEAK_ASSERT(arg16 == _arg16, return);
		_WEAK_ASSERT(arg32 == _arg32, return);
		_WEAK_ASSERT(argd == _argd, return);
	}
};

void my_main() {

	loopback_serial_interface_t loopback_host;
	loopback_serial_interface_t loopback_local;
	loopback_local.wire(&loopback_host);
	loopback_host.wire(&loopback_local);

	link::serializer_t ser;
	link::host_serializer_t hser;
	ser.init(&loopback_local);
	hser.init(&loopback_host);

	debug_intf_impl_t debug_intf_impl;
	ser.add_implementation(&debug_intf_impl);

	hser.get_exported_debug_interface()->test_request(arg8,arg16,arg32,argd);

	while (1) {
	}

}
