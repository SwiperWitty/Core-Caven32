# Core-STM32

因为本怪精力有限，[Cavendish 3.0](https://github.com/SwiperWiity/MCU-Cavendish) 已经**停止维护**，除非出现重大BUG。

Caven 3.14的硬件和Caven 3.0 **不完全一样**，是一个更大的进化，板载硬件DAP-Link和TTL串口（可以Debug和串口打印数据）。

|    Caven     |           3.0            |       3.14        |
| :----------: | :----------------------: | :---------------: |
|   下载方式   |         外置SWD          |  USB(CMSIS-DAP)   |
|   USB接口    |         硬件USB          | DAP-Link+串口3/4  |
| 软件编程难度 | 跟正点原子差不多（一般） | Caven风格（简单） |
|     BTB      |       0.8mm板对板        |    1.0mm板对板    |
|   MCU电压    |         固定3.3V         |       3.3V        |



硬件方面只需要一根Type-c和一台电脑即可开发 [Caven 3.14](https://github.com/SwiperWiity/Core-STM32/tree/main/Caven-V3.14-M3)！

本设计仅供于学习、参考，不可商用和水毕业论文（参与开发者随意）！

支持芯片：

**M3:**	STM32F103RC、GD32F103RC	及以上（doing）

**M4:**	STM32F405RC、GD32F405RC	及以上（planning）

**M4F:**	AT32F403RC								及以上（planning）

### 软件方面

![a](https://github.com/SwiperWiity/Core-STM32/blob/main/Caven-V3.14-M3/卡文迪许怪.png)

#### BASE

##### SYSTIME

- [x] 提供{小时，分钟，秒，毫秒}
- [x] 提供Delay函数
- [ ] 自动结束（Over time）-（80%，未部署）

##### SPI（通信）

- [ ] 提高LCD刷屏效率-（ok）
- [ ] 自动显示内容并设缓冲区-（20%）
- [ ] 硬件DMA的SPI-（10%）
- [ ] SPI振铃问题-（40%）
- [ ] DMA

##### UART（通信）

- [ ] 串口1、2、3、4跑通（一般只用2、3）-（90%）
- [ ] DMA

##### IIC（通信）

- [x] 软件模拟

##### CAN（通信）



##### ADC

- [x] 读取电压

##### DAC

- [x] 电压输出

##### PWM

- [ ] 通道4、8跑通 --(90%)

##### Encode

- [ ] 通道1、2、3、4、5跑通（4平时不用）--(80%)

-------

### Mode

##### 弱控制输出

- [ ] LED(WS2812B)
- [x] BZZ
- [ ] HardWare_IC

##### 强控制输出

- [ ] 电机（TIME8），使用74HC595做方向IO控制（方向与速度隔离）
- [x] 舵机（TIME4）

##### 外部输入

- [x] 触摸TTP223/RH6030
- [x] 循迹

##### 外部检测模块

- [ ] 超声波
- [ ] 红外测温
- [ ] 姿态解析（MPU6050）

##### LCD显示

### 算法部署（长期）

- [ ] PID
- [ ] 卡尔曼滤波
- [ ] Other

---



### 基础功能的结构体函数指针

- 完成

### MODE功能的结构体函数指针

- [ ] LCD(ok)
- [ ] 超声波（80%）
- [ ] 红外测温（80%）
- [ ] 电机（50%）
- [ ] 舵机（80%）
- [ ] 热成像（20%）
