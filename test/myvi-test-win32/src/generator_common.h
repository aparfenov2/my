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

#define _NAN 0x8fffffff
#define _NANF (double)_NAN

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

// все изменяемые строки здесь и далее длиной 255
typedef myvi::string_impl_t<255> volatile_string_impl_t;


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
	typedef std::unordered_map<myvi::string_t, myvi::string_t, string_t_hash_t> smap_t;
	typedef std::unordered_map<myvi::string_t, s32, string_t_hash_t> imap_t;
	typedef std::unordered_map<myvi::string_t, double, string_t_hash_t> fmap_t;
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

// путь до обьекта мета-модели
class meta_path_base_t {
public:
	class iterator_t {
	public:
		s32 lasti;
		bool has_next;
		meta_path_base_t *that;
	public:
		iterator_t() {
			lasti = 0;
			that = 0;
			has_next = true;
		}

		myvi::string_t next() {

			myvi::string_t spath = * that->spath;
			s32 spath_length = spath.length();

			if (!lasti) {
				_MY_ASSERT(adjust_lasti(lasti, spath, spath_length), return 0);
			}

			if (has_next) {
				for (s32 i=lasti; i < spath_length; i++) {

					if (spath[i] == '.') { // case 2b, 2c, 3, 3a

						s32 param_length = i - lasti;

						_MY_ASSERT(param_length >= 0, return 0);

						if (param_length > 0) {
							myvi::string_t param_id = spath.sub(lasti, param_length);
							lasti = i;

							has_next = adjust_lasti(lasti, spath, spath_length);

							return param_id;
						}
					}
				}
			}
			// case 2, 3, 3a
			if (lasti < spath_length) {
				myvi::string_t param_id = spath.sub(lasti, spath_length - lasti);
				lasti = spath_length;
				return param_id;
			}

			return 0;
		}

	private:

		bool adjust_lasti(s32 &lasti, const myvi::string_t spath, s32 spath_length) {

			if (lasti >= spath_length) {
				return false;
			}

			if (spath[lasti] == '.') lasti++;
			_MY_ASSERT(lasti < spath_length, return false); // путь не может заканчиваться точкой
			_MY_ASSERT(spath[lasti] != '.', return false); // путь не может содержать несколько точек подряд

			return true; 
		}

	};
protected:
	myvi::string_t * spath;
private:
	iterator_t _iterator;
public:

	meta_path_base_t() {
		spath = 0;
	}

	meta_path_base_t(myvi::string_t &_spath)  {
		spath = &_spath;
		_MY_ASSERT(!_spath.is_empty(), return); // case 0
		_iterator.that = this;
	}

	bool is_relative() const {
		return (*spath)[0] == '.';
	}

	iterator_t iterator() {
		return _iterator;
	}


};




// изменяемый путь
class volatile_path_t : public meta_path_base_t {
	typedef meta_path_base_t super;
public:
	volatile_string_impl_t path;
public:
	volatile_path_t() : super(path) {
	}

	volatile_path_t( const volatile_path_t & other) {
		this->path = other.path;
		this->spath = &path;
	}


	void add_relative(myvi::string_t id) {

		_MY_ASSERT(!id.is_empty(), return);
		if (id[0] != '.') {
			path += ".";
		}
		path += id;
	}

	void add_absolute(myvi::string_t id) {
		_MY_ASSERT(!id.is_empty(), return);
		if (!path.is_empty() && id[0] != '.') {
			path += ".";
		}
		path += id;
	}
};


// неизменяемый путь
class meta_path_t : public meta_path_base_t {
	typedef meta_path_base_t super;
public:
	myvi::string_t path;
public:
	meta_path_t(myvi::string_t _path) : path(_path), super(path) {
	}

	meta_path_t(const meta_path_t & other) {
		path = other.path;
		this->spath = &path;
	}

	meta_path_t(const volatile_path_t & other) {
		path = other.path;
		this->spath = &path;
	}

};



class view_meta_t;
class parameter_meta_t;


class view_build_context_t {
private:
	myvi::gobject_t *view;
	view_meta_t *view_meta;
	parameter_meta_t *parameter_meta;
	volatile_path_t parameter_path; // полный путь до параметра
public:
	view_build_context_t() {
		view = 0;
		view_meta = 0;
		parameter_meta = 0;
	}

