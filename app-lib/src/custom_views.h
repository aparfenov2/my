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
	typedef std::hash_map<u32, variant_holder_t *> children_map_t;
	children_map_t children;
public:

	variant_holder_t *get_holder(myvi::string_t key) {
		u32 ikey = (std::size_t)key;
		children_map_t::iterator iter = children.find(ikey);
		if(iter != children.end()) return iter->second;
		return 0;
	}

	virtual void update(myvi::string_t parameter_path, variant_t &value) OVERRIDE {
		variant_holder_t *holder = get_holder(parameter_path);
		_MY_ASSERT(holder, return);
		holder->assign(value);
		model_message_t msg(parameter_path, value);
		notify(msg);
	}


	virtual void read(myvi::string_t parameter_path, variant_t &value) OVERRIDE {
		variant_holder_t *holder = get_holder(parameter_path);
		_MY_ASSERT(holder, return);
		value = holder->get_value();
	}

	virtual void try_register_path(myvi::string_t parameter_path, variant_t &initial_value, variant_type_t::variant_type_t expected_type) OVERRIDE  {

		variant_holder_t *holder = get_holder(parameter_path);
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
		variant_holder_t *holder = get_holder(parameter_path);
		if (!holder) {
			holder = new variant_holder_t(expected_type);
			children[(std::size_t)parameter_path] = holder;
		}
		return holder;
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
		std::sprintf((char *)str.c_str(), "%ld", value.get_int_value());
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
		s32 len = std::sprintf(buf, "%.4f", value.get_float_value());

		while (len > 1 && (buf[len-1] == '0' || buf[len-1] == '.')) {
			--len;
			if (buf[len] == '.') break;
		}
		str.set_length(len);
	}

	virtual bool from_string(myvi::string_t &str, variant_t &value) OVERRIDE {
		char *end;
		float fval = std::strtod(str.c_str(), &end);
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
	virtual void init(view_build_context_t ctx) = 0;
};



// интерфейс контроллера вида
class view_controller_t {
public:
	virtual void init(view_build_context_t ctx) = 0;
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

	virtual void set_selected(myvi::gobject_t *obj, bool selected) {
	}

	virtual void set_enabled(myvi::gobject_t *obj, bool enabled) {
	}

	virtual void set_captured(myvi::gobject_t *obj, bool captured) {
	}

	virtual void set_valid(myvi::gobject_t *obj, bool valid) {
	}

};


class decorator_array_t : public decorator_t {
public:
	std::vector<decorator_t *> children;
public:
	decorator_array_t(gen::meta_t *meta) {
	}

	void add_child(decorator_t *child) {
		children.push_back(child);
	}

	virtual void render_before(myvi::gobject_t *obj, myvi::surface_t &dst) OVERRIDE {
		for (std::vector<decorator_t*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
			(*iter)->render_before(obj, dst);
		}
	}

	virtual void render_after(myvi::gobject_t *obj, myvi::surface_t &dst) OVERRIDE {
		for (std::vector<decorator_t*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
			(*iter)->render_after(obj, dst);
		}
	}

	virtual void adjust_preferred_size(myvi::gobject_t *obj, s32 &pw, s32 &ph) OVERRIDE {

		for (std::vector<decorator_t*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
			(*iter)->adjust_preferred_size(obj, pw, ph);
		}
	}

	virtual void set_selected(myvi::gobject_t *obj, bool selected) OVERRIDE {

		for (std::vector<decorator_t*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
			(*iter)->set_selected(obj, selected);
		}
	}

	virtual void set_enabled(myvi::gobject_t *obj, bool enabled) OVERRIDE {

		for (std::vector<decorator_t*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
			(*iter)->set_enabled(obj, enabled);
		}
	}

	virtual void set_captured(myvi::gobject_t *obj, bool captured) OVERRIDE {

		for (std::vector<decorator_t*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
			(*iter)->set_captured(obj, captured);
		}
	}

	virtual void set_valid(myvi::gobject_t *obj, bool valid) OVERRIDE {

		for (std::vector<decorator_t*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
			(*iter)->set_valid(obj, valid);
		}
	}

};


class decorator_aware_t {
public:
	virtual void set_decorator(decorator_t *decorator) = 0;
};



// вид с фоном
class background_decorator_t : public decorator_t  {
public:
	bool selected;
	bool captured;
	bool disabled;
	bool invalid;

	u32  default_color;
	u32  selected_color;
	u32  captured_color;
	u32  disabled_color;
	u32  invalid_color;
public:
	background_decorator_t(gen::meta_t *meta) {

		selected = false;
		captured = false;
		disabled = false;
		invalid = false;

		default_color = 0xffffff;
		selected_color = 0xffffff;
		captured_color = 0xffffff;
		disabled_color = 0xffffff;
		invalid_color = 0xffffff;

		myvi::string_t color = meta->get_string_param("default_color");
		_MY_ASSERT(!color.is_empty(), return);

		this->default_color = view_factory_t::instance().parse_color(color);

		color = meta->get_string_param("selected_color");
		if (!color.is_empty()) {
			this->selected_color = view_factory_t::instance().parse_color(color);
		}
		color = meta->get_string_param("captured_color");
		if (!color.is_empty()) {
			this->captured_color = view_factory_t::instance().parse_color(color);
		}
		color = meta->get_string_param("disabled_color");
		if (!color.is_empty()) {
			this->disabled_color = view_factory_t::instance().parse_color(color);
		}
		color = meta->get_string_param("invalid_color");
		if (!color.is_empty()) {
			this->invalid_color = view_factory_t::instance().parse_color(color);
		}

	}

