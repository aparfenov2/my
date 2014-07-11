#include "test_tools.h"

#include <tchar.h>
#include <windows.h>
#include "Serial.h"

using namespace test;
/*
static int ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Hello world"), MB_ICONSTOP|MB_OK);
	return 1;
}
*/

#define SERIAL ((CSerial*)serial)

void serial_interface_impl_t::init(serial_port_t port) {
	serial = new CSerial();

	LONG    lLastError = ERROR_SUCCESS;

    // Attempt to open the serial port (COM1)
	lLastError = SERIAL->Open((LPCTSTR)port.port_name,0,0,false);
	if (lLastError != ERROR_SUCCESS) {
		_LOG2(SERIAL->GetLastError(), ("Unable to open COM-port"));
		return;
	}

    // Setup the serial port (9600,N81) using hardware handshaking
	lLastError = SERIAL->Setup(CSerial::EBaud115200,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS) {
		_LOG2(SERIAL->GetLastError(), ("Unable to set COM-port setting"));
		return;
	}

	// Setup handshaking
	lLastError = SERIAL->SetupHandshaking(CSerial::EHandshakeOff);
	if (lLastError != ERROR_SUCCESS) {
		_LOG2(SERIAL->GetLastError(), ("Unable to set COM-port handshaking"));
		return;
	}

}

void serial_interface_impl_t::send(u8 *data, u32 len)  {
	LONG  lLastError = SERIAL->Write(data,len);
	if (lLastError != ERROR_SUCCESS) {
		_LOG2(SERIAL->GetLastError(), ("Unable to send data"));
	}
}

void serial_interface_impl_t::cycle() {
	u8 buf[1024];
	u32 read;
	LONG  lLastError = SERIAL->Read(buf, 1024, &read);
	if (lLastError != ERROR_SUCCESS) {
		_LOG2(SERIAL->GetLastError(), ("Unable to receive data"));
	} else if (read) {
		receiver->receive(buf, read);
	}
}
