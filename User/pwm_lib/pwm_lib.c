
/**	\file		pwm_lib.c
*		\author	Alex
*		\date		29.07.2020
*
* Библиотека для работы с ШИМ на STM32F0
*
* Предположим, что мы производим устройства с ШИМ-выходами, для управления светильниками и другим оборудованием. 
* Эти устройства имеют несколько выходов. Они построены на микроконтроллере STM32F030K6.
* Все выходы ШИМ инженер-схемотехник всегда заводил на выходы TIMx_CHx. 
* Однако на разных устройствах разное количество выходов, и подключены они в разном порядке и к разным ножкам микроконтроллера.
* устройства перечень выходов (1-N)
* WB-PWM2      PA10 PA11
* WB-PWM4      PA6 PA10 PA8 PA7
* WB-PWM8      PB14 PB7 PA6 PA10 PA8 PA7 PB1 PB4
* Нам нужно сделать прошивки для каждого из этих трёх устройств. Прошивки должны использовать одну библиотеку для управления выходами. 
* Ваша задача - реализовать эту библиотеку.
* Выходы должны описываться в универсальном формате, чтобы библиотеку можно было использовать и с другими устройствами. 
* Частота и разрядность ШИМ должна быть одинаковая на всех выходах. 
* Управление выходами должно происходить через функцию вида: pwm_out_set(uint8_t channel_index, uint16_t value)
* Нужно сделать тестовую прошивку, которая будет демонстрировать использование библиотеки. 
* Реализовать возможность сборки прошивки для каждого устройства. 
* Что должно получиться в результате:
* Библиотека для ШИМ в git репозитории с README с примером использования и инструкциями по сборке
* Демонстрационная прошивка для WB-PWM*, использующая библиотеку в git репозитории с тремя таргетами: под WB-PWM2, WB-PWM4, WB-PWM8
* с README с описанием сборки и добавления нового таргета
*/

#include "pwm_lib.h"

#define TIM_CCMR_CLR_MASK_1						0xff00
#define TIM_CCMR_CLR_MASK_2						0x00ff
#define TIM_PWM1_MODE									( 6 << 4 )

static pwm_out_t* pwms = NULL;
static uint8_t pwm_count = 0;
static uint16_t	timer_prescaler = 0xffff;
static uint16_t	timer_counter_max = 0xffff;

/** Установка предделителя для используемых тайиеров
*	\param[in]	value			- значение предделителя для всех таймеров
*/
void set_pwm_prescaler( uint16_t value ) {
	timer_prescaler = value;
	for( uint8_t i = 0; i < pwm_count; i++ ) {
		pwms[i].Tim->PSC = value;
	}
}

/** Чтение предделителя для используемых тайиеров
*	\return	- значение предделителя таймеров
*/
uint16_t get_pwm_prescaler( void ) {
	return timer_prescaler;
}

/** Установка максимального значения счётчика для используемых тайиеров
*	\param[in]	value			- верхнее значение счётчика для всех таймеров
*/
void set_pwm_max( uint16_t value ) {
	timer_counter_max = value;
	for( uint8_t i = 0; i < pwm_count; i++ ) {
		uint16_t temp = pwms[i].Tim->CR1;
		pwms[i].Tim->CR1 = 0;
		pwms[i].Tim->ARR = value;
		pwms[i].Tim->CNT = 0;
		pwms[i].Tim->CR1 = temp;
	}	
}

/** Чтение максимального значения счётчика для используемых тайиеров
*	\return	- значение счётчика для всех таймеров
*/
uint16_t get_pwm_max( void ) {
	return timer_counter_max;
}

