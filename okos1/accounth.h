/*
 * accounth.h
 *
 * Created: 3/16/2014 9:12:10 PM
 *  Author: vicky
 */ 


#ifndef ACCOUNTH_H_
#define ACCOUNTH_H_

/*
This class maintains the accounting related information of the smart meter.
Improvements: Add recharge class;
Add a time class;
*/

#define gen_address(address, val_to_add) (const uint32_t)(address+val_to_add)


//Meter states
enum Account_states
{
	ALL_OK = 0,
	ZERO_BALANCE = 1,
	NO_VALIDITY = 2,
	NEGATIVE_BALANCE = 4,
	INVALID_RATE = 8,
	OVERLOAD = 16,
	RELAY_OFF = 32,
};


class Account
{
	uint32_t ip_address;
	uint8_t hw_version;
	uint8_t sw_version;
	uint32_t installation_date;
	
	uint32_t rate; //Price per kwh
	uint16_t power_limit; //Maximum power that can be drawn from the meter
	uint8_t state; //State of the meter
	
	int balance; //Credits available in the meter.
	uint32_t validity; //Validity of the balance in seconds. Can store up to 136 years.
	uint32_t carry_forward_units; //Number of energy-units(joules), that to be forwarded to next period when the validity ends.
	uint32_t energy_remaining; // Energy available to the user, in joules. Maximum value can be 1193 kwh.
	uint32_t energy_used;// Total energy used by by the consumer till now
	uint32_t last_recharge_time;
	uint32_t _last_second; //Last second.
	uint32_t _overload_time;//Time when the meter was overloaded
	uint8_t eeprom_updated;
	public:
	Account();
	void read_from_rom();
	void write_to_rom();
	void update(Meter *meter);
	uint8_t recharge(int recharge_amount, uint32_t recharge_validity, uint32_t carry_forward_energy_units);
	inline uint8_t get_state()
	{
		return state;
	}
	inline void set_state(uint8_t new_state)
	{
		state = new_state;
	}
	inline uint8_t get_hw_version()
	{
		return hw_version;
	}
	inline void set_hw_version(uint8_t new_hw_version)
	{
		hw_version = new_hw_version;
	}
	inline uint8_t get_sw_version()
	{
		return sw_version;
	}
	inline void set_sw_version(uint8_t new_sw_version)
	{
		sw_version = new_sw_version;
	}
	inline int get_balance()
	{
		return balance;
	}
	inline uint32_t get_rate()
	{
		return rate;
	}
	inline void set_rate(uint32_t new_rate)
	{
		rate = new_rate;
	}
	inline uint32_t get_energy_used()
	{
		return energy_used;
	}
	inline void set_energy_remaining(uint32_t new_energy_remaining)
	{
		energy_remaining = new_energy_remaining;
	}
	inline uint32_t get_energy_remaining()
	{
		return energy_remaining;
	}
	inline uint32_t get_validity()
	{
		return validity;
	}
	inline uint32_t get_last_time()
	{
		return _last_second;
	}
	inline uint32_t get_ip_address()
	{
		return ip_address;
	}
	inline void set_ip_address(uint32_t address)
	{
		ip_address = address;
	}
	inline uint16_t get_power_limit()
	{
		return power_limit;
	}
	inline void set_power_limit(uint16_t new_power_limit)
	{
		power_limit = new_power_limit;
	}
	inline uint32_t get_installation_date()
	{
		return installation_date;
	}
	inline uint32_t get_carry_forward_units()
	{
		return carry_forward_units;
	}
	inline uint32_t get_last_recharge_time()
	{
		return last_recharge_time;
	}
	inline void set_installation_date(uint32_t new_installation_date)
	{
		installation_date = new_installation_date;
	}
};
Account::Account()
{
	//Improvement: Read data from EEPROM here.
	/*ip_address = 1234;
	hw_version = 1;
	sw_version = 1;
	installation_date = 99999;
	balance = 10;
	validity = 999999;
	rate = 999999;
	carry_forward_units = 9999;
	energy_remaining = balance*rate;
	energy_used = 0;
	_last_second = 0;
	power_limit = 100;
	write_to_rom();*/
	read_from_rom();
	eeprom_updated = 0;
	_overload_time = 0;
}

