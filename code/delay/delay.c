#include "delay.h"
#include "stm32f10x.h"

void delay_us(u32 nus)
{
	u32 temp;   

	SysTick->LOAD = 9*nus;   
	SysTick->VAL  = 0x00;	//清空计数器  
	SysTick->CTRL = 0x01;	//使能 减到0无动作，采用外部时钟源

	do  {    
		temp=SysTick->CTRL;	//读当前到计数值
	} while((temp&0x01)&&(!(temp&(1<<16))));	//等待时间到达

	SysTick->CTRL = 0x00;		//关闭定时器  
	SysTick->VAL  = 0X00;		//清空定时器
}

void delay_ms(u32 nms)
{
	u32 temp;   
	SysTick->LOAD = 9000 * nms;
	SysTick->VAL  = 0x00;		//清空计数器  
	SysTick->CTRL = 0x01;		//使能 减到0无动作，采用外部时钟源

	do  {    
		temp=SysTick->CTRL;		//读当前到计数值
	} while((temp&0x01)&&(!(temp&(1<<16))));	//等待时间到达

	SysTick->CTRL = 0x00;		//关闭定时器  
	SysTick->VAL  = 0X00;		//清空定时器
}
