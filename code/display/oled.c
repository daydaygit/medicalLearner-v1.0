
//  功能描述   : OLED 4接口演示例程(51系列)
//              GND    电源地
//              VCC  接5V或3.3v电源
//              D0   接PA7     换成   PB5
//              D1   接PA6     换成   PB4
//              RES  接PA5     换成   PB3
//              DC   接PA4     换成   PB1
//              CS   接PA3     换成   PB0
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

#include "oled.h"
#include "oledfont.h"  	 
#include "delay.h"

u8 OLED_GRAM[128][8];

#ifdef FUNC_OLED_QXY
/**
 * strlen - Find the length of a string
 * @s: The string to be sized
 */
u32 stringLen(const char *s)
{
        const char *sc;

        for (sc = s; *sc != '\0'; ++sc)
                /* nothing */;
        return sc - s;
}
#endif

//向SSD1106写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 
//      0: 命令;
//      1: 数据;
void OLED_WR_Byte(u8 dat,u8 cmd)          // 4-wire SPI write
{	
	u8 i;			  

	if(cmd)   {   OLED_DC_Set();   }            // 数据,则B1_DC = H
	else      {   OLED_DC_Clr();   }            // 命令,则B1_DC = L

	OLED_CS_Clr();                              // B0_CS = L                        //先设置DC，再设置CS

	for(i=0;i<8;i++)
	{  
		OLED_SCLK_Clr();                        // B5_D0 = L   //D0: SCLK

		if(dat&0x80)  {   OLED_SDIN_Set();   }  // B4_D1 = H   //D1: SDin
		else          {   OLED_SDIN_Clr();   }  // B4_D1 = L

		OLED_SCLK_Set();                        // B5_D0 = H

		dat<<=1;   
	}

	OLED_CS_Set();                              // B0_CS  = H                      //先设置CSCS，再设置DC
	OLED_DC_Set();                              // B1_DC  = H	  
} 

void OLED_Set_Pos(u8 x, u8 y) 
{ 
	OLED_WR_Byte(0xb0+y,              OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10, OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,       OLED_CMD); 
}

//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)
{  
	u8 i,n;	

	for(i=0;i<8;i++)                        //page0~7共8页
	{  
		OLED_WR_Byte(0xb0+i,OLED_CMD);      //设置页地址（0~7）
		OLED_WR_Byte(0x02,  OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte(0x10,  OLED_CMD);      //设置显示位置—列高地址 

		for(n=0; n<128; n++)
			OLED_WR_Byte(0,OLED_DATA); 
	}
}

//在指定位置显示一个字符 调用OLED_ShowChar(48,6,16,t);
//x0,y0: 显示起点坐标,x0范围(0～127), y0范围(0～7) * 8 
//size:  选择字体 16/12 
//chr:   字符串ascii码
void OLED_ShowChar(u8 x, u8 y, u8 size, u8 chr)  // x-y: 128-64    //48, 0
{      	
	u8 c=0,i=0;	
	c=chr-' ';                      //得到偏移后的值.  其中" "(space)是ASCII第32个字符
	if(x > X_WIDTH - 1) {
		x = 0;
		y = y + 2;
    }

	if(size ==ASCII32x32)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<ASCII32x32/4;i++)   // 64 / 4 = 16
		   OLED_WR_Byte(ASCII16x32[c*ASCII32x32+i],OLED_DATA);

		OLED_Set_Pos(x,y+1);
		for(i=0;i<ASCII32x32/4;i++)
		   OLED_WR_Byte(ASCII16x32[c*ASCII32x32+i+ASCII32x32/4],OLED_DATA);  //四列显示完

		OLED_Set_Pos(x,y+2);
		for(i=0;i<ASCII32x32/4;i++)
		   OLED_WR_Byte(ASCII16x32[c*ASCII32x32+i+2*ASCII32x32/4],OLED_DATA);

		OLED_Set_Pos(x,y+3);
		for(i=0;i<ASCII32x32/4;i++)
		   OLED_WR_Byte(ASCII16x32[c*ASCII32x32+i+3*ASCII32x32/4],OLED_DATA);

	} else if(size ==ASCII24x24) {
		OLED_Set_Pos(x,y);	
		for(i=0;i<ASCII24x24/3;i++)   // 36  / 3 = 12
		   OLED_WR_Byte(ASCII12x24[c*ASCII24x24+i],OLED_DATA);

		OLED_Set_Pos(x,y+1);
		for(i=0;i<ASCII24x24/3;i++)
		   OLED_WR_Byte(ASCII12x24[c*ASCII24x24+i+ASCII24x24/3],OLED_DATA);//三列显示完

		OLED_Set_Pos(x,y+2);
		for(i=0;i<ASCII24x24/3;i++)
		   OLED_WR_Byte(ASCII12x24[c*ASCII24x24+i+2*ASCII24x24/3],OLED_DATA);

	} else if(size ==ASCII16x16) {                                        // 走的是8x16的字库
		OLED_Set_Pos(x,y);	
		for(i=0;i<ASCII16x16/2;i++)   // 16 / 2 = 8     //2*8*8 二列
		   OLED_WR_Byte(ASCII8X16[c*ASCII16x16+i],OLED_DATA);

		OLED_Set_Pos(x,y+1);
		for(i=0;i<ASCII16x16/2;i++)
		   OLED_WR_Byte(ASCII8X16[c*ASCII16x16+i+ASCII16x16/2],OLED_DATA); //两列显示完

	} else if(size ==ASCII12x8) {	
		OLED_Set_Pos(x,y);
		for(i=0;i<ASCII12x8;i++)
		   OLED_WR_Byte(ASCII6x8[c*ASCII12x8+i],OLED_DATA);	               //一列显示完
	}
}

