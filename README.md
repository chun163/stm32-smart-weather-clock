<img width="1706" height="1279" alt="c3ca56dbd7b8f3312bb927c807d53945" src="https://github.com/user-attachments/assets/3436598e-4e8b-4e7c-940e-691f766f27e9" /># STM32 智能天气时钟

基于 STM32F103 的智能天气时钟，支持 WiFi 联网、实时天气获取、SNTP 网络授时及 OLED 显示。

##  演示
<img width="1706" height="1279" alt="c3ca56dbd7b8f3312bb927c807d53945" src="https://github.com/user-attachments/assets/bd0126ce-9fc5-45c8-97b0-6b0d0c4229a4" />
<img width="1706" height="1279" alt="e5f28734cd454ec635a1cfb5cbb481c8" src="https://github.com/user-attachments/assets/23ae6b04-e00a-4950-bf88-ec79e0672460" />

## 功能特性
- 支持 ESP-AT 指令集，通过 ESP32-C3 连接 WiFi
- 定时通过 HTTP 获取心知天气实时数据 (温度、天气状态)
- 定时通过 SNTP 同步网络时间，校准本地 RTC
- I2C OLED 显示天气图标、时间、环境温度、网络信息
- 支持按键切换双界面（时钟模式 / 天气模式）
- 裸机非阻塞式任务轮询架构

## 硬件平台
- MCU: STM32F103C8T6
- WiFi: ESP32-C3-MINI-1 (运行 AT 固件)
- 显示: 0.96寸 I2C OLED (SSD1306)
- 传感器: MPU6050 (读取环境温度)

## 硬件接线

### I2C 外设
- **OLED 显示屏**
  - SCL -> PB8
  - SDA -> PB9
- **MPU6050 传感器**
  - SCL -> PB10
  - SDA -> PB11

### 按键
- **KEY**
  - 一端接地 (GND)，另一端接 PB15。

### 串口通信 (STM32 <-> ESP32-C3)
- **STM32 USART2**
  - STM32 TX (PA2) -> ESP32 GPIO7 (RX)
  - STM32 RX (PA3) -> ESP32 GPIO6 (TX)
  - GND -> GND (务必共地)

## 软件架构
- **非阻塞调度**：基于定时器实现任务轮询，避免 delay 阻塞主循环
- **分层设计**：main.c -> esp_at.c -> esp_usart.c
- **轻量 JSON 解析**：基于 strstr / strchr 提取字段，节省 Flash 开销
- **状态机**：按键驱动的 UI 状态切换

## 编译说明
1. 使用 Keil uVision5 打开 `mdk/stm32f103.uvprojx`
2. 修改 main.c 中的  wifi_ssid  和  wifi_password  为实际的 WiFi 账号密码
3. 编译并下载到 STM32F103
