#ifndef _GENERATED_META_H
#define _GENERATED_META_H

#include "generator_common.h"
#include "basic.h"
#include "widgets.h"
#include "custom_views.h"

/*
*  Схема UI, сериализованная в целевой формат
*/

namespace gen {


class ch_parameter_meta_t : public parameter_meta_t {
public:

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "ch";
		if (key == "name") return "Канал DME";
		if (key == "type") return "u8";
		if (key == "validators") return "range";
		return 0;
	}

	virtual s32 get_int_param(myvi::string_t key) {
		if (key == "lo") return -127;
		if (key == "hi") return 128;
		return 0;
	}

};


class dme_sfx_t_type_meta_t : public type_meta_t {
public:
	class combobox_item_iterator_t : public myvi::iterator_t<myvi::combobox_item_t> {
	public:
		dme_sfx_t_type_meta_t *type_meta;
	public:
		virtual myvi::combobox_item_t* next(void* prev) OVERRIDE {

			bool go = !prev;

			for (s32 i=0; ;i++) {
				parameter_meta_t *child_meta = type_meta->get_child(i);
				if (!child_meta) {
					return 0;
				}
				if (go) {
					return dynamic_cast<myvi::combobox_item_t *>(child_meta);
				}
				if (dynamic_cast<myvi::combobox_item_t *>(child_meta) == prev) {
					go = true;
				}
			}
			return 0;
		}
	};

	class X_parameter_meta_t : public parameter_meta_t, public myvi::combobox_item_t {
	public:
		virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
			if (key == "id") return "X";
			if (key == "name") return "X";
			return 0;
		}

		virtual s32 get_int_param(myvi::string_t key) {
			if (key == "value") return 0;
			return 0;
		}

		virtual myvi::string_t get_string_value() OVERRIDE {
			return get_string_param("name");
		}
	};

	class Y_parameter_meta_t : public parameter_meta_t, public myvi::combobox_item_t  {
	public:
		virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
			if (key == "id") return "Y";
			if (key == "name") return "Y";
			return 0;
		}

		virtual s32 get_int_param(myvi::string_t key) {
			if (key == "value") return 1;
			return 0;
		}

		virtual myvi::string_t get_string_value() OVERRIDE {
			return get_string_param("name");
		}
	};

public:
	combobox_item_iterator_t combobox_iterator;
	X_parameter_meta_t X_parameter_meta;
	Y_parameter_meta_t Y_parameter_meta;
public:

	dme_sfx_t_type_meta_t() {
		combobox_iterator.type_meta = this;
	}

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "dme_sfx_t";
		if (key == "name") return "Суффикс DME";
		if (key == "type") return "enum";
		return 0;
	}

	virtual parameter_meta_t * get_child(s32 i) OVERRIDE {
		switch (i) {
		case 0 : return &X_parameter_meta;
		case 1 : return &Y_parameter_meta;
		}
		return 0;
	}

	virtual myvi::iterator_t<myvi::combobox_item_t> * get_combobox_iterator() OVERRIDE {
		return &combobox_iterator;
	}
};


class sfx_parameter_meta_t : public parameter_meta_t {
public:
	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "sfx";
		if (key == "name") return "Суффикс DME";
		if (key == "type") return "dme_sfx_t";
		return 0;
	}

};

class dme_t_type_meta_t : public type_meta_t {
public:
	ch_parameter_meta_t ch_parameter_meta;
	sfx_parameter_meta_t sfx_parameter_meta;
public:
	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "dme_t";
		if (key == "name") return "Параметры DME";
		if (key == "type") return "complex";
		return 0;
	}

	virtual parameter_meta_t * get_child(s32 i) OVERRIDE {
		switch(i) {
		case 0 : return &ch_parameter_meta;
		case 1 : return &sfx_parameter_meta;
		}
		return 0;
	}
};

// пример сериализованного параметра из схемы
/*
<parameter id="dme" type="dme_t"  name="Параметры DME" view="dme"  />
*/
class dme_parameter_meta_t : public parameter_meta_t {
public:
	// void init: создает child-контроллеры и связывает их с view, передавая им  переопределения id полей вида
	// либо может воспользоваться фабрикой view_factory_t для стандартной генерации вида

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "dme";
		if (key == "type") return "dme_t";
		if (key == "name") return "Параметры DME";
		if (key == "view") return "dme";
		return 0;
	}

};


