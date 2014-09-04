/*
 * funcs.h
 *
 *  Created on: 02.04.2012
 *      Author: pushkarev
 */

#ifndef SOME_FUNCS_H_
#define SOME_FUNCS_H_

#include "app_events.h"
#include "types.h"

namespace hw {

//#ifdef __cplusplus
//extern "C" {
//#endif
//
void init_zone7();
void init_pie_table();

void kbd_init();
void enc_init();
s16  enc_reset_counter();
myvi::key_t::key_t kbd_get_key();

// ��������� ���������� : 0 - ����, 1 - �������, 2 - �������
void led_init(void);
void set_led_state(u8 state);
//void init_pll(unsigned int val);
//void dsbl_dog(void);
//void init_perif_clock(void);
//void init_hardware(void);
//void initGPIO();
//void initTest();


//
//#ifdef __cplusplus
//}
//#endif
} // ns
#endif /* FUNCS_H_ */
