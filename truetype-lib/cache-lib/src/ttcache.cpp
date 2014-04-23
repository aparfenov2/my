#include "ttcache.h"
extern "C" {
	#include <string.h>
}

#include "truetype.h"
#include "assert_impl.h"
#include "bmp_math.h"


#ifdef TTCACHE_USE_FT
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

using namespace myvi;
//#include <stdio.h>
ttcache_t globals::ttcache;

#define STREAM_READ_SAFE

#define ALIGNED(ptr) (!(0x03 & (u32)(ptr)))

stream_t::stream_t(u8 *_buf, u32 _buf_sz) {
	_MY_ASSERT(_buf && _buf_sz && ALIGNED(_buf) && ALIGNED(_buf_sz),0);
	buf = _buf;
	buf_sz = _buf_sz;
	ptr = buf;
	end = buf + _buf_sz;
}

void stream_t::read(u8 *dst, u32 sz) {
	_MY_ASSERT((ptr + sz <= end),return);
	memcpy(dst,ptr,sz);
	ptr += sz;
}

u8 stream_t::read_u8() {
	_MY_ASSERT((ptr + 1 <= end),return 0);
	u8 ret = *ptr++;
	return ret;
}

u32 stream_t::read_u32() {
	_MY_ASSERT(ALIGNED(ptr) && (ptr + 4 <= end),return 0);
	u32 ret = *ptr++;
	ret |= (u32)((u32)(*ptr++) << 8);
	ret |= (u32)((u32)(*ptr++) << 16);
	ret |= (u32)((u32)(*ptr++) << 24);
	return ret;
}

void stream_t::write_u8(u8 v) {
	_MY_ASSERT((ptr + 1 <= end),return);
	*ptr++ = v;
}

void stream_t::write_u32(u32 v) {
	_MY_ASSERT(ALIGNED(ptr) && (ptr + 4 <= end),return);
	*ptr++ = (u8)v;
	*ptr++ = (u8)(v >> 8);
	*ptr++ = (u8)(v >> 16);
	*ptr++ = (u8)(v >> 24);
}

u8 * stream_t::advance(u32 sz) {
	_MY_ASSERT(sz && ALIGNED(sz),return 0);
	_MY_ASSERT(ALIGNED(ptr) && (ptr + sz <= end),return 0);
	u8 *ret = ptr;
	ptr += sz;
	return ret;
}

void stream_t::write(u8 *src, u32 sz) {
	_MY_ASSERT(src && sz && ALIGNED(src) && ALIGNED(sz),return);
	_MY_ASSERT(ALIGNED(ptr) && (ptr + sz <= end),return);
	memcpy(ptr,src,sz);
	ptr += sz;
}

u8 * stream_t::advance_no_check(u32 sz) {
	_MY_ASSERT(sz>0,return 0);
	_MY_ASSERT(ptr + sz <= end,return 0);
	u8 *ret = ptr;
	ptr += sz;
	return ret;
}

void stream_t::write_no_check(u8 *src, u32 sz) {
	_MY_ASSERT(src && sz,return);
	_MY_ASSERT(ptr + sz <= end,return);
	memcpy(ptr,src,sz);
	ptr += sz;
}


#ifdef TTCACHE_USE_FT
FT_Library library;
bool ft_lib_initialied = false;
#else
bool ft_lib_initialied = true;
#endif

void ttcache_t::init_lib() {

//	_MY_ASSERT(!ft_lib_initialied);

#ifdef TTCACHE_USE_FT
	FT_Error error;
	error = FT_Init_FreeType( &library );
	if (!error) {
		ft_lib_initialied = true;
	}
	_MY_ASSERT(!error,return);
#endif
}


#ifndef TTCACHE_USE_FT
font_handle_t * ttcache_t::open_face(char * folder, char * _fname, u8 * _mem_font, u32 _mem_sz) {
	_MY_ASSERT(_fname || _mem_font,return 0);
// search handle
	font_handle_t *hdl = handles;
	while (hdl) {
		if (!strncmp(hdl->name2, _fname, FNT_NAME_SIZE)) {
			return hdl;
		}
		hdl = hdl->next;
	}
	_MY_ASSERT(hdl,return 0);
	return hdl;
}
#endif


