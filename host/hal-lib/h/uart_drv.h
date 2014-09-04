/*
 * uart_drv.h
 *
 */

#ifndef UART_DRV_H_
#define UART_DRV_H_

#include "types.h"
#include "assert_impl.h"

#include "my_avalon_uart.h"

namespace hw {


class uart_drv_t {
private:
	my_avalon_uart_state st;
public:
	void init(void * base, u32 irq_controller_id,  u32 irq, u32 rx_buf_size, u32 tx_buf_size);

	u32 read(u8 *buf, u32 len);
	u32 write(u8 *buf, u32 len);
};

}

#endif /* UART_DRV_H_ */
