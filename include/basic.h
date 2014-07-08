#ifndef _BASIC_TYPES_H
#define _BASIC_TYPES_H

#include "types.h"
#include "assert_impl.h"

namespace myvi {


/*
class singleton_t {
private:
   singleton_t();
public:
   static singleton_t& instance() {
      static singleton_t INSTANCE;
      return INSTANCE;
   }
};
*/


template<typename T, s32 _max_len>
class stack_t {
protected:
    s32   count;
public:
    T data[_max_len];
    stack_t() {
		count=(0);
	}

	void assign(T *values, s32 len) {
		_MY_ASSERT(len <= _max_len, return);
		count = len;
		for (s32 i=0; i<len; i++) {
			data[i] = values[i];
		}
	}

	void append(T *values, s32 len) {
		_MY_ASSERT(len+count <= _max_len, return);
		for (s32 i=0; i<len; i++) {
			data[count++] = values[i];
		}
	}

    void clear() {
        count = 0;
    }

    s32 length() const {
        return count;
    }
    s32 capacity() const {
        return _max_len;
    }

    T operator[](s32 inx) const {
		_MY_ASSERT(inx < count,return T());
        return data[inx];
    }

    bool  is_empty() const {
        return count == 0;
    }

	void push(T v) {
		_MY_ASSERT(count < _max_len,return);
		data[count] = v;
		count++;
	}

	T pop() {
		_MY_ASSERT(count,return T());
		count--;
		return data[count];
	}

	T first() {
		_MY_ASSERT(count, return T());
		return data[0];
	}

	T last() {
		_MY_ASSERT(count, return T());
		return data[count-1];
	}

	T * last_ref() {
		_MY_ASSERT(count, return 0);
		return & data[count-1];
	}

};



template <typename T>
class subscriber_t {
public:
public:
	virtual void accept(T &msg) = 0;
};

template <typename T, s32 max_subscribers>
class publisher_t {
public:
	stack_t<subscriber_t<T> *,max_subscribers> subscribers;
public:
	void subscribe(subscriber_t<T> *subscriber) {
		subscribers.push(subscriber);
	}

	void notify(T &msg) {
		for (s32 i=0; i < subscribers.length(); i++) {
			subscribers[i]->accept(msg);
		}
	}
};

//----------------------- string implementation -------------------------

template<typename T>
class string_tt {
protected:
    T *data;
    s32   count;
public:
    string_tt() {
		data=0, count=0;
	}

    string_tt(const T* ptr, s32 len) {
		data=((T *)ptr), count=(len);
	}

    string_tt(const T* ptr) {
		data = ((T *)ptr);
        count = strlen(data);  
    }

    T operator[](s32 inx) const {
		_MY_ASSERT(data && inx < count, return T());
        return data[inx];
    }

    bool operator==(const string_tt<T>& o) const {
        if(count != o.count) return false;
        return strcmp(data, o.data, count);
    }

    bool  is_empty() const {
        return count == 0;
    }

    s32 length() const {
        return count;
    }

    const T* c_str() const {
		_MY_ASSERT(data && !data[count], return 0);
        return data;
    }

    string_tt sub(s32 start) const {
		_MY_ASSERT(data, return string_tt(""));
        if(start > count) start = count;

        return string_tt(data + start, count - start);
    }

    string_tt sub(s32 start, s32 len) const {
		_MY_ASSERT(data, return string_tt(""));
        if(start > count) start = count;
        if((start + len) > count) len = count - start;

        return string_tt(data + start, len);
    }

protected:
    s32 strlen(const T* p) {
        if(p == null) return 0;

        s32 len = 0;
        while(*p)
        {
            len++;
            p++;
        }
        return len;
    }

    bool strcmp(const char* s1, const char* s2, unsigned int len) {
        for(unsigned int i=0; i<len; i++)
            if(*s1++ != *s2++)
                return false;
        return true;
    }

};


typedef string_tt<char> string_t;
typedef string_tt<u32> string32_t;


// изменяемая строка - требует буфер в памяти

template<typename T>
class volatile_string_tt : public string_tt<T> {
private:
	s32 max_str_len;
public:
	volatile_string_tt(T *_string_buf, s32 cnt, s32 _max_str_len) {
		this->count = cnt;
		this->data = _string_buf;
		this->max_str_len = _max_str_len;
	}

