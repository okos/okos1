/*
 * meter.h
 *
 * Created: 3/18/2014 4:57:25 PM
 *  Author: vicky
 */ 

#include <math.h>
#ifndef METER_H_
#define METER_H_

extern Time system_time;
class MeterCofficients
{
	int32_t voltage_offset;
	int32_t voltege_scale;
	int32_t current_offset;
	int32_t current_scale;
	public:
	MeterCofficients();
	void calculate_parameters(uint8_t type, int32_t actual_val1, int32_t measured_val1, int32_t actual_val2, int32_t measured_val2);
	int32_t calculate_values(int type, int32_t measured_val);
	void store_parameters();	
};

MeterCofficients::MeterCofficients()
{
	//Load values from EEPROM
	voltage_offset = 0;
	voltege_scale = 10;
	current_scale =20;
	current_offset = 0;
}

void MeterCofficients::calculate_parameters(uint8_t type, int32_t actual_val1, int32_t measured_val1, int32_t actual_val2, int32_t measured_val2)
{
	float scale = float(actual_val2 - actual_val2)/(measured_val2 - measured_val1);
	int32_t offset = actual_val2 - scale*measured_val2;
	if (type == 0)
	{
		voltage_offset = offset;
		voltege_scale = int32_t(scale);
	}
	else
	{
		current_offset = offset;
		current_scale = int32_t(scale);
	}
}

int32_t MeterCofficients::calculate_values(int type, int32_t measured_val)
{
	int scale, offset;
	if (type ==1 )
	{
		scale = voltege_scale;
		offset = voltage_offset;
	}
	else
	{
		scale = current_scale;
		offset = current_offset;
	}
	return int32_t(scale)*measured_val + offset;
}

void MeterCofficients::store_parameters()
{
	//Store the parameters into EEPROM
}


class Meter
{
	int32_t voltage_rms;
	int32_t voltage_raw, voltage_raw_avg, voltage_raw_sum;
	int32_t voltage_unscaled, voltage_unscaled_sum;

	uint8_t counter_raw, counter_unscaled;
	
	int32_t current_rms;
	int32_t current_raw, current_raw_avg, current_raw_sum;
	int32_t current_unscaled, current_unscaled_sum;
		
	int32_t power;
	uint32_t energy;
	uint32_t last_tick;
	Adc adc;
	MeterCofficients meter_coff;
	
	public:
	Meter();
	void update();
	inline int32_t voltage()
	{
		return voltage_rms;
	}
	inline int32_t current()
	{
		return current_rms;
	}
	inline int32_t voltage_val_raw()
	{
		return voltage_raw;
	}
	inline int32_t current_val_raw()
	{
		return current_raw;
	}
	inline int32_t voltage_val_unscaled()
	{
		return voltage_unscaled;
	}
	inline int32_t current_val_unscaled()
	{
		return current_unscaled;
	}
	inline int32_t power_val()
	{
		return power;
	}
	inline int32_t energy_val()
	{
		return energy;
	}
};

Meter::Meter()
{
	adc = Adc();
	counter_raw = 0;
	counter_unscaled = 0;
	voltage_raw_sum = 0;
	voltage_unscaled_sum = 0;
	current_raw_sum = 0;
	current_unscaled_sum = 0;
}

void Meter::update()
{
	
	current_raw = adc.read(CURRENT_CHANNEL);
	voltage_raw = adc.read(VOLTAGE_CHANNEL);

	if(counter_raw < SAMPLES_PER_AVG)
	{
		voltage_raw_sum += voltage_raw;
		current_raw_sum += current_raw;
		
		counter_raw += 1;
	}
	else
	{
		voltage_raw_avg = voltage_raw_sum/SAMPLES_PER_AVG;
		current_raw_avg = current_raw_sum/SAMPLES_PER_AVG;
		voltage_raw_sum = 0;
		current_raw_sum	= 0;
		counter_raw = 0;
		
		if (counter_unscaled < SAMPLES_PER_RMS)
		{
			voltage_unscaled_sum += int32_t(pow(voltage_raw_avg, 2));
			current_unscaled_sum += int32_t(pow(current_raw_avg, 2));
			
			counter_unscaled += 1;
		}
		else
		{
			int32_t temp_voltage;
			int32_t temp_current;
			uint32_t this_time, time_diff;
			temp_voltage = voltage_unscaled_sum/SAMPLES_PER_RMS;
			temp_current = current_unscaled_sum/SAMPLES_PER_RMS;
			voltage_unscaled = int32_t(sqrt(temp_voltage));
			current_unscaled = int32_t(sqrt(temp_current));
			voltage_unscaled_sum = 0;
			current_unscaled_sum = 0;
			counter_unscaled = 0;
			
			voltage_rms = meter_coff.calculate_values(1, voltage_unscaled);
			current_rms = meter_coff.calculate_values(0, current_unscaled);
			
			
			//Temporary
			//voltage_rms = 2300;
			//current_rms = 3400;
			
			power = (voltage_rms*current_rms)/10000;
			this_time = system_time.get();
			time_diff = this_time - last_tick;
			last_tick = this_time;
			energy = power*time_diff;
			
		}
	}
}

#endif /* METER_H_ */