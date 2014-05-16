
#include "widgets.h"
#include "assert_impl.h"

using namespace myvi;

bool globals::gui_debug = false;
modal_overlay_t globals::modal_overlay;

u32 rasterizer_t::colors[4] = {0x00ff00, 0x0000ff, 0xff0000, 0xff00ff};
u32 rasterizer_t::deepLevel = 0;




void focus_manager_t::key_event(key_t::key_t key, gobject_t *root) {

	_MY_ASSERT(root, return);
	
	gobject_t *last_captured = captured_child;

	if (captured_child) {
		_MY_ASSERT(captured_child->visible && captured_child->enabled, return);
		captured_child->key_event(key);
		// если не освободили захват - выходим
		if (captured_child) {
			return;
		}
	}


	if (key != key_t::K_LEFT && key != key_t::K_RIGHT && key != key_t::K_UP && key != key_t::K_DOWN) {

		if (selected && selected != last_captured) {
			_MY_ASSERT(selected->visible && selected->enabled, return);
			selected->key_event(key);
		}
		return ;
	}


	gobject_t *p = root->next_selectable(0);

	gobject_t *next = 0;

	if (!selected) {
		next = p;

	} else {
		// определяем ближайшие обьекты рядом с selected
		gobject_t *next_up = 0;
		gobject_t *next_down = 0;
		gobject_t *next_left = 0;
		gobject_t *next_right = 0;

		while (p) {
			if (p == selected) {
				p = root->next_selectable(p);
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
			p = root->next_selectable(p);
		}

		if (key == key_t::K_UP) 
			next = next_up;
		if (key == key_t::K_DOWN) 
			next = next_down;
		if (key == key_t::K_LEFT) 
			next = next_left;
		if (key == key_t::K_RIGHT) 
			next = next_right;
	}

	if (next) {

		// захватываем ввод у родительского менеджера фокуса
		if (root->parent) {
			root->parent->capture_key_events(root);
		}

		if (!next->can_be_selected) {
			// если встретили обьект, который не может быть сам выбран, но содержит selectable детей
			next->focus_manager.key_event(key,next);
		}
		select(next);

	} else {
		// освобождаем захват и снимаем выделение
		if (root->parent) {
			root->parent->capture_key_events(0);
			select(0); // только если есть parent
		}
	}
}





void focus_manager_t::select(gobject_t *p) {

	//if (p) {
	//	_MY_ASSERT(p->can_be_selected,return);
	//}

	//if (p == selected)
	//	return;

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
