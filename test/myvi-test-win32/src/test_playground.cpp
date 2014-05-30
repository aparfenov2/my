#include <tchar.h>
#include "surface.h"

#include "bmp_math.h"
#include "assert_impl.h"

#include "test_tools.h"
#include "resources.h"
#include "widgets.h"

#include "menu_common.h"

#include "ttcache.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

#include "disp_def.h"


using namespace myvi;

u8 buf0[BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16)];
surface_16bpp_t s1(TFT_WIDTH,TFT_HEIGHT,BMP_GET_SIZE(TFT_WIDTH,TFT_HEIGHT,16), buf0);

extern resources_t res;



class my_test_drawer_t : public test_drawer_t {
public:
	s32 kx;
	s32 ky;
	gobject_t *gobj;
public:
	my_test_drawer_t(gobject_t *agobj) {
		gobj = agobj;
		w = TFT_WIDTH * 2;
		h = TFT_HEIGHT * 2;
		kx = 2;
		ky = 2;
		std::cout << "set size to " << w << "x" << h << std::endl;
	}

	virtual bool callback(key_t::key_t key, s32 mx, s32 my, mkey_t::mkey_t mkey) OVERRIDE {
		if (key == key_t::K_SAVE) {
		}
		if (key) {
			focus_aware_t * focus_aware = dynamic_cast<focus_aware_t*>(gobj);
			if (focus_aware) {
				focus_aware->key_event((key_t::key_t)key);
			}
		}
		return rasterizer_t::render(gobj, s1);
	}

};


class logger_impl_t : public logger_t {
public:
	std::ofstream log;
public:

	logger_impl_t(std::string path):log(path) {
	}

    virtual logger_t& operator << (s32 v) OVERRIDE {
		std::cout << v;
		log << v;
//		log.flush();
        return *this;
    }

    virtual logger_t& operator << (const char *v) OVERRIDE {
		if (v == _endl) {
			std::cout << std::endl;
			log << std::endl;
		} else {
			std::cout << v;
			log << v;
		}
//		log.flush();
        return *this;
    }

};

logger_impl_t logger_impl("log.log"); 
logger_t *logger_t::instance = &logger_impl;


// Строка из текстбокса и комбобокса с суффиксами

class tedit_t : public inp_dlg_row_t {
public:

	virtual void do_layout() OVERRIDE {
		_MY_ASSERT(w && h, return);

		lval.w = (s32)(w * 0.7);
		lval.h = h;
		lsfx.h = h;
		lsfx.x = lval.w + 5;
		lsfx.w = w - lsfx.x;

		layout_children();
	}
};

// строка из названия и tedit-a справа
class menu_row_t :  public gobject_t {
public:
	label_t lname;
	tedit_t valbox;
	stack_layout_t stack_layout;
public:

	menu_row_t() {
		stack_layout.vertical = false;
		layout = &stack_layout;
	}

	virtual void init() {
		gobject_t::init();

		menu_context_t &ctx = menu_context_t::instance;

		lname.visible = true;
		lname.ctx = ctx.lctx1;
		lname.ctx.font_size = font_size_t::FS_20;

		//_MY_ASSERT(parent,return);
		//parent->capture_key_events(this);
	}

	virtual void set_dirty(bool dirty) OVERRIDE {
		_MY_ASSERT(parent,return);
		parent->dirty = dirty;
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lname;
		else if (prev == &lname) return &valbox;
		return 0;
	}

	virtual void do_layout() OVERRIDE {
		_MY_ASSERT(w && h, return);
		lname.w = (s32)(w * 0.7);
		lname.h = h;
		valbox.h = h;
		valbox.x = lname.w + 5;
		valbox.w = w - valbox.x;

		layout_children();
	}


};


