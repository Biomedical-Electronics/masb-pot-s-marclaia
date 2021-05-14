/**
  ******************************************************************************
  * @file		chronoamperometry.c
  * @brief		Gestión de la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/chronoamperometry.h"
#include "components/ad5280_driver.h"
#include "components/mcp4725_driver.h"
#include "components/i2c_lib.h"

extern TIM_HandleTypeDef htim3;

void CA_meas(struct CA_config) { //lectura de la CA
	double Vcell=CA_config.eDC;
	uint32_t sp=CA_config.eDC;
	uint32_t mt=CA_config.samplingPeriodMs;

	float Vdac=1.65-(Vcell/2);

	int32_t Vref=0;
	int32_t Vtia=0;
	int32_t Icell=0;

	MCP4725_SetOutputVoltage(hdac, Vdac); //seteamos la Vcell a eDC
	HAL_GPIO_WritePin(BME_CS_GPIO_Port, RELAY_Pin, GPIO_PIN_SET); //CERRAR RELÉ
	//FUNCIÓN O CÓDIGO QUE SETEE EL PERIODO DEL TIMER A SP
	set_timer(uint32_t sp);

	uint32_t counter=0;
	uint32_t n=mt/sp; //measurement time divided by sampling period

	while counter<mt{
		if bool_sp==TRUE{
			//cuando haya pasado el sampling period:
			HAL_ADC_Start(&hadc1); // iniciamos la conversion de Vcell real
			HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la conversion
			Vref=HAL_ADC_GetValue(&hadc1);  //guardamos el resultado de la conversion de Vcell real

			HAL_ADC_Start(&hadc1); // iniciamos la conversion de I
			HAL_ADC_PollForConversion(&hadc1, 200); // esperamos que finalice la 2a conversión
			Vtia=HAL_ADC_GetValue(&hadc1);  // guardamos el resultado de la 2a conversión

			Icell=(Vtia-1.65)*2/Rtia;

			//enviar valores al host
			int32_t txBuffer[2] = { 0 }; //size 2: t, intensitat???
			HAL_UART_Transmit(&huart2, &txBuffer, , 100);

			//actualizamos el contador
			counter+=n
		}
	}
	HAL_GPIO_WritePin(BME_CS_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET); //abrimos relé
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3) { //timer 1 sampling period
	bool_sp = !bool_sp;
}

void set_timer(uint32_t sp){
	  htim3.Init.Prescaler = 4199;
	  htim3.Init.Period = 10000;
}
