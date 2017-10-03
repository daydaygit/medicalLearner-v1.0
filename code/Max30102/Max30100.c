#include "Max_30100_I2C.h"
#include "Max30100_Config.h"
#include "Max30100.h"
#include "soft_i2c_core.h"

static int Sample_len = 6;
__IO uint8_t FIFO_Read_Ptr = 0;


NVIC_InitTypeDef   NVIC_InitStructure;



//#define USE_SOFT_I2C 1
void Max30102_EXTI_Config(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	
  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  /* Configure PA.60 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Enable AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	
  /* Connect EXTI0 Line to PA.50 pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI6 Interrupt to the lowest priority */
	
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
}

Error_Success InitMax30100()
{
	uint8_t RegValue;

	Max30102_EXTI_Config();

#ifdef USE_SOFT_I2C
	BSP_InitI2C();

	ResetMax30100();	/*复位30100*/

  if(!Max30102_SoftI2C_Write(REG_INT_ENALE1_ADDR,0xc0))      {   return I2C_Com_Error;   } // INTR setting

  if(!Max30102_SoftI2C_Write(REG_INT_ENALE2_ADDR,0x00))      {   return I2C_Com_Error;   }

  if(!Max30102_SoftI2C_Write(REG_FIFO_WR_PTR_ADDR,0x00))     {   return I2C_Com_Error;   }  //FIFO_WR_PTR[4:0]

  if(!Max30102_SoftI2C_Write(REG_FIFO_OV_COUNTER_ADDR,0x00)) {   return I2C_Com_Error;   }  //OVF_COUNTER[4:0]

  if(!Max30102_SoftI2C_Write(REG_FIFO_RD_PTR_ADDR,0x00))     {   return I2C_Com_Error;   }  //FIFO_RD_PTR[4:0]

  if(!Max30102_SoftI2C_Write(REG_FIFO_CONFIG_ADDR,0x0f))     {   return I2C_Com_Error;   }  //sample avg = 1, fifo rollover=false, fifo almost full = 17

  if(!Max30102_SoftI2C_Write(REG_MODE_CONFIG_ADDR,0x03))     {   return I2C_Com_Error;   }   //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED

  if(!Max30102_SoftI2C_Write(REG_SPO2_CONFIG_ADDR,0x27))     {   return I2C_Com_Error;   }  // SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)

  
  if(!Max30102_SoftI2C_Write(REG_LED1_PA_ADDR,0x24))         {   return I2C_Com_Error;   }   //Choose value for ~ 7mA for LED1

  if(!Max30102_SoftI2C_Write(REG_LED2_PA_ADDR,0x24))         {   return I2C_Com_Error;   }   // Choose value for ~ 7mA for LED2

  if(!Max30102_SoftI2C_Write(REG_PILOT_PA_ADDR,0x7f))        {   return I2C_Com_Error;   }   // Choose value for ~ 25mA for Pilot LED

#else

	InitMax30100I2C();	/*初始化30100的I2C通讯*/

	ResetMax30100();	/*复位30100*/  
	
	/*中断设置*/
	if(Max30100_WriteReg(REG_INT_ENALE1_ADDR,INT_A_FULL_EN) <= 0) {   return I2C_Com_Error;   }//INT_PROX_INT_EN INT_PPG_RDY_EN 开放中断，FIFO满，new data，光照条件

	if(Max30100_WriteReg(REG_INT_ENALE2_ADDR,0x00) <= 0)	      {   return I2C_Com_Error;   }

	
	//FIFO 配置
	if(Max30100_WriteReg(REG_FIFO_WR_PTR_ADDR,0) <= 0) 	          {   return I2C_Com_Error;	}	/*写指针清0*/

	if(Max30100_WriteReg(REG_FIFO_OV_COUNTER_ADDR,0) <= 0)	      {   return I2C_Com_Error;	}	/*溢出值清0*/

	if(Max30100_WriteReg(REG_FIFO_RD_PTR_ADDR,0) <= 0)	          {   return I2C_Com_Error;	}	/*读指针清0*/
	
	if(Max30100_WriteReg(REG_FIFO_CONFIG_ADDR,AMP_AVE_BY_8|0xf) <= 0) {   return I2C_Com_Error;   } //相邻4点平均, fifo 覆盖存储禁止, FIFO 满标志 = 17


	/*工作模式配置*/
	SetMax30100WorkMode(SpO2_Only);//0x02 HR 只红灯, 0x03 for SpO2 模式 0x07 复合 LED

	if(Max30100_WriteReg(REG_SPO2_CONFIG_ADDR,SPO2_ADC_RGE_2|SPO2_SR_100|LED_PW_AD_18) <= 0)   {   return I2C_Com_Error;   }	//SpO2 模式

	if(Max30100_WriteReg(REG_LED1_PA_ADDR,0x41) <= 0)             {   return I2C_Com_Error;   } //设置LED1 ~ 7mA 

	if(Max30100_WriteReg(REG_LED2_PA_ADDR,0x41) <= 0)             {   return I2C_Com_Error;   } //设置LED2 ~ 7mA 

	if(Max30100_WriteReg(REG_PILOT_PA_ADDR,0x8f) <= 0)            {   return I2C_Com_Error;   } //25mA for Pilot LED

	if(Max30100_WriteReg(REG_PROXIMITY_INT_THRESH_ADDR,30) <= 0)  {   return I2C_Com_Error;   } //25mA for Pilot LED

    /*if(Max30100_WriteReg(REG_MULTI_LED_CONFIG1_ADDR,(LED1_RED_PA<<SLOT13_POS)|(LED2_IR_PA<<SLOT24_POS)) <= 0)  	{	return I2C_Com_Error;	} //25mA for Pilot LED */
#endif

	return Success;
}
uint8_t ResetMax30100()
{
	#ifdef USE_SOFT_I2C
	if(Max30102_SoftI2C_Write(REG_MODE_CONFIG_ADDR,MODE_REST) <= 0 )
	{
		return I2C_Com_Error;
	}
	#else
	if( Max30100_WriteReg (REG_MODE_CONFIG_ADDR,MODE_REST) <= 0)
	{
		return I2C_Com_Error;
	}
	#endif
	return Success;
}
uint8_t ReadMax30100Tmp(uint8_t * tmp)
{
	#ifdef USE_SOFT_I2C
	*tmp =  Max30102_SoftI2C_Read (REG_DIE_TMP_INTEGER_ADDR);
#else	
	if( Max30100_ReadReg (REG_DIE_TMP_INTEGER_ADDR,tmp) <= 0)
	{
		return I2C_Com_Error;
	}
	#endif
	return Success;
}
uint8_t ReadMax30100Status(uint16_t * status)
{
	uint8_t temp;

#ifdef USE_SOFT_I2C
	*status =  Max30102_SoftI2C_Read (REG_INT_STATUS0_ADDR);
	temp =  Max30102_SoftI2C_Read (REG_INT_STATUS1_ADDR);
	*status += (temp<<8);
#else	
	if( Max30100_ReadReg (REG_INT_STATUS0_ADDR,&temp) <= 0)
	{
		return I2C_Com_Error;
	}
	*status = temp;
	if( Max30100_ReadReg (REG_INT_STATUS1_ADDR,&temp) <= 0)
	{
		return I2C_Com_Error;
	}
	*status += (temp<<8);
#endif

	return Success;
}
uint8_t ReadMax30100FifoLength(uint8_t * sampleLen)
{
	uint8_t tmp_write;
	uint8_t tmp_read;
	if(Max30100_ReadReg(REG_FIFO_WR_PTR_ADDR,&tmp_write) <= 0)
	{
		return I2C_Com_Error;
	}
	if(Max30100_ReadReg(REG_FIFO_RD_PTR_ADDR,&tmp_read) <= 0)
	{
		return I2C_Com_Error;
	}
	FIFO_Read_Ptr = tmp_read;
	if( tmp_write >= tmp_read)
		* sampleLen = tmp_write - tmp_read;
	else
		* sampleLen = MAX30100_FIFO_LENGTH - tmp_read + tmp_write;
	
	return Success;
}
void FIFO_Read(uint32_t *pun_red_led, uint32_t *pun_ir_led)
{
	
	#ifdef USE_SOFT_I2C
	*pun_red_led = 0;
	*pun_ir_led = 0;
	Max30102_SoftI2C_FIFO_Read(pun_red_led,pun_ir_led);
	#else
	 uint32_t un_temp;
	uint8_t TmpBuf[6];
	
	*pun_red_led = 0;
	*pun_ir_led = 0;
	if(Max30100_ReadRegDMA(REG_FIFO_DATA_ADDR,TmpBuf,1*Sample_len)> 0 )
	//if( Max30100_ReadFIFOData(FIFO_Read_Ptr,TmpBuf,Sample_len) > 0)
	{
		 un_temp=(unsigned char) TmpBuf[0];
  un_temp<<=16;
  *pun_red_led+=un_temp;
  un_temp=(unsigned char) TmpBuf[1];
  un_temp<<=8;
  *pun_red_led+=un_temp;
  un_temp=(unsigned char) TmpBuf[2];
  *pun_red_led+=un_temp;
  
  un_temp=(unsigned char) TmpBuf[3];
  un_temp<<=16;
  *pun_ir_led+=un_temp;
  un_temp=(unsigned char) TmpBuf[4];
  un_temp<<=8;
  *pun_ir_led+=un_temp;
  un_temp=(unsigned char) TmpBuf[5];
  *pun_ir_led+=un_temp;
  *pun_red_led&=0x03FFFF;  //Mask MSB [23:18]
  *pun_ir_led&=0x03FFFF;  //Mask MSB [23:18]
	}
	#endif
}
uint8_t ReadMax30100FifoBuf(uint8_t * buf,uint8_t sampleLen)
{
	if(Max30100_ReadRegDMA(REG_FIFO_DATA_ADDR,buf,sampleLen*Sample_len)> 0 )
	{
		return Success;
	}
	/*if( Max30100_ReadFIFOData(FIFO_Read_Ptr,buf,sampleLen*Sample_len) > 0)
	{
		FIFO_Read_Ptr += sampleLen;
		if( FIFO_Read_Ptr >= MAX30100_FIFO_LENGTH )
			FIFO_Read_Ptr = 0;
		return Success;
	}*/
	return I2C_Com_Error;
}
/**********************************************
//函数名称
//输入参数
//返回值
//功能说明
**********************************************/
uint8_t SetMax30100WorkMode(Work_Mode mode)
{
	uint8_t WriteValue=0;
	/*if(Max30100_ReadReg(REG_INT_STATUS0_ADDR,&WriteValue) <= 0)
	{
		return I2C_Com_Error;
	}
	WriteValue &= MODE_MASK;*/
	Sample_len = 6;
	if( mode == HR_Only )
	{
		Sample_len = 3;
		WriteValue |= MODE_HR_ONLY;
	}
	else if( mode == SpO2_Only )
	{
		WriteValue |= MODE_SPO2_ONLY;
	}
	else
	{
		WriteValue |= MODE_SPO2_HR;
	}
	#ifdef USE_SOFT_I2C
	if( Max30102_SoftI2C_Write (REG_MODE_CONFIG_ADDR,WriteValue) <= 0)
	{
		return I2C_Com_Error;
	}
#else	
	if( Max30100_WriteReg (REG_MODE_CONFIG_ADDR,WriteValue) <= 0)
	{
		return I2C_Com_Error;
	}
	#endif
	
	return Success;
}