/** Инициализация таймеров
*	\param[in]	config	- указатель на структуру-конфигуратор
*	\return							- статус опреации 0 - успешно, иначе - ошибка
*/
static int init_tim( pwm_out_t* config ) {
	config->Tim->CR1 = 0;
	switch( config->Channel & ~TIM_CANNEL_N ) {
		case TIM_CHANNEL_1 : {
			config->Tim->CCR1		= 0;
			config->Tim->CCMR1 	&= TIM_CCMR_CLR_MASK_1;
			config->Tim->CCMR1 	|= TIM_PWM1_MODE;
			config->Tim->CCER		&= ~0x000f;
			if( config->Channel & TIM_CANNEL_N ) {
				config->Tim->CCER	|= TIM_CCER_CC1NE;
			}		
			else {
				config->Tim->CCER	|= TIM_CCER_CC1E;
			}	
			break;
		}
		case TIM_CHANNEL_2 : {
			config->Tim->CCR2		= 0;			
			config->Tim->CCMR1 &= TIM_CCMR_CLR_MASK_2;
			config->Tim->CCMR1 |= TIM_PWM1_MODE << 8;	
			config->Tim->CCER		&= 0xff0f;
			if( config->Channel & TIM_CANNEL_N ) {
				config->Tim->CCER	|= TIM_CCER_CC1NE << 4;
			}		
			else {
				config->Tim->CCER	|= TIM_CCER_CC1E << 4;
			}				
			break;
		}
		case TIM_CHANNEL_3 : {
			config->Tim->CCR3		= 0;			
			config->Tim->CCMR2 &= TIM_CCMR_CLR_MASK_1;
			config->Tim->CCMR2 |= TIM_PWM1_MODE;
			config->Tim->CCER		&= 0xf0ff;
			if( config->Channel & TIM_CANNEL_N ) {
				config->Tim->CCER	|= TIM_CCER_CC1NE << 8;
			}		
			else {
				config->Tim->CCER	|= TIM_CCER_CC1E << 8;
			}			
			break;
		}
		case TIM_CHANNEL_4 : {
			config->Tim->CCR4		= 0;			
			config->Tim->CCMR2 &= TIM_CCMR_CLR_MASK_2;
			config->Tim->CCMR2 |= TIM_PWM1_MODE << 8;
			config->Tim->CCER		&= 0x0fff;
			if( config->Channel & TIM_CANNEL_N ) {
				config->Tim->CCER	|= TIM_CCER_CC1NE << 12;
			}		
			else {
				config->Tim->CCER	|= TIM_CCER_CC1E << 12;
			}			
			break;
		}
		default : return PWM_RES_FAULT;
	}
	config->Tim->BDTR = TIM_BDTR_MOE;
	config->Tim->PSC = timer_prescaler;
	config->Tim->ARR = timer_counter_max;
	config->Tim->CR1 = TIM_CR1_CEN;
	
	return PWM_RES_OK;
}

