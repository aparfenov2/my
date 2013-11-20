#ifndef _TRUETYPE_H
#define _TRUETYPE_H

#include "surface.h"
#include "ttcache.h"
#include "assert_impl.h"
//typedef enum {
//	cmapUnicode,
//	cmapSymbol
//} char_map_t;

namespace myvi {


class ttype_font_t {
public:
	surface_8bpp_t char_surface;
	font_handle_t *hdl;
private:
	u8 last_sz_pxx;
	u8 last_sz_pxy;
	u8 last_sz_ptx;
	u8 last_sz_pty;
	glyph_t *gly;
public:
	ttype_font_t() {
		last_sz_pxx=(0);
		last_sz_pxy=(0);
		last_sz_ptx=(0);
		last_sz_pty=(0);
		hdl = 0, gly = 0;
	}

	//s32 get_last_advance_x() {
	//	_MY_ASSERT(gly);
	//	return gly->hadvance >> 6;
	//}

	void init(char *folder, char * _fname, u8 * _mem_font, u32 _mem_sz);
	void set_char_size_px(s32 pxx, s32 pxy) { // высота в пикселях
		globals::ttcache.set_char_size_px(hdl,pxx,pxy);
	}
	//void set_char_size_pt(s32 ptx, s32 pty) { // высота в поинтах
	//	ttcache.set_char_size_pt(hdl,ptx,pty);
	//}
	void print_to(s32 x,s32 y, surface_t &surface, string_t str);
	void print_to(s32 x,s32 y, surface_t &surface, string32_t str);
	void gly_to(s32 x,s32 y, surface_t &surface, u32 glyph_index);
	void get_string_size(string_t &str, s32 &sw, s32 &sh);
	void get_string_size(string32_t &str, s32 &sw, s32 &sh);
	void get_gly_size(u32 glyph_index, s32 &sw, s32 &sh);
//	int set_char_map(char_map_t charmap);
};
} // ns
#endif
