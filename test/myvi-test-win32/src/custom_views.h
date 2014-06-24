#ifndef _CUSTOM_VIEWS_H
#define _CUSTOM_VIEWS_H

#include "menu_common.h"
#include "generator_common.h"
#include <sstream>

namespace custom {

/*
* ====================== ПРЕДОПРЕДЕЛЕННЫЕ ВИДЫ =======================
*/


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
	myvi::text_box_t *tb;
public:

	tbox_controller_t() {
		tb = 0;
	}

	virtual void init(myvi::gobject_t *view, gen::view_meta_t *view_meta, gen::parameter_meta_t *parameter_meta) OVERRIDE {

		myvi::string_t child_param_id = view_meta->get_string_param("childParameter");

		if (!child_param_id.is_empty()) {
			gen::parameter_meta_t *child_parameter_meta = parameter_meta->find_child_meta(child_param_id);
			parameter_meta = child_parameter_meta;
		}
		_MY_ASSERT(parameter_meta, return);

		tb = dynamic_cast<myvi::text_box_t *>(view);
		_MY_ASSERT(tb, return);

		tb->value = "123";
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
	myvi::combo_box_t *cb;
public:
	cbox_controller_t() {
		cb = 0;
	}

	virtual void init(myvi::gobject_t *view, gen::view_meta_t *view_meta, gen::parameter_meta_t *parameter_meta) OVERRIDE  {

		myvi::string_t child_param_id = view_meta->get_string_param("childParameter");

		if (!child_param_id.is_empty()) {
			gen::parameter_meta_t *child_parameter_meta = parameter_meta->find_child_meta(child_param_id);
			parameter_meta = child_parameter_meta;
		}

		_MY_ASSERT(parameter_meta, return);

		cb = dynamic_cast<myvi::combo_box_t *>(view);
		_MY_ASSERT(cb, return);

		gen::type_meta_t * type_meta = parameter_meta->get_type_meta();
		_MY_ASSERT(type_meta->is_enum(), return);


		myvi::iterator_t<myvi::combobox_item_t> *iter = type_meta->get_combobox_iterator();
		cb->values = iter;
		cb->value = iter->next(0);

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



/*
* ====================== ЛАЙОУТЫ И ПРОЧ. =======================
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
		this->preferred_item_size = meta->get_string_param("preferred_item_size") == "true";
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
