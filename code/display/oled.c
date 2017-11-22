
//  ¹¦ÄÜÃèÊö   : OLED 4½Ó¿ÚÑÝÊ¾Àý³Ì(51ÏµÁÐ)
//              GND    µçÔ´µØ
//              VCC  ½Ó5V»ò3.3vµçÔ´
//              D0   ½ÓPA7     »»³É   PB5
//              D1   ½ÓPA6     »»³É   PB4
//              RES  ½ÓPA5     »»³É   PB3
//              DC   ½ÓPA4     »»³É   PB1
//              CS   ½ÓPA3     »»³É   PB0
//OLEDµÄÏÔ´æ
//´æ·Å¸ñÊ½ÈçÏÂ.
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

//ÏòSSD1106Ð´ÈëÒ»¸ö×Ö½Ú¡£
//dat:ÒªÐ´ÈëµÄÊý¾Ý/ÃüÁî
//cmd:Êý¾Ý/ÃüÁî±êÖ¾ 
//      0: ÃüÁî;
//      1: Êý¾Ý;
void OLED_WR_Byte(u8 dat,u8 cmd)          // 4-wire SPI write
{	
	u8 i;			  

	if(cmd)   {   OLED_DC_Set();   }            // Êý¾Ý,ÔòB1_DC = H
	else      {   OLED_DC_Clr();   }            // ÃüÁî,ÔòB1_DC = L

	OLED_CS_Clr();                              // B0_CS = L                        //ÏÈÉèÖÃDC£¬ÔÙÉèÖÃCS

	for(i=0;i<8;i++)
	{  
		OLED_SCLK_Clr();                        // B5_D0 = L   //D0: SCLK

		if(dat&0x80)  {   OLED_SDIN_Set();   }  // B4_D1 = H   //D1: SDin
		else          {   OLED_SDIN_Clr();   }  // B4_D1 = L

		OLED_SCLK_Set();                        // B5_D0 = H

		dat<<=1;   
	}

	OLED_CS_Set();                              // B0_CS  = H                      //ÏÈÉèÖÃCSCS£¬ÔÙÉèÖÃDC
	OLED_DC_Set();                              // B1_DC  = H	  
} 

void OLED_Set_Pos(u8 x, u8 y) 
{ 
	OLED_WR_Byte(0xb0+y,              OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10, OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,       OLED_CMD); 
}

//¿ªÆôOLEDÏÔÊ¾    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDCÃüÁî
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//¹Ø±ÕOLEDÏÔÊ¾     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDCÃüÁî
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

//ÇåÆÁº¯Êý,ÇåÍêÆÁ,Õû¸öÆÁÄ»ÊÇºÚÉ«µÄ!ºÍÃ»µãÁÁÒ»Ñù!!!	  
void OLED_Clear(void)
{  
	u8 i,n;	

	for(i=0;i<8;i++)                        //page0~7¹²8Ò³
	{  
		OLED_WR_Byte(0xb0+i,OLED_CMD);      //ÉèÖÃÒ³µØÖ·£¨0~7£©
		OLED_WR_Byte(0x02,  OLED_CMD);      //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁÐµÍµØÖ·
		OLED_WR_Byte(0x10,  OLED_CMD);      //ÉèÖÃÏÔÊ¾Î»ÖÃ¡ªÁÐ¸ßµØÖ· 

		for(n=0; n<128; n++)
			OLED_WR_Byte(0,OLED_DATA); 
	}
}

