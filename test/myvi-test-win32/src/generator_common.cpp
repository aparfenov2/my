#include "view_factory.h"

using namespace gen;

gen::view_factory_impl_t view_factory_impl;
gen::view_factory_t * gen::view_factory_t::_instance;

// метод фабрики вида по умолчанию для составного вида
myvi::gobject_t * view_meta_t::build_view_no_ctx() {
	return build_view(gen::view_build_context_t(0,this, 0));
}
myvi::gobject_t * view_meta_t::build_view(gen::view_build_context_t ctx) {
	ctx.view_meta = this;
	return gen::view_factory_t::instance()->build_view(ctx);
}

// метод фабрики вида по умолчанию для составного параметра
myvi::gobject_t * parameter_meta_t::build_menu_view() {
	return gen::view_factory_t::instance()->build_menu_view(this);
}

parameter_meta_t * parameter_meta_t::find_child_meta(myvi::string_t child_id) {

	gen::type_meta_t *type_meta = get_type_meta();
	return type_meta->find_child_meta(child_id);
}

type_meta_t * parameter_meta_t::get_type_meta() {

	myvi::string_t type_id = this->get_type();
	_MY_ASSERT(!type_id.is_empty(), return 0);

	gen::type_meta_t *type_meta = gen::meta_registry_t::instance().find_type_meta(type_id);
	return type_meta;
}