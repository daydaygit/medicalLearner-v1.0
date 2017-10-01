#ifndef _UART_H_
#define _UART_H_

#include "stm32f10x.h"

void USART_Config(u32 BandRate);
void USART1_SendDataDMA(uint8_t * DataBuf,uint8_t len);
void USART2_SendDataDMA(uint8_t * DataBuf,uint8_t len);
void USART_SendString(char * DataBuf);
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...);
#endif
