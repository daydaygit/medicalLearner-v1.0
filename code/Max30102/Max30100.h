#ifndef _MAX_30100_H
#define _MAX_30100_H
#include "stm32f10x.h"



typedef enum
{
	HR_Only=0,
	SpO2_Only,
	Mutil_Led
}Work_Mode;
typedef enum
{
	Success =0,
	I2C_Com_Error,
	Max30100_PWR_Error
}Error_Success;

Error_Success InitMax30100(void);
uint8_t ResetMax30100(void);
uint8_t ReadMax30100FifoLength(uint8_t * sampleLen);
uint8_t ReadMax30100FifoBuf(uint8_t * buf,uint8_t sampleLen);
uint8_t ReadMax30100Status(uint16_t * status);
uint8_t ReadMax30100Tmp(uint8_t * tmp);
uint8_t SetMax30100WorkMode(Work_Mode mode);
void FIFO_Read(uint32_t *pun_red_led, uint32_t *pun_ir_led);
#endif
