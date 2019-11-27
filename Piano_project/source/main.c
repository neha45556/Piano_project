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



#define A3 (~PINA & 0x08)
#define A4 (~PINA & 0x10)
#define A5 (~PINA & 0x20)
#define A6 (~PINA & 0x40)

#define B0 (~PINB & 0x01)
#define B1 (~PINB & 0x02)
#define B2 (~PINB & 0x04)

enum states {init, C, D, E1, F, G, A, B} state;

void sound(){
	switch(state){
		case init:
			if(B0 == 0x01){
				state = C;
			}
			else if(B1 == 0x02){
				state = D;
			}
			else if(B2 == 0x04){
				state = E1;
			}
			else if(A3 == 0x08){
				state = F;
			}
			else if(A4 == 0x10){
				state = G;
			}
			else if(A5 == 0x20){
				state = A;
			}
			else if(A6 == 0x40){
				state = B;
			}
			else{
				state = init; 
			}	
		break;
		case C:
			if(B0 == 0x01){
				state = C;
			}
			else if(B1 == 0x02){
				state = D;
			}
			else if(B2 == 0x04){
				state = E1;
			}
			else if(A3 == 0x08){
				state = F;
			}
			else if(A4 == 0x10){
				state = G;
			}
			else if(A5 == 0x20){
				state = A;
			}
			else if(A6 == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case D:
			if(B0 == 0x01){
				state = C;
			}
			else if(B1 == 0x02){
				state = D;
			}
			else if(B2 == 0x04){
				state = E1;
			}
			else if(A3 == 0x08){
				state = F;
			}
			else if(A4 == 0x10){
				state = G;
			}
			else if(A5 == 0x20){
				state = A;
			}
			else if(A6 == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case E1:
			if(B0 == 0x01){
				state = C;
			}
			else if(B1 == 0x02){
				state = D;
			}
			else if(B2 == 0x04){
				state = E1;
			}
			else if(A3 == 0x08){
				state = F;
			}
			else if(A4 == 0x10){
				state = G;
			}
			else if(A5 == 0x20){
				state = A;
			}
			else if(A6 == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case F:
			if(B0 == 0x01){
				state = C;
			}
			else if(B1 == 0x02){
				state = D;
			}
			else if(B2 == 0x04){
				state = E1;
			}
			else if(A3 == 0x08){
				state = F;
			}
			else if(A4 == 0x10){
				state = G;
			}
			else if(A5 == 0x20){
				state = A;
			}
			else if(A6 == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case G:
			if(B0 == 0x01){
				state = C;
			}
			else if(B1 == 0x02){
				state = D;
			}
			else if(B2 == 0x04){
				state = E1;
			}
			else if(A3 == 0x08){
				state = F;
			}
			else if(A4 == 0x10){
				state = G;
			}
			else if(A5 == 0x20){
				state = A;
			}
			else if(A6 == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case A:
			if(B0 == 0x01){
				state = C;
			}
			else if(B1 == 0x02){
				state = D;
			}
			else if(B2 == 0x04){
				state = E1;
			}
			else if(A3 == 0x08){
				state = F;
			}
			else if(A4 == 0x10){
				state = G;
			}
			else if(A5 == 0x20){
				state = A;
			}
			else if(A6 == 0x40){
				state = B;
			}
			else{
				state = init;
			}
		break;
		case B:
			if(B0 == 0x01){
				state = C;
			}
			else if(B1 == 0x02){
				state = D;
			}
			else if(B2 == 0x04){
				state = E1;
			}
			else if(A3 == 0x08){
				state = F;
			}
			else if(A4 == 0x10){
				state = G;
			}
			else if(A5 == 0x20){
				state = A;
			}
			else if(A6 == 0x40){
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

// int main(){
// 	DDRA = 0x00; PORTA = 0xFF;
// 	DDRB = 0xFF; PORTB = 0x00;
// 	DDRD = 0xFF; PORTD = 0x00;
// 	DDRC = 0xFF; PORTC = 0x00;

// 	PWM_on();
// 	HC595Init();
// 	lcd_init(LCD_DISP_ON_BLINK);
         
// 	state = init;
	
// 	while(1){
// 		HC595Write(0b00000000);
// 		sound();
// 	}
// 	return 1;
// }

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
int main(void)
{
DDRA = 0x00; PORTA = 0xFF;
DDRB = 0xE0; PORTB = 0x1F;
DDRD = 0xFF; PORTD = 0x00;

    char a[20], b[20], c[20];   
    uint16_t x,y,z;
    InitADC();         //INITIALIZE ADC
    lcd_init(LCD_DISP_ON_BLINK);   
    uint8_t led = 0;
    lcd_led(led); //set led
	PWM_on();
    
    while(1)
    {
	sound();
        lcd_home();         
        x=readadc(0);      //READ ADC VALUE FROM PA.0
        y=readadc(1);      //READ ADC VALUE FROM PA.1
	itoa(x,a,10);    
        itoa(y,b,10);
        lcd_puts("x=");     //DISPLAY THE RESULTS ON LCD
        lcd_gotoxy(2,0);
        lcd_puts(a);
        lcd_gotoxy(7,0);
        lcd_puts("y=");
        lcd_gotoxy(9,0);
        lcd_puts(b);
	
	if(x > 500){
		lcd_puts("HI NEHA I WORK");
	}
	else if(x > 1000){
		lcd_puts("beee");
	}
		
		
    }
}






// int main(void)
// {
// 	//init uart

//     //sei();

//     //init lcd
//     lcd_init(LCD_DISP_ON_BLINK);

//     //lcd go home
//     lcd_home();

//     uint8_t led = 0;
//     lcd_led(led); //set led

// 	lcd_puts("HI NEHA I WORK");
//     while(1) {
//     }
// }
