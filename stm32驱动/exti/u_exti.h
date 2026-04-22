#ifndef __U_EXTI_H
#define __U_EXTI_H

#include "stm32f103xb.h"

void U_EXTI_Init(void);
uint8_t U_EXTI_GetKey1Flag(void);
uint8_t U_EXTI_GetKey2Flag(void);
#endif