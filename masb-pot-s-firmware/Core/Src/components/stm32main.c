/*
 * stm32main.c
 *
 *  Created on: May 7, 2021
 *      Author: Laia
 */

#include "components/stm32main.h"
#include "components/masb_comm_s.h" //to have available all the functionality defined in masb_comm_s


void setup(void) { //toma como parametro el puntero a la estrucutra Handles_S
    MASB_COMM_S_waitForMessage(); //esperamos la llegada de datos
}

void loop(void) {

}