//ÔÚÖ¸¶¨Î»ÖÃÏÔÊ¾Ò»¸ö×Ö·û µ÷ÓÃOLED_ShowChar(48,6,16,t);
//x0,y0: ÏÔÊ¾Æðµã×ø±ê,x0·¶Î§(0¡«127), y0·¶Î§(0¡«7) * 8 
//size:  Ñ¡Ôñ×ÖÌå 16/12 
//chr:   ×Ö·û´®asciiÂë
void OLED_ShowChar(u8 x, u8 y, u8 size, u8 chr)  // x-y: 128-64    //48, 0
{      	
	u8 c=0,i=0;	
	c=chr-' ';                      //µÃµ½Æ«ÒÆºóµÄÖµ.  ÆäÖÐ" "(space)ÊÇASCIIµÚ32¸ö×Ö·û
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
		   OLED_WR_Byte(ASCII16x32[c*ASCII32x32+i+ASCII32x32/4],OLED_DATA);  //ËÄÁÐÏÔÊ¾Íê

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
		   OLED_WR_Byte(ASCII12x24[c*ASCII24x24+i+ASCII24x24/3],OLED_DATA);//ÈýÁÐÏÔÊ¾Íê

		OLED_Set_Pos(x,y+2);
		for(i=0;i<ASCII24x24/3;i++)
		   OLED_WR_Byte(ASCII12x24[c*ASCII24x24+i+2*ASCII24x24/3],OLED_DATA);

	} else if(size ==ASCII16x16) {                                        // ×ßµÄÊÇ8x16µÄ×Ö¿â
		OLED_Set_Pos(x,y);	
		for(i=0;i<ASCII16x16/2;i++)   // 16 / 2 = 8     //2*8*8 ¶þÁÐ
		   OLED_WR_Byte(ASCII8X16[c*ASCII16x16+i],OLED_DATA);

		OLED_Set_Pos(x,y+1);
		for(i=0;i<ASCII16x16/2;i++)
		   OLED_WR_Byte(ASCII8X16[c*ASCII16x16+i+ASCII16x16/2],OLED_DATA); //Á½ÁÐÏÔÊ¾Íê

	} else if(size ==ASCII12x8) {	
		OLED_Set_Pos(x,y);
		for(i=0;i<ASCII12x8;i++)
		   OLED_WR_Byte(ASCII6x8[c*ASCII12x8+i],OLED_DATA);	               //Ò»ÁÐÏÔÊ¾Íê
	}
}

