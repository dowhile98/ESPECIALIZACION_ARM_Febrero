/*
 * delay.h
 *
 *  Created on: 13 mar. 2022
 *      Author: Lenovo
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_


#include "stm32f4xx.h"
/**
 * 1: retardo en milisegundos y  microsegundos
 * 0: retardo en milisegundos
 */
#define USE_DELAY_US		1


/**
 * @brief inicializa la funcion que genera retardos en milisegundos/microsegundos
 * @param[priority]: prioridad de la IT de la SysTick
 */
void delay_initIT(uint8_t priority);

/**
 * @brief funcion que genera retardos en milisegudos
 * @param[delay]: cantidad de retardos en milisegundos
 * @return none
 */
void delay_ms(uint32_t delay);

#if USE_DELAY_US

/**
 * @brief funcion que genera retardos en microsegundos
 * @param[delay]: cantidad de retardos en milisegundos
 * @return none
 */
void delay_us(uint32_t delay);

#endif


#endif /* INC_DELAY_H_ */
