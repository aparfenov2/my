#include "menu.h"
#include "app_model.h"

using namespace myvi;


const char * logger_t::_endl = "endl";

menu_context_t menu_context_t::instance;
app_model_t app_model_t::instance;
menu_item_dme_t::suffixes_t menu_item_dme_t::suffixes_t::instance;
input_dialog_t input_dialog_t::instance;
dialog_t dialog_t::instance;
menu_set_i * menu_set_i::instance = NULL;

suffix_t units::time2_suffixes_t::ms = suffix_t("Ã—",units::MS);
suffix_t units::time2_suffixes_t::sec = suffix_t("—≈ ",units::SEC);
suffix_t units::time2_suffixes_t::min = suffix_t("Ã»Õ",units::MIN);

u32 menu_item_req_t::dl_12mks1_str[] = {0x002c,0x002c, 0};

// globals
//dialog_t globals::dialogs::dialog;
//input_dialog_t input_dialog_t::instance;
//app_model_t app_model_t::instance;
//msg::exported_interface_t * msg::exported_interface = &app_model_t::instance;
//menu_context_t menu_context_t::instance;

// internal statics
//menu_item_dme_t::suffixes_t menu_item_dme_t::suffixes;

