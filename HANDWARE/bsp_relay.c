#include "board_config.h"
#include "delay.h"
void Relay_Init(void)
{
    // 1. 开启时钟 GPIOA, GPIOB
    RCC->APB2ENR |= 1<<2; // GPIOA
    RCC->APB2ENR |= 1<<3; // GPIOB

    // 2. 配置 PA5, PA6, PA7, PA8 为推挽输出
    // PA5,6,7 (CRL)
    GPIOA->CRL &= 0x000FFFFF; 
    GPIOA->CRL |= 0x33300000; 
    // PA8 (CRH)
    GPIOA->CRH &= 0xFFFFFFF0;
    GPIOA->CRH |= 0x00000003;

    // 3. 配置 PB1 (蜂鸣器) 为推挽输出
    GPIOB->CRL &= 0xFFFFFF0F;
    GPIOB->CRL |= 0x00000030;

    // 4. 默认全部关闭 (低电平)
    RELAY_HEATER = 0;
    RELAY_FAN    = 0;
    RELAY_UV     = 0;
    RELAY_LIGHT  = 0;
    BEEP         = 0;
}

// 蜂鸣器报警音
void Beep_Alarm(void)
{
    BEEP = 1;
    delay_ms(100); // 滴
    BEEP = 0;
}