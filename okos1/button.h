/*
 * button.h
 *
 * Created: 3/17/2014 12:01:32 PM
 *  Author: vicky
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_

/* Class to sense button press activity. It is interrupt based.
*/

class Button
{
	volatile uint8_t state;
	volatile uint8_t is_read;
	uint8_t _counter;
	public:	
	Button();
	inline void press()
	{
		if (BUTTON_CONTROL_PORT & (1<<BUTTON_PIN))
		{
			state = 1;
			is_read = 0;
		}
		else
			state = 0;
	}
	void read();
	uint8_t get_and_clear();
	uint8_t get_and_clear_state();
	inline uint8_t get_state()
	{
		return state;
	}
};
Button::Button()
{
	BUTTON_PORT_DIRECTION &= ~(1<<BUTTON_PIN);
	state = 0;
	is_read = 1;
	_counter = 0;
}
void Button::read()
{
	if (BUTTON_CONTROL_PORT & (1<<BUTTON_PIN))
		_counter += 1;
	else
		_counter = 0;
	if (_counter > 5)
	{
		state = 1;
		is_read = 0;
		_counter = 0;
	}
	else
		state = 0;
}
uint8_t Button::get_and_clear()
{
	if (is_read==0)
	{
		is_read = 1;
		return 1;
	}
	else
	return 0;
}
uint8_t Button::get_and_clear_state()
{
	if (state)
	{
		state = 0;
		return 1;
	}
	else
	return 0;
}


#endif /* BUTTON_H_ */