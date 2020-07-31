
/**	\file		pwm_lib.h
*		\author	Alex
*		\date		29.07.2020
*
* Библиотека для работы с ШИМ на STM32F0
*
* Предположим, что мы производим устройства с ШИМ-выходами, для управления светильниками и другим оборудованием. Эти устройства имеют несколько выходов. Они построены на микроконтроллере STM32F030K6.
* Все выходы ШИМ инженер-схемотехник всегда заводил на выходы TIMx_CHx. Однако на разных устройствах разное количество выходов, и подключены они в разном порядке и к разным ножкам микроконтроллера.
* устройства     перечень выходов (1-N)
* WB-PWM2      PA10 PA11
* WB-PWM4      PA6 PA10 PA8 PA7
* WB-PWM8      PB14 PB7 PA6 PA10 PA8 PA7 PB1 PB4
* Нам нужно сделать прошивки для каждого из этих трёх устройств. Прошивки должны использовать одну библиотеку для управления выходами. 
* Ваша задача - реализовать эту библиотеку.
* Выходы должны описываться в универсальном формате, чтобы библиотеку можно было использовать и с другими устройствами. 
* Частота и разрядность ШИМ должна быть одинаковая на всех выходах. 
* Управление выходами должно происходить через функцию вида: pwm_out_set(uint8_t channel_index, uint16_t value)
* Нужно сделать тестовую прошивку, которая будет демонстрировать использование библиотеки. Реализовать возможность сборки прошивки для каждого устройства. 
* Что должно получиться в результате:
* Библиотека для ШИМ в git репозитории с README с примером использования и инструкциями по сборке
* Демонстрационная прошивка для WB-PWM*, использующая библиотеку в git репозитории с тремя таргетами: под WB-PWM2, WB-PWM4, WB-PWM8
* с README с описанием сборки и добавления нового таргета
*/

#pragma once

#include <stdint.h>

#include <stm32f0xx.h>

#define PWM_RES_OK							0
#define PWM_RES_FAULT						-1

#define TIM_CANNEL_N						0x8000
#define TIM_CHANNEL_1N					( TIM_CHANNEL_1 | TIM_CANNEL_N )
#define TIM_CHANNEL_2N					( TIM_CHANNEL_2 | TIM_CANNEL_N )
#define TIM_CHANNEL_3N					( TIM_CHANNEL_3 | TIM_CANNEL_N )
#define TIM_CHANNEL_4N					( TIM_CHANNEL_4 | TIM_CANNEL_N )

typedef struct {
	uint16_t			Pin;				// Пин порта
	uint16_t			Alter;			// Функция пина
	GPIO_TypeDef* Port;				// Порт
	uint16_t 			Channel;		// Канал таймера
	TIM_TypeDef*	Tim;				// Таймер
} pwm_out_t;

// Описание стандартных пинов
#define PA6				{ .Pin = GPIO_PIN_6, 	.Alter = GPIO_AF5_TIM16, 	.Port = GPIOA, .Channel = TIM_CHANNEL_1, 	.Tim	= TIM16 }
#define PA7				{ .Pin = GPIO_PIN_7, 	.Alter = GPIO_AF1_TIM3, 	.Port = GPIOA, .Channel = TIM_CHANNEL_2, 	.Tim	= TIM3 }
#define PA8				{ .Pin = GPIO_PIN_8, 	.Alter = GPIO_AF2_TIM1, 	.Port = GPIOA, .Channel = TIM_CHANNEL_1, 	.Tim	= TIM1 }
#define PA9				{ .Pin = GPIO_PIN_9, 	.Alter = GPIO_AF2_TIM1, 	.Port = GPIOA, .Channel = TIM_CHANNEL_2, 	.Tim	= TIM1 }
#define PA10			{ .Pin = GPIO_PIN_10, .Alter = GPIO_AF2_TIM1, 	.Port = GPIOA, .Channel = TIM_CHANNEL_3, 	.Tim	= TIM1 }
#define PA11			{ .Pin = GPIO_PIN_11, .Alter = GPIO_AF2_TIM1, 	.Port = GPIOA, .Channel = TIM_CHANNEL_4, 	.Tim	= TIM1 }

#define PB1				{ .Pin = GPIO_PIN_1, 	.Alter = GPIO_AF0_TIM14, 	.Port = GPIOB, .Channel = TIM_CHANNEL_1, 	.Tim	= TIM14 }
#define PB4				{ .Pin = GPIO_PIN_4, 	.Alter = GPIO_AF1_TIM3, 	.Port = GPIOB, .Channel = TIM_CHANNEL_1, 	.Tim	= TIM3 }
#define PB7				{ .Pin = GPIO_PIN_7, 	.Alter = GPIO_AF2_TIM1, 	.Port = GPIOB, .Channel = TIM_CHANNEL_1N, .Tim	= TIM17 }
#define PB14			{ .Pin = GPIO_PIN_14, .Alter = GPIO_AF1_TIM15, 	.Port = GPIOB, .Channel = TIM_CHANNEL_1, 	.Tim	= TIM15 }

// Описание стандартных устройств
#define wb_pwm2		{ PA10, PA11 }
#define wb_pwm4		{ PA6, PA10, PA8, PA7 }
#define wb_pwm8		{ PB14, PB7, PA6, PA10, PA8, PA7, PB1, PB4 }

/** Инициализация используемых выходов ШИМ
*	\param[in]	pwm				-	указатель на массив описателей выходов
*	\param[in]	count			-	количество используемых выходов
*	\return								- статус опреации 0 - успешно, иначе - ошибка
*/
int init_pwm_out( pwm_out_t* pwm, uint8_t count );

/** Установка предделителя для используемых тайиеров
*	\param[in]	value			- значение предделителя для всех таймеров
*/
void set_pwm_prescaler( uint16_t value );

/** Чтение предделителя для используемых тайиеров
*	\return	- значение предделителя таймеров
*/
uint16_t get_pwm_prescaler( void );

/** Установка максимального значения счётчика для используемых тайиеров
*	\param[in]	value			- верхнее значение счётчика для всех таймеров
*/
void set_pwm_max( uint16_t value );

/** Чтение максимального значения счётчика для используемых тайиеров
*	\return	- значение счётчика для всех таймеров
*/
uint16_t get_pwm_max( void );

/** Установка значения ШИМ для указанного выхода
*	\param[in]	index			- номер выхода в массиве описателей
*	\param[in]	value			-	значение ШИМ для выхода
*	\return								- статус опреации 0 - успешно, иначе - ошибка
*/
int pwm_out_set(uint8_t cindex, uint16_t value);
