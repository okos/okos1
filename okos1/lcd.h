/*
 * lcd.h
 *
 * Created: 3/12/2014 12:07:25 AM
 *  Author: vicky
 */ 

#ifndef LCD_H_
#define LCD_H_


class Lcd
{
	/*volatile*/ int8_t value[4];
	/*volatile*/ uint8_t turn;
	public:
	Lcd();
	void update();
	void print(int8_t, int8_t, uint8_t);
	void print_number(int16_t decimal);
	
	inline void clear_all()
	{
		#ifdef LCD_CONTROL_PORT_HIGH_
		LCD_CONTROL_PORT &= ~0xf0;
		#else
		LCD_CONTROL_PORT &= ~0x0f;
		#endif
	}
	inline void clear()
	{
		LCD_DATA_PORT =  0b00000000;
	}
	inline void display_all()
	{
		LCD_DATA_PORT |= 0xff;
		#ifdef LCD_CONTROL_PORT_HIGH_
		LCD_CONTROL_PORT &= ~0xf0;
		#else
		LCD_CONTROL_PORT &= ~0x0f;
		#endif
	}
};
Lcd::Lcd()
{
	LCD_DATA_DIRECTION = 0xff;
	#ifdef LCD_CONTROL_PORT_HIGH_
	LCD_CONTROL_DIRECTION |= 0xf0;
	#else
	LCD_CONTROL_DIRECTION |= 0xf0;
	#endif
	turn = 0;
	for(uint8_t i=0;i<4;i++)
	value[i]=0;
}
void Lcd::update()
{
	if(turn == 4)
	turn = 0;
	else
	turn += 1;
	#ifdef DEV_BOARD
		LCD_DATA_PORT =  0b00000000; //clear the digit
	#else
		LCD_DATA_PORT =  ~0b00000000; //clear the digit
	#endif

	#ifdef LCD_CONTROL_PORT_HIGH_
		LCD_CONTROL_PORT &= 0x0f;
		PORTD |= 1<<(4+turn);
	#else
		LCD_CONTROL_PORT  &= 0xf0;
		PORTD |= 1<<(turn);
	#endif


	#ifdef DEV_BOARD
		LCD_DATA_PORT = value[turn];
	#else
		LCD_DATA_PORT = ~value[turn];
	#endif

}
void Lcd::print(int8_t segment_number, int8_t val, uint8_t is_decimal=0)
{
	switch(val)
	{
		case ' ':
		value[segment_number] = 0b00000000;
		break;
		case 0:
		value[segment_number] = 0b00111111;
		break;
		case 1:
		value[segment_number] = 0b00000110;
		break;
		case 2:
		value[segment_number] = 0b01011011;
		break;
		case 3:
		value[segment_number] = 0b01001111;
		break;
		case 4:
		value[segment_number] = 0b01100110;
		break;
		case 5:
		value[segment_number] = 0b01101101;
		break;
		case 6:
		value[segment_number] = 0b01111101;
		break;
		case 7:
		value[segment_number] = 0b00000111;
		break;
		case 8:
		value[segment_number] = 0b01111111;
		break;
		case 9:
		value[segment_number] = 0b01101111;
		break;
		case '.':
		value[segment_number] = 0b10000000;
		break;
		case '-':
		value[segment_number] = 0b01000000;
		break;
		case 'A':
		value[segment_number] = 0b01110111;
		break;
		case 'C':
		value[segment_number] = 0b00111001;
		break;
		case 'E':
		value[segment_number] = 0b01111001;
		break;
		case 'F':
		value[segment_number] = 0b01110001;
		break;
		case 'H':
		value[segment_number] = 0b01110110;
		break;
		case 'J':
		value[segment_number] = 0b00011110;
		break;
		case 'L':
		value[segment_number] = 0b00111000;
		break;
		case 'P':
		value[segment_number] = 0b01110011;
		break;
		case 'd':
		value[segment_number] = 0b01011110;
		break;
		case 'h':
		value[segment_number] = 0b01110100;
		break;
		case 'i':
		value[segment_number] = 0b00110000;
		break;
		case 'n':
		value[segment_number] = 0b01010100;
		break;
		default:
		value[segment_number] = 0b00000000;
		break;
	}
	if (is_decimal)
	{
		value[segment_number] |= 0x80;
	}
}

void Lcd::print_number(int16_t decimal)
{
	uint8_t number_of_digits=0, count = 0;
	uint16_t temp_val;
	for (temp_val = 0;temp_val<4;temp_val++)
	print(temp_val, ' ');
	if(decimal == 0)
	print(0,0);
	else if (decimal > 9999 or decimal < -999)
	{
		print(2, 'L');
		print(1,'A');
	}
	else
	{
		number_of_digits = 0;
		temp_val = decimal>0?decimal:-1*decimal;
		while (temp_val > 0)
		{
			temp_val /= 10;
			number_of_digits += 1;
		}
		if (decimal < 0)
		{
			print(number_of_digits, '-');
			number_of_digits -= 1;
			decimal *= -1;
		}
		while(decimal>0)
		{
			print(count, decimal % 10);
			decimal /= 10;
			count += 1;
		}
	}
}
#endif /* LCD_H_ */
