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
	
	Time()
	{
		ticks = 0;
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
	
};



#endif /* TIME_H_ */