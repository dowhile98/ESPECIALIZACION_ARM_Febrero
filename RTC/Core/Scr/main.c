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
#include "RCC.h"
#include "USART.h"
#include <stdio.h>

#define USE_SWV			0


typedef enum{
	LUNES = 0,
	MARTES,
	MIERCOLES,
	JUEVES,
	VIERNES,
	SABADO,
	DOMINGO
}DAY_Def;

typedef struct
{
  uint8_t WeekDay;
  uint8_t Month;
  uint8_t Date;
  uint8_t Year;

}DateTypeDef;

/**
  * @brief  RTC Time structure definition
  */
typedef struct
{
  uint8_t Hours;
  uint8_t Minutes;
  uint8_t Seconds;
  uint8_t TimeFormat;
  uint32_t SubSeconds;
  uint32_t SecondFraction;
  uint32_t DayLightSaving;
  uint32_t StoreOperation;
}TimeTypeDef;

/**
 * Global variables
 */
DateTypeDef date;
TimeTypeDef time;


void TIM4_BaseIT(uint32_t update);
/**
 * @brief configura los parametros los rtc
 */
void RTC_Config(void);
/**
 * @brief contigurar el tiempo
 */
void RTC_SetTime(uint8_t hours,uint8_t minutes,uint8_t seconds);
/**
 * @brief configurar la fecha
 */
void RTC_SetDate(uint8_t WeekDay,uint8_t Date,uint8_t Month,uint8_t Year);

/**
 * @brief lectura de la hora
 */
void RTC_GetTime(TimeTypeDef *sTime);
/**
 * @brief lectura de la fecha
 */
void RTC_GetDate(DateTypeDef *sDate);

void delay_ms(uint32_t delay){
	SysTick->CTRL = 0;
	SysTick->LOAD = SystemCoreClock / 1000 - 1;
	SysTick->VAL = 0;
	SysTick->CTRL |= 1U<<2 | 1U<<0;
	for(uint32_t i=0;i<delay;i++){
		while(!(SysTick->CTRL & 1U<<16));
	}
	SysTick->CTRL = 0;
	return;
}
int main(void)
{
	/*flash configuration*/
	flash_cofig();
	/*external clock configuration*/
	HSE_Config();
	/*pll configution*/
	PLL_Config();
/*************************************************************************/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOX_MODER(MODE_OUT,LED);
/*************************************************************************/
	USART_Init(USART2, 115200);
	printf("TEST RTC\r\n");
/*************************************************************************/
	//TIM4_BaseIT(2000);
/*************************************************************************/
	RTC_Config();
	RTC_SetTime(10, 0, 0);
	RTC_SetDate(SABADO, 26, 2, 22);
    /* Loop forever */
	for(;;){
		RTC_GetDate(&date);
		RTC_GetTime(&time);
		printf("%02d:%02d:%02d  ",time.Hours,time.Minutes,time.Seconds);
		printf("%02d-%2d-%2d\r\n",date.Date,date.Month, (2000 + date.Year));
		delay_ms(200);
	}
}

void TIM4_BaseIT(uint32_t update){
	uint16_t arr;
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	/*configuration timer*/
	TIM4->CR1 = 0;
	TIM4->PSC = 8400 - 1;

	arr = (SystemCoreClock / (TIM4->PSC + 1)) * ((float)update/1000)  - 1;

	TIM4->ARR = arr;
	//enable interrutp
	TIM4->DIER |= TIM_DIER_UIE;
	NVIC_SetPriority(TIM4_IRQn,4);
	NVIC_EnableIRQ(TIM4_IRQn);

	TIM4->CR1 |= TIM_CR1_CEN;
	return;
}


/**
 * @brief configura los parametros los rtc
 */
