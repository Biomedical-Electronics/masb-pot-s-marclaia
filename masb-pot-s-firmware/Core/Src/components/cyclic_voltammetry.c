/**
  ******************************************************************************
  * @file		cyclic_voltammetry.c
  * @brief		Gestión de la voltammetría cíclica.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/cyclic_voltammetry.h"
#include "math.h"

#define MAX_VAR    0.00001 //maximum variation

//--------------PUNTEROS------------------
static TIM_HandleTypeDef *htim;
static ADC_HandleTypeDef *hadc;
static MCP4725_Handle_T hdac;

//--------------INICIALIZAMOS VARIABLES-------------------

int32_t Vtia;
int32_t Rtia;
uint32_t point;
uint32_t counter; //será el measurement time cada vez que cogemos un punto
uint32_t samplingPeriod;
uint8_t counter_cycles;
extern _Bool bool_samplingPeriod;
double vCell;

void CV_meas(struct CV_Configuration_S cvConfiguration) {

	//------------LEEMOS LA CONFIGURACIÓN DE LA VOLTOMETRÍA----------------
	double eBegin = cvConfiguration.eBegin;
	double eVertex1 = cvConfiguration.eVertex1;
	double eVertex2 = cvConfiguration.eVertex2;
	uint8_t cycles = cvConfiguration.cycles;
	double scanRate = cvConfiguration.scanRate;
	double eStep =	cvConfiguration.eStep;

	bool_samplingPeriod = FALSE;


	//----------------SETEAMOS EL PERDIODO--------------------------------
	samplingPeriod = (eStep/scanRate)*1000; //counter period que necesitamos, en ms

	__HAL_TIM_SET_AUTORELOAD(htim, samplingPeriod * 10); //seteamos el periodo
	__HAL_TIM_SET_COUNTER(htim,0);
	HAL_TIM_Base_Start_IT(htim); //iniciamos el timer con sus interrupciones


	vCell = eBegin; //Vcell que introduim amb el dac
	//----------SETEAMOS LA TENSIÓN DEL DAC-----------------------------
	// vcell --> vdac --> vdac->MCP
	MCP4725_SetOutputVoltage(hdac, eBegin); //seteamos la Vcell a eDC

	double vObjetivo = eVertex1;


	//----------------CERRAR RELÉ-------------------------------------
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
	//inicializamos los contadores a 0
	point = 0;
	counter=0;
	counter_cycles = 0;
	CV_sendData(); //enviamos el primer punto

	//-------------ENTRAMOS EN EL LOOP----------------------------------
	while (counter_cycles < cycles) { //mientras el contador de ciclos sea menor al num de ciclos determinado
		if (bool_samplingPeriod){ //si ha pasado el sampling period
			if (vObjetivo < vCell){ //si estamos decrementando ponemos valor absoluto
				eStep = -fabs(eStep); //el step ha de ser negatiu
				if ((vCell + eStep) < vObjetivo){ //si sobrepasamos el vobjetivo
						MCP4725_SetOutputVoltage(hdac, vObjetivo);
						vCell = vObjetivo; //seteamos vCell a vobjetivo
				} else{ //si no lo sobrepasamos
					vCell = vCell + eStep; //(de)incrementamos
				}
			}
			if (vObjetivo > vCell) { //si estamos incrementando no poenmos valor absoluto
				eStep=fabs(eStep);
				if ((vCell + eStep) > vObjetivo){ //si sobrepasamos el vobjetivo
						MCP4725_SetOutputVoltage(hdac, vObjetivo);
						vCell = vObjetivo; //seteamos vCell a vobjetivo
				} else{ //si no lo sobrepasamos
					vCell = vCell + eStep; //(de)incrementamos
				}
			}

			CV_sendData();
			if (fabs(vCell - vObjetivo) < MAX_VAR){ //si vCell es igual a vObjetivo
				if (fabs(vObjetivo - eVertex1) < MAX_VAR){ //si vCell es casi igual a eVertex1
					vObjetivo = eVertex2; //seteamos el objetivo al vertice 2
					}
				else if (fabs(vObjetivo - eVertex2) < MAX_VAR){ //si vCell es igual a eVertex2
					vObjetivo = eBegin;
				}
				else{ //si vCell es casi igual a eBegin
					vObjetivo = eVertex1;
					counter_cycles++; //terminamos ciclo y añadimos uno al contador
				}
			}
		}
	} //salimos del bucle cuando counter_cycles sea igual al num de ciclos predeterminado

	//----------------PARAMOS EL TIMER------------------------------
	HAL_TIM_Base_Stop_IT(htim); //paramos el timer

	//---------------------ABRIMOS RELÉ-----------------------------
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}

//-----------------FUNCIONES--------------------------------

void CV_setDac(MCP4725_Handle_T newHdac) {
	hdac = newHdac; //puntero al handle type de la hdac
}

void CV_setTimer(TIM_HandleTypeDef *newHtim) {
	htim = newHtim;//puntero al handle type del timer
}

void CV_setAdc(ADC_HandleTypeDef *newHadc) {
	hadc = newHadc;//puntero al handle type del ADC
}

void CV_sendData(void){
	bool_samplingPeriod=FALSE;
	point++;
	HAL_ADC_Start(hadc); // iniciamos la conversion de Vcell real
	HAL_ADC_PollForConversion(hadc, 200); // esperamos que finalice la conversion
	uint32_t Vref=HAL_ADC_GetValue(hadc);  //guardamos el resultado de la conversion de Vcell real
	double Vcell_real = (1.65-Vref)*2;
	HAL_ADC_Start(hadc); // iniciamos la conversion de I
	HAL_ADC_PollForConversion(hadc, 200); // esperamos que finalice la 2a conversión
	Vtia=HAL_ADC_GetValue(hadc);  // guardamos el resultado de la 2a conversión

	double iCell =(Vtia-1.65)*2/Rtia;

	//enviar valores al host

	struct Data_S data;
	data.point = point;
	data.timeMs = counter; //REVISAR!!!
	//data.voltage = Vcell_real;
	data.voltage = vCell; //prueba
	//data.current = iCell;
	data.current = vCell / 10e3; //prueba
	counter += samplingPeriod;

	MASB_COMM_S_sendData(data);
}
