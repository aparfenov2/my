#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__
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
    
    Title:          FIFO ring buffer
    Author(s):      Pieter Conradie
    Creation Date:  2008/08/06
    Revision Info:  $Id: ring_buffer.h 118 2010-07-22 15:28:55Z pieterconradie $

============================================================================= */

/** 
 *  @ingroup GENERAL
 *  @defgroup RING_BUFFER ring_buffer.h : FIFO ring buffer
 *
 *  A data structure that uses a single, fixed-size buffer as if it were
 *  connected end-to-end (circular).
 *  
 *  Files: ring_buffer.h & ring_buffer.c
 *  
 *  A fixed-sized buffer is managed as a FIFO buffer with a "zero-copy" policy,
 *  i.e. data is not shifted (copied) when data is removed or added to the
 *  buffer. If more data is written to the buffer than there is space for, it is
 *  ignored/discarded; no buffer overflow vulnerability.
 *  
 *  In this implementation, the maximum amount of bytes that can be stored is
 *  one less than the size of the fixed-size buffer, e.g. if the buffer size is
 *  8 bytes, then a maximum of 7 bytes can be stored.
 *  
 *  @see http://en.wikipedia.org/wiki/Circular_buffer
 *  
 *  Graphical examples:
 *  @code
 *  Buffer is empty:
 *  
 *   start       head     end
 *   |           |        |
 *  [ ][ ][ ][ ][ ][ ][ ][ ]
 *               |
 *               tail
 *  
 *  One byte is written to the buffer ('1'):
 *  
 *                  head
 *                  |
 *  [ ][ ][ ][ ][1][ ][ ][ ]
 *               |
 *               tail
 *  
 *  One byte is read ('1'); buffer is empty again:
 * 
 *                  head
 *                  |
 *  [ ][ ][ ][ ][ ][ ][ ][ ]
 *                  |
 *                  tail
 *  
 *  5 bytes are written ('2','3','4','5','6'); buffer wraps:
 *  
 *         head
 *         |
 *  [5][6][ ][ ][ ][2][3][4]
 *                  |
 *                  tail
 *  
 *  2 bytes are written ('7','8'); buffer is full:
 *  
 *               head
 *               |
 *  [5][6][7][8][ ][2][3][4]
 *                  |
 *                  tail
 *  @endcode
 *  
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdlib.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "common.h"
/* _____DEFINITIONS _________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Ring buffer structure
typedef struct
{
    u8_t *start;    ///< Pointer to first byte of fixed-size buffer
    u8_t *end;      ///< Pointer to last byte of fixed-size buffer
    u8_t *head;     ///< Pointer that is one ahead of last byte written to the buffer
    u8_t *tail;     ///< Pointer to the first byte to be read from the buffer
} ring_buffer_t;


/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/** 
 *  Initialize the ring buffer.
 * 
 *  @param ring_buffer  Pointer to the ring buffer object
 *  @param buffer       Fixed-size data buffer
 *  @param buffer_size  Fixed-size data buffer size
 */
extern void ring_buffer_init(ring_buffer_t *ring_buffer,
                             u8_t          *buffer,
                             size_t         buffer_size);

/** 
 *  See if the ring buffer is empty.
 * 
 *  @param ring_buffer  Pointer to the ring buffer object
 * 
 *  @retval TRUE        buffer is empty
 *  @retval FALSE       buffer contains data
 */
extern bool_t ring_buffer_empty(ring_buffer_t *ring_buffer);

/** 
 *  See if the ring buffer is full.
 * 
 *  @param ring_buffer  Pointer to the ring buffer object
 * 
 *  @retval TRUE        buffer is full
 *  @retval FALSE       buffer is NOT full
 */
extern bool_t ring_buffer_full(ring_buffer_t *ring_buffer);

/** 
 *  Write (store) a byte in the ring buffer.
 * 
 *  @param ring_buffer  Pointer to the ring buffer object
 *  @param data         The byte to store in the ring buffer
 * 
 *  @retval TRUE        Byte has been stored in the ring buffer
 *  @retval FALSE       Buffer is full and byte was not stored
 */
extern bool_t ring_buffer_write_byte(ring_buffer_t *ring_buffer,
                                     const u8_t    data);

/** 
 *  Write (store) data in the ring buffer
 * 
 *  @param ring_buffer      Pointer to the ring buffer object
 *  @param data             Pointer to array of data to be stored in the ring
 *                          buffer
 *  @param bytes_to_write   Amount of data bytes to be written
 * 
 *  @return u16_t           The actual number of data bytes stored, which may
 *                          be less than the number specified, because the
 *                          buffer is full.
 */
extern u16_t ring_buffer_write_data(ring_buffer_t *ring_buffer, 
                                    const u8_t    *data,
                                    u16_t         bytes_to_write);

/** 
 *  Read (retrieve) a byte from the ring buffer.
 * 
 *  @param ring_buffer     Pointer to the ring buffer object
 *  @param data            Pointer to location where byte must be stored
 * 
 *  @retval TRUE           Valid byte has been retrieved
 *  @retval FALSE          Buffer is empty
 */
extern bool_t ring_buffer_read_byte(ring_buffer_t *ring_buffer,
                                    u8_t          *data);

/** 
 * Read (retrieve) data from the ring buffer.
 * 
 * @param ring_buffer   Pointer to the ring buffer object
 * @param data          Pointer to location where data must be stored
 * @param bytes_to_read Number of bytes to retrieve    
 * 
 * @return u16_t        The actual number of bytes retrieved, which may be less 
 *                      than the number specified, because the buffer is empty.
 */
extern u16_t ring_buffer_read_data(ring_buffer_t *ring_buffer,
                                   u8_t          *data,
                                   u16_t         bytes_to_read);

/* _____MACROS_______________________________________________________________ */

/**
 *  @}
 */
#endif
