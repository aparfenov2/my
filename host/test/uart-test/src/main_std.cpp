/*
 * main.cpp
 *
 *  Created on: 03.09.2014
 *      Author: user
 */

#include "system.h"
#include <stdio.h>
#include <sys/alt_irq.h>


int main(void) {

//	asm("trap");

	int pie = alt_irq_enabled();
	int uie = alt_ic_irq_enabled(UART_0_IRQ_INTERRUPT_CONTROLLER_ID, UART_0_IRQ);

	if (!pie || !uie) {
		while(1);
	}

	FILE* fp;
	char* msg = "hello world";
	fp = fopen ("/dev/uart_0", "w");

	if (fp!=NULL) {
		do {
			fprintf(fp, "%s",msg);
		} while(true);

		fclose (fp);
	}
	return 0;

}
