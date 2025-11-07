/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : cavendish
 * Version            : V1.0.0
 * Date               : 2025/11/4
 * Description        : Main program body.
*********************************************************************************
/*
 *@Note
TCP Server example, demonstrating that TCP Server
receives data and sends back after connecting to a client.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
 */
#include "string.h"
#include "eth_driver.h"
#include "IQmath_RV32.h"
#define KEEPALIVE_ENABLE                1               //Enable keep alive function

#include "Base_ETH.h"
#include "Mode.h"

u16 srcport = 8160;                                     //source port

u8 SocketIdForListen;                                   //Socket for Listening
u8 SocketIdForclient = 0xff;                                   //Socket for Listening

u8 socket[WCHNET_MAX_SOCKET_NUM];                       //Save the currently connected socket
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  //socket receive buffer
u8 MyBuf[RECE_BUF_LEN];
u8 link_sock;
/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @param   iError - error constants.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
}

/*********************************************************************
 * @fn      WCHNET_CreateTcpSocketListen
 *
 * @brief   Create TCP Socket for Listening
 *
 * @return  none
 */

void WCHNET_CreateTcpSocketListen(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;
    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.SourPort = 8160;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    i = WCHNET_SocketCreat(&SocketIdForListen, &TmpSocketInf);
    mStopIfError(i);
    printf("server SocketId %d\r\n", SocketIdForclient);
    i = WCHNET_SocketListen(SocketIdForListen);                   //listen for connections
    mStopIfError(i);

}

/*********************************************************************
 * @fn      WCHNET_CreateTcpSocket
 *
 * @brief   Create TCP Socket
 *
 * @return  none
 */
void WCHNET_CreateTcpSocket(void)
{
    u8 i;
    SOCK_INF TmpSocketInf;
    u8 ip_array[10];
    Caven_Str_To_ip ("192.168.1.160",ip_array,4);
    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    memcpy((void *) TmpSocketInf.IPAddr, ip_array, 4);
    TmpSocketInf.DesPort = 8000;
    TmpSocketInf.SourPort = 8000;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    i = WCHNET_SocketCreat(&SocketIdForclient, &TmpSocketInf);
    printf("client SocketId %d\r\n", SocketIdForclient);
    mStopIfError(i);
    i = WCHNET_SocketConnect(SocketIdForclient);                        //make a TCP connection
    mStopIfError(i);
}

/*********************************************************************
 * @fn      WCHNET_DataLoopback
 *
 * @brief   Data loopback function.
 *
 * @param   id - socket id.
 *
 * @return  none
 */
