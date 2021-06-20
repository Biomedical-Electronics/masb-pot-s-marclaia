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
#include "components/mcp4725_driver.h"
#include "components/masb_comm_s.h"
#include "components/ad5280_driver.h"
#include "components/i2c_lib.h"
#include "components/stm32main.h" //to have available the different pointers
#include "main.h"

//MACROS
#define FALSE	0
#define TRUE	1

//CA configuration structure that stores the parameters received from viSense-S
struct CA_Configuration_S {

	double eDC;
	uint32_t samplingPeriodMs;
	uint32_t measurementTime;

};

//----PROTOTYPES-------
void CA_meas(struct CA_Configuration_S CA_config);
void CA_setTimer(TIM_HandleTypeDef *newHtim);
void CA_setAdc(ADC_HandleTypeDef *newHadc);
void CA_setDac(MCP4725_Handle_T newHdac);
void CA_sendData(void);

#endif /* INC_COMPONENTS_CHRONOAMPEROMETRY_H_ */
