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
		if (meta_path.is_relative()) {
			_MY_ASSERT(root_meta, return 0);
			meta_path_t::iterator_t iter = meta_path.iterator();
			return resolve(iter, root_meta);
		} else {
			return resolve();
		}
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



class view_build_context_aware_t {
public:
	virtual void init(view_build_context_t &ctx) = 0;
};



// интерфейс контроллера вида
class view_controller_t : public view_build_context_aware_t {
public:
	// dynamic_view вызывает этот метод на этапе инициализации вида, когда g-дерево полностью построено
	virtual void on_view_init() {
	}

};








class decorator_t {
public:
	virtual void render_before(myvi::gobject_t *obj, myvi::surface_t &dst) {
	}

	virtual void render_after(myvi::gobject_t *obj, myvi::surface_t &dst) {
	}

	virtual void adjust_preferred_size(myvi::gobject_t *obj, s32 &pw, s32 &ph) {
	}
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

	virtual void render_before(myvi::surface_t &dst) OVERRIDE {
		if (decorator) {
			decorator->render_before(this, dst);
		}
	}

	virtual void render_after(myvi::surface_t &dst) OVERRIDE {
		if (decorator) {
			decorator->render_after(this, dst);
		}
	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		TBase::set_dirty(dirty);
		if (dirty && parent && !decorator) {
			parent->dirty = true;
		}
	}

