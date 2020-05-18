/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-12-31     Administrator       the first version
 */
#include <stdlib.h>
#include <board.h>
#include "stm32f1xx_hal.h"
#include "sys_conf.h"

#define HWTIMER_DEV_NAME   "timer2"     /* 定时器名称 */
static rt_device_t hw_dev = RT_NULL;   /* 定时器设备句柄 */

DMA_HandleTypeDef hdma_dac_ch1;
DAC_HandleTypeDef hdac;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim2;

//const uint16_t OPWR_IND[10] =
//{
//        260, 412, 582, 713, 823, 1008, 1164, 1302, 1426, 1920
//};

//1x
//const uint16_t OPWR_IND[10] =
//{
//        353, 588, 789, 967, 1116, 1367, 1578, 1765, 1933, 2603
//};

//1.5x
const uint16_t OPWR_IND[10] =
{
        432, 683, 967, 1184, 1367, 1674, 1933, 2161, 2368, 2900
};

const uint16_t Sine12bit[256] =
{ //正弦波描点
    2048, 2098, 2148, 2198, 2248, 2298, 2348, 2398, 2447, 2496,
    2545, 2594, 2642, 2690, 2737, 2785, 2831, 2877, 2923, 2968,
    3013, 3057, 3100, 3143, 3185, 3227, 3267, 3307, 3347, 3385,
    3423, 3460, 3496, 3531, 3565, 3598, 3631, 3662, 3692, 3722,
    3750, 3778, 3804, 3829, 3854, 3877, 3899, 3920, 3940, 3958,
    3976, 3992, 4007, 4021, 4034, 4046, 4056, 4065, 4073, 4080,
    4086, 4090, 4093, 4095, 4095, 4095, 4093, 4090, 4086, 4080,
    4073, 4065, 4056, 4046, 4034, 4021, 4007, 3992, 3976, 3958,
    3940, 3920, 3899, 3877, 3854, 3829, 3804, 3778, 3750, 3722,
    3692, 3662, 3631, 3598, 3565, 3531, 3496, 3460, 3423, 3385,
    3347, 3307, 3267, 3227, 3185, 3143, 3100, 3057, 3013, 2968,
    2923, 2877, 2831, 2785, 2737, 2690, 2642, 2594, 2545, 2496,
    2447, 2398, 2348, 2298, 2248, 2198, 2148, 2098, 2047, 1997,
    1947, 1897, 1847, 1797, 1747, 1697, 1648, 1599, 1550, 1501,
    1453, 1405, 1358, 1310, 1264, 1218, 1172, 1127, 1082, 1038,
    995, 952, 910, 868, 828, 788, 748, 710, 672, 635,
    599, 564, 530, 497, 464, 433, 403, 373, 345, 317,
    291, 266, 241, 218, 196, 175, 155, 137, 119, 103,
    88, 74, 61, 49, 39, 30, 22, 15, 9, 5,
    2, 0, 0, 0, 2, 5, 9, 15, 22, 30,
    39, 49, 61, 74, 88, 103, 119, 137, 155, 175,
    196, 218, 241, 266, 291, 317, 345, 373, 403, 433,
    464, 497, 530, 564, 599, 635, 672, 710, 748, 788,
    828, 868, 910, 952, 995, 1038, 1082, 1127, 1172, 1218,
    1264, 1310, 1358, 1405, 1453, 1501, 1550, 1599, 1648, 1697,
    1747, 1797, 1847, 1897, 1947, 1997
};

int16_t sr_hwt_stop(void);

