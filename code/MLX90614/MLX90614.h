#ifndef _MLX90614_H
#define _MLX90614_H

#include "stm32f10x.h"


#define MLX90614_REG_TA       0x06  /*!< 环境温度地址      */
#define MLX90614_REG_TOBJ1    0x07  /*!< 物体温度低 8 位  */
#define MLX90614_REG_TOBJ2    0x08  /*!< 物体温度高 8 位  */




void MLX90614_Init(void);
int SMBus_ReadTemp(void);

#endif
