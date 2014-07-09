#include "custom_views.h"

using namespace custom;

// custom views globals
dynamic_model_t dynamic_model_t::_instance;
converter_factory_t converter_factory_t::_instance;
keyboard_filter_chain_t keyboard_filter_chain_t::_instance;



void view_build_context_t::set_parameter_meta(gen::parameter_meta_t *_parameter_meta) {
	parameter_meta = _parameter_meta;
	parameter_path.add_absolute(_parameter_meta->get_id());
}
