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
struct dot_pos scaleBuffer[ENDPOINT_FOR_R30][SCALE_LEN] = {0};  //应该选用1/8圆弧上通过描点法找到第一象限的15个点

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
	{ 18, 25 },   //{ 17, 24 },  // 同时buf数据要修正下
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
	u16 m;						/*下标要达到512,不能用char !!!*/
	u8 ret = 0;

	/* 64x64的panel可以看成64x64个点阵数组,也可看成64*8个字节数组*/

	ax = X;

	y1 = (Y / 8);
	y2 = (Y % 8);					/* 可理解成一个字节的第几个bit */

#if 0
	if((y1==0))     {	m = ax;				}	//第一行,即LCD第0页
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


/* arc_dots_to_panel()将bresenham算法得到的arc数据投到panel上
  *  panel和arc数据需要有个共同基点:panel中心和arc圆心在同一个位置!!!
  *  clkPlate->dots_pos指向的数组中的数据(x,y)都是相对原点的坐标
  *  arc的数据本属于plate的,但是简便来可直接投到panel上去,否则空间占用大
  *  arc是1/8圆周
  */
int arc_dots_to_panel(struct clk_plate_prop *clkPlate, struct clk_panel_prop *clkPanel, u8 n)
{
	u8 X,Y,x,y,ax,ay;               /* X & Y is belong to panel axes, (X<= clkPanel->width<=64<128, Y<= clkPanel->height<=64)
                                         * x & y是1/8圆弧arc数据坐标,且在第一象限直线y=x上方转换到panel上后的坐标
                                         * ax & ay是clkPanel->dots_buf指向的(64x64)数组中的成员下标*/
	u8 y1,y2,m,b,v;
	char ret = 0;

	if((clkPlate == NULL) || (n > clkPlate->r))
		return (char)-EINVAL;

	/*
	 * 给出一个点，要map到panel上去
	 * 这些arc点都在第一象限45度上半部分
	 * 现已将坐标轴上4个象限从新按顺时钟方向分成8个区域
	 * area1 的(x,y)映射到panel的其他area上可通过MAP_AREA_x宏来实现
	 * 上面是点(x,y)的表示,下面难点是如何标示panel里数组的下标?
	 * panel成员panel_buf最大是128*64个字节
	 * 表盘上(x,y)如何转换到panel_buf上(x',y')上去?
	 * (1)(x,y)在(x0,y0)坐标系的第1象限,则x'=x0+x, y'=y0-y;
	 * (2)                        2象限,则x'=x0-x, y'=y0-y;
	 * (3)                        3象限,则x'=x0-x, y'=y0+y;
	 * (4)                        4象限,则x'=x0+x, y'=y0+y;
	 * (5)                        x轴正半轴上,则x'=x0+x, y'=0;
	 * (6)                        x轴负半轴上,则x'=x0-x, y'=0;
	 * (7)                        y轴正半轴上,则x'=x0,    y'=y0-y;
	 * (8)                        y轴负半轴上,则x'=x0,    y'=y0+y
	 */

	/* 1/8圆弧实际就是部分表盘plate，所以两者的圆心是同一个
	 *  将圆弧数组坐标转成panel上坐标,注意是一个dot，不是一个字节
	 *  再将dot转成一个数组成员(字节)中的某一bit位上
	 */

	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	/* 第1象限上半部分 */
	X = *(clkPanel->panCenter->cx) + x;
	Y = *(clkPanel->panCenter->cy) - y;
	ret = set_panel_dot(clkPanel, X, Y);

	/* 第2象限上半部分 */
	X = *(clkPanel->panCenter->cx) - x;
	Y = *(clkPanel->panCenter->cy) - y;
	ret = set_panel_dot(clkPanel, X, Y);

	/* 第3象限上半部分 */
	X = *(clkPanel->panCenter->cx) - y;
	Y = *(clkPanel->panCenter->cy) + x;
	ret = set_panel_dot(clkPanel, X, Y);

	/* 第4象限上半部分 */
	X = *(clkPanel->panCenter->cx) + y;
	Y = *(clkPanel->panCenter->cy) + x;
	ret = set_panel_dot(clkPanel, X, Y);

	/* 第1象限下半部分 */
	X = *(clkPanel->panCenter->cx) + y;
	Y = *(clkPanel->panCenter->cy) - x;
	ret = set_panel_dot(clkPanel, X, Y);

	/* 第2象限下半部分 */
	X = *(clkPanel->panCenter->cx) - y;
	Y = *(clkPanel->panCenter->cy) - x;
	ret = set_panel_dot(clkPanel, X, Y);

	/* 第4象限下半部分 */
	X = *(clkPanel->panCenter->cx) + x;
	Y = *(clkPanel->panCenter->cy) + y;
	ret = set_panel_dot(clkPanel, X, Y);

	/* 第3象限下半部分 */
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

	/* 结构体指针中的实体变量也是虚的?*/
	/* 那结构体指针中的变量是用指针变量还是普通变量?*/

	//clkPanel->dots_buf  = panel_dotsPos;

	//clkPanel->dots_buf  = &panel_dotsPos[0][0];
	//clkPanel->dots_buf  = *(panel_dotsPos+0);
	//clkPanel->dots_buf  = panel_dotsPos[0];
	//clkPanel->dots_buf  = *panel_dotsPos;		/* 第0行第0列地址*/
	clkPanel->dots_buf  = (u8 *)panel_64x64_data;

	//clkPanel->dots_buf	= panel_dotsPos;			/* 第0行首地址*/  /* 这样设计,编译器对使用地方报错*/

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

	stepX = (x0 > x1) ? -1 : 1;			// x0 > x1时是从右向左画
	stepY = (y0 > y1) ? -1 : 1;			// y0 > y1时是从上向下画

	if(dx > dy)  					// 沿着最长的那个轴前进
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

	/*1.默认已知原点坐标(0,0) 
	 *2.依据给定x& angle,通过查表法找到y轴的值
	 *3.将值和plate_dotsPos_buf[]中数据进行对比,找到最接近的那个坐标
	 *4.最后返回的值(x,y)是plate_dotsPos_buf[]中的数据
	 */

	/*0/3/6/9/12刻度要用到这个*/

	/*但angle_xy_r26_tab[]只要知道x&angle就能知道y值!!!*/
	/*还是用上面的吧,这样出来的可能指向不到0/3/6/9刻度这些点上去*/
	/*但是,如果外面圆由于使用的算法导致画出来的不标准，难道直线也要不标准下去吗? */

	/*画圆用bresenham算法,画直线关键是要知道终点,终点通过查表法得到的靠谱吗*/
	/*还是保持一致性,即用bresenmham算法得到终点x,y坐标*/

	/*终点还是觉得用x^2+y^2=r^2 & y=kx 来求,而不是另一套通过excel得到的angle_xy_r26_tab[]来求
	 *得到x=±[(r^2/(1+K^2))^(1/2)], y=kx */

	/*结合excel表计算出直线和圆相交点的具体坐标组,y=kx, x是r/k的函数,r定下后就只有k变,依据角度变导致k变可以得到数据组
	 *而bersenham 画圆算法可通过计算机直接得到xy的值，
	 *通过excel画图，注意要标准的正方形，而不是肉眼看到的正方形，可结合bersionham画的圆经矫正后得到6倍角的坐标*/

	/*体现在struct dot_pos endpoint_r30_on_plate[16]*/

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

	struct dot_pos buf[SCALE_R] = {0};              /*临时存放bresenham算法得到的值*/
//	struct dot_pos *buf = NULL;                     /*临时存放bresenham算法得到的值*/

	int bufLen = 0, len = 0;
	struct dot_pos *dotPos_l = NULL;
	struct dot_pos *endpoint_r30 = NULL;

	unsigned int time/*, htime*/;
	char quadrant, *last_quad = NULL;

	/* 圆环和12个刻度是固定不变的,可以用1/8 part来表示；
	 * 圆环不可能上面所有坐标都放到内存里,?蛭绻鹯很大?枰芏嗍荩?
	 * 而12个刻度完全可以绑定到圆环上；
	 * 而每秒的时分秒针是要变的*/

//	sz = sizeof(endpoint_r30_on_plate) / sizeof(endpoint_r30_on_plate[0]);  /* 依据endpoint_r30_on_plate[]中16个终点和原点(0,0)生成第1象限中scale数据*/
//	sz = line_get_endpoint_base_linetable_and_arcdate(clkPanel->plate->r, clkscale->angle, endpoint_r30, LINE_ALL);/* size和其他变量一样也丢到一个函数处理 */
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

		/* 填充时分秒针dots_pos_buf */
//		for(j=0; j < bufLen;       j++) {
//		for(j=0; j < cnt;          j++) {
//		for(j=0; j < len;          j++) {
		for(j=0; j < cnt-handDiff; j++) {
			(*(dotPos_l + j)).x = (*(buf + j)).x;
			(*(dotPos_l + j)).y = (*(buf + j)).y;
		}

		for(j=0,k=0; j<cnt-handDiff; j++,k++) {  // 将总长度cnt-末尾4个点即可，而不是定死为从0开始的26个。Bresenham算法直线点数是不定的
			//(*(*(clkscale->scaleBuf + 0) + k)).x = (*(buf + j)).x;	/*使用数组指针指向的数组0，不使用其他数组1/2/3是可以的*/
			//(*(*(clkscale->scaleBuf + 0) + k)).x = (*(buf + j)).y;
			switch(*last_quad) {
			  case 1:		/*第1 象限*/
//				m = *(clkPanel->panCenter->cx) + buf[j].x;
//				n  = *(clkPanel->panCenter->cy) - buf[j].y;

				m = *(clkPanel->panCenter->cx) + (*(buf + j)).x;
				n = *(clkPanel->panCenter->cy) - (*(buf + j)).y;

				ret = set_panel_dot(clkPanel, m, n);
				break;
			  case 4:		/*第4 象限*/
				m = *(clkPanel->panCenter->cx) + (*(buf + j)).x;
				n = *(clkPanel->panCenter->cy) + (*(buf + j)).y;

				ret = set_panel_dot(clkPanel, m, n);
				break;
			  case 3:		/*第3 象限*/
				m = *(clkPanel->panCenter->cx) - (*(buf + j)).x;
				n = *(clkPanel->panCenter->cy) + (*(buf + j)).y;

				ret = set_panel_dot(clkPanel, m, n);
				break;
			  case 2:		/*第2 象限*/
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

/* bresenham_algorithm_create_arc_dots()记录相对于原点(0,0)的arc dots坐标
 * 返回存放坐标的数组指针
 * 后面函数通过坐标转??,将这些dots覆盖到panel数据中
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

	//clkPlate->dots_pos[0]->x = x;	// dots_pos是指针，不能这么表达!!!!!!
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
			d = d + 4 * (x - y) + 10;	//离P2近, 下一个点Y值减1
			y--;
		}
		x++;

		(clkPlate->dots_pos+i)->x = x;
		(clkPlate->dots_pos+i)->y = y;
	}

	//plate.dots_pos[i].x = '\0';	/*返回i就知道数据有多少个，这里可以不用也可以*/
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
//	struct dot_pos plate_dotsPos_buf[CLK_PLATE_8thARC_SIZE];	/* 结构体数组*/
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

	//clkscale->scaleBuf = scaleBuffer[0];		// scaleBuf指向第0行第0列元素地址
	clkscale->scaleBuf = scaleBuffer;		// scaleBuf指向第0行首地址
	//clkscale->size   = SCALE_8thPART_SIZE;
	clkscale->size	 = ENDPOINT_FOR_R30;		// 1/8圆弧可能有26个点,但scale对应圆弧中不连续而又平均的点，通过excel描点找到是15个点
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
	platecenter->centDot->cy = &platecenter->plate->y0; /*platecenter中心指向plate的中心。但platecenter坐标不参与plateCent_data_to_panel运算*/

	platecenter->cent_buf = circ_cent_buf;

	//plateCent_data_to_panel(platecenter->plate->panel);
	plateCent_data_to_panel(platecenter);

	platecenter->active = TRUE;

	return ret;
}

//功能:在(x0,y0)到(x1,y1)之间画一条直线(Bresenham算法)
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

	dx = x1-x0;			//计算X坐标增量 (x轴始末增量)
	dy = y1-y0;  			//计算Y坐标增量 (y轴始末增量)

	//col = x0;
	//row = y0;
	dots_pos[0].x = x0;			//起始坐标
	dots_pos[0].y = y0;

	//设置单步方向,判断递增方向
	if(dx == 0)     {   incx =  0;            }     //垂直线
	else if(dx > 0) {   incx =  1;            }
	else            {   incx = -1;  dx *= -1; }		//求X增量的绝对值

	if(dy == 0)     {   incy = 0;             }		//水平线
	else if(dy > 0) {   incy = 1;             }
	else            {   incy = -1;  dy *= -1; }     //求Y增量的绝对值

	distance = (dx > dy) ? dx : dy;	                //选取最大值增量;  dx大则若在x正半轴上斜率小于45度

	/* distance 最好和dots_pos 的大小比较一下*/
	if(distance > sizeof(dots_pos)/sizeof(dots_pos[0]))
		return -EFAULT;

	//画线
	for(i=1; i<=distance+2; i++) {
		dots_pos[i].x = col;						//每单步递增记录一个点
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
		LCD12864_DrawPoint(col,row);	//每单步递增画一个点
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

	/* humAngle: 0度12点位置,90度3点位置,120度4点位置 */
	/* 360 + 90 = 450 */
	if((0 < humAngle) && (humAngle < 90)) {
		angle = 90 - humAngle;
	} else if((90 < humAngle) && (humAngle < 360)) {
		angle = 450 - humAngle;
	}

	/* 0/90/180/270/360 ???   0 <= a < 360, 都符合上述公式*/

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

	/* 0. 要用bresenham算法画直线,需要知道起始点& 终点
	  * 1. 如何获取时针的终点坐标
	  * 2. 已知的量是(0,0) & handlen & angle
	  * 3. 画直线怎么和画圆是本末倒置
	  * 4. angle相当于斜率k
	  * 5. 终点坐标应该好得到吧
	  * 6. ---- angle is changing ----
	  * 7. 虽时分秒针重复工作,想用数组保存,但发现一个针长30点,存放360个xy坐标要30*2*360=2w1
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
	minuhand->Htimer   = &minuhand->plate->timer->hour;  /* Htimer 作用?*/
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
	sechand->Htimer   = &sechand->plate->timer->hour;  /* Htimer 作用?*/
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
  * 定时1s时间到,则secHand中计数增1，角度增1
  * 
*/