void RTC_Config(void){
	uint32_t tmpreg1 = 0U;
	/*enable clock for PWR and rtc backup domain*/
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;
	while(!(PWR->CR & PWR_CR_DBP));

	/*enable LSE*/
	RCC->BDCR |= RCC_BDCR_LSEON;
	/*wait*/
	while(!(RCC->BDCR & RCC_BDCR_LSERDY));
	/*Seleccionar la fuente del RTC*/


	tmpreg1 = (RCC->BDCR & RCC_BDCR_RTCSEL);
	tmpreg1 = (RCC->BDCR & ~(RCC_BDCR_RTCSEL));
	RCC->BDCR |= RCC_BDCR_BDRST;
	RCC->BDCR &=~ RCC_BDCR_BDRST;
	/* Restore the Content of BDCR register */
	RCC->BDCR = tmpreg1;

	RCC->BDCR &=~ RCC_BDCR_RTCSEL;
	RCC->BDCR |= RCC_BDCR_RTCSEL_0;	//LSE
	/*Habilitar el reloj del RTC*/
	RCC->BDCR |= RCC_BDCR_RTCEN;
/******************************************************/
	/* Disable the write protection for RTC registers */
	RTC->WPR = 0xCAU;
	RTC->WPR = 0x53U;
	/* Check if the Initialization mode is set */
	if(!(RTC->ISR  & RTC_ISR_INITF)){
		/* Set the Initialization mode */
		RTC->ISR = 0xFFFFFFFFU;
		while(!(RTC->ISR & RTC_ISR_INITF));
	}else{
		RTC->WPR = 0xFFU;
		return;
	}
/******************************************************/
	/* Clear RTC_CR FMT, OSEL and POL Bits */
	RTC->CR &= ((uint32_t)~(RTC_CR_FMT | RTC_CR_OSEL | RTC_CR_POL));
	 /* Set RTC_CR register */
	RTC->CR |= RTC_CR_FMT;			//AM/PM hour format
	/* Configure the RTC PRER */
	RTC->PRER = 255;
	RTC->PRER |= 127<<16U;

	/* Exit Initialization mode */
	RTC->ISR &= (uint32_t)~RTC_ISR_INIT;
	/* If  CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
	if(!(RTC->CR & RTC_CR_BYPSHAD)){
		/* Clear RSF flag */
		RTC->ISR &= 0xFFFFFF5FU;
		while(!(RTC->ISR & RTC_ISR_RSF));
	}

	RTC->TAFCR &= (uint32_t)~RTC_TAFCR_ALARMOUTTYPE;
	RTC->TAFCR |= 0;

	/* Enable the write protection for RTC registers */
	RTC->WPR = 0xFFU;

	return;
}
/**
  * @brief  Converts a 2 digit decimal to BCD format.
  * @param  Value Byte to be converted
  * @retval Converted byte
  */
uint8_t RTC_ByteToBcd2(uint8_t Value)
{
  uint32_t bcdhigh = 0U;

  while(Value >= 10U)
  {
    bcdhigh++;
    Value -= 10U;
  }

  return  ((uint8_t)(bcdhigh << 4U) | Value);
}

/**
  * @brief  Converts from 2 digit BCD to Binary.
  * @param  Value BCD value to be converted
  * @retval Converted word
  */
uint8_t RTC_Bcd2ToByte(uint8_t Value)
{
  uint32_t tmp = 0U;
  tmp = ((uint8_t)(Value & (uint8_t)0xF0) >> (uint8_t)0x4) * 10;
  return (tmp + (Value & (uint8_t)0x0F));
}


/**
 * @brief contigurar el tiempo
 */
void RTC_SetTime(uint8_t hours,uint8_t minutes,uint8_t seconds){
	uint32_t tmpreg = 0U;

	//cambiar el formato de binario a bcd
	tmpreg = (uint32_t)RTC_ByteToBcd2(hours) << 16U;
	tmpreg |= ((uint32_t)RTC_ByteToBcd2(minutes) << 8U);
	tmpreg |= ((uint32_t)RTC_ByteToBcd2(seconds));

	/* Disable the write protection for RTC registers */
	RTC->WPR = 0xCAU;
	RTC->WPR = 0x53U;
	/* Set Initialization mode */
	if(!(RTC->ISR  & RTC_ISR_INITF)){

		RTC->ISR = 0xFFFFFFFFU;
		while(!(RTC->ISR & RTC_ISR_INITF));
	}else{
		RTC->WPR = 0xFFU;
		return;
	}
	/* Set the RTC_TR register */
	RTC->TR = ((uint32_t)tmpreg & 0x007F7F7FU);
	/* This interface is deprecated. To manage Daylight Saving Time, please use HAL_RTC_DST_xxx functions */
	RTC->CR &=(uint32_t)~RTC_CR_BCK;
	/* This interface is deprecated. To manage Daylight Saving Time, please use HAL_RTC_DST_xxx functions */

	/* Exit Initialization mode */

	RTC->ISR &= (uint32_t)~RTC_ISR_INIT;


	/* If  CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
	if(!(RTC->CR & RTC_CR_BYPSHAD)){
		/* Clear RSF flag */
		RTC->ISR &= 0xFFFFFF5FU;
		while(!(RTC->ISR & RTC_ISR_RSF));
	}

	/* Enable the write protection for RTC registers */
	RTC->WPR = 0xFFU;

	return;
}


/**
 * @brief configurar la fecha
 */
