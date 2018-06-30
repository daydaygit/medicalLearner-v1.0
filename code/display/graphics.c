#include <stm32f10x.h>		// u8
#include "graphics.h"

struct clk_panel_prop panel;
//panel
struct icon_center  pancenter;
u8 panel_cent_x, panel_cent_y;

//plate
struct clk_plate_prop plate;
struct dot_pos plate_dotsPos_buf[CLK_PLATE_8thARC_SIZE];


//plate-scale
struct clk_scale_prop  clkScale;
//struct dot_pos scaleBuffer[SCALE_8thPART_SIZE][SCALE_LEN] = {0};
struct dot_pos scaleBuffer[ENDPOINT_FOR_R30][SCALE_LEN] = {0};  //Ӧ��ѡ��1/8Բ����ͨ����㷨�ҵ���һ���޵�15����

//plate center dot
u8 circ_cent_buf[CIRCLE_CENT_BSZ] = {1,1,1,1,1};  /* Left,Right,Up,Down,Center */

//hand
struct clk_hands_prop hourHand;
struct clk_hands_prop minuHand;
struct clk_hands_prop secHand;


struct plate_cent_prop plateCenter;
struct clk_scale_prop  clkScale;

struct timer_digital   digTimer;


//struct dot_pos  sec_dotspos_buf[CLK_SHAND_LEN * CLK_SHAND_WID] = {0};
//struct dot_pos minu_dotspos_buf[CLK_MHAND_LEN * CLK_MHAND_WID] = {0};
//struct dot_pos hour_dotspos_buf[CLK_HHAND_LEN * CLK_HHAND_WID] = {0};
struct dot_pos  sec_dotspos_buf[26] = {0};
struct dot_pos minu_dotspos_buf[23] = {0};
struct dot_pos hour_dotspos_buf[20] = {0};

#if ENABLE_DYNAMIC_TIME
volatile unsigned int dateTime[6] = {2018, 4, 1, 0, 4, 47};
volatile char update_panel_enable = 0;

int draw_kinds_line(struct clk_panel_prop *clkPanel, enum LINE_TYPE type);
#endif

/*const */ static struct dot_pos eighth_arc_dots_r30[] = {
	{  0, 30 },
	{  1, 30 },
	{  2, 30 },
	{  3, 30 },
	{  4, 30 },
	{  5, 30 },
	{  6, 30 },
	{  7, 29 },
	{  8, 29 },
	{  9, 29 },
	{ 10, 28 },
	{ 11, 28 },
	{ 12, 28 },
	{ 13, 27 },
	{ 14, 27 },
	{ 15, 26 },
	{ 16, 26 },
	{ 17, 25 },
	{ 18, 24 },
	{ 19, 24 },
	{ 20, 23 },
	{ 21, 22 },
	{ 22, 21 },
};

/*const */ struct dot_pos endpoint_r30_on_plate[ENDPOINT_FOR_R30] = {
#if 1
	{  0, 30 },
	{  3, 30 },
	{  6, 30 },
	{  9, 29 },
	{ 12, 28 },
	{ 15, 27 },
	{ 18, 25 },   //{ 17, 24 },  // ͬʱbuf����Ҫ������
	{ 20, 23 },
	{ 23, 20 },
	{ 25, 18 },
	{ 27, 15 },
	{ 28, 12 },
	{ 29,  9 },
	{ 30,  6 },
	{ 30,  3 },
	{ 30,  0 },
#else
	{  0, 30 },
	{  3, 30 },
	{  6, 30 },
	{  9, 29 },
	{ 12, 28 },
	{ 15, 27 },
	{ 18, 25 },
	{ 21, 22 },
	{ 23, 20 },
	{ 24, 18 },
	{ 26, 15 },
	{ 28, 12 },
	{ 29,  9 },
	{ 30,  6 },
	{ 30,  3 },
	{ 30,  0 },
#endif
};

/*const */u8 panel_64x64_data[512]={
	/* default data : */
#if ENABLE_FULL_PATTERN
	0xff,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xff,

	0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,

	0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,

	0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,

	0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,

	0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,

	0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,

	0xff,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
	0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xff,
#else
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

 #endif
};

int set_panel_dot(struct clk_panel_prop *clkPanel, u8 X, u8 Y)
{
	u8 ax, y1, y2;
	u16 m;						/*�±�Ҫ�ﵽ512,������char !!!*/
	u8 ret = 0;

	/* 64x64��panel���Կ���64x64����������,Ҳ�ɿ���64*8���ֽ�����*/

	ax = X;

	y1 = (Y / 8);
	y2 = (Y % 8);					/* ������һ���ֽڵĵڼ���bit */

#if 0
	if((y1==0))     {	m = ax;				}	//��һ��,��LCD��0ҳ
	else if((y1==1)){	m = clkPanel->width *1 +ax;	}	// line 2, page1
	else if((y1==2)){	m = clkPanel->width *2 +ax;	}	// line 3, page2
	else if((y1==3)){	m = clkPanel->width *3 +ax;	}	// line 4, page3
	else if((y1==4)){	m = clkPanel->width *4 +ax;	}	// line 5, page4
	else if((y1==5)){	m = clkPanel->width *5 +ax;	}	// line 6, page5
	else if((y1==6)){	m = clkPanel->width *6 +ax;	}	// line 7, page6
	else if((y1==7)){	m = clkPanel->width *7 +ax;	}	// line 8, page7

#else
	m = clkPanel->width * y1 + ax; 					// line (y1+1), page-y1
#endif

	*(clkPanel->dots_buf + m) |= 1 << y2;

	return ret;
}


