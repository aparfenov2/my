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
/*
class scrollable_interior_t : public gobject_t {
public:
public:
};
*/

// окно с прокруткой.
class scrollable_window_t : public gobject_t, public subscriber_t<gobject_t *>, public focus_master_t {
	typedef gobject_t super;
public:
public:
	scrollable_window_t() {
		focus_manager_t::instance.subscribe(this);
	}

	gobject_t * get_interior() {
		_MY_ASSERT(this->children.size() == 1, return 0);
		return this->children[0];
	}

	// оповещение от focus_manager о выбранном объекте
	// обьект не наш, а interior !!!

	virtual void accept(gobject_t* &sel) OVERRIDE;

	// прокрутить чтобы выбранный children стал виден
	void scroll_to(gobject_t *interior_child);

	virtual void get_preferred_size(s32 &pw, s32 &ph) OVERRIDE {
		_MY_ASSERT(get_interior(),return);
		get_interior()->get_preferred_size(pw,ph);
	}

	virtual void do_layout() OVERRIDE ;

	virtual void alter_focus_intention(focus_intention_t &intention) OVERRIDE ;
};



namespace globals {
	namespace dialogs {

		extern void init();

		extern void show(gobject_t *dlg);
	}
}



} // ns
#endif
