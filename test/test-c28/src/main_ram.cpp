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

#include "app_events.h"

#include "devices.h"
#include "Spi.h"
#include "uart_drv.h"

#include "ttcache.h"
#include "resources.h"

#include "file_map.h"

#include "widgets.h"

//void testPins();

extern "C" void SSD1963_InitHW();
//bool isExtRamOK2(u16 *ext_ram_mas, s32 sz);
//bool isExtRamOK();
//extern "C" void minit();
//extern "C" void *malloc(size_t size);

using namespace myvi;

ssd1963drv_t drv1;

Spi spi;
FRAM fram;
FlashDev flash;
uart_drv_t	uart;

extern resources_t res;




int main(void)
{
    InitSysCtrl();

    init_zone7();

    kbd_init();

//    while(1) {
//    	key_t::key_t k = kbd_get_key();
//    	if (k) {
//    		asm(" ESTOP0");
//    	}
//    }

// ram bug test
//    u8 *p1 = (u8*)0x002026B8;
//    u8 *p2 = (u8*)0x002426B8;
//    *p1 = 0xabba;
//    *p2 = 0xcafe;
//    _MY_ASSERT(*p1 != *p2,);
//    while(1) asm(" ESTOP0");


    SSD1963_InitHW();

    spi.Init();

    u32 ft_magic = 0;

    for (u32 ofs = 0; ofs < sizeof(ft_magic); ofs++) {
    	((u16*)&ft_magic)[ofs] = flash.ReadData(FILE_TABLE_ADDR + (ofs*2));
    }

    if (ft_magic == FILE_TABLE_MAGIC) {
        for (u32 ofs = 0; ofs < sizeof(file_table); ofs++) {
        	((u16*)&file_table)[ofs] = flash.ReadData(FILE_TABLE_ADDR + (ofs*2));
        }
    }

	file_rec_t *fr = find_file(TTCACHE_FILE_ID);
	_MY_ASSERT(fr,);
	u32 ttcache_sz = fr->cur_len;
	_MY_ASSERT(ttcache_sz,);

	u8 *ttcache_dat = new u8[ttcache_sz + 0x0f];

#define ALIGNED(ptr) (!(0x03 & (u32)(ptr)))
	while (!ALIGNED(ttcache_dat)) {
		ttcache_dat++;
	}

	flash.ReadData2(fr->offset,(u16 *)ttcache_dat, ttcache_sz);

	globals::ttcache.init((u8 *)ttcache_dat,ttcache_sz);

	res.init();

// uart echo test
//    uart.init(&ScibRegs);
//	init_pie_table();
//
//	while (1) {
//		while (!uart.is_empty()) {
//
//			u8 byte = uart.read();
//			uart.write(byte);
//		}
//	}


// display palete test
#define W 20
#define H 20


    drv1.init();

    u8 *buf0 = new u8[BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16)];
    surface_16bpp_t s1(TFT_WIDTH,TFT_HEIGHT,BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16), buf0);

    drv1.set_allowed_area(0,0,TFT_WIDTH,TFT_HEIGHT);
    s1.set_allowed_area(0,0,TFT_WIDTH,TFT_HEIGHT);

//0x203E95;
//	sa16.ctx.pen_color = 0x292929;
//	sa16.fill(0,0,W,H);
//	sa16.copy_to(0,0,-1,-1,0,0,drv1);


//    while(1) asm(" ESTOP0");

	while(1) {
		s1.ctx.pen_color = 0;
		s1.fill(0,0,TFT_WIDTH,TFT_HEIGHT);

		s1.ctx.pen_color = 0x00ff00;
		res.ttf.set_char_size_px(0,font_size_t::FS_20);
		res.ttf.print_to(20,20,s1,"›‘‘≈ “»¬ÕŒ—“‹");

		s1.copy_to(0,0,-1,-1,0,0,drv1);

		asm(" ESTOP0");
	}


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
