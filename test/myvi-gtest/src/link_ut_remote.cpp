#include "gtest/gtest.h"
#include "link_sys_impl.h"
#include "exported_stub.h"
#include "basic.h"
#include "test_tools.h"


u8 arg8 = 0xaf;
u16 arg16 = 0xafaf;
u32 arg32 = 0xafafafaf;
float argd = 10203040506070.0;

bool go_exit = false;

class host_debug_intf_impl_t : 
	public link::host_system_interface_t,
	public link::host_debug_interface_t {
public:
	virtual void test_response (u32 _arg8, u32 _arg16, u32 _arg32, float _argd) OVERRIDE {
		go_exit = true;
		_LOG1("test_response received");
		EXPECT_EQ(arg8, _arg8);
		EXPECT_EQ(arg16, _arg16);
		EXPECT_EQ(arg32, _arg32);
		EXPECT_EQ(argd, _argd);
	}

	virtual void log_event (const char * msg) OVERRIDE {
	}
};

//#define TEST(a,b) void a##b()
TEST(LinkTest, Remote) {
	test::serial_port_t port(L"COM8");
	test::serial_interface_impl_t sintf;
	sintf.init(port);

// host side
	link::host_serializer_t hser;
	hser.init(&sintf);

	host_debug_intf_impl_t host_debug_intf_impl;
	hser.add_implementation(&host_debug_intf_impl);


	u32 cnt0 = 0xffff;
	u32 cnt = cnt0;
	while (!go_exit) {
		cnt--;
		if (!cnt) {
			cnt = cnt0;
			hser.get_exported_debug_interface()->test_request(arg8,arg16,arg32,argd);
			_LOG1("test_request sent");
		}
		sintf.cycle();
	}
}
