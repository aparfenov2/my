#ifndef __HDLC_H__
#define __HDLC_H__
/* =============================================================================

    Copyright (c) 2006 Pieter Conradie [www.piconomic.co.za]
    All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
    
    * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
    
    * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

    Title:          HDLC encapsulation layer
    Author(s):      Pieter Conradie
    Creation Date:  2007-03-31
    Revision Info:  $Id: hdlc.h 117 2010-06-24 20:21:28Z pieterconradie $

============================================================================= */
/** 
 *  @ingroup PROTOCOL
 *  @defgroup HDLC hdlc.h : HDLC encapsulation layer
 *
 *  This component encapsulates packets in HDLC-like frames.
 *
 *  Files: hdlc.h & hdlc.c
 *
 *  The purpose of this module is detect the start and end of a data packet and
 *  if any errors occured during transmission.
 * 
 *  This is accomplished appending a 16-bit CRC to the data and by adding a
 *  unique start and end marker to the packet (byte 0x7E or ASCII character '~').
 *  This allows a receiver to detect the start of a new packet, without using a
 *  timeout mechanism (no timers). This is very useful when communicating with
 *  a non real-time operating system, e.g. Windows.
 *
 *  To make sure that 0x7E only occurs to mark the valid start and end of a 
 *  packet, any 0x7E bytes in the data is replaced with a [0x7D, 0x5E] sequence
 *  (known as "escaping"). 0x5E = 0x7E xor 0x20. Any data byte 0x7D must also 
 *  be "escaped", meaning it must be replaced with a [0x7D, 0x5D] sequence. 
 *  0x5D = 0x7D xor 0x20.
 *
 *  For example, to transmit the following data:
 *  @code
 *         [0x01] [0x02] [0x7e] [0x03]
 *  @endcode
 *  The following packet will be generated:
 *  @code
 *  [0x7e] [0x01] [0x02] [0x07d] [0x5e] [0x03] [CRC-LO] [CRC-HI] [0x7e]
 *  @endcode
 * 
 *  In general any data byte can be escaped by replacing it with 0x7D and the 
 *  original data byte xor'd with 0x20 (toggling bit 5). This is useful if 
 *  software flow control is used with XON and XOFF characters and these 
 *  characters must not occur accidently in the data.
 *
 *  @par
 *  The overhead with escaping may mean that a packet may be up to 
 *  double in size if all of the data bytes is 0x7D or 0x7E, but the 
 *  probability of this sequency occuring is low.
 *
 *  @par
 *  Linking dependency to the higher communication layer is avoided by 
 *  passing a pointer to the function hdlc_init(). The pointer function 
 *  call overhead can be avoided by replacing a direct call to the 
 *  function handler if it is known at compile time. This means that 
 *  hdlc.c must be modifed to avoid a small processing overhead.
 *
 * @par Reference:
 *  - <a href="http://tools.ietf.org/html/rfc1662">RFC 1662 "PPP in HDLC-like Framing"</a>
 *  
 *  @par Example:
 *  @include hdlc_test.c
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */
#ifndef HDLC_MRU
/// Receive Packet size (Maximum Receive Unit)
#define HDLC_MRU    256
#endif

/* _____TYPE DEFINITIONS_____________________________________________________ */
/**
 * Definition for a pointer to a function that will be called to 
 * send a character
 */
typedef void (*hdlc_put_char_t)(char data);

/**
 * Definition for a pointer to a function that will be called once a frame 
 * has been received.
 */
typedef void (*hdlc_on_rx_frame_t)(const u8_t *buffer, u16_t bytes_received);

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/**
 *  Initialise HDLC encapsulation layer.
 * 
 * @param[in] put_char      Pointer to a function that will be called to 
 *                          send a character.
 * @param[in] on_rx_frame   Pointer to function that is called when a correct 
 *                          frame is received.
 */
extern void hdlc_init(hdlc_put_char_t    put_char,
                      hdlc_on_rx_frame_t on_rx_frame);

/**
 *  Function handler that is fed all raw received data.
 * 
 *  @param[in] data     received 8-bit data
 * 
 */
extern void hdlc_on_rx_byte(u8_t data);

/**
 *  Encapsulate and send an HDLC frame.
 * 
 *  @param[in] buffer         Buffer containing data for transmission
 *  @param[in] bytes_to_send  Number of bytes in buffer to be transmitted
 *
 */
extern void hdlc_tx_frame(const u8_t *buffer, u16_t bytes_to_send);

/**
 *  @}
 */
#endif
