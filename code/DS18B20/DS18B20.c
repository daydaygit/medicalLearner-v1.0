#include "DS18B20.h"
#include "delay.h"

uint16_t  DS18B20;


const unsigned char  CrcTable [256]={
0,  94, 188,  226,  97,  63,  221,  131,  194,  156,  126,  32,  163,  253,  31,  65,
157,  195,  33,  127,  252,  162,  64,  30,  95,  1,  227,  189,  62,  96,  130,  220,
35,  125,  159,  193,  66,  28,  254,  160,  225,  191,  93,  3,  128,  222,  60,  98,
190,  224,  2,  92,  223,  129,  99,  61,  124,  34,  192,  158,  29,  67,  161,  255,
70,  24,  250,  164,  39,  121,  155,  197,  132,  218,  56,  102,  229,  187,  89,  7,
219,  133, 103,  57,  186,  228,  6,  88,  25,  71,  165,  251,  120,  38,  196,  154,
101,  59, 217,  135,  4,  90,  184,  230,  167,  249,  27,  69,  198,  152,  122,  36,
248,  166, 68,  26,  153,  199,  37,  123,  58,  100,  134,  216,  91,  5,  231,  185,
140,  210, 48,  110,  237,  179,  81,  15,  78,  16,  242,  172,  47,  113,  147,  205,
17,  79,  173,  243,  112,  46,  204,  146,  211,  141,  111,  49,  178,  236,  14,  80,
175,  241, 19,  77,  206,  144,  114,  44,  109,  51,  209,  143,  12,  82,  176,  238,
50,  108,  142,  208,  83,  13,  239,  177,  240,  174,  76,  18,  145,  207,  45,  115,
202,  148, 118,  40,  171,  245,  23,  73,  8,  86,  180,  234,  105,  55,  213, 139,
87,  9,  235,  181,  54,  104,  138,  212,  149,  203,  41,  119,  244,  170,  72,  22,
233,  183,  85,  11,  136,  214,  52,  106,  43,  117,  151,  201,  74,  20,  246,  168,
116,  42,  200,  150,  21,  75,  169,  247,  182,  232,  10,  84,  215,  137,  107,  53}; 

unsigned char  temp_buff[9]; //�洢��ȡ���ֽڣ�read scratchpadΪ9�ֽڣ�read rom IDΪ8�ֽ�
unsigned char  id_buff[8];

unsigned char  crc_data;


/************************************************************
*����:������DS18B20 ͨ�ŵ�MCU��IO
*����:
*����ֵ:
*�޸�:
*************************************************************/
char ConfiureIoFor18b20(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	DS18B20 = GPIO_Pin;
	/* ����DS18b20 ͨ�� io */	
	GPIO_InitStructure.GPIO_Pin   = DS18B20;             //��DS18B20��Ϊ16��
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;    //��©���	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //��������Ƶ��Ϊ50HZ
	GPIO_Init(GPIOA, &GPIO_InitStructure);               //��ʼ��GPIOA�˿�
	GPIO_Write(GPIOA,0xffff);                            //��GPIOA 16���˿�ȫ����Ϊ�ߵ�ƽ
}

/************************************************************
*����:18B20��ʼ��
*����:
*����ֵ:
*�޸�:
*************************************************************/
char Init18b20 (void)
{
	char ret = 0;
	
	
	GPIO_SetBits(GPIOA , DS18B20); //�������øߵ�ƽ
	delay_us(2); //��ʱ2΢��
	GPIO_ResetBits(GPIOA , DS18B20);	//�õ�
	delay_us(490);   //delay 530 uS//80
    GPIO_SetBits(GPIOA , DS18B20);	 //�ø�
	delay_us(100);   //delay 100 uS//14

	if(GPIO_ReadInputDataBit(GPIOA , DS18B20)== 0)
		ret = 1;   //detect 1820 success!
	
	delay_us(480);        //��ʱ480΢��
	GPIO_SetBits(GPIOA , DS18B20);  //�������øߵ�ƽ
	
	return ret;
}
/************************************************************
*Function:��18B20д��һ���ֽ�
*parameter:
*Return:
*Modify:
*************************************************************/
void WriteByte (unsigned char  wr)  //���ֽ�д��
{
	unsigned char  i;
	for (i=0;i<8;i++)
	{
		GPIO_ResetBits(GPIOA , DS18B20);	 //D18B20 = 0;
    	delay_us(2);
		//D18B20=wr&0x01;
		if(wr&0x01)	GPIO_SetBits(GPIOA , DS18B20);
		else  GPIO_ResetBits(GPIOA , DS18B20);
		delay_us(45);   //delay 45 uS //5
		
		GPIO_SetBits(GPIOA , DS18B20);  //D18B20=1;
		wr >>= 1;
	}
}

