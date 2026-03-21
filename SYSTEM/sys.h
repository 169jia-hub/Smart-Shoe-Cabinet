#ifndef __SYS_H
#define __SYS_H	
#include "stm32f10x.h"

// 0, 不支持ucos; 1, 支持ucos
#define SYSTEM_SUPPORT_OS		0		

// 位带操作核心宏，实现 51 类似的 GPIO 控制
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x02000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

// IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 

// IO口操作宏, 只对使能了时钟的IO口有效
// 输出 (Output)
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  
#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  

// 输入 (Input)
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)   
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)   
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)   
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)   
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)   

#endif