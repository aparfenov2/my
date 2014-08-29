#include <stdlib.h>
#include <stdio.h>

#include "DSP2833x_device.h"
#include "DSP2833x_GlobalPrototypes.h"
#include "DSP2833x_CpuTimers.h"
#include "DSP2833x_Examples.h"
#include "DSP2833x_SysCtrl.h"
#include "cpy_tbl.h"
#include <new.h>
#include "some_funcs.h"
#include "c28/pins_def.h"

#include "types.h"
#include "assert_impl.h"

#include "app_events.h"

#include "devices.h"
#include "Spi.h"
#include "uart_drv.h"


#include "link.h"
#include "link_sys_impl.h"
#include "file_server.h"
#include "file_map.h"
#include "file_system_impl.h"

#include "serial_intf_impl.h"
#include "flash_intf_impl.h"

using namespace myvi;
using namespace link;
using namespace hw;



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
	}

	virtual void test_request (u32 arg8, u32 arg16, u32 arg32, float argd) OVERRIDE {
		_MY_ASSERT(host, return);
		host->test_response(arg8,arg16,arg32,argd);
	}
};

extern "C" {
	void SSD1963_ClearScreen(u32 color);
	void SSD1963_Reset(void);
	void SSD1963_Init(void);
	void SSD1963_InitHW();
}


Spi spi;
FRAM fram;
FlashDev flash;
uart_drv_t	uart;

serial_interface_impl_t sintf;
link::serializer_t serializer;
app::file_server_t file_server;
file_system_impl_t file_system;
debug_intf_impl_t debug_intf_impl;
flash_intf_impl_t flash_intf_impl;


int main(void)
{
    InitSysCtrl();

    init_zone7();
    SSD1963_InitHW();

	SSD1963_Reset();
	SSD1963_Init();

	SSD1963_ClearScreen(0x0000FF);

    spi.Init();
	fram.init(&spi);
	flash.init(&spi);

	_WEAK_ASSERT(
			file_system.init(&fram, &flash)
			, 0);

    uart.init(&ScibRegs);
	sintf.init(&uart);
    serializer.init(&sintf);

    file_server.init(serializer.get_host_file_interface(), &file_system);
    serializer.add_implementation(&file_server);

    debug_intf_impl.init(serializer.get_host_debug_interface());
    serializer.add_implementation(&debug_intf_impl);

	flash_intf_impl.init(serializer.get_host_flash_interface());
	serializer.add_implementation(&flash_intf_impl);

	init_pie_table();
	while(1) {
		sintf.cycle();
	}

}
