/*
 * surface.cpp
 *
 *  Created on: 13.11.2012
 *      Author: Parfenov
 */

#include "surface.h"
#include "bmp_math.h"
#include "assert_impl.h"

using namespace myvi;

// ------------------------------------------------------------------------
// --------------------------- surface_t ----------------------------------
// ------------------------------------------------------------------------

void surface_t::copy_to(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst) {
	_MY_ASSERT(buf_sz,return);
	if (sw <= 0 && sh <= 0) {
		sw = w;
		sh = h;
	}
	// self
	_MY_ASSERT(w > 0 && h > 0,return);
	// params
	_MY_ASSERT(sx >=0 && sy >= 0 && sw >0 && sh > 0 && sx +sw <= w && sy + sh <= h,return);
	// dst
	_MY_ASSERT(dst.w > 0 && dst.h > 0 && dst.buf,return);
	_MY_ASSERT(dx >= 0 && dy >= 0 && dx + sw <= dst.w && dy + sh <= dst.h,return);

	if (dst.begin_paint(*this,sx,sy,sw,sh, dx,dy) == 0) {

		for (s32 ssy = sy, ddy = dy; ssy < sy + sh; ssy++, ddy++) {
			for (s32 ssx = sx, ddx = dx; ssx < sx + sw; ssx++, ddx++) {
				u32 c = this->getpx(ssx,ssy);
				putpx_to(dst,ddx,ddy,c);
			}
		}

	}
	dst.end_paint();
}

void surface_t::putpx_to(surface_t &dst, s32 ddx,s32 ddy, u32 c) {
	if ((ctx.mode & copy_mode_t::cmInverse) == 0) {
		// нормальный режим копирования
		if (c == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
			// пропускаем альфацвет
		} else {
			if (ctx.alfa == 0xff) {
				dst.putpx(ddx,ddy,c);
			} else {
				u32 dc = dst.getpx(ddx,ddy);
				dst.putpx(ddx,ddy,ALFA_BLEND(c,dc,ctx.alfa));
			}
		}
	} else {
		// инверсный режим
		if (c == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
			// пропускаем альфацвет
		} else {
			if (ctx.alfa == 0xff) {
				dst.putpx(ddx,ddy,INVERSE_COLOR(c));
			} else {
				u32 dc = dst.getpx(ddx,ddy);
				dst.putpx(ddx,ddy,ALFA_BLEND(INVERSE_COLOR(c),dc,ctx.alfa));
			}
		}
	}
}


void surface_t::line(s32 x1,s32 y1, s32 len, bool isVertical) {
	_MY_ASSERT(buf_sz,return);
	_MY_ASSERT(x1 >=0 && y1>=0 && len > 0,return);
	if (!isVertical) {
		if (y1 >= h) return;
		if (x1+len > w) len = w-x1;
		for (int x=x1; x<x1+len; x++) {
			this->putpx_to(*this,x,y1,ctx.pen_color);
		}
	} else {
		if (x1 >= w) return;
		if (y1+len > h) len = h-y1;
		for (int y=y1; y<y1+len; y++) {
			this->putpx_to(*this, x1,y,ctx.pen_color);
		}
	}
}


void surface_t::rect(s32 x1,s32 y1, s32 rw, s32 rh) {
//	_MY_ASSERT(x1 >=0 && y1>=0 && (x1+rw-1 < w) && (y1+rh-1 < h),return);
	line(x1,y1,rw,false);
	line(x1,y1+rh-1,rw,false);
	line(x1,y1,rh,true);
	line(x1+rw-1,y1,rh,true);
}

// fallback mode
void surface_t::fill(s32 x1,s32 y1, s32 rw, s32 rh) {
	_MY_ASSERT(x1 >=0 && y1>=0 && (x1+rw < w) && (y1+rh < h),return);
	for (s32 y=y1; y<y1+rh; y++) {
		for (s32 x=x1; x<x1+rw; x++) {
			this->putpx_to(*this,x,y,ctx.pen_color);
		}
	}
}


#define abs(x) ((x)<0 ? -(x) : (x))

