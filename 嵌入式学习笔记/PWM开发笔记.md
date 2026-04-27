# PWM

# 一、PWM 初始化只有 5 步（永远不变）

## 第 1 步：开时钟

```c
// 定时器时钟
RCC->APB1ENR |= 1<<0; // TIM2
// 或 RCC->APB1ENR |=1<<1; // TIM3
// 或 RCC->APB1ENR |=1<<2; // TIM4
// GPIO时钟
RCC->APB2ENR |=1<<2; // GPIOA
```

---

## 第 2 步：配置 GPIO（永远一样）

```c
GPIOx->CRL &= ~(0xF << 位移);
GPIOx->CRL |= (0xB << 位移); // 复用推挽输出
```

**记住：PWM 引脚一律 0xB！**
---

## 第 3 步：TIM 基础配置（永远一样）

```c
TIMx->PSC = 71; // 固定
TIMx->ARR = 数值; // 决定频率
TIMx->CR1 |= 1<<7; // ARR预装载
```

---

## 第 4 步：PWM 模式配置（万能！）

### 通道 1 / 2 用 CCMR1

### 通道 3 / 4 用 CCMR2

### 【CH1、CH2 固定写法】

```c
// CH1
TIMx->CCMR1 &= ~(7<<4); // 清空
TIMx->CCMR1 |= 6<<4; // PWM模式1
TIMx->CCMR1 |= 1<<3; // 预装载

// CH3
TIMx->CCMR2 &= ~(7<<4); // 清空
TIMx->CCMR2 |= 6<<4; // PWM模式3
TIMx->CCMR2 |= 1<<3; // 预装载
```

```c
// CH2
TIMx->CCMR1 &= ~(7<<12); // 清空
TIMx->CCMR1 |= 6<<12; // PWM模式1
TIMx->CCMR1 |= 1<<11; // 预装载
```

### 规律：

- CH1：**4位偏移**
- CH2：**12位偏移**

---

## 第 5 步：通道使能（万能！）

```c
CH1: TIMx->CCER |= 1<<0;
CH2: TIMx->CCER |= 1<<4;
CH3: TIMx->CCER |= 1<<8;
CH4: TIMx->CCER |= 1<<12;
```

**规律：每个通道偏移 4 位！**
---

# 二、终极规律（你记这 3 条，所有定时器通吃）

## 1. CCMR1 管 CH1、CH2

- CH1：偏移 **4**

- CH2：偏移 **12**
  
  ## 2. CCMR2 管 CH3、CH4

- CH3：偏移 **4**

- CH4：偏移 **12**
  
  ## 3. CCER 通道使能

- CH1: 0

- CH2: 4

- CH3: 8

- CH4: 12
  **全部 +4！**

---

# 三、你现在随便换定时器

我给你演示：

## TIM3_CH1 → 直接套

```c
TIM3->CCMR1 &= ~(7<<4);
TIM3->CCMR1 |= 6<<4;
TIM3->CCMR1 |= 1<<3;
TIM3->CCER |= 1<<0;
```

## TIM4_CH2 → 直接套

```c
TIM4->CCMR1 &= ~(7<<12);
TIM4->CCMR1 |= 6<<12;
TIM4->CCMR1 |= 1<<11;
TIM4->CCER |= 1<<4;
```

---

## 四、你之前为什么懵？

因为你**没看到规律**，以为每个定时器都不一样。
**其实它们是一个模子刻出来的！**

---

## 五、我现在给你一段

### **万能可复制 PWM 驱动（任何TIM、任何通道都能套）**

```c
// 时钟
RCC->APB1ENR |= 1<<0; // TIM2
GPIOA->CRL &= ~(0xF<<4);
GPIOA->CRL |= 0xB<<4;
// 时基
TIM2->PSC = 71;
TIM2->ARR = 1999;
TIM2->CR1 |= 1<<7;
// PWM模式（CH2）
TIM2->CCMR1 &= ~(7<<12);
TIM2->CCMR1 |= 6<<12;
TIM2->CCMR1 |= 1<<11;
// 使能通道2
TIM2->CCER |= 1<<4;
// 启动
TIM2->EGR |= 1<<0;
TIM2->CR1 |= 1<<0;
```

---

### 默认固定引脚（必须背）

