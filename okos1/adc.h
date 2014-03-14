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
	public:
	Adc()	// Initialization of ADC
	{
		ADMUX=(1<<REFS0)|(1<<REFS1);	// AVcc with internal 2.56 volt refrence
		ADCSRA=(1<<ADEN)|(0<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
		// Enable ADC and set Prescaler division factor as 8
	}

	unsigned int read(uint8_t ch)
	{
		ch= ch & 0b00000111;	// channel must be b/w 0 to 7
		ADMUX |= ch;	// selecting channel
		
		ADCSRA|=(1<<ADSC);	// start conversion
		while(!(ADCSRA & (1<<ADIF)));	// waiting for ADIF, conversion complete
		ADCSRA|=(1<<ADIF);	// clearing of ADIF, it is done by writing 1 to it
		
		return (ADC);
	}
};

#endif /* ADC_H_ */