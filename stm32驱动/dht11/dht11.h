#ifndef __DHT11__
#define __DHT11__

#include "stm32f1xx.h"

void DHT11_Init();

uint8_t DHT11_Get_Data(uint8_t *temp, uint8_t *humi);

#endif