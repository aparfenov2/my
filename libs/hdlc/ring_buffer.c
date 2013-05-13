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
    
    Title:          FIFO ring buffer
    Author(s):      Pieter Conradie
    Creation Date:  2008/08/06
    Revision Info:  $Id: ring_buffer.c 118 2010-07-22 15:28:55Z pieterconradie $

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "ring_buffer.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION PROTOTYPES____________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void ring_buffer_init(ring_buffer_t *ring_buffer,
                      u8_t          *buffer,
                      size_t        buffer_size)
{
    // Initialise the ring buffer structure to be empty
    ring_buffer->start = buffer;
    ring_buffer->end   = buffer+(buffer_size-1);
    ring_buffer->head  = buffer;
    ring_buffer->tail  = buffer;
}

bool_t ring_buffer_empty(ring_buffer_t *ring_buffer)
{
    return (ring_buffer->tail == ring_buffer->head);
}

bool_t ring_buffer_full(ring_buffer_t* ring_buffer)
{
    // Calculate next position of in pointer
    u8_t *next_pos = ring_buffer->head;
    if (next_pos == ring_buffer->end)
    {
        // Wrap pointer to start of buffer
        next_pos = ring_buffer->start;
    }
    else
    {
        // Increment pointer
        next_pos++;
    }

    return(next_pos == ring_buffer->tail);
}

bool_t ring_buffer_write_byte(ring_buffer_t *ring_buffer, 
                              const u8_t    data)
{
    // Calculate next position of in pointer
    u8_t *next_pos = ring_buffer->head;
    if (next_pos == ring_buffer->end)
    {
        // Wrap pointer to start of buffer
        next_pos = ring_buffer->start;
    }
    else
    {
        // Increment pointer
        next_pos++;
    }

    // Make sure buffer is not full
    if (next_pos == ring_buffer->tail)
    {
        // Buffer is full
        return FALSE;
    }

    // Add data to buffer
    *ring_buffer->head = data;

    // Advance pointer
    ring_buffer->head = next_pos;

    return TRUE;
}

u16_t ring_buffer_write_data(ring_buffer_t *ring_buffer, 
                             const u8_t    *data, 
                             u16_t          bytes_to_write)
{
    u8_t *next_pos;
    u16_t bytes_written = 0;    

    while (bytes_to_write)
    {
        // Calculate next position of in pointer
        next_pos = ring_buffer->head;
        if (next_pos == ring_buffer->end)
        {
            // Wrap pointer to start of buffer
            next_pos = ring_buffer->start;
        }
        else
        {
            // Increment pointer
            next_pos++;
        }
        // Make sure buffer is not full
        if (next_pos == ring_buffer->tail)
        {
            // Buffer is full
            break;
        }

        // Add data to buffer
        *ring_buffer->head = *data++;

        // Advance pointer
        ring_buffer->head = next_pos;

        // Next byte
        bytes_written++;
        bytes_to_write--;
    }

    return bytes_written;
}

bool_t ring_buffer_read_byte(ring_buffer_t *ring_buffer,
                             u8_t          *data)
{
    u8_t *next_pos;

    // See if there is data in the buffer
    if (ring_buffer->head == ring_buffer->tail)
    {
        // Buffer is empty
        return FALSE;
    }

    // Fetch data
    *data = *ring_buffer->tail;

    // Calculate next position of out pointer
    next_pos = ring_buffer->tail;
    if (next_pos == ring_buffer->end)
    {
        // Wrap pointer to start of buffer
        next_pos = ring_buffer->start;
    }
    else
    {
        // Increment pointer
        next_pos++;
    }

    // Advance pointer
    ring_buffer->tail = next_pos;

    return TRUE;
}

u16_t ring_buffer_read_data(ring_buffer_t *ring_buffer,
                            u8_t          *data,
                            u16_t         bytes_to_read)
{
    u8_t  *next_pos;
    u16_t bytes_read = 0;    

    while (bytes_to_read)
    {
        // See if there is data in the buffer
        if (ring_buffer->head == ring_buffer->tail)
        {
            // Buffer is empty
            break;
        }
        // Fetch data
        *data++ = *ring_buffer->tail;

        // Calculate next position
        next_pos = ring_buffer->tail;
        if (next_pos == ring_buffer->end)
        {
            // Wrap pointer to start of buffer
            next_pos = ring_buffer->start;
        }
        else
        {
            // Increment pointer
            next_pos++;
        }

        // Advance pointer
        ring_buffer->tail = next_pos;

        // Next byte
        bytes_read++;
        bytes_to_read--;
    }

    return bytes_read;
}

/* _____LOG__________________________________________________________________ */
/*

 2008/08/06 : Pieter.Conradie
 - Created
 
 2010/07/22 : Pieter.Conradie
 - Renamed pointers "in" to "head" and "out" to "tail"
   
*/
