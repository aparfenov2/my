#include "view_factory.h"

using namespace gen;

gen::view_factory_impl_t view_factory_impl;
gen::view_factory_t * gen::view_factory_t::_instance;

// custom views globals
custom::dynamic_model_t custom::dynamic_model_t::_instance;
custom::converter_factory_t custom::converter_factory_t::_instance;


// метод фабрики вида по умолчанию для составного вида
myvi::gobject_t * view_meta_t::build_view_no_ctx() {
	gen::view_build_context_t ctx = gen::view_build_context_t();
	ctx.set_view_meta(this);
	return build_view(ctx);
}
myvi::gobject_t * view_meta_t::build_view(gen::view_build_context_t ctx) {
	ctx.set_view_meta(this);
	return gen::view_factory_t::instance()->build_view(ctx);
}

// метод фабрики вида по умолчанию для составного параметра
myvi::gobject_t * parameter_meta_t::build_menu_view(view_build_context_t ctx) {
	ctx.set_parameter_meta(this);
	return gen::view_factory_t::instance()->build_menu_view(ctx);
}

parameter_meta_t * parameter_meta_t::find_child_meta(myvi::string_t child_id) {

	gen::type_meta_t *type_meta = get_type_meta();
	return type_meta->find_child_meta(child_id);
}

variant_type_t::variant_type_t parameter_meta_t::match_value_type() {

	type_meta_t *type_meta = this->get_type_meta();
	_MY_ASSERT(!type_meta->is_complex(), return gen::variant_type_t::STRING);

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


type_meta_t * parameter_meta_t::get_type_meta() {

	myvi::string_t type_id = this->get_type_id();
	_MY_ASSERT(!type_id.is_empty(), return 0);

	gen::type_meta_t *type_meta = gen::meta_registry_t::instance().find_type_meta(type_id);
	return type_meta;
}

void view_build_context_t::set_parameter_meta(parameter_meta_t *_parameter_meta) {
	parameter_meta = _parameter_meta;
	parameter_path.add_absolute(_parameter_meta->get_id());
}


