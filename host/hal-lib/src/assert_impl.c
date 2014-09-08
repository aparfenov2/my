/*
 * assert_impl.c
 *
 *  Created on: 23.11.2012
 *      Author: Parfenov
 */

#include <stdio.h>

char* my_itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i = -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

void my_assert (const char *msg, const char *file, int line) {
	char ls[10];
	my_itoa(line,ls);
	printf("%s in %s at %s\n",msg,file,ls);
//	asm(" ESTOP0 ");
	while(1);
}

void my_trace (const char *msg, const char *file, int line) {
	char ls[10];
	my_itoa(line,ls);
	printf("%s in %s at %s\n",msg,file,ls);
//	while(1);
}
