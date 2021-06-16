/*
 * stm32main.h
 *
 *  Created on: May 7, 2021
 *      Author: Laia
 */

#ifndef INC_COMPONENTS_STM32MAIN_H_
#define INC_COMPONENTS_STM32MAIN_H_

#include "stm32f4xx_hal.h"

//MACROS
#define	CV	0
#define CA	1
#define IDLE	2

//VARIABLE INITIALIZATION
uint32_t ESTADO;

// we define the structure to handle the pointers
struct Handles_S {
     UART_HandleTypeDef *huart;
     ADC_HandleTypeDef *hadc;
     I2C_HandleTypeDef *hi2c;
     TIM_HandleTypeDef *htim;
 };


//PROTOTYPES (to be used in stm32main.c)
void setup(struct Handles_S *handles);
void loop(void);

#endif /* INC_COMPONENTS_STM32MAIN_H_ */
