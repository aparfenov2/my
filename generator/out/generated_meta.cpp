
#include "generated_meta.h"

using namespace gen;

void meta_registry_t::init() {

/*
 * =================== ТИПЫ ==========================
*/
	
	// float
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","float")
			->set_string_param("type","base")
		);
	// u8
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","u8")
			->set_string_param("type","base")
		);
	// s8
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","s8")
			->set_string_param("type","base")
		);
	// u16
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","u16")
			->set_string_param("type","base")
		);
	// s16
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","s16")
			->set_string_param("type","base")
		);
	// u32
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","u32")
			->set_string_param("type","base")
		);
	// s32
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","s32")
			->set_string_param("type","base")
		);
	// dme_sfx_t
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","dme_sfx_t")
			->set_string_param("name","Суффикс DME")
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
	// dme_t
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","dme_t")
			->set_string_param("name","Параметры DME")
			->set_string_param("type","complex")
			->add_parameter(
				(new dynamic_parameter_meta_t())
					->set_int_param("hi",128)
					->set_string_param("id","ch")
					->set_int_param("lo",-127)
					->set_string_param("name","Канал DME")
					->set_string_param("type","u8")
					->set_string_param("validators","range")
			)
			->add_parameter(
				(new dynamic_parameter_meta_t())
					->set_string_param("id","sfx")
					->set_string_param("name","Суффикс DME")
					->set_string_param("type","dme_sfx_t")
			)
		);
	// measure_ctl_t
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","measure_ctl_t")
			->set_string_param("name","Измерение")
			->set_string_param("type","enum")
			->add_enum_value(
				(new dynamic_enum_meta_t())
					->set_string_param("id","START")
					->set_string_param("name","Старт")
					->set_int_param("value",0)
			)
			->add_enum_value(
				(new dynamic_enum_meta_t())
					->set_string_param("id","STOP")
					->set_string_param("name","Стоп")
					->set_int_param("value",1)
			)
		);
	// output_mode_t
	types.push_back(
		(new dynamic_type_meta_t())
			->set_string_param("id","output_mode_t")
			->set_string_param("name","Выход")
			->set_string_param("type","enum")
			->add_enum_value(
				(new dynamic_enum_meta_t())
					->set_string_param("glyph","A")
					->set_string_param("id","ANT")
					->set_string_param("name","Антенна")
					->set_int_param("value",0)
			)
			->add_enum_value(
				(new dynamic_enum_meta_t())
					->set_string_param("glyph","B")
					->set_string_param("id","ATTEN")
					->set_string_param("name","Аттен.")
					->set_int_param("value",1)
			)
			->add_enum_value(
				(new dynamic_enum_meta_t())
					->set_string_param("glyph","C")
					->set_string_param("id","CALIB")
					->set_string_param("name","Калибр.")
					->set_int_param("value",2)
			)
		);
	
/*
 * =================== ПАРАМЕТРЫ ==========================
*/
	// dme
	parameters.push_back(
		(new dynamic_parameter_meta_t())
			->set_string_param("id","dme")
			->set_string_param("name","Параметры DME")
			->set_string_param("type","dme_t")
			->set_string_param("view","dme")
		);
	// measure_ctl
	parameters.push_back(
		(new dynamic_parameter_meta_t())
			->set_string_param("id","measure_ctl")
			->set_string_param("name","Измерение")
			->set_string_param("type","measure_ctl_t")
		);
	// output_mode
	parameters.push_back(
		(new dynamic_parameter_meta_t())
			->set_string_param("id","output_mode")
			->set_string_param("name","Выход")
			->set_string_param("type","output_mode_t")
		);
	// out_level
	parameters.push_back(
		(new dynamic_parameter_meta_t())
			->set_int_param("hi",70)
			->set_string_param("id","out_level")
			->set_string_param("label","dBm")
			->set_int_param("lo",-70)
			->set_string_param("name","Уровень")
			->set_int_param("precision",0.1)
			->set_string_param("type","float")
			->set_string_param("validators","range")
			->set_string_param("view","tbox_label")
		);
	
/*
 * =================== ВИДЫ ==========================
*/

	// dme
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("background","BACKGROUND_LIGHT")
			->set_string_param("drawer","background")
			->set_string_param("id","dme")
			->set_string_param("layout","stack")
			->set_string_param("preferred_item_size","true")
			->set_string_param("vertical","false")
		->add_child(
			(new dynamic_view_meta_t())
				->set_string_param("childParameter","ch")
				->set_string_param("controller","tbox")
				->set_string_param("id","ch")
				->set_string_param("inherited","tbox")
		)
		->add_child(
			(new dynamic_view_meta_t())
				->set_string_param("childParameter","sfx")
				->set_string_param("controller","cbox")
				->set_string_param("id","sfx")
				->set_string_param("inherited","cbox")
		)
		);
	// tbox
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","tbox")
			->set_string_param("kind","predefined")
		);
	// cbox
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","cbox")
			->set_string_param("kind","predefined")
		);
	// tbox_cbox
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","tbox_cbox")
			->set_string_param("kind","predefined")
		);
	// tbox_label
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","tbox_label")
			->set_string_param("kind","predefined")
		);
	// lab
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("id","lab")
			->set_string_param("kind","predefined")
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
			->set_string_param("kind","generated")
			->set_string_param("layout","stretch")
		->add_child(
			(new dynamic_view_meta_t())
				->set_string_param("controller","menu")
				->set_string_param("id","menu")
				->set_string_param("itemTemplateView","item_template")
				->set_string_param("kind","generated")
				->set_string_param("layout","stack")
				->set_string_param("menuRef","main")
				->set_string_param("vertical","true")
		)
		);
	// item_template
	views.push_back(
		(new dynamic_view_meta_t())
			->set_string_param("controller","menu_item")
			->set_string_param("id","item_template")
			->set_string_param("layout","menu")
		->add_child(
			(new dynamic_view_meta_t())
				->set_string_param("id","lab")
				->set_string_param("inherited","lab")
		)
		);

/*
 * =================== МЕНЮ ==========================
*/

	// main
	menus.push_back(
		(new dynamic_menu_meta_t())
			->set_string_param("id","main")
			->set_string_param("name","Главное")
		->add_child("dme")
		);

}