
#include "widgets.h"
#include "assert_impl.h"
#include "math.h"

using namespace myvi;

modal_overlay_t modal_overlay_t::_instance;

u32 rasterizer_t::colors[4] = {0x00ff00, 0x0000ff, 0xff0000, 0xff00ff};
u32 rasterizer_t::deepLevel = 0;
bool rasterizer_t::debug = false;

focus_manager_t focus_manager_t::_instance;




void focus_manager_t::key_event(key_t::key_t key, gobject_t *root) {

	_MY_ASSERT(root, return);
	

	if (captured.length()) {
		gobject_t *captured_child = captured.last();
		_MY_ASSERT(captured_child && captured_child->visible && captured_child->enabled, return);

		if (captured_child != root) {
			focus_aware_t *focus_aware = dynamic_cast<focus_aware_t*>(captured_child);
			if (focus_aware) {
				focus_aware->key_event(key);
			} else {
				this->key_event(key, captured_child);
			}
			return;
		}
	}


	if (key != key_t::K_LEFT && key != key_t::K_RIGHT) {

		// проверим что selected не отпустил захавт только что (combobox_t)
		if (selected) {
			gobject_t *selected_g = (selected);
			_MY_ASSERT(selected_g && selected_g->visible && selected_g->enabled, return);
			focus_aware_t * focus_aware = dynamic_cast<focus_aware_t*>(selected);
			if (focus_aware) {
				focus_aware->key_event(key);
			}
		}
		return ;
	}


	gobject_t::iterator_selectable_deep_t iter = root->iterator_selectable_deep();
	gobject_t *p , *first = iter.next(), *pp = 0;
	bool selected_found = false;

	p = first;
	while (p) {
		if (p == selected) {
			selected_found = true;
			if (key == key_t::K_LEFT) {
				if (pp) {
					select(pp);
				}
			}
			if (key == key_t::K_RIGHT) {
				p = iter.next();
				if (p) {
					select(p);
				}
			}
			break;
		}
		pp = p;
		p = iter.next();
	}
	if (!selected_found) {
		select(first);
	}

}


void focus_manager_t::select(gobject_t *p) {


	if (selected) {
		_MY_ASSERT(dynamic_cast<focus_client_t*>(selected), return);

		dynamic_cast<focus_client_t*>(selected)->selected = false;
		selected->dirty = true;
	}

	selected = p;

	if (selected) {
		_MY_ASSERT(dynamic_cast<focus_client_t*>(selected), return);

		dynamic_cast<focus_client_t*>(selected)->selected = true;
		selected->dirty = true;
		notify(selected);
	}
}


void focus_aware_t::key_event(key_t::key_t key) {
	gobject_t * _this = dynamic_cast<gobject_t*>(this);
	_MY_ASSERT(_this && _this->visible && _this->enabled, return);
	// отдаем события детям
	focus_manager_t::instance().key_event(key, _this);
}



gobject_t * gobject_t::iterator_all_t::next() {

	if (!that) return 0;

	if (neverRun) {
		it = that->children.begin();
	}
			
	if (it != that->children.end()) {
		if (neverRun) {
			neverRun = false;
			return *it;
		}
		it++;
		if (it != that->children.end()) {
			return *it;
		}
	}
	return 0;
}


gobject_t * gobject_t::iterator_visible_deep_t::next() {

	gobject_t * p = iter_stack.last_ref()->next();
			
	do {

		if (p) {
			iterator_visible_t child_iter = p->iterator_visible();
			iter_stack.push(child_iter);
			return p;

		}

		while (!p) {
			iter_stack.pop();

			if (iter_stack.is_empty()) {
				return 0;
			}
			p = iter_stack.last_ref()->next();
		}

	} while (p);


	return p;
}


static void clear_dirty(gobject_t *p) {
	p->dirty = (false);
	gobject_t::iterator_visible_t iter = p->iterator_visible();
	gobject_t *pp = iter.next();
	while (pp) {
		clear_dirty(pp);
		pp = iter.next();
	}
}

bool rasterizer_t::render(gobject_t *p, surface_t &dst, bool force_redreaw, s32 pax, s32 pay, s32 paw, s32 pah) {
	deepLevel++;
	_MY_ASSERT(p->visible,return false);
	bool ret = false;

	if (p->dirty || force_redreaw) {
		s32 x1,y1, w1,h1;
		p->translate(x1,y1);
		w1 = p->w;
		h1 = p->h;
		if (pah >= 0) {
			if (!surface_t::trim_to(x1,y1,w1,h1, pax,pay,paw,pah)) {
				return false;
			};
		}
		pax = x1;
		pay = y1;
		paw = w1;
		pah = h1;
		dst.set_allowed_area(pax,pay,paw,pah);

		p->do_render(dst);
// DEBUG DRAW:
		if (debug) {
			dst.ctx.pen_color = colors[deepLevel & 0x03];
			if (x1 >=0 && y1>=0 && (x1+p->w-1 < dst.w) && (y1+p->h-1 < dst.h)) {
				dst.rect(x1,y1,p->w,p->h);
			}
		}

		clear_dirty(p);
		force_redreaw = true;
		ret = true;
//			return true;
	} 
	// только дочерние
	gobject_t::iterator_visible_t iter = p->iterator_visible();
	gobject_t *pp = iter.next();

	while (pp) {
		bool ret1 = render(pp, dst, force_redreaw, pax, pay, paw, pah);
		if (!ret && ret1)
			ret = true;
		pp = iter.next();
	}
	deepLevel--;
	return ret;
}



