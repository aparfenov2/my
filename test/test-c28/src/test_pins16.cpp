#include "DSP2833x_device.h"
#include "DSP2833x_GlobalPrototypes.h"
#include "DSP2833x_CpuTimers.h"
#include "DSP2833x_Examples.h"
#include "DSP2833x_SysCtrl.h"

#include "types.h"

#define GPIO_WRA(R,v) \
    R##.bit.GPIO5 = v; \
	R##.bit.GPIO7 = v; \
	R##.bit.GPIO15 = v; \
	R##.bit.GPIO16 = v; \
	R##.bit.GPIO17 = v; \
	R##.bit.GPIO18 = v; \
	R##.bit.GPIO19 = v; \
	R##.bit.GPIO21 = v; \
	R##.bit.GPIO22 = v; \
	R##.bit.GPIO28 = v; \
	R##.bit.GPIO29 = v; \
	R##.bit.GPIO30 = v;

#define GPIO_WRB(R,v) \
    R##.bit.GPIO32 = v; \
	R##.bit.GPIO34 = v; \
	R##.bit.GPIO35 = v; \
	R##.bit.GPIO57 = v; \
	R##.bit.GPIO58 = v; \
	R##.bit.GPIO59 = v; \
	R##.bit.GPIO48 = v; \
	R##.bit.GPIO49 = v; \
	R##.bit.GPIO50 = v; \
	R##.bit.GPIO51 = v; \
	R##.bit.GPIO52 = v; \
	R##.bit.GPIO53 = v; \
	R##.bit.GPIO54 = v; \
	R##.bit.GPIO55 = v; \
	R##.bit.GPIO60 = v; \
	R##.bit.GPIO61 = v; \
	R##.bit.GPIO36 = v;

void testPins16() {

    EALLOW;
    GPIO_WRA(GpioCtrlRegs.GPADIR,1)
    GPIO_WRB(GpioCtrlRegs.GPBDIR,1)
    EDIS;

beg:
    GPIO_WRA(GpioDataRegs.GPASET,1)
    GPIO_WRB(GpioDataRegs.GPBSET,1)

//    for (int i=0; i< 0xFFFF; i++);

    GPIO_WRA(GpioDataRegs.GPACLEAR,1)
    GPIO_WRB(GpioDataRegs.GPBCLEAR,1)

//    for (int i=0; i< 0xFFFF; i++);
//    goto beg;

beg2:
	GpioDataRegs.GPASET.bit.GPIO5 = 1; //A25
	GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
	GpioDataRegs.GPASET.bit.GPIO7 = 1; //A24
	GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
	GpioDataRegs.GPASET.bit.GPIO15 = 1; //C31
	GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;
	GpioDataRegs.GPASET.bit.GPIO16 = 1; //A20
	GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;
	GpioDataRegs.GPASET.bit.GPIO17 = 1; //A21
	GpioDataRegs.GPACLEAR.bit.GPIO17 = 1;
	GpioDataRegs.GPASET.bit.GPIO18 = 1; //A22
	GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;
	GpioDataRegs.GPASET.bit.GPIO19 = 1; //A23
	GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;
	GpioDataRegs.GPASET.bit.GPIO21 = 1; //A27
	GpioDataRegs.GPACLEAR.bit.GPIO21 = 1;
	GpioDataRegs.GPASET.bit.GPIO22 = 1; //A28
	GpioDataRegs.GPACLEAR.bit.GPIO22 = 1;
	GpioDataRegs.GPASET.bit.GPIO28 = 1; //A18
	GpioDataRegs.GPACLEAR.bit.GPIO28 = 1;
	GpioDataRegs.GPASET.bit.GPIO29 = 1; //A19
	GpioDataRegs.GPACLEAR.bit.GPIO29 = 1;

	GpioDataRegs.GPBSET.bit.GPIO32 = 1; //A29
	GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;
	GpioDataRegs.GPBSET.bit.GPIO34 = 1; //C29
	GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
	GpioDataRegs.GPBSET.bit.GPIO35 = 1; //C30
	GpioDataRegs.GPBCLEAR.bit.GPIO35 = 1;

	GpioDataRegs.GPBSET.bit.GPIO57 = 1; //C3
	GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;
	GpioDataRegs.GPBSET.bit.GPIO58 = 1; //C4
	GpioDataRegs.GPBCLEAR.bit.GPIO58 = 1;
	GpioDataRegs.GPBSET.bit.GPIO59 = 1; //C5
	GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1;
	GpioDataRegs.GPBSET.bit.GPIO48 = 1; //C20
	GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;
	GpioDataRegs.GPBSET.bit.GPIO49 = 1; //C21
	GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
	GpioDataRegs.GPBSET.bit.GPIO50 = 1; //C22
	GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
	GpioDataRegs.GPBSET.bit.GPIO51 = 1; //C23
	GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;
	GpioDataRegs.GPBSET.bit.GPIO52 = 1; //C24
	GpioDataRegs.GPBCLEAR.bit.GPIO52 = 1;
	GpioDataRegs.GPBSET.bit.GPIO53 = 1; //C25
	GpioDataRegs.GPBCLEAR.bit.GPIO53 = 1;
	GpioDataRegs.GPBSET.bit.GPIO54 = 1; //C26
	GpioDataRegs.GPBCLEAR.bit.GPIO54 = 1;
	GpioDataRegs.GPBSET.bit.GPIO55 = 1; //C27
	GpioDataRegs.GPBCLEAR.bit.GPIO55 = 1;
	GpioDataRegs.GPBSET.bit.GPIO60 = 1; //C8
	GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;
	GpioDataRegs.GPBSET.bit.GPIO61 = 1; //C9
	GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;
	GpioDataRegs.GPASET.bit.GPIO30 = 1; //C18
	GpioDataRegs.GPACLEAR.bit.GPIO30 = 1;
	GpioDataRegs.GPBSET.bit.GPIO36 = 1; //C19
	GpioDataRegs.GPBCLEAR.bit.GPIO36 = 1;

    goto beg2;

    while(1) asm(" ESTOP0");
}


int main(void) {
    InitSysCtrl();
    testPins16();
    while(1) asm(" ESTOP0");
}
