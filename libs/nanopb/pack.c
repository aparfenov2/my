/*
 * pack.c
 *
 *  Created on: 29.08.2014
 *      Author: user
 */

#include "types.h"

#ifdef PLATFORM_C28

u32 pack(const u8 *data, u32 sz, void *dst) {
	u32 sw=0;
	int *dsti = dst;
	for (; sw < sz; sw++) {
		__byte(dsti , sw & 0x01) = data[sw];
		if (sw & 0x01) dsti++;
	}
	return sz >> 1;
}

u32 unpack(const void *data, u32 sz, u8 *dst) {
	u32 dw=0;
	u32 sw=0;
	for (; sw < sz; sw++) {
		dst[dw] = __byte((int *)data,dw);
		dw++;
		dst[dw] = __byte((int *)data,dw);
		dw++;
	}
	return dw;
}
#endif