	myvi::gobject_t * get_view() {
		_MY_ASSERT(view, return 0);
		return view;
	}

	view_meta_t * get_view_meta() {
		_MY_ASSERT(view_meta, return 0);
		return view_meta;
	}

	parameter_meta_t * get_parameter_meta() {
		_MY_ASSERT(parameter_meta, return 0);
		return parameter_meta;
	}

	parameter_meta_t * try_get_parameter_meta() {
		return parameter_meta;
	}

	meta_path_t get_parameter_path() {
		return parameter_path;
	}

	void set_view(myvi::gobject_t *_view) {
		view = _view;
	}

	void set_view_meta(view_meta_t *_view_meta) {
		view_meta = _view_meta;
	}

	void set_parameter_meta(parameter_meta_t *_parameter_meta);

};

class type_meta_t;


namespace variant_type_t {
	typedef enum {
		STRING,
		INT,
		FLOAT
	} variant_type_t;
}


// метаинфа о параметре
class parameter_meta_t : public meta_t {
public:
	myvi::gobject_t * build_menu_view(view_build_context_t ctx);

	myvi::string_t get_type_id() {
		return this->get_string_param("type");
	}

	myvi::string_t get_view_id() {
		return this->get_string_param("view");
	}

	parameter_meta_t * find_child_meta(myvi::string_t child_id);
	type_meta_t * get_type_meta();

	variant_type_t::variant_type_t match_value_type();

};

class dynamic_parameter_meta_t : public dynamic_meta_t<dynamic_parameter_meta_t, parameter_meta_t> {
};

class enum_meta_t : public meta_t, public myvi::combobox_item_t {
public:
		virtual myvi::string_t get_string_value() OVERRIDE {
			return get_string_param("name");
		}
		s32 get_vlaue() {
			return get_int_param("value");
		}
};

class dynamic_enum_meta_t : public dynamic_meta_t<dynamic_enum_meta_t, enum_meta_t> {
};

