#ifndef _VIEW_FACTORY_CPP 
#error should only be included from view_factory.cpp
#endif

#ifndef _CUSTOM_VIEWS_H
#define _CUSTOM_VIEWS_H

#include "menu_common.h"
#include "generator_common.h"
#include "custom_common.h"
#include <cstdio>

namespace custom {




class parameter_meta_ex_t {
public:
	gen::parameter_meta_t *parameter_meta;
public:
	parameter_meta_ex_t(gen::parameter_meta_t *_parameter_meta) {
		parameter_meta = _parameter_meta;
	}
	myvi::gobject_t * build_menu_view(view_build_context_t ctx) {
		ctx.set_parameter_meta(parameter_meta);
		return view_factory_t::instance().build_menu_view(ctx);
	}
	variant_type_t::variant_type_t match_value_type() {
		gen::type_meta_t *type_meta = parameter_meta->get_type_meta();
		_MY_ASSERT(!type_meta->is_complex(), return variant_type_t::STRING);

		if (type_meta->is_basic()) {
			if (type_meta->get_id() == "u8") {
				return variant_type_t::INT;
			} else if (type_meta->get_id() == "s8") {
				return variant_type_t::INT;
			} else if (type_meta->get_id() == "u16") {
				return variant_type_t::INT;
			} else if (type_meta->get_id() == "s16") {
				return variant_type_t::INT;
			} else if (type_meta->get_id() == "u32") {
				return variant_type_t::INT;
			} else if (type_meta->get_id() == "s32") {
				return variant_type_t::INT;
			} else if (type_meta->get_id() == "float") {
				return variant_type_t::FLOAT;
			} else if (type_meta->get_id() == "string") {
				return variant_type_t::STRING;
			}
		} else if (type_meta->is_enum()) {
			return variant_type_t::INT;
		}
		// неизвестный простой тип
		_MY_ASSERT(0, return variant_type_t::STRING);
		return variant_type_t::STRING;
	}
};






class variant_holder_t : public variant_tt<volatile_string_impl_t> {
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
	meta_path_t & meta_path;
public:
	parameter_path_resolver_t(meta_path_t &_meta_path)
		:meta_path(_meta_path) {
	}
	// resolve absolute path
	gen::parameter_meta_t * resolve() {

		_MY_ASSERT(!meta_path.is_relative(), return 0);

		meta_path_t::iterator_t iter = meta_path.iterator();

		myvi::string_t root_id = iter.next();
		gen::parameter_meta_t *root_meta = gen::meta_registry_t::instance().find_parameter_meta(root_id);

		return resolve(iter, root_meta);
	}

	// resolve path relative to root_meta
	gen::parameter_meta_t * resolve(gen::parameter_meta_t *root_meta) {
		_MY_ASSERT(meta_path.is_relative(), return 0);
		meta_path_t::iterator_t iter = meta_path.iterator();
		return resolve(iter, root_meta);
	}


private:
	gen::parameter_meta_t * resolve(meta_path_t::iterator_t &iter,  gen::parameter_meta_t *root_meta) {

		for (;;) {
			myvi::string_t child_id = iter.next();
			if (child_id.is_empty()) break;
			root_meta = root_meta->find_child_meta(child_id);
		}
		return root_meta;
	}
};




class dynamic_model_t : public model_t { 

public:
	std::unordered_map<myvi::string_t, variant_holder_t *, gen::string_t_hash_t> children;
public:

	virtual void update(myvi::string_t parameter_path, variant_t &value) OVERRIDE {
//		variant_holder_t * holder = get_or_make_holder(parameter_path, value.type);
		variant_holder_t *holder = children[parameter_path];
		_MY_ASSERT(holder, return);
		holder->assign(value);
		notify(model_message_t(parameter_path, value));
	}


	virtual void read(myvi::string_t parameter_path, variant_t &value) OVERRIDE {
		variant_holder_t *holder = children[parameter_path];
		_MY_ASSERT(holder, return);
		value = holder->get_value();
	}

