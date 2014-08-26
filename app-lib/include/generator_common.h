#ifndef _GENERATOR_COMMON_H
#define _GENERATOR_COMMON_H

#include "widgets.h"
#include <vector>
#include <hash_map>

namespace gen {


// Вариантный тип. Надо избавиться !
/*
class variant_t {
public:
public:
};
*/


// ======================================= меты =============================================

#define _NAN 0x8fffffff
#define _NANF (double)_NAN

/*
// C++ requirements:
// must be a function object type that is default_constructible, copy_assignable and swappable
// must have the nested types 'argument_type' and 'result_type' (std::size_t)
class string_t_hash_t : std::unary_function< const myvi::string_t&, std::size_t > {
public:
	// should not throw any exceptions
	std::size_t operator() ( const myvi::string_t& key ) const 
	{
		_MY_ASSERT(!key.is_empty(), return 0);

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
*/

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
		return _NAN;
	}
	virtual double get_float_param(myvi::string_t key) {
		return _NANF;
	}
};


template<typename TSelf, typename TBase>
class dynamic_meta_t : public TBase {
	typedef std::hash_map<myvi::string_t, myvi::string_t> smap_t;
	typedef std::hash_map<myvi::string_t, s32> imap_t;
	typedef std::hash_map<myvi::string_t, double> fmap_t;
public:
	smap_t string_param_map;
	imap_t int_param_map;
	fmap_t float_param_map;
public:
	TSelf * set_string_param(myvi::string_t id, myvi::string_t val) {
		string_param_map[id] = val;
		return (TSelf *) this;
	}

	TSelf * set_int_param(myvi::string_t id, s32 val) {
		int_param_map[id] = val;
		return (TSelf *) this;
	}

	TSelf * set_float_param(myvi::string_t id, double val) {
		float_param_map[id] = val;
		return (TSelf *) this;
	}

	virtual myvi::string_t get_string_param(myvi::string_t key) OVERRIDE {
		smap_t::iterator iter = string_param_map.find(key);
		if(iter != string_param_map.end()) return iter->second;
		return myvi::string_t();
	}

	virtual s32 get_int_param(myvi::string_t key) OVERRIDE {
		imap_t::iterator iter = int_param_map.find(key);
		if(iter != int_param_map.end()) return iter->second;
		return _NAN;
	}

	virtual double get_float_param(myvi::string_t key) OVERRIDE {
		fmap_t::iterator iter = float_param_map.find(key);
		if(iter != float_param_map.end()) return iter->second;
		return _NANF;
	}

};

template<typename TSelf, typename TItem, typename TBase>
class dynamic_composite_meta_t : public dynamic_meta_t<TSelf,TBase> {
public:
	std::vector<TItem> children;
public:
	TSelf * add_child(TItem child) {
		children.push_back(child);
		return (TSelf *) this;
	}
};


class type_meta_t;

// метаинфа о параметре
class parameter_meta_t : public meta_t {
public:

	myvi::string_t get_type_id() {
		return this->get_string_param("type");
	}

	myvi::string_t get_view_id() {
		return this->get_string_param("view");
	}

	parameter_meta_t * find_child_meta(myvi::string_t child_id);
	type_meta_t * get_type_meta();

};

class dynamic_parameter_meta_t : public dynamic_meta_t<dynamic_parameter_meta_t, parameter_meta_t> {
};

// элемент в выпадающем списке
class combobox_item_proto_t {
public:
	virtual myvi::string_t get_string_value() = 0;
	virtual s32 get_int_value() = 0;
};

class enum_meta_t : public meta_t, public combobox_item_proto_t {
public:
		virtual myvi::string_t get_string_value() OVERRIDE {
			return get_string_param("name");
		}
		virtual s32 get_int_value() OVERRIDE {
			return get_int_param("value");
		}
};

class dynamic_enum_meta_t : public dynamic_meta_t<dynamic_enum_meta_t, enum_meta_t> {
};

