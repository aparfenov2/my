/*
 * spi_async.h
 *
 *  Created on: 07.03.2013
 *      Author: gordeev
 */

#ifndef SPI_ASYNC_H_
#define SPI_ASYNC_H_

#include "types.h"
#include "assert_impl.h"
extern "C" {
#include "ring_buffer.h"
}

#define SPI_BUFF_SIZE 256

#define SPI_TXFIFO_SIZE     16
#define SPI_RXFIFO_SIZE     16

class spi_master_t {
private:
	ring_buffer_t ring_buf;
	u8 _buf[SPI_BUFF_SIZE];

	enum eChannel
	{							 //     210
		FRAM 	       = 0x00400000, // 001
		Flash		   = 0x00000000, // 000
		DDS   		   = 0x00100000, // 100
		DET2_CS 	   = 0x00500000, // 101
		DET1_CS 	   = 0x00900000, // 110
		DAC 		   = 0x00D00000, // 111
		Heterodin_L    = 0x00800000, // 010
		Heterodin_G    = 0x00C00000 // 011
	};

	eChannel channel;

public:

	void init();

	// тут же и читает сразу в кольцевой буфер
	// должно вызываться также во время простоя
	void write(u8 byte);

	u8 read() {
		u8_t ret;
		_MY_ASSERT(ring_buffer_read_byte(&ring_buf, &ret),return 0);
		return (u8)ret;
	}

	bool is_empty() {
		return ring_buffer_empty(&ring_buf);
	}


private:
    void Setup (eChannel channel);
};

#endif /* SPI_ASYNC_H_ */
