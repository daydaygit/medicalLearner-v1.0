#ifndef _FUNCTION_DEF_H
#define _FUNCTION_DEF_H

#include "stm32f10x.h"

//#define FOR_ANDROID 1

#define FUNCTION_NONE  0
#define FUNCTION_ECG  1
#define FUNCTION_PPG 2
#define FUNCTION_GSR 3
#define FUNCTION_SPO2 4

extern __IO uint8_t Function_Select;
void DrawWaves(int data);
void ClearWaves(void);
void Caculate_HR_SpO2(uint8_t * DataBuf,uint8_t dataLen);
void CaculateHR(void);
void SetBoardTestMode(uint8_t mode);
void SendDataToPc(uint8_t * DataBuf,uint8_t dataLen);
void ShowTemperature(void);
void KeyPadProcess(void);
#endif
