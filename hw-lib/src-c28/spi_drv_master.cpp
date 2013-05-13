/*
 * spi_async.cpp
 *
 *  Created on: 07.03.2013
 *      Author: gordeev
 */

#include "spi_drv_master.h"

#include "c28/pins_def.h"     // DSP281x Headerfile Include File
#include "DSP2833x_device.h"
#include "DSP2833x_GlobalPrototypes.h"
#include "DSP2833x_Examples.h"

void spi_master_t::init() {
	ring_buffer_init(&ring_buf,(u8_t*)_buf, SPI_BUFF_SIZE);

    EALLOW;

    GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0; // Enable pull-up on GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0; // Enable pull-up on GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0; // Enable pull-up on GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0; // Enable pull-up on GPIO19 (SPISTEA)

    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3; // Asynch input GPIO16 (SPISIMOA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3; // Asynch input GPIO17 (SPISOMIA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18 = 3; // Asynch input GPIO18 (SPICLKA)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3; // Asynch input GPIO19 (SPISTEA)

    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 1;
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;

    GpioCtrlRegs.GPADIR.bit.PIN_SPI_CS = 1;
    GpioCtrlRegs.GPADIR.bit.PIN_SPI_D0 = 1; // PIN_SPI_D0  out
    GpioCtrlRegs.GPADIR.bit.PIN_SPI_D1 = 1; // PIN_SPI_D1  out
    GpioCtrlRegs.GPADIR.bit.PIN_SPI_D2 = 1; // PIN_SPI_D2  out

    EDIS;

    channel = FRAM;
    Setup(channel);

    SpiaRegs.SPICCR.bit.SPISWRESET  = 0;            // Reset SPI
    SpiaRegs.SPICTL.all             = 0x06;         // (MASTER(bit 2) TALK (bit 1))
    SpiaRegs.SPICCR.all             = 7;            // character len = 8 bit.
    SpiaRegs.SPIFFTX.all            = 0xE040;       // TX FIFO settings
    SpiaRegs.SPIFFRX.all            = 0x2040;       // RX FIFO settings
    SpiaRegs.SPIFFCT.all            = 0;            // tramsmit delay 0
    SpiaRegs.SPIPRI.bit.FREE        = 1;
    SpiaRegs.SPICCR.bit.SPISWRESET  = 1;            // SPI to work mode

    SpiaRegs.SPIFFTX.bit.TXFIFO     = 1;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;
}




//#pragma CODE_SECTION ("ramfuncs")
void spi_master_t::Setup(eChannel channel)
{
    EALLOW;
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;

    //set spi channel
    GpioDataRegs.GPACLEAR.all = 0x00D00000;
    GpioDataRegs.GPASET.all = /*GpioDataRegs.GPASET.all |*/ (u32) channel;
    const u16 spi_brr = 50;
    switch (channel)
    {
        case FRAM:
            SpiaRegs.SPICCR.all = 7; // SPI transactions are divisible by 8 bits
            SpiaRegs.SPICTL.all = 0x0E;
            SpiaRegs.SPIBRR = spi_brr;
            break;
        case Flash:
            SpiaRegs.SPICCR.all = 7; // SPI transactions are divisible by 8 bits
            SpiaRegs.SPICTL.all = 0x0E;
            SpiaRegs.SPIBRR = spi_brr;
            break;
        case DDS:
            SpiaRegs.SPICCR.all = 7; // SPI transactions are divisible by 8 bits
            SpiaRegs.SPICTL.all = 0x0E;
            SpiaRegs.SPIBRR = spi_brr;
            break;
        case DAC:
            SpiaRegs.SPICCR.all = 11; // SPI transactions are divisible by 12 bits
            SpiaRegs.SPICTL.all = 0x0E;
            SpiaRegs.SPIBRR = spi_brr;
            break;
        case Heterodin_G:
            SpiaRegs.SPICCR.all = 7; // SPI transactions are divisible by 8 bits
            SpiaRegs.SPICTL.all = 0x0E;
            SpiaRegs.SPIBRR = spi_brr;
            break;
        case Heterodin_L:
            SpiaRegs.SPICCR.all = 7; // SPI transactions are divisible by 8 bits
            SpiaRegs.SPICTL.all = 0x0E;
            SpiaRegs.SPIBRR = spi_brr;
            break;
        case DET2_CS:
            SpiaRegs.SPICCR.all = 7; // SPI transactions are divisible by 8 bits
            SpiaRegs.SPICTL.all = 0x0F;
            SpiaRegs.SPIBRR = spi_brr;
            break;
        case DET1_CS:
            SpiaRegs.SPICCR.all = 7; // SPI transactions are divisible by 8 bits
            SpiaRegs.SPICTL.all = 0x0F;
            SpiaRegs.SPIBRR = spi_brr;
            break;
    }

    SpiaRegs.SPICCR.bit.SPISWRESET = 1;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 0;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;
    EDIS;
}


void spi_master_t::write(u8 byte) {

    while (SpiaRegs.SPIFFTX.bit.TXFFST >= 15) {

        while (SpiaRegs.SPIFFRX.bit.RXFFST)
        {
            u16 byte = SpiaRegs.SPIRXBUF;
            ring_buffer_write_byte(&ring_buf,byte);
        }

    }

    SpiaRegs.SPITXBUF = byte << 8;                  // сдвиг обусловлен особенностю TX SPI (left aligned)

    while (SpiaRegs.SPIFFRX.bit.RXFFST)
    {
        u16 byte = SpiaRegs.SPIRXBUF;
        ring_buffer_write_byte(&ring_buf,byte);
    }

}

