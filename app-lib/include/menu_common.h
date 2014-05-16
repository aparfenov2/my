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
	menu_context_t() {
	}
public:
	static menu_context_t instance;
};




class inp_dlg_row_t : public gobject_t {
public:
	text_box_t lval;
	combo_box_t lsfx;
	stack_layout_t stack_layout;
	preferred_stack_layout_t preferred_stack_layout;
public:
	inp_dlg_row_t() {
		stack_layout.vertical = false;
		stack_layout.preferred_item_size = true;
		preferred_stack_layout.vertical = false;
		layout = &stack_layout;
		preferred_layout = &preferred_stack_layout;
	}

	virtual void init() {
		gobject_t::init();

		menu_context_t &ctx = menu_context_t::instance;

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


	virtual void child_size_changed(gobject_t *child) OVERRIDE {
		set_preferred_size();
		do_layout();
		if (parent)
			parent->child_size_changed(this);
		dirty = true;
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lval;
		else if (prev == &lval) return &lsfx;
		return 0;
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
