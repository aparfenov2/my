#include "DSP2833x_device.h"
#include "c28/pins_def.h"

#include "types.h"

void testPins() {

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

    dir.all = GpioDataRegs.GPBDAT.all;

    dir.bits.PIN_DC   = 0;
    dir.bits.PIN_WR   = 0;
    dir.bits.PIN_RD   = 0;
    dir.bits.dat      = 0;
    dir.bits.PIN_CS   = 0;
    dir.bits.PIN_RST  = 0;
    dir.bits.PIN_UD   = 0;
	GpioDataRegs.GPBDAT.all = dir.all;
	GpioDataRegs.GPACLEAR.bit.PIN_RL = 1;

    dir.bits.PIN_DC   = 1;
    dir.bits.PIN_WR   = 1;
    dir.bits.PIN_RD   = 1;
    dir.bits.dat   = 0xff;
    dir.bits.PIN_CS   = 1;
    dir.bits.PIN_RST   = 1;
    dir.bits.PIN_UD   = 1;
	GpioDataRegs.GPBDAT.all = dir.all;
	GpioDataRegs.GPASET.bit.PIN_RL = 1;


}

/*
#pragma DATA_SECTION ("ext_ram")
u16 ext_ram_mas[5];

bool isExtRamOK()
{

    // инициализирую массив
    for (int ii = 0; ii < sizeof(ext_ram_mas); ++ii)
        ext_ram_mas[ii]=ii;

    // осуществляю проверку
    for (int ii = 0; ii < sizeof(ext_ram_mas); ++ii)
        if (ext_ram_mas[ii] != ii)
            return false;

    return true;
}
*/

bool isExtRamOK2(u16 *ext_ram_mas, s32 sz)
{

    // инициализирую массив
    for (int ii = 0; ii < sz; ++ii)
        ext_ram_mas[ii]=ii;

    // осуществляю проверку
    for (int ii = 0; ii < sz; ++ii)
        if (ext_ram_mas[ii] != ii)
            return false;

    return true;
}

