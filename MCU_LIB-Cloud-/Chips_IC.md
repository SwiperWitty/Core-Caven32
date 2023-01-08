# 外围IC驱动逻辑

### HC595缓存器

  \#define LATCH_CLOCK   GPIO_Pins_1     //门阀时钟

  \#define SHIFT_CLOCK   GPIO_Pins_2     //移动时钟

  \#define HC595_Data    GPIO_Pins_3     //数据



![image-20220927143027820](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220927143027820.png)



### WS2812

单线控制

#### 0.75us(H)+0.35us(L)为 1

![image-20220815091436195](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220815091436195.png)

![image-20220815091508614](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220815091508614.png)



#### 0.35us(H) + 0.75us(L)为 0

![image-20220815091552453](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220815091552453.png)

![image-20220815091558571](https://raw.githubusercontent.com/SwiperWitty/img/main/img/image-20220815091558571.png)