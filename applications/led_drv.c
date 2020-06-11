/*
 * led.c
 *
 *  Created on: 2019Äê5ÔÂ6ÈÕ
 *      Author: Administrator
 */
#include <stdlib.h>
#include <board.h>
#include "sys_conf.h"

#define LED_STS        GET_PIN(B, 3)
#define LED_COM        GET_PIN(B, 4)

#define LED_BAT0       GET_PIN(C, 3)
#define LED_BAT1       GET_PIN(C, 2)
#define LED_BAT2       GET_PIN(C, 1)
#define LED_BAT3       GET_PIN(C, 0)

#define LED_F0         GET_PIN(C, 10)
#define LED_F1         GET_PIN(C, 11)
#define LED_F2         GET_PIN(C, 12)
#define LED_F3         GET_PIN(A, 3)

#define LED_P0         GET_PIN(A, 5)
#define LED_P1         GET_PIN(A, 6)
#define LED_P2         GET_PIN(A, 7)
#define LED_P3         GET_PIN(A, 8)

#define FAN_CTRL       GET_PIN(A, 12)

#define     Bit_RESET		0
#define     Bit_SET		 	1	

void set_bat_led(uint16_t bat_cnt)
{
    static uint8_t bat_reg = 0;
    switch(bat_cnt)
    {
        case 0:
        {
            if(bat_reg == 0)
            {
                rt_pin_write(LED_BAT0, PIN_HIGH);
                rt_pin_write(LED_BAT1, PIN_HIGH);
                rt_pin_write(LED_BAT2, PIN_HIGH);
                rt_pin_write(LED_BAT3, PIN_LOW);
                bat_reg = 1;
            }
            else
            {
                rt_pin_write(LED_BAT0, PIN_HIGH);
                rt_pin_write(LED_BAT1, PIN_HIGH);
                rt_pin_write(LED_BAT2, PIN_HIGH);
                rt_pin_write(LED_BAT3, PIN_HIGH);
                bat_reg = 0;
            }
            break;
        }
        case 1:
        {
            rt_pin_write(LED_BAT0, PIN_HIGH);
            rt_pin_write(LED_BAT1, PIN_HIGH);
            rt_pin_write(LED_BAT2, PIN_HIGH);
            rt_pin_write(LED_BAT3, PIN_LOW);
            break;
        }
        case 2:
        {
            rt_pin_write(LED_BAT0, PIN_HIGH);
            rt_pin_write(LED_BAT1, PIN_HIGH);
            rt_pin_write(LED_BAT2, PIN_LOW);
            rt_pin_write(LED_BAT3, PIN_LOW);
            break;
        }
        case 3:
        {
            rt_pin_write(LED_BAT0, PIN_HIGH);
            rt_pin_write(LED_BAT1, PIN_LOW);
            rt_pin_write(LED_BAT2, PIN_LOW);
            rt_pin_write(LED_BAT3, PIN_LOW);
            break;
        }
        case 4:
        {
            rt_pin_write(LED_BAT0, PIN_LOW);
            rt_pin_write(LED_BAT1, PIN_LOW);
            rt_pin_write(LED_BAT2, PIN_LOW);
            rt_pin_write(LED_BAT3, PIN_LOW);
            break;
        }
        default:
        {
            rt_pin_write(LED_BAT0, PIN_HIGH);
            rt_pin_write(LED_BAT1, PIN_HIGH);
            rt_pin_write(LED_BAT2, PIN_HIGH);
            rt_pin_write(LED_BAT3, PIN_HIGH);
            break;
        }
    }
}

void set_vol_led(uint16_t vol_cnt)
{
    switch(vol_cnt)
    {
        case 1:
        {
            rt_pin_write(LED_P0, PIN_LOW);
            rt_pin_write(LED_P1, PIN_HIGH);
            rt_pin_write(LED_P2, PIN_HIGH);
            rt_pin_write(LED_P3, PIN_HIGH);
            break;
        }
        case 2:
        {
            rt_pin_write(LED_P0, PIN_HIGH);
            rt_pin_write(LED_P1, PIN_LOW);
            rt_pin_write(LED_P2, PIN_HIGH);
            rt_pin_write(LED_P3, PIN_HIGH);
            break;
        }
        case 3:
        {
            rt_pin_write(LED_P0, PIN_HIGH);
            rt_pin_write(LED_P1, PIN_HIGH);
            rt_pin_write(LED_P2, PIN_LOW);
            rt_pin_write(LED_P3, PIN_HIGH);
            break;
        }
        case 4:
        {
            rt_pin_write(LED_P0, PIN_HIGH);
            rt_pin_write(LED_P1, PIN_HIGH);
            rt_pin_write(LED_P2, PIN_HIGH);
            rt_pin_write(LED_P3, PIN_LOW);
            break;
        }
        default:
        {
            rt_pin_write(LED_P0, PIN_HIGH);
            rt_pin_write(LED_P1, PIN_HIGH);
            rt_pin_write(LED_P2, PIN_HIGH);
            rt_pin_write(LED_P3, PIN_HIGH);
            break;
        }
    }
}

