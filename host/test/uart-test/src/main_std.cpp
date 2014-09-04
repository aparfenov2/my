/*
 * main.cpp
 *
 *  Created on: 03.09.2014
 *      Author: user
 */

#include "system.h"
#include <stdio.h>


int main(void) {

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
