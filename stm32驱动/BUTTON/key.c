#include "key.h"
#include "stm32f103xb.h"
#include <stdint.h>

// ======================= 硬件配置区 =======================

typedef struct{
    GPIO_TypeDef *port;
    uint8_t pin;
    uint8_t rcc_bit;
} KEY_Config;

static const KEY_Config key_table[KEY_COUNT] = {
    [KEY_TEST0] = {GPIOB, 3, 3}, // GPIOB PA3 RCC 3
    [KEY_TEST1] = {GPIOB, 4, 3}, // GPIOB PA4 RCC 3
    [KEY_TEST2] = {GPIOB, 5, 3} // GPIOB PA5 RCC 3
};

// ======================= 硬件配置区 =======================


 static void delay(void){
    for(int i=0; i<10000; i++);
 }


/**
 * @brief  按键是否处于按下状态
 * @param  按键id 
 * @return 1 按下 0 位处于按下状态
 */
static uint8_t key_IsDown(KEY_ID id){
    if(id >= KEY_COUNT) return 0;

    const KEY_Config *cfg = &key_table[id];
    
    if((cfg->port -> IDR &(1 << cfg->pin)) == 0){
        delay();
        if((cfg->port -> IDR &(1 << cfg->pin)) == 0){
            return 1;
        }
    }
    return 0;
}

void KEY_Init(KEY_ID id){
    if(id >= KEY_COUNT) return;
    const KEY_Config *cfg = &key_table[id];

    RCC -> APB2ENR |= (1 << cfg->rcc_bit); 

    if ((cfg ->pin) > 7) {
        const uint8_t pos = cfg->pin - 8;
        cfg->port -> CRH &= ~(0x0F << (4 * pos));
        cfg->port -> CRH |= (0x08 << (4 * pos));
        cfg->port -> ODR |= (1 << pos);
    }else {
        cfg->port -> CRL &= ~(0x0F << (4 * cfg->pin));
        cfg->port -> CRL |= (0x08 << (4 * cfg->pin));
        cfg->port -> ODR |= (1 << cfg->pin);
    }
}

uint8_t KEY_IsPressed(KEY_ID id){
    return key_IsDown(id);
}