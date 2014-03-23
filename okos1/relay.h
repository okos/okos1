/*
 * relay.h
 *
 * Created: 3/16/2014 2:06:11 PM
 *  Author: vicky
 */ 


#ifndef RELAY_H_
#define RELAY_H_

class Relay
{
	
	public:
	uint8_t state;
	Relay()
	{
		state = 0;
		RELAY_PORT_DIRECTION |= (1<<RELAY_PIN);	
		RELAY_CONTROL &= ~(1<<RELAY_PIN);
	}
	
	void turn_off()
	{
		RELAY_CONTROL &= ~(1<<RELAY_PIN);
		state = 0;
	}
	
	void turn_on()
	{
		RELAY_CONTROL |= (1<<RELAY_PIN);
		state = 1;
	}
};

#endif /* RELAY_H_ */