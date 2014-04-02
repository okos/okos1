#!/usr/lib/python
#Library to make packet and check packet to communicate with smart meter
#written on date: 28-03-2014
#Author: Vikas Lamba

import serial;
import sys;
import time;

START_BYTE = 0xaa;
STOP_BYTE = 0xaa;
#Packet types
NORMAL = 0;
RECHARGE = 1;
DATA = 2;
ACCOUNT = 3;
DEVICE = 4;
METER = 6;
CALIBRATION =7;
PACKET_LENGTH_WITHOUT_PAYLOAD = 12; #1 start byte, 4 source address, 4 destination address, 1 packet length, 1 packet type, and 1 stop byte
MAX_PACKET_LENGTH = 150

class Packet:
    # Class to represent packet of communication.

    def __init__(self):
        self.start_byte = START_BYTE;
        self.stop_byte = STOP_BYTE;
        self.source_address_serializd = "0.0.0.1";
	self.destination_address_serialized = "0.0.0.0";
	self.packet_type = 'Normal';
        self.data = '';
        self.packet_length = PACKET_LENGTH_WITHOUT_PAYLOAD;


    #def __init__(self, source_address, destination_address, packet_type, data):
    #    self.make_packet(source_address, destination_address, packet_type, data);
    #def __init__(self, packet_data):
    #    self.deserlize_packet(packet_data);

    def check_integrity(self):
	errors = '';
        if self.start_byte != START_BYTE:
            errors += 'Invalid start byte, ';
        if self.stop_byte != STOP_BYTE:
            errors += 'Invalid stop byte, ';
        if self.packet_length > MAX_PACKET_LENGTH:
            errors += 'PAcket length exceed max allowd length, ';
        if self.packet_type != NORMAL and self.packet_type != RECHARGE and self.packet_type != DATA and self.packet_type != ACCOUNT and self.packet_type != DEVICE:
            errors += 'Invalid packet type, ';
        return errors;

    def make_packet(self, source_address, destination_address, packet_type, data):
        self.start_byte = START_BYTE;
        self.stop_byte = STOP_BYTE;
	self.source_address_serialized = source_address;
        self.source_address_deserialized = self._deserialize_address(source_address);
        self.destination_address_serialized = destination_address;
        self.destination_address_deserialized = self._deserialize_address(destination_address);
        self.packet_type = packet_type;
        self.data = data;
        self.packet_length = PACKET_LENGTH_WITHOUT_PAYLOAD + len(data);


    def _str_to_hex_address(self, address):
        to_return = '';
        tmp_list = address.split('.');
        for i in range(0, 4):
            to_return += chr(int(tmp_list[i], 16));
        return to_return;

    def _hex_address_to_str(self, address):
        to_return = '';
        to_return = str(ord(address[0])) + '.';
        to_return += str(ord(address[1])) + '.';
        to_return += str(ord(address[2])) + '.';
        to_return += str(ord(address[3]));
        return to_return;

    def serialize_packet(self):
        #Converts packet into raw bytes
        packet_data = '';
        packet_data += chr(self.start_byte);
        packet_data += self._str_to_hex_address(self.source_address_serialized);
	packet_data += self._str_to_hex_address(self.destination_address_serialized);
	packet_data += chr(self.packet_length);
	packet_data += chr(self.packet_type);
	packet_data += str(self.data);
	packet_data += chr(self.stop_byte);
        return packet_data;

    def deserialize_packet(self, packet_data):
        #converts raw bytes into packet
        self.start_byte = ord(packet_data[0]);
        self.stop_byte = ord(packet_data[-1]);
        self.source_address_serialized = self._hex_address_to_str(packet_data[1:5]);
        self.source_address_deserialized = self._deserialize_address(self.source_address_serialized);
        self.destination_address_serialized = self._hex_address_to_str(packet_data[5:9]);
        self.destination_address_deserialized = self._deserialize_address(self.destination_address_serialized);
        self.packet_length = ord(packet_data[9]);
        self.packet_type = ord(packet_data[10]);
        self.data = packet_data[11:-1];

    def _serialize_address(self, address):
        a = address//(256*3);
        address = address % (256*3);
        b = address//(256*2);
        address = address % (256*2);
        c = address//256;
        address = address % 256;
        d = address;
        return str(a) + '.' + str(b) + '.' + str(c) + '.' + str(d);

    def _deserialize_address(self, address):
	address_deserialized = 0;
        tmp_list = address.split('.');
	if len(tmp_list) == 4:
	    address_deserialized = (int(tmp_list[0])<<3*8) + (int(tmp_list[1])<<2*8) + (int(tmp_list[2])<<1*8) + (int(tmp_list[3])<<0*8);
            return address_deserialized;
        else:
            return "Invalid address";

#Test for packet library
if __name__ == '__main__':
    p = Packet();
    if len(sys.argv) != 3:
        sys.exit("usage: packet.py destination_address message");
    destination = sys.argv[1];
    message = sys.argv[2];
    if 'recharge' in message:
        tmp_list = message.split(' ');
        recharge_amount = int(tmp_list[1]);
        recharge_validity = int(tmp_list[2]);
        carry_forward_units = int(tmp_list[3]);
        p.make_packet('0.0.0.0', destination, RECHARGE, str(recharge_amount)+','+str(recharge_validity)+','+str(carry_forward_units)+',');
    elif 'data' in message:
        p.make_packet('0.0.0.0', destination, DATA, '');
    elif 'account' in message:
        p.make_packet('0.0.0.0', destination, ACCOUNT, '');
    elif 'device' in message:
        p.make_packet('0.0.0.0', destination, DEVICE, '');
    elif 'meter' in message:
        p.make_packet('0.0.0.0', destination, METER, '');
    elif 'calibration' in message:
        p.make_packet('0.0.0.0', destination, CALIBRATION, '');
    else:
        p.make_packet('0.0.0.0', destination, NORMAL, message);
    data =  p.serialize_packet();
    dev = serial.Serial('/dev/ttyUSB0', timeout = 0.4, baudrate = 9600, bytesize = 8, stopbits = 1, parity='N');
    for i in data:
        dev.write(i);
    #print len(data);
    error = p.check_integrity;
    print error;
    #print "send:"
    #print ":".join("{:02x}".format(ord(c)) for c in data)
    data = '';
    time.sleep(1);
    ch = dev.read();
    while ch != '':
        data += ch;
        ch = dev.read(1);
    #print "receive:";
    #print data;
    #print ":".join("{:02x}".format(ord(c)) for c in data)
    p.deserialize_packet(data);
    print p.check_integrity();
    print p.data;
    dev.close();