	virtual void try_register_path(myvi::string_t parameter_path, variant_t &initial_value, variant_type_t::variant_type_t expected_type) OVERRIDE  {

		variant_holder_t *holder = children[parameter_path];
		if (holder) {
			_MY_ASSERT(holder->type == expected_type, return);
			return;
		}

		_MY_ASSERT(initial_value.type == expected_type, return);

		holder = get_or_make_holder(parameter_path, expected_type);
		holder->assign(initial_value);
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



// интерфейс конвертера значения в строку и обратно
class converter_t {
public:
	virtual void to_string(variant_t &value, volatile_string_impl_t &str) = 0;
	virtual bool from_string(myvi::string_t &str, variant_t &value) = 0;
};


class string_converter_t : public converter_t {
public:
	virtual void to_string(variant_t &value, volatile_string_impl_t &str) OVERRIDE {
		str = value.get_string_value();
	}

	virtual bool from_string(myvi::string_t &str, variant_t &value) OVERRIDE {
		value.set_value(str);
		return true;
	}
};

class int_converter_t : public converter_t {
public:
	virtual void to_string(variant_t &value, volatile_string_impl_t &str) OVERRIDE {		
		std::sprintf((char *)str.c_str(), "%d", value.get_int_value());
		str.update_length();
	}

	virtual bool from_string(myvi::string_t &str, variant_t &value) OVERRIDE {
		char *end;
		s32 ival = std::strtol(str.c_str(), &end, 10);
		if (!*end) {
			value.set_value(ival);
		}
		return !*end;
	}
};

class float_converter_t : public converter_t {
public:
	virtual void to_string(variant_t &value, volatile_string_impl_t &str) OVERRIDE {

		char *buf = (char *)str.c_str();
		s32 len = std::sprintf(buf, "%.6f", value.get_float_value());

		while (len > 1 && (buf[len-1] == '0' || buf[len-1] == '.')) {
			--len;
		}
		str.set_length(len);
	}

	virtual bool from_string(myvi::string_t &str, variant_t &value) OVERRIDE {
		char *end;
		double fval = std::strtod(str.c_str(), &end);
		if (!*end) {
			value.set_value(fval);
		}
		return !*end;
	}
};


class converter_factory_t {
public:
	static converter_factory_t _instance;

	string_converter_t string_converter;
	int_converter_t int_converter;
	float_converter_t float_converter;

public:
	static converter_factory_t instance() {
		return _instance;
	}

	converter_t * for_type(variant_type_t::variant_type_t expected_type) {
		switch (expected_type) {
		case variant_type_t::STRING : return &string_converter;
		case variant_type_t::INT : return &int_converter;
		case variant_type_t::FLOAT : return &float_converter;
		}
		_MY_ASSERT(0, return 0); // not found
		return 0;
	}
};

/*
* ====================== ПРЕДОПРЕДЕЛЕННЫЕ ВИДЫ =======================
*/






// интерфейс контроллера вида
class view_controller_t {
public:
public:
	// связывание с видом. Использует RTTI для определения класса вида, см. view_meta_t
	virtual void init(view_build_context_t &ctx) {
	}

	// dynamic_view вызывает этот метод на этапе инициализации вида, когда g-дерево полностью построено
	virtual void on_view_init() {
	}

};






class focus_master_mixin_t : public myvi::focus_master_t {
public:
	myvi::gobject_t *view;
public:
	focus_master_mixin_t(myvi::gobject_t *_view) {
		view = _view;
	}
};


class tbox_cbox_focus_master_mixin_t : public focus_master_mixin_t {
public:
	s32 selected_count;
public:
	tbox_cbox_focus_master_mixin_t(myvi::gobject_t *_view, gen::meta_t * meta) : focus_master_mixin_t(_view) {
		selected_count = 0;
	}

	virtual void alter_focus_intention(myvi::focus_intention_t &intention) OVERRIDE {

		_MY_ASSERT(intention.current, return );
		myvi::gobject_t *current_g = (intention.current);
		_MY_ASSERT(current_g, return );
		myvi::gobject_t *next_g = (intention.next);
		_MY_ASSERT(next_g, return );

		if (next_g->is_child_of(view)) {
			selected_count++;

		} else if (current_g->is_child_of(view)) {
		// если intention.current наш, а intention.next не наш, т.е. выходим из области, то
		
		// если были_выбраны_оба, то не изменяем, и сбрасываем были_выбраны_оба
			if (selected_count >= 1) {
				selected_count = 0;
			} else {
		// иначе выбираем не выбранный
				myvi::gobject_t::iterator_visible_t iter = view->iterator_visible();
				myvi::gobject_t *p = iter.next();
				while(p) {
					if (p != current_g) {
						intention.next = p;
						selected_count++;
						break;
					}
					p = iter.next();
				}
			}
		}
	}
};


class decorator_t {
public:
	virtual void render(myvi::gobject_t *obj, myvi::surface_t &dst, bool beforeSuper) = 0;
};


class decorator_aware_t {
public:
	virtual void set_drawer(decorator_t *decorator) = 0;
};


template<typename TBase>
class decorator_aware_impl_t : public TBase,  public decorator_aware_t {
public:
	decorator_t *decorator;
public:
	decorator_aware_impl_t() {
		decorator = 0;
	}

	virtual void set_drawer(decorator_t *_drawer) OVERRIDE {
		decorator = _drawer;
	}

	virtual void render(myvi::surface_t &dst) OVERRIDE {
		if (decorator) {
			decorator->render(this, dst, true);
		}
		TBase::render(dst);
		if (decorator) {
			decorator->render(this, dst, false);
		}
	}
	
	virtual void set_dirty(bool dirty) OVERRIDE {
		TBase::set_dirty(dirty);
		if (dirty && parent && !decorator) {
			parent->dirty = true;
		}
	}

};


// вид с фоном
class background_decorator_t : public decorator_t  {
public:
	myvi::surface_context_t ctx;
	bool hasBorder;
public:
	background_decorator_t(gen::meta_t *meta) {

		hasBorder = false;
		ctx.alfa = 0;

		myvi::string_t color = meta->get_string_param("background");

		this->ctx.pen_color = view_factory_t::instance().parse_color(color);
		if (this->ctx.pen_color > 0) {
			this->ctx.alfa = 0xff;
		}
	}

	virtual void render(myvi::gobject_t *obj, myvi::surface_t &dst, bool beforeSuper) OVERRIDE {
		if (!beforeSuper) return;
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




typedef dynamic_view_mixin_aware_impl_t<
								decorator_aware_impl_t<myvi::gobject_t>
								> _dynamic_view_t_super_t;

class dynamic_view_t : public _dynamic_view_t_super_t, public myvi::focus_master_t {
	typedef _dynamic_view_t_super_t super;
private:
	gen::view_meta_t *view_meta;
	view_controller_t *view_controller;
	focus_master_mixin_t *focus_master;
	s32 fw, fh;
public:
	dynamic_view_t(view_build_context_t &ctx) {
		focus_master = 0;
		view_meta = ctx.get_view_meta();
		view_controller = 0; // устанавливается позже


		myvi::string_t focus_master_id = view_meta->get_string_param("focusMaster");
		if (!focus_master_id.is_empty()) {
			focus_master = build_focus_master(focus_master_id, view_meta);
		}

		fw = this->view_meta->get_int_param("width");
		fh = this->view_meta->get_int_param("height");

	}


	focus_master_mixin_t * build_focus_master(myvi::string_t focus_master_id, gen::meta_t * meta) {
		focus_master_mixin_t *ret = 0;
		if (focus_master_id == "tbox_cbox") {
			ret = new tbox_cbox_focus_master_mixin_t(this, meta);
		}
		_MY_ASSERT(ret, return 0);
		return ret;
	}


	void set_view_controller(view_controller_t *_view_controller) {
		_MY_ASSERT(_view_controller, return);
		this->view_controller = _view_controller;
	}

	view_controller_t *get_view_controller() {
		_MY_ASSERT(this->view_controller, return 0);
		return this->view_controller;
	}

	gen::view_meta_t * get_view_meta() {
		_MY_ASSERT(this->view_meta, return 0);
		return this->view_meta;
	}

	virtual void init() OVERRIDE {
		super::init();
		if (this->view_controller) {
			this->view_controller->on_view_init();
		}
	}

	virtual void child_request_size_change(myvi::gobject_t *child, s32 aw, s32 ah) OVERRIDE {
		do_layout();
	}

	virtual void alter_focus_intention(myvi::focus_intention_t &intention) OVERRIDE {
		if (focus_master) {
			focus_master->alter_focus_intention(intention);
		}
	}
	
	virtual void get_preferred_size(s32 &pw, s32 &ph) OVERRIDE {

		super::get_preferred_size(pw,ph);

		if (fw != _NAN) {
			pw = fw;
		}
		if (fh != _NAN) {
			ph = fh;
		}
	}

};


class view_controller_impl_base_t : public view_controller_t {
public:
	volatile_string_impl_t parameter_path;
	variant_type_t::variant_type_t type;
public:
	view_controller_impl_base_t() {
		type = variant_type_t::STRING;
	}

	void set_initial(view_build_context_t &ctx, variant_t &value) {

		myvi::string_t initial_str = ctx.get_parameter_meta()->get_string_param("initial");
		if (!initial_str.is_empty()) {
			_MY_ASSERT(type == variant_type_t::STRING, return);
			value.set_value(initial_str);
			model_t::instance()->update(ctx.get_parameter_path().path, value);
		}

		s32 initial_int = ctx.get_parameter_meta()->get_int_param("initial");
		if (initial_int != _NAN) {
			_MY_ASSERT(type == variant_type_t::INT, return);
			value.set_value(initial_int);
			model_t::instance()->update(ctx.get_parameter_path().path, value);
		}

		double initial_float = ctx.get_parameter_meta()->get_float_param("initial");
		if (initial_float != _NANF) {
			_MY_ASSERT(type == variant_type_t::FLOAT, return);
			value.set_value(initial_float);
			model_t::instance()->update(ctx.get_parameter_path().path, value);
		}
	}

	virtual void init(view_build_context_t &ctx) OVERRIDE {


		myvi::string_t parameter_path = ctx.get_view_meta()->get_string_param("parameterPath");

		if (!parameter_path.is_empty()) {
			meta_path_t path = meta_path_t(parameter_path);
			parameter_path_resolver_t resolver(path);

			gen::parameter_meta_t *child_parameter_meta = resolver.resolve(ctx.get_parameter_meta());
			ctx.set_parameter_meta(child_parameter_meta);
		}

//		_MY_ASSERT(ctx.get_parameter_meta(), return); - меты параметра может и не быть

//		_LOG1(ctx.get_parameter_path().path.c_str());

// 0. Регистрирует поле в модели , если его ещё там нет
	}

	// вызывается из контроллера texbox/combobox
	void register_in_model(view_build_context_t &ctx) {

		gen::type_meta_t *type_meta = ctx.get_parameter_meta()->get_type_meta();
		_MY_ASSERT(!type_meta->is_complex(), return);

		variant_t value;
		this->parameter_path = ctx.get_parameter_path().path;
		this->type = parameter_meta_ex_t(ctx.get_parameter_meta()).match_value_type();
		value.type = type;
		model_t::instance()->try_register_path(this->parameter_path, value, type);
		set_initial(ctx, value);

	}

};


class lab_view_t : public myvi::label_t {
public:
	gen::meta_t *meta;
public:

	lab_view_t(view_build_context_t &ctx) {

		meta = ctx.get_view_meta();
		
	}

	virtual void init() OVERRIDE {
		myvi::gobject_t::init();


		myvi::menu_context_t &ctx = myvi::menu_context_t::instance();

		visible = true;
		this->ctx = ctx.lctx1;

		view_factory_t::instance().prepare_context(this->ctx, meta);

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

	virtual void init(view_build_context_t &ctx) OVERRIDE {
		super::init(ctx);

		lab = dynamic_cast<myvi::label_t *>(ctx.get_view());
		_MY_ASSERT(lab, return);

		myvi::string_t static_text = ctx.get_view_meta()->get_string_param("staticText");
		if (!static_text.is_empty()) {
			lab->text = static_text;
			return;
		}

		myvi::string_t label_source = ctx.get_view_meta()->get_string_param("labelSource");
		if (label_source.is_empty()) {
			label_source = "name";
		}


		lab->text = ctx.get_parameter_meta()->get_string_param(label_source);
	}

};

class meta_ex_t {
public:
	gen::meta_t *meta;
public:
	meta_ex_t(gen::meta_t *_meta) {
		meta = _meta;
	}
	variant_t get_variant_param(myvi::string_t key) {

		if (!meta->get_string_param(key).is_empty()) {
			return meta->get_string_param(key);

		} else if (meta->get_int_param(key) != _NAN) {
			return meta->get_int_param(key);

		} else if (meta->get_float_param(key) != _NANF) {
			return meta->get_float_param(key);
		}
		return variant_t();
	}
};


class event_bus_msg_t {
public:
	myvi::string_t event_name;
	variant_t arg0;
public:
	event_bus_msg_t(myvi::string_t _event_name, variant_t _arg0) {
		event_name = _event_name;
		arg0 = _arg0;
	}
};

#define _EVENT_BUS_MAX_SUBSCRIBERS 32

class event_bus_t : public myvi::publisher_t<event_bus_msg_t, _EVENT_BUS_MAX_SUBSCRIBERS> {
private:
	event_bus_t() {
	}
public:
	static event_bus_t & instance() {
		static event_bus_t _instance;
		return _instance;
	}

};

class button_view_t : public decorator_aware_impl_t<myvi::button_t>, public myvi::focus_aware_t {
	typedef decorator_aware_impl_t<myvi::button_t> super;
public:
	gen::meta_t *meta;
public:
	button_view_t(gen::meta_t *_meta) {
		meta = _meta;
	}

	virtual void init() OVERRIDE {
		super::init();


		myvi::menu_context_t &ctx = myvi::menu_context_t::instance();
		this->l_mid.visible = true;
		this->l_mid.ctx = ctx.lctx1;
		this->l_mid.text = meta->get_string_param("name");

		view_factory_t::instance().prepare_context(this->l_mid.ctx, meta);

	}

	virtual void key_event(myvi::key_t::key_t key) OVERRIDE {
		if (key == myvi::key_t::K_ENTER) {
			myvi::string_t event_id = meta->get_string_param("event");
			if (!event_id.is_empty()) {
				variant_t arg0 = meta_ex_t(meta).get_variant_param("arg0");
				if (!arg0.is_empty()) {
					event_bus_t::instance().notify(event_bus_msg_t(event_id, arg0));
				}
			}
		}
	}
};

/*
class button_view_controller_t : public view_controller_impl_base_t {
	typedef view_controller_impl_base_t super;
public:
public:
	virtual void init(view_build_context_t &ctx) OVERRIDE {
		super::init(ctx);
	}

};
*/

class tbox_view_t : public decorator_aware_impl_t<myvi::text_box_t> {
	typedef decorator_aware_impl_t<myvi::text_box_t> super;
public:
	gen::meta_t *meta;
public:
	tbox_view_t(gen::meta_t *_meta) {
		meta = _meta;
	}

	virtual void init() OVERRIDE {
		super::init();

		myvi::menu_context_t &ctx = myvi::menu_context_t::instance();

		this->visible = true;
		this->lab.ctx = ctx.lctx1;

		view_factory_t::instance().prepare_context(this->lab.ctx, meta);

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
class tbox_controller_t 
	: public view_controller_impl_base_t, 
	public myvi::subscriber_t<model_message_t>, 
	public myvi::subscriber_t<myvi::textbox_msg_t> {

	typedef view_controller_impl_base_t super;
public:
	myvi::text_box_t *tb;
	volatile_string_impl_t string_value;
public:

	tbox_controller_t() {
		tb = 0;
	}


	virtual void init(view_build_context_t &ctx) OVERRIDE {
// 0. Регистрирует поле в модели , если его ещё там нет
		super::init(ctx);
		register_in_model(ctx);
// 1. Заполняет поле начальным значением из модели

		variant_t value;
		model_t::instance()->read(this->parameter_path, value, type);

		this->tb = dynamic_cast<myvi::text_box_t *>(ctx.get_view());
		_MY_ASSERT(tb, return);

		converter_t *conv = converter_factory_t::instance().for_type(type);
		conv->to_string(value, string_value);
		this->tb->value = string_value;

// 2. Подписывается на изменения в модели и обновления поля
		model_t::instance()->subscribe(this);

// 3. Подписывается на изменения поля и обновления модели
		tb->subscribe(this);
	}

	// обновление от модели
	virtual void accept(model_message_t &msg) OVERRIDE {

		if (!(msg.path == this->parameter_path)) {
			return;
		}
		variant_t value = msg.value;
		_MY_ASSERT(value.type == this->type, return);

		converter_t *conv = converter_factory_t::instance().for_type(this->type);
		conv->to_string(value, string_value);
		this->tb->value = string_value;
		this->tb->dirty = true;
	}

	// обновление от виджета
	virtual void accept(myvi::textbox_msg_t &msg) OVERRIDE {

		if (msg.state != myvi::textbox_msg_t::COMPLETE) {
			return; // в режиме редактирования ниче не меняем
		}
		converter_t *conv = converter_factory_t::instance().for_type(this->type);

		variant_t value;
		bool ret = conv->from_string(msg.value, value);
		if (ret) {
			model_t::instance()->update(this->parameter_path, value);
		}
	}


};

class cbox_view_t : public decorator_aware_impl_t<myvi::combo_box_t> {
	typedef myvi::combo_box_t super;
public:
	gen::meta_t *meta;
public:
	cbox_view_t(gen::meta_t *_meta) {
		meta = _meta;
	}


	virtual void init() OVERRIDE {
		super::init();

		myvi::menu_context_t &ctx = myvi::menu_context_t::instance();

		this->visible = true;
		this->lab.ctx = ctx.lctx1;

		view_factory_t::instance().prepare_context(this->lab.ctx, meta);
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

	virtual void init(view_build_context_t &ctx) OVERRIDE  {
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
class menu_controller_t : public view_controller_t {
public:
public:
	virtual void init(view_build_context_t &ctx) OVERRIDE {

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
			myvi::gobject_t *child_view = parameter_meta_ex_t(child_meta).build_menu_view(ctx);

			// создаем обёртку для вида параметра на основе шаблона
			view_build_context_t child_ctx = ctx;
			child_ctx.set_view_meta(template_meta);
			child_ctx.set_parameter_meta(child_meta);

			myvi::gobject_t *child_wrapper = view_meta_ex_t(template_meta).build_view(child_ctx);
			child_wrapper->add_child(child_view);
			ctx.get_view()->add_child(child_wrapper);
		}

//		gen::view_factory_t::instance()->append_menu_view(view, menu_meta);
	}
};


// вставляет вид, создаваемый параметром, в свой вид
class parameter_view_controller_t : public view_controller_t {
public:
	virtual void init(view_build_context_t &ctx) OVERRIDE {

		myvi::string_t parameter_path = ctx.get_view_meta()->get_string_param("parameterPath");

		_MY_ASSERT(!parameter_path.is_empty(), return);

		meta_path_t path = meta_path_t(parameter_path);
		parameter_path_resolver_t resolver(path);

		gen::parameter_meta_t *child_parameter_meta = resolver.resolve();
//			ctx.set_parameter_meta(child_parameter_meta);

		myvi::gobject_t * child_view = parameter_meta_ex_t(child_parameter_meta).build_menu_view(ctx);
		ctx.get_view()->add_child(child_view);

	}
};


typedef myvi::scrollable_window_t scroll_window_view_t;


// контроллер вида выбора вида
class window_selector_controller_t : public view_controller_t, public myvi::subscriber_t<event_bus_msg_t> {

public:
	class kbd_filter_t : public keyboard_filter_t {
	public:
		window_selector_controller_t *that;
	public:
		kbd_filter_t() {
			that = 0;
		}

		myvi::gobject_t * get_child(s32 i) {

			myvi::gobject_t::iterator_all_t iter = that->view->iterator_all();
			myvi::gobject_t *p = iter.next();
			while (i-- && p) {
				p = iter.next();
			}
			return p;
		}

		virtual bool processKey(myvi::key_t::key_t key) OVERRIDE {

			if (key == myvi::key_t::K_F1) {
				if (myvi::gobject_t *child = get_child(0)) {
					that->select(child);
					return true;
				}
			} else if (key == myvi::key_t::K_F2) {
				if (myvi::gobject_t *child = get_child(1)) {
					that->select(child);
					return true;
				}
			} else if (key == myvi::key_t::K_F3) {
				if (myvi::gobject_t *child = get_child(2)) {
					that->select(child);
					return true;
				}
			} else if (key == myvi::key_t::K_F4) {
				if (myvi::gobject_t *child = get_child(3)) {
					that->select(child);
					return true;
				}
			} else if (key == myvi::key_t::K_F5) {
				if (myvi::gobject_t *child = get_child(4)) {
					that->select(child);
					return true;
				}
			}
			return false;
		}
	};

public:
	myvi::gobject_t *view;
	kbd_filter_t kbd_filter;
	myvi::string_t nameLabelPath;
	myvi::gobject_t * initial_view;
	myvi::string_t event_name;
public:
	window_selector_controller_t() {
		view = 0;
		kbd_filter.that = this;
		initial_view = 0;
	}

	virtual void init(view_build_context_t &ctx) OVERRIDE {

		this->view = ctx.get_view();
		_MY_ASSERT(this->view, return);

		myvi::string_t initial_view_id = ctx.get_view_meta()->get_string_param("initial");
		event_name = ctx.get_view_meta()->get_string_param("listen");

		if (initial_view_id.is_empty()) {
			gen::view_meta_t *child_meta = ctx.get_view_meta()->get_view_child(0);
			_MY_ASSERT(child_meta, return);
			initial_view_id = child_meta->get_id();
		}

		this->initial_view = resolve_view(initial_view_id);
		_MY_ASSERT(initial_view, return);


		keyboard_filter_chain_t::instance().add_filter(&kbd_filter);

		event_bus_t::instance().subscribe(this);

		this->nameLabelPath = ctx.get_view_meta()->get_string_param("nameLabelPath");

	}

	// dynamic_view вызывает этот метод на этапе инициализации вида, когда g-дерево полностью построено
	virtual void on_view_init() OVERRIDE {
		select(this->initial_view);
	}

	// сообщение по шине
	virtual void accept(event_bus_msg_t &msg) OVERRIDE {
		if (msg.event_name == this->event_name) {
			_WEAK_ASSERT(!msg.arg0.is_empty() && msg.arg0.type == variant_type_t::STRING, return);
			select(resolve_view(msg.arg0.get_string_value()));
		}
	}


	// делает видимым указанный вид и скрывает все остальные
	void select(myvi::gobject_t * selected_child) {

		_MY_ASSERT(this->view, return);
		_MY_ASSERT(selected_child, return);
		_MY_ASSERT(this->view->is_direct_parent_of(selected_child), return);
		

		selected_child->visible = true;

		myvi::gobject_t::iterator_visible_t iter = view->iterator_visible();
		myvi::gobject_t *child = iter.next();
		while (child) {
			if (child != selected_child) {
				child->visible = false;
			}
			child = iter.next();
		}
		if (this->view->w && this->view->h) {
			this->view->do_layout();
		}
		update_name(selected_child);

		this->view->dirty = true;
	}

private:

	myvi::gobject_t * resolve_view(myvi::string_t view_id) {
		dynamic_view_mixin_t *dvm = dynamic_cast<dynamic_view_mixin_t *>(this->view);
		_MY_ASSERT(dvm, return 0);
		myvi::gobject_t *view = dvm->get_child(view_id);
		_MY_ASSERT(view, return 0);
		return view;
	}

	void update_name(myvi::gobject_t *child) {
		_MY_ASSERT(child, return);
		dynamic_view_t *dv = dynamic_cast<dynamic_view_t *>(child);
		_MY_ASSERT(dv, return);
		update_name(dv->get_view_meta()->get_name());
	};

	void update_name(myvi::string_t name) {

		if (nameLabelPath.is_empty()) return;

		myvi::gobject_t *root = this->view->get_root();
		dynamic_view_mixin_t *dvm = dynamic_cast<dynamic_view_mixin_t *>(root);
		_MY_ASSERT(dvm, return);
		myvi::gobject_t *name_lab_view = dvm->resolve_path(nameLabelPath);
		_MY_ASSERT(name_lab_view, return);

		// HACK : по идее надо обращатся к контроллеру вида , а не писать в чужой вид напрямую
		myvi::label_t * lab = dynamic_cast<myvi::label_t *>(name_lab_view);
		_MY_ASSERT(lab, return);
		lab->text = name;
		lab->dirty = true;
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
		this->stretch_last = meta->get_string_param("stretch_last") == "true";
		if (meta->get_int_param("bw") != _NAN) {
			this->bw = meta->get_int_param("bw");
		}
		if (meta->get_int_param("bh") != _NAN) {
			this->bw = meta->get_int_param("bh");
		}
	}
};


// центрирует дочерний виджет по обеим осям родительского
// EXPECT: children.count == 1
class center_meta_layout_t : public myvi::layout_t {
public:
	center_meta_layout_t(gen::meta_t *meta) {
	}

	virtual void get_preferred_size(myvi::gobject_t *parent, s32 &aw, s32 &ah) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		myvi::gobject_t *p = iter.next();
		_MY_ASSERT(p,return);
		p->get_preferred_size(aw,ah);
		p = iter.next();
		_MY_ASSERT(!p,return);
	}

	virtual void layout(myvi::gobject_t *parent) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		myvi::gobject_t *p = iter.next();
		_MY_ASSERT(p,return);

		p->get_preferred_size(p->w,p->h);
		if (p->h > parent->h) {
			p->h = parent->h;
		}
		if (p->w > parent->w) {
			p->w = parent->w;
		}

		p->y = (parent->h - p->h)/2;
		p->x = (parent->w - p->w)/2;

		p = iter.next();
		_MY_ASSERT(!p,return);
	}

};

#define _META_LAYOUT_PERCENTAGE 0.6

class menu_meta_layout_t : public myvi::layout_t {

public:
	menu_meta_layout_t(gen::meta_t *meta) {
	}

	virtual void get_preferred_size(myvi::gobject_t *parent, s32 &pw, s32 &ph) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		// 1st child
		myvi::gobject_t *first  = iter.next();
		_MY_ASSERT(first, return);

		s32 w1, h1;
		first->get_preferred_size(w1, h1);
		pw = (s32)(w1 / _META_LAYOUT_PERCENTAGE);
		ph = h1;

		// 2st child
		s32 w2, h2;
		myvi::gobject_t *second  = iter.next();
		_MY_ASSERT(second, return);
		second->get_preferred_size(w2, h2);
		if (ph < h2) {
			ph = h2;
		}
		pw += w2 + 5;

		// none other children
		_MY_ASSERT(!iter.next(), return);
		
	}

	virtual void layout(myvi::gobject_t *parent) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		// 1st child
		myvi::gobject_t *first  = iter.next();
		_MY_ASSERT(first, return);
		first->w = (s32)(parent->w * _META_LAYOUT_PERCENTAGE);
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




}  // ns
#endif
