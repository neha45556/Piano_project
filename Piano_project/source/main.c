/*	Author: ngupt009
 *  Partner(s) Name: none
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "io.h"
//#ifdef _SIMULATE_
//#include "/Users/nehagupta/Downloads/simavr-master/simavr/sim/avr/avr_mcu_section.h"
//#endif




int main(void){
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	const char *hello = "Hello World";
	LCD_init();
	LCD_DisplayString(1, hello);
	
	while(1){continue;}
	
}
	
	