// окно с прокруткой. Состоит из viewport и virtual окна. Для прокрутки сдвигаем virtual. Как наследоваться ?

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
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return interior;
		return 0;
	}

	// оповещение от focus_manager о выбранном объекте
	// обьект не наш, а interior !!!

	virtual void accept(focus_client_t* &sel) OVERRIDE {
		// если это наш объект (или одно из детей наших детей)
		gobject_t *p = next_selectable_deep(0);
		gobject_t *our = 0;
		while (p) {
			if (p == dynamic_cast<gobject_t *>(sel)) {
				our = p;
				break;
			}
			p = next_selectable_deep(p);
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

// ----------------- пример наследования окна с прокруткой --------------------------


class suffixes_t : public iterator_t<string_t> {
public:
	string_t str1;
	string_t str2;
public:
	suffixes_t() {
		str1 = string_t("A");
		str2 = string_t("B");
	}

	virtual string_t* next(void* prev) OVERRIDE {
		if (!prev) return &str1;
		if (prev == &str1) return &str2;
		return 0;
	}

};



// наследник класса внутреннего окна
class scrollable_menu_interior_t : public scrollable_interior_t {
	typedef scrollable_interior_t super;
public:
	std::vector<menu_row_t *> rows;

	suffixes_t suffixes;
	stack_layout_t stack_layout;
public:

	scrollable_menu_interior_t() {

		stack_layout.preferred_item_size = true;
		layout = &stack_layout;

		for (s32 i=0; i < 1; i++) {
			rows.push_back(new menu_row_t());
		}
	}

	void init_row(menu_row_t *row, int i) {

		std::stringstream str;
		str << "Menu_name " << i;

		row->valbox.lval.value = "Value";
		row->valbox.lsfx.values = &suffixes;
		row->valbox.lsfx.value = *suffixes.next(0);
		std::string *s = new std::string(str.str());
		row->lname.text = (*s).c_str();
	}

	virtual void init() OVERRIDE {
		super::init();

		gobject_t *p = next_visible(0);
		int i = 0;
		while (p) {
			init_row((menu_row_t *)p, i++);
			p = next_visible(p);
		}

	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		std::vector<menu_row_t *>::iterator it = rows.begin();

		if (!prev) {
			return *it;
		}

		while (it != rows.end() && *it != prev) {
			it++;
		}
		if (it != rows.end()) {
			it++;
			if (it != rows.end()) {
				return *it;
			}
		}
		return 0;
	}

};




// наследник класса окна с прокруткой
class scrollable_menu_t : public scrollable_window_t {
	typedef scrollable_window_t super;
public:
	scrollable_menu_interior_t interior;

public:
	scrollable_menu_t(): super(&interior) {
	}

	virtual void render(surface_t &dst) OVERRIDE {
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0x8a8a8a;
		s32 ax,ay;
		translate(ax,ay);
		dst.fill(ax,ay,w,h);
	}

};

/*

class tedit_bkg_t : public tedit_t {
	typedef tedit_t super;
public:
public:

	virtual void render(surface_t &dst) OVERRIDE {
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0xaaaaaa;
		s32 ax,ay;
		translate(ax,ay);
		dst.fill(ax,ay,w,h);
	}

};
*/


// ------------------------------------------------------------
// ------------ прототип поля ввода параметра -----------------
// ------------------------------------------------------------




// контроллер вида с полем ввода и комбобоксом
class tbox_cbox_controller_t  {
public:
	//units
	//validators
	//textbox*
	//combobox*
public:
};


// вид с полем ввода и комбобоксом
class tbox_cbox_view_t : public gobject_t {
public:
	text_box_t lval;
	combo_box_t lsfx;
	stack_layout_t stack_layout;
//	input_controller_t controller;
public:
	tbox_cbox_view_t() {
		stack_layout.vertical = false;
		stack_layout.preferred_item_size = true;
		layout = &stack_layout;
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


	virtual void child_request_size_change(gobject_t *child, s32 aw, s32 ah) OVERRIDE {

		get_preferred_size(aw,ah);

		if (parent) {
			parent->child_request_size_change(this, aw, ah);

		} else {
			do_layout();
			dirty = true;
		}
	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &lval;
		else if (prev == &lval) return &lsfx;
		return 0;
	}
};


// ----------------- DME - наследник вида с текстбоксом и комбобоксом ----------------------


class dme_view_t : public gobject_t {
public:
	label_t menu_label;
	tbox_cbox_view_t tbox_cbox;
public:
};


// -------- контроллеры -----------------

// Вариантный тип. Надо избавиться !
/*
class variant_t {
public:
public:
};
*/

// примесь для класса вида, позволяет находить детей по id
class view_meta_t {
public:
public:
	virtual gobject_t * get_child_by_id(u32 id) = 0; 
};

class parameter_meta_t;

// интерфейс контроллера вида
class view_controller_t {
public:
public:
	// связывание с видом. Использует RTTI для определения класса вида, см. view_meta_t
	virtual void init(gobject_t *view, parameter_meta_t *meta) = 0;
};

// метаинфа о параметре
class parameter_meta_t {
public:
	virtual void get_name(string_t &dst) = 0;
	virtual parameter_meta_t * get_children(void *prev) = 0;

	// метод фабрики вида
	virtual gobject_t * build_view() = 0;
	virtual view_controller_t * build_controller() = 0;
};


class model_t {
public:
	// обновление модели
//	virtual void update_me(parameter_meta_t *parameter_meta, variant_t &value) = 0;
};


// инициализирует поле названия элемента меню
class label_controller_t {
public:
	label_t *lab;
public:
	void init(label_t *alab, parameter_meta_t *parameter_meta) {
		lab = alab;
	}
};


// пример контроллера составного параметра
// Контроллеры статически определены в коде до генерации
class dme_controller_t {
public:
	label_controller_t lab_ctl;
	tbox_cbox_controller_t tcb_ctl;
public:
	void init(dme_view_t *dme_view, parameter_meta_t *parameter_meta) {
	}
};

// пример сериализованного параметра из схемы
class dme_parameter_t : public parameter_meta_t {
public:
public:
};

// динамический вид для фабрики видов. Позволяет динамически добавлять дочерние виды до инициализации
class dynamic_view_t : public gobject_t {
public:
public:
	void add_child(gobject_t *child) {
	}
};

// фабрика виджетов меню
class menu_factory_t {
public:
public:
	gobject_t * build_menu(parameter_meta_t * meta_root) {

		dynamic_view_t *menu_root = new dynamic_view_t();
		parameter_meta_t *child_meta = meta_root->get_children(0);

		while (child_meta) {

			gobject_t *child_view = child_meta->build_view();
			menu_root->add_child(child_view);

			view_controller_t *child_controller = child_meta->build_controller();
			child_controller->init(child_view, child_meta);

			child_meta = meta_root->get_children(child_meta);
		}
	}
};

// ------------------------------- весь экран ------------------------------------
class test_screen_t : public gobject_t, public focus_aware_t {

public:
//	tedit_t hdr_box;
	dme_view_t dme;

//	scrollable_menu_t menu;

	suffixes_t suffixes;

public:
	test_screen_t() {
	}

	virtual void init() OVERRIDE {

		w = TFT_WIDTH;
		h = TFT_HEIGHT;

		button_context_t bctx1;
		bctx1.bk_sel_color = 0x292929; // gray
		bctx1.bk_color = 0x203E95; // blue

		label_context_t lctx1;
		lctx1.sctx.pen_color = 0x010101;
		lctx1.font = &res.ttf;
		lctx1.font_size = font_size_t::FS_8;

		label_context_t lctxg;
		lctxg.sctx.pen_color = 0x010101;
		lctxg.font = &res.gly;
		lctxg.font_size = font_size_t::FS_30;

		menu_context_t::instance.bctx1 = bctx1;
		menu_context_t::instance.lctx1 = lctx1;
		menu_context_t::instance.lctxg = lctxg;

		dme.x = 0;
		dme.y = 0;
		dme.w = w;
		dme.h = 20;
		//hdr_box.lval.value = "Helo!";
		//hdr_box.lsfx.values = &suffixes;
		//hdr_box.lsfx.value = *suffixes.next(0);

		//menu.x = 0;
		//menu.y = 20;
		//menu.w = w;
		//menu.h = h/4;


		init_children();

		do_layout();
		dirty = true;

	}

	virtual void render(surface_t &dst) OVERRIDE {
		dst.ctx.alfa = 0xff;
		dst.ctx.pen_color = 0xf9f9f9;//0x203E95;
		s32 ax,ay;
		translate(ax,ay);
		dst.fill(ax,ay,w,h);

	}

	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &dme;
//		if (prev == &hdr_box) return &menu;
		return 0;
	}

};



int _tmain(int argc, _TCHAR* argv[]) {

	_LOG1("log started");

// init ttcache
	ttcache_t::init_lib();
	res.init();

	test_screen_t test_screen;

	test_screen.init(); // init whole tree

	my_test_drawer_t test_drawer(&test_screen);

	test_drawer.create_window(s1);

	bool exit = false;
	while (!exit) {
		exit = test_drawer.cycle(s1);
	}

	return 0;
}