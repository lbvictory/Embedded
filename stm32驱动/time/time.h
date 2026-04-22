#ifndef __TIME_H
#define __TIME_H

#include "stm32f1xx.h"

void TIME_Init();

void Delay_us(uint16_t nus);

void Delay_ms(uint16_t nms);

#endif