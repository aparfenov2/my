/*
 * surface.h
 *
 *  Created on: 13.11.2012
 *      Author: Parfenov
 */

#ifndef SURFACE_H_
#define SURFACE_H_

#include "types.h"
#include "basic.h"
#include "bmp_math.h"

namespace myvi {

// битовое поле режима копирования
namespace copy_mode_t {
	enum copy_mode_t {
		cmNone    = 0,
		cmOverlay = 0x01,
		cmInverse = 0x02
	};
}

namespace img_type_t {
	enum img_type_t {
		bpp1 = 1, // 1bit
		bpp8  = 8, // grey, 8 bit
		bpp24 = 24,  // bpp24, 8bit/ch
		bppGeneric = 255  // driver surface
	};
}
// begin_paint return values
namespace bp_return_t {
	enum bp_return_t {
		bpOk = 0,
		bpBecomeMaster = 1	// dst surface bacame master and do all self
	};
}

typedef struct { // memory layout
	u8 r;
	u8 g;
	u8 b;
} rgb_t;


#define RGB2U32(r,g,b) ((((u32)(r) << 16) & 0x00ff0000) | (((u32)(g) << 8) & 0x0000ff00) | ((u32)(b) & 0x000000ff))
#define U32R(v) (((v) >> 16) & 0xff)
#define U32G(v) (((v) >> 8) & 0xff)
#define U32B(v) ((v) & 0xff)

#define INVERSE_COLOR(v) (RGB2U32((0xff-U32R(v)),(0xff-U32G(v)),(0xff-U32B(v))))
#define LUMY(c) ((U32R(c)*3+U32B(c)+U32G(c)*4)>>3)
#define THRESHOLD(c) (LUMY(c) > 50)

//#define ALFA_AV 128
#define ALFA_BLEND_CHANNEL(sc,dc,av) (((u32)(dc) * (u32)(0xff-(u32)(av)) + (u32)(sc) * (u32)(av) )/(u32)255)
#define ALFA_BLEND(sc,dc,av) RGB2U32( \
		ALFA_BLEND_CHANNEL(U32R(sc),U32R(dc),av), \
		ALFA_BLEND_CHANNEL(U32G(sc),U32G(dc),av), \
		ALFA_BLEND_CHANNEL(U32B(sc),U32B(dc),av) \
	)

#define INIT_PEN_COLOR 0x99FF33

// базовый класс поверхности

class surface_context_t {
public:
	u32 pen_color; // цвет пера
	u32 overlay_color; // цвет оверлея
	u8  alfa;   // прозрачность
	copy_mode_t::copy_mode_t mode; // операция при отрисовке
public:
	surface_context_t() {
		reset();
	}
	void reset() {
		pen_color=(INIT_PEN_COLOR),overlay_color=(0),alfa=(0xff),mode=((copy_mode_t::copy_mode_t)0);
	}
};

class surface_t {
public:
	s32 w;		// ширина поверхности, px
	s32 h;		// высота поверхности, px
	s32 buf_sz;	// размер буфера данных, байт
	u8 *buf;	// указатель на буфер с данными
	img_type_t::img_type_t itype;
	surface_context_t ctx;
protected:
	s32 allowed_x1;
	s32 allowed_y1;
	s32 allowed_w;
	s32 allowed_h;
public:
	surface_t():w(0),h(0),buf_sz(0),buf(0),itype(img_type_t::bppGeneric) {
		w = h = buf_sz = 0;
		buf = 0;
		itype = img_type_t::bppGeneric;
		allowed_x1 = allowed_y1 = allowed_w = allowed_h = 0;
	}

	surface_t(s32 _w, s32 _h, s32 _buf_sz, u8 *_buf, img_type_t::img_type_t _itype) {
		w=(_w),h=(_h),buf_sz=(_buf_sz),buf=(_buf),itype=(_itype); 
		_MY_ASSERT(w > 0 && h > 0,return);
	}

	void assign(s32 _w, s32 _h, s32 _buf_sz, u8 *_buf, img_type_t::img_type_t _itype) {
		_MY_ASSERT(_w > 0 && _h > 0,return);
		w = _w;
		h = _h;
		buf_sz = _buf_sz;
		buf = _buf;
		itype = _itype;
	}
// pixel interface
	virtual void putpx(s32 x1,s32 y1, u32 c) = 0;
	virtual u32  getpx(s32 x1,s32 y1) = 0;
// driver optimization
	virtual bp_return_t::bp_return_t begin_paint(surface_t &src, s32 sx,s32 sy,s32 sw,s32 sh,s32 dx,s32 dy) {
		return bp_return_t::bpOk;
	}

	virtual void end_paint() {
	}
	// called from rasterizer_t
	void set_allowed_area(s32 x1, s32 y1,s32 sw,s32 sh) {

		if (!_trim_to(x1,y1,sw,sh,0,0,w,h)) {
			allowed_x1 = allowed_y1 = allowed_w = allowed_h = 0;
		} else {
			allowed_x1 = x1;
			allowed_y1 = y1;
			allowed_w = sw;
			allowed_h = sh;
		}
	}

// can be futher optimized
	virtual void copy_to(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst);
	virtual void fill(s32 x1,s32 y1, s32 w, s32 h);
// cant be optimized
	void line(s32 x1,s32 y1, s32 len, bool isVertical);
	void line(s32 x1,s32 y1, s32 x2,s32 y2);
	void circle(s32 x1,s32 y1,s32 r);
	void rect(s32 x1,s32 y1, s32 w, s32 h);
	void putpx_to(surface_t &dst, s32 x1,s32 y1, u32 c); // putpx with mode