void surface_t::line(s32 x1,s32 y1, s32 x2,s32 y2) {

    const int deltaX = abs(x2 - x1);
    const int deltaY = abs(y2 - y1);
    const int signX = x1 < x2 ? 1 : -1;
    const int signY = y1 < y2 ? 1 : -1;

    int error = deltaX - deltaY;

	if (x2 < 0 || y2 <0 || x2 >= w || y2 >= h) 
		return;

    this->putpx_to(*this,x2, y2, ctx.pen_color);
	s32 cnt = w + h;

    while(cnt && (x1 != x2 || y1 != y2)) {
		_MY_ASSERT(cnt--,break);
		if (x1 < 0 || y1 <0 || x1 >= w || y1 >= h) 
			break;

    	this->putpx_to(*this,x1, y1,ctx.pen_color);

        const int error2 = error * 2;
        //
        if(error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }
        if(error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
}

void surface_t::circle(s32 x0,s32 y0,s32 radius) {
        int x = 0;
        int y = radius;
        int delta = 2 - 2 * radius;
        int error = 0;
        while(y >= 0) {
        	this->putpx_to(*this,x0 + x, y0 + y,ctx.pen_color);
        	this->putpx_to(*this,x0 + x, y0 - y,ctx.pen_color);
            this->putpx_to(*this,x0 - x, y0 + y,ctx.pen_color);
            this->putpx_to(*this,x0 - x, y0 - y,ctx.pen_color);
                error = 2 * (delta + y) - 1;
                if(delta < 0 && error <= 0) {
                        ++x;
                        delta += 2 * x + 1;
                        continue;
                }
                error = 2 * (delta - x) - 1;
                if(delta > 0 && error > 0) {
                        --y;
                        delta += 1 - 2 * y;
                        continue;
                }
                ++x;
                delta += 2 * (x - y);
                --y;
        }
}

// ------------------------------------------------------------------------
// --------------------------- surface_1bpp_t ----------------------------------
// ------------------------------------------------------------------------

void surface_1bpp_t::putpx(s32 x1,s32 y1, u32 c) {
	s32 offs = BMP_GET_OFFS(x1,y1,w,1);
	u8 msk = BMP_MAKE_MSK(x1);
	if (THRESHOLD(c)) {
		buf[offs] |= msk;
	} else {
		buf[offs] &= ~msk;
	}
}

u32 surface_1bpp_t::getpx(s32 x1,s32 y1) {
	s32 offs = BMP_GET_OFFS(x1,y1,w,1);
	u8 msk = BMP_MAKE_MSK(x1);
	return buf[offs] & msk ? ctx.pen_color : ctx.overlay_color;
}

// ------------------------------------------------------------------------
// --------------------------- surface_8bpp_t ----------------------------------
// ------------------------------------------------------------------------


u32 surface_8bpp_t::getpx(s32 x1,s32 y1) {
	s32 offs = BMP_GET_OFFS(x1,y1,w,8);
	u8 c = buf[offs];
	return ALFA_BLEND_CHANNEL(ctx.pen_color,0x000000,c);
}

void surface_8bpp_t::putpx(s32 x1,s32 y1, u32 c) {
		s32 offs = BMP_GET_OFFS(x1,y1,w,8);
		buf[offs] = LUMY(c);
}


void surface_8bpp_t::copy_to(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst) {
	_MY_ASSERT(buf_sz,return);
	if (sw <= 0 && sh <= 0) {
		sw = w;
		sh = h;
	}
	// self
	_MY_ASSERT(w > 0 && h > 0 && buf_sz >= BMP_GET_SIZE(w,h,8) && buf,return);
	// params
	_MY_ASSERT(sx >=0 && sy >= 0 && sw >0 && sh > 0 && sx +sw <= w && sy + sh <= h,return);
	// dst
	if (!dst.trim_to_allowed(dx,dy,sw,sh))
		return;
	_MY_ASSERT(dx + sw <= dst.w && dy + sh <= dst.h,return);

	switch (dst.itype) {
	case img_type_t::bpp1:
		_MY_ASSERT(dst.w > 0 && dst.h > 0 && dst.buf,return);
		copy_to_1bpp(sx,sy,sw,sh,dx,dy,dst);
		break;
	case img_type_t::bpp16:
		_MY_ASSERT(dst.w > 0 && dst.h > 0 && dst.buf,return);
		copy_to_16bpp(sx,sy,sw,sh,dx,dy,dst);
		break;
	case img_type_t::bpp24:
		_MY_ASSERT(dst.w > 0 && dst.h > 0 && dst.buf,return);
		copy_to_24bpp(sx,sy,sw,sh,dx,dy,dst);
		break;
	default:
		surface_t::copy_to(sx,sy,sw,sh,dx,dy,dst);
		break;
	}

}


void surface_8bpp_t::copy_to_1bpp(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst) {

	_MY_ASSERT(dst.itype == img_type_t::bpp8,return);
	_MY_ASSERT(dst.buf_sz >= BMP_GET_SIZE(dst.w,dst.h,8),return);

	u8 * sbend = &buf[buf_sz];
	u8 * dbend = &dst.buf[dst.buf_sz];

	for (s32 ssy = sy, ddy = dy; ssy < sy + sh; ssy++, ddy++) {

		u8 *sp = &buf[BMP_GET_OFFS(sx,ssy,w,8)];
		u8 *spend = &buf[BMP_GET_OFFS(sx + sw,ssy,w,8)];
		u8 *dp = &dst.buf[BMP_GET_OFFS(dx,ddy,dst.w,8)];

		if (!ctx.mode) {
			// fast row copy if no effects
			for (; sp < spend; sp++, dp++) {
				_MY_ASSERT(sp < sbend,return);
				_MY_ASSERT(dp < dbend,return);
				*dp = *sp;
			}
			continue;
		}

		for (; sp < spend; sp++, dp++) {

			_MY_ASSERT(sp < sbend,return);
			_MY_ASSERT(dp < dbend,return);

			if ((ctx.mode & copy_mode_t::cmInverse) == 0) {
				// нормальный режим копирования
				if (*sp == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						*dp = *sp;
					} else {
						// alfa blending
						*dp = ALFA_BLEND_CHANNEL(*sp,*dp,ctx.alfa);
					}
				}
			} else {
				// инверсный режим
				if (*sp == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						*dp = 0xff - *sp;
					} else {
						// inverse alfa blending
						*dp = ALFA_BLEND_CHANNEL((0xff-*sp),*dp,ctx.alfa);
					}
				}
			}
		}
	}
}

