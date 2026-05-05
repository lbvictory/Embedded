#ifndef __U_SPI
#define __U_SPI

#include "stm32f1xx.h"
#include "stm32f103xb.h"
#include <stdint.h>

#define CS_LOW() GPIOA->BSRR = (1 << 20)
#define CS_HIGH() GPIOA->BSRR = (1 << 4)

void U_SPI_Init(void);
void test_fun(void);
uint8_t SPI1_TransmitByte(uint8_t tx);

#endif