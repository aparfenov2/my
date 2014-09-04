/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/

#ifndef __MY_AVALON_UART_H__
#define __MY_AVALON_UART_H__

#include <stddef.h>
#include <sys/termios.h>

#include "sys/alt_warning.h"

#include "os/alt_sem.h"
#include "os/alt_flag.h"
#include "alt_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined(ALT_USE_SMALL_DRIVERS) || defined(ALTERA_AVALON_UART_SMALL)
#error "UART small driver not supported"
#else /* fast driver */

/*
 **********************************************************************
 *********************** FAST DRIVER **********************************
 **********************************************************************
 */

/*
 * ALT_AVALON_UART_READ_RDY and ALT_AVALON_UART_WRITE_RDY are the bitmasks 
 * that define uC/OS-II event flags that are releated to this device.
 *
 * ALT_AVALON_UART_READY_RDY indicates that there is read data in the buffer 
 * ready to be processed. ALT_UART_WRITE_RDY indicates that the transmitter is
 * ready for more data.
 */

#define ALT_UART_READ_RDY  0x1
#define ALT_UART_WRITE_RDY 0x2

/*
 * ALT_AVALON_UART_BUF_LEN is the length of the circular buffers used to hold
 * pending transmit and receive data. This value must be a power of two.
 */

//#define ALT_AVALON_UART_BUF_LEN (64)

/*
 * ALT_AVALON_UART_BUF_MSK is used as an internal convenience for detecting 
 * the end of the arrays used to implement the transmit and receive buffers.
 */

#define ALT_AVALON_UART_BUF_MSK(len) (len - 1)

/*
 * This is somewhat of an ugly hack, but we need some mechanism for
 * representing the non-standard 9 bit mode provided by this UART. In this
 * case we abscond with the 5 bit mode setting. The value CS5 is defined in
 * termios.h.
 */

#define CS9 CS5

/*
 * The value ALT_AVALON_UART_FB is a value set in the devices flag field to
 * indicate that the device has a fixed baud rate; i.e. if this flag is set
 * software can not control the baud rate of the device.
 */

#define ALT_AVALON_UART_FB 0x1

/*
 * The value ALT_AVALON_UART_FC is a value set in the device flag field to
 * indicate the the device is using flow control, i.e. the driver must 
 * throttle on transmit if the nCTS pin is low.
 */

#define ALT_AVALON_UART_FC 0x2

/*
 * The altera_avalon_uart_state structure is used to hold device specific data.
 * This includes the transmit and receive buffers.
 *
 * An instance of this structure is created in the auto-generated 
 * alt_sys_init.c file for each UART listed in the systems PTF file. This is
 * done using the ALTERA_AVALON_UART_STATE_INSTANCE macro given below.
 */

typedef struct my_avalon_uart_state_s
{
  void*            base;            /* The base address of the device */
  alt_u32          ctrl;            /* Shadow value of the control register */
  alt_u32          rx_start;        /* Start of the pending receive data */
  volatile alt_u32 rx_end;          /* End of the pending receive data */
  volatile alt_u32 tx_start;        /* Start of the pending transmit data */
  alt_u32          tx_end;          /* End of the pending transmit data */
  alt_u32          flags;           /* Configuation flags */
  ALT_FLAG_GRP     (events)         /* Event flags used for 
                                     * foreground/background in mult-threaded
                                     * mode */
  ALT_SEM          (read_lock)      /* Semaphore used to control access to the 
                                     * read buffer in multi-threaded mode */
  ALT_SEM          (write_lock)     /* Semaphore used to control access to the
                                     * write buffer in multi-threaded mode */
  alt_u32 rx_buf_len;
  alt_u32 tx_buf_len;
  alt_u8           *rx_buf; /* The receive buffer */
  alt_u8           *tx_buf; /* The transmit buffer */
} my_avalon_uart_state;

/*
 * Conditionally define the data structures used to process ioctl requests.
 * The following macros are defined for use in creating a device instance:
 *
 * ALTERA_AVALON_UART_TERMIOS - Initialise the termios structure used to
 *                              describe the UART configuration.
 * ALTERA_AVALON_UART_FREQ    - Initialise the 'freq' field of the device
 *                              structure, if the field exists.
 * ALTERA_AVALON_UART_IOCTL   - Initialise the 'ioctl' field of the device
 *                              callback structure, if ioctls are enabled.
 */

#ifdef ALTERA_AVALON_UART_USE_IOCTL
#error "uart ioctl not supported"
#endif /* ALTERA_AVALON_UART_USE_IOCTL */

/*
 * The macro ALTERA_AVALON_UART_INSTANCE is used by the auto-generated file
 * alt_sys_init.c to create an instance of this device driver state.
 */

#define MY_AVALON_UART_STATE_INSTANCE(name, state) \
  my_avalon_uart_state state =                     \
   {                                                   \
     (void*) name##_BASE,                              \
     0,                                                \
     0,                                                \
     0,                                                \
     0,                                                \
     0                                                \
   }

/*
 * altera_avalon_uart_init() is called by the auto-generated function 
 * alt_sys_init() for each UART in the system. This is done using the 
 * ALTERA_AVALON_UART_INIT macro given below.
 *
 * This function is responsible for performing all the run time initilisation
 * for a device instance, i.e. registering the interrupt handler, and 
 * regestering the device with the system.
 */
extern void my_avalon_uart_init(my_avalon_uart_state* sp,
                                    alt_u32 irq_controller_id, alt_u32 irq);

/*
 * The macro ALTERA_AVALON_UART_STATE_INIT is used by the auto-generated file
 * alt_sys_init.c to initialize an instance of the device driver state.
 *
 * This macro performs a sanity check to ensure that the interrupt has been
 * connected for this device. If not, then an apropriate error message is 
 * generated at build time.
 */

#define MY_AVALON_UART_STATE_INIT(name, state)                         \
  if (name##_IRQ == ALT_IRQ_NOT_CONNECTED)                                 \
  {                                                                        \
    ALT_LINK_ERROR ("Error: Interrupt not connected for " #name ". "       \
                    "You have selected the interrupt driven version of "   \
                    "the ALTERA Avalon UART driver, but the interrupt is " \
                    "not connected for this device. You can select a "     \
                    "polled mode driver by checking the 'small driver' "   \
                    "option in the HAL configuration window, or by "       \
                    "using the -DALTERA_AVALON_UART_SMALL preprocessor "   \
                    "flag.");                                              \
  }                                                                        \
  else                                                                     \
  {                                                                        \
    my_avalon_uart_init(&state, name##_IRQ_INTERRUPT_CONTROLLER_ID,    \
      name##_IRQ);                                                         \
  }

#endif /* small driver */



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ALT_AVALON_UART_H__ */
