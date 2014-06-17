#ifndef _CUSTOM_VIEWS_H
#define _CUSTOM_VIEWS_H

#include "menu_common.h"
#include "generator_common.h"
#include <sstream>

namespace myvi {


// —трока из текстбокса и комбобокса с суффиксами

class tedit_t : public inp_dlg_row_t {
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
class menu_row_t :  public gobject_t {
public:
	label_t lname;
	tedit_t valbox;
	stack_layout_t stack_layout;
public:

	menu_row_t() {
		stack_layout.vertical = false;
		layout = &stack_layout;
		add_child(&lname);
		add_child(&valbox);
	}

	virtual void init() {
		gobject_t::init();

		menu_context_t &ctx = menu_context_t::instance();

		lname.visible = true;
		lname.ctx = ctx.lctx1;
		lname.ctx.font_size = font_size_t::FS_20;

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


// контроллер вида с полем ввода и комбобоксом
class tbox_cbox_controller_t  {
public:
	//units
	//validators
	//textbox*
	//combobox*
public:
	void init( gobject_t *view,  gen::parameter_meta_t *meta)  {
	}
};


// контроллер комбобокса
class cbox_controller_t  {
public:
	//units
	//validators
	//textbox*
	//combobox*
public:
	void init(gobject_t *view,  gen::parameter_meta_t *meta)  {
	}
};


// вид с полем ввода и комбобоксом
class tbox_cbox_view_t : public gobject_t {
public:
	text_box_t lval;
	combo_box_t lsfx;
	stack_layout_t stack_layout;
//	input_controller_t controller;
public:
	tbox_cbox_view_t() {

		stack_layout.vertical = false;
		stack_layout.preferred_item_size = true;
		layout = &stack_layout;

		add_child(&lval);
		add_child(&lsfx);
	}

	virtual void init() {
		gobject_t::init();

		menu_context_t &ctx = menu_context_t::instance();

		lval.visible = true;
		lval.lab.ctx = ctx.lctx1;
		lval.lab.ctx.font_size = font_size_t::FS_20;

		lsfx.visible = true;
		lsfx.lab.ctx = ctx.lctx1;
		lsfx.lab.ctx.font_size = font_size_t::FS_20;
	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		_MY_ASSERT(parent,return);
		parent->dirty = dirty;
	}


	virtual void child_request_size_change(gobject_t *child, s32 aw, s32 ah) OVERRIDE {

		get_preferred_size(aw,ah);

		if (parent) {
			parent->child_request_size_change(this, aw, ah);

		} else {
			do_layout();
			dirty = true;
		}
	}

};


// ----------------- пример наследовани€ окна с прокруткой --------------------------


// наследник класса внутреннего окна
class scrollable_menu_interior_t : public scrollable_interior_t {
	typedef scrollable_interior_t super;
public:
	stack_layout_t stack_layout;
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

		gen::type_meta_t *dme_sfx_meta = gen::meta_registry_t::instance().find_type_meta("dme_sfx_t");
		myvi::iterator_t<myvi::combobox_item_t> *suffixes = dme_sfx_meta->get_combobox_iterator();

		row->valbox.lsfx.values = suffixes;
		row->valbox.lsfx.value = suffixes->next(0);
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
class scrollable_menu_t : public scrollable_window_t {
	typedef scrollable_window_t super;
public:
	scrollable_menu_interior_t interior;

public:
	scrollable_menu_t(): super(&interior) {
	}

	virtual void render(surface_t &dst) OVERRIDE {
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0x8a8a8a;
		s32 ax,ay;
		translate(ax,ay);
		dst.fill(ax,ay,w,h);
	}

};

/*

class tedit_bkg_t : public tedit_t {
	typedef tedit_t super;
public:
public:

	virtual void render(surface_t &dst) OVERRIDE {
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0xaaaaaa;
		s32 ax,ay;
		translate(ax,ay);
		dst.fill(ax,ay,w,h);
	}

};
*/


// ----------------- DME - наследник вида с текстбоксом и комбобоксом ----------------------


class dme_view_t : public gobject_t {
public:
	label_t menu_label;
	tbox_cbox_view_t tbox_cbox;
public:
	dme_view_t() {
		add_child(&menu_label);
		add_child(&tbox_cbox);
	}
};



// -----------------------------------------
// ----------- контроллеры -----------------
// -----------------------------------------


// инициализирует поле названи€ элемента меню
class label_controller_t {
public:
	label_t *lab;
public:
	void init(label_t *alab, gen::parameter_meta_t *parameter_meta) {
		lab = alab;
	}
};


// пример контроллера составного параметра
//  онтроллеры статически определены в коде до генерации
class dme_controller_t {
public:
	label_controller_t lab_ctl;
	tbox_cbox_controller_t tcb_ctl;
public:
	void init(dme_view_t *dme_view, gen::parameter_meta_t *parameter_meta) {
	}
};

// контроллер вида меню, по menuRef получает мету меню, и достраивает вид, потом обрабытвает событи€ от вида
class menu_controller_t : public view_controller_t {
public:
public:
	virtual void init(gobject_t *view, gen::view_meta_t *view_meta) OVERRIDE {

		string_t menu_id = view_meta->get_string_param("menuRef");
		gen::menu_meta_t *menu_meta = gen::meta_registry_t::instance().find_menu_meta(menu_id);
		// вставл€ем элементы меню в данный нам вид
		view_factory_t::instance()->append_menu_view(view, menu_meta);
	}
};



// вид с фоном
class background_view_t : public gobject_t {
public:
	surface_context_t ctx;
	bool hasBorder;
public:
	background_view_t() {
		hasBorder = false;
	}

	virtual void render(surface_t &dst) OVERRIDE {
		s32 ax, ay;
		translate(ax,ay);
		dst.ctx = ctx;

		if (ctx.alfa) {
			dst.fill(ax,ay,w,h);
		}
		if (hasBorder) {
			dst.rect(ax,ay,w,h);
		}
	}
};


}  // ns
#endif
