#ifndef __CRC16_CCITT_H__
#define __CRC16_CCITT_H__
/* =============================================================================

    Copyright (c) 2008 Pieter Conradie [www.piconomic.co.za]
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
    
    Title:          16-bit CRC calculator
    Author(s):      Pieter.Conradie
    Creation Date:  2008/11/06
    Revision Info:  $Id: crc16_ccitt.h 116 2010-06-20 21:52:14Z pieterconradie $

============================================================================= */

/** 
 *  @ingroup PROTOCOL
 *  @defgroup CRC16_CCITT crc16_ccitt.h : 16-bit CRC calculator
 *
 *  16-bit Cyclic Redundancy Check calculator for checksums.
 *  
 *  Files: crc16_ccitt.h & crc16_ccitt.h
 *  
 *  A CRC is an error-detecting code that is used for data integrity checks.
 *  
 *  @see http://en.wikipedia.org/wiki/Cyclic_redundancy_check
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"

/* _____DEFINITIONS _________________________________________________________ */
/** 
 *   The generator polynomial.
 *  
 *   CRC16-CCITT: x^16 + x^12 + x^5 + x^0
 */
#define CRC16_CCITT_POLYNOMIAL  0x8408

/// Initial CRC value
#define CRC16_CCITT_INIT_VAL    0xffff

/// Magic CRC value
#define CRC16_CCITT_MAGIC_VAL   0xf0b8

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initialise CRC lookup table.
 */
extern void  crc16_ccitt_init(void);

/** 
 * Calculate the CRC over one byte.
 * 
 * @param crc       The initial CRC to start the calculation with
 * @param data      The value to calculate the CRC over
 * 
 * @return u16_t    The resultant CRC
 */
extern u16_t crc16_ccitt_calc_byte(u16_t crc, u8_t data);

/** 
 * Calculate the CRC over a number of bytes.
 * 
 * @param crc           The initial CRC to start the calculation with
 * @param data          Pointer to the data to calculate the CRC over
 * @param data_length   The amount of bytes to calculate the CRC over
 * 
 * @return u16_t        The resultant CRC over the group of bytes
 */
extern u16_t crc16_ccitt_calc_data(u16_t crc, u8_t* data, u16_t data_length);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
