#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <board.h>

#include "dac_drv.h"
#include "sys_conf.h"
#include "reg_chk.h"
#include "led_drv.h"

#define     PB_FREQ_U       GET_PIN(B, 9)
#define     PB_FREQ_D		GET_PIN(B, 8)
#define     PB_VOL_U        GET_PIN(B, 7)
#define     PB_VOL_D		GET_PIN(B, 13)
#define     PB_OUT_EN       GET_PIN(B, 14)

#define     STS_GPRS        GET_PIN(C, 15)
#define     STS_LINK1       GET_PIN(A, 15)
#define     STS_LINK2       GET_PIN(C, 13)
#define     STS_LOAD_DET    GET_PIN(B, 1)

#define     Bit_RESET		0
#define     Bit_SET		 	1	

void pb_evt(uint16_t pin_id)
{
    extern  sys_reg_st  g_sys; 															//global parameter declairation
	switch (pin_id)
	{
		case PB_FREQ_D:
		{
            if(g_sys.stat.out_en == 0)
                break;
			if(g_sys.stat.freq_index > 0)
			    g_sys.stat.freq_index--;
			freq_opt(g_sys.stat.freq_index);
			break;		
		}
		case PB_VOL_D:
		{
            if(g_sys.stat.out_en == 0)
                break;
			if(g_sys.stat.volum_index > 0)
				g_sys.stat.volum_index--;
			volum_opt(g_sys.stat.volum_index);
			break;		
		}
		case PB_FREQ_U:
		{
            if(g_sys.stat.out_en == 0)
                break;
			if(g_sys.stat.freq_index < 3)
				g_sys.stat.freq_index++;
			freq_opt(g_sys.stat.freq_index);
			break;		
		}
		case PB_VOL_U:
		{
            if(g_sys.stat.out_en == 0)
                break;
			if(g_sys.stat.volum_index < 4)
				g_sys.stat.volum_index++;
			volum_opt(g_sys.stat.volum_index);
			break;		
		}
		case PB_OUT_EN:
		{
		    power_en_opt(!g_sys.stat.out_en);
			break;		
		}
	}
}

static void pb_sts_update(void);
static void pb_timer_cb(void* arg)
{
    pb_sts_update();
}

static rt_timer_t tim_pb;
static void tim_init(void)
{
    tim_pb = rt_timer_create("tim_pb", pb_timer_cb,
                                 RT_NULL,
                                 30,
                                 RT_TIMER_FLAG_PERIODIC);
    if (tim_pb != RT_NULL) rt_timer_start(tim_pb);
}

int pb_init(void)
{
    rt_pin_mode(PB_FREQ_D, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PB_FREQ_U, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PB_OUT_EN, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PB_VOL_D, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PB_VOL_U, PIN_MODE_INPUT_PULLUP);

    rt_pin_mode(STS_GPRS, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(STS_LINK1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(STS_LINK2, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(STS_LOAD_DET, PIN_MODE_INPUT_PULLUP);

    tim_init();

    return 0;
}

inline static uint8_t get_pb_sts(void)
{
    uint8_t bitmap = 0;

    bitmap |= rt_pin_read(PB_FREQ_D);
    bitmap |= rt_pin_read(PB_FREQ_U)<<1;
    bitmap |= rt_pin_read(PB_OUT_EN)<<2;
    bitmap |= rt_pin_read(PB_VOL_D)<<3;
    bitmap |= rt_pin_read(PB_VOL_U)<<4;

    return bitmap;
}

uint8_t in_sts_update(void)
{
    extern  sys_reg_st  g_sys;
    uint16_t bitmap = 0;

    bitmap |= rt_pin_read(STS_GPRS);
    bitmap |= rt_pin_read(STS_LINK1)<<1;
    bitmap |= rt_pin_read(STS_LOAD_DET)<<3;
    bitmap |= (g_sys.stat.out_en)<<2;

    g_sys.stat.ms_status = (g_sys.stat.ms_status&0xfff0)|(bitmap&0x000f);
//    rt_kprintf("bm:%x,ms:%x\n",bitmap,g_sys.stat.ms_status);
    return bitmap;
}

static uint16_t resolve_pin_id(uint8_t pin_id)
{
    uint16_t pin_no;
    switch (pin_id)
    {
        case (0):
        {
            pin_no = PB_FREQ_D;
            break;
        }
        case (1):
        {
            pin_no = PB_FREQ_U;
            break;
        }
        case (2):
        {
            pin_no = PB_OUT_EN;
            break;
        }
        case (3):
        {
            pin_no = PB_VOL_D;
            break;
        }
        case (4):
        {
            pin_no = PB_VOL_U;
            break;
        }
        default:
        {
            pin_no = 0xffff;
            break;
        }
    }
    return pin_no;
}

static void pb_sts_update(void)
{
    uint16_t i;
    static uint8_t bitmap_reg = 0;
    static uint8_t stage_reg = 0;
    uint8_t bitmap = 0;
    uint8_t bitmap_and;

    bitmap = (~get_pb_sts())&0x1f;

    if(stage_reg == 0)
    {
        if(((bitmap^bitmap_reg) != 0)&&(bitmap != 0))
            stage_reg = 1;
        else
            stage_reg = 0;
    }
    else
    {
        bitmap_and = bitmap&bitmap_reg;
        if(bitmap_and != 0)
        {
            for(i=0;i<5;i++)
            {
                if((bitmap_and>>i)&0x01)
                {
                    pb_evt(resolve_pin_id(i));
                    break;
                }
            }
        }
        stage_reg = 0;
    }
    bitmap_reg = bitmap;
}

uint8_t get_atten(void)
{
    extern  sys_reg_st  g_sys;
    if(g_sys.stat.volum_index > 3)
        return 4;
    else
        return 3 - g_sys.stat.volum_index;
}

static void show_pin_sts(void)
{
    rt_kprintf("pb sts:%x\n",get_pb_sts());
    rt_kprintf("in sts:%x\n",in_sts_update());
}

//INIT_BOARD_EXPORT(pb_init);
MSH_CMD_EXPORT(show_pin_sts, get input pin status);
