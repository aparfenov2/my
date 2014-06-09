#ifndef _GENERATED_META_H
#define _GENERATED_META_H

#include "generator_common.h"
#include "basic.h"

/*
*  ����� UI, ��������������� � ������� ������
*/

namespace gen {

class suffixes_t : public myvi::iterator_t<myvi::string_t> {
public:
	myvi::string_t str1;
	myvi::string_t str2;
public:
	suffixes_t() {
		str1 = myvi::string_t("A");
		str2 = myvi::string_t("B");
	}

	virtual myvi::string_t* next(void* prev) OVERRIDE {
		if (!prev) return &str1;
		if (prev == &str1) return &str2;
		return 0;
	}

};

class ch_parameter_meta_t : public parameter_meta_t {
public:

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "ch";
		if (key == "name") return "����� DME";
		if (key == "type") return "u8";
		if (key == "validators") return "range";
		if (key == "lo") return "-127";
		if (key == "hi") return "128";
		return 0;
	}

	virtual parameter_meta_t * get_child(s32 i) OVERRIDE {
		return 0;
	}
};

//TODO: ��� ����������� enum ?

class dme_sfx_t_type_meta_t : public type_meta_t {
public:
	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "dme_sfx_t";
		if (key == "name") return "������� DME";
		if (key == "type") return "enum";
		return 0;
	}

	virtual parameter_meta_t * get_child(s32 i) OVERRIDE {
		return 0;
	}
};

class sfx_parameter_meta_t : public parameter_meta_t {
public:
	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "sfx";
		if (key == "name") return "������� DME";
		if (key == "type") return "dme_sfx_t";
		return 0;
	}

	virtual parameter_meta_t * get_child(s32 i) OVERRIDE {
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
		if (key == "name") return "��������� DME";
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

// ������ ���������������� ��������� �� �����
/*
<parameter id="dme" type="dme_t"  name="��������� DME" view="dme"  />
*/
class dme_parameter_meta_t : public parameter_meta_t {
public:
	// void init: ������� child-����������� � ��������� �� � view, ��������� ��  ��������������� id ����� ����
	// ���� ����� ��������������� �������� view_factory_t ��� ����������� ��������� ����

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		if (key == "id") return "dme";
		if (key == "type") return "dme_t";
		if (key == "name") return "��������� DME";
		if (key == "dme") return "dme";
		return 0;
	}

	virtual parameter_meta_t * get_child(s32 i) OVERRIDE {
		return 0;
	}

};

/*
<view id="menu" kind="generated" menuRef="menu" layout="stack" vertical="true" controller="menu" />
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

// �������� ���� ������������ ����
	virtual view_meta_t * get_child(s32 i) OVERRIDE {
		return 0;
	}

};


/*
<view id="root">
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

};


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
		if (key == "name") return "�������";
		return 0;
	}
};

} // ns
#endif