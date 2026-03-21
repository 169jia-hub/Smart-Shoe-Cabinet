#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H
#include "sys.h"

// ================= 输出设备 (继电器/蜂鸣器) =================
// 继电器由三极管驱动，原理图显示高电平导通 (NPN)
#define HEATER_PIN      5   // PA5 加热
#define FAN_PIN         6   // PA6 通风
#define UV_PIN          7   // PA7 消毒
#define LIGHT_PIN       8   // PA8 照明

// 蜂鸣器 PB1
#define BEEP_PIN        1   // PB1

// 控制宏
#define RELAY_HEATER    PAout(HEATER_PIN)
#define RELAY_FAN       PAout(FAN_PIN)
#define RELAY_UV        PAout(UV_PIN)
#define RELAY_LIGHT     PAout(LIGHT_PIN)
#define BEEP            PBout(BEEP_PIN)

// ================= 输入设备 (传感器/按键) =================
// 1. 模拟量 (ADC)
// 光敏 -> PA0 (ADC1_IN0)
// MQ135 -> PA4 (ADC1_IN4)

// 2. 数字量 (GPIO)
// DHT11 -> PB12
// 红外(门磁) -> PB15 (假设: 1=关门, 0=开门，需实测，这里暂定1为关)
#define DOOR_PIN        PBin(15) 

// 3. 按键 (PB13, PB14, PB8, PB9)
// 注意：SW4 原理图是 PB7，但与 OLED 冲突，代码中改用 PB14
#define KEY_MENU_PIN    13  // SW3 (PB13)
#define KEY_UP_PIN      14  // SW4 (PB14) - 修改了这里！
#define KEY_DOWN_PIN    8   // SW5 (PB8)
#define KEY_BACK_PIN    9   // SW6 (PB9)

#define KEY_MENU        PBin(KEY_MENU_PIN)
#define KEY_UP          PBin(KEY_UP_PIN)
#define KEY_DOWN        PBin(KEY_DOWN_PIN)
#define KEY_BACK        PBin(KEY_BACK_PIN)

#endif