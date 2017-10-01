
//  ��������   : OLED 4�ӿ���ʾ����(51ϵ��)
//              GND    ��Դ��
//              VCC  ��5V��3.3v��Դ
//              D0   ��PA7     ����   PB5
//              D1   ��PA6     ����   PB4
//              RES  ��PA5     ����   PB3
//              DC   ��PA4     ����   PB1
//              CS   ��PA3     ����   PB0
//OLED���Դ�
//��Ÿ�ʽ����.
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
//��SSD1106д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd)
	  OLED_DC_Set();
	else 
	  OLED_DC_Clr();		  
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
} 

void OLED_Set_Pos(u8 x, u8 y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
} 

//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x02,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //������ʾ
}

//��ָ��λ����ʾһ���ַ� ����OLED_ShowChar(48,6,16,t);
//x0,y0��ʾ�������,x0(��Χ0��127),y0(��Χ0��7)*8 
//size:ѡ������ 16/12 
//chr:�ַ���ascii��
void OLED_ShowChar(u8 x, u8 y, u8 size, u8 chr)
{      	
	u8 c=0,i=0;	
	c=chr-' ';//�õ�ƫ�ƺ��ֵ			
	if(x>X_WIDTH-1){x=0;y=y+2;}
	if(size ==ASCII32x32)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<ASCII32x32/4;i++)
		OLED_WR_Byte(ASCII16x32[c*ASCII32x32+i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<ASCII32x32/4;i++)
		OLED_WR_Byte(ASCII16x32[c*ASCII32x32+i+ASCII32x32/4],OLED_DATA);//������ʾ��
		OLED_Set_Pos(x,y+2);
		for(i=0;i<ASCII32x32/4;i++)
		OLED_WR_Byte(ASCII16x32[c*ASCII32x32+i+2*ASCII32x32/4],OLED_DATA);
		OLED_Set_Pos(x,y+3);
		for(i=0;i<ASCII32x32/4;i++)
		OLED_WR_Byte(ASCII16x32[c*ASCII32x32+i+3*ASCII32x32/4],OLED_DATA);
	}	
	else if(size ==ASCII24x24)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<ASCII24x24/3;i++)
		OLED_WR_Byte(ASCII12x24[c*ASCII24x24+i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<ASCII24x24/3;i++)
		OLED_WR_Byte(ASCII12x24[c*ASCII24x24+i+ASCII24x24/3],OLED_DATA);//������ʾ��
		OLED_Set_Pos(x,y+2);
		for(i=0;i<ASCII24x24/3;i++)
		OLED_WR_Byte(ASCII12x24[c*ASCII24x24+i+2*ASCII24x24/3],OLED_DATA);
	}		
	else if(size ==ASCII16x16)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<ASCII16x16/2;i++)
		OLED_WR_Byte(ASCII8X16[c*ASCII16x16+i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<ASCII16x16/2;i++)
		OLED_WR_Byte(ASCII8X16[c*ASCII16x16+i+ASCII16x16/2],OLED_DATA);//������ʾ��
	}
	else if(size ==ASCII12x8)
	{	
		OLED_Set_Pos(x,y);
		for(i=0;i<ASCII12x8;i++)
		OLED_WR_Byte(ASCII6x8[c*ASCII12x8+i],OLED_DATA);	//һ����ʾ��			
	}
}

//m^n����
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	

//��ʾ���� ����OLED_ShowNum(103,6,t,3,16); 		  
//x0,y0��ʾ�������,x0(��Χ0��127),y0(��Χ0��7)*8 
//len :���ֵ�λ��
//size:ѡ������ 16/12 
//num:��ֵ(0~65536);	
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