	void reset() {
		this->count = 0;
		this->data[0] = 0;
	}

	s32 capacity() {
		return max_str_len;
	}

	// обновляет счетчик
	void update_length() {
		_MY_ASSERT(data, return);
		count = strlen(data);
	}

	// used in itoa
	void set_length(s32 len) {
		_MY_ASSERT(len <= max_str_len, return);
		this->count = len;
		data[count] = 0;
	}

    volatile_string_tt<T>& operator = (const string_tt<T> &cstr) {
		_MY_ASSERT(cstr.length() <= max_str_len, return *this);
		this->count = cstr.length();
		for (s32 i=0; i < this->count; i++) {
			this->data[i] = cstr[i];
		}
		this->data[this->count] = 0;
        return *this;
    }

    volatile_string_tt<T>& operator = (const volatile_string_tt<T> &other) {
		volatile_string_tt<T>::operator=((string_tt<T>)other);
        return *this;
    }

    volatile_string_tt<T>& operator += (string_tt<T> cstr) {
		_MY_ASSERT(this->count <= max_str_len - (s32)cstr.length(),return *this);
		insert_at(this->count, cstr);
        return *this;
    }

	void append(string_tt<T> cstr) {
		_MY_ASSERT(this->count <= max_str_len - (s32)cstr.length(), return);
		insert_at(this->count, cstr);
	}

	void append(u8 ch) {
		_MY_ASSERT(this->count < max_str_len, return);
		insert_at(this->count, ch);
	}

	void delete_at(s32 caret_pos) {
		_MY_ASSERT(caret_pos >= 0 && caret_pos < this->count && this->count,return);
		this->count--;
		for (s32 i=caret_pos; i < this->count; i++) {
			this->data[i] = this->data[i+1];
		}
		this->data[this->count] = 0;
	}

	void insert_at(s32 caret_pos, u8 ch) {
		_MY_ASSERT(caret_pos >= 0 && caret_pos <= this->count && this->count < max_str_len,return);
		for (s32 i = this->count-1; i >= caret_pos; i--) {
			this->data[i+1] = this->data[i];
		}
		this->data[caret_pos] = ch;
		this->count++;
		this->data[this->count] = 0;
	}


	void insert_at(s32 caret_pos, string_tt<T> cstr) {
		_MY_ASSERT(caret_pos >= 0 && caret_pos <= this->count && this->count <= max_str_len - (s32)cstr.length(),return);
		if (!cstr.length()) return;
		_MY_ASSERT(this->count-1 + cstr.length() < max_str_len,return);

		for (s32 i = this->count-1; i >= caret_pos; i--) {
			this->data[i + cstr.length()] = this->data[i];
		}

		for (s32 i = 0; i < cstr.length(); i++) {
			this->data[caret_pos] = cstr[i];
			caret_pos++;
		}
		this->count += cstr.length();

		_MY_ASSERT(caret_pos <= this->count,return);
		_MY_ASSERT(this->count <= max_str_len,return);
		this->data[this->count] = 0;
	}

};


typedef volatile_string_tt<char> volatile_string_t;


template <typename T, s32 _max_str_len>
class string_impl_tt : public volatile_string_tt<T>  {
	typedef volatile_string_tt<T> super;
private:
	T _data[_max_str_len+1];
public:
	string_impl_tt() : volatile_string_tt<T>(_data, 0,_max_str_len) {
		reset();
	}

