#ifndef __DHT11__
#define __DHT11__

#include "stm32f1xx.h"

typedef enum{
    DHT11_1,
    DHT11_COUNT
} DHT11_id;

void DHT11_Init(DHT11_id id);

uint8_t DHT11_Get_Data(uint8_t *temp, uint8_t *humi, DHT11_id id);

#endif