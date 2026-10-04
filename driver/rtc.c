#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f10x.h"                  // Device header
#include "rtc.h"

//У��ʱ���Ƿ�Ϲ�
static bool date_validate(const rtc_date_t *date)
{
    if (date->year < 1970 || date->year > 2099)
        return false;
    if (date->month < 1 || date->month > 12)
        return false;
    if (date->day < 1 || date->day > 31)
        return false;
    if (date->hour > 23)
        return false;
    if (date->minute > 59)
        return false;
    if (date->second > 59)
        return false;

    return true;
}

//����->ʱ���
uint32_t date_to_ts(const rtc_date_t *date)
{
    uint16_t year = date->year;
    uint8_t month = date->month;
    uint8_t day = date->day;
    uint8_t hour = date->hour;
    uint8_t minute = date->minute;
    uint8_t second = date->second;

    uint64_t ts = 0;

    /* �������£����㵱�����һ���·� */
    month -= 2;
    if ((int8_t)month <= 0)
    {
        month += 12;
        year -= 1;
    }

    /* ����ʱ��� */
    ts = (((year / 4 - year / 100 + year / 400 + 367 * month / 12 + day + year * 365 - 719499) * 24 +
          hour) * 60 + minute) * 60 + second;

    return ts;
}

//ʱ���->����
void ts_to_date(uint32_t seconds, rtc_date_t *date)
{
    uint32_t leapyears = 0, yearhours = 0;
    const uint32_t mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const uint16_t ONE_YEAR_HOURS = 8760;

    memset(date, 0, sizeof(rtc_date_t));

    /* �� */
    date->second = seconds % 60;
    seconds /= 60;

    /* �� */
    date->minute = seconds % 60;
    seconds /= 60;

    /* �� */
    leapyears = seconds / (1461 * 24);
    date->year = (leapyears << 2) + 1970;
    seconds %= 1461 * 24;

    for (;;)
    {
        yearhours = ONE_YEAR_HOURS;

        /* �����һ�� */
        if ((date->year & 3) == 0)
            yearhours += 24;

        /* ʣ��ʱ���Ѿ�����һ��������ѭ�� */
        if (seconds < yearhours)
            break;

        date->year++;
        /* ��ȥһ���ʱ�� */
        seconds -= yearhours;
    }

    /* ʱ */
    date->hour = seconds % 24;
    seconds /= 24;
    seconds++;

    /* ���� */
    if ((date->year & 3) == 0)
    {
        if (seconds > 60)
        {
            seconds--;
        }
        else
        {
            if (seconds == 60)
            {
                date->month = 2;
                date->day = 29;
                return;
            }
        }
    }

    /* �� */
    for (date->month = 0; mdays[date->month] < seconds; date->month++)
        seconds -= mdays[date->month];
    date->month++;

    /* �� */
    date->day = seconds;
}

void rtc_init(void)
{
	//RTCʱ��Դ����Ϊ�ⲿʱ��
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	//ʹ���ⲿʱ��
	RCC_RTCCLKCmd(ENABLE);
	//�ȴ�ʱ��ͬ��,�ȴ���һ���������
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();
	//���÷�Ƶֵ:Ĭ����32767 + 1���ⲿ����ʱ�ӵľ�����32768�����1Hz�ķ�ƵҲ����1s
	RTC_SetPrescaler(39999);
	RTC_WaitForLastTask();
}

void rtc_set_timestamp(uint32_t timestamp)
{
    RTC_WaitForLastTask();
    RTC_SetCounter(timestamp);
    RTC_WaitForLastTask();
}

void rtc_get_timestamp(uint32_t *timestamp)
{
    if(timestamp)
    {
        *timestamp = RTC_GetCounter();
    }
}

void rtc_set_date(rtc_date_t *date)
{
	//У��ʱ���Ƿ��
	if(!date_validate(date))
	{
		return;
	}
	uint32_t ts = date_to_ts(date);
	
	RTC_WaitForLastTask();
	RTC_SetCounter(ts);
	RTC_WaitForLastTask();
}

void rtc_get_date(rtc_date_t *date)
{
	uint32_t ts = RTC_GetCounter();
	
	//date��ֵ
	if(date)
	{
		ts_to_date(ts,date);
	}
}