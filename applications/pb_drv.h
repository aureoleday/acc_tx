/*
 * pb_drv.h
 *
 *  Created on: 2019Äê5ÔÂ6ÈÕ
 *      Author: Administrator
 */

#ifndef COMPONENTS_DRV_PB_DRV_H_
#define COMPONENTS_DRV_PB_DRV_H_

int pb_init(void);
//void pb_cb(void);
void pb_sts_update(void);
uint8_t in_sts_update(void);
uint8_t get_atten(void);
void pb_evt(uint16_t pin_id);
#endif /* COMPONENTS_DRV_PB_DRV_H_ */
