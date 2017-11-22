#include "Max_30100_I2C.h"
#include "Max30100_Config.h"
#include "delay.h"

#define MAX30100_ADDR 0xAE
#define MAX30100_I2C_DR                      ((uint32_t)0x40005810)
#define MAX30100_DMA_TX_CHANNEL              DMA1_Channel4
#define MAX30100_DMA_RX_CHANNEL              DMA1_Channel5
#define MAX30100_DMA_TX_TCFLAG               DMA1_FLAG_TC4
#define MAX30100_DMA_RX_TCFLAG               DMA1_FLAG_TC5   

#define MAX30100_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define MAX30100_LONG_TIMEOUT         ((uint32_t)(10 * MAX30100_FLAG_TIMEOUT))  

#define LED_RED  GPIO_Pin_14
#define LED_IR  GPIO_Pin_13
#define BEEP    GPIO_Pin_15


__IO uint32_t  Max30100_Timeout = MAX30100_LONG_TIMEOUT; 

__IO uint8_t I2ComError_Flag = 0;

void LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  /*!< Configure I2C2 pins: SCL */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure I2C2 pins: SDA */
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  //GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = LED_RED | LED_IR ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}
void ResetI2c()
{
	uint8_t i,tmp;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	
	//if((I2C2->SR1 & 0x100) == 0x100)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		if(GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_11) == 0)
		{
			
		I2C_DeInit(I2C2);
		
		
		
		for( i = 0; i < 9; i++)
		{
			
		
			GPIO_SetBits(GPIOB , GPIO_Pin_10);
			delay_us(2);
			tmp=GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_11);
			GPIO_ResetBits(GPIOB , GPIO_Pin_10);
			delay_us(2);
		}
		
		//InitMax30100I2C();
		I2ComError_Flag = 0;
		}
	}
}
void InitMax30100I2C()
{
	I2C_InitTypeDef   I2C_InitStructure;
	
	LowLevel_Init();
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 400000;
  I2C_Init(I2C2, &I2C_InitStructure);

  I2C_ITConfig(I2C2, I2C_IT_ERR, ENABLE);
	
  I2C_Cmd(I2C2, ENABLE);
}
static void DMA_Config(DMADirection_TypeDef Direction, uint8_t* buffer, uint8_t NumData)
{
  DMA_InitTypeDef DMA_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* Initialize the DMA_PeripheralBaseAddr member */
  DMA_InitStructure.DMA_PeripheralBaseAddr = MAX30100_I2C_DR;
  /* Initialize the DMA_MemoryBaseAddr member */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
   /* Initialize the DMA_PeripheralInc member */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  /* Initialize the DMA_MemoryInc member */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  /* Initialize the DMA_PeripheralDataSize member */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /* Initialize the DMA_MemoryDataSize member */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  /* Initialize the DMA_Mode member */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  /* Initialize the DMA_Priority member */
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  /* Initialize the DMA_M2M member */
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  /* If using DMA for Reception */
  if (Direction == DMA_RX)
  {
    /* Initialize the DMA_DIR member */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    
    /* Initialize the DMA_BufferSize member */
    DMA_InitStructure.DMA_BufferSize = NumData;
    
    DMA_DeInit(MAX30100_DMA_RX_CHANNEL);
    
    DMA_Init(MAX30100_DMA_RX_CHANNEL, &DMA_InitStructure);
  }
   /* If using DMA for Transmission */
  else if (Direction == DMA_TX)
  { 
    /* Initialize the DMA_DIR member */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    
    /* Initialize the DMA_BufferSize member */
    DMA_InitStructure.DMA_BufferSize = NumData;
    
    DMA_DeInit(MAX30100_DMA_TX_CHANNEL);
    
    DMA_Init(MAX30100_DMA_TX_CHANNEL, &DMA_InitStructure);
  }
}