void OLED_ShowWord(u8 x, u8 y, u8 size, u32 index)  // x-y: 128-64    //48, 0  .    char已经获取到该字在GB_WORD_24x24[]的下标
{
	u8 i=0, j=0;	

	if(x > X_WIDTH - 1)				{  x=0;  y=y+2;  }

	/* step:
	 *  1. 设置x,y坐标
	 *  2. 写一个字节数据(使用OLED_WR_Byte) , 如此循环24次就可以写完一行
	 *  3. 重新设置x,y坐标
	 *  4. 继续写一个字节数据, 如此循环24次写完第二行
	 *  5. 重新设置x,y坐标
	 *  6. 继续写一个字节数据, 如此循环24次写完第二行
	 */

	switch(size) {
	  case WIDTH_24x24:
						for(j=0; j< (WIDTH_24x24 / 8); j++) {   // 纵向24个点，每8个点是一个字节
							OLED_Set_Pos(x,y);
							for(i=0; i < WIDTH_24x24; i++) {
								OLED_WR_Byte(GB_WORD_24x24[index + i], OLED_DATA);
							}

							if(i == WIDTH_24x24 - 1)  {  y+=1;  }
						}
						break;
	  default:
						break;
	}



}


//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	

//显示数字 调用OLED_ShowNum(103,6,t,3,16); 		  
//x0,y0显示起点坐标,x0(范围0～127),y0(范围0～7)*8 
//len :数字的位数
//size:选择字体 16/12 
//num:数值(0~65536);	
void OLED_ShowNum(u8 x,u8 y,u16 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,size,' ');
				continue;
			}else enshow=1; 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,size,temp+'0'); 
	}
} 

//显示一个字符号串 调用OLED_ShowString(30,0,"OLED TEST");
//x0,y0: 显示起点坐标,x0(范围0～127), y0(范围0～7)*8
//size:   选择字体 16/12 
//char:  字符串ascii码
void OLED_ShowString(u8 x,u8 y,u8 size,char *chr)
{
	u8 j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x, y, size, chr[j]);
		if(size==ASCII12x8)        {  x+=6;             }         //单个ascii偏移量
		else if(size==ASCII16x16)  {  x+=ASCII16x16/2;  }         //单个ascii偏移量
		else if(size==ASCII24x24)  {  x+=ASCII24x24/3;  }         //单个ascii偏移量
		else if(size==ASCII32x32)  {  x+=ASCII32x32/4;  }         //单个ascii偏移量

 		if(x > X_WIDTH - size) {
			x=0;
			y+=size/8;
		}

 		j++;
	}
}

#ifdef FUNC_OLED_QXY

