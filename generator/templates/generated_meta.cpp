#include "generated_meta.h"

using namespace gen;

// types
dme_t_type_meta_t dme_t_type_meta;
dme_sfx_t_type_meta_t dme_sfx_t_type_meta;

type_meta_t * meta_registry_t::types[] = {
	&dme_t_type_meta,
	&dme_sfx_t_type_meta,
	0
};

// parameters
dme_parameter_meta_t dme_parameter_meta;

parameter_meta_t * meta_registry_t::parameters[] = {
	&dme_parameter_meta,
	0
};


// views

root_view_meta_t root_view_meta;
dme_view_meta_t dme_view_meta;
tbox_view_meta_t tbox_view_meta;
cbox_view_meta_t cbox_view_meta;
tbox_cbox_view_meta_t tbox_cbox_view_meta;
tbox_label_view_meta_t tbox_label_view_meta;

view_meta_t * meta_registry_t::views[] = {
	&root_view_meta,
	&dme_view_meta,
	&tbox_view_meta,
	&cbox_view_meta,
	&tbox_cbox_view_meta,
	&tbox_label_view_meta,
	0
};

// menus
menu_menu_meta_t menu_menu_meta;

menu_meta_t * meta_registry_t::menus[] = {
	&menu_menu_meta,
	0
};

