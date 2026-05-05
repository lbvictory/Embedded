#include "u_spi.h"
#include <stdint.h>
#include "u_uart.h"

static void clock_init(void){
    RCC -> APB2ENR |= 1 << 12; // SPI1
    RCC -> APB2ENR |= 1 << 2; // GPIOA
}

static void gpio_init(void){
    // PA5 SCK 复用推挽 0x0B
    // PA7 MOSI 复用推挽 0x0B
    // PA6 MISO 浮空输入 0x04
    // PA4 CS 通用推挽输出 0x03

    GPIOA -> CRL &= ~((0x0F << 16) | (0x0F << 20) | (0x0F << 24) | (0x0F << 28));
    // PA5
    GPIOA -> CRL |= (0x0B <<  20);
    // PA7 
    GPIOA -> CRL |= (0x0B << 28);
    // PA6
    GPIOA -> CRL |= (0x04 << 24);
    // PA4
    GPIOA -> CRL |= (0x03 << 16);
    // 空闲PA4 高电平
    GPIOA -> BSRR = (1 << 4);
}

static void spi_config(void){
    // 复位SPI
    SPI1 -> CR1 = 0;
    // 设置为主机模式
    SPI1 -> CR1 |= (1 << 2);
    // 模式0
    SPI1 -> CR1 |= (0 << 1) | (0 << 0);
    // 分频 010 8 9MHz
    SPI1 -> CR1 |= (2 << 3);
    // 软件NSS
    SPI1 -> CR1 |= (1 << 9) | (1 << 8);
    // 开启SPI1 （使能）
    SPI1 -> CR1 |= (1 << 6);
}

void U_SPI_Init(void){
    clock_init();
    gpio_init();
    spi_config();
}

// 核心收发函数：发送一个字节，同时接收一个字节
uint8_t SPI1_TransmitByte(uint8_t tx){
    // 等待 TXE=1 （发送缓冲区空）
    while(!(SPI1 -> SR & (1 << 1)));
    // 写入数据寄存器，启动传输
    SPI1 -> DR = tx;
    // 等待RXNE=1 （接收非空）
    while(!(SPI1 -> SR & (1 << 0)));
    // 返回接收到的数据
    return SPI1 -> DR;
}

void test_fun(void){
    CS_LOW();                      // PA4 拉低
    SPI1_TransmitByte(0x55);       // 发送 0x55
    CS_HIGH();                     // PA4 拉高
    HAL_Delay(500);                 // 每500ms一次，方便触发
    UART1_SendString("这个是测试函数\r\n");
}