/* arc_dots_to_panel()��bresenham�㷨�õ���arc����Ͷ��panel��
  *  panel��arc������Ҫ�и���ͬ����:panel���ĺ�arcԲ����ͬһ��λ��!!!
  *  clkPlate->dots_posָ��������е�����(x,y)�������ԭ�������
  *  arc�����ݱ�����plate��,���Ǽ������ֱ��Ͷ��panel��ȥ,����ռ�ռ�ô�
  *  arc��1/8Բ��
  */
int arc_dots_to_panel(struct clk_plate_prop *clkPlate, struct clk_panel_prop *clkPanel, u8 n)
{
	u8 X,Y,x,y,ax,ay;               /* X & Y is belong to panel axes, (X<= clkPanel->width<=64<128, Y<= clkPanel->height<=64)
                                         * x & y��1/8Բ��arc��������,���ڵ�һ����ֱ��y=x�Ϸ�ת����panel�Ϻ������
                                         * ax & ay��clkPanel->dots_bufָ���(64x64)�����еĳ�Ա�±�*/
	u8 y1,y2,m,b,v;
	char ret = 0;

	if((clkPlate == NULL) || (n > clkPlate->r))
		return (char)-EINVAL;

	/*
	 * ����һ���㣬Ҫmap��panel��ȥ
	 * ��Щarc�㶼�ڵ�һ����45���ϰ벿��
	 * ���ѽ���������4�����޴��°�˳ʱ�ӷ���ֳ�8������
	 * area1 ��(x,y)ӳ�䵽panel������area�Ͽ�ͨ��MAP_AREA_x����ʵ��
	 * �����ǵ�(x,y)�ı�ʾ,�����ѵ�����α�ʾpanel��������±�?
	 * panel��Աpanel_buf�����128*64���ֽ�
	 * ������(x,y)���ת����panel_buf��(x',y')��ȥ?
	 * (1)(x,y)��(x0,y0)����ϵ�ĵ�1����,��x'=x0+x, y'=y0-y;
	 * (2)                        2����,��x'=x0-x, y'=y0-y;
	 * (3)                        3����,��x'=x0-x, y'=y0+y;
	 * (4)                        4����,��x'=x0+x, y'=y0+y;
	 * (5)                        x����������,��x'=x0+x, y'=0;
	 * (6)                        x�Ḻ������,��x'=x0-x, y'=0;
	 * (7)                        y����������,��x'=x0,    y'=y0-y;
	 * (8)                        y�Ḻ������,��x'=x0,    y'=y0+y
	 */

	/* 1/8Բ��ʵ�ʾ��ǲ��ֱ���plate���������ߵ�Բ����ͬһ��
	 *  ��Բ����������ת��panel������,ע����һ��dot������һ���ֽ�
	 *  �ٽ�dotת��һ�������Ա(�ֽ�)�е�ĳһbitλ��
	 */

	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	/* ��1�����ϰ벿�� */
	X = *(clkPanel->panCenter->cx) + x;
	Y = *(clkPanel->panCenter->cy) - y;
	ret = set_panel_dot(clkPanel, X, Y);

	/* ��2�����ϰ벿�� */
	X = *(clkPanel->panCenter->cx) - x;
	Y = *(clkPanel->panCenter->cy) - y;
	ret = set_panel_dot(clkPanel, X, Y);

	/* ��3�����ϰ벿�� */
	X = *(clkPanel->panCenter->cx) - y;
	Y = *(clkPanel->panCenter->cy) + x;
	ret = set_panel_dot(clkPanel, X, Y);

	/* ��4�����ϰ벿�� */
	X = *(clkPanel->panCenter->cx) + y;
	Y = *(clkPanel->panCenter->cy) + x;
	ret = set_panel_dot(clkPanel, X, Y);

	/* ��1�����°벿�� */
	X = *(clkPanel->panCenter->cx) + y;
	Y = *(clkPanel->panCenter->cy) - x;
	ret = set_panel_dot(clkPanel, X, Y);

	/* ��2�����°벿�� */
	X = *(clkPanel->panCenter->cx) - y;
	Y = *(clkPanel->panCenter->cy) - x;
	ret = set_panel_dot(clkPanel, X, Y);

	/* ��4�����°벿�� */
	X = *(clkPanel->panCenter->cx) + x;
	Y = *(clkPanel->panCenter->cy) + y;
	ret = set_panel_dot(clkPanel, X, Y);

	/* ��3�����°벿�� */
	X = *(clkPanel->panCenter->cx) - x;
	Y = *(clkPanel->panCenter->cy) + y;
	ret = set_panel_dot(clkPanel, X, Y);

	return ret;
}

