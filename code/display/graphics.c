#include <stm32f10x.h>		// u8
#include "graphics.h"

struct clk_plate_prop plate;
struct clk_panel_prop panel;

struct clk_hands_prop hourHand;
struct clk_hands_prop minuHand;
struct clk_hands_prop secHand;


struct plate_cent_prop plateCenter;
struct clk_scale_prop  clkScale;

struct timer_digital   digTimer;


#if ENABLE_DYNAMIC_TIME
volatile unsigned int dateTime[6] = {2018, 4, 1, 0, 4, 47};
volatile char update_panel_enable = 0;

int draw_kinds_line(struct clk_panel_prop *clkPanel, enum LINE_TYPE type);
#endif

int arc_dot_data_to_panel(struct clk_plate_prop *clkPlate, u8 n, struct clk_panel_prop *clkPanel)
{
	u8 X,Y,x,y;		// X & Y is belong to panel axes, x0 & y0 are belong too
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
	 * (x,y)在(x0,y0)坐标系的第1象限,则x'=x0+x, y'=y0-y;  此时xx0,yy0
	 *                                                 2           ,则x'=x0-x, y'=y0-y;  此时xx0,yy0
	 *                                                 3           ,则x'=x0-x, y'=y0+y;  此时xx0,yy0
	 *                                                 4           ,则x'=x0+x, y'=y0+y;  此时xx0,yy0
	 *                                  x轴正半轴上,则x'=x0+x, y'=0    ;  此时xx0,yy0
	 *                                  x轴负半轴上,则x'=x0-x, y'=0     ;  此时xx0,yy0
	 *                                  y轴正半轴上,则x'=x0,     y'=y0-y;  此时xx0,yy0
	 *                                  y轴负半轴上,则x'=x0,     y'=y0+y;  此时xx0,yy0
	 */


	/* the 8th arc is in the top half of the 1st quadrant */
	/* 第1象限上半部分 */
	//x = clkPlate->dots_pos[n]->x;   /* 指针和指针数组*/
	//y = clkPlate->dots_pos[n]->y;

	//x = clkPlate->dots_pos[n]->x;   /* 指针和指针数组*/
	//y = clkPlate->dots_pos[n]->y;

	/* clkPlate->dots_pos已经指向1维结构体数组首地址(首个元素地址)*/
	/* clkPlate->dots_pos + 1指向1维结构体数组第二个元素地址*/
	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	X = clkPlate->x0 + x;  // contain x=0, x0 contain margin
	Y = clkPlate->y0 - y;

	if((X > clkPanel->height) || (Y > clkPanel->width)) {
		return -EFAULT;
	}
	//clkPanel->dots_buf[ X][ Y] = 1; // 第一象限上半部分

	/* clkPanel->dots_buf指向第0行首地址 */
	/* (x) clkPanel->dots_buf + 1指向第1行首地址 */
	/* (x) *(clkPanel->dots_buf + 1)指向第1行第0列元素地址,非&(clkPanel->dots_buf + 1) */
	/* (x) *(clkPanel->dots_buf + 1) + 2 指向第1行第2列元素地址 */
	/* (x) *(*(clkPanel->dots_buf + 1) + 2 )指向第1行第2列元素的值 */

	/* clkPanel->dots_buf指向第0行首地址 */
	/* clkPanel->dots_buf是u8 *类型的, +1不是指向二维数组下一行*/
	*(clkPanel->dots_buf + X * clkPanel->width + Y) = 1;  /* 是否应该dots_buf指向第0行第0列元素地址更好? */


	/* 第1象限下半部分 : (x,y)->(y,x), '+' */
	//x = clkPlate->dots_pos[n]->y;
	//y = clkPlate->dots_pos[n]->x;
	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	X = clkPlate->x0 + x;
	Y = clkPlate->y0 - y;

	if((X > clkPanel->height) || (Y > clkPanel->width)) {
		return -EFAULT;
	}
	//clkPanel->dots_buf[ X][ Y] = 1;
	*(clkPanel->dots_buf + X * clkPanel->width + Y) = 1;


	/* 第2象限上半部分 */
	//x = -clkPlate->dots_pos[n]->x;
	//y =  clkPlate->dots_pos[n]->y;
	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	X = clkPlate->x0 - x;
	Y = clkPlate->y0 - y;

	if((X > clkPanel->height) || (Y > clkPanel->width)) {
		return -EFAULT;
	}
	//clkPanel->dots_buf[ X][ Y] = 1;
	*(clkPanel->dots_buf + X * clkPanel->width + Y) = 1;

	/* 第2象限下半部分 */
	//x = -clkPlate->dots_pos[n]->y;
	//y =  clkPlate->dots_pos[n]->x;
	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	X = clkPlate->x0 - x;
	Y = clkPlate->y0 - y;

	if((X > clkPanel->height) || (Y > clkPanel->width)) {
		return -EFAULT;
	}
	//clkPanel->dots_buf[ X][ Y] = 1;
	*(clkPanel->dots_buf + X * clkPanel->width + Y) = 1;

	/* 第3象限上半部分 */
	//x = -clkPlate->dots_pos[n]->y;
	//y = -clkPlate->dots_pos[n]->x;
	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	X = clkPlate->x0 - x;
	Y = clkPlate->y0 + y;

	if((X > clkPanel->height) || (Y > clkPanel->width)) {
		return -EFAULT;
	}
	//clkPanel->dots_buf[ X][ Y] = 1;
	*(clkPanel->dots_buf + X * clkPanel->width + Y) = 1;

	/* 第3象限下半部分 */
	//x = -clkPlate->dots_pos[n]->x;
	//y = -clkPlate->dots_pos[n]->y;
	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	X = clkPlate->x0 - x;
	Y = clkPlate->y0 + y;

	if((X > clkPanel->height) || (Y > clkPanel->width)) {
		return -EFAULT;
	}
	//clkPanel->dots_buf[ X][ Y] = 1;
	*(clkPanel->dots_buf + X * clkPanel->width + Y) = 1;

	/* 第4象限上半部分 */
	//x =  clkPlate->dots_pos[n]->x;
	//y = -clkPlate->dots_pos[n]->y;
	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	X = clkPlate->x0 + x;
	Y = clkPlate->y0 + y;

	if((X > clkPanel->height) || (Y > clkPanel->width)) {
		return -EFAULT;
	}
	//clkPanel->dots_buf[ X][ Y] = 1;
	*(clkPanel->dots_buf + X * clkPanel->width + Y) = 1;

	/* 第4象限下半部分 */
	//x =  clkPlate->dots_pos[n]->y;
	//y = -clkPlate->dots_pos[n]->x;
	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	X = clkPlate->x0 + x;
	Y = clkPlate->y0 + y;

	if((X > clkPanel->height) || (Y > clkPanel->width)) {
		return -EFAULT;
	}
	//clkPanel->dots_buf[ X][ Y] = 1;
	*(clkPanel->dots_buf + X * clkPanel->width + Y) = 1;

	return ret;

}

