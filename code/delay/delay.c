#include "delay.h"
#include "stm32f10x.h"

void delay_us(u32 nus)
{
	u32 temp;   

	SysTick->LOAD = 9*nus;   
	SysTick->VAL  = 0x00;	//��ռ�����  
	SysTick->CTRL = 0x01;	//ʹ�� ����0�޶����������ⲿʱ��Դ

	do  {    
		temp=SysTick->CTRL;	//����ǰ������ֵ
	} while((temp&0x01)&&(!(temp&(1<<16))));	//�ȴ�ʱ�䵽��

	SysTick->CTRL = 0x00;		//�رն�ʱ��  
	SysTick->VAL  = 0X00;		//��ն�ʱ��
}

void delay_ms(u32 nms)
{
	u32 temp;   
	SysTick->LOAD = 9000 * nms;
	SysTick->VAL  = 0x00;		//��ռ�����  
	SysTick->CTRL = 0x01;		//ʹ�� ����0�޶����������ⲿʱ��Դ

	do  {    
		temp=SysTick->CTRL;		//����ǰ������ֵ
	} while((temp&0x01)&&(!(temp&(1<<16))));	//�ȴ�ʱ�䵽��

	SysTick->CTRL = 0x00;		//�رն�ʱ��  
	SysTick->VAL  = 0X00;		//��ն�ʱ��
}
