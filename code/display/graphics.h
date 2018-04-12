#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

typedef enum {
  FALSE = 0,
  TRUE  = !FALSE,
} bool;


/*
        \   |    /
         \8 |1/
          \ | /
       7   \|/ 2
      ------+-------
       6   /|\  3
          / |  \
         /5 |4  \

*/


/*
  * arc pixel buffer size:  2*pi*r / 8 = 0.785r
  *    r_max = 64 / 2 = 32,  arc length = 25.12
  */
#define CLK_PLATE_8thARC_SIZE        26
#define CLK_POINTER_EXTRA            4


#define CLK_SEC_HAND_BSIZE           30
#define CKL_SEC_HAND_BEXTRA          4


#define PANLE_CENT_X                 32
#define PANLE_CENT_Y                 32
#define PANLE_HORIZONTAL             64
#define PANLE_VERTICAL               64

#define PLATE_X0_POS                 32
#define PLATE_Y0_POS                 32
#define PLATE_R                      30

#define PLATE_MARGIN_X               2
#define PLATE_MARGIN_Y               2

#define CIRCLE_CENT_BSZ              5  /* 中心用5个点表示,如何摆放5点要代码中实现*/
#define CIRCLE_CENT_X                32
#define CIRCLE_CENT_Y                32

#define CLK_HHAND_LEN                20   //15
#define CLK_HHAND_WID                3
#define CLK_MHAND_LEN                23   //22
#define CLK_MHAND_WID                2
#define CLK_SHAND_LEN                26
#define CLK_SHAND_WID                1

#define SCALE_LEN                    2  /* 非0/3/6/9要在此基础上-1，或者充0 */
#define SCALE_8thPART_SIZE           CLK_PLATE_8thARC_SIZE
#define SCALE_R                      PLATE_R

#define ENDPOINT_FOR_R30	     16


//const char clk_plate_eighth_buf[CLK_PLATE_8thARC_SIZE +  CLK_POINTER_EXTRA] = {0};

//const char clk_sec_hand_buf[CLK_SEC_HAND_BSIZE + CKL_SEC_HAND_BEXTRA] = {0};

struct dot_pos {
	u8 x;
	u8 y;
};
/*
  * panle 和plate 有什么区别???
  * panle设计为一个矩形区域,plate是时钟表盘,放在panle上
  */
struct clk_plate_prop {
	u8 x0;		/* origin x */
	u8 y0;
	u8 r;

	u8 margin_x;	// gap between plate edge and panle edge
	u8 margin_y;

	struct timer_digital *timer;

	struct dot_pos *dots_pos;	/* 用于往plate表盘上投dot,所以放在plate上*/
	u8 arc_bufsize;

	struct clk_hands_prop  *hourhand;
	struct clk_hands_prop  *minuhand;
	struct clk_hands_prop  *sechand;

	struct plate_cent_prop *platcenter;
	struct clk_scale_prop  *clkscale;

	struct clk_panle_prop  *panle;

	struct dot_pos         *endpoint;   /* used for scale, hourhand, minuhand, sechand */

	bool active;
};

struct timer_digital {
	u8 year;
	u8 month;
	u8 day;

	u8 hour;
	u8 minute;
	u8 second;

	struct clk_plate_prop *plate;
	bool active;
};

struct clk_hands_prop {
	u8 x0;
	u8 y0;
	int angle;

	u8 handLen;
	u8 handwide;

	u8 *Htimer;			/*什么作用????*/

	struct dot_pos *dots_pos;

	struct clk_plate_prop *plate;

	bool active;
};

struct clk_coordinate {
	u8 *xCoor;
	u8 *yCoor;
};

struct clk_scale_prop {
	u8  size;	/*一个刻度线有多长*/
	u8  len;

	int angle;

	int r;		/*半径,用于存放当前scal坐在直线中的值*/
	struct clk_coordinate clk_coor;
	struct clk_plate_prop *plate;

	//struct dot_pos *scaleBuf[SCALE_8thPART_SIZE];
	//struct dot_pos (*scaleBuf)[SCALE_8thPART_SIZE];
	struct dot_pos (*scaleBuf)[SCALE_LEN];   /*数组指针,该指针指向一个数组(不是一个数),每个数组元素是一个struct dot_pos的变量
												scaleBuf+1指向哪里?*/

	bool active;
};

struct icon_center {
	u8 *cx;
	u8 *cy;
};

struct plate_cent_prop {
	u8 *cent_buf;
	struct icon_center *plaCenter;

	struct clk_plate_prop *plate;
	bool active;
};

struct clk_panle_prop {
	//struct clk_coordinate *clkCenter;

	u8 height;
	u8 width;

	struct icon_center   *panCenter;

	//struct clk_scale_prop clkScale;

	struct clk_hands_prop *hourHand;
	struct clk_hands_prop *minuHand;
	struct clk_hands_prop *secHand;

	//struct panle_dots_buf *dots_buf;
	//struct panle_dots_buf **dots_buf;
	//u8 **dots_buf;
	u8 *dots_buf;
	// dots_bhsize;		/* buf height size */  /* can be placedwith height & width above */
	// dots_bwsize;		/* buf width  size */

	struct clk_plate_prop *plate;

	bool active;

};

//#define TRUE                      1
//#define FALSE                     0

#define EFAULT                    1
#define EINVAL                    2


#define MAP_AREA_1(x, y)    ( x, y)	// 第一象限上半部分
#define MAP_AREA_2(x, y)    ( y, x)	// 第一象限下半部分
#define MAP_AREA_3(x, y)    ( y,-x)
#define MAP_AREA_4(x, y)    ( x,-y)
#define MAP_AREA_5(x, y)    (-x,-y)
#define MAP_AREA_6(x, y)    (-y,-x)
#define MAP_AREA_7(x, y)    (-y, x)
#define MAP_AREA_8(x, y)    (-x, y)

#ifndef NULL
#define NULL ((void *)0)
#endif

enum clkElement {
	CLK_RESERVED   = 0,
	CLK_CENTER,
	CLK_PLATE,
	CLK_TIME_SCALE,
	CLK_HOUR_HAND,
	CLK_MINT_HAND,
	CLK_SECD_HAND,
};


enum LINE_TYPE {
	LINE_NULL = 0,
	LINE_SCALE,
	LINE_SECOND,
	LINE_MINUTE,
	LIEN_HOUR,
};

struct angle_xy_table {
	int angle;
	u8 x;
	u8 y;
};

#endif
