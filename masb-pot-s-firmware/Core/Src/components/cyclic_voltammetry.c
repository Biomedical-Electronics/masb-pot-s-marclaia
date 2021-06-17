/**
  ******************************************************************************
  * @file		cyclic_voltammetry.c
  * @brief		Gestión de la voltammetría cíclica.
  * @author		Albert Álvarez Carulla
  * @copyright	Copyright 2020 Albert Álvarez Carulla. All rights reserved.
  ******************************************************************************
  */

#include "components/cyclic_voltammetry.h"
#include "components/formulas.h"
#include "math.h"


//--------------POINTERS------------------
static TIM_HandleTypeDef *htim;
static ADC_HandleTypeDef *hadc;
static MCP4725_Handle_T hdac;

//--------------INIT VARIABLES-------------------

int32_t Vtia;
int32_t Rtia;
uint32_t point;
uint32_t counter_cv; //measurement time where we take a point
uint32_t samplingPeriod;
uint8_t counter_cycles;
extern _Bool bool_samplingPeriod;
double vCell;

void CV_meas(struct CV_Configuration_S cvConfiguration) {

	//------------READING THE CONFIG OF THE VOLTAMMETRY----------------
	double eBegin = cvConfiguration.eBegin;
	double eVertex1 = cvConfiguration.eVertex1;
	double eVertex2 = cvConfiguration.eVertex2;
	uint8_t cycles = cvConfiguration.cycles;
	double scanRate = cvConfiguration.scanRate;
	double eStep =	cvConfiguration.eStep;

	bool_samplingPeriod = FALSE;


	//----------------SETTING THE PERIOD--------------------------------
	samplingPeriod = (eStep/scanRate)*1000; //counter period que necesitamos, en mscounter period needed in ms

	__HAL_TIM_SET_AUTORELOAD(htim, samplingPeriod * 10); //setting the perioc
	__HAL_TIM_SET_COUNTER(htim,0); //setting the counter
	HAL_TIM_Base_Start_IT(htim); //Init the timer in interrupt mode


	//----------SETTING DAC VOLTAGE-----------------------------
	// vcell --> vdac --> vdac->MCP

	vCell = (eBegin/2.0)+2.0; //Vcell introduced with dac
	MCP4725_SetOutputVoltage(hdac, vCell); //Setting Vcell to eDC

	double vObjetivo = eVertex1;


	//----------------CLOSE RELE-------------------------------------
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
	//Init counters to 0
	point = 0;
	counter_cv=0;
	counter_cycles = 0;
	CV_sendData(); //enviamos el primer punto

	//-------------ENTERING THE LOOP----------------------------------
	while (counter_cycles < cycles) { //while the counter of voltammetry cycles is smaller than the desired cycles
		if (bool_samplingPeriod){ //and if the time between samplings (sampling period) has passed
			if (vObjetivo < vCell){ //case where the tension objective (vObjective) is below our current tension (vCell)
				eStep = -fabs(eStep); // the eStep must be negative because we are going down to the objective potential
				if ((vCell + eStep) < vObjetivo){ //in the case where er are going down in the voltammetry, if we surpass the objective tension we will have to relocate vCell to vObjetivo (which is the maximum)
						vCell = vObjetivo; //setting vCell to vObjetivo
				} else{ //if vObjetivo is not reached
					vCell = vCell + eStep; //we just decrease a value eStep (because eStep is negative)
				}
			}
			if (vObjetivo > vCell) { //Otherwise, if we are in the case where we are in vCell and vObjetivo is over it
				eStep=fabs(eStep); //The step will be positibe, in sorder to afvance towards eStep
				if ((vCell + eStep) > vObjetivo){ //in case we surpass the objective we will maintain vCell at vObjetivo and not above
						vCell = vObjetivo;
				} else{ // if we are not surpassing the objective
					vCell = vCell + eStep; //we just increment vCell a step eStep
				}
			}

			CV_sendData(); 	//the send data function restarts the sampling period bool to False, reads the 3 adc channels to obtain vCell real and iCell and sends the value points. Also it updated the pointer counter

			MCP4725_SetOutputVoltage(hdac, (vCell/2.0)+2.0); //we set vCell with the dac to the new value
			if (fabs(vCell - vObjetivo) < MAX_VAR){ // In order to compare vCell and vObjetivo we must declare a max variation between them bc they will never be exactly ==.
				if (fabs(vObjetivo - eVertex1) < MAX_VAR){ //Also if vCell is equal eVertex1
					vObjetivo = eVertex2; //we flip vObjetivo to a new value eVertex2 (the cyclic voltammetry goes from a upper corner (eVertex1) to a down one (eVertex2). vObjetivo is the one we are approaching at every moment.
					}
				else if (fabs(vObjetivo - eVertex2) < MAX_VAR){ //Otherwise, if vCell is equal eVertex2
					vObjetivo = eBegin; //we approach the beggining tension value (we have finished the cycle and approach the start point)
				}
				else{ //If vCell is equal to eBegin (taking into consideration the 3 conditions above it is the only possibe option
					vObjetivo = eVertex1; //We approach again eVertex1 (restarting the cycle)
					counter_cycles++; //terminamos ciclo y añadimos uno al contadorThe cycle counter adds one
			}
		}
	} //When the number of cycles asked have been done we leave the loop

	//----------------STOP THE TIMER------------------------------
	HAL_TIM_Base_Stop_IT(htim); //stoping the timer

	//---------------------OPEN RELE-----------------------------
	HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);
}

//-----------------FUNCTIONS--------------------------------

void CV_setDac(MCP4725_Handle_T newHdac) {
	hdac = newHdac; //pointer to handle type of hdac
}

void CV_setTimer(TIM_HandleTypeDef *newHtim) {
	htim = newHtim;//pointer to handle type of timer
}

void CV_setAdc(ADC_HandleTypeDef *newHadc) {
	hadc = newHadc;//pointer to handle type of ADC
}

void CV_sendData(void){
	bool_samplingPeriod=FALSE;
	point++; //counter
	HAL_ADC_Start(hadc); // start conversion
	HAL_ADC_PollForConversion(hadc, 200); // wait till end conversion
	uint32_t Vref=HAL_ADC_GetValue(hadc);  // saving result of conversion in Vref to obtain vcell_real
	double Vcell_real = calculateVrefVoltage(Vref);

	HAL_ADC_Start(hadc); // start conversion
	HAL_ADC_PollForConversion(hadc, 200); // wait till end conversion
	uint32_t Vtia=HAL_ADC_GetValue(hadc);  // saving result of conversion in vtia to obtain iCell
	double iCell = calculateIcellCurrent(Vtia);

	//generating a structure data with alll the values due to send to host

	struct Data_S data;
	data.point = point;
	data.timeMs = counter_cv;
	data.voltage = Vcell_real;
	data.current = iCell;
	counter_cv += samplingPeriod;

	MASB_COMM_S_sendData(data); //sending the values to the Host
}
