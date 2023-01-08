#include "PWM.h"

void TIM4_PWM_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

    /* Output Compare Toggle Mode configuration: Channel1 */
    //    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

    TIM_OC1Init(TIM4, &TIM_OCInitStructure);                            // pwm ‰≥ˆÕ®µ¿1
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs(TIM4,ENABLE );
    TIM_ARRPreloadConfig( TIM4,ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

//void TIM8_PWM_Init(u16 arr, u16 psc)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//    TIM_OCInitTypeDef TIM_OCInitStructure;
//
//    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//    TIM_TimeBaseStructure.TIM_Period = arr;
//    TIM_TimeBaseStructure.TIM_Prescaler = psc;
//    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//
//    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
//
//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
//    TIM_OCInitStructure.TIM_Pulse = 100;
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
//
//    TIM_OC1Init(TIM8, &TIM_OCInitStructure);
//    TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
//
//    TIM_OC2Init(TIM8, &TIM_OCInitStructure);
//    TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
//
//    TIM_OC3Init(TIM8, &TIM_OCInitStructure);
//    TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
//
//    TIM_OC4Init(TIM8, &TIM_OCInitStructure);
//    TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
//
//    TIM_ARRPreloadConfig(TIM8, ENABLE);
//    TIM_CtrlPWMOutputs(TIM8, ENABLE);
//    TIM_Cmd(TIM8, ENABLE);
//}

