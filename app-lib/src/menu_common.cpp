/*
 * menu_common.cpp
 *
 *  Created on: 19.06.2014
 *      Author: Администратор
 */

#include"menu_common.h"

using namespace myvi;

//myvi::menu_context_t myvi::menu_context_t::_instance;

void scrollable_window_t::accept(gobject_t* &sel) {
	// если это наш объект (или одно из детей наших детей)
	gobject_t *p = (sel);
	gobject_t *our = 0;
	while (p) {
		if (p == this) {
			our = (sel);
			break;
		}
		p = p->parent;
	}
	// то пытаемся скрыть крайние дочерние объекты чтобы объект в фокусе оставался видимым
	if (our) {
		scroll_to(our);
	}
}


void scrollable_window_t::do_layout()  {
	_MY_ASSERT(w && h, return);

	gobject_t *in = this->get_interior();
	s32 ipw,iph;
	in->get_preferred_size(ipw,iph);
	in->x = in->y = 0;
	in->w = w;
	in->h = h;

	// пока только вертикальный скролл
	//if (ipw > this->get_interior()->w) {
	//	this->get_interior()->w = ipw;
	//}
	if (iph > in->h) {
		in->h = iph;
	}

	in->do_layout();
}

void scrollable_window_t::render(surface_t &dst) {
	gobject_t *in = this->get_interior();
		
	if (in->y < 0 || in->y + in->h > this->h) {
		if (in->h == 0) {
			return;
		}
		s32 bar_height = (this->h * this->h ) / in->h;
		s32 bar_y = -in->y;

		if (bar_y < 0) {
			bar_height += bar_y;
			bar_y = 0;
		}

		if (bar_y + bar_height > this->h) {
			bar_height = this->h - bar_y;
		}

		s32 ax,ay;
		translate(ax,ay);

		dst.ctx.reset();
		dst.ctx.pen_color = 0x000000;
		dst.line(ax+this->w-1,ay+bar_y, bar_height, true);
		dst.line(ax+this->w-2,ay+bar_y, bar_height, true);
	}

}



// прокрутить чтобы выбранный children стал виден
void scrollable_window_t::scroll_to(gobject_t *interior_child) {

	gobject_t *p = interior_child->parent;
	while (p) {
		if (p == this) {
			break;
		}
		p = p->parent;
	}
	_MY_ASSERT(p, return); // проверим что дитё находится в нашей ветви

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
		get_interior()->x += dx;
		get_interior()->y += dy;
		this->dirty = true;
	}
}

void scrollable_window_t::alter_focus_intention(focus_intention_t &intention) {

	_MY_ASSERT(intention.current, return );
	if (!intention.next) return;

	gobject_t *p = (intention.current);
	while (p && p->parent != get_interior()) {
		p = p->parent;
	}
	_MY_ASSERT(p, return );

	p = (intention.next);
	while (p && p->parent != get_interior()) {
		p = p->parent;
	}
	if (!p) { // менеджер фокуса собирается перейти на чужой объект
		// проверим, есть ли возможность перейти на обьект внутри нашего inerior
		gobject_t *next = focus_manager_t::instance().locate_next(intention.direction, this->get_interior());
		if (next) {
			intention.next = next;
		}
	}

}
