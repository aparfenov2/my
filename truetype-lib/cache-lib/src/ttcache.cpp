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

#include "ttcache.pb.h"
#ifdef PLATFORM_C28
#include "pb_decode.h"
#endif

using namespace myvi;
//#include <stdio.h>
ttcache_t globals::ttcache;


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
	_MY_ASSERT(hdl,return);
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
	_MY_ASSERT(hdl,return 0);


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
			if (!pp) hdl->root = gly;
			else pp->next = gly;
		}

		gly->font_height = hdl->curr_height;
		gly->glyph_index = glyph_index;

		gly->bh = ((FT_Face)hdl->face)->glyph->bitmap.rows;
		gly->bw = ((FT_Face)hdl->face)->glyph->bitmap.width;

		if (!gly->buf && mode == load_mode_t::LM_RENDER && ((FT_Face)hdl->face)->glyph->bitmap.buffer) {

			gly->buf_sz = BMP_GET_SIZE(gly->bw,gly->bh,8);

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

	_MY_ASSERT(hdl,return 0);

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


class charmap_pair_impl_t : public charmap_pair_t {
public:
	//charmap_pair_impl_t(charmap_pair_t *src) {
	//	glyph_index = src->glyph_index, ucode = src->ucode, next = src->next;
	//}

#ifdef PLATFORM_C28
	charmap_pair_impl_t(const myvi_proto_charmap_pair_t &src) {
		glyph_index = src.glyph_index;
		ucode = src.ucode;
		next = 0;
	}
#else
	charmap_pair_impl_t(const proto::charmap_pair_t &src) {
		glyph_index = src.glyph_index();
		ucode = src.ucode();
		next = 0;
	}
	void make_proto(proto::charmap_pair_t *src) {
		src->set_glyph_index(glyph_index);
		src->set_ucode(ucode);
	}
#endif
};

class glyph_impl_t : public glyph_t {
public:
#ifdef PLATFORM_C28
	glyph_impl_t(const myvi_proto_glyph_t &src) {
		bw = src.bw;
		bh = src.bh;
		buf_sz = src.buf_sz;
//		_MY_ASSERT(src.buf_tag, return);
		buf = (u8*)src.buf_tag;
//		memcpy(buf, src.data().data(), buf_sz);

		hadvance = src.hadvance;
		vadvance = src.vadvance;
		glyph_index = src.glyph_index;
		font_height = src.font_height;
		next = 0;
		bitmap_top = src.bitmap_top;
		bitmap_left = src.bitmap_left;
		mw = src.mw;
		mh = src.mh;

	}
#else
	glyph_impl_t(const proto::glyph_t &src) {
		bw = src.bw();
		bh = src.bh();

		buf_sz = src.buf_sz();
		_MY_ASSERT(buf_sz == src.data().size(),);
		buf = 0;
		if (buf_sz) {
			buf = new u8[buf_sz];
			memcpy(buf, src.data().data(), buf_sz);
		}

		hadvance = src.hadvance();
		vadvance = src.vadvance();
		glyph_index = src.glyph_index();
		font_height = src.font_height();
		next = 0;
		bitmap_top = src.bitmap_top();
		bitmap_left = src.bitmap_left();
		mw = src.mw();
		mh = src.mh();

	}

	void make_proto(proto::glyph_t *src) {

		src->set_bw(this->bw);
		src->set_bh(this->bh);
		src->set_hadvance(this->hadvance);
		src->set_vadvance(this->vadvance);
		src->set_glyph_index(this->glyph_index);
		src->set_font_height(this->font_height);

		src->set_buf_sz(this->buf_sz);
		src->set_data(this->buf, this->buf_sz);
		src->set_bitmap_top(this->bitmap_top);
		src->set_bitmap_left(this->bitmap_left);
		src->set_mw(this->mw);
		src->set_mh(this->mh);
	}
#endif
};

#ifdef PLATFORM_C28


bool buf_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {

    myvi_proto_glyph_t *rec  = (myvi_proto_glyph_t *)*arg;
    rec->buf_tag = 0;
    if (!rec->buf_sz) return true;
    _MY_ASSERT(rec->buf_sz == stream->bytes_left, return false);
    u8 *buf = new u8[rec->buf_sz];
    rec->buf_tag = (u32)buf;

    bool stat = pb_read(stream, buf, rec->buf_sz);
    _MY_ASSERT(stat, return false);
    return stat;
}

//u8 buf_stub[100];

bool glyph_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {

	myvi_proto_font_handle_t *hdl = (myvi_proto_font_handle_t *)*arg;

    myvi_proto_glyph_t rec;
    memset(&rec,0, sizeof(rec));
    rec.data.arg = &rec;
    rec.data.funcs.decode = buf_callback;

    if (!pb_decode(stream, myvi_proto_glyph_t_fields, &rec))
        return false;

//    rec.buf_tag = (uint32_t)&buf_stub;

//    _MY_ASSERT(rec.buf_tag, return false);
    glyph_impl_t *gi = new glyph_impl_t(rec);

    if (!hdl->first_root_tag) {
    	hdl->first_root_tag = (uint32_t)gi;
    }
    if (hdl->cur_root_tag) {
    	((glyph_impl_t *)hdl->cur_root_tag)->next =  gi;
    }
	hdl->cur_root_tag = (uint32_t)gi;

    return true;
}

bool charmap_callback(pb_istream_t *stream, const pb_field_t *field, void **arg) {

	myvi_proto_font_handle_t *hdl = (myvi_proto_font_handle_t *)*arg;

    myvi_proto_charmap_pair_t rec;
    memset(&rec,0, sizeof(rec));
    if (!pb_decode(stream, myvi_proto_charmap_pair_t_fields, &rec))
        return false;

    charmap_pair_t *cm = new charmap_pair_impl_t(rec);

    if (!hdl->first_charmap_tag) {
    	hdl->first_charmap_tag = (uint32_t)cm;
    }
    if (hdl->cur_charmap_tag) {
    	((charmap_pair_t *)hdl->cur_charmap_tag)->next = cm;
    }
	hdl->cur_charmap_tag = (uint32_t)cm;

    return true;
}

#endif

class font_handle_impl_t : public font_handle_t {
public:
#ifdef PLATFORM_C28
	font_handle_impl_t(const myvi_proto_font_handle_t &src) {
		face = 0;
		memcpy(name2, src.name.bytes, src.name.size);
		curr_height = src.curr_height;
		next = 0;

		_MY_ASSERT(src.first_root_tag, return);
		root = (glyph_t*) src.first_root_tag;
		_MY_ASSERT(src.first_charmap_tag, return);
		charmap = (charmap_pair_t*) src.first_charmap_tag;
	}
#else

	font_handle_impl_t(const proto::font_handle_t &src) {
		face = 0;
		root = (0);
		src.name().copy(name2,FNT_NAME_SIZE);
//		memcpy(name2, src.name().data(), FNT_NAME_SIZE);
		curr_height = src.curr_height();
		next = 0;
		charmap = 0;
		charmap_pair_t *cm, *pcm = 0;

		for (int i=0; i < src.charmap_size(); i++) {

			cm = new charmap_pair_impl_t(src.charmap(i));

			if (!charmap) {
				charmap = cm;
			}
			if (pcm) {
				pcm->next = cm;
			}
			pcm = cm;
		}

		glyph_t *g, *pg = 0;
		for (int i=0; i < src.root_size(); i++) {

			g = new glyph_impl_t(src.root(i));

			if (!root) {
				root = g;
			}
			if (pg) {
				pg->next = g;
			}
			pg = g;
		}

	}

	void make_proto(proto::font_handle_t *src) {
		src->set_name(this->name2, FNT_NAME_SIZE);
		src->set_curr_height(this->curr_height);

		charmap_pair_t *cm = charmap;
		while (cm) {
//			charmap_pair_impl_t cmi(cm);
			((charmap_pair_impl_t*)cm)->make_proto(src->add_charmap());
			cm = cm->next;
		}
		glyph_t *g = root;
		while (g) {
//			glyph_impl_t gi(g);
			((glyph_impl_t*)g)->make_proto(src->add_root());
			g = g->next;
		}
	}
#endif

};


#ifdef PLATFORM_C28


void ttcache_t::init(u8 *memdata, u32 data_sz) {

	myvi_proto_ttcache_t rec;
	memset(&rec,0, sizeof(rec));

	pb_istream_t istream = pb_istream_from_buffer((uint8_t*)memdata, data_sz);

	for (int i=0; i < sizeof(rec.handles) / sizeof(rec.handles[0]); i++) {
		rec.handles[i].charmap.funcs.decode = charmap_callback;
		rec.handles[i].charmap.arg = &rec.handles[i];
		rec.handles[i].root.funcs.decode = glyph_callback;
		rec.handles[i].root.arg = &rec.handles[i];
	}

	bool status = pb_decode(&istream, myvi_proto_ttcache_t_fields, &rec);
	_MY_ASSERT(status, return);
// read 1st handle
	font_handle_t *hdl, *phdl = 0;

	for (int i=0; i < rec.handles_count; i++) {
		if (!(rec.handles[i].first_root_tag)) {
			break;
		}
		hdl = new font_handle_impl_t(rec.handles[i]);
		if (!handles) {
			handles = hdl;
		}
		if (phdl) {
			phdl->next = hdl;
		}
		phdl = hdl;
	}
	
}

#else
// Win32 impl
u32 ttcache_t::save(u8 *buf, u32 buf_sz) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	
	_MY_ASSERT(handles,return 0);

	proto::ttcache_t rec;

	font_handle_t *hdl = handles;
	while (hdl) {
		proto::font_handle_t *hrec = rec.add_handles();
		((font_handle_impl_t*)hdl)->make_proto(hrec);
		hdl = hdl->next;
	}

	u32 ret = rec.ByteSize();
	_MY_ASSERT(ret && ret < buf_sz,return 0);
	rec.SerializeToArray(buf, buf_sz);
	return ret;
}

void ttcache_t::init(u8 *memdata, u32 data_sz) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	proto::ttcache_t rec;
	rec.ParseFromArray(memdata, data_sz);
	font_handle_t *hdl, *phdl = 0;

	for (int i=0; i < rec.handles_size(); i++) {
		hdl = new font_handle_impl_t(rec.handles(i));
		if (!handles) {
			handles = hdl;
		}
		if (phdl) {
			phdl->next = hdl;
		}
		phdl = hdl;
	}
	
}
#endif
