#ifndef _CUSTOM_VIEWS_H
#define _CUSTOM_VIEWS_H

#include "menu_common.h"
#include "generator_common.h"
#include <sstream>

namespace custom {


namespace variant_type_t {
	typedef enum {
		STRING,
		INT,
		FLOAT
	} variant_type_t;
}

template <typename T>
class variant_tt {
public:
	variant_type_t::variant_type_t type;
	T sval;
	s32 ival;
	double fval;
public:
	variant_tt() {
		type = variant_type_t::STRING;
		ival = 0;
		fval = 0;
	}

	variant_tt(myvi::string_t _sval) {
		set_value(_sval);
	}

	variant_tt(s32 _ival) {
		set_value(_ival);
	}

	variant_tt(double _fval) {
		set_value(_fval);
	}

	void set_value(myvi::string_t _sval) {
		sval = _sval;
		type = variant_type_t::STRING;
	}

	void set_value(s32 _ival) {
		ival = _ival;
		type = variant_type_t::INT;
	}

	void set_value(double _fval) {
		fval = _fval;
		type = variant_type_t::FLOAT;
	}

	myvi::string_t get_string_value() {
		_MY_ASSERT(type == variant_type_t::STRING, return 0);
		return sval;
	}

	s32 get_int_value() {
		_MY_ASSERT(type == variant_type_t::INT, return 0);
		return ival;
	}

	double get_float_value() {
		_MY_ASSERT(type == variant_type_t::FLOAT, return 0);
		return fval;
	}

};

// служит только для передачи, но не для хранения значения !
typedef variant_tt<myvi::string_t> variant_t;

#define _MAX_STRING_LEN 100

class variant_holder_t : public variant_tt<myvi::string_impl_t<_MAX_STRING_LEN>> {
public:
	variant_holder_t (variant_type_t::variant_type_t expected_type) {
		this->type = expected_type;
	}

	void assign(variant_t &other) {
		_MY_ASSERT(this->type == other.type, return);
		switch(other.type) {
		case variant_type_t::STRING  : this->set_value(other.get_string_value()); break;
		case variant_type_t::INT  : this->set_value(other.get_int_value()); break;
		case variant_type_t::FLOAT  : this->set_value(other.get_float_value()); break;
		}
	}

	variant_t get_value() {
		variant_t ret;

		switch(this->type) {
		case variant_type_t::STRING  : ret.set_value(this->get_string_value()); break;
		case variant_type_t::INT  : ret.set_value(this->get_int_value()); break;
		case variant_type_t::FLOAT  : ret.set_value(this->get_float_value()); break;
		}

		return ret;
	}
};





// резолвер пути до меты параметра
// используется в контроллерах для разрешения parameterPath
class parameter_path_resolver_t {
public:
	gen::meta_path_t & meta_path;
public:
	parameter_path_resolver_t(gen::meta_path_t &_meta_path)
		:meta_path(_meta_path) {
	}
	// resolve absolute path
	gen::parameter_meta_t * resolve() {

		_MY_ASSERT(!meta_path.is_relative(), return 0);

		gen::meta_path_t::iterator_t iter = meta_path.iterator();

		myvi::string_t root_id = iter.next();
		gen::parameter_meta_t *root_meta = gen::meta_registry_t::instance().find_parameter_meta(root_id);

		return resolve(iter, root_meta);
	}

	// resolve path relative to root_meta
	gen::parameter_meta_t * resolve(gen::parameter_meta_t *root_meta) {
		_MY_ASSERT(meta_path.is_relative(), return 0);
		gen::meta_path_t::iterator_t iter = meta_path.iterator();
		return resolve(iter, root_meta);
	}


private:
	gen::parameter_meta_t * resolve(gen::meta_path_t::iterator_t &iter,  gen::parameter_meta_t *root_meta) {

		for (;;) {
			myvi::string_t child_id = iter.next();
			if (child_id.is_empty()) break;
			root_meta = root_meta->find_child_meta(child_id);
		}
		return root_meta;
	}
};



class model_t {
public:
	// обновление модели
	virtual void update(myvi::string_t parameter_path, variant_t &value) = 0;
	// ожидает правильного типа значения до вызова !
	virtual void read(myvi::string_t parameter_path, variant_t &value) = 0;

};


class dynamic_model_t : public model_t  { // , public publisher_t<msg>

public:
	std::unordered_map<myvi::string_t, variant_holder_t *, gen::string_t_hash_t> children;
public:

	virtual void update(myvi::string_t parameter_path, variant_t &value) OVERRIDE {
		variant_holder_t * holder = get_or_make_holder(parameter_path, value.type);
		holder->assign(value);
	}

	virtual void read(myvi::string_t parameter_path, variant_t &value) OVERRIDE {
		variant_holder_t * holder = get_or_make_holder(parameter_path, value.type);
		value = holder->get_value();
	}

private:
	variant_holder_t * get_or_make_holder(myvi::string_t parameter_path, variant_type_t::variant_type_t expected_type) {
		variant_holder_t *ret = children[parameter_path];
		if (!ret) {
			ret = new variant_holder_t(expected_type);
			children[parameter_path] = ret;
		}
		return ret;
	}
};

/*
* ====================== ПРЕДОПРЕДЕЛЕННЫЕ ВИДЫ =======================
*/


class dynamic_view_t : public myvi::gobject_t, public gen::dynamic_view_mixin_t {
	typedef myvi::gobject_t super;
public:
	gen::drawer_t *drawer;
public:
	dynamic_view_t(gen::meta_t *meta) {
		drawer = 0;

		myvi::string_t drawer_id = meta->get_string_param("drawer");
		if (!drawer_id.is_empty()) {
			drawer = gen::view_factory_t::instance()->build_drawer(drawer_id, meta);
		}
	}

