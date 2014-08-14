#include "generator_common.h"
#include "rapidxml.hpp"

using namespace gen;
using namespace rapidxml;

parameter_meta_t * parameter_meta_t::find_child_meta(myvi::string_t child_id) {

	gen::type_meta_t *type_meta = get_type_meta();
	return type_meta->find_child_meta(child_id);
}

type_meta_t * parameter_meta_t::get_type_meta() {

	myvi::string_t type_id = this->get_type_id();
	_MY_ASSERT(!type_id.is_empty(), return 0);

	gen::type_meta_t *type_meta = gen::meta_registry_t::instance().find_type_meta(type_id);
	return type_meta;
}

extern u32 parse_hex(myvi::string_t color);

template<typename T>
void emit_set_params(xml_node<> * node, T *dynamic_meta) {

	for (xml_attribute<> * attr = node->first_attribute(0); attr; attr = attr->next_attribute()) {

		do {
			char *end;
			s32 ival = std::strtol(attr->value(), &end, 10);
			if (!*end) {
				dynamic_meta->set_int_param(attr->name(),ival);
				break;
			}
			double fval = std::strtod(attr->value(), &end);
			if (!*end) {
				dynamic_meta->set_float_param(attr->name(),fval);
				break;
			}
			dynamic_meta->set_string_param(attr->name(), attr->value());
		} while(false);
	}
}

parameter_meta_t *emit_parameter_meta(xml_node<> * node) {

	dynamic_parameter_meta_t *meta = new dynamic_parameter_meta_t();
	emit_set_params<dynamic_parameter_meta_t>(node, meta);
	return meta;
}

enum_meta_t *emit_enum_meta(xml_node<> * node) {

	dynamic_enum_meta_t *meta = new dynamic_enum_meta_t();
	emit_set_params<dynamic_enum_meta_t>(node, meta);
	return meta;
}

type_meta_t * emit_type_meta(xml_node<> * node) {

	dynamic_type_meta_t *meta = new dynamic_type_meta_t();
	emit_set_params<dynamic_type_meta_t>(node, meta);

	for (xml_node<> * item = node->first_node("parameter"); item; item = item->next_sibling()) {
		meta->add_parameter(emit_parameter_meta(item));
	}

	for (xml_node<> * item = node->first_node("enum"); item; item = item->next_sibling()) {
		meta->add_enum_value(emit_enum_meta(item));
	}

	return meta;
}

view_meta_t *emit_view_meta(xml_node<> * node) {

	dynamic_view_meta_t *meta = new dynamic_view_meta_t();
	emit_set_params<dynamic_view_meta_t>(node, meta);

	for (xml_node<> * item = node->first_node("view"); item; item = item->next_sibling()) {
		meta->add_child(emit_view_meta(item));
	}
	return meta;
}

menu_meta_t *emit_menu_meta(xml_node<> * node) {

	dynamic_menu_meta_t *meta = new dynamic_menu_meta_t();
	emit_set_params<dynamic_menu_meta_t>(node, meta);

	for (xml_node<> * item = node->first_node("parameter_ref"); item; item = item->next_sibling()) {
		meta->add_child(item->first_attribute("id")->value());
	}
	return meta;
}

void meta_registry_t::init(char *xml) {

	rapidxml::xml_document<> doc;
	doc.parse<0>(xml);
	xml_node<> * root_node = doc.first_node("schema");
	_MY_ASSERT(root_node, return);

	xml_node<> * collection = root_node->first_node("colors");
	if (collection) {
		for (xml_node<> * item = collection->first_node("color"); item; item = item->next_sibling()) {
			this->colors[item->first_attribute("id")->value()] = parse_hex(item->first_attribute("hex")->value());
		}
	}
	collection = root_node->first_node("fonts");
	if (collection) {
		for (xml_node<> * item = collection->first_node("font"); item; item = item->next_sibling()) {
			this->fonts[item->first_attribute("id")->value()] = item->first_attribute("name")->value();
			if (item->first_attribute("default")) {
				this->default_font_id = item->first_attribute("id")->value();
			}
		}
	}
	collection = root_node->first_node("font_sizes");
	if (collection) {
		for (xml_node<> * item = collection->first_node("font_size"); item; item = item->next_sibling()) {
			this->font_sizes[item->first_attribute("id")->value()] = std::atoi(item->first_attribute("value")->value());
			if (item->first_attribute("default")) {
				this->default_font_size_id = item->first_attribute("id")->value();
			}
		}
	}
	collection = root_node->first_node("types");
	if (collection) {
		for (xml_node<> * item = collection->first_node("type"); item; item = item->next_sibling()) {
			this->types.push_back(emit_type_meta(item));
		}
	}
	collection = root_node->first_node("parameters");
	if (collection) {
		for (xml_node<> * item = collection->first_node("parameter"); item; item = item->next_sibling()) {
			this->parameters.push_back(emit_parameter_meta(item));
		}
	}
	collection = root_node->first_node("views");
	if (collection) {
		for (xml_node<> * item = collection->first_node("view"); item; item = item->next_sibling()) {
			this->views.push_back(emit_view_meta(item));
		}
	}
	collection = root_node->first_node("menus");
	if (collection) {
		for (xml_node<> * item = collection->first_node("menu"); item; item = item->next_sibling()) {
			this->menus.push_back(emit_menu_meta(item));
		}
	}
}
