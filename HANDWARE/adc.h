#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

// 函数声明
void Adc_Init(void);                      // 初始化ADC1 (PA0, PA4)
u16  Get_Adc(u8 ch);                      // 获得某个通道值
u16  Get_Adc_Average(u8 ch, u8 times);    // 获取多次采样的平均值

#endif