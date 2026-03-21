#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "sys.h"

void Key_Init(void);
u8 Key_Scan(void);
u8 Key_Scan_Long(void);

#endif