void Account::read_from_rom()
{
	uint32_t address = EEPROM_START_ADDRESS_ACCOUNT;
	
	ip_address = eeprom_read_dword((const uint32_t*)address);
	address += 4;
	hw_version = eeprom_read_byte((const uint8_t*)address);
	address += 1;
	sw_version = eeprom_read_byte((const uint8_t*)address);
	address += 1;
	installation_date = eeprom_read_dword((const uint32_t*)address);
	address += 4;
	
	rate = eeprom_read_dword((const uint32_t*)address);
	address += 4;
	power_limit = eeprom_read_word((const uint16_t*)address);
	address += 2;
	state = eeprom_read_byte((const uint8_t*)address);
	address += 1;
	
	balance = eeprom_read_word((const uint16_t *)address);
	address += 2;
	validity = eeprom_read_dword((const uint32_t*)address);
	address += 4;
	carry_forward_units = eeprom_read_dword((const uint32_t*)address);
	address += 4;
	energy_remaining = eeprom_read_dword((const uint32_t*)address);
	address += 4;
	energy_used = eeprom_read_dword((const uint32_t*)address);
	address += 4;
	last_recharge_time = eeprom_read_dword((const uint32_t*)address);
	address += 4;
	_last_second = eeprom_read_dword((const uint32_t*)address);
	address += 4;
}

void Account::write_to_rom()
{
		
		uint32_t address = EEPROM_START_ADDRESS_ACCOUNT;
		eeprom_write_dword((uint32_t*)address, ip_address);
		address += 4;
		eeprom_write_byte((uint8_t*)address, hw_version);
		address += 1;
		eeprom_write_byte((uint8_t*)address, sw_version);
		address += 1;
		eeprom_write_dword((uint32_t*)address, installation_date);
		address += 4;
		
		
		eeprom_write_dword((uint32_t*)address, rate);
		address += 4;
		eeprom_write_word((uint16_t*)address, power_limit);
		address += 2;
		eeprom_write_byte((uint8_t*)address, state);
		address += 1;
		
		
		eeprom_write_word((uint16_t*)address, balance);
		address += 2;
		eeprom_write_dword((uint32_t*)address, validity);
		address += 4;
		eeprom_write_dword((uint32_t*)address, carry_forward_units);
		address += 4;
		eeprom_write_dword((uint32_t*)address, energy_remaining);
		address += 4;
		eeprom_write_dword((uint32_t*)address, energy_used);
		address += 4;
		eeprom_write_dword((uint32_t*)address, last_recharge_time);
		address += 4;
		eeprom_write_dword((uint32_t*)address, _last_second);
		address += 4;
		
		eeprom_updated = 1;
}

uint8_t Account::recharge(int recharge_amount, uint32_t recharge_validity, uint32_t carry_forward_energy_units)
{
	if (validity <= 0 && energy_remaining > 0)//if the validity of the previous recharge is finished
	{
		//If the remaining energy from previous recharge is greater than the
		//maximum number of energy units that can be forwarded to next span,
		//then discard the extra energy.
		energy_remaining = carry_forward_units;
		carry_forward_units = 0;
	}
	uint32_t new_energy = recharge_amount*rate;
	energy_remaining += new_energy;
	balance = energy_remaining/rate;
	carry_forward_units += carry_forward_energy_units;
	validity += recharge_validity;
	last_recharge_time = system_time.get();
	state = 0;
	return 1;
}

void Account::update(Meter *meter)
{
	
	uint8_t temp_state = 0;
	uint32_t energy = meter->energy_val();
	uint32_t this_second = system_time.get();
	if (this_second>_last_second)
	{
		if (validity>0)
			validity -= 1;
		_last_second = this_second;
	}
	energy_remaining -= energy;
	energy_used += energy;
	balance = energy_remaining/rate;
	if (balance == 0)
		state |= ZERO_BALANCE;
	else
		state &= ~ZERO_BALANCE;
	if (balance < 0)
		state |= NEGATIVE_BALANCE;
	else
		state &= ~NEGATIVE_BALANCE;
	if (validity <= 0)
		state |= NO_VALIDITY;
	else
		state &= ~NO_VALIDITY;
	if (rate <= 0)
		state |= INVALID_RATE;
	else
		state &= ~INVALID_RATE;
	
	if (state&OVERLOAD)
	{
		if (system_time.get()-_overload_time > OVERLOAD_RESET_TIME)
		{
			state &= ~OVERLOAD;
		}
	}
	
	if (meter->power_apparent_val() > power_limit)
		state |= OVERLOAD;
		
	if (meter->voltage() < 1500)
	{
		if (!eeprom_updated)
		{
			write_to_rom();
		}
	}
}

#endif /* ACCOUNTH_H_ */