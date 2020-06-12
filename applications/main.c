/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */
#define USE_HAL_DRIVER
#include <stdlib.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "dac_drv.h"
#include "sys_conf.h"
#include "at_device.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include "pb_drv.h"
#include "led_drv.h"

/* defined the LED3 pin: PA3 */
#define LED3_PIN    GET_PIN(B, 15)

#define FAN_CTRL       GET_PIN(A, 12)

#define TEMP_HYST   20

static rt_timer_t fan_pwm_tm;

static void fan_pwm_to(void *parameter)
{
    extern sys_reg_st  g_sys;
    static uint16_t tim_cnt = 0;
    static uint8_t state = 0;

    if((g_sys.stat.mcu_temp > (g_sys.conf.fan_start_temp+TEMP_HYST))&&(g_sys.conf.fan_en == 1))
        state = 1;
    else if(g_sys.stat.mcu_temp < (g_sys.conf.fan_start_temp - TEMP_HYST))
        state = 0;

    if(state == 1)
    {
        if (tim_cnt < g_sys.conf.fan_speed)
            rt_pin_write(FAN_CTRL, PIN_HIGH);
        else
            rt_pin_write(FAN_CTRL, PIN_LOW);
    }
    else
        rt_pin_write(FAN_CTRL, PIN_LOW);
//    if(tim_cnt == 10)
//        rt_kprintf("e:%d,s:%d,ts:%d,tc:%d\n",g_sys.conf.fan_en,state,g_sys.stat.mcu_temp,g_sys.conf.fan_start_temp);

    tim_cnt++;
    if(tim_cnt > 10)
        tim_cnt = 0;
}

static void fan_init(void)
{
    rt_pin_mode(FAN_CTRL, PIN_MODE_OUTPUT);
    fan_pwm_tm = rt_timer_create("fan_pwm_tm", fan_pwm_to,
                             RT_NULL, 1,
                             RT_TIMER_FLAG_PERIODIC);

    if (fan_pwm_tm != RT_NULL) rt_timer_start(fan_pwm_tm);
}

int main(void)
{
    int count = 1;
    gvar_init();
    /* set LED0 pin mode to output */
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);

    dac_init();
    pb_init();
    fan_init();

    while (count++)
    {
        in_sts_update();
        sts_led_update();

        rt_pin_write(LED3_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED3_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}

static void att(void)
{
    at_response_t resp = RT_NULL;
    resp = at_create_resp(128, 0, rt_tick_from_millisecond(10));
    at_exec_cmd(resp, "RDYexe\r\n");
    at_delete_resp(resp);
}

static void atrd(int argc, char**argv)
{
    at_response_t resp = RT_NULL;
    uint16_t reg_addr = atoi(argv[1]);
    uint16_t reg_cnt = atoi(argv[2]);

    if (argc < 3)
    {
        rt_kprintf("Please input'atrd <reg_addr> <reg_val>'\n");
        return;
    }

    resp = at_create_resp(64, 0, rt_tick_from_millisecond(10));
    at_exec_cmd(resp, "RD_REG=%d,%d\r\n",reg_addr,reg_cnt);
    at_delete_resp(resp);
}

static void atwr(int argc, char**argv)
{
    at_response_t resp = RT_NULL;
    uint16_t reg_addr = atoi(argv[1]);
    uint16_t reg_data = atoi(argv[2]);

    if (argc < 3)
    {
        rt_kprintf("Please input'atwr <reg_addr> <reg_val>'\n");
        return;
    }

    resp = at_create_resp(64, 0, rt_tick_from_millisecond(10));
    at_exec_cmd(resp, "WR_REG=%d,%d\r\n",reg_addr,reg_data);
    at_delete_resp(resp);
}

MSH_CMD_EXPORT(att, at test1.);
MSH_CMD_EXPORT(atrd, at rd.);
MSH_CMD_EXPORT(atwr, at wr.);
