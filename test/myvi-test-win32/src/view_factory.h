// отделена от generator_common для компилируемости
#ifndef _VIEW_FACTORY_H
#define _VIEW_FACTORY_H

#include "widgets.h"
#include "generator_common.h"

namespace custom {


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

	static view_factory_t * instance() {
		return &_instance;
	}


	// метод фабрики вида по умолчанию для составного вида
	myvi::gobject_t * build_view(view_build_context_t ctx) ;

	// Метод фабрики вида для параметра
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

} // ns myvi
#endif