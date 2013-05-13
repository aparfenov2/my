#ifndef _LOG_H
#define _LOG_H

#include "types.h"

class logger_t {
public:
	static logger_t *instance;
	static const char *_endl;
public:

  //  virtual logger_t& operator << (u32 v) {
		//if (instance) *instance << v;
  //      return *this;
  //  }

    virtual logger_t& operator << (s32 v) {
		if (instance) *instance << v;
        return *this;
    }

    virtual logger_t& operator << (const char *v) {
		if (instance) *instance << v;
        return *this;
    }

  //  virtual logger_t& operator << (const string_t &v) {
		//if (instance) *instance << v;
  //      return *this;
  //  }

};

#define _LOG1(v1) if (logger_t::instance) *logger_t::instance << (v1) << logger_t::_endl
#define _LOG2(v1,v2) if (logger_t::instance) *logger_t::instance << (v1) << (v2) << logger_t::_endl
#define _LOG3(v1,v2,v3) if (logger_t::instance) *logger_t::instance << (v1) << (v2) << (v3) << logger_t::_endl
#define _LOG4(v1,v2,v3,v4) if (logger_t::instance) *logger_t::instance << (v1) << (v2) << (v3) << (v4) << logger_t::_endl
#define _LOG5(v1,v2,v3,v4,v5) if (logger_t::instance) *logger_t::instance << (v1) << (v2) << (v3) << (v4) << (v5) << logger_t::_endl
#define _LOG6(v1,v2,v3,v4,v5,v6) if (logger_t::instance) *logger_t::instance << (v1) << (v2) << (v3) << (v4) << (v5) << (v6) << logger_t::_endl

#endif