void surface_8bpp_t::copy_to_16bpp(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst) {

	_MY_ASSERT(dst.itype == img_type_t::bpp16,return);
	_MY_ASSERT(dst.buf_sz >= BMP_GET_SIZE_16(dst.w,dst.h),return);
	_MY_ASSERT(sx >=0 && sy >= 0 && sw >= 0 && sh >= 0 && dx >= 0 && dy >= 0,return);

	u8 * sbend = &buf[buf_sz];
	u8 * dbend = &dst.buf[dst.buf_sz];

	for (s32 ssy = sy, ddy = dy; ssy < sy + sh; ssy++, ddy++) {

		u8 *sp = &buf[BMP_GET_OFFS(sx,ssy,w,8)];
		u8 *spend = &buf[BMP_GET_OFFS(sx + sw,ssy,w,8)];
		u8 *dp = &dst.buf[BMP_GET_OFFS_16(dx,ddy,dst.w)];

		for (; sp < spend; sp++, dp += sizeof(u16)) {

			_MY_ASSERT(sp < sbend,return);
			_MY_ASSERT(dp < dbend,return);

			rgb565_t *drp = (rgb565_t*)(dp);

			if ((ctx.mode & copy_mode_t::cmInverse) == 0) {
				// нормальный режим копирования
				if (*sp == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						drp->r = ALFA_BLEND_CHANNEL(U32R(ctx.pen_color),drp->r << 3,*sp) >> 3;
						drp->set_g(ALFA_BLEND_CHANNEL(U32G(ctx.pen_color),drp->g() << 2, *sp) >> 2);
						drp->b = ALFA_BLEND_CHANNEL(U32B(ctx.pen_color),drp->b << 3, *sp) >> 3;
					} else {
						// alfa blending
						u8 r = ALFA_BLEND_CHANNEL(U32R(ctx.pen_color),drp->r << 3, *sp);
						u8 g = ALFA_BLEND_CHANNEL(U32G(ctx.pen_color),drp->g() << 2, *sp);
						u8 b = ALFA_BLEND_CHANNEL(U32B(ctx.pen_color),drp->b << 3, *sp);

						drp->r = ALFA_BLEND_CHANNEL(r,drp->r << 3, ctx.alfa) >> 3;
						drp->set_g(ALFA_BLEND_CHANNEL(g,drp->g() << 2, ctx.alfa) >> 2);
						drp->b = ALFA_BLEND_CHANNEL(b,drp->b << 3, ctx.alfa) >> 3;

					}
				}
			} else {
				// инверсный режим
				if (*sp == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						drp->r = ALFA_BLEND_CHANNEL(0xff-U32R(ctx.pen_color),drp->r << 3, *sp) >> 3;
						drp->set_g(ALFA_BLEND_CHANNEL(0xff-U32G(ctx.pen_color),drp->g() << 2, *sp) >> 2);
						drp->b = ALFA_BLEND_CHANNEL(0xff-U32B(ctx.pen_color),drp->b << 3, *sp) >> 3;
					} else {
						// inverse alfa blending
						u8 r = ALFA_BLEND_CHANNEL(U32R(ctx.pen_color),drp->r << 3, *sp) >> 3;
						u8 g = ALFA_BLEND_CHANNEL(U32G(ctx.pen_color),drp->g() << 2, *sp) >> 2;
						u8 b = ALFA_BLEND_CHANNEL(U32B(ctx.pen_color),drp->b << 3, *sp) >> 3;

						drp->r = ALFA_BLEND_CHANNEL(0xff-r,drp->r << 3, ctx.alfa) >> 3;
						drp->set_g(ALFA_BLEND_CHANNEL(0xff-g,drp->g() << 2, ctx.alfa) >> 2);
						drp->b = ALFA_BLEND_CHANNEL(0xff-b,drp->b << 3, ctx.alfa) >> 3;
					}
				}
			}
		}
	}
}


