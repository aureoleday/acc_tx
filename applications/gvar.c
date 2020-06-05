#include <stdlib.h>
#include "sys_conf.h"
#include "reg_chk.h"

//configuration parameters
sys_reg_st  g_sys; 																	//global parameter declairation

//configuration register map declairation
const conf_reg_map_st conf_reg_map_inst[CONF_REG_MAP_NUM]=  
{	//id		mapped registers		                 min	    max				    default			 type    chk_prt
    {	0,		&g_sys.conf.pwr_en,                      0,		    1,                  0,               0,       power_en_opt},
    {	1,		&g_sys.conf.freq_set,                    0,         4,                  1,               0,       freq_opt},
    {	2,		&g_sys.conf.volum_set,                   0,         8,                  4,               0,       volum_opt},
    {	3,		NULL,                                    0,         0,                  0,               0,       NULL},
    {	4,		NULL,                                    0,         0,                  0,               0,       NULL},
    {	5,		&g_sys.conf.rect_freq,                   100,       6000,               470,             0,       rect_freq_opt},
    {	6,		&g_sys.conf.phase_offset,                0,         255,                0,               0,       NULL},
    {	7,		&g_sys.conf.volum_step,                  1,         100,                1,               0,       NULL},
    {	8,		&g_sys.conf.dac_offset,                  0,         2048,               2047,            0,       NULL},
    {	9,		&g_sys.conf.bat_up_lim,                  3000,      6400,               4200,            0,       NULL},
    {	10,		&g_sys.conf.bat_low_lim,                 0,         4500,               2800,            0,       NULL},
    {	11,		&g_sys.conf.bat_mav_cnt,			     0,		    128,                32,              0,       NULL},
    {	12,		NULL,			                         0,		    0,                  0,               0,       NULL},
    {	13,		NULL,			                         0,		    0,                  0,               0,       NULL},
    {	14,		NULL,			                         0,		    0,                  0,               0,       NULL},
    {	15,		NULL,			                         9527,		9527,               0,               0,       sys_reset_opt},
};


const sts_reg_map_st status_reg_map_inst[STAT_REG_MAP_NUM]=
{// id			 mapped registers
  { 0,           &g_sys.stat.software_ver,               SOFTWARE_VER},
  { 1,           &g_sys.stat.hardware_ver,               HARDWARE_VER},
  { 2,           NULL,                                   0},
  { 3,           NULL,                                   0},
  { 4,           &g_sys.stat.mcu_temp,                   0},
  { 5,           &g_sys.stat.volum_reg,                  0},
  { 6,           &g_sys.stat.rect_freq,                  470},
  {	7,           &g_sys.stat.ms_status,					 0},
  { 8,           &g_sys.stat.bat_volt,                   0},
  { 9,           &g_sys.stat.bat_volum,                  0},
  { 10,          &g_sys.stat.bat_adc_raw,                0},
  { 11,          &g_sys.stat.half_period,                50},
  { 12,          &g_sys.stat.mul_factor,                 6},
  { 13,          &g_sys.stat.out_en,                     0},
  { 14,          &g_sys.stat.freq_index,                 1},
  { 15,          &g_sys.stat.volum_index,                4}
};

static void init_load_status(void)
{
    uint16_t i;	
    for(i=0;i<STAT_REG_MAP_NUM;i++)
    {
        if(status_reg_map_inst[i].reg_ptr != NULL)
        {
            *(status_reg_map_inst[i].reg_ptr) = status_reg_map_inst[i].dft;
        }
    }
}

static uint16_t init_load_default(void)
{
    uint16_t i, ret=0;
    for(i=0;i<CONF_REG_MAP_NUM;i++)		//initialize global variable with default values
    {
        if(conf_reg_map_inst[i].reg_ptr != NULL)
        {
            *(conf_reg_map_inst[i].reg_ptr) = conf_reg_map_inst[i].dft;
        }
    }    
    return ret;
}

void gvar_init(void)
{
    init_load_status();
    init_load_default();
}

/**
  * @brief  write register map with constraints.
  * @param  reg_addr: reg map address.
  * @param  wr_data: write data. 
	* @param  permission_flag:  
  *   This parameter can be one of the following values:
  *     @arg PERM_PRIVILEGED: write opertion can be performed dispite permission level
  *     @arg PERM_INSPECTION: write operation could only be performed when pass permission check
  * @retval 
  *   This parameter can be one of the following values:
  *     @arg 1: write operation success
  *     @arg 0: write operation fail
  */
