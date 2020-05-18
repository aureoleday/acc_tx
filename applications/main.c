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


int main(void)
{
    int count = 1;
    gvar_init();
    /* set LED0 pin mode to output */
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);

    dac_init();
    pb_init();

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