#ifdef TTCACHE_USE_FT
font_handle_t * ttcache_t::open_face(char * folder, char * _fname, u8 * _mem_font, u32 _mem_sz) {
	FT_Error error;
	_MY_ASSERT((folder && _fname) || _mem_font,return 0);
// search handle
	font_handle_t *hdl = handles;
	while (hdl) {
		if (!strncmp(hdl->name2, _fname, FNT_NAME_SIZE)) {
			if (ft_lib_initialied) {
				goto hdl_lib_init;
			} else {
				return hdl;
			}
		}
		hdl = hdl->next;
	}
// create new handle and access library
	hdl = new font_handle_t();
	if (!handles) {
		handles = hdl;
	} else {
		font_handle_t *p = handles;
		while (p) {
			if (!p->next) {
				p->next = hdl;
				break;
			}
			p = p->next;
		}
	}
	hdl->name = _fname;
	strncpy(hdl->name2,_fname,FNT_NAME_SIZE);

hdl_lib_init:
	if (_mem_font && _mem_sz) {
		error = FT_New_Memory_Face(library, (FT_Byte *) _mem_font, _mem_sz, 0, (FT_Face*) &hdl->face);
	} else {

		std::string fn1;
#ifndef __gnu_linux__
		fn1.append(folder);
		fn1.append("\\");
#else
//		fn1.append("./");
		fn1.append(folder);
		fn1.append("/");
#endif
		fn1.append(_fname);
		_LOG2("try open font ",fn1.c_str());
		error = FT_New_Face( library, fn1.c_str(), 0, (FT_Face*) &hdl->face );
	}
	if (error) {
		_LOG2("can't open font ",_fname);
	}
	_MY_ASSERT(!error,return 0);
	_MY_ASSERT(hdl,return 0);
	return hdl;
}
#endif


void ttcache_t::set_char_size_px(font_handle_t *hdl, s32 pxx, s32 pxy) {
#ifdef TTCACHE_USE_FT
	FT_Error error;
#endif
	_MY_ASSERT(hdl && hdl->face,return);
	_MY_ASSERT(!pxx,return);
#ifdef TTCACHE_USE_FT
	if (ft_lib_initialied) {
		error = FT_Set_Pixel_Sizes((FT_Face)hdl->face,pxx,pxy);
		_MY_ASSERT(!error,return);
	}
#endif
	hdl->curr_height = pxy;
}

// this method unsupported
//void ttcache_t::set_char_size_pt(font_handle_t *hdl, s32 ptx, s32 pty) {
//	FT_Error error;
//	_MY_ASSERT(ft_lib_initialied && hdl && hdl->face);
//	_MY_ASSERT(0);
//	error = FT_Set_Char_Size(
//			(FT_Face)hdl->face, /* handle to face object */
//			ptx*64, /* char_width in 1/64th of points */
//			pty*64, /* char_height in 1/64th of points */
//			dpi_x, /* horizontal device resolution */
//			dpi_y); /* vertical device resolution */
//	_MY_ASSERT(!error);
//}