uint16 reg_map_write(uint16_t reg_addr, uint16_t *wr_data, uint8_t wr_cnt)
{
    uint16_t i;
    uint16_t err_code;
    err_code = REGMAP_ERR_NOERR;		
    
    if((reg_addr+wr_cnt) > CONF_REG_MAP_NUM)	//address range check
    {
        err_code = REGMAP_ERR_ADDR_OR;
        rt_kprintf("REGMAP_ERR_ADDR_OR1 failed\n");
        return err_code;
    }
    
    for(i=0;i<wr_cnt;i++)										//min_max limit check
    {
        if((*(wr_data+i)>conf_reg_map_inst[reg_addr+i].max)||(*(wr_data+i)<conf_reg_map_inst[reg_addr+i].min))		//min_max limit check
        {
            err_code = REGMAP_ERR_DATA_OR;
            rt_kprintf("REGMAP_ERR_WR_OR03 failed\n");
            return err_code;	
        }

        if(conf_reg_map_inst[reg_addr+i].chk_ptr != NULL)
        {
            if(conf_reg_map_inst[reg_addr+i].chk_ptr(*(wr_data+i))==0)
            {
                err_code = REGMAP_ERR_CONFLICT_OR;
                rt_kprintf("CHK_PTR:REGMAP_ERR_WR_OR failed\n");
                return err_code;	
            }
        }
    }
    
    for(i=0;i<wr_cnt;i++)										//data write
    {
        if(conf_reg_map_inst[reg_addr+i].reg_ptr != NULL)
            *(conf_reg_map_inst[reg_addr+i].reg_ptr) = *(wr_data+i);//write data to designated register
    }	
    return err_code;		
}

/**
  * @brief  read register map.
  * @param  reg_addr: reg map address.
	* @param  *rd_data: read data buffer ptr.
  * @retval 
  *   This parameter can be one of the following values:
  *     @arg 1: write operation success
  *     @arg 0: read operation fail
  */
uint16 reg_map_read(uint16_t reg_addr,uint16_t* reg_data,uint8_t read_cnt)
{
    uint16_t i;
    uint16_t err_code;
    err_code = REGMAP_ERR_NOERR;
    if((reg_addr&0x8000) != 0)
    {
        reg_addr &= 0x7fff;
        if(reg_addr > STAT_REG_MAP_NUM)	//address out of range
        {
            err_code = REGMAP_ERR_ADDR_OR;
        }
        else
        {
            for(i=0;i<read_cnt;i++)
            {
                *(reg_data+i) = *(status_reg_map_inst[reg_addr+i].reg_ptr);//read data from designated register
            }
        }
    }
    else
    {
        reg_addr = reg_addr;
        if(reg_addr > CONF_REG_MAP_NUM)	//address out of range
        {
            err_code = REGMAP_ERR_ADDR_OR;
        }
        else
        {
            for(i=0;i<read_cnt;i++)
            {
                *(reg_data+i) = *(conf_reg_map_inst[reg_addr+i].reg_ptr);//read data from designated register						
            }
        }		
    }	
    return err_code;
}

/**
  * @brief  show register map content.
  * @param  reg_addr: reg map address.
	* @param  *rd_data: register read count.
  * @retval none
  */
static void rd_reg_map(int argc, char**argv)
{
    uint16_t reg_buf[32];
    uint16_t i;
    uint16_t reg_addr = atoi(argv[1]);
    uint16_t reg_cnt = atoi(argv[2]);
    
    if (argc < 3)
    {
        rt_kprintf("Please input'wr_reg_map <reg_addr> <reg_val>'\n");
        return;
    }
    
    reg_map_read(reg_addr, reg_buf, reg_cnt);
    rt_kprintf("Reg map info:\n");
    for(i=0;i<reg_cnt;i++)
    {
        rt_kprintf("addr:%d;data:%x\n", (reg_addr+i)&0x3fff,reg_buf[i]);
    }
}

static void wr_reg_map(int argc, char**argv)
{
    uint16_t wr_data;
    if (argc < 3)
    {
        rt_kprintf("Please input'wr_reg_map <reg_addr> <reg_val>'\n");
        return;
    }
    
    wr_data = atoi(argv[2]);
    
    reg_map_write(atoi(argv[1]),&wr_data,1);
}

MSH_CMD_EXPORT(wr_reg_map, write data into conf registers.);
MSH_CMD_EXPORT(rd_reg_map, show registers map.);
