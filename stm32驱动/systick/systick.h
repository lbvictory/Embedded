#ifndef __SYSTICK_H
#define __SYSTICK_H

#include <stdint.h>

// 初始化SysTick，参数ms：中断周期（毫秒）
void SysTick_Init(uint32_t ms);

// 获取当前系统tick（毫秒）
uint32_t SysTick_GetTick(void);

#endif