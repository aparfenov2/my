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
};

template<typename T>
class composite_meta_t : public meta_t {
public:
// �������� ��������� ������������ ��������� 
	virtual T * get_child(s32 i)  = 0;
};

// �������� � ���������
class parameter_meta_t : public composite_meta_t<parameter_meta_t> {
};

// �������� � ����
class type_meta_t : public composite_meta_t<parameter_meta_t> {
};

// �������� � ����
class view_meta_t : public composite_meta_t<view_meta_t> {
public:
// ����� ������� ����. ������� ��� � ����������� � ���� ����������
// �� ����������, �.�. ������� ���������� �������
	//myvi::gobject_t * build_view() {
	//	return myvi::view_factory_t::build_view(this);
	//}
};

// �������� � ����
class menu_meta_t : public composite_meta_t<parameter_meta_t> {
};






// -------------- ������� ���� ------------------------------
class meta_registry_t {
private:
	static menu_meta_t * menus;
	static parameter_meta_t * parameters;
	static type_meta_t * types;
	static view_meta_t * views;
private:
	meta_registry_t() {
	}

	meta_t * find_meta(myvi::string_t id, meta_t *metas) {
		meta_t *p = metas;
		while (p) {
			if (p->match_id(id)) {
				return p;
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
		return (menu_meta_t *) find_meta(id, menus);
	}
	parameter_meta_t * find_parameter_meta(myvi::string_t id) {
		return (parameter_meta_t *) find_meta(id, parameters);
	}
	type_meta_t * find_type_meta(myvi::string_t id) {
		return (type_meta_t *) find_meta(id, types);
	}
	view_meta_t * find_view_meta(myvi::string_t id) {
		return (view_meta_t *) find_meta(id, views);
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


// ��������� ����������� ����
class view_controller_t {
public:
public:
	// ���������� � �����. ���������� RTTI ��� ����������� ������ ����, ��. view_meta_t
	virtual void init( gobject_t *view,  gen::parameter_meta_t *meta) = 0;
};


// ������� �������� �����. Ÿ ������ ������� ��� � ������� ��� � �����������
class view_factory_t {
public:
public:

/*
	// ������������ �������� ���� ����
	static void bind_child_views(parameter_meta_t * meta_root, gobject_t *view_root) {

		// ��������� ���� �������� ����������
		parameter_meta_t *child_meta = meta_root->get_children(0);
		while (child_meta) {

			gobject_t *child_view = child_meta->build_view();
			view_root->add_child(child_view);

			child_meta = meta_root->get_children(child_meta);
		}
	}
*/
	// ������ ������������ ����������� ���
	static gobject_t * build_view(gen::view_meta_t * meta) {

		gobject_t *obj = new gobject_t();

		// set layout
		//TODO: ������� �������
		//myvi::stack_layout_t *layout = new myvi::stack_layout_t();
		//obj->layout = layout;

		// set controller
		// TODO: ������� ������������, GENERATED !

		for (s32 i=0; ;i++) {
			gen::view_meta_t *child_meta = meta->get_child(i);
			if (!child_meta) break;
			gobject_t *child = build_view(child_meta);
			obj->add_child(child);
		}

		return obj;
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

} // ns
#endif