glyph_t * ttcache_t::load_glyph(font_handle_t *hdl, u32 glyph_index, load_mode_t::load_mode_t mode) {
	_MY_ASSERT(hdl && hdl->face,return 0);


	// lookup in cache

	glyph_t *gly = hdl->root, *pp = 0;
	while (gly) {
		if (gly->glyph_index == glyph_index && gly->font_height == hdl->curr_height) {
			if (mode == load_mode_t::LM_DEFAULT || (mode == load_mode_t::LM_RENDER && gly->buf)) {
				return gly;
			} else break;
		}
		pp = gly;
		gly = gly->next;
	}

#ifdef TTCACHE_USE_FT
	if (ft_lib_initialied) {
		u32 mde = (mode == load_mode_t::LM_DEFAULT) ? FT_LOAD_DEFAULT : FT_LOAD_RENDER;
		FT_Error error = FT_Load_Glyph( (FT_Face)hdl->face, glyph_index, mde );
		_MY_ASSERT(!error,return 0);
		if (!gly) {
			gly = new glyph_t();
			total_size += sizeof(glyph_t);
			if (!pp) hdl->root = gly;
			else pp->next = gly;
		}

		gly->font_height = hdl->curr_height;
		gly->glyph_index = glyph_index;

		gly->bh = ((FT_Face)hdl->face)->glyph->bitmap.rows;
		gly->bw = ((FT_Face)hdl->face)->glyph->bitmap.width;

		if (!gly->buf && mode == load_mode_t::LM_RENDER && ((FT_Face)hdl->face)->glyph->bitmap.buffer) {

			gly->buf_sz = BMP_GET_SIZE(gly->bw,gly->bh,8);
			total_size += gly->buf_sz;

			gly->buf = (u8 *)malloc(gly->buf_sz);
			memcpy(gly->buf,((FT_Face)hdl->face)->glyph->bitmap.buffer,gly->buf_sz);

			gly->bitmap_top = ((FT_Face)hdl->face)->glyph->bitmap_top;
			gly->bitmap_left = ((FT_Face)hdl->face)->glyph->bitmap_left;
		}
		gly->vadvance = ((FT_Face)hdl->face)->glyph->metrics.vertAdvance;
		gly->hadvance = ((FT_Face)hdl->face)->glyph->metrics.horiAdvance;
		gly->mw = ((FT_Face)hdl->face)->glyph->metrics.width;
		gly->mh = ((FT_Face)hdl->face)->glyph->metrics.height;
	}
#endif
	_WEAK_ASSERT(gly,return 0);
	return gly;
}



u32  ttcache_t::get_char_index(font_handle_t *hdl, u32 ucode) {

	_MY_ASSERT(hdl && hdl->face,return 0);

// lookup in cache
	charmap_pair_t *p = hdl->charmap;
	while (p) {
		if (p->ucode == ucode) {
			return p->glyph_index;
		}
		p = p->next;
	}
#ifdef TTCACHE_USE_FT
	if (ft_lib_initialied) {
		u32 glyph_index = FT_Get_Char_Index( (FT_Face)hdl->face, ucode );

		p = hdl->charmap;
		charmap_pair_t *pp = 0;
		while (p) {
			if (p->glyph_index == glyph_index) {
				p->ucode = ucode;
				break;
			}
			pp = p;
			p = p->next;
		}
		if (!p) {
			p = new charmap_pair_t();
			if (pp)	pp->next = p;
			else hdl->charmap = p;
			hdl->charmap_count++;

			p->glyph_index = glyph_index;
			p->ucode = ucode;
		}
		_WEAK_ASSERT(glyph_index, return 0);
		return glyph_index;
	}
#else
	_WEAK_ASSERT(0, return 0);
#endif
	return 0;
}

// ================================= stream IO =================================

u32 ttcache_t::save(u8 *buf, u32 buf_sz) {
	_MY_ASSERT(handles,return 0);

	stream_t stream(buf, buf_sz);
	hdr.write(stream);
	font_handle_t *hdl = handles;
	while (hdl) {
		hdl->write(stream);
		hdl = hdl->next;
	}
	u32 ret = stream.ptr - stream.buf;
	_MY_ASSERT(ret && ret < buf_sz,return 0);
	return ret;
}

void ttcache_t::init(u8 *memdata, u32 data_sz) {

	stream_t stream(memdata, data_sz);

	cache_header_t::read(stream);
// read 1st handle
	font_handle_t *hdl, *phdl = 0;
	hdl = font_handle_t::read(stream);
	handles = hdl;
// restore hdl chain
	while(hdl->next) {
		phdl = hdl;
		hdl = font_handle_t::read(stream);
		phdl->next = hdl;
	}
	u32 sz = stream.ptr - stream.buf;
	_MY_ASSERT(sz == data_sz,return);
}