	virtual void adjust_preferred_size(s32 &pw, s32 &ph) OVERRIDE {
		if (decorator) {
			decorator->adjust_preferred_size(this, pw, ph);
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

	virtual void render_before(myvi::gobject_t *obj, myvi::surface_t &dst) OVERRIDE {
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


// рисует 3d рамку вокруг контрола
class d3_decorator_t : public decorator_t {
public:
	bool inversed;
public:
	d3_decorator_t(gen::meta_t *meta) {
		inversed = false;

		if (!meta->get_string_param("d3_inversed").is_empty()) {
			inversed = meta->get_string_param("d3_inversed") == "true";
		}
	}

	virtual void render_after(myvi::gobject_t *obj, myvi::surface_t &dst) OVERRIDE {

		s32 x,y, w = obj->w, h = obj->h;
		obj->translate(x, y);
		dst.ctx.alfa = 0xff;

		u32 black = 0x222625;
		u32 white = 0xC3D4DB;
		u32 gray = 0xA79FAA;

		dst.ctx.pen_color = inversed ? white : black;
		dst.line(x,y,w,false);
		dst.line(x,y,h,true);

		dst.ctx.pen_color = inversed ? gray : black;
		dst.line(x+1,y+1,w-1,false);
		dst.line(x+1,y+1,h-1,true);

		dst.ctx.pen_color = inversed ? black : white;
		dst.line(x,y+h-1,w,false);
		dst.line(x+w-1,y,h,true);

		dst.ctx.pen_color = inversed ? black : gray;
		dst.line(x+1,y+h-2,w-2,false);
		dst.line(x+w-2,y+1,h-2,true);

	}

	virtual void adjust_preferred_size(myvi::gobject_t *obj, s32 &pw, s32 &ph) OVERRIDE {
		pw += 4;
		ph += 4;
	}

};

// рисует галочку у cbox-a
class cbox_decorator_t : public decorator_t {
	typedef decorator_t super;
public:
public:
	cbox_decorator_t(gen::meta_t *meta) {
	}

	virtual void render_after(myvi::gobject_t *obj, myvi::surface_t &dst) OVERRIDE {

		u32 black = 0x222625;
		u32 gray = 0x909986;
		u32 light_gray = 0xB3C7C6;

		s32 x,y, w = obj->w, h = obj->h;
		obj->translate(x, y);

		s32 w1 = h;

		if (w < w1) return;

		x += w - w1;
		y += 2;
		w = w1-2;
		h = h-4;

		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = light_gray;
		dst.line(x,y,w,false);
		dst.line(x,y,h,true);

		dst.ctx.pen_color = black;
		dst.line(x,y+h-1,w,false);
		dst.line(x+w-1,y,h,true);

		x += 1; y += 1;
		w -= 2; h -= 2;
		dst.ctx.pen_color = gray;
		dst.fill(x,y,w,h);

		x += w1/3; y += h/3;

		dst.ctx.pen_color = black;
		dst.line(x,y,x+6,y);
		dst.line(x,y,x+3,y+6);
		dst.line(x+6,y,x+3,y+6);
	}

	virtual void adjust_preferred_size(myvi::gobject_t *obj, s32 &pw, s32 &ph) OVERRIDE {
		pw += ph;
	}

};


typedef decorator_aware_impl_t<myvi::gobject_t> decorator_aware_base_t;

class dynamic_view_base_t : 
	public decorator_aware_base_t,
	public view_build_context_aware_t {

	typedef decorator_aware_base_t super;
private:
	gen::view_meta_t *view_meta;
	view_controller_t *view_controller;
	s32 fw, fh;
public:

	dynamic_view_base_t() {
		view_meta = 0;
		view_controller = 0; // устанавливается позже

		fw = fh = _NAN;
	}

	virtual void init(view_build_context_t &ctx) OVERRIDE {
		view_meta = ctx.get_view_meta();
		view_controller = 0; // устанавливается позже

		fw = this->view_meta->get_int_param("width");
		fh = this->view_meta->get_int_param("height");
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


	
	virtual void vget_preferred_size(s32 &pw, s32 &ph) OVERRIDE {

		super::vget_preferred_size(pw,ph);

		if (fw != _NAN) {
			pw = fw;
		}
		if (fh != _NAN) {
			ph = fh;
		}
	}

};

class dynamic_view_t : public dynamic_view_mixin_aware_impl_t<dynamic_view_base_t> {
};

class _mouse_decorator_aware_base_t : public dynamic_view_base_t, public myvi::focus_client_t {
};

typedef mouse_aware_impl_t<_mouse_decorator_aware_base_t> mouse_decorator_aware_base_t;


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
	
	myvi::string_t get_parameter_path(myvi::gobject_t *view) {
		myvi::string_t parameter_path;

		while (view && parameter_path.is_empty()) {
			dynamic_view_base_t *dv = dynamic_cast<dynamic_view_base_t *>(view);
			if (dv) {
				parameter_path = dv->get_view_meta()->get_string_param("parameterPath");
			}
			view = view->parent;
		}
		return parameter_path;
	}

	virtual void init(view_build_context_t &ctx) OVERRIDE {


		myvi::string_t parameter_path = get_parameter_path(ctx.get_view());

		if (!parameter_path.is_empty()) {
			meta_path_t path = meta_path_t(parameter_path);
			parameter_path_resolver_t resolver(path);

			gen::parameter_meta_t *child_parameter_meta = resolver.resolve(ctx.try_get_parameter_meta());
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

		_MY_ASSERT(!_event_name.is_empty(), return);
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



#define _TEXT_PADDING 2

class label_t  : public myvi::gobject_t {
public:
	myvi::string_t text;  // текст
	label_context_t ctx;
public:
	label_t() {
		visible = false;
	}

	virtual void render(myvi::surface_t &dst) OVERRIDE {
	
		s32 ax = 0, ay = 0;
		_MY_ASSERT(visible && ctx.font,return);
		ctx.font->char_surface.ctx = ctx.sctx;
		ctx.font->set_char_size_px(0,ctx.font_size);

		translate(ax,ay);

		ax += _TEXT_PADDING;

		if (!text.is_empty()) {
			// text
			ctx.font->print_to(ax,ay,dst,text);	
		} else {
			// do nothing
		}
	}

	virtual void vget_preferred_size(s32 &aw, s32 &ah) OVERRIDE {
		_MY_ASSERT(visible,return);
		_MY_ASSERT(ctx.font,return);
		ctx.font->set_char_size_px(0,ctx.font_size);
		if (!text.is_empty()) {
			// text
			ctx.font->get_string_size(this->text, aw, ah);
		} else {
			// no content - try measure something
			myvi::string_t cs1 = myvi::string_t("1");
			ctx.font->get_string_size(cs1, aw, ah);
		}
	}
};

class button_context_t {
public:
	u32 bk_color;	// цвет не выбранной кнопки
	u32 bk_sel_color;	// цвет выбранной кнопки
	u32 bk_pressed_color;	// цвет нажатой кнопки
	u32 bk_disabled_color; // цвет неактивной кнопки
	myvi::surface_context_t sctx;
public:
	button_context_t() {
		bk_color=(0xB8BDCF),bk_sel_color=(0x848794),bk_pressed_color=(0x6B8896);
		bk_disabled_color = 0xA0A0A0;
	}
};

class text_aware_t {
public:
	virtual label_context_t & get_text_context() = 0;
};

class lab_view_t : public label_t, public text_aware_t {
public:

	lab_view_t() {
		visible = true;
	}

	virtual label_context_t & get_text_context() OVERRIDE {
		return ctx;
	}

};


class label_controller_t : public view_controller_impl_base_t, public myvi::subscriber_t<event_bus_msg_t> {
	typedef view_controller_impl_base_t super;
public:
	label_t *lab;
	myvi::string_t listen_event_name;
public:

	label_controller_t() {
		lab = 0;
	}

	virtual void init(view_build_context_t &ctx) OVERRIDE {
		super::init(ctx);

		lab = dynamic_cast<label_t *>(ctx.get_view());
		_MY_ASSERT(lab, return);

		this->listen_event_name = ctx.get_view_meta()->get_string_param("listen");

		myvi::string_t static_text = ctx.get_view_meta()->get_string_param("staticText");
		if (!static_text.is_empty()) {

			lab->text = static_text;
		} else {

			myvi::string_t label_source = ctx.get_view_meta()->get_string_param("labelSource");
			if (label_source.is_empty()) {
				label_source = "name";
			}
			lab->text = ctx.get_parameter_meta()->get_string_param(label_source);
		}

		if (! this->listen_event_name.is_empty()) {
			event_bus_t::instance().subscribe(this);
		}
	}

	virtual void accept(event_bus_msg_t &msg) OVERRIDE {

		_MY_ASSERT (! this->listen_event_name.is_empty(), return);

		if (this->listen_event_name == msg.event_name) {
			lab->text = msg.arg0.get_string_value();
		}
	}

};



class view_cache_t {
	typedef std::unordered_map<myvi::string_t, myvi::gobject_t *, gen::string_t_hash_t> vmap_t;
public:
	vmap_t view_map;
public:
	myvi::gobject_t * get_view(myvi::string_t view_id) {
		_MY_ASSERT(!view_id.is_empty(), return 0);

		vmap_t::iterator iter = view_map.find(view_id);
		if(iter != view_map.end()) return iter->second;

		gen::view_meta_t *view_meta = gen::meta_registry_t::instance().find_view_meta(view_id);
		myvi::gobject_t *view = view_meta_ex_t(view_meta).build_view_no_ctx();
		view->init();
		view_map[view_id] = view;
		return view;
	}
};


class popup_manager_t  {
private:
	popup_manager_t() {
	}
public:
	view_cache_t view_cache;
public:
	static popup_manager_t & instance() {
		static popup_manager_t _instance;
		return _instance;
	}


	void popup(myvi::string_t view_id) {
		myvi::gobject_t *view = view_cache.get_view(view_id);
		myvi::modal_overlay_t::instance().push_modal(view);
		view->w = myvi::modal_overlay_t::instance().w;
		view->h = myvi::modal_overlay_t::instance().h;
		view->do_layout();
	}

	void popdown() {
		myvi::modal_overlay_t::instance().pop_modal();
		myvi::focus_manager_t::instance().select(0);
	}

};



class button_t : 
	public mouse_decorator_aware_base_t,
	public myvi::focus_aware_t,
	public myvi::publisher_t<myvi::key_t::key_t, 1> {
public:
	button_context_t ctx;
	myvi::property_t<bool, button_t> pressed;
private:
	bool _pressed;	// текущее состояние
private:
	bool get_pressed() {
		return _pressed;
	}
	void set_pressed(bool apressed) {
		_pressed = apressed;
		dirty = true;
	}

public:
	button_t() {
		_pressed = (false);
		pressed.init(this,&button_t::get_pressed, &button_t::set_pressed);

	}


	virtual void render(myvi::surface_t &dst) OVERRIDE {
		dst.ctx = ctx.sctx;
		if (!enabled) 
			dst.ctx.pen_color = ctx.bk_disabled_color;
		else if (pressed) 
			dst.ctx.pen_color = ctx.bk_pressed_color;
		else if (selected) 
			dst.ctx.pen_color = ctx.bk_sel_color;
		else 
			dst.ctx.pen_color = ctx.bk_color;

		s32 ax = 0, ay = 0;

		translate(ax,ay);

		dst.ctx.alfa = 0xff;
		dst.fill(ax,ay,w,h);

		dst.ctx.pen_color = 0xffffff;
		dst.ctx.alfa = 128;
		dst.rect(ax,ay,w,h);
	}

	virtual void key_event(myvi::key_t::key_t key) OVERRIDE {
		notify(key);
	}

};

typedef button_t button_view_t;


class button_view_controller_t : 
	public view_controller_impl_base_t, 
	public myvi::subscriber_t<myvi::key_t::key_t> {

	typedef view_controller_impl_base_t super;
public:
	button_view_t *button_view;
	gen::meta_t *meta;
	myvi::string_t popup_view_id;
	myvi::string_t event_id;
	bool close;
public:
	button_view_controller_t() {
		button_view = 0;
		meta = 0;
		close = false;
	}

	label_t * find_label() {
		_MY_ASSERT(button_view, return 0);
		label_t *ret = 0;
		myvi::gobject_t::iterator_visible_t iter = button_view->iterator_visible();
		myvi::gobject_t *p = iter.next();
		while(p) {
			ret = dynamic_cast<label_t *>(p);
			if (ret) {
				return ret;
			}
			p = iter.next();
		}
		_MY_ASSERT(ret, return 0);
		return ret;
	}

	virtual void init(view_build_context_t &ctx) OVERRIDE {
		super::init(ctx);

		this->button_view = dynamic_cast<button_view_t *>(ctx.get_view());
		_MY_ASSERT(this->button_view, return);
		button_view->subscribe(this);

		this->meta = ctx.get_view_meta();
		_MY_ASSERT(this->meta, return);

		label_t *lab = find_label();
		lab->text = ctx.get_view_meta()->get_string_param("name");

		this->popup_view_id = ctx.get_view_meta()->get_string_param("popup_view");
		this->event_id = meta->get_string_param("event");
		this->close = meta->get_string_param("close") == "true";
	}

	virtual void accept(myvi::key_t::key_t & key) OVERRIDE {

		if (key == myvi::key_t::K_ENTER) {

			if (!this->event_id.is_empty()) {
				variant_t arg0 = meta_ex_t(meta).get_variant_param("arg0");
				if (!arg0.is_empty()) {
					event_bus_t::instance().notify(event_bus_msg_t(event_id, arg0));
				}

			} 
			
			if (!this->popup_view_id.is_empty()) {
				popup_manager_t::instance().popup(this->popup_view_id);
			} 

			if (this->close) {
				popup_manager_t::instance().popdown();
			}
		}
	}

};



#define INPUT_MAX_LEN 30
#define COLOR_SELECTED 0xAFBFCF
#define COLOR_CAPTURED 0x267F00

class textbox_msg_t {
public:
	enum state_t {
		EDIT, // значенеие еще редактируется
		COMPLETE // пользователь нажал Enter и мы вышли из режима редактирования
	} state;
	myvi::string_t value;
public:
	textbox_msg_t(state_t _state, myvi::string_t _value) : state(_state), value(_value) {
	}
};

class text_box_t : 
	public mouse_decorator_aware_base_t, 
	public myvi::focus_aware_t, 
	public myvi::publisher_t<textbox_msg_t, 1> {
	typedef gobject_t super;
public:
	myvi::property_t<myvi::string_t , text_box_t> value;
	label_t lab;
	bool cursor_visible;
	u32 cursor_color;
private:
	myvi::string_impl_t<INPUT_MAX_LEN> _value;
	s32 cursor_pos[INPUT_MAX_LEN];
	s32 caret_pos;
private:
	void set_value(myvi::string_t cvalue) {
		_value=(cvalue);
		lab.text = _value;
		caret_pos = _value.length();
	}

	myvi::string_t get_value() {
		return _value;
	}
public:
	text_box_t() {
		value.init(this,&text_box_t::get_value, &text_box_t::set_value);
		lab.ctx.sctx.pen_color = 0x00;
		lab.visible = true;
		caret_pos = 0;
		cursor_visible = false;
		cursor_color = 0x000000;

		add_child(&lab);
	}

	void measure_cursor_pos() {
		for (s32 i=1; i <= _value.length(); i++) {
			lab.text = _value.sub(0,i);
			s32 lw, lh;
			lab.get_preferred_size(lw,lh);
			cursor_pos[i-1] = lw;
		}
		lab.text = _value;
	}

	virtual void vget_preferred_size(s32 &aw, s32 &ah) OVERRIDE {
		measure_cursor_pos();
		lab.get_preferred_size(aw, ah);
	}

	virtual void do_layout() OVERRIDE {
		lab.x = 0;
		lab.y = 0;
		lab.w = w; // место для курсора
		lab.h = h;
	}

	virtual void key_event(myvi::key_t::key_t key) OVERRIDE {

		s32 cur_len = _value.length();

		if (key == myvi::key_t::K_ENTER) {
			_MY_ASSERT(parent,return);
			if (!cursor_visible) {
				this->capture_focus();
				cursor_visible = true;
			} else {
				this->release_focus();
				cursor_visible = false;
				notify(textbox_msg_t(textbox_msg_t::COMPLETE, _value));
			}
			goto lab_update_input;
		}
		// не должны редактироваться пока не перешли в активное состояние
		if (!cursor_visible) {
			return;
		}

		if (key == myvi::key_t::K_LEFT) {
			if (caret_pos) caret_pos--;
			goto lab_update_input;
		}
		if (key == myvi::key_t::K_RIGHT) {
			if (caret_pos < cur_len) caret_pos++;
			goto lab_update_input;
		}

		if ((key == myvi::key_t::K_BKSP && caret_pos) || (key == myvi::key_t::K_DELETE && caret_pos < cur_len)) {
			if (key == myvi::key_t::K_BKSP) caret_pos--;
			_value.delete_at(caret_pos);
			goto lab_update_input;
		}

		if (caret_pos < INPUT_MAX_LEN-1) {
			u8 ch = 0;
			if (key == myvi::key_t::K_0) ch = '0';
			if (key == myvi::key_t::K_1) ch = '1';
			if (key == myvi::key_t::K_2) ch = '2';
			if (key == myvi::key_t::K_3) ch = '3';
			if (key == myvi::key_t::K_4) ch = '4';
			if (key == myvi::key_t::K_5) ch = '5';
			if (key == myvi::key_t::K_6) ch = '6';
			if (key == myvi::key_t::K_7) ch = '7';
			if (key == myvi::key_t::K_8) ch = '8';
			if (key == myvi::key_t::K_9) ch = '9';
			if (key == myvi::key_t::K_DOT) ch = '.';
			if (key == myvi::key_t::K_F1) ch = '+';
			if (key == myvi::key_t::K_F2) ch = '-';
			if (ch) {
				_value.insert_at(caret_pos, ch);
				caret_pos++;
				goto lab_update_input;
			}
		}
		return;
	lab_update_input:
		lab.text = _value;

		if (parent) {
			parent->child_request_size_change(this);
		}
		dirty = true;
		notify(textbox_msg_t(textbox_msg_t::EDIT, _value));
	}


	virtual void set_selected(bool selected) {
		_MY_ASSERT(parent,return);
		dirty = true;
		focus_client_t::set_selected(selected);
	}

	virtual void render(myvi::surface_t &dst) OVERRIDE {
		s32 ax, ay;
		translate(ax,ay);
		if (selected) {
			dst.ctx.reset();
			dst.ctx.pen_color = cursor_visible ? COLOR_CAPTURED : COLOR_SELECTED;
			dst.fill(ax,ay,w,h);
		}
		// draw vline
		if (cursor_visible) {
			dst.ctx.pen_color = cursor_color;
			s32 acp = 0;
			_MY_ASSERT(caret_pos <= INPUT_MAX_LEN,return);
			if (caret_pos) 
				acp += cursor_pos[caret_pos-1];

			dst.line(ax + acp, ay, lab.h,true);
		}

	}

};



class tbox_view_t : public text_box_t, public text_aware_t {
public:
	virtual label_context_t & get_text_context() OVERRIDE {
		return this->lab.ctx;
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
	public myvi::subscriber_t<textbox_msg_t> {

	typedef view_controller_impl_base_t super;
public:
	text_box_t *tb;
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

		this->tb = dynamic_cast<text_box_t *>(ctx.get_view());
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
	virtual void accept(textbox_msg_t &msg) OVERRIDE {

		if (msg.state != textbox_msg_t::COMPLETE) {
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


typedef gen::combobox_item_proto_t combobox_item_t;

// метка с функцией выбора из списка значений
class combo_box_t : 
	public mouse_decorator_aware_base_t, 
	public myvi::focus_aware_t, 
	public myvi::publisher_t<combobox_item_t *, 1> {

	typedef gobject_t super;

	class empty_iterator_t : public myvi::iterator_t<combobox_item_t> {
	public:
		virtual combobox_item_t* next(void* prev) OVERRIDE {
			return 0;
		}
	};
public:
	label_t lab;
	myvi::wo_property_t<myvi::iterator_t<combobox_item_t>*,combo_box_t> values;
	myvi::property_t<combobox_item_t *, combo_box_t> value;
private:
	combobox_item_t * _value;
	myvi::string_impl_t<INPUT_MAX_LEN> _str_value;
	myvi::iterator_t<combobox_item_t> *_values;
	bool captured;
	combobox_item_t *sprev;
	myvi::reverse_iterator_t<combobox_item_t> revit;
	empty_iterator_t empty_iterator;
private:
	void set_values(myvi::iterator_t<combobox_item_t> *values) {
		_values = values;
		if (!values)
			_values = &empty_iterator;
		revit.assign(_values);
		sprev = 0;
	}

	void set_value(combobox_item_t * value) {
		_value=(value);
		lab.text = _value->get_string_value();
	}

	combobox_item_t * get_value() {
		return _value;
	}
public:

	combo_box_t() {
		captured = false;
		_values = 0;
		_value = 0;
		sprev = 0;
		lab.ctx.sctx.pen_color = 0x00;
		lab.visible = true;
		values.init(this,&combo_box_t::set_values);
		value.init(this,&combo_box_t::get_value, &combo_box_t::set_value);

		values = &empty_iterator;

		add_child(&lab);
	}

	//virtual void set_dirty(bool dirty) OVERRIDE {
	//	if (dirty && parent) {
	//		parent->dirty = true;
	//	}
	//	super::set_dirty(dirty);
	//}

	virtual void vget_preferred_size(s32 &aw, s32 &ah) OVERRIDE {
		lab.get_preferred_size(aw,ah);
		//if (h < lab.h) h = lab.h;
		//if (w < lab.w + 5) w = lab.w + 5;
	}

	virtual void do_layout() OVERRIDE {
		lab.x = 0;
		lab.y = 0;
		lab.w = w;
		lab.h = h;
	}

	virtual void key_event(myvi::key_t::key_t key) OVERRIDE {
		_MY_ASSERT(_values,return);

		if (key == myvi::key_t::K_ENTER) {
			_MY_ASSERT(parent,return);
			if (!captured) {
				this->capture_focus();
				captured = true;
			} else {
				this->release_focus();
				captured = false;
			}
			goto lab_update_cbox;
		}
		if (!captured) {
			return;
		}

		if (key == myvi::key_t::K_UP || key == myvi::key_t::K_LEFT) {
			sprev = revit.next(sprev);
			if (!sprev) {
				sprev = revit.next(sprev);
				if (!sprev)
					return;
			}
			goto lab_update_cbox;
		}
		if (key == myvi::key_t::K_DOWN || key == myvi::key_t::K_RIGHT) {
			sprev = _values->next(sprev);
			if (!sprev) {
				sprev = _values->next(sprev);
				if (!sprev)
					return;
			}
			goto lab_update_cbox;
		}
		return;

	lab_update_cbox:

		if (sprev)
			_value=(sprev);

		if (_value) {
			_str_value = _value->get_string_value();;
			lab.text = _str_value;
		}
		if (parent)
			parent->child_request_size_change(this);
		dirty = true;

		notify(_value);
	}

	virtual void render(myvi::surface_t &dst) OVERRIDE {
		s32 ax, ay;
		translate(ax,ay);
		if (selected) {
			dst.ctx.reset();
			dst.ctx.pen_color = captured ? COLOR_CAPTURED : COLOR_SELECTED;
			dst.fill(ax,ay,w,h);
		}
	}

};




class cbox_view_t : public combo_box_t, public text_aware_t {
public:
	virtual label_context_t & get_text_context() OVERRIDE {
		return this->lab.ctx;
	}
};


/*
Контроллер комбобокса

1. Заполняет поле начальным значением из модели
2. Подписывается на изменения в модели и обновления поля
3. Подписывается на изменения поля и обновления модели

4. использует конвертор для преобразования значения параметра из combobox_item_t в число и обратно

*/

class cbox_controller_t : 
	public view_controller_impl_base_t,
	public myvi::subscriber_t<model_message_t>,
	public myvi::subscriber_t<combobox_item_t *> {

	typedef view_controller_impl_base_t super;
public:
	//units
	//validators
	//textbox*
	//combobox*
	combo_box_t *cb;
	myvi::iterator_t<combobox_item_t> *values;
public:
	cbox_controller_t() {
		cb = 0;
		values = 0;
	}

	virtual void init(view_build_context_t &ctx) OVERRIDE  {
		super::init(ctx);
// 0. Регистрирует поле в модели , если его ещё там нет
		register_in_model(ctx);

// 1. Заполняет поле начальным значением из модели

		variant_t value;
		model_t::instance()->read(this->parameter_path, value, this->type);

		cb = dynamic_cast<combo_box_t *>(ctx.get_view());
		_MY_ASSERT(cb, return);

		gen::type_meta_t * type_meta = ctx.get_parameter_meta()->get_type_meta();
		_MY_ASSERT(type_meta->is_enum(), return);


		values = type_meta->get_combobox_iterator();
		cb->values = values;
		// устанавливаем начальное значение
		set_value(value.get_int_value());


// 2. Подписывается на изменения в модели и обновления поля
		model_t::instance()->subscribe(this);

// 3. Подписывается на изменения поля и обновления модели
		cb->subscribe(this);

	}

	void set_value(s32 val) {
		combobox_item_t * p = values->next(0);
		while (p) {
			if (p->get_int_value() == val) {
				cb->value = p;
				return;
			}
			p = values->next(p);
		}
		_WEAK_ASSERT(0, return);
	}

	// обновление от модели
	virtual void accept(model_message_t &msg) OVERRIDE {

		if (!(msg.path == this->parameter_path)) {
			return;
		}
		variant_t value = msg.value;
		_MY_ASSERT(value.type == this->type, return);

		set_value(value.get_int_value());
		this->cb->dirty = true;
	}

	// обновление от виджета
	virtual void accept(combobox_item_t * &msg) OVERRIDE {
		variant_t value(msg->get_int_value());
		model_t::instance()->update(this->parameter_path, value);
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
class parameter_view_controller_t : public view_controller_impl_base_t {
	typedef view_controller_impl_base_t super;
public:
	virtual void init(view_build_context_t &ctx) OVERRIDE {

		super::init(ctx);

		myvi::gobject_t * child_view = parameter_meta_ex_t(ctx.get_parameter_meta()).build_menu_view(ctx);
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
	myvi::gobject_t * initial_view;
	myvi::string_t event_name;
	myvi::string_t name_event_name;
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
		this->event_name = ctx.get_view_meta()->get_string_param("listen");
		this->name_event_name = ctx.get_view_meta()->get_string_param("event");

		if (initial_view_id.is_empty()) {
			gen::view_meta_t *child_meta = ctx.get_view_meta()->get_view_child(0);
			_MY_ASSERT(child_meta, return);
			initial_view_id = child_meta->get_id();
		}

		this->initial_view = resolve_view(initial_view_id);
		_MY_ASSERT(initial_view, return);


		keyboard_filter_chain_t::instance().add_filter(&kbd_filter);

		event_bus_t::instance().subscribe(this);

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
		this->view->child_request_size_change(selected_child);
		//if (this->view->w && this->view->h) {
		//	this->view->do_layout();
		//}
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
		if (!name_event_name.is_empty()) {
			event_bus_t::instance().notify(event_bus_msg_t(name_event_name, name));
		}
	}

/*
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
*/
};

/*
* ====================== ЛАЙОУТЫ И ПРОЧ. =======================
*/

class stretch_meta_layout_t : public myvi::stretch_layout_t {
	typedef myvi::stretch_layout_t super;
public:
	stretch_meta_layout_t(gen::meta_t *meta) {
		padding = 0;
		if (meta->get_int_param("padding") != _NAN) {
			padding = meta->get_int_param("padding");
		}
	}

};


class stack_meta_layout_t : public myvi::stack_layout_t {
public:
	stack_meta_layout_t(gen::meta_t *meta) {
		if (!meta->get_string_param("vertical").is_empty()) {
			this->vertical = meta->get_string_param("vertical") == "true";
		}
		if (!meta->get_string_param("preferred_item_size").is_empty()) {
			this->preferred_item_size = meta->get_string_param("preferred_item_size") == "true";
		}
		if (!meta->get_string_param("stretch_last").is_empty()) {
			this->stretch_last = meta->get_string_param("stretch_last") == "true";
		}
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
	s32 padding;
public:
	center_meta_layout_t(gen::meta_t *meta) {
		padding = 0;
		if (meta->get_int_param("padding") != _NAN) {
			padding = meta->get_int_param("padding");
		}
	}

	virtual void get_preferred_size(myvi::gobject_t *parent, s32 &aw, s32 &ah) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		myvi::gobject_t *p = iter.next();
		_MY_ASSERT(p,return);
		p->get_preferred_size(aw,ah);

		aw += padding * 2;
		ah += padding * 2;

		p = iter.next();
		_MY_ASSERT(!p,return);
	}

	virtual void layout(myvi::gobject_t *parent) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		myvi::gobject_t *p = iter.next();
		_MY_ASSERT(p,return);

		p->get_preferred_size(p->w,p->h);




		p->x = (parent->w - p->w)/2;
		p->y = (parent->h - p->h)/2;


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
