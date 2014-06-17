#include "view_factory.h"

using namespace gen;

myvi::view_factory_impl_t view_factory_impl;
myvi::view_factory_t * myvi::view_factory_t::_instance;

// метод фабрики вида по умолчанию для составного вида
myvi::gobject_t * view_meta_t::build_view() {
	return myvi::view_factory_t::instance()->build_view(this);
}

// метод фабрики вида по умолчанию для составного параметра
myvi::gobject_t * parameter_meta_t::build_menu_view() {
	return myvi::view_factory_t::instance()->build_menu_view(this);
}