/** Инициализация используемых выходов ШИМ
*	\param[in]	pwm				-	указатель на массив описателей выходов
*	\param[in]	count			-	количество используемых выходов
*	\return								- статус опреации 0 - успешно, иначе - ошибка
*/
int init_pwm_out( pwm_out_t* pwm, uint8_t count ) {
	
	if( NULL == pwm ) return PWM_RES_FAULT;
	if( 0 == count ) return PWM_RES_FAULT;
	
	pwms = pwm;
	pwm_count = count;
	
	for( uint8_t i = 0; i < pwm_count; i++ ) {

		if( NULL == pwms[i].Tim ) return PWM_RES_FAULT;		
		
		uint8_t idx = 0;
		uint16_t pin = pwms[i].Pin;
		while(( pin & 0x0001 ) == 0 ) {
			idx++;
			pin >>= 1;
		}		
		
		switch(( uint32_t )pwms[i].Port ) {
			case ( uint32_t )GPIOA : {
				RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
				break;
			}
			case ( uint32_t )GPIOB : {
				RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
				break;
			}
			case ( uint32_t )GPIOC : {
				RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
				break;
			}
			case ( uint32_t )GPIOD : {
				RCC->AHBENR |= RCC_AHBENR_GPIODEN;
				break;
			}			
		}
		
		pwms[i].Port->BRR 		= 	pwms[i].Pin;
		pwms[i].Port->MODER 	&= 	~( 3 << ( idx * 2 ));
		pwms[i].Port->MODER 	|= 	( 2 << ( idx * 2 ));		
		pwms[i].Port->OTYPER 	&= 	~pwms[i].Pin;
		pwms[i].Port->OSPEEDR &= 	~( 3 << ( idx * 2 ));
		pwms[i].Port->PUPDR 	&= 	~( 3 << ( idx * 2 ));
		
		if( idx < 8 ) {
			pwms[i].Port->AFR[0] &= ~( 0x000f << ( idx * 4 ));
			pwms[i].Port->AFR[0] |= pwms[i].Alter << ( idx * 4 );
		}
		else {
			pwms[i].Port->AFR[1] &= ~( 0x000f << (( idx - 8 ) * 4 ));
			pwms[i].Port->AFR[1] |= pwms[i].Alter << (( idx - 8 ) * 4 );			
		}
		switch(( uint32_t )pwm[i].Tim ) {
			case ( uint32_t )TIM1 : {
				RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
				if( PWM_RES_OK != init_tim( &pwms[i] )) return PWM_RES_FAULT;
				break;
			}
			case ( uint32_t )TIM3 : {
				RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
				if( PWM_RES_OK != init_tim( &pwms[i] )) return PWM_RES_FAULT;				
				break;
			}
			case ( uint32_t )TIM14 : {
				if(( pwms[i].Channel & ~TIM_CANNEL_N ) != TIM_CHANNEL_1 ) return PWM_RES_FAULT;
				RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
				if( PWM_RES_OK != init_tim( &pwms[i] )) return PWM_RES_FAULT;				
				break;
			}	
			case ( uint32_t )TIM15 : {
//				if((( pwms[i].Channel & ~TIM_CANNEL_N ) != TIM_CHANNEL_1 ) || (( pwms[i].Channel & ~TIM_CANNEL_N ) != TIM_CHANNEL_2 )) return PWM_RES_FAULT;
				RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
				if( PWM_RES_OK != init_tim( &pwms[i] )) return PWM_RES_FAULT;				
				break;
			}
			case ( uint32_t )TIM16 : {
				if(( pwms[i].Channel & ~TIM_CANNEL_N ) != TIM_CHANNEL_1 ) return PWM_RES_FAULT;
				RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
				if( PWM_RES_OK != init_tim( &pwms[i] )) return PWM_RES_FAULT;				
				break;
			}
			case ( uint32_t )TIM17 : {
				if(( pwms[i].Channel & ~TIM_CANNEL_N ) != TIM_CHANNEL_1 ) return PWM_RES_FAULT;
				RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
				if( PWM_RES_OK != init_tim( &pwms[i] )) return PWM_RES_FAULT;				
				break;
			}				
		}		
	}
	return count;
}

/** Установка значения ШИМ для указанного выхода
*	\param[in]	index			- номер выхода в массиве описателей
*	\param[in]	value			-	значение ШИМ для выхода
*	\return								- статус опреации 0 - успешно, иначе - ошибка
*/
int pwm_out_set( uint8_t index, uint16_t value ) {
	if( NULL == pwms ) return PWM_RES_FAULT;
	if( index >= pwm_count ) return PWM_RES_FAULT;
	
	switch( pwms[index].Channel & ~TIM_CANNEL_N ) {
		case TIM_CHANNEL_1 : {
			pwms[index].Tim->CCR1 = value;
			break;
		}
		case TIM_CHANNEL_2 : {
			pwms[index].Tim->CCR2 = value;		
			break;
		}	
		case TIM_CHANNEL_3 : {
			pwms[index].Tim->CCR3 = value;		
			break;
		}
		case TIM_CHANNEL_4 : {
			pwms[index].Tim->CCR4 = value;		
			break;
		}		
	}
	return PWM_RES_OK;
}
