# include "led.h"

/**
 * @brief  初始化LED对应的GPIO引脚
 * @param  无
 * @retval 无
 */
void LED_Init(void){
    // 开GPIOA时钟 
    RCC -> APB2ENR |= (1 << LED_RCC_BIT);

    if(LED_GPIO_PIN < 8){
        //CRL 
        // 清空引脚原有配置
        LED_GPIO_PORT -> CRL &= ~(0x0F << 4*(LED_GPIO_PIN));
        // 配置引脚为推挽输出
        LED_GPIO_PORT -> CRL |= (0x03 << 4*(LED_GPIO_PIN));

    }else{
        // CRH 
        uint8_t pos = LED_GPIO_PIN - 8;
        // 清空引脚原有配置
        LED_GPIO_PORT -> CRH &= ~(0x0F << 4*(pos));
        // 配置引脚为推挽输出
        LED_GPIO_PORT -> CRH |= (0x03 << 4*(pos));
    }

    // 初始引脚设为低电平
    LED_Off();
}

/**
 * @brief 点亮LED
 * @param 无
 * @retval 无
 */
void LED_On(void){
    // BSRR pin
    LED_GPIO_PORT -> BSRR = 1 << LED_GPIO_PIN;
}

/**
 * @brief 熄灭LED
 * @param 无
 * @retval 无
 */
void LED_Off(void){
    // BSRR pin+16
    LED_GPIO_PORT -> BSRR = 1 << (LED_GPIO_PIN + 16);
}

/**
 * @brief 翻转LED
 * @param 无
 * @retval 无
 */
 void LED_Toggle(void){
    // ODR & 1<<pin 
    LED_GPIO_PORT -> ODR ^= (1 << LED_GPIO_PIN);
 }