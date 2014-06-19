#include "view_factory.h"

using namespace gen;

gen::view_factory_impl_t view_factory_impl;
gen::view_factory_t * gen::view_factory_t::_instance;

// метод фабрики вида по умолчанию для составного вида
myvi::gobject_t * view_meta_t::build_view(parameter_meta_t *parameter_meta) {
	return gen::view_factory_t::instance()->build_view(this, parameter_meta);
}

// метод фабрики вида по умолчанию для составного параметра
myvi::gobject_t * parameter_meta_t::build_menu_view() {
	return gen::view_factory_t::instance()->build_menu_view(this);
}
