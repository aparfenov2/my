#include "view_factory.h"

using namespace gen;


parameter_meta_t * parameter_meta_t::find_child_meta(myvi::string_t child_id) {

	gen::type_meta_t *type_meta = get_type_meta();
	return type_meta->find_child_meta(child_id);
}

type_meta_t * parameter_meta_t::get_type_meta() {

	myvi::string_t type_id = this->get_type_id();
	_MY_ASSERT(!type_id.is_empty(), return 0);

	gen::type_meta_t *type_meta = gen::meta_registry_t::instance().find_type_meta(type_id);
	return type_meta;
}