int cast_arc_dots_to_panel(struct clk_plate_prop *clkPlate, struct clk_panel_prop *clkPanel)
{
	u8 i;
	int ret=0;

	if((clkPlate == NULL) || (clkPlate->active != TRUE) || (clkPanel == NULL))
		return -EINVAL;

	for(i=0; i<clkPlate->arc_bufsize; i++) {
		arc_dots_to_panel(clkPlate, clkPanel, i);
	}

	return ret;
}
int timer_digit_data_init(struct timer_digital *digTime)
{
	int ret =0;

	if(digTime == NULL)
		return -EFAULT;

	digTime->plate  = &plate;

#if ENABLE_DYNAMIC_TIME
	digTime->year	= &dateTime[0];
	digTime->month	= &dateTime[1];
	digTime->day	= &dateTime[2];

	digTime->hour	= &dateTime[3];
	digTime->minute = &dateTime[4];
	digTime->second = &dateTime[5];  /*  dateTime+5*d  */
#else
	digTime->year	= 0;
	digTime->month	= 0;
	digTime->day	= 0;

	digTime->hour	= 10;
	digTime->minute = 21;
	digTime->second = 13;
#endif

	return ret;
}


int panel_data_init(struct clk_panel_prop *clkPanel)
{
	//u8 panel_dotsPos[PANEL_HORIZONTAL][PANEL_VERTICAL] = {0};
	//u8 cent_x, cent_y;
	char ret = 0;

	if(clkPanel == NULL)
		return -EFAULT;

	panel_cent_x = PANEL_CENT_X;
	panel_cent_y = PANEL_CENT_Y;

	/* �ṹ��ָ���е�ʵ�����Ҳ�����?*/
	/* �ǽṹ��ָ���еı�������ָ�����������ͨ����?*/

	//clkPanel->dots_buf  = panel_dotsPos;

	//clkPanel->dots_buf  = &panel_dotsPos[0][0];
	//clkPanel->dots_buf  = *(panel_dotsPos+0);
	//clkPanel->dots_buf  = panel_dotsPos[0];
	//clkPanel->dots_buf  = *panel_dotsPos;		/* ��0�е�0�е�ַ*/
	clkPanel->dots_buf  = (u8 *)panel_64x64_data;

	//clkPanel->dots_buf	= panel_dotsPos;			/* ��0���׵�ַ*/  /* �������,��������ʹ�õط�����*/

	clkPanel->plate     = &plate;
	clkPanel->clkScale  = &clkScale;

	clkPanel->hourHand  = &hourHand;
	clkPanel->minuHand  = &minuHand;
	clkPanel->secHand   = &secHand;

	clkPanel->panCenter     = &pancenter;
	clkPanel->panCenter->cx = &panel_cent_x;
	clkPanel->panCenter->cy = &panel_cent_y;

	clkPanel->height    = PANEL_HORIZONTAL;
	clkPanel->width     = PANEL_VERTICAL;

	clkPanel->active    = TRUE;
	
	return ret;
}

int bresenham_algorithm_create_line_dots(struct clk_panel_prop *clkPanel,struct dot_pos *buf, u8 x0, u8 y0, u8 x1, u8 y1)
{
	int dx = ABS(x1 - x0);
	int dy = ABS(y1 - y0);
	int  x = x0;
	int  y = y0;
	int e=0, i=0;
	int stepX, stepY;

	stepX = (x0 > x1) ? -1 : 1;			// x0 > x1ʱ�Ǵ�������
	stepY = (y0 > y1) ? -1 : 1;			// y0 > y1ʱ�Ǵ������»�

	if(dx > dy)  					// ��������Ǹ���ǰ��
	{
		e = dy * 2 - dx;

		for(i=0; i<dx; i++) {			// for(i=0; i<=dx; i++)

			(*(buf + i)).x = x;		// should be ->  ??????
			(*(buf + i)).y = y;

			x += stepX;
			e += dy;

			if(e >= 0) {
				y += stepY;
				e -= dx;
			}
		}
	} else {
		e = 2 * dx - dy;

		for(i=0; i<dy; i++)			// for(i=0; i<=dy; i++)

			(*(buf + i)).x = x;
			(*(buf + i)).y = y;

			y += stepY;
			e += dx;

			if(e >= 0) {
				x += stepX;
				e -= dy;
			}
		}
	}
#if 0
	(*(buf + i)).x = '\0';
	(*(buf + i)).y = '\0';
#endif

	return i;
}

