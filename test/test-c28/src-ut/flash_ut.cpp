
#include "assert_impl.h"

#include "link.h"
#include "exported_sys.h"
#include "link_sys_impl.h"

#include "serial_intf_impl.h"
#include "flash_intf_impl.h"
#include "uart_drv.h"
#include "some_funcs.h"



using namespace link;
using namespace hw;


extern "C" {
	void SSD1963_ClearScreen(u32 color);
	void SSD1963_Reset(void);
	void SSD1963_Init(void);
}

uart_drv_t	uart;
serial_interface_impl_t sintf;
link::serializer_t ser;
flash_intf_impl_t flash_intf_impl;

void my_main() {

	SSD1963_Reset();
	SSD1963_Init();

	SSD1963_ClearScreen(0x0000FF);

    uart.init(&ScibRegs);
	sintf.init(&uart);

	ser.init(&sintf);
	flash_intf_impl.init(ser.get_host_flash_interface());
	ser.add_implementation(&flash_intf_impl);


	init_pie_table();
	while(1) {
		sintf.cycle();
	}

}
