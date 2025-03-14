# Core-Caven32

​	因为本怪精力有限，[Cavendish 3.0]已经**停止维护**，除非出现重大BUG，现在推出了新硬件**Caven 3.14** ！**Core-Caven32**就是为其设计的全新代码框架。

​	Caven 3.14 板载硬件DAP-Link和TTL串口（可以Debug和串口3打印数据），此外还有一个LCD屏。

​	只需要一根Type-c数据线和一台电脑即可开发 [Caven 3.14](https://github.com/SwiperWitty/Core-Caven32) ！

![logo](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202404151059005.jpg)



_____

​	其实Core-Caven32里面放的是一个32位系统的代码库,包括ARM-M0、ARM-M0+、ARM-M3、ARM-M4(F)、RISC-V（虽然它的初衷是为Cavend 3.14服务）。

本设计仅供于学习、参考，不可商用和水毕业论文（参与开发者随意）！

**更新日志**

2020.开始创建

2021.框架稳定

2022.驱动

2024.开始收尾，CavenRTOS

|  更新速度Top  | Base底层完成度（独立） | Mode调度层完成度(共享)  |
| :-----------: | :--------------------: | :---------------------: |
| **AT32F415**  |          90%           |           95%           |
|   AT32F403A   |          40%           |           95%           |
| **STM32F103** |           --           |           95%           |
| **GD32F405**  |           --           |           95%           |
| **CH32V203**  |          90%           |           95%           |
|   HC32L110    |         99.8%          | 不在Caven框架(内存太小) |
|     Other     |           --           |           --            |



_____

首先，要先确认Caven 3.14上面的CH549芯片是否有固件/驱动。

<img src="https://gitee.com/Swiper_witty/caven_img/raw/master/img/202404021731513.png" alt="image-20240402173154416" style="zoom: 80%;" />

如果没用，请到`Core-Caven32\DAP-Link`文件夹下面查看使用说明！如果电脑检测不到驱动，请去WCH官网寻求帮助，谢谢！[嘉立创EDA dap-link硬件资料](https://u.lceda.cn/account/user/projects/index/detail?project=f7e730190b18464dbe3e176b97edfd54&listType=all)



______

### 开始使用

**驱动&资料**

[MCU的Keil依赖&工具&资料](https://github.com/SwiperWitty/MCU_Pack-Datasheet_Lib) （因为这个东西不经常变更就不放在这里了）

[子模块链接](https://github.com/SwiperWitty/MCU_LIB-Cloud-) （里面有食用方法）

Cavendish硬件资料 [嘉立创EDA](https://u.lceda.cn/account/user/projects/index/detail?project=c74409cf0db64e179e8eee7457e84763&folder=all) 或者 [SwiperWitty/PCB-MCU](https://github.com/SwiperWitty/PCB-MCU) ，注意是Caven 3.14哦！



**关于有些小白直接下载压缩包**

如果你直接下载，它里面会缺少[Caven-MCU_Lib]文件的内容，因为[Caven-MCU_Lib]文件是子模块。解决办法是git 下载这个子模块手动copy到[Core-Caven32]文件夹，但是这样极其愚蠢，我是不推荐的，而且没法获取代码更新。

![image-20240120151722408](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202401201517453.png)

**关于git 下载此项目**

​	因为这个工程里面有子模块（就是上面那个子模块链接，以防呆瓜不看，再说一遍），所以**使用之前**要从`\Core-Caven32`文件`cd`到`\Caven-MCU_Lib`

~~~shell
git clone --recursive https://github.com/SwiperWitty/Core-Caven32.git
cd .\Caven-MCU_Lib\
git branch				// 查看分支
git checkout main		// 切换到主分支

cd ..
// ok了
~~~

如果你发现[Caven-MCU_Lib]文件夹是空的，那么你就需要执行下面的命令

~~~shell
//在Core-Caven32文件夹下的命令行

cd .\Caven-MCU_Lib\
git submodule update --init --recursive
// 如果没反应
git pull
// 如果还是没反应，那就应该去看git子模块使用方法
// 如果成功了，接下来执行切换到主分支
git branch
git checkout main

cd ..
~~~

其他疑问：

如果`port 443 after : Timed out`，你可以试试

设置网络代理，系统代理的 IP 地址和端口号。

~~~~shell
git config --global http.proxy http://127.0.0.1:7890 
~~~~

或者，取消网络代理（例如使用gitee）

~~~~shell
git config --global --unset http.proxy
~~~~



![image-20230802141749444](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202308021417513.png)



______

### 主要功能

自动告诉你有哪些功能以及如何使用(Keil也会)！

所有会自动提升的功能都是已经写好的！

![image-20240402181002084](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202404021810133.png)



#### CavenRTOS

这个功能是最重要的东西，当然你可能主观上不想用，但是实际上你在启动的时候，他就运行了。

- 系统实时时钟管理
- 多任务（状态机&触发器）

##### 实时时钟

由`Base_Sys_time.c`提供最基础的调用，注意此文件提供三点：初始化系统时钟、获取/设置系统实时时钟、delay。

~~~~c
typedef struct
{
    u32 SYS_Time_H; //每Frequency进1
    u32 SYS_Time_L; // 24bit 的
}SYS_Time_Type;

typedef struct
{
    U32 SYS_Sec;
    U32 SYS_Us;             // 这里最大 1000 000
}SYS_BaseTIME_Type;

void SYS_Time_Init(int Set);

void SYS_Time_Set(SYS_BaseTIME_Type * time);
void SYS_Time_Get(SYS_BaseTIME_Type * time);

void SYS_Base_Delay(int time, int speed);
~~~~

由`MODE_Time.c`提供用户使用的时钟函数，提供用户层能使用的数据，例如当日时间`Caven_Watch_Type`，日期`Caven_Date_Type`，其中`Caven_Watch_Type`是内带系统总秒数的。

~~~c
// 日期
typedef struct
{
    U16 year;
    U8 month;
    U8 day;
    U32 SYS_Day;
}Caven_Date_Type;

// 时间
typedef struct
{
    U8 hour;
    U8 minutes;
    U8 second;
    U32 time_us;            // 这里最大 1000 000
    U32 SYS_Sec;
}Caven_Watch_Type;

Mode_Init.TIME(ENABLE);
Mode_Use.TIME.Delay_Ms(10);
Mode_Use.TIME.Get_Watch_pFun();
~~~

##### 多任务

- 时间触发函数
- 事件任务函数

由`Time_Handle.c`提供时间触发函数，只由时间触发，但是并不执行任务，是否执行任务请访问`Task_Overtime_Type`结构体。

~~~~c
typedef struct
{
    volatile Caven_Watch_Type Set_time;
    volatile Caven_Watch_Type Begin_time;
    volatile char Trigger_Flag;              //[000][1][000]
    volatile char Flip_falg;                 //[000][111][000]   only Read
    int Switch;
}Task_Overtime_Type;
int API_Task_Timer (Task_Overtime_Type *task,Caven_Watch_Type now_time);
~~~~

由`Caven_event_frame.c`提供事件任务函数。

~~~~c
typedef struct
{
    int events_num;
    unsigned char events[CAVEN_EVENTS_MAX];
    D_pFun events_pFun[CAVEN_EVENTS_MAX];

}Caven_event_Type;


int Caven_new_event_Fun(Caven_event_Type *events,D_pFun event_pFun,int *handle);
int Caven_delete_event_Fun(Caven_event_Type *events,int *handle);

int Caven_trigger_event_Fun(Caven_event_Type *events,int const handle,char data);

int Caven_handle_event_Fun(Caven_event_Type *events);
~~~~

例如

~~~~c
Caven_Watch_Type now_time;
Caven_event_Type SYS_events;
int BZZ_event_Handle = 0;

void BZZ_event_task_Fun (void * data)
{
    int temp_data = *(int *)data;

    if (temp_data) {
		Mode_Use.BZZ.SET_pFun(ENABLE);
        Mode_Use.BZZ.SET_pFun(DISABLE);
    }
    else {
        Mode_Use.BZZ.SET_pFun(DISABLE);
    }
    *(int *)data = 0;		// 退出任务
}

void Main_Init(void)
{
    int reverse = 0;
    system_clock_config();
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    Mode_Index();

    Mode_Init.TIME_Init_State = Mode_Init.TIME(ENABLE);
	Mode_Init.LED(ENABLE);
    Mode_Init.BZZ(ENABLE);
    
	now_time = Mode_Use.TIME.Get_Watch_pFun();
    Caven_new_event_Fun(&SYS_events,BZZ_event_task_Fun,&BZZ_event_Handle); // 创建BZZ事件任务
}

int main (void)
{
    Main_Init();
    
    Task_Overtime_Type LED_Task = {
            .Switch = 1,
            .Begin_time = now_time,
            .Set_time.second = 1,
            .Set_time.time_us = 5000,
            .Flip_falg = 1,
    };
	API_Task_Timer (&LED_Task,now_time);        // 创建LED时间任务
	
    while(1)
    {
        Mode_Use.LED.SET_pFun(1,LED_Task.Flip_falg);	// 访问任务状态
        if(LED_Task.Trigger_Flag)
        {
            Caven_trigger_event_Fun(&SYS_events,BZZ_event_Handle,1);	// 触发事件
        }
        Caven_handle_event_Fun(&SYS_events);			// 事件运行函数
    }
}
~~~~

由例程看出，LED灯被LED_Task结构体控制，每1.5s交换一次状态，每次交换状态，BZZ都会响一次。



___

### 硬件芯片资源

![image-20240402181020882](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202404021810147.png)



#### 支持芯片

**M0:**	STM32F030F4、STM32G030F6(M0+)

**M3:**	STM32F103RC、GD32F103RC

**M4:**	STM32F405RC、AT32F415RC、AT32F425RC、AT32F403ARC、

**RISC-V:**	CH32V203C8T6、CH32V307RET6（这玩意Flash不行）

![image-20240402181034923](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202404021810967.png)



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



---

### 你可能会遇到的问题...

Keil会用吧？

WCH-Link驱动会装吧？

Github会clone吧？

满足这些你就可以正常使用了！



**MDK设置**

![image-20240402181043064](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202404021810109.png)

下载方式是CMSIS-DAP，也就是所谓的**WCH-Link**，按照上述步骤你就可以看到这个

![](https://gitee.com/Swiper_witty/caven_img/raw/master/img/202404021810833.png)

如果上述有问题请参考一下内容（如果你认真看完这些，依旧没解决，我可以帮帮你；如果你没有认真看，那我只能请你熟读并背诵了）

WCH-Link相关：[看这里](https://www.wch.cn/downloads/WCH-LinkUserManual_PDF.html)
如果需要给WCH-Link下载程序，那么**需要将CH549的D-接到VDD激活CH549的USB下载模式**，然后如果是**V1.0版本**的WCH-Link固件，需要根据TX接GND去切换RSIC-V/ARM下载模式。当然你可以选择直接下载ARM专用固件（WCH-Link_APP_IAP_ARM.bin）或者RSIC-V专用固件（WCH-Link_APP_IAP_RV.bin）。

芯片检测不到：[看这里](https://blog.csdn.net/ReCclay/article/details/103449476)   的【 三、设置keil 章节】的 【3、配置好相应的Flash Download选项】

如果问题还是没有解决：[看这里](https://www.baidu.com/) 



_____

### 如何找到UP？

卡文迪许怪

qq讨论群:455839434

[GitHub](https://github.com/SwiperWitty) & [Bilibili](https://space.bilibili.com/102898291?spm_id_from=333.1007.0.0) 

