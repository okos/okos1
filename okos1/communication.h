/*
 * communication.h
 *
 * Created: 3/25/2014 10:13:35 PM
 *  Author: vicky
 */ 

//Add packet length to packet
#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#define PACKET_START_BYTE 0xaa
#define PACKET_END_BYTE 0xaa
#define PACKET_HEADER_LENGTH  11//One start byte, Four bytes of source address, 4 bytes of destination address, 1 byte to describe packet length, and 1 byte to describe packet type
#define PAYLOAD_LENGTH 38
#define MAX_PACKET_LENGTH 150 //Maximum length of packet including header

#define NORMAL_PACKET 0
#define RECHARGE_PACKET 1
#define DATA_PACKET 2
#define ACCOUNT_DETAILS_PACKET 3
#define DEVICE_DETAILS_PACKET 4
#define CALIBRATION_PACKET 5

class Packet
{
	public:
	uint32_t source_address;
	uint32_t destination_address;
	uint8_t length;
	uint8_t type;
	uint8_t current_byte;
	char data[PAYLOAD_LENGTH];
	uint8_t packet_complete;
	
	Packet();
	void copy(Packet*);
	uint8_t add_byte(uint8_t data_byte);
	uint8_t get_next_byte();
};

Packet::Packet()
{
	source_address = 0;
	destination_address = 0;
	length = 0;
	type = 0;
	current_byte = 0;
	data[0] = '\0';
	packet_complete = 0;
}

void Packet::copy(Packet* pkt)
{
	uint8_t i;
	pkt->length = length;
	pkt->source_address = source_address;
	pkt->destination_address = destination_address;
	pkt->type = type;
	pkt->current_byte = current_byte;
	for (i=0;data[i]!='\0';i++)
		pkt->data[i] = data[i];
	pkt->data[i] = '\0';
	pkt->packet_complete = packet_complete;
}

uint8_t Packet::get_next_byte()
{
	uint8_t data_byte;
	if (current_byte == 0)
	{
		data_byte = PACKET_START_BYTE;
		//send_string("Start");		
	}
	else if(current_byte < 5)
	{
		data_byte = source_address>>((4 - current_byte)*8);
		//send_string("SA");	
	}
	else if (current_byte < 9)
	{
		data_byte = destination_address>>((8 - current_byte)*8);
		//send_string("DA");	
	}
	else if (current_byte < 10)
	{
		data_byte = length;
		//send_string("PL");	
	}
	else if (current_byte < 11)
	{
		data_byte = type;
		//send_string("TP");	
	}
	else if(current_byte < length-1)
	{
		data_byte = int(data[current_byte - 11]);
		//send_string("DT");	
	}
	else
	{
		data_byte = PACKET_END_BYTE;
		//send_string("EB");	
	}
	current_byte += 1;
	return data_byte;
}

uint8_t Packet::add_byte(uint8_t data_byte)
{
	uint8_t error = 0;
	//USART_send_byte(data_byte);
	if (current_byte == 0)
	{
		if (data_byte == PACKET_START_BYTE)
		{
			source_address = 0;
			destination_address = 0;
			length = 0;
			type = 0;
			data[0] = '\0';
			packet_complete = 0;
			//send_string("SB\n");			
		}
		else
			return 2;
	}
	else if(current_byte < 5)
	{
		//send_string("SA\n");
		source_address |= data_byte&0xff;
		if (current_byte < 4)
			source_address = source_address<<8;
	}
	else if (current_byte < 9)
	{
		//send_string("DA\n");	
		destination_address |= data_byte&0xff;
		if (current_byte < 8)
			destination_address = destination_address<<8;
	}
	else if (current_byte < 10)
	{
		//send_string("PL\n");
		length = data_byte;
	}
	else if (current_byte < 11)
	{
		//send_string("TP\n");
		type = data_byte;
	}
	else if (current_byte < length-1)
	{
		//send_string("DT\n");
		data[current_byte - 11] = (char)data_byte;
		data[current_byte - 10] = '\0';
	}
	else if (data_byte == PACKET_END_BYTE)
	{
		//send_string("EB\n");
		//length = current_byte;
		packet_complete = 1;
	}
	else
		return 3;
	current_byte += 1;
	return 1;
}



class Communication
{
	uint8_t is_sending;
	uint8_t sent_bytes;
	//uint8_t is_receiving;
	public:
	Packet sending_pkt;
	Packet receiving_pkt;
	
	
	Communication();
	uint8_t send_packet();
	uint8_t receive_packet();
	uint8_t send(Packet *pkt);
	uint8_t receive(Packet *pkt);
	inline void update();
};

Communication::Communication()
{
	//is_receiving = 0;
	is_sending = 0;
	sent_bytes = MAX_PACKET_LENGTH+1;
	sending_pkt = Packet();
	sending_pkt.current_byte = 0;
	sending_pkt.length = 0;
	receiving_pkt = Packet();
}

void Communication::update()
{
	send_packet();
	receive_packet();
}

uint8_t Communication::send(Packet *pkt)
{
	if ( pkt == NULL || is_sending )
	{
		//send_string("SE\n");
		return 4;
	}
	pkt->copy(&sending_pkt);
	sending_pkt.current_byte = 0;
	sent_bytes = 0;
	is_sending = 1;
	//send_string("SS\n");
	return 1;
}

uint8_t Communication::receive(Packet *pkt)
{
	if (receiving_pkt.packet_complete == 1)
	{
		receiving_pkt.copy(pkt);
		receiving_pkt.packet_complete = 0;
		receiving_pkt.current_byte = 0;
		return 1;
	}
	else
		return 0;
}

uint8_t Communication::send_packet()
{
	if (sent_bytes < sending_pkt.length)
	{
		is_sending = 1;
		USART_send_byte(sending_pkt.get_next_byte());
		sent_bytes += 1;
	}
	else
	{
		is_sending = 0;
		sent_bytes = MAX_PACKET_LENGTH+1;
		sending_pkt.length = 0;
		//send_string("SC\n");	
	}
	return 1;
}

uint8_t Communication::receive_packet()
{
	uint8_t data_byte;
	if (!receiving_pkt.packet_complete)
	{
		if (USART_any_byte(&data_byte))
			receiving_pkt.add_byte(data_byte);
	}
}

#endif /* COMMUNICATION_H_ */