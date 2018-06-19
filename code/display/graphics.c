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
	sechand->Htimer   = &sechand->plate->timer->hour;  /* Htimer ����?*/
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
  * ��ʱ1sʱ�䵽,��secHand�м�����1���Ƕ���1
  * 
*/
