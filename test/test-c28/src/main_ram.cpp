#include <stdlib.h>
#include <stdio.h>

#include "DSP2833x_device.h"
#include "DSP2833x_GlobalPrototypes.h"
#include "DSP2833x_CpuTimers.h"
#include "DSP2833x_Examples.h"
#include "DSP2833x_SysCtrl.h"
#include "cpy_tbl.h"
#include <new.h>
#include "some_funcs.h"
#include "pins_def.h"

#include "types.h"
#include "assert_impl.h"

#include "ssd1963drv.h"

void testPins();

extern "C" void SSD1963_InitHW();
bool isExtRamOK2(u16 *ext_ram_mas, s32 sz);
bool isExtRamOK();
//extern "C" void minit();
extern "C" void *malloc(size_t size);

myvi::ssd1963drv_t drv1;



int main(void)
{	
    InitSysCtrl();

    init_zone7();

    SSD1963_InitHW();

//    testPins();
//    if(!isExtRamOK())
//    	asm(" ESTOP0");
//    minit();
//#define ALLOC_SZ 5
//    u16 *buf = new u16[ALLOC_SZ];
//    u16 *buf = (u16 *)malloc(ALLOC_SZ);
//    if(!isExtRamOK2(buf,ALLOC_SZ))
//    	asm(" ESTOP0");

    drv1.init();
    u32 color[] = {0x00, 0xffffff};
    int i = 0;
    while(1) {
    	i++;
    	i = i & 0x01;
    	drv1.ctx.pen_color = color[i];
    	drv1.fill(0,0,320,240);
    }

    while(1) asm(" ESTOP0");
}