- **TIM2_CH1 → PA0**
- **TIM2_CH2 → PA1**
- **TIM3_CH1 → PA6** ✅
- **TIM3_CH2 → PA7**
- **TIM4_CH1 → PB6

### PWM 舵机案例

```c
// 接线
// SG90 黑 / 棕 = GND，红 = 5V，橙 = 信号


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void CLOCK_Init();
void CLOCK_Config();
void Use_GPIO();
void PWM_Init();

int main(void)
{

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* MCU Configuration--------------------------------------------------------*/

/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
 HAL_Init();

/* Configure the system clock */
 SystemClock_Config();

/* USER CODE BEGIN 2 */
 CLOCK_Init();
 CLOCK_Config();
 Use_GPIO();
 PWM_Init();
 /* USER CODE END 2 */

/* Infinite loop */
 /* USER CODE BEGIN WHILE */
 while (1)
 {
 /* USER CODE END WHILE */
 // 设置占空比 CCR
 // 0.5ms 高电平
 TIM2 -> CCR1 = 500;
 HAL_Delay(1000);
 // 1.5ms 中位（舵机90°）
 TIM2 -> CCR1 = 1500;
 HAL_Delay(1000);
 // 2.5ms 最大角度
 TIM2 -> CCR1 = 2500;
 HAL_Delay(1000);
 /* USER CODE BEGIN 3 */
 }
 /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */

/*
 1.时钟初始化
*/
void CLOCK_Init(){
 // GPIOA
 RCC -> APB2ENR |= (1<<2);
 // TIM2
 RCC -> APB1ENR |= (1<<0);
 // AFIO (复用功能)
 RCC -> APB2ENR |= (1<<0);
}

/*
 2.配置GPIO
*/
void Use_GPIO(){
 // clean PA1
 GPIOA -> CRL &=~(0xF << 0);
 // pull (复用推挽输出)
 GPIOA -> CRL |= (0xB << 0);
}

/*
 3.配置定时器基础（50Hz 20ms）
 72MHz / 72 = 1MHZ(1us计数一次)
 20000us --> 20ms --> 50Hz
*/
void CLOCK_Config(void){
 // 预分频 72-1
 TIM2 -> PSC = 72-1;
 // 自动重装载值 20000-1
 TIM2 -> ARR = 20000-1;
 // 向上计数模式（默认）
 TIM2 -> CR1 &=~(1<<4);
}

/*
4.配置PWM模式(PWM1)
CNT < CCR → 高电平
CNT ≥ CCR → 低电平
*/
void PWM_Init(){
 // 清空通道1的配置
 TIM2 -> CCMR1 &=~(0xFF << 0);

// PWM1 模式+输出使能
 TIM2 -> CCMR1 |= (6<<4); // PWM1 模式
 TIM2 -> CCMR1 |= (1<<3); // 预装载使能

// 使能PWM通道+启动定时器
 // 高电平有效
 TIM2 -> CCER |= 1<<0;
 // 启动定时器
 TIM2 -> CR1 |= 1<<0;
}
/* USER CODE END 4 */
```

## LED呼吸灯案例

```c
/* USER CODE BEGIN 0 */
void LED_PWM_Init(){
  // 开时钟
  RCC->APB2ENR |= 1<<2;    // GPIOA
  RCC->APB1ENR |= 1<<0;    // TIM2
  RCC->APB2ENR |= 1<<0;    // AFIO
  __DSB();

  // PA1 复用推挽输出 
  GPIOA->CRL &= ~(0xF << 4);
  GPIOA->CRL |=  (0xB << 4);

  // 复位 TIM2（防止寄存器脏数据）
  RCC->APB1RSTR |=  (1<<0);
  RCC->APB1RSTR &= ~(1<<0);

  // TIM2 基础：2ms 500Hz
  TIM2->PSC = 71;
  TIM2->ARR = 1999;
  TIM2->CR1 |= 1<<7; // ARR预装载使能

  // CH2 PWM1 模式
  TIM2->CCMR1 &= ~(7 << 12);
  TIM2->CCMR1 |=  (6  << 12);   // PWM1
  TIM2->CCMR1 |=  (1  << 11);   // CCR2预装载

  // 通道使能 + 高电平有效
  TIM2->CCER &= ~(1 << 5);     // 极性：高有效
  TIM2->CCER |=  (1 << 4);     // 开启通道2输出

  // 立即更新配置
  TIM2->EGR |= 1<<0;

  // 开定时器
  TIM2->CR1 |= 1<<0;
}

void LED_Open(){
    // 变亮
  for(int i=0; i<=1999; i++){
    TIM2->CCR2 = i;
    HAL_Delay(1);
  }
  // 变暗
  for(int i=1999; i>=0; i--){
    TIM2->CCR2 = i;
    HAL_Delay(1);
  }
}
/* USER CODE END 0 */
```

