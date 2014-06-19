#ifndef _TEST_PLAYGROUND
#define _TEST_PLAYGROUND

#include "disp_def.h"
#include "menu_common.h"
#include "resources.h"

#include <vector>
#include <sstream>
#include <string>

class suffixes_t : public myvi::iterator_t<myvi::string_t> {
public:
	myvi::string_t str1;
	myvi::string_t str2;
public:
	suffixes_t() {
		str1 = myvi::string_t("A");
		str2 = myvi::string_t("B");
	}

	virtual myvi::string_t* next(void* prev) OVERRIDE {
		if (!prev) return &str1;
		if (prev == &str1) return &str2;
		return 0;
	}

};

// —трока из текстбокса и комбобокса с суффиксами

class tedit_t : public myvi::inp_dlg_row_t {
public:

	virtual void do_layout() OVERRIDE {
		_MY_ASSERT(w && h, return);

		lval.w = (s32)(w * 0.7);
		lval.h = h;
		lsfx.h = h;
		lsfx.x = lval.w + 5;
		lsfx.w = w - lsfx.x;

		layout_children();
	}
};

// строка из названи€ и tedit-a справа
class menu_row_t :  public myvi::gobject_t {
public:
	myvi::label_t lname;
	tedit_t valbox;
	myvi::stack_layout_t stack_layout;
public:

	menu_row_t() {
		stack_layout.vertical = false;
		layout = &stack_layout;
		add_child(&lname);
		add_child(&valbox);
	}

	virtual void init() {
		myvi::gobject_t::init();

		myvi::menu_context_t &ctx = myvi::menu_context_t::instance();

		lname.visible = true;
		lname.ctx = ctx.lctx1;
		lname.ctx.font_size = myvi::font_size_t::FS_20;

	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		_MY_ASSERT(parent,return);
		parent->dirty = dirty;
	}


	virtual void do_layout() OVERRIDE {
		_MY_ASSERT(w && h, return);
		lname.w = (s32)(w * 0.7);
		lname.h = h;
		valbox.h = h;
		valbox.x = lname.w + 5;
		valbox.w = w - valbox.x;

		layout_children();
	}


};

// ----------------- пример наследовани€ окна с прокруткой --------------------------


// наследник класса внутреннего окна
class scrollable_menu_interior_t : public myvi::scrollable_interior_t {
	typedef scrollable_interior_t super;
public:
	suffixes_t suffixes;
	myvi::stack_layout_t stack_layout;
public:

	scrollable_menu_interior_t() {

		stack_layout.preferred_item_size = true;
		layout = &stack_layout;

		for (s32 i=0; i < 12; i++) {
			children.push_back(new menu_row_t());
		}
	}

	void init_row(menu_row_t *row, int i) {

		std::stringstream str;
		str << "Menu_name " << i;

		row->valbox.lval.value = "Value";
		row->valbox.lsfx.values = &suffixes;
		row->valbox.lsfx.value = *suffixes.next(0);
		std::string *s = new std::string(str.str());
		row->lname.text = (*s).c_str();
	}

	virtual void init() OVERRIDE {
		super::init();

		gobject_t::iterator_visible_t iter = this->iterator_visible();
		gobject_t *p = iter.next();
		int i = 0;
		while (p) {
			init_row((menu_row_t *)p, i++);
			p = iter.next();
		}

	}

};




// наследник класса окна с прокруткой
class scrollable_menu_t : public myvi::scrollable_window_t {
	typedef scrollable_window_t super;
public:
	scrollable_menu_interior_t interior;

public:
	scrollable_menu_t(): super(&interior) {
	}

	virtual void render(myvi::surface_t &dst) OVERRIDE {
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0x8a8a8a;
		s32 ax,ay;
		translate(ax,ay);
		dst.fill(ax,ay,w,h);
	}

};



class test_screen_t : public myvi::gobject_t, public myvi::focus_aware_t {

public:
	tedit_t hdr_box;
	scrollable_menu_t scrollable;
	suffixes_t suffixes;


public:

	virtual void init() OVERRIDE {

		w = TFT_WIDTH;
		h = TFT_HEIGHT;

		extern resources_t res;

		myvi::button_context_t bctx1;
		bctx1.bk_sel_color = 0x292929; // gray
		bctx1.bk_color = 0x203E95; // blue

		myvi::label_context_t lctx1;
		lctx1.sctx.pen_color = 0x010101;
		lctx1.font = &res.ttf;
		lctx1.font_size = myvi::font_size_t::FS_8;

		myvi::label_context_t lctxg;
		lctxg.sctx.pen_color = 0x010101;
		lctxg.font = &res.gly;
		lctxg.font_size = myvi::font_size_t::FS_30;

		myvi::menu_context_t::instance().bctx1 = bctx1;
		myvi::menu_context_t::instance().lctx1 = lctx1;
		myvi::menu_context_t::instance().lctxg = lctxg;

		//gen::view_meta_t *root_view_meta = gen::meta_registry_t::instance().find_view_meta("root");
		//gobject_t *menu = view_factory_t::build_view(root_view_meta);

		//add_child(menu);

		//menu->x = 0;
		//menu->y = 0;
		//menu->w = w;
		//menu->h = h;


		hdr_box.x = 0;
		hdr_box.y = 0;
		hdr_box.w = w;
		hdr_box.h = 20;

		hdr_box.lval.value = "Helo!";
		hdr_box.lsfx.values = &suffixes;
		hdr_box.lsfx.value = *suffixes.next(0);

		add_child(&hdr_box);

		scrollable.x = 0;
		scrollable.y = 20;
		scrollable.w = w;
		scrollable.h = h/4;

		add_child(&scrollable);


		init_children();

		do_layout();
		dirty = true;

	}

	virtual void render(myvi::surface_t &dst) OVERRIDE {
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0xf9f9f9;//0x203E95;
		s32 ax,ay;
		translate(ax,ay);
		dst.fill(ax,ay,w,h);

	}


};


#endif
