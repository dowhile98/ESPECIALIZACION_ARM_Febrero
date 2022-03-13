/*
 * stm32f4xx_it.c
 *
 *  Created on: 13 mar. 2022
 *      Author: Lenovo
 */
#include "stm32f4xx.h"
#include "Config.h"
#include <stdio.h>


int count;

void EXTI15_10_IRQHandler(void){
	if(EXTI->PR & EXTI_PR_PR13){
		EXTI->PR |= EXTI_PR_PR13;		//borra el pending bit
		/*code here*/
		GPIOX_ODR(LED) ^= 1;
		count++;
		printf("INTERRUPCION ->%d\r\n",count);
	}


	return;
}

