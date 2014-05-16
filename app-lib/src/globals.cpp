#include "app_model.h"
#include "widgets.h"
#include "menu_common.h"

using namespace myvi;


const char * logger_t::_endl = "endl";

menu_context_t menu_context_t::instance;
app_model_t app_model_t::instance;

suffix_t units::time2_suffixes_t::ms = suffix_t("��",units::MS);
suffix_t units::time2_suffixes_t::sec = suffix_t("���",units::SEC);
suffix_t units::time2_suffixes_t::min = suffix_t("���",units::MIN);

