#include "Adc.h"
#include "uart.h"
#include "usbio.h"
#include "FunctionDef.h"


#ifndef FOR_ANDROID
#define SAMPLE_RATE 500
#define AD_DATABUF_LEN 111
#else
#define SAMPLE_RATE 200
#define SEND_INTERVAL 40
#define AD_DATABUF_LEN ((SAMPLE_RATE*SEND_INTERVAL)*2/1000)
#endif
static uint8_t Uart_Count=0;
static uint8_t LB=11;

uint16_t       filter_buff[11]={0};
uint8_t        AdDataBuf[AD_DATABUF_LEN];
__IO uint8_t   buff_counter = 0;
__IO uint16_t  ADC_ConvertedValue[M];
__IO u8        SampleCount = 0;
__IO uint8_t   DrawCount = 0;

static uint8_t   Function_Select_Back = FUNCTION_NONE;
extern uint32_t  aun_ir_buffer[];
extern __IO int  ir_red_data_count;

void ADC_Configure()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	/* Enable DMA clock */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);	/* Enable ADC1 and GPIOC clock */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0 | GPIO_Pin_4;	/*����PA0 PA1 PA4 Ϊ ADC0 1 4 */  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	DMA_DeInit(DMA1_Channel1);	/* DMA channel1 configuration */
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 			    //ADC??
	DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)&ADC_ConvertedValue;	    //????
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize         = M;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;	    //??????
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;  			//??????
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//??
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;				//????
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	DMA_Cmd(DMA1_Channel1, ENABLE);	                                            /* Enable DMA channel1 */
	                                                                            /* ADC1 configuration */	
	ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;			//??ADC??
	ADC_InitStructure.ADC_ScanConvMode       = ENABLE ; 	 				    //??????,???????????
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			                //????????,??????ADC??
	ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;       //?????????
	ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right; 	        //???????
	ADC_InitStructure.ADC_NbrOfChannel       = M;	 						    //????????1
	ADC_Init(ADC1, &ADC_InitStructure);

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);                                           /*??ADC??,?PCLK2?8??,?9MHz*/
 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);	/*??ADC1???11?55.	5?????,???1 */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_55Cycles5);

	ADC_DMACmd(ADC1, ENABLE);		                                            /* Enable ADC1 DMA */

	ADC_Cmd(ADC1, ENABLE);			                                            /* Enable ADC1 */

	ADC_ResetCalibration(ADC1);		                                            /*??????? */   

	while(ADC_GetResetCalibrationStatus(ADC1));	                                /*??????????? */

	ADC_StartCalibration(ADC1);		                                            /* ADC?? */

	while(ADC_GetCalibrationStatus(ADC1));		                                /* ??????*/
 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	                                	/* ??????????,????????ADC?? */
}

void TIM3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/*TIM_Period--1000   TIM_Prescaler--71 -->�ж�����Ϊ1ms*/
void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
	TIM_DeInit(TIM3);
	TIM_TimeBaseStructure.TIM_Period=(1000000/SAMPLE_RATE);		/* �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) */ 
																/* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
	TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);				/* ʱ��Ԥ��Ƶ�� 72M/72��1M  ����1000�ν�һ���ж� ��ÿ1ms��һ���ж�*/
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		/* ������Ƶ */
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;	/* ���ϼ���ģʽ */
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);						/* �������жϱ�־ */
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM3, ENABLE);										/* ����ʱ�� */

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , DISABLE);		/*�ȹرյȴ�ʹ��*/    
}

void ADC1_Init()
{	
	ADC_Configure();
	ADC1_Mode_Config();
	TIM3_NVIC_Configuration();
	TIM3_Configuration();
	StartCollect;
}

void TIM3_IRQHandler(void)
{
	int i;
	uint16_t sum = 0;
	uint16_t average;
	int wave_point;

	if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) {	
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);

		if( Function_Select_Back != Function_Select ) {  //ģʽ�л�,��ս��ջ�����
			Function_Select_Back = Function_Select;
			ClearWaves();
			buff_counter = 0;
			Uart_Count = 0;
			SampleCount = 0;
		}

		if(Function_Select == FUNCTION_ECG )       {  filter_buff[buff_counter++] = ADC1_Channel_1;  }
		else if(Function_Select == FUNCTION_PPG )  {  filter_buff[buff_counter++] = ADC1_Channel_0;  }
		else if(Function_Select == FUNCTION_GSR )  {  filter_buff[buff_counter++] = ADC1_Channel_4;  }
		else                                       {  return;                                        }

		if (buff_counter>=LB)                      {  buff_counter=0;                                }

		for (i=0;i<LB;i++) {
			sum += filter_buff[i];
		} 

		average = (uint16_t)(sum/LB);
		// xxxxxxxxxx   5�����һ��,500hz�Ĳ����ʽ���Ϊ100 xxxxxxxxxxxxxxxxxxxxxxx
		if( SampleCount == 0 )
			aun_ir_buffer[ir_red_data_count++] = average;

		SampleCount ++;
		if( SampleCount >= 5 )                         {  SampleCount = 0;               }

		DrawCount ++;
		if ( DrawCount >= 4 ) {
			DrawCount = 0;
			if(Function_Select == FUNCTION_ECG )    {  wave_point = 4096 - average;  }
			else                                    {  wave_point = average;         }

			DrawWaves(wave_point);
		}

		if(Function_Select != FUNCTION_GSR )            {   Caculate_HR();                }

		AdDataBuf[Uart_Count++] = average >> 8;
		AdDataBuf[Uart_Count++] = average & 0xff;
		if( Uart_Count >= AD_DATABUF_LEN) {
			SendDataToPc(AdDataBuf,Uart_Count);
			Uart_Count = 0;
		}
	}
}
