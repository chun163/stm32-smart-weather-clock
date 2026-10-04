#ifndef __OLED_H
#define __OLED_H

#include "stm32f10x.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_DrawBitmap(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t *bitmap);

/* ========== 天气图标描述 ========== */
typedef struct
{
    const char    *name;    /* 天气英文名，如 "Clear" */
    const uint8_t *data;    /* 单色位图数据 */
    uint8_t        width;   /* 图标宽度 */
    uint8_t        height;  /* 图标高度 */
} weather_icon_t;

/* 根据天气英文名查找图标，找不到返回 NULL */
const weather_icon_t *weather_icon_get(const char *name);

#endif