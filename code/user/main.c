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
void InitBeep(void);


int main(void)
{	
	RCC_Configuration();                                    // ÏµÍ³Ê±ÖÓÅäÖÃº¯Êý 
	NVIC_Configuration();                                   // Ç¶Ì×ÏòÁ¿ÖÐ¶Ï¿ØÖÆÆ÷

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	/* Æô¶¯ GPIOAÊ±ÖÓ */

	USART_Config(115200);                                   /* ´®¿ÚÅäÖÃ*/ 

	OLED_Init();			                                //³õÊ¼»¯OLED : SSD1306 (128 x 64 µãÕó)
	OLED_Clear();

	SetBoardTestMode(FUNCTION_SPO2);	                    /*Ä¬ÈÏÎªÑªÑõÄ£Ê½*/

	ConfiureIoFor18b20(GPIOA, DS18B20);	                    /*ÅäÖÃ18b20*/

	MLX90614_Init();                                        /*³õÊ¼»¯MLX90614: Melexis MLX90614ºìÍâÎÂ¶È*/

	InitMax30100();                                         /*³õÊ¼»¯Max30100:  Âö²«ÑªÑõ±¥ºÍ¡¢ÐÄÂÊ´«¸ÐÆ÷*/
	ReadMax30100Status(&IntStatus);                         /*Çå³ýMax30100ÖÐ¶Ï*/
	EXTI_ClearITPendingBit(EXTI_Line12);                    //??? 

	USB_Interrupts_Config();	                            /* usb³õÊ¼»¯*/
	Set_USBClock();
	USB_Init();

	InitKeyPad();	                                        /*°´¼üÉèÖÃ*/

	InitBeep();	                                            /*·äÃùÆ÷ÉèÖÃ*/

	TimerInit();                                            /*¶¨Ê±Æ÷ÅäÖÃ*/

	ADC1_Init();                                            /*ECG¡¢PPG¡¢GSR³õÊ¼»¯*/

	BeepControl(ON);                                        /*  <B7><E4><C3><F9><C6><F7><CF><EC>¿<C9><F9>*/
	delay_ms(500);
	BeepControl(OFF);

	while(1)
	{		
		if( I2ComError_Flag == 1 ) {                         //I2CÍ¨Ñ¶´íÎó£¬ÖØÐÂÅäÖÃMax30100
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
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;  /*  ÍÆÍìÊä³ö*/

	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void BeepControl(Status_Ctl status)
{
	if( status == 1)   {  GPIO_SetBits(GPIOB ,   BEEP);  }
	else               {  GPIO_ResetBits(GPIOB , BEEP);  }
}

void InitKeyPad()
{
	GPIO_InitTypeDef GPIO_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	   /* ´ò¿ªGPIOÊ±ÖÓ            */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;	           /* ÉÏÀ­ÊäÈë                    */
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_15;  /* PA8:S2(ECG),  PA15: S3(PPG) */
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;   /* PB8:S4(GSR),  PB9: S5(x)       */
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*****************************************************************************
*                        ¶¨Ê±ÖÐ¶Ï´¦Àíº¯Êý                                                              *
*******************************************************************************/
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {	//¼ì²âÖÆ¶¨µÄÖÐ¶ÏÊÇ·ñ·¢Éú
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		//Çå³ýÖÐ¶Ï´¦ÀíÎ»¡£	
		ShowTemperature();
		KeyPadProcess();
	}
}

void TimerInit(void)
{	//10ms
	/***772MÏÂ¶¨Ê±ÖµµÄ¼ÆËã£¨£¨1+Ô¤·ÖÆµTIM_Prescaler£©/72*(1+¶¨Ê±ÖÜÆÚTIM_Period)£©*/

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);          //ÅäÖÃRCC£¬Ê¹ÄÜTIM2

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler     = 7199;               //Ê±ÖÓÔ¤·ÖÆµÊý ÀýÈç:Ê±ÖÓÆµÂÊ=72/(Ê±ÖÓÔ¤·ÖÆµ+1)  
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; //¶¨Ê±Æ÷Ä£Ê½ ÏòÉÏ¼ÆÊý  
	TIM_TimeBaseStructure.TIM_Period        = 99;                 //×Ô¶¯ÖØ×°ÔØ¼Ä´æÆ÷ÖÜÆÚµÄÖµ(¶¨Ê±Ê±¼ä)
                                                                  //ÀÛ¼Æ 0xFFFF¸öÆµÂÊºó²úÉú¸ö¸üÐÂ»òÕßÖÐ¶Ï(Ò²ÊÇËµ¶¨Ê±Ê±¼äµ½) 3s
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                  //Ê±¼ä·Ö¸îÖµ  

	//TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);               //³õÊ¼»¯¶¨Ê±Æ÷2

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                    //´ò¿ªÖÐ¶Ï Òç³öÖÐ¶Ï  

	TIM_Cmd(TIM2, ENABLE);                                        //´ò¿ª¶¨Ê±Æ÷

	//TIM_CtrlPWMOutputs(TIM1, ENABLE);                                                 /* Main Output Enable */
}

/*******************************************************************************
*                           ÅäÖÃRCC
*******************************************************************************/
void RCC_Configuration(void)
{   
	ErrorStatus HSEStartUpStatus;

	RCC_DeInit();                                                    //¸´Î»RCCÍâ²¿Éè±¸¼Ä´æÆ÷µ½Ä¬ÈÏÖµ

	RCC_HSEConfig(RCC_HSE_ON);                                       //´ò¿ªÍâ²¿¸ßËÙ¾§Õñ

	HSEStartUpStatus = RCC_WaitForHSEStartUp();                      //µÈ´ýÍâ²¿¸ßËÙÊ±ÖÓ×¼±¸ºÃ

	if(HSEStartUpStatus == SUCCESS)                                  //Íâ²¿¸ßËÙÊ±ÖÓÒÑ¾­×¼±ðºÃ
	{
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);    //¿ªÆôFLASHµÄÔ¤È¡¹¦ÄÜ

		FLASH_SetLatency(FLASH_Latency_2);                       //FLASHÑÓ³Ù2¸öÖÜÆÚ,µÈ´ý×ÜÏßÍ¬²½²Ù×÷

		RCC_HCLKConfig(RCC_SYSCLK_Div1);                         //ÅäÖÃAHB(HCLK)Ê±ÖÓ=SYSCLK

		RCC_PCLK2Config(RCC_HCLK_Div1);                          //ÅäÖÃAPB2(PCLK2)ÖÓ=AHBÊ±ÖÓ

		RCC_PCLK1Config(RCC_HCLK_Div2);                          //ÅäÖÃAPB1(PCLK1)ÖÓ=AHB 1/2Ê±ÖÓ

		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);     //ÅäÖÃPLLÊ±ÖÓ == Íâ²¿¸ßËÙ¾§ÌåÊ±ÖÓ*9  PLLCLK = 8MHz * 9 = 72 MHz £¬Ö»ÄÜÔÚPLL disableÊ±Ê¹ÓÃ

		RCC_PLLCmd(ENABLE);                                      //Ê¹ÄÜPLLÊ±ÖÓ

		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)       //µÈ´ýPLLÊ±ÖÓ¾ÍÐ÷
		{
		}

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);               //ÅäÖÃÏµÍ³Ê±ÖÓ = PLLÊ±ÖÓ

		while(RCC_GetSYSCLKSource() != 0x08)                     //¼ì²éPLLÊ±ÖÓÊÇ·ñ×÷ÎªÏµÍ³Ê±ÖÓ
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
            if( ReadMax30100FifoBuf(Max30102DataBuf,SampleLen) == Success ) {  //102 ¸ö×Ö½ÚÒ»×é
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
