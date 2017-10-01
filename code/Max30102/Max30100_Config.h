#ifndef _MAX_30100_CONFIG_H
#define _MAX_30100_CONFIG_H

#define MAX30100_FIFO_LENGTH 32

#define REG_INT_STATUS0_ADDR 0x00     //中断状态1
#define REG_INT_STATUS1_ADDR 0x01     //中断状态2
#define REG_INT_ENALE1_ADDR  0x02     //中断设置1
#define REG_INT_ENALE2_ADDR  0x03     //中断设置2
#define REG_FIFO_WR_PTR_ADDR 0x04     //FIFO 写地址寄存器
#define REG_FIFO_OV_COUNTER_ADDR 0x05 //溢出数据统计寄存器
#define REG_FIFO_RD_PTR_ADDR 0x06     //FIFO 读地址寄存器
#define REG_FIFO_DATA_ADDR 0x07       //FIFO 数据地址


#define REG_FIFO_CONFIG_ADDR 0x08    //FIFO配置寄存器
#define REG_MODE_CONFIG_ADDR 0x09    //模式配置寄存器
#define REG_SPO2_CONFIG_ADDR 0x0a    //血氧配置寄存器
#define REG_LED1_PA_ADDR 0x0C    //led1 脉冲放大（电流）设置
#define REG_LED2_PA_ADDR 0x0D    //led2 脉冲放大（电流）设置
#define REG_PILOT_PA_ADDR 0x10   //接近脉冲设置
#define REG_MULTI_LED_CONFIG1_ADDR 0x11
#define REG_MULTI_LED_CONFIG2_ADDR 0x12

#define REG_DIE_TMP_INTEGER_ADDR 0x1F
#define REG_DIE_TMP_FRACTION_ADDR 0x20
#define REG_DIE_TMP_CONFIG_ADDR 0x21  //模块内部温度检测

#define REG_PROXIMITY_INT_THRESH_ADDR 0x30
/*************寄存器设置说明***************/
/********中断状态1*******/
#define INT_A_FULL_FLAG (1 << 7)  //FIFO 将要满，阈值在寄存器REG_FIFO_CONFIG_ADDR 设置FIFO_A_FULL[3，0]，读后清除 
#define INT_PPG_RDY_FLAG (1 << 6)  // FIFO 存在新数据，读此寄存器或者读FIFO_DATA寄存器后清零
#define INT_ALC_OVF_FLAG (1 << 5)  //SpO2的光二极管的环境光消除功能达到极限值，环境光将影响测量的准确性
#define INT_PROX_INT_FLAG (1 << 4)  //检测到物体接近，可以进行测量
#define INT_PWR_RDY_FLAG (1 << 0)  //设备供电稳定

/********中断状态2*******/
#define INT_DIE_TEMP_RDY_FLAG (1 << 0)  //磨具温度
/********中断设置1*******/
#define INT_A_FULL_EN (1 << 7)  //
#define INT_PPG_RDY_EN (1 << 6)  // 
#define INT_ALC_OVF_EN (1 << 5)  //
#define INT_PROX_INT_EN (1 << 4)  //


/********中断设置2*******/
#define INT_DIE_TEMP_RDY_EN (1 << 0)  //磨具温度

/********FIFO配置寄存器***/
#define AMP_AVE_NONE  0
#define AMP_AVE_BY_2  1
#define AMP_AVE_BY_4  2
#define AMP_AVE_BY_8  3
#define AMP_AVE_BY_16  4
#define AMP_AVE_BY_32  7

#define SMP_AVE_POS 5          //为降低数据量，设置用来做平均的相邻采样值的个数
#define FIFO_ROLLOVER_EN_POS (1<<4) //FIFO满后，新数据是否可以覆盖旧数据
#define FIFO_A_FULL_THRESH_POS 0  //FIFO满的阈值
/******模式设置*********/
#define MODE_MASK (~(7<<0))
#define MODE_POS (0) 

#define MODE_SHDN  (1<<7)   //置位1 MX30100进入待机模式，寄存器值保留，中断置位为0
#define MODE_REST  (1<<6)   //置位1，所有设置复位
#define MODE_HR_ONLY (2)    //Hr mode only
#define MODE_SPO2_ONLY (3)    //SpO2 mode only
#define MODE_SPO2_HR (7) //SpO2 and mode
/*********血氧配置寄存器*******/
//SpO2 ADC Range Control
#define SPO2_ADC_RGE_MASK (~(2<<5))
#define SPO2_ADC_RGE_POS (5) 

#define SPO2_ADC_RGE_0 (0<<5)   //7.81 pA -2048 nA  
#define SPO2_ADC_RGE_1 (1<<5)   //15.63 pA -4096 nA 
#define SPO2_ADC_RGE_2 (2<<5)   //31.25 pA -8192 nA 
#define SPO2_ADC_RGE_3 (3<<5)   //62.5 pA -16384www nA 

//SpO2 Sample Rate Control
#define SPO2_SR_MASK (~(7<<2))
#define SPO2_SR_POS (2) 

#define SPO2_SR_50 (0<<2)   //50Hz
#define SPO2_SR_100 (1<<2)   //100Hz
#define SPO2_SR_200 (2<<2)   //200Hz
#define SPO2_SR_400 (3<<2)   //400Hz
#define SPO2_SR_800 (4<<2)   //800Hz
#define SPO2_SR_1000 (5<<2)   //1000Hz
#define SPO2_SR_1600 (6<<2)   //1600Hz
#define SPO2_SR_3200 (7<<2)   //3200Hz

//LED Pulse Width Control
#define LED_PW_MASK (~(3))
#define LED_PW_POS (0) 

#define LED_PW_AD_15  0 //AD 15位 脉冲宽度69us
#define LED_PW_AD_16  1 //AD 16位 脉冲宽度118us
#define LED_PW_AD_17  2 //AD 17位 脉冲宽度215us
#define LED_PW_AD_18  3 //AD 18位 脉冲宽度411us

//模块内部温度检测

#define TEMP_EN (1<<0)

//
#define LED1_RED_PA (1)
#define LED2_IR_PA (2)
#define LED1_RED_PILOT_PA (5)
#define LED2_IR_PILOT_PA (6)

#define SLOT13_POS  0
#define SLOT24_POS  4
#endif