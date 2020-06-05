#include "led_drv.h"
#include "sys_conf.h"
#include <board.h>

#define BAT_APP_THREAD_STACK_SIZE        512
#define MAV_MAX_CNT 64
//#define TS1_CAL_Flash_Add  0x1ffff7b8
//#define TS2_CAL_Flash_Add  0x1ffff7c2


typedef struct
{
    uint16_t	mav_buffer[MAV_MAX_CNT];
    uint16_t    mav_cnt;
    uint32_t	accum_sum;
    uint16_t	buffer_ff;
}bat_st;

static bat_st bat_inst,temp_inst;

static rt_adc_device_t adc_device = RT_NULL;

static void adc_init(void)
{
    adc_device = (rt_adc_device_t)rt_device_find("adc1");
}

static uint16_t adc_get_raw(uint8_t chan_id)
{
    return rt_adc_read(adc_device, chan_id);
}


static uint16_t adc_get_volt(uint8_t chan_id)
{
    uint16_t temp,volt;
    temp = adc_get_raw(chan_id);

    volt = 0.8059*temp;
    return volt;
}

static uint16_t adc_get_temp(void)
{
    uint16_t temp,volt;
//    tc1 = *((__IO uint16_t*)( (uint32_t)TS1_CAL_Flash_Add ));
//    tc2 = *((__IO uint16_t*)( (uint32_t)TS2_CAL_Flash_Add ));
    float temperate;
    temp = adc_get_raw(ADC_CHANNEL_TEMPSENSOR);

//    rt_kprintf("tc1:%d,tc2:%d\n",tc1,tc2);
    temperate = ((float)temp*3.3/0x1000);
    temperate = 25 + (1.34-temperate)/0.0043;
    volt = temperate*10;
    return volt;
}

static uint16_t bat_mav_calc(uint16_t mav_cnt_set)
{
	uint16_t bat_mav_volt = 0;
	uint16_t cur_volt = adc_get_volt(1);
	if(bat_inst.buffer_ff == 0)
	{
		bat_inst.mav_buffer[bat_inst.mav_cnt] = cur_volt; 
		bat_inst.accum_sum += bat_inst.mav_buffer[bat_inst.mav_cnt];
		bat_inst.mav_cnt ++;
		bat_mav_volt = bat_inst.accum_sum/bat_inst.mav_cnt;
		if(bat_inst.mav_cnt >= mav_cnt_set)
	    {
       	 	bat_inst.mav_cnt = 0;
        	bat_inst.buffer_ff = 1;
   		}
	}
	else
	{
		bat_inst.accum_sum -= bat_inst.mav_buffer[bat_inst.mav_cnt];
		bat_inst.mav_buffer[bat_inst.mav_cnt] = cur_volt;
		bat_inst.accum_sum += bat_inst.mav_buffer[bat_inst.mav_cnt]; 
		bat_inst.mav_cnt ++;
		if(bat_inst.mav_cnt >= mav_cnt_set)
       	 	bat_inst.mav_cnt = 0;
		bat_mav_volt = bat_inst.accum_sum/mav_cnt_set;
	}
	return bat_mav_volt*105/50;
}

static uint16_t temp_mav_calc(uint16_t mav_cnt_set)
{
    uint16_t temp_mav_volt = 0;
    uint16_t temp_volt = adc_get_temp();
    if(temp_inst.buffer_ff == 0)
    {
        temp_inst.mav_buffer[temp_inst.mav_cnt] = temp_volt;
        temp_inst.accum_sum += temp_inst.mav_buffer[temp_inst.mav_cnt];
        temp_inst.mav_cnt ++;
        temp_mav_volt = temp_inst.accum_sum/temp_inst.mav_cnt;
        if(temp_inst.mav_cnt >= mav_cnt_set)
        {
            temp_inst.mav_cnt = 0;
            temp_inst.buffer_ff = 1;
        }
    }
    else
    {
        temp_inst.accum_sum -= temp_inst.mav_buffer[temp_inst.mav_cnt];
        temp_inst.mav_buffer[temp_inst.mav_cnt] = temp_volt;
        temp_inst.accum_sum += temp_inst.mav_buffer[temp_inst.mav_cnt];
        temp_inst.mav_cnt ++;
        if(temp_inst.mav_cnt >= mav_cnt_set)
            temp_inst.mav_cnt = 0;
        temp_mav_volt = temp_inst.accum_sum/mav_cnt_set;
    }
    return temp_mav_volt;
}

static int16_t bat_pwr_calc(uint16_t up_lim, uint16_t low_lim, uint16_t bat_volt)
{
    int16_t res_pwr = 0;
	res_pwr = 100*(bat_volt-low_lim)/(up_lim-low_lim);
	if(res_pwr > 100)
		res_pwr = 100;
	else if(res_pwr<0)
	    res_pwr = 0;
	return res_pwr;
}

void bat_update(void)
{
	extern sys_reg_st g_sys;
	g_sys.stat.bat_volt = bat_mav_calc(g_sys.conf.bat_mav_cnt);
	g_sys.stat.bat_volum = bat_pwr_calc(g_sys.conf.bat_up_lim,g_sys.conf.bat_low_lim,g_sys.stat.bat_volt);
	g_sys.stat.mcu_temp = temp_mav_calc(g_sys.conf.bat_mav_cnt);
//	g_sys.stat.mcu_temp = adc_get_temp();
//	rt_kprintf("t:%d",g_sys.stat.mcu_temp);
	set_bat_led(g_sys.stat.bat_volum/10);
}

void bat_ds_init(void)
{
	uint16_t i;
	for(i=0;i<MAV_MAX_CNT;i++)
	{
		bat_inst.mav_buffer[i] = 0;
	}
	bat_inst.accum_sum = 0;
	bat_inst.mav_cnt = 0;
	adc_init();
}

static int bat_drv_init(void)
{
    adc_init();
    bat_ds_init();
    return RT_EOK;
}

static void bat_volt(void)
{
    rt_kprintf("adc val:%d\n",adc_get_volt(1));
    rt_kprintf("temp val:%d\n",adc_get_temp());
}

static void bat_app_entry(void *parameter)
{
    rt_thread_delay(200);
    while(1)
    {
        bat_update();
        rt_thread_delay(1000);
    }
}

static int bat_app(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("bat_app",
                            bat_app_entry,
                            RT_NULL,
                            BAT_APP_THREAD_STACK_SIZE,
                            12,
                            20
                          );

    RT_ASSERT(tid != RT_NULL);

    rt_thread_startup(tid);
    return 0;
}

INIT_APP_EXPORT(bat_app);

INIT_BOARD_EXPORT(bat_drv_init);

MSH_CMD_EXPORT(bat_volt, get bat volt.);

