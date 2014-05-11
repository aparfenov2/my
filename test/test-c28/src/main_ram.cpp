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
#include "exported2_impl.h"

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

char buffer[256];

void uart_print(const char *format, ...) {
	va_list args;
	va_start (args, format);
	int w = vsnprintf (buffer,256, format, args);
	for (int i=0; i<w; i++) {
		uart.write(buffer[i]);
	}
	va_end (args);
}

int main(void)
{
    InitSysCtrl();

    init_zone7();

    kbd_init();
    enc_init();
    uart.init(&ScibRegs);
    init_pie_table();



    key_t::key_t lk;

	while(1) {
		s16 enc_cnt = enc_reset_counter();
		key_t::key_t k = kbd_get_key();
		if (lk != k) {
			lk = k;
			if (k) {
				uart_print("%d\n\r",k);
			}
		}
	}
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


    u8 *ttcache_dat;
    u32 ttcache_sz;

    _MY_ASSERT(read_file_table(),);
//    _MY_ASSERT(allocate_and_read_font_cache(ttcache_dat,ttcache_sz),);

//	globals::ttcache.init((u8 *)ttcache_dat,ttcache_sz);

//	res.init();

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

    u32 picbuf_sz = BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,24);
    u8 *picbuf = new u8[picbuf_sz];
    surface_24bpp_t spic(TFT_WIDTH,TFT_HEIGHT,picbuf_sz, picbuf);

//    u8 *buf0 = new u8[BMP_GET_SIZE_16(TFT_WIDTH,TFT_HEIGHT)];
//    surface_16bpp_t s1(TFT_WIDTH,TFT_HEIGHT,BMP_GET_SIZE_16(TFT_WIDTH,TFT_HEIGHT), buf0);

    drv1.set_allowed_area(0,0,TFT_WIDTH,TFT_HEIGHT);
//    s1.set_allowed_area(0,0,TFT_WIDTH,TFT_HEIGHT);
    spic.set_allowed_area(0,0,TFT_WIDTH,TFT_HEIGHT);


	file_rec_t *fr = find_file(LOGO_FILE_ID);
	_MY_ASSERT(fr, return false);
	u32 pic_sz = fr->cur_len;
	_MY_ASSERT(pic_sz && pic_sz <= picbuf_sz, return false);

	flash.ReadData2(fr->offset,(u16 *)picbuf, picbuf_sz);


	spic.copy_to(0,0,-1,-1,0,0,drv1);
    while(1) asm(" ESTOP0");

}

/*
	while(1) {
		s1.ctx.pen_color = 0;
		s1.fill(0,0,TFT_WIDTH,TFT_HEIGHT);

		s1.ctx.pen_color = 0x00ff00;
		res.ttf.set_char_size_px(0,font_size_t::FS_20);

		s16 enc_cnt = enc_reset_counter();

		char sbuf[255];
		snprintf(sbuf, 255, "%d", enc_cnt);

		res.ttf.print_to(20,20,s1,sbuf);

		s1.copy_to(0,0,-1,-1,0,0,drv1);

//		asm(" ESTOP0");
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

*/
