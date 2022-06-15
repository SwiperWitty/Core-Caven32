 /*
  **************************************************************************
  * File   : wm8988_i2s.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  :
  **************************************************************************
  */

 #ifndef __WM8988_I2S_H
 #define __WM8988_I2S_H
 #include "at32f4xx.h"

#define I2S_MAX_TXLEN             288
typedef struct __I2S_Trans_Buffer__
{
    uint8_t u8I2S_BufferA[I2S_MAX_TXLEN];
    uint8_t u8I2S_BufferB[I2S_MAX_TXLEN];
    uint8_t *u8I2SCurBuffer;
    uint8_t u8I2SBufAB;                      /*0 is buffer A, 1 is buffer b*/
}I2S_TransBuffer;

void AT32_I2S_Init(uint32_t freq, uint32_t bitw);
void AT32_SetI2S_FREQ(uint32_t freq);
uint8_t *AT32_I2S1_GetSpeakerBuffer(void);
uint8_t *AT32_I2S1_GetMicrophoneBuffer(void);
uint32_t AT32_I2S1_GetMicrophoneFrameLen(void);
uint32_t AT32_I2S1_SetSpeakerFrameLen(uint32_t u32Len);
void I2S1_DMA1_Channel3_Tx(void);
uint8_t AT32_I2S1_WriteDataToSpkFifoBuffer(uint8_t *u8Pointer, uint16_t u16Len);
uint32_t AT32_I2S1_GetNbyteDataSend(void);
 
#define AT32_I2S_IS_SLAVE       0
#endif


