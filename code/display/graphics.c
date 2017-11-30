#include <stm32f10x.h>		// u8
#include "graphics.h"

struct clk_plate_prop plate;
struct clk_panle_prop panle;

struct clk_hands_prop hourHand;
struct clk_hands_prop minuHand;
struct clk_hands_prop secHand;


struct plate_cent_prop plateCenter;
struct clk_scale_prop  clkScale;

struct timer_digital   digTimer;


static const struct angle_xy_table angle_xy_r26_tab[] = {
 /*度数 cos:x  sin:y (r=26) */

    { 0,   26,  0  },
    { 1,   26,  0  },
    { 2,   26,  1  },
    { 3,   26,  1  },
    { 4,   26,  2  },
    { 5,   26,  2  },
    { 6,   26,  3  },
    { 7,   26,  3  },
    { 8,   26,  4  },
    { 9,   26,  4  },
    { 10,  26,  5  },
    { 11,  26,  5  },
    { 12,  25,  5  },
    { 13,  25,  6  },
    { 14,  25,  6  },
    { 15,  25,  7  },
    { 16,  25,  7  },
    { 17,  25,  8  },
    { 18,  25,  8  },
    { 19,  25,  8  },
    { 20,  24,  9  },
    { 21,  24,  9  },
    { 22,  24,  10 },
    { 23,  24,  10 },
    { 24,  24,  11 },
    { 25,  24,  11 },
    { 26,  23,  11 },
    { 27,  23,  12 },
    { 28,  23,  12 },
    { 29,  23,  13 },
    { 30,  23,  13 },
    { 31,  22,  13 },
    { 32,  22,  14 },
    { 33,  22,  14 },
    { 34,  22,  15 },
    { 35,  21,  15 },
    { 36,  21,  15 },
    { 37,  21,  16 },
    { 38,  20,  16 },
    { 39,  20,  16 },
    { 40,  20,  17 },
    { 41,  20,  17 },
    { 42,  19,  17 },
    { 43,  19,  18 },
    { 44,  19,  18 },
    { 45,  18,  18 },
    { 46,  18,  19 },
    { 47,  18,  19 },
    { 48,  17,  19 },
    { 49,  17,  20 },
    { 50,  17,  20 },
    { 51,  16,  20 },
    { 52,  16,  20 },
    { 53,  16,  21 },
    { 54,  15,  21 },
    { 55,  15,  21 },
    { 56,  15,  22 },
    { 57,  14,  22 },
    { 58,  14,  22 },
    { 59,  13,  22 },
    { 60,  13,  23 },
    { 61,  13,  23 },
    { 62,  12,  23 },
    { 63,  12,  23 },
    { 64,  11,  23 },
    { 65,  11,  24 },
    { 66,  11,  24 },
    { 67,  10,  24 },
    { 68,  10,  24 },
    { 69,   9,  24 },
    { 70,   9,  24 },
    { 71,   8,  25 },
    { 72,   8,  25 },
    { 73,   8,  25 },
    { 74,   7,  25 },
    { 75,   7,  25 },
    { 76,   6,  25 },
    { 77,   6,  25 },
    { 78,   5,  25 },
    { 79,   5,  26 },
    { 80,   5,  26 },
    { 81,   4,  26 },
    { 82,   4,  26 },
    { 83,   3,  26 },
    { 84,   3,  26 },
    { 85,   2,  26 },
    { 86,   2,  26 },
    { 87,   1,  26 },
    { 88,   1,  26 },
    { 89,   0,  26 },
    { 90,   0,  26 },
};

