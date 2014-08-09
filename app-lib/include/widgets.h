#ifndef _WIDGETS_H
#define _WIDGETS_H

#include "surface.h"
#include "truetype.h"
#include "app_events.h"
#include "assert_impl.h"
#include "basic.h"

#include <vector>


namespace myvi {


class gobject_t;

class layout_t {
public:
	virtual void get_preferred_size(gobject_t *parent, s32 &pw, s32 &ph) = 0;
	virtual void layout(gobject_t *parent) = 0;
};



#define _MAX_GOBJECT_TREE_DEPTH 32
#define _MAX_FOCUS_MANAGER_SUBSCRIBERS 32

namespace direction_t {
	typedef enum {
		UP,
		DOWN,
		LEFT,
		RIGHT
	} direction_t;
}

class focus_client_t;
class focus_aware_t;

// глобальный менеджер фокуса
class focus_manager_t : public publisher_t<gobject_t *,_MAX_FOCUS_MANAGER_SUBSCRIBERS> {
public:
	gobject_t *selected;
	stack_t<focus_aware_t *, _MAX_GOBJECT_TREE_DEPTH> captured;


private:
	static focus_manager_t _instance;
	focus_manager_t() {
		selected = 0;
	}

public:

	static focus_manager_t & instance() {
		return _instance;
	}

	void key_event(key_t::key_t key, gobject_t *root);
	void select(gobject_t *p);

	void capture_child(focus_aware_t *child) {
		_MY_ASSERT(child,return);
		if (captured.length()) {
			_MY_ASSERT(captured.last() != child, return);
		}
		captured.push(child);
	}

	void release_child(focus_aware_t *child) {
		_MY_ASSERT(child,return);
		_MY_ASSERT(captured.last()  == child,return);
		captured.pop();
	}

};


// могут захватывать фокус, обрабатывать сообщения клавиатуры
class focus_aware_t {
public:
	virtual void key_event(key_t::key_t key);

	void capture_focus() {
		focus_manager_t::instance().capture_child(this);
	}

	void release_focus() {
		focus_manager_t::instance().release_child(this);
	}

};


// могут быть в фокусе, "выбраны"
class focus_client_t {
public:
	property_t<bool, focus_client_t> selected;
private:
	bool _selected;

private:
	bool get_selected() {
		return _selected;
	}
public:
	focus_client_t() {
		_selected = false;
		selected.init(this,&focus_client_t::get_selected, &focus_client_t::set_selected);
	}


	virtual void set_selected(bool selected) {
		_selected = selected;
	}

};



// Базовый класс всех объектов экрана
class gobject_t { 
public:

	// итератор по всей коллекции детей
	class iterator_all_t  {
	private:
		const gobject_t *that;
		std::vector<gobject_t *>::const_iterator it;
		bool neverRun;
	public:
		iterator_all_t() {
			that = 0;
			neverRun = true;
		}

		iterator_all_t(const gobject_t *_that) {
			that = _that;
			neverRun = true;
			it = that->children.begin();
		}

		gobject_t * next();
	};

	// итератор по видимым детям
	class iterator_visible_t  {
	private:
		iterator_all_t iter_all;
	public:
		iterator_visible_t() {
		}

		iterator_visible_t(iterator_all_t _iter_all) {
			iter_all = _iter_all;
		}

		gobject_t * next() {
			gobject_t * nxt = iter_all.next();
			while (nxt && !nxt->visible) {
				nxt = iter_all.next();
			}
			return nxt;
		}

	};

	// итератор по selectable детям с заходом в глубину
	class iterator_visible_deep_t {
	private:
		stack_t<iterator_visible_t, _MAX_GOBJECT_TREE_DEPTH> iter_stack;
	public:
		iterator_visible_deep_t() {
		}

		iterator_visible_deep_t(iterator_visible_t _iter_visible) {
			iter_stack.push(_iter_visible);
		}

		gobject_t * next();

	};

	// итератор по selectable детям с заходом в глубину
	class iterator_selectable_deep_t {
	private:
		iterator_visible_deep_t iter_visible_deep;
	public:
		iterator_selectable_deep_t(iterator_visible_deep_t _iterator_visible_deep) {
			iter_visible_deep = _iterator_visible_deep;
		}

