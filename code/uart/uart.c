#include "uart.h"
#include <stdarg.h>
#include <string.h>



#define RECVBUFF_SIZE 32

uint8_t RxBuffer1[RECVBUFF_SIZE];
uint8_t RxBuffer2[RECVBUFF_SIZE];

__IO uint8_t RxCounter = 0;
__IO uint8_t RxCounter1 = 0;
/*LPC2102串口*/
#define USARTy                   USART1
#define USARTy_GPIO              GPIOA
#define USARTy_CLK               RCC_APB2Periph_USART1
#define USARTy_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USARTy_RxPin             GPIO_Pin_10
#define USARTy_TxPin             GPIO_Pin_9
#define USARTy_Tx_DMA_Channel    DMA1_Channel4
#define USARTy_Tx_DMA_FLAG       DMA1_FLAG_TC4
#define USARTy_DR_Base           (USART1_BASE + 0x4)

/*蓝牙串口*/
#define USARTz                   USART2
#define USARTz_GPIO              GPIOA
#define USARTz_CLK               RCC_APB1Periph_USART2
#define USARTz_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USARTz_RxPin             GPIO_Pin_3
#define USARTz_TxPin             GPIO_Pin_2
#define USARTz_Tx_DMA_Channel    DMA1_Channel7
#define USARTz_Tx_DMA_FLAG       DMA1_FLAG_TC7
#define USARTz_DR_Base           (USART2_BASE + 0x4)
#define USARTz_IRQn              USART2_IRQn
	
void USART_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  
  /* Configure USARTy Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USARTy_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(USARTy_GPIO, &GPIO_InitStructure);
  
  
  
  /* Configure USARTy Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USARTy_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(USARTy_GPIO, &GPIO_InitStructure);

	/* Configure USARTz Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USARTz_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(USARTz_GPIO, &GPIO_InitStructure);
  
  
  
  /* Configure USARTz Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USARTz_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(USARTz_GPIO, &GPIO_InitStructure);

   
}
void USART1_IRQHandler(void)
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    RxBuffer1[RxCounter++] = USART_ReceiveData(USART1);

    if(RxCounter >= RECVBUFF_SIZE)
    {
       RxCounter = 0;
    }
  }
}
void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    RxBuffer2[RxCounter1++] = USART_ReceiveData(USART2);

    if(RxCounter1 >= RECVBUFF_SIZE)
    {
       RxCounter1 = 0;
    }
  }
}
void USART_NVIC_Configuration(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTz Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
}
void USART1_DMA_Configuration(uint8_t* buffer, uint8_t NumData)
{
	
  
	
  DMA_InitTypeDef DMA_InitStructure;
	
  

  /* USARTy_Tx_DMA_Channel (triggered by USARTy Tx event) Config */
 
  DMA_InitStructure.DMA_PeripheralBaseAddr = USARTy_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = NumData;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_DeInit(USARTy_Tx_DMA_Channel);
  DMA_Init(USARTy_Tx_DMA_Channel, &DMA_InitStructure);
  
  
}
void USART2_DMA_Configuration(uint8_t* buffer, uint8_t NumData)
{
	
  
	
  DMA_InitTypeDef DMA_InitStructure;
	
  

  /* USARTy_Tx_DMA_Channel (triggered by USARTy Tx event) Config */
 
  DMA_InitStructure.DMA_PeripheralBaseAddr = USARTz_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = NumData;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_DeInit(USARTz_Tx_DMA_Channel);
  DMA_Init(USARTz_Tx_DMA_Channel, &DMA_InitStructure);
  
  
}
void USART_RCC_Configuration(void)
{    
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(USARTy_GPIO_CLK | USARTz_GPIO_CLK , ENABLE);
  

  /* Enable USARTy Clock */
  RCC_APB2PeriphClockCmd(USARTy_CLK , ENABLE); 

}
/*
 * 函数名：USART1_Config
 * 描述  ：USART1 GPIO 配置,工作模式配置。115200 8-N-1
 * 输入  ：无
 * 输出  :    无
 * 调用  ：外部调用
 */
