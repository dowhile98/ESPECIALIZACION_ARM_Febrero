
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

/**
 * @brief cofiguurar el timer2 para generar base de tiempo
 */
void TIM2_Config(uint32_t freq);

/**
 * @brief  configurar el DAM2 stream 4 para el ADC
 */
void DMA2_Stream4Config(void);



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
/***********************************************/
	DMA2_Stream4Config();
	TIM2_Config(200);
    /* Loop forever */
	for(;;){

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
	/*CONFIGURAR EL EL TRIGER EXTERNO*/
	ADC1->CR2 |= ADC_CR2_EXTEN_0;			//disparo externo con flanco ascendente
	ADC1->CR2 |= 0x6U<<ADC_CR2_EXTSEL_Pos;	//Timer2 TRIGO event (Update event)

	/*DMA REQUEST*/
	ADC1->CR2 |=  1U<<10;
	ADC1->CR2 |= ADC_CR2_DDS;
	ADC1->CR2 |= ADC_CR2_DMA;				//DMA REQUEST FOR HW
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


	return;
}

/**
 * @brief  configurar el DAM2 stream 4 para el ADC
 */
void DMA2_Stream4Config(void){
	/*habilitar el reloj*/
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	/* 1. deshabilitar el stream*/
	DMA2_Stream4->CR &=~ DMA_SxCR_EN;
	while(DMA2_Stream4->CR & DMA_SxCR_EN);
	DMA2->HIFCR |= 0xF<<2 | 1U;				//clear para el stream 4

	/*2. Seleccionar el SxPAR*/
	DMA2_Stream4->PAR = (uint32_t)&(ADC1->DR);

	/*3. selecionar el SxMAR*/
	DMA2_Stream4->M0AR = (uint32_t)adcData;
	/*4. Configurar el umero de datos a ser transferido*/
	DMA2_Stream4->NDTR = 3;

	/*5. Seleccionar el canal DMA*/
	DMA2_Stream4->CR &=~ DMA_SxCR_CHSEL;	//Se selecciona el channel 0
	/*6. */

	/*7. Configurar la prioridad del stream*/
	DMA2_Stream4->CR |= DMA_SxCR_PL;		//very high priority

	/*8. Configurar la FIFO*/
	DMA2_Stream4->FCR &=~ DMA_SxFCR_DMDIS;	//direct mode
	/*9. Configuracion del modo de transferencia, tamaÃ±o de datos, modo circular etc*/

	DMA2_Stream4->CR &=~ DMA_SxCR_MSIZE;
	DMA2_Stream4->CR |= DMA_SxCR_MSIZE_0;	//HALF-WORD (16BIT)
	DMA2_Stream4->CR &=~ DMA_SxCR_PSIZE;
	DMA2_Stream4->CR |= DMA_SxCR_PSIZE_0;	//HALF-WORD (16BIT)
	DMA2_Stream4->CR |= DMA_SxCR_MINC;		//memory increment mode
	DMA2_Stream4->CR |= DMA_SxCR_CIRC;		//modo circular

	/*10. configurar la interrupciones*/
	DMA2_Stream4->CR |= 1U<<4 | 1U<<3 | 1U<<2 | 1U<<1;
	NVIC_EnableIRQ(DMA2_Stream4_IRQn);
	DMA2_Stream4->FCR &=~ (DMA_SxFCR_FTH);

	//habilita el estream
	DMA2_Stream4->CR |= DMA_SxCR_EN;		//SE HABILITA EL STREAM

	return;
}

void DMA_TrnsmitCpltCallback(void){
	/*envia datos hacia labview*/
	printf("%u,%u,%u\r\n",adcData[0],adcData[1],adcData[2]);
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




