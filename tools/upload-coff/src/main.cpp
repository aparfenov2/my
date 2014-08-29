#include <tchar.h>

#include "assert_impl.h"
#include "link_sys_impl.h"
#include "test_tools.h"


#include <string>
#include <iostream>
#include <fstream>


#include "basic.h"

extern "C" {
#include "hdlc.h"
}


#pragma pack(push,1)


typedef struct {
	u16 ver;
	u16 sec_count;
	u32 timestamp;
	u32 sym_tab_offs;
	u32 sym_count;
	u16 opt_hdr_size;
	u16 flags;
	u16 target_id;

} coff_file_header_t;

STATIC_ASSERT(sizeof(coff_file_header_t) == 22, sizeof_coff_file_header_t);


typedef struct {
	u16 magic; // 0x0108
	u16 ver;
	u32 text_size;
	u32 data_size;
	u32 bss_size;
	u32 entry_point;
	u32 text_start_addr;
	u32 data_start_addr;

} opt_file_header_t;

STATIC_ASSERT(sizeof(opt_file_header_t) == 28, sizeof_coff_file_header_t);


typedef struct {
	char name[8];
	u32 phys_addr;
	u32 virt_addr;
	u32 sec_size;
	u32 file_offs;
	u32 file_reloc_offs;
	u32 rsv;
	u32 reloc_count;
	u32 lines_count;
	u32 flags;
	u16 rsv2;
	u16 mem_page;

} section_header_t;

STATIC_ASSERT(sizeof(section_header_t) == 48, sizeof_coff_file_header_t);
#pragma pack(pop)



#define BUF_SIZE (400 * 1024)
#define CRC_SEED 0xabba
#define _SER_MAX_CHART_LEN 100

extern "C" u16_t crc16_ccitt_calc_data(u16_t crc, u8_t* data, u16_t data_length);


class host_flash_intf_impl_t : 
	public link::host_system_interface_t, 
	public link::host_flash_interface_t {
public:
	u32 define_sec_code;
	u32 post_data_code;
	u32 transfer_complete_accepted_code;
	bool define_sec_ack;
	bool post_data_ack;
	bool transfer_complete_accepted_ack;
public:
	host_flash_intf_impl_t() {
		define_sec_code = 0;
		post_data_code = 0;
		transfer_complete_accepted_code = 0;
		define_sec_ack = false;
		post_data_ack = false;
		transfer_complete_accepted_ack = false;
	}

	virtual void define_section_ack (u32 define_section_ack) OVERRIDE {
		define_sec_ack = true;
		define_sec_code = define_section_ack;
		_WEAK_ASSERT(define_section_ack == 0, return)
	}

	virtual void post_sec_data_ack (u32 post_sec_data_ack) OVERRIDE {
		post_data_ack = true;
		post_data_code = post_sec_data_ack;
		_WEAK_ASSERT(post_sec_data_ack == 0, return)
	}

	virtual void transfer_complete_accepted (u32 transfer_complete_accepted) OVERRIDE {
		transfer_complete_accepted_ack = true;
		transfer_complete_accepted_code = transfer_complete_accepted;
	}
};


