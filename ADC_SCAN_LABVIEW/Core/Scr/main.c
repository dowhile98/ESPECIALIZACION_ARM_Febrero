
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
#include "Config.h"
#include "delay.h"
#include "USART.h"
#include <stdio.h>


#define USART2_TX			A , 2
#define USART2_RX			A , 3
#define ADC1_IN0		A , 0
#define ADC1_IN1		A , 1
#define ADC1_IN4		A , 4
#define ADC1_IN8		B , 0
#define ADC1_IN10		C , 0
#define ADC1_IN11		C , 1


#define ADC_CH0			0
#define ADC_CH1			1
#define ADC_CH2			2
#define ADC_CH3			3
#define ADC_CH4			4
#define ADC_CH5			5
#define ADC_CH6			6
#define ADC_CH7			7
#define ADC_CH8			8
#define ADC_CH9			9
#define ADC_CH10		10
#define ADC_CH11		11
#define ADC_CH12		12
#define ADC_CH13		13
#define ADC_CH14		14
#define ADC_CH15		15

/******************************************************/
uint16_t adcData[3];
USART_Handle_t usart2_handle;
uint8_t rxBuffer[20];
/*****************************************************/

/**
 * @brief configuracion de pines del USART2
 */
void USART2_GPIOConfig(void);
/**
 * @brief configuracion del ADC1
 */
void ADC1_InitScanMode(void);

/**
 * @brief configuracion de canales
 */
void ADC1_ChanelConfiguration(void);



int main(void)
{
	delay_Init();
	USART2_GPIOConfig();
	USART_Init(USART2, 115200);

	usart2_handle.pUSARTx = USART2;
	USART_IRQInterruptConfig(USART2_IRQn, ENABLE);
	USART_IRQPriorityConfig(USART2_IRQn, 1);
/***********************************************/
	ADC1_ChanelConfiguration();
	ADC1_InitScanMode();
	USART_ReceiveDataIT(&usart2_handle, rxBuffer, 1);
    /* Loop forever */
	for(;;){
		/*TRIGER*/
		ADC1->CR2 |= ADC_CR2_SWSTART;
		while(!(ADC1->SR & ADC_SR_EOC));		//wait
		adcData[0] = ADC1->DR;

		while(!(ADC1->SR & ADC_SR_EOC));		//wait
		adcData[1] = ADC1->DR;

		while(!(ADC1->SR & ADC_SR_EOC));		//wait
		adcData[2] = ADC1->DR;
		printf("%u,%u,%u\r\n",adcData[0],adcData[1],adcData[2]);
		delay_ms(100);
	}
}


/**
 * @brief configuracion de canales
 */
void ADC1_ChanelConfiguration(void){
	/*habilitar el reloj*/
	GPIO_CLOCK_ENABLE(A);
	GPIO_CLOCK_ENABLE(B);
	GPIO_CLOCK_ENABLE(C);
	/*configurar los canales en modo analogico*/
	/*PA0->CH0, PA1->CH1, PA4->CH4, PB0->CH8, PC1->CH11, PC0->CH10*/
	GPIOX_MODER(MODE_ANALOG_INPUT,ADC1_IN0);
	GPIOX_PUPDR(MODE_PU_NONE, ADC1_IN0);

	GPIOX_MODER(MODE_ANALOG_INPUT,ADC1_IN1);
	GPIOX_PUPDR(MODE_PU_NONE, ADC1_IN1);

	GPIOX_MODER(MODE_ANALOG_INPUT,ADC1_IN4);
	GPIOX_PUPDR(MODE_PU_NONE, ADC1_IN4);

	GPIOX_MODER(MODE_ANALOG_INPUT,ADC1_IN8);
	GPIOX_PUPDR(MODE_PU_NONE, ADC1_IN8);

	GPIOX_MODER(MODE_ANALOG_INPUT,ADC1_IN10);
	GPIOX_PUPDR(MODE_PU_NONE, ADC1_IN10);

	GPIOX_MODER(MODE_ANALOG_INPUT,ADC1_IN11);
	GPIOX_PUPDR(MODE_PU_NONE, ADC1_IN11);
	/*SAMPLING TIME*/
	ADC1->SMPR2 = 0; ADC1->SMPR1 = 0;
	ADC1->SMPR1 |= ADC_SMPR1_SMP11;			//480 cycles
	ADC1->SMPR1 |= ADC_SMPR1_SMP10;
	ADC1->SMPR2 |= ADC_SMPR2_SMP8;
	ADC1->SMPR2 |= ADC_SMPR2_SMP4;
	ADC1->SMPR2 |= ADC_SMPR2_SMP1;
	ADC1->SMPR2 |= ADC_SMPR2_SMP0;
	return;
}

/**
 * @brief configuracion del ADC1
 */
void ADC1_InitScanMode(void){
	/*habilitar el reloj*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	/*configurar el ADC*/
	ADC1_COMMON->CCR &=~ADC_CCR_ADCPRE;		//PCLK2 / 2 = 8MHZ
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;
	ADC1->CR1 |= ADC_CR1_SCAN;				//habilita el modo scan
	ADC1->CR2 |= ADC_CR2_EOCS;				//habilita el flag EOC para cada conversion
	/*secuencia y la cantidad de conversiones*/
	ADC1->SQR1 &=~ ADC_SQR1_L;
	ADC1->SQR1 |= (3 - 1)<<ADC_SQR1_L_Pos;
	ADC1->SQR3 = 0;
	ADC1->SQR3 |= ADC_CH11;						//1ra conversion
	ADC1->SQR3 |= ADC_CH10<<ADC_SQR3_SQ2_Pos;	//2da conversion
	ADC1->SQR3 |= ADC_CH8<<ADC_SQR3_SQ3_Pos;	//3ra conversion

	/*habilitar el ADC*/
	ADC1->CR2 |= ADC_CR2_ADON;
	return;
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
void USART_ApplicationEventCallback(USART_Handle_t *pUSARTHandle,uint8_t ApEv){
	/*recepcion completa*/
	if(ApEv == USART_EVENT_RX_CMPLT){
		if(*rxBuffer == 'S'){
			*rxBuffer = '\0';			//se limpia el buffer
			TIM2->CR1 |= TIM_CR1_CEN;
		}else{
			*rxBuffer = '\0';
			TIM2->CR1 &=~ TIM_CR1_CEN;
			TIM2->SR = 0;
			TIM2->CNT = 0;				//ser reinicia el conteo
		}
	}
	USART_ReceiveDataIT(&usart2_handle, rxBuffer, 1);
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




