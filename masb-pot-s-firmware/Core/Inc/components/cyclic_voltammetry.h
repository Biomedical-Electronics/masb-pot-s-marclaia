/**
  ******************************************************************************
  * @file		cyclic_voltammetry.h
  * @brief		Archivo de encabezado del componente encargado de la gestión de
  *             la voltammetría cíclica.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#ifndef INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_
#define INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_

#include "stm32f4xx_hal.h"
#include "components/masb_comm_s.h"
#include "components/ad5280_driver.h"
#include "components/mcp4725_driver.h"
#include "components/i2c_lib.h"
#include "components/stm32main.h" //para tener disponibles los punteros del timer, adc, i2c y
#include "main.h"

struct CV_Configuration_S {

	double eBegin;
	double eVertex1;
	double eVertex2;
	uint8_t cycles;
	double scanRate;
	double eStep;

};

//--------------PROTOTYPING------------------
void CV_meas(struct CV_Configuration_S cvConfiguration);
void CV_setTimer(TIM_HandleTypeDef *newHtim);
void CV_setAdc(ADC_HandleTypeDef *newHadc);
void CV_setDac(MCP4725_Handle_T newHdac);
void CV_sendData(void);
#endif /* INC_COMPONENTS_CYCLIC_VOLTAMMETRY_H_ */
