#include "custom_common.h"

#define _VIEW_FACTORY_CPP
#include "custom_views.h"

#include "resources.h"

using namespace custom;

view_factory_t view_factory_t::_instance;

static dynamic_model_t dynamic_model;
model_t * model_t::_instance = &dynamic_model;

converter_factory_t converter_factory_t::_instance;
keyboard_filter_chain_t keyboard_filter_chain_t::_instance;



void view_build_context_t::set_parameter_meta(gen::parameter_meta_t *_parameter_meta) {
	parameter_meta = _parameter_meta;
	parameter_path.add_absolute(_parameter_meta->get_id());
}


static myvi::layout_t * build_layout(myvi::string_t layout_id, gen::meta_t * meta) {

	myvi::layout_t * ret = 0;

	if (layout_id == "stretch") {
		ret = new custom::stretch_meta_layout_t(meta);

	} else if (layout_id == "stack") {
		ret = new custom::stack_meta_layout_t(meta);

	} else if (layout_id == "menu_item") {
		ret = new custom::menu_meta_layout_t(meta);

	} else if (layout_id == "center") {
		ret = new custom::center_meta_layout_t(meta);
	}
	_MY_ASSERT(layout_id.is_empty() || ret, return 0);
	return ret;
}

static myvi::layout_t * build_layout(gen::meta_t * meta) {
	return build_layout(meta->get_string_param("layout"), meta);
}


static view_controller_t * build_controller(gen::view_meta_t * meta) {

	myvi::string_t controller_id = meta->get_string_param("controller");

	view_controller_t *ret = 0;
	if (controller_id == "menu") {
		ret = new custom::menu_controller_t();

	} else if (controller_id == "lab") {
		ret = new custom::label_controller_t();

	} else if (controller_id == "cbox") {
		ret = new custom::cbox_controller_t();

	} else if (controller_id == "tbox") {
		ret = new custom::tbox_controller_t();

	} else if (controller_id == "window_selector") {
		ret = new custom::window_selector_controller_t();

	} else if (controller_id == "parameter_view") {
		ret = new custom::parameter_view_controller_t();

	//} else if (controller_id == "btn") {
	//	ret = new custom::button_view_controller_t();

	}
	_MY_ASSERT(controller_id.is_empty() || ret, return 0);
	return ret;
}

// gen::parameter_meta_t * meta otional
static myvi::gobject_t * build_predefined_view(custom::view_build_context_t &ctx) {
	myvi::gobject_t *view = 0;
	myvi::string_t view_id = ctx.get_view_meta()->get_predefined_id();

	if (view_id == "cbox") {
		view = new custom::cbox_view_t(ctx.get_view_meta());

	} else if (view_id == "tbox") {
		view = new custom::tbox_view_t(ctx.get_view_meta());

	} else if (view_id == "lab") {
		view = new custom::lab_view_t(ctx);

	} else if (view_id == "scroll_window") {
		view = new custom::scroll_window_view_t();

	} else if (view_id == "btn") {
		view = new custom::button_view_t(ctx.get_view_meta());
	}
	_MY_ASSERT(view, return 0);
	return view;
}


static void combine_view_meta(gen::view_meta_t *src, gen::view_meta_t *inherited, gen::dynamic_view_meta_t *combined) {

	gen::dynamic_view_meta_t *dyn_src = dynamic_cast<gen::dynamic_view_meta_t *>(src);
	_MY_ASSERT(dyn_src, return );

	gen::dynamic_view_meta_t *dyn_inherited = dynamic_cast<gen::dynamic_view_meta_t *>(inherited);
	_MY_ASSERT(dyn_inherited, return );

	// порядок важен !
	combined->mixin_params_from(*dyn_inherited);
	combined->mixin_params_from(*dyn_src);

}

static void build_child_views_of_view(view_build_context_t ctx) {
	dynamic_view_mixin_t * dvmx = dynamic_cast<dynamic_view_mixin_t *>(ctx.get_view());

	for (s32 i=0; ;i++) {
		gen::view_meta_t *child_meta = ctx.get_view_meta()->get_view_child(i);
		if (!child_meta) break;
		myvi::gobject_t *child_view = view_meta_ex_t(child_meta).build_view(ctx);
		if (dvmx) {
			dvmx->add_child(child_view,child_meta->get_id());
		} else {
			ctx.get_view()->add_child(child_view);
		}
	}
}

