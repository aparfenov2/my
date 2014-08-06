/*
 * menu_common.cpp
 *
 *  Created on: 19.06.2014
 *      Author: Администратор
 */

#include"menu_common.h"

using namespace myvi;

myvi::menu_context_t myvi::menu_context_t::_instance;

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

	s32 ipw,iph;
	get_interior()->get_preferred_size(ipw,iph);

	this->get_interior()->w = w;
	this->get_interior()->h = h;

	// пока только вертикальный скролл
	//if (ipw > this->get_interior()->w) {
	//	this->get_interior()->w = ipw;
	//}
	if (iph > this->get_interior()->h) {
		this->get_interior()->h = iph;
	}

	this->get_interior()->do_layout();
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