//int line_get_endpoint_base_linetable_and_arcdate(u8 r, int angle, struct dot_pos *endpoint, enum LINE_TYPE type)
int line_get_endpoint_base_linetable_and_arcdate(u8 r, struct dot_pos **endpoint, enum LINE_TYPE type)
{
	int size = 0, ret = 0;

	/*1.Ĭ����֪ԭ������(0,0) 
	 *2.���ݸ���x& angle,ͨ������ҵ�y���ֵ
	 *3.��ֵ��plate_dotsPos_buf[]�����ݽ��жԱ�,�ҵ���ӽ����Ǹ�����
	 *4.��󷵻ص�ֵ(x,y)��plate_dotsPos_buf[]�е�����
	 */

	/*0/3/6/9/12�̶�Ҫ�õ����*/

	/*��angle_xy_r26_tab[]ֻҪ֪��x&angle����֪��yֵ!!!*/
	/*����������İ�,���������Ŀ���ָ�򲻵�0/3/6/9�̶���Щ����ȥ*/
	/*����,�������Բ����ʹ�õ��㷨���»������Ĳ���׼���ѵ�ֱ��ҲҪ����׼��ȥ��? */

	/*��Բ��bresenham�㷨,��ֱ�߹ؼ���Ҫ֪���յ�,�յ�ͨ������õ��Ŀ�����*/
	/*���Ǳ���һ����,����bresenmham�㷨�õ��յ�x,y����*/

	/*�յ㻹�Ǿ�����x^2+y^2=r^2 & y=kx ����,��������һ��ͨ��excel�õ���angle_xy_r26_tab[]����
	 *�õ�x=��[(r^2/(1+K^2))^(1/2)], y=kx */

	/*���excel������ֱ�ߺ�Բ�ཻ��ľ���������,y=kx, x��r/k�ĺ���,r���º��ֻ��k��,���ݽǶȱ䵼��k����Եõ�������
	 *��bersenham ��Բ�㷨��ͨ�������ֱ�ӵõ�xy��ֵ��
	 *ͨ��excel��ͼ��ע��Ҫ��׼�������Σ����������ۿ����������Σ��ɽ��bersionham����Բ��������õ�6���ǵ�����*/

	/*������struct dot_pos endpoint_r30_on_plate[16]*/

//	endpoint = endpoint_r30_on_plate;
	*endpoint = endpoint_r30_on_plate;
	size = sizeof(endpoint_r30_on_plate) / sizeof(endpoint_r30_on_plate[0]);

	return size;
}

