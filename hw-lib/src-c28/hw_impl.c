/*
 * hw_impl.c
 *
 *  Created on: 20.11.2012
 *      Author: gordeev
 */


//#include "sys/alt_stdio.h"

//#include <io.h>
//#include "system.h"
//#include "altera_avalon_pio_regs.h"
//#include "ssd1963.h"

//#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

#include "DSP2833x_device.h"
#include "c28/pins_def.h"

#define DIR_RL 0
#define DIR_UD 1



port_a_t dat_a;
port_b_t dat_b;

void SSD1963_InitHW() {

    EALLOW;
    port_a_t dir_a;
    dir_a.all = 0;
    dir_a.bits.BL_E = 1;
    dir_a.bits.RST = 1;
    dir_a.bits.DISP = 1;
    dir_a.bits.WR = 1;
    dir_a.bits.CS = 1;
    dir_a.bits.RD = 1;
    dir_a.bits.DC = 1;
    GpioCtrlRegs.GPADIR.all = dir_a.all;

    port_b_t dir_b;
    dir_b.all = 0;
    dir_b.bits.D0_15 = 0xffff;
    GpioCtrlRegs.GPBDIR.all = dir_b.all;
    EDIS;


	dat_a.all = 0;
	dat_a.bits.BL_E = 1;
	dat_a.bits.RST = 1;
	dat_a.bits.DISP = 1;
	dat_a.bits.WR = 1;
    dir_a.bits.CS = 1;
	dat_a.bits.RD = 1;
	dat_a.bits.DC = 1;
	GpioDataRegs.GPADAT.all = dat_a.all;

	dat_b.all = 0;
	dat_b.bits.D0_15 = 0xffff;
	GpioDataRegs.GPBDAT.all = dat_b.all;
}


void SSD1963_WriteCommand(unsigned int commandToWrite) {

	dat_a.bits.DC = 0;
	dat_a.bits.WR = 0;
	dat_a.bits.CS = 0;

	dat_b.bits.D0_15 = commandToWrite;

	GpioDataRegs.GPBDAT.all = dat_b.all;

	GpioDataRegs.GPADAT.all = dat_a.all;
	//	asm(" rpt #2 || nop");
	dat_a.bits.WR = 1;
	dat_a.bits.CS = 1;
	GpioDataRegs.GPADAT.all = dat_a.all;

}

void SSD1963_WriteData(unsigned int dataToWrite) {

	dat_a.bits.DC = 1;
	dat_a.bits.WR = 0;
	dat_a.bits.CS = 0;

	dat_b.bits.D0_15 = dataToWrite;

	GpioDataRegs.GPBDAT.all = dat_b.all;

	GpioDataRegs.GPADAT.all = dat_a.all;
	//	asm(" rpt #2 || nop");
	dat_a.bits.WR = 1;
	dat_a.bits.CS = 1;
	GpioDataRegs.GPADAT.all = dat_a.all;

}

void SSD1963_Reset() {
	volatile unsigned int dly;
	dat_a.bits.RST = 0;
	GpioDataRegs.GPADAT.all = dat_a.all;
	for(dly = 0; dly < 1000; dly++);
	dat_a.bits.RST = 1;
	GpioDataRegs.GPADAT.all = dat_a.all;
	for(dly = 0; dly < 1000; dly++);
}
