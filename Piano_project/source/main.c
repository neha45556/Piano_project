/*	Author: ngupt009
 *  Partner(s) Name: none
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include "io.h"
#ifdef _SIMULATE_
#include "/Users/nehagupta/Downloads/simavr-master/simavr/sim/avr/avr_mcu_section.h"
#endif

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))
          
/*-------------------------------------------------------------------------*/

#define DATA_BUS PORTC		// port connected to pins 7-14 of LCD display
#define CONTROL_BUS PORTD	// port connected to pins 4 and 6 of LCD disp.
#define RS 6			// pin number of uC connected to pin 4 of LCD disp.
#define E 7			// pin number of uC connected to pin 6 of LCD disp.

/*-------------------------------------------------------------------------*/

void LCD_ClearScreen(void) {
   LCD_WriteCommand(0x01);
}

void LCD_init(void) {

    //wait for 100 ms.
	delay_ms(100);
	LCD_WriteCommand(0x38);
	LCD_WriteCommand(0x06);
	LCD_WriteCommand(0x0f);
	LCD_WriteCommand(0x01);
	delay_ms(10);						 
}

void LCD_WriteCommand (unsigned char Command) {
   CLR_BIT(CONTROL_BUS,RS);
   DATA_BUS = Command;
   SET_BIT(CONTROL_BUS,E);
   asm("nop");
   CLR_BIT(CONTROL_BUS,E);
   delay_ms(2); // ClearScreen requires 1.52ms to execute
}

void LCD_WriteData(unsigned char Data) {
   SET_BIT(CONTROL_BUS,RS);
   DATA_BUS = Data;
   SET_BIT(CONTROL_BUS,E);
   asm("nop");
   CLR_BIT(CONTROL_BUS,E);
   delay_ms(1);
}

void LCD_DisplayString( unsigned char column, const unsigned char* string) {
   LCD_ClearScreen();
   unsigned char c = column;
   while(*string) {
      LCD_Cursor(c++);
      LCD_WriteData(*string++);
   }
}

void LCD_Cursor(unsigned char column) {
   if ( column < 17 ) { // 16x1 LCD: column < 9
						// 16x2 LCD: column < 17
      LCD_WriteCommand(0x80 + column - 1);
   } else {
      LCD_WriteCommand(0xB8 + column - 9);	// 16x1 LCD: column - 1
											// 16x2 LCD: column - 9
   }
}

void delay_ms(int miliSec) //for 8 Mhz crystal

{
    int i,j;
    for(i=0;i<miliSec;i++)
    for(j=0;j<775;j++)
  {
   asm("nop");
  }
}

void createChar(unsigned char loc, unsigned char* p){
	unsigned char i;
	if(loc < 8 ){
		LCD_WriteCommand(0x40+(loc*8));
		for(i = 0; i < 8; i++){
			LCD_WriteData(p[i]);
		}
	}
	//LCD_WriteCommand(0x80);
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

void InitADC(void)
{
    ADMUX|=(1<<REFS0);    
    ADCSRA|=(1<<ADEN)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2); //ENABLE ADC, PRESCALER 128
}
uint16_t readadc(uint8_t ch)
{
    ch&=0b00000111;         //ANDing to limit input to 7
    ADMUX = (ADMUX & 0xf8)|ch;  //Clear last 3 bits of ADMUX, OR with ch
    ADCSRA|=(1<<ADSC);        //START CONVERSION
    while((ADCSRA)&(1<<ADSC));    //WAIT UNTIL CONVERSION IS COMPLETE
    return(ADC);        //RETURN ADC VALUE
}




#define buttons (~PINB & 0xFF) //7F

