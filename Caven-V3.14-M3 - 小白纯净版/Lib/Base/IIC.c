#include "IIC.h"

struct IIC_	IIC;			//结构体本体

void IIC_StartBit(void)
{
    IIC_SDA_H();  // Set SDA line
    Delay_10us(2); // Wait a few microseconds
    IIC_SCK_H();  // Set SCL line
    Delay_10us(2); // Generate bus free time between Stop
    IIC_SDA_L();  // Clear SDA line
    Delay_10us(2); // Hold time after (Repeated) Start
    // Condition. After this period, the first clock is generated.
    //(Thd:sta=4.0us min)在SCK=1时，检测到SDA由1到0表示通信开始（下降沿）
    IIC_SCK_L();  // Clear SCL line
    Delay_10us(2); // Wait a few microseconds
}

void IIC_StopBit(void)
{
    IIC_SCK_L();  // Clear SCL line
    Delay_10us(2); // Wait a few microseconds
    IIC_SDA_L();  // Clear SDA line
    Delay_10us(2); // Wait a few microseconds
    IIC_SCK_H();  // Set SCL line
    Delay_10us(2); // Stop condition setup time(Tsu:sto=4.0us min)
    IIC_SDA_H();  // Set SDA line在SCK=1时，检测到SDA由0到1表示通信结束（上升沿）
}

void IIC_SendBit(char Bit_out, int speed)
{
    if (Bit_out == 0)
    {
        IIC_SDA_L();
    }
    else
    {
        IIC_SDA_H();
    }
    Delay_10us(speed); // Tsu:dat = 250ns minimum
    IIC_SCK_H();      // Set SCL line
    Delay_10us(speed); // High Level of Clock Pulse
    IIC_SCK_L();      // Clear SCL line
    Delay_10us(speed); // Low Level of Clock Pulse
    //	IIC_SDA_H();				    // Master release SDA line
}

char IIC_ReadBit(int Speed)
{
    char Ack_bit;

    IIC_SDA_H();      //引脚靠外部电阻上拉，当作输入
    Delay_10us(Speed); // High Level of Clock Pulse
    IIC_SCK_H();      // Set SCL line
    Delay_10us(Speed); // High Level of Clock Pulse
    if (IIC_SDA_IN())
    {
        Ack_bit = 1;
    }
    else
    {
        Ack_bit = 0;
    }
    IIC_SCK_L();      // Clear SCL line
    Delay_10us(Speed); // Low Level of Clock Pulse

    return Ack_bit;
}

/*

Byte_8 : 需要发的数据
Speed : 速度
返回是否应答

*/

char IICs_SendByte(char Byte_8, int Speed)
{
    char Bit_counter;
    char Ack_bit;
    char Bit_out;

    for (Bit_counter = 8; Bit_counter; Bit_counter--)
    {
        if (Byte_8 & 0x80)
        {
            Bit_out = 1; // If the current bit of Tx_buffer is 1 set Bit_out
        }
        else
        {
            Bit_out = 0; // else clear Bit_out
        }
        IIC_SendBit(Bit_out, Speed); // Send the current bit on SDA
        Byte_8 <<= 1;                // Get next bit for checking
    }

    Ack_bit = IIC_ReadBit(Speed); // Get acknowledgment bit
    return Ack_bit;
}

char IICs_ReadByte(char ack_nack, int Speed)
{
    char RX_buffer;
    char Bit_Counter;

    for (Bit_Counter = 8; Bit_Counter; Bit_Counter--)
    {
        if (IIC_ReadBit(Speed)) // Get a bit from the SDA line
        {
            RX_buffer <<= 1; // If the bit is HIGH save 1  in RX_buffer
            RX_buffer |= 0x01;
        }
        else
        {
            RX_buffer <<= 1; // If the bit is LOW save 0 in RX_buffer
            RX_buffer &= 0xfe;
        }
    }
    IIC_SendBit(ack_nack, Speed); // Sends acknowledgment bit
    return RX_buffer;
}

void IIC_Software_Init(FunctionalState SET)
{
	if(SET)
	{
		IIC_GPIO_Init();							//GPIO
		IIC_SCK_H();
		IIC_SDA_H();
	}
	else
	{
		IIC_GPIO_Exit();
    }
	IIC.Soft_ReadByte = IICs_ReadByte;
	IIC.Soft_SendByte = IICs_SendByte;
}
