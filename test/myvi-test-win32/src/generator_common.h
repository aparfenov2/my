#ifndef _GENERATOR_COMMON_H
#define _GENERATOR_COMMON_H

#include "widgets.h"

namespace gen {


// ¬ариантный тип. Ќадо избавитьс€ !
/*
class variant_t {
public:
public:
};
*/


// ======================================= меты =============================================

#define _NAN 0xffffffff

// базовый класс меты
class meta_t {
public:
	// поиск по id
	bool match_id( myvi::string_t id) {
		return get_string_param("id") == id;
	}

	myvi::string_t get_name() {
		return get_string_param("name");
	}
	// карта строковых параметров
	virtual myvi::string_t get_string_param(myvi::string_t key) = 0;
	// карта численных параметров
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
// дочерние параметры комплексного параметра 
	virtual T * get_child(s32 i) {
		return 0;
	}
};

// метаинфа о параметре
class parameter_meta_t : public composite_meta_t<parameter_meta_t> {
public:
	virtual myvi::gobject_t * build_view();
};

// метаинфа о типе
class type_meta_t : public composite_meta_t<parameter_meta_t> {
public:
	// итератор Enum дл€ combobx-a
	virtual myvi::iterator_t<myvi::combobox_item_t> * get_combobox_iterator() {
		return 0;
	}

	// возвращает численное значение элемента enum по его id
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

// метаинфа о виде
class view_meta_t : public composite_meta_t<view_meta_t> {
public:
// метод фабрики вида. —оздает вид и прив€зывает к нему контроллер
	virtual myvi::gobject_t * build_view();
};

// матаинфа о меню
class menu_meta_t : public composite_meta_t<parameter_meta_t> {
public:
	virtual myvi::gobject_t * build_view();
};






// -------------- регистр меты ------------------------------
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

// ====================================== контроллеры, итп ===================================================

namespace myvi {


class model_t {
public:
	// обновление модели
//	virtual void update_me(parameter_meta_t *parameter_meta, variant_t &value) = 0;
};


// view mixin
class meta_provider_t {
public:
	virtual gen::meta_t * get_meta() = 0;
};


class dynamic_view_t : public gobject_t, public meta_provider_t {
	gen::meta_t * meta;
public:

	dynamic_view_t (gen::meta_t * _meta) {
		meta = _meta;
	}

	virtual gen::meta_t * get_meta() OVERRIDE {
		return meta;
	}
};

// интерфейс контроллера вида
class view_controller_t {
public:
public:
	// св€зывание с видом. »спользует RTTI дл€ определени€ класса вида, см. view_meta_t
	virtual void init( gobject_t *view,  gen::view_meta_t *meta) = 0;
};


// фабрика дочерних видов. ≈Є задача создать вид и св€зать его с контроллерм
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

	static void build_child_views(gobject_t *view, gen::menu_meta_t * meta) {

		for (s32 i=0; ;i++) {
			gen::parameter_meta_t *child_meta = meta->get_child(i);
			if (!child_meta) break;
			gobject_t *child_view = child_meta->build_view();
			view->add_child(child_view);
		}
	}

	static void build_child_views(gobject_t *view, gen::parameter_meta_t * meta) {

		string_t type_id = meta->get_string_param("type");

		// если тип определен отдельно - строим по типу
		if (!type_id.is_empty()) {
			gen::type_meta_t *type_meta = gen::meta_registry_t::instance().find_type_meta(type_id);
			// строим виды только дл€ составных типов, виды простых типов предопределены и конструируютс€ сразу в meta_t::build_view()
			_MY_ASSERT(type_meta, return);

			string_t type_of_type = type_meta->get_string_param("type");
			_MY_ASSERT(type_of_type == "complex", return);

			build_type_child_views(view, type_meta);

		} else {
			// строим по inline - типу
			build_type_child_views(view, meta);
		}
	}

private:
	static void build_type_child_views(gobject_t *view, gen::composite_meta_t<gen::parameter_meta_t> * meta) {
		for (s32 i=0; ;i++) {
			gen::parameter_meta_t *child_meta = meta->get_child(i);
			if (!child_meta) break;
			gobject_t *child_view = child_meta->build_view();
			view->add_child(child_view);
		}
	}


};


// вид с фоном
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


// менеджер размещени€ с заранее заданными размерами
// ожидает от инстанса вида поддержки meta_provider_t
class absolute_layout_t : public layout_t {
private:
	s32 check_w(gobject_t *parent, gobject_t *child, s32 w) {
		_MY_ASSERT(w != _NAN, return _NAN);

		if (child->x + w > parent->w) w = parent->w - child->x;
		if (w <= 0) {
			w = _NAN;
		}
		return w;
	}
	s32 check_h(gobject_t *parent, gobject_t *child, s32 h) {
		_MY_ASSERT(h != _NAN, return _NAN);

		if (child->y + h > parent->h) h = parent->h - child->y;
		if (h <= 0) {
			h = _NAN;
		}
		return h;
	}

public:
	virtual void get_preferred_size(gobject_t *parent, s32 &pw, s32 &ph) OVERRIDE {

		gobject_t::iterator_visible_t iter = parent->iterator_visible();
		gobject_t * child = iter.next();

		pw = 0;
		ph = 0;

		while (child) {
			meta_provider_t *meta_provider = dynamic_cast<meta_provider_t *>(child);
			if (meta_provider) {
				gen::meta_t *meta = meta_provider->get_meta();
				s32 x = meta->get_int_param("x");
				_WEAK_ASSERT(x != _NAN, continue);
				s32 y = meta->get_int_param("y");
				_WEAK_ASSERT(y != _NAN, continue);
				s32 w = meta->get_int_param("w");
				_WEAK_ASSERT(w != _NAN, continue);
				s32 h = meta->get_int_param("h");
				_WEAK_ASSERT(h != _NAN, continue);

				if (pw < x+w) pw = x+w;
				if (ph < y+h) ph = y+h;
			}
			child = iter.next();
		}
		
	}

	virtual void layout(gobject_t *parent) OVERRIDE {

		gobject_t::iterator_visible_t iter = parent->iterator_visible();
		gobject_t * child = iter.next();

		while (child) {
			meta_provider_t *meta_provider = dynamic_cast<meta_provider_t *>(child);
			if (meta_provider) {
				gen::meta_t *meta = meta_provider->get_meta();

				s32 x = meta->get_int_param("x");
				_WEAK_ASSERT(x != _NAN, continue);
				s32 y = meta->get_int_param("y");
				_WEAK_ASSERT(y != _NAN, continue);
				s32 w = meta->get_int_param("w");
				_WEAK_ASSERT(w != _NAN, continue);
				s32 h = meta->get_int_param("h");
				_WEAK_ASSERT(h != _NAN, continue);

				child->x = x;
				child->y = y;
				child->w = check_w(parent, child, w);
				child->h = check_h(parent, child, h);

			}
			child = iter.next();
		}
	}
};

} // ns myvi
#endif