---

# 一、PWM 通用寄存器

```
TIMx->CR1 控制寄存器（开/关定时器、计数模式）
TIMx->PSC 预分频（71=1μs）
TIMx->ARR 周期（决定频率）
TIMx->CNT 当前计数值
TIMx->CCR1 通道1占空比
TIMx->CCR2 通道2占空比
TIMx->CCR3 通道3占空比
TIMx->CCR4 通道4占空比
TIMx->CCMR1 配置 CH1、CH2 模式
TIMx->CCMR2 配置 CH3、CH4 模式
TIMx->CCER 通道输出使能 + 极性
TIMx->EGR 事件生成（让配置立刻生效）
TIMx->SR 状态标志（一般清0）
```

---

# 二、PWM 模式 1 / 模式 2 区别

```
PWM 模式1（6）：
计数器 < CCR → 高电平
计数器 >= CCR → 低电平
PWM 模式2（7）：
计数器 < CCR → 低电平
计数器 >= CCR → 高电平
日常永远用 6！
```

---

# 三、极性配置

```
TIMx->CCER &= ~(1<<1); 高电平有效（默认）
TIMx->CCER |= (1<<1); 低电平有效
```

---

# 四、初始化黄金顺序

```
1. 开时钟（RCC）
2. 配置GPIO
3. 配置TIM时基（PSC/ARR）
4. 配置PWM模式（CCMR）
5. 配置通道使能（CCER）
6. 启动定时器
```

---

# 五、EGR 作用

```
TIMx->EGR |= 1<<0;
作用：
让 PSC、ARR、CCR 立即生效！
不写这句，配置不会立刻更新！
```

---

# 六、重映射什么时候开？

```
只有引脚不是默认引脚时，才需要开 AFIO + 重映射。
默认引脚不需要开 AFIO！
```

---

# 七、频率 / 周期 计算

```
72MHz / (PSC+1) = 计数频率
默认 PSC=71 → 1MHz → 1μs 一次
周期 = (ARR+1) × 1μs
频率 = 1 / 周期
例：
ARR=1999 → 2000μs → 2ms → 500Hz
```

---

# 八、占空比公式

```
占空比 = CCR / ARR × 100%
CCR越大，高电平时间越长 → 灯越亮 / 舵机角度越大
```

---

# 九、通用定时器通道与引脚

### 🔥 **高级定时器**

```
TIM1_CH1 → PA8
TIM1_CH2 → PA9
TIM1_CH3 → PA10
TIM1_CH4 → PA11
```

#### TIM1 是高级定时器，时钟在 **APB2**

```c
RCC->APB2ENR |= 1 << 11;  // TIM1 时钟
```

#### 配置方法 **和通用定时器几乎一样**

- CCMR1/CCMR2
- PWM 模式 1/2
- 通道偏移 4/12
- CCER 使能 0/4/8/12

### 🔥 **普通定时器**

```planetext
TIM2_CH1 → PA0
TIM2_CH2 → PA1
TIM2_CH3 → PA2
TIM2_CH4 → PA3
TIM3_CH1 → PA6
TIM3_CH2 → PA7
TIM3_CH3 → PB0
TIM3_CH4 → PB1
TIM4_CH1 → PB6
TIM4_CH2 → PB7
TIM4_CH3 → PB8
TIM4_CH4 → PB9
```

---

# 十、常见BUG（避坑指南）

```
1. 先开时钟，再配置GPIO！
2. GPIO必须清 4 位：~(0xF << 位移)
3. 不用重映射不要开 AFIO
4. PWM配置完必须 EGR 刷新
5. 通道使能是 0/4/8/12，不是 1/2/3/4
6. CH1/3 用偏移 4，CH2/4 用偏移 12
```

---
