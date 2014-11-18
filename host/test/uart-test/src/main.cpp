/*
 * main.cpp
 *
 *  Created on: 03.09.2014
 *      Author: user
 */

#include "system.h"
#include "uart_drv.h"


hw::uart_drv_t uart;

int main(void) {
	uart.init((void *)UART_0_BASE, UART_0_IRQ_INTERRUPT_CONTROLLER_ID, UART_0_IRQ, 512, 64);

	u8 buf[256];
	do {
		u32 read = uart.read(buf, 256);
		if (read) {
			uart.write(buf, read);
		}
	} while(true);

}