int draw_kinds_line(struct clk_panel_prop *clkPanel, enum LINE_TYPE type)
{
	int i, j, k, sz, cnt;
	int ret = 0;
	int m,n;
	int t;
	int handDiff;

	struct dot_pos buf[SCALE_R] = {0};              /*��ʱ���bresenham�㷨�õ���ֵ*/
//	struct dot_pos *buf = NULL;                     /*��ʱ���bresenham�㷨�õ���ֵ*/

	int bufLen = 0, len = 0;
	struct dot_pos *dotPos_l = NULL;
	struct dot_pos *endpoint_r30 = NULL;

	unsigned int time/*, htime*/;
	char quadrant, *last_quad = NULL;

	/* Բ����12���̶��ǹ̶������,������1/8 part����ʾ��
	 * Բ�������������������궼�ŵ��ڴ���,?�Ϊ���r�ܴ�?�Ҫ�ܶ����ݣ?
	 * ��12���̶���ȫ���԰󶨵�Բ���ϣ�
	 * ��ÿ���ʱ��������Ҫ���*/

//	sz = sizeof(endpoint_r30_on_plate) / sizeof(endpoint_r30_on_plate[0]);  /* ����endpoint_r30_on_plate[]��16���յ��ԭ��(0,0)���ɵ�1������scale����*/
//	sz = line_get_endpoint_base_linetable_and_arcdate(clkPanel->plate->r, clkscale->angle, endpoint_r30, LINE_ALL);/* size����������һ��Ҳ����һ���������� */
//	sz = line_get_endpoint_base_linetable_and_arcdate(clkPanel->plate->r, endpoint_r30, LINE_ALL);
	sz = line_get_endpoint_base_linetable_and_arcdate(clkPanel->plate->r, &endpoint_r30, LINE_ALL);

	switch(type) {
	  case LINE_SECOND:
	  case LINE_MINUTE:
	  case LINE_HOUR:
		if(type == LINE_SECOND) {
			time = *clkPanel->secHand->time;
			handDiff = 10;                        // 4
			m = time / 15;                        /* 15 dots in each quadrant */

			if(clkPanel->secHand->handLen > sz)   {  clkPanel->secHand->handLen = sz;  }

			dotPos_l  = clkPanel->secHand->dots_pos;
			bufLen    = clkPanel->secHand->dotPos_buf_size;
			last_quad = &(clkPanel->secHand->last_quadrant);

		} else if(type == LINE_MINUTE) {
			time = *clkPanel->minuHand->time;

			if((i == 6) || (i == 7) || (i == 8))  {  handDiff = 4+2;  }
			else                                  {  handDiff = 4+3;  }

			m = time / 15;                        /* 15 dots in each quadrant */

			if(clkPanel->minuHand->handLen > sz)  {  clkPanel->minuHand->handLen = sz;  }

			dotPos_l  = clkPanel->minuHand->dots_pos;
			bufLen    = clkPanel->minuHand->dotPos_buf_size;
			last_quad = &(clkPanel->minuHand->last_quadrant);

		} else if(type == LINE_HOUR) {
			time = *clkPanel->hourHand->time;
			time *= 5;                            /* 1 hour jump 5 scale */

			if((i == 6) || (i == 7) || (i == 8))  {  handDiff = 4+3+2;  }
			else                                  {  handDiff = 4+3+3;  }

			m = time / 15;

			if(clkPanel->hourHand->handLen > sz)  {  clkPanel->hourHand->handLen = sz;  }

			dotPos_l  = clkPanel->hourHand->dots_pos;
			bufLen    = clkPanel->hourHand->dotPos_buf_size;
			last_quad = &(clkPanel->hourHand->last_quadrant);
		}

		switch(m) {
			case 0:  *last_quad = 1;  n = time % 15;       break;   /* period is 15 */
			case 1:  *last_quad = 4;  n = 15 - time % 15;  break;
			case 2:  *last_quad = 3;  n = time % 15;       break;
			case 3:  *last_quad = 2;  n = 15 - time % 15;  break;
		}

		cnt = bresenham_algorithm_create_line_dots(clkPanel, buf, 0, 0, (endpoint_r30+n)->x, (endpoint_r30+n)->y);

		len = (bufLen < cnt) ? bufLen : cnt;

		/* ���ʱ������dots_pos_buf */
//		for(j=0; j < bufLen;       j++) {
//		for(j=0; j < cnt;          j++) {
//		for(j=0; j < len;          j++) {
		for(j=0; j < cnt-handDiff; j++) {
			(*(dotPos_l + j)).x = (*(buf + j)).x;
			(*(dotPos_l + j)).y = (*(buf + j)).y;
		}

		for(j=0,k=0; j<cnt-handDiff; j++,k++) {  // ���ܳ���cnt-ĩβ4���㼴�ɣ������Ƕ���Ϊ��0��ʼ��26����Bresenham�㷨ֱ�ߵ����ǲ�����
			//(*(*(clkscale->scaleBuf + 0) + k)).x = (*(buf + j)).x;	/*ʹ������ָ��ָ�������0����ʹ����������1/2/3�ǿ��Ե�*/
			//(*(*(clkscale->scaleBuf + 0) + k)).x = (*(buf + j)).y;
			switch(*last_quad) {
			  case 1:		/*��1 ����*/
//				m = *(clkPanel->panCenter->cx) + buf[j].x;
//				n  = *(clkPanel->panCenter->cy) - buf[j].y;

				m = *(clkPanel->panCenter->cx) + (*(buf + j)).x;
				n = *(clkPanel->panCenter->cy) - (*(buf + j)).y;

				ret = set_panel_dot(clkPanel, m, n);
				break;
			  case 4:		/*��4 ����*/
				m = *(clkPanel->panCenter->cx) + (*(buf + j)).x;
				n = *(clkPanel->panCenter->cy) + (*(buf + j)).y;

				ret = set_panel_dot(clkPanel, m, n);
				break;
			  case 3:		/*��3 ����*/
				m = *(clkPanel->panCenter->cx) - (*(buf + j)).x;
				n = *(clkPanel->panCenter->cy) + (*(buf + j)).y;

				ret = set_panel_dot(clkPanel, m, n);
				break;
			  case 2:		/*��2 ����*/
				m = *(clkPanel->panCenter->cx) - (*(buf + j)).x;
				n = *(clkPanel->panCenter->cy) - (*(buf + j)).y;

				ret = set_panel_dot(clkPanel, m, n);
				break;
			}
		}

		break;
	  case LINE_SCALE:
		clkscale = clkPanel->plate->clkscale;
		for(i=0; i<sz; i++) {
			clkscale->angle = 6 * i;

//			cnt = bresenham_algorithm_create_line_dots(clkPanel, buf, 0, 0, endpoint_r30[i].x, endpoint_r30[i].y);
			cnt = bresenham_algorithm_create_line_dots(clkPanel, buf, 0, 0, (*(endpoint_r30+i)).x, (*(endpoint_r30+i)).y);
			break;
		}
	}

	return ret;
}

