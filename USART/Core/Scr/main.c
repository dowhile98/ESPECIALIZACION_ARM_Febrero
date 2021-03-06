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
#include "defines.h"
#include "stm32f4xx.h"
#include "USART.h"
#include <string.h>
#include <stdio.h>
/**
 * 0: printf mediante el usart2
 * 1: printf mediante el SWO
 */
#define USE_SWV			0


/**
 * @brief configuracion de pines del USART2
 */
void USART2_GPIOConfig(void);

uint8_t str[] = "hola mundo usart2\r\n";
uint8_t rxBuffer[20];

USART_Handle_t usart2;

int main(void)
{
/********************************************************/
	USART2_GPIOConfig();
	USART_Init(USART2, 115200);

	usart2.pUSARTx = USART2;
	USART_IRQInterruptConfig(USART2_IRQn, ENABLE);
	USART_IRQPriorityConfig(USART2_IRQn, 1);
/********************************************************/
	USART_SendData(USART2, str, strlen((char*)str));
//	USART_ReceiveData(USART2, rxBuffer, 5);
//	USART_SendData(USART2, rxBuffer, 5);
	USART_ReceiveDataIT(&usart2, rxBuffer, 5);
	printf("TODO ESTA CORRECTO\r\n");
    /* Loop forever */
	for(;;){

	}
}
/**
 * @brief configuracion de pines del USART2
 */
void USART2_GPIOConfig(void){
	//PA2->USART2_TX , PA3 -> USART2_RX
	RCC->AHB1ENR |= GPIOX_CLOCK(USART2_TX);
	GPIOX_MODER(MODE_ALTER,USART2_TX);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,USART2_TX);
	GPIOX_AFR(7U,USART2_TX);

	GPIOX_MODER(MODE_ALTER,USART2_RX);
	GPIOX_OSPEEDR(MODE_SPD_VHIGH,USART2_RX);
	GPIOX_AFR(7U,USART2_RX);
	return;
}

/************************************************************/
void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t event)
{
	if(USART_EVENT_RX_CMPLT == event){
		USART_SendDataIT(&usart2, rxBuffer, 5);
	}
	else if(USART_EVENT_TX_CMPLT == event){
		USART_SendData(USART2, (uint8_t*)"\r\nCOMPLETO\r\n", 12);
	}
}
/************************************************************/
int __io_putchar(int ch){
#if (USE_SWV== 1)
	ITM_SendChar((uint32_t)ch);
#else
	uint8_t c = ch & 0xFF;
	while(!(USART2->SR & USART_SR_TXE));  //espera hasta que usart este listo para transmitir otro byte
	USART2->DR = c;
#endif
	return ch;
}
