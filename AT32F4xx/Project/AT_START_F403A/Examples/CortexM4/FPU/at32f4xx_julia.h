/**
  ******************************************************************************
  * File   : CortexM4/FPU/at32f4xx_julia.h 
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : at32f4xx julia header file
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AT32F4XX_JULIA_H
#define __AT32F4XX_JULIA_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "at32f4xx.h"

/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_FPU
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define  ITERATION      	((uint32_t)128)
#define	 REAL_CONSTANT		(0.285f)
#define	 IMG_CONSTANT		(0.01f)
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void GenerateJulia_fpu(uint16_t size_x,
                       uint16_t size_y,
					   uint16_t offset_x,
					   uint16_t offset_y,
					   uint16_t zoom,
					   uint8_t * buffer);

#ifdef __cplusplus
}
#endif

#endif /* __AT32F4XX_JULIA_H */
/**
  * @}
  */ 

/**
  * @}
  */ 

