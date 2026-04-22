#ifndef __U_UART_H
#define __U_UART_H

#include "stm32f103xb.h"
#include <stdint.h>

void U_UART_Init(void);

void Recive_Data_Completed(void);

void UART1_SendString(char *str);

#endif 