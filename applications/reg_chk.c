#include <board.h>
#include "dac_drv.h"
#include "sys_conf.h"
#include "led_drv.h"

uint16_t power_en_opt(uint16_t pram)
{
    extern sys_reg_st  g_sys;

    if((pram == 1)&&(((g_sys.stat.ms_status>>GBM_LOAD)&0x0001) == 0))
    {
        g_sys.stat.volum_reg = 0;
        sr_hwt_start(0,g_sys.stat.rect_freq);
        g_sys.stat.out_en = 1;
    }
    else
    {
        g_sys.stat.out_en = 0;
        sr_hwt_stop();
    }

    led_en(g_sys.stat.out_en);
    set_ind_led(0,!g_sys.stat.out_en);
    return 1;
}

uint16_t volum_opt(uint16_t pram)
{
    extern sys_reg_st  g_sys;
    dac_set_volum();
    g_sys.stat.volum_index = pram;
    set_vol_led(g_sys.stat.volum_index);
    return 1;
}

uint16_t rect_freq_opt(uint16_t pram)
{
    extern sys_reg_st  g_sys;
    g_sys.stat.rect_freq = pram;
    sr_hwt_stop();
    rt_thread_delay(1);
    sr_hwt_start(0,pram);
    return 1;
}


uint16_t freq_opt(uint16_t pram)
{
    extern sys_reg_st  g_sys;
    switch (pram) {
        case 0:
        {
            g_sys.stat.half_period = 27;
            g_sys.stat.mul_factor = 2;
            g_sys.stat.freq_index = 0;
            g_sys.stat.rect_freq = 290;
            break;
        }
        case 1:
        {
            g_sys.stat.half_period = 50;
            g_sys.stat.mul_factor = 6;
            g_sys.stat.freq_index = 1;
            g_sys.stat.rect_freq = 470;
            break;
        }
        case 2:
        {
            g_sys.stat.half_period = 27;
            g_sys.stat.mul_factor = 4;
            g_sys.stat.freq_index = 2;
            g_sys.stat.rect_freq = 580;
            break;
        }
        case 3:
        {
            g_sys.stat.half_period = 34;
            g_sys.stat.mul_factor = 6;
            g_sys.stat.freq_index = 3;
            g_sys.stat.rect_freq = 690;
            break;
        }
        case 4:
        {
//            g_sys.stat.half_period = 34;
//            g_sys.stat.mul_factor = 6;
            g_sys.stat.freq_index = 4;
            g_sys.stat.rect_freq = g_sys.conf.rect_freq;
            break;
        }
        default:
        {
            g_sys.stat.half_period = 50;
            g_sys.stat.mul_factor = 6;
            g_sys.stat.freq_index = 0;
            g_sys.stat.rect_freq = 470;
            break;
        }
    }

    rect_freq_opt(g_sys.stat.rect_freq);

    set_freq_led(g_sys.stat.freq_index);
    return 1;
}

uint16_t sys_reset_opt(uint16_t pram)
{  
    if(pram == 9527)
        NVIC_SystemReset();
    return 1;
}