/* bresenham_algorithm_create_arc_dots()��¼�����ԭ��(0,0)��arc dots����
 * ���ش�����������ָ��
 * ���溯��ͨ������ת??,����Щdots���ǵ�panel������
*/
int bresenham_algorithm_create_arc_dots(struct clk_plate_prop *clkPlate)
{
	u8 i = 0;
	int d;
	int ret = 0;

	if((clkPlate == NULL) || (clkPlate->dots_pos == NULL))
		return -EFAULT;

#if 0
	x = 0;
	y = clkPlate->r;
	d = 3 - 2 * clkPlate->r;

	//clkPlate->dots_pos[0]->x = x;	// dots_pos��ָ�룬������ô���!!!!!!
	//clkPlate->dots_pos[0]->y = y;

	(clkPlate->dots_pos+0)->x = x;
	(clkPlate->dots_pos+0)->y = y;

	while(x<y) {
		++i;
		if(i > CLK_PLATE_8thARC_SIZE)
		//if(++i > CLK_PLATE_8thARC_SIZE)
			return -EFAULT;

		if(d < 0) {
			d = d + 4 * x + 3;
		} else {
			d = d + 4 * (x - y) + 10;	//��P2��, ��һ����Yֵ��1
			y--;
		}
		x++;

		(clkPlate->dots_pos+i)->x = x;
		(clkPlate->dots_pos+i)->y = y;
	}

	//plate.dots_pos[i].x = '\0';	/*����i��֪�������ж��ٸ���������Բ���Ҳ����*/
	//plate.dots_pos[i].y = '\0';

	clkPlate->arc_bufsize = i;

	return ret;
#else
	/* directly calculate to get the coordinate values by PC */

	for(i=0; i<sizeof(eighth_arc_dots_r30); i++) {
		(clkPlate->dots_pos + i)->x = eighth_arc_dots_r30[i].x;
		(clkPlate->dots_pos + i)->y = eighth_arc_dots_r30[i].y;
	}

	return i--;
#endif
}

int plate_data_init(struct clk_plate_prop *clkPlate)
{
//	struct dot_pos plate_dotsPos_buf[CLK_PLATE_8thARC_SIZE];	/* �ṹ������*/
	u8 i, *tmp = NULL;
	u8 size;
	char ret = 0;

	if(clkPlate == NULL)
		return -EFAULT;

	/* clean plate buffer */
	tmp = (u8 *)plate_dotsPos_buf;		// &plate_dotsPos_buf
	for(i=0; i<sizeof(plate_dotsPos_buf); i++) {
		*(tmp + i) = 0;
	}

	clkPlate->panel      = &panel;
	clkPlate->timer      = &digTimer;

	clkPlate->hourhand   = &hourHand;
	clkPlate->minuhand   = &minuHand;
	clkPlate->sechand    = &secHand;

	clkPlate->platcenter = &plateCenter;
	clkPlate->clkscale   = &clkScale;

	clkPlate->r          = PLATE_R;
	clkPlate->x0         = *clkPlate->panel->panCenter->cx;
	clkPlate->y0         = *clkPlate->panel->panCenter->cy;

	clkPlate->margin_x   = 0;
	clkPlate->margin_y   = 0;

	//clkPlate->dots_pos   = &plate_dotsPos_buf;
	clkPlate->dots_pos   = plate_dotsPos_buf;
	clkPlate->arc_bufsize = sizeof(plate_dotsPos_buf);	/* default size */

	clkPlate->endpoint   = endpoint_r30_on_plate;

	size = bresenham_algorithm_create_arc_dots(clkPlate);
	if(! size) {
		return -EFAULT;
	} else {
		clkPlate->arc_bufsize = size;			// updata arc_bufsize
	}

	clkPlate->active	 = TRUE;

#if ENABLE_FULL_PATTERN
	ret = cast_arc_dots_to_panel(clkPlate, clkPlate->panel);
#endif

	return ret;
}

int plate_scale_data_init(struct clk_scale_prop *clkscale)
{
	struct dot_pos buf[SCALE_R] = {0};
	int ret = 0;

	if(clkscale == NULL)
		return -EFAULT;

	//clkscale->scaleBuf = scaleBuffer[0];		// scaleBufָ���0�е�0��Ԫ�ص�ַ
	clkscale->scaleBuf = scaleBuffer;		// scaleBufָ���0���׵�ַ
	//clkscale->size   = SCALE_8thPART_SIZE;
	clkscale->size	 = ENDPOINT_FOR_R30;		// 1/8Բ��������26����,��scale��ӦԲ���в���������ƽ���ĵ㣬ͨ��excel����ҵ���15����
	clkscale->len    = SCALE_LEN;

	clkscale->r      = SCALE_R;

	clkscale->plate  = &plate; 
	draw_kinds_line(clkscale->plate->panel, LINE_SCALE);

	clkscale->active = TRUE;

	return ret;
}

int plateCent_data_to_panel(struct plate_cent_prop *plaCenter)
{
	u8 panelCenter_x, panelCenter_y;
	struct clk_panel_prop  *pan;
	u8 x0,y0;
	int m, n;
	int ret = 0;

	if((plaCenter == NULL) || (plaCenter->plate == NULL))
		return -EFAULT;

	pan           = plaCenter->plate->panel;

	panelCenter_x = *(plaCenter->plate->panel->panCenter->cx);
	panelCenter_y = *(plaCenter->plate->panel->panCenter->cy);


	m = panelCenter_x;
	n = panelCenter_y;
	ret = set_panel_dot(pan, m, n);

	m = panelCenter_x + 1;
	n = panelCenter_y;
	ret = set_panel_dot(pan, m, n);

	m = panelCenter_x - 1;
	n = panelCenter_y;
	ret = set_panel_dot(pan, m, n);

	m = panelCenter_x;
	n = panelCenter_y + 1;
	ret = set_panel_dot(pan, m, n);

	m = panelCenter_x;
	n = panelCenter_y - 1;
	ret = set_panel_dot(pan, m, n);

	return ret;
}