cache_header_t *cache_header_t::read(stream_t &stream) {
#ifdef STREAM_READ_SAFE
	cache_header_t *cp = new cache_header_t();
	//stream.read((u8*)cp,sizeof(*cp));
	cp->magic = stream.read_u32();
	cp->obj_size = stream.read_u32();
#else
	cache_header_t *cp = (cache_header_t *) stream.advance(sizeof(*cp));
#endif
	_MY_ASSERT(cp->magic == CACHE_HDR_MAGIC,return 0);
#ifndef PLATFORM_C28
	_MY_ASSERT(cp->obj_size == sizeof(*cp), return 0);
#endif
	return cp;
}


void cache_header_t::write(stream_t &stream) {
	stream.write_u32(this->magic);
	stream.write_u32(this->obj_size);
}


charmap_pair_t *charmap_pair_t::read(stream_t &stream) {
#ifdef STREAM_READ_SAFE
	charmap_pair_t *cp = new charmap_pair_t();
//	stream.read((u8*)cp,sizeof(*cp));
	cp->magic = stream.read_u32();
	cp->obj_size = stream.read_u32();
	cp->next = (charmap_pair_t*)stream.read_u32();
	cp->glyph_index = stream.read_u32();
	cp->ucode = stream.read_u32();
#else
	charmap_pair_t *cp = (charmap_pair_t *) stream.advance(sizeof(*cp));
#endif
	_MY_ASSERT(cp->magic == CACHE_CMAP_MAGIC,return 0);
#ifndef PLATFORM_C28
	_MY_ASSERT(cp->obj_size == sizeof(*cp),return 0);
#endif
	return cp;
}

void charmap_pair_t::write(stream_t &stream) {
	stream.write_u32(this->magic);
	stream.write_u32(this->obj_size);
	stream.write_u32((u32)this->next);
	stream.write_u32(this->glyph_index);
	stream.write_u32(this->ucode);
}


void font_handle_t::write(stream_t &stream) {
	_MY_ASSERT(root,return);

	stream.write_u32(this->magic);
	stream.write_u32(this->obj_size);
	stream.write_u32((u32)this->next);
	stream.write_u32((u32)this->face);
	stream.write_u32((u32)this->root);
	stream.write_u32(this->charmap_count);
	stream.write_u32((u32)this->charmap);
	stream.write_u32((u32)this->name);
	stream.write((u8*)this->name2, sizeof(this->name2));
	stream.write_u32(this->curr_height);

	s32 cp_cnt = 0;
	charmap_pair_t *cp = charmap;
	while (cp) {
		cp->write(stream);
		cp = cp->next;
		cp_cnt++;
	}
	_MY_ASSERT(cp_cnt == charmap_count,return);

	glyph_t *gp = root;
	while (gp) {
		gp->write(stream);
		gp = gp->next;
	}
}

font_handle_t * font_handle_t::read(stream_t &stream) {

#ifdef STREAM_READ_SAFE
	font_handle_t *_this = new font_handle_t();
//	stream.read((u8*)_this,sizeof(*_this));
	_this->magic = stream.read_u32();
	_this->obj_size = stream.read_u32();
	_this->next = (font_handle_t *)stream.read_u32();
	_this->face = (u32 *)stream.read_u32();
	_this->root = (glyph_t *)stream.read_u32();
	_this->charmap_count = stream.read_u32();
	_this->charmap = (charmap_pair_t*)stream.read_u32();
	_this->name = (char *)stream.read_u32();
	stream.read((u8 *)_this->name2,sizeof(_this->name2));
	_this->curr_height = stream.read_u32();

#else
	font_handle_t *_this = (font_handle_t *) stream.advance(sizeof(*_this));
#endif

	_MY_ASSERT(_this->magic == CACHE_FNT_MAGIC,return 0);
#ifndef PLATFORM_C28
	_MY_ASSERT(_this->obj_size == sizeof(*_this),return 0);
#endif
	_this->name = _this->name2;
// read charmap
	if (_this->charmap_count) {
		charmap_pair_t *cp = charmap_pair_t::read(stream);
		charmap_pair_t *pcp = 0;
		_this->charmap = cp;
		s32 cp_cnt = 1;
		while (cp->next) {
			pcp = cp;
			cp = charmap_pair_t::read(stream);
			pcp->next = cp;
			cp_cnt++;
		}
		_MY_ASSERT(cp_cnt == _this->charmap_count, return 0);
	}
// read glyphs
	glyph_t *pgly = 0, *gly;
	gly = glyph_t::read(stream);
	_this->root = gly;
	while(gly->next) {
		pgly = gly;
		gly = glyph_t::read(stream);
		pgly->next = gly;
	}
	return _this;
}