uint8_t Max30100_WriteRegDMA(uint8_t RegName, uint8_t * pDataBuf,int Len)
{   
  
  
  /* Test on BUSY Flag */
  Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY)) 
  {
    if((Max30100_Timeout--) == 0) return 0;
  }
  
  /* Configure DMA Peripheral */
  DMA_Config(DMA_TX, (uint8_t*)pDataBuf, Len);
  
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(I2C2, ENABLE);
  
  /* Test on SB Flag */
  Max30100_Timeout = MAX30100_FLAG_TIMEOUT;
  while (I2C_GetFlagStatus(I2C2,I2C_FLAG_SB) == RESET) 
  {
    if((Max30100_Timeout--) == 0) return 0;
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(I2C2, MAX30100_ADDR, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  Max30100_Timeout = MAX30100_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((Max30100_Timeout--) == 0) return 0;
  }
  
  /* Transmit the first address for r/w operations */
  I2C_SendData(I2C2, RegName);
  
  /* Test on TXE FLag (data sent) */
  Max30100_Timeout = MAX30100_FLAG_TIMEOUT;
  while ((!I2C_GetFlagStatus(I2C2,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C2,I2C_FLAG_BTF)))  
  {
    if((Max30100_Timeout--) == 0) return 0;
  }
  
  /* Enable I2C DMA request */
  I2C_DMACmd(I2C2,ENABLE);
  
  /* Enable DMA TX Channel */
  DMA_Cmd(MAX30100_DMA_TX_CHANNEL, ENABLE);
  
  /* Wait until DMA Transfer Complete */
  Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  while (!DMA_GetFlagStatus(MAX30100_DMA_TX_TCFLAG))
  {
    if((Max30100_Timeout--) == 0) return 0;
  }  
  
  /* Wait until BTF Flag is set before generating STOP */
  Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(I2C2,I2C_FLAG_BTF))  
  {
    if((Max30100_Timeout--) == 0) return 0;
  }
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(I2C2, ENABLE);
  
  /* Disable DMA TX Channel */
  DMA_Cmd(MAX30100_DMA_TX_CHANNEL, DISABLE);
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(I2C2,DISABLE);
  
  /* Clear DMA TX Transfer Complete Flag */
  DMA_ClearFlag(MAX30100_DMA_TX_TCFLAG);
  
  return 1;
}
uint8_t Max30100_WriteReg(uint8_t RegName, uint8_t RegValue)
{
	
	
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY)) 
  {
    if((Max30100_Timeout--) == 0) goto _exit_write;
  }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  I2C_GenerateSTART  ( I2C2, ENABLE);
	
  while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT))    //ev5
	{
    if((Max30100_Timeout--) == 0) goto _exit_write;
  }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  I2C_Send7bitAddress  ( I2C2,MAX30100_ADDR, I2C_Direction_Transmitter); 
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ))
	{
    if((Max30100_Timeout--) == 0) goto _exit_write;
  }
	
  I2C_SendData  ( I2C2, RegName);
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING))    //ev8
	{
    if((Max30100_Timeout--) == 0) goto _exit_write;
  }
  I2C_SendData  ( I2C2, RegValue);
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED))     //ev8_2
	{
    if((Max30100_Timeout--) == 0) goto _exit_write;
  }	
  I2C_GenerateSTOP(I2C2, ENABLE);
  
 
  
  return 1;
