#include "delay.h"
#include "oled.h"
#include "stdio.h"
#include "Max30100.h"
#include "Max_30100_I2C.h"
#include "algorithm.h"
#include "uart.h"
#include "DS18B20.h"
#include "MLX90614.h"
#include "Adc.h"
#include "FunctionDef.h"
#include "usbio.h"
#include "hw_config.h"
#include "usb_init.h"

#define DS18B20 GPIO_Pin_7 //PA7


#define BEEP    GPIO_Pin_15

#define KEY_ECG GPIO_Pin_8 //GPA8
#define KEY_PPG GPIO_Pin_15 //GPA15
//#define KEY_PPG GPIO_Pin_8 //GPB8
#define KEY_SPO2 GPIO_Pin_9 //GPB9


__IO uint8_t Function_Select = 0;
__IO uint8_t Max30102_INT_Flag = 0;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
uint8_t Max30102DataBuf[192];



uint16_t IntStatus;
uint8_t  SampleLen = 0;

void RCC_Configuration(void);
void TimerInit(void);


 float f_temp;   
int32_t n_brightness;

void InitKeyPad(void);
void BeepControl(Status_Ctl status);
void InitBeep(void);


int main(void)
{
	
	RCC_Configuration();      //ϵͳʱ�����ú��� 
	NVIC_Configuration();
	/* ���� GPIOAʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/*��������*/
  USART_Config(115200); 
	OLED_Init();			//��ʼ��OLED  
	
	OLED_Clear();
	/*Ĭ��ΪѪ��ģʽ*/
	SetBoardTestMode(FUNCTION_SPO2);
	/*����18b20*/
	ConfiureIoFor18b20(GPIOA,DS18B20);
	/*��ʼ��MLX90614*/
	MLX90614_Init();
	/*��ʼ��Max30100*/
	InitMax30100();
	/*���Max30100�ж�*/
	ReadMax30100Status(&IntStatus);
	EXTI_ClearITPendingBit(EXTI_Line12);       //??? 
	
	/* usb��ʼ��*/
	
	USB_Interrupts_Config();
	Set_USBClock();
  USB_Init();
	
	/*��������*/
	InitKeyPad();	
	/*����������*/
	InitBeep();
	/*��ʱ������*/
  TimerInit();
	
	/*ECG PPG GSR��ʼ��*/
	ADC1_Init();
	
	/*��������һ��*/
	BeepControl(ON);
	delay_ms(500);
	BeepControl(OFF);
	
	while(1)
	{
		
		if( I2ComError_Flag == 1 )
		{//I2CͨѶ������������Max30100
			ResetI2c();
			InitMax30100();
			ReadMax30100Status(&IntStatus);
	    EXTI_ClearITPendingBit(EXTI_Line6); 
		}
	}
	
}

void NVIC_Configuration(void)
{
 NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
	
	 NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;

  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void InitBeep()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	 GPIO_InitStructure.GPIO_Pin = BEEP  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void BeepControl(Status_Ctl status)
{
	if( status == 1)
	{
		GPIO_SetBits(GPIOB , BEEP); 
	}
	else
	{
		GPIO_ResetBits(GPIOB , BEEP);
	}
}

void InitKeyPad()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	/* ��©���ģʽ */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
*
*                        ��ʱ�жϴ�����
*
*******************************************************************************/
void TIM2_IRQHandler(void)
{
  
	
	

  if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)  //����ƶ����ж��Ƿ���

  {

     TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //����жϴ���λ��	
			ShowTemperature();
		  KeyPadProcess();
	   

   }
}
void TimerInit(void)
{//10ms
	/***772M�¶�ʱֵ�ļ��㣨��1+Ԥ��ƵTIM_Prescaler��/72*(1+��ʱ����TIM_Period)��*/
 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //����RCC��ʹ��TIMx

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 7199;  //ʱ��Ԥ��Ƶ�� ����:ʱ��Ƶ��=72/(ʱ��Ԥ��Ƶ+1)  
  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; ////��ʱ��ģʽ ���ϼ���  

  TIM_TimeBaseStructure.TIM_Period = 99;//�Զ���װ�ؼĴ������ڵ�ֵ(��ʱʱ��)�ۼ� 0xFFFF��Ƶ�ʺ���������»����ж�(Ҳ��˵��ʱʱ�䵽) 3s

  TIM_TimeBaseStructure.TIM_ClockDivision = 0; ////ʱ��ָ�ֵ  

  //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //��ʼ����ʱ��2
  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //���ж� ����ж�  


  TIM_Cmd(TIM2, ENABLE);  //�򿪶�ʱ��

  /* Main Output Enable */
  //TIM_CtrlPWMOutputs(TIM1, ENABLE);
}
/*******************************************************************************
*                           ����RCC
*******************************************************************************/
void RCC_Configuration(void)
{   
	ErrorStatus HSEStartUpStatus;
 //��λRCC�ⲿ�豸�Ĵ�����Ĭ��ֵ
  RCC_DeInit();

  //���ⲿ���پ���
  RCC_HSEConfig(RCC_HSE_ON);

   //�ȴ��ⲿ����ʱ��׼����
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)   //�ⲿ����ʱ���Ѿ�׼���
  {								    
    //����FLASH��Ԥȡ����
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    //FLASH�ӳ�2������
    FLASH_SetLatency(FLASH_Latency_2);
 	
  //����AHB(HCLK)ʱ��=SYSCLK
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  
  
   //����APB2(PCLK2)��=AHBʱ��
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    //����APB1(PCLK1)��=AHB 1/2ʱ��
    RCC_PCLK1Config(RCC_HCLK_Div2);

     //����PLLʱ�� == �ⲿ���پ���ʱ��*9  PLLCLK = 8MHz * 9 = 72 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);


    //ʹ��PLLʱ��
    RCC_PLLCmd(ENABLE);

   //�ȴ�PLLʱ�Ӿ���
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

  //����ϵͳʱ�� = PLLʱ��
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

   //���PLLʱ���Ƿ���Ϊϵͳʱ��
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}

void EXTI15_10_IRQHandler(void)         //???:EXTI15_10 (??????10~15??????)  
{   
		
    if(EXTI_GetITStatus(EXTI_Line12) != RESET) //?????????????????,??????????  
    {  
		    Max30102_INT_Flag = 1;
        EXTI_ClearITPendingBit(EXTI_Line12);       //???  
        ReadMax30100Status(&IntStatus);
				if( ReadMax30100FifoLength(&SampleLen) == Success)		
				{
					if( SampleLen > 0  )
					{
						if( ReadMax30100FifoBuf(Max30102DataBuf,SampleLen) == Success )
						{//102 ���ֽ�һ��
							if(Function_Select == FUNCTION_SPO2 )
							{
								#ifndef FOR_ANDROID
								SendDataToPc(Max30102DataBuf,SampleLen*6);
								#endif
								Caculate_HR_SpO2(Max30102DataBuf,SampleLen);
								
							}
						}
					}
				}
         
    }  
} 	
