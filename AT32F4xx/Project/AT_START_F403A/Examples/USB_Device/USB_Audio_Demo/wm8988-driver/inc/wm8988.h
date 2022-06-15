/*
  **************************************************************************
  * File   : wm8988.h
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : WM8988 driver header
  **************************************************************************
 */
#ifndef __WM8988__H
#define __WM8988__H

#include "at32f4xx.h"
/*WM8988 Register address define*/
#define WM8988_R0_LEFT_INPUT_VOLUME                0x00    /*Left channel PGA control*/
#define WM8988_R1_RIGHT_INPUT_VOLUME               0x01    /*Right channel PGA control*/
#define WM8988_R2_LOUT1_VOLUME                     0x02
#define WM8988_R3_ROUT1_VOLUME                     0x03
#define WM8988_R4_RESERVED                         0x04
#define WM8988_R5_ADC_DAC_CONTROL                  0x05
#define WM8988_R6_RESERVED                         0x06
#define WM8988_R7_AUDIO_INTERFACE                  0x07
#define WM8988_R8_SAMPLE_RATE                      0x08
#define WM8988_R9_RESERVED                         0x09
#define WM8988_R10_LEFT_DAC_VOLUME                 0x0A
#define WM8988_R11_RIGHT_DAC_VOLUME                0x0B
#define WM8988_R12_BASS_CONTROL                    0x0C
#define WM8988_R13_TREBLE_CONTROL                  0x0D
#define WM8988_R14_
#define WM8988_R15_RESET                           0x0F
#define WM8988_R16_3D_CONTROL                      0x10
#define WM8988_R17_ALC1                            0x11
#define WM8988_R18_ALC2                            0x12
#define WM8988_R19_ALC3                            0x13
#define WM8988_R20_NOISE_GATE                      0x14
#define WM8988_R21_LEFT_ADC_VOLUME                 0x15
#define WM8988_R22_RIGHT_ADC_VOLUME                0x16
#define WM8988_R23_ADDITIONAL_1_CONTROL            0x17
#define WM8988_R24_ADDITIONAL_2_CONTROL            0x18
#define WM8988_R25_PWR_1_MGMT                      0x19
#define WM8988_R26_PWR_2_MGMT                      0x1A
#define WM8988_R27_ADDITIONAL_3_CONTROL            0x1B

#define WM8988_R31_ADC_INPUT_MODE                  0x1F
#define WM8988_R32_ADC_L_SIGNAL_PATH               0x20
#define WM8988_R33_ADC_R_SIGNAL_PATH               0x21
#define WM8988_R34_LEFT_OUT_MIX_1                  0x22
#define WM8988_R35_LEFT_OUT_MIX_2                  0x23
#define WM8988_R36_RIGHT_OUT_MIX_1                 0x24
#define WM8988_R37_RIGHT_OUT_MIX_2                 0x25
#define WM8988_R38_RESERVED
#define WM8988_R39_RESERVED
#define WM8988_R40_LOUT2_VOLUME                    0x28
#define WM8988_R41_ROUT2_VOLUME                    0x29
#define WM8988_R42_RESERVED
#define WM8988_R43_LOW_POWER_PALYBACK              0x43



#define WM8988_I2C_ADDR_CSB_LOW  0x34
#define WM8988_I2C_ADDR_CSB_HIGH 0x36

uint8_t WM8988_Init(uint32_t freq, uint32_t bitw);
uint8_t WM8988_Set_Freq(uint32_t freq);
uint8_t *WM8988_GetSpeakerBuffer(void);
uint8_t *WM8988_GetMicrophoneBuffer(void);
//uint8_t WM8988_Set_Volume(uint8_t volume);
uint32_t WM8988_GetMicrophoneFrameLen(void);
//uint8_t WM8988_Set_Mute(uint8_t mute);

uint8_t WM8988_SetADCMic_Volume(uint16_t volume);
uint8_t WM8988_SetDACSpk_Volume(uint16_t volume);
uint8_t WM8988_SetADCMic_Mute(uint8_t mute);
uint8_t WM8988_SetDACSpk_Mute(uint8_t mute);
uint32_t WM8988_SetSpeakerFrameLen(uint32_t u32Len);
void WM8988_WriteSpeakerBuffer(uint8_t *u8Pointer, uint16_t u16Len);
void WM8988_Poll(void);

#endif