void USART_Config(u32 BandRate)
{	
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1  | RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  
	USART_NVIC_Configuration();
	USART_GPIO_Configuration();

	USART_InitStructure.USART_BaudRate = BandRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USARTy, &USART_InitStructure);
	USART_Init(USARTz, &USART_InitStructure);
 
    /* Enable the USARTz Receive Interrupt */
    USART_ITConfig(USARTy, USART_IT_RXNE, ENABLE);

    /* Enable USARTy */
    USART_Cmd(USARTy, ENABLE);

    /* Enable USARTz */
    USART_Cmd(USARTz, ENABLE); 
}

void USART_SendString(char * DataBuf)
{
	USART1_SendDataDMA((uint8_t *)DataBuf,strlen(DataBuf));
}

void USART1_SendDataDMA(uint8_t * DataBuf,uint8_t len)
{
	
  int uart_timeout = 0x1000*120;
	USART1_DMA_Configuration(DataBuf,len);
	
	 USART_DMACmd(USARTy,USART_DMAReq_Tx,ENABLE);
	/* Enable USARTy DMA TX Channel */
  DMA_Cmd(USARTy_Tx_DMA_Channel, ENABLE);
	 while (!DMA_GetFlagStatus(USARTy_Tx_DMA_FLAG))
  {
		if( (uart_timeout--) == 0)
			return;
  }
	
	 /* Disable DMA TX Channel */
  DMA_Cmd(USARTy_Tx_DMA_Channel, DISABLE);
  
  /* Disable I2C DMA request */  
  USART_DMACmd(USARTy,USART_DMAReq_Tx,DISABLE);
  
  /* Clear DMA TX Transfer Complete Flag */
  DMA_ClearFlag(USARTy_Tx_DMA_FLAG);
}
void USART2_SendDataDMA(uint8_t * DataBuf,uint8_t len)
{
	
  int uart_timeout = 0x1000*120;
	USART2_DMA_Configuration(DataBuf,len);
	
	 USART_DMACmd(USARTz,USART_DMAReq_Tx,ENABLE);
	/* Enable USARTy DMA TX Channel */
  DMA_Cmd(USARTz_Tx_DMA_Channel, ENABLE);
	 while (!DMA_GetFlagStatus(USARTz_Tx_DMA_FLAG))
  {
		if( (uart_timeout--) == 0)
			return;
  }
	
	 /* Disable DMA TX Channel */
  DMA_Cmd(USARTz_Tx_DMA_Channel, DISABLE);
  
  /* Disable I2C DMA request */  
  USART_DMACmd(USARTz,USART_DMAReq_Tx,DISABLE);
  
  /* Clear DMA TX Transfer Complete Flag */
  DMA_ClearFlag(USARTz_Tx_DMA_FLAG);
}
 





/*
 * 函数名：itoa ---这个是否可以调用C库函数处理呢？
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART1_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*
 * 函数名：USART1_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口1，即USART1
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART1_printf( USART1, "\r\n this is a demo \r\n" );
 *            	   USART1_printf( USART1, "\r\n %d \r\n", i );
 *            	   USART1_printf( USART1, "\r\n %s \r\n", j );
 */
void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...)
{
	const char *s;
	int d;   
	char buf[16];

	va_list ap;
	va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{                          
		if ( *Data == 0x5c )  //'\'
		{
			switch ( *++Data )
			{
				case 'r':								//回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':								//换行符
					USART_SendData(USARTx, 0x0a);
					Data ++;
					break;

				default:
					Data ++;
					break;
			}
		}
		else if ( *Data == '%')
		{
			switch ( *++Data )
			{
				case 's':								//字符串
					s = va_arg(ap, const char *);
					for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
					}
					Data++;
					break;

				case 'd':								//十进制
					d = va_arg(ap, int);
					itoa(d, buf, 10);
					for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
					}
					Data++;
					break;
				default:
					Data++;
					break;
			}
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
	}
}
