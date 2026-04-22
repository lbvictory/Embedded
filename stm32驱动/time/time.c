#include "time.h"
#include "stm32f103xb.h"
#include <stdint.h>

static uint8_t time_inited = 0;

static void base_time(){
    // 复位TIM2
    TIM2 -> CR1 = 0x0000;
    // 清除所有状态标志位
    TIM2 -> SR = 0x0000;

    // 设置预分频器
    TIM2 -> PSC = 71;

    // 设置重装计数器最大值
    TIM2 -> ARR = 0xFFFF;

    TIM2 -> EGR |= (1<<0);
    //清除标志
    TIM2 -> SR &=~(1 << 0);
    // 计数器置为0
    TIM2 -> CNT = 0;
}

void TIME_Init(){
    if(time_inited) return;
    time_inited = 1;

    RCC -> APB1ENR |= (1<<0);
    __DSB();
    base_time();
}

/**
 * @brief  微秒级延时
 * @param  延时 nus 微秒
 * @return 无
 */
void Delay_us(uint16_t nus){
    if(nus == 0) return;
    // 计数器置为0
    TIM2 -> CNT = 0;
    // 让计数器从0开始计数
    TIM2 -> CR1 |= (1 << 0);
    // 直至计数到nus
    while(TIM2 -> CNT < nus);
    // 计数结束，停止计数
    TIM2 -> CR1 &= ~(1 << 0);
}

/**
 * @brief  毫秒级延时
 * @param  延时 nms 毫秒
 * @return 无
 */
void Delay_ms(uint16_t mus){
    if(mus == 0) return;
    while (mus--) {
        Delay_us(1000);
    }
}