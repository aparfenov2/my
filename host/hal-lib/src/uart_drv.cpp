/*
 * uart_async.cpp
 *
 *  Created on: 19.03.2013
 *      Author: gordeev
 */


#include "uart_drv.h"
#include <string.h>
#include <fcntl.h>

using namespace hw;

extern "C" {
int my_avalon_uart_read(my_avalon_uart_state* sp, char* ptr, int len,  int flags);
int my_avalon_uart_write(my_avalon_uart_state* sp, const char* ptr, int len, int flags);
}


void uart_drv_t::init(void * base, u32 irq_controller_id,  u32 irq, u32 rx_buf_size, u32 tx_buf_size) {
	memset(&this->st, 0, sizeof(this->st));
	this->st.base = base;
	this->st.rx_buf_len = rx_buf_size;
	this->st.tx_buf_len = tx_buf_size;
	this->st.rx_buf = new u8[this->st.rx_buf_len];
	this->st.tx_buf = new u8[this->st.tx_buf_len];
	my_avalon_uart_init(&this->st, (alt_u32) irq_controller_id, (alt_u32) irq );
}


u32 uart_drv_t::read(u8 *buf, u32 len) {
	int ret = my_avalon_uart_read(&this->st, (char *)buf, (int)len, O_NONBLOCK);
	return ret < 0 ? 0 : ret;
}

u32 uart_drv_t::write(u8 *buf, u32 len) {
	int ret = my_avalon_uart_write(&this->st, (char *)buf, (int)len, O_NONBLOCK);
	return ret < 0 ? 0 : ret;
}
