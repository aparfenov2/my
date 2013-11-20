/*
 * uart_async.cpp
 *
 *  Created on: 19.03.2013
 *      Author: gordeev
 */


#include "uart_drv.h"

#include "c28/pins_def.h"     // DSP281x Headerfile Include File
#include "DSP2833x_device.h"
#include "DSP2833x_GlobalPrototypes.h"
#include "DSP2833x_Examples.h"

uart_drv_t *uart_drv_t::instance = 0;

void uart_drv_t::init(volatile SCI_REGS *asci) {
	ring_buffer_init(&ring_buf,(u8_t*)_buf, UART_BUFF_SIZE);
	sci = asci;

	if (sci == &ScibRegs)
	{
		EALLOW;

        GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;
        GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;

        GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 3;
        GpioCtrlRegs.GPAQSEL1.bit.GPIO9 = 3;

        GpioCtrlRegs.GPADIR.bit.GPIO11 = 0; //SCIRXDB
        GpioCtrlRegs.GPADIR.bit.GPIO9 = 1; //SCITXDB

		GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 2;
		GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 2;
		EDIS;
	}
	if (sci == &ScicRegs)
	{
		EALLOW;

        GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;
        GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;

        GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;
        GpioCtrlRegs.GPBQSEL2.bit.GPIO63 = 3;

        GpioCtrlRegs.GPBDIR.bit.GPIO62 = 0; //SCIRXDC
        GpioCtrlRegs.GPBDIR.bit.GPIO63 = 1; //SCITXDC

		GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;
		GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;
		EDIS;

	}

    EALLOW;

	sci->SCIHBAUD = 0x0000;
	sci->SCILBAUD = 162;		// 115200   SPEED = 150000000/((ScibRegs.SCILBAUD+1) * 8)

	sci->SCICCR.all = 0x0007;   // 1 stop bit,  no loopback, no parity,8 char bits,async mode, idle-line protocol
	sci->SCICTL1.all = 0x0003; //0x0043 // enable TX, RX, internal SCICLK,Disable RX ERR, SLEEP, TXWAKE

	sci->SCICTL2.bit.TXINTENA = 0;
	sci->SCICTL2.bit.RXBKINTENA = 1;
 	sci->SCIFFTX.bit.SCIRST = 0;
 	sci->SCIFFTX.all = 0xC002; //sci->SCIFFTX.all = 0xC000;
 	sci->SCIFFRX.all= 0x0021; //sci->SCIFFRX.all = 0x0021;
	sci->SCIPRI.all = 0x0018;
//	sci->SCIFFTX.bit.TXFFIENA = 1; //sci->SCIFFTX.bit.TXFFIENA = 1;

	sci->SCIFFCT.all = 0x00;

	sci->SCICTL1.all = 0x0023;     // Relinquish SCI from Reset

	sci->SCIFFTX.bit.TXFIFOXRESET = 1;
	sci->SCIFFRX.bit.RXFIFORESET = 1;
    EDIS;

}

void uart_drv_t::write(u8 byte) {
	while (sci->SCIFFTX.bit.TXFFST >= 15) {
	}
	sci->SCITXBUF = byte;
}


#pragma CODE_SECTION ("ramfuncs")
interrupt void Scib_Rx_Int (void)
{
	EINT;

//	isA_RX = true;

	if (ScibRegs.SCIRXST.bit.RXERROR)
	{
		ScibRegs.SCIFFRX.bit.RXFIFORESET = 0;
		ScibRegs.SCIFFRX.bit.RXFIFORESET = 1;
		ScibRegs.SCICTL1.bit.SWRESET = 0;
		ScibRegs.SCICTL1.bit.SWRESET = 1;
//		rx_err_cnt1++;
		return;
	}

	u8 byte = ScibRegs.SCIRXBUF.bit.RXDT;
	if (uart_drv_t::instance) {
		ring_buffer_write_byte(&uart_drv_t::instance->ring_buf,byte);
	}

	ScibRegs.SCIFFRX.bit.RXFFOVRCLR = 1;			// Clear Overflow flag
	ScibRegs.SCIFFRX.bit.RXFFINTCLR = 1;			// Clear Interrupt flag
	PieCtrlRegs.PIEACK.all |= PIEACK_GROUP9;		// Issue PIE ack
}

#pragma CODE_SECTION ("ramfuncs")
interrupt void Scic_Rx_Int (void)
{
	EINT;

//	isB_RX = true;

	if (ScicRegs.SCIRXST.bit.RXERROR) {
		ScicRegs.SCIFFRX.bit.RXFIFORESET = 0;
		ScicRegs.SCIFFRX.bit.RXFIFORESET = 1;
		ScicRegs.SCICTL1.bit.SWRESET = 0;
		ScicRegs.SCICTL1.bit.SWRESET = 1;
//		rx_err_cnt2++;
		return;
	}
	u8 byte = ScicRegs.SCIRXBUF.bit.RXDT;
	if (uart_drv_t::instance) {
		ring_buffer_write_byte(&uart_drv_t::instance->ring_buf,byte);
	}

	ScicRegs.SCIFFRX.bit.RXFFOVRCLR = 1;			// Clear Overflow flag
	ScicRegs.SCIFFRX.bit.RXFFINTCLR = 1;			// Clear Interrupt flag
	PieCtrlRegs.PIEACK.all |= PIEACK_GROUP8;		// Issue PIE ack
}
