#ifndef _MAX_30100_I2C_H
#define _MAX_30100_I2C_H
#include "stm32f10x.h"
typedef enum
{
  DMA_TX = 0,
  DMA_RX = 1
}DMADirection_TypeDef;

typedef enum
{
  OFF = 0,
  ON
}Status_Ctl;

extern __IO uint8_t I2ComError_Flag;
void InitMax30100I2C(void);



uint8_t Max30100_WriteRegDMA(uint8_t RegName, uint8_t * pDataBuf,int Len);
uint8_t Max30100_ReadRegDMA(uint8_t RegName,uint8_t * pRegValueBuf,int Len);

uint8_t Max30100_WriteReg(uint8_t RegName, uint8_t RegValue);
uint8_t Max30100_ReadReg(uint8_t RegName,uint8_t * pRegValue);
uint8_t Max30100_ReadFIFOData(uint8_t read_ptr,uint8_t * buff,uint8_t buf_len);
void ResetI2c(void);
#endif
