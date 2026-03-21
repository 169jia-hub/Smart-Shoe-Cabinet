#ifndef __RTC_H
#define __RTC_H
#include "sys.h"

// 时间结构体
typedef struct 
{
    u8 hour;
    u8 min;
    u8 sec;
} Calendar_obj;

extern Calendar_obj calendar; // 全局时间变量

u8 RTC_Init(void);        // 初始化RTC
void RTC_Get(void);       // 获取当前时间
void RTC_Set(u16 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec); // 设置时间(这里简化，只用时分秒)

#endif