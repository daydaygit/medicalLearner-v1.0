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

#include "function_oem.h"

#define DS18B20           GPIO_Pin_7    //PA7

#define BEEP              GPIO_Pin_15

#define KEY_ECG           GPIO_Pin_8    //GPA8
#define KEY_PPG           GPIO_Pin_15   //GPA15
//#define KEY_PPG                GPIO_Pin_8       //GPB8
#define KEY_SPO2          GPIO_Pin_9    //GPB9


__IO uint8_t Function_Select = 0;
__IO uint8_t Max30102_INT_Flag = 0;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
uint8_t      Max30102DataBuf[192];


uint16_t     IntStatus;
uint8_t      SampleLen = 0;

float        f_temp;   
int32_t      n_brightness;

void RCC_Configuration(void);
void TimerInit(void);

void InitKeyPad(void);
void BeepControl(Status_Ctl status);
void BeepPeriodOn(int enable, int Period);
void InitBeep(void);


int main(void)
{
	RCC_Configuration();                                    // ϵͳʱ�����ú��� 
	NVIC_Configuration();                                   // Ƕ�������жϿ�����

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	/* ���� GPIOAʱ�� */

	USART_Config(115200);                                   /* ��������*/ 

	OLED_Init();			                                //��ʼ��OLED : SSD1306 (128 x 64 ����)
	OLED_Clear();

#if 1
	InitBeep();	                                        /*����������*/
	BeepPeriodOn(ON, 200);                                  /*  ��������500ms */

#ifdef FUNC_OLED_QXY
	print_logo(LOGO_V1);
#endif
	delay_ms(1000);
	OLED_Clear();
#endif

	SetBoardTestMode(FUNCTION_SPO2);	                    /*Ĭ��ΪѪ��ģʽ*/

	ConfiureIoFor18b20(GPIOA, DS18B20);	                    /*����18b20*/

	MLX90614_Init();                                        /*��ʼ��MLX90614: Melexis MLX90614�����¶�*/

	InitMax30100();                                         /*��ʼ��Max30100:  ����Ѫ�����͡����ʴ�����*/
	ReadMax30100Status(&IntStatus);                         /*���Max30100�ж�*/
	EXTI_ClearITPendingBit(EXTI_Line12);                    //??? 

#ifndef USB_FUNC_DISABLE
	USB_Interrupts_Config();	                            /* usb��ʼ��*/
	Set_USBClock();
	USB_Init();
#endif

	InitKeyPad();	                                        /*��������*/

	TimerInit();                                            /*��ʱ������*/

	ADC1_Init();                                            /*ECG��PPG��GSR��ʼ��*/

	while(1)
	{		
		if( I2ComError_Flag == 1 ) {                         //I2CͨѶ������������Max30100
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

	GPIO_InitStructure.GPIO_Pin   = BEEP  ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;  /*  �������*/

	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void BeepControl(Status_Ctl status)
{
	if( status == 1)   {  GPIO_SetBits(GPIOB ,   BEEP);  }
	else               {  GPIO_ResetBits(GPIOB , BEEP);  }
}

void BeepPeriodOn(int enable, int Period)
{
	BeepControl((Status_Ctl)enable);
	delay_ms(Period);
	BeepControl(!enable);
}

void InitKeyPad()
{
	GPIO_InitTypeDef GPIO_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   /* ��GPIOʱ��            */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;	           /* ��������                    */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_15;  /* PA8:S2(ECG),  PA15: S3(PPG) */
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;   /* PB8:S4(GSR),  PB9: S5(x)       */
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*****************************************************************************
*                        ��ʱ�жϴ�����                                                              *
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {	//����ƶ����ж��Ƿ���
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		//����жϴ���λ	
		ShowTemperature();
		KeyPadProcess();
	}
}

void TimerInit(void)
{	//10ms
	/***772M�¶�ʱֵ�ļ��㣨��1+Ԥ��ƵTIM_Prescaler��/72*(1+��ʱ����TIM_Period)��*/

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);          //����RCC��ʹ��TIM2

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler     = 7199;               //ʱ��Ԥ��Ƶ�� ����:ʱ��Ƶ��=72/(ʱ��Ԥ��Ƶ+1)  = 72 / 7200 = 1/100
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; //��ʱ��ģʽ ���ϼ���  
	TIM_TimeBaseStructure.TIM_Period        = 99;                 //�Զ���װ�ؼĴ������ڵ�ֵ(��ʱʱ��)
                                                                  //�ۼ� 0xFFFF��Ƶ�ʺ���������»����ж�(Ҳ��˵��ʱʱ�䵽) 3s
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                  //ʱ��ָ�ֵ  

	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);               //��ʼ����ʱ��2

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                    //���ж� ����ж�  

	TIM_Cmd(TIM2, ENABLE);                                        //�򿪶�ʱ��

	//TIM_CtrlPWMOutputs(TIM1, ENABLE);                                                 /* Main Output Enable */
}

/*******************************************************************************
*                           ����RCC
*******************************************************************************/
void RCC_Configuration(void)
{   
	ErrorStatus HSEStartUpStatus;

	RCC_DeInit();                                                    //��λRCC�ⲿ�豸�Ĵ�����Ĭ��ֵ

	RCC_HSEConfig(RCC_HSE_ON);                                       //���ⲿ���پ���

	HSEStartUpStatus = RCC_WaitForHSEStartUp();                      //�ȴ��ⲿ����ʱ��׼����

	if(HSEStartUpStatus == SUCCESS)                                  //�ⲿ����ʱ���Ѿ�׼���
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);    //����FLASH��Ԥȡ����

		FLASH_SetLatency(FLASH_Latency_2);                       //FLASH�ӳ�2������,�ȴ�����ͬ������

		RCC_HCLKConfig(RCC_SYSCLK_Div1);                         //����AHB(HCLK)ʱ��=SYSCLK

		RCC_PCLK2Config(RCC_HCLK_Div1);                          //����APB2(PCLK2)��=AHBʱ��

		RCC_PCLK1Config(RCC_HCLK_Div2);                          //����APB1(PCLK1)��=AHB 1/2ʱ��

		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);     //����PLLʱ�� == �ⲿ���پ���ʱ��*9  PLLCLK = 8MHz * 9 = 72 MHz ��ֻ����PLL disableʱʹ��

		RCC_PLLCmd(ENABLE);                                      //ʹ��PLLʱ��

		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)       //�ȴ�PLLʱ�Ӿ���
		{
		}

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);               //����ϵͳʱ�� = PLLʱ��

		while(RCC_GetSYSCLKSource() != 0x08)                     //���PLLʱ���Ƿ���Ϊϵͳʱ��
		{
		}
	}
}

void EXTI15_10_IRQHandler(void)            //???:EXTI15_10 (??????10~15??????)  
{
   if(EXTI_GetITStatus(EXTI_Line12) != RESET)    //?????????????????,??????????  
   {  
      Max30102_INT_Flag = 1;
      EXTI_ClearITPendingBit(EXTI_Line12);       //???  
      ReadMax30100Status(&IntStatus);

      if( ReadMax30100FifoLength(&SampleLen) == Success) {
         if( SampleLen > 0  ) {
            if( ReadMax30100FifoBuf(Max30102DataBuf,SampleLen) == Success ) {  //102 ���ֽ�һ��
                if(Function_Select == FUNCTION_SPO2 ) {
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
