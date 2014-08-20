#ifndef _FS_IMPL_H
#define _FS_IMPL_H

#include "file_system.h"
#include "log.h"
#include <string>
#include <fstream>
#include <sstream>

namespace test {

class file_system_impl_t : public app::file_system_t {
public:
	std::string file_dir;
public:
	void init(char *_file_dir) {
		file_dir = _file_dir;
	}

	std::string get_file_name(u32 file_id) {
		std::ostringstream oss;
		oss << file_dir << "\\" << file_id;
		return oss.str();
	}

	virtual bool read_file(u32 file_id, u32 offset, u32 len, u8 *data, u32 &read ) OVERRIDE {
		read = 0;
		std::ifstream infile(get_file_name(file_id), std::ifstream::binary);

		infile.seekg( offset, std::ios::end );
		u32 sz = (u32)infile.tellg();
		if (offset >= sz) {
			return false;
		}
		infile.seekg( offset, std::ios::beg );
		if (offset + len > sz) {
			len = sz - offset;
		}
		infile.read((char *)data, len);
		infile.close();

		read = len;
		return true;
	}

	virtual bool write_file(u32 file_id, u32 offset, u32 len, u8 *data, u32 &written ) OVERRIDE {

		written = 0;

		std::ofstream outfile (get_file_name(file_id),std::ofstream::binary | std::ofstream::app);
		outfile.seekp( 0, std::ios::end );
		u32 sz = outfile.tellp();
		if (!offset && sz) {
			_LOG1("file already exist !");
			return false;
		}
		if (offset != sz) {
			return false;
		}
		outfile.write((char *)data, len);
		outfile.close();

		written = len;
		return true;
	}

	virtual bool get_info(u32 file_id, u32 &len, u32 &max_len) OVERRIDE {

		std::ifstream infile(get_file_name(file_id), std::ofstream::binary);
		infile.seekg( 0, std::ios::end );
		len = (u32)infile.tellg();
		max_len = 0xffffffff;
		return true;
	}

	virtual bool set_info(u32 file_id, u32 len) OVERRIDE {
		return true;
	}
};

} // ns

#endif