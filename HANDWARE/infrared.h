#ifndef __INFRARED_H
#define __INFRARED_H
#include "sys.h"

// 定义引脚读取宏，方便主函数调用
// 读取 GPIOB 的第 15 位
#define IR_SENSOR_PIN   PBin(15) 

void Infrared_Init(void); // 初始化函数

#endif