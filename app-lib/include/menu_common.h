#ifndef _MENU_COMMON_H
#define _MENU_COMMON_H

#include "widgets.h"
 
namespace myvi {

class menu_context_t {
public:
	button_context_t bctx1;
	label_context_t lctx1;
	label_context_t lctxg;
private:
	static menu_context_t _instance;

	menu_context_t() {
	}
public:
	static menu_context_t & instance() {
		return _instance;
	}
};




class inp_dlg_row_t : public gobject_t {
public:
	text_box_t lval;
	combo_box_t lsfx;
	stack_layout_t stack_layout;
public:
	inp_dlg_row_t() {
		stack_layout.vertical = false;
		stack_layout.preferred_item_size = true;
		layout = &stack_layout;

		add_child(&lval);
		add_child(&lsfx);
	}

	virtual void init() {
		gobject_t::init();

		menu_context_t &ctx = menu_context_t::instance();

		lval.visible = true;
		lval.lab.ctx = ctx.lctx1;
		lval.lab.ctx.font_size = font_size_t::FS_20;

		lsfx.visible = true;
		lsfx.lab.ctx = ctx.lctx1;
		lsfx.lab.ctx.font_size = font_size_t::FS_20;
	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		_MY_ASSERT(parent,return);
		parent->dirty = dirty;
	}


	virtual void child_request_size_change(gobject_t *child, s32 aw, s32 ah) OVERRIDE {

		get_preferred_size(aw,ah);

		if (parent) {
			parent->child_request_size_change(this, aw, ah);

		} else {
			do_layout();
			dirty = true;
		}
	}

};



// окно с прокруткой. Состоит из viewport и virtual окна. Для прокрутки сдвигаем virtual. 

// интерфейс interior-окна
class scrollable_interior_t : public gobject_t {
public:
public:
};

// окно с прокруткой.
class scrollable_window_t : public gobject_t, public subscriber_t<focus_client_t *>, public focus_master_t {
	typedef gobject_t super;
public:
	scrollable_interior_t *interior;
public:
	scrollable_window_t(scrollable_interior_t *ainterior) {
		interior = ainterior;
		focus_manager_t::instance.subscribe(this);

		add_child(interior);
	}


	// оповещение от focus_manager о выбранном объекте
	// обьект не наш, а interior !!!

	virtual void accept(focus_client_t* &sel) OVERRIDE {
		// если это наш объект (или одно из детей наших детей)
		gobject_t::iterator_selectable_deep_t iter = this->iterator_selectable_deep();
		gobject_t *p = iter.next();
		gobject_t *our = 0;
		while (p) {
			if (p == dynamic_cast<gobject_t *>(sel)) {
				our = p;
				break;
			}
			p = iter.next();
		}
		// то пытаемся скрыть крайние дочерние объекты чтобы объект в фокусе оставался видимым
		if (our) {
			// находим непосредственного childrena
			gobject_t *p = our;
			gobject_t *our_child = 0;

			while (p && p != this && p != this->interior) {
				our_child = p;
				p = p->parent;
			}
			_MY_ASSERT(our_child && our_child->parent == this->interior, return);
			
			scroll_to(our_child);
		}
	}

	// прокрутить чтобы выбранный children стал виден
	void scroll_to(gobject_t *interior_child) {

		_MY_ASSERT(interior_child->parent == this->interior, return);

		s32 ix,iy, ax,ay;
		interior_child->translate(ix,iy);
		this->translate(ax,ay);

		// пытаемся сдвинуть окно
		s32 dx = 0;

		if (ix < ax) {
			dx = ax - ix;
		} else if ((ix + interior_child->w) > (ax + this->w)) {
			dx = (ax + this->w) - (ix + interior_child->w);
		}

		s32 dy = 0;

		if (iy < ay) {
			dy = ay - iy;
		} else if ((iy + interior_child->h) > (ay + this->h)) {
			dy = (ay + this->h) - (iy + interior_child->h);
		}

		if (dx || dy) {
			interior->x += dx;
			interior->y += dy;
			this->dirty = true;
		}
	}

	virtual void get_preferred_size(s32 &pw, s32 &ph) OVERRIDE {
		_MY_ASSERT(interior,return);
		interior->get_preferred_size(pw,ph);
	}

	virtual void do_layout() OVERRIDE {
		_MY_ASSERT(w && h, return);

		s32 ipw,iph;
		interior->get_preferred_size(ipw,iph);

		interior->w = w;
		interior->h = h;

		if (ipw > interior->w) {
			interior->w = ipw;
		}
		if (iph > interior->h) {
			interior->h = iph;
		}

		interior->do_layout();
	}

	virtual void alter_focus_intention(focus_intention_t &intention) OVERRIDE {

		_MY_ASSERT(intention.current, return );
		if (!intention.next) return;

		gobject_t *p = dynamic_cast<gobject_t*>(intention.current);
		while (p && p->parent != interior) {
			p = p->parent;
		}
		_MY_ASSERT(p, return );

		p = dynamic_cast<gobject_t*>(intention.next);
		while (p && p->parent != interior) {
			p = p->parent;
		}
		if (!p) { // менеджер фокуса собирается перейти на чужой объект
			// проверим, есть ли возможность перейти на обьект внутри нашего inerior
			focus_client_t *next = focus_manager_t::instance.locate_next(intention.direction, interior);
			if (next) {
				intention.next = next;
			}
		}

	}
};



namespace globals {
	namespace dialogs {

		extern void init();

		extern void show(gobject_t *dlg);
	}
}



} // ns
#endif
