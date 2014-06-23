#include "generated_meta.h"

using namespace gen;

void meta_registry_t::init() {

/*
 * =================== ÒÈÏÛ ==========================
*/
	// dme_t
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","dme_t")
			->set_string_param("name","Ïàğàìåòğû DME")
			->set_string_param("type","complex")
			->add_parameter(
				(new dynamic_parameter_meta_t())
					->set_string_param("id","ch")
					->set_string_param("name","Êàíàë DME")
					->set_string_param("type","u8")
					->set_string_param("validators","range")
					->set_int_param("lo",-127)
					->set_int_param("hi",128)
			)
			->add_parameter(
				(new dynamic_parameter_meta_t())
					->set_string_param("id","sfx")
					->set_string_param("name","Ñóôôèêñ DME")
					->set_string_param("type","dme_sfx_t")
			)
		);

	// dme_sfx_t
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","dme_sfx_t")
			->set_string_param("name","Ñóôôèêñ DME")
			->set_string_param("type","enum")
			->add_enum_value(
				(new dynamic_enum_meta_t())
					->set_string_param("id","X")
					->set_string_param("name","X")
					->set_int_param("value",0)
			)
			->add_enum_value(
				(new dynamic_enum_meta_t())
					->set_string_param("id","Y")
					->set_string_param("name","Y")
					->set_int_param("value",1)
			)
		);

/*
 * =================== ÏÀĞÀÌÅÒĞÛ ==========================
*/
	parameters.push_back(
		(new dynamic_parameter_meta_t())
			->set_string_param("id","dme")
			->set_string_param("type","dme_t")
			->set_string_param("name","Ïàğàìåòğû DME")
			->set_string_param("view","dme")
		);
	
/*
 * =================== ÂÈÄÛ ==========================
*/

	// lab
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","lab")
			->set_string_param("kind","predefined")
		);
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","tbox")
			->set_string_param("kind","predefined")
		);
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","cbox")
			->set_string_param("kind","predefined")
		);
	// dme
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","dme")
			->set_string_param("layout","stack")
			->set_string_param("vertical","false")
			->set_string_param("controller","dme")
			->set_string_param("drawer","background")
			->set_int_param("background",0xF9FFF4)
			->add_child(
				(new dynamic_view_meta_t())
					->set_string_param("id","tbox")
					->set_string_param("inherited","tbox")
					)
			->add_child(
				(new dynamic_view_meta_t())
					->set_string_param("id","cbox")
					->set_string_param("inherited","cbox")
					)
		);
	// menu
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","menu")
			->set_string_param("menuRef","menu")
			->set_string_param("layout","stack")
			->set_string_param("vertical","true")
			->set_string_param("controller","menu")
			->set_string_param("itemTemplateView","item_template")
		);
	// item_template
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","item_template")
			->set_string_param("layout","menu")
			->set_string_param("controller","menu_item")
			->add_child(
				(new dynamic_view_meta_t())
					->set_string_param("id","lab")
					->set_string_param("inherited","lab")
			)
		);
	// default_composite_template
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","default_composite_template")
			->set_string_param("layout","stack")
			->set_string_param("vertical","true")
		);

	// root
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","root")
			->set_string_param("layout","stretch")
			->add_child(
				(new dynamic_view_meta_t())
					->set_string_param("id","menu")
					->set_string_param("inherited","menu")
				)
		);

/*
 * =================== ÌÅÍŞ ==========================
*/

	menus.push_back(
		(new dynamic_menu_meta_t())
			->set_string_param("id","menu")
			->set_string_param("name","Ãëàâíîå")
			->add_child("dme")
		);

}