void RTC_SetDate(uint8_t WeekDay,uint8_t Date,uint8_t Month,uint8_t Year){
	uint32_t datetmpreg = 0U;

	datetmpreg = (((uint32_t)RTC_ByteToBcd2(Year) << 16U) | \
	                 ((uint32_t)RTC_ByteToBcd2(Month) << 8U) | \
	                 ((uint32_t)RTC_ByteToBcd2(Date)) | \
	                 ((uint32_t)WeekDay << 13U));

	/* Disable the write protection for RTC registers */
	RTC->WPR = 0xCAU;
	RTC->WPR = 0x53U;
	/* Set Initialization mode */
	if(!(RTC->ISR  & RTC_ISR_INITF)){

		RTC->ISR = 0xFFFFFFFFU;
		while(!(RTC->ISR & RTC_ISR_INITF));
	}else{
		RTC->WPR = 0xFFU;
		return;
	}

	 /* Set the RTC_DR register */
	RTC->DR = (uint32_t)(datetmpreg & 0x00FFFF3FU);

	/* Exit Initialization mode */
	RTC->ISR &= (uint32_t)~RTC_ISR_INIT;
	/* If  CR_BYPSHAD bit = 0, wait for synchro else this check is not needed */
	if(!(RTC->CR & RTC_CR_BYPSHAD)){
		/* Clear RSF flag */
		RTC->ISR &= 0xFFFFFF5FU;
		while(!(RTC->ISR & RTC_ISR_RSF));
	}
	/* Enable the write protection for RTC registers */
	RTC->WPR = 0xFFU;
	return;
}

void RTC_GetTime(TimeTypeDef *sTime)
{
  uint32_t tmpreg = 0U;


  /* Get subseconds structure field from the corresponding register */
  sTime->SubSeconds = (uint32_t)(RTC->SSR);

  /* Get SecondFraction structure field from the corresponding register field*/
  sTime->SecondFraction = (uint32_t)(RTC->PRER & RTC_PRER_PREDIV_S);

  /* Get the TR register */
  tmpreg = (uint32_t)(RTC->TR & 0x007F7F7FU);

  /* Fill the structure fields with the read parameters */
  sTime->Hours = (uint8_t)((tmpreg & (RTC_TR_HT | RTC_TR_HU)) >> 16U);
  sTime->Minutes = (uint8_t)((tmpreg & (RTC_TR_MNT | RTC_TR_MNU)) >> 8U);
  sTime->Seconds = (uint8_t)(tmpreg & (RTC_TR_ST | RTC_TR_SU));
  sTime->TimeFormat = (uint8_t)((tmpreg & (RTC_TR_PM)) >> 16U);

   /* Convert the time structure parameters to Binary format */
  sTime->Hours = (uint8_t)RTC_Bcd2ToByte(sTime->Hours);
  sTime->Minutes = (uint8_t)RTC_Bcd2ToByte(sTime->Minutes);
  sTime->Seconds = (uint8_t)RTC_Bcd2ToByte(sTime->Seconds);

  return;
}


void RTC_GetDate(DateTypeDef *sDate)
{
  uint32_t datetmpreg = 0U;


  /* Get the DR register */
  datetmpreg = (uint32_t)(RTC->DR & 0x00FFFF3FU);

  /* Fill the structure fields with the read parameters */
  sDate->Year = (uint8_t)((datetmpreg & (RTC_DR_YT | RTC_DR_YU)) >> 16U);
  sDate->Month = (uint8_t)((datetmpreg & (RTC_DR_MT | RTC_DR_MU)) >> 8U);
  sDate->Date = (uint8_t)(datetmpreg & (RTC_DR_DT | RTC_DR_DU));
  sDate->WeekDay = (uint8_t)((datetmpreg & (RTC_DR_WDU)) >> 13U);

  /* Convert the date structure parameters to Binary format */
  sDate->Year = (uint8_t)RTC_Bcd2ToByte(sDate->Year);
  sDate->Month = (uint8_t)RTC_Bcd2ToByte(sDate->Month);
  sDate->Date = (uint8_t)RTC_Bcd2ToByte(sDate->Date);

  return ;
}


/******************************************************************************/

void TIM4_IRQHandler(void){
	if(TIM4->SR & TIM_SR_UIF){
		TIM4->SR &=~ (TIM_SR_UIF);
		GPIOX_ODR(LED) ^= 1;

		/*mostrar el tiempo*/
		RTC_GetDate(&date);
		RTC_GetTime(&time);
		printf("%02d:%02d:%02d  ",time.Hours,time.Minutes,time.Seconds);
		printf("%02d-%2d-%2d\r\n",date.Date,date.Month, (2000 + date.Year));

	}
	return;
}

/******************************************************************************/
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