// метаинфа о типе
class type_meta_t : public meta_t {
public:
	// итератор Enum для combobx-a
	virtual myvi::iterator_t<combobox_item_proto_t> * get_combobox_iterator() {
		return 0;
	}

	virtual parameter_meta_t * get_parameter_child(s32 i) {
		return 0;
	}
	virtual enum_meta_t * get_enum_child(s32 i) {
		return 0;
	}

	// возвращает численное значение элемента enum по его id
	s32 get_enum_value(myvi::string_t id) {

		for (s32 i=0; ;i++) {
			enum_meta_t *child_meta = this->get_enum_child(i);
			if (!child_meta) break;

			if (child_meta->match_id(id)) {
				return child_meta->get_int_value();
			}
		}
		_MY_ASSERT(0, return -1);
		return -1;
	}

	enum_meta_t * get_enum_by_value(s32 v) {

		for (s32 i=0; ;i++) {
			enum_meta_t *child_meta = this->get_enum_child(i);
			if (!child_meta) break;

			if (child_meta->get_int_value() == v) {
				return child_meta;
			}
		}
		_MY_ASSERT(0, return 0);
		return 0;
	}

	bool is_basic() {
		return this->get_string_param("type") == "base";
	}

	bool is_complex() {
		return this->get_string_param("type") == "complex";
	}

	bool is_enum() {
		return this->get_string_param("type") == "enum";
	}

	myvi::string_t get_type_id() {
		return this->get_string_param("type");
	}

	parameter_meta_t * find_child_meta(myvi::string_t child_id) {

		_MY_ASSERT(this->is_complex(), return 0);

		for (s32 i=0; ;i++) {
			parameter_meta_t *child_meta = this->get_parameter_child(i);
			if (!child_meta) break;
			if (child_meta->match_id(child_id)) {
				return child_meta;
			}
		}
		_MY_ASSERT(0, return 0);
		return 0;
	}
};

class dynamic_type_meta_t : public dynamic_meta_t<dynamic_type_meta_t, type_meta_t> {

	class combobox_item_iterator_t : public myvi::iterator_t<combobox_item_proto_t> {
	public:
		dynamic_type_meta_t *type_meta;
	public:
		virtual combobox_item_proto_t* next(void* prev) OVERRIDE {

			bool go = !prev;

			for (s32 i=0; ;i++) {
				enum_meta_t *child_meta = type_meta->get_enum_child(i);
				if (!child_meta) {
					return 0;
				}
				if (go) {
					return static_cast<combobox_item_proto_t *>(child_meta);
				}
				if (static_cast<combobox_item_proto_t *>(child_meta) == prev) {
					go = true;
				}
			}
			return 0;
		}
	};

public:
	combobox_item_iterator_t combobox_iterator;
	std::vector<parameter_meta_t *> parameters;
	std::vector<enum_meta_t *> enums;
public:
	dynamic_type_meta_t() {
		combobox_iterator.type_meta = this;
	}

	dynamic_type_meta_t * add_parameter(parameter_meta_t * child) {
		parameters.push_back(child);
		return this;
	}

	dynamic_type_meta_t * add_enum_value(enum_meta_t * child) {
		enums.push_back(child);
		return this;
	}

	virtual parameter_meta_t * get_parameter_child(s32 i) OVERRIDE {
		if ((u32)i >= parameters.size()) {
			return 0;
		}
		return parameters[i];
	}

	virtual enum_meta_t * get_enum_child(s32 i) OVERRIDE {
		if ((u32)i >= enums.size()) {
			return 0;
		}
		return enums[i];
	}

	virtual myvi::iterator_t<combobox_item_proto_t> * get_combobox_iterator() OVERRIDE {
		return &combobox_iterator;
	}


};





// метаинфа о виде
class view_meta_t : public meta_t {
public:

	virtual view_meta_t * get_view_child(s32 i) {
		return 0;
	}

	myvi::string_t get_predefined_id() {
		return this->get_string_param("predefined");
	}