u8 OLED_show_ellipsis(u8 x, u8 y, u8 size)
{
	u8 i,j,x1,y1;
	u8 ret=0;

	switch(size) {
	  case WIDTH_24x24:
						if((x+ELL_5x8 > X_WIDTH) || (y+8 > Y_WIDTH)) {	// 一个点都不能打,立马返回1
							return 1;
						}

						for(i=0; i<3; i++) {							// 预定打印3个'.'，不能打够3个则退出,但肯定有打印,返回0
							x1 = x+ELL_5x8*i;
							y1 = y;

							if((x1+ELL_5x8 > X_WIDTH) || (y1+8 > Y_WIDTH)) {	  // 允许不打印完3个点,但当前点放不下，就跳出
								return 0;
							}

							OLED_Set_Pos(x1, y1);

							for(j=0; j<ELL_5x8; j++) {
								OLED_WR_Byte(ellipsis_5x8[j],OLED_DATA);
							}
						}
						return 0;
	  default:
	  					ret = 1;
				break;
	}

	return ret;
}


/*
  *null,0和'\0'在ascii中用数字读取时值是0，用字符串读取时就是'\0'(结束符)或者null(与编译器有关)
  * 而字符'\0'在ascii中,用数字读取时值是48
  * Ps：在数组和字符串中，0和'\0'是等价的,都能表示结束符。
  */
void OLED_show_case_word(struct characters_prop *dots_info)
{
	u8 i,j,k,x1,y1;

	if(dots_info == NULL)
		return;

	switch(dots_info->size) {
	  case WIDTH_12x16:
						for(i=0; i<stringLen(dots_info->index); i++) {
							x1 = dots_info->x + WIDTH_12x16 * i + dots_info->xgap * i;
							y1 = dots_info->y;

							//注意: 纵向高度还是16
							for(j=0; j<WIDTH_16x16/8; j++) {		// 每个24x24字体的字循环打点3次(换一行重来共3次)
								OLED_Set_Pos(x1,y1+j);				// 每重来一次,y+1

								for(k=0; k<WIDTH_12x16; k++) {		// 每个24x24字体的字横向打点24 个字节
																	// POS会自加1 
									OLED_WR_Byte(GB_WORD_12x16[(dots_info->index[i]* WIDTH_12x16 * 2) + (j * WIDTH_12x16) + k], OLED_DATA);
								}
							}
						}

						break;

	  case WIDTH_16x16:
						for(i=0; i<stringLen(dots_info->index); i++) {
							x1 = dots_info->x + WIDTH_16x16 * i + dots_info->xgap * i;
							y1 = dots_info->y;

							for(j=0; j<WIDTH_16x16/8; j++) {	  	// 每个24x24字体的字循环打点3次(换一行重来共3次)
								OLED_Set_Pos(x1,y1+j);		  	// 每重来一次,y+1

								for(k=0; k<WIDTH_16x16; k++) {   // 每个24x24字体的字横向打点24 个字节
																// POS会自加1 
									OLED_WR_Byte(GB_WORD_16x16[(dots_info->index[i]* WIDTH_16x16 * 2) + (j * WIDTH_16x16) + k], OLED_DATA);
								}
							}
						}

						break;

	  case WIDTH_24x24:
	  					//for(i=0; dbg_arr[i] != '\0'; i++) {  //由于第一个index就是0，而'\0'也是0，所以一开始调试时是什么都没有
						for(i=0; i<stringLen(dots_info->index); i++) {
							x1 = dots_info->x + WIDTH_24x24 * i + dots_info->xgap * i;
							y1 = dots_info->y;

							for(j=0; j<WIDTH_24x24/8; j++) {	// 每个24x24字体的字循环打点3次(换一行重来共3次)
								OLED_Set_Pos(x1,y1+j);			// 每重来一次,y+1

								for(k=0; k<WIDTH_24x24; k++) {	// 每个24x24字体的字横向打点24 个字节
																// POS会自加1 
									OLED_WR_Byte(GB_WORD_24x24[(dots_info->index[i]* WIDTH_24x24 * 3) + (j * WIDTH_24x24) + k], OLED_DATA);
								}
							}
						}
						break;
	  default:			break;
	}

}

#endif

void OLED_ClearBMP(u8 x0, u8 y0,u8 x1,u8 y1)
{
	
	 u8 x,y;
  
  //if(y1%8==0) y1=y1/8;      
  //else y1=y1/8+1;
	
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x0+x1;x++)
		{      
			OLED_WR_Byte(0,OLED_DATA);	    	
		}
	}
}	
//显示显示BMP图片 调用OLED_DrawBMP(0,0,128,8,BMP1);最大图片
//x0,y0:显示起点坐标,x0(范围0～127),y0(范围0～7)*8
//x1,y1:显示图片长度和宽度,x1(范围0～127),y1(范围0～7)*8
//BMP[]:图片首地址
void OLED_DrawBMP(u8 x0, u8 y0,u8 x1, u8 y1,const u8 BMP[])
{ 	
	u16 j=0;
	u8 x,y;

	//if(y1%8==0) y1=y1/8;      
	//else y1=y1/8+1;

	for(y=y0;y<y1;y++) {
		OLED_Set_Pos(x0,y);
		for(x=x0;x<x0+x1;x++) {      
			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
		}
	}
}