int plateCent_data_init(struct plate_cent_prop *platecenter)
{
	//u8 circ_cent_buf[CIRCLE_CENT_BSZ] = {1,1,1,1,1};
	int ret = 0;

	if(platecenter == NULL)
		return -EFAULT;

	platecenter->plate = &plate;

	platecenter->centDot     = &centerDot;
	platecenter->centDot->cx = &platecenter->plate->x0; /*recode center dot's x,y coordinate */
	platecenter->centDot->cy = &platecenter->plate->y0; /*platecenter����ָ��plate�����ġ���platecenter���겻����plateCent_data_to_panel����*/

	platecenter->cent_buf = circ_cent_buf;

	//plateCent_data_to_panel(platecenter->plate->panel);
	plateCent_data_to_panel(platecenter);

	platecenter->active = TRUE;

	return ret;
}

//����:��(x0,y0)��(x1,y1)֮�仭һ��ֱ��(Bresenham�㷨)
int draw_bresenham_line(struct dot_pos *dots_pos, u8 x0, u8 y0, u8 x1, u8 y1)
{
	u8 i;
	u8 xerr=0,yerr=0;
	char dx,dy;
	u8 distance;
	signed char incx,incy;
	u8 row,col;
	int ret = 0;

	if(dots_pos == NULL)
		return -EFAULT;

	dx = x1-x0;			//����X�������� (x��ʼĩ����)
	dy = y1-y0;  			//����Y�������� (y��ʼĩ����)

	//col = x0;
	//row = y0;
	dots_pos[0].x = x0;			//��ʼ����
	dots_pos[0].y = y0;

	//���õ�������,�жϵ�������
	if(dx == 0)     {   incx =  0;            }     //��ֱ��
	else if(dx > 0) {   incx =  1;            }
	else            {   incx = -1;  dx *= -1; }		//��X�����ľ���ֵ

	if(dy == 0)     {   incy = 0;             }		//ˮƽ��
	else if(dy > 0) {   incy = 1;             }
	else            {   incy = -1;  dy *= -1; }     //��Y�����ľ���ֵ

	distance = (dx > dy) ? dx : dy;	                //ѡȡ���ֵ����;  dx��������x��������б��С��45��

	/* distance ��ú�dots_pos �Ĵ�С�Ƚ�һ��*/
	if(distance > sizeof(dots_pos)/sizeof(dots_pos[0]))
		return -EFAULT;

	//����
	for(i=1; i<=distance+2; i++) {
		dots_pos[i].x = col;						//ÿ����������¼һ����
		dots_pos[i].y = row;

		xerr += dx;
		yerr += dy;

		if(xerr > distance) {
			xerr -= distance;
			col += incx;
		}
		if(yerr > distance) {
			yerr -= distance;
			row += incy;
		}
	}

#if 0
	for(i=0; i<=distance+1; i++) {
		LCD12864_DrawPoint(col,row);	//ÿ����������һ����
		xerr += dx;
		yerr += dy;
		if(xerr > distance) {
			xerr -= distance;
			col += incx;
		}
		if(yerr > distance) {
			yerr -= distance;
			row += incy;
		}
	}
#endif

	return ret;
}



int transfer_humAngle_to_axesAngle(int humAngle)
{
	int angle = 0;

	/* humAngle: 0��12��λ��,90��3��λ��,120��4��λ�� */
	/* 360 + 90 = 450 */
	if((0 < humAngle) && (humAngle < 90)) {
		angle = 90 - humAngle;
	} else if((90 < humAngle) && (humAngle < 360)) {
		angle = 450 - humAngle;
	}

	/* 0/90/180/270/360 ???   0 <= a < 360, ������������ʽ*/

	return angle;
}