enum states {init, C, D, E1, F, G, A, B} state;
void sound(){
	switch(state){
		case init:
			if(buttons == 0x01){
				state = C;
			}
			else if(buttons == 0x02){
				state = D;
			}
			else if(buttons == 0x04){
				state = E1;
			}
			else if(buttons == 0x08){
				state = F;
			}
			else if(buttons == 0x10){
				state = G;
			}
			else if(buttons == 0x20){
				state = A;
			}
			else if(buttons == 0x40){
				state = B;
			}
			else{
				state = init; 
			}	
		break;
		case C:
			if(buttons == 0x01){
				state = C;
			}
			else if(buttons == 0x02){
				state = D;
			}
			else if(buttons == 0x04){
				state = E1;
			}
			else if(buttons == 0x08){
				state = F;
			}
			else if(buttons == 0x10){
				state = G;
			}
			else if(buttons == 0x20){
				state = A;
			}
			else if(buttons == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case D:
			if(buttons == 0x01){
				state = C;
			}
			else if(buttons == 0x02){
				state = D;
			}
			else if(buttons == 0x04){
				state = E1;
			}
			else if(buttons == 0x08){
				state = F;
			}
			else if(buttons == 0x10){
				state = G;
			}
			else if(buttons == 0x20){
				state = A;
			}
			else if(buttons == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case E1:
			if(buttons == 0x01){
				state = C;
			}
			else if(buttons == 0x02){
				state = D;
			}
			else if(buttons == 0x04){
				state = E1;
			}
			else if(buttons == 0x08){
				state = F;
			}
			else if(buttons == 0x10){
				state = G;
			}
			else if(buttons == 0x20){
				state = A;
			}
			else if(buttons == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case F:
			if(buttons == 0x01){
				state = C;
			}
			else if(buttons == 0x02){
				state = D;
			}
			else if(buttons == 0x04){
				state = E1;
			}
			else if(buttons == 0x08){
				state = F;
			}
			else if(buttons == 0x10){
				state = G;
			}
			else if(buttons == 0x20){
				state = A;
			}
			else if(buttons == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case G:
			if(buttons == 0x01){
				state = C;
			}
			else if(buttons == 0x02){
				state = D;
			}
			else if(buttons == 0x04){
				state = E1;
			}
			else if(buttons == 0x08){
				state = F;
			}
			else if(buttons == 0x10){
				state = G;
			}
			else if(buttons == 0x20){
				state = A;
			}
			else if(buttons == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case A:
			if(buttons == 0x01){
				state = C;
			}
			else if(buttons == 0x02){
				state = D;
			}
			else if(buttons == 0x04){
				state = E1;
			}
			else if(buttons == 0x08){
				state = F;
			}
			else if(buttons == 0x10){
				state = G;
			}
			else if(buttons == 0x20){
				state = A;
			}
			else if(buttons == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case B:
			if(buttons == 0x01){
				state = C;
			}
			else if(buttons == 0x02){
				state = D;
			}
			else if(buttons == 0x04){
				state = E1;
			}
			else if(buttons == 0x08){
				state = F;
			}
			else if(buttons == 0x10){
				state = G;
			}
			else if(buttons == 0x20){
				state = A;
			}
			else if(buttons == 0x40){
				state = B;
			}
			else{
				state = init;
			}
			
		break;
		default:
			state = init;
			break;
	}
	switch(state){
		case init:
			set_PWM(0);
			HC595Write(0b00000000);
			break;
		case C:
			set_PWM(261.63);
			HC595Write(0b00000010);
			break;
		case D:
			set_PWM(293.66);
			HC595Write(0b00000100);
			break;
		case E1:
			set_PWM(329.63);
			HC595Write(0b00001000);
			break;
		case F:
			set_PWM(349.23);
			HC595Write(0b00010000);
			break;
		case G:
			set_PWM(392.00);
			HC595Write(0b00100000);
			break;
		case A:
			set_PWM(440.00);
			HC595Write(0b01000000);
			break;
		case B:
			set_PWM(493.88);
			HC595Write(0b10000000);
			break;
		default:
			break;
	}
				
			
}

void menu(){
	//LCD_DisplayString(5, " Free Play ");
	LCD_DisplayString(3, "FreePlay        Songs ");
	uint16_t x,y,z;
	x = readadc(0);
	y = readadc(1);
	
}

int main(void){
  	DDRA = 0xF8; PORTA = 0x00;
 	DDRB = 0x00; PORTB = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	PWM_on();
	HC595Init();
	short x,y,z;

	
	state = init;
	InitADC();
	//uint8_t led_pattern ;
 	
	
	LCD_init();
	menu();
	
	
	while(1) {
		x = readadc(0);
		y = readadc(1);
		HC595Write(0b00000000);
		sound();
		if(y < 500){
			LCD_DisplayString(1, "up");	
		}
		else if(y > 580){
			LCD_DisplayString(1, "down");
		}
		else{
			LCD_DisplayString(1, "middle");
		}	
	}
	return 1;
}

