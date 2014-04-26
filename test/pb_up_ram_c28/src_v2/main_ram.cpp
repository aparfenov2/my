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

#include "ssd1963drv.h"
#include "surface.h"
#include "bmp_math.h"

#include "app_events.h"

#include "devices.h"
#include "Spi.h"
#include "uart_drv.h"
#include "link.h"
#include "exported2_impl.h"



extern "C" void SSD1963_InitHW();

using namespace myvi;

ssd1963drv_t drv1;

Spi spi;
FRAM fram;
FlashDev flash;
uart_drv_t	uart;

using namespace myvi;

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



int main(void)
{
    InitSysCtrl();

    init_zone7();

    spi.Init();

    _MY_ASSERT(read_file_table(),);

    uart.init(&ScibRegs);
	init_pie_table();

	serial_interface_impl_t sintf;
	serializer_t ser;
	exported_interface2_impl_t ex2(&ser);
	ser.init(0,&ex2,&sintf);

	while(1) {
		sintf.cycle();
	}

}