class decorator_array_t : public decorator_t {
public:
	std::vector<decorator_t *> children;
public:
	decorator_array_t(gen::meta_t *meta) {
	}

	void add_child(decorator_t *child) {
		children.push_back(child);
	}

	virtual void render(myvi::gobject_t *obj, myvi::surface_t &dst, bool beforeSuper) OVERRIDE {
		for (std::vector<decorator_t*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
			(*iter)->render(obj, dst, beforeSuper);
		}
	}
};


// рисует 3d рамку вокруг контрола
class d3_decorator_t : public decorator_t {
public:
public:
	d3_decorator_t(gen::meta_t *meta) {
	}

	virtual void render(myvi::gobject_t *obj, myvi::surface_t &dst, bool beforeSuper) OVERRIDE {
		if (beforeSuper) return;

		s32 x,y, w = obj->w, h = obj->h;
		obj->translate(x, y);
		dst.ctx.alfa = 0xff;

		u32 black = 0x222625;
		u32 white = 0xC3D4DB;
		u32 gray = 0xA79FAA;

		dst.ctx.pen_color = black;
		dst.line(x,y,w,false);
		dst.line(x,y+1,w,false);
		dst.line(x,y,h,true);
		dst.line(x+1,y,h,true);

		dst.ctx.pen_color = white;
		dst.line(x,y+h-1,w,false);
		dst.line(x+w-1,y,h,true);

		dst.ctx.pen_color = gray;
		dst.line(x,y+h-2,w,false);
		dst.line(x+w-2,y,h,true);

	}
};

// рисует галочку у cbox-a
class cbox_decorator_t : public decorator_t {
public:
public:
	cbox_decorator_t(gen::meta_t *meta) {
	}

	virtual void render(myvi::gobject_t *obj, myvi::surface_t &dst, bool beforeSuper) OVERRIDE {
		if (beforeSuper) return;

		u32 black = 0x222625;
		u32 gray = 0x909986;
		u32 light_gray = 0xB3C7C6;

		s32 x,y, w = obj->w, h = obj->h;
		obj->translate(x, y);

		s32 w1 = 15;

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

		x += 2; y += 2;

		dst.ctx.pen_color = black;
		dst.line(x,y,x+6,y);
		dst.line(x,y,x+3,y+6);
		dst.line(x+6,y,x+3,y+6);
	}
};

static decorator_t * build_decorator_simple(myvi::string_t decorator_id, gen::meta_t * meta) {
	decorator_t *ret = 0;
	if (decorator_id == "background") {
		ret = new background_decorator_t(meta);

	} else if (decorator_id == "3d") {
		ret = new d3_decorator_t(meta);

	} else if (decorator_id == "cbox") {
		ret = new cbox_decorator_t(meta);
	}

	_MY_ASSERT(ret, return 0);
	return ret;
}

static decorator_t * build_decorator(myvi::string_t decorator_id, gen::meta_t * meta) {
	decorator_t *ret = 0;
	bool multiple = false;
	decorator_array_t *decorator_array = 0;

	for (s32 i=0; i < decorator_id.length(); i++) {
		if (decorator_id[i] == ',') {
			multiple = true;
			break;
		}
	}
	if (multiple) {
		decorator_array = new decorator_array_t(meta);
		s32 lasti = 0;
		for (s32 i=0; i < decorator_id.length(); i++) {
			if (decorator_id[i] == ',') {
				myvi::string_t child_id = decorator_id.sub(lasti, i-lasti);
				lasti = i+1;

				decorator_t *child = build_decorator_simple(child_id, meta);
				decorator_array->add_child(child);
			}
		}
		myvi::string_t child_id = decorator_id.sub(lasti, decorator_id.length()-lasti);
		decorator_t *child = build_decorator_simple(child_id, meta);
		decorator_array->add_child(child);
	}

	if (decorator_array) {
		ret = decorator_array;
	} else {
		ret = build_decorator_simple(decorator_id, meta);
	}
	_MY_ASSERT(ret, return 0);
	return ret;
}