void surface_8bpp_t::copy_to_24bpp(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst) {

	_MY_ASSERT(dst.itype == img_type_t::bpp24,return);
	_MY_ASSERT(dst.buf_sz >= BMP_GET_SIZE(dst.w,dst.h,24),return);
	_MY_ASSERT(sx >=0 && sy >= 0 && sw >= 0 && sh >= 0 && dx >= 0 && dy >= 0,return);

	u8 * sbend = &buf[buf_sz];
	u8 * dbend = &dst.buf[dst.buf_sz];

	for (s32 ssy = sy, ddy = dy; ssy < sy + sh; ssy++, ddy++) {

		u8 *sp = &buf[BMP_GET_OFFS(sx,ssy,w,8)];
		u8 *spend = &buf[BMP_GET_OFFS(sx + sw,ssy,w,8)];
		u8 *dp = &dst.buf[BMP_GET_OFFS(dx,ddy,dst.w,24)];

		for (; sp < spend; sp++, dp += 3) {

			_MY_ASSERT(sp < sbend,return);
			_MY_ASSERT(dp < dbend,return);

			rgb_t *drp = (rgb_t*)(dp);

			if ((ctx.mode & copy_mode_t::cmInverse) == 0) {
				// нормальный режим копирования
				if (*sp == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						drp->r = ALFA_BLEND_CHANNEL(U32R(ctx.pen_color),drp->r,*sp);
						drp->g = ALFA_BLEND_CHANNEL(U32G(ctx.pen_color),drp->g,*sp);
						drp->b = ALFA_BLEND_CHANNEL(U32B(ctx.pen_color),drp->b,*sp);
					} else {
						// alfa blending
						u8 r = ALFA_BLEND_CHANNEL(U32R(ctx.pen_color),drp->r,*sp);
						u8 g = ALFA_BLEND_CHANNEL(U32G(ctx.pen_color),drp->g,*sp);
						u8 b = ALFA_BLEND_CHANNEL(U32B(ctx.pen_color),drp->b,*sp);

						drp->r = ALFA_BLEND_CHANNEL(r,drp->r,ctx.alfa);
						drp->g = ALFA_BLEND_CHANNEL(g,drp->g,ctx.alfa);
						drp->b = ALFA_BLEND_CHANNEL(b,drp->b,ctx.alfa);

					}
				}
			} else {
				// инверсный режим
				if (*sp == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						drp->r = ALFA_BLEND_CHANNEL(0xff-U32R(ctx.pen_color),drp->r,*sp);
						drp->g = ALFA_BLEND_CHANNEL(0xff-U32G(ctx.pen_color),drp->g,*sp);
						drp->b = ALFA_BLEND_CHANNEL(0xff-U32B(ctx.pen_color),drp->b,*sp);
					} else {
						// inverse alfa blending
						u8 r = ALFA_BLEND_CHANNEL(U32R(ctx.pen_color),drp->r,*sp);
						u8 g = ALFA_BLEND_CHANNEL(U32G(ctx.pen_color),drp->g,*sp);
						u8 b = ALFA_BLEND_CHANNEL(U32B(ctx.pen_color),drp->b,*sp);

						drp->r = ALFA_BLEND_CHANNEL(0xff-r,drp->r,ctx.alfa);
						drp->g = ALFA_BLEND_CHANNEL(0xff-g,drp->g,ctx.alfa);
						drp->b = ALFA_BLEND_CHANNEL(0xff-b,drp->b,ctx.alfa);
					}
				}
			}
		}
	}
}


