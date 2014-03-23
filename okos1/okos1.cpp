/*
 * okos1.cpp
 *
 * Created: 3/11/2014 10:57:23 PM
 *  Author: vicky
 */ 

# define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "configurations.h"
#include "lcd.h"
#include "Led.h"
#include "relay.h"
#include "adc.h"
#include "time.h"
#include "meter.h"
#include "accounth.h"
#include "button.h"
#include "ui.h"

Lcd lcd = Lcd();
Led led = Led();
Relay relay = Relay();
Meter meter = Meter();
Account account = Account();
Time system_time = Time();
Button toggle_button = Button();
UI ui = UI(&lcd, &led, &toggle_button);

// timer0 overflow interrupt
ISR(TIMER0_OVF_vect) {
	system_time.add_tick();
}

ISR(INT0_vect)
{
	toggle_button.press();
}

int main(void)
{
	//Enabling timer 0 interrupt for system clock
	TIMSK=(1<<TOIE0);
	// set timer0 counter initial value to 0
	TCNT0=0x00;
	// start timer0 with /1024 prescaler
	TCCR0 = (1<<CS02) | (1<<CS00);
	
	//Enabling interrupt for the button
	GICR|=0x40;
	MCUCR=0x03;
	MCUCSR=0x00;
	GIFR=0x40;
	//Enable interrupts
	sei();
	int32_t last_second, this_second;
	int8_t counter = 0;
	relay.turn_on();
	while(1)
	{
		
		meter.update();
		//toggle_button.read();
		account.update(&meter);
		this_second = system_time.get();
		if (account.get_state())
			relay.turn_off();
		else
			relay.turn_on();
		if (last_second != this_second)
		{
			last_second = this_second;
			lcd.print_number(meter.power_val());
			//ui.update(&account, &meter);
		}
		lcd.update();
	}
}