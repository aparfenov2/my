#include "view_factory.h"

using namespace gen;

myvi::view_factory_impl_t view_factory_impl;
myvi::view_factory_t * myvi::view_factory_t::_instance;

// ����� ������� ���� �� ��������� ��� ���������� ����
myvi::gobject_t * view_meta_t::build_view() {
	return myvi::view_factory_t::instance()->build_view(this);
}

// ����� ������� ���� �� ��������� ��� ���������� ���������
myvi::gobject_t * parameter_meta_t::build_menu_view() {
	return myvi::view_factory_t::instance()->build_menu_view(this);
}
