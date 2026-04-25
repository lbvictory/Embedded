#include "oled.h"
#include "u_i2c.h"


const uint8_t OLED_Num[10][6] = {
    {0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C}, // 0
    {0x00, 0x44, 0x7E, 0x40, 0x00, 0x00}, // 1
    {0x42, 0x62, 0x52, 0x4A, 0x46, 0x42}, // 2
    {0x22, 0x41, 0x41, 0x49, 0x49, 0x36}, // 3
    {0x10, 0x18, 0x14, 0x12, 0x7E, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x49, 0x36}, // 8（你现在用的）
    {0x06, 0x49, 0x49, 0x49, 0x29, 0x1E}  // 9
};

#define ID I2C_1


/* 向OLED中写命令 */
void OLED_WriteCmd(uint8_t cmd){
  I2C_Start(ID);
  I2C_SendAddress(0x78, ID);  // 从机地址
  I2C_SendByte(0x00, ID);     // 0x00=命令标志
  I2C_SendByte(cmd, ID);      // 命令内容
  I2C_Stop(ID);
}

/* 向OLED中写数据 */
void OLED_WriteData(uint8_t data){
  I2C_Start(ID);
  I2C_SendAddress(0x78,ID);
  I2C_SendByte(0x40,ID); // 0x40 = 数据标志位
  I2C_SendByte(data,ID);      // 数据内容
  I2C_Stop(ID);
}

/* 清屏 */
void OLED_Clear(){
  uint8_t page,col;

  // 8页：0-7 起始位置：0xB0
  for(page=0; page<8; page++){
    // 设置当前页
    OLED_WriteCmd(0xB0 + page);
    // 设置列起始位置 （低4位+高四位 0x00 + 0x10）
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x10);

    // 每一页 128 列 全写0
    for(col=0; col<128; col++){
      OLED_WriteData(0x00);
    }

  }

}


static void oled_init(){
    HAL_Delay(100);  // 上电等稳定

    // 1.关闭显示 0xAE
    OLED_WriteCmd(0xAE);

    // 2.配置时钟
    OLED_WriteCmd(0xD5);
    OLED_WriteCmd(0x80);

    // 3.配置64行屏
    OLED_WriteCmd(0xA8);
    OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xA1);  // 列映射
    OLED_WriteCmd(0xC8);  // 行映射

    // 4.开电荷泵
    OLED_WriteCmd(0x8D);
    OLED_WriteCmd(0x14);

    // 5.打开显示
    OLED_WriteCmd(0xAF);
}

void OLED_Init(){
    // I2C_1 PB6 PB7
    // I2C_2 PB10 PB11
    I2C_Init(ID);
    oled_init();
    OLED_Clear();
}

// 在第page页，第x列，显示一个6X8字符
static void OLED_ShowChar(uint8_t page, uint8_t x, const uint8_t *chr){
  uint8_t i;

  // 1.设置光标位置 
  OLED_WriteCmd(0xB0 + page); // 第几页
  OLED_WriteCmd(0x00 | (x & 0x0F)); // 列 低四位
  OLED_WriteCmd(0x10 | (x >> 4)); // 列 高四位

  // 2.连续写 6 字节的数据
  for(i=0; i<6; i++){
    OLED_WriteData(chr[i]);
  }
} 

// 显示字符串
void OLED_ShowStr(uint8_t y, uint8_t x, char *str){
  uint8_t index = 0;
  x =  x*8;
  while(str[index] != '\0'){
    if(str[index]>='0' && str[index] <= '9'){
      OLED_ShowChar(y, x, OLED_Num[str[index]-'0']);
      x += 8;
    }
    index++;
  }
}

