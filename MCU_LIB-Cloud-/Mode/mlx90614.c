///*******************************************************************************
//* 文件名		: mlx90614.c
//* 作  者	:
//* 版  本	:
//* 日  期	: 2013-08-07
//* 描  述	: mlx90614函数
//PB6：SCL
//PB7：SDA
//在主函数中先初始化SMBus_Init();
//需要读取温度就调用temp=SMBus_ReadTemp();  //读取温度，temp是浮点数，转整数：i=ceil(temp);
//*******************************************************************************/
//
///* Includes ------------------------------------------------------------------*/
//
//#include "mlx90614.h"
//
//#define SA				0x00 //Slave address 单个MLX90614时地址为0x00,多个时地址默认为0x5a
//#define RAM_ACCESS		0x00 //RAM access command RAM存取命令
//#define EEPROM_ACCESS	0x20 //EEPROM access command EEPROM存取命令
//#define RAM_TOBJ1		0x07 //To1 address in the eeprom 目标1温度,检测到的红外温度 -70.01 ~ 382.19度
//
//
//
//void Mlx90614_Init(void)
//{
//	IIC_Init(ENABLE);
//}
//
//unsigned char PEC_Calculation(unsigned char pec[])
//{
//    unsigned char 	crc[6];
//    unsigned char	BitPosition=47;
//    unsigned char	shift;
//    unsigned char	i;
//    unsigned char	j;
//    unsigned char	temp;
//
//    do
//    {
//        /*Load pattern value 0x000000000107*/
//        crc[5]=0;
//        crc[4]=0;
//        crc[3]=0;
//        crc[2]=0;
//        crc[1]=0x01;
//        crc[0]=0x07;
//
//        /*Set maximum bit position at 47 ( six bytes byte5...byte0,MSbit=47)*/
//        BitPosition=47;
//
//        /*Set shift position at 0*/
//        shift=0;
//
//        /*Find first "1" in the transmited message beginning from the MSByte byte5*/
//        i=5;
//        j=0;
//        while((pec[i]&(0x80>>j))==0 && i>0)
//        {
//            BitPosition--;
//            if(j<7)
//            {
//                j++;
//            }
//            else
//            {
//                j=0x00;
//                i--;
//            }
//        }/*End of while */
//
//        /*Get shift value for pattern value*/
//        shift=BitPosition-8;
//
//        /*Shift pattern value */
//        while(shift)
//        {
//            for(i=5; i<0xFF; i--)
//            {
//                if((crc[i-1]&0x80) && (i>0))
//                {
//                    temp=1;
//                }
//                else
//                {
//                    temp=0;
//                }
//                crc[i]<<=1;
//                crc[i]+=temp;
//            }/*End of for*/
//            shift--;
//        }/*End of while*/
//
//        /*Exclusive OR between pec and crc*/
//        for(i=0; i<=5; i++)
//        {
//            pec[i] ^=crc[i];
//        }/*End of for*/
//    }
//    while(BitPosition>8); /*End of do-while*/
//
//    return pec[0];
//}
//
//unsigned int SMBus_ReadMemory(unsigned char slaveAddress, unsigned char command)
//{
//    unsigned int data;			// Data storage (DataH:DataL)
//    unsigned char Pec;				// PEC byte storage
//    unsigned char DataL=0;			// Low data byte storage
//    unsigned char DataH=0;			// High data byte storage
//    unsigned char arr[6];			// Buffer for the sent bytes
//    unsigned char PecReg;			// Calculated PEC byte storage
//    unsigned char ErrorCounter;	// Defines the number of the attempts for communication with MLX90614
//
//    ErrorCounter=0x00;				// Initialising of ErrorCounter
//	slaveAddress <<= 1;	//2-7位表示从机地址
//
//    do
//    {
//repeat:
//        IIC_StopBit();			    //If slave send NACK stop comunication
//        --ErrorCounter;				    //Pre-decrement ErrorCounter
//        if(!ErrorCounter) 			    //ErrorCounter=0?
//        {
//            break;					    //Yes,go out from do-while{}
//        }
//
//        IIC_StartBit();					//Start condition
//        if(IIC_Send(slaveAddress,IIC_Speed))//Send SlaveAddress 最低位Wr=0表示接下来写命令
//        {
//            goto	repeat;			    //Repeat comunication again
//        }
//        if(IIC_Send(command,IIC_Speed))	    //Send command
//        {
//            goto	repeat;		    	//Repeat comunication again
//        }
//
//        IIC_StartBit();					//Repeated Start condition
//        if(IIC_Send(slaveAddress+1,IIC_Speed))	//Send SlaveAddress 最低位Rd=1表示接下来读数据
//        {
//            goto	repeat;             	//Repeat comunication again
//        }
//
//        DataL = IIC_Read(ACK,IIC_Speed);	//Read low data,master must send ACK
//        DataH = IIC_Read(ACK,IIC_Speed); //Read high data,master must send ACK
//        Pec = IIC_Read(NACK,IIC_Speed);	//Read PEC byte, master must send NACK
//        IIC_StopBit();				//Stop condition
//
//        arr[5] = slaveAddress;		//
//        arr[4] = command;			//
//        arr[3] = slaveAddress+1;	//Load array arr
//        arr[2] = DataL;				//
//        arr[1] = DataH;				//
//        arr[0] = 0;					//
//        PecReg=PEC_Calculation(arr);//Calculate CRC
//    }
//    while(PecReg != Pec);		//If received and calculated CRC are equal go out from do-while{}
//
//	data = (DataH<<8) | DataL;	//data=DataH:DataL
//    return data;
//}
//
//
//
//float Mlx90614_ReadTemp(void)
//{
//	float temp;
//	temp = SMBus_ReadMemory(SA, RAM_ACCESS|RAM_TOBJ1)*0.02-273.15;
//	return temp;
//}
//
