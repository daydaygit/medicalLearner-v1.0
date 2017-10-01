#ifndef _MAX_30100_CONFIG_H
#define _MAX_30100_CONFIG_H

#define MAX30100_FIFO_LENGTH 32

#define REG_INT_STATUS0_ADDR 0x00     //�ж�״̬1
#define REG_INT_STATUS1_ADDR 0x01     //�ж�״̬2
#define REG_INT_ENALE1_ADDR  0x02     //�ж�����1
#define REG_INT_ENALE2_ADDR  0x03     //�ж�����2
#define REG_FIFO_WR_PTR_ADDR 0x04     //FIFO д��ַ�Ĵ���
#define REG_FIFO_OV_COUNTER_ADDR 0x05 //�������ͳ�ƼĴ���
#define REG_FIFO_RD_PTR_ADDR 0x06     //FIFO ����ַ�Ĵ���
#define REG_FIFO_DATA_ADDR 0x07       //FIFO ���ݵ�ַ


#define REG_FIFO_CONFIG_ADDR 0x08    //FIFO���üĴ���
#define REG_MODE_CONFIG_ADDR 0x09    //ģʽ���üĴ���
#define REG_SPO2_CONFIG_ADDR 0x0a    //Ѫ�����üĴ���
#define REG_LED1_PA_ADDR 0x0C    //led1 ����Ŵ󣨵���������
#define REG_LED2_PA_ADDR 0x0D    //led2 ����Ŵ󣨵���������
#define REG_PILOT_PA_ADDR 0x10   //�ӽ���������
#define REG_MULTI_LED_CONFIG1_ADDR 0x11
#define REG_MULTI_LED_CONFIG2_ADDR 0x12

#define REG_DIE_TMP_INTEGER_ADDR 0x1F
#define REG_DIE_TMP_FRACTION_ADDR 0x20
#define REG_DIE_TMP_CONFIG_ADDR 0x21  //ģ���ڲ��¶ȼ��

#define REG_PROXIMITY_INT_THRESH_ADDR 0x30
/*************�Ĵ�������˵��***************/
/********�ж�״̬1*******/
#define INT_A_FULL_FLAG (1 << 7)  //FIFO ��Ҫ������ֵ�ڼĴ���REG_FIFO_CONFIG_ADDR ����FIFO_A_FULL[3��0]��������� 
#define INT_PPG_RDY_FLAG (1 << 6)  // FIFO ���������ݣ����˼Ĵ������߶�FIFO_DATA�Ĵ���������
#define INT_ALC_OVF_FLAG (1 << 5)  //SpO2�Ĺ�����ܵĻ������������ܴﵽ����ֵ�������⽫Ӱ�������׼ȷ��
#define INT_PROX_INT_FLAG (1 << 4)  //��⵽����ӽ������Խ��в���
#define INT_PWR_RDY_FLAG (1 << 0)  //�豸�����ȶ�

/********�ж�״̬2*******/
#define INT_DIE_TEMP_RDY_FLAG (1 << 0)  //ĥ���¶�
/********�ж�����1*******/
#define INT_A_FULL_EN (1 << 7)  //
#define INT_PPG_RDY_EN (1 << 6)  // 
#define INT_ALC_OVF_EN (1 << 5)  //
#define INT_PROX_INT_EN (1 << 4)  //


/********�ж�����2*******/
#define INT_DIE_TEMP_RDY_EN (1 << 0)  //ĥ���¶�

/********FIFO���üĴ���***/
#define AMP_AVE_NONE  0
#define AMP_AVE_BY_2  1
#define AMP_AVE_BY_4  2
#define AMP_AVE_BY_8  3
#define AMP_AVE_BY_16  4
#define AMP_AVE_BY_32  7

#define SMP_AVE_POS 5          //Ϊ����������������������ƽ�������ڲ���ֵ�ĸ���
#define FIFO_ROLLOVER_EN_POS (1<<4) //FIFO�����������Ƿ���Ը��Ǿ�����
#define FIFO_A_FULL_THRESH_POS 0  //FIFO������ֵ
/******ģʽ����*********/
#define MODE_MASK (~(7<<0))
#define MODE_POS (0) 

#define MODE_SHDN  (1<<7)   //��λ1 MX30100�������ģʽ���Ĵ���ֵ�������ж���λΪ0
#define MODE_REST  (1<<6)   //��λ1���������ø�λ
#define MODE_HR_ONLY (2)    //Hr mode only
#define MODE_SPO2_ONLY (3)    //SpO2 mode only
#define MODE_SPO2_HR (7) //SpO2 and mode
/*********Ѫ�����üĴ���*******/
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

#define LED_PW_AD_15  0 //AD 15λ ������69us
#define LED_PW_AD_16  1 //AD 16λ ������118us
#define LED_PW_AD_17  2 //AD 17λ ������215us
#define LED_PW_AD_18  3 //AD 18λ ������411us

//ģ���ڲ��¶ȼ��

#define TEMP_EN (1<<0)

//
#define LED1_RED_PA (1)
#define LED2_IR_PA (2)
#define LED1_RED_PILOT_PA (5)
#define LED2_IR_PILOT_PA (6)

#define SLOT13_POS  0
#define SLOT24_POS  4
#endif