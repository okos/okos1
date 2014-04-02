/*
 * Led.h
 *
 * Created: 3/21/2014 1:26:59 PM
 *  Author: vicky
 */ 


#ifndef LED_H_
#define LED_H_

class Led
{
	public:
	Led();
	void turn_on(uint8_t led_pin);
	void turn_off(uint8_t led_pin);
};

Led::Led()
{
	LED_DATA_DIRECTION |= (1<<LED_PIN1 | 1<<LED_PIN2);
	LED_CONTROL_PORT &= ~(1<<LED_PIN1 | 1<<LED_PIN2);
}

void Led::turn_off(uint8_t led_pin)
{
	LED_CONTROL_PORT &= ~(1<<led_pin);
}

void Led::turn_on(uint8_t led_pin)
{
	LED_CONTROL_PORT |= (1<<led_pin);
}
#endif /* LED_H_ */