// ------------------------------------------------------------------------
// --------------------------- surface_24bpp_t ----------------------------------
// ------------------------------------------------------------------------

void surface_24bpp_t::fill(s32 x1,s32 y1, s32 rw, s32 rh) {
	_MY_ASSERT(buf && buf_sz,return);
	this->trim_to_allowed(x1,y1,rw,rh);
	_MY_ASSERT(x1>=0 && y1>=0 && x1+rw <= w && y1+rh <=h,return);

	for (s32 y=y1; y<y1+rh; y++) {
		for (s32 x=x1; x<x1+rw; x++) {
			s32 offs = BMP_GET_OFFS(x,y,w,24);
			rgb_t* p = (rgb_t*)(&buf[offs]);

			u32 c = RGB2U32(p->r,p->g,p->b);

			if ((ctx.mode & copy_mode_t::cmInverse) == 0) {
				// нормальный режим копирования
				if (c == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						p->r = U32R(ctx.pen_color);
						p->g = U32G(ctx.pen_color);
						p->b = U32B(ctx.pen_color);
					} else {
						// alfa blending
						p->r = ALFA_BLEND_CHANNEL(U32R(ctx.pen_color),p->r,ctx.alfa);
						p->g = ALFA_BLEND_CHANNEL(U32G(ctx.pen_color),p->g,ctx.alfa);
						p->b = ALFA_BLEND_CHANNEL(U32B(ctx.pen_color),p->b,ctx.alfa);
					}
				}
			} else {
				// инверсный режим
				if (c == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						p->r = 0xff-U32R(ctx.pen_color);
						p->g = 0xff-U32G(ctx.pen_color);
						p->b = 0xff-U32B(ctx.pen_color);
					} else {
						// inverse alfa blending
						p->r = ALFA_BLEND_CHANNEL((0xff-U32R(ctx.pen_color)),p->r,ctx.alfa);
						p->g = ALFA_BLEND_CHANNEL((0xff-U32G(ctx.pen_color)),p->g,ctx.alfa);
						p->b = ALFA_BLEND_CHANNEL((0xff-U32B(ctx.pen_color)),p->b,ctx.alfa);
					}
				}
			}
		} // for x
	}
}


void surface_24bpp_t::putpx(s32 x1,s32 y1, u32 c) {
	
	_MY_ASSERT(0 <= x1 && x1 < 0 + w, return);
	_MY_ASSERT(0 <= y1 && y1 < 0 + h, return);

	s32 offs = BMP_GET_OFFS(x1,y1,w,24);
	rgb_t* p = (rgb_t*)(&buf[offs]);
	p->r = U32R(c);
	p->g = U32G(c);
	p->b = U32B(c);
}

u32 surface_24bpp_t::getpx(s32 x1,s32 y1) {
	_MY_ASSERT(0 <= x1 && x1 < 0 + w, return 0);
	_MY_ASSERT(0 <= y1 && y1 < 0 + h, return 0);

	s32 offs = BMP_GET_OFFS(x1,y1,w,24);
	rgb_t* p = (rgb_t*)(&buf[offs]);
	return RGB2U32(p->r, p->g, p->b);
}



