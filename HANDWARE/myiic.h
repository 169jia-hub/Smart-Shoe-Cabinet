#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"

// === 极其关键：强制绑定 SCL 到 PB5, SDA 到 PB6 ===
#define IIC_SCL    PBout(5) // SCL输出线
#define IIC_SDA    PBout(6) // SDA输出线
#define READ_SDA   PBin(6)  // SDA输入线

// IIC所有操作函数
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(unsigned char ack);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);

#endif