		gobject_t * next() {

			gobject_t * p = iter_visible_deep.next();

			while (p) {
				if (p->enabled) {
					focus_client_t *focus_client = dynamic_cast<focus_client_t*>(p);
					if (focus_client) {
						break;
					}
			}
				p = iter_visible_deep.next();
			}
			return p;
		}

	};


public:
	s32 x; // координаты относительно родительского контейнера
	s32 y;
	s32 w; // размеры обьекта
	s32 h;

	bool visible;

	gobject_t *parent;

	layout_t *layout;

	property_t<bool, gobject_t> enabled;
	property_t<bool, gobject_t> dirty;

	std::vector<gobject_t *> children;

private:
	bool _enabled;
	bool _dirty; // if true - shall render
	gobject_t *last_selectable;


public:
	gobject_t() {
		parent = 0;
		x=(0),y=(0),w=(0),h=(0),_dirty=(false), 
		visible = true, layout = 0,  _enabled = true;
		enabled.init(this,&gobject_t::get_enabled, &gobject_t::set_enabled);
		dirty.init(this,&gobject_t::get_dirty, &gobject_t::set_dirty);
		last_selectable = 0;
	}

	virtual void init() {
		init_children();
	}
private:	
	bool get_dirty() {
		return _dirty;
	}
	bool get_enabled() {
		return _enabled;
	}
public:

	// если фон непрозрачный, обязательно перекрыть этот метод чтобы отрисовка не распространялась дальше !
	virtual void set_dirty(bool dirty) {
		_dirty = dirty;
		if (dirty && parent) {
			parent->dirty = true;
		}
	}
	virtual void set_enabled(bool enabled) {
		_enabled = enabled;
	}

	void add_child(gobject_t *child) {
		child->parent = this;
		children.push_back(child);
	}

	
	void init_children() {		// must be called from derived class ctor
		iterator_all_t iter = iterator_all();
		gobject_t *p = iter.next();
		while(p) {
			p->parent = this;
			p->init();
			p = iter.next();
		}
	}

	void translate(s32 &ax, s32 &ay) {
		gobject_t *p = parent;
		ax = x;
		ay = y;
		while (p) {
			ax += p->x;
			ay += p->y;
			p = p->parent;
		}
	}

	bool is_child_of(gobject_t *aparent) {

		_MY_ASSERT(aparent, return false);

		gobject_t *p = this->parent;
		while(p) {
			if (p == aparent) {
				return true;
			}
			p = p->parent;
		}
		return false;
	}

	bool is_direct_child_of(gobject_t *aparent) {

		_MY_ASSERT(aparent, return false);
		return this->parent == aparent;
	}


	bool is_parent_of(gobject_t *child) {
		
		_MY_ASSERT(child, return false);
		return child->is_child_of(this);
	}

	bool is_direct_parent_of(gobject_t *child) {
		
		_MY_ASSERT(child, return false);
		return child->parent == this;
	}

	gobject_t * get_root() {
		gobject_t *pp = 0, *p = this->parent;
		while(p) {
			pp = p;
			p = p->parent;
		}
		return pp ? pp : this;
	}

	virtual void render_before(surface_t &dst) {
	}
	virtual void render(surface_t &dst) {
	}
	virtual void render_after(surface_t &dst) {
	}
	void do_render(surface_t &dst) {
		render_before(dst);
		render(dst);
		render_after(dst);
	}
	
	// перечислитель всех дочерних обьектов
	const iterator_all_t iterator_all() const {
		return iterator_all_t(this);
	}

	// перечислитель всех видимых дочерних обьектов
	const iterator_visible_t iterator_visible() const {
		return iterator_visible_t(iterator_all());
	}

	// перечислитель всех видимых дочерних обьектов с заходом в глубину
	const iterator_visible_deep_t iterator_visible_deep() const {
		return iterator_visible_deep_t(iterator_visible());
	}


	// итератор по selectable детям с заходом в глубину
	const iterator_selectable_deep_t iterator_selectable_deep() const {
		return iterator_selectable_deep_t(iterator_visible_deep());
	}


	// вызывает do_layout для каждого из детей
	void layout_children() {
		_MY_ASSERT(w && h, return);
		iterator_visible_t iter = iterator_visible();
		gobject_t *bt = iter.next();
		while (bt) {
			bt->do_layout();
			bt = iter.next();
		}
	}
	// дитё запрашивает изменение своего размера
	virtual void child_request_size_change(gobject_t *child) {
		s32 pw, ph;
		this->get_preferred_size(pw,ph);

		if (pw != this->w || ph != this->h) {
			if (parent) {
				parent->child_request_size_change(this);
			} else {
				do_layout();
			}
		} else {
			do_layout();
		}
	}


