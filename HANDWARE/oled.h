#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"

// 宏定义 OLED 写命令与数据
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

// 函数声明
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,u8 *p,u8 size);	 
void OLED_ShowChinese(u8 x,u8 y,u8 no);

#endif