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

enum Account_states
{
	ALL_OK = 0,
	ZERO_BALANCE = 1,
	NO_VALIDITY = 2,
	NEGATIVE_BALANCE = 4,
	INVALID_RATE = 8
};
class Account
{
	int balance; //Credits available in the meter.
	uint32_t validity; //Validity of the balance in seconds. Can store up to 136 years.
	uint32_t rate; //Price per kwh
	uint32_t carry_farword_units; //Number of energy-units(joules), that to be forwarded to next period when the validity ends.
	uint32_t energy_raminning; // Energy available to the user, in joules. Maximum value can be 1193 kwh.
	uint32_t energy_used;// Total energy used by by the consumer till now
	//Recharge recharge; //To be added.
	uint32_t last_recharge_time;
	//uint32_t last_managed_time; //Time when the account was last maintained.
	uint32_t _last_second; //Last second.
	uint8_t state;
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
	inline int get_balance()
	{
		return balance;
	}
	inline uint32_t get_rate()
	{
		return rate;
	}
	inline uint32_t get_energy_used()
	{
		return energy_used;
	}
	inline uint32_t get_validity()
	{
		return validity;
	}
	inline uint32_t get_last_time()
	{
		return _last_second;
	}
};
Account::Account()
{
	//Improvement: Read data from EEPROM here.
	balance = 10;
	validity = 999999;
	rate = 999999;
	carry_farword_units = 9999;
	energy_raminning = balance*rate;
	energy_used = 0;
	_last_second = 0;
}

void Account::read_from_rom()
{
	uint32_t address = EEPROM_START_ADDRESS_ACCOUNT;
	balance = eeprom_read_word(gen_address(EEPROM_START_ADDRESS_ACCOUNT,0));
	address += 2;
	validity = eeprom_read_dword(gen_address(EEPROM_START_ADDRESS_ACCOUNT,2));
	address += 4;
	rate = eeprom_read_dword(address);
	address += 4;
	carry_farword_units = eeprom_read_dword(address);
	address += 4;
	energy_raminning = eeprom_read_dword(address);
	address += 4;
	energy_used = eeprom_read_dword(address);
	address += 4;
	last_recharge_time = eeprom_read_dword(address);
	address += 4;
	_last_second = eeprom_read_dword(address);
	address += 4;
	state = eeprom_read_byte(address);
	address += 1;
}
void Account::write_to_rom()
{
		uint32_t address = EEPROM_START_ADDRESS_ACCOUNT;
		eeprom_write_word(address, balance);
		address += 2;
		eeprom_write_dword(address, validity);
		address += 4;
		eeprom_write_dword(address, rate);
		address += 4;
		eeprom_write_dword(address, carry_farword_units);
		address += 4;
		eeprom_write_dword(address, energy_raminning);
		address += 4;
		eeprom_write_dword(address, energy_used);
		address += 4;
		eeprom_write_dword(address, last_recharge_time);
		address += 4;
		eeprom_write_dword(address, _last_second);
		address += 4;
		eeprom_write_byte(address, state);
		address += 1;
}
uint8_t Account::recharge(int recharge_amount, uint32_t recharge_validity, uint32_t carry_forward_energy_units)
{
	if (validity <= 0 && energy_raminning > 0)//if the validity of the previous recharge is finished
	{
		//If the remaining energy from previous recharge is greater than the
		//maximum number of energy units that can be forwarded to next span,
		//then discard the extra energy.
		energy_raminning = carry_farword_units;
		carry_farword_units = 0;
	}
	uint32_t new_energy = recharge_amount*rate;
	energy_raminning += new_energy;
	balance = energy_raminning/rate;
	carry_farword_units += carry_forward_energy_units;
	validity += recharge_validity;
	last_recharge_time = system_time.get();
	state = 0;
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
	energy_raminning -= energy;
	energy_used += energy;
	balance = energy_raminning/rate;
	if (balance == 0)
	state |= ZERO_BALANCE;
	if (balance < 0)
	state |= NEGATIVE_BALANCE;
	if (validity <= 0)
	state |= NO_VALIDITY;
	if (rate <= 0)
	state |= INVALID_RATE;
	
	
	//if (temp_state == 0)
	//	state = 0;
	//else
	//	state = temp_state;
}

#endif /* ACCOUNTH_H_ */