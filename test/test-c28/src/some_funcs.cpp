/*
 * some_funcs.cpp
 *
 *  Created on: 02.04.2012
 *      Author: pushkarev
 */

#include "some_funcs.h"
#include "DSP2833x_device.h"
#include "c28/pins_def.h"
#include "DSP2833x_Examples.h"
#include "macro.h"
#include <stdio.h>
#include <string.h>

using std::sprintf;
using std::memset;

//#ifdef __cplusplus
//extern "C" {
//#endif

// В данном файле находится весь мусор из main

//=============================================================================
/// Инициализация интерфейса для работы с внешним ОЗУ
//=============================================================================
void init_zone7(void)
{

    // Configure the GPIO for XINTF with a 16-bit data bus
    // This function is in DSP2833x_Xintf.c
    EALLOW;

    // Make sure the XINTF clock is enabled
    SysCtrlRegs.PCLKCR3.bit.XINTFENCLK = 1;

    GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 3;  // XD15
    GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 3;  // XD14
    GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 3;  // XD13
    GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 3;  // XD12
    GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 3;  // XD11
    GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 3;  // XD10
    GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 3;  // XD19
    GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 3;  // XD8
    GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 3;  // XD7
    GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 3;  // XD6
    GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 3;  // XD5
    GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 3;  // XD4
    GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 3;  // XD3
    GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 3;  // XD2
    GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 3;  // XD1
    GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 3;  // XD0

    GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 3;  // XA0/XWE1n
    GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 3;  // XA1
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 3;  // XA2
    GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 3;  // XA3
    GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 3;  // XA4
    GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 3;  // XA5
    GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 3;  // XA6
    GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 3;  // XA7

    GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 3;  // XA8
    GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 3;  // XA9
    GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 3;  // XA10
    GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 3;  // XA11
    GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 3;  // XA12
    GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 3;  // XA13
    GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 3;  // XA14
    GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 3;  // XA15
    GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 3;  // XA16
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 3;  // XA17
    GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 3;  // XA18
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 3;  // XA19

    //GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 3;  // XREADY
    //GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 3;  // XRNW
    GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 3;  // XWE0

    //GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 3;  // XZCS0
    GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 3;  // XZCS7
    //GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 3;  // XZCS6
    EDIS;

    EALLOW;
    // All Zones---------------------------------
    // Timing for all zones based on XTIMCLK = SYSCLKOUT
    XintfRegs.XINTCNF2.bit.XTIMCLK = 0;
    // No write buffering
    XintfRegs.XINTCNF2.bit.WRBUFF = 0;
    // XCLKOUT is enabled
    XintfRegs.XINTCNF2.bit.CLKOFF = 0;
    // XCLKOUT = XTIMCLK
    XintfRegs.XINTCNF2.bit.CLKMODE = 0;

    // Zone will not sample XREADY signal
    XintfRegs.XTIMING7.bit.USEREADY = 0;
    XintfRegs.XTIMING7.bit.READYMODE = 0;

    // Zone 7------------------------------------
    // When using ready, ACTIVE must be 1 or greater
    // Lead must always be 1 or greater
    // Zone write timing
    XintfRegs.XTIMING7.bit.XWRLEAD = 1;
    XintfRegs.XTIMING7.bit.XWRACTIVE = 1;
    XintfRegs.XTIMING7.bit.XWRTRAIL = 0;
    // Zone read timing
    XintfRegs.XTIMING7.bit.XRDLEAD = 1;
    XintfRegs.XTIMING7.bit.XRDACTIVE = 1;
    XintfRegs.XTIMING7.bit.XRDTRAIL = 0;

    // don't double all Zone read/write lead/active/trail timing
    XintfRegs.XTIMING7.bit.X2TIMING = 0;

    // 1,1 = x16 data bus
    // 0,1 = x32 data bus
    // other values are reserved
    XintfRegs.XTIMING7.bit.XSIZE = 3;
    EDIS;

    //Force a pipeline flush to ensure that the write to
    //the last register configured occurs before returning.
    asm(" RPT #7 || NOP");
}


void dsbl_dog(void)
{
    EALLOW;
    SysCtrlRegs.WDCR = 0x0068;
    EDIS;
}

