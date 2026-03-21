#include "sys.h"
#include "usart.h"	  

// 加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
struct __FILE { int handle; }; 
FILE __stdout;       
void _sys_exit(int x) { x = x; } 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
    while((USART1->SR&0X40)==0); // 等待上一次发送完毕   
    USART1->DR = (u8) ch;      	 // 将数据发给蓝牙模块
    return ch;
}
#endif 

u8 USART_RX_BUF[USART_REC_LEN];     // 接收缓冲
u16 USART_RX_STA = 0;               // 接收状态标记	

// 初始化串口1 (PA9 TX, PA10 RX)
// bound: 波特率 (一般蓝牙模块默认是 9600)
void uart_init(u32 bound)
{
    // 1. 计算波特率
    float temp;
    u16 mantissa;
    u16 fraction;	   
    temp = (float)(72*1000000)/(bound*16); 
    mantissa = temp;				 
    fraction = (temp-mantissa)*16; 
    mantissa <<= 4;
    mantissa += fraction; 
    
    // 2. 开启时钟
    RCC->APB2ENR |= 1<<2;   // 使能 PORTA 时钟  
    RCC->APB2ENR |= 1<<14;  // 使能串口1 时钟 
    
    // 3. 配置引脚 PA9, PA10
    GPIOA->CRH &= 0xFFFFF00F; 
    GPIOA->CRH |= 0x000008B0; // PA9(TX)推挽复用, PA10(RX)浮空输入
      
    // 4. 配置串口寄存器
    RCC->APB2RSTR |= 1<<14;   // 复位串口1
    RCC->APB2RSTR &= ~(1<<14);// 停止复位	   	   
    USART1->BRR = mantissa;   // 设置波特率
    USART1->CR1 |= 0X200C;    // 设置工作模式: 1位停止位,无校验,使能发送接收
    
    // 5. 开启中断
    USART1->CR1 |= 1<<5;      // 接收缓冲区非空中断使能	    
    MY_NVIC_Init(3, 3, USART1_IRQn, 2); // 抢占3，响应3，组2 (需要sys.c里的中断分组)
}

// 串口1中断服务函数：负责接收手机APP发来的指令
void USART1_IRQHandler(void)
{
    u8 Res;
    if(USART1->SR & (1<<5))	// 接收到数据
    {	 
        Res = USART1->DR; // 读取接收到的数据
        
        // 这里做一个最简单的协议：假设手机发来的指令以 回车(\r\n) 结尾
        if((USART_RX_STA & 0x8000) == 0) // 还没接收完成
        {
            if(USART_RX_STA & 0x4000) // 已经收到了回车符 '\r' (0x0D)
            {
                if(Res != 0x0A) USART_RX_STA = 0; // 接收错误,重新开始
                else USART_RX_STA |= 0x8000;	  // 接收完成了!
            }
            else // 还没收到回车
            {	
                if(Res == 0x0D) USART_RX_STA |= 0x4000; // 收到回车了
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3FFF] = Res ; // 存入数组
                    USART_RX_STA++;
                    if(USART_RX_STA > (USART_REC_LEN-1)) USART_RX_STA = 0; // 太长了，丢弃
                }		 
            }
        }  		 
    }
}