#include <tchar.h>

#include "assert_impl.h"

#include "test_tools.h"
#include "file_system_impl.h"

#include <iostream>
#include <fstream>



#include "link_sys_impl.h"
#include "file_server.h"


using namespace link;


class host_file_intf_impl_t : 
	public link::host_system_interface_t, 
	public link::host_file_interface_t {
public:
	bool file_info_received;
	u32 cur_len;
	u32 max_len;
	u32 file_id;
	bool ack_received;
	u32 error_code;
	bool data_received;
	u8 *dst;
public:
	host_file_intf_impl_t() {
		file_info_received = false;
		cur_len = 0;
		max_len = 0;
		file_id = 0;
		ack_received = false;
		error_code = 0;
		data_received = false;
		dst = 0;
	}

	virtual void download_response (u32 file_id, u32 offset, bool first, u8 * data, u32 len) OVERRIDE {
		data_received = true;
		_MY_ASSERT(dst, return);

		memcpy(dst,data,len);
	}

	virtual void file_info_response (u32 _file_id, u32 _cur_len, u32 _max_len) OVERRIDE {
		file_info_received = true;
		file_id = _file_id;
		cur_len = _cur_len;
		max_len = _max_len;
	}

	virtual void error (u32 error) OVERRIDE {
		ack_received = true;
		error_code = error;
	}
};



#define _SER_MAX_CHART_LEN 100

int _tmain(int argc, _TCHAR* argv[]) {

	_LOG1("log started");

	_TCHAR* com_port_name = 0;
	_TCHAR* file_name = 0;
	u32 slot_id = 0;

	bool upload = false;
// parse command line
	for (s32 i=0; i < argc; i++) {
		if (_tcscmp(argv[i], _T("--com")) == 0) {
			i++;
			if (i < argc) {
				com_port_name = argv[i];
				continue;
			} else break;
		}
		if (_tcscmp(argv[i], _T("--file")) == 0) {
			i++;
			if (i < argc) {
				file_name = argv[i];
				continue;
			} else break;
		}
		if (_tcscmp(argv[i], _T("--slot")) == 0) {
			i++;
			if (i < argc) {
				slot_id = _ttoi(argv[i]);
				continue;
			} else break;
		}
		if (_tcscmp(argv[i], _T("--up")) == 0) {
			upload = true;
		}
	}

	if (!com_port_name || !file_name || !slot_id) {
		std::cout << "--com: COM port name, --file: file, --slot: slot_id, --up, --down" << std::endl;
		return 1;
	}



	test::serial_port_t port(com_port_name);
	test::serial_interface_impl_t sintf;

	if (!sintf.init(port)) {
		std::cout << "cant initialize port" << std::endl;
		return 1;
	}

	// роль хоста
	link::host_serializer_t *host_serializer = new link::host_serializer_t();
	host_serializer->init(&sintf);

	host_file_intf_impl_t hfi_impl;
	host_serializer->add_implementation(&hfi_impl);

	link::exported_file_interface_t *file_intf = host_serializer->get_exported_file_interface();

	file_intf->read_file_info(slot_id);

	while (!hfi_impl.file_info_received) {
		sintf.cycle();
	}

	std::cout << "remote info : " << "slot " << hfi_impl.file_id << " : space available : max_len " << hfi_impl.max_len << ", file_size: " << hfi_impl.cur_len << std::endl;


	if (upload) {
		// ---------------------- upload ---------------------------------
		std::ifstream infile (file_name, std::ofstream::binary);
		u32 file_sz = 0;

		if (!infile) {
			_LOG1("cant open file");
			return -1;
		} 

		infile.seekg( 0, std::ios::end );
		file_sz = (u32)infile.tellg();
		infile.seekg( 0, std::ios::beg );

		u8 *file_buf = new u8[file_sz];
		infile.read((char *)file_buf, file_sz);
		infile.close();


		std::cout << "new file_size: " << file_sz << std::endl;
		if (file_sz > hfi_impl.max_len) {
			std::cout << "not space enough" << std::endl;
			return -1;
		}


		u32 remain = file_sz;
		u32 offset = 0;
		s32 frame_count = 0;

		do {
			u32 pkt_len = _MY_MIN(remain,_SER_MAX_CHART_LEN);
			remain -= pkt_len;

			file_intf->upload_file(slot_id,offset,frame_count == 0, file_buf + offset, pkt_len);

			std::cout << "\r" << remain << "                      ";
			offset += _SER_MAX_CHART_LEN;

			frame_count++;
	// wait ACK

			while (!hfi_impl.ack_received) {
				sintf.cycle();
			}
			hfi_impl.ack_received = false;

		} while (remain);

		std::cout << std::endl << frame_count << " frames" << std::endl;
		std::cout << "update file info, file_id: " << slot_id << ", cur_len: " << file_sz << std::endl;

		file_intf->update_file_info(slot_id, file_sz);

		while (!hfi_impl.ack_received) {
			sintf.cycle();
		}
		hfi_impl.ack_received = false;

	} else {
		// ----------------------- download ------------------------------

		u32 file_sz = hfi_impl.cur_len;

		if (!file_sz) {
			std::cout << "slot " << slot_id << " have zero length" << std::endl;
			return -1;
		}

		std::ofstream outfile (file_name, std::ofstream::binary);

		if (!outfile) {
			_LOG1("cant output file");
			return -1;
		} 

		u8 *file_buf = new u8[file_sz];


		u32 remain = file_sz;
		u32 offset = 0;


		u32 frame_count = 0;

		do {
			u32 pkt_len = _MY_MIN(remain,_SER_MAX_CHART_LEN);
			remain -= pkt_len;

			file_intf->download_file(slot_id,offset,pkt_len);

			hfi_impl.dst = file_buf + offset;

			while (!hfi_impl.data_received) {
				sintf.cycle();
			}
			hfi_impl.data_received = false;


			std::cout << "\r" << remain << "                      ";
			offset += _SER_MAX_CHART_LEN;
			frame_count++;


		} while (remain);

		std::cout << std::endl << frame_count << " frames" << std::endl;


		outfile.write((char *)file_buf, file_sz);
		outfile.close();

	}


	std::cout << "done" << std::endl;

	return 0;
}

