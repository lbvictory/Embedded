#include "u_exti.h"
#include "stm32f103xb.h"
#include "time.h"
#include <stdint.h>

volatile uint8_t key_press1_flag = 0;
volatile uint8_t key_press2_flag = 0;

static void gpio_init(){
    GPIOB -> CRL &= ~(0xF << 12);  // PB3
    GPIOB -> CRL &= ~(0xF << 16);  // PB4

    // 上拉输入
    GPIOB -> CRL |= (0x8 << 12);
    GPIOB -> CRL |= (0x8 << 16);
    
    // 开启上拉电阻
    GPIOB -> ODR |= (1 << 3);
    GPIOB -> ODR |= (1 << 4);
}

static void exti_config(){
    // PB3 映射到 EXTI3   → EXTICR[0]
    AFIO -> EXTICR[0] &= ~(0x0F << 12);
    AFIO -> EXTICR[0]|= (0x01 << 12);
    // PB4 映射到 EXTI4   → EXTICR[1]
    AFIO -> EXTICR[1] &= ~(0x0F << 0);
    AFIO -> EXTICR[1] |= (0x01 << 0);

    EXTI -> IMR |= (1 << 3);
    EXTI -> FTSR |= (1 << 3);

    EXTI -> IMR |= (1 << 4);
    EXTI -> FTSR |= (1 << 4);

    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_SetPriority(EXTI3_IRQn, 10);

    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_SetPriority(EXTI4_IRQn, 13);
}


void U_EXTI_Init(){
    RCC -> APB2ENR |= 1 << 3;
    RCC -> APB2ENR |= 1 << 0;
    __DSB();
    gpio_init();
    exti_config();

}

void EXTI4_IRQHandler(void){
    // 清除中断标志位
    EXTI -> PR |= 1 << 4;

    Delay_ms(20);

    if((GPIOB -> IDR & (1 << 4)) == 0){
        key_press2_flag = 1;
    }
}

void EXTI3_IRQHandler(void){
    // 清除中断标志位
    EXTI -> PR |= 1 << 3;

    Delay_ms(20);

    if((GPIOB -> IDR & (1 << 3)) == 0){
        key_press1_flag = 1;
    }
}

uint8_t U_EXTI_GetKey1Flag(void) {
    if (key_press1_flag) {
        key_press1_flag = 0;
        return 1;
    }
    return 0;
}

uint8_t U_EXTI_GetKey2Flag(void) {
    if (key_press2_flag) {
        key_press2_flag = 0;
        return 1;
    }
    return 0;
}