	virtual void add_child(myvi::gobject_t *child, myvi::string_t id) OVERRIDE {
		dynamic_view_mixin_t::add_child(child, id);
		super::add_child(child);
	}

	virtual void render(myvi::surface_t &dst) OVERRIDE {
		if (drawer) {
			drawer->render(this, dst);
		}
	}
	
	virtual void set_dirty(bool dirty) OVERRIDE {
		super::set_dirty(dirty);
		if (dirty && parent && !drawer) {
			parent->dirty = true;
		}
	}

};


class view_controller_impl_base_t : public gen::view_controller_t {
public:

	virtual void init(gen::view_build_context_t &ctx) OVERRIDE {


		myvi::string_t parameter_path = ctx.get_view_meta()->get_string_param("parameterPath");

		if (!parameter_path.is_empty()) {
			gen::meta_path_t path = gen::meta_path_t(parameter_path);
			parameter_path_resolver_t resolver(path);

			gen::parameter_meta_t *child_parameter_meta = resolver.resolve(ctx.get_parameter_meta());
			ctx.set_parameter_meta(child_parameter_meta);
		}

		_MY_ASSERT(ctx.get_parameter_meta(), return);

		_LOG1(ctx.get_parameter_path().path.c_str());

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


class label_controller_t : public view_controller_impl_base_t {
	typedef view_controller_impl_base_t super;
public:
	myvi::label_t *lab;
public:

	label_controller_t() {
		lab = 0;
	}

	virtual void init(gen::view_build_context_t &ctx) OVERRIDE {
		super::init(ctx);

		lab = dynamic_cast<myvi::label_t *>(ctx.get_view());
		_MY_ASSERT(lab, return);

		lab->text = ctx.get_parameter_meta()->get_name();
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


/*
Контроллер редактируемого строкового поля

0. Регистрирует поле в модели , если его ещё там нет
1. Заполняет поле начальным значением из модели
2. Подписывается на изменения в модели и обновления поля
3. Подписывается на изменения поля и обновления модели

4. использует конвертор для преобразования значения параметра из строки и в строку

*/
class tbox_controller_t : public view_controller_impl_base_t {
	typedef view_controller_impl_base_t super;
public:
	myvi::text_box_t *tb;
public:

	tbox_controller_t() {
		tb = 0;
	}

	virtual void init(gen::view_build_context_t &ctx) OVERRIDE {
		super::init(ctx);


		tb = dynamic_cast<myvi::text_box_t *>(ctx.get_view());
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


/*
Контроллер комбобокса

1. Заполняет поле начальным значением из модели
2. Подписывается на изменения в модели и обновления поля
3. Подписывается на изменения поля и обновления модели

4. использует конвертор для преобразования значения параметра из combobox_item_t в число и обратно

*/

class cbox_controller_t : public view_controller_impl_base_t {
	typedef view_controller_impl_base_t super;
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

	virtual void init(gen::view_build_context_t &ctx) OVERRIDE  {
		super::init(ctx);

		cb = dynamic_cast<myvi::combo_box_t *>(ctx.get_view());
		_MY_ASSERT(cb, return);

		gen::type_meta_t * type_meta = ctx.get_parameter_meta()->get_type_meta();
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
	virtual void init(gen::view_build_context_t &ctx) OVERRIDE {

		myvi::string_t menu_id = ctx.get_view_meta()->get_string_param("menuRef");
		_MY_ASSERT(!menu_id.is_empty(), return);

		myvi::string_t item_template_id = ctx.get_view_meta()->get_string_param("itemTemplateView");
		_MY_ASSERT(!item_template_id.is_empty(), return);

		gen::view_meta_t *template_meta = gen::meta_registry_t::instance().find_view_meta(item_template_id);

		gen::menu_meta_t *menu_meta = gen::meta_registry_t::instance().find_menu_meta(menu_id);

		// здесь view - конетейнер меню
		// вставляем элементы меню в данный нам вид
		for (s32 i=0; ;i++) {
			myvi::string_t child_id = menu_meta->get_parameter_child(i);
			if (child_id.is_empty()) break;

			gen::parameter_meta_t *child_meta = gen::meta_registry_t::instance().find_parameter_meta(child_id);
			myvi::gobject_t *child_view = child_meta->build_menu_view(ctx);

			// создаем обёртку для вида параметра на основе шаблона
			gen::view_build_context_t child_ctx = ctx;
			child_ctx.set_view_meta(template_meta);
			child_ctx.set_parameter_meta(child_meta);

			myvi::gobject_t *child_wrapper = template_meta->build_view(child_ctx);
			child_wrapper->add_child(child_view);
			ctx.get_view()->add_child(child_wrapper);
		}

//		gen::view_factory_t::instance()->append_menu_view(view, menu_meta);
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
