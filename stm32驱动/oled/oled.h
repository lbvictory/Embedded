#ifndef __OLED
#define __OLED

#include "u_i2c.h"

void OLED_ShowStr(uint8_t y, uint8_t x, char *str);
void OLED_Init();

#endif