// метод фабрики вида по умолчанию для составного вида
myvi::gobject_t * view_factory_t::build_view(custom::view_build_context_t ctx) {

	//if (ctx.get_view_meta()->get_id() == "menu_name_lab") {
	//	int i = 0;
	//}

	if (ctx.get_view_meta()->is_inherited()) {
		gen::view_meta_t * inherited_meta = gen::meta_registry_t::instance().find_view_meta(ctx.get_view_meta()->get_inherited_id());

		gen::dynamic_view_meta_t *dvm = dynamic_cast<gen::dynamic_view_meta_t *>(ctx.get_view_meta());
		_MY_ASSERT(dvm, return 0);

		if (dvm->is_extension_of(inherited_meta)) {

			gen::dynamic_view_meta_t *combined_meta = new gen::dynamic_view_meta_t();

			combine_view_meta(ctx.get_view_meta(),inherited_meta, combined_meta);
			ctx.set_view_meta(combined_meta);
		}
	}
	ctx.set_view(0);
	if (ctx.get_view_meta()->is_predefined()) {
		ctx.set_view(
			build_predefined_view(ctx)
			);

	} else {
		ctx.set_view (new custom::dynamic_view_t(ctx));
	}

	myvi::layout_t *layout = build_layout(ctx.get_view_meta());
	if (layout) {
		ctx.get_view()->layout = layout;
	}

	myvi::string_t drawer_id = ctx.get_view_meta()->get_string_param("decorator");
	if (!drawer_id.is_empty()) {
		decorator_aware_t *drawer_aware = dynamic_cast<decorator_aware_t *>(ctx.get_view());
		_MY_ASSERT(drawer_aware, return 0);

		drawer_aware->set_drawer(
			build_decorator(drawer_id, ctx.get_view_meta())
			);
	}


	build_child_views_of_view(ctx);

	view_controller_t * view_controller = build_controller(ctx.get_view_meta());
	if (view_controller) {
		view_controller->init(ctx);

		custom::dynamic_view_t *dv = dynamic_cast<custom::dynamic_view_t *>(ctx.get_view());
		if (dv) {
			dv->set_view_controller(view_controller);
		}
	}

	return ctx.get_view();
}


// сопоставления типа и вида по умолчанию
static myvi::string_t match_default_view_for_type (myvi::string_t type_id) {

	gen::type_meta_t *type_meta = gen::meta_registry_t::instance().find_type_meta(type_id);

	if (type_id == "u8") {
		return "tbox_cbox";

	} else if (type_meta) {
		if (type_meta->get_string_param("type") == "enum") {
			return "cbox";
		}
	}
	return 0;
}

static myvi::gobject_t * build_default_view_for_complex_type(view_build_context_t ctx) {

	myvi::string_t type_id = ctx.get_parameter_meta()->get_type_id();
	gen::view_meta_t *view_template_meta = gen::meta_registry_t::instance().find_view_meta("default_composite_template");
	ctx.set_view_meta(view_template_meta);
	myvi::gobject_t * view = view_factory_t::instance().build_view(ctx);
	// строим виды только для составных типов, виды простых типов предопределены и конструируются сразу в meta_t::build_view()
	gen::type_meta_t *type_meta = gen::meta_registry_t::instance().find_type_meta(type_id);
	_MY_ASSERT(type_meta, return 0);

	_MY_ASSERT(type_meta->is_complex(), return 0);

	for (s32 i=0; ;i++) {
		gen::parameter_meta_t *child_meta = type_meta->get_parameter_child(i);
		if (!child_meta) break;
		myvi::gobject_t *child_view = parameter_meta_ex_t(child_meta).build_menu_view(ctx);
		view->add_child(child_view);
	}
	return view;
}

// Метод фабрики вида для параметра
myvi::gobject_t * view_factory_t::build_menu_view(view_build_context_t ctx) {

	myvi::gobject_t *view = 0;
	myvi::string_t view_id = ctx.get_parameter_meta()->get_view_id();

	if (view_id.is_empty()) {
		view_id = match_default_view_for_type(ctx.get_parameter_meta()->get_type_id());
	}

	if (!view_id.is_empty()) {
		gen::view_meta_t *view_meta = gen::meta_registry_t::instance().find_view_meta(view_id);
		view = view_meta_ex_t(view_meta).build_view(ctx);
	}
//		_MY_ASSERT(view, return 0);
//		 вид может быть и не указан, тогда
//		 строим вид для составного типа
	if (!view) {
		view = build_default_view_for_complex_type(ctx);
	}

	return view;
}


