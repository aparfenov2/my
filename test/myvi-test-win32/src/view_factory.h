// отделена от generator_common для компилируемости
#ifndef _VIEW_FACTORY_H
#define _VIEW_FACTORY_H

#include "generator_common.h"
#include "custom_views.h"

namespace myvi {
// фабрика дочерних видов. Её задача создать вид и связать его с контроллерм
class view_factory_impl_t : public view_factory_t {
public:
	view_factory_impl_t() {
		_instance = this;
	}

	layout_t * build_layout(gen::meta_t * meta) {

		layout_t * ret = 0;
		string_t layout_id = meta->get_string_param("layout");

		if (layout_id == "stretch") {
			stretch_layout_t *layout = new stretch_layout_t();
			ret = layout;
		} else {
			// размещение по умолчанию - стек
			stack_layout_t *layout = new stack_layout_t();
			if (meta->get_string_param("vertical") == "true") {
				layout->vertical = true;
			}
			ret = layout;
		}
		return ret;
	}


	view_controller_t * build_controller(gen::view_meta_t * meta) {

		string_t controller_id = meta->get_string_param("controller");

		view_controller_t *ret = 0;
		if (controller_id == "menu") {
			ret = new menu_controller_t();
		}
		return ret;
	}


	// метод фабрики вида по умолчанию для составного вида
	gobject_t * build_view(gen::view_meta_t * meta) OVERRIDE {

		gobject_t *view = new dynamic_view_t(meta);
		view->layout = build_layout(meta);


		view_controller_t * view_controller = build_controller(meta);
		if (view_controller) {
			view_controller->init(view, meta);
		}

		build_child_views_of_view(view, meta);
		return view;
	}

	void build_child_views_of_view(gobject_t *view, gen::view_meta_t * meta) {

		for (s32 i=0; ;i++) {
			gen::view_meta_t *child_meta = meta->get_child(i);
			if (!child_meta) break;
			gobject_t *child_view = child_meta->build_view();
			view->add_child(child_view);
		}
	}


// ----------------- метод фабрики вида по умолчанию для составного параметра --------------------


	gobject_t * build_predefined_view(string_t view_id, gen::parameter_meta_t * meta) {
		gobject_t *view = 0;
		if (view_id == "tbox_cbox") {
			view = new tbox_cbox_view_t();
			tbox_cbox_controller_t *controller = new tbox_cbox_controller_t();
			controller->init(view, meta);

		} else if (view_id == "cbox") {
			view = new combo_box_t();
			cbox_controller_t *controller = new cbox_controller_t();
			controller->init(view, meta);
		}
		return view;
	}

	gobject_t * build_view_or_predefined(string_t view_id, gen::parameter_meta_t * meta) {
		gobject_t *view = build_predefined_view(view_id, meta);
		if (!view) {
			gen::view_meta_t *view_meta = gen::meta_registry_t::instance().find_view_meta(view_id);
			view = view_meta->build_view();
		}
		return view;
	}

	// сопоставления типа и вида по умолчанию
	string_t match_default_view_for_type (string_t type_id) {

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

	gobject_t * build_default_view_for_complex_type(gen::parameter_meta_t * meta, string_t type_id) {

		gobject_t * view = new myvi::dynamic_view_t(meta);
		view->layout = build_layout(meta);
		// строим виды только для составных типов, виды простых типов предопределены и конструируются сразу в meta_t::build_view()
		gen::type_meta_t *type_meta = gen::meta_registry_t::instance().find_type_meta(type_id);
		_MY_ASSERT(type_meta, return 0);

		string_t type_of_type = type_meta->get_string_param("type");
		_MY_ASSERT(type_of_type == "complex", return 0);

		build_child_views_of_composite(view, type_meta);
		return view;
	}

	// Метод фабрики вида для параметра
	gobject_t * build_menu_view(gen::parameter_meta_t * meta) OVERRIDE {

		gobject_t *view = 0;
		string_t view_id = meta->get_string_param("view");
		string_t type_id = meta->get_string_param("type");

		if (view_id.is_empty()) {
			view_id = match_default_view_for_type(type_id);
		}
		if (!view_id.is_empty()) {
			view = build_view_or_predefined(view_id, meta);
		}
		// вид может быть и не указан, тогда
		// строим вид для составного типа
		if (!view) {
			// тип всегда должен быть указан. Составных параемтров нет
			_MY_ASSERT(!type_id.is_empty(), return 0);
			view = build_default_view_for_complex_type(meta, type_id);
		}
		return view;
	}

	// called from menu_controller_t
	virtual void append_menu_view(gobject_t *view, gen::menu_meta_t *meta) OVERRIDE {
		build_child_views_of_composite(view, meta);
	}

private:
	void build_child_views_of_composite(gobject_t *view, gen::composite_meta_t<gen::parameter_meta_t> * meta) {
		for (s32 i=0; ;i++) {
			gen::parameter_meta_t *child_meta = meta->get_child(i);
			if (!child_meta) break;
			gobject_t *child_view = child_meta->build_menu_view();
			view->add_child(child_view);
		}
	}


};

} // ns myvi
#endif