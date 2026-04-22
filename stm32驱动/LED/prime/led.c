#include "led.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_def.h"
#include <stdint.h>

// ======================= 硬件配置区 =======================

typedef struct{
    GPIO_TypeDef *port;
    uint8_t pin;
    uint8_t rcc_bit;
}LED_Config;

static const LED_Config led_table[LED_COUNT]={
    [LED_TEST] = {GPIOA, 5, 2},  // GPIOA PA5
    [LED_TEST1] = {GPIOA, 6, 2},  // GPIOA PA6
    [LED_TEST2] = {GPIOA, 7, 2},  // GPIOA PA7

};
// ======================= 硬件配置区 =======================


static void led_gpio_init(const LED_Config *cfg){
    // 开启对应GPIO总线的时钟如GPIOA 2
    RCC -> APB2ENR |= (1 << cfg->rcc_bit);
    const uint8_t pos = cfg->pin - 8;
    
// 配置引脚为推挽输出，50mhz
    if(cfg->pin > 7){
        cfg -> port -> CRH &= ~(0xF << (4*pos));
        cfg->port -> CRH |= (0x03 << (4*pos));
    }else{
        cfg -> port -> CRL &= ~(0xF << (4 * cfg->pin));
        cfg->port -> CRL |= (0x03 << (4 * cfg->pin));
    }
}


/**
 * @brief  LED初始化
 * @param  LED id
 * @return 无
 */

void LED_Init(LED_ID id){
    if(id>= LED_COUNT) return;

    const LED_Config *cfg = &led_table[id];
    led_gpio_init(cfg);
    LED_Off(id);
}

void LED_On(LED_ID id){
    if(id >= LED_COUNT) return;

    const LED_Config *cfg = &led_table[id];

    cfg->port -> BSRR = (1 << (cfg->pin));
}

void LED_Off(LED_ID id){
    if(id >= LED_COUNT) return;

    const LED_Config *cfg = &led_table[id];
    const uint8_t pos = cfg->pin + 16;

    cfg->port -> BSRR = (1 << pos);
}

void LED_Toggle(LED_ID id){
    if(id >= LED_COUNT) return;

    const LED_Config *cfg = &led_table[id];

    cfg->port -> ODR ^= (1 << cfg->pin);
}