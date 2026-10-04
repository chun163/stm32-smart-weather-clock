#ifndef _TIMER_H
#define _TIMER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x.h"                  // Device header


typedef void (*timer_elapsed_callback_t)(void);

void timer_init(uint32_t period_us);
void timer_start(void);
void timer_stop(void);
void timer_elapsed_register(timer_elapsed_callback_t callback);

#endif