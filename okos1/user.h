/*
 * user.h
 *
 * Created: 5/5/2014 9:43:51 PM
 *  Author: vicky
 */ 


#ifndef USER_H_
#define USER_H_

class User
{
	//Class to represent each user of the meter. 
	Meter meter;
	Account account;
	Relay relay;
	public:
	User()
	{
		
	}
};



#endif /* USER_H_ */