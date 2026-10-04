#include "main.h"
#include "stm32f10x.h"                  // Device header

void delay_us(uint32_t us)
{
    //把这个值写入`LOAD`重载寄存器
    SysTick->LOAD = us * (SystemCoreClock / 1000000 ) - 1;
    //清空当前计数器 VAL 寄存器，让计数器立刻从 LOAD 值开始倒计时
    SysTick->VAL = 0;
    //选择**内核时钟作为 SysTick 时钟源**,打开 SysTick 定时器
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    //没开定时器/还没倒计时到 0一直阻塞等待
    while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    //关闭 SysTick 定时器，停止计数。
    SysTick->CTRL = 0;
}

void delay_ms(uint32_t ms)
{
    while(ms--)
    {
        delay_us(1000);
    }
}