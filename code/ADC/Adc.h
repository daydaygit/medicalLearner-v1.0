#ifndef __ADC_H
#define __ADC_H

#include "stm32f10x.h"

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)

#define M 5 //采集的通道数目

extern __IO uint16_t ADC_ConvertedValue[M];

#define ADC1_Channel_0  ADC_ConvertedValue[0]
#define ADC1_Channel_1  ADC_ConvertedValue[1]
#define ADC1_Channel_2  ADC_ConvertedValue[2]
#define ADC1_Channel_4  ADC_ConvertedValue[4]
#define ADC1_Channel_3  ADC_ConvertedValue[3]

void ADC1_Init(void);

#define StartCollect  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);TIM_Cmd(TIM3, ENABLE)
#define StopCollect   TIM_Cmd(TIM3, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE)

#endif
