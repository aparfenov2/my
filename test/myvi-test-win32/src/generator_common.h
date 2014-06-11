#ifndef _GENERATOR_COMMON_H
#define _GENERATOR_COMMON_H

#include "widgets.h"

namespace gen {


// ���������� ���. ���� ���������� !
/*
class variant_t {
public:
public:
};
*/


// ======================================= ���� =============================================

// ������� ����� ����
class meta_t {
public:
	// ����� �� id
	bool match_id( myvi::string_t id) {
		return get_string_param("id") == id;
	}

	myvi::string_t get_name() {
		return get_string_param("name");
	}
	// ����� ��������� ����������
	virtual myvi::string_t get_string_param(myvi::string_t key) = 0;
	// ����� ��������� ����������
	virtual s32 get_int_param(myvi::string_t key) {
		return 0;
	}
	virtual float get_float_param(myvi::string_t key) {
		return 0.0;
	}
};

template<typename T>
class composite_meta_t : public meta_t {
public:
// �������� ��������� ������������ ��������� 
	virtual T * get_child(s32 i) {
		return 0;
	}
};

// �������� � ���������
class parameter_meta_t : public composite_meta_t<parameter_meta_t> {
};

// �������� � ����
class type_meta_t : public composite_meta_t<parameter_meta_t> {
public:
	// �������� Enum ��� combobx-a
	virtual myvi::iterator_t<myvi::combobox_item_t> * get_combobox_iterator() {
		return 0;
	}

	// ���������� ��������� �������� �������� enum �� ��� id
/*
	s32 get_enum_value(myvi::string_t id) {
		for (s32 i=0; ;i++) {
			parameter_meta_t *child_meta = this->get_child(i);
			_MY_ASSERT(child_meta, return -1);

			if (child_meta->match_id(id)) {
				return child_meta->get_int_param("value");
			}
		}
		_MY_ASSERT(0, return -1);
		return -1;
	}
*/
};

// �������� � ����
class view_meta_t : public composite_meta_t<view_meta_t> {
public:
// ����� ������� ����. ������� ��� � ����������� � ���� ����������
	virtual myvi::gobject_t * build_view();
};

// �������� � ����
class menu_meta_t : public composite_meta_t<parameter_meta_t> {
};






// -------------- ������� ���� ------------------------------
class meta_registry_t {
private:
	static menu_meta_t * menus[];
	static parameter_meta_t * parameters[];
	static type_meta_t * types[];
	static view_meta_t * views[];
private:
	meta_registry_t() {
	}

	meta_t * find_meta(myvi::string_t id, meta_t **metas) {
		meta_t **p = metas;
		while (*p) {
			if ((*p)->match_id(id)) {
				return *p;
			}
			p++;
		}
		_MY_ASSERT(0, return 0);
		return 0;
	}

public:

	static meta_registry_t & instance() {
		static meta_registry_t instance;
		return instance;
	}

	menu_meta_t * find_menu_meta(myvi::string_t id) {
		return (menu_meta_t *) find_meta(id, (meta_t**)menus);
	}
	parameter_meta_t * find_parameter_meta(myvi::string_t id) {
		return (parameter_meta_t *) find_meta(id, (meta_t**)parameters);
	}
	type_meta_t * find_type_meta(myvi::string_t id) {
		return (type_meta_t *) find_meta(id, (meta_t**)types);
	}
	view_meta_t * find_view_meta(myvi::string_t id) {
		return (view_meta_t *) find_meta(id, (meta_t**)views);
	}
};


} // ns

// ====================================== �����������, ��� ===================================================

namespace myvi {


class model_t {
public:
	// ���������� ������
//	virtual void update_me(parameter_meta_t *parameter_meta, variant_t &value) = 0;
};

// view mixin
class composite_view_t {
public:
	// ���������� ����������� view, ��������� ��������� ��� id
	virtual  gobject_t * get_part_by_id(string_t id)  = 0; 
};

// view mixin
class meta_provider_t {
public:
	virtual gen::meta_t * get_meta() = 0;
};


class dynamic_view_t : public gobject_t, public composite_view_t, public meta_provider_t {
	gen::view_meta_t * meta;
public:

	dynamic_view_t (gen::view_meta_t * _meta) {
		meta = _meta;
	}

	virtual  gobject_t * get_part_by_id(string_t id)  OVERRIDE {
		return 0;
	}

	virtual gen::meta_t * get_meta() OVERRIDE {
		return meta;
	}
};

// ��������� ����������� ����
class view_controller_t {
public:
public:
	// ���������� � �����. ���������� RTTI ��� ����������� ������ ����, ��. view_meta_t
	virtual void init( gobject_t *view,  gen::view_meta_t *meta) = 0;
};


// ������� �������� �����. Ÿ ������ ������� ��� � ������� ��� � �����������
class view_factory_t {
public:
	static void build_child_views(gobject_t *view, gen::view_meta_t * meta) {

		for (s32 i=0; ;i++) {
			gen::view_meta_t *child_meta = meta->get_child(i);
			if (!child_meta) break;
			gobject_t *child_view = child_meta->build_view();
			view->add_child(child_view);
		}
	}

};


// ��� � �����
class background_view_t : public gobject_t {
public:
	surface_context_t ctx;
	bool hasBorder;
public:
	background_view_t() {
		hasBorder = false;
	}

	virtual void render(surface_t &dst) OVERRIDE {
		s32 ax, ay;
		translate(ax,ay);
		dst.ctx = ctx;

		if (ctx.alfa) {
			dst.fill(ax,ay,w,h);
		}
		if (hasBorder) {
			dst.rect(ax,ay,w,h);
		}
	}
};


// �������� ���������� � ������� ��������� ���������
// ������� �� �������� ���� ��������� meta_provider_t
class static_layout_t : public layout_t {
public:
	virtual void get_preferred_size(gobject_t *parent, s32 &pw, s32 &ph) OVERRIDE {
	}

	virtual void layout(gobject_t *parent) OVERRIDE {
	}
};

} // ns
#endif