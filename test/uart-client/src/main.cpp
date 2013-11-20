// uart-client.cpp : Defines the entry point for the console application.
//

#define STRICT
#include <tchar.h>
#include <windows.h>
#include "Serial.h"

#include "types.h"

extern "C" {
#include "hdlc.h"
}
#include "packetizer.h"
#include "serializer.h"
#include "host_stub.h"

#include <iostream>
#include <conio.h>

#include "host_emu.h"


int ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Hello world"), MB_ICONSTOP|MB_OK);
	return 1;
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

logger_impl_t logger_impl;
logger_t *logger_t::instance = &logger_impl;
const char *logger_t::_endl = "endl";


CSerial serial;
myvi::packetizer_impl_t pkz;
myvi::host_serializer_t ser;
host_stub_t host_stub;
myvi::host_emu_t emu;

void rs485_put_char(char data) {
	LONG    lLastError = ERROR_SUCCESS;

	lLastError = serial.Write(&data, 1);

	if (lLastError != ERROR_SUCCESS) {
		ShowError(serial.GetLastError(), _T("Unable to send data"));
	}
}

// к нам пришел пакет от удаленной системы
void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received) {
	pkz.receive((const u8*)buffer,bytes_received);
}

void my_hdlc_tx_frame(const u8 *buffer, u16 bytes_to_send) {
	hdlc_tx_frame((const u8_t*)buffer, bytes_to_send);
}

myvi::key_t::key_t read_key() {
	int ch = _getch();
	switch(ch) {
	case 224:
		ch = _getch();
		switch(ch) {
		case 75 : return (myvi::key_t::K_LEFT);
		case 77 : return (myvi::key_t::K_RIGHT);
		case 72 : return (myvi::key_t::K_UP);
		case 80 : return (myvi::key_t::K_DOWN);
		}
		break;
	case 0:
		ch = _getch();
		switch(ch) {
		case 59 : return (myvi::key_t::K_F1);
		case 60 : return (myvi::key_t::K_F2);
		case 61 : return (myvi::key_t::K_F3);
		case 62 : return (myvi::key_t::K_F4);
		}
		break;
	case 13: return (myvi::key_t::K_ENTER);
	case 27: return (myvi::key_t::K_ESC);
	case 8: return (myvi::key_t::K_BKSP);
	case 83: return (myvi::key_t::K_DELETE);
	case 46: return (myvi::key_t::K_DOT);
	case 48: return (myvi::key_t::K_0);
	case 49: return (myvi::key_t::K_1);
	case 50: return (myvi::key_t::K_2);
	case 51: return (myvi::key_t::K_3);
	case 52: return (myvi::key_t::K_4);
	case 53: return (myvi::key_t::K_5);
	case 54: return (myvi::key_t::K_6);
	case 55: return (myvi::key_t::K_7);
	case 56: return (myvi::key_t::K_8);
	case 57: return (myvi::key_t::K_9);
	}
	return (myvi::key_t::key_t)0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	LONG    lLastError = ERROR_SUCCESS;

	_LOG1("MYVI UART Client Monitor");

	if (argc < 2) {
		std::cout << "arg0 : COM port name" << std::endl;
		return -1;
	}

    // Attempt to open the serial port (COM1)
    lLastError = serial.Open(argv[1],0,0,false);
//    lLastError = serial.Open(_T("\\\\.\\COM60"),0,0,false);
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

    // The serial port is now ready and we can send/receive data. If
	// the following call blocks, then the other side doesn't support
	// hardware handshaking.



 //   lLastError = serial.Write("Hello world\n");
	//if (lLastError != ERROR_SUCCESS)
	//	return ::ShowError(serial.GetLastError(), _T("Unable to send data"));


    hdlc_init(&rs485_put_char, &hdlc_on_rx_frame);
    pkz.init(&my_hdlc_tx_frame);
	ser.init(&pkz,&host_stub);
	emu.set_target(&ser);

	DWORD last_tick_count = GetTickCount();
    while (1) {
		char byte;
		u32 read = 0;

		if (_kbhit()) {
			myvi::key_t::key_t key = read_key();
			std::cout << "myvi::key_t::key_t : " << key << std::endl;
			ser.key_event(key);
		}

		lLastError = serial.Read(&byte,1,&read);

		if (lLastError != ERROR_SUCCESS) {
			ShowError(serial.GetLastError(), _T("Unable to send data"));
		}

		while (read) {
			hdlc_on_rx_byte(byte);

			lLastError = serial.Read(&byte,1,&read);

			if (lLastError != ERROR_SUCCESS) {
				ShowError(serial.GetLastError(), _T("Unable to send data"));
			}
		}
		DWORD current_tick_count = GetTickCount();
		if (last_tick_count + 1000 < current_tick_count) {
			last_tick_count = current_tick_count;
//			_LOG1("emu.update()");
			emu.update();
			std::cout << ".";
		}
		Sleep(1);
    }

    // Close the port again
    serial.Close();
	return 0;
}

