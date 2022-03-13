/*
 * stm32f4xx_it.c
 *
 *  Created on: 13 mar. 2022
 *      Author: Lenovo
 */

#include "stm32f4xx.h"

extern uint32_t uwTick;


void SysTick_Handler(void){
	if(SysTick->CTRL & 1U<<16){		//Verifica la bandero de IT
		uwTick += 1;
	}
	return;
}
