#ifndef _TTCACHE_H
#define _TTCACHE_H

#include "surface.h"

namespace myvi {

namespace load_mode_t {
	enum load_mode_t {
		LM_DEFAULT,
		LM_RENDER
	};
}

class stream_t {
public:
	u8 *buf;
	u32 buf_sz;
	u8 *ptr;
	u8 *end;
public:
	stream_t(u8 *_buf, u32 _buf_sz);
	u8 * advance(u32 sz);
	void read(u8 *dst, u32 sz);
	void write(u8 *src, u32 sz);
	void write_no_check(u8 *src, u32 sz);
	u8 * advance_no_check(u32 sz);
#ifdef PLATFORM_C28
	u8 read_u8();
	u32 read_u32();
#endif

};

#define CACHE_CMAP_MAGIC 1

class charmap_pair_t {
public:
	u32 magic;
	u32 obj_size;
	charmap_pair_t *next;
	u32 glyph_index;
	u32 ucode;		  // unicode code
public:
	charmap_pair_t() {
		magic = CACHE_CMAP_MAGIC;
		obj_size = sizeof(*this);
		glyph_index = 0, ucode = 0, next = 0;
	}
	static charmap_pair_t *read(stream_t &stream);
	void write(stream_t &stream);
};

#define CACHE_GLYPH_MAGIC 2

class glyph_t {
public:
	u32 magic;
	u32 obj_size;
	u32 glyph_index;
	s32	font_height;  // font pixel size h
	glyph_t *next;

	s32 bw; // bitmap.width
	s32 bh; // bitmap.rows
	u8 *buf; // bitmap.buffer
	s32 buf_sz;
	s32 aligned_sz;
	s32 bitmap_top;
	s32 bitmap_left;
	s32 mw; // metrics.width
	s32 mh; // metrics.height
	s32 hadvance; // metrics.horAdvance
	s32 vadvance; // metrics.vertAdvance
public:
	glyph_t() {
		magic = CACHE_GLYPH_MAGIC;
		obj_size = sizeof(*this);
		bw = 0,bh = 0, buf = 0, hadvance = 0, vadvance = 0;
		glyph_index = 0, font_height = 0, next = 0, buf_sz = 0, aligned_sz = 0;
	}
	void write(stream_t &stream);
	static glyph_t* read(stream_t &stream);
};

#define FNT_NAME_SIZE 32
#define CACHE_FNT_MAGIC 3

class font_handle_t {
public:
	u32 magic;
	u32 obj_size;
	font_handle_t *next;
	u32 * face;
	glyph_t *root;
	s32 charmap_count;
	charmap_pair_t *charmap;
	char *name;
	char name2[FNT_NAME_SIZE];
	s32	curr_height; // текущий размер шрифта
public:
	font_handle_t() {
		magic = CACHE_FNT_MAGIC;
		obj_size = sizeof(*this);
		face = (0),root = (0), name = 0, curr_height = 0;
		next = 0, charmap = 0, charmap_count = 0;
	}
	void write(stream_t &stream);
	static font_handle_t * read(stream_t &stream);
};

#define CACHE_HDR_MAGIC 4

class cache_header_t {
public:
	u32 magic;
	u32 obj_size;
public:
	cache_header_t() {
		magic = CACHE_HDR_MAGIC;
		obj_size = sizeof(*this);
	}
	void write(stream_t &stream);
	static cache_header_t * read(stream_t &stream);
};

class ttcache_t {
public:
	cache_header_t hdr;
	font_handle_t *handles;
	u32 total_size;
	//u8 dpi_x;
	//u8 dpi_y;
public:
	ttcache_t() {
		//dpi_x=(76),	dpi_y=(76);
		total_size = 0;
		handles = 0;
	}
	static void init_lib();
	void init(u8 *memdata, u32 data_sz);
	font_handle_t * open_face(char * _fname, u8 * _mem_font, u32 _mem_sz);
	void set_char_size_px(font_handle_t *hdl, s32 pxx, s32 pxy); // высота в пикселях
//	void set_char_size_pt(font_handle_t *hdl, s32 ptx, s32 pty); // высота в поинтах
	glyph_t* load_glyph(font_handle_t *hdl, u32 glyph_index, load_mode_t::load_mode_t mode);
	u32  get_char_index(font_handle_t *hdl, u32 ucode);

	u32 save(u8 *buf, u32 buf_sz);
};

namespace globals {
extern ttcache_t ttcache;
}
} // ns
#endif
