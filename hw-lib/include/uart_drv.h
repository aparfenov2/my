/*
 * uart_drv.h
 *
 *  Created on: 19.03.2013
 *      Author: gordeev
 */

#ifndef UART_DRV_H_
#define UART_DRV_H_

#include "types.h"
#include "assert_impl.h"
extern "C" {
#include "ring_buffer.h"
}
#include "DSP2833x_device.h"
#include "DSP2833x_Sci.h"

#define UART_BUFF_SIZE 512

class uart_drv_t {
private:
	u8 _buf[UART_BUFF_SIZE];
	volatile SCI_REGS *sci;
public:
	ring_buffer_t ring_buf;
	static uart_drv_t *instance;

	uart_drv_t() {
		instance = this;
		sci = 0;
	}
	void init(volatile SCI_REGS *asci);

	void write(u8 byte);

	u8 read() {
		u8_t ret;
		_MY_ASSERT(ring_buffer_read_byte(&ring_buf, &ret),return 0);
		return (u8)ret;
	}

	bool is_empty() {
		return ring_buffer_empty(&ring_buf);
	}
};


#endif /* UART_DRV_H_ */
