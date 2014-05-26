/*
 * uart.h
 *
 * Created: 3/25/2014 9:59:02 PM
 *  Author: vicky
 */ 


#ifndef UART_H_
#define UART_H_

//#define F_CPU 8000000UL  // 8 MHz

/*
Very Important - change F_CPU to match target clock
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Define baud rate
//#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define RECEIVE_BUFFER_SIZE 200
static char received_data[RECEIVE_BUFFER_SIZE];
uint8_t data_char;
static uint8_t received_data_counter, read_data_counter;
void check_data();

ISR(USART_RXC_vect)
{
	check_data();
}

void USART_init(void)
{
	// Set baud rate
	UBRRL = BAUD_PRESCALE;// Load lower 8-bits into the low byte of the UBRR register
	UBRRH = (BAUD_PRESCALE >> 8); 
	/*
	Load upper 8-bits into the high byte of the UBRR register
    Default frame format is 8 data bits, no parity, 1 stop bit
	to change use UCSRC, see AVR data sheet
	*/ 

	//Enable receiver and transmitter and receive complete interrupt 
	UCSRB = ((1<<TXEN)|(1<<RXEN) | (1<<RXCIE));
	//UCSRB = ((1<<TXEN)|(1<<RXEN));
	
	received_data_counter = 0;
	read_data_counter = 0;
}


void USART_send_byte(uint8_t u8Data)
{
	// Wait until last byte has been transmitted
	while((UCSRA &(1<<UDRE)) == 0);
	// Transmit data
	UDR = u8Data;
}

/*void USART_send_data(char* data, uint8_t size)
{
	uint8_t i;
	for(i=0; i<size; i++)
		// Wait until last byte has been transmitted
		while((UCSRA &(1<<UDRE)) == 0);
		// Transmit data
		UDR = data[i];
}*/

uint8_t USART_receive_byte()
{
	while((UCSRA &(1<<RXC)) == 0); //Wait if its receiving data yet
	return UDR;
}

uint8_t USART_any_byte(uint8_t *data)
{
	if ((UCSRA &(1<<RXC)) == 0)//If its receiving bits
		return 0;
	*data = UDR;
	return 1;
}

inline void check_data()
{
	if (USART_any_byte(&data_char))
	{
		if(received_data_counter < RECEIVE_BUFFER_SIZE)
		{
			received_data[received_data_counter] = data_char;
			received_data_counter += 1;
		}
		else
		{
			received_data[0] = data_char;
			received_data_counter = 1;
		}
	}
}

uint8_t USART_any_data(uint8_t *data)
{
	if (received_data_counter == read_data_counter)
		return 0;
	if(read_data_counter == RECEIVE_BUFFER_SIZE)
		read_data_counter = 0;
	*data = (uint8_t)received_data[read_data_counter];
	read_data_counter += 1;
	return 1;
}

void send_string(char* string)
{
	while (*string != '\0')
	{
		USART_send_byte(*string);
		string += 1;
	}
}

#endif /* UART_H_ */