static const struct angle_xy_table angle_xy_r23_tab[] = {
 /*度数 cos:x  sin:y (r=23) */

    { 0,   23,  0  },
    { 1,   23,  0  },
    { 2,   23,  1  },
    { 3,   23,  1  },
    { 4,   23,  2  },
    { 5,   23,  2  },
    { 6,   23,  2  },
    { 7,   23,  3  },
    { 8,   23,  3  },
    { 9,   23,  4  },
    { 10,  23,  4  },
    { 11,  23,  4  },
    { 12,  22,  5  },
    { 13,  22,  5  },
    { 14,  22,  6  },
    { 16,  22,  6  },
    { 17,  22,  7  },
    { 18,  22,  7  },
    { 19,  22,  7  },
    { 20,  22,  8  },
    { 21,  21,  8  },
    { 22,  21,  9  },
    { 23,  21,  9  },
    { 24,  21,  9  },
    { 25,  21,  10 },
    { 26,  21,  10 },
    { 27,  20,  10 },
    { 28,  20,  11 },
    { 29,  20,  11 },
    { 30,  20,  11 },
    { 31,  20,  12 },
    { 32,  20,  12 },
    { 33,  19,  13 },
    { 34,  19,  13 },
    { 35,  19,  13 },
    { 36,  19,  14 },
    { 37,  18,  14 },
    { 38,  18,  14 },
    { 39,  18,  14 },
    { 40,  18,  15 },
    { 41,  17,  15 },
    { 42,  17,  15 },
    { 43,  17,  16 },
    { 44,  17,  16 },
    { 45,  16,  16 },
    { 46,  16,  17 },
    { 47,  16,  17 },
    { 48,  15,  17 },
    { 49,  15,  17 },
    { 50,  15,  18 },
    { 51,  14,  18 },
    { 52,  14,  18 },
    { 53,  14,  18 },
    { 54,  14,  19 },
    { 55,  13,  19 },
    { 56,  13,  19 },
    { 57,  13,  19 },
    { 58,  12,  20 },
    { 59,  12,  20 },
    { 60,  12,  20 },
    { 61,  11,  20 },
    { 62,  11,  20 },
    { 63,  10,  20 },
    { 64,  10,  21 },
    { 65,  10,  21 },
    { 66,  9,   21 },
    { 67,  9,   21 },
    { 68,  9,   21 },
    { 69,  8,   21 },
    { 70,  8,   22 },
    { 71,  7,   22 },
    { 72,  7,   22 },
    { 73,  7,   22 },
    { 74,  6,   22 },
    { 75,  6,   22 },
    { 76,  6,   22 },
    { 77,  5,   22 },
    { 78,  5,   22 },
    { 79,  4,   23 },
    { 80,  4,   23 },
    { 81,  4,   23 },
    { 82,  3,   23 },
    { 83,  3,   23 },
    { 84,  2,   23 },
    { 85,  2,   23 },
    { 86,  2,   23 },
    { 87,  1,   23 },
    { 88,  1,   23 },
    { 89,  0,   23 },
    { 90,  0,   23 },
};

static const struct angle_xy_table angle_xy_r20_tab[] = {
 /*度数 cos:x  sin:y (r=20) */

     { 0,   20,  0 },
     { 1,   20,  0 },
     { 2,   20,  1 },
     { 3,   20,  1 },
     { 4,   20,  1 },
     { 5,   20,  2 },
     { 6,   20,  2 },
     { 7,   20,  2 },
     { 8,   20,  3 },
     { 9,   20,  3 },
     { 10,  20,  3 },
     { 11,  20,  4 },
     { 12,  20,  4 },
     { 13,  19,  4 },
     { 14,  19,  5 },
     { 15,  19,  5 },
     { 16,  19,  6 },
     { 17,  19,  6 },
     { 18,  19,  6 },
     { 19,  19,  7 },
     { 20,  19,  7 },
     { 21,  19,  7 },
     { 22,  19,  7 },
     { 23,  18,  8 },
     { 24,  18,  8 },
     { 25,  18,  8 },
     { 26,  18,  9 },
     { 27,  18,  9 },
     { 28,  18,  9 },
     { 29,  17,  10 },
     { 30,  17,  10 },
     { 31,  17,  10 },
     { 32,  17,  11 },
     { 33,  17,  11 },
     { 34,  17,  11 },
     { 35,  16,  11 },
     { 36,  16,  12 },
     { 37,  16,  12 },
     { 38,  16,  12 },
     { 39,  16,  13 },
     { 40,  15,  13 },
     { 41,  15,  13 },
     { 42,  15,  13 },
     { 43,  15,  14 },
     { 44,  14,  14 },
     { 45,  14,  14 },
     { 46,  14,  14 },
     { 47,  14,  15 },
     { 48,  13,  15 },
     { 49,  13,  15 },
     { 50,  13,  15 },
     { 51,  13,  16 },
     { 52,  12,  16 },
     { 53,  12,  16 },
     { 54,  12,  16 },
     { 55,  11,  16 },
     { 56,  11,  17 },
     { 57,  11,  17 },
     { 58,  11,  17 },
     { 59,  10,  17 },
     { 60,  10,  17 },
     { 61,  10,  17 },
     { 62,  9,	 18 },
     { 63,  9,	 18 },
     { 64,  9,	 18 },
     { 65,  8,	 18 },
     { 66,  8,	 18 },
     { 67,  8,	 18 },
     { 68,  7,	 19 },
     { 69,  7,	 19 },
     { 70,  7,	 19 },
     { 71,  7,	 19 },
     { 72,  6,	 19 },
     { 73,  6,	 19 },
     { 74,  6,	 19 },
     { 75,  5,	 19 },
     { 76,  5,	 19 },
     { 77,  4,	 19 },
     { 78,  4,	 20 },
     { 79,  4,	 20 },
     { 80,  3,	 20 },
     { 81,  3,	 20 },
     { 82,  3,	 20 },
     { 83,  2,	 20 },
     { 84,  2,	 20 },
     { 85,  2,	 20 },
     { 86,  1,	 20 },
     { 87,  1,	 20 },
     { 88,  1,	 20 },
     { 89,  0,	 20 },
     { 90,  0,	 20 },
};

