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

/*static char received_data[40];
uint8_t data_char;
static uint8_t received_data_counter;
*/
/*ISR(USART_RXC_vect)
{
	check_data();
}*/

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
	//UCSRB = ((1<<TXEN)|(1<<RXEN) | (1<<RXCIE));
	UCSRB = ((1<<TXEN)|(1<<RXEN));
	
	//received_data_counter = 0;
}


void USART_send_byte(uint8_t u8Data)
{
	// Wait until last byte has been transmitted
	while((UCSRA &(1<<UDRE)) == 0);
	// Transmit data
	UDR = u8Data;
}


uint8_t USART_receive_byte()
{
	while((UCSRA &(1<<RXC)) == 0);
	return UDR;
}

uint8_t USART_any_byte(uint8_t *data)
{
	if ((UCSRA &(1<<RXC)) == 0)
		return 0;
	*data = UDR;
	return 1;
}

/*inline void check_data()
{
	if (USART_any_byte(&data_char))
	{
		received_data[received_data_counter] = data_char;
		received_data_counter += 1;
	}
}

uint8_t USART_any_data(char *data)
{
	if (received_data_counter == 0)
		return 0;
	received_data[received_data_counter] = '\0';
	received_data_counter = 0;
	data = received_data;
	return 1;
}
*/
void send_string(char* string)
{
	while (*string != '\0')
	{
		USART_send_byte(*string);
		string += 1;
	}
}

#endif /* UART_H_ */