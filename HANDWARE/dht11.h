#ifndef __DHT11_H
#define __DHT11_H
#include "sys.h"

// 类型定义，用于存储读取结果
u8 DHT11_Init(void); // 初始化，返回0成功，1失败
u8 DHT11_Read_Data(u8 *temp, u8 *humi); // 读取温湿度，返回0成功

#endif