static rt_err_t dac_cb(rt_device_t dev, rt_size_t size)
{
    extern sys_reg_st  g_sys;
    static uint16_t f1_ind = 0;
    static uint16_t rect_flag=0;
    int16_t sig0=0,sig1=0;
    uint16_t sig_mix = 0;

    if(g_sys.conf.wg_mode == 0)
    {

        if(g_sys.stat.volum_index >= 8)
            sig0 = OPWR_IND[8];
        else
            sig0 = OPWR_IND[g_sys.stat.volum_index];

        if(rect_flag == 0)
        {
            sig_mix = 200;
            rect_flag = 1;
        }
        else
        {
            sig_mix = g_sys.stat.volum_reg + 200;
            rect_flag = 0;
        }

        if(g_sys.stat.out_en == 0)
        {
            if(g_sys.stat.volum_reg > 0)
                g_sys.stat.volum_reg--;
            else
                sr_hwt_stop();
        }
        else
        {
            if(g_sys.stat.volum_reg < sig0)
            {
                g_sys.stat.volum_reg += g_sys.conf.volum_step;
            }
            else if(g_sys.stat.volum_reg > sig0)
            {
                g_sys.stat.volum_reg -= g_sys.conf.volum_step;
            }
        }
    }
    else if(g_sys.conf.wg_mode == 1)
    {
        sig1 = (Sine12bit[f1_ind]-2047)>>(10-g_sys.stat.volum_index);
        sig_mix = (sig1>>(10-g_sys.stat.volum_index)) +g_sys.conf.dac_offset;
        f1_ind = (f1_ind+g_sys.stat.mul_factor)&0x00ff;
        if(g_sys.stat.out_en == 0)
            sr_hwt_stop();
    }
    else
    {
        sig_mix = 0;
    }

    HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,sig_mix);

    return 0;
}



static int hwt_init(void)
{
    rt_err_t ret = RT_EOK;
    rt_hwtimer_mode_t mode;         /* 定时器模式 */

    /* 查找定时器设备 */
    hw_dev = rt_device_find(HWTIMER_DEV_NAME);
    if (hw_dev == RT_NULL)
    {
        rt_kprintf("hwtimer sample run failed! can't find %s device!\n", HWTIMER_DEV_NAME);
        return RT_ERROR;
    }

    /* 以读写方式打开设备 */
    ret = rt_device_open(hw_dev, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("open %s device failed!\n", HWTIMER_DEV_NAME);
        return ret;
    }

    /* 设置超时回调函数 */
    rt_device_set_rx_indicate(hw_dev, dac_cb);

    /* 设置模式为周期性定时器 */
    mode = HWTIMER_MODE_PERIOD;
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_MODE_SET, &mode);
    if (ret != RT_EOK)
    {
        rt_kprintf("set mode failed! ret is :%d\n", ret);
    }
    return ret;
}

int16_t sr_hwt_start(int16_t sec,int16_t u_sec)
{
    extern sys_reg_st  g_sys;
    rt_err_t ret = RT_EOK;
    rt_hwtimerval_t timeout_s;      /* 定时器超时值 */
    /* 设置定时器超时值为u_sec us并启动定时器 */
    timeout_s.sec = sec;      /* 秒 */
    if(g_sys.conf.wg_mode == 0)
        timeout_s.usec = 500000/u_sec + 1;
    else
        timeout_s.usec = u_sec;     /* 微秒 */

    if (rt_device_write(hw_dev, 0, &timeout_s, sizeof(timeout_s)) != sizeof(timeout_s))
    {
        rt_kprintf("set timeout value failed\n");
        return RT_ERROR;
    }
    return ret;
}

int16_t sr_hwt_stop(void)
{
    rt_err_t ret = RT_EOK;
    rt_hwtimer_mode_t mode;         /* 定时器模式 */
    ret = rt_device_control(hw_dev, HWTIMER_CTRL_STOP, &mode);
    return ret;
}

static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */
  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

void dac_set(uint32_t dac_val)
{
    HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,dac_val);
}

static void dac_set_val(int argc, char**argv)
{
    if (argc < 2)
    {
        rt_kprintf("Please input'dac_set_val <dac_val>'\n");
        return;
    }
    dac_set(atoi(argv[1]));
}

int dac_start(void)
{
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    dac_set(1000);
    return 0;
}

void dac_init(void)
{
    extern sys_reg_st  g_sys;
    MX_DAC_Init();
    dac_start();
    hwt_init();
}

MSH_CMD_EXPORT(dac_set_val, set dac);
