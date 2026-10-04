#ifndef _ESP_AT_H
#define _ESP_AT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x.h"   

bool esp_at_init(void);
//发送命令:rsp长度，超时
bool esp_at_send_command(const char *cmd,const char **rsp,uint32_t *length,uint32_t timeout);
//发送数据
bool esp_at_send_data(const uint8_t *data,uint32_t length);

//复位
bool esp_at_reset(void);

//连接WIFI
bool esp_at_wifi_init(void);
bool esp_at_wifi_connect(const char *ssid,const char *pwd);

//获取ip,mac地址
bool esp_at_wifi_get_mac(char mac[18]);
bool esp_at_wifi_get_ip(char ip[16]);

//获取HTTP请求
bool esp_at_get_http(const char *url,const char **rsp,uint32_t *length,uint32_t timeout);

//设置为中国时区，再获取时间戳
bool esp_at_sntp_init(void);

//获取时间戳
bool esp_at_get_time(uint32_t *timestamp);


#endif