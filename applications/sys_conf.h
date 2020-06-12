/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-08     balanceTWK   first version
 */

#ifndef __SYS_CONF__
#define __SYS_CONF__

#include <rtthread.h>
#include "sys_def.h"
//#define CONF_REG_MAP_NUM		16
//#define STAT_REG_MAP_NUM      8


typedef struct
{
    uint16_t    pwr_en;         //0:disable;1:enable
    uint16_t    freq_set;       //0:290;1:470;2:580;3:690
    uint16_t    volum_set;      //0:1/8;1:1/4;2:1/2;0:1/1;
    uint16_t    fan_en;      //0:1/8;1:1/4;2:1/2;0:1/1;
    uint16_t    high_pwr_timeout;  //0:rectangle_wave;1:sin_wave;
    uint16_t    rect_freq;      //
    uint16_t    rect_val;      //
    uint16_t    dac_offset;
    uint16_t    volum_step;
    uint16_t    beep_en;
    uint16_t    bat_mav_cnt;
    uint16_t    bat_up_lim;
    uint16_t    bat_low_lim;
    uint16_t    fan_start_temp;
    uint16_t    fan_speed;
}config_st;

typedef struct
{
    uint16_t    software_ver;
    uint16_t    hardware_ver;
    uint16_t    mcu_temp;
    uint16_t    bat_adc_raw;
    uint16_t    bat_volt;
    uint16_t    bat_volum;
    uint16_t    ms_status;      //0:uninit;1:master;2:slave
    uint16_t    half_period;
    uint16_t    mul_factor;
    uint16_t    freq_index;
    uint16_t    volum_index;
    uint16_t    out_en;
    uint16_t    rect_freq;
    uint16_t    volum_reg;
}status_st;

typedef struct
{
    config_st     conf;
    status_st     stat;	
}sys_reg_st;

typedef struct 
{
    uint16_t    id;
    uint16_t*   reg_ptr;
    int16_t     min;
    uint16_t    max;
    uint16_t    dft;
    uint8_t	    type;                       //0:RW, 1:WO
    uint16_t    (*chk_ptr)(uint16_t pram);
}conf_reg_map_st;

typedef struct 
{
    uint16_t 	id;
    uint16_t*	reg_ptr;
    uint16_t    dft;
}sts_reg_map_st;

void gvar_init(void);
uint16 reg_map_read(uint16_t reg_addr,uint16_t* reg_data,uint8_t read_cnt);
uint16 reg_map_write(uint16_t reg_addr, uint16_t *wr_data, uint8_t wr_cnt);
#endif //__SYS_CONF__
