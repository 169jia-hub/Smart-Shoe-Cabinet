#include "sys.h"
#include "delay.h"
#include "board_config.h"
#include "bsp_relay.h"
#include "bsp_key.h"
#include "adc.h"      
#include "dht11.h"    
#include "infrared.h" 
#include "oled.h"
#include "rtc.h"
#include "usart.h" 

// ====== 系统全局变量 (阈值) ======
u8  temp_limit_min = 20;   
u8  humi_limit_max = 80;   
u16 mq_limit_max   = 1500; 
u16 light_limit_max= 2000; 
u16 pm25_limit_max = 520; 

// 定时消毒 (包含时分秒)
u8  uv_start_h = 2, uv_start_m = 0, uv_start_s = 0;
u8  uv_stop_h  = 3, uv_stop_m  = 0, uv_stop_s  = 0;

// 系统状态机
u8 sys_mode = 0;         // 0自动, 1手动, 2主设置菜单, 3二级设置菜单
u8 main_menu_index = 0;  // 主菜单光标 (0~5，共6项)
u8 sub_menu_index = 0;   // 二级菜单内部参数切换光标
u8 manual_cursor = 0;    // 手动模式下外设的光标 (0~4)

// 格式化显示时间
void Show_Time(u8 x, u8 y) {
    OLED_ShowChinese(x, y, 9);     // 时
    OLED_ShowChinese(x+16, y, 10); // 分
    OLED_ShowChar(x+32, y, ':', 16);
    OLED_ShowNum(x+40, y, calendar.hour, 2, 16);
    OLED_ShowChar(x+56, y, ':', 16);
    OLED_ShowNum(x+64, y, calendar.min, 2, 16);
    OLED_ShowChar(x+80, y, ':', 16);
    OLED_ShowNum(x+88, y, calendar.sec, 2, 16);
}

// 自动控制逻辑核心 (加入了 PM2.5 报警)
void Auto_Control_Task(u8 t, u8 h, u16 mq, u16 lux, u8 door_closed, u16 pm25)
{
    u16 now_min = calendar.hour * 60 + calendar.min;
    u16 start_min = uv_start_h * 60 + uv_start_m;
    u16 stop_min = uv_stop_h * 60 + uv_stop_m;
    u8 is_uv_time = 0;

    // 智能加热
    if(t < temp_limit_min && t > 0) RELAY_HEATER = 1;
    else RELAY_HEATER = 0;

    // 智能排湿与空气质量/粉尘报警
    if(h > humi_limit_max || mq > mq_limit_max || pm25 > pm25_limit_max) {
        RELAY_FAN = 1; // 超标必定开风扇通风
        if(mq > mq_limit_max || pm25 > pm25_limit_max) Beep_Alarm(); // 空气或粉尘超标蜂鸣器报警
    } else {
        RELAY_FAN = 0;
    }

    // 智能照明
    if(door_closed == 0) { 
        if(lux > light_limit_max) RELAY_LIGHT = 1; 
        else RELAY_LIGHT = 0; 
    } else {
        RELAY_LIGHT = 0; 
    }

    // 定时紫外线
    if(start_min < stop_min) { 
        if(now_min >= start_min && now_min < stop_min) is_uv_time = 1;
    } else { 
        if(now_min >= start_min || now_min < stop_min) is_uv_time = 1;
    }
    if(is_uv_time && door_closed == 1) RELAY_UV = 1;
    else RELAY_UV = 0; 
}

// 显示自动模式主界面
void UI_Auto_Page(u8 t, u8 h, u16 mq, u16 lux, u8 door) {
    Show_Time(0, 0); 

    OLED_ShowChinese(0, 2, 0);  // 温
    OLED_ShowChar(16, 2, ':', 16); 
    OLED_ShowNum(24, 2, t, 2, 16);
    
    OLED_ShowChinese(64, 2, 2); // 湿
    OLED_ShowChar(80, 2, ':', 16); 
    OLED_ShowNum(88, 2, h, 2, 16); 
    OLED_ShowChar(104, 2, '%', 16);

    OLED_ShowChinese(0, 4, 3);  // 光
    OLED_ShowChinese(16, 4, 4); // 照
    OLED_ShowChar(32, 4, ':', 16); 
    OLED_ShowNum(40, 4, lux, 4, 16);
    
    OLED_ShowString(0, 6, "air:", 16); 
    OLED_ShowNum(32, 6, mq, 4, 16);
    
    OLED_ShowChinese(64, 6, 8); // 柜
    OLED_ShowChinese(80, 6, 5); // 门
    OLED_ShowChar(96, 6, ':', 16); 
    if(door) OLED_ShowChinese(104, 6, 7); // 关
    else OLED_ShowChinese(104, 6, 6);     // 开
}

