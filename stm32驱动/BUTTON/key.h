#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx.h"
#include <stdint.h>

// KEY id 按键编号
typedef enum{
    KEY_TEST0,
    KEY_TEST1,
    KEY_TEST2,
    KEY_COUNT
}KEY_ID;

void KEY_Init(KEY_ID id);

uint8_t KEY_IsPressed(KEY_ID id);

#endif