void surface_24bpp_t::copy_to(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst) {
	_MY_ASSERT(buf_sz,return);
	if (sw <= 0 && sh <= 0) {
		sw = w;
		sh = h;
	}
	// self
	_MY_ASSERT(w > 0 && h > 0 && buf_sz >= BMP_GET_SIZE(w,h,24) && buf,return);
	// params
	_MY_ASSERT(sx >=0 && sy >= 0 && sw >0 && sh > 0 && sx +sw <= w && sy + sh <= h,return);
	// dst
	_MY_ASSERT(dx + sw <= dst.w && dy + sh <= dst.h,return);
	if (dst.itype != img_type_t::bpp24) {
		surface_t::copy_to(sx,sy,sw,sh,dx,dy,dst);
		return;
	}
	_MY_ASSERT(dst.w > 0 && dst.h > 0 && dst.buf_sz >= BMP_GET_SIZE(dst.w,dst.h,24) && dst.buf,return);

	u8 * sbend = &buf[buf_sz];
	u8 * dbend = &dst.buf[dst.buf_sz];

	for (s32 ssy = sy, ddy = dy; ssy < sy + sh; ssy++, ddy++) {
		//s32 sp_inx = BMP_GET_OFFS(sx,ssy,w,24);
		// _MY_ASSERT(sp_inx >= 0 && sp_inx < buf_sz);
		u8 *sp = &buf[BMP_GET_OFFS(sx,ssy,w,24)];
		//s32 spend_inx = BMP_GET_OFFS(sx + sw,ssy,w,24);
		//_MY_ASSERT(spend_inx >= 0 && spend_inx <= buf_sz);
		u8 *spend = &buf[BMP_GET_OFFS(sx + sw,ssy,w,24)];
		//s32 dp_inx = BMP_GET_OFFS(dx,ddy,dst.w,24);
		//_MY_ASSERT(dp_inx >= 0 && dp_inx < dst.buf_sz);
		u8 *dp = &dst.buf[BMP_GET_OFFS(dx,ddy,dst.w,24)];
		//s32 dpend_inx = BMP_GET_OFFS(dx + sw,ddy,dst.w,24);
		//_MY_ASSERT(dpend_inx >= 0 && dpend_inx <= dst.buf_sz);
		//u8 *dpend = &buf[dpend_inx];
		_MY_ASSERT(sp < sbend,return);
		_MY_ASSERT(spend <= sbend,return);
		_MY_ASSERT(dp < dbend,return);
		//_MY_ASSERT(dpend <= sbend);
		if (!ctx.mode) {
			// fast row copy if no effects
			for (; sp < spend; sp++, dp++) {
				_MY_ASSERT(sp < sbend,return);
				_MY_ASSERT(dp < dbend,return);
				*dp = *sp;
			}
			continue;
		}

		for (; sp < spend; sp+=3, dp+=3) {

			_MY_ASSERT(sp < sbend,return);
			_MY_ASSERT(dp < dbend,return);

			rgb_t* t = (rgb_t*)sp;
			u32 c = RGB2U32(t->r,t->g,t->b);

			if ((ctx.mode & copy_mode_t::cmInverse) == 0) {
				// нормальный режим копирования
				if (c == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						*(rgb_t*)(dp) = *t;
					} else {
						// alfa blending

						rgb_t *drp = (rgb_t*)(dp);
						drp->r = ALFA_BLEND_CHANNEL(t->r,drp->r,ctx.alfa);
						drp->g = ALFA_BLEND_CHANNEL(t->g,drp->g,ctx.alfa);
						drp->b = ALFA_BLEND_CHANNEL(t->b,drp->b,ctx.alfa);
					}
				}
			} else {
				// инверсный режим
				if (c == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					rgb_t *drp = (rgb_t*)(dp);
					if (ctx.alfa == 0xff) {
						drp->r = 0xff-t->r;
						drp->g = 0xff-t->g;
						drp->b = 0xff-t->b;
					} else {
						// inverse alfa blending
						drp->r = ALFA_BLEND_CHANNEL((0xff-t->r),drp->r,ctx.alfa);
						drp->g = ALFA_BLEND_CHANNEL((0xff-t->g),drp->g,ctx.alfa);
						drp->b = ALFA_BLEND_CHANNEL((0xff-t->b),drp->b,ctx.alfa);
					}
				}
			}
		}
	}
}


// ------------------------------------------------------------------------
// --------------------------- surface_16bpp_t ----------------------------
// ------------------------------------------------------------------------


surface_16bpp_t::surface_16bpp_t(s32 _w, s32 _h, s32 _buf_sz, u8 *_buf)
	:surface_t(_w,_h,_buf_sz,_buf,img_type_t::bpp16) {
	_MY_ASSERT(w > 0 && h > 0 && buf_sz >= BMP_GET_SIZE_16(w,h) && buf, return);
}

