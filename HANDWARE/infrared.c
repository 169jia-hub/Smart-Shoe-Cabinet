#include "infrared.h"

// 初始化光电红外传感器 (PB15)
void Infrared_Init(void)
{
    // 1. 开启 GPIOB 时钟
    RCC->APB2ENR |= 1<<3; 

    // 2. 配置 PB15 为上拉输入 (Input Pull-up)
    // PB15 在 CRH 寄存器中控制 (Pin8~Pin15 用 CRH)
    // 偏移量：(15 - 8) * 4 = 28位
    GPIOB->CRH &= 0x0FFFFFFF; // 清空第 28~31 位
    GPIOB->CRH |= 0x80000000; // 配置为 1000 (CNF=10, MODE=00 -> 上拉/下拉输入)
    
    // 3. ODR 寄存器置 1，设置为上拉 (如果是 0 就是下拉)
    // 通常红外模块输出是开漏或推挽，配合上拉输入最稳定
    GPIOB->ODR |= 1<<15;      
}