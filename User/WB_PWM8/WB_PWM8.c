
/**	\file 	WB_PWM8.c
*		\author	Alex
*		\date		29.07.2020
*/

#include "WB_PWM8.h"

#include "pwm_lib.h"

static pwm_out_t pwms[] = wb_pwm8;

void init_pwm( void ) {
	init_pwm_out( pwms, sizeof( pwms ) / sizeof( pwms[0] ));
	set_pwm_prescaler( 500 );
	set_pwm_max( 500 );

	pwm_out_set( 0, 20 );
	pwm_out_set( 1, 40 );
	pwm_out_set( 2, 60 );
	pwm_out_set( 3, 80 );
	pwm_out_set( 4, 100 );
	pwm_out_set( 5, 120 );
	pwm_out_set( 6, 140 );
	pwm_out_set( 7, 160 );
}


