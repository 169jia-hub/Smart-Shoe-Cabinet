#include "board_config.h"
#include "delay.h"

void Key_Init(void)
{
    RCC->APB2ENR |= 1<<3; // 开启 GPIOB 时钟

    // 1. 配置 PB7 (CRL寄存器的高4位) -> 对应 KEY_UP (按键2)
    GPIOB->CRL &= 0x0FFFFFFF; // 正确：清空 28-31位
    GPIOB->CRL |= 0x80000000; // 正确：设置为 1000 (上拉/下拉输入)

    // 2. 配置 PB8, PB9, PB13 (CRH寄存器)
    // PB8(0-3位), PB9(4-7位), PB13(20-23位)
    GPIOB->CRH &= 0xFF0FFF00; // 🌟 修复：末尾两个0，彻底清空 PB8 和 PB9，中间的0清空 PB13
    GPIOB->CRH |= 0x00800088; // 🌟 优化：一步到位！分别给 PB13, PB9, PB8 填入 1000 (0x8)
    
    // 3. ODR 置 1 (必须写1，才能把上面的“上下拉输入”确认为“上拉输入”)
    GPIOB->ODR |= (1<<7)|(1<<8)|(1<<9)|(1<<13);
}

// 扫描代码不用变，因为 board_config.h 里 KEY_UP 已经改回了 PB7
u8 Key_Scan(void)
{
    // ... (保持之前的代码不变) ...
    static u8 key_up = 1;
    if(key_up && (KEY_MENU==0 || KEY_UP==0 || KEY_DOWN==0 || KEY_BACK==0))
    {
        delay_ms(20);
        key_up = 0;
        if(KEY_MENU==0) return 1;
        if(KEY_UP==0)   return 2;
        if(KEY_DOWN==0) return 3;
        if(KEY_BACK==0) return 4;
    }
    else if(KEY_MENU==1 && KEY_UP==1 && KEY_DOWN==1 && KEY_BACK==1)
    {
        key_up = 1;
    }
    return 0;
}

u8 Key_Scan_Long(void)
{
    // ... (保持之前的代码不变) ...
    if(KEY_MENU==0)
    {
        delay_ms(20);
        if(KEY_MENU==0)
        {
            u16 time = 0;
            while(KEY_MENU==0 && time < 200)
            {
                delay_ms(10);
                time++;
            }
            if(time >= 200) return 1;
            else return 2;
        }
    }
    return 0;
}