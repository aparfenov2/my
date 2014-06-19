#ifndef _GENERATOR_COMMON_H
#define _GENERATOR_COMMON_H

#include "widgets.h"
#include <unordered_map>

namespace gen {


// Вариантный тип. Надо избавиться !
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

	myvi::string_t get_id() {
		return get_string_param("id");
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


// метаинфа о параметре
class parameter_meta_t : public meta_t {
public:
	myvi::gobject_t * build_menu_view();
};

// метаинфа о типе
class type_meta_t : public meta_t {
public:
	// итератор Enum для combobx-a
	virtual myvi::iterator_t<myvi::combobox_item_t> * get_combobox_iterator() {
		return 0;
	}

	virtual myvi::string_t get_parameter_child(s32 i) {
		return 0;
	}
	virtual parameter_meta_t * get_enum_child(s32 i) {
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
class view_meta_t : public meta_t {
public:
// метод фабрики вида. Создает вид и привязывает к нему контроллер
	myvi::gobject_t * build_view(parameter_meta_t *parameter_meta = 0);

	virtual myvi::string_t get_view_child(s32 i) {
		return 0;
	}

	bool is_predefined() {
		return this->get_string_param("kind") == "predefined";
	}

};

// матаинфа о меню
class menu_meta_t : public meta_t {
public:
	virtual myvi::string_t get_parameter_child(s32 i) {
		return 0;
	}
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
		meta_t *ret = try_find_meta(id, metas);
		_MY_ASSERT(ret, return 0);
		return ret;
	}

	meta_t * try_find_meta(myvi::string_t id, meta_t **metas) {
		meta_t **p = metas;
		while (*p) {
			if ((*p)->match_id(id)) {
				return *p;
			}
			p++;
		}
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
		return (type_meta_t *) try_find_meta(id, (meta_t**)types);
	}
	view_meta_t * find_view_meta(myvi::string_t id) {
		return (view_meta_t *) find_meta(id, (meta_t**)views);
	}
};



// ====================================== контроллеры, итп ===================================================


class model_t {
public:
	// обновление модели
//	virtual void update_me(parameter_meta_t *parameter_meta, variant_t &value) = 0;
};


// C++ requirements:
// must be a function object type that is default_constructible, copy_assignable and swappable
// must have the nested types 'argument_type' and 'result_type' (std::size_t)
class string_t_hash_t : std::unary_function< const myvi::string_t&, std::size_t > {
public:
	// should not throw any exceptions
	std::size_t operator() ( const myvi::string_t& key ) const /* can add noexcept or throw() here */
	{
		std::size_t hash = 0U ;
		const std::size_t mask = 0xF0000000 ;
		for( std::string::size_type i = 0 ; i < (std::string::size_type)key.length() ; ++i ) {
			hash = ( hash << 4U ) + key[i] ;
			std::size_t x = hash & mask ;
			if( x != 0 ) hash ^= ( x >> 24 ) ;
			hash &= ~x ;
		}
		return hash;
	}
};

class dynamic_view_t : public myvi::gobject_t {
	typedef myvi::gobject_t super;
public:
	std::unordered_map<myvi::string_t, myvi::gobject_t *, string_t_hash_t> id_map;
public:
	// добавляет дочерний вид с привязкой идентификатора
	void add_child(myvi::gobject_t *child, myvi::string_t id) {
		_MY_ASSERT(child, return);
		super::add_child(child);
		id_map[id] = child;
	}

	myvi::gobject_t *get_child(myvi::string_t id) {
		return id_map[id];
	}
};

// интерфейс контроллера вида
class view_controller_t {
public:
public:
	// связывание с видом. Использует RTTI для определения класса вида, см. view_meta_t
	virtual void init( myvi::gobject_t *view,  gen::view_meta_t *meta, gen::parameter_meta_t *parameter_meta) {
	}

};





// интерфейс фабрики видов - выделен для компилируемости
class view_factory_t {
protected:
	static view_factory_t * _instance;
public:
	static view_factory_t * instance() {
		return _instance;
	}
	// called from menu_controller_t
//	virtual void append_menu_view(myvi::gobject_t *view, gen::menu_meta_t *meta) = 0;
	// метод фабрики вида по умолчанию для составного вида
	virtual myvi::gobject_t * build_view(gen::view_meta_t * meta, gen::parameter_meta_t * parameter_meta) = 0;
	// Метод фабрики вида для параметра
	virtual myvi::gobject_t * build_menu_view(gen::parameter_meta_t * meta) = 0;
};


} // ns myvi
#endif