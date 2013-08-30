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

#include "ssd1963drv.h"
#include "surface.h"
#include "bmp_math.h"

//void testPins();

extern "C" void SSD1963_InitHW();
//bool isExtRamOK2(u16 *ext_ram_mas, s32 sz);
//bool isExtRamOK();
//extern "C" void minit();
//extern "C" void *malloc(size_t size);

using namespace myvi;

ssd1963drv_t drv1;

extern "C" {
	void SSD1963_WriteCommand(u8 commandToWrite);
	void SSD1963_WriteData(u8 dataToWrite);
}

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
#define W 20
#define H 20

//	s32 buf_sz = BMP_GET_SIZE_16(W,H);
//	u8 *buf0 = new u8[buf_sz];
//	surface_16bpp_t sa16(W,H,buf_sz, buf0);

    drv1.init();

//0x203E95;
//	sa16.ctx.pen_color = 0x292929;
//	sa16.fill(0,0,W,H);
//	sa16.copy_to(0,0,-1,-1,0,0,drv1);


	while(1)
	for (s32 s=0; s<3; s++ ) {

		drv1.ctx.pen_color = 0;
		drv1.fill(0,0,TFT_WIDTH,TFT_HEIGHT);

		s32 x = 0, y = 0;
		for (s32 i=0; i<256; i++) {

			drv1.ctx.pen_color = i << (s * 8);
			drv1.fill(x,y,W,H);

			x += W+1;
			if (x+W > TFT_WIDTH) {
				x = 0;
				y += H+1;
				if (y+H > TFT_HEIGHT)
					break;
			}
		}
		asm(" ESTOP0");
	}

    while(1) asm(" ESTOP0");

//    u32 color[] = {0x00, 0xffffff};
//    int i = 0;
//    while(1) {
//    	i++;
//    	i = i & 0x01;
//    	drv1.ctx.pen_color = color[i];
//    	drv1.fill(0,0,320,240);
//    }
//
//    while(1) asm(" ESTOP0");
}
