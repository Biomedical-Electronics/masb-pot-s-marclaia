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
#include "components/stm32main.h" //para tener disponibles los punteros del timer, adc, i2c y
#include "main.h"

static UART_HandleTypeDef *huart;
static TIM_HandleTypeDef *htim;
static ADC_HandleTypeDef *hadc;
static MCP4725_Handle_T *hdac;

volatile _Bool bool_sp=FALSE;

void CA_meas(struct CA_Configuration_S CA_config) {
	//--------------INICIALIZAMOS VARIABLES-------------------
    int32_t Vtia=0;
	int32_t Icell=0;
	int32_t Rtia=10000;
	//------------LEEMOS LA CONFIGURACIÓN DE LA CRONO----------------
	double eDC=CA_config.eDC;
	uint32_t sp=CA_config.eDC;
	uint32_t mt=CA_config.samplingPeriodMs;

	//----------SETEAMOS LA TENSIÓN DEL DAC-----------------------------
	float Vdac = (float)(1.65-(eDC/2.0));
	MCP4725_SetOutputVoltage(hdac, Vdac); //seteamos la Vcell a eDC

	//----------------CERRAR RELÉ-------------------------------------
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);

	//----------------SETEAMOS EL PERDIODO--------------------------------
	uint32_t counter_period = sp*(84e6/1000); //counter period que necesitamos

	if (counter_period>65535){ //si el counter period sobrepasa el maximo que podemos fijar
		//prescalamos la frecuencia a 10000
		uint32_t new_counter_period = 84e6*(counter_period/10e3); //new counter period
		uint32_t new_psc = (84e6/10e3) - 1; //new prescaler
		__HAL_TIM_SET_COUNTER(htim,new_counter_period);
		__HAL_TIM_SET_PRESCALER(htim,new_psc);
	} else{ //si no sobrepasa el máximo
		__HAL_TIM_SET_COUNTER(htim,counter_period);
		__HAL_TIM_SET_PRESCALER(htim,0); //dejamos el preescalado a 0
	}

	HAL_TIM_Base_Start_IT(htim); //iniciamos el timer con sus interrupciones

	//-------------ENTRAMOS EN EL LOOP---------------
	uint32_t counter=0; //será el measurement time cada vez que cogemos un punto
	while (counter<mt) {
		if (bool_sp==TRUE){
			//cuando haya pasado el sampling period:

			//actualizamos el contador
			counter+=sp;

			HAL_ADC_Start(hadc); // iniciamos la conversion de Vcell real
			HAL_ADC_PollForConversion(hadc, 200); // esperamos que finalice la conversion
			uint32_t Vref=HAL_ADC_GetValue(hadc);  //guardamos el resultado de la conversion de Vcell real

			HAL_ADC_Start(hadc); // iniciamos la conversion de I
			HAL_ADC_PollForConversion(hadc, 200); // esperamos que finalice la 2a conversión
			Vtia=HAL_ADC_GetValue(hadc);  // guardamos el resultado de la 2a conversión

			Icell=(Vtia-1.65)*2/Rtia;

			//enviar valores al host
			uint32_t txBuffer[2] = {counter,Icell}; //size 2: t, intensitat??? sí
			HAL_UART_Transmit(huart,&txBuffer,2,100);
		}
	}
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET); //abrimos relé
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3) { //timer 1 sampling period
	bool_sp = !bool_sp;
}

void CA_setUart(UART_HandleTypeDef *newHuart) {
	huart = newHuart; //puntero al handle type de la huart
}

void CA_setTimer(TIM_HandleTypeDef *newHtim) {
	htim = newHtim;
}

void CA_setAdc(ADC_HandleTypeDef *newHadc) {
	hadc = newHadc;
}
