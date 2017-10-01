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
	
	RCC_Configuration();      //系统时钟配置函数 
	NVIC_Configuration();
	/* 启动 GPIOA时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	/*串口配置*/
  USART_Config(115200); 
	OLED_Init();			//初始化OLED  
	
	OLED_Clear();
	/*默认为血养模式*/
	SetBoardTestMode(FUNCTION_SPO2);
	/*配置18b20*/
	ConfiureIoFor18b20(GPIOA,DS18B20);
	/*初始化MLX90614*/
	MLX90614_Init();
	/*初始化Max30100*/
	InitMax30100();
	/*清除Max30100中断*/
	ReadMax30100Status(&IntStatus);
	EXTI_ClearITPendingBit(EXTI_Line12);       //??? 
	
	/* usb初始化*/
	
	USB_Interrupts_Config();
	Set_USBClock();
  USB_Init();
	
	/*按键设置*/
	InitKeyPad();	
	/*蜂鸣器设置*/
	InitBeep();
	/*定时器配置*/
  TimerInit();
	
	/*ECG PPG GSR初始化*/
	ADC1_Init();
	
	/*蜂鸣器响一声*/
	BeepControl(ON);
	delay_ms(500);
	BeepControl(OFF);
	
	while(1)
	{
		
		if( I2ComError_Flag == 1 )
		{//I2C通讯错误，重新配置Max30100
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

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	/* 开漏输出模式 */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_15;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
*
*                        定时中断处理函数
*
*******************************************************************************/
void TIM2_IRQHandler(void)
{
  
	
	

  if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)  //检测制定的中断是否发生

  {

     TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除中断处理位。	
			ShowTemperature();
		  KeyPadProcess();
	   

   }
}
void TimerInit(void)
{//10ms
	/***772M下定时值的计算（（1+预分频TIM_Prescaler）/72*(1+定时周期TIM_Period)）*/
 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //配置RCC，使能TIMx

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 7199;  //时钟预分频数 例如:时钟频率=72/(时钟预分频+1)  
  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; ////定时器模式 向上计数  

  TIM_TimeBaseStructure.TIM_Period = 99;//自动重装载寄存器周期的值(定时时间)累计 0xFFFF个频率后产生个更新或者中断(也是说定时时间到) 3s

  TIM_TimeBaseStructure.TIM_ClockDivision = 0; ////时间分割值  

  //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //初始化定时器2
  
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //打开中断 溢出中断  


  TIM_Cmd(TIM2, ENABLE);  //打开定时器

  /* Main Output Enable */
  //TIM_CtrlPWMOutputs(TIM1, ENABLE);
}
/*******************************************************************************
*                           配置RCC
*******************************************************************************/
void RCC_Configuration(void)
{   
	ErrorStatus HSEStartUpStatus;
 //复位RCC外部设备寄存器到默认值
  RCC_DeInit();

  //打开外部高速晶振
  RCC_HSEConfig(RCC_HSE_ON);

   //等待外部高速时钟准备好
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)   //外部高速时钟已经准别好
  {								    
    //开启FLASH的预取功能
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    //FLASH延迟2个周期
    FLASH_SetLatency(FLASH_Latency_2);
 	
  //配置AHB(HCLK)时钟=SYSCLK
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  
  
   //配置APB2(PCLK2)钟=AHB时钟
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    //配置APB1(PCLK1)钟=AHB 1/2时钟
    RCC_PCLK1Config(RCC_HCLK_Div2);

     //配置PLL时钟 == 外部高速晶体时钟*9  PLLCLK = 8MHz * 9 = 72 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);


    //使能PLL时钟
    RCC_PLLCmd(ENABLE);

   //等待PLL时钟就绪
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

  //配置系统时钟 = PLL时钟
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

   //检查PLL时钟是否作为系统时钟
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
						{//102 个字节一组
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
