
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
	int32_t voltage_scale;
	int32_t current_offset;
	int32_t current_scale;
	public:
	MeterCofficients();
	void calculate_parameters(uint8_t type, int32_t actual_val1, int32_t measured_val1, int32_t actual_val2, int32_t measured_val2);
	int32_t calculate_values(int type, int32_t measured_val);
	inline void store_parameters();
	void read_from_rom();
	void write_to_rom();
	inline int32_t get_voltage_offset()
	{
		return voltage_offset;
	}
	inline int32_t get_voltage_scale()
	{
		return voltage_scale;
	}
	inline int32_t get_current_scale()
	{
		return current_scale;
	}
	inline int32_t get_current_offset()
	{
		return current_offset;
	}
};

MeterCofficients::MeterCofficients()
{
	//Load values from EEPROM
	/*voltage_offset = 0;
	voltage_scale = 1;
	current_scale =1;
	current_offset = 0;
	write_to_rom();*/
	read_from_rom();
}

void MeterCofficients::calculate_parameters(uint8_t type, int32_t actual_val1, int32_t measured_val1, int32_t actual_val2, int32_t measured_val2)
{
	float scale = float((actual_val2 - actual_val1)*10000)/(measured_val2 - measured_val1);
	int32_t offset = actual_val2*10000 - scale*measured_val2;
	if (type == 1)
	{
		voltage_offset = offset;
		voltage_scale = int32_t(scale);
	}
	else
	{
		current_offset = offset;
		current_scale = int32_t(scale);
	}
}

int32_t MeterCofficients::calculate_values(int type, int32_t measured_val)
{
	int32_t scale, offset;
	if (type ==1 )
	{
		scale = voltage_scale;
		offset = voltage_offset;
	}
	else
	{
		scale = current_scale;
		offset = current_offset;
	}
	return (scale*measured_val + offset)/10000;
}

inline void MeterCofficients::store_parameters()
{
	//Store the parameters into EEPROM
	write_to_rom();
}

void MeterCofficients::read_from_rom()
{
	uint32_t address = EEPROM_START_ADDRESS_CALIBRATION;
	voltage_offset = (int32_t)eeprom_read_dword((const uint32_t *)address);
	address += 4;
	voltage_scale = (int32_t)eeprom_read_dword((const uint32_t *)address);
	address += 4;
	current_offset = (int32_t)eeprom_read_dword((const uint32_t *)address);
	address += 4;
	current_scale = (int32_t)eeprom_read_dword((const uint32_t *)address);
}

void MeterCofficients::write_to_rom()
{
	uint32_t address = EEPROM_START_ADDRESS_CALIBRATION;
	eeprom_write_dword((uint32_t*)address, voltage_offset);
	address += 4;
	eeprom_write_dword((uint32_t*)address, voltage_scale);
	address += 4;
	eeprom_write_dword((uint32_t*)address, current_offset);
	address += 4;
	eeprom_write_dword((uint32_t*)address, current_scale);
}


class Meter
{
	int32_t voltage_scaled;
	int32_t voltage_raw, voltage_rms, voltage_rms_sum;
	int32_t voltage_unscaled, voltage_unscaled_sum;

	uint8_t counter_rms, counter_avg;
	
	int32_t current_scaled;
	int32_t current_raw, current_rms, current_rms_sum;
	int32_t current_unscaled, current_unscaled_sum;
		
	int32_t power_apparent;
	//int32_t power_active, power_active_sum;
	//uint8_t power_factor;
	/*
	uint32_t energy;
	uint32_t last_tick;
	*/
	Adc adc;
	
	public:
	MeterCofficients meter_coff;
	
	Meter();
	void update();
	inline int32_t voltage()
	{
		return voltage_scaled;
	}
	inline int32_t current()
	{
		return current_scaled;
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
	/*inline int32_t power_val()
	{
		return power_active;
	}*/
	inline int32_t power_apparent_val()
	{
		return power_apparent;
	}
	/*inline int32_t power_factor_val()
	{
		return power_factor;
	}*/
	/*inline int32_t energy_val()
	{
		return energy;
	}*/
};

Meter::Meter()
{
	adc = Adc();
	counter_rms = 0;
	counter_avg = 0;
	voltage_rms_sum = 0;
	voltage_unscaled_sum = 0;
	current_rms_sum = 0;
	current_unscaled_sum = 0;
	//power_active_sum = 0;
}

void Meter::update()
{
	uint32_t this_time, time_diff;
	//int32_t pwr_val;
	current_raw = adc.read(CURRENT_CHANNEL);
	voltage_raw = adc.read(VOLTAGE_CHANNEL);
	//pwr_val = current_raw*voltage_raw;
	//if (pwr_val < 0)
	//	pwr_val *= -1;
	//power_active_sum += pwr_val; 
	voltage_rms_sum += int32_t(pow(voltage_raw, 2));
	current_rms_sum += int32_t(pow(current_raw, 2));	
	
	counter_rms += 1;
	
	if(counter_rms >= SAMPLES_PER_RMS)
	{
		voltage_rms = int32_t(sqrt(voltage_rms_sum/SAMPLES_PER_RMS));
		current_rms = int32_t(sqrt(current_rms_sum/SAMPLES_PER_RMS));
		voltage_rms_sum = 0;
		current_rms_sum	= 0;
		counter_rms = 0;
		
		voltage_unscaled_sum += voltage_rms;
		current_unscaled_sum += current_rms;
		counter_avg += 1;
		
		if (counter_avg >= SAMPLES_PER_AVG)
		{
			voltage_unscaled = voltage_unscaled_sum/SAMPLES_PER_AVG;
			current_unscaled = current_unscaled_sum/SAMPLES_PER_AVG;
			voltage_unscaled_sum = 0;
			current_unscaled_sum = 0;
			counter_avg = 0;
			
			voltage_scaled = meter_coff.calculate_values(1, voltage_unscaled);
			current_scaled = meter_coff.calculate_values(0, current_unscaled);
			
			
			//Temporary
			//voltage_rms = 2300;
			//current_rms = 3400;
			
			//power_active = power_active_sum/(SAMPLES_PER_AVG+SAMPLES_PER_RMS);
			//power_active_sum = 0;
			power_apparent = (voltage_scaled*current_scaled)/10000;
			//power_factor = power_apparent/power_active;
			
			/*this_time = system_time.get();
			time_diff = this_time - last_tick;
			last_tick = this_time;
			energy = power_apparent*time_diff;*/
		}
	}
}

#endif /* METER_H_ */