	virtual void set_selected(myvi::gobject_t *obj, bool _selected) {
		this->selected = _selected;
	}

	virtual void set_enabled(myvi::gobject_t *obj, bool _enabled) {
		this->disabled = !_enabled;
	}

	virtual void set_captured(myvi::gobject_t *obj, bool _captured) {
		this->captured = _captured;
	}

	virtual void set_valid(myvi::gobject_t *obj, bool _valid) {
		this->invalid = !_valid;
	}

	virtual void render_before(myvi::gobject_t *obj, myvi::surface_t &dst) OVERRIDE {
		s32 ax, ay;
		obj->translate(ax,ay);
		dst.ctx.reset();

		dst.ctx.pen_color = this->default_color;
		if (this->selected) {
			dst.ctx.pen_color = this->selected_color;
		}
		if (this->captured) {
			dst.ctx.pen_color = this->captured_color;
		} 
		if (this->disabled) {
			dst.ctx.pen_color = this->disabled_color;
		}
		if (this->invalid) {
			dst.ctx.pen_color = this->invalid_color;
		}
		dst.fill(ax,ay,obj->w,obj->h);
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

class box_decorator_t : public decorator_t {
	typedef decorator_t super;
public:
	u32 box_color;
public:
	box_decorator_t(gen::meta_t *meta) {
		box_color = 0x00FF00;
		myvi::string_t color = meta->get_string_param("box_color");
		if (!color.is_empty()) {
			this->box_color = view_factory_t::instance().parse_color(color);
		}
	}

	virtual void render_after(myvi::gobject_t *obj, myvi::surface_t &dst) OVERRIDE {
		s32 x,y;
		obj->translate(x, y);
		dst.ctx.reset();
		dst.ctx.pen_color = box_color;
		dst.rect(x,y,obj->w,obj->h);
	}

	virtual void adjust_preferred_size(myvi::gobject_t *obj, s32 &pw, s32 &ph) OVERRIDE {
		pw += 2;
		ph += 2;
	}

};


class decorator_aware_view_t : public myvi::gobject_t,  public decorator_aware_t {
	typedef myvi::gobject_t super;
public:
	decorator_t *decorator;
public:
	decorator_aware_view_t() {
		decorator = 0;
	}

	virtual void set_decorator(decorator_t *_drawer) OVERRIDE {
		decorator = _drawer;
	}

	virtual void set_enabled(bool enabled) {
		super::set_enabled(enabled);
		if (decorator) {
			decorator->set_enabled(this, enabled);
		}
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
		super::set_dirty_no_parent(dirty);
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


class dynamic_view_base_t : 
	public decorator_aware_view_t,
	public view_build_context_aware_t {

	typedef decorator_aware_view_t super;
private:
	gen::view_meta_t *view_meta;
	view_controller_t *view_controller;
	s32 fw, fh, min_w, min_h, max_w, max_h;
public:

	dynamic_view_base_t() {
		view_meta = 0;
		view_controller = 0; // устанавливается позже

		fw = fh = min_w = min_h = max_w = max_h = _NAN;
	}

	virtual void init(view_build_context_t ctx) OVERRIDE {
		view_meta = ctx.get_view_meta();
		view_controller = 0; // устанавливается позже

		fw = this->view_meta->get_int_param("width");
		fh = this->view_meta->get_int_param("height");
		min_w = this->view_meta->get_int_param("min_width");
		min_h = this->view_meta->get_int_param("min_height");
		max_w = this->view_meta->get_int_param("max_width");
		max_h = this->view_meta->get_int_param("max_height");
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


	virtual void adjust_preferred_size(s32 &pw, s32 &ph) OVERRIDE {

		super::adjust_preferred_size(pw, ph);

		if (fw != _NAN) {
			pw = fw;
		}
		if (fh != _NAN) {
			ph = fh;
		}

		if (min_w != _NAN && pw < min_w) {
			pw = min_w;
		}
		if (min_h != _NAN && ph < min_h) {
			ph = min_h;
		}

		if (max_w != _NAN && pw > max_w) {
			pw = max_w;
		}
		if (max_h != _NAN && ph > max_h) {
			ph = max_h;
		}

	}

};

typedef dynamic_view_base_t widget_t;

class dynamic_view_t : public dynamic_view_mixin_aware_impl_t<widget_t> {
};



class selectable_widget_t : public widget_t, public myvi::focus_client_t, public mouse_aware_t {
	typedef widget_t super;
public:


	virtual void mouse_event(myvi::mkey_t::mkey_t mkey) OVERRIDE {
		// static assert that TBase derived from focus_client_t
		bool b = this->selected;

		if (mkey == myvi::mkey_t::MK_1) {
			myvi::gobject_t *_this = dynamic_cast<myvi::gobject_t *>(this);
			_MY_ASSERT(_this, return);
			myvi::focus_manager_t::instance().select(_this);
		}
	}

	virtual void set_selected(bool selected) OVERRIDE {
		if (this->decorator) {
			this->decorator->set_selected(this,selected);
		}
		dirty = true;
	}
};



class view_controller_impl_base_t : public view_controller_t {
protected:
	volatile_path_t parameter_path;
	variant_type_t::variant_type_t parameter_type;
protected:
	view_controller_impl_base_t() {
		parameter_type = variant_type_t::STRING;
	}

	void set_initial(view_build_context_t ctx, variant_t &value) {

		myvi::string_t initial_str = ctx.get_parameter_meta()->get_string_param("initial");
		if (!initial_str.is_empty()) {
			_MY_ASSERT(parameter_type == variant_type_t::STRING, return);
			value.set_value(initial_str);
			model_t::instance()->update(ctx.get_parameter_path().path(), value);
		}

		s32 initial_int = ctx.get_parameter_meta()->get_int_param("initial");
		if (initial_int != _NAN) {
			_MY_ASSERT(parameter_type == variant_type_t::INT, return);
			value.set_value(initial_int);
			model_t::instance()->update(ctx.get_parameter_path().path(), value);
		}

		float initial_float = ctx.get_parameter_meta()->get_float_param("initial");
		if (initial_float != _NANF) {
			_MY_ASSERT(parameter_type == variant_type_t::FLOAT, return);
			value.set_value(initial_float);
			model_t::instance()->update(ctx.get_parameter_path().path(), value);
		}
	}
	
	// вызыается первым из потомков, изменяет локальную копию контекста !
	void init(view_build_context_t &ctx) {

		this->parameter_path = ctx.get_parameter_path();

		myvi::string_t parameter_path_attr = ctx.get_view_meta()->get_string_param("parameter_path");

		if (!parameter_path_attr.is_empty()) {
			meta_path_t path = meta_path_t(parameter_path_attr);
			parameter_path_resolver_t resolver(path);

			ctx.set_parameter_meta(
				resolver.resolve(ctx.try_get_parameter_meta())
			);
			this->parameter_path.add(path);
		}

		ctx.set_parameter_path(this->parameter_path);

	}

	// вызывается из контроллера texbox/combobox
	void register_in_model(view_build_context_t ctx) {

		gen::type_meta_t *type_meta = ctx.get_parameter_meta()->get_type_meta();
		_MY_ASSERT(!type_meta->is_complex(), return);

		variant_t value;
		this->parameter_type = parameter_meta_ex_t(ctx.get_parameter_meta()).match_value_type();
		value.type = parameter_type;
		model_t::instance()->try_register_path(ctx.get_parameter_path().path(), value, parameter_type);
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





#define _TEXT_PADDING 2

class label_t  : public myvi::gobject_t {
private:
	myvi::string_t _text;  // текст
public:
	myvi::wo_property_t<myvi::string_t,label_t> text;
	label_context_t ctx;
private:
	void set_text(myvi::string_t text) {
		_text = text;
		dirty = true;
	}
public:
	label_t() {
		visible = false;
		text.init(this,&label_t::set_text);
	}

	virtual void render(myvi::surface_t &dst) OVERRIDE {
	
		s32 ax = 0, ay = 0;
		_MY_ASSERT(visible && ctx.font,return);
		ctx.font->char_surface.ctx = ctx.sctx;
		ctx.font->set_char_size_px(0,ctx.font_size);

		translate(ax,ay);

		ax += _TEXT_PADDING;

		if (!_text.is_empty()) {
			// text
			ctx.font->print_to(ax,ay,dst,_text);	
		} else {
			// do nothing
		}
	}

	// used in textbox_t
	void get_text_size(s32 &aw, s32 &ah, myvi::string_t text) {
		_MY_ASSERT(ctx.font,return);
		ctx.font->set_char_size_px(0,ctx.font_size);
		ctx.font->get_string_size(text, aw, ah);
	}

	virtual void vget_preferred_size(s32 &aw, s32 &ah) OVERRIDE {
		_MY_ASSERT(visible,return);
		_MY_ASSERT(ctx.font,return);
		ctx.font->set_char_size_px(0,ctx.font_size);
		if (!_text.is_empty()) {
			// text
			ctx.font->get_string_size(this->_text, aw, ah);
		} else {
			// no content - try measure something
			myvi::string_t cs1 = myvi::string_t("1");
			ctx.font->get_string_size(cs1, aw, ah);
		}
		aw += 2 * _TEXT_PADDING;
		ah += 2 * _TEXT_PADDING;
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


class label_controller_t : 
	public view_controller_impl_base_t, 
	public myvi::subscriber_t<event_bus_msg_t>, 
	public myvi::subscriber_t<model_message_t> {

	typedef view_controller_impl_base_t super;
public:
	label_t *lab;
	myvi::string_t listen_event_name;
	volatile_string_impl_t *string_value;
	converter_t *conv;
	gen::type_meta_t *type_meta;
public:

	label_controller_t() {
		lab = 0;
		conv = 0;
		string_value = 0;
		type_meta = 0;
	}

	void set_value(variant_t value) {

		_MY_ASSERT(type_meta, return);

		if (!type_meta->is_enum()) {
			_MY_ASSERT(conv && string_value, return);
			conv->to_string(value, *string_value);
			lab->text = *string_value;

		} else {
			gen::enum_meta_t * enum_item = type_meta->get_enum_by_value(value.get_int_value());
			lab->text = enum_item->get_string_value();
		}
	}

	virtual void init(view_build_context_t ctx) OVERRIDE {
		super::init(ctx);

		lab = dynamic_cast<label_t *>(ctx.get_view());
		_MY_ASSERT(lab, return);

		this->listen_event_name = ctx.get_view_meta()->get_string_param("listen");

		if (!ctx.get_view_meta()->get_string_param("static_text").is_empty()) {
			lab->text = ctx.get_view_meta()->get_string_param("static_text");

		} else if (!ctx.get_view_meta()->get_string_param("text_source").is_empty()) {

			myvi::string_t label_source = ctx.get_view_meta()->get_string_param("text_source");
			lab->text = ctx.get_parameter_meta()->get_string_param(label_source);

		} else if (!ctx.get_view_meta()->get_string_param("dynamic_source").is_empty()) {
			if (ctx.get_view_meta()->get_string_param("dynamic_source") == "parameter_value") {

				register_in_model(ctx);
				this->type_meta = ctx.get_parameter_meta()->get_type_meta();
				model_t::instance()->subscribe(this);

				if (!this->type_meta->is_enum()) {
					conv = converter_factory_t::instance().for_type(parameter_type);
					string_value = new volatile_string_impl_t();
				}

				variant_t value;
				model_t::instance()->read(this->parameter_path.path(), value);

				set_value(value);

			} else {
				_MY_ASSERT(0, return);
			}
		} else {
			_MY_ASSERT(0, return);
		}

		if (! this->listen_event_name.is_empty()) {
			event_bus_t::instance().subscribe(this);
		}
	}

	virtual void accept(model_message_t &msg) OVERRIDE {

		if (msg.path == this->parameter_path.path()) {
			set_value(msg.value);
		}
	}

	virtual void accept(event_bus_msg_t &msg) OVERRIDE {

		_MY_ASSERT (! this->listen_event_name.is_empty(), return);

		if (this->listen_event_name == msg.event_name) {
			lab->text = msg.arg0.get_string_value();
		}
	}

};





class button_clicked_msg_t {
};


class button_t : 
	public selectable_widget_t,
	public myvi::focus_aware_t,
	public myvi::publisher_t<button_clicked_msg_t, 1> {
		typedef selectable_widget_t super;
public:
	virtual void key_event(myvi::key_t::key_t key) OVERRIDE {
		if (key == myvi::key_t::K_ENTER) {
			button_clicked_msg_t msg;
			notify(msg);
		}
	}
	virtual void mouse_event(myvi::mkey_t::mkey_t mkey) OVERRIDE {
		super::mouse_event(mkey);
		if (mkey == myvi::mkey_t::MK_1) {
			button_clicked_msg_t msg;
			notify(msg);
		}
	}
};

typedef button_t button_view_t;


class button_view_controller_t : 
	public view_controller_impl_base_t, 
	public myvi::subscriber_t<button_clicked_msg_t> {

	typedef view_controller_impl_base_t super;
public:
	button_view_t *button_view;
	gen::meta_t *meta;
	gen::type_meta_t *type_meta;
	myvi::string_t popup_view_id;
	myvi::string_t event_id;
	bool close;
public:
	button_view_controller_t() {
		button_view = 0;
		meta = 0;
		close = false;
		type_meta = 0;
	}

	/*
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
	*/

	virtual void init(view_build_context_t ctx) OVERRIDE {
		super::init(ctx);

		this->button_view = dynamic_cast<button_view_t *>(ctx.get_view());
		_MY_ASSERT(this->button_view, return);
		button_view->subscribe(this);

		this->meta = ctx.get_view_meta();
		_MY_ASSERT(this->meta, return);

		this->type_meta = ctx.get_type_meta();

		//label_t *lab = find_label();
		//lab->text = ctx.get_view_meta()->get_string_param("name");

		this->popup_view_id = ctx.get_view_meta()->get_string_param("popup_view");
		this->event_id = meta->get_string_param("event");
		this->close = meta->get_string_param("close") == "true";
	}

	virtual void accept(button_clicked_msg_t &msg) OVERRIDE {

		if (!this->event_id.is_empty()) {
			variant_t arg0 = meta_ex_t(meta).get_variant_param("arg0");
			if (!arg0.is_empty()) {
				event_bus_msg_t msg(event_id, arg0);
				event_bus_t::instance().notify(msg);
			}

		} 
			
		if (!this->popup_view_id.is_empty()) {
			if (!this->type_meta) {
				popup_manager_t::instance().popup(this->popup_view_id);
			} else {
				view_build_context_t ctx;
				ctx.set_type_meta(this->type_meta);
				popup_manager_t::instance().popup(this->popup_view_id, ctx);
			}
		} 

		if (this->close) {
			popup_manager_t::instance().popdown();
		}

		if (this->type_meta) {
			//_LOG2("clicked: ", this->type_meta->get_id().c_str() );

		}
	}

};



#define INPUT_MAX_LEN 30

class textbox_msg_t {
public:
	enum state_t {
		EDIT, // значенеие еще редактируется
		COMPLETE // пользователь нажал Enter и мы вышли из режима редактирования
	} state;
	myvi::string_t value;
	myvi::key_t::key_t key;
public:
	textbox_msg_t(myvi::key_t::key_t _key, state_t _state, myvi::string_t _value) {
		state = _state;
		key = _key;
		value = _value;
	}
};

class text_box_t : 
	public selectable_widget_t, 
	public myvi::focus_aware_t, 
	public myvi::publisher_t<textbox_msg_t, 1> {
	typedef selectable_widget_t super;
public:
	myvi::property_t<myvi::string_t , text_box_t> value;
	label_t lab;
	bool cursor_visible;
	bool captured;
	bool allow_cursor;
	bool allow_no_capture;
	u32 cursor_color;
protected:
	myvi::string_impl_t<INPUT_MAX_LEN> _value;
	s32 cursor_pos[INPUT_MAX_LEN];
	s32 caret_pos;
	bool just_selected;
protected:
	void set_value(myvi::string_t cvalue) {
		_value=(cvalue);
		lab.text = _value;
		caret_pos = _value.length();
		measure_cursor_poses();
	}

	myvi::string_t get_value() {
		return _value;
	}

	void set_captured(bool _captured) {
		this->captured = _captured;
		if (this->decorator) {
			this->decorator->set_captured(this, _captured);
		}
	}

public:
	text_box_t() {
		value.init(this,&text_box_t::get_value, &text_box_t::set_value);
		lab.ctx.sctx.pen_color = 0x00;
		lab.visible = true;
		caret_pos = 0;
		cursor_visible = false;
		allow_cursor = true;
		captured = false;
		cursor_color = 0x000000;
		allow_no_capture = false;
		just_selected = false;

		add_child(&lab);
	}

	void measure_cursor_poses() {
		for (s32 i=1; i <= _value.length(); i++) {
			s32 lw, lh;
			lab.get_text_size(lw,lh,_value.sub(0,i));
			cursor_pos[i-1] = lw;
		}
	}

	virtual void vget_preferred_size(s32 &aw, s32 &ah) OVERRIDE {
		lab.get_preferred_size(aw, ah);
	}

	virtual void do_layout() OVERRIDE {
		lab.x = 0;
		lab.y = 0;
		lab.w = w; // место для курсора
		lab.h = h;
	}

	virtual void set_selected(bool selected) {
		super::set_selected(selected);
		if (selected) {
			just_selected = true;
		}
	}

	virtual void key_event(myvi::key_t::key_t key) OVERRIDE {

		s32 cur_len = _value.length();

		if (key == myvi::key_t::K_ENTER) {
			_MY_ASSERT(parent,return);
			if (!captured) {
				this->capture_focus();
				set_captured(true);
				if (allow_cursor) {
					this->cursor_visible = true;
				}
			} else {
				this->release_focus();
				set_captured(false);
				this->cursor_visible = false;
				textbox_msg_t msg(key, textbox_msg_t::COMPLETE, _value);
				notify(msg);
			}

			if (decorator) {
				decorator->set_valid(this, true);
			}

			goto lab_update_input;
		}
		// не должны редактироваться пока не перешли в активное состояние
		if (!captured && !allow_no_capture) {
			return;
		}

		if (!captured && just_selected && allow_no_capture) {
			just_selected = false;
			value = "";
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
		update();
		textbox_msg_t msg(key, textbox_msg_t::EDIT, _value);
		notify(msg);
	}

	void update() {
		lab.text = _value;
		if (this->initialized && parent) {
			parent->child_request_size_change(this);
		}
		dirty = true;
	}

	virtual void render(myvi::surface_t &dst) OVERRIDE {
		// draw vline
		if (cursor_visible) {
			s32 ax, ay;
			translate(ax,ay);

			dst.ctx.pen_color = cursor_color;
			s32 acp = 0;
			_MY_ASSERT(caret_pos <= INPUT_MAX_LEN,return);
			if (caret_pos) 
				acp += cursor_pos[caret_pos-1];

			dst.line(ax + acp, ay, lab.h,true);
		}

	}

};



class tbox_view_t : 
	public text_box_t, 
	public text_aware_t {

	typedef text_box_t super;
public:
	tbox_view_t() {
		this->allow_cursor = false;
		this->allow_no_capture = true;
	}

	virtual label_context_t & get_text_context() OVERRIDE {
		return this->lab.ctx;
	}


};

class validator_t {
public:
	virtual void init(gen::meta_t *meta) = 0;
	virtual bool validate(variant_t & value) = 0;
};

class range_validator_t : public validator_t {
public:
	variant_t lo;
	variant_t hi;
public:
	virtual void init(gen::meta_t *meta) OVERRIDE {
		lo = meta_ex_t(meta).get_variant_param("lo");
		hi = meta_ex_t(meta).get_variant_param("hi");
	}

	virtual bool validate(variant_t & value) OVERRIDE {
		return value.cmp(lo) >= 0 && value.cmp(hi) <= 0;
	}
};

class validator_factory_t {

private:
	validator_factory_t() {
	}
	static validator_factory_t _instance;
public:

	static validator_factory_t & instance() {
		return _instance;
	}

	validator_t * try_build_validator(gen::meta_t *meta) {

		myvi::string_t validator_id = meta->get_string_param("validator");
		if (validator_id.is_empty()) {
			return 0;
		}

		validator_t * validator = 0;

		if (validator_id == "range") {
			validator = new range_validator_t();
		}
		_MY_ASSERT(validator, return 0);
		validator->init(meta);

		return validator;
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
	variant_t increment;
	validator_t *validator;
public:

	tbox_controller_t() {
		tb = 0;
		validator = 0;
	}


	virtual void init(view_build_context_t ctx) OVERRIDE {
// 0. Регистрирует поле в модели , если его ещё там нет
		super::init(ctx);
		register_in_model(ctx);

		increment = meta_ex_t(ctx.get_parameter_meta()).get_variant_param("increment");
		validator = validator_factory_t::instance().try_build_validator(ctx.get_parameter_meta());
// 1. Заполняет поле начальным значением из модели

		variant_t value;
		model_t::instance()->read(this->parameter_path.path(), value, parameter_type);

		this->tb = dynamic_cast<text_box_t *>(ctx.get_view());
		_MY_ASSERT(tb, return);

		converter_t *conv = converter_factory_t::instance().for_type(parameter_type);
		conv->to_string(value, string_value);
		this->tb->value = string_value;

// 2. Подписывается на изменения в модели и обновления поля
		model_t::instance()->subscribe(this);

// 3. Подписывается на изменения поля и обновления модели
		tb->subscribe(this);
	}

	// обновление от модели
	virtual void accept(model_message_t &msg) OVERRIDE {

		if (!(msg.path == this->parameter_path.path())) {
			return;
		}
		_MY_ASSERT(msg.value.type == this->parameter_type, return);
		update(msg.value);
	}

	void update(variant_t & value) {
		converter_t *conv = converter_factory_t::instance().for_type(this->parameter_type);
		conv->to_string(value, string_value);
		this->tb->value = string_value;
		this->tb->update();
	}

	// обновление от виджета
	virtual void accept(textbox_msg_t &msg) OVERRIDE {
		if (!msg.value.is_empty()) {

			converter_t *conv = converter_factory_t::instance().for_type(this->parameter_type);

			variant_t value;
			bool ret = conv->from_string(msg.value, value);
			if (ret) {
				if (!increment.is_empty()) {
					variant_t tmp = value;
					if (msg.key == myvi::key_t::K_LEFT || msg.key == myvi::key_t::K_DOWN) {
						tmp.dec(increment);

					} else if (msg.key == myvi::key_t::K_RIGHT || msg.key == myvi::key_t::K_UP) {
						tmp.inc(increment);
					}
					if (validator) {
						bool valid = validator->validate(tmp);
						if (tb->decorator) {
							tb->decorator->set_valid(tb, valid);
						}
						if (valid) {
							value = tmp;
						}
					} else {
						value = tmp;
					}
					update(value);
				}

				model_t::instance()->update(this->parameter_path.path(), value);
			}
		}
	}


};


typedef gen::combobox_item_proto_t combobox_item_t;

// метка с функцией выбора из списка значений
class combo_box_t : 
	public selectable_widget_t, 
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
	bool captured;
private:
	combobox_item_t * _value;
	myvi::string_impl_t<INPUT_MAX_LEN> _str_value;
	myvi::iterator_t<combobox_item_t> *_values;
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
		sprev = value;
		lab.text = _value->get_string_value();
	}

	combobox_item_t * get_value() {
		return _value;
	}

	void set_captured(bool _captured) {
		this->captured = _captured;
		if (this->decorator) {
			this->decorator->set_captured(this, _captured);
		}
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
				set_captured(true);
			} else {
				this->release_focus();
				set_captured(false);
				notify(_value);
			}
			dirty = true;
			return;
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

	virtual void init(view_build_context_t ctx) OVERRIDE  {
		super::init(ctx);
// 0. Регистрирует поле в модели , если его ещё там нет
		register_in_model(ctx);

// 1. Заполняет поле начальным значением из модели

		variant_t value;
		model_t::instance()->read(this->parameter_path.path(), value, this->parameter_type);

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
				this->cb->value = p;
				this->cb->dirty = true;
				return;
			}
			p = values->next(p);
		}
		_WEAK_ASSERT(0, return);
	}

	// обновление от модели
	virtual void accept(model_message_t &msg) OVERRIDE {

		if (!(msg.path == this->parameter_path.path())) {
			return;
		}
		variant_t value = msg.value;
		_MY_ASSERT(value.type == this->parameter_type, return);

		set_value(value.get_int_value());
		
	}

	// обновление от виджета
	virtual void accept(combobox_item_t * &msg) OVERRIDE {

		if (!this->cb->captured) {
			variant_t value(msg->get_int_value());
			model_t::instance()->update(this->parameter_path.path(), value);
		}
	}
};



// контроллер вида меню, по menuRef получает мету меню, и достраивает вид, потом обрабытвает события от вида
class menu_controller_helper_t  {
public:
	class iterator_t {
	public:
		virtual gen::parameter_meta_t * next() = 0;
	};
public:

	void build_menu(view_build_context_t & ctx, iterator_t &iter)  {

		myvi::string_t item_template_id = ctx.get_view_meta()->get_string_param("item_template_view");
		_MY_ASSERT(!item_template_id.is_empty(), return);

		myvi::string_t item_template_complex_id = ctx.get_view_meta()->get_string_param("item_template_view_complex");
		_MY_ASSERT(!item_template_complex_id.is_empty(), return);

		gen::view_meta_t *template_meta = gen::meta_registry_t::instance().find_view_meta(item_template_id);
		gen::view_meta_t *template_meta_complex = gen::meta_registry_t::instance().find_view_meta(item_template_complex_id);

		gen::parameter_meta_t *child_meta = iter.next();

		// здесь view - конетейнер меню
		// вставляем элементы меню в данный нам вид
		while (child_meta) {
			// создаем обёртку для вида параметра на основе шаблона
			view_build_context_t child_ctx = ctx;
			child_ctx.set_view_meta(template_meta);
			child_ctx.set_parameter_meta(child_meta);

			volatile_path_t parameter_path; 
			parameter_path.add_absolute(child_meta->get_id());
			child_ctx.set_parameter_path(parameter_path);

			gen::type_meta_t *type_meta = child_meta->get_type_meta();
			child_ctx.set_type_meta(type_meta);

			if (type_meta->is_complex() && !type_meta->is_enum()) {
				myvi::gobject_t *child_wrapper = view_meta_ex_t(template_meta_complex).build_view(child_ctx);
				ctx.get_view()->add_child(child_wrapper);
			} else {
				myvi::gobject_t *child_wrapper = view_meta_ex_t(template_meta).build_view(child_ctx);
				myvi::gobject_t *child_view = parameter_meta_ex_t(child_meta).build_menu_view(child_ctx);
				child_wrapper->add_child(child_view);
				ctx.get_view()->add_child(child_wrapper);
			}
			child_meta = iter.next();
		}

	}
};


// контроллер вида меню, по menuRef получает мету меню, и достраивает вид, потом обрабытвает события от вида
class menu_controller_t : public view_controller_t {
protected:
	class menu_iterator_t : public menu_controller_helper_t::iterator_t {
	public:
		menu_controller_t *that;
		s32 lasti;
	public:
		menu_iterator_t(menu_controller_t *_that) {
			that = _that;
			lasti = 0;
		}
		virtual gen::parameter_meta_t * next() OVERRIDE {
			myvi::string_t child_id = that->menu_meta->get_parameter_child(lasti++);
			if (child_id.is_empty()) return 0;
			gen::parameter_meta_t *child_meta = gen::meta_registry_t::instance().find_parameter_meta(child_id);
			return child_meta;
		}
	};
public:
	
	gen::menu_meta_t *menu_meta;
	menu_controller_helper_t helper;
public:
	menu_controller_t() {
		menu_meta = 0;
	}

	virtual void init(view_build_context_t ctx) OVERRIDE {
		myvi::string_t menu_id = ctx.get_view_meta()->get_string_param("menu_ref");
		_MY_ASSERT(!menu_id.is_empty(), return);
		this->menu_meta = gen::meta_registry_t::instance().find_menu_meta(menu_id);


		menu_iterator_t iter(this);
		helper.build_menu(ctx, iter );

	}

};


/*
*	контроллер меню в сплывающем окне.
*	строит меню по ctx.type_meta
*/

class popup_menu_controller_t : public view_controller_t {
protected:
	class menu_iterator_t : public menu_controller_helper_t::iterator_t {
	public:
		popup_menu_controller_t *that;
		s32 lasti;
	public:
		menu_iterator_t(popup_menu_controller_t *_that) {
			that = _that;
			lasti = 0;
		}
		virtual gen::parameter_meta_t * next() OVERRIDE {
			return that->type_meta->get_parameter_child(lasti++);
		}
	};
public:
	gen::type_meta_t *type_meta;
	menu_controller_helper_t helper;
	myvi::string_t name_event_name;
public:
	popup_menu_controller_t() {
		type_meta = 0;
	}

	virtual void init(view_build_context_t ctx) OVERRIDE {

		this->type_meta = ctx.get_type_meta();
		_MY_ASSERT(type_meta && type_meta->is_complex() && !type_meta->is_enum(), return);

		this->name_event_name = ctx.get_view_meta()->get_string_param("name_event");

		if (!name_event_name.is_empty()) {
			event_bus_msg_t msg(name_event_name, variant_t(type_meta->get_name()));
			event_bus_t::instance().notify(msg);
		}

		menu_iterator_t iter(this);
		helper.build_menu(ctx, iter );
	}

};




// вставляет вид, создаваемый параметром, в свой вид
class parameter_view_controller_t : public view_controller_impl_base_t {
	typedef view_controller_impl_base_t super;
public:
	virtual void init(view_build_context_t ctx) OVERRIDE {

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

		virtual bool process_key(myvi::key_t::key_t key) OVERRIDE {

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

	virtual void init(view_build_context_t ctx) OVERRIDE {

		this->view = ctx.get_view();
		_MY_ASSERT(this->view, return);

		myvi::string_t initial_view_id = ctx.get_view_meta()->get_string_param("initial");
		this->event_name = ctx.get_view_meta()->get_string_param("listen");
		this->name_event_name = ctx.get_view_meta()->get_string_param("name_event");

		if (initial_view_id.is_empty()) {
			gen::view_meta_t *child_meta = ctx.get_view_meta()->get_view_child(0);
			_MY_ASSERT(child_meta, return);
			initial_view_id = child_meta->get_id();
		}

		this->initial_view = resolve_view(initial_view_id);
		_MY_ASSERT(initial_view, return);

		if (this->view->children.size() > 1) {
			keyboard_filter_chain_t::instance().add_filter(&kbd_filter);
		}
		if (!this->event_name.is_empty()) {
			event_bus_t::instance().subscribe(this);
		}

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
//		this->view->child_request_size_change(selected_child);
		if (this->view->parent) {
			if (this->view->parent->initialized) {
				this->view->parent->do_layout();
			}
		}
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
			event_bus_msg_t msg(name_event_name, name);
			event_bus_t::instance().notify(msg);
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
		if (!meta->get_string_param("stretch").is_empty()) {
			if (meta->get_string_param("stretch") == "first") {
				this->stretch = 0;

			} else if (meta->get_string_param("stretch") == "last") {
				this->stretch = 0xff;
			}
		} else if (meta->get_int_param("stretch") != _NAN) {
			this->stretch = meta->get_int_param("stretch");
		}

		if (meta->get_int_param("bw") != _NAN) {
			this->bw = meta->get_int_param("bw");
		}
		if (meta->get_int_param("bh") != _NAN) {
			this->bw = meta->get_int_param("bh");
		}
	}
};

// размещает внутренний вид справа \ снизу родительского
class right_bottom_layout_t : public myvi::layout_t {
public:
	s32 padding;
	bool vertical;
public:
	right_bottom_layout_t(gen::meta_t *meta) {
		padding = 0;
		vertical = false;

		if (meta->get_int_param("padding") != _NAN) {
			padding = meta->get_int_param("padding");
		}
		if (!meta->get_string_param("vertical").is_empty()) {
			this->vertical = meta->get_string_param("vertical") == "true";
		}
	}

	virtual void get_preferred_size(myvi::gobject_t *parent, s32 &pw, s32 &ph) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		myvi::gobject_t *p = iter.next();
		_MY_ASSERT(p,return);
		p->get_preferred_size(pw,ph);

		pw += !vertical ? padding : 0;
		ph += vertical ? padding : 0;

		p = iter.next();
		_MY_ASSERT(!p,return);
	}

	virtual void layout(myvi::gobject_t *parent) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		myvi::gobject_t *p = iter.next();
		_MY_ASSERT(p,return);

		p->get_preferred_size(p->w,p->h);
		p->x = p->y = 0;

		if (vertical) {
			p->y = parent->h - p->h - padding;
			p->w = parent->w;
		} else {
			p->x = parent->w - p->w - padding;
			p->h = parent->h;
		}


		p = iter.next();
		_MY_ASSERT(!p,return);
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


class menu_meta_layout_t : public myvi::layout_t {
public:
	float percent;
public:
	menu_meta_layout_t(gen::meta_t *meta) {
		percent = 0.6;
		if (meta->get_float_param("ratio") != _NANF) {
			percent = meta->get_float_param("ratio");
		}
	}

	virtual void get_preferred_size(myvi::gobject_t *parent, s32 &pw, s32 &ph) OVERRIDE {

		myvi::gobject_t::iterator_visible_t iter = parent->iterator_visible();
		// 1st child
		myvi::gobject_t *first  = iter.next();
		_MY_ASSERT(first, return);

		s32 w1, h1;
		first->get_preferred_size(w1, h1);
		pw = (s32)(w1 / percent);
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
		first->w = (s32)(parent->w * percent);
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
