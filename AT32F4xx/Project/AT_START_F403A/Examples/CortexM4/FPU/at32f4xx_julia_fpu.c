/**
  ******************************************************************************
  * File   : CortexM4/FPU/at32f4xx_julia_fpu.c
  * Version: V1.2.7
  * Date   : 2020-11-10
  * Brief  : at32f4xx julia_fpu source file
  ******************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
  
#include "at32f4xx_julia.h"
/** @addtogroup AT32F403A_StdPeriph_Examples
  * @{
  */

/** @addtogroup CortexM4_FPU
  * @{
  */ 
  
/**
  * @brief  Julia Calculation with FPU option disable
  * @param  size_x: screen width in pixel
  * @param  size_y: screen height in pixel
  * @param  offset_x: x offset for starting point
  * @param  offset_y: y offset for starting point
  * @param  zoom: Iteration zoom ratio
  * @param  buffer: pointer to output buffer where values are stored
  * @retval None
  */
void GenerateJulia_fpu(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom, uint8_t * buffer)
{
  float       tmp1, tmp2;
  float       num_real, num_img;
  float       rayon;

  uint8_t       i;
  uint16_t        x, y;

  for (y = 0; y < size_y; y++)
  {
    for (x = 0; x < size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i = 0;
      rayon = 0;
      while ((i < ITERATION - 1) && (rayon < 4))
      {
        tmp1 = num_real * num_real;
        tmp2 = num_img * num_img;
        num_img = 2 * num_real * num_img + IMG_CONSTANT;
        num_real = tmp1 - tmp2 + REAL_CONSTANT;
        rayon = tmp1 + tmp2;
        i++;
      }
      /* Store the value in the buffer */
      buffer[x+y*size_x] = i;
    }
  }

}

/**
  * @}
  */ 

/**
  * @}
  */ 
  
