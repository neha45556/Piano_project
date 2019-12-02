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
#define HC595_PORT   PORTD
#define HC595_DDR    DDRD
#define HC595_DS_POS PD0      //Data pin (DS) pin location
#define HC595_SH_CP_POS PD1      //Shift Clock (SH_CP) pin location
#define HC595_ST_CP_POS PD2 	//Store Clock (ST_CP) pin location

void HC595Init()
{
  //Make the Data(DS), Shift clock (SH_CP), Store Clock (ST_CP) lines output
  HC595_DDR|=((1<<HC595_SH_CP_POS)|(1<<HC595_ST_CP_POS)|(1<<HC595_DS_POS));
}
#define HC595DataHigh() (HC595_PORT|=(1<<HC595_DS_POS))
#define HC595DataLow() (HC595_PORT&=(~(1<<HC595_DS_POS)))
void HC595Pulse()
{
  //Pulse the Shift Clock
  HC595_PORT|=(1<<HC595_SH_CP_POS);//HIGH
  HC595_PORT&=(~(1<<HC595_SH_CP_POS));//LOW
}
void HC595Latch()
{
  //Pulse the Store Clock
  HC595_PORT|=(1<<HC595_ST_CP_POS);//HIGH
  _delay_loop_1(1);
  HC595_PORT&=(~(1<<HC595_ST_CP_POS));//LOW
  _delay_loop_1(1);
}
void HC595Write(uint8_t data)
{
  for(uint8_t i=0;i<8;i++)
  {
     if(data & 0b10000000)
     {
        HC595DataHigh();
     }
     else
     {
        HC595DataLow();
     }

     HC595Pulse(); 
     data=data<<1; 
  }
  HC595Latch();
}
void Wait()
{
  for(uint8_t i=0;i<30;i++)
  {
     _delay_loop_2(0);
  }
}


int main(){
DDRB = 0xFF; PORTB = 0x00;
PWM_on();
	while(1){
		set_PWM(320.00);
	}
}
