
#include "widgets.h"
#include "assert_impl.h"
#include "math.h"

using namespace myvi;

modal_overlay_t modal_overlay_t::_instance;

u32 rasterizer_t::colors[4] = {0x00ff00, 0x0000ff, 0xff0000, 0xff00ff};
u32 rasterizer_t::deepLevel = 0;
bool rasterizer_t::debug = false;

focus_manager_t focus_manager_t::_instance;


#define _TEXT_PADDING 2

void focus_manager_t::key_event(key_t::key_t key, gobject_t *root) {

	_MY_ASSERT(root, return);
	

	if (captured.length()) {
		focus_aware_t *captured_child = captured.last();

		gobject_t *captured_child_g = dynamic_cast<gobject_t*>(captured_child);
		_MY_ASSERT(captured_child_g && captured_child_g->visible && captured_child_g->enabled, return);

		if (captured_child_g != root) {
			captured_child->key_event(key);
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

/*
	focus_intention_t intention;

	if (key == key_t::K_UP) {
		intention.direction = direction_t::UP;
	}
	if (key == key_t::K_DOWN) {
		intention.direction = direction_t::DOWN;
	}
	if (key == key_t::K_LEFT) {
		intention.direction = direction_t::LEFT;
	}
	if (key == key_t::K_RIGHT) {
		intention.direction = direction_t::RIGHT;
	}

	intention.current = (selected);
	intention.next = locate_next(intention.direction, root);

	if (intention.next) {
		if (intention.current) {
			gobject_t *current_g = dynamic_cast<gobject_t*>(intention.current);
			_MY_ASSERT(current_g, return);

			focus_master_t *focus_master = current_g->get_focus_master();

			while (focus_master) {
				gobject_t *intention_last = intention.next;

				focus_master->alter_focus_intention(intention);
				// если фокус-мастрер что-то поменял - выбираем его предпочтение
				if (intention.next != intention_last) {
					break;
				}
				// опрашиваем фокус-мастеров верхних уровней
				current_g = dynamic_cast<gobject_t*>(focus_master);
				if (!current_g) {
					break;
				}
				focus_master = current_g->get_focus_master();
			}
		}
		select(intention.next);
	}
*/
}

// предидущее значение направления для сравнения с текущим
static direction_t::direction_t last_direction;
// текущее переназначенное направление
static direction_t::direction_t overriden_direction;


gobject_t * focus_manager_t::locate_next(direction_t::direction_t direction, gobject_t *root) {

	// определяем ближайшие обьекты рядом с selected
	gobject_t *next = 0;
	gobject_t *next_up = 0;
	gobject_t *next_down = 0;
	gobject_t *next_left = 0;
	gobject_t *next_right = 0;

	s32 sx,sy;
	gobject_t *selected_g = dynamic_cast<gobject_t*>(selected);
	selected_g->translate(sx,sy);

	double sxf = (double)sx, syf = (double)sy;
	double ud,dd,ld,rd;

	gobject_t::iterator_selectable_deep_t iter = root->iterator_selectable_deep();
	gobject_t *p =  iter.next();
	while (p) {
		if (p == selected_g) {
			p = iter.next();
			continue;
		}

		s32 px,py;
		p->translate(px,py);
		double pxf = (double)px, pyf = (double)py;
		double pxd = pxf-sxf, pyd = pyf-syf;
		double pd = sqrt(pxd*pxd + pyd*pyd);

		if (py < sy) {
			if (!next_up || (next_up && (pd < ud))) {
				next_up = p;
				ud = pd;
			}
		}
		if (py > sy) {
			if (!next_down || (next_down && (pd < dd))) {
				next_down = p;
				dd = pd;
			}
		}
		if (px < sx) {
			if (!next_left || (next_left && (pd < ld))) {
				next_left = p;
				ld = pd;
			}
		}
		if (px > sx) {
			if (!next_right || (next_right && (pd < rd))) {
				next_right = p;
				rd = pd;
			}
		}
		p = iter.next();
	}

	if (direction != last_direction) {
		last_direction = direction;
		overriden_direction = direction;
	}

	s32 owerride_cnt = 0;

	while (owerride_cnt < 2) {

		switch (overriden_direction) {

		case direction_t::LEFT: 
			next = next_left;
			if (!next) {
				overriden_direction = direction_t::UP;
				owerride_cnt++;
				continue;
			} 
			break;

		case direction_t::UP: 
			next = next_up;
			if (!next) {
				overriden_direction = direction_t::LEFT;
				owerride_cnt++;
				continue;
			} 
			break;

		case direction_t::RIGHT:
			next = next_right;
			if (!next) {
				overriden_direction = direction_t::DOWN;
				owerride_cnt++;
				continue;
			} 
			break;

		case direction_t::DOWN: 
			next = next_down;
			if (!next) {
				overriden_direction = direction_t::RIGHT;
				owerride_cnt++;
				continue;
			} 
			break;
		}
		break;
	}

	return next;

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
		_MY_ASSERT(dynamic_cast<gobject_t*>(selected), return);

		dynamic_cast<focus_client_t*>(selected)->selected = true;
		dynamic_cast<gobject_t*>(selected)->dirty = true;
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

		p->render(dst);
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

/*
void center_layout_t::layout(gobject_t *parent) {

	gobject_t::iterator_visible_t iter = parent->iterator_visible();
	gobject_t *p = iter.next();
	s32 py = 0, px = 0;
	while (p) {
		_WEAK_ASSERT(p->visible,return);
		if (vertical) {
			if (p->w > parent->w) 
				p->w = parent->w;
			p->x = (parent->w - p->w)/2;
			_WEAK_ASSERT(p->x >= 0,return);
			p->y = py;
			py += p->h + spy;
		} else {
			if (p->h > parent->h) 
				p->h = parent->h;
			p->y = (parent->h - p->h)/2;
			_WEAK_ASSERT(p->y >= 0,return);
			p->x = px;
			px += p->w + spx;
		}
		_WEAK_ASSERT(p->y + p->h <= parent->h && p->x + p->w <= parent->w,return);
		p = iter.next();
	}
	if (!vertical && px) px -= spx;
	if (vertical && py) py -= spy;
	_WEAK_ASSERT((vertical && py <= parent->h) || (!vertical && px <= parent->w),return);

	s32 dy = (parent->h - py)/2;
	s32 dx = (parent->w - px)/2;

	iter = parent->iterator_visible();
	p = p = iter.next();
	while (p) {
		if (vertical) {
			p->y += dy;
		} else {
			p->x += dx;
		}
		_WEAK_ASSERT(p->y + p->h <= parent->h && p->x + p->w <= parent->w,return);
		p = iter.next();
	}
}
*/


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


void stack_layout_t::layout(gobject_t *parent) {

	_WEAK_ASSERT((vertical && bh) || (!vertical && bw),return);

	gobject_t::iterator_visible_t iter = parent->iterator_visible();
	gobject_t *child = iter.next(), *pchild = 0;

	s32 px = 0;
	s32 py = 0;
	while (child) {
		_WEAK_ASSERT(child->visible,return);

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
				// пробуем урезать последний элемент 
				_WEAK_ASSERT(!iter.next(), break); 
				py -= child->h + spy; // rewind py
				child->h = parent->h - py;
				_MY_ASSERT(child->h > 0, break);
				break;
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
				// пробуем урезать последний элемент 
				_WEAK_ASSERT(!iter.next(), break); 
				px -= child->w + spx; // rewind py
				child->w = parent->w - px;
				_MY_ASSERT(child->w > 0, break);
				break;
			}
		}

		pchild = child;
		child = iter.next();

		if (!child && stretch_last) {
			if (vertical) {
				py -= pchild->h + spy; // rewind py
				pchild->h = parent->h - py;
			} else {
				px -= pchild->x + spx; // rewind px
				pchild->w = parent->w - px;
			}
			break;
		}
	}
	if (!vertical && px) px -= spx;
	if (vertical && py) py -= spy;
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



void button_t::render(surface_t &dst) {
	dst.ctx = ctx.sctx;
	if (!enabled) 
		dst.ctx.pen_color = ctx.bk_disabled_color;
	else if (pressed) 
		dst.ctx.pen_color = ctx.bk_pressed_color;
	else if (selected) 
		dst.ctx.pen_color = ctx.bk_sel_color;
	else 
		dst.ctx.pen_color = ctx.bk_color;

	s32 ax = 0, ay = 0;

	translate(ax,ay);

	dst.ctx.alfa = 0xff;
	dst.fill(ax,ay,w,h);

	dst.ctx.pen_color = 0xffffff;
	dst.ctx.alfa = 128;
	dst.rect(ax,ay,w,h);

}


void text_box_t::key_event(key_t::key_t key) {

	s32 cur_len = _value.length();

	if (key == key_t::K_ENTER) {
		_MY_ASSERT(parent,return);
		if (!cursor_visible) {
			this->capture_focus();
			cursor_visible = true;
		} else {
			this->release_focus();
			cursor_visible = false;
			notify(textbox_msg_t(textbox_msg_t::COMPLETE, _value));
		}
		goto lab_update_input;
	}
	// не должны редактироваться пока не перешли в активное состояние
	if (!cursor_visible) {
		return;
	}

	if (key == key_t::K_LEFT) {
		if (caret_pos) caret_pos--;
		goto lab_update_input;
	}
	if (key == key_t::K_RIGHT) {
		if (caret_pos < cur_len) caret_pos++;
		goto lab_update_input;
	}

	if ((key == key_t::K_BKSP && caret_pos) || (key == key_t::K_DELETE && caret_pos < cur_len)) {
		if (key == key_t::K_BKSP) caret_pos--;
		_value.delete_at(caret_pos);
		goto lab_update_input;
	}

	if (caret_pos < INPUT_MAX_LEN-1) {
		u8 ch = 0;
		if (key == key_t::K_0) ch = '0';
		if (key == key_t::K_1) ch = '1';
		if (key == key_t::K_2) ch = '2';
		if (key == key_t::K_3) ch = '3';
		if (key == key_t::K_4) ch = '4';
		if (key == key_t::K_5) ch = '5';
		if (key == key_t::K_6) ch = '6';
		if (key == key_t::K_7) ch = '7';
		if (key == key_t::K_8) ch = '8';
		if (key == key_t::K_9) ch = '9';
		if (key == key_t::K_DOT) ch = '.';
		if (key == key_t::K_F1) ch = '+';
		if (key == key_t::K_F2) ch = '-';
		if (ch) {
			_value.insert_at(caret_pos, ch);
			caret_pos++;
			goto lab_update_input;
		}
	}
	return;
lab_update_input:
	lab.text = _value;

	if (parent) {
		parent->child_request_size_change(this);
	}
	dirty = true;
	notify(textbox_msg_t(textbox_msg_t::EDIT, _value));
}


void text_box_t::render(surface_t &dst) {
	s32 ax, ay;
	translate(ax,ay);
	if (selected) {
		dst.ctx.reset();
		dst.ctx.pen_color = cursor_visible ? COLOR_CAPTURED : COLOR_SELECTED;
		dst.fill(ax,ay,w,h);
	}
	// draw vline
	if (cursor_visible) {
		dst.ctx.pen_color = cursor_color;
		s32 acp = 0;
		_MY_ASSERT(caret_pos <= INPUT_MAX_LEN,return);
		if (caret_pos) 
			acp += cursor_pos[caret_pos-1];

		dst.line(ax + acp, ay, lab.h,true);
	}

//		render_children(dst);
}



void combo_box_t::key_event(key_t::key_t key) {
	_MY_ASSERT(_values,return);

	if (key == key_t::K_ENTER) {
		_MY_ASSERT(parent,return);
		if (!captured) {
			this->capture_focus();
			captured = true;
		} else {
			this->release_focus();
			captured = false;
		}
		goto lab_update_cbox;
	}
	if (!captured) {
		return;
	}

	if (key == key_t::K_UP || key == key_t::K_LEFT) {
		sprev = revit.next(sprev);
		if (!sprev) {
			sprev = revit.next(sprev);
			if (!sprev)
				return;
		}
		goto lab_update_cbox;
	}
	if (key == key_t::K_DOWN || key == key_t::K_RIGHT) {
		sprev = _values->next(sprev);
		if (!sprev) {
			sprev = _values->next(sprev);
			if (!sprev)
				return;
		}
		goto lab_update_cbox;
	}
	return;

lab_update_cbox:

	if (sprev)
		_value=(sprev);

	if (_value) {
		_str_value = _value->get_string_value();;
		lab.text = _str_value;
	}
	if (parent)
		parent->child_request_size_change(this);
	dirty = true;

	notify(_value);
}



void combo_box_t::render(surface_t &dst) {
	s32 ax, ay;
	translate(ax,ay);
	if (selected) {
		dst.ctx.reset();
		dst.ctx.pen_color = captured ? COLOR_CAPTURED : COLOR_SELECTED;
		dst.fill(ax,ay,w,h);
	}
//		render_children(dst);
}


void label_t::render(surface_t &dst) {

	s32 ax = 0, ay = 0;
	_MY_ASSERT(visible && ctx.font,return);
	ctx.font->char_surface.ctx = ctx.sctx;
	ctx.font->set_char_size_px(0,ctx.font_size);

	translate(ax,ay);

	ax += _TEXT_PADDING;

	if (!text32.is_empty()) {
		// glyph string
		ctx.font->print_to(ax,ay,dst,text32);	
	} else if (!text.is_empty()) {
		// text
		ctx.font->print_to(ax,ay,dst,text);	
	} else if (this->glyph_code) {
		// glyph
		ctx.font->gly_to(ax,ay,dst,glyph_code);
	} else {
		// do nothing
	}
//		render_children(dst);
}

void label_t::get_preferred_size(s32 &aw, s32 &ah)  {

	_MY_ASSERT(visible,return);
	_MY_ASSERT(ctx.font,return);
	ctx.font->set_char_size_px(0,ctx.font_size);
	if (!text32.is_empty()) {
		// glyph string
		ctx.font->get_string_size(text32, aw, ah);
	} else if (!text.is_empty()) {
		// text
		ctx.font->get_string_size(this->text, aw, ah);
	} else if (this->glyph_code) {
		// glyph
		ctx.font->get_gly_size(this->glyph_code, aw, ah);
	} else {
		// no content - try measure something
		string_t cs1 = string_t("1");
		ctx.font->get_string_size(cs1, aw, ah);
	}

}
