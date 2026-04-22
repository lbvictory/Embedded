#include "dht11.h"
#include "time.h"
#include <stdint.h>


static void clock_init(){
    // 开GPIOA
    RCC -> APB2ENR |= 1 << 2;
}

static void gpio_init(){
    // PA6 推挽输出
    GPIOA -> CRL &= ~(1 << 24);
    GPIOA -> CRL |= (0x03 << 24);
    GPIOA -> BSRR = (1 << 6);
}

void DHT11_Init(){
    clock_init();
    gpio_init();
}

/*
  起始信号：
  主机起始信号
  拉低总线：≥ 18 ms
  拉高总线：20 ~ 40 μs
*/
static void dht11_start_info(void){
    // 拉低总线
    GPIOA -> BSRR = 1 << (6 + 16);
    // 延时30ms
    Delay_ms(30);
    // 拉高
    GPIOA -> BSRR = 1 << 6;
    Delay_us(30);   
}

/*
DHT11 应答信号
拉低总线：约 80 μs
拉高总线：约 80 μs
*/
static uint8_t dht11_recieve_info(){
    uint16_t time_out = 40000;

    // 切换未浮空输入
    GPIOA -> CRL &= ~(1 << 24);
    GPIOA -> CRL |= (0x04 << 24);

    while((GPIOA -> IDR & (1 << 6)) && time_out--);
    if(time_out == 0) return 1;

    time_out = 40000;

    while(!(GPIOA -> IDR & (1 << 6)) && time_out--);
    if(time_out == 0) return 1;

    Delay_us(100);
    
    return 0;
}

/**
 * @brief  读取一位数据
 * @param  NULL
 * @return 返回一位数据 0 或 1
 */
static uint8_t dht11_read_bit(){
    uint8_t bit = 0;
    uint16_t time_out = 10000;

    while(!(GPIOA -> IDR & (1 << 6)) && time_out--);
    if(time_out == 0) return 3;

    Delay_us(40);

    bit = (GPIOA -> IDR & 1 << 6) ? 1:0;

    time_out = 10000;
    while((GPIOA -> IDR & (1 << 6)) && time_out--);
    if(time_out == 0) return 3;

    return bit;
}

/**
 * @brief  读取一字节数据
 * @param  NULL
 * @return 返回一字节
 */

static uint8_t dht11_read_byte(){
    uint8_t i, byte=0;
    uint8_t bit = 0;
    for(i=0; i < 8; i++){
        bit = dht11_read_bit();
        if(bit == 3) return 3;
        byte <<= 1;
        byte |= bit;
    }

    return byte;
}

/**
 * @brief  获取温度 湿度
 * @param  temp 存取温度的地址 humi 湿度的地址
 * @return  成功读取数据0 数据读取失败3 应答失败 2 数据校验错误 1
 */

uint8_t DHT11_Get_Data(uint8_t *temp, uint8_t *humi){
    uint8_t buf[5]={0};
    GPIOA->CRL &= ~(0x0F << 24);
    GPIOA->CRL |= (0x03 << 24);
    GPIOA->BSRR = 1 << 6;
    Delay_ms(10);

    // 发送起始信号
    dht11_start_info();

    // 检测应答
    if(dht11_recieve_info() != 0) return 2;

    // 读取五个字节的的数据
    for(int i=0; i<5; i++){
        buf[i] = dht11_read_byte();
        if(buf[i] == 3) return 3;
    }

    // 数据校验
    uint8_t sum = buf[0] + buf[1] + buf[2] + buf[3];
    if(sum == buf[4]){
        *temp = buf[2];
        *humi = buf[0];
        return 0;
    }
    return 1;
}