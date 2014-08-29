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
#include "c28/pins_def.h"

#include "types.h"
#include "assert_impl.h"


extern COPY_TABLE bootTimeTable;
void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);
void my_main();
extern "C" void SSD1963_InitHW();



int main(void)
{	
    InitSysCtrl();

    std::copy_in(&bootTimeTable);

    // ВАЖНО!!! функцию по инициализации флеши вызывать после c_int00 и после std::copy_in(&bootTimeTable)
    InitFlash();

    hw::init_zone7();

    SSD1963_InitHW();

    my_main();

    while(1) asm(" ESTOP0");
}
