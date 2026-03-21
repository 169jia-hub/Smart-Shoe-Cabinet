#include "adc.h"
#include "delay.h"

void Adc_Init(void)
{
    RCC->APB2ENR |= 1<<9;    // 使能 ADC1
    RCC->APB2ENR |= 1<<2;    // 使能 GPIOA

    RCC->CFGR &= ~(3<<14);   
    RCC->CFGR |= (2<<14);    // 6分频

    GPIOA->CRL &= 0xFFF0FFF0; // PA0, PA4 模拟输入
    GPIOA->CRL |= 0x00000000; 

    ADC1->CR1 = 0;           
    ADC1->CR2 = 0; 
    
    // 重点修复：完整的软件触发配置
    ADC1->CR2 |= 7<<17;      // EXTSEL = 111 (软件触发)
    ADC1->CR2 |= 1<<20;      // EXTTRIG = 1 (开启外部触发)
    ADC1->CR2 |= 1<<0;       // ADON = 1    (开启ADC)
    
    delay_ms(10);
    
    ADC1->CR2 |= 1<<3;       // 复位校准
    while(ADC1->CR2 & 1<<3); 
    ADC1->CR2 |= 1<<2;       // 开启校准
    while(ADC1->CR2 & 1<<2); 
}

u16 Get_Adc(u8 ch)
{
    u16 timeout = 0; // 超时计数器
    
    ADC1->SQR3 &= 0xFFFFFFE0; 
    ADC1->SQR3 |= ch;         
    
    ADC1->CR2 |= 1<<22;       // 触发转换 (SWSTART)
    
    // 重点修复：加入防死机超时退出机制！
    while(!(ADC1->SR & 1<<1)) 
    {
        timeout++;
        if(timeout > 10000) break; // 超过时间直接退出，绝不允许死机卡住屏幕！
    }
    return ADC1->DR;          
}

u16 Get_Adc_Average(u8 ch, u8 times)
{
    u32 temp_val = 0;
    u8 t;
    for(t=0; t<times; t++)
    {
        temp_val += Get_Adc(ch);
        delay_ms(5); 
    }
    return temp_val / times;
}