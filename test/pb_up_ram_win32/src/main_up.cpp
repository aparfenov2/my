#include <tchar.h>

#include "types.h"

#include "assert_impl.h"

#include <iostream>
#include <fstream>

#include <windows.h>
#include "Serial.h"

extern "C" {
#include "hdlc.h"
#include "ring_buffer.h"
}

#include "myvi.pb.h"

//using namespace std;
//using namespace myvi;

CSerial serial;


int ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Hello world"), MB_ICONSTOP|MB_OK);
	return 1;
}

#define HDLC_RING_BUF_LEN 512
u8 _hdlc_ring_buf[HDLC_RING_BUF_LEN];
ring_buffer_t hdlc_ring_buf;

void rs485_put_char(char data) {
	ring_buffer_write_byte(&hdlc_ring_buf,data);
}



// к нам пришел пакет от удаленной системы
bool response_frame_received = false;
u32 max_len = 0;

void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {

	link::proto::host_interface_t host_interface;

	_WEAK_ASSERT(host_interface.ParseFromArray(buffer,bytes_received), return);

	response_frame_received = true;
	if (! host_interface.has_file_info_response()) return;

	max_len = host_interface.file_info_response().max_len();

}



class logger_impl_t : public logger_t {
public:
public:

    virtual logger_t& operator << (s32 v) OVERRIDE {
		std::cout << v;
        return *this;
    }

    virtual logger_t& operator << (const char *v) OVERRIDE {
		if (v == _endl)
			std::cout << std::endl;
		else
			std::cout << v;
        return *this;
    }

};

extern "C" {
void my_assert (const char *msg, const char *file, int line)
{
	std::cout << msg << " in " << file << " at " << line << std::endl;
}
void my_trace (const char *msg, const char *file, int line)
{
	std::cout << msg << " in " << file << " at " << line << std::endl;
}
}


logger_impl_t logger_impl; 
logger_t *logger_t::instance = &logger_impl;
const char *logger_t::_endl = "endl";

extern "C" u16_t crc16_ccitt_calc_data(u16_t crc, u8_t* data, u16_t data_length);

u8 *hdlc_buf = 0;

void send_message(link::proto::exported_interface_t &exported_interface);

int _tmain(int argc, _TCHAR* argv[]) {

	GOOGLE_PROTOBUF_VERIFY_VERSION;

	if (argc < 4) {
		std::cout << "arg0 : COM port name, arg1 : file, arg2: slot_id" << std::endl;
		return -1;
	}
//	_LOG1("log started");


	std::ifstream infile (argv[2],std::ofstream::binary);

	u32 font_sz = 0;

	u32 file_id = _wtoi(argv[3]);

	if (!infile) {
		_LOG1("cant open font cache file");
		return -1;
	} 

	infile.seekg( 0, std::ios::end );
	font_sz = (u32)infile.tellg();
	infile.seekg( 0, std::ios::beg );

	u8 *font_buf = new u8[font_sz];;
	infile.read((char *)font_buf, font_sz);
	infile.close();

	_LOG2("font cache loaded, size: ",font_sz);


	LONG    lLastError = ERROR_SUCCESS;

	_LOG1("opening COM port...");
    // Attempt to open the serial port (COM1)
    lLastError = serial.Open(argv[1],0,0,false);
	if (lLastError != ERROR_SUCCESS)
		return ::ShowError(serial.GetLastError(), _T("Unable to open COM-port"));

    // Setup the serial port (9600,N81) using hardware handshaking
	lLastError = serial.Setup(CSerial::EBaud115200,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
		return ::ShowError(serial.GetLastError(), _T("Unable to set COM-port setting"));

	// Setup handshaking
	lLastError = serial.SetupHandshaking(CSerial::EHandshakeOff);
	if (lLastError != ERROR_SUCCESS)
		return ::ShowError(serial.GetLastError(), _T("Unable to set COM-port handshaking"));

	ring_buffer_init(&hdlc_ring_buf,(u8_t*)_hdlc_ring_buf, HDLC_RING_BUF_LEN);

    hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);

#define _SER_MAX_CHART_LEN 100
#define HDLC_BUF_LEN (_SER_MAX_CHART_LEN + 100)
#define CRC_SEED 0xabba

	s32 remain = font_sz;
	s32 offset = 0;

	hdlc_buf = new u8[HDLC_BUF_LEN];

	s32 frame_count = 0;

	link::proto::exported_interface_t exported_interface;

// get file info
	exported_interface.set_read_file_info(file_id);
	send_message(exported_interface);


	u32 read = 0;
	response_frame_received = false;

	while (!response_frame_received) {
		lLastError = serial.Read(hdlc_buf,HDLC_BUF_LEN,&read);
		u32 ofs = 0;
		while (read) {
			read--;
			hdlc_on_rx_byte(hdlc_buf[ofs++]);
		}
	}
	response_frame_received = false;

	exported_interface.Clear();
	std::cout << "file slot " << file_id << " : space available : max_len " << max_len << ", file_size: " << font_sz << std::endl;
	if (font_sz > max_len) {
		std::cout << "not space enough" << std::endl;
		return -1;
	}

// upload file
	do {
		s32 pkt_len = _MY_MIN(remain,_SER_MAX_CHART_LEN);
		remain -= pkt_len;

		link::proto::file_data_t *updata = exported_interface.mutable_upload_file();

		updata->set_file_id(file_id);
		updata->set_first(frame_count == 0);
		updata->set_offset(offset);
		updata->set_data(font_buf + offset, pkt_len);
		updata->set_len(pkt_len);
		u16 crc = crc16_ccitt_calc_data(CRC_SEED,font_buf + offset, pkt_len);
		updata->set_crc(crc);

		send_message(exported_interface);

		std::cout << "\r" << remain << "                      ";
		offset += _SER_MAX_CHART_LEN;

		frame_count++;
// wait ACK

		u32 read;
		do {
			serial.Read(hdlc_buf,HDLC_BUF_LEN,&read);
		} while(read);

	} while (remain);

	std::cout << std::endl << frame_count << " frames" << std::endl;
	std::cout << "update file info, file_id: " << file_id << ", cur_len: " << font_sz << std::endl;

	exported_interface.Clear();

	link::proto::file_info_t * file_info = exported_interface.mutable_update_file_info();

	file_info->set_file_id(file_id);
	file_info->set_cur_len(font_sz);

	send_message(exported_interface);
	return 0;
}

void send_message(link::proto::exported_interface_t &exported_interface) {

	LONG    lLastError = ERROR_SUCCESS;

	_MY_ASSERT(exported_interface.SerializeToArray(hdlc_buf, HDLC_BUF_LEN),);

	u32 bytes_to_send = exported_interface.ByteSize();

	hdlc_ring_buf.tail = hdlc_ring_buf.head;
	hdlc_tx_frame((const u8_t*)hdlc_buf, bytes_to_send);

	bytes_to_send = ring_buffer_read_data(&hdlc_ring_buf,(u8_t*)hdlc_buf,HDLC_BUF_LEN);

	DWORD written;
	while (bytes_to_send) {
		lLastError = serial.Write(hdlc_buf, bytes_to_send, &written);
		bytes_to_send -= written;
	}

	if (lLastError != ERROR_SUCCESS) {
		ShowError(serial.GetLastError(), _T("Unable to send data"));
	}
}