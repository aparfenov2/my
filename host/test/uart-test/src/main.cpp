/*
 * main.cpp
 *
 *  Created on: 03.09.2014
 *      Author: user
 */

#include "system.h"
#include "uart_drv.h"
#include "link_sys_impl.h"

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


class serial_interface_impl_t : public link::serial_interface_t {
public:
	link::serial_data_receiver_t *receiver;
	hw::uart_drv_t *uart;
	u8 buf[256];
public:
	serial_interface_impl_t() {
		receiver = 0;
		uart = 0;
	}

	void init(hw::uart_drv_t *_uart) {
		uart = _uart;
	}

	virtual void send(u8 *data, u32 len) OVERRIDE {
		uart->write(data, len);
	}

	virtual void subscribe(link::serial_data_receiver_t *areceiver) OVERRIDE {
		receiver = areceiver;
	}

	void cycle() {

		u32 read = uart->read(buf, 256);
		if (read) {
			receiver->receive(buf, read);
		}
	}
};



hw::uart_drv_t uart;
serial_interface_impl_t sintf;
link::serializer_t serializer;
debug_intf_impl_t debug_intf_impl;

int main(void) {
	uart.init((void *)UART_0_BASE, UART_0_IRQ_INTERRUPT_CONTROLLER_ID, UART_0_IRQ, 512, 64);

    sintf.init(&uart);
    serializer.init(&sintf);

    debug_intf_impl.init(serializer.get_host_debug_interface());
    serializer.add_implementation(&debug_intf_impl);

	while (true) {
		sintf.cycle();
	}

/*
	u8 buf[256];
	do {
		u32 read = uart_2.read(buf, 256);
		if (read) {
			uart_2.write(buf, read);
		}
	} while(true);
*/
}
