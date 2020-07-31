
/**	\file 	WB_PWM4.c
*		\author	Alex
*		\date		29.07.2020
*/

#include "WB_PWM4.h"

#include "pwm_lib.h"

static pwm_out_t pwms[] = wb_pwm4;

void init_pwm( void ) {
	init_pwm_out( pwms, sizeof( pwms ) / sizeof( pwms[0] ));
	set_pwm_prescaler( 500 );
	set_pwm_max( 500 );

	pwm_out_set( 0, 20 );
	pwm_out_set( 1, 40 );
	pwm_out_set( 2, 60 );
	pwm_out_set( 3, 80 );
}