void OLED_ShowWord(u8 x, u8 y, u8 size, u32 index)  // x-y: 128-64    //48, 0  .    charÒÑ¾­»ñÈ¡µ½¸Ã×ÖÔÚGB_WORD_24x24[]µÄÏÂ±ê
{
	u8 i=0, j=0;	

	if(x > X_WIDTH - 1)				{  x=0;  y=y+2;  }

	/* step:
	 *  1. ÉèÖÃx,y×ø±ê
	 *  2. Ð´Ò»¸ö×Ö½ÚÊý¾Ý(Ê¹ÓÃOLED_WR_Byte) , Èç´ËÑ­»·24´Î¾Í¿ÉÒÔÐ´ÍêÒ»ÐÐ
	 *  3. ÖØÐÂÉèÖÃx,y×ø±ê
	 *  4. ¼ÌÐøÐ´Ò»¸ö×Ö½ÚÊý¾Ý, Èç´ËÑ­»·24´ÎÐ´ÍêµÚ¶þÐÐ
	 *  5. ÖØÐÂÉèÖÃx,y×ø±ê
	 *  6. ¼ÌÐøÐ´Ò»¸ö×Ö½ÚÊý¾Ý, Èç´ËÑ­»·24´ÎÐ´ÍêµÚ¶þÐÐ
	 */

	switch(size) {
	  case WIDTH_24x24:
						for(j=0; j< (WIDTH_24x24 / 8); j++) {   // ×ÝÏò24¸öµã£¬Ã¿8¸öµãÊÇÒ»¸ö×Ö½Ú
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


//m^nº¯Êý
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	

//ÏÔÊ¾Êý×Ö µ÷ÓÃOLED_ShowNum(103,6,t,3,16); 		  
//x0,y0ÏÔÊ¾Æðµã×ø±ê,x0(·¶Î§0¡«127),y0(·¶Î§0¡«7)*8 
//len :Êý×ÖµÄÎ»Êý
//size:Ñ¡Ôñ×ÖÌå 16/12 
//num:ÊýÖµ(0~65536);	
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

//ÏÔÊ¾Ò»¸ö×Ö·ûºÅ´® µ÷ÓÃOLED_ShowString(30,0,"OLED TEST");
//x0,y0: ÏÔÊ¾Æðµã×ø±ê,x0(·¶Î§0¡«127), y0(·¶Î§0¡«7)*8
//size:   Ñ¡Ôñ×ÖÌå 16/12 
//char:  ×Ö·û´®asciiÂë
void OLED_ShowString(u8 x,u8 y,u8 size,char *chr)
{
	u8 j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x, y, size, chr[j]);
		if(size==ASCII12x8)        {  x+=6;             }         //µ¥¸öasciiÆ«ÒÆÁ¿
		else if(size==ASCII16x16)  {  x+=ASCII16x16/2;  }         //µ¥¸öasciiÆ«ÒÆÁ¿
		else if(size==ASCII24x24)  {  x+=ASCII24x24/3;  }         //µ¥¸öasciiÆ«ÒÆÁ¿
		else if(size==ASCII32x32)  {  x+=ASCII32x32/4;  }         //µ¥¸öasciiÆ«ÒÆÁ¿

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
						if((x+ELL_5x8 > X_WIDTH) || (y+8 > Y_WIDTH)) {	// Ò»¸öµã¶¼²»ÄÜ´ò,Á¢Âí·µ»Ø1
							return 1;
						}

						for(i=0; i<3; i++) {							// Ô¤¶¨´òÓ¡3¸ö'.'£¬²»ÄÜ´ò¹»3¸öÔòÍË³ö,µ«¿Ï¶¨ÓÐ´òÓ¡,·µ»Ø0
							x1 = x+ELL_5x8*i;
							y1 = y;

							if((x1+ELL_5x8 > X_WIDTH) || (y1+8 > Y_WIDTH)) {	  // ÔÊÐí²»´òÓ¡Íê3¸öµã,µ«µ±Ç°µã·Å²»ÏÂ£¬¾ÍÌø³ö
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
  *null,0ºÍ'\0'ÔÚasciiÖÐÓÃÊý×Ö¶ÁÈ¡Ê±ÖµÊÇ0£¬ÓÃ×Ö·û´®¶ÁÈ¡Ê±¾ÍÊÇ'\0'(½áÊø·û)»òÕßnull(Óë±àÒëÆ÷ÓÐ¹Ø)
  * ¶ø×Ö·û'\0'ÔÚasciiÖÐ,ÓÃÊý×Ö¶ÁÈ¡Ê±ÖµÊÇ48
  * Ps£ºÔÚÊý×éºÍ×Ö·û´®ÖÐ£¬0ºÍ'\0'ÊÇµÈ¼ÛµÄ,¶¼ÄÜ±íÊ¾½áÊø·û¡£
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

							//×¢Òâ: ×ÝÏò¸ß¶È»¹ÊÇ16
							for(j=0; j<WIDTH_16x16/8; j++) {		// Ã¿¸ö24x24×ÖÌåµÄ×ÖÑ­»·´òµã3´Î(»»Ò»ÐÐÖØÀ´¹²3´Î)
								OLED_Set_Pos(x1,y1+j);				// Ã¿ÖØÀ´Ò»´Î,y+1

								for(k=0; k<WIDTH_12x16; k++) {		// Ã¿¸ö24x24×ÖÌåµÄ×ÖºáÏò´òµã24 ¸ö×Ö½Ú
																	// POS»á×Ô¼Ó1 
									OLED_WR_Byte(GB_WORD_12x16[(dots_info->index[i]* WIDTH_12x16 * 2) + (j * WIDTH_12x16) + k], OLED_DATA);
								}
							}
						}

						break;

	  case WIDTH_16x16:
						for(i=0; i<stringLen(dots_info->index); i++) {
							x1 = dots_info->x + WIDTH_16x16 * i + dots_info->xgap * i;
							y1 = dots_info->y;

							for(j=0; j<WIDTH_16x16/8; j++) {	  	// Ã¿¸ö24x24×ÖÌåµÄ×ÖÑ­»·´òµã3´Î(»»Ò»ÐÐÖØÀ´¹²3´Î)
								OLED_Set_Pos(x1,y1+j);		  	// Ã¿ÖØÀ´Ò»´Î,y+1

								for(k=0; k<WIDTH_16x16; k++) {   // Ã¿¸ö24x24×ÖÌåµÄ×ÖºáÏò´òµã24 ¸ö×Ö½Ú
																// POS»á×Ô¼Ó1 
									OLED_WR_Byte(GB_WORD_16x16[(dots_info->index[i]* WIDTH_16x16 * 2) + (j * WIDTH_16x16) + k], OLED_DATA);
								}
							}
						}

						break;

	  case WIDTH_24x24:
	  					//for(i=0; dbg_arr[i] != '\0'; i++) {  //ÓÉÓÚµÚÒ»¸öindex¾ÍÊÇ0£¬¶ø'\0'Ò²ÊÇ0£¬ËùÒÔÒ»¿ªÊ¼µ÷ÊÔÊ±ÊÇÊ²Ã´¶¼Ã»ÓÐ
						for(i=0; i<stringLen(dots_info->index); i++) {
							x1 = dots_info->x + WIDTH_24x24 * i + dots_info->xgap * i;
							y1 = dots_info->y;

							for(j=0; j<WIDTH_24x24/8; j++) {	// Ã¿¸ö24x24×ÖÌåµÄ×ÖÑ­»·´òµã3´Î(»»Ò»ÐÐÖØÀ´¹²3´Î)
								OLED_Set_Pos(x1,y1+j);			// Ã¿ÖØÀ´Ò»´Î,y+1

								for(k=0; k<WIDTH_24x24; k++) {	// Ã¿¸ö24x24×ÖÌåµÄ×ÖºáÏò´òµã24 ¸ö×Ö½Ú
																// POS»á×Ô¼Ó1 
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
//ÏÔÊ¾ÏÔÊ¾BMPÍ¼Æ¬ µ÷ÓÃOLED_DrawBMP(0,0,128,8,BMP1);×î´óÍ¼Æ¬
//x0,y0:ÏÔÊ¾Æðµã×ø±ê,x0(·¶Î§0¡«127),y0(·¶Î§0¡«7)*8
//x1,y1:ÏÔÊ¾Í¼Æ¬³¤¶ÈºÍ¿í¶È,x1(·¶Î§0¡«127),y1(·¶Î§0¡«7)*8
//BMP[]:Í¼Æ¬Ê×µØÖ·
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
	//ÁÐÑ¡Ôñ
	
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	//OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 	
	OLED_WR_Byte((x&0x0f),OLED_CMD);
	OLED_GRAM[x-2][y]=0;	
	OLED_WR_Byte(OLED_GRAM[x-2][y],OLED_DATA);  	
	}		
}	
void OLED_DrawPoint(u8 x ,u8 y) //Õâ¸öº¯ÊýÓÐÎÊÌâ£¬²Î¿¼ÏÂÃæµÄÔÙ¸Ä
{ 
	u8 data=0x01;
	u8 y1;
	//u8 y2=0x01;
	
	//ÐÐÑ¡Ôñ
	//y/=8;
	OLED_WR_Byte(0xb0+y/8,OLED_CMD);
	//ÁÐÑ¡Ôñ
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
	
	//½«yÖá·´Ïò
	if(yy>63)
		yy=63;
	y=63-yy;
	
	//u8 y2=0x01;
	
	//ÐÐÑ¡Ôñ	
	   
	y2=y/8;
	OLED_WR_Byte(0xb0+y2,OLED_CMD);
	
	//ÁÐÑ¡Ôñ
	x+=2;
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	//OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 	
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
		
	y1=y%8;
	data<<=y1;
	OLED_GRAM[x-2][y2] |=data;
	OLED_WR_Byte(OLED_GRAM[x-2][y2],OLED_DATA);  

	
	
} 
//-----------------------------------ÏÔÊ¾ÖÐÎÄ------------------
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
	for(i=0;i<leng/4;i++)  //32Îªµ¥Æ¬»ú4¸ö×Ö½Ú
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
		chinese_ascii = chinese_ascii<<8|*str; 		//ÕÒµ½ºº×ÖÄÚÂë	
		p_address=getchinesecodepos(chinese_ascii);										
		Get_Code(buffer,p_address,32);  					// È¡×ÖÄ£Êý¾Ý 
		OLED_Set_Pos(x,y);	
		for(i=0;i<CHINESECHARSIZE;i++)
		OLED_WR_Byte(buffer[i],OLED_DATA);
		OLED_Set_Pos(x,y+1);
		for(i=0;i<CHINESECHARSIZE;i++)
		OLED_WR_Byte(buffer[i+CHINESECHARSIZE],OLED_DATA);
}

// * º¯ÊýÃû£ºLCD_Str_CH diao
// * ÃèÊö  £ºÔÚÖ¸¶¨×ø±ê´¦ÏÔÊ¾16*16´óÐ¡ºº×Ö×Ö·û´®
// * ÊäÈë  : 	- x: ÏÔÊ¾Î»ÖÃºáÏò×ø±ê	 
// *         	       - y: ÏÔÊ¾Î»ÖÃ×ÝÏò×ø±ê 
// *			     - str: ÏÔÊ¾µÄÖÐÎÄ×Ö·û´®
// * ¾ÙÀý  £º	
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

//³õÊ¼»¯SSD1306					    
void OLED_Init(void)
{ 	 	 
 	GPIO_InitTypeDef  GPIO_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);     // ¸¨Öú¹¦ÄÜIOÊ±ÖÓ¿ªÆô
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  
 	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //Ê¹ÄÜPD¶Ë¿ÚÊ±ÖÓ

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_1;	 
 	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;		 //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;        //ËÙ¶È50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	                 //³õÊ¼»¯GPIOD3,6
 	GPIO_SetBits(GPIOB,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_0|GPIO_Pin_1);
 
    OLED_RST_Set();
	delay_ms(100);
	OLED_RST_Clr();
	delay_ms(100);
	OLED_RST_Set(); 

    /*??????????????????? ´ËÊ±oledÊÇÔÚÉÏÃæÄ£Ê½Ï???????????????????????????????????????????????????????*/
	OLED_WR_Byte(0xAE,OLED_CMD);                             //--turn off oled panel

	OLED_WR_Byte(0x02,OLED_CMD);                             //---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);                             //---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);                             //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)

	OLED_WR_Byte(0x81,OLED_CMD);                             //--set contrast control register¡£        ¶Ô±È¶È,ºÍ±³¹âÏà¹Ø
	OLED_WR_Byte(0xCF,OLED_CMD);                             // Set SEG Output Current Brightness£¬ÉèÖÃ¶Ô±È¶ÈµÄÖÃå

	OLED_WR_Byte(0xA1,OLED_CMD);                             //--Set SEG/Column Mapping     ( 0xa0×óÓÒ·´ÖÃ; 0xa1Õý³£)
	                                                         // »á¸Ä±äºóÐøÊäÈëµÄÊý¾Ý£¬¶ÔGDDRAMÖÐÒÑ¾­´æÔÚµÄÃ»Ó°Ïì
	OLED_WR_Byte(0xC8,OLED_CMD);                             //Set COM/Row Scan Direction    ( 0xc0ÉÏÏÂ·´ÖÃ ; 0xc8Õý³£)

	OLED_WR_Byte(0xA6,OLED_CMD);                             //--set normal display

	OLED_WR_Byte(0xA8,OLED_CMD);                             //--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);                             //--1/64 duty

	OLED_WR_Byte(0x81,OLED_CMD);                             //¶Ô±È¶ÈÉèÖÃ  // ÔõÃ´ÓÖÉèÖÃ?????
	OLED_WR_Byte(0xfF,OLED_CMD);                             //1~255;  Ä¬ÈÏ0X7F (ÁÁ¶ÈÉèÖÃ,Ô½´óÔ½ÁÁ)

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
	
	OLED_WR_Byte(0xAF,OLED_CMD);                             /*display ON*/   // ÔõÃ´ÓÖÐ´Ò»´Îå????

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

