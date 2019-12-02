/*
lcdpcf8574 lib sample

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>


//#include "io.h"


#ifdef _SIMULATE_


#include "/Users/nehagupta/Downloads/simavr-master/simavr/sim/avr/avr_mcu_section.h"


#endif


#include "lcdpcf8574.h"


void set_PWM(double frequency){
   static double current_frequency;
   if(frequency != current_frequency){
       if(!frequency){ TCCR3B &= 0x08; }
       else{ TCCR3B |= 0x03; }
       if(frequency < .954){ OCR3A = 0xFFFF;}
       else if(frequency > 31250) { OCR3A = 0x0000; }
       else { OCR3A = (short)(8000000 / (128* frequency)) -1; }
       TCNT3 = 0;
       current_frequency = frequency;
   }
}



void PWM_on(){
   TCCR3A = (1 << COM3A0);
   TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
   set_PWM(0);
}



void PWM_off(){
   TCCR3A = 0x00;
   TCCR3B = 0x00;
}

int main(){
DDRB = 0xFF; PORTB = 0x00;
PWM_on();
	while(1){
		set_PWM(320.00);
	}
}