u8 draw_dots_arc_x8(u8 x0, u8 y0, u8 x, u8 y, u8 r)
{
	char x1, y1;
	u8 ret=0;	
}

u8 get_slope_from_table_x1000(u16 d)
{
	

}

char arc_dot_data_to_panle(struct clk_plate_prop *clkPlate, u8 n, struct clk_panle_prop *clkPanle)
{
	u8 X,Y,x,y,i,tmp;		// X & Y is belong to panle axes, x0 & y0 are belong too
	char ret = 0;

	if((clkPlate == NULL) || (n > clkPlate->r))
		return -EINVAL;

	/*
	 * 给出一个点，要map到panle上去
	 * 这些arc点都在第一象限45度上半部分
	 * 现已将坐标轴上4个象限从新按顺时钟方向分成8个区域
	 * area1 的(x,y)映射到panle的其他area上可通过MAP_AREA_x宏来实现
	 * 上面是点(x,y)的表示,下面难点是如何标示panle里数组的下标?
	 * panle成员panle_buf最大是128*64个字节
	 * 表盘上(x,y)如何转换到panle_buf上(x',y')上去?
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
	x = clkPlate->dots_pos[n]->x;
	y = clkPlate->dots_pos[n]->y;

	X = clkPlate->x0 + x;  // contain x=0, x0 contain margin
	Y = clkPlate->y0 - y;

	if((X > clkPanle->height) || (Y > clkPanle->width)) {
		return -EFAULT;
	}
	clkPanle->dots_buf[ X][ Y] = 1; // 第一象限上半部分


	/* 第1象限下半部分 : (x,y)->(y,x), '+' */
	x = clkPlate->dots_pos[n]->y;
	y = clkPlate->dots_pos[n]->x;

	X = clkPlate->x0 + x;
	Y = clkPlate->y0 - y;

	if((X > clkPanle->height) || (Y > clkPanle->width)) {
		return -EFAULT;
	}
	clkPanle->dots_buf[ X][ Y] = 1;


	/* 第2象限上半部分 */
	x = -clkPlate->dots_pos[n]->x;
	y =  clkPlate->dots_pos[n]->y;

	X = clkPlate->x0 - x;
	Y = clkPlate->y0 - y;

	if((X > clkPanle->height) || (Y > clkPanle->width)) {
		return -EFAULT;
	}
	clkPanle->dots_buf[ X][ Y] = 1;

	/* 第2象限下半部分 */
	x = -clkPlate->dots_pos[n]->y;
	y =  clkPlate->dots_pos[n]->x;

	X = clkPlate->x0 - x;
	Y = clkPlate->y0 - y;

	if((X > clkPanle->height) || (Y > clkPanle->width)) {
		return -EFAULT;
	}
	clkPanle->dots_buf[ X][ Y] = 1;

	/* 第3象限上半部分 */
	x = -clkPlate->dots_pos[n]->y;
	y = -clkPlate->dots_pos[n]->x;

	X = clkPlate->x0 - x;
	Y = clkPlate->y0 + y;

	if((X > clkPanle->height) || (Y > clkPanle->width)) {
		return -EFAULT;
	}
	clkPanle->dots_buf[ X][ Y] = 1;

	/* 第3象限下半部分 */
	x = -clkPlate->dots_pos[n]->x;
	y = -clkPlate->dots_pos[n]->y;

	X = clkPlate->x0 - x;
	Y = clkPlate->y0 + y;

	if((X > clkPanle->height) || (Y > clkPanle->width)) {
		return -EFAULT;
	}
	clkPanle->dots_buf[ X][ Y] = 1;

	/* 第4象限上半部分 */
	x =  clkPlate->dots_pos[n]->x;
	y = -clkPlate->dots_pos[n]->y;

	X = clkPlate->x0 + x;
	Y = clkPlate->y0 + y;

	if((X > clkPanle->height) || (Y > clkPanle->width)) {
		return -EFAULT;
	}
	clkPanle->dots_buf[ X][ Y] = 1;

	/* 第4象限下半部分 */
	x =  clkPlate->dots_pos[n]->y;
	y = -clkPlate->dots_pos[n]->x;

	X = clkPlate->x0 + x;
	Y = clkPlate->y0 + y;

	if((X > clkPanle->height) || (Y > clkPanle->width)) {
		return -EFAULT;
	}
	clkPanle->dots_buf[ X][ Y] = 1;

	return ret;

}

