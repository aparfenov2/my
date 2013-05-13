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



#define MAKE_CTLW(rstn,dcn,rdn,wrn,csn) ((DIR_RL << PIO_RL)|(DIR_UD << PIO_UD)|(hw_mode << PIO_HW_MODE)|(rstn << PIO_RST_N)|(dcn << PIO_DC_N)|(rdn << PIO_RD_N)|(wrn << PIO_WR_N)|(csn << PIO_CS_N))


void SSD1963_InitHW() {

    EALLOW;
    port_b_t dir;
    dir.all = GpioCtrlRegs.GPBDIR.all;
    dir.bits.PIN_DC   = 1;
    dir.bits.PIN_WR   = 1;
    dir.bits.PIN_RD   = 1;
    dir.bits.dat   = 0xff;
    dir.bits.PIN_CS   = 1;
    dir.bits.PIN_RST   = 1;
    dir.bits.PIN_UD   = 1;
    GpioCtrlRegs.GPBDIR.all = dir.all;

    GpioCtrlRegs.GPADIR.bit.PIN_RL   = 1;

    EDIS;

	GpioDataRegs.GPADAT.bit.PIN_RL = DIR_RL;

	port_b_t dat;
	dat.all = GpioDataRegs.GPBDAT.all;
	dat.bits.PIN_DC = 1;
	dat.bits.PIN_UD = DIR_UD;
	dat.bits.PIN_CS = 1;
	dat.bits.PIN_RD = 1;
	dat.bits.dat   = 0xff;
	dat.bits.PIN_RST = 1;
	dat.bits.PIN_WR = 1;
	GpioDataRegs.GPBDAT.all = dat.all;
}


void SSD1963_WriteCommand(unsigned int commandToWrite) {
	port_b_t dat;
	dat.all = GpioDataRegs.GPBDAT.all;
	dat.bits.PIN_DC = 0;
	dat.bits.dat = commandToWrite;
	dat.bits.PIN_CS = dat.bits.PIN_WR = 0;
	GpioDataRegs.GPBDAT.all = dat.all;
//	asm(" rpt #2 || nop");
	dat.bits.PIN_CS = dat.bits.PIN_WR = 1;
	GpioDataRegs.GPBDAT.all = dat.all;


//	WR_DAT(commandToWrite);
//	WR_CTL(MAKE_CTLW(1,0,1,0,0));
//	asm(" nop");
//	asm(" nop");
//	asm(" nop");
//	asm(" nop");
//	WR_CTL(MAKE_CTLW(1,0,1,1,1));
}

void SSD1963_WriteData(unsigned int dataToWrite) {
	port_b_t dat;
	dat.all = GpioDataRegs.GPBDAT.all;
	dat.bits.PIN_DC = 1;
	dat.bits.dat = dataToWrite;
	dat.bits.PIN_CS = dat.bits.PIN_WR = 0;
	GpioDataRegs.GPBDAT.all = dat.all;
//	asm(" rpt #2 || nop");
	dat.bits.PIN_CS = dat.bits.PIN_WR = 1;
	GpioDataRegs.GPBDAT.all = dat.all;

//	WR_DAT(dataToWrite);
//	WR_CTL(MAKE_CTLW(1,1,1,0,0));
//	asm(" nop");
//	asm(" nop");
//	asm(" nop");
//	asm(" nop");
//	WR_CTL(MAKE_CTLW(1,1,1,1,1));
}

void SSD1963_Reset() {
	volatile unsigned int dly;
	port_b_t dat;
	dat.all = GpioDataRegs.GPBDAT.all;
	dat.bits.PIN_RST = 0;
	GpioDataRegs.GPBDAT.all = dat.all;
	//WR_CTL(MAKE_CTLW(0,1,1,1,1));
	for(dly = 0; dly < 1000; dly++);
	dat.bits.PIN_RST = 1;
	GpioDataRegs.GPBDAT.all = dat.all;
//	WR_CTL(MAKE_CTLW(1,1,1,1,1));
	for(dly = 0; dly < 1000; dly++);
}
