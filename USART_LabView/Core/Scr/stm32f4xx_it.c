/*
 * stm32f4xx_it.c
 *
 *  Created on: 19 mar. 2022
 *      Author: Lenovo
 */
#include "USART.h"



extern USART_Handle_t usart2;


void USART2_IRQHandler(void){
	/*el manejador de las interrupciones*/
	USART_IRQHandling(&usart2);
}

