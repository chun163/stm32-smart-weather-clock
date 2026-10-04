#include "stm32f10x.h"                  // Device header

void board_lowlevel_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);

	PWR_BackupAccessCmd(ENABLE);
	BKP_DeInit();

	// ========== LSI �ڲ�ʱ�ӣ�����LSE������Ҫ32.768K���� ==========
	RCC_LSICmd(ENABLE);
	//�ȴ�LSI�ȶ�
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
	//RTCʱ��Դѡ��LSI
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	RCC_RTCCLKCmd(ENABLE);
}
