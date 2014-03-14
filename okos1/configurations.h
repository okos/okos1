/*
 * configurations.h
 *
 * Created: 3/13/2014 9:33:40 PM
 *  Author: vicky
 */ 


#ifndef CONFIGURATIONS_H_
#define CONFIGURATIONS_H_

//LCD port configurations
#define LCD_DATA_DIRECTION DDRB
#define LCD_DATA_PORT PORTB
#define LCD_CONTROL_PORT_HIGH_
#define LCD_CONTROL_DIRECTION DDRD
#define LCD_CONTROL_PORT PORTD


//ACD configurations
#define ADC_PRESCALER 8 //Currently not using it.
#define VOLTAGE_CHANNEL 0
#define CURRENT_CHANNEL 1



#endif /* CONFIGURATIONS_H_ */