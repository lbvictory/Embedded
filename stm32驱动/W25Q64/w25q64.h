#ifndef __W25Q23_H
#define __W25Q23_H

#include "u_spi.h"

void W25Q64_Init(void);
uint16_t W25Q64_ReadID(void);
void W25Q64_SectorErase(uint32_t addr);
void W25Q64_ReadData(uint32_t addr, uint8_t *buf, uint16_t len);
void W25Q64_PageEdit(uint32_t addr, uint8_t *data, uint16_t len);

#endif