// 显示手动控制模式界面
void UI_Manual_Page(void) {
    char* items[5] = {"1.Light", "2.UV", "3.Fan", "4.Heater", "5.Beep"};
    u8 start_i;
    u8 i; 
    
    OLED_ShowString(0, 0, "== MANUAL ==", 16);
    start_i = (manual_cursor / 3) * 3; 
    
    for(i = 0; i < 3; i++) { 
        if(start_i + i < 5) {
            if((start_i + i) == manual_cursor) OLED_ShowString(0, 2+i*2, ">", 16); 
            else OLED_ShowString(0, 2+i*2, " ", 16);
            OLED_ShowString(16, 2+i*2, items[start_i+i], 16);
        } else {
            OLED_ShowString(0, 2+i*2, "                ", 16);
        }
    }
}

// 一级设置菜单：滚动列表显示 
void UI_Setting_Main_Page(void) {
    char* menu_strs[6] = {
        "1.Sys Time",
        "2.Temp&Humi",
        "3.Light Limit",
        "4.Air Quality",
        "5.PM2.5 Limit",
        "6.UV Timer"
    };
    u8 start_i = (main_menu_index / 4) * 4; 
    u8 i; 
    
    for(i = 0; i < 4; i++) { 
        if(start_i + i < 6) { 
            if((start_i + i) == main_menu_index) OLED_ShowString(0, i*2, ">", 16); 
            else OLED_ShowString(0, i*2, " ", 16);
            OLED_ShowString(16, i*2, menu_strs[start_i + i], 16);
        } else {
            OLED_ShowString(0, i*2, "                ", 16); 
        }
    }
}

// 二级设置菜单：具体参数调节显示
void UI_Setting_Sub_Page(void) {
    OLED_ShowString(0, 0, "-- SUB MENU --", 16);
    switch(main_menu_index) {
        case 0: // 系统时间
            OLED_ShowString(0,2,"Set Time:",16);
            OLED_ShowNum(0,4,calendar.hour,2,16); OLED_ShowString(16,4,":",16);
            OLED_ShowNum(24,4,calendar.min,2,16); OLED_ShowString(40,4,":",16);
            OLED_ShowNum(48,4,calendar.sec,2,16);
            OLED_ShowString(0,6,"                ",16); 
            if(sub_menu_index==0) OLED_ShowString(0,6,"^H",16);
            if(sub_menu_index==1) OLED_ShowString(24,6,"^M",16);
            if(sub_menu_index==2) OLED_ShowString(48,6,"^S",16);
            break;
        case 1: // 温湿度
            OLED_ShowString(0,2,"Temp Min:",16); OLED_ShowNum(80,2,temp_limit_min,2,16);
            OLED_ShowString(0,4,"Humi Max:",16); OLED_ShowNum(80,4,humi_limit_max,2,16);
            if(sub_menu_index==0) OLED_ShowString(104,2,"<-",16); else OLED_ShowString(104,2,"  ",16);
            if(sub_menu_index==1) OLED_ShowString(104,4,"<-",16); else OLED_ShowString(104,4,"  ",16);
            break;
        case 2: // 光照
            OLED_ShowString(0,2,"Light Max:",16); OLED_ShowNum(0,4,light_limit_max,4,16); break;
        case 3: // 空气质量
            OLED_ShowString(0,2,"Air Max:",16); OLED_ShowNum(0,4,mq_limit_max,4,16); break;
        case 4: // PM2.5
            OLED_ShowString(0,2,"PM2.5 Max:",16); OLED_ShowNum(0,4,pm25_limit_max,4,16); break;
        case 5: // 定时时间 (消毒开/关)
            OLED_ShowString(0,2,"ON :",16);
            OLED_ShowNum(32,2,uv_start_h,2,16); OLED_ShowString(48,2,":",16);
            OLED_ShowNum(56,2,uv_start_m,2,16); OLED_ShowString(72,2,":",16);
            OLED_ShowNum(80,2,uv_start_s,2,16);
            
            OLED_ShowString(0,4,"OFF:",16);
            OLED_ShowNum(32,4,uv_stop_h,2,16); OLED_ShowString(48,4,":",16);
            OLED_ShowNum(56,4,uv_stop_m,2,16); OLED_ShowString(72,4,":",16);
            OLED_ShowNum(80,4,uv_stop_s,2,16);

            OLED_ShowString(0,6,"                ",16);
            if(sub_menu_index==0) OLED_ShowString(32,6,"^H",16);
            if(sub_menu_index==1) OLED_ShowString(56,6,"^M",16);
            if(sub_menu_index==2) OLED_ShowString(80,6,"^S",16);
            if(sub_menu_index==3) OLED_ShowString(32,6,"^H(OFF)",16);
            if(sub_menu_index==4) OLED_ShowString(56,6,"^M(OFF)",16);
            if(sub_menu_index==5) OLED_ShowString(80,6,"^S(OFF)",16);
            break;
    }
}