int arc_data_to_panel(struct clk_plate_prop *clkPlate, struct clk_panel_prop *clkPanel)
{
	u8 i;
	int ret=0;

	if((clkPlate == NULL) || (clkPanel == NULL) || (clkPlate->active != TRUE))
		return -EINVAL;

	for(i=0; i<clkPlate->arc_bufsize; i++) {
		arc_dot_data_to_panel(clkPlate, i, clkPanel);
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
	u8 panel_dotsPos[PANEL_HORIZONTAL][PANEL_VERTICAL] = {0};
	u8 cent_x, cent_y;
	char ret = 0;

	if(clkPanel == NULL)
		return -EFAULT;

	/* 结构体指针中的实体变量也是虚的?*/
	/* 那结构体指针中的变量是用指针变量还是普通变量?*/

	//clkPanel->dots_buf  = panel_dotsPos;

	//clkPanel->dots_buf  = &panel_dotsPos[0][0];
	//clkPanel->dots_buf  = *(panel_dotsPos+0);
	//clkPanel->dots_buf  = panel_dotsPos[0];
	clkPanel->dots_buf  = *panel_dotsPos;		/* 第0行第0列地址*/

	//clkPanel->dots_buf	= panel_dotsPos;			/* 第0行首地址*/  /* 这样设计,编译器对使用地方报错*/

	//kPanel->dots_bhsize = PANEL_HORIZONTAL;
	//kPanel->dots_bwsize = PANEL_VERTICAL;

	clkPanel->plate     = &plate;
	//clkPanel->clkScale  = &clkScale;

	//clkPanel->hourHand  = &hourHand;
	//clkPanel->minuHand  = &minuHand;
	//clkPanel->secHand   = &secHand;

	cent_x = PANEL_CENT_X;
	cent_y = PANEL_CENT_Y;

	clkPanel->panCenter->cx = &cent_x;
	clkPanel->panCenter->cy = &cent_y;

	clkPanel->height    = PANEL_HORIZONTAL;
	clkPanel->width     = PANEL_VERTICAL;

	clkPanel->active    = TRUE;
	
	return ret;
}

int plate_cent_data_to_panel(struct plate_cent_prop *placenter)
{
	u8 x0,y0;
	int ret = 0;

	if((placenter == NULL) || (placenter->plate == NULL) || (placenter->active != TRUE))
		return -EFAULT;

	x0 = *placenter->plaCenter->cx;
	y0 = *placenter->plaCenter->cy;

#if 0
	placenter->plate->dots_pos[x0][y0]   = placenter->cent_buf[0];
	placenter->plate->dots_pos[x0][y0+1] = placenter->cent_buf[1];
	placenter->plate->dots_pos[x0][y0-1] = placenter->cent_buf[2];
	placenter->plate->dots_pos[x0+1][y0] = placenter->cent_buf[3];
	placenter->plate->dots_pos[x0-1][y0] = placenter->cent_buf[4];
#endif

	return ret;
}

int plateCent_data_init(struct plate_cent_prop *platecenter)
{
	u8 circ_cent_buf[CIRCLE_CENT_BSZ] = {1,1,1,1,1};
	int ret = 0;

	if(platecenter == NULL)
		return -EFAULT;

	platecenter->plate = &plate;

	platecenter->plaCenter->cx = &platecenter->plate->x0;
	platecenter->plaCenter->cy = &platecenter->plate->y0;

	platecenter->cent_buf = circ_cent_buf;

	platecenter->active = TRUE;

	plate_cent_data_to_panel(platecenter);

	return ret;
}

int bresenham_circle_plate(struct clk_plate_prop *clkPlate)
{
	u8 x,y,i = 0;
	int d;
	char ret = 0;

	x = 0;
	y = clkPlate->r;
	d = 3 - 2 * clkPlate->r;

	//clkPlate->dots_pos[0]->x = x;
	//clkPlate->dots_pos[0]->y = y;

	clkPlate->dots_pos->x = x;
	clkPlate->dots_pos->y = y;

	//draw_dots_arc_x8(x0, y0, x, y, r);

	while(x<y) {
		if(++i > CLK_PLATE_8thARC_SIZE)
			return -EFAULT;

		if(d < 0) {
			d = d + 4 * x + 3;
		} else {
			d = d + 4 * (x - y) + 10;	//离P2近, 下一个点Y值减1
			y--;
		}
		x++;

		//plate.dots_pos[i]->x = x;
		//plate.dots_pos[i]->y = y;

		(plate.dots_pos + i)->x = x;
		(plate.dots_pos + i)->y = y;

		//draw_dots_arc_x8(x0, y0, x, y, r);
	}

	//plate.dots_pos[i].x = '\0';
	//plate.dots_pos[i].y = '\0';

	clkPlate->arc_bufsize = i;

	return ret;
}

int plate_data_init(struct clk_plate_prop *clkPlate)
{
	struct dot_pos plate_dotsPos_buf[CLK_PLATE_8thARC_SIZE];	/* 结构体数组*/
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

	//clkPlate->dots_pos   = &plate_dotsPos_buf;	/* 1维结构体数组名还是代表数组首地址*/
	clkPlate->dots_pos   = plate_dotsPos_buf;
	clkPlate->arc_bufsize = sizeof(plate_dotsPos_buf);	/* default size */

	clkPlate->active     = TRUE;

	size = bresenham_circle_plate(clkPlate);
	if(! size)
		return -EFAULT;

	ret = arc_data_to_panel(clkPlate, clkPlate->panel);

	return ret;
}

int plate_scale_data_init(struct clk_scale_prop *clkscale)
{
	int ret = 0;

	if(clkscale == NULL)
		return -EFAULT;

																																																																																																										

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

int hour_hand_data_init(struct clk_hands_prop *hourhand)
{
	u8 i, *tmp = NULL;
	struct dot_pos hour_dotspos[CLK_HHAND_LEN * CLK_HHAND_WID];

	char ret = 0;

	if(hourhand == NULL)
		return -EFAULT;

	tmp = (u8 *)hour_dotspos;
	for(i=0; i<sizeof(hour_dotspos);i++) {
		*(tmp + i) = 0;
	}

	hourhand->plate    = &plate;

	hourhand->x0       = hourhand->plate->x0;
	hourhand->y0       = hourhand->plate->y0;

	hourhand->handLen  = CLK_HHAND_LEN;
	hourhand->handwide = CLK_HHAND_WID;

	hourhand->Htimer   = &hourhand->plate->timer->hour;
	hourhand->angle    = 0;

	hourhand->dots_pos = hour_dotspos;

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

int minute_hand_data_init(struct clk_hands_prop *minuhand)
{
	char ret = 0;

	if(minuhand == NULL)
		return -EFAULT;

	return ret;
}

int second_hand_data_init(struct clk_hands_prop *sechand, struct timer_digital *dtimer)
{
	struct dot_pos sec_dotspos_buf[CLK_SHAND_LEN * CLK_SHAND_WID];
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
	sechand->dotPos_buf_size = sizeof(sec_dotspos_buf);
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

	hour_hand_data_init(&hourHand);

	minute_hand_data_init(&minuHand);

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
