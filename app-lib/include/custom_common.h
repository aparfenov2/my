// отделена от generator_common для компилируемости
#ifndef _VIEW_FACTORY_H
#define _VIEW_FACTORY_H

#include "widgets.h"
#include "generator_common.h"

namespace custom {


class keyboard_filter_t {
public:
	virtual bool processKey(myvi::key_t::key_t key) = 0;
};

// агрегатор всех фильтров клавиатуры
class keyboard_filter_chain_t {
public:
	std::vector<keyboard_filter_t *> chain;
	static keyboard_filter_chain_t _instance;
public:
	static keyboard_filter_chain_t & instance() {
		return _instance;
	}

	void add_filter(keyboard_filter_t * filter) {
		chain.push_back(filter);
	}

	bool processKey(myvi::key_t::key_t key) {
		for (std::vector<keyboard_filter_t *>::const_iterator it = chain.begin(); it != chain.end(); it++) {
			if ((*it)->processKey(key)) {
				return true;
			}
		}
		return false;
	}
};


// все изменяемые строки здесь и далее длиной 255
typedef myvi::string_impl_t<255> volatile_string_impl_t;


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

		myvi::string_t next();

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


class view_build_context_t {
private:
	myvi::gobject_t *view;
	gen::view_meta_t *view_meta;
	gen::parameter_meta_t *parameter_meta;
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

	gen::view_meta_t * get_view_meta() {
		_MY_ASSERT(view_meta, return 0);
		return view_meta;
	}

	gen::parameter_meta_t * get_parameter_meta() {
		_MY_ASSERT(parameter_meta, return 0);
		return parameter_meta;
	}

	gen::parameter_meta_t * try_get_parameter_meta() {
		return parameter_meta;
	}

	meta_path_t get_parameter_path() {
		return parameter_path;
	}

	void set_view(myvi::gobject_t *_view) {
		view = _view;
	}

	void set_view_meta(gen::view_meta_t *_view_meta) {
		view_meta = _view_meta;
	}

	void set_parameter_meta(gen::parameter_meta_t *_parameter_meta);

};




// фабрика дочерних видов. Её задача создать вид и связать его с контроллерм
class view_factory_t {
public:
	static view_factory_t _instance;
public:
	view_factory_t() {
	}

	static view_factory_t & instance() {
		return _instance;
	}


	// метод фабрики вида по умолчанию для составного вида
	myvi::gobject_t * build_view(view_build_context_t ctx) ;

	// Метод фабрики вида для параметра
	myvi::gobject_t * build_menu_view(view_build_context_t ctx);

	u32 parse_color(myvi::string_t color);

	void prepare_context(myvi::label_context_t &ret, gen::meta_t *meta);

};


class view_meta_ex_t {
public:
	gen::view_meta_t *view_meta;
public:
	view_meta_ex_t(gen::view_meta_t *_view_meta) {
		view_meta = _view_meta;
	}

// метод фабрики вида. Создает вид и привязывает к нему контроллер
	myvi::gobject_t * build_view(view_build_context_t ctx) {
		ctx.set_view_meta(view_meta);
		return custom::view_factory_t::instance().build_view(ctx);
	}

	myvi::gobject_t * build_view_no_ctx() {
		custom::view_build_context_t ctx = custom::view_build_context_t();
		ctx.set_view_meta(view_meta);
		return build_view(ctx);
	}

};

class dynamic_view_mixin_t  {
public:
	std::unordered_map<myvi::string_t, myvi::gobject_t *, gen::string_t_hash_t> id_map;
public:
	// добавляет дочерний вид с привязкой идентификатора
	virtual void add_child(myvi::gobject_t *child, myvi::string_t id) {
		_MY_ASSERT(child, return);
		id_map[id] = child;
	}

	myvi::gobject_t *get_child(myvi::string_t id) {
		return id_map[id];
	}

	myvi::gobject_t * resolve_path(myvi::string_t _path);
};


template<typename TBase>
class dynamic_view_mixin_aware_impl_t : public TBase,  public dynamic_view_mixin_t {
public:

	virtual void add_child(myvi::gobject_t *child, myvi::string_t id) OVERRIDE {
		dynamic_view_mixin_t::add_child(child, id);
		TBase::add_child(child);
	}

};



namespace variant_type_t {
	typedef enum {
		STRING,
		INT,
		FLOAT
	} variant_type_t;
}



template <typename T>
class variant_tt {
public:
	variant_type_t::variant_type_t type;
	T sval;
	s32 ival;
	double fval;
public:
	variant_tt() {
		type = variant_type_t::STRING;
		ival = 0;
		fval = 0;
	}

	bool is_empty() {
		switch (type) {
		case variant_type_t::STRING:
			return sval.is_empty();

		case variant_type_t::INT:
			return ival == _NAN;

		case variant_type_t::FLOAT:
			return fval == _NANF;
		}
		return true;
	}

	variant_tt(myvi::string_t _sval) {
		set_value(_sval);
	}

	variant_tt(s32 _ival) {
		set_value(_ival);
	}

	variant_tt(double _fval) {
		set_value(_fval);
	}

	void set_value(myvi::string_t _sval) {
		sval = _sval;
		type = variant_type_t::STRING;
	}

	void set_value(s32 _ival) {
		ival = _ival;
		type = variant_type_t::INT;
	}

	void set_value(double _fval) {
		fval = _fval;
		type = variant_type_t::FLOAT;
	}

	myvi::string_t get_string_value() {
		_MY_ASSERT(type == variant_type_t::STRING, return 0);
		return sval;
	}

	s32 get_int_value() {
		_MY_ASSERT(type == variant_type_t::INT, return 0);
		return ival;
	}

	double get_float_value() {
		_MY_ASSERT(type == variant_type_t::FLOAT, return 0);
		return fval;
	}

};

// служит только для передачи, но не для хранения значения !
typedef variant_tt<myvi::string_t> variant_t;


class model_message_t {
public:
	myvi::string_t path;
	variant_t value;
public:
	model_message_t(myvi::string_t _path, variant_t _value) : path(_path), value(_value) {
	}
};

#define _MODEL_MAX_SUBSCRIBERS 128

class model_t : public myvi::publisher_t<model_message_t, _MODEL_MAX_SUBSCRIBERS> {
public:
	static model_t *_instance;
public:
	static model_t * instance() {
		return _instance;
	}
	// обновление модели
	virtual void update(myvi::string_t parameter_path, variant_t &value) = 0;
	// ожидает правильного типа значения до вызова !
	virtual void read(myvi::string_t parameter_path, variant_t &value) = 0;

	virtual void try_register_path(myvi::string_t parameter_path, variant_t &initial_value, variant_type_t::variant_type_t expected_type) = 0;

	void read(myvi::string_t parameter_path, variant_t &value, variant_type_t::variant_type_t expected_type)  {
		read(parameter_path, value);
		_MY_ASSERT(value.type == expected_type, return);
	}

};


} // ns myvi
#endif