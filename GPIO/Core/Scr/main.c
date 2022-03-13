/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f4xx.h"
#include <stdio.h>
/**
 * SWV
 * 0:   USART2 PORT
 * 1:   ITM    PORT
 * PB3  SW0 -> permite enviar mensaje usando el printf (STDIO.H)
 */
#define SWV			1
/**
 * PA5 -> LED
 * PC13 -> USER BUTTON
 */
void GPIO_Config(void);


int main(void)
{
	GPIO_Config();

	GPIOA->MODER |= GPIO_MODER_MODE4_0;
	//PA4
	//GPIOA->ODR |= 1U<<4;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER &=~ GPIO_MODER_MODE1;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD1_0;


	printf("CONFIGURACION DE PINES CORRECTO\r\n");
	GPIOA->ODR |= 1U<<5;
	printf("LED ENCENDIDO\r\n");
//	GPIOA->ODR &=~ 1U<<5;
//	printf("LED APAGADO\r\n");
//	GPIOA->BSRR |= GPIO_BSRR_BS5;
//	GPIOA->BSRR |= GPIO_BSRR_BR5;

    /* Loop forever */
	for(;;){
		if(!(GPIOB->IDR & 1U<<1)){
			GPIOA->BSRR |= GPIO_BSRR_BS4;
			printf("LED ENCENDIDO\r\n");
		}else{
			printf("LED APAGADO\r\n");
			GPIOA->BSRR |= GPIO_BSRR_BR4;
		}

	}
}

/**
 * PA5 -> LED
 * PC13 -> USER BUTTON
 */
void GPIO_Config(void){
	//Habilitar el reloj
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN; // 1U<<2 | 1U;
	//PA5 -> SALIDA
	GPIOA->MODER &=~ (GPIO_MODER_MODE5);			//RESET
	GPIOA->MODER |= GPIO_MODER_MODE5_0;				//salida de proposito general
	GPIOA->OTYPER &=~ GPIO_OTYPER_OT5;				//push pull
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;			//very high speed
	GPIOA->PUPDR &=~ GPIO_PUPDR_PUPD5;				//no pull up/pull down
	//PC13 -> ENTRADA
	GPIOC->MODER &=~ GPIO_MODER_MODE13;				//entrada
	GPIOC->PUPDR &=~ GPIO_PUPDR_PUPD13;				//no pull up/pull down
	return;
}
/**********************************************************************************/

int __io_putchar(int ch){
#if SWV
	ITM_SendChar(ch);
#else
	//USART2

#endif
	return ch;
}