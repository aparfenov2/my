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
    Revision Info:  $Id: hdlc.c 117 2010-06-24 20:21:28Z pieterconradie $

============================================================================= */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "hdlc.h"
#include "crc16_ccitt.h"
//#include "uart1.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
// Significant octet values
#define HDLC_FLAG_SEQUENCE  0x7e   // Flag Sequence
#define HDLC_CONTROL_ESCAPE 0x7d   // Asynchronous Control Escape
#define HDLC_ESCAPE_BIT     0x20   // Asynchronous transparency modifier

/* _____LOCAL VARIABLES______________________________________________________ */
static u8_t   hdlc_rx_frame[HDLC_MRU];
static u8_t   hdlc_rx_frame_index;
static u16_t  hdlc_rx_frame_fcs;
//static bool_t hdlc_rx_char_esc;
static u8_t hdlc_rx_char_esc;

/// Pointer to the function that will be called to send a character
static hdlc_put_char_t    hdlc_put_char;

/// Pointer to the function that will be called to handle a received HDLC frame
static hdlc_on_rx_frame_t hdlc_on_rx_frame;

/* _____PRIVATE FUNCTION PROTOTYPES__________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____PRIVATE FUNCTIONS____________________________________________________ */
/// Function to send a byte
static void hdlc_tx_byte(u8_t data)
{
    (*hdlc_put_char)(data);
}
/* _____FUNCTIONS_____________________________________________________ */
void hdlc_init(hdlc_put_char_t    put_char,
               hdlc_on_rx_frame_t on_rx_frame)
{
    hdlc_rx_frame_index = 0;
//    hdlc_rx_frame_fcs   = HDLC_INITFCS;
    hdlc_rx_frame_fcs   = CRC16_CCITT_INIT_VAL;
    hdlc_rx_char_esc    = FALSE;
    hdlc_put_char       = put_char;
    hdlc_on_rx_frame    = on_rx_frame;
}

void hdlc_on_rx_byte(u8_t data)
{
    // Start/End sequence
    if(data == HDLC_FLAG_SEQUENCE)
    {
        // If Escape sequence + End sequence is received then this packet must be silently discarded
        if(hdlc_rx_char_esc == TRUE)
        {
            hdlc_rx_char_esc = FALSE;
        }
        //  Minimum requirement for a valid frame is reception of good FCS
        else if(  (hdlc_rx_frame_index >= sizeof(hdlc_rx_frame_fcs)) 
                &&(hdlc_rx_frame_fcs   == CRC16_CCITT_MAGIC_VAL    )  )
        {
            // Pass on frame with FCS field removed
            (*hdlc_on_rx_frame)(hdlc_rx_frame,(u8_t)(hdlc_rx_frame_index-2));
        }
        // Reset for next packet
        hdlc_rx_frame_index = 0;
        hdlc_rx_frame_fcs   = CRC16_CCITT_INIT_VAL;
        return;
    }

    // Escape sequence processing
    if(hdlc_rx_char_esc)
    {
        hdlc_rx_char_esc  = FALSE;
        data             ^= HDLC_ESCAPE_BIT;
    }
    else if(data == HDLC_CONTROL_ESCAPE)
    {
        hdlc_rx_char_esc = TRUE;
        return;
    }

    // Store received data
    hdlc_rx_frame[hdlc_rx_frame_index] = data;

    // Calculate checksum
    hdlc_rx_frame_fcs = crc16_ccitt_calc_byte(hdlc_rx_frame_fcs,data);

    // Go to next position in buffer
    hdlc_rx_frame_index++;

    // Check for buffer overflow
    if(hdlc_rx_frame_index == HDLC_MRU)
    {
        // Wrap index
        hdlc_rx_frame_index  = 0;

        // Invalidate FCS so that packet will be rejected
        hdlc_rx_frame_fcs  ^= 0xFFFF;
    }
}

void hdlc_tx_frame(const u8_t *buffer, u16_t bytes_to_send)
{
    u8_t  data;
    u16_t fcs = CRC16_CCITT_INIT_VAL;    

    // Start marker
    hdlc_tx_byte(HDLC_FLAG_SEQUENCE);

    // Send escaped data
    while(bytes_to_send)
    {
        // Get next data
        data = *buffer++;
        
        // Update checksum
        fcs = crc16_ccitt_calc_byte(fcs,data);
        
        // See if data should be escaped
        if((data == HDLC_CONTROL_ESCAPE) || (data == HDLC_FLAG_SEQUENCE))
        {
            hdlc_tx_byte(HDLC_CONTROL_ESCAPE);
            data ^= HDLC_ESCAPE_BIT;
        }
        
        // Send data
        hdlc_tx_byte(data);
        
        // decrement counter
        bytes_to_send--;
    }

    // Invert checksum
    fcs ^= 0xffff;

    // Low byte of inverted FCS
    data = U16_LO8(fcs);
    if((data == HDLC_CONTROL_ESCAPE) || (data == HDLC_FLAG_SEQUENCE))
    {
        hdlc_tx_byte(HDLC_CONTROL_ESCAPE);
        data ^= HDLC_ESCAPE_BIT;
    }
    hdlc_tx_byte(data);

    // High byte of inverted FCS
    data = U16_HI8(fcs);
    if((data == HDLC_CONTROL_ESCAPE) || (data == HDLC_FLAG_SEQUENCE))
    {
        hdlc_tx_byte(HDLC_CONTROL_ESCAPE);
        data ^= HDLC_ESCAPE_BIT;
    }
    hdlc_tx_byte(data);

    // End marker
    hdlc_tx_byte(HDLC_FLAG_SEQUENCE);    
}

/* _____LOG__________________________________________________________________ */
/*

 2007-03-31 : Pieter Conradie
 - First release
   
*/
