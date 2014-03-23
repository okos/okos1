/*
 * adc.h
 *
 * Created: 3/15/2014 1:38:34 AM
 *  Author: vicky
 */ 


#ifndef ADC_H_
#define ADC_H_

class Adc
{
	uint8_t _ADMUX;
	public:
	Adc();
	unsigned int read(uint8_t ch);
};

Adc::Adc()	// Initialization of ADC
{	
	_ADMUX = (1<<REFS0)|(1<<REFS1);// AVcc with internal 2.56 volt refrence
	ADMUX = _ADMUX;
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0);
	// Enable ADC and set Prescaler division factor as 64
	//Currently we have a external clock of 8mhz.
	//For the ADC to work correctly the ADC clock should be 100-200khz.
	//So this prescaler will bring the ADC clock to 125khz.
}

unsigned int Adc::read(uint8_t ch)
{
	ch= ch & 0b00000111;	// channel must be b/w 0 to 7
	ADMUX = _ADMUX | ch;
	//ADMUX |= ch;	// selecting channel
	ADCSRA|=(1<<ADSC);	// start conversion
	while(!(ADCSRA & (1<<ADIF)));	// waiting for ADIF, conversion complete
	ADCSRA|=(1<<ADIF);	// clearing of ADIF, it is done by writing 1 to it
	return (ADC);
}
#endif /* ADC_H_ */