/************************************************************
*Function:��18B20��һ���ֽ�
*parameter:
*Return:
*Modify:
*************************************************************/
unsigned char ReadByte (void)     //��ȡ���ֽ�
{
	unsigned char  i,u=0;
	for(i=0;i<8;i++)
	{		
		GPIO_ResetBits(GPIOA , DS18B20);	   	//D18B20 = 0;
		delay_us (2);
		u >>= 1;
	
		GPIO_SetBits(GPIOA , DS18B20);	 	//D18B20 = 1;
		delay_us (4);
		if(GPIO_ReadInputDataBit(GPIOA , DS18B20) == 1)
		u |= 0x80;
		delay_us (65);
	}
	return(u);
}

/************************************************************
*Function:��18B20
*parameter:
*Return:
*Modify:
*************************************************************/
unsigned char ReadBytes(u8 *buf,u8 len)
{
	 unsigned char  i;
	 for(i=0;i<len;i++)
	 {
		  buf[i] = ReadByte();
		 
	 }
}

/************************************************************
*Function:CRCУ��
*parameter:
*Return:
*Modify:
*************************************************************/
unsigned char Temp_CRC (unsigned char j)
{
   	unsigned char  i,crc_data=0;
  	for(i=0;i<j;i++)  //���У��
    	crc_data = CrcTable[crc_data^temp_buff[i]];
    return (crc_data);
}



/************************************************************
*Function:�ڲ�����
*parameter:
*Return:
*Modify:
*************************************************************/
void Config18b20 (void)  //�������ñ����޶�ֵ�ͷֱ���
{
     Init18b20();
     WriteByte(0xcc);  //skip rom
     WriteByte(0x4e);  //write scratchpad
     WriteByte(0x19);  //����
     WriteByte(0x1a);  //����
     WriteByte(0x7f);     //set 12 bit (0.125)
     Init18b20();
     WriteByte(0xcc);  //skip rom
     WriteByte(0x48);  //�����趨ֵ
     Init18b20();
     WriteByte(0xcc);  //skip rom
     WriteByte(0xb8);  //�ص��趨ֵ
}

/************************************************************
*Function:��18B20ID
*parameter:
*Return:
*Modify:
*************************************************************/
void ReadID (void)//��ȡ���� id
{
	Init18b20();
	WriteByte(0x33);  //read rom
	ReadBytes(id_buff,8);
}

/************************************************************
*Function:18B20IDȫ����
*parameter:
*Return:
*Modify:
*************************************************************/
int ReadTemperature(void)
{
  	int temp = 0;
  	ReadID();
  	Config18b20();
	Init18b20 ();
	WriteByte(0xcc);   //skip rom
	WriteByte(0x44);   //Temperature convert

	Init18b20 ();
	WriteByte(0xcc);   //skip rom
	WriteByte(0xbe);   //read Temperature
	ReadBytes (temp_buff,9);
   if (Temp_CRC(9)==0) //У����ȷ
   {
		
	    temp = (temp_buff[1]&0x07)*0x100 + temp_buff[0];
		  //temp /= 16;
		 
		 if((temp_buff[1]&0xf8) == 0xf8 )
		 {
			 temp *= -1;
		 }
		delay_us(10);
    }
	 
		return temp;
}