	bool is_predefined() {
		return !this->get_predefined_id().is_empty();
	}
	bool is_inherited() {
		return !get_inherited_id().is_empty();
	}
	myvi::string_t get_inherited_id() {
		return this->get_string_param("inherited");
	}
};


class dynamic_view_meta_t : public dynamic_composite_meta_t<dynamic_view_meta_t, view_meta_t *, view_meta_t> {
private:
	typedef std::hash_map<myvi::string_t, myvi::string_t> _str_map;
	typedef std::hash_map<myvi::string_t, s32> _int_map;
	typedef std::hash_map<myvi::string_t, double> _float_map;
public:

	virtual view_meta_t * get_view_child(s32 i) OVERRIDE {
		if ((u32)i >= children.size()) {
			return 0;
		}
		return children[i];
	}

	void mixin_param_from(view_meta_t *other, myvi::string_t param_id) {

		_MY_ASSERT(other, return);
		if (!other->get_string_param(param_id).is_empty()) {
			this->set_string_param(param_id, other->get_string_param(param_id));

		} else if (other->get_int_param(param_id) != _NAN) {
			this->set_int_param(param_id, other->get_int_param(param_id));

		} else if (other->get_float_param(param_id) != _NANF) {
			this->set_float_param(param_id, other->get_float_param(param_id));

		} else {
			_MY_ASSERT(0, return); // no such param
		}
	}

	void mixin_params_from( dynamic_view_meta_t &other) {


		for (_str_map::const_iterator it = other.string_param_map.begin(); it != other.string_param_map.end(); it++) {
			this->set_string_param((*it).first,(*it).second);
		}
		for (_int_map::const_iterator it = other.int_param_map.begin(); it != other.int_param_map.end(); it++) {
			this->set_int_param((*it).first,(*it).second);
		}
		for (_float_map::const_iterator it = other.float_param_map.begin(); it != other.float_param_map.end(); it++) {
			this->set_float_param((*it).first,(*it).second);
		}

		for (s32 i=0; ;i++) {
			view_meta_t *child_meta = other.get_view_child(i);
			if (!child_meta) break;
			this->add_child(child_meta);
		}
	}

	// признак того, что данная мета имеет долнительные признаки к inherited
	bool is_extension_of(view_meta_t *inherited) {
		for (_str_map::const_iterator it = this->string_param_map.begin(); it != this->string_param_map.end(); it++) {
			myvi::string_t v = inherited->get_string_param((*it).first);
			if (v.is_empty()) return true; // если такого значения нет в inherited
			if (!(v == (*it).second)) return true; // если inherited имеет другое значение
		}
		for (_int_map::const_iterator it = this->int_param_map.begin(); it != this->int_param_map.end(); it++) {
			s32 v = inherited->get_int_param((*it).first);
			if (v == _NAN) return true; // если такого значения нет в inherited
			if (v != (*it).second) return true; // если inherited имеет другое значение
		}
		for (_float_map::const_iterator it = this->float_param_map.begin(); it != this->float_param_map.end(); it++) {
			double v = inherited->get_float_param((*it).first);
			if (v == _NANF) return true; // если такого значения нет в inherited
			if (v != (*it).second) return true; // если inherited имеет другое значение
		}
		return false;
	}

};

// матаинфа о меню
class menu_meta_t : public meta_t {
public:
	virtual myvi::string_t get_parameter_child(s32 i) {
		return 0;
	}
};

class dynamic_menu_meta_t : public dynamic_composite_meta_t<dynamic_menu_meta_t, myvi::string_t, menu_meta_t> {
public:
	virtual myvi::string_t get_parameter_child(s32 i) OVERRIDE {
		if ((u32)i >= children.size()) {
			return 0;
		}
		return children[i];
	}
};




// -------------- регистр меты ------------------------------


