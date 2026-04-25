#include "u_i2c.h"
#include "stm32f103xb.h"
#include <stdint.h>

typedef struct {
    I2C_TypeDef *I2C_id;
    GPIO_TypeDef *port;
    uint8_t i2c_rcc;
    uint8_t gpio_rcc;
    uint8_t scl_pin;
    uint8_t sda_pin;
}I2C_Config;

// ======================= 硬件配置 =======================

static const I2C_Config i2c_configs[I2C_COUNT] = {
    [I2C_1] = {I2C1, GPIOB, 21, 3, 6,7},
    [I2C_2] = {I2C2, GPIOB, 22, 3, 10,11},
}; 

// ======================= 硬件配置 =======================

static void clock_init(I2C_Config *cfg){
    RCC -> APB2ENR |= 1 << cfg->gpio_rcc;
    RCC -> APB1ENR |= 1 << cfg->i2c_rcc;
}

static void gpio_init(I2C_Config *cfg){
    if(cfg->scl_pin > 8 && cfg->sda_pin > 8){
        // 引脚复位
        cfg->port -> CRH &= ~(0x0F << (cfg->scl_pin-8)*4);
        cfg->port -> CRH &= ~(0x0F << (cfg->sda_pin-8)*4);

        // 复用开漏
        cfg->port -> CRH |= (0x0F << (cfg->scl_pin-8)*4);
        cfg->port -> CRH |= (0x0F << (cfg->sda_pin-8)*4);
    }else {
        // 引脚复位
        cfg->port -> CRL &= ~(0x0F << cfg->scl_pin*4);
        cfg->port -> CRL &= ~(0x0F << cfg->sda_pin*4);

        // 复用开漏
        cfg->port -> CRL |= (0x0F << cfg->scl_pin*4);
        cfg->port -> CRL |= (0x0F << cfg->sda_pin*4);
    }
}

static void i2c_config(I2C_Config *cfg){
    // I2C 软件复位
    cfg->I2C_id -> CR1 |= 1 << 15;
    cfg->I2C_id -> CR1 &= ~(1 << 15);

    // 配置APB1频率36Mhz
    cfg->I2C_id -> CR2 = 36;

    // 配置 100KHz
    cfg->I2C_id -> CCR = 180;
    cfg->I2C_id -> TRISE = 37;

    // 使能 I2C
    cfg->I2C_id -> CR1 |= 1 << 0;
}

void I2C_Init(I2C_ID id){
    
    const I2C_Config *cfg = &i2c_configs[id];

    clock_init(cfg);
    gpio_init(cfg);
    i2c_config(cfg);

}


/**
 * @brief  发送起始信号
 * @param  id -> I2C1 I2C2
 * @return 无
 */

void I2C_Start(I2C_ID id){
    const I2C_Config *cfg = &i2c_configs[id];  
    cfg->I2C_id -> CR1 |= 1 << 8; // 硬件起始
    // 等待硬件起始成功
    while(!(cfg->I2C_id -> SR1 & (1 << 0)));
}

/**
 * @brief  发送设备地址 等待ACK
 * @param  addr 设备地址 id I2C1 I2C2
 * @return 无
 */

void I2C_SendAddress(uint8_t addr, I2C_ID id){
    const I2C_Config *cfg = &i2c_configs[id];
    
    // 发送地址
    cfg->I2C_id -> DR = addr;
    
    // 等待ADDR标志 （地址发送完成 + 从机发送ACK）
    while(!(cfg->I2C_id -> SR1 & (1 << 1)));
    
    // 清除ADDR标志：读取SR1 SR2
    volatile uint32_t temp = cfg->I2C_id -> SR1;
    temp = cfg->I2C_id -> SR2;
    (void)temp;
}

/**
 * @brief  停止信号
 * @param  id ···
 * @return 无
 */
void I2C_Stop(I2C_ID id){
    const I2C_Config *cfg = &i2c_configs[id];
    cfg->I2C_id -> CR1 |= 1 << 9;
}

/**
 * @brief  发送一个字节的数据
 * @param  
 * @return 无
 */

void I2C_SendByte(uint32_t data, I2C_ID id){
    const I2C_Config *cfg = &i2c_configs[id];

    cfg->I2C_id -> DR = data;
    while(!(cfg->I2C_id -> SR1 & (1<<7)));  // 等待发送完成

    //SR1 和 SR2 清除状态标志
    uint32_t temp = cfg->I2C_id -> SR1;
    temp = cfg->I2C_id->SR2;
}

/*
写入流程

1. 发送 起始信号
2. 发送「设备地址+写位」
3. 等待从机 ACK
4. 发送 寄存器/命令地址
5. 等待 ACK
6. 发送 要写入的数据
7. 等待 ACK
8. 发送 停止信号

读取流程

1. 起始信号
2. 设备地址+写位 → 指定寄存器
3. ACK
4. 重复起始
5. 设备地址+读位
6. 读取1字节
7. 主机发送 NACK 结束读取  
8. 停止信号
*/
