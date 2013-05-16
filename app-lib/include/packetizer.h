/*
 * hdlc_my.h
 *
 *  Created on: 14.03.2013
 *      Author: gordeev
 *      Реализация протокола HDLC с учетом u8 = u16
 */

#ifndef HDLC_MY_H_
#define HDLC_MY_H_

#include "types.h"

namespace myvi {

// здесь указатель на данные - указывает непосредственно на структуру ! Это не массив байт на платформе с28 !
class packetizer_t {
public:
	class receiver_t {
	public:
		virtual void receive(const void *data, s32 size) = 0;
	};
public:
	virtual void send(const void *data, s32 size) = 0;
	virtual void subscribe(receiver_t *receiver) = 0;
};


class packetizer_impl_t : public packetizer_t {
public:
	typedef void (*hdlc_tx_frame_t)(const u8 *buffer, u16 bytes_to_send);
private:
	receiver_t *receiver;
	hdlc_tx_frame_t hdlc_tx_frame;
#ifdef PLATFORM_C28
	u8 buf[256];
#endif
public:
	packetizer_impl_t() {
		receiver = 0;
		hdlc_tx_frame = 0;
	}

	void init(hdlc_tx_frame_t ahdlc_tx_frame) {
		hdlc_tx_frame = ahdlc_tx_frame;
	}

	virtual void send(const void *data, s32 size) OVERRIDE {
		if (hdlc_tx_frame) {
#ifdef PLATFORM_C28
			_MY_ASSERT(2*size < sizeof(buf),return);
			s32 unpacked_sz = unpack(data,size,buf);
			hdlc_tx_frame(buf,unpacked_sz);
#else
			hdlc_tx_frame((const u8 *)data,(u16)size);
#endif
		}
	}

	virtual void subscribe(receiver_t *areceiver) OVERRIDE {
		receiver = areceiver;
	}

	// получаем массив байт и превращаем в структуру
	// вызывается из
	// void hdlc_on_rx_frame(const u8_t* buffer, u16_t bytes_received)
	void receive(const u8 *data, s32 size) {
		if (receiver) {
#ifdef PLATFORM_C28
			_MY_ASSERT(size < sizeof(buf),return);
			s32 packed_sz = pack(data,size,buf);
			receiver->receive(buf,packed_sz);
#else
			receiver->receive(data,size);
#endif
		}
	}

private:
#ifdef PLATFORM_C28
	// структура в массив, sz - длина структуры, sizeof()
	s32 unpack(const void *data, s32 sz, u8 *dst) {
		s32 dw=0;
		for (s32 sw=0; sw < sz; sw++) {
			dst[dw] = __byte((int *)data,dw);
			dw++;
			dst[dw] = __byte((int *)data,dw);
			dw++;
		}
		return dw;
	}
	// массив в структуру, sz - длина массива, шт
	s32 pack(const u8 *data, s32 sz, void *dst) {
		for (s32 sw=0; sw < sz; sw++) {
			__byte((int *)dst,sw) = data[sw];
		}
		return sz/2;
	}
#endif
};

} // ns myvi

#endif /* HDLC_MY_H_ */
