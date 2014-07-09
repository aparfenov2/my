#include "log.h"
#include <iostream>
#include <fstream>

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

const char * logger_t::_endl = "endl";