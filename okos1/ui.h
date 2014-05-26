/*
 * ui.h
 *
 * Created: 3/21/2014 4:07:22 PM
 *  Author: vicky
 */ 


#ifndef UI_H_
#define UI_H_

#define MAX_STATE_ACTIVE_COUNT 20
enum UI_states
{
	NORMAL = 0,
	ERROR = 1,
	TAMPERED = 2,
	INFO = 3,
	RECHARGING = 4,
	CALMODE = 8,
	IDLE = 16,
};

class UI
{
	uint8_t state;
	Lcd *_lcd;
	Led *_led;
	Button *_button;
	uint8_t internal_state_counter;
	uint8_t state_active_time_counter;
	public:
	UI(Lcd *lcd, Led *led, Button *);
	void update(Account*, Meter*);
};

UI::UI(Lcd *lcd, Led *led, Button *button)
{
	_lcd = lcd;
	_led = led;
	_button = button;
	internal_state_counter = 0;
	state_active_time_counter = 0;
}

void UI::update(Account *account, Meter *meter)
{
	uint8_t acc_state = account->get_state();
	uint16_t days;
	state_active_time_counter += 1;

	if (_button->get_and_clear())
	{
		if(state == IDLE)
			state = NORMAL;
		else
			state = INFO;
		internal_state_counter = 0;
		state_active_time_counter = 0;
	}

	if (state != INFO)
	{
		if (acc_state == 0)
			state = NORMAL;
		else if(acc_state&CAL_MODE)
			state = CALMODE;
		else
			state = ERROR;
	}

	if (state_active_time_counter >= MAX_STATE_ACTIVE_COUNT)
	{
		state_active_time_counter = MAX_STATE_ACTIVE_COUNT;
		state = IDLE;
	}
	

	if (state == IDLE)
	{
		_lcd->print(3, ' ');
		_lcd->print(2, ' ');
		_lcd->print(1, ' ');
		_lcd->print(0, ' ');
		_led->turn_on(LED_PIN1);
		_led->turn_on(LED_PIN2);
	}	
	else if (state == NORMAL)
	{
		if (internal_state_counter == 0)
		{
			_lcd->print_number(account->get_balance());
			_led->turn_on(LED_PIN1);
			_led->turn_off(LED_PIN2);
			internal_state_counter = 1;
		}
		else
		{
			_lcd->print_number(meter->power_apparent_val());
			_led->turn_off(LED_PIN1);
			_led->turn_on(LED_PIN2);
			internal_state_counter = 0;
		}
	}
	else if (state == INFO)
	{
		switch(internal_state_counter)
		{
			case 0:
				_lcd->print(3, 1);
				_lcd->print(2, 'n');
				_lcd->print(1, 'F');
				_lcd->print(0, 0);
				_led->turn_on(LED_PIN1);
				_led->turn_on(LED_PIN2);
				internal_state_counter += 1;
				break;
			case 1:
				days = account->get_validity()/(3600*24);
				_lcd->print_number(days);
				_led->turn_on(LED_PIN1);
				_led->turn_off(LED_PIN2);
				internal_state_counter += 1;
				break;
			case 2:
				_lcd->print_number(meter->voltage());
				_led->turn_off(LED_PIN1);
				_led->turn_on(LED_PIN2);
				internal_state_counter += 1;
				break;
			case 3:
				_lcd->print_number(meter->current());
				_led->turn_on(LED_PIN1);
				_led->turn_off(LED_PIN2);
				internal_state_counter += 1;
				break;
			case 4:
				_lcd->print_number(meter->power_apparent_val());
				_led->turn_off(LED_PIN1);
				_led->turn_on(LED_PIN2);
				internal_state_counter += 1;
				break;
			default:
				_lcd->print(3, 'E');
				_lcd->print(2, 'n');
				_lcd->print(1, 'd');
				_lcd->print(0, ' ');
				_led->turn_off(LED_PIN1);
				_led->turn_off(LED_PIN2);
				internal_state_counter = 0;	
				state = NORMAL;
		}
	}
	else if (state == CALMODE)
	{
		if (internal_state_counter == 0)
		{
			_lcd->print(3, 'C');
			_lcd->print(2, 'A');
			_lcd->print(1, 'L');
			_lcd->print(0, ' ');
			internal_state_counter += 1;
		}
		else
		{
			_lcd->clear();
			internal_state_counter = 0;
		}
	}
	else if (state == ERROR)
	{
		if (internal_state_counter == 0)
		{
			_lcd->print_number(acc_state);
			_lcd->print(3, 'E');
			internal_state_counter += 1;
		}
		else
		{
			_lcd->clear();
			internal_state_counter = 0;
		}
	}
}

#endif /* UI_H_ */
