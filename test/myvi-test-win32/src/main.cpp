#include <tchar.h>

#include "surface.h"

#include "bmp_math.h"

#include "assert_impl.h"

#include "test_tools.h"
#include "resources.h"
#include "widgets.h"

#include "ttcache.h"

#include <iostream>
#include <fstream>


#include "link.h"
#include "disp_def.h"

#include "menu_common.h"
#include "custom_common.h"
#include "link_facade.h"
#include "link_model_updater.h"

#include "rapidxml_utils.hpp"


using namespace std;
using namespace myvi;

#define _MAX_INT 2147483647

u8 buf0[BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16)];
surface_16bpp_t s1(TFT_WIDTH,TFT_HEIGHT,BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16), buf0);

extern resources_t res;



void save_ttcache() {
	u32 sz = 1024 * 200; 
	u8 *buf = new u8[sz];
	u32 dsz = globals::ttcache.save(buf,sz);

	ofstream outfile ("bmp\\ttcache.dat",ofstream::binary);
	outfile.write((char *)buf, dsz);
	outfile.close();

	_LOG2("font cache saved, size: " ,dsz );

//	globals::ttcache.init(buf,dsz);
	delete[] buf;
}



// ------------------------------- весь экран ------------------------------------

typedef custom::dynamic_view_mixin_aware_impl_t<gobject_t> _test_screen_super_t;

class test_screen_t : public _test_screen_super_t, public focus_aware_t {
	typedef _test_screen_super_t super;
public:
//	custom::tedit_t hdr_box;
//	custom::scrollable_menu_t scrollable;


public:

	virtual void init() OVERRIDE {

		w = TFT_WIDTH;
		h = TFT_HEIGHT;

//		rasterizer_t::debug = true;


		gen::view_meta_t *root_view_meta = gen::meta_registry_t::instance().find_view_meta("root");
		gobject_t *root_view = custom::view_meta_ex_t(root_view_meta).build_view_no_ctx();

		add_child(root_view, "root");

		root_view->x = 0;
		root_view->y = 0;
		root_view->w = w;
		root_view->h = h;


		init_children();

		do_layout();
		dirty = true;

	}

	virtual void render(surface_t &dst) OVERRIDE {
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0xf9f9f9;//0x203E95;
		s32 ax,ay;
		translate(ax,ay);
		dst.fill(ax,ay,w,h);

	}


	virtual void set_dirty(bool dirty) OVERRIDE {
		super::set_dirty(dirty);
		if (dirty) {
			int i = 0;
		}
	}


};


class my_test_drawer_t : public test::test_drawer_t {
public:
	s32 kx;
	s32 ky;
	gobject_t *gobj;
	bool drawFlag;
public:
	my_test_drawer_t(gobject_t *agobj) {
		drawFlag = false;
		gobj = agobj;
		w = TFT_WIDTH * 2;
		h = TFT_HEIGHT * 2;
		kx = 2;
		ky = 2;
		std::cout << "set size to " << w << "x" << h << std::endl;
	}

	void process_mouse(s32 mx, s32 my, mkey_t::mkey_t mkey) {

		myvi::gobject_t * captured = gobj;
		if (myvi::focus_manager_t::instance().captured.length() > 0) {
			captured = myvi::focus_manager_t::instance().captured.last();
		}

		myvi::gobject_t::iterator_visible_deep_t iter = captured->iterator_visible_deep();
		myvi::gobject_t *p = iter.next();
		while(p) {
			s32 ax, ay;
			p->translate(ax, ay);
			if (ax < mx && mx < ax + p->w && ay < my && my < ay + p->h && mkey) {
				custom::mouse_aware_t * ma = dynamic_cast<custom::mouse_aware_t *>(p);
				if (ma) {
					ma->mouse_event(mkey);
					break;
				}
			}
			p = iter.next();
		}
	}

	virtual bool callback(key_t::key_t key, s32 mx, s32 my, mkey_t::mkey_t mkey) OVERRIDE {

		if (key == key_t::K_SAVE) {
			save_ttcache();
		}
		static mkey_t::mkey_t mkey_last = mkey_t::MK_NONE;
		if (mkey != mkey_last) {
			mkey_last = mkey;
			if (mkey) {
				mx = mx / kx;
				my = my / ky;
				process_mouse(mx,my,mkey);
			}
		}

		if (key) {
			// сначала отдаем на перехват фильтрам клавиатуры
			if (!custom::keyboard_filter_chain_t::instance().process_key(key)) {

				focus_aware_t * focus_aware = dynamic_cast<focus_aware_t*>(gobj);
				if (focus_aware) {
					focus_aware->key_event((key_t::key_t)key);
				}
			}
		}
		bool ret = rasterizer_t::render(gobj, s1);
		if (ret) {
			drawFlag = !drawFlag;
			s1.putpx(0,0, drawFlag ? 0x00ff00 : 0xff0000);
		}
		return ret;
	}

};


