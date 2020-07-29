
/**	\file		pwm_lib.c
*		\author	Alex
*		\date		29.07.2020
*
* ���������� ��� ������ � ��� �� STM32F0
*
* �����������, ��� �� ���������� ���������� � ���-��������, ��� ���������� ������������� � ������ �������������. ��� ���������� ����� ��������� �������. ��� ��������� �� ���������������� STM32F030K6.
* ��� ������ ��� �������-����������� ������ ������� �� ������ TIMx_CHx. ������ �� ������ ����������� ������ ���������� �������, � ���������� ��� � ������ ������� � � ������ ������ ����������������.
* ����������     �������� ������� (1-N)
* WB-PWM2      PA10 PA11
* WB-PWM4      PA6 PA10 PA8 PA7
* WB-PWM8      PB14 PB7 PA6 PA10 PA8 PA7 PB1 PB4
* ��� ����� ������� �������� ��� ������� �� ���� ��� ���������. �������� ������ ������������ ���� ���������� ��� ���������� ��������. ���� ������ - ����������� ��� ����������.
* ������ ������ ����������� � ������������� �������, ����� ���������� ����� ���� ������������ � � ������� ������������. 
* ������� � ����������� ��� ������ ���� ���������� �� ���� �������. 
* ���������� �������� ������ ����������� ����� ������� ����: pwm_out_set(uint8_t channel_index, uint16_t value)
* ����� ������� �������� ��������, ������� ����� ��������������� ������������� ����������. ����������� ����������� ������ �������� ��� ������� ����������. 
* ��� ������ ���������� � ����������:
* ���������� ��� ��� � git ����������� � README � �������� ������������� � ������������ �� ������
* ���������������� �������� ��� WB-PWM*, ������������ ���������� � git ����������� � ����� ���������: ��� WB-PWM2, WB-PWM4, WB-PWM8
* � README � ��������� ������ � ���������� ������ �������
*/

#include "pwm_lib.h"


void init_pwm_out( void ) {
}

/** ���������� ������� pwm
*	\param[in]		index		- ����� ������ � �������
*	\param[in]		value		- �������� ��� �� ������
*/
pwm_out_set(uint8_t channel_index, uint16_t value) {
}