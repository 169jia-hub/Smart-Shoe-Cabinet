#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  200  	// 定义最大接收字节数 200

// 外部声明，方便 main 函数调用接收到的数据
extern u8  USART_RX_BUF[USART_REC_LEN]; // 接收缓冲
extern u16 USART_RX_STA;         		// 接收状态标记

void uart_init(u32 bound);      // 串口初始化函数

#endif