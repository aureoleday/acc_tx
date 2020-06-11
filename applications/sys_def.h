#ifndef	__SYS_DEF
#define	__SYS_DEF
// Define   NULL   pointer   
#ifndef   NULL 
#   ifdef   __cplusplus 
#     define   NULL      0 
#   else 
#     define   NULL      ((void   *)0) 
#   endif 
#endif //   NULL 

#include <stdint.h>

typedef unsigned char 		uint8;
typedef char 							int8;
typedef unsigned short 		uint16;
typedef short					 		int16;
typedef unsigned long 		uint32;
typedef long					 		int32; 
//typedef uint32_t          time_t;


#define USR_EOK                 0
#define USR_ERR                 (-1)


//#define SYS_DEBUG
#define STAT_REG_MAP_NUM        16
#define CONF_REG_MAP_NUM		16
#define SOFTWARE_VER			0x0204
#define HARDWARE_VER			0x0200

enum
{
    REGMAP_ERR_NOERR = 0,
    REGMAP_ERR_ADDR_OR,
    REGMAP_ERR_DATA_OR,
    REGMAP_ERR_PERM_OR,
    REGMAP_ERR_WR_OR,
    REGMAP_ERR_CONFLICT_OR
};

enum
{    
    INIT_MODE_USR = 0,    
    INIT_MODE_FACTORY,
    INIT_MODE_DEBUT,
    INIT_MODE_DEFAULT
};

enum
{    
    GBM_GPRS = 0,
    GBM_LINK,
    GBM_POUT,
    GBM_LOAD
};

#endif //__SYS_DEF
