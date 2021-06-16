/*
 * stm32main.c
 *
 *  Created on: May 7, 2021
 *      Author: Laia
 */

#include "components/stm32main.h"
#include "components/masb_comm_s.h" //to have available all the functionality defined in masb_comm_s
#include "components/chronoamperometry.h" //to have available the CA functions
#include "components/cyclic_voltammetry.h" //to have available the CV functions
#include "components/ad5280_driver.h" //
#include "components/mcp4725_driver.h"
#include "components/i2c_lib.h"

struct CV_Configuration_S cvConfiguration; //we assign the CV config structure to cvConfiguration variable
struct CA_Configuration_S caConfiguration;//we assign the CA config structure to caConfiguration variable
//struct Data_S data;
MCP4725_Handle_T hdac = NULL;

// SETUP FUNCTION: CALLED ONLY ONCE WHEN THE PROGRAM IS STARTED
void setup(struct Handles_S *handles) { //takes as parameter the Handles_S structure
	HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, GPIO_PIN_SET);
	HAL_Delay(500);

	//we pass the pointers to each of the functions in CA, CV, MASB_COMM_S and I2C, so that the pointers
	//are available to each file
	MASB_COMM_S_setUart(handles->huart);
	CA_setTimer(handles->htim);
	CA_setAdc(handles->hadc);
	CV_setTimer(handles->htim);
	CV_setAdc(handles->hadc);
	I2C_init(handles->hi2c);

	//------------POTENCIOMETRO--------------------
	//----borrar quan haguem fet es proves-----
	AD5280_Handle_T hpot = NULL;
	hpot = AD5280_Init();
	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	AD5280_ConfigWriteFunction(hpot, I2C_write); // MIRAR I2C!!
	AD5280_SetWBResistance(hpot, 50e3f); //50kohms!!

	//------------DAC------------------

	hdac = MCP4725_Init(); //we initialize the DAC
	MCP4725_ConfigSlaveAddress(hdac, 0x66); //we configure the slave adress
	MCP4725_ConfigVoltageReference(hdac, 4.0f);
	MCP4725_ConfigWriteFunction(hdac, I2C_write);

	//we make the dac pointer avilable in both CA and CV files
	CA_setDac(hdac);
	CV_setDac(hdac);

	MASB_COMM_S_waitForMessage(); //esperamos la llegada de datos

	ESTADO = IDLE;
}

//VOID FUNCTION: CALLED REPITETIVELY INSIDE THE WHILE LOOP UNTIL THE PROGRAM IS STOPED
void loop(void) {
	if (MASB_COMM_S_dataReceived()) { // If a message has been received

		switch (MASB_COMM_S_command()) { // We check what we have received

		case START_CV_MEAS: // If START_CV_MEAS was received
			//we read the received configuration and store it in the cvConfiguration variable
			cvConfiguration = MASB_COMM_S_getCvConfiguration();
			ESTADO = CV; //we change the estado variable to CV (0)
			__NOP(); // To place a breakpoint, can be erased

			break;

		case START_CA_MEAS: // If START_CA_MEAS was received
			//we read the received configuration and store it in the caConfiguration variable
			caConfiguration = MASB_COMM_S_getCaConfiguration();
			ESTADO = CA; //we change the estado variable to CA (1)
			__NOP();

			break;

		case STOP_MEAS: // If STOP_MEAS was received

			ESTADO = IDLE;
			__NOP();

			break;

		default: // If an unknown command is received, wait for another message

			break;

		}
		// We wait for another message
		MASB_COMM_S_waitForMessage();
	} else { //if a command is not received, meaning that we have already received one
		//we check the value of the ESTADO variable
		switch (ESTADO) {
		case CV: //if it is CV we call the CV_meas function
			CV_meas(cvConfiguration);
			ESTADO = IDLE; //change the ESTADO to idle (2)
			__NOP();
			break;
			//if(ultimo punto = true) seteamos ESTADO=IDLE i break

		case CA:
			CA_meas(caConfiguration);
			ESTADO = IDLE;
			__NOP();
			break;
			//if it is the last point, ESTADO=IDLE and break

		default: //If ESTADO idle (2)
			break;

		}

	}


}