static void led_set_vol(int argc, char**argv)
{
    if (argc < 2)
    {
        rt_kprintf("Please input'set_vol_led <led_hex>'\n");
        return;
    }
    set_vol_led(atoi(argv[1]));
}

void set_freq_led(uint16_t freq_cnt)
{
    switch(freq_cnt)
    {
        case 0:
        {
            rt_pin_write(LED_F0, PIN_LOW);
            rt_pin_write(LED_F1, PIN_HIGH);
            rt_pin_write(LED_F2, PIN_HIGH);
            rt_pin_write(LED_F3, PIN_HIGH);
            break;
        }
        case 1:
        {
            rt_pin_write(LED_F0, PIN_HIGH);
            rt_pin_write(LED_F1, PIN_LOW);
            rt_pin_write(LED_F2, PIN_HIGH);
            rt_pin_write(LED_F3, PIN_HIGH);
            break;
        }
        case 2:
        {
            rt_pin_write(LED_F0, PIN_HIGH);
            rt_pin_write(LED_F1, PIN_HIGH);
            rt_pin_write(LED_F2, PIN_LOW);
            rt_pin_write(LED_F3, PIN_HIGH);
            break;
        }
        case 3:
        {
            rt_pin_write(LED_F0, PIN_HIGH);
            rt_pin_write(LED_F1, PIN_HIGH);
            rt_pin_write(LED_F2, PIN_HIGH);
            rt_pin_write(LED_F3, PIN_LOW);
            break;
        }
        default:
        {
            rt_pin_write(LED_F0, PIN_HIGH);
            rt_pin_write(LED_F1, PIN_HIGH);
            rt_pin_write(LED_F2, PIN_HIGH);
            rt_pin_write(LED_F3, PIN_HIGH);
            break;
        }
    }
}

static void led_set_freq(int argc, char**argv)
{
    if (argc < 2)
    {
        rt_kprintf("Please input'led_set_freq <led_hex>'\n");
        return;
    }
    set_freq_led(atoi(argv[1]));
}

void set_ind_led(uint8_t led_type, uint8_t bit_action)
{
	if(led_type == 0)
	{
	    rt_pin_write(LED_STS, bit_action);
	}
	if(led_type == 1)
	{
	    rt_pin_write(LED_COM, bit_action);
	}
}

static void fan_ctrl(uint16_t enable)
{
    if(enable == 1)
        rt_pin_write(FAN_CTRL, 1);
    else
        rt_pin_write(FAN_CTRL, 0);
}

void sts_led_update(void)
{
    extern sys_reg_st  g_sys;

    if(g_sys.stat.out_en)
        set_ind_led(0,0);
    else
        set_ind_led(0,1);

    if(((g_sys.stat.ms_status>>GBM_LINK)&0x0001) == 1)
        set_ind_led(1,0);
    else
        set_ind_led(1,1);

    fan_ctrl(g_sys.conf.fan_en);
}

static int  led_init(void)
{
    rt_pin_mode(LED_STS, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_COM, PIN_MODE_OUTPUT);

    rt_pin_mode(LED_BAT0, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_BAT1, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_BAT2, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_BAT3, PIN_MODE_OUTPUT);

    rt_pin_mode(LED_F0, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_F1, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_F2, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_F3, PIN_MODE_OUTPUT);

    rt_pin_mode(LED_P0, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_P1, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_P2, PIN_MODE_OUTPUT);
    rt_pin_mode(LED_P3, PIN_MODE_OUTPUT);

    rt_pin_mode(FAN_CTRL, PIN_MODE_OUTPUT);

    set_freq_led(5);
    set_vol_led(0);
    return 0;
}

void led_en(uint16_t enable)
{
    extern sys_reg_st  g_sys; 															
    if(enable != 0)
    {
        set_freq_led(g_sys.stat.freq_index);
        set_vol_led(g_sys.stat.volum_index);
    }
    else
    {
        set_freq_led(5);
        set_vol_led(0);
    }
}

INIT_BOARD_EXPORT(led_init);
MSH_CMD_EXPORT(led_set_freq, freq led set);
MSH_CMD_EXPORT(led_set_vol, vol led set);


