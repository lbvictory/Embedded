#include "u_uart.h"
#include "stm32f103xb.h"
#include <stdint.h>
#include <string.h>
#include <sys/syslimits.h>

static uint8_t uart1_recv_buf[32];
static uint8_t uart1_recv_len = 0;
static uint8_t uart1_recv_complete = 0;


static void clock_init(){
    // 使用uart1
    RCC -> APB2ENR |= 1 << 14;
    RCC -> APB2ENR |= 1 << 2;   // GPIOA
    RCC -> APB2ENR |= 1 << 0;   // AFIO
}

/**
 * @brief  串口用到的引脚初始化
 * @param  无
 * @return 无
 */
static void gpio_init(){
    // PA9 TX PA10 RX
    GPIOA -> CRH &= ~(0x0F << 4);
    GPIOA -> CRH &= ~(0x0F << 8);

    GPIOA -> CRH |= (0x0B << 4); // 复用推挽输出
    GPIOA -> CRH |= (0x04 << 8); // 浮空输入

}

static void uart_config(){
    // 清除UART1的原有配置
    USART1 -> CR1 &= ~(1 << 13);

    // 配置帧格式: 8位数据 + 无校验 + 1位停止
    USART1 -> CR1 &= ~(1 << 12); // 数据位 8位 M位0
    USART1 -> CR1 &= ~(1 << 10); // 数据位 无位 PCE位0
    USART1 -> CR2 &= ~(3 << 12); // 停止位 1位 STOP位 00
    
    // 波特率
    // USARTDIV = 主频 / (16 * 波特率)
    // 72MHz / (16*115200) ≈ 39.0625 → 整数部分=39=0x27，小数部分=0.0625*16=1
    USART1 -> BRR = 0x271;
    // 4. 开启发送+接收+USART总开关
    USART1->CR1 |= (1 << 3);    // TE=1：开启发送
    USART1->CR1 |= (1 << 2);    // RE=1：开启接收
    USART1->CR1 |= (1 << 13);   // UE=1：开启USART1

    // 开启串口中断
    USART1 -> CR1 |= 1 << 5;  // RXNEIE=1接受中断使能
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 14);

}


void U_UART_Init(){
    clock_init();
    gpio_init();
    uart_config();
}


void UART1_SendString(char *str){
    while (*str != '\0') {
        // 1. 看状态 → 空了吗？
        while((USART1 -> SR & (1 << 7)) == 0);
        // 2. 往DR塞字节 → 硬件自动发
        USART1 -> DR = *str++;
        // 3. 看状态 → 发完了吗？
        while((USART1 -> SR & (1 << 6)) == 0);
    }
}

void Get_Receive_Data(char *data){
    if(uart1_recv_complete){
        for(int i=0; i < uart1_recv_len; i++){
            *(data+i) =  uart1_recv_buf[i];
        }
        data[uart1_recv_len] = '\0';

        uart1_recv_complete = 0;
    }
    
}

void USART1_IRQHandler(void){
    if(USART1 -> SR & (1 << 5)){
     
        uint8_t data = USART1 -> DR;

        if(data == '\r'){
            uart1_recv_buf[uart1_recv_len] = '\0';
            uart1_recv_complete = 1;
            uart1_recv_len = 0;
        }else if(data == '\n')
        {
            return; // 丢掉，不处理
        }
        else{
            if(uart1_recv_len < 31)
            {
                uart1_recv_buf[uart1_recv_len++] = data;
            }
        } 
    }
}
