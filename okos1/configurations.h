/*
 * configurations.h
 *
 * Created: 3/13/2014 9:33:40 PM
 *  Author: vicky
 */ 


#ifndef CONFIGURATIONS_H_
#define CONFIGURATIONS_H_

/*Important* Comment it for actual board*/
//#define DEV_BOARD
#define ACCOUNT_TYPE_PREPAID
//LCD port configurations
#define LCD_DATA_DIRECTION DDRB
#define LCD_DATA_PORT PORTB
#define LCD_CONTROL_PORT_HIGH_
#define LCD_CONTROL_DIRECTION DDRD
#define LCD_CONTROL_PORT PORTD
#define LCD_REFRESH_TIME 3 //LCD refresh time in ticks. Currently not using it

//LED port configurations
#define LED_DATA_DIRECTION DDRC
#define LED_CONTROL_PORT PORTC
#define LED_PIN1 6
#define LED_PIN2 7

//ACD configurations
#define VOLTAGE_CHANNEL 0
#define CURRENT_CHANNEL 1

//Relay Configurations
#define RELAY_PORT_DIRECTION DDRD
#define RELAY_CONTROL PORTD
#define RELAY_PIN 3

//Timer configuration
<<<<<<< HEAD
#define TICKS_PER_SECOND 60 //30 for 8mhz crystal
=======
#define TICKS_PER_SECOND 60//30
>>>>>>> dev

//Button configurations
#define BUTTON_PORT_DIRECTION DDRD
#define BUTTON_CONTROL_PORT PIND
#define BUTTON_PIN 2

//Meter configurations
#define SAMPLES_PER_RMS 25
#define SAMPLES_PER_AVG 25
#define OVERLOAD_RESET_TIME 30

//EEPROM configurations
#define EEPROM_START_ADDRESS_ACCOUNT 0x0000
#define EEPROM_START_ADDRESS_CALIBRATION EEPROM_START_ADDRESS_ACCOUNT + 300
#define EEPROM_START_ADDRESS_TIMER EEPROM_START_ADDRESS_CALIBRATION + 100

//USART configurations
<<<<<<< HEAD
#define F_CPU 16000000UL
#define USART_BAUDRATE 9600
=======
#define F_CPU 16000000UL/*8000000UL*/
#define USART_BAUDRATE 38400/*9600*/
>>>>>>> dev
#endif /* CONFIGURATIONS_H_ */
