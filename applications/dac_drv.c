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

#define MUX_SEL       GET_PIN(A, 2)
#define BEEPER        GET_PIN(B, 0)

#define HWTIMER_DEV_NAME   "timer2"     /* 定时器名称 */
static rt_device_t hw_dev = RT_NULL;   /* 定时器设备句柄 */

DMA_HandleTypeDef hdma_dac_ch1;
DAC_HandleTypeDef hdac;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim2;

//1x-200DAC
//const uint16_t OPWR_IND[10] =
//{
//      //2     5    10   15   20   30    40    50    60    hl
//        203, 438, 639, 817, 966, 1217, 1428, 1615, 1783, 2453
//};

//1x-150DAC
const uint16_t OPWR_IND[5] =
{
      //2    10   20    30    50
        203, 639, 966,  1217, 1615
};

//1.5x
//const uint16_t OPWR_IND[10] =
//{
//        432, 683, 967, 1184, 1367, 1674, 1933, 2161, 2368, 2900
//};


int16_t sr_hwt_stop(void);

//static rt_err_t dac_cb(rt_device_t dev, rt_size_t size)
//{
//    extern sys_reg_st  g_sys;
//    static uint16_t f1_ind = 0;
//    static uint16_t rect_flag=0;
//    int16_t sig0=0,sig1=0;
//    uint16_t sig_mix = 0;
//
//    if(g_sys.conf.wg_mode == 0)
//    {
//
//        if(g_sys.stat.volum_index >= 8)
//            sig0 = OPWR_IND[8];
//        else
//            sig0 = OPWR_IND[g_sys.stat.volum_index];
//
//        if(rect_flag == 0)
//        {
//            sig_mix = 200;
//            rect_flag = 1;
//        }
//        else
//        {
//            sig_mix = g_sys.stat.volum_reg + 200;
//            rect_flag = 0;
//        }
//
//        if(g_sys.stat.out_en == 0)
//        {
//            if(g_sys.stat.volum_reg > 0)
//                g_sys.stat.volum_reg--;
//            else
//                sr_hwt_stop();
//        }
//        else
//        {
//            if(g_sys.stat.volum_reg < sig0)
//            {
//                g_sys.stat.volum_reg += g_sys.conf.volum_step;
//            }
//            else if(g_sys.stat.volum_reg > sig0)
//            {
//                g_sys.stat.volum_reg -= g_sys.conf.volum_step;
//            }
//        }
//    }
//    else if(g_sys.conf.wg_mode == 1)
//    {
//        sig1 = (Sine12bit[f1_ind]-2047)>>(10-g_sys.stat.volum_index);
//        sig_mix = (sig1>>(10-g_sys.stat.volum_index)) +g_sys.conf.dac_offset;
//        f1_ind = (f1_ind+g_sys.stat.mul_factor)&0x00ff;
//        if(g_sys.stat.out_en == 0)
//            sr_hwt_stop();
//    }
//    else
//    {
//        sig_mix = 0;
//    }
//
//    HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,sig_mix);
//
//    return 0;
//}

static rt_err_t dac_cb(rt_device_t dev, rt_size_t size)
{
    extern sys_reg_st  g_sys;
    static uint16_t rect_flag=0;
    if(rect_flag == 0)
    {
        rt_pin_write(MUX_SEL, PIN_HIGH);
        if(g_sys.conf.beep_en == 1)
            rt_pin_write(BEEPER, PIN_HIGH);
        rect_flag = 1;
    }
    else
    {
        rt_pin_write(MUX_SEL, PIN_LOW);
        rt_pin_write(BEEPER, PIN_LOW);
        rect_flag = 0;
    }
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
    timeout_s.usec = 500000/u_sec + 1;

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
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
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

void dac_set_volum(void)
{
    extern sys_reg_st  g_sys;
    dac_set(OPWR_IND[g_sys.stat.volum_index]);
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
    rt_pin_mode(MUX_SEL, PIN_MODE_OUTPUT);
    rt_pin_mode(BEEPER, PIN_MODE_OUTPUT);
    rt_pin_write(MUX_SEL, PIN_LOW);
    rt_pin_write(BEEPER, PIN_LOW);
    MX_DAC_Init();
    dac_start();
    hwt_init();
}

MSH_CMD_EXPORT(dac_set_val, set dac);