int main(void)
{
    u8 t=0, h=0;
    u16 mq=0, lux=0, pm25_val=0; 
    u8 timer_cnt = 0; 
    u8 soft_sec_cnt = 0; 
    
    SystemInit(); delay_init();
    uart_init(9600); 
    Relay_Init(); 
    Adc_Init(); Infrared_Init();
    DHT11_Init(); 
    OLED_Init(); 
   
    Key_Init(); // 依然放在最后，确保 PB7 控制权

    while(1)
    {
        // === 1. 软件时钟走时逻辑 ===
        soft_sec_cnt++;
        if(soft_sec_cnt >= 100)  
        {
            soft_sec_cnt = 0;
            calendar.sec++;               
            if(calendar.sec >= 60) {
                calendar.sec = 0;
                calendar.min++;           
                if(calendar.min >= 60) {
                    calendar.min = 0;
                    calendar.hour = (calendar.hour + 1) % 24; 
                }
            }
        }
        
        // === 2. 传感器扫描 ===
        mq = Get_Adc_Average(4, 5);  
        lux = Get_Adc_Average(0, 5); 
        u8 door = DOOR_PIN; 

        timer_cnt++;
        if(timer_cnt >= 200)  
        {
            timer_cnt = 0;
            DHT11_Read_Data(&t, &h); 
            pm25_val = 300; 
            printf("环境温度:%d,湿度:%d,光照:%d,空气:%d,PM2.5:%d\r\n", t,h,lux,mq,pm25_val);
        }

        // === 3. 蓝牙接收处理 ===
        if(USART_RX_STA & 0x8000) 
        {					   
            u8 len = USART_RX_STA & 0x3FFF; 
            if(len > 0) 
            {
                sys_mode = 1; 
                switch(USART_RX_BUF[0]) 
                {
                    case 'A': RELAY_FAN = 1; break; case 'B': RELAY_FAN = 0; break;
                    case 'C': RELAY_UV = 1;  break; case 'D': RELAY_UV = 0;  break;
                    case 'E': RELAY_LIGHT = 1; break; case 'F': RELAY_LIGHT = 0; break;
                    case 'G': RELAY_HEATER = 1; break; case 'H': RELAY_HEATER = 0; break;
                }
            }
            USART_RX_STA = 0; 
        }

        // === 4. 按键分发系统 ===
        if (sys_mode == 0 || sys_mode == 1) {
            u8 k1_status = Key_Scan_Long();
            
            if (k1_status == 1) { 
                sys_mode = 2; main_menu_index = 0; OLED_Clear(); // 切换界面保留清屏
            } else if (k1_status == 2) { 
                sys_mode = (sys_mode == 0) ? 1 : 0; 
                manual_cursor = 0; OLED_Clear(); // 切换界面保留清屏
                RELAY_HEATER=0; RELAY_FAN=0; RELAY_UV=0; RELAY_LIGHT=0; BEEP=0;
            } else {
                u8 key_other = Key_Scan(); 
                if (sys_mode == 1 && key_other > 0) { 
                    if (key_other == 2) { 
                        manual_cursor = (manual_cursor + 1) % 5; 
                        // ? 修复：去掉了这里的 OLED_Clear()，告别闪屏！
                    }
                    else if (key_other == 3) { 
                        if(manual_cursor==0) RELAY_LIGHT=1;
                        if(manual_cursor==1) RELAY_UV=1;
                        if(manual_cursor==2) RELAY_FAN=1;
                        if(manual_cursor==3) RELAY_HEATER=1;
                        if(manual_cursor==4) BEEP=1;
                    }
                    else if (key_other == 4) { 
                        if(manual_cursor==0) RELAY_LIGHT=0;
                        if(manual_cursor==1) RELAY_UV=0;
                        if(manual_cursor==2) RELAY_FAN=0;
                        if(manual_cursor==3) RELAY_HEATER=0;
                        if(manual_cursor==4) BEEP=0;
                    }
                }
            }
        } 
        else {
            u8 key_val = Key_Scan(); 
            
            if (sys_mode == 2 && key_val > 0) { 
                if (key_val == 1 || key_val == 4) { 
                    sys_mode = 0; OLED_Clear(); // 切换界面保留清屏
                } else if (key_val == 2) { 
                    main_menu_index = (main_menu_index + 1) % 6; 
                    // ? 修复：去掉了这里的 OLED_Clear()，告别闪屏！
                } else if (key_val == 3) { 
                    sys_mode = 3; sub_menu_index = 0; OLED_Clear(); // 切换界面保留清屏
                }
            } 
            else if (sys_mode == 3 && key_val > 0) { 
                if (key_val == 1) {
                    sys_mode = 2; OLED_Clear(); // 切换界面保留清屏
                }
                else if (key_val == 2) {
                    sub_menu_index++;
                    if(main_menu_index == 0 && sub_menu_index > 2) sub_menu_index = 0; 
                    else if(main_menu_index == 1 && sub_menu_index > 1) sub_menu_index = 0; 
                    else if(main_menu_index == 5 && sub_menu_index > 5) sub_menu_index = 0; 
                    else if(main_menu_index >= 2 && main_menu_index <= 4) sub_menu_index = 0; 
                    // ? 修复：去掉了这里的 OLED_Clear()，告别闪屏！
                } else if (key_val == 3 || key_val == 4) { 
                    int change = (key_val == 4) ? 1 : -1;
                    
                    switch(main_menu_index) {
                        case 0: 
                            if(sub_menu_index==0) calendar.hour = (calendar.hour + change + 24) % 24;
                            if(sub_menu_index==1) calendar.min = (calendar.min + change + 60) % 60;
                            if(sub_menu_index==2) calendar.sec = (calendar.sec + change + 60) % 60;
                            break;
                        case 1: 
                            if(sub_menu_index==0) temp_limit_min += change;
                            if(sub_menu_index==1) humi_limit_max += change;
                            break;
                        case 2: light_limit_max += (change * 100); break;
                        case 3: mq_limit_max += (change * 50); break;
                        case 4: pm25_limit_max += (change * 10); break; 
                        case 5: 
                            if(sub_menu_index==0) uv_start_h = (uv_start_h + change + 24) % 24;
                            if(sub_menu_index==1) uv_start_m = (uv_start_m + change + 60) % 60;
                            if(sub_menu_index==2) uv_start_s = (uv_start_s + change + 60) % 60;
                            if(sub_menu_index==3) uv_stop_h = (uv_stop_h + change + 24) % 24;
                            if(sub_menu_index==4) uv_stop_m = (uv_stop_m + change + 60) % 60;
                            if(sub_menu_index==5) uv_stop_s = (uv_stop_s + change + 60) % 60;
                            break;
                    }
                }
            }
        }
        
        // === 5. UI 分发执行 ===
        if(sys_mode == 0) {
            Auto_Control_Task(t, h, mq, lux, door, pm25_val);
            UI_Auto_Page(t, h, mq, lux, door);
        } else if(sys_mode == 1) {
            UI_Manual_Page();
        } else if(sys_mode == 2) {
            UI_Setting_Main_Page();
        } else if(sys_mode == 3) {
            UI_Setting_Sub_Page();
        }
        
        delay_ms(10); 
    }
}