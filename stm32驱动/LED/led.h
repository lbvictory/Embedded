#ifndef __LED_H
#define __LED_H

// 包含核心寄存器头文件
#include "stm32f1xx.h"

// ==================== 硬件配置区 ====================
#define LED_GPIO_PORT GPIOA      // LED 连接的端口
#define LED_GPIO_PIN 6           // LED 连接的GPIO引脚号 （6号引脚）
#define LED_RCC_BIT 2            // GPIO端口对应时钟的使能位（GPIOA 2）
// ===================================================

void LED_Init(void);        // LED初始化函数
void LED_On(void);          // 点亮LED
void LED_Off(void);         // 熄灭LED
void LED_Toggle(void);      // 反转LED状态
#endif /* __LED_H */