void surface_16bpp_t::fill(s32 x1,s32 y1, s32 rw, s32 rh) {
	_MY_ASSERT(buf && buf_sz,return);
	if (!this->trim_to_allowed(x1,y1,rw,rh)) {
		return;
	}
	_MY_ASSERT(x1>=0 && y1>=0 && x1+rw <= w && y1+rh <=h,return);

	for (s32 y=y1; y < y1+rh; y++) {
		for (s32 x=x1; x < x1+rw; x++) {

			s32 offs = BMP_GET_OFFS_16(x,y,w);
			rgb565_t* p = (rgb565_t*)(&buf[offs]);

			u32 c = RGB2U32(p->r<<3,p->g()<<2,p->b<<3);

			if ((ctx.mode & copy_mode_t::cmInverse) == 0) {
				// нормальный режим копирования
				if (c == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						p->r = U32R(ctx.pen_color)>>3;
						p->set_g(U32G(ctx.pen_color)>>2);
						p->b = U32B(ctx.pen_color)>>3;
					} else {
						// alfa blending
						p->r = ALFA_BLEND_CHANNEL(U32R(ctx.pen_color),p->r<<3,ctx.alfa)>>3;
						p->set_g(ALFA_BLEND_CHANNEL(U32G(ctx.pen_color),p->g()<<2,ctx.alfa)>>2);
						p->b = ALFA_BLEND_CHANNEL(U32B(ctx.pen_color),p->b<<3,ctx.alfa)>>3;
					}
				}
			} else {
				// инверсный режим
				if (c == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						p->r = (0xff-U32R(ctx.pen_color))>>3;
						p->set_g((0xff-U32G(ctx.pen_color))>>2);
						p->b = (0xff-U32B(ctx.pen_color))>>3;
					} else {
						// inverse alfa blending
						p->r = ALFA_BLEND_CHANNEL((0xff-U32R(ctx.pen_color)),p->r<<3,ctx.alfa)>>3;
						p->set_g(ALFA_BLEND_CHANNEL((0xff-U32G(ctx.pen_color)),p->g()<<2,ctx.alfa)>>2);
						p->b = ALFA_BLEND_CHANNEL((0xff-U32B(ctx.pen_color)),p->b<<3,ctx.alfa)>>3;
					}
				}
			}
		} // for x
	}
}


void surface_16bpp_t::putpx(s32 x1,s32 y1, u32 c) {
	
	_MY_ASSERT(0 <= x1 && x1 < 0 + w, return);
	_MY_ASSERT(0 <= y1 && y1 < 0 + h, return);

	s32 offs = BMP_GET_OFFS_16(x1,y1,w);
	rgb565_t* p = (rgb565_t*)(&buf[offs]);
	p->r = U32R(c) >> 3;
	p->set_g(U32G(c) >> 2);
	p->b = U32B(c) >> 3;
}

u32 surface_16bpp_t::getpx(s32 x1,s32 y1) {
	_MY_ASSERT(0 <= x1 && x1 < 0 + w, return 0);
	_MY_ASSERT(0 <= y1 && y1 < 0 + h, return 0);

	s32 offs = BMP_GET_OFFS_16(x1,y1,w);
	rgb565_t* p = (rgb565_t*)(&buf[offs]);
	return RGB2U32(p->r << 3, p->g() << 2, p->b << 3);
}