void glyph_t::write(stream_t &stream) {
	if (buf && buf_sz) {
		aligned_sz = buf_sz;
		for(;!ALIGNED(aligned_sz); aligned_sz++);
	}
//	stream.write((u8*)this,sizeof(*this));

	stream.write_u32(this->magic);
	stream.write_u32(this->obj_size);
	stream.write_u32(this->glyph_index);
	stream.write_u32(this->font_height);
	stream.write_u32((u32)this->next);
	stream.write_u32(this->bw);
	stream.write_u32(this->bh);
	stream.write_u32((u32)this->buf);
	stream.write_u32(this->buf_sz);
	stream.write_u32(this->aligned_sz);
	stream.write_u32(this->bitmap_top);
	stream.write_u32(this->bitmap_left);
	stream.write_u32(this->mw);
	stream.write_u32(this->mh);
	stream.write_u32(this->hadvance);
	stream.write_u32(this->vadvance);
//	_LOG2("w:",this->vadvance);

	if (buf && buf_sz) {
		stream.write_no_check((u8*)buf,buf_sz);
		if (aligned_sz - buf_sz) {
			stream.advance_no_check(aligned_sz - buf_sz);
		}
	}
}

glyph_t* glyph_t::read(stream_t &stream) {
#ifdef STREAM_READ_SAFE
	glyph_t *_this = new glyph_t();
//	stream.read((u8*)_this,sizeof(*_this));
	_this->magic = stream.read_u32();
	_this->obj_size = stream.read_u32();
	_this->glyph_index = stream.read_u32();
	_this->font_height = stream.read_u32();
	_this->next = (glyph_t *)stream.read_u32();
	_this->bw = stream.read_u32();
	_this->bh = stream.read_u32();
	_this->buf = (u8 *)stream.read_u32();
	_this->buf_sz = stream.read_u32();
	_this->aligned_sz = stream.read_u32();
	_this->bitmap_top = stream.read_u32();
	_this->bitmap_left = stream.read_u32();
	_this->mw = stream.read_u32();
	_this->mh = stream.read_u32();
	_this->hadvance = stream.read_u32();
	_this->vadvance = stream.read_u32();
//	_LOG2("r:",_this->vadvance);

#else
	glyph_t *_this = (glyph_t *)stream.advance(sizeof(*_this));
#endif

//	printf("ptr:%x, mag:%x",_this, _this->magic);
	_MY_ASSERT(_this->magic == CACHE_GLYPH_MAGIC,return 0);
#ifndef PLATFORM_C28
	_MY_ASSERT(_this->obj_size == sizeof(*_this),return 0);
#endif

	if (_this->buf && _this->buf_sz) {
		_MY_ASSERT(_this->buf_sz <= _this->aligned_sz, return 0);
//#ifdef STREAM_READ_SAFE
//		_this->buf = new u8[_this->aligned_sz];
//		stream.read(_this->buf,_this->aligned_sz);
//#else
//		_this->buf = stream.advance(_this->aligned_sz);
//#endif
		_this->buf = stream.advance(_this->aligned_sz);
	}
	return _this;
}
