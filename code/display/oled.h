#ifndef __OLED_H
#define __OLED_H			  	 
#include "stm32_ioport.h"
//0:4�ߴ���ģʽ
#define ASCII12x8  	6  //һ��Ӣ��ascii���������*8
#define ASCII16x16  16
#define ASCII24x24 	36
#define ASCII32x32 	64

#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED�˿ڶ���----------------  					   
#define OLED_CS_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_0)//CS
#define OLED_CS_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_0)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_1)//DC
#define OLED_DC_Set() GPIO_SetBits(GPIOB,GPIO_Pin_1)

#define OLED_RST_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_3)//RES
#define OLED_RST_Set() GPIO_SetBits(GPIOB,GPIO_Pin_3)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_4)//DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_4)
		     
#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_5)//CLK
#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_5)

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

//OLED�����ú���
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x ,u8 y);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 size,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u16 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,u8 size, char *p);	 
void OLED_Set_Pos(u8 x, u8 y);
void OLED_DrawBMP(u8 x0, u8 y0,u8 x1,u8 y1,const u8 BMP[]);
void OLED_ClearBMP(u8 x0, u8 y0,u8 x1,u8 y1);
void LCD_Str_CHinese(u8 x,u8 y,vu8 *str);
void OLED_DrawPoint_Inverse(u8 x ,u8 yy);
void OLED_ClearLine(u8 x);
#endif 
