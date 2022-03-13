/*
 * delay.c
 *
 *  Created on: 13 mar. 2022
 *      Author: Lenovo
 */
#include "delay.h"


/*VARIABLES PRIVADAS*/
uint32_t uwTick;				//incrementar 1us/ms
/**
 * @brief Retorna el valor de la variabla ticks
 * @param none
 * @return ticks
 */
static uint32_t GetTick(void){

	return uwTick;
}

/**
 * @brief inicializa la funcion que genera retardos en milisegundos/microsegundos
 */
void delay_initIT(uint8_t priority){
	uint32_t temp;

#if USE_DELAY_US
	temp = SystemCoreClock / 1000000;
#else
	temp = SystemCoreClock / 1000;
#endif
	SysTick->CTRL = 0;
	SysTick->LOAD = temp - 1;
	SysTick->VAL = 0;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;		//sysclk
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;			//habilita la IT para la systick
/*******************************************************/
	NVIC_SetPriority(SysTick_IRQn,priority);
/*******************************************************/
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;			//habilita el conteo de la systick
	return;
}

#if USE_DELAY_US
void delay_us(uint32_t delay){
	int32_t tickstart = GetTick();
	uint32_t wait = delay;

	while((GetTick() - tickstart)<wait);				//genera un retardo
	return;
}
#endif

void delay_ms(uint32_t delay){

#if USE_DELAY_US
	uint32_t i;
	for(i = 0;i<delay;i++){
		delay_us(1000);									//retardo de 1ms
	}
#else
	int32_t tickstart = GetTick();
	uint32_t wait = delay;
	while ((GetTick() - tickstart) < wait);//retardo en ms
#endif
	return;
}