	bool trim_to_allowed(s32 &x1, s32 &y1,s32 &sw,s32 &sh) {
		return _trim_to(x1,y1,sw,sh,allowed_x1, allowed_y1, allowed_w, allowed_h );
	}
private:
	bool _trim_to(s32 &x1, s32 &y1,s32 &sw,s32 &sh, s32 x0, s32 y0, s32 w0, s32 h0) {
		_MY_ASSERT(sw >= 0 && sh >= 0 && x0 >= 0 && y0 >= 0 && w0 >= 0 && h0 >= 0, return false); // все положительные
		_MY_ASSERT(x0+w0 <= this->w && y0+h0 <= this->h, return false); // рамка меньше границы
		// перейдем в абсолютные координаты

		s32 x1e = x1+sw;
		s32 y1e = y1+sh;
		s32 x0e = x0+w0;
		s32 y0e = y0+h0;

		if (x0e <= 0 || y0e <= 0) // рамка нулевой ширины
			return false;
		if (x1 >= x0e || y1 >= y0e) // левая точка вышла за правую границу рамки 
			return false;

		if (x1 < x0) x1 = x0;	//if (x1 >= x0e) x1 = x0e-1;
		if (y1 < y0) y1 = y0;	//if (y1 >= y0e) y1 = y0e-1;
		
		if (x1e < x0) x1e = x0;	if (x1e > x0e) x1e = x0e;
		if (y1e < y0) y1e = y0;	if (y1e > y0e) y1e = y0e;

		// обратно в относительные
		sw = x1e-x1;
		sh = y1e-y1;
//		_MY_ASSERT(x1 >= 0 && y1 >=0 && sw >= 0 && sh >= 0);
		return x1 >= 0 && y1 >= 0 && sw > 0 && sh > 0;
	}

};


// черно-белая, 1 бит

class surface_1bpp_t : public surface_t {
public:
	surface_1bpp_t():surface_t() {
	}
	surface_1bpp_t(s32 _w, s32 _h, s32 _buf_sz, u8 *_buf):surface_t(_w,_h,_buf_sz,_buf,img_type_t::bpp1) {
		_MY_ASSERT(w > 0 && h > 0 && buf_sz >= BMP_GET_SIZE(w,h,1) && buf, return);
	}
	virtual u32  getpx(s32 x1,s32 y1) OVERRIDE;
	virtual void putpx(s32 x1,s32 y1, u32 c) OVERRIDE;
};

// серая, 8 бит

class surface_8bpp_t : public surface_t {
public:
	surface_8bpp_t():surface_t() {
	}
	surface_8bpp_t(s32 _w, s32 _h, s32 _buf_sz, u8 *_buf):surface_t(_w,_h,_buf_sz,_buf,img_type_t::bpp1) {
		_MY_ASSERT(w > 0 && h > 0 && buf_sz >= BMP_GET_SIZE(w,h,8) && buf, return);
	}
	virtual u32  getpx(s32 x1,s32 y1) OVERRIDE {
		s32 offs = BMP_GET_OFFS(x1,y1,w,8);
		return buf[offs];
	}
	virtual void putpx(s32 x1,s32 y1, u32 c) OVERRIDE {
		s32 offs = BMP_GET_OFFS(x1,y1,w,8);
		buf[offs] = (u8)c;
	}
	virtual void copy_to(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst) OVERRIDE;
private:
	void copy_to_1bpp(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst);
	void copy_to_24bpp(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst);
};

// rgb 24 bit

class surface_24bpp_t : public surface_t {
public:
	surface_24bpp_t():surface_t() {
	}
	surface_24bpp_t(s32 _w, s32 _h, s32 _buf_sz, u8 *_buf):surface_t(_w,_h,_buf_sz,_buf,img_type_t::bpp24) {
		_MY_ASSERT(w > 0 && h > 0 && buf_sz >= BMP_GET_SIZE(w,h,24) && buf, return);
	}
	virtual void copy_to(s32 sx,s32 sy,s32 sw,s32 sh, s32 dx, s32 dy, surface_t &dst) OVERRIDE;
	virtual void fill(s32 x1,s32 y1, s32 w, s32 h) OVERRIDE;
	virtual u32  getpx(s32 x1,s32 y1) OVERRIDE;
	virtual void putpx(s32 x1,s32 y1, u32 c) OVERRIDE;
};

class font_t {
private:
	s32 cw;		// ширина символа
	s32 ch;		// высота символа
	s32 ccntw;	// количество символов по горизонтали
	s32 ccnth;	// количество символов по вертикали
	s32 start; // начало диапазона, если не указан алвафит
	s32 end;   // конец диапазона, если не указан алфавит
	s32 *widths;
	const char *alfabet; // алфавит, также определяет порядок символов
	surface_t *surf;
public:
	font_t(s32 _cw, s32 _ch, s32 _ccntw, s32 _ccnth,
		const char* _alfabet, s32 _start, s32 _end, s32 *_widths, surface_t *_surf ) {
		surf=(_surf),start=(_start),end=(_end);
		cw=(_cw),ch=(_ch),ccntw=(_ccntw),ccnth=(_ccnth),alfabet=(_alfabet),widths=(_widths);
	}
	bool exctract(char c, surface_t &res, s32 dx, s32 dy, s32 &width);
	bool print_to(s32 x,s32 y, surface_t &surface, string_t &str);
};

} // ns
#endif /* SURFACE_H_ */
