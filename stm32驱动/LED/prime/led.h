#ifndef __LED_H
#define __LED_H

#include "stm32f1xx.h"

// LED 灯编号
typedef enum{
    LED_TEST,
    LED_TEST1,
    LED_TEST2,
    LED_COUNT
} LED_ID;


void LED_Init(LED_ID id);

void LED_On(LED_ID id);

void LED_Off(LED_ID id);

void LED_Toggle(LED_ID id);
#endif