char arc_data_to_panle(struct clk_plate_prop *clkPlate, struct clk_panle_prop *clkPanle)
{
	char x,y;
	u8 i;
	char ret=0;

	if((clkPlate == NULL) || (clkPanle == NULL) || (clkPlate->active != TRUE))
		return -EINVAL;

	for(i=0; i<clkPlate->arc_size; i++) {
		arc_dot_data_to_panle(clkPlate, i, clkPanle);
	}

	return ret;
}


u8 beeline_show(u8 x0, u8 y0, u16 angle)
{
	u8 x,y,d,k;	/* slope = 0~1 */

	x = 0;
	y = 0;
	k = get_slope_from_table_x1000(angle);
	d = 1 - 2*k;

	

	show_dots_beeline(x0, y0, x, y, r);



}

char timer_digit_data_init(struct timer_digital *digTime)
{
	char ret =0;

	if(digTime == NULL)
		return -EFAULT;

	digTime->plate  = &plate;

	digTime->year   = 0;
	digTime->month  = 0;
	digTime->day    = 0;

	digTime->hour   = 0;
	digTime->minute = 0;
	digTime->second = 0;

	return ret;
}


char panel_data_init(struct clk_panle_prop *clkPanle)
{
	u8 panel_dotsPos[PANLE_HORIZONTAL][PANLE_VERTICAL] = {0};
	u8 cent_x, cent_y;
	char ret = 0;

	if(clkPanle == NULL)
		return -EFAULT;

	/* 结构体指针中的实体变量也是虚的?*/
	/* 那结构体指针中的变量是用指针变量还是普通变量?*/

	clkPanle->dots_buf  = panel_dotsPos;

	clkPanle->plate     = &plate;
	//clkPanle->clkScale  = &clkScale;

	//clkPanle->hourHand  = &hourHand;
	//clkPanle->minuHand  = &minuHand;
	//clkPanle->secHand   = &secHand;

	cent_x = PANLE_CENT_X;
	cent_y = PANLE_CENT_Y;

	clkPanle->panCenter->cx = &cent_x;
	clkPanle->panCenter->cy = &cent_y;

	clkPanle->height    = PANLE_HORIZONTAL;
	clkPanle->width     = PANLE_VERTICAL;

	clkPanle->active    = TRUE;
	
	return ret;
}