static bool adjust_lasti(s32 &lasti, const myvi::string_t spath, s32 spath_length) {

	if (lasti >= spath_length) {
		return false;
	}

	if (spath[lasti] == '.') lasti++;
	_MY_ASSERT(lasti < spath_length, return false); // путь не может заканчиваться точкой
	_MY_ASSERT(spath[lasti] != '.', return false); // путь не может содержать несколько точек подряд

	return true; 
}

myvi::string_t meta_path_base_t::iterator_t::next() {

	myvi::string_t spath = * that->spath;
	s32 spath_length = spath.length();

	if (!lasti) {
		_MY_ASSERT(adjust_lasti(lasti, spath, spath_length), return 0);
	}

	if (has_next) {
		for (s32 i=lasti; i < spath_length; i++) {

			if (spath[i] == '.') { // case 2b, 2c, 3, 3a

				s32 param_length = i - lasti;

				_MY_ASSERT(param_length >= 0, return 0);

				if (param_length > 0) {
					myvi::string_t param_id = spath.sub(lasti, param_length);
					lasti = i;

					has_next = adjust_lasti(lasti, spath, spath_length);

					return param_id;
				}
			}
		}
	}
	// case 2, 3, 3a
	if (lasti < spath_length) {
		myvi::string_t param_id = spath.sub(lasti, spath_length - lasti);
		lasti = spath_length;
		return param_id;
	}

	return 0;
}


u32 view_factory_t::parse_color(myvi::string_t color) {
	if (color == "BACKGROUND_WHITE") {
		return 0xF9FFF4;
	} else if (color == "BACKGROUND_BLUE") {
		return 0x679AB9;

	} else if (color == "BACKGROUND_GRAY") {
		return 0x97A39F;

	} else if (color == "FONT_WHITE") {
		return 0xE5E9F2;
	}
	_MY_ASSERT(0, return 0);
	return 0;
}

static myvi::font_size_t::font_size_t parse_font_size(myvi::string_t font_size_id) {
	if (font_size_id == "FS_8") {
		return myvi::font_size_t::FS_8;

	} else if (font_size_id == "FS_15") {
		return myvi::font_size_t::FS_15;

	} else if (font_size_id == "FS_20") {
		return myvi::font_size_t::FS_20;

	} else if (font_size_id == "FS_30") {
		return myvi::font_size_t::FS_30;
	}
	_MY_ASSERT(0, return (myvi::font_size_t::font_size_t) 0);
	return (myvi::font_size_t::font_size_t) 0;
}


extern resources_t res;

static myvi::ttype_font_t * resolve_font(myvi::string_t font_id) {

	if (font_id == "TTF") {
		return &res.ttf;

	} else if (font_id == "TTF_BOLD") {
		return &res.ttf_bold;

	} else if (font_id == "GLY") {
		return &res.gly;
	}
	_MY_ASSERT(0, return  0);
	return 0;
}

void view_factory_t::prepare_context(myvi::label_context_t &ret, gen::meta_t *meta) {


	myvi::string_t font_id = meta->get_string_param("font");
	if (!font_id.is_empty()) {
		ret.font = resolve_font(font_id);
	} else {
		ret.font = resolve_font("TTF");
	}

	myvi::string_t font_size_id = meta->get_string_param("fontSize");
	if (!font_size_id.is_empty()) {
		ret.font_size = parse_font_size(font_size_id);
	} else {
		ret.font_size = parse_font_size("FS_20");
	}

	myvi::string_t font_color_id = meta->get_string_param("fontColor");
	if (!font_color_id.is_empty()) {
		ret.sctx.pen_color = parse_color(font_color_id);
	} else {
		ret.sctx.pen_color = 0;
	}
}

myvi::gobject_t * dynamic_view_mixin_t::resolve_path(myvi::string_t _path) {

	meta_path_t path(_path);
	meta_path_t::iterator_t iter = path.iterator();
	myvi::string_t elem = iter.next();
	dynamic_view_mixin_t *ret = this;
	while (!elem.is_empty()) {

		myvi::gobject_t * child = ret->get_child(elem);
		if (!child) return 0;

		elem = iter.next();
		if (elem.is_empty()) return child;

		ret = dynamic_cast<dynamic_view_mixin_t *>(child);
		if (!ret) return 0;
	}
	return 0;
}
