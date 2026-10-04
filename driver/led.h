#ifndef _LED_H
#define _LED_H

#include <stdbool.h>
#include "stm32f10x.h"                  // Device header

void led_Init(void);
void led_off(void);
void led_toggle(void);
void led_on(void);
void led_set(bool on);

#endif