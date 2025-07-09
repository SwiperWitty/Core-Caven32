## 说明

此设计是针对于STM32毕业设计的

包括但不限于

信号发生器、接收器

传感器数据处理

热成像数据

电机控制

智能IOT

...



### software

[Core-Caven32](https://github.com/SwiperWitty/Core-Caven32/tree/master/Caven-V3.14-M3/STM32F103C8)



### hardware

new

![image-20250512165200944](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202505121652048.png)



引脚定义

- LCD:	*PA10(DC)*、PB0(RES)、PB12(NSS) 13(SLCK) 14 15(MOSI) PB1(SPI2-NSS2)
- OLED:	PB10(SCL) PB11(SDA)
- ENCODE:	[TIM2]PA15、PB3	[KEY]PC13
- PWM:	[TIM4]PB6 7 8 9
- ADC:	PA4、PA5
- BLE:	[UART1]PA9、*PA10* 
- Open AI:	[UART2]PA2、PA3
- HC_SR04:	PB4、PB5
- DS18B20:	PA8
- RZ7899:	*PA0、PA1*
- A4988:	*PA6、PA7* (PC4、PC5)
- ULN2003:	*PA0、PA1、PA6、PA7*
- LED:	PA11
- BZZ:	PA12

注意：

1. PA10在LCD和BLE有冲突(不可同时使用)
2. RZ7899和A4988分别都与ULN2003冲突

兼容了CavenRTOS。







