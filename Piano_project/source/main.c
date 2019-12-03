
#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#ifdef _SIMULATE_
#include "/Users/nehagupta/Downloads/simavr-master/simavr/sim/avr/avr_mcu_section.h"
#endif

#include "lcdpcf8574.h"
#include "timer.h"

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

void ADC_Init()
{
	DDRA = 0x00;		/* Make ADC port as input */
	ADCSRA = 0x87;		/* Enable ADC, fr/128  */
	ADMUX = 0x40;		/* Vref: Avcc, ADC channel: 0 */
}

int ADC_Read(char channel)
{
	int ADC_value;
	
	ADMUX = (0x40) | (channel & 0x07);/* set input channel to read */
	ADCSRA |= (1<<ADSC);	/* start conversion */
	while((ADCSRA &(1<<ADIF))== 0);	/* monitor end of conversion interrupt flag */
	
	ADCSRA |= (1<<ADIF);	/* clear interrupt flag */
	ADC_value = (int)ADCL;	/* read lower byte */
	ADC_value = ADC_value + (int)ADCH*256;/* read higher 2 bits, Multiply with weightage */

	return ADC_value;		/* return digital value */
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

enum statesSong { init1, song1, song2} state1 ;
unsigned char cntr;
unsigned short y;
unsigned short x;
unsigned short press;

const double NOTE_C4 = 261.63;
const double NOTE_D4 = 293.66;
const double NOTE_F4 = 349.23;
const double NOTE_E4 = 329.63;
const double NOTE_G4 = 392.00;
const double NOTE_A4 = 440.00;
const double NOTE_B4 = 493.88;
const double NOTE_C5 = 523.00;

int happyMelody[] = {NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4};// NOTE_E4,
//NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
//NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_D4,
//(NOTE_A4 + NOTE_B4) / 2, (NOTE_A4 + NOTE_B4) / 2, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4} ;

int happyDurations[] = { 4,4,4,4,4};//,4,2,4,4,4,4,2,4,4,4,4,4,4,4,4,2,4,4,4,4,2 };

void menu(){
	x = ADC_Read(1);
	y = ADC_Read(0);
	press = ADC_Read(2);
	
	switch(state1){
		case init1:
			state1 = song1;
			break;	
		case song1:
			if(y > 800){
				state1 = song2;
			}
			else{
				state1 = song1;
			}
			break;
		case song2:
			if(y < 400){
				state1 = song1;
			}
			break;
	}
	switch(state1){
		case init1:
			break;
		case song1:
			lcd_gotoxy(0, 0);
		
			if(press < 600){
			
				for(int i = 0; i < 27; ++i){
					while(cntr < happyDurations[i]){
						set_PWM(happyMelody[i]);
						cntr++;
					}
					cntr = 0;
					while(!TimerFlag){}
					TimerFlag = 0;
				}
			}
			set_PWM(0);
		break;
		case song2:
			lcd_gotoxy(0, 1);
			break;
	}
}



				
			
			
			
			
			
			
// 	if(y < 400){
// 			HC595Write(0b10000000);
// 			lcd_gotoxy(6, 0);
			
// 		}
// 		else if(y > 800){
// 			HC595Write(0b01000000);
// 			lcd_gotoxy(3, 0);
// 		}
// 		else if(x < 500){
// 			HC595Write(0b00100000);
// 			lcd_gotoxy(10, 0);
// 		}
// 		else if(x > 600){ 
// 			HC595Write(0b00010000);
// 			lcd_gotoxy(3, 0);
// 		}
// 		else if(press < 600){ 
// // 			menu();
// 			HC595Write(0b11111111);
// 			lcd_gotoxy(10, 4);
			
// 		}
// 		else{
// 			HC595Write(0b00000000);
// 		}




int main(void){
 	DDRA = 0x00; PORTA = 0xFF;
	//DDRB = 0xFF; PORTB = 0x00;
	DDRB = 0xE0; PORTB = 0x1F;
	DDRD = 0xFF; PORTD = 0x00;
	PWM_on();
	HC595Init();
	state = init;
	state1 = init1;
	unsigned char ctr;
	
	TimerSet(10);
	TimerOn(); 
	
	lcd_init(LCD_DISP_ON_BLINK);  
	uint8_t led = 0;
	lcd_led(led);
	
	lcd_gotoxy(0, 0);
  	lcd_puts(" SONG1");
	
	lcd_gotoxy(0, 2);
	lcd_puts(" SONG2");
		

	ADC_Init();
	
	while(1) {    
		//HC595Write(0b00000000);
		sound();
		if(ctr >= 100){
			menu();
			ctr = 0; 
		}
		ctr++;
		
		while(!TimerFlag){}
		TimerFlag = 0;
	}
	
	return 1;
}











