#ifndef _ESP_usart_H
#define _ESP_usart_H

#include <stdbool.h>
#include "stm32f10x.h"                  // Device header

void esp_usart_init(void);
void esp_usart_write_data(uint8_t *data,uint16_t length);
void esp_usart_write_string(const char *string);

typedef void (*esp_usart_receive_callback_t)(uint8_t data);
void esp_usart_receive_register(esp_usart_receive_callback_t callback);

#endif