char plate_cent_data_to_panle(struct plate_cent_prop *placenter)
{
	u8 x0,y0;
	char ret = 0;

	if((placenter == NULL) || (placenter->plate == NULL) || (platecenter->active != TRUE))
		return -EFAULT;

	x0 = *placenter->plaCenter->cx;
	y0 = *placenter->plaCenter->cy;

	placenter->plate->dots_buf[x0][y0]   = placenter->cent_buf[0];
	placenter->plate->dots_buf[x0][y0+1] = placenter->cent_buf[1];
	placenter->plate->dots_buf[x0][y0-1] = placenter->cent_buf[2];
	placenter->plate->dots_buf[x0+1][y0] = placenter->cent_buf[3];
	placenter->plate->dots_buf[x0-1][y0] = placenter->cent_buf[4];

	return ret;
}

char plateCent_data_init(struct plate_cent_prop *platecenter)
{
	u8 circ_cent_buf[CIRCLE_CENT_BSZ] = {1,1,1,1,1};
	char ret = 0;

	if(platecenter == NULL))
		return -EFAULT;

	platecenter->plate = &plate;

	platecenter->plaCenter->cx = platecenter->plate->x0;
	platecenter->plaCenter->cy = platecenter->plate->y0;

	platecenter->cent_buf = circ_cent_buf;

	platecenter->active = TRUE;

	plate_cent_data_to_panle(platecenter);

	return ret;
}

char bresenham_circle_plate(struct clk_plate_prop *clkPlate)
{
	u8 x,y,i = 0;
	int d;
	char ret = 0;

	x = 0;
	y = r;
	d = 3 - 2 * r;

	clkPlate.dots_pos[0].x = x;
	clkPlate.dots_pos[0].y = y;

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

		plate.dots_pos[i].x = x;
		plate.dots_pos[i].y = y;

		//draw_dots_arc_x8(x0, y0, x, y, r);
	}

	//plate.dots_pos[i].x = '\0';
	//plate.dots_pos[i].y = '\0';

	clkPlate->arc_size = i;

	return ret;
}

char plate_data_init(struct clk_plate_prop *clkPlate)
{
	struct dot_pos plate_dotsPos_buf[CLK_PLATE_8thARC_SIZE];
	u8 i, *tmp = NULL;
	u8 size;
	char ret = 0;

	if(clkPlate == NULL)
		return -EFAULT;

	/* clean plate buffer */
	tmp = (char *)plate_dotsPos_buf;		// &plate_dotsPos_buf
	for(i=0; i<sizeof(plate_dotsPos_buf); i++) {
		*(tmp + i) = 0;
	}

	clkPlate->panle      = &panle;
	clkPlate->timer      = &digTimer;

	clkPlate->hourhand   = &hourHand;
	clkPlate->minuhand   = &minuHand;
	clkPlate->sechand    = &secHand;

	clkPlate->platcenter = &plateCenter;
	clkPlate->clkscale   = &clkScale;

	clkPlate->r          = PLATE_R;
	clkPlate->x0         = *clkPlate->panle->panCenter->cx;
	clkPlate->y0         = *clkPlate->panle->panCenter->cy;

	clkPlate->margin_x   = 0;
	clkPlate->margin_y   = 0;

	clkPlate->dots_pos   = plate_dotsPos_buf;

	clkPlate->active     = TRUE;

	size = bresenham_circle_plate(clkPlate);
	if(! size)
		return -EFAULT;

	ret = arc_data_to_panle(clkPlate, clkPlate->panle);

	return ret;
}

char plate_scale_data_init(struct clk_scale_prop *clkscale)
{
	char ret = 0;

	if(clkscale == NULL))
		return -EFAULT;

																																																																																																										

	return ret;
}

