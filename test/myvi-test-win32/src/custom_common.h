// отделена от generator_common дл€ компилируемости
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


// все измен€емые строки здесь и далее длиной 255
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




// измен€емый путь
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


// неизмен€емый путь
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




// фабрика дочерних видов. ≈Є задача создать вид и св€зать его с контроллерм
class view_factory_t {
public:
	static view_factory_t _instance;
public:
	view_factory_t() {
	}

	static view_factory_t & instance() {
		return _instance;
	}


	// метод фабрики вида по умолчанию дл€ составного вида
	myvi::gobject_t * build_view(view_build_context_t ctx) ;

	// ћетод фабрики вида дл€ параметра
	myvi::gobject_t * build_menu_view(view_build_context_t ctx);

	u32 parse_color(myvi::string_t color) {
		if (color == "BACKGROUND_LIGHT") {
			return 0xF9FFF4;
		} else if (color == "BACKGROUND_BLUE") {
			return 0x679AB9;
		}
		_MY_ASSERT(0, return 0);
		return 0;
	}

};


class view_meta_ex_t {
public:
	gen::view_meta_t *view_meta;
public:
	view_meta_ex_t(gen::view_meta_t *_view_meta) {
		view_meta = _view_meta;
	}

// метод фабрики вида. —оздает вид и прив€зывает к нему контроллер
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
	// добавл€ет дочерний вид с прив€зкой идентификатора
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




} // ns myvi
#endif