// ---------------------- predefined views --------------------------------
/*
<view id="dme" kind="predefined"/>
*/
class dme_view_meta_t : public view_meta_t {
public:

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "dme";
		if (key == "kind") return "predefined";
		return 0;
	}

	virtual myvi::gobject_t * build_predefined_view() OVERRIDE {
		return new custom::dme_view_t();
	}

	virtual gen::view_controller_t  * build_predefined_controller() OVERRIDE {
		return new custom::dme_controller_t();
	}

};

/*
<view id="tbox" kind="predefined"/>
*/
class tbox_view_meta_t : public view_meta_t {
public:

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "tbox";
		if (key == "kind") return "predefined";
		return 0;
	}

	virtual myvi::gobject_t * build_predefined_view() OVERRIDE {
		return new custom::tbox_view_t();
	}

	virtual gen::view_controller_t  * build_predefined_controller() OVERRIDE {
		return new custom::tbox_controller_t();
	}

};

/*
<view id="cbox" kind="predefined"/>
*/
class cbox_view_meta_t : public view_meta_t {
public:

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "cbox";
		if (key == "kind") return "predefined";
		return 0;
	}

	virtual myvi::gobject_t * build_predefined_view() OVERRIDE {
		return new custom::cbox_view_t();
	}

	virtual gen::view_controller_t  * build_predefined_controller() OVERRIDE {
		return new custom::cbox_controller_t();
	}

};

/*
<view id="tbox_cbox" kind="predefined"/>
*/
class tbox_cbox_view_meta_t : public view_meta_t {
public:

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "tbox_cbox";
		if (key == "kind") return "predefined";
		return 0;
	}

	virtual myvi::gobject_t * build_predefined_view() OVERRIDE {
		return new custom::tbox_cbox_view_t();
	}

	virtual gen::view_controller_t  * build_predefined_controller() OVERRIDE {
		return new custom::tbox_cbox_controller_t();
	}

};

/*
<view id="tbox_label" kind="predefined"/>
*/
class tbox_label_view_meta_t : public view_meta_t {
public:

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "tbox_label";
		if (key == "kind") return "predefined";
		return 0;
	}

	virtual myvi::gobject_t * build_predefined_view() OVERRIDE {
		return new custom::tbox_label_view_t();
	}

	virtual gen::view_controller_t  * build_predefined_controller() OVERRIDE {
		return new custom::tbox_label_controller_t();
	}

};

/*
<view id="menu" kind="generated" menuRef="menu" layout="stack" vertical="true" x="0" y="0" w="-1" h="-1" controller="menu" />
*/
class menu_view_meta_t : public view_meta_t {
public:

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "menu";
		if (key == "menuRef") return "menu";
		if (key == "layout") return "stack";
		if (key == "vertical") return "true";
		if (key == "controller") return "menu";
		return 0;
	}

	virtual gen::view_controller_t  * build_predefined_controller() OVERRIDE {
		return new custom::menu_controller_t();
	}

	virtual myvi::layout_t * build_layout() OVERRIDE {
		return new custom::stack_meta_layout_t(this);
	}

};


/*
<view id="root" kind="generated" layout="static">
*/
class root_view_meta_t : public view_meta_t {
public:
	menu_view_meta_t menu_view_meta;
public:

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "root";
		if (key == "layout") return "stretch";
		return 0;
	}

	virtual view_meta_t * get_child(s32 i)  OVERRIDE {
		switch(i) {
		case 0 : return &menu_view_meta;
		}
		return 0;
	}

	virtual myvi::layout_t * build_layout() OVERRIDE {
		return new custom::stretch_meta_layout_t(this);
	}

};

/*
<menu id="main" name="Главное">
*/
class menu_menu_meta_t : public menu_meta_t {
public:
	virtual  parameter_meta_t * get_child(s32 i)  OVERRIDE {
		switch(i) {
		case 0 : return meta_registry_t::instance().find_parameter_meta("dme");
		}
		return 0;
	}

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "menu";
		if (key == "name") return "Главное";
		return 0;
	}
};

} // ns
#endif