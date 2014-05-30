/*
Сгенерированные описания виджетов меню
*/
#ifndef _MENU_H
#define _MENU_H

// Генерируем все параметры, определяем их тип
	
	
	
	
		
class dme_ui_t : public menu_item_t {
	typedef menu_item_t super;
public:
	textbox_t ch;
	combobox_t sfx;
	
	stack_layout_t stack_layout;
	preferred_stack_layout_t preferred_stack_layout;
public:
	dme_ui_t() {
		layout = &stack_layout;
		preferred_layout = &preferred_stack_layout;
	}
	
	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &ch;
		else if (prev == &ch) return &sfx;
		return 0;
	}
};
		
	
	
	

// Генерируем группы меню
class main_ui_t : public menu_t {
	typedef menu_t super;
public:
	dme_ui_t dme;
	combobox_t measure_ctl;
	combobox_t output_mode;
	textbox_t out_level;
	
	stack_layout_t stack_layout;
	preferred_stack_layout_t preferred_stack_layout;
public:
	main_ui_t() {
		layout = &stack_layout;
		preferred_layout = &preferred_stack_layout;
	}
	
	virtual gobject_t* next_all(void* prev) OVERRIDE {
		if (!prev) return &dme;
		else if (prev == &dme) return &measure_ctl;
		else if (prev == &measure_ctl) return &output_mode;
		else if (prev == &output_mode) return &out_level;
		return 0;
	}
};
#endif
