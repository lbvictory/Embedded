#include "dht11.h"
#include "stm32f103xb.h"
#include "time.h"
#include <iso646.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct{
    GPIO_TypeDef *port;
    uint8_t pin;
    uint8_t rcc_bit;  // 需要使用的引脚总线GPIOA2 GPIOB 3 GPIOC 4
} DHT11_Config;

// ======================= 硬件配置区 =======================

static const DHT11_Config dht11_table[DHT11_COUNT]={
    [DHT11_1] = {GPIOA, 6, 2},
};

// ======================= 硬件配置区 =======================


/**
 * @brief  初始化时钟
 * @param  
 * @return 无
 */

static void clock_init(const DHT11_Config *cfg){
    // 开GPIOA
    RCC -> APB2ENR |= 1 << (cfg->rcc_bit);
}

/**
 * @brief  初始化引脚
 * @param  
 * @return 无
 */

static void gpio_init(const DHT11_Config *cfg){
    // DHT11引脚 推挽输出
    const uint8_t temp = cfg->pin -8;

    if(cfg->pin < 8){
        cfg->port -> CRL &= ~(0x0F << ((cfg->pin) * 4));
        cfg->port -> CRL |= (0x03 << ((cfg->pin) * 4));
        cfg->port -> BSRR = (1 << cfg->pin);
    }else {
        cfg->port -> CRH &= ~(0x0F << (temp * 4));
        cfg->port -> CRH |= (0x03 << (temp * 4));
        cfg->port -> BSRR = (1 << cfg->pin); 
    }
    
    
}

/**
 * @brief  更换引脚模式
 * @param  
 * @return 无
 */

static void change_gpio(const DHT11_Config *cfg){
    const uint8_t temp = cfg->pin - 8;
    if(cfg->pin < 8){
        cfg->port -> CRL &= ~(0x0F << (cfg->pin * 4));
        cfg->port -> CRL |= (0x04 << (cfg->pin * 4));
    }else {
        cfg->port -> CRH &= ~(0x0F << (temp * 4));
        cfg->port -> CRH |= (0x04 << (temp * 4));
    }
}

/**
 * @brief  重置引脚
 * @param  
 * @return 无
 */

static void reset_gpio(const DHT11_Config *cfg){
    const uint8_t temp = cfg->pin - 8;
    if(cfg->pin < 8){
        cfg->port->CRL &= ~(0x0F << (cfg->pin * 4));
        cfg->port->CRL |= (0x03 << (cfg->pin * 4));
        cfg->port->BSRR = 1 << cfg->pin;
        Delay_ms(10);
    }else {
        cfg->port->CRH &= ~(0x0F << (temp * 4));
        cfg->port->CRH |= (0x03 << (temp * 4));
        cfg->port->BSRR = 1 << cfg->pin;
        Delay_ms(10);
    }
}

/**
 * @brief  初始化DHT11
 * @param  
 * @return 无
 */

void DHT11_Init(DHT11_id id){
    const DHT11_Config *cfg = &dht11_table[id];
    clock_init(cfg);
    gpio_init(cfg);
}

/*
  起始信号：
  主机起始信号
  拉低总线：≥ 18 ms
  拉高总线：20 ~ 40 μs
*/

/**
 * @brief  发送起始信号
 * @param  
 * @return 无
 */

static void dht11_start_info(const DHT11_Config *cfg){
    const uint8_t temp = cfg->pin + 16;

    // 拉低总线
    cfg->port -> BSRR = 1 << temp;
    // 延时30ms
    Delay_ms(30);
    // 拉高
    cfg->port -> BSRR = 1 << cfg->pin;
    Delay_us(30);   
}

/*
DHT11 应答信号
拉低总线：约 80 μs
拉高总线：约 80 μs
*/

/**
 * @brief  检查应答
 * @param  
 * @return 无
 */

static uint8_t dht11_recieve_info(const DHT11_Config *cfg){
    uint16_t time_out = 40000;

    // 切换为浮空输入
    
    change_gpio(cfg);

    while((cfg->port -> IDR & (1 << cfg->pin)) && time_out--);
    if(time_out == 0) return 1;

    time_out = 40000;

    while(!(cfg->port -> IDR & (1 << cfg->pin)) && time_out--);
    if(time_out == 0) return 1;

    Delay_us(100);
    
    return 0;
}

/**
 * @brief  读取一位数据
 * @param  NULL
 * @return 返回一位数据 0 或 1
 */
static uint8_t dht11_read_bit(const DHT11_Config *cfg){
    uint8_t bit = 0;
    uint16_t time_out = 10000;

    while(!(cfg->port -> IDR & (1 << cfg->pin)) && time_out--);
    if(time_out == 0) return 3;

    Delay_us(40);

    bit = (cfg->port -> IDR & 1 << cfg->pin) ? 1:0;

    time_out = 10000;
    while((cfg->port -> IDR & (1 << cfg->pin)) && time_out--);
    if(time_out == 0) return 3;

    return bit;
}

/**
 * @brief  读取一字节数据
 * @param  NULL
 * @return 返回一字节
 */

static uint8_t dht11_read_byte(const DHT11_Config *cfg){
    uint8_t i, byte=0;
    uint8_t bit = 0;
    for(i=0; i < 8; i++){
        bit = dht11_read_bit(cfg);
        if(bit == 3) return 3;
        byte <<= 1;
        byte |= bit;
    }

    return byte;
}

/**
 * @brief  获取温度 湿度
 * @param  temp 存取温度的地址 humi 湿度的地址 id DHT11的id
 * @return  成功读取数据0 数据读取失败3 应答失败 2 数据校验错误 1
 */

uint8_t DHT11_Get_Data(uint8_t *temp, uint8_t *humi, DHT11_id id){
    uint8_t buf[5]={0};
    const DHT11_Config *cfg = &dht11_table[id];
    reset_gpio(cfg);

    // 发送起始信号
    dht11_start_info(cfg);

    // 检测应答
    if(dht11_recieve_info(cfg) != 0) return 2;

    // 读取五个字节的的数据
    for(int i=0; i<5; i++){
        buf[i] = dht11_read_byte(cfg);
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