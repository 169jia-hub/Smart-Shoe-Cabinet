#include "sys.h"

void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)
{
    u32 temp,temp1;
    temp1=(~NVIC_Group)&0x07;
    temp1<<=8;
    temp=SCB->AIRCR;  
    temp&=0X0000F8FF; 
    temp|=0X05FA0000; 
    temp|=temp1;
    SCB->AIRCR=temp;  
}

void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)
{
    u32 temp;
    MY_NVIC_PriorityGroupConfig(NVIC_Group);
    temp=NVIC_PreemptionPriority<<(4-NVIC_Group);
    temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
    temp&=0xf; 
    NVIC->ISER[NVIC_Channel/32]|=(1<<(NVIC_Channel%32));
    NVIC->IP[NVIC_Channel]|=temp<<4;
}