#ifndef _SOFT_I2C_CORE_H
#define SOFT_I2C_CORE_H
#include "stm32f10x.h"



void BSP_InitI2C(void);
u8 Max30102_SoftI2C_Write(u8 Register_Address, u8 Word_Data);
u8 Max30102_SoftI2C_Read(u8 Register_Address);
void Max30102_SoftI2C_FIFO_Read(uint32_t *pun_red_led, uint32_t *pun_ir_led);
#endif