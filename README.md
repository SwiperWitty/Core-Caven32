# Core-Caven32

​	因为本怪精力有限，[Cavendish 3.0]已经**停止维护**，除非出现重大BUG，现在推出了新硬件**Caven 3.14** ！**Core-Caven32**就是为其设计的全新代码框架。

​	Caven 3.14 板载硬件DAP-Link和TTL串口（可以Debug和串口3打印数据），此外还有一个LCD-1。3寸屏。

​	只需要一根Type-c数据线和一台电脑即可开发 [Caven 3.14](https://github.com/SwiperWitty/Core-Caven32) ！

_____

​	其实Core-Caven32里面放的是一个32位系统的代码库,包括ARM-M0、ARM-M0+、ARM-M3、ARM-M4(F)、RISC-V（虽然它的初衷是为Cavend 3.14服务）。

本设计仅供于学习、参考，不可商用和水毕业论文（参与开发者随意）！

![image-20230802143110689](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202308021431731.png)



______

### 开始使用

**驱动&资料**

[MCU的Keil依赖&工具&资料](https://github.com/SwiperWitty/MCU_Pack-Datasheet_Lib) （因为这个东西不经常变更就不放在这里了）

[子模块链接](https://github.com/SwiperWitty/MCU_LIB-Cloud-) （里面有食用方法）

硬件资料 [嘉立创EDA](https://u.lceda.cn/account/user/projects/index/detail?project=c74409cf0db64e179e8eee7457e84763&folder=all) 或者 [SwiperWitty/PCB-MCU](https://github.com/SwiperWitty/PCB-MCU) ，注意是Caven 3.14哦！



**关于git 下载此项目**

​	因为这个工程里面有子模块（就是上面那个子模块链接，以防呆瓜不看，再说一遍），所以**使用之前**要从`\Core-Caven32`文件`cd`到`\Caven-MCU_Lib`

~~~shell
git clone https://github.com/SwiperWitty/Core-Caven32.git

cd .\Core-Caven32\
git branch

cd .\Caven-MCU_Lib\
git submodule update --init --recursive
git branch
git checkout main

cd ..
~~~



![image-20230802141749444](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202308021417513.png)

如果`port 443 after : Timed out`，你可以试试

设置网络代理，系统代理的 IP 地址和端口号。

~~~~shell
git config --global http.proxy http://127.0.0.1:7890 
~~~~

或者，取消网络代理（例如使用gitee）

~~~~
git config --global --unset http.proxy
~~~~



______

### 主要功能

自动告诉你有哪些功能以及如何使用(Keil也会)！

所有会自动提升的功能都是已经写好的！

![image-20220913201218059](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220913201218059.png)



_______

### 工程结构

![image-20220916142856598](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220916142856598.png)

其中**Caven-MCU_Lib**文件

​	目的是减少重复造轮子（比如串口收发逻辑等），明明都是可以用同一个函数表达，只是底层不同。

而且放在GitHub，修改一个文件push之后，即可多数文件可以直接pull使用！（子项目）

___

### 硬件芯片资源

![image-20220822194832619](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220822194832619.png)

#### 支持芯片

**M0:**	STM32F030F4、STM32G030F6(M0+)

**M3:**	STM32F103RC、GD32F103RC

**M4:**	STM32F405RC、AT32F415RC、AT32F425RC、AT32F403ARC、

**RISC-V:**	CH32V203C8T6、CH32V307RET6（这玩意Flash不行）

![image-20220916145800524](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220916145800524.png)

**通信：**

1. UART（硬）
2. SPI（软、硬）（硬件->【普\DMA】）
3. IIC（软）
4. USB(部分)（硬）
5. CAN（部分）（硬）

**模拟:**

1. ADC（普、DMA）
2. DAC(部分)

**系统时钟：**

1. SYS_TICK
2. 看门狗（选）

**Flash**

1. 掉电保存
2. Bootloard（部分）

**数字（时钟输入输出）：**

1. PWM
2. ENCODE（编码）

**GPIO：**

1. 输入
2. 输出



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

____

##### 开始使用Caven框架

![image-20220617104417701](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220617104417701.png)

索引是必须最先运行的，它的作用是将框架搭起来（简单理解就是，将把肉馅塞到馒头里），让User快乐起来！（这个框架并不怎么占内存）



---

### 你可能会遇到的问题...

Keil会用吧？

WCH-Link驱动会装吧？

Github会clone吧？

满足这些你就可以正常使用了！



**MDK设置**

![image-20220617113014960](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220617113014960.png)

下载方式是CMSIS-DAP，也就是所谓的**WCH-Link**，按照上述步骤你就可以看到这个

![image-20220617113831539](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220617113831539.png)

如果上述有问题请参考一下内容（如果你认真看完这些，依旧没解决，我可以帮帮你；如果你没有认真看，那我只能请你熟读并背诵了）

WCH-Link相关：[看这里](https://www.wch.cn/downloads/WCH-LinkUserManual_PDF.html)
如果需要给WCH-Link下载程序，那么**需要将CH549的D-接到VDD激活CH549的USB下载模式**，然后如果是**V1.0版本**的WCH-Link固件，需要根据TX接GND去切换RSIC-V/ARM下载模式。当然你可以选择直接下载ARM专用固件（WCH-Link_APP_IAP_ARM.bin）或者RSIC-V专用固件（WCH-Link_APP_IAP_RV.bin）。

芯片检测不到：[看这里](https://blog.csdn.net/ReCclay/article/details/103449476)   的【 三、设置keil 章节】的 【3、配置好相应的Flash Download选项】

如果问题还是没有解决：[看这里](https://www.baidu.com/) 



_____

### 如何找到UP？

卡文迪许怪

[GitHub](https://github.com/SwiperWitty) & [Bilibili](https://space.bilibili.com/102898291?spm_id_from=333.1007.0.0) 