void stack_layout_t::get_preferred_size(gobject_t *parent, s32 &aw, s32 &ah) {

	gobject_t::iterator_visible_t iter = parent->iterator_visible();
	gobject_t *bt = iter.next();
	aw = 0;
	ah = 0;
	while (bt) {
		s32 btw, bth;
		bt->get_preferred_size(btw, bth);
		_MY_ASSERT(btw && bth, return);

		if (vertical) {
			if (btw > aw) {
				aw = btw;
			}
			ah += (preferred_item_size ? bth : bh) + spy;
		} else {
			if (bth > ah) {
				ah = bth;
			}
			aw += (preferred_item_size ? btw : bw) + spx;
		}
		bt = iter.next();
	}
	if (vertical && ah) ah -= spy;
	if (!vertical && aw) aw -= spx;
}

bool stack_layout_t::layout_item(gobject_t *parent ,gobject_t *child, s32 &px, s32 &py) {
	if (vertical) {
		child->x = 0;
		child->y = py;

		if (preferred_item_size) {
			s32 cw, ch;
			child->get_preferred_size(cw,ch);
			child->h = ch;
		} else child->h = bh;

		child->w = parent->w;
		py += child->h + spy;

		if (py > parent->h) {
			return false;
		}
	} else {
		child->x = px;
		child->y = 0;
		if (preferred_item_size) {
			s32 cw, ch;
			child->get_preferred_size(cw,ch);
			child->w = cw;
		} else child->w = bw;
		child->h = parent->h;
		px += child->w + spx;

		if (px > parent->w) {
			return false;
		}
	}
	return true;
}

void stack_layout_t::truncate_last(gobject_t *parent ,gobject_t *child, s32 &px, s32 &py) {
	if (vertical) {
		_MY_ASSERT(py > parent->h, return);
		py -= child->h + spy; // rewind py
		child->h = parent->h - py;
		_MY_ASSERT(child->h > 0, return );

	} else {
		_MY_ASSERT(px > parent->w, return);
		px -= child->w + spx; // rewind px
		child->w = parent->w - px;
		_MY_ASSERT(child->w > 0, return );
	}
}

bool stack_layout_t::get_size(gobject_t *parent , s32 &px, s32 &py, gobject_t::iterator_visible_t &iter) {
	gobject_t *child = iter.next();
	px = py = 0;
	while (child) {
		if (!layout_item(parent,child,px,py)) {
			return false;
		}
		child = iter.next();
	}
	return true;
}

void stack_layout_t::layout(gobject_t *parent) {

	_WEAK_ASSERT((vertical && bh) || (!vertical && bw),return);

	gobject_t::iterator_visible_t iter = parent->iterator_visible();
	gobject_t *child = iter.next();
	// подсчитаем общее количество элементов
	s32 children_total = 0;
	while (child) {
		children_total++;
		child = iter.next();
	}

	s32 px = 0, py = 0;
	s32 child_num = 0;
	iter = parent->iterator_visible();
	child = iter.next();

	while (child) {

		if (!layout_item(parent,child,px,py)) {
			truncate_last(parent,child,px,py);
			break;
		}

		if (stretch >= 0 && (stretch >= 0xff && child_num >= children_total-1 || child_num == stretch)) {
			// определим размеры оставшихся
			gobject_t::iterator_visible_t iter1 = iter;
			s32 remained_w = 0, remined_h = 0;

			if (child_num < children_total-1) {
				if (!get_size(parent,remained_w,remined_h, iter)) {
					_MY_ASSERT(0, return);
				}
			}

			iter = iter1;

			if (vertical) {
				py -= child->h;
				child->h = parent->h - remined_h - py;
				py += child->h;
				_MY_ASSERT(child->h > 0, return);
			} else {
				px -= child->w;
				child->w = parent->w - remained_w - px;
				px += child->w;
				_MY_ASSERT(child->w > 0, return);
			}
		}


		child = iter.next();
		child_num++;

	}

	if (!vertical && px) px -= spx;
	if (vertical && py) py -= spy;
	if (!((py <= parent->h) && (px <= parent->w))) {
		int i = 0;
	}
	_WEAK_ASSERT((py <= parent->h) && (px <= parent->w),return);
}


void levels_layout_t::get_preferred_size(gobject_t *parent, s32 &aw, s32 &ah)  {

	gobject_t::iterator_visible_t iter = parent->iterator_visible();
	gobject_t *child = iter.next();
	_WEAK_ASSERT(child, return);

	aw = ah = 0;
	while (child) {

		s32 cw, ch;
		child->get_preferred_size(cw, ch);
		if (aw < cw) {
			aw = cw;
		}
		ah += ch;
		child = iter.next();
	}
}


void levels_layout_t::layout(gobject_t *parent) {

	gobject_t::iterator_all_t iter = parent->iterator_all();
	gobject_t *child = iter.next();
	gobject_t *pchild = 0;
	s32 i = 0;
	while (child) {
		if (!child->visible) {
//			i++;
			child = iter.next();
			continue;
		}
		child->get_preferred_size(child->w, child->h);

		if (child->w > parent->w)
			child->w = parent->w;
		child->x = (parent->w - child->w)/2;

		_WEAK_ASSERT(i < LL_LEV_SIZE && levels[i] >= 0 && levels[i] < 100,return);
		child->y = (parent->h * levels[i])/100;
		if (pchild && pchild->y + pchild->h > child->y) {
			pchild->h = child->y - pchild->y;
			_MY_ASSERT(pchild->h >= 0,return);
		}
		i++;
		pchild = child;
		child = iter.next();
	}
	if (pchild && pchild->y + pchild->h > parent->h) {
		pchild->h = parent->h - pchild->y;
		_WEAK_ASSERT(pchild->h >= 0,return);
	}
}
