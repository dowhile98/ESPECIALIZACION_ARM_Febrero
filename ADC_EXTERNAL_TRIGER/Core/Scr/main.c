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


#define ADC1_IN0		A , 0
#define ADC1_IN1		A , 1
#define ADC1_IN4		A , 4
#define ADC1_IN8		B , 0
#define ADC1_IN10		C , 0
#define ADC1_IN11		C , 1
#define USART2_TX		A , 2
#define USART2_RX		A , 3

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
uint16_t adcData;
/*****************************************************/
/**
 * @brief configuracion del ADC1
 */
void ADC1_InitSingleMode(void);

/**
 * @brief configuracion de canales
 */
void ADC1_ChanelConfiguration(void);
/**
 * @brief cofiguurar el timer2 para generar base de tiempo
 */
void TIM2_Config(uint32_t freq);

/**
 * @brief configuracion de pines del USART2
 */
void USART2_GPIOConfig(void);

int main(void)
{
	delay_Init();
	USART2_GPIOConfig();
	USART_Init(USART2, 115200);
/***********************************************/
	ADC1_ChanelConfiguration();
	ADC1_InitSingleMode();
	TIM2_Config(50);

    /* Loop forever */
	for(;;){

	}
}
/**
 * @brief configuracion del ADC1
 */
void ADC1_InitSingleMode(void){
	/*habilitar el reloj*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	/*CONFIGURAR LOS PARAMETROS DEL ADC*/
	ADC1_COMMON->CCR &=~ ADC_CCR_ADCPRE;		//PLCLK2 / 2 = 8MHz
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;

	/*CONFIGURAR EL EL TRIGER EXTERNO*/
	ADC1->CR2 |= ADC_CR2_EXTEN_0;			//disparo externo con flanco ascendente
	ADC1->CR2 |= 0x6U<<ADC_CR2_EXTSEL_Pos;	//Timer2 TRIGO event (Update event)
	/*habilitar la interrupcion*/
	ADC1->CR1 |= ADC_CR1_EOCIE;
	NVIC_EnableIRQ(ADC_IRQn);
	/*ADC ON*/
	ADC1->CR2 |= ADC_CR2_ADON;

	return;
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
	ADC1->SMPR2 |= ADC_SMPR2_SMP4_1;		//28 cycles
	ADC1->SMPR1 |= ADC_SMPR1_SMP11;			//480 cycles

	return;
}

void TIM2_Config(uint32_t freq){
	uint32_t arr;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	/*Configura el PSC y ARR*/
	/**
	 * update event = Ftim/(PSC + 1)*(ARR + 1)
	 * PSC = 0;
	 * ARR = 16MHZ/(freq) - 1:
	 */
	arr = SystemCoreClock / freq - 1;
	TIM2->PSC = 0;
	TIM2->ARR = arr;
	TIM2->CR2 &=~ TIM_CR2_MMS;
	TIM2->CR2 |= TIM_CR2_MMS_1;		//updata event como salida TRGO
	TIM2->CR1 |= TIM_CR1_CEN;		//habilita el conteo

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
void ADC_CvsCpltCallback(void){
	adcData = ADC1->DR;
	printf("ADC->%d\r\n",adcData);
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

