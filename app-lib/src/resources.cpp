#include "resources.h"
#include "assert_impl.h"

//#include "ARIALNI.h"
//#include "glyphs.h"

//s32 fnt_digits_10x20_widths[12] = {10,4,10,10,10,10,10,10,10,10,10,10};
//surface_1bpp_t fnt_digits_10x20_surf(10*12, 25*1, BMP_GET_FONT_SIZE(10,25,12,1,1),(u8*)impact_10x25);
//font_t fnt_digits_10x20(10,25,12,1,"_.0123456789",-1,-1,fnt_digits_10x20_widths, &fnt_digits_10x20_surf);

//surface_24bpp_t bk_surface(320,240,BMP_GET_SIZE(320,240,24), lena_320);
using namespace myvi;

void resources_t::init() {
#ifndef __gnu_linux__
	ttf.init("fonts","fonts\\ARIALNB.ttf", 0,0);
	gly.init("fonts","glyphs.ttf", 0,0);
#else
	ttf.init("fonts","fonts\\ARIALNB.ttf", 0,0);
	gly.init("fonts","glyphs.ttf", 0,0);
#endif
}

resources_t res;