_exit_write:
	I2C_GenerateSTOP(I2C2, ENABLE);
	I2ComError_Flag = 1;
	return 0;
}
uint8_t Max30100_ReadRegDMA(uint8_t RegName,uint8_t * pRegValueBuf,int Len)
{
	
  
   /* Test on BUSY Flag */
  Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY)) 
  {
    if((Max30100_Timeout--) == 0) goto _exit_READ_DMA;
  }
  
  /* Configure DMA Peripheral */
  DMA_Config(DMA_RX, (uint8_t*)pRegValueBuf, Len);  
  
  /* Enable DMA NACK automatic generation */
  I2C_DMALastTransferCmd(I2C2, ENABLE);
  
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(I2C2, ENABLE);
  
  /* Test on SB Flag */
  Max30100_Timeout = MAX30100_FLAG_TIMEOUT;
  while (!I2C_GetFlagStatus(I2C2,I2C_FLAG_SB)) 
  {
    if((Max30100_Timeout--) == 0) goto _exit_READ_DMA;
  }
  I2C_AcknowledgeConfig(I2C2, ENABLE); //
  /* Send device address for write */
  I2C_Send7bitAddress(I2C2, MAX30100_ADDR, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  Max30100_Timeout = MAX30100_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) 
  {
    if((Max30100_Timeout--) == 0) goto _exit_READ_DMA;
  }
  
  /* Send the device's internal address to write to */
  I2C_SendData(I2C2, RegName);  
  
  /* Test on TXE FLag (data sent) */
  Max30100_Timeout = MAX30100_FLAG_TIMEOUT;
  while ((!I2C_GetFlagStatus(I2C2,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C2,I2C_FLAG_BTF)))  
  {
    if((Max30100_Timeout--) == 0) goto _exit_READ_DMA;
  }
  
  /* Send START condition a second time */  
  I2C_GenerateSTART(I2C2, ENABLE);
  
  /* Test on SB Flag */
  Max30100_Timeout = MAX30100_FLAG_TIMEOUT;
  while (!I2C_GetFlagStatus(I2C2,I2C_FLAG_SB)) 
  {
    if((Max30100_Timeout--) == 0) goto _exit_READ_DMA;
  }
  
  /* Send LM75 address for read */
  I2C_Send7bitAddress(I2C2, MAX30100_ADDR, I2C_Direction_Receiver);
  
  /* Test on ADDR Flag */
  Max30100_Timeout = MAX30100_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))   
  {
    if((Max30100_Timeout--) == 0) goto _exit_READ_DMA;
  }
  
  /* Enable I2C DMA request */
  I2C_DMACmd(I2C2,ENABLE);
  
  /* Enable DMA RX Channel */
  DMA_Cmd(MAX30100_DMA_RX_CHANNEL, ENABLE);
  
  /* Wait until DMA Transfer Complete */
  Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  while (!DMA_GetFlagStatus(MAX30100_DMA_RX_TCFLAG))
  {
    if((Max30100_Timeout--) == 0) goto _exit_READ_DMA;
  }        
  
  /* Send STOP Condition */
  I2C_GenerateSTOP(I2C2, ENABLE);
  
  /* Disable DMA RX Channel */
  DMA_Cmd(MAX30100_DMA_RX_CHANNEL, DISABLE);
  
  /* Disable I2C DMA request */  
  I2C_DMACmd(I2C2,DISABLE);
  
  /* Clear DMA RX Transfer Complete Flag */
  DMA_ClearFlag(MAX30100_DMA_RX_TCFLAG);
  
 
  return 1; 
_exit_READ_DMA:
	I2C_GenerateSTOP(I2C2, ENABLE);
	I2ComError_Flag = 1;
	return 0;	
}

