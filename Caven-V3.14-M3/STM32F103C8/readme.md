## 文件的说明

- Startup是单片机启动文件，描述了从哪个地址开始启动等等（0x0800000默认），每类不同内存大小的启动文件不一样
- CMSIS是内核的文件，不希望改动
- Config是时钟等配置文件（设置使用内/外部晶振、频率、IT中断）每个程序不一定相同
- SDK是官方提供的**标准外设库**（又称固件库），把寄存器封装成函数的库
- User是用户对IDE的配置文件和Main函数（黑匣子.a文件也放这里吧）
- OBJ是编译生成文件，包含中间文件，最重要的HEX & bin

- Caven-MCU_Lib 是我提供的云端库文件，它可以在其他的MCU中使用（随时可能会更改，所以打包了起来）（它不在这个 文件夹内，在前面...）

- 一些单片机还有.ld文件（链接脚本）
- 还有可能有历程文件



引脚定义

- LCD:	*PA10(DC)*、PB0(RES)、PB12(NSS) 13(SLCK) 14 15(MOSI) PB1(SPI2-NSS2)
- OLED:	PB10(SCL) PB11(SDA)
- ENCODE:	[TIM2]PA3、PB15	[KEY]PC13
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

1. PA10在LCD和BLE有冲突
2. RZ7899和A4988分别都与ULN2003冲突

兼容了CavenRTOS。







