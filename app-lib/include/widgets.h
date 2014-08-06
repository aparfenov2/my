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

	gobject_t * locate_next(direction_t::direction_t direction, gobject_t *root);

};

class focus_intention_t {
public:
	gobject_t *current;
	gobject_t *next;
	direction_t::direction_t direction;
public:
	focus_intention_t() {
		current = 0;
		next = 0;
		direction = direction_t::UP;
	}
};


// могут изменять назначения фокуса
class focus_master_t {
public:
public:
	virtual void alter_focus_intention(focus_intention_t &intention) = 0;
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
				if (p->enabled && dynamic_cast<focus_client_t*>(p)) {
					break;
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

	virtual void render(surface_t &dst) {
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


	focus_master_t * get_focus_master() {

		gobject_t *p = parent;
		while (p) {
			focus_master_t *ret = dynamic_cast<focus_master_t*>(p);
			if (ret) {
				return ret;
			}
			p = p->parent;
		}
		return 0;
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
	virtual void child_request_size_change(gobject_t *child, s32 new_w, s32 new_h) {
	}


	virtual void get_preferred_size(s32 &pw, s32 &ph) {
		_MY_ASSERT(layout,return);
		layout->get_preferred_size(this, pw,ph);
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

class label_context_t {
public:
//	u32 text_color; // цвет текста и глифа
	font_size_t::font_size_t font_size;    // размер текста \ глифа в пикселах
	ttype_font_t *font;	// шрифт текста \ глифа
	surface_context_t sctx;
public:
	label_context_t() {
		font_size=font_size_t::FS_8;
		font=(0);
		sctx.pen_color = 0xffffff;
	}
};

class label_t  : public gobject_t {
public:
	u32 glyph_code;	// глиф
	string32_t text32;  // текст
	string_t text;  // текст
	label_context_t ctx;
public:
	label_t() {
		glyph_code=(0x0000);
		visible = false;
	}

	virtual void render(surface_t &dst) OVERRIDE ;

	virtual void get_preferred_size(s32 &aw, s32 &ah) OVERRIDE ;
};

class button_context_t {
public:
	u32 bk_color;	// цвет не выбранной кнопки
	u32 bk_sel_color;	// цвет выбранной кнопки
	u32 bk_pressed_color;	// цвет нажатой кнопки
	u32 bk_disabled_color; // цвет неактивной кнопки
	surface_context_t sctx;
public:
	button_context_t() {
		bk_color=(0xB8BDCF),bk_sel_color=(0x848794),bk_pressed_color=(0x6B8896);
		bk_disabled_color = 0xA0A0A0;
	}
};


// выравнивает дочерние обьекты по центру
class center_layout_t : public layout_t {
public:
	s32 spx;
	s32 spy;
	bool vertical;
public:
	center_layout_t() {
		spx = 5, spy = 5;
		vertical = true;
	}


	virtual void get_preferred_size(gobject_t *parent, s32 &aw, s32 &ah) OVERRIDE {
		_MY_ASSERT(0,return);
	}

	virtual void layout(gobject_t *parent) OVERRIDE;
};


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
		preferred_item_size = false;
		stretch_last = false;
	}

	virtual void get_preferred_size(gobject_t *parent, s32 &aw, s32 &ah) ;


	virtual void layout(gobject_t *parent) OVERRIDE;
};


// растягивает дочерний виджет на весь размер текущего
class stretch_layout_t : public layout_t {
public:
public:
	virtual void get_preferred_size(gobject_t *parent, s32 &aw, s32 &ah) OVERRIDE {
		gobject_t::iterator_visible_t iter = parent->iterator_visible();
		gobject_t *child = iter.next();
		_MY_ASSERT(child,return);

		child->get_preferred_size(aw, ah);
		_MY_ASSERT(!iter.next(),return); // нельзя растянуть все обьекты
	}

	virtual void layout(gobject_t *parent) OVERRIDE {
		gobject_t::iterator_visible_t iter = parent->iterator_visible();
		gobject_t *child = iter.next();
		_MY_ASSERT(child,return);

		child->x = 0;
		child->y = 0;
		child->h = parent->h;
		child->w = parent->w;

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



class button_t : public gobject_t, public focus_client_t {
public:
	label_t l_top;
	label_t l_mid;
	label_t l_bot;
	button_context_t ctx;
	levels_layout_t levels_layout;
	property_t<bool, button_t> pressed;
private:
	bool _pressed;	// текущее состояние
private:
	bool get_pressed() {
		return _pressed;
	}
	void set_pressed(bool apressed) {
		_pressed = apressed;
		dirty = true;
	}

public:
	button_t() {
		_pressed = (false);
		layout = &levels_layout;
		pressed.init(this,&button_t::get_pressed, &button_t::set_pressed);

		add_child(&l_top);
		add_child(&l_mid);
		add_child(&l_bot);
	}

	virtual void render(surface_t &dst) OVERRIDE ;

};


#define INPUT_MAX_LEN 30
#define COLOR_SELECTED 0xAFBFCF
#define COLOR_CAPTURED 0x267F00

class textbox_msg_t {
public:
	enum state_t {
		EDIT, // значенеие еще редактируется
		COMPLETE // пользователь нажал Enter и мы вышли из режима редактирования
	} state;
	myvi::string_t value;
public:
	textbox_msg_t(state_t _state, myvi::string_t _value) : state(_state), value(_value) {
	}
};

class text_box_t : public gobject_t, public focus_client_t, public focus_aware_t, public publisher_t<textbox_msg_t, 1> {
	typedef gobject_t super;
public:
	property_t<string_t , text_box_t> value;
	label_t lab;
	bool cursor_visible;
	u32 cursor_color;
private:
	string_impl_t<INPUT_MAX_LEN> _value;
	s32 cursor_pos[INPUT_MAX_LEN];
	s32 caret_pos;
private:
	void set_value(string_t cvalue) {
		_value=(cvalue);
		lab.text = _value;
		caret_pos = _value.length();
	}

	string_t get_value() {
		return _value;
	}
public:
	text_box_t() {
		value.init(this,&text_box_t::get_value, &text_box_t::set_value);
		lab.ctx.sctx.pen_color = 0x00;
		lab.visible = true;
		caret_pos = 0;
		cursor_visible = false;
		cursor_color = 0x000000;

		add_child(&lab);
	}

	void measure_cursor_pos() {
		for (s32 i=1; i <= _value.length(); i++) {
			lab.text = _value.sub(0,i);
			s32 lw, lh;
			lab.get_preferred_size(lw,lh);
			cursor_pos[i-1] = lw;
		}
		lab.text = _value;
	}

	virtual void get_preferred_size(s32 &aw, s32 &ah) OVERRIDE {
		measure_cursor_pos();
		lab.get_preferred_size(aw, ah);
		//if (h < lab.h) h = lab.h;
		//if (w < lab.w + 5) w = lab.w + 5;
	}

	virtual void do_layout() OVERRIDE {
		lab.x = 0;
		lab.y = 0;
		lab.w = w; // место для курсора
		lab.h = h;
	}

	virtual void key_event(key_t::key_t key) OVERRIDE;

	//virtual void set_dirty(bool dirty) OVERRIDE {
	//	if (dirty && parent) {
	//		parent->dirty = true;
	//	}
	//	super::set_dirty(dirty);
	//}

	virtual void set_selected(bool selected) {
		_MY_ASSERT(parent,return);
		dirty = true;
		focus_client_t::set_selected(selected);
	}

	virtual void render(surface_t &dst) OVERRIDE;

};

// элемент в выпадающем списке
class combobox_item_t {
public:
	virtual string_t get_string_value() = 0;
};

// метка с функцией выбора из списка значений
class combo_box_t : public gobject_t, public focus_client_t, public focus_aware_t, public publisher_t<combobox_item_t *, 1> {
	typedef gobject_t super;

	class empty_iterator_t : public iterator_t<combobox_item_t> {
	public:
		virtual combobox_item_t* next(void* prev) OVERRIDE {
			return 0;
		}
	};
public:
	label_t lab;
	wo_property_t<iterator_t<combobox_item_t>*,combo_box_t> values;
	property_t<combobox_item_t *, combo_box_t> value;
private:
	combobox_item_t * _value;
	string_impl_t<INPUT_MAX_LEN> _str_value;
	iterator_t<combobox_item_t> *_values;
	bool captured;
	combobox_item_t *sprev;
	reverse_iterator_t<combobox_item_t> revit;
	empty_iterator_t empty_iterator;
private:
	void set_values(iterator_t<combobox_item_t> *values) {
		_values = values;
		if (!values)
			_values = &empty_iterator;
		revit.assign(_values);
		sprev = 0;
	}

	void set_value(combobox_item_t * value) {
		_value=(value);
		lab.text = _value->get_string_value();
	}

	combobox_item_t * get_value() {
		return _value;
	}
public:

	combo_box_t() {
		captured = false;
		_values = 0;
		_value = 0;
		sprev = 0;
		lab.ctx.sctx.pen_color = 0x00;
		lab.visible = true;
		values.init(this,&combo_box_t::set_values);
		value.init(this,&combo_box_t::get_value, &combo_box_t::set_value);

		values = &empty_iterator;

		add_child(&lab);
	}

	//virtual void set_dirty(bool dirty) OVERRIDE {
	//	if (dirty && parent) {
	//		parent->dirty = true;
	//	}
	//	super::set_dirty(dirty);
	//}

	virtual void get_preferred_size(s32 &aw, s32 &ah) OVERRIDE {
		lab.get_preferred_size(aw,ah);
		//if (h < lab.h) h = lab.h;
		//if (w < lab.w + 5) w = lab.w + 5;
	}

	virtual void do_layout() OVERRIDE {
		lab.x = 0;
		lab.y = 0;
		lab.w = w;
		lab.h = h;
	}

	virtual void key_event(key_t::key_t key) OVERRIDE;

	virtual void render(surface_t &dst) OVERRIDE;

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
