/**
  ******************************************************************************
  * @file		chronoamperometry.c
  * @brief		Gestión de la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/chronoamperometry.h"

static TIM_HandleTypeDef *htim;
static ADC_HandleTypeDef *hadc;
static MCP4725_Handle_T hdac;

volatile _Bool bool_samplingPeriod=FALSE;

//--------------INICIALIZAMOS VARIABLES-------------------
int32_t Vtia=0;
int32_t Rtia=50000;
//inicializamos el contador y el point a 0
uint32_t point = 0;
uint32_t counter=0; //será el measurement time cada vez que cogemos un punto
uint32_t samplingPeriod;
uint32_t measurementTime=0;

void CA_meas(struct CA_Configuration_S CA_config) {
	//------------LEEMOS LA CONFIGURACIÓN DE LA CRONO----------------
	double eDC=CA_config.eDC;
	samplingPeriod=CA_config.samplingPeriodMs; //ms
	measurementTime=CA_config.measurementTime*1000; //segons

	//----------SETEAMOS LA TENSIÓN DEL DAC-----------------------------
	float Vdac = eDC/2.0+2.0; //pasarle con una funcion
	MCP4725_SetOutputVoltage(hdac, Vdac); //seteamos la Vcell a eDC

	//----------------CERRAR RELÉ-------------------------------------
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);

	//----------------SETEAMOS EL PERDIODO--------------------------------
	uint32_t counter_period = samplingPeriod* 10; //counter period que necesitamos

	__HAL_TIM_SET_AUTORELOAD(htim, counter_period); //seteamos el periodo
	__HAL_TIM_SET_COUNTER(htim,0);

	HAL_TIM_Base_Start_IT(htim); //iniciamos el timer con sus interrupciones

	point=0;
	counter=0;
	CA_sendData(); //enviamos el primer punto

	//-------------ENTRAMOS EN EL LOOP---------------
	while (counter<=measurementTime) {
		if (bool_samplingPeriod==TRUE){
			//cuando haya pasado el sampling period:
			CA_sendData();
		}
	}
	HAL_TIM_Base_Stop_IT(htim); //paramos el timer

	//---------------------ABRIMOS RELÉ-----------------------------
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3) { //timer 1 sampling period
	bool_samplingPeriod = TRUE;
}

void CA_setDac(MCP4725_Handle_T newHdac) {
	hdac = newHdac; //puntero al handle type de la hdac
}

void CA_setTimer(TIM_HandleTypeDef *newHtim) {
	htim = newHtim;
}

void CA_setAdc(ADC_HandleTypeDef *newHadc) {
	hadc = newHadc;
}

void CA_sendData(void){
	bool_samplingPeriod=FALSE;
	point++;

	HAL_ADC_Start(hadc); // iniciamos la conversion de Vcell real
	HAL_ADC_PollForConversion(hadc, 200); // esperamos que finalice la conversion
	uint32_t Vref=HAL_ADC_GetValue(hadc);  //guardamos el resultado de la conversion de Vcell real
	double Vref_d = ((double)Vref)/4095.0*3.3;
	double vCell = - ((Vref_d*8.0/3.3)-4.0);

	HAL_ADC_Start(hadc); // iniciamos la conversion de I
	HAL_ADC_PollForConversion(hadc, 200); // esperamos que finalice la 2a conversión
	uint32_t Vtia=HAL_ADC_GetValue(hadc);  // guardamos el resultado de la 2a conversión
	double Vtia_d = ((double)Vtia)/4095.0*3.3;
	double iCell =((Vtia_d*2.42)-4.0)/Rtia;

	//enviar valores al host

	struct Data_S data;
	data.point = point;
	data.timeMs = counter;
	data.voltage = vCell;
	data.current = iCell;
	counter += samplingPeriod;

	MASB_COMM_S_sendData(data);
}