//=============================================================================
/// Инициализация PLL
//=============================================================================
void init_pll(unsigned int val)
{
    volatile Uint16 iVol;

    if (SysCtrlRegs.PLLCR.bit.DIV != val)
    {

        EALLOW;
        SysCtrlRegs.PLLCR.bit.DIV = val;
        EDIS;

        // Optional: Wait for PLL to lock.
        // During this time the CPU will switch to OSCCLK/2 until the PLL is
        // stable.  Once the PLL is stable the CPU will switch to the new PLL value.
        //
        // This switch time is 131072 CLKIN cycles as of Rev C silicon.
        //
        // Code is not required to sit and wait for the PLL to lock.
        // However, if the code does anything that is timing critical,
        // and requires the correct clock be locked, then it is best to
        // wait until this switching has completed.

        // If this function is run from waitstated memory, then the loop count can
        // be reduced as long as the minimum switch time is still met.

        // iVol is volatile so the compiler will not optimize this loop out
        //
        // The watchdog should be disabled before this loop, or fed within
        // the loop.

        dsbl_dog();

        // Wait lock cycles.
        // Note,  This loop is tuned to 0-waitstate RAM memory.  If this
        // function is run from wait-stated memory such as Flash or XINTF,
        // then the number of times through the loop can be reduced
        // accordingly.
        for (iVol = 0; iVol < ((131072 / 2) / 12); iVol++)
        {

        }
    }
}


// This function initializes the clocks to the peripheral modules.
// First the high and low clock prescalers are set
// Second the clocks are enabled to each peripheral.
// To reduce power, leave clocks to unused peripherals disabled
// Note: If a peripherals clock is not enabled then you cannot
// read or write to the registers for that peripheral
void init_perif_clock(void)
{
    EALLOW;
// HISPCP/LOSPCP prescale register settings, normally it will be set to default values
    SysCtrlRegs.HISPCP.all = 0x0000;
    SysCtrlRegs.LOSPCP.all = 0x0000;

// Peripheral clock enables set for the selected peripherals.
    //SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;
    //SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;
    //SysCtrlRegs.PCLKCR0.bit.SCIAENCLK=1;
    SysCtrlRegs.PCLKCR0.bit.SCIBENCLK = 1;
    SysCtrlRegs.PCLKCR0.bit.SCICENCLK = 1;   // SCI-C
    SysCtrlRegs.PCLKCR0.bit.MCBSPAENCLK = 1;
    SysCtrlRegs.PCLKCR0.bit.MCBSPBENCLK = 1;
    SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;
    SysCtrlRegs.PCLKCR0.bit.ECANAENCLK = 0;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 0;
    SysCtrlRegs.PCLKCR3.bit.DMAENCLK = 1;
    SysCtrlRegs.PCLKCR3.bit.XINTFENCLK = 1;

    EDIS;
}

////=============================================================================
//// Инициализация портов ввода/вывода
////=============================================================================
void initGPIO()
{
    EALLOW;
    GpioCtrlRegs.GPADIR.bit.PIN_CMTR_CLK_O  = 1;
    GpioCtrlRegs.GPADIR.bit.PIN_CMTR_DATA_O = 1;
    GpioCtrlRegs.GPADIR.bit.PIN_CMTR_WR_O   = 1;
    GpioCtrlRegs.GPADIR.bit.PIN_CMTR_DIR    = 1;
    GpioCtrlRegs.GPADIR.bit.PIN_INP_CMTR    = 1;
    GpioCtrlRegs.GPADIR.bit.PIN_LG          = 1;
    GpioCtrlRegs.GPADIR.bit.PIN__LG         = 1;
    GpioCtrlRegs.GPADIR.bit.PIN_REC_SYNC    = 1;
    GpioCtrlRegs.GPADIR.bit.PIN__DAC_LD     = 1;
    GpioCtrlRegs.GPADIR.bit.PIN_HET_STATE   = 0;
    GpioCtrlRegs.GPADIR.bit.PIN_DDS_RST     = 1;

    GpioCtrlRegs.GPADIR.bit.PIN_M25_MOUT    = 1;
    GpioCtrlRegs.GPADIR.bit.PIN_M25_MIN     = 0;
    GpioCtrlRegs.GPADIR.bit.PIN_M25_CLK     = 1;
    GpioCtrlRegs.GPADIR.bit.PIN__M25_CS     = 1;

    GpioCtrlRegs.GPBDIR.bit.PIN_PIN_WIDE   = 1;
    GpioCtrlRegs.GPBDIR.bit.PIN_PIN_NARR   = 1;
    EDIS;
}

void initTest()
{
//    if (!isExtRamOK())
//        _ASSERT(0);
}
