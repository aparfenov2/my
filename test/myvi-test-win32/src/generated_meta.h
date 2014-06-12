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
		if (key == "dme") return "dme";
		return 0;
	}
/*
	virtual myvi::gobject_t * build_view() OVERRIDE {
		// custom view
		return new myvi::dme_view_t();
	}
*/
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

	virtual myvi::gobject_t * build_view() OVERRIDE {

		myvi::gobject_t *view = new myvi::dynamic_view_t(this);

		myvi::stack_layout_t *layout = new myvi::stack_layout_t();
		layout->vertical = true;
		view->layout = layout;

		myvi::menu_controller_t *menu_controller = new myvi::menu_controller_t();
		menu_controller->init(view, this);

		myvi::view_factory_t::build_child_views(view, this);

		return view;
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
		return 0;
	}

	virtual view_meta_t * get_child(s32 i)  OVERRIDE {
		switch(i) {
		case 0 : return &menu_view_meta;
		}
		return 0;
	}

	virtual myvi::gobject_t * build_view() OVERRIDE {

		myvi::gobject_t *view = new myvi::dynamic_view_t(this);
		myvi::stretch_layout_t *layout = new myvi::stretch_layout_t();
		view->layout = layout;

		myvi::view_factory_t::build_child_views(view, this);

		return view;
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