// метаинфа о типе
class type_meta_t : public meta_t {
public:
	// итератор Enum для combobx-a
	virtual myvi::iterator_t<myvi::combobox_item_t> * get_combobox_iterator() {
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
				return child_meta->get_vlaue();
			}
		}
		_MY_ASSERT(0, return -1);
		return -1;
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

	class combobox_item_iterator_t : public myvi::iterator_t<myvi::combobox_item_t> {
	public:
		dynamic_type_meta_t *type_meta;
	public:
		virtual myvi::combobox_item_t* next(void* prev) OVERRIDE {

			bool go = !prev;

			for (s32 i=0; ;i++) {
				enum_meta_t *child_meta = type_meta->get_enum_child(i);
				if (!child_meta) {
					return 0;
				}
				if (go) {
					return static_cast<myvi::combobox_item_t *>(child_meta);
				}
				if (static_cast<myvi::combobox_item_t *>(child_meta) == prev) {
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

	virtual myvi::iterator_t<myvi::combobox_item_t> * get_combobox_iterator() OVERRIDE {
		return &combobox_iterator;
	}


};





// метаинфа о виде
class view_meta_t : public meta_t {
public:
// метод фабрики вида. Создает вид и привязывает к нему контроллер
	myvi::gobject_t * build_view(view_build_context_t ctx);
	myvi::gobject_t * build_view_no_ctx();

	virtual view_meta_t * get_view_child(s32 i) {
		return 0;
	}

	bool is_predefined() {
		return this->get_string_param("kind") == "predefined";
	}
	bool is_inherited() {
		return ! get_inherited().is_empty();
	}
	myvi::string_t get_inherited() {
		return this->get_string_param("inherited");
	}
};


class dynamic_view_meta_t : public dynamic_composite_meta_t<dynamic_view_meta_t, view_meta_t *, view_meta_t> {
private:
	typedef std::unordered_map<myvi::string_t, myvi::string_t> _str_map;
	typedef std::unordered_map<myvi::string_t, s32> _int_map;
	typedef std::unordered_map<myvi::string_t, double> _float_map;
public:

	virtual view_meta_t * get_view_child(s32 i) OVERRIDE {
		if ((u32)i >= children.size()) {
			return 0;
		}
		return children[i];
	}


	void mixin_params_from(const dynamic_view_meta_t &other) {


		for (_str_map::const_iterator it = other.string_param_map.begin(); it != other.string_param_map.end(); it++) {
			this->set_string_param((*it).first,(*it).second);
		}
		for (_int_map::const_iterator it = other.int_param_map.begin(); it != other.int_param_map.end(); it++) {
			this->set_int_param((*it).first,(*it).second);
		}
		for (_float_map::const_iterator it = other.float_param_map.begin(); it != other.float_param_map.end(); it++) {
			this->set_float_param((*it).first,(*it).second);
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
private:
	std::vector<menu_meta_t *> menus;
	std::vector<parameter_meta_t *> parameters;
	std::vector<type_meta_t *> types;
	std::vector<view_meta_t *> views;
private:
	meta_registry_t() {
	}

	template<typename T>
	T * find_meta(myvi::string_t id, std::vector<T*> &metas) {
		T *ret = try_find_meta(id, metas);
		_MY_ASSERT(ret, return 0);
		return ret;
	}

	template<typename T>
	T * try_find_meta(myvi::string_t id, std::vector<T*> &metas) {

		for (std::vector<T *>::const_iterator it = metas.begin(); it != metas.end(); it++) {
			if ((*it)->match_id(id)) {
				return *it;
			}
		}
		return 0;
	}

public:

	static meta_registry_t & instance() {
		static meta_registry_t instance;
		return instance;
	}

	// обязательно вызвать после инициализации кучи
	void init();

	menu_meta_t * find_menu_meta(myvi::string_t id) {
		return find_meta<menu_meta_t>(id, menus);
	}
	parameter_meta_t * find_parameter_meta(myvi::string_t id) {
		return find_meta<parameter_meta_t>(id, parameters);
	}
	type_meta_t * find_type_meta(myvi::string_t id) {
		return find_meta<type_meta_t>(id, types);
	}
	view_meta_t * find_view_meta(myvi::string_t id) {
		return find_meta<view_meta_t>(id, views);
	}
};



// ====================================== контроллеры, итп ===================================================



class drawer_t {
public:
	virtual void render(myvi::gobject_t *obj, myvi::surface_t &dst) = 0;
};





class dynamic_view_mixin_t  {
public:
	std::unordered_map<myvi::string_t, myvi::gobject_t *, string_t_hash_t> id_map;
public:
	// добавляет дочерний вид с привязкой идентификатора
	virtual void add_child(myvi::gobject_t *child, myvi::string_t id) {
		_MY_ASSERT(child, return);
		id_map[id] = child;
	}

	myvi::gobject_t *get_child(myvi::string_t id) {
		return id_map[id];
	}

	myvi::gobject_t * resolve_path(myvi::string_t _path) {

		meta_path_t path(_path);
		meta_path_t::iterator_t iter = path.iterator();
		myvi::string_t elem = iter.next();
		dynamic_view_mixin_t *ret = this;
		while (!elem.is_empty()) {

			myvi::gobject_t * child = ret->get_child(elem);
			if (!child) return 0;

			elem = iter.next();
			if (elem.is_empty()) return child;

			ret = dynamic_cast<dynamic_view_mixin_t *>(child);
			if (!ret) return 0;
		}
		return 0;
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
	virtual myvi::gobject_t * build_view(view_build_context_t ctx) = 0;
	// Метод фабрики вида для параметра
	virtual myvi::gobject_t * build_menu_view(view_build_context_t ctx) = 0;
	// фабрика отрисовщиков
	virtual drawer_t * build_drawer(myvi::string_t drawer_id, gen::meta_t * meta) = 0;

	virtual u32 parse_color(myvi::string_t color) = 0;
};



// интерфейс контроллера вида
class view_controller_t {
public:
public:
	// связывание с видом. Использует RTTI для определения класса вида, см. view_meta_t
	virtual void init(view_build_context_t &ctx) {
	}

	// dynamic_view вызывает этот метод на этапе инициализации вида, когда g-дерево полностью построено
	virtual void on_view_init() {
	}

};






} // ns myvi
#endif