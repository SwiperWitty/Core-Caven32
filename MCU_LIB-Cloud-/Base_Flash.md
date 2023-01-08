# Base_Flash

- 宏定义

~~~c
#define BOARD_RAM_SIZE 0x5000
#define BOARD_RAM_BASEADDR 0x20000000
#define BOARD_Flash_BASEADDR 0x08000000
#define BOARD_Flash_SIZE 0x00010000

#define FLASH_PAGE_SIZE 256

~~~



- 写入

先看要写入的长度，够几页（取整），看看Sart和End会不会超。



解除Flash锁，

1. ​	先把数据复制到页缓冲区
2. ​	开始擦除、写入缓冲区

按页循环上述操作，并查看数据完整性、Flag。

锁Flash

退出

~~~~c
int Flash_Write(int addr,const void *DATA,int Length)

~~~~





- 读取

先看要读取的长度，够几页（取整），看看Sart和End会不会超。

1. 按页先读出来存到缓冲区
2. 缓冲区copy到目标地址内

按页循环上述操作，并查看Flag。

退出

~~~c
void Flash_Read(int addr,const void *Back_DATA,int Length)

~~~

