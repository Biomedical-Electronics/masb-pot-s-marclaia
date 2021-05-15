/**
  ******************************************************************************
  * @file		chronoamperometry.h
  * @brief		Archivo de encabezado del componente encargado de la gestión de
  *             la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#ifndef INC_COMPONENTS_CHRONOAMPEROMETRY_H_
#define INC_COMPONENTS_CHRONOAMPEROMETRY_H_

#include "stm32f4xx_hal.h"
#include "main.h"

#define FALSE	0
#define TRUE	1

struct CA_Configuration_S {

	double eDC;
	uint32_t samplingPeriodMs;
	uint32_t measurementTime;

};

//----PROTOTYPES-------
void CA_meas(struct CA_Configuration_S CA_config);
void CA_setUart(UART_HandleTypeDef *newHuart);
void CA_setTimer(TIM_HandleTypeDef *newHtim);
void CA_setAdc(ADC_HandleTypeDef *newHadc);

#endif /* INC_COMPONENTS_CHRONOAMPEROMETRY_H_ */
