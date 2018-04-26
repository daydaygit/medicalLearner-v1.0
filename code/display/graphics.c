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


static const struct angle_xy_table angle_xy_r26_tab[] = {
 /*���� cos:x  sin:y (r=26) */

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
 /*���� cos:x  sin:y (r=23) */

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
 /*���� cos:x  sin:y (r=20) */

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

int arc_dot_data_to_panel(struct clk_plate_prop *clkPlate, u8 n, struct clk_panel_prop *clkPanel)
{
	u8 X,Y,x,y;		// X & Y is belong to panel axes, x0 & y0 are belong too
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
	 * (x,y)��(x0,y0)����ϵ�ĵ�1����,��x'=x0+x, y'=y0-y;  ��ʱxx0,yy0
	 *                                                 2           ,��x'=x0-x, y'=y0-y;  ��ʱxx0,yy0
	 *                                                 3           ,��x'=x0-x, y'=y0+y;  ��ʱxx0,yy0
	 *                                                 4           ,��x'=x0+x, y'=y0+y;  ��ʱxx0,yy0
	 *                                  x����������,��x'=x0+x, y'=0    ;  ��ʱxx0,yy0
	 *                                  x�Ḻ������,��x'=x0-x, y'=0     ;  ��ʱxx0,yy0
	 *                                  y����������,��x'=x0,     y'=y0-y;  ��ʱxx0,yy0
	 *                                  y�Ḻ������,��x'=x0,     y'=y0+y;  ��ʱxx0,yy0
	 */


	/* the 8th arc is in the top half of the 1st quadrant */
	/* ��1�����ϰ벿�� */
	//x = clkPlate->dots_pos[n]->x;   /* ָ���ָ������*/
	//y = clkPlate->dots_pos[n]->y;

	//x = clkPlate->dots_pos[n]->x;   /* ָ���ָ������*/
	//y = clkPlate->dots_pos[n]->y;

	/* clkPlate->dots_pos�Ѿ�ָ��1ά�ṹ�������׵�ַ(�׸�Ԫ�ص�ַ)*/
	/* clkPlate->dots_pos + 1ָ��1ά�ṹ������ڶ���Ԫ�ص�ַ*/
	x = (clkPlate->dots_pos + n)->x;
	y = (clkPlate->dots_pos + n)->y;

	X = clkPlate->x0 + x;  // contain x=0, x0 contain margin
	Y = clkPlate->y0 - y;

	if((X > clkPanel->height) || (Y > clkPanel->width)) {
		return -EFAULT;
	}
	//clkPanel->dots_buf[ X][ Y] = 1; // ��һ�����ϰ벿��

	/* clkPanel->dots_bufָ���0���׵�ַ */
	/* (x) clkPanel->dots_buf + 1ָ���1���׵�ַ */
	/* (x) *(clkPanel->dots_buf + 1)ָ���1�е�0��Ԫ�ص�ַ,��&(clkPanel->dots_buf + 1) */
	/* (x) *(clkPanel->dots_buf + 1) + 2 ָ���1�е�2��Ԫ�ص�ַ */
	/* (x) *(*(clkPanel->dots_buf + 1) + 2 )ָ���1�е�2��Ԫ�ص�ֵ */

	/* clkPanel->dots_bufָ���0���׵�ַ */
	/* clkPanel->dots_buf��u8 *���͵�, +1����ָ���ά������һ��*/
	*(clkPanel->dots_buf + X * clkPanel->width + Y) = 1;  /* �Ƿ�Ӧ��dots_bufָ���0�е�0��Ԫ�ص�ַ����? */


	/* ��1�����°벿�� : (x,y)->(y,x), '+' */
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


	/* ��2�����ϰ벿�� */
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

	/* ��2�����°벿�� */
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

	/* ��3�����ϰ벿�� */
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

	/* ��3�����°벿�� */
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

	/* ��4�����ϰ벿�� */
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

	/* ��4�����°벿�� */
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

	digTime->year   = 0;
	digTime->month  = 0;
	digTime->day    = 0;

	digTime->hour   = 0;
	digTime->minute = 0;
	digTime->second = 0;

	return ret;
}


int panel_data_init(struct clk_panel_prop *clkPanel)
{
	u8 panel_dotsPos[PANEL_HORIZONTAL][PANEL_VERTICAL] = {0};
	u8 cent_x, cent_y;
	char ret = 0;

	if(clkPanel == NULL)
		return -EFAULT;

	/* �ṹ��ָ���е�ʵ�����Ҳ�����?*/
	/* �ǽṹ��ָ���еı�������ָ�����������ͨ����?*/

	//clkPanel->dots_buf  = panel_dotsPos;

	//clkPanel->dots_buf  = &panel_dotsPos[0][0];
	//clkPanel->dots_buf  = *(panel_dotsPos+0);
	//clkPanel->dots_buf  = panel_dotsPos[0];
	clkPanel->dots_buf  = *panel_dotsPos;		/* ��0�е�0�е�ַ*/

	//clkPanel->dots_buf	= panel_dotsPos;			/* ��0���׵�ַ*/  /* �������,��������ʹ�õط�����*/

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
			d = d + 4 * (x - y) + 10;	//��P2��, ��һ����Yֵ��1
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
	struct dot_pos plate_dotsPos_buf[CLK_PLATE_8thARC_SIZE];	/* �ṹ������*/
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

	//clkPlate->dots_pos   = &plate_dotsPos_buf;	/* 1ά�ṹ�����������Ǵ��������׵�ַ*/
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

int update_bresenham_line(enum clkElement type, void *param)
{
	struct clk_hands_prop *hourhand = NULL;
	struct clk_hands_prop *minuhand = NULL;
	struct clk_hands_prop *sechand  = NULL;
	struct clk_scale_prop *platscal   = NULL;
	u8 x0,y0,x1,y1,coorAngle;
	int ret = 0;

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

		/* ʱ������ҪͶ��panel��ȥ��,ʱ�������Ǳ仯��,��Ҫ�����Ҫ�ܴ�ռ�
		  * �����Ͷ��panel��,��Ҫ�и�����ʱʱ�̸̿������ݵ�panel��(case1)
		  * �ܷ�ʱ�����ݸ��˺�,panel��ӡ����ʱ������,����˵��ָ��ָ��ʱ������?(case2)
		  * �һ���ѡcase1. case2����ɢ����,���������icon����ֶദɨ��panel
		  * ����case1, ��Ҫ�õ�/ͳ��Բ����������ϵ��Ҫ��ӡ1�ĵ�����(x & y)
		  *                      �ٱ��浽panel��buf��
		  */

		//BresenhamLine(x0,y0,x1,y1);
		draw_bresenham_line(hourhand->dots_pos, x0,y0,x1,y1);

	  	break;
	  case CLK_MINT_HAND:
	  	minuhand = (struct clk_hands_prop *)param;
		coorAngle = transfer_humAngle_to_axesAngle(minuhand->angle);
		break;
	  case CLK_SECD_HAND:
	  	sechand  = (struct clk_hands_prop *)param;
		coorAngle = transfer_humAngle_to_axesAngle(sechand->angle);
		break;
	  case CLK_TIME_SCALE:
		platscal = (struct clk_scale_prop *)param;
		coorAngle = transfer_humAngle_to_axesAngle(platscal->angle);
		break;
	}

  return ret;
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

	/* 0. Ҫ��bresenham�㷨��ֱ��,��Ҫ֪����ʼ��& �յ�
	  * 1. ��λ�ȡʱ����յ�����
	  * 2. ��֪������(0,0) & handlen & angle
	  * 3. ��ֱ����ô�ͻ�Բ�Ǳ�ĩ����
	  * 4. angle�൱��б��k
	  * 5. �յ�����Ӧ�úõõ���
	  * 6. ---- angle is changing ----
	  * 7. ��ʱ�������ظ�����,�������鱣��,������һ���볤30��,���360��xy����Ҫ30*2*360=2w1
	  */

	ret = update_bresenham_line(CLK_HOUR_HAND, (void *)hourhand);
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

int second_hand_data_init(struct clk_hands_prop *sechand)
{
	char ret = 0;

	if(sechand == NULL)
		return -EFAULT;

	return ret;
}

char clk_panel_init(void)
{
	char ret = 0;

	panel_data_init(&panel);

	plate_data_init(&plate);

	plateCent_data_init(&plateCenter);

	timer_digit_data_init(&digTimer);

	plate_scale_data_init(&clkScale);  // panel

	hour_hand_data_init(&hourHand);

	minute_hand_data_init(&minuHand);

	second_hand_data_init(&secHand);

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
