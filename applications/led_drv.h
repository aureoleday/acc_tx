/*
 * led_drv.h
 *
 *  Created on: 2019Äê5ÔÂ6ÈÕ
 *      Author: Administrator
 */

#ifndef COMPONENTS_DRV_LED_DRV_H_
#define COMPONENTS_DRV_LED_DRV_H_
#include "sys_conf.h"
void led_init(void);
void set_bat_led(uint16_t bat_cnt);
void set_vol_led(uint16_t vol_cnt);
void set_freq_led(uint16_t freq_cnt);
void set_ind_led(uint8_t led_type, uint8_t bit_action);
void sts_led_update(void);
void led_en(uint16_t enable);
#endif /* COMPONENTS_DRV_LED_DRV_H_ */
