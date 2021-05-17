/*
 * stm32main.c
 *
 *  Created on: May 7, 2021
 *      Author: Laia
 */

#include "components/stm32main.h"
#include "components/masb_comm_s.h" //to have available all the functionality defined in masb_comm_s
#include "components/chronoamperometry.h"
#include "components/ad5280_driver.h"
#include "components/mcp4725_driver.h"
#include "components/i2c_lib.h"

struct CV_Configuration_S cvConfiguration;
struct CA_Configuration_S caConfiguration;


void setup(struct Handles_S *handles) { //toma como parametro el puntero a la estrucutra Handles_S
	//-----------DESCOMENTAR QUAN SAPIGUEM PERQUÈ FALLA---------------
    MASB_COMM_S_setUart(handles->huart);
	CA_setTimer(handles->htim);
    CA_setUart(handles->huart);
    CA_setAdc(handles->hadc);
    I2C_Init(handles->hi2c);

    //------------POTENCIOMETRO--------------------
    //----borrar quan haguem fet es proves-----
    AD5280_Handle_T hpot = NULL;
    hpot = AD5280_Init();
	AD5280_ConfigSlaveAddress(hpot, 0x2C);
	AD5280_ConfigNominalResistorValue(hpot, 50e3f);
	AD5280_ConfigWriteFunction(hpot, I2C_Write); // MIRAR I2C!!
	AD5280_SetWBResistance(hpot, 10e3f); //10kohms!!

	//------------DAC------------------
    MCP4725_Handle_T hdac = NULL;
    hdac = MCP4725_Init();
    MCP4725_ConfigSlaveAddress(hdac, 0x66);
    MCP4725_ConfigVoltageReference(hdac, 4.0f);
    MCP4725_ConfigWriteFunction(hdac, I2C_Write);

    MASB_COMM_S_waitForMessage(); //esperamos la llegada de datos
}

void loop(void) {
    if (MASB_COMM_S_dataReceived()) { // Si se ha recibido un mensaje...

 		switch(MASB_COMM_S_command()) { // Miramos que comando hemos recibido

 			case START_CV_MEAS: // Si hemos recibido START_CV_MEAS

                 // Leemos la configuracion que se nos ha enviado en el mensaje y
                 // la guardamos en la variable cvConfiguration
				cvConfiguration = MASB_COMM_S_getCvConfiguration();
				ESTADO=CV;
 				__NOP(); // Esta instruccion no hace nada y solo sirve para poder anadir un breakpoint

 				break;

 			case START_CA_MEAS:
				// Leemos la configuracion que se nos ha enviado en el mensaje y
				// la guardamos en la variable caConfiguration
				caConfiguration = MASB_COMM_S_getCaConfiguration();
				ESTADO=CA;
				__NOP();

				break;


			case STOP_MEAS: // Si hemos recibido STOP_MEAS

 				/*
 				 * Debemos de enviar esto desde CoolTerm:
 				 * 020300
 				 */
				ESTADO=IDLE;
 				__NOP(); // Esta instruccion no hace nada y solo sirve para poder anadir un breakpoint

 				// Aqui iria el codigo para tener la medicion si implementais el comando stop.

 				break;

 			default: // En el caso que se envia un comando que no exista, espera otro

 				break;

 		}

       // Una vez procesado los comando, esperamos el siguiente
 		MASB_COMM_S_waitForMessage();
 	}
 	// Aqui es donde deberia de ir el codigo de control de las mediciones si se quiere implementar
   // el comando de STOP.
    switch(ESTADO){
    	case CV:
    		//Obtener siguiente punto (cyclic_voltammetry.c)
    		//enviar punto al host
    		//if(ultimo punto = true) seteamos ESTADO=IDLE

    	case CA:
    		CA_meas(caConfiguration);
    		ESTADO=IDLE;
    		break;
    		//if(ultimo punto = true) seteamos ESTADO=IDLE i break


    	case IDLE:
    		break;

    }

}