//功能:在(x0,y0)到(x1,y1)之间画一条直线(Bresenham算法)
void draw_bresenham_line(struct dot_pos *dots_pos, u8 x0, u8 y0, u8 x1, u8 y1)
{
	u8 i;
	u8 xerr=0,yerr=0;
	u8 dx,dy;
	u8 distance;
	u8 incx,incy;
	u8 row,col;

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
	else            {   incx = -1;  dx=-dx;   }		//求X增量的绝对值

	if(dy == 0)     {   incy = 0;             }		//水平线
	else if(dy > 0) {   incy = 1;             }
	else            {   incy = -1;  dy=-dy;   }     //求Y增量的绝对值

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

char update_bresenham_line(clkElement type, void *param)
{
	struct clk_hands_prop *hourhand = NULL;
	struct clk_hands_prop *minuhand = NULL;
	struct clk_hands_prop *sechand  = NULL;
	struct clk_scale_prop *sprop   = NULL;
	u8 x0,y0,x1,y1,coorAngle;
	char ret = 0;

	if(param == NULL)
		return -EFAULT;

	switch(type) {
	  case CLK_HOUR_HAND:
	  	hourhand = (struct clk_hands_prop *)param;
		coorAngle = transfer_humAngle_to_axesAngle(hourhand->angle);

		x0 = hourhand->x0;
		y0 = hourhand->y0;

		x1 = angle_xy_r26_tab[coorAngle].x;
		y1 = angle_xy_r26_tab[coorAngle].y;

		/* 时针数据要投到panle上去吗,时针数据是变化的,且要保存就要很大空间
		  * 如果不投到panle上,则要有个功能时时刻刻更新数据到panle上(case1)
		  * 能否时针数据改了后,panel打印的是时针数据,就是说用指针指向时针数据?(case2)
		  * 我还是选case1. case2是零散数据,到后来多个icon会出现多处扫描panle
		  * 对于case1, 先要得到/统计圆心所在坐标系中要打印1的点坐标(x & y)
		  *                      再保存到panle的buf中
		  */

		//BresenhamLine(x0,y0,x1,y1);
		draw_bresenham_line(hourhand->dots_pos, x0,y0,x1,y1);

	  	break;
	  case CLK_MINT_HAND:
	  	minuhand = (struct clk_hands_prop *)param;
		break;
	  case CLK_SECD_HAND:
	  	sechand  = (struct clk_hands_prop *)param;
		break;
	  case CLK_TIME_SCALE:
	  	sprop = (struct clk_hands_prop *)param;
		break;
	}

}

char hourhand_data_to_panle()
{

}

char hour_hand_data_init(struct clk_hands_prop *hourhand)
{
	u8 i, *tmp = NULL;
	struct dot_pos hour_dotspos[CLK_HHAND_LEN * CLK_HHAND_WID];

	char ret = 0;

	if(hourhand == NULL))
		return -EFAULT;

	tmp = (char *)hour_dotspos;
	for(i=0; i<sizeof(hour_dotspos);i++) {
		*(tmp + i) = 0;
	}

	hourhand->plate    = &plate;

	hourhand->x0       = hourhand->plate->x0;
	hourhand->y0       = hourhand->plate->y0;

	hourhand->handLen  = CLK_HHAND_LEN;
	hourhand->handwide = CLK_HHAND_WID;

	hourhand->Htimer   = hourhand->plate->timer->hour;
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

	ret = update_bresenham_line(CLK_HOUR_HAND, (void *)hourhand);
	ret = hourhand_data_to_panle();

	return ret;
}

char minute_hand_data_init(struct clk_hands_prop *minuhand)
{
	char ret = 0;

	if(minuhand == NULL))
		return -EFAULT;

	return ret;
}

char second_hand_data_init(struct clk_hands_prop *sechand)
{
	char ret = 0;

	if(sechand == NULL))
		return -EFAULT;

	return ret;
}

char clk_panle_init(void)
{
	char ret = 0;

	panel_data_init(&panle);

	plate_data_init(&plate);

	plateCent_data_init(&plateCenter);

	timer_digit_data_init(&digTimer);

	plate_scale_data_init(&clkScale);  // panle

	hour_hand_data_init(&hourHand);

	minute_hand_data_init(&minuHand);

	second_hand_data_init(&secHand);

	return ret;
}

char draw_panle_graphics(void)
{
	u8 i,j;
	char ret = 0;

	for(i=0; i<panle->width; i++) {
		for(j=0; j<panle->height; j++) {
			//draw_dots(panle->dots_buf[i][j]);
		}
	}
}

/*
  * interrupt. update clock second/minute/hour hand date
  * 定时1s时间到,则secHand中计数增1，角度增1
  * 
*/
