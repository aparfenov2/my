/*
 * hw_impl.c
 *
 *  Created on: 20.11.2012
 *      Author: gordeev
 */


#include "sys/alt_stdio.h"

#include <io.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
//#include "ssd1963.h"

#define PIO_DC_N 0
#define PIO_RST_N 1
#define PIO_RL 2
#define PIO_UD 3

#define PIO_WR_N 4
#define PIO_RD_N 5
#define PIO_CS_N 6
#define PIO_HW_MODE 7

#define DIR_RL 0
#define DIR_UD 1

char hw_mode = 0;

#define MAKE_CTLW(rstn,dcn,rdn,wrn,csn) ((DIR_RL << PIO_RL)|(DIR_UD << PIO_UD)|(hw_mode << PIO_HW_MODE)|(rstn << PIO_RST_N)|(dcn << PIO_DC_N)|(rdn << PIO_RD_N)|(wrn << PIO_WR_N)|(csn << PIO_CS_N))
#define WR_CTL(v) IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, v)

#define WR_DAT(v) IOWR_ALTERA_AVALON_PIO_DATA(PIO_1_BASE, v)
#define WR_DAT_HW(v) IOWR(SSD1963_0_BASE,0, v)

void SSD1963_WriteCommand(unsigned int commandToWrite) {
	if (hw_mode) {
		WR_CTL(MAKE_CTLW(1,0,1,1,1));
		WR_DAT_HW(commandToWrite);
	} else {
		WR_DAT(commandToWrite);
		WR_CTL(MAKE_CTLW(1,0,1,0,0));
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		WR_CTL(MAKE_CTLW(1,0,1,1,1));
	}
}

void SSD1963_WriteData(unsigned int dataToWrite) {
	if (hw_mode) {
		WR_CTL(MAKE_CTLW(1,1,1,1,1));
		WR_DAT_HW(dataToWrite);
	} else {
		WR_DAT(dataToWrite);
		WR_CTL(MAKE_CTLW(1,1,1,0,0));
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		WR_CTL(MAKE_CTLW(1,1,1,1,1));
	}
}

void SSD1963_Reset() {
	volatile unsigned int dly;
	WR_CTL(MAKE_CTLW(0,1,1,1,1));
	for(dly = 0; dly < 1000; dly++);
	WR_CTL(MAKE_CTLW(1,1,1,1,1));
	for(dly = 0; dly < 1000; dly++);
}
