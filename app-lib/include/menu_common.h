#ifndef _MENU_COMMON_H
#define _MENU_COMMON_H

#include "widgets.h"
 
namespace myvi {




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
		focus_manager_t::instance().subscribe(this);
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




} // ns
#endif
