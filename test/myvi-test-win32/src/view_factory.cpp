#include "view_factory.h"
#include "custom_views.h"

using namespace custom;

view_factory_t view_factory_t::_instance;

static myvi::layout_t * build_layout(gen::meta_t * meta) {

	myvi::layout_t * ret = 0;
	myvi::string_t layout_id = meta->get_string_param("layout");

	if (layout_id == "stretch") {
		ret = new custom::stretch_meta_layout_t(meta);

	} else if (layout_id == "stack") {
		ret = new custom::stack_meta_layout_t(meta);

	} else if (layout_id == "menu_item") {
		ret = new custom::menu_meta_layout_t(meta);
	}
//		_MY_ASSERT(ret, return 0);
	return ret;
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

	}
	_MY_ASSERT(controller_id.is_empty() || ret, return 0);
	return ret;
}

// gen::parameter_meta_t * meta otional
static myvi::gobject_t * build_predefined_view(custom::view_build_context_t &ctx) {
	myvi::gobject_t *view = 0;
	myvi::string_t view_id = ctx.get_view_meta()->get_id();

	if (view_id == "cbox") {
		view = new custom::cbox_view_t();

	} else if (view_id == "tbox") {
		view = new custom::tbox_view_t();

	} else if (view_id == "lab") {
		view = new custom::lab_view_t(ctx);

	} else if (view_id == "scroll_window") {
		view = new custom::scroll_window_view_t();
	}
	_MY_ASSERT(view, return 0);
	return view;
}


static void combine_view_meta(gen::view_meta_t *src, gen::view_meta_t *inherited, gen::dynamic_view_meta_t *combined) {

	gen::dynamic_view_meta_t *dyn_src = dynamic_cast<gen::dynamic_view_meta_t *>(src);
	_MY_ASSERT(dyn_src, return );

	gen::dynamic_view_meta_t *dyn_inherited = dynamic_cast<gen::dynamic_view_meta_t *>(inherited);
	_MY_ASSERT(dyn_inherited, return );

	combined->mixin_params_from(*dyn_src);
	combined->mixin_params_from(*dyn_inherited);

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


// метод фабрики вида по умолчанию для составного вида
myvi::gobject_t * view_factory_t::build_view(custom::view_build_context_t ctx) {


	if (ctx.get_view_meta()->is_inherited()) {
		gen::view_meta_t * inherited_meta = gen::meta_registry_t::instance().find_view_meta(ctx.get_view_meta()->get_inherited());

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
	myvi::gobject_t * view = view_factory_t::instance()->build_view(ctx);
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
