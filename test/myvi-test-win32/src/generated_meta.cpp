#include "generated_meta.h"

using namespace gen;

// types
dme_t_type_meta_t dme_t_type_meta;

type_meta_t * meta_registry_t::types = {
	&dme_t_type_meta
};

// parameters
dme_parameter_meta_t dme_parameter_meta;

parameter_meta_t * meta_registry_t::parameters = {
	&dme_parameter_meta
};


// views

root_view_meta_t root_view_meta;

view_meta_t * meta_registry_t::views = {
	&root_view_meta
};

// menus
menu_menu_meta_t menu_menu_meta;

menu_meta_t * meta_registry_t::menus = {
	&menu_menu_meta
};

