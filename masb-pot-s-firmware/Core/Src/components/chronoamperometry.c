/**
  ******************************************************************************
  * @file		chronoamperometry.c
  * @brief		Gestión de la cronoamperometría.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

//INCLUDE OF THE CA FUNCTIONS AND THE TRANSFORMATION FORMULAS
#include "components/chronoamperometry.h"
#include "components/formulas.h"

static TIM_HandleTypeDef *htim;
static ADC_HandleTypeDef *hadc;
static MCP4725_Handle_T hdac;

volatile _Bool bool_samplingPeriod=FALSE;

//--------------VARIABLES INITIALIZATION-------------------
int32_t Vtia=0;
int32_t Rtia=10000;
uint32_t point = 0;
uint32_t counter=0; //será el measurement time cada vez que cogemos un punto
uint32_t samplingPeriod;
uint32_t measurementTime=0;

void CA_meas(struct CA_Configuration_S CA_config) {

	//------------READING THE CRONO CONFIGURATION----------------
	//We get the configuration parameters from the structure CA_configuration_S in the CA header
	double eDC=CA_config.eDC;
	samplingPeriod=CA_config.samplingPeriodMs; //ms
	measurementTime=CA_config.measurementTime*1000; //segons

	//----------DAC VOLTAGE SET-----------------------------
	float Vdac = calculateDacOutputVoltage(eDC); //pasarle con una funcion
	MCP4725_SetOutputVoltage(hdac, Vdac); //setting Vcell to eDC with the dac

	//----------------CLOSING RELE-------------------------------------
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);

	//----------------PERDIOD SETTING--------------------------------
	uint32_t counter_period = samplingPeriod* 10; //counter period que necesitamos

	__HAL_TIM_SET_AUTORELOAD(htim, counter_period); //setting the period
	__HAL_TIM_SET_COUNTER(htim,0); //Setting the counter

	HAL_TIM_Base_Start_IT(htim); //Init the TIM base generation in interrupt mode

	point=0;
	counter=0;
	CA_sendData(); //sending the first point

	//-------------ENTERING THE LOOP---------------
	//The while loop checks that we have not passed the total measurement time of the experiment
	//and only sends a point if the time between samplings has been met

	while (counter<=measurementTime) {
		if (bool_samplingPeriod==TRUE){
			//cuando haya pasado el sampling period:
			CA_sendData();
		}
	}
	HAL_TIM_Base_Stop_IT(htim); //Stop the timer

	//---------------------OPEN RELE-----------------------------
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim3) { //timer 1 sampling period
	bool_samplingPeriod = TRUE; //callback of the timer interruption
}

void CA_setDac(MCP4725_Handle_T newHdac) {
	hdac = newHdac;
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

	HAL_ADC_Start(hadc); //Vcell real conversion init
	HAL_ADC_PollForConversion(hadc, 200); //wait for the end of conversion
	uint32_t Vref=HAL_ADC_GetValue(hadc);  //saving the result
	double vCell = calculateVrefVoltage(Vref); //calibration formula

	HAL_ADC_Start(hadc); // Icell conversion init
	HAL_ADC_PollForConversion(hadc, 200); // wait for the end of conversion
	uint32_t Vtia=HAL_ADC_GetValue(hadc);  // saving the result
	double iCell = calculateIcellCurrent(Vtia); // calibration formula

	//Send values to host in a structure

	struct Data_S data;
	data.point = point;
	data.timeMs = counter;
	data.voltage = vCell;
	data.current = iCell;
	counter += samplingPeriod;

	MASB_COMM_S_sendData(data);
}