int _tmain(int argc, _TCHAR* argv[]) {


	_LOG1("log started");

	_TCHAR* coff_file_name = 0;
	_TCHAR* com_port_name = 0;

// parse command line
	for (s32 i=0; i < argc; i++) {
		if (_tcscmp(argv[i], _T("--com")) == 0) {
			i++;
			if (i < argc) {
				com_port_name = argv[i];
				continue;
			} else break;
		}
		if (_tcscmp(argv[i], _T("--coff")) == 0) {
			i++;
			if (i < argc) {
				coff_file_name = argv[i];
				continue;
			} else break;
		}
	}

	if (!com_port_name || !coff_file_name) {
		std::cout << "--coff <coff file>, --com <com port>" << std::endl;
		return 1;
	}


	std::ifstream coff_file (coff_file_name, std::ofstream::binary);
	if (!coff_file) {
		_LOG1("cant open coff_file");
		return 1;
	}

	test::serial_port_t port(com_port_name);
	test::serial_interface_impl_t sintf;

	if (!sintf.init(port)) {
		std::cout << "cant initialize port" << std::endl;
		return 1;
	}

	link::host_serializer_t *host_serializer = new link::host_serializer_t();
	host_serializer->init(&sintf);

	host_flash_intf_impl_t hfi_impl;
	host_serializer->add_implementation(&hfi_impl);

	link::exported_flash_interface_t * exported_flash_interface = host_serializer->get_exported_flash_interface();

	u8 *buf = new u8[BUF_SIZE];


	coff_file_header_t coff_file_header;
	coff_file.read((char *)&coff_file_header, sizeof(coff_file_header));

	if (coff_file_header.opt_hdr_size) {
		opt_file_header_t opt_file_header;
		_WEAK_ASSERT(coff_file_header.opt_hdr_size == sizeof(opt_file_header), return 1);

		coff_file.read((char *)&opt_file_header, sizeof(opt_file_header));
		_WEAK_ASSERT(opt_file_header.magic == 0x0108, return 1);
	}

	u32 total_size = 0;

	//for (s32 i=0; i < coff_file_header.sec_count; i++) {
	//	section_header_t section_header;
	//	coff_file.read((char *)&section_header, sizeof(section_header));

	//	if (!section_header.file_offs) continue;
	//	if (section_header.virt_addr < 0x300000) continue;

	//	section_header.sec_size *= 2; // for C2800 size in words !

	//	std::cout << "write section " << section_header.name << ", sz: " << section_header.sec_size 
	//		<< ", mem_addr: " << section_header.phys_addr << ", file_addr: " << section_header.file_offs << std::endl;
	//}

	//std::string s;
	//std::getline (std::cin, s);

	//return 0;



	for (s32 i=0; i < coff_file_header.sec_count; i++) {
		section_header_t section_header;
		coff_file.read((char *)&section_header, sizeof(section_header));

		if (!section_header.file_offs) continue;
		if (section_header.virt_addr < 0x300000) continue;

		section_header.sec_size *= 2; // for C2800 size in words !

		std::cout << "write section " << section_header.name << ", sz: " << section_header.sec_size 
			<< ", mem_addr: " << section_header.virt_addr << ", file_addr: " << section_header.file_offs << std::endl;


		u32 last_file_offs = coff_file.tellg();

		coff_file.seekg(section_header.file_offs, std::ios::beg);
		_MY_ASSERT(section_header.sec_size < BUF_SIZE, break);
		coff_file.read((char *)buf, section_header.sec_size);

		coff_file.seekg(last_file_offs, std::ios::beg);

		u32 sec_crc = crc16_ccitt_calc_data(CRC_SEED,buf,section_header.sec_size);


		hfi_impl.define_sec_ack = false;
		exported_flash_interface->define_section(i, section_header.sec_size, section_header.virt_addr, sec_crc);

		while (!hfi_impl.define_sec_ack) {
			sintf.cycle();
		}

		u32 remain = section_header.sec_size;
		total_size += section_header.sec_size;
		u32 offset = 0;
		s32 frame_count = 0;

		do {
			u32 pkt_len = _MY_MIN(remain,_SER_MAX_CHART_LEN);
			remain -= pkt_len;

			hfi_impl.post_data_ack = false;
			exported_flash_interface->post_sec_data(i,pkt_len, remain, buf + offset);

			std::cout << "\r" << remain << "                      ";
			offset += pkt_len;

			frame_count++;
	// wait ACK

			while (!hfi_impl.post_data_ack) {
				sintf.cycle();
			}

		} while (remain);

		std::cout << std::endl << frame_count << " frames" << std::endl;

	}

	hfi_impl.transfer_complete_accepted_ack = false;

	exported_flash_interface->transfer_complete(coff_file_header.sec_count);

	while (!hfi_impl.transfer_complete_accepted_ack) {
		sintf.cycle();
	}

	std::cout << "bytes total: " << total_size << std::endl;

	std::cout << "done" << std::endl;

	//std::string s;
	//std::getline (std::cin, s);


	return 0;
}

