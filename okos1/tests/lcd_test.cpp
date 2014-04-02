/*
 * okos1.cpp
 *
 * Created: 3/11/2014 10:57:23 PM
 *  Author: vicky
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "configurations.h"
#include "lcd.h"

LCD lcd = LCD();
//# define F_CPU 8000000UL
// timer0 overflow interrupt
ISR(TIMER0_OVF_vect) {
	lcd.update();
	_delay_ms(5);
}


int main(void)
{
	
	TIMSK=(1<<TOIE0);
	// set timer0 counter initial value to 0
	TCNT0=0x00;

	// start timer0 with /1024 prescaler
	TCCR0 = (1<<CS02) | (1<<CS00);
	
	
	//sei();
	int a = 1, counter = 0;
	lcd.print_number(a);
	while(1)
	{
		lcd.update();
		_delay_ms(2);
		counter += 1;
		if (counter > 100)
		{
			a += 1;
			lcd.print_number(a);
			counter = 0;
		}
	}
}