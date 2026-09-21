#include "stm32f10x.h"

// 简单的软件延时函数（约1秒）
void Delay_s(uint32_t s) {
    uint32_t i, j;
    for(i = 0; i < s; i++) {
        for(j = 0; j < 8000000; j++); // 根据实际主频微调
    }
}

int main(void) {
    /* 1. 使能时钟 */
    RCC->APB2ENR |= (1 << 2) | (1 << 3) | (1 << 4) | (1 << 0); // GPIOA/B/C, AFIO

    /* 2. 配置PA2、PB4、PC15为推挽输出 */
    GPIOA->CRL &= ~(0xF << 8);  GPIOA->CRL |= (0x3 << 8);   // PA2
    AFIO->MAPR &= ~(0x7 << 24); AFIO->MAPR |= (0x2 << 24);  // 禁用JTAG，释放PB4
    GPIOB->CRL &= ~(0xF << 16); GPIOB->CRL |= (0x3 << 16);  // PB4
    GPIOC->CRH &= ~(0xF << 28); GPIOC->CRH |= (0x3 << 28);  // PC15

    /* 3. 配置PC13为推挽输出 (板载LED) */
    // 清除PC13的配置位（第20-23位）
    GPIOC->CRH &= ~(0xF << 20);
    // 设置为推挽输出，最大速度2MHz (MODE=10, CNF=00) 即可，PC13灌电流能力弱，无需50MHz
    GPIOC->CRH |= (0x2 << 20);

    /* 4. 初始状态：全灭 */
    // PA2, PB4, PC15 高电平熄灭 (假设外接LED高电平点亮)
    GPIOA->BSRR = (1 << 18); // PA2置0
    GPIOB->BSRR = (1 << 20); // PB4置0
    GPIOC->BSRR = (1 << 31); // PC15置0
    // PC13 低电平熄灭 (板载LED低电平点亮)
    GPIOC->BSRR = (1 << 13); // PC13置1 (高电平熄灭)

    /* 5. 主循环：4灯流水灯 */
    while (1) {
        // 步骤1：点亮PA2，其余熄灭
        GPIOA->BSRR = (1 << 2);  // PA2置1 (点亮)
        GPIOB->BSRR = (1 << 20); // PB4置0 (熄灭)
        GPIOC->BSRR = (1 << 31); // PC15置0 (熄灭)
        GPIOC->BSRR = (1 << 13); // PC13置1 (熄灭)
        Delay_s(1);

        // 步骤2：点亮PB4，其余熄灭
        GPIOA->BSRR = (1 << 18); // PA2置0 (熄灭)
        GPIOB->BSRR = (1 << 4);  // PB4置1 (点亮)
        GPIOC->BSRR = (1 << 31); // PC15置0 (熄灭)
        GPIOC->BSRR = (1 << 13); // PC13置1 (熄灭)
        Delay_s(1);

        // 步骤3：点亮PC15，其余熄灭
        GPIOA->BSRR = (1 << 18); // PA2置0 (熄灭)
        GPIOB->BSRR = (1 << 20); // PB4置0 (熄灭)
        GPIOC->BSRR = (1 << 15); // PC15置1 (点亮)
        GPIOC->BSRR = (1 << 13); // PC13置1 (熄灭)
        Delay_s(1);

        // 步骤4：点亮PC13(板载LED)，其余熄灭
        GPIOA->BSRR = (1 << 18); // PA2置0 (熄灭)
        GPIOB->BSRR = (1 << 20); // PB4置0 (熄灭)
        GPIOC->BSRR = (1 << 31); // PC15置0 (熄灭)
        // 【重点】PC13输出低电平点亮 (BSRR的高16位写1实现清零)
        GPIOC->BSRR = (1 << (13 + 16)); // PC13置0 (点亮)
        Delay_s(1);
    }
}