class meta_registry_t {
public:
	std::vector<menu_meta_t *> menus;
	std::vector<parameter_meta_t *> parameters;
	std::vector<type_meta_t *> types;
	std::vector<view_meta_t *> views;
	typedef std::hash_map<myvi::string_t, u32> colors_map_t;
	colors_map_t colors;
	typedef std::hash_map<myvi::string_t, myvi::string_t> fonts_map_t;
	fonts_map_t fonts;
	myvi::string_t default_font_id;
	typedef std::hash_map<myvi::string_t, s32> font_sizes_map_t;
	font_sizes_map_t font_sizes;
	myvi::string_t default_font_size_id;

private:
	meta_registry_t() {
	}

// Херась ! Шаблоны не сработали в CCS !

	menu_meta_t * find_meta(myvi::string_t id, std::vector<menu_meta_t*> &metas) {

		for (std::vector<menu_meta_t*>::iterator it = metas.begin(); it != metas.end(); it++) {
			if ((*it)->match_id(id)) {
				return *it;
			}
		}
		_MY_ASSERT(0, return 0);
		return 0;
	}

	parameter_meta_t * find_meta(myvi::string_t id, std::vector<parameter_meta_t*> &metas) {

		for (std::vector<parameter_meta_t*>::iterator it = metas.begin(); it != metas.end(); it++) {
			if ((*it)->match_id(id)) {
				return *it;
			}
		}
		_MY_ASSERT(0, return 0);
		return 0;
	}

	type_meta_t * find_meta(myvi::string_t id, std::vector<type_meta_t*> &metas) {

		for (std::vector<type_meta_t*>::iterator it = metas.begin(); it != metas.end(); it++) {
			if ((*it)->match_id(id)) {
				return *it;
			}
		}
		_MY_ASSERT(0, return 0);
		return 0;
	}

	view_meta_t * find_meta(myvi::string_t id, std::vector<view_meta_t*> &metas) {

		for (std::vector<view_meta_t*>::iterator it = metas.begin(); it != metas.end(); it++) {
			if ((*it)->match_id(id)) {
				return *it;
			}
		}
		_MY_ASSERT(0, return 0);
		return 0;
	}


	u32 map_get(colors_map_t &map, myvi::string_t id) {
		colors_map_t::iterator iter = map.find(id);
		if(iter != map.end()) return iter->second;
		_MY_ASSERT(0, return 0);
		return 0;
	}

	u32 map_get(font_sizes_map_t &map, myvi::string_t id) {
		font_sizes_map_t::iterator iter = map.find(id);
		if(iter != map.end()) return iter->second;
		_MY_ASSERT(0, return 0);
		return 0;
	}

	myvi::string_t map_get(fonts_map_t &map, myvi::string_t id) {
		fonts_map_t::iterator iter = map.find(id);
		if(iter != map.end()) return iter->second;
		_MY_ASSERT(0, return 0);
		return 0;
	}

	static meta_registry_t *_instance;

public:


	static meta_registry_t & instance() {
		if (!_instance) {
			_instance = new meta_registry_t();
		}
		return *_instance;
	}

	// обязательно вызвать после инициализации кучи
	void init(char *xml);

	menu_meta_t * find_menu_meta(myvi::string_t id) {
		return find_meta(id, menus);
	}
	parameter_meta_t * find_parameter_meta(myvi::string_t id) {
		return find_meta(id, parameters);
	}
	type_meta_t * find_type_meta(myvi::string_t id) {
		return find_meta(id, types);
	}
	view_meta_t * find_view_meta(myvi::string_t id) {
		return find_meta(id, views);
	}

	u32 resolve_color(myvi::string_t id) {
		return map_get(colors, id);
	}

	u32 resolve_font_size(myvi::string_t id) {
		return map_get(font_sizes, id);
	}

	myvi::string_t resolve_font_name(myvi::string_t id) {
		return map_get(fonts, id);
	}

	myvi::string_t get_default_font_id() {
		_MY_ASSERT(!this->default_font_id.is_empty(), return 0);
		return this->default_font_id;
	}

	myvi::string_t get_default_font_size_id() {
		_MY_ASSERT(!this->default_font_size_id.is_empty(), return 0);
		return this->default_font_size_id;
	}
};



// ====================================== контроллеры, итп ===================================================










} // ns myvi
#endif
