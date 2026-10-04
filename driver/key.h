#ifndef _KEY_H
#define _KEY_H

#include "stm32f10x.h"                  // Device header
#include <stdbool.h>

void key_init(void);
void key_pressed(void);
void key_wait_release(void);
extern uint8_t lcd_mode;

#endif