void OLED_ClearLine(u8 x)
{	u8 y;
	if(x==128)
		x=0;
	
	x+=2;
	
	for(y=2;y<6;y++){
	
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	//列选择
	
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	//OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 	
	OLED_WR_Byte((x&0x0f),OLED_CMD);
	OLED_GRAM[x-2][y]=0;	
	OLED_WR_Byte(OLED_GRAM[x-2][y],OLED_DATA);  	
	}		
}	
void OLED_DrawPoint(u8 x ,u8 y) //这个函数有问题，参考下面的再改
{ 
	u8 data=0x01;
	u8 y1;
	//u8 y2=0x01;
	
	//行选择
	//y/=8;
	OLED_WR_Byte(0xb0+y/8,OLED_CMD);
	//列选择
	x+=2;
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	//OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 	
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
	
	y1=y%8;
	if(y1==0){
	OLED_GRAM[x-2][y/8]|=data;
  OLED_WR_Byte(OLED_GRAM[x-2][y/8],OLED_DATA);  
	}
	else{
		data<<=y1;
		OLED_GRAM[x-2][y/8]|=data;
		OLED_WR_Byte(OLED_GRAM[x-2][y/8],OLED_DATA);  
		
	}
} 
void OLED_DrawPoint_Inverse(u8 x ,u8 yy) //x:0~127,y:0~63
{ 
	u8 data=0x01;
	u8 y;
	u8 y1;
	u8 y2;
	
	//将y轴反向
	if(yy>63)
		yy=63;
	y=63-yy;
	
	//u8 y2=0x01;
	
	//行选择	
	   
	y2=y/8;
	OLED_WR_Byte(0xb0+y2,OLED_CMD);
	
	//列选择
	x+=2;
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	//OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 	
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
		
	y1=y%8;
	data<<=y1;
	OLED_GRAM[x-2][y2] |=data;
	OLED_WR_Byte(OLED_GRAM[x-2][y2],OLED_DATA);  

	
	
} 
//-----------------------------------显示中文------------------
u32 *getchinesecodepos(u16 ac)
{   
		u16 min,max,mid,midc;
    min=0;
    max=CHINESECHARNUMBER-1;
    while(1)
    {   if(max-min<2)
        {   if(ChineseCode[min]==ac)
                mid=min;
            else if(ChineseCode[max]==ac)
                mid=max;
            else
                mid=0;
            break;
        }
        mid=(max+min)/2;
        midc=ChineseCode[mid];
        if(midc==ac)
            break;
        else if(midc>ac)
            max=mid-1;
        else
            min=mid+1;
    }
    return (u32*)(ChineseCharDot+mid*CHINESECHARDOTSIZE);
}

void Get_Code(u8* dis_buf,u32* address,u8 leng)
{
	u8 i;
	for(i=0;i<leng/4;i++)  //32为单片机4个字节
	{
		*dis_buf=*address;
		dis_buf++;
		*dis_buf=*address>>8;
		dis_buf++;
		*dis_buf=*address>>16;
		dis_buf++;
		*dis_buf=*address>>24;
		dis_buf++;
		address++;
	}
}

