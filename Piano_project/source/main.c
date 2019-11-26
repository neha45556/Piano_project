/*
lcdpcf8574 lib sample

copyright (c) Davide Gironi, 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lcdpcf8574/lcdpcf8574.h"
//#include "uart/uart.h"


#define PIN(x) (*(&x - 2))    /* address of input register of port x */


int main(void)
{
	//init uart

    //sei();


    //init lcd
    lcd_init(LCD_DISP_ON_BLINK);

    //lcd go home
    lcd_home();

    uint8_t led = 0;
    lcd_led(led); //set led

	lcd_puts("HI NEHA I WORK");
    while(1) {
    }
}