//��ʾһ���ַ��Ŵ� ����OLED_ShowString(30,0,"OLED TEST");
//x0,y0:��ʾ�������,x0(��Χ0��127),y0(��Χ0��7)*8
//size:ѡ������ 16/12 
//char:�ַ���ascii��
void OLED_ShowString(u8 x,u8 y,u8 size,char *chr)
{
	u8 j=0;
	while (chr[j]!='\0')
	{		
			OLED_ShowChar(x,y,size,chr[j]);
			if(size==ASCII12x8)
			{
				x+=6;//����asciiƫ����
			}
			else if(size==ASCII16x16)
			{
				x+=ASCII16x16/2;//����asciiƫ����
			}
			else if(size==ASCII24x24)
			{
				x+=ASCII24x24/3;//����asciiƫ����
			}
			else if(size==ASCII32x32)
			{
				x+=ASCII32x32/4;//����asciiƫ����
			}
 			if(x>X_WIDTH-size){x=0;y+=size/8;}
 			j++;
	}
}
                                                                                 
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
//��ʾ��ʾBMPͼƬ ����OLED_DrawBMP(0,0,128,8,BMP1);���ͼƬ
//x0,y0:��ʾ�������,x0(��Χ0��127),y0(��Χ0��7)*8
//x1,y1:��ʾͼƬ���ȺͿ��,x1(��Χ0��127),y1(��Χ0��7)*8
//BMP[]:ͼƬ�׵�ַ
void OLED_DrawBMP(u8 x0, u8 y0,u8 x1, u8 y1,const u8 BMP[])
{ 	
	 u16 j=0;
	 u8 x,y;
  
  //if(y1%8==0) y1=y1/8;      
  //else y1=y1/8+1;
	
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x0+x1;x++)
		{      
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
	//��ѡ��
	
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	//OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 	
	OLED_WR_Byte((x&0x0f),OLED_CMD);
	OLED_GRAM[x-2][y]=0;	
	OLED_WR_Byte(OLED_GRAM[x-2][y],OLED_DATA);  	
	}		
}	
void OLED_DrawPoint(u8 x ,u8 y) //������������⣬�ο�������ٸ�
{ 
	u8 data=0x01;
	u8 y1;
	//u8 y2=0x01;
	
	//��ѡ��
	//y/=8;
	OLED_WR_Byte(0xb0+y/8,OLED_CMD);
	//��ѡ��
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
	
	//��y�ᷴ��
	if(yy>63)
		yy=63;
	y=63-yy;
	
	//u8 y2=0x01;
	
	//��ѡ��
	
	

	   
	y2=y/8;
	OLED_WR_Byte(0xb0+y2,OLED_CMD);
	
	//��ѡ��
	x+=2;
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	//OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 	
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
		
	y1=y%8;
	data<<=y1;
	OLED_GRAM[x-2][y2] |=data;
	OLED_WR_Byte(OLED_GRAM[x-2][y2],OLED_DATA);  

	
	
} 
//-----------------------------------��ʾ����------------------
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
	for(i=0;i<leng/4;i++)  //32Ϊ��Ƭ��4���ֽ�
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
		chinese_ascii = chinese_ascii<<8|*str; 		//�ҵ���������	
		p_address=getchinesecodepos(chinese_ascii);										
		Get_Code(buffer,p_address,32);  					// ȡ��ģ���� 
		OLED_Set_Pos(x,y);	
		for(i=0;i<CHINESECHARSIZE;i++)
		OLED_WR_Byte(buffer[i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<CHINESECHARSIZE;i++)
		OLED_WR_Byte(buffer[i+CHINESECHARSIZE],OLED_DATA);
}

// * ��������LCD_Str_CH diao
// * ����  ����ָ�����괦��ʾ16*16��С�����ַ���
// * ����  : 	- x: ��ʾλ�ú�������	 
// *         	- y: ��ʾλ���������� 
// *			- str: ��ʾ�������ַ���
// * ����  ��	
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

//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	 	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PD�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_1;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //��ʼ��GPIOD3,6
 	GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_1);
 
  OLED_RST_Set();
	delay_ms(100);
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 
					  
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x02,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xfF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
	OLED_Clear();
	OLED_Set_Pos(0,0); 	
}  



