/*
 * ssd1963_drv.cpp
 *
 *  Created on: 20.11.2012
 *      Author: gordeev
 */

#include "surface.h"
#include "ssd1963.h"
#include "assert_impl.h"
#include "ssd1963drv.h"
#include "bmp_math.h"

using namespace myvi;

extern "C" {
	void SSD1963_Reset();
	void SSD1963_Init();
	void SSD1963_FillArea(u16 sx, u16 ex, u16 sy, u16 ey, u32 color);
	void GLCD_SetPixel(u16 x, u16 y, u32 color);
	void SSD1963_SetArea(u16 sx, u16 ex, u16 sy, u16 ey);
	void SSD1963_WriteCommand(u8 commandToWrite);
	void SSD1963_WriteData(u8 dataToWrite);
	void SSD1963_WriteColor(u32 color);
}

void ssd1963drv_t::init() {
	SSD1963_Reset();
	SSD1963_Init();
//	u32 old_color = ctx.pen_color;
//	ctx.pen_color = 0x000000;
//	fill(0,0,w,h);
//	ctx.pen_color = old_color;
}

bp_return_t::bp_return_t ssd1963drv_t::begin_paint(surface_t &src, s32 sx,s32 sy, s32 sw,s32 sh, s32 dx,s32 dy) {
	if (ctx.alfa != 0xff) {
		// режим оверлея пропускает пиксели, поэтому ускориться не удастся
		return bp_return_t::bpOk;
	}
	bpsx = dx;
	bpsy = dy;
	bpsw = sw;
	bpsh = sh;
	SSD1963_SetArea(dx,dx+sw-1,dy,dy+sh-1);
	SSD1963_WriteCommand(SSD1963_WRITE_MEMORY_START);

//WARNING: bpp24 will only work with 8 bit mode

	if (!src.ctx.mode && src.itype == img_type_t::bpp24 && src.w == sw && sx == 0) {
#ifdef RGB565_MODE
		_MY_ASSERT(0,return);
#endif
		// source is RGB, full width,
		u8 *p = &src.buf[BMP_GET_OFFS(sx,sy,src.w, 24)];
		u8 *pend = &src.buf[BMP_GET_OFFS(sx,sy+sh,src.w, 24)];
		for (; p < pend; p++) {
			SSD1963_WriteData(*p);
		}
		return bp_return_t::bpBecomeMaster;

	} else if (!src.ctx.mode && src.itype == img_type_t::bpp16 && src.w == sw && sx == 0) {
			// source is RGB, full width,
			u8 *p = &src.buf[BMP_GET_OFFS_16(sx,sy,src.w)];
			u8 *pend = &src.buf[BMP_GET_OFFS_16(sx,sy+sh,src.w)];
			for (; p < pend; p += sizeof(u16)) {
				SSD1963_WriteData(*p);
			}
			return bp_return_t::bpBecomeMaster;

	} else if (!src.ctx.mode && src.itype == img_type_t::bpp24) {
#ifdef RGB565_MODE
		_MY_ASSERT(0,return);
#endif
		// row by row copy
		for (s32 r=0; r < sh; r++) {
			u8 *p = &src.buf[BMP_GET_OFFS(sx,sy+r,src.w, 24)];
			u8 *pend = &src.buf[BMP_GET_OFFS(sx+sw,sy+r,src.w, 24)];
			for (; p < pend; p++) {
				SSD1963_WriteData(*p);
			}
		}
		return bp_return_t::bpBecomeMaster;

	} else if (!src.ctx.mode && src.itype == img_type_t::bpp16) {
		// row by row copy
		for (s32 r=0; r < sh; r += sizeof(u16)) {
			u8 *p = &src.buf[BMP_GET_OFFS_16(sx,sy+r,src.w)];
			u8 *pend = &src.buf[BMP_GET_OFFS_16(sx+sw,sy+r,src.w)];
			for (; p < pend; p++) {
				SSD1963_WriteData(*p);
			}
		}
		return bp_return_t::bpBecomeMaster;
	}
	return bp_return_t::bpOk;
}

void ssd1963drv_t::end_paint() {
	bpsx = -1;
	SSD1963_WriteCommand(0x0);
}

// pixel interface
void ssd1963drv_t::putpx(s32 x1,s32 y1, u32 c) {
	if (bpsx >= 0) {
		// begin_paint called
		SSD1963_WriteColor(c);
	} else {
		// no streamlined putpx
		GLCD_SetPixel(x1,y1,c);
	}
}
u32  ssd1963drv_t::getpx(s32 x1,s32 y1) {
	_MY_ASSERT(0, return 0);
	return 0;
}
// can be further optimized
//	virtual void copyTo(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst, copy_mode_t useAlfa);
//	virtual void line(s32 x1,s32 y1, s32 len, bool isVertical);
void ssd1963drv_t::fill(s32 x1,s32 y1, s32 w, s32 h) {
	SSD1963_FillArea(x1,x1+w-1,y1,y1+h-1,ctx.pen_color);
}