void I2C_WaitEepromStandbyState(u8 id)         //????,????
{
    vu16 SR1_Tmp = 0;

  do
  {
    /* Send START condition */
    I2C_GenerateSTART(I2C2, ENABLE);
    /* Read I2C1 SR1 register */
    SR1_Tmp = I2C_ReadRegister(I2C2, I2C_Register_SR1);
    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C2, id, I2C_Direction_Transmitter);
  }while(!(I2C_ReadRegister(I2C2, I2C_Register_SR1) & 0x0002));      //estimate the value of ADDR
  
  /* Clear AF flag */
  I2C_ClearFlag(I2C2, I2C_FLAG_AF);    
}
uint8_t Max30100_ReadReg(uint8_t RegName,uint8_t * pRegValue)
{
	u8 temp;
	
  
  //I2C_WaitEepromStandbyState(MAX30100_ADDR);
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY)) {
        if((Max30100_Timeout--) == 0) goto _exit;
    }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;
	I2C_GenerateSTART(I2C2, ENABLE);
	
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT)) {
        if((Max30100_Timeout--) == 0) goto _exit;
    }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;	
	I2C_AcknowledgeConfig(I2C2, DISABLE); //
	I2C_Send7bitAddress  ( I2C2,MAX30100_ADDR, I2C_Direction_Transmitter);
	
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if((Max30100_Timeout--) == 0) goto _exit;
    }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;	
	I2C_SendData  ( I2C2, RegName);
	
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {
        if((Max30100_Timeout--) == 0) goto _exit;
    }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;	
	I2C_GenerateSTART(I2C2, ENABLE);      //restart
	
  while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT))//ev5
	{
    if((Max30100_Timeout--) == 0) goto _exit;
  }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;		
	I2C_Send7bitAddress  ( I2C2,MAX30100_ADDR, I2C_Direction_Receiver);
	
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) //ev6
	{
    if((Max30100_Timeout--) == 0) goto _exit;
  }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;	
	I2C_GenerateSTOP(I2C2, ENABLE);
	
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED))//ev7
	{
    if((Max30100_Timeout--) == 0) goto _exit;
  }
		
		temp =I2C_ReceiveData(I2C2);
	*pRegValue=temp;

	return 1;
  
	_exit:
	I2C_GenerateSTOP(I2C2, ENABLE);
	I2ComError_Flag = 1;
	return 0;
  
}
uint8_t Max30100_ReadFIFOData(uint8_t read_ptr,uint8_t * buff,uint8_t buf_len)
{
	int j;
	uint8_t i = 0;
	uint8_t temp = 0;
	//
	//I2C_WaitEepromStandbyState(MAX30100_ADDR);
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;
  while (I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY)) 
  {
    if((Max30100_Timeout--) == 0) goto _exit_FIFO;
  }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;
	I2C_GenerateSTART(I2C2, ENABLE);
	
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT))
	{
    if((Max30100_Timeout--) == 0) goto _exit_FIFO;
  }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;	
	I2C_AcknowledgeConfig(I2C2, ENABLE); //
	I2C_Send7bitAddress  ( I2C2,MAX30100_ADDR, I2C_Direction_Transmitter);
	
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
    if((Max30100_Timeout--) == 0) goto _exit_FIFO;
  }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;	
	I2C_SendData  ( I2C2, REG_FIFO_DATA_ADDR);
	
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
    if((Max30100_Timeout--) == 0) goto _exit_FIFO;
  }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;	
	I2C_GenerateSTART(I2C2, ENABLE);      //restart
	
  while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT))//ev5
	{
    if((Max30100_Timeout--) == 0) goto _exit_FIFO;
  }
	Max30100_Timeout = MAX30100_LONG_TIMEOUT;		
	I2C_Send7bitAddress  ( I2C2,MAX30100_ADDR, I2C_Direction_Receiver);
	
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) //ev6
	{
    if((Max30100_Timeout--) == 0) goto _exit_FIFO;
  }
	
		//I2C_AcknowledgeConfig(I2C2, ENABLE);

		for( i = 0; i < buf_len;i++)
	{
		Max30100_Timeout = MAX30100_LONG_TIMEOUT;
		while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED))//ev7
	{
    if((Max30100_Timeout--) == 0) goto _exit_FIFO;
  }
		
		buff[i] = I2C_ReceiveData(I2C2);
		
	
	
	 
		
	}
	 

    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C2, ENABLE);

    
    
	
	return 1;
	_exit_FIFO:
	I2C_GenerateSTOP(I2C2, ENABLE);
	I2ComError_Flag = 1;
	return 0;
}


