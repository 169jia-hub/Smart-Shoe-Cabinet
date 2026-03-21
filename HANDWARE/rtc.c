#include "rtc.h"
#include "delay.h"

Calendar_obj calendar; // 存放当前时间

// 初始化RTC
// 返回0:成功, 1:失败
u8 RTC_Init(void)
{
    // 检查是否第一次配置 (BKP备份寄存器)
    RCC->APB1ENR |= 1<<28; // 使能电源时钟
    RCC->APB1ENR |= 1<<27; // 使能备份时钟
    PWR->CR |= 1<<8;       // 取消备份区写保护
    
    // 这里的 0xA5A5 是我自己定义的标记，如果读出来不是这个，说明第一次上电
    if (BKP->DR1 != 0xA5A5)
    {
        RCC->BDCR |= 1<<16;  // 备份区域软复位
        RCC->BDCR &= ~(1<<16);
        RCC->BDCR |= 1<<0;   // 开启外部低速振荡器(LSE)
        
        while(!(RCC->BDCR & 0x02)); // 等待LSE就绪
        
        RCC->BDCR |= 1<<8;   // 选择LSE作为RTC时钟
        RCC->BDCR |= 1<<15;  // 使能RTC时钟
        
        while(!(RTC->CRL & (1<<5))); // 等待RTC寄存器操作完成(RTOFF)
        while(!(RTC->CRL & (1<<3))); // 等待RSF同步
        
        RTC->CRL |= 1<<4;    // 进入配置模式
        RTC->PRLH = 0;
        RTC->PRLL = 32767;   // 分频系数 (32.768kHz / 32768 = 1Hz)
        RTC->CRL &= ~(1<<4); // 退出配置模式
        while(!(RTC->CRL & (1<<5))); // 等待写入完成
        
        BKP->DR1 = 0xA5A5;   // 写入标记，下次不用初始化了
        RTC_Set(2026, 2, 13, 12, 0, 0); // 默认设为 12:00:00
    }
    else
    {
        while(!(RTC->CRL & (1<<3))); // 等待RSF同步
        while(!(RTC->CRL & (1<<5))); // 等待RTOFF
    }
    return 0;
}

// 设置时间 (简化版，只处理时分秒，实际上RTC存的是总秒数)
void RTC_Set(u16 year,u8 mon,u8 day,u8 hour,u8 min,u8 sec)
{
    // 简单算法：把时分秒换算成总秒数存进去 (忽略日期，假设每天循环)
    u32 seccount = 0;
    seccount = hour*3600 + min*60 + sec;
    
    RCC->APB1ENR |= 1<<28;
    RCC->APB1ENR |= 1<<27;
    PWR->CR |= 1<<8;
    
    RTC->CRL |= 1<<4;   // 配置模式
    RTC->CNTH = seccount >> 16;
    RTC->CNTL = seccount & 0xFFFF;
    RTC->CRL &= ~(1<<4); // 退出配置
    while(!(RTC->CRL & (1<<5))); 
}

// 获取时间
void RTC_Get(void)
{
    u32 timecount = 0;
    timecount = (RTC->CNTH << 16) | RTC->CNTL;
    
    // 超过24小时归零 (简易逻辑)
    if(timecount >= 86400) {
        timecount = 0;
        RTC_Set(0,0,0,0,0,0);
    }
    
    calendar.hour = timecount / 3600;
    calendar.min  = (timecount % 3600) / 60;
    calendar.sec  = (timecount % 3600) % 60;
}