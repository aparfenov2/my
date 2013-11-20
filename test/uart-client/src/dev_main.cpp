// uart-client.cpp : Defines the entry point for the console application.
//

#define STRICT
#include <tchar.h>
#include <windows.h>
#include "Serial.h"

#include "types.h"

extern "C" {
#include "hdlc/hdlc.h"
}
#include "packetizer.h"
#include "serializer.h"
#include "exported_stub.h"

#include <iostream>
#include <conio.h>


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
myvi::serializer_t ser;
exported_stub_t exported_stub;

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

int _tmain(int argc, _TCHAR* argv[])
{
	LONG    lLastError = ERROR_SUCCESS;

    // Attempt to open the serial port (COM1)
    lLastError = serial.Open(_T("\\\\.\\CNCB2"),0,0,false);
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
	ser.init(&pkz,&exported_stub);

//	serial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);

    while (1) {
		char byte;
		u32 read = 0;

//TODO: добавить посылку сообщения по кнопке
		if (_kbhit()) {
			_getch();
			std::cout << "send pkt" << std::endl;
			ser.RFLevelChanged(-70);
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
		Sleep(1);
    }

    // Close the port again
    serial.Close();
	return 0;
}