void LCD_Char_CH(u8 x,u8 y,vu8 *str)
{
    u8  i,buffer[CHINESECHARDOTSIZE]={0};
		u16  chinese_ascii;
		u32 *p_address;
		chinese_ascii = *str;
		str++;
		chinese_ascii = chinese_ascii<<8|*str; 		//找到汉字内码	
		p_address=getchinesecodepos(chinese_ascii);										
		Get_Code(buffer,p_address,32);  					// 取字模数据 
		OLED_Set_Pos(x,y);	
		for(i=0;i<CHINESECHARSIZE;i++)
		OLED_WR_Byte(buffer[i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<CHINESECHARSIZE;i++)
		OLED_WR_Byte(buffer[i+CHINESECHARSIZE],OLED_DATA);
}

// * 函数名：LCD_Str_CH diao
// * 描述  ：在指定坐标处显示16*16大小汉字字符串
// * 输入  : 	- x: 显示位置横向坐标	 
// *         	       - y: 显示位置纵向坐标 
// *			     - str: 显示的中文字符串
// * 举例  ：	
void LCD_Str_CHinese(u8 x,u8 y,vu8 *str)  
{   
    while(*str != '\0')
    {
					LCD_Char_CH(x,y,str);      
					str += 2 ;
					x += 16 ;	
    }
}
//-----------------------------------------------------------------------------

//初始化SSD1306					    
void OLED_Init(void)
{ 	 	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);     // 辅助功能IO时钟开启
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PD端口时钟

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_1;	 
 	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	                 //初始化GPIOD3,6
 	GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_1);
 
    OLED_RST_Set();
	delay_ms(100);
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 

    /*??????????????????? 此时oled是在上面模式�???????????????????????????????????????????????????????*/
	OLED_WR_Byte(0xAE,OLED_CMD);                             //--turn off oled panel

	OLED_WR_Byte(0x02,OLED_CMD);                             //---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);                             //---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);                             //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)

	OLED_WR_Byte(0x81,OLED_CMD);                             //--set contrast control register。        对比度,和背光相关
	OLED_WR_Byte(0xCF,OLED_CMD);                             // Set SEG Output Current Brightness，设置对比度的置�

	OLED_WR_Byte(0xA1,OLED_CMD);                             //--Set SEG/Column Mapping     ( 0xa0左右反置; 0xa1正常)
	                                                         // 会改变后续输入的数据，对GDDRAM中已经存在的没影响
	OLED_WR_Byte(0xC8,OLED_CMD);                             //Set COM/Row Scan Direction    ( 0xc0上下反置 ; 0xc8正常)

	OLED_WR_Byte(0xA6,OLED_CMD);                             //--set normal display

	OLED_WR_Byte(0xA8,OLED_CMD);                             //--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);                             //--1/64 duty

	OLED_WR_Byte(0x81,OLED_CMD);                             //对比度设置  // 怎么又设置?????
	OLED_WR_Byte(0xfF,OLED_CMD);                             //1~255;  默认0X7F (亮度设置,越大越亮)

	OLED_WR_Byte(0xD3,OLED_CMD);                             //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);                             //-not offset

	OLED_WR_Byte(0xd5,OLED_CMD);                             //--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);                             //--set divide ratio, Set Clock as 100 Frames/Sec

	OLED_WR_Byte(0xD9,OLED_CMD);                             //--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);                             //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

	OLED_WR_Byte(0xDA,OLED_CMD);                             //--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);

	OLED_WR_Byte(0xDB,OLED_CMD);                             //--set vcomh regulator output
	OLED_WR_Byte(0x40,OLED_CMD);                             //Set VCOM Deselect Level

	OLED_WR_Byte(0x20,OLED_CMD);                             //-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);

	OLED_WR_Byte(0x8D,OLED_CMD);                             //--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);                             //--set(0x10) disable                         // 0x14: Enable Charge Pump

	OLED_WR_Byte(0xA4,OLED_CMD);                             // Disable Entire Display On (0xa4/0xa5)

	OLED_WR_Byte(0xA6,OLED_CMD);                             // Disable Inverse Display On (0xa6/a7) 

	OLED_WR_Byte(0xAF,OLED_CMD);                             //--turn on oled panel
	
	OLED_WR_Byte(0xAF,OLED_CMD);                             /*display ON*/   // 怎么又写一次�????

	OLED_Clear();

	OLED_Set_Pos(0,0); 	
}  


#ifdef FUNC_OLED_QXY

void OLED_Show_logo_str(u8 version) 
{
	struct characters_prop dots_info;

	switch(version) {
	  case LOGO_V1:
				dots_info.index[0] = 1;
				dots_info.index[1] = 2;
				dots_info.index[2] = 3;
				dots_info.index[3] = '\0';

				dots_info.x        = 20;
				dots_info.y        =  1;
				dots_info.xgap     =  6;
				dots_info.size     = WIDTH_24x24;

				OLED_show_case_word(&dots_info);


				dots_info.index[0] = 1;
				dots_info.index[1] = 2;
				dots_info.index[2] = 3;
				dots_info.index[3] = 4;
				dots_info.index[4] = 5;
				dots_info.index[5] = '\0';

				dots_info.x        = 20;
				dots_info.y        =  5;
				dots_info.xgap     =  6;
				dots_info.size     = WIDTH_12x16;

				OLED_show_case_word(&dots_info);

	  case LOGO_V2:
	  default:  break;
	}

}
#endif