font_size_t::font_size_t sizes[] = 
		{font_size_t::FS_8, font_size_t::FS_15, font_size_t::FS_20, font_size_t::FS_30 ,(font_size_t::font_size_t)0};

void print_chars(ttype_font_t &fnt, surface_t &s1, const char * chars) {
	for (s32 s=0; s<100 && sizes[s]; s++ ) {
		fnt.set_char_size_px(0,sizes[s]);
		fnt.print_to(0,0,s1,string_t(chars));
	}
}

void print_chars_gly(ttype_font_t &fnt, surface_t &s1, u32 *str) {
	for (s32 s=0; s<100 && sizes[s]; s++ ) {
		fnt.set_char_size_px(0,sizes[s]);
		fnt.print_to(0,0,s1,string32_t(str));
	}
}

int _tmain(int argc, _TCHAR* argv[]) {

	if (argc < 2) {
		cout << "arg0 : COM port name, arg2: --nottf" << endl;
		return -1;
	}
	_LOG1("log started");

	_TCHAR* com_port_name = argv[1];
	bool no_ttf = false;
	bool host_mode = false;
// parse command line
	for (s32 i=0; i < argc; i++) {
		if (i < 2) continue;
		if (_tcscmp(argv[i], _T("--nottf")) == 0) no_ttf = true;
		if (_tcscmp(argv[i], _T("--host")) == 0) host_mode = true;
	}
// init ttcache
	if (!no_ttf) {
		ttcache_t::init_lib();
	} else {
		cout << "skipped ttcache_t::init_lib() " << endl;
	}

	ifstream infile ("bmp\\ttcache.dat",ofstream::binary);
	if (!infile) {
		_LOG1("cant open font cache file");
	} else {
		infile.seekg( 0, std::ios::end );
		u32 sz = (u32)infile.tellg();
		infile.seekg( 0, std::ios::beg );
		u8 *buf = new u8[sz];
		infile.read((char *)buf, sz);
		infile.close();

		globals::ttcache.init(buf,sz);
		_LOG2("font cache loaded, size: ",sz);
	}

	res.init();
// standard values
	static u32 gly_str[] = {0x0026,0x0027,0x0028,0x0029,0x002a,0x002b,0x002c,0x002d,0x002e,0x002f,0x0030,0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x00};
	print_chars_gly(res.gly,s1,gly_str);
	print_chars(res.ttf,s1,"abcdefghijklmnopqrstuvwxyz");
	print_chars(res.ttf,s1,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	print_chars(res.ttf,s1,"абвгдеЄжзийклмнопрстуфхцчщшьыъэю€");
	print_chars(res.ttf,s1,"јЅ¬√ƒ≈®∆«»… ЋћЌќѕ–—“”‘’÷„ўЎ№џЏЁёя");
	print_chars(res.ttf,s1,"`1234567890-=\\~!@#$%^&*()_+|[]{};':"",./<>?");

	_MY_ASSERT(com_port_name, return -1);
	test::serial_port_t port(com_port_name);
	test::serial_interface_impl_t sintf;
	sintf.init(port);

	char *wnd_title = "<title>";
	if (!host_mode) {
		_LOG1("link_mode: slave");
		wnd_title = "myvi: slave";
		// локальна€ роль
		custom::link_model_updater_t *link_model_updater = new custom::link_model_updater_t();
		link::local_facade_t *local_facade = new link::local_facade_t();
		local_facade->init(link_model_updater, &sintf);
		link_model_updater->init(local_facade->get_sys_interface(), 0);

	} else {
		_LOG1("link_mode: host");
		wnd_title = "myvi: host";
		// роль хоста
		custom::link_model_repeater_t *link_model_repeater = new custom::link_model_repeater_t();
		link::host_serializer_t *host_serializer = new link::host_serializer_t();
		host_serializer->init(link_model_repeater, &sintf);

		link_model_repeater->init(host_serializer, 0);
	}

	rapidxml::file<> xml("gen\\ui_ru.xml");
	gen::meta_registry_t::instance().init(xml.data());
	
	test_screen_t test_screen;
	test_screen.init(); // init whole tree

	modal_overlay_t::instance().w = TFT_WIDTH;
	modal_overlay_t::instance().h = TFT_HEIGHT;
	modal_overlay_t::instance().push_modal(&test_screen);

	my_test_drawer_t test_drawer(& modal_overlay_t::instance());
	test_drawer.create_window(s1, wnd_title);

	bool exit = false;
	while (!exit) {
		exit = test_drawer.cycle(s1);
		sintf.cycle();
	}

	return 0;
}

