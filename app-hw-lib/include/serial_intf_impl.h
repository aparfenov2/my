/*
 * serial_intf_impl.h
 *
 *  Created on: 29.08.2014
 *      Author: user
 */

#ifndef SERIAL_INTF_IMPL_H_
#define SERIAL_INTF_IMPL_H_

#include "link.h"
#include "uart_drv.h"

namespace hw {

class serial_interface_impl_t : public link::serial_interface_t {
public:
	link::serial_data_receiver_t *receiver;
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

	virtual void subscribe(link::serial_data_receiver_t *areceiver) OVERRIDE {
		receiver = areceiver;
	}

	void cycle() {
		if (!uart->is_empty()) {

			u8 byte = uart->read();
			receiver->receive(&byte, 1);
		}
	}
};

}

#endif /* SERIAL_INTF_IMPL_H_ */
