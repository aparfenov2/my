#ifndef _CUSTOM_VIEWS_H
#define _CUSTOM_VIEWS_H

#include "menu_common.h"
#include "generator_common.h"
#include <sstream>

namespace custom {


// Строка из текстбокса и комбобокса с суффиксами

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

// строка из названия и tedit-a справа
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
		gobject_t::init();

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





// ----------------- пример наследования окна с прокруткой --------------------------


// наследник класса внутреннего окна
class scrollable_menu_interior_t : public myvi::scrollable_interior_t {
	typedef scrollable_interior_t super;
public:
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

		gen::type_meta_t *dme_sfx_meta = gen::meta_registry_t::instance().find_type_meta("dme_sfx_t");
		myvi::iterator_t<myvi::combobox_item_t> *suffixes = dme_sfx_meta->get_combobox_iterator();

		row->valbox.lsfx.values = suffixes;
		row->valbox.lsfx.value = suffixes->next(0);
		std::string *s = new std::string(str.str());
		row->lname.text = (*s).c_str();
	}

	virtual void init() OVERRIDE {
		super::init();

		myvi::gobject_t::iterator_visible_t iter = this->iterator_visible();
		myvi::gobject_t *p = iter.next();
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




class tbox_view_t : public myvi::text_box_t {
	typedef myvi::text_box_t super;
public:
	virtual void init() OVERRIDE {
		super::init();

		myvi::menu_context_t &ctx = myvi::menu_context_t::instance();

		this->visible = true;
		this->lab.ctx = ctx.lctx1;
		this->lab.ctx.font_size = myvi::font_size_t::FS_20;
	}
};

class tbox_controller_t : public gen::view_controller_t {
public:
	myvi::label_t *lab;
public:

	virtual void init(myvi::gobject_t *view, gen::view_meta_t *view_meta, gen::parameter_meta_t *parameter_meta) OVERRIDE {
	}

};

class cbox_view_t : public myvi::combo_box_t {
	typedef myvi::combo_box_t super;
public:
	virtual void init() OVERRIDE {
		super::init();

		myvi::menu_context_t &ctx = myvi::menu_context_t::instance();

		this->visible = true;
		this->lab.ctx = ctx.lctx1;
		this->lab.ctx.font_size = myvi::font_size_t::FS_20;
	}
};

// контроллер комбобокса
class cbox_controller_t : public gen::view_controller_t {
public:
	//units
	//validators
	//textbox*
	//combobox*
public:
	virtual void init(myvi::gobject_t *view, gen::view_meta_t *view_meta, gen::parameter_meta_t *meta) OVERRIDE  {
	}
};


// вид с полем ввода и комбобоксом
class tbox_cbox_view_t : public myvi::gobject_t {
public:
	myvi::text_box_t lval;
	myvi::combo_box_t lsfx;
	myvi::stack_layout_t stack_layout;
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
		myvi::gobject_t::init();

		myvi::menu_context_t &ctx = myvi::menu_context_t::instance();

		lval.visible = true;
		lval.lab.ctx = ctx.lctx1;
		lval.lab.ctx.font_size = myvi::font_size_t::FS_20;

		lsfx.visible = true;
		lsfx.lab.ctx = ctx.lctx1;
		lsfx.lab.ctx.font_size = myvi::font_size_t::FS_20;
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

// контроллер вида с полем ввода и комбобоксом
class tbox_cbox_controller_t : public gen::view_controller_t {
public:
	//units
	//validators
	//textbox*
	//combobox*
public:
	virtual void init( myvi::gobject_t *view, gen::view_meta_t *view_meta, gen::parameter_meta_t *meta) OVERRIDE  {
	}
};


class lab_view_t : public myvi::label_t {
public:

	virtual void init() OVERRIDE {
		myvi::gobject_t::init();


		myvi::menu_context_t &ctx = myvi::menu_context_t::instance();

		visible = true;
		this->ctx = ctx.lctx1;
		this->ctx.font_size = myvi::font_size_t::FS_20;

	}

};

class tbox_label_view_t : public myvi::gobject_t {
public:
	myvi::text_box_t lval;
	myvi::label_t lsfx;
	myvi::stack_layout_t stack_layout;
//	input_controller_t controller;
public:
	tbox_label_view_t() {

		stack_layout.vertical = false;
		stack_layout.preferred_item_size = true;
		layout = &stack_layout;

		add_child(&lval);
		add_child(&lsfx);
	}

	virtual void init() OVERRIDE {
		myvi::gobject_t::init();

		myvi::menu_context_t &ctx = myvi::menu_context_t::instance();

		lval.visible = true;
		lval.lab.ctx = ctx.lctx1;
		lval.lab.ctx.font_size = myvi::font_size_t::FS_20;

		lsfx.visible = true;
		lsfx.ctx = ctx.lctx1;
		lsfx.ctx.font_size = myvi::font_size_t::FS_20;
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


class tbox_label_controller_t : public gen::view_controller_t {
public:
	//units
	//validators
	//textbox*
	//combobox*
public:
	virtual void init( myvi::gobject_t *view, gen::view_meta_t *view_meta, gen::parameter_meta_t *meta) OVERRIDE  {
	}
};


// инициализирует поле названия элемента меню
class label_controller_t {
public:
	myvi::label_t *lab;
public:
	void init(myvi::label_t *alab, gen::parameter_meta_t *parameter_meta) {
		lab = alab;
	}
};



// ----------------- DME - наследник вида с текстбоксом и комбобоксом ----------------------

/*
class dme_view_t : public myvi::gobject_t {
public:
	myvi::label_t menu_label;
	custom::tbox_cbox_view_t tbox_cbox;
	myvi::stack_layout_t stack_layout;
public:
	dme_view_t() {
		stack_layout.vertical = false;
		stack_layout.preferred_item_size = true;
		layout = &stack_layout;

		menu_label.text = "text";

		add_child(&menu_label);
		add_child(&tbox_cbox);
	}
};
*/

// пример контроллера составного параметра
// Контроллеры статически определены в коде до генерации
class dme_controller_t : public gen::view_controller_t {
public:
public:
	void init(myvi::gobject_t *view, gen::view_meta_t *view_meta, gen::parameter_meta_t *parameter_meta) OVERRIDE {

		gen::dynamic_view_mixin_t *dv = dynamic_cast<gen::dynamic_view_mixin_t *>(view);
		_MY_ASSERT(dv, return);
		myvi::gobject_t *ch_view = dv->get_child("ch");
//		_MY_ASSERT(ch_view, return);

	}
};


// контроллер вида меню, по menuRef получает мету меню, и достраивает вид, потом обрабытвает события от вида
class menu_controller_t : public gen::view_controller_t {
public:
public:
	virtual void init(myvi::gobject_t *view, gen::view_meta_t *view_meta, gen::parameter_meta_t *parameter_meta) OVERRIDE {

		myvi::string_t menu_id = view_meta->get_string_param("menuRef");
		_MY_ASSERT(!menu_id.is_empty(), return);

		myvi::string_t item_template_id = view_meta->get_string_param("itemTemplateView");
		_MY_ASSERT(!item_template_id.is_empty(), return);

		gen::view_meta_t *template_meta = gen::meta_registry_t::instance().find_view_meta(item_template_id);

		gen::menu_meta_t *menu_meta = gen::meta_registry_t::instance().find_menu_meta(menu_id);

		// здесь view - конетейнер меню
		// вставляем элементы меню в данный нам вид
		for (s32 i=0; ;i++) {
			myvi::string_t child_id = menu_meta->get_parameter_child(i);
			if (child_id.is_empty()) break;

			gen::parameter_meta_t *child_meta = gen::meta_registry_t::instance().find_parameter_meta(child_id);
			myvi::gobject_t *child_view = child_meta->build_menu_view();

			// создаем обёртку для вида параметра на основе шаблона
			myvi::gobject_t *child_wrapper = template_meta->build_view(child_meta);
			child_wrapper->add_child(child_view);
			view->add_child(child_wrapper);
		}

//		gen::view_factory_t::instance()->append_menu_view(view, menu_meta);
	}
};

// контроллер элемента меню
// заполняет label шаблона названием элемента
class menu_item_controller_t : public gen::view_controller_t {
public:
public:
	virtual void init(myvi::gobject_t *view, gen::view_meta_t *view_meta, gen::parameter_meta_t *parameter_meta) OVERRIDE {
		// найдем метку lab в шаблоне вида элемнта меню
		gen::dynamic_view_t *dynamic_view = dynamic_cast<gen::dynamic_view_t *>(view);
		_MY_ASSERT(dynamic_view, return);

		myvi::gobject_t *lab_obj = dynamic_view->get_child("lab");
		_MY_ASSERT(lab_obj, return);
		myvi::label_t *lab = dynamic_cast<myvi::label_t *>(lab_obj);
		_MY_ASSERT(lab, return);
		lab->text = parameter_meta->get_name();
	}
};




// менеджер размещения с заранее заданными размерами
// ожидает от инстанса вида поддержки meta_provider_t
/*
class absolute_layout_t : public myvi::layout_t {
private:
	s32 check_w(myvi::gobject_t *parent, myvi::gobject_t *child, s32 w) {
		_MY_ASSERT(w != _NAN, return _NAN);

		if (child->x + w > parent->w) w = parent->w - child->x;
		if (w <= 0) {
			w = _NAN;
		}
		return w;
	}
	s32 check_h(myvi::gobject_t *parent, myvi::gobject_t *child, s32 h) {
		_MY_ASSERT(h != _NAN, return _NAN);

		if (child->y + h > parent->h) h = parent->h - child->y;
		if (h <= 0) {
			h = _NAN;
		}
		return h;
	}

public:

	absolute_layout_t(gen::meta_t *meta) {
	}

	virtual void get_preferred_size(myvi::gobject_t *parent, s32 &pw, s32 &ph) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		myvi::gobject_t * child = iter.next();

		pw = 0;
		ph = 0;

		while (child) {
			gen::meta_provider_t *meta_provider = dynamic_cast<gen::meta_provider_t *>(child);
			if (meta_provider) {
				gen::meta_t *meta = meta_provider->get_meta();
				s32 x = meta->get_int_param("x");
				_WEAK_ASSERT(x != _NAN, continue);
				s32 y = meta->get_int_param("y");
				_WEAK_ASSERT(y != _NAN, continue);
				s32 w = meta->get_int_param("w");
				_WEAK_ASSERT(w != _NAN, continue);
				s32 h = meta->get_int_param("h");
				_WEAK_ASSERT(h != _NAN, continue);

				if (pw < x+w) pw = x+w;
				if (ph < y+h) ph = y+h;
			}
			child = iter.next();
		}
		
	}

	virtual void layout(myvi::gobject_t *parent) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		myvi::gobject_t * child = iter.next();

		while (child) {
			gen::meta_provider_t *meta_provider = dynamic_cast<gen::meta_provider_t *>(child);
			if (meta_provider) {
				gen::meta_t *meta = meta_provider->get_meta();

				s32 x = meta->get_int_param("x");
				_WEAK_ASSERT(x != _NAN, continue);
				s32 y = meta->get_int_param("y");
				_WEAK_ASSERT(y != _NAN, continue);
				s32 w = meta->get_int_param("w");
				_WEAK_ASSERT(w != _NAN, continue);
				s32 h = meta->get_int_param("h");
				_WEAK_ASSERT(h != _NAN, continue);

				child->x = x;
				child->y = y;
				child->w = check_w(parent, child, w);
				child->h = check_h(parent, child, h);

			}
			child = iter.next();
		}
	}

};
*/

class stretch_meta_layout_t : public myvi::stretch_layout_t {
public:
	stretch_meta_layout_t(gen::meta_t *meta) {
	}
};


class stack_meta_layout_t : public myvi::stack_layout_t {
public:
	stack_meta_layout_t(gen::meta_t *meta) {
		this->vertical = meta->get_string_param("vertical") == "true";
	}
};

class menu_meta_layout_t : public myvi::layout_t {
public:
	menu_meta_layout_t(gen::meta_t *meta) {
	}

	virtual void get_preferred_size(myvi::gobject_t *parent, s32 &pw, s32 &ph) OVERRIDE {
		_MY_ASSERT(0, return);
	}

	virtual void layout(myvi::gobject_t *parent) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		// 1st child
		myvi::gobject_t *first  = iter.next();
		_MY_ASSERT(first, return);
		first->w = (s32)(parent->w * 0.6);
		first->h = parent->h;
		// 2st child
		myvi::gobject_t *second  = iter.next();
		_MY_ASSERT(second, return);

		second->h = parent->h;
		second->x = first->w + 5;
		second->w = parent->w - second->x;

		// none other children
		_MY_ASSERT(!iter.next(), return);

	}

};


// вид с фоном
class background_drawer_t : public gen::drawer_t  {
public:
	myvi::surface_context_t ctx;
	bool hasBorder;
public:
	background_drawer_t(gen::meta_t *meta) {

		hasBorder = false;
		ctx.alfa = 0;

		myvi::string_t color = meta->get_string_param("background");

		this->ctx.pen_color = gen::view_factory_t::instance()->parse_color(color);
		if (this->ctx.pen_color > 0) {
			this->ctx.alfa = 0xff;
		}
	}

	virtual void render(myvi::gobject_t *obj, myvi::surface_t &dst) OVERRIDE {
		s32 ax, ay;
		obj->translate(ax,ay);
		dst.ctx = ctx;

		if (ctx.alfa) {
			dst.fill(ax,ay,obj->w,obj->h);
		}
		if (hasBorder) {
			dst.rect(ax,ay,obj->w,obj->h);
		}
	}
};


}  // ns
#endif
