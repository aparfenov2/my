#include "truetype.h"
#include "assert_impl.h"
#include "bmp_math.h"
#include "ttcache.h"

using namespace myvi;

#define INTERSTEP 1


void ttype_font_t::init(char * folder, char * _fname, u8 * _mem_font, u32 _mem_sz) {

	_MY_ASSERT(!hdl && folder && _fname, return);

	hdl = globals::ttcache.open_face(folder, _fname,_mem_font,_mem_sz);
	_MY_ASSERT(hdl, return);
	set_char_size_px(0,8);
}


/*
int ttype_font_t::set_char_map(char_map_t charmap) {
	FT_Error error;
	_MY_ASSERT(ft_lib_initialied && initialized);
	switch (charmap) {
	case cmapUnicode :
		error = FT_Select_Charmap((FT_Face)face, FT_ENCODING_UNICODE);
		break;
	case cmapSymbol :
		error = FT_Select_Charmap((FT_Face)face, FT_ENCODING_MS_SYMBOL);
		break;
	}
	return error;
}
*/

typedef struct {
	u8 ansi_start;
	u16 utf8_start;
	u8 ansi_end;
	u16 utf8_end;
} utf8_range_t;

// from http://jrgraphix.net/r/Unicode/0400-04FF

utf8_range_t win1251_ucode_map[] = {
	{0xc0,0x0410,0xff,0x044f}, // А-я
	{0xa8,0x0401,0x00,0x0000}, // Ё
	{0xb8,0x0451,0x00,0x0000}, // ё
};

u16 get_unicode(u8 ansi_c, utf8_range_t* charmap, u32 map_sz) {
	u16 ret = ansi_c;
	if (ansi_c < charmap->ansi_start) {
		return ret;
	}
	for (utf8_range_t *p = 0, *pp = charmap; (u32)p < map_sz; p++,pp++) {
		if ((pp->ansi_end && (pp->ansi_start <= ansi_c && ansi_c <= pp->ansi_end)) ||
			(!pp->ansi_end && (ansi_c == pp->ansi_start))) {

			ret = pp->utf8_start + (ansi_c - pp->ansi_start);
			_MY_ASSERT((pp->ansi_end && ret <= pp->utf8_end) || (!pp->ansi_end && !pp->utf8_end), return 0);
			break;
		}
	}
	return ret;
}


void ttype_font_t::gly_to(s32 x,s32 y, surface_t &surface, u32 glyph_index) {
	_MY_ASSERT(hdl, return);

	gly = globals::ttcache.load_glyph(hdl,glyph_index,load_mode_t::LM_RENDER);
	if (!gly) return;
	if (gly->bw > 0 && gly->bh > 0) {
		char_surface.assign(gly->bw, gly->bh, BMP_GET_SIZE(gly->bw,gly->bh,8), gly->buf, img_type_t::bpp8);

//		char_surface.rect(0,0,bw,bh);
		char_surface.copy_to(0,0, gly->bw, gly->bh, x, y, surface);
	}
}


void ttype_font_t::print_to(s32 x,s32 y, surface_t &surface, string_t str) {

	_MY_ASSERT(hdl,return);
	int num_chars = str.length();

	s32 sw=0,sh=0;
	get_string_size(str,sw,sh);
	_MY_ASSERT(sw && sh,return);

//	surface.rect(x,y,sw,sh);

	y += sh;

	for (s32 n = 0; n < num_chars; n++ ) { /* load glyph image into the slot (erase previous one) */
		u32 ucode = get_unicode(str[n], win1251_ucode_map, sizeof(win1251_ucode_map));
		u32 glyph_index = globals::ttcache.get_char_index(hdl,ucode);

		gly = globals::ttcache.load_glyph(hdl,glyph_index,load_mode_t::LM_RENDER);
		if (!gly) continue;
//		int y2 = y - (((FT_Face)face)->glyph->metrics.height >> 6 );
		int y2 = y - gly->bitmap_top;
		int x2 = x + gly->bitmap_left;
		gly_to(x2,y2,surface,glyph_index);

		x += (gly->hadvance >> 6) + INTERSTEP;
	}

}


void ttype_font_t::print_to(s32 x,s32 y, surface_t &surface, string32_t str) {

	_MY_ASSERT(hdl,return);

	s32 sw=0,sh=0;
	get_string_size(str ,sw,sh);
	_MY_ASSERT(sw && sh,return);

//	surface.rect(x,y,sw,sh);

//	y += sh;
	s32 pw = 0;
	for (s32 n = 0; n < str.length(); n++ ) { /* load glyph image into the slot (erase previous one) */

		gly_to(x,y,surface,str[n]);

		_MY_ASSERT(gly,return);
		x += (gly->hadvance >> 6);
		pw += (gly->hadvance >> 6);
	}
	_MY_ASSERT(pw == sw,return);
}

void ttype_font_t::get_string_size(string_t &str, s32 &sw, s32 &sh) {

	_MY_ASSERT(hdl,return);

	int num_chars = str.length();

	s32 x = 0, y_max = 0;
	for (s32 n = 0; n < num_chars; n++ ) { /* load glyph image into the slot (erase previous one) */
		u32 ucode = get_unicode(str[n], win1251_ucode_map, sizeof(win1251_ucode_map));
		u32 glyph_index = globals::ttcache.get_char_index(hdl,ucode);

		gly = globals::ttcache.load_glyph(hdl,glyph_index,load_mode_t::LM_DEFAULT);
		if (!gly) continue;
		x += (gly->hadvance >> 6) + INTERSTEP;
		if (y_max < gly->vadvance) {
			y_max = gly->vadvance;
		}
	}
	sw = x;
	sh = y_max >> 6; 

	if (!sw) sw = 1;
	if (!sh) sh = 1;
}

void ttype_font_t::get_string_size(string32_t &str, s32 &sw, s32 &sh) {

	_MY_ASSERT(hdl,return);

	s32 x = 0, y_max = 0;
	for (s32 n = 0; n < str.length(); n++ ) { /* load glyph image into the slot (erase previous one) */

		gly = globals::ttcache.load_glyph(hdl,str[n],load_mode_t::LM_DEFAULT);
		if (!gly) continue;
		x += gly->hadvance >> 6;
		if (y_max < gly->vadvance) {
			y_max = gly->vadvance;
		}
	}
	sw = x;
	sh = y_max >> 6; 
	if (!sw) sw = 1;
	if (!sh) sh = 1;

}


void ttype_font_t::get_gly_size(u32 glyph_index, s32 &sw, s32 &sh) {
	_MY_ASSERT(hdl,return);
	gly = globals::ttcache.load_glyph(hdl,glyph_index,load_mode_t::LM_DEFAULT);
	if (gly) {
		sw = gly->mw >> 6;
		sh = gly->mh >> 6;
	}
	if (!sw) sw = 1;
	if (!sh) sh = 1;
}
