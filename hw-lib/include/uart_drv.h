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
#include "DSP2833x_device.h"
#include "DSP2833x_Sci.h"

#define UART_BUFF_SIZE 1024

namespace hw {

class uart_drv_t {
private:
	u8 _buf[UART_BUFF_SIZE];
	volatile SCI_REGS *sci;
public:
	uart_drv_t() {
		sci = 0;
	}
	void init(volatile SCI_REGS *asci);

	void write(u8 byte);

	u8 read();
	bool is_empty();
};

}

#endif /* UART_DRV_H_ */
