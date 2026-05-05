#include "w25q64.h"
#include "u_spi.h"
#include <stdint.h>

void W25Q64_Init(){
    U_SPI_Init();
}


// 读取 W25Q32 的 JEDEC ID (0x9F 命令) —— 你也可以用 0x90，这里用标准命令
// 我们保留原来的 0x90 方式，但为了规范，也可以改成 0x9F。
// 这里保持和之前兼容：0x90 命令，3 个地址字节为 0，返回 2 字节 ID。
uint16_t W25Q64_ReadID(void){
    uint16_t id;
    CS_LOW();
    SPI1_TransmitByte(0x90);
    SPI1_TransmitByte(0x00);
    SPI1_TransmitByte(0x00);
    SPI1_TransmitByte(0x00);
    id = SPI1_TransmitByte(0xFF) << 8;
    id |= SPI1_TransmitByte(0xFF);
    CS_HIGH();
    return id;
}

// 读状态寄存器（命令 0x05）
// 返回值：bit0 = BUSY (1=忙), bit1 = WEL (写使能锁存)
static uint8_t W25Q64_ReadStatus(void){
    uint8_t status;
    CS_LOW();
    SPI1_TransmitByte(0x05);
    status = SPI1_TransmitByte(0xFF);
    CS_HIGH();
    return status;
}

// 打开写权限 （0x06）
// 在执行任何写/擦除操作之前，必须先发送此命令，将 WEL位 置 1
static void W25Q64_WriteEnable(void){
    CS_LOW();
    SPI1_TransmitByte(0x06);
    CS_HIGH();
}

// 扇区擦除（4kB对齐 0X20）
// 参数addr：要擦除的起始地址（低12位会被忽略，因为4kB=4096=0x1000）
// 注意：擦除耗时越45--150ms，函数内部会轮询到BUSY 位等待完成
void W25Q64_SectorErase(uint32_t addr){
    // 1. 写使能
    W25Q64_WriteEnable();
    // 2. 发送擦除指令
    CS_LOW();
    SPI1_TransmitByte(0x20);
    SPI1_TransmitByte((addr >> 16) & 0xFF);     // 地址高8位
    SPI1_TransmitByte((addr >> 8) & 0xFF);      // 地址中8位
    SPI1_TransmitByte(addr & 0xFF);             // 地址低8位
    CS_HIGH();
    // 3. 等待擦除完成（BUSY 位清零）
    while (W25Q64_ReadStatus() & 0x01);
}

// 页编程（最多写入 256 字节，命令 0x02）
// 注意：写入前必须确保目标扇区已被擦除（全为 0xFF），否则写入结果不可预知。
// 函数不会自动处理跨页边界，调用时请保证同一页内（地址低 8 位 + len ≤ 256)
void W25Q64_PageEdit(uint32_t addr, uint8_t *data, uint16_t len){
    if(len > 256) len = 256; // 页大小限制
    W25Q64_WriteEnable();
    CS_LOW();
    SPI1_TransmitByte(0x02); // 页编辑
    SPI1_TransmitByte((addr >> 16) & 0xFF);
    SPI1_TransmitByte((addr >> 8) & 0xFF);
    SPI1_TransmitByte(addr & 0xFF);
    for(uint16_t i=0; i<len; i++){
        SPI1_TransmitByte(data[i]); // 发送数据
    }
    CS_HIGH();
    while(W25Q64_ReadStatus() & 0x01); // 等待写入完成
}

// 读取数据（命令 0x03）
// 可以任意地址，任意长度（最大整片 4MB）
void W25Q64_ReadData(uint32_t addr, uint8_t *buf, uint16_t len){
    CS_LOW();
    SPI1_TransmitByte(0x03);
    SPI1_TransmitByte((addr >> 16) & 0xFF);
    SPI1_TransmitByte((addr >> 8) & 0xFF);
    SPI1_TransmitByte(addr & 0xFF);
    for(uint16_t i=0; i<len; i++){
        buf[i] = SPI1_TransmitByte(0xFF);
    }
    CS_HIGH();
}
