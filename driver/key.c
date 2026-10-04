#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "key.h"

#define KEY_PROT	GPIOB
#define KEY_PIN		GPIO_Pin_15

uint8_t B1_state;
uint8_t B1_last_state = 1;

void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = KEY_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(KEY_PROT,&GPIO_InitStructure);
}

void key_pressed(void)
{
	B1_state = GPIO_ReadInputDataBit(KEY_PROT,GPIO_Pin_15);
	if(B1_state == 0 && B1_last_state == 1)
	{
		lcd_mode++;
		if(lcd_mode > 1)
		{
			lcd_mode = 0;
		}
	}
	B1_last_state = B1_state;
}