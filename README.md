# Core-STM32

因为本怪精力有限，[Cavendish 3.0](https://github.com/SwiperWitty/MCU-Cavendish) 已经**停止维护**，除非出现重大BUG。

Caven 3.14的硬件和Caven 3.0 **不完全一样**，是一个更大的进化，板载硬件DAP-Link和TTL串口（可以Debug和串口打印数据）。

只需要一根Type-c和一台电脑即可开发 [Caven 3.14](https://github.com/SwiperWitty/Core-STM32/tree/main/Caven-V3.14-M3)！

本设计仅供于学习、参考，不可商用和水毕业论文（参与开发者随意）！

___

### 硬件方面

|    Caven     |           3.0            |       3.14        |
| :----------: | :----------------------: | :---------------: |
|   下载方式   |         外置SWD          |  USB(CMSIS-DAP)   |
|   USB接口    |         硬件USB          | DAP-Link+串口3/4  |
| 软件编程难度 | 跟正点原子差不多（一般） | Caven风格（简单） |
|     BTB      |       0.8mm板对板        |    1.0mm板对板    |
|   MCU电压    |         固定3.3V         |       3.3V        |
|    晶振1     |         8M 无源          |      8M 有源      |
|    晶振2     |         32.768K          |        无         |

#### 为啥取消RTC的晶振?

**RTC时钟**晶振一般为32.768khz,溢出一次的时间为：
$$
1 / 32768 ≈ 0.03s
$$
这个时间对于系统中一些功能的计时是不够的（例如超声波）、此核心板被没有给RTC时钟单独供电。综上，没有使用RTC功能的必要，Caven的源码中，给系统提供时间信息的是滴答定时器（10us一次溢出）。



#### 支持芯片

**M3:**	STM32F103RC、GD32F103RC	及以上（doing）

**M4:**	STM32F405RC、GD32F405RC	及以上（planning）

**M4F:**	AT32F403RC								及以上（planning）

_____

### 软件方面

#### 工程框架

![image-20220617103736956](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220617103736956.png)

如果对这个框架表示不是很能理解，那么就需要对【.c】、【.h】的文件关系有一个理解

##### 文件关系

![66](https://raw.githubusercontent.com/SwiperWitty/img/main/img/66.png)

一个**工程**中有很多个**团**，每个**团**由若干个**元素**组成。

**团**：主要是有各个大方向的集合内容，这里以API为例。

**元素**：就是一个小集合【.c+.h】、【.h】这样，一个元素可以没有【.c】但一定要有【.h】。【.h】更像是一本书的目录，他是先行者（给编译器看）。		如果main有调用【.h】的函数，那么他才会去对应【.c】找内容是什么。有一种情况是只看【.h】就够了，那就是【.h】全是宏和定义，这个可以理解为饭店的菜单，只有目录没有内容（例如LCD中保存图片的文件都在【.h】中）。

**共识：**我给**所有的项目**，无论是C51、STM32或者是Linux的工程都添加了一个叫**Caven.h**的文件（共识）。它普遍运用在**元素**内，**标准库**就是一个很经典的共识，共识的本质就是元素。

（不会有人问我标准库是什么吧？拒绝回答这类问题）



##### 开始使用Caven框架

![image-20220617104417701](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220617104417701.png)

索引是必须最先运行的，它的作用是将框架搭起来（简单理解就是，将把肉馅塞到馒头里），让User快乐起来！（这个框架并不怎么占内存）



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

#### Mode

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

#### 算法部署(API)（长期）

- [ ] PID
- [ ] 卡尔曼滤波
- [ ] Other

---

### 你可能会遇到的问题...

MDK会用吧？

项目依赖会装吧？

满足这些你就可以正常使用了！

#### MDK设置

![image-20220617113014960](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220617113014960.png)

下载方式是CMSIS-DAP，也就是所谓的**WCH-Link**，按照上述步骤你就可以看到这个

![image-20220617113831539](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220617113831539.png)

如果上述有问题请参考一下内容（如果你认真看完这些，依旧没解决，我可以帮帮你；如果你没有认真看，那我只能请你熟读并背诵了）

WCH-Link相关：[看这里](https://github.com/SwiperWitty/CMSIS-DAPbyWCH)

芯片检测不到：[看这里](https://blog.csdn.net/ReCclay/article/details/103449476)   的【 三、设置keil 章节】的 【3、配置好相应的Flash Download选项】

如果问题还是没有解决：[看这里](https://www.baidu.com/)