	virtual void vget_preferred_size(s32 &pw, s32 &ph) {

		pw = this->w;
		ph = this->h;

		if (layout) {
			layout->get_preferred_size(this, pw,ph);
		}
	}

	virtual void adjust_preferred_size(s32 &pw, s32 &ph) {
	}

	void get_preferred_size(s32 &pw, s32 &ph) {
		vget_preferred_size(pw,ph);
		adjust_preferred_size(pw,ph);
	}

	// размещает детей в пространстве родителя
	virtual void do_layout() {
		_MY_ASSERT(w && h,return);
		// если layout не указан, то считаем, что все дети были расположены в init
		if (layout) {
			layout->layout(this);
		}
		layout_children();	
	}

};



class rasterizer_t {
public:
	static u32 colors[4];
	static u32 deepLevel;
	static bool debug;
public:

	// очищаем dirty для обьекта и всех дочерних, т.к. он уже перерисовался
	static bool render(gobject_t *p, surface_t &dst, bool force_redreaw = false) {
		return render(p,dst,force_redreaw,-1,-1,-1,-1);
	}

	static bool render(gobject_t *p, surface_t &dst, bool force_redreaw, s32 pax, s32 pay, s32 paw, s32 pah);

};
namespace font_size_t {
	enum font_size_t {
		FS_8 = 12,
		FS_15 = 16,
		FS_20 = 21,
		FS_30 = 30
	};
}



// располагает дочерние компоненты подряд

class stack_layout_t : public layout_t {
public:
	s32 spx;
	s32 spy;  // высота пробела
	s32 bw;   // button width
	s32 bh;
	bool vertical;
	bool preferred_item_size;  // использовать предпочитаемый размер компонента вместо размера ячейки
	bool stretch_last;
public:
	stack_layout_t() {
		spx=5,spy=5,vertical=(true);
		bw=20,bh=20;
		preferred_item_size = true;
		stretch_last = false;
	}

	virtual void get_preferred_size(gobject_t *parent, s32 &aw, s32 &ah) ;


	virtual void layout(gobject_t *parent) OVERRIDE;
};


// растягивает дочерний виджет на весь размер текущего
class stretch_layout_t : public layout_t {
public:
	s32 padding;
public:
	stretch_layout_t() {
		padding = 0;
	}

	virtual void get_preferred_size(gobject_t *parent, s32 &aw, s32 &ah) OVERRIDE {
		gobject_t::iterator_visible_t iter = parent->iterator_visible();
		gobject_t *child = iter.next();
		_MY_ASSERT(child,return);

		child->get_preferred_size(aw, ah);
		aw += padding * 2;
		ah += padding * 2;

		_MY_ASSERT(!iter.next(),return); // нельзя растянуть все обьекты
	}

	virtual void layout(gobject_t *parent) OVERRIDE {
		gobject_t::iterator_visible_t iter = parent->iterator_visible();
		gobject_t *child = iter.next();
		_MY_ASSERT(child,return);

		child->x = padding;
		child->y = padding;
		child->h = parent->h - padding * 2;
		child->w = parent->w - padding * 2;

		_MY_ASSERT(!iter.next(),return); // нельзя растянуть все обьекты
	}
};

// выравнивает виджеты по заданным линейкам
#define LL_LEV_SIZE 4
class levels_layout_t : public layout_t {
public:
	s32 levels[LL_LEV_SIZE];
public:
	levels_layout_t() {
		levels[0] = 5;
		levels[1] = 30;
		levels[2] = 75;
		levels[3] = -1;
	}

	virtual void get_preferred_size(gobject_t *parent, s32 &aw, s32 &ah) OVERRIDE ;

	virtual void layout(gobject_t *parent) OVERRIDE ;
};




// оверлей модальных диалогов - должен находиться в корне дерева отображения


class modal_overlay_t : public gobject_t, public focus_aware_t {
private:
	modal_overlay_t() {
	}
	static modal_overlay_t _instance;
public:
	static modal_overlay_t & instance() {
		return _instance;
	}

	void push_modal(gobject_t *modal) {

		add_child(modal);
		modal->dirty = true;
	}

	void pop_modal() {

		gobject_t *modal = children.back();
		children.pop_back();

//		_MY_ASSERT(modal == popped,return);
		modal->parent = 0;
		children.front()->dirty = true;
	}

};

} // ns
#endif
