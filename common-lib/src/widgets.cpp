
#include "widgets.h"
#include "assert_impl.h"

using namespace myvi;

bool globals::gui_debug = false;
modal_overlay_t globals::modal_overlay;

bool focus_manager_t::key_event(key_t::key_t key, iterator_t<gobject_t> *iter) {

	if (key != key_t::K_LEFT && key != key_t::K_RIGHT && key != key_t::K_UP && key != key_t::K_DOWN) 
		return false;


	gobject_t *p = iter->next(0);
	while (p && !p->can_be_selected) {
		p = iter->next(p);
	}
	if (!p) return false;

	if (!selected) {
		select(p);
		return true;
	}
	// определяем ближайшие обьекты рядом с selected
	gobject_t *next_up = 0;
	gobject_t *next_down = 0;
	gobject_t *next_left = 0;
	gobject_t *next_right = 0;

	p = iter->next(0);
	while (p) {
		if (p == selected || !p->can_be_selected) {
			p = iter->next(p);
			continue;
		}
		if (p->y < selected->y)
			if (!next_up || (next_up && (p->y > next_up->y)))
				next_up = p;
		if (p->y > selected->y)
			if (!next_down || (next_down && (p->y < next_down->y)))
				next_down = p;
		if (p->x < selected->x)
			if (!next_left || (next_left && (p->x > next_left->x)))
				next_left = p;
		if (p->x > selected->x)
			if (!next_right || (next_right && (p->x < next_right->x)))
				next_right = p;
		p = iter->next(p);
	}

	gobject_t *next = 0;
	if (key == key_t::K_UP) 
		next = next_up;
	if (key == key_t::K_DOWN) 
		next = next_down;
	if (key == key_t::K_LEFT) 
		next = next_left;
	if (key == key_t::K_RIGHT) 
		next = next_right;

	if (!next) return false;
	select(next);
	return true;
}

void focus_manager_t::select(gobject_t *p) {
	_MY_ASSERT(p->can_be_selected,return);

	if (p == selected)
		return;

	if (selected) {
		selected->selected = (false);
		selected->dirty = true;
	}
	selected = p;
	if (selected) {
		selected->selected = (true);
		selected->dirty = true;
	}
}
