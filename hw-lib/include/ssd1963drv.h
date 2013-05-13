/*
 * ssd1963drv.h
 *
 *  Created on: 20.11.2012
 *      Author: Parfenov
 */

#ifndef SSD1963DRV_H_
#define SSD1963DRV_H_

#include "surface.h"

namespace myvi {
class ssd1963drv_t : public surface_t {
private:
	s32 bpsx;
	s32 bpsy;
	s32 bpsw;
	s32 bpsh;
public:
	ssd1963drv_t():surface_t(320,240,-1,(u8*)(-1),img_type_t::bppGeneric),bpsx(-1) {
	}

	void init();
// pixel interface
	virtual void putpx(s32 x1,s32 y1, u32 c);
	virtual u32  getpx(s32 x1,s32 y1);
	virtual bp_return_t::bp_return_t begin_paint(surface_t &src, s32 sx,s32 sy,s32 sw,s32 sh,s32 dx,s32 dy);
	virtual void end_paint();
// can be further optimized
//	virtual void copyTo(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst);
//	virtual void line(s32 x1,s32 y1, s32 len, bool isVertical);
	virtual void fill(s32 x1,s32 y1, s32 w, s32 h);
};

} // ns
#endif /* SSD1963DRV_H_ */
