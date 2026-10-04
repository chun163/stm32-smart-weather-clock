#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"                  // Device header
#include "led.h"
#include "main.h"
#include "timer.h"
#include "rtc.h"
#include "OLED.h"
#include "key.h"
#include "esp_at.h"
#include "weather.h"
#include "mpu6050.h"
#include "swi2c.h"

static const char *wifi_ssid = "ChinaNet-fueU";
static const char *wifi_password = "7hxt6t3c";
static const char *weather_uri = "https://api.seniverse.com/v3/weather/now.json?key=Su35oNk6FaDQVcXcZ&location=shenzhen&language=en&unit=c";

static uint32_t runms = 0;
static uint8_t disp_height = 1;
static char str[64];
uint8_t lcd_mode;

//计数每24小时清零
static void timer_elapsed_callback()
{
	runms++;
	if(runms >= 24 * 60 * 60 * 1000)
	{
		runms = 0;
	}
}

static void wifi_init(void)
{
	sprintf(str,"Init ESP32...");
	OLED_ShowString(disp_height, 1, str);
	disp_height += 1;
    if(!esp_at_init())
	{
		sprintf(str,"Failed!!!");
		OLED_ShowString(disp_height, 1, str);
		disp_height += 1;
		while(1);
	}
		

	sprintf(str,"Iint WIFI...");
	OLED_ShowString(disp_height, 1, str);
	if(!esp_at_wifi_init())
	{
		sprintf(str,"Failed!!!");
		OLED_ShowString(disp_height, 1, str);
		disp_height += 1;
		while(1);
	}
	
	delay_ms(500);
	OLED_Clear();
	disp_height = 1;
	sprintf(str,"Connect WIFI...");
	OLED_ShowString(disp_height, 1, str);
	disp_height += 1;
	
	if(!esp_at_wifi_connect(wifi_ssid,wifi_password))
	{
		sprintf(str,"Failed!!!");
		OLED_ShowString(disp_height, 1, str);
		disp_height += 1;
		while(1);
	}
	
	sprintf(str,"Sync Time...");
	OLED_ShowString(disp_height, 1, str);
	disp_height += 1;
	if(!esp_at_sntp_init())
	{
		sprintf(str,"Failed!!!");
		OLED_ShowString(disp_height, 1, str);
		disp_height += 1;
		while(1);
	}
	
}

int main(void)
{
    board_lowlevel_Init();
    led_Init();
    key_init();
    rtc_init();
    mpu6050_init();
    bool weather_ok = false;
    bool sntp_ok = false;

    timer_init(1000);
    timer_elapsed_register(timer_elapsed_callback);

    OLED_Init();

    //显示开机内容
    sprintf(str,"Initializing...");
    OLED_ShowString(disp_height, 1, str);
    disp_height += 1;
    delay_ms(500);

    sprintf(str,"Wait ESP32...");
    OLED_ShowString(disp_height, 1, str);
    disp_height += 1;
    delay_ms(1500);

    wifi_init();

    sprintf(str,"Ready...");
    OLED_ShowString(disp_height, 1, str);
    disp_height += 1;
    delay_ms(500);

    OLED_Clear();

    uint8_t last_mode = 0xFF;   // 用 0xFF 强制首次进入时刷新一次

    while (true)
    {
        /* ========== 1. 按键扫描（每轮都调，内部自己检测边沿） ========== */
        key_pressed();

        /* ========== 2. 只在模式切换时清屏 + 重画固定内容 ========== */
        if (lcd_mode != last_mode)
        {
            OLED_Clear();
            last_mode = lcd_mode;

        }

        /* ========== 3. 模式 0：时钟 + 网络信息 ========== */
        if (lcd_mode == 0)
        {
			//每100ms更新时间
            if (runms % 100 == 0)
            {
                rtc_date_t date;
                rtc_get_date(&date);
                sprintf(str,"%02d/%02d",date.month,date.day);
                OLED_ShowString(1, 1, str);
                sprintf(str,"%02d%s%02d",date.hour,date.second % 2 ? " " : ":",date.minute);
                OLED_ShowString(2, 1, str);
            }
			
			//每一小时同步联网
            if (!sntp_ok || runms % (1000 * 60 * 60) == 0)
            {
                uint32_t ts;
                sntp_ok = esp_at_get_time(&ts);
                rtc_set_timestamp(ts + 8 * 60 * 60);
            }
			
			//网络信息每30s更新一次
            if (runms % (1000 * 30) == 0)
            {
                OLED_ShowString(3, 1, "ChinaNet-fueU");
                char ip[16];
                esp_at_wifi_get_ip(ip);
                OLED_ShowString(4, 1, ip);
            }
        }
        /* ========== 4. 模式 1：天气 + 温度 ========== */
       else if (lcd_mode == 1)
	   {
			/* ---------- 环境温度（每 10s） ---------- */
			if (runms % (1000 * 10) == 0)
			{
				float temper = mpu6050_read_temper();
				sprintf(str, "%5.1fC", temper);
				OLED_ShowString(2, 11, str);
			}

			/* ---------- 天气（每 10 分钟） ---------- */
			if (!weather_ok || runms % (1000 * 60 * 10) == 0)
			{
				const char *rsp;
				weather_ok = esp_at_get_http(weather_uri, &rsp, NULL, 10000);

				weather_t weather;
				weather_parse(rsp, &weather);
                sprintf(str, "%s", weather.weather);
				if(strcmp(weather.weather ,"Cloudy") == 0)
				{
					 OLED_ShowString(1, 1, str);
				}
				else if(strcmp(weather.weather ,"Wind") == 0)
				{
					 OLED_ShowString(1, 1, str);
				}
				else if(strcmp(weather.weather ,"Clear") == 0)
				{
					 OLED_ShowString(1, 1, str);
				}
				else if(strcmp(weather.weather ,"Snow") == 0)
				{
					 OLED_ShowString(1, 1, str);
				}
				else if(strcmp(weather.weather ,"Overcast") == 0)
				{
					 OLED_ShowString(1, 1, str);
				}
				else if(strcmp(weather.weather ,"Rain") == 0)
				{
					 OLED_ShowString(1, 1, str);
				}
				else if(strcmp(weather.weather ,"Light rain") == 0)
				{
					 OLED_ShowString(1, 1, str);
				}
				/* 取图标（找不到返回 NULL） */
				const weather_icon_t *icon = weather_icon_get(weather.weather);

				if (icon != NULL)
				{
					OLED_DrawBitmap(0, 16, icon->width, icon->height, icon->data);
				}
				else
				{
					/* 表里没有的天气，直接显示文字，避免白屏 */
					OLED_ShowString(3, 1, weather.weather);
				}

				/* 顶部显示温度（和图标分开） */
				sprintf(str, "%sC", weather.temperature);
				OLED_ShowString(1, 13, str);
			}
		}
    }
}