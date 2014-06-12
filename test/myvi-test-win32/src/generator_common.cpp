#include "generator_common.h"

using namespace gen;

myvi::gobject_t * view_meta_t::build_view() {
	myvi::gobject_t *view = new myvi::dynamic_view_t(this);
	myvi::view_factory_t::build_child_views(view, this);
	return view;
}

myvi::gobject_t * menu_meta_t::build_view() {
	myvi::gobject_t *view = new myvi::dynamic_view_t(this);
	myvi::view_factory_t::build_child_views(view, this);
	return view;
}

myvi::gobject_t * parameter_meta_t::build_view() {
	myvi::gobject_t *view = new myvi::dynamic_view_t(this);
	myvi::view_factory_t::build_child_views(view, this);
	return view;
}
