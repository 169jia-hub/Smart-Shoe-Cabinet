#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H
#include "sys.h"

// ================= 输出设备 =================
// 1. 加热模块 (直接接 PA5, 高电平开启)
#define HEATER_PIN      5   // PA5
#define RELAY_HEATER      PAout(HEATER_PIN)

// 2. 继电器设备 (风扇、消毒、照明)
#define FAN_PIN         6   // PA6 (通风)
#define UV_PIN          7   // PA7 (消毒)
#define LIGHT_PIN       8   // PA8 (照明)

#define RELAY_FAN       PAout(FAN_PIN)
#define RELAY_UV        PAout(UV_PIN)
#define RELAY_LIGHT     PAout(LIGHT_PIN)

// 3. 蜂鸣器
#define BEEP_PIN        1   // PB1
#define BEEP            PBout(BEEP_PIN)

// ================= 输入设备 =================
#define DOOR_PIN        PBin(15) // 红外 PB15

// ================= 按键 =================
// SW3->PB13, SW4->PB7, SW5->PB8, SW6->PB9
#define KEY_MENU        PBin(13)
#define KEY_UP          PBin(7)
#define KEY_DOWN        PBin(8)
#define KEY_BACK        PBin(9)

#endif