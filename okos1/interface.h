/*
 * interface.h
 *
 * Created: 3/27/2014 10:57:13 AM
 *  Author: vicky
 */ 


#ifndef INTERFACE_H_
#define INTERFACE_H_

class Interface
{	
	Account *account;
	Meter *meter;
	Communication *comm;
	Time *system_time;
	char buffer[120];
	Packet pkt;
	int32_t temp_val;	
	public:
	Interface(Account*, Meter*, Communication*, Time*);
	uint8_t check_packet(Packet*);
	void make_packet(Packet *packet, uint32_t destination_address, uint8_t pkt_type, char *data);
	void update();
};

Interface::Interface(Account *acc, Meter *mtr, Communication *cmm, Time *syt)
{
	pkt = Packet();
	account = acc;
	meter = mtr;
	comm = cmm;
	system_time = syt;
}

inline uint8_t Interface::check_packet(Packet *packet)
{
	if (packet->destination_address == account->get_ip_address() || packet->destination_address == 0 || packet->destination_address == 255)
		return 1;
	return 0;
}

void Interface::make_packet(Packet *packet, uint32_t destination_address, uint8_t pkt_type, char *data)
{
	uint8_t i;
	packet->source_address = account->get_ip_address();
	packet->destination_address = destination_address;
	packet->type = pkt_type;
	for (i=0;data[i]!='\0';i++)
		packet->data[i] = data[i];
	packet->data[i] = '\0';
	packet->length = 12 + i;
}

void Interface::update()
{
	uint32_t val;	
	if (!comm->receive(&pkt))
		return;
	if (!check_packet(&pkt))
		return;
	if (pkt.type == NORMAL_PACKET)
	{
		uint8_t reconized = 0;
		//sscanf(pkt.data, "%s", buffer);
		strcpy(buffer, pkt.data);
		val = 0;
		if (sscanf(buffer, "set address %lu", &val) == 1)
		{
			account->set_ip_address(val);
			/*buffer[0] = '\0';
			sprintf(buffer, "Address is now %lu", val);*/
			reconized = 1;
		}
		else if (sscanf(buffer, "set rate %lu", &val) == 1)
		{
			account->set_rate(val);
			/*buffer[0] = '\0';
			sprintf(buffer, "Rate is now %lu", val);*/
			reconized = 1;
		}
		else if (sscanf(buffer, "set power limit %lu", &val) == 1)
		{
			account->set_rate(val);
			account->write_to_rom();
			/*buffer[0] = '\0';
			sprintf(buffer, "Power limit is now %lu", val);*/
			reconized = 1;
		}
		else if (sscanf(buffer, "set time %lu", &val) == 1)
		{
			system_time->set(val);
			reconized = 1;
		}
		else if (sscanf(buffer, "set hw version %lu", &val) == 1)
		{
			account->set_hw_version((uint8_t)val);
			reconized = 1;
		}
		else if (sscanf(buffer, "set sw version %lu", &val) == 1)
		{
			account->set_hw_version((uint8_t)val);
			reconized = 1;
		}
		else if (sscanf(buffer, "set installation date %lu", &val) == 1)
		{
			account->set_installation_date(val);
			reconized = 1;
		}
		else if (strcmp(buffer, "reset energy") == 0)
		{
			account->set_energy_remaining(0);
			reconized = 1;
		}
		else if (sscanf(buffer, "cal v %lu", &val) == 1)
		{
			meter->meter_coff.calculate_parameters(1, 0, 0, meter->voltage_val_unscaled(), (int32_t)val);
			meter->meter_coff.store_parameters();
			reconized = 1;
		}
		else if (strcmp(buffer, "cal i ofs") == 0)
		{
			temp_val = meter->current_val_unscaled();
			reconized = 1;
		}
		else if (sscanf(buffer, "cal i %lu", &val) == 1)
		{
			meter->meter_coff.calculate_parameters(0, temp_val, 0, meter->current_val_unscaled(), (int32_t)val);
			meter->meter_coff.store_parameters();
			reconized = 1;
		}
		else if (strcmp(buffer, "turn relay off") == 0)
		{
			account->set_state(account->get_state()|RELAY_OFF);
			reconized = 1;
		}
		else if (strcmp(buffer, "turn relay on") == 0)
		{
			account->set_state(account->get_state()&(~RELAY_OFF));
			reconized = 1;
		}
		if (reconized)
		{
			buffer[0] = '\0';
			sprintf(buffer,"Done(%lu)",val);
			make_packet(&pkt, pkt.destination_address, NORMAL_PACKET, buffer);
			account->write_to_rom();
		}
		else
			make_packet(&pkt, pkt.destination_address, NORMAL_PACKET, "Invalid command");		
		comm->send(&pkt);
	}
	else if (pkt.type == DATA_PACKET)
	{
		buffer[0] = '\0';
		char tmp_buffer[15];
		sprintf(tmp_buffer, "%lu,\0", system_time->get());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%lu,\0", account->get_ip_address());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%ld,\0", meter->voltage());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%ld,\0", meter->current());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%ld,\0", meter->power_apparent_val());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%lu,\0", account->get_energy_used());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%d,\0", account->get_balance());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%hu,\0", account->get_state());
		strcat(buffer, tmp_buffer);
		make_packet(&pkt, pkt.source_address, DATA_PACKET, buffer);
		comm->send(&pkt);
	}
	else if (pkt.type == RECHARGE_PACKET)
	{
		uint8_t result;
		int recharge_amount;
		uint32_t recharge_validity, carry_forward_energy_units;
		sscanf(pkt.data, "%d,%lu,%lu,", &recharge_amount, &recharge_validity, &carry_forward_energy_units);
		result = account->recharge(recharge_amount, recharge_validity, carry_forward_energy_units);
		buffer[0] = '\0';
		sprintf(buffer, "%d,%lu,%lu,%hu\0", recharge_amount, recharge_validity, carry_forward_energy_units, result);
		make_packet(&pkt, pkt.source_address, NORMAL_PACKET, buffer);
		account->write_to_rom();
		comm->send(&pkt);
	}
	else if (pkt.type == ACCOUNT_DETAILS_PACKET)
	{
		buffer[0] = '\0';
		char tmp_buffer[15];
		
		sprintf(tmp_buffer, "%d,\0", account->get_balance());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%lu,\0", account->get_validity());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%lu,\0", account->get_carry_forward_units());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%lu,\0", account->get_energy_remaining());
		strcat(buffer, tmp_buffer);
		
		sprintf(tmp_buffer, "%lu,\0", account->get_energy_used());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%lu,\0", account->get_last_recharge_time());
		strcat(buffer, tmp_buffer);
		
		make_packet(&pkt, pkt.source_address, ACCOUNT_DETAILS_PACKET, buffer);
		comm->send(&pkt);
	}
	else if (pkt.type == DEVICE_DETAILS_PACKET)
	{
		buffer[0] = '\0';
		char tmp_buffer[15];
		sprintf(tmp_buffer, "%lu,\0", account->get_ip_address());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%hu,\0", account->get_hw_version());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%hu,\0", account->get_sw_version());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%lu,\0", account->get_installation_date());
		strcat(buffer, tmp_buffer);
		
		sprintf(tmp_buffer, "%lu,\0", account->get_rate());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%u,\0", account->get_power_limit());
		strcat(buffer, tmp_buffer);
		sprintf(tmp_buffer, "%hu,\0", account->get_state());
		strcat(buffer, tmp_buffer);
		
		make_packet(&pkt, pkt.source_address, ACCOUNT_DETAILS_PACKET, buffer);
		comm->send(&pkt);
	}
}


#endif /* INTERFACE_H_ */