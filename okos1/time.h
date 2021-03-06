/*
 * time.h
 *
 * Created: 3/16/2014 9:47:05 PM
 *  Author: vicky
 */ 


#ifndef TIME_H_
#define TIME_H_

/*
Time class to srore real time.
*/

class Time
{
	public:
	volatile uint8_t ticks;
	private:
	uint32_t seconds;
	
	public:
	void read_from_rom();
	void write_to_rom();
	Time()
	{
		ticks = 0;
		read_from_rom();
	}
	
	void read()
	{
		//Read time from RTC.
	}
	
	inline void add_tick()
	{
		if (ticks>=TICKS_PER_SECOND)
		{
			seconds += 1;
			ticks = 0;
		}
		else
			ticks += 1;
	}
	inline uint32_t get()
	{
		return seconds;
	}
	inline void set(uint32_t new_seconds)
	{
		seconds = new_seconds;
	}
};

void Time::read_from_rom()
{
	uint32_t address = EEPROM_START_ADDRESS_TIMER;	
	seconds = eeprom_read_dword((const uint32_t*)address);
}

void Time::write_to_rom()
{
	uint32_t address = EEPROM_START_ADDRESS_TIMER;	
	eeprom_write_dword((uint32_t*)address, seconds);
}

#endif /* TIME_H_ */