	string_impl_tt & operator =(const volatile_string_tt<T> &other) {
		_MY_ASSERT(this->data == _data, return *this);
		super::operator=(other);
		return *this;
	}
	string_impl_tt & operator =(const string_tt<T> &other) {
		_MY_ASSERT(this->data == _data, return *this);
		super::operator=(other);
		return *this;
	}

};

template <s32 _max_str_len>
class string_impl_t : public string_impl_tt<char,_max_str_len> {
	typedef string_impl_tt<char,_max_str_len> super;
public:
	string_impl_t & operator =(const volatile_string_tt<char> &other) {
		super::operator=(other);
		return *this;
	}
	string_impl_t & operator =(const string_tt<char> &other) {
		super::operator=(other);
		return *this;
	}
};


template <typename T>
class iterator_t {
public:
	virtual T* next(void* prev) = 0;
};


template <typename T>
class reverse_iterator_t : public iterator_t<T> {
public:
	iterator_t<T> *iter;
public:
	reverse_iterator_t() {
		iter = 0;
	}
	void assign(iterator_t<T> *_iter) {
		_MY_ASSERT(_iter,return);
		iter = _iter;
	}
	virtual T* next(void* prev) OVERRIDE {
		_MY_ASSERT(iter,return 0);
		T *p = iter->next(0), *pp = 0;
		while (p) {
			if (prev && p == prev) 
				return pp;
			pp = p;
			p = iter->next(p);
		}
		_MY_ASSERT(!prev,return 0);
		return pp;
	}
};



template <typename TBase, typename T>
class delegate_iterator_t : public iterator_t<T>  {
	typedef T * (TBase::*delegate_t) ( void* prev );
private:
	delegate_t dlg;
	TBase *base;
public:
	void init(TBase *_base, delegate_t _dlg) {
		dlg = _dlg;
		base = _base;
	}
	virtual T* next(void* prev) OVERRIDE {
		return (base->*dlg)(prev);
	}
};



// Базовый класс шаблона свойства.
template <typename propowner> 
class abstract_property_t {
protected:
    propowner * m_owner;
public:
	abstract_property_t() {
		m_owner = 0;
	}
};

template <typename proptype, typename propowner> 
class ro_property_t : public abstract_property_t<propowner> {
protected:
    typedef proptype (propowner::*getter)();
    getter m_getter;
public:
    // Оператор приведения типа. Реализует свойство для чтения.
    operator proptype() {
        // Здесь может быть проверка "m_owner" и "m_getter" на NULL
		_MY_ASSERT(this->m_owner && m_getter,_STOP(0));
        return (this->m_owner->*m_getter)();
    }
    // Конструктор по умолчанию.
    ro_property_t() {
        m_getter=(NULL);
    }
    // Инициализация
    void init(propowner * const owner, getter getmethod) {
		_MY_ASSERT(owner && getmethod,return);
		this->m_owner = owner;
        m_getter = getmethod;
    }
};

template <typename proptype, typename propowner> 
class wo_property_t : public abstract_property_t<propowner> {
protected:
    typedef void (propowner::*setter)(proptype);
    setter m_setter;
public:
    // Оператор присваивания. Реализует свойство для записи.
    void operator =(proptype data) {
        // Здесь может быть проверка "m_owner" и "m_setter" на NULL
		_MY_ASSERT(this->m_owner && m_setter,return);
        (this->m_owner->*m_setter)(data);
    }
    // Конструктор по умолчанию.
    wo_property_t() {
        m_setter=(NULL);
    }
    // Инициализация
    void init(propowner * const owner, setter setmethod) {
		_MY_ASSERT(owner && setmethod, return);
		this->m_owner = owner;
        m_setter = setmethod;
    }
};


template <typename proptype, typename propowner> 
class property_t : public abstract_property_t<propowner> {
protected:
    typedef proptype (propowner::*getter)();
    getter m_getter;
    typedef void (propowner::*setter)(proptype);
    setter m_setter;
public:
    // Оператор приведения типа. Реализует свойство для чтения.
    operator proptype() {
        // Здесь может быть проверка "m_owner" и "m_getter" на NULL
		_MY_ASSERT(this->m_owner && m_getter,_STOP(0));
        return (this->m_owner->*m_getter)();
    }
    // Оператор присваивания. Реализует свойство для записи.
    void operator =(proptype data) {
        // Здесь может быть проверка "m_owner" и "m_setter" на NULL
		_MY_ASSERT(this->m_owner && m_setter, return);
        (this->m_owner->*m_setter)(data);
    }
    // Конструктор по умолчанию.
    property_t() {
        m_getter=(NULL);
        m_setter=(NULL);
    }
    // Инициализация
    void init(propowner * const owner, getter getmethod, setter setmethod) {
		_MY_ASSERT(owner && getmethod && setmethod, return);
		this->m_owner = owner;
        m_getter = getmethod;
        m_setter = setmethod;
    }
};

/*
class CValue
{
private:
    int m_value;
    int get_Value()
    {
        return m_value;    // Или более сложная логика
    }
    void put_Value(int value)
    {
        m_value = value;     // Или более сложная логика
    }
public:
    property <int, CValue> Value;
    CValue()
    {
        Value.init(this, get_Value, put_Value);
    }
};

CValue val;
val.Value = 50;

*/



} // ns
#endif