int hour_hand_data_init(struct clk_hands_prop *hourhand, struct timer_digital *dtimer)
{
	u8 i, *tmp = NULL;
	//struct dot_pos hour_dotspos_buf[CLK_HHAND_LEN * CLK_HHAND_WID];
	int ret = 0;

	if(hourhand == NULL)
		return -EFAULT;

	tmp = (u8 *)hour_dotspos_buf;
	for(i=0; i<sizeof(hour_dotspos_buf);i++) {
		*(tmp + i) = 0;
	}

	hourhand->plate    = &plate;

	hourhand->x0       = hourhand->plate->x0;
	hourhand->y0       = hourhand->plate->y0;

	hourhand->handLen  = CLK_HHAND_LEN;
	hourhand->handwide = CLK_HHAND_WID;
#if 0
	hourhand->Htimer   = &hourhand->plate->timer->hour;
#else
	hourhand->time   = dtimer->hour;
#endif
	hourhand->angle    = 0;

	hourhand->dots_pos = hour_dotspos_buf;
	//hourhand->dotPos_buf_size = sizeof(hour_dotspos_buf);
	hourhand->dotPos_buf_size = sizeof(hour_dotspos_buf) / sizeof(hour_dotspos_buf[0]);

#if 1
	draw_kinds_line(hourhand->plate->panel, LINE_HOUR);

#endif

	hourhand->active   = TRUE;

	/* 0. Ҫ��bresenham�㷨��ֱ��,��Ҫ֪����ʼ��& �յ�
	  * 1. ��λ�ȡʱ����յ�����
	  * 2. ��֪������(0,0) & handlen & angle
	  * 3. ��ֱ����ô�ͻ�Բ�Ǳ�ĩ����
	  * 4. angle�൱��б��k
	  * 5. �յ�����Ӧ�úõõ���
	  * 6. ---- angle is changing ----
	  * 7. ��ʱ�������ظ�����,�������鱣��,������һ���볤30��,���360��xy����Ҫ30*2*360=2w1
	  */

	//ret = update_bresenham_line(CLK_HOUR_HAND, (void *)hourhand);
	//ret = hourhand_data_to_panel();

	return ret;
}

int minute_hand_data_init(struct clk_hands_prop *minuhand, struct timer_digital *dtimer)
{
	u8 i, *tmp = NULL;
	int ret = 0;

	if(minuhand == NULL)
		return -EFAULT;

	tmp = (u8 *)minu_dotspos_buf;
	for(i=0; i<sizeof(minu_dotspos_buf);i++) {
		*(tmp + i) = 0;
	}

	minuhand->plate	  = &plate;

	minuhand->x0 	  = minuhand->plate->x0;
	minuhand->y0 	  = minuhand->plate->y0;

	minuhand->handLen  = CLK_MHAND_LEN;
	minuhand->handwide = CLK_MHAND_WID;
#if 0
	minuhand->Htimer   = &minuhand->plate->timer->hour;  /* Htimer ����?*/
#else
	minuhand->time	 = dtimer->minute;
#endif

	minuhand->angle	  = 0;

	minuhand->dots_pos = minu_dotspos_buf;
	//minuhand->dotPos_buf_size = sizeof(minu_dotspos_buf);
	minuhand->dotPos_buf_size = sizeof(minu_dotspos_buf) / sizeof(minu_dotspos_buf[0]);

#if 1
	draw_kinds_line(minuhand->plate->panel, LINE_MINUTE);
#endif

	minuhand->active   = TRUE;

	return ret;
}

int second_hand_data_init(struct clk_hands_prop *sechand, struct timer_digital *dtimer)
{
//	struct dot_pos sec_dotspos_buf[CLK_SHAND_LEN * CLK_SHAND_WID];
	u8 i, *tmp = NULL;
	int ret = 0;

	if(sechand == NULL)
		return -EFAULT;

	tmp = (u8 *)sec_dotspos_buf;
	for(i=0; i<sizeof(sec_dotspos_buf);i++) {
		*(tmp + i) = 0;
	}

	sechand->plate    = &plate;

	sechand->x0       = sechand->plate->x0;
	sechand->y0       = sechand->plate->y0;

	sechand->handLen  = CLK_SHAND_LEN;
	sechand->handwide = CLK_SHAND_WID;
#if 0
	sechand->Htimer   = &sechand->plate->timer->hour;  /* Htimer ����?*/
#else
	sechand->time	 = dtimer->second;
#endif

	sechand->angle    = 0;

	sechand->dots_pos = sec_dotspos_buf;
//	sechand->dotPos_buf_size = sizeof(sec_dotspos_buf);
	sechand->dotPos_buf_size = sizeof(sec_dotspos_buf) / sizeof(sec_dotspos_buf[0]);

#if 1
	draw_kinds_line(sechand->plate->panel, LINE_SECOND);
#endif

	sechand->active   = TRUE;

	return ret;
}

char clk_panel_init(void)
{
	char ret = 0;

	timer_digit_data_init(&digTimer);

	panel_data_init(&panel);

	plate_data_init(&plate);

	plateCent_data_init(&plateCenter);

	plate_scale_data_init(&clkScale);  // panel

	hour_hand_data_init(&hourHand, &digTimer);

	minute_hand_data_init(&minuHand, &digTimer);

	second_hand_data_init(&secHand, &digTimer);

	return ret;
}

int draw_panel_graphics(void)
{
	u8 i,j;
	int ret = 0;

	for(i=0; i<panel.width; i++) {
		for(j=0; j<panel.height; j++) {
			ret = 0;
			//draw_dots(panel->dots_buf[i][j]);
		}
	}

	return ret;
}

/*
  * interrupt. update clock second/minute/hour hand date
  * ��ʱ1sʱ�䵽,��secHand�м�����1���Ƕ���1
  * 
*/
