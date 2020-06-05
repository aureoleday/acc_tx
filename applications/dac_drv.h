/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-31     Administrator       the first version
 */
#ifndef DAC_DRV_H_
#define DAC_DRV_H_

void dac_init(void);
void dac_set(uint32_t dac_val);
void dac_set_volum(void);
int16_t sr_hwt_stop(void);
int16_t sr_hwt_start(int16_t sec,int16_t u_sec);
int16_t sr_hwt_stop(void);

#endif /* DAC_DRV_H_ */
