#include "systick.h"
#include "stm32f103xb.h"


static volatile uint32_t system_tick = 0;

/**
 * @brief  初始化SysTick定时中断
 * @param  ms 中断间隔时间，单位：毫秒
 *          例如：ms=10  → 每10ms进一次中断
 *                ms=1   → 每1ms进一次中断
 *                ms=100 → 每100ms进一次中断
 * @return 无
 * @note   SysTick是ARM内核自带的定时器，不需要配置时钟和引脚
 * @note   使用该函数后，主循环不再需要阻塞式延时
 * 
 * @example
 *     SysTick_Init(10);  // 每10ms进一次中断
 *     while(1) {
 *         uint32_t now = SysTick_GetTick();
 *         if((now - last) >= 100) {  // 100次×10ms=1000ms
 *             DoSomething();
 *             last = now;
 *         }
 *     }
 */
void SysTick_Init(uint32_t ms) {
    // 假设系统时钟72MHz
    // 计算重装载值：时间(秒) × 时钟频率 - 1
    uint32_t load = (ms * 72000) - 1;  // ms * (72MHz/1000)
    
    SysTick->LOAD = load;      // 设置重装载值
    SysTick->VAL = 0;          // 清空当前计数值
    SysTick->CTRL = 0x07;      // 使能+中断+系统时钟
    
    __enable_irq();            // 开启总中断
}

uint32_t SysTick_GetTick(void) {
    return system_tick;
}

void SysTick_Handler(void) {
    system_tick++;  // 每ms增加一次
    // 注意：如果Init时ms=10，这里每10ms加1，单位是10ms
    // 想要单位是ms，需要每次加ms值
}
