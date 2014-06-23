// отделена от generator_common для компилируемости
#ifndef _VIEW_FACTORY_H
#define _VIEW_FACTORY_H

#include "generator_common.h"
#include "custom_views.h"

namespace gen {
// фабрика дочерних видов. Её задача создать вид и связать его с контроллерм
class view_factory_impl_t : public view_factory_t {
public:
	view_factory_impl_t() {
		_instance = this;
	}

	myvi::layout_t * build_layout(gen::meta_t * meta) {

		myvi::layout_t * ret = 0;
		myvi::string_t layout_id = meta->get_string_param("layout");

		if (layout_id == "stretch") {
			ret = new custom::stretch_meta_layout_t(meta);

		} else if (layout_id == "stack") {
			ret = new custom::stack_meta_layout_t(meta);

		} else if (layout_id == "menu") {
			ret = new custom::menu_meta_layout_t(meta);
		}
//		_MY_ASSERT(ret, return 0);
		return ret;
	}


	view_controller_t * build_controller(gen::view_meta_t * meta) {

		myvi::string_t controller_id = meta->get_string_param("controller");

		view_controller_t *ret = 0;
		if (controller_id == "menu") {
			ret = new custom::menu_controller_t();

		} else if (controller_id == "menu_item") {
			ret = new custom::menu_item_controller_t();

		} else if (controller_id == "dme") {
			ret = new custom::dme_controller_t();
		}
		_MY_ASSERT(controller_id.is_empty() || ret, return 0);
		return ret;
	}

	virtual gen::drawer_t * build_drawer(myvi::string_t drawer_id, gen::meta_t * meta) OVERRIDE {
		gen::drawer_t *ret = 0;
		if (drawer_id == "background") {
			ret = new custom::background_drawer_t(meta);
		}
		_MY_ASSERT(ret, return 0);
		return ret;
	}

	myvi::gobject_t * build_predefined_view(myvi::string_t view_id, gen::parameter_meta_t * meta) {
		myvi::gobject_t *view = 0;
		if (view_id == "tbox_cbox") {
			view = new custom::tbox_label_view_t();

		} else if (view_id == "tbox_label") {
			view = new custom::tbox_cbox_view_t();
		} else if (view_id == "cbox") {
			view = new custom::cbox_view_t();

		} else if (view_id == "tbox") {
			view = new custom::tbox_view_t();

		} else if (view_id == "lab") {
			view = new custom::lab_view_t();
		}
		_MY_ASSERT(view, return 0);
		return view;
	}


	// метод фабрики вида по умолчанию для составного вида
	myvi::gobject_t * build_view(gen::view_meta_t * meta, gen::parameter_meta_t * parameter_meta) OVERRIDE {

		if (meta->is_inherited()) {
			gen::view_meta_t * inherited_meta = gen::meta_registry_t::instance().find_view_meta(meta->get_inherited());
//			mixin_view_meta(meta,inherited_meta);
			meta = inherited_meta;
		}
		myvi::gobject_t *view = 0;
		if (meta->is_predefined()) {
			view = build_predefined_view(meta->get_id(), parameter_meta);

		} else {
			view = new dynamic_view_t(meta);
		}

		myvi::layout_t *layout = build_layout(meta);
		if (layout) {
			view->layout = layout;
		}
		dynamic_view_mixin_t * dvmx = dynamic_cast<dynamic_view_mixin_t *>(view);
		if (dvmx) {
			build_child_views_of_view(dvmx, meta, parameter_meta);
		}

		view_controller_t * view_controller = build_controller(meta);
		if (view_controller) {
			view_controller->init(view, meta, parameter_meta);
		}

		return view;
	}

	void build_child_views_of_view(dynamic_view_mixin_t *view, gen::view_meta_t * meta, gen::parameter_meta_t * parameter_meta) {

		for (s32 i=0; ;i++) {
			gen::view_meta_t *child_meta = meta->get_view_child(i);
			if (!child_meta) break;
			myvi::gobject_t *child_view = child_meta->build_view(parameter_meta);
			view->add_child(child_view,child_meta->get_id());
		}
	}


// ----------------- метод фабрики вида по умолчанию для составного параметра --------------------


	// сопоставления типа и вида по умолчанию
	myvi::string_t match_default_view_for_type (myvi::string_t type_id) {

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

	myvi::gobject_t * build_default_view_for_complex_type(gen::parameter_meta_t * meta, myvi::string_t type_id) {

		gen::view_meta_t *view_template_meta = gen::meta_registry_t::instance().find_view_meta("default_composite_template");
		myvi::gobject_t * view = build_view(view_template_meta, meta);
		// строим виды только для составных типов, виды простых типов предопределены и конструируются сразу в meta_t::build_view()
		gen::type_meta_t *type_meta = gen::meta_registry_t::instance().find_type_meta(type_id);
		_MY_ASSERT(type_meta, return 0);

		myvi::string_t type_of_type = type_meta->get_string_param("type");
		_MY_ASSERT(type_of_type == "complex", return 0);

		for (s32 i=0; ;i++) {
			gen::parameter_meta_t *child_meta = type_meta->get_parameter_child(i);
			if (!child_meta) break;
			myvi::gobject_t *child_view = child_meta->build_menu_view();
			view->add_child(child_view);
		}
		return view;
	}

	// Метод фабрики вида для параметра
	myvi::gobject_t * build_menu_view(gen::parameter_meta_t * meta) OVERRIDE {

		myvi::gobject_t *view = 0;
		myvi::string_t view_id = meta->get_string_param("view");
		myvi::string_t type_id = meta->get_string_param("type");

		if (view_id.is_empty()) {
			view_id = match_default_view_for_type(type_id);
		}
		if (!view_id.is_empty()) {
			gen::view_meta_t *view_meta = gen::meta_registry_t::instance().find_view_meta(view_id);
			view = view_meta->build_view(meta);
		}
//		_MY_ASSERT(view, return 0);
//		 вид может быть и не указан, тогда
//		 строим вид для составного типа
		if (!view) {
			// тип всегда должен быть указан. Составных параемтров нет
			_MY_ASSERT(!type_id.is_empty(), return 0);
			view = build_default_view_for_complex_type(meta, type_id);
		}

		return view;
	}

	virtual u32 parse_color(myvi::string_t color) OVERRIDE {
		if (color == "BACKGROUND_LIGHT") {
			return 0xF9FFF4;
		}
		_MY_ASSERT(0, return 0);
		return 0;
	}

};

} // ns myvi
#endif