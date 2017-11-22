#ifndef _MLX90614_H
#define _MLX90614_H

#include "stm32f10x.h"


#define MLX90614_REG_TA       0x06  /*!< �����¶ȵ�ַ      */
#define MLX90614_REG_TOBJ1    0x07  /*!< �����¶ȵ� 8 λ  */
#define MLX90614_REG_TOBJ2    0x08  /*!< �����¶ȸ� 8 λ  */




void MLX90614_Init(void);
int SMBus_ReadTemp(void);

#endif
