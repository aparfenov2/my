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

using namespace myvi;
using namespace link;
using namespace hw;



class serial_interface_impl_t : public serial_interface_t {
public:
	serial_data_receiver_t *receiver;
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

	virtual void subscribe(serial_data_receiver_t *areceiver) OVERRIDE {
		receiver = areceiver;
	}

	void cycle() {
		if (!uart->is_empty()) {

			u8 byte = uart->read();
			receiver->receive(&byte, 1);
		}
	}
};


Spi spi;
FRAM fram;
FlashDev flash;
uart_drv_t	uart;

serial_interface_impl_t sintf;
link::serializer_t serializer;
app::file_server_t file_server;
file_system_impl_t file_system;



int main(void)
{
    InitSysCtrl();

    init_zone7();

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

	init_pie_table();
	while(1) {
		sintf.cycle();
	}

}