void WCHNET_DataLoopback(u8 id)
{
#if 1
    u8 i;
    u32 len;
    u32 endAddr = SocketInf[id].RecvStartPoint + SocketInf[id].RecvBufLen;       //Receive buffer end address

    if ((SocketInf[id].RecvReadPoint + SocketInf[id].RecvRemLen) > endAddr) {    //Calculate the length of the received data
        len = endAddr - SocketInf[id].RecvReadPoint;
    }
    else {
        len = SocketInf[id].RecvRemLen;
    }
    i = WCHNET_SocketSend(id, (u8 *) SocketInf[id].RecvReadPoint, &len);        //send data
    if (i == WCHNET_ERR_SUCCESS) {
        WCHNET_SocketRecv(id, NULL, &len);                                      //Clear sent data
    }
#else
    u32 len, totallen;
    u8 *p = MyBuf, TransCnt = 255;

    len = WCHNET_SocketRecvLen(id, NULL);                                //query length
    printf("Receive Len = %d\r\n", len);
    totallen = len;
    WCHNET_SocketRecv(id, MyBuf, &len);                                  //Read the data of the receive buffer into MyBuf
    while(1){
        len = totallen;
        WCHNET_SocketSend(id, p, &len);                                  //Send the data
        totallen -= len;                                                 //Subtract the sent length from the total length
        p += len;                                                        //offset buffer pointer
        if( !--TransCnt )  break;                                        //Timeout exit
        if(totallen) continue;                                           //If the data is not sent, continue to send
        break;                                                           //After sending, exit
    }
#endif
}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
u8 last_sock = 0;
void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    u8 i;

    if (intstat & SINT_STAT_RECV)                                 //receive data
    {
        stb_printf("socket[%d] stat recv\r\n",socketid);
        WCHNET_DataLoopback(socketid);                            //Data loopback
    }
    if (intstat & SINT_STAT_CONNECT)                              //connect successfully
    {
#if KEEPALIVE_ENABLE
        WCHNET_SocketSetKeepLive(socketid, ENABLE);
#endif
        WCHNET_ModifyRecvBuf(socketid, (u32) SocketRecvBuf[socketid],
        
        RECE_BUF_LEN);
        if(SocketIdForclient != socketid)
        {
            if(last_sock != 0 && last_sock != socketid)
            {
                stb_printf("socket[%d] has been abandon\r\n",last_sock);
                WCHNET_SocketClose(last_sock,TCP_CLOSE_ABANDON);
            }
            link_sock = socketid;
            last_sock = socketid;
        }
        stb_printf("socket[%d] con\r\n",socketid);
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
            if (socket[i] == 0xff) {                              //save connected socket id
                socket[i] = socketid;
                break;
            }
        }
    }
    if (intstat & SINT_STAT_DISCONNECT)                           //disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        stb_printf("socket[%d] discon\r\n",socketid);
        if(SocketIdForclient == socketid)
        {
            SocketIdForclient = 0;
        }
    }
    if (intstat & SINT_STAT_TIM_OUT)                              //timeout disconnect
    {
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {             //delete disconnected socket id
            if (socket[i] == socketid) {
                socket[i] = 0xff;
                break;
            }
        }
        stb_printf("socket[%d] over time\r\n",socketid);
        if(SocketIdForclient == socketid)
        {
            SocketIdForclient = 0;
        }
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt(void)
{
    u8 intstat;
    u16 i;
    u8 socketint;
    u8 socketid;
    intstat = WCHNET_GetGlobalInt();                              //get global interrupt flag
    if (intstat & GINT_STAT_UNREACH)                              //Unreachable interrupt
    {
        stb_printf("GINT_STAT_UNREACH\r\n");
    }
    if (intstat & GINT_STAT_IP_CONFLI)                            //IP conflict
    {
        stb_printf("GINT_STAT_IP_CONFLI\r\n");
    }

    if (intstat & GINT_STAT_PHY_CHANGE)                           //PHY status change
    {
        i = WCHNET_GetPHYStatus();
        if (i & PHY_Linked_Status)
        {
            stb_printf("PHY Link Success\r\n");
            SocketIdForclient = 0;
            link_sock = 0;
        }
        else
        {
            stb_printf("PHY Link down\r\n");
            if(link_sock)
            {
                stb_printf("eth link_sock %d \r\n",link_sock);
                WCHNET_SocketClose(link_sock,TCP_CLOSE_ABANDON);
                link_sock = 0;
            }
            if(SocketIdForclient)
            {
                stb_printf("eth link_sock %d \r\n",link_sock);
                WCHNET_SocketClose(SocketIdForclient,TCP_CLOSE_ABANDON);
                SocketIdForclient = 0xff;
            }
        }
    }
    if (intstat & GINT_STAT_SOCKET) {                             //socket related interru
        for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) 
        {
            socketint = WCHNET_GetSocketInt(i);
            if (socketint)
                WCHNET_HandleSockInt(i, socketint);
        }
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
 uint8_t array[1000];
int main(void)
{
    u8 i;
    int addr = 0;
    Caven_BaseTIME_Type now_time;
    SystemCoreClockUpdate();
    Mode_Index();
    Mode_Init.TIME(ENABLE);
    Mode_Init.UART(DEBUG_OUT,115200,ENABLE);

    now_time.SYS_Sec = 1762236074;
    Mode_Use.TIME.Set_BaseTIME_pFun (now_time);
    stb_printf("MCU Init,MCU_SYS_FREQ: %d Hz \n",MCU_SYS_FREQ);

    Base_ETH_Task_CallBcak_Bind(WCHNET_HandleGlobalInt);
    Base_ETH_config_local_ip (1,"192.168.1.168","192.168.1.1","255.255.255.0");
    Base_ETH_Init(1,ENABLE);
    Mode_Use.TIME.Delay_S(3);
    memset(socket, 0xff, WCHNET_MAX_SOCKET_NUM);
    WCHNET_CreateTcpSocketListen();                               //Create TCP Socket for Listening
    
    int old_sec = 0;
    while(1)
    {
        now_time = Mode_Use.TIME.Get_BaseTIME_pFun();
        if (old_sec != now_time.SYS_Sec)
        {
            old_sec = now_time.SYS_Sec;
            // stb_printf("sys time utc: %d s \n",old_sec);
        }
        if (SocketIdForclient == 0)
        {
            WCHNET_CreateTcpSocket();
        }
        Base_ETH_Task ();
    }
}

