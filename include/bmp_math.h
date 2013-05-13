#ifndef _BMP_MATH_H
#define _BMP_MATH_H

/*
#define BMP_GET_JUNK_LEN(w,bpp) ((~((w) * bpp - 1) >> 3) & 3)
#define BMP_GET_ROW_LEN_NO_JUNK(w,bpp) (1+(((w)*bpp-1)>>3))

#define BMP_GET_ROW_LEN(w,bpp) (BMP_GET_ROW_LEN_NO_JUNK(w,bpp) + BMP_GET_JUNK_LEN(w,bpp) )
*/
#define BMP_GET_ROW_LEN(w,bpp) ((((s32)(w)) * ((s32)bpp))/ ((s32)8))
#define BMP_GET_OFFS(x,y,w,bpp) (BMP_GET_ROW_LEN(w,bpp)*((s32)(y)) + ((((s32)(x)) * ((s32)bpp))/((s32)8)))
#define BMP_GET_SIZE(w,h,bpp) BMP_GET_ROW_LEN(w,bpp)*((s32)(h))

#define BMP_GET_FONT_SIZE(cw,ch,cntw,cnth,bpp) BMP_GET_SIZE((cw) * (cntw), (ch) * (cnth),bpp)

#define BMP_MAKE_MSK(x) (1 << (7-(x & 7)))


#endif
