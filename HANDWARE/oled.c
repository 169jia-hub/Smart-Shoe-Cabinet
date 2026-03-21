#include "oled.h"
#include "oledfont.h"
#include "myiic.h"
#include "delay.h"

void OLED_WR_Byte(u8 dat,u8 mode) {
    IIC_Start();
    IIC_Send_Byte(0x78);
    IIC_Wait_Ack();
    if(mode){IIC_Send_Byte(0x40);}
    else{IIC_Send_Byte(0x00);}
    IIC_Wait_Ack();
    IIC_Send_Byte(dat);
    IIC_Wait_Ack();
    IIC_Stop();
}

void OLED_Set_Pos(unsigned char x, unsigned char y) { 
    OLED_WR_Byte(0xb0+y,0);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,0);
    OLED_WR_Byte((x&0x0f),0); 
}   

void OLED_Clear(void) {  
    u8 i,n;		    
    for(i=0;i<8;i++) {  
        OLED_WR_Byte (0xb0+i,0);    
        OLED_WR_Byte (0x00,0);      
        OLED_WR_Byte (0x10,0);      
        for(n=0;n<128;n++)OLED_WR_Byte(0,1); 
    }
}

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size) {      	
    unsigned char c=0,i=0;	
    c=chr-' ';			
    if(x>120){x=0;y=y+2;}
    if(size ==16) {
        OLED_Set_Pos(x,y);	
        for(i=0;i<8;i++) OLED_WR_Byte(F8X16[c*16+i],1);
        OLED_Set_Pos(x,y+1);
        for(i=0;i<8;i++) OLED_WR_Byte(F8X16[c*16+i+8],1);
    }
}

u32 oled_pow(u8 m,u8 n) {
    u32 result=1;	 
    while(n--)result*=m;    
    return result;
}				  

void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size) {         	
    u8 t,temp;
    u8 enshow=0;						   
    for(t=0;t<len;t++) {
        temp=(num/oled_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1)) {
            if(temp==0) {
                OLED_ShowChar(x+(size/2)*t,y,' ',size);
                continue;
            }else enshow=1; 
        }
        OLED_ShowChar(x+(size/2)*t,y,temp+'0',size);
    }
}

void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size) {
    unsigned char j=0;
    while (chr[j]!='\0') {		
        OLED_ShowChar(x,y,chr[j],size);
        x+=8;
        if(x>120){x=0;y+=2;}
        j++;
    }
}

void OLED_ShowChinese(u8 x,u8 y,u8 no) {      			    
    u8 t;
    if(no > 10) return; // 安全锁：索引超过10直接退出，绝不越界读乱码！
    OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++) OLED_WR_Byte(Hzk[2*no][t],1); 
    OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++) OLED_WR_Byte(Hzk[2*no+1][t],1); 
}
void OLED_Init(void) {
    IIC_Init(); 
    delay_ms(200);
    OLED_WR_Byte(0xAE,0);
    OLED_WR_Byte(0x00,0);
    OLED_WR_Byte(0x10,0);
    OLED_WR_Byte(0x40,0);
    OLED_WR_Byte(0x81,0); 
    OLED_WR_Byte(0xCF,0);
    OLED_WR_Byte(0xA1,0);
    OLED_WR_Byte(0xC8,0);
    OLED_WR_Byte(0xA6,0);
    OLED_WR_Byte(0xA8,0);
    OLED_WR_Byte(0x3f,0);
    OLED_WR_Byte(0xD3,0);
    OLED_WR_Byte(0x00,0);
    OLED_WR_Byte(0xd5,0);
    OLED_WR_Byte(0x80,0);
    OLED_WR_Byte(0xD9,0);
    OLED_WR_Byte(0xF1,0);
    OLED_WR_Byte(0xDA,0);
    OLED_WR_Byte(0x12,0);
    OLED_WR_Byte(0xDB,0);
    OLED_WR_Byte(0x40,0);
    OLED_WR_Byte(0x20,0);
    OLED_WR_Byte(0x02,0);
    OLED_WR_Byte(0x8D,0);
    OLED_WR_Byte(0x14,0);
    OLED_WR_Byte(0xA4,0);
    OLED_WR_Byte(0xA6,0);
    OLED_WR_Byte(0xAF,0);
    OLED_Clear();
}