#include "myiic.h"
#include "delay.h"

// 初始化IIC
void IIC_Init(void)
{
    RCC->APB2ENR |= 1<<3; // 使能GPIOB时钟
    
    // 改为 0x03300000 (推挽输出)，提供强劲驱动力，无视上拉电阻缺失
    GPIOB->CRL &= 0xF00FFFFF; 
    GPIOB->CRL |= 0x03300000; 
    
    GPIOB->ODR |= 3<<5;       // PB5, PB6 默认拉高
}

// 产生IIC起始信号
void IIC_Start(void)
{
    IIC_SDA=1;	  
    IIC_SCL=1;
    delay_us(4);
    IIC_SDA=0; 
    delay_us(4);
    IIC_SCL=0; 
}

// 产生IIC停止信号
void IIC_Stop(void)
{
    IIC_SCL=0;
    IIC_SDA=0; 
    delay_us(4);
    IIC_SCL=1; 
    IIC_SDA=1; 
    delay_us(4);							   	
}

u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime=0;
    IIC_SDA=1;delay_us(1);	   
    IIC_SCL=1;delay_us(1);	 
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL=0; 	   
    return 0;  
}

void IIC_Ack(void)
{
    IIC_SCL=0;
    IIC_SDA=0;
    delay_us(2);
    IIC_SCL=1;
    delay_us(2);
    IIC_SCL=0;
}

void IIC_NAck(void)
{
    IIC_SCL=0;
    IIC_SDA=1;
    delay_us(2);
    IIC_SCL=1;
    delay_us(2);
    IIC_SCL=0;
}					 				     

void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
    IIC_SCL=0; 
    for(t=0;t<8;t++)
    {              
        if((txd&0x80)>>7) IIC_SDA=1;
        else IIC_SDA=0;
        txd<<=1; 	  
        delay_us(2);   
        IIC_SCL=1;
        delay_us(2); 
        IIC_SCL=0;	
        delay_us(2);
    }	 
}

u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    IIC_SDA=1;
    for(i=0;i<8;i++ )
    {
        IIC_SCL=0; 
        delay_us(2);
        IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
        delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck(); 
    else
        IIC_Ack(); 
    return receive;
}