#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "stm32f10x.h"   
#include "esp_at.h"
#include "esp_usart.h"
#include "main.h"

//RX缓冲区
#define RX_BUFFER_SIZE  4096
//ESP返回状态
#define RX_RESULT_OK    0
#define RX_RESULT_ERROR 1
#define RX_RESULT_FAIL  2   //其他类型错误：单片机

//缓存的数据
static uint8_t rxdata[RX_BUFFER_SIZE];
//接收的数据长度
static uint32_t rxlen;
//发送命令才开始准备接收数据，不发命令时不把数据存储到rxdata
static bool rxready;
//保存ESP返回状态的值
static uint8_t rxresult;


static void on_usart_received(uint8_t data)
{
    //没有数据请求，不接收数据
    if(!rxready)
    {
        return;
    }
    //接收数据，防止缓冲区溢出
    if(rxlen < RX_BUFFER_SIZE)
    {
        rxdata[rxlen++] = data;
    }
    //接收失败
    else
    {
        rxready = false;
        rxresult = RX_RESULT_FAIL;
        return;
    }

    //数据接收完毕判断
    if(data == '\n')
    {
        //接收换行符是否为\r\n
        if(rxlen >= 2 && rxdata[rxlen - 2] == '\r')
        {
            //收到OK
            if(rxlen >= 4 && rxdata[rxlen - 4] == 'O' && rxdata[rxlen - 3] == 'K')
            {
                rxready = false;
                rxresult = RX_RESULT_OK;
            }
            else if(rxlen >= 7 
                && rxdata[rxlen - 5] == 'E' && rxdata[rxlen - 4] == 'R' 
                && rxdata[rxlen - 3] == 'R' && rxdata[rxlen - 2] == 'O'
                && rxdata[rxlen - 1] == 'R' )
            {
                rxready = false;
                rxresult = RX_RESULT_ERROR;
            }
        }
    }
}

bool esp_at_init(void)
{
    //没有接收数据的准备
    rxready =false;
    //串口初始化
    esp_usart_init();
    //调用注册函数,接收到的数据进行判断
    esp_usart_receive_register(on_usart_received);

    //返回复位后的结果
    return esp_at_reset();
}

//发送命令:rsp长度，超时
bool esp_at_send_command(const char *cmd,const char **rsp,uint32_t *length,uint32_t timeout)
{
    rxlen = 0;
    rxready = true;
    rxresult = RX_RESULT_FAIL;//?

    esp_usart_write_string(cmd);
    esp_usart_write_string("\r\n");

    //判断超时
    while(rxready && timeout--)
    {
        delay_ms(1);
    }

    rxready = false;

    if(rsp)
    {
        *rsp = (const char *)rxdata;
    }
    if(length)
    {
        *length = rxlen;
    }
    
    return rxresult == RX_RESULT_OK;
}

//发送数据
bool esp_at_send_data(const uint8_t *data,uint32_t length)
{
    esp_usart_write_data((uint8_t *)data,length);

    return true;
}

//复位
bool esp_at_reset(void)
{
    //复位ESP32
    if(!esp_at_send_command("AT+RESTORE",NULL,NULL,1000))
    {
        return false;
    }
    delay_ms(2000);
    //关闭回显
    if(!esp_at_send_command("ATE0",NULL,NULL,1000))
    {
        return false;
    }
    //关闭存储
    if(!esp_at_send_command("AT+SYSSTORE=0",NULL,NULL,1000))
    {
        return false;
    }
    return true;
}

//连接WIFI
bool esp_at_wifi_init(void)
{
    //设置为station模式
    if(!esp_at_send_command("AT+CWMODE=1",NULL,NULL,1000))
    {
        return false;
    }

    return true;
}

bool esp_at_wifi_connect(const char *ssid, const char *pwd)
{
    char cmd[64];

    // 连接wifi
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
    if (!esp_at_send_command(cmd, NULL, NULL, 10000))
    {
        return false;
    }

    return true;
}

//获取HTTP请求
bool esp_at_get_http(const char *url, const char **rsp, uint32_t *length, uint32_t timeout)
{
    char cmd[128];

    snprintf(cmd, sizeof(cmd), "AT+HTTPCGET=\"%s\"", url);
    if (!esp_at_send_command(cmd,rsp,length, 10000))
    {
        return false;
    }

    return true;
}

bool esp_at_sntp_init(void)
{
    //设置SNTP模式
    if(!esp_at_send_command("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"",NULL,NULL,1000))
    {
        return false;
    }
    //查询SNTP时间
    if(!esp_at_send_command("AT+CIPSNTPTIME?",NULL,NULL,1000))
    {
        return false;
    }
    
    return true;
}

//获取时间戳
bool esp_at_get_time(uint32_t *timestamp)
{
    const char *rsp;
    uint32_t length;

    if(!esp_at_send_command("AT+SYSTIMESTAMP?",&rsp,&length,1000))
    {
        return false;
    }

    char *sts = strstr(rsp,"+SYSTIMESTAMP:");
    sts += strlen("+SYSTIMESTAMP:");

    //把数字形式的字符串，转换成 int 类型的数字
    *timestamp = atoi(sts);

    return true;
}   

bool esp_at_wifi_get_ip(char ip[16])
{
	const char *rsp;
	
    if(!esp_at_send_command("AT+CIPSTA?",&rsp,NULL,1000))
    {
        return false;
    }
	
	//解析ip地址
	const char *pip = strstr(rsp,"+CIPSTA:ip:") + strlen("+CIPSTA:ip:");
	if(pip)
	{
		for(int i = 0; i < 16; i++)
		{
			if(pip[i] == '\r')
			{
				ip[i] = '\0';
				break;
			}
			ip[i] = pip[i];
		}
		return true;
	}
	return true;
}

bool esp_at_wifi_get_mac(char mac[18])
{
	const char *rsp;
	if(!esp_at_send_command("AT+CIPSTAMAC?",&rsp,NULL,1000))
	{
		return false;
	}
	
	//解析mac地址
	const char *pmac = strstr(rsp,"+CIPSTAMAC:mac:") + strlen("+CIPSTAMAC:mac:");
	if(pmac)
	{
		strncpy(mac,pmac,18);
		return true;
	}
	
	return true;
}