void surface_16bpp_t::copy_to(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst) {
	_MY_ASSERT(buf_sz,return);
	if (sw <= 0 && sh <= 0) {
		sw = w;
		sh = h;
	}
	// self
	_MY_ASSERT(w > 0 && h > 0 && buf_sz >= BMP_GET_SIZE_16(w,h) && buf,return);
	// params
	_MY_ASSERT(sx >=0 && sy >= 0 && sw >0 && sh > 0 && sx +sw <= w && sy + sh <= h,return);
	// dst
	_MY_ASSERT(dx + sw <= dst.w && dy + sh <= dst.h,return);
	if (dst.itype != img_type_t::bpp16) {
		surface_t::copy_to(sx,sy,sw,sh,dx,dy,dst);
		return;
	}
	_MY_ASSERT(dst.w > 0 && dst.h > 0 && dst.buf_sz >= BMP_GET_SIZE_16(dst.w,dst.h) && dst.buf,return);

	u8 * sbend = &buf[buf_sz];
	u8 * dbend = &dst.buf[dst.buf_sz];

	for (s32 ssy = sy, ddy = dy; ssy < sy + sh; ssy++, ddy++) {
		u8 *sp = &buf[BMP_GET_OFFS_16(sx,ssy,w)];
		u8 *spend = &buf[BMP_GET_OFFS_16(sx + sw,ssy,w)];
		u8 *dp = &dst.buf[BMP_GET_OFFS_16(dx,ddy,dst.w)];
		_MY_ASSERT(sp < sbend,return);
		_MY_ASSERT(spend <= sbend,return);
		_MY_ASSERT(dp < dbend,return);
		//_MY_ASSERT(dpend <= sbend);
		if (!ctx.mode) {
			// fast row copy if no effects
			for (; sp < spend; sp++, dp++) {
				_MY_ASSERT(sp < sbend,return);
				_MY_ASSERT(dp < dbend,return);
				*dp = *sp;
			}
			continue;
		}

		for (; sp < spend; sp += sizeof(u16), dp += sizeof(u16)) {

			_MY_ASSERT(sp < sbend,return);
			_MY_ASSERT(dp < dbend,return);

			rgb565_t* t = (rgb565_t*)sp;
			u32 c = RGB2U32(t->r << 3, t->g() << 2, t->b << 3);

			if ((ctx.mode & copy_mode_t::cmInverse) == 0) {
				// нормальный режим копирования
				if (c == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					if (ctx.alfa == 0xff) {
						*(rgb565_t*)(dp) = *t;
					} else {
						// alfa blending

						rgb565_t *drp = (rgb565_t*)(dp);
						drp->r = ALFA_BLEND_CHANNEL(t->r << 3, drp->r << 3, ctx.alfa) >> 3;
						drp->set_g(ALFA_BLEND_CHANNEL(t->g() << 2, drp->g() << 2, ctx.alfa) >> 2);
						drp->b = ALFA_BLEND_CHANNEL(t->b << 2, drp->b << 2, ctx.alfa) >> 3;
					}
				}
			} else {
				// инверсный режим
				if (c == ctx.overlay_color && (ctx.mode & copy_mode_t::cmOverlay)) {
					// пропускаем альфа-цвет
				} else {
					rgb565_t *drp = (rgb565_t*)(dp);
					if (ctx.alfa == 0xff) {
						drp->r = (0xff- (t->r<<3))>>3;
						drp->set_g((0xff-(t->g()<<2))>>2);
						drp->b = (0xff-(t->b<<3))>>3;
					} else {
						// inverse alfa blending
						drp->r = ALFA_BLEND_CHANNEL((0xff-(t->r<<3)),drp->r<<3,ctx.alfa)>>3;
						drp->set_g(ALFA_BLEND_CHANNEL((0xff-(t->g()<<2)),drp->g()<<2,ctx.alfa)>>2);
						drp->b = ALFA_BLEND_CHANNEL((0xff-(t->b<<3)),drp->b<<3,ctx.alfa)>>3;
					}
				}
			}
		}
	}
}


// ------------------------------------------------------------------------
// --------------------------- font_t ----------------------------------
// ------------------------------------------------------------------------


bool font_t::exctract(char c, surface_t &res, s32 dx, s32 dy, s32 &width) {
	s32 cnum = -1;
	width = cw;

	if (alfabet) {
		if (!c)
			return false;
		for (s32 i=0; i<255; i++) {
			if (!alfabet[i])
				return false;
			if (alfabet[i] == c) {
				cnum = i;
				break;
			}
		}
	} else {
		// если алвафит не указан, считаем, что используется системный
		u32 _c = c;
		cnum = (_c & 0xff);
		// если код символа больше максимального кода
		if (cnum > end)
			return false;
		// позиция = код - начало
		cnum -= start;
	}
	_MY_ASSERT(cnum >= 0,return false);

	s32 posV = cnum / ccntw;
	_MY_ASSERT(posV < ccnth,return false);
	s32 posH = cnum - posV * ccntw;
	_MY_ASSERT(posH >=0 && posH < ccntw,return false);
	//s32 offs = BMP_GET_OFFS(posH * cw, posV * ch, w);
	surf->copy_to(posH * cw, posV * ch, cw, ch, dx,dy,res);

	if (widths)
		width = widths[cnum];

	return true;
}


bool font_t::print_to(s32 x,s32 y, surface_t &surface, string_t &str) {
	bool res = true;

	for (s32 i=0; i<(s32)str.length(); i++) {

		if (x+cw >= surface.w || y+ch >= surface.h)
			return false;

		s32 width;
		if (!exctract(str[i],surface,x,y,width)) {
			res = false;
			x+= cw;
			continue;
		